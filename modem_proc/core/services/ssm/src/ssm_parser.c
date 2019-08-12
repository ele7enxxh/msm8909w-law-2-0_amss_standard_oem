/*=============================================================================

             S E C U R E  S E R V I C E S   M O D U L E

GENERAL DESCRIPTION
  Implements SSM framework functionality

EXTERNALIZED FUNCTIONS
 ssm_client_init_func
    Register a client with SSM

  ssm_get_perm
    Gets permission for a particular set of attributes

  INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2011-2014 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_parser.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/20/14   din     Fixed concurreny issue seen due to retry of sfs read operations.
04/03/14   din     Added new apis to get mode and to write to permission file. 
                   Also added diag command to get ssm mode.
04/02/14   psu    Update diag macros using optimized versions.
08/22/13   din     Fix for NULL check for the callback pointer and 
                   fix for incorrect value copied to the new buffer.
04/29/13   din     Changes pertaining to ADSP were done.
04/22/13   din     SSM apis were modified to facilitate whitelisting.
04/02/13   din     Replaced memcpy with memscpy.
01/15/13   din     Possible buffer overflow issue fixed seen due to usage of strcmp.
11/19/12   din     Resolved klocwork errors.
09/13/12   din     New SSM apis to get the size of perm data and copy perm data.
08/24/12   din     Reinserted EFS based perm file fucntionality within a Feature.
08/24/12   din     Removed warnings pertaining to insertion of task.h
07/25/12   din     Resolved concurrency issue which leads to return Incorrect err code 
                   during SSM client initialization.
07/19/12   din     Changes done to switch from efs to sfs based permission file.
02/14/12   din     Corrected Minor bug in condition check relating to failure.
                   of returning correct permission in certain case.
09/01/12   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "ssm_task_v.h"
#include "ssm_v.h"
#include "ssm_ipc_v.h"
#include <stringl/stringl.h>
#include "qurt.h"
#ifdef FEATURE_SSM_REX_API_PRESENT
#include "rex.h"
//#include "blast.h"
#else /*  FEATURE_SSM_REX_API_PRESENT */
#include "qmutex.h"
#include "qurt.h"
#endif /* FEATURE_SSM_REX_API_PRESENT */
#include "queue.h"
#include "timetick.h"
#include "timer.h"
#include "ssm_smdl_v.h"

#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
#include <fs_public.h>
#include "sfs_api.h"
#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

/* Timer expiry period in secs : 600 secs */
#define SSM_TIMER_EXPIRY_PERIOD_SEC    600

#define SIZEOFATTR(id)             (sizeof(ssm_attr_data_t)* 2 * \
	                           ssm_parser_clients[id].num_of_attr)  
#define SIZEOFATTR_PERM(id)        (SIZEOFATTR(id) + sizeof( ssm_perm_data_t))
#define MAX_FILE_NAME_SIZE              20
#define SSM_MAX_CLIENTS                 10
#define SSM_TEMP_CHAR_ARR_SIZE          100
#define SSM_DATA_SEPARATOR              '|'
#define SSM_MAX_ENTRIES_EXPECTED        5
#define SSM_MAX_ENTRIES_EXPECTED_INC    20
#define SSM_LINE_FEED_CHARACTER         10
#define SSM_GET_PERM_MAX                (0x0FFFFFFF)

#ifndef UINT32_MAX
#define UINT32_MAX                      (0xFFFFFFFF)
#endif /* UINT32_MAX */

#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC

/* Pointer to the mutex object for protecting critical-section of ssm code*/
static qurt_mutex_t ssm_parser_mutex;
static qurt_mutex_t *pssm_parser_mutex = NULL;

/* MACRO for locking/unlocking the qmutex*/
#define SSM_INTLOCK()          \
  if ( NULL == pssm_parser_mutex )              \
  {                                      \
    qurt_pimutex_init(&ssm_parser_mutex);       \
    pssm_parser_mutex = &ssm_parser_mutex;             \
  }                                      \
  qurt_pimutex_lock(&ssm_parser_mutex); 
  
#define SSM_INTFREE()          \
  qurt_pimutex_unlock(&ssm_parser_mutex); 

#else
/* Pointer to the mutex object for protecting critical-section of ssm code*/
static qmutex_t ssm_parser_mutex = 0;

/* Function protect the critical section of ssm code */
static void ssm_parser_qmutex_init(void);

/* MACRO for locking/unlocking the qmutex*/
#define SSM_INTLOCK()          \
  if ( 0 == ssm_parser_mutex )        \
    ssm_parser_qmutex_init();         \
  qmutex_lock(ssm_parser_mutex); 
  
#define SSM_INTFREE()          \
  qmutex_unlock(ssm_parser_mutex); 

#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

/* SSM mode */
static uint32 ssm_parser_mode = SSM_DEFAULT_MODE;

/* Last Timetick when mode is modified */
static timetick_type ssm_parser_last_tick;

/* Counter for ssm_get_perm */
static uint32 ssm_parser_get_perm_counter;

/* Depicts status of the SSM CLIENT  */
typedef enum ssm_client_status
{
  /* Indicates that client is yet not initialized  */
  SSM_CLIENT_STATUS_NOT_INITIALIZED = 0,
  /** Indicates that client is in read file queue */
  SSM_CLIENT_STATUS_READ_FILE_IN_QUEUE,
  /** Indicates that read file in progress */
  SSM_CLIENT_STATUS_READ_FILE_IN_PROGESS,
  /** Indicates that client is initialized */
  SSM_CLIENT_STATUS_INITIALIZED,
  /** Indicates that client initialization failed */
  SSM_CLIENT_STATUS_INITIALIZATION_FAILED

} ssm_client_status_t ;

/* Queue to store client id to be processed for reading perm files */
static q_type                            ssm_perm_file_q;

typedef struct ssm_client_queue_node_struct_type
{
  /* Link for the next queue */
  q_link_type                            link;

  /* Id of the client to be processed */
  uint32                                 client_id;

  /* Flag to indicate if the not is in queue */
  boolean                                in_queue;

} ssm_client_queue_node_struct;

/* Info of clients registering with SSM module */
struct ssm_parser_clients_struct
{
  char * ssm_data_ptr;
  char * ssm_data_ptr_fb;
  char filename[MAX_FILE_NAME_SIZE];
  uint32 num_of_attr;
  uint32 max_num_of_entries;
  uint32 max_num_of_entries_fb;
  uint32 num_of_entries;
  uint32 num_of_entries_fb;
  uint32 default_perm;
  ssm_attr_t attr_t;
  ssm_client_status_t status;
  ssm_client_status_t status_fb;
  ssm_cb_type ssm_cb;
  uint16 current_entry_loc;
  uint16 current_entry_loc_fb;
  char * fallback_buffer;
  uint16 len_fallback_buffer;
  char * residue_buffer;
  uint16 len_residue_buffer;
  ssm_client_queue_node_struct q_node;
  qurt_mutex_t mutex;
  /* Used to store information about unprocessed remote file reads */
  #ifndef FEATURE_SSM_PERIPHERAL_SERVER_PROC
  uint32 last_pkt_num;
  #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
};

/* List of clients registered to SSM module */
static struct ssm_parser_clients_struct * ssm_parser_clients;

/* Maintains number of clients regsitered with SSM module */
static uint32 num_clients_registered=0;

/* Variable indicating if queue is initialized */
static boolean ssm_client_queue_init = FALSE;

#ifdef FEATURE_SSM_REX_API_PRESENT
/* Look up pointer for ssm tcb */
static rex_tcb_type* ssm_tcb_ptr=NULL;
#else /* FEATURE_SSM_REX_API_PRESENT */
extern qurt_anysignal_t  ssm_sig_qurt;
#endif /* FEATURE_SSM_REX_API_PRESENT */

#ifndef FEATURE_SSM_PERIPHERAL_SERVER_PROC
static uint32 ssm_parser_clnt_process = SSM_MAX_CLIENTS;
#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */


#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC

/* Structure to store information about remote permission file 
 * that is being read on modem processor */

typedef struct
{
  ssm_proc_enum_t     proc;
  char *              filename;
  uint16              len_file_read;
  uint16              bytes_read_last;
  uint16              num_pkts_sent;
  uint32              mode_used;
  boolean             initialized;
  boolean             data_complete;
} ssm_parser_remote_perm_file_reads_struct_t;

static ssm_parser_remote_perm_file_reads_struct_t   ssm_parser_remote_perm_file_read;

/* Queue to store filename and proc for read remote processor read request */
/* This is used when more than one remote read file requests are seen */
static q_type          ssm_parser_remote_read_perm_file_q;

typedef struct 
{
  /* Link for the next queue */
  q_link_type                            link;

  /* Proc from which request came in */
  ssm_proc_enum_t                        proc;

  /* IPC that made the request */
  ssm_ipc_req_t                          ipc;

  /* Filename */
  char *                                 filename;

  /* Length of file name */
  uint32                                 len_filename;

} ssm_remote_read_perm_file_node_struct;

#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

static timer_ptr_type ssm_parser_mode_timer;

#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC

static boolean ssm_parser_sfs_ready_try;
static boolean ssm_parser_sfs_ready_tried;
static uint32  ssm_parser_sfs_ready_try_id = 0xFFFFFFFF;

#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */


/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

#ifndef FEATURE_SSM_PERIPHERAL_SERVER_PROC
/**
This function initializes the qmutex that will be used for protecting
the crit-section portion of SSM code.

@param[in]  
None

@return 
None 

@dependencies
None

*/

static void ssm_parser_qmutex_init(void)
{
  qmutex_attr_t mutex_attr;
  int intErr=0;

  mutex_attr.type = QMUTEX_PRIORITY;
  if(0 == ssm_parser_mutex) 
  {
    intErr = qmutex_create ( &ssm_parser_mutex, &mutex_attr );
  }
  if(0 != intErr)
  {
    ERR_FATAL("Failed to create ssm_parser_mutex",0,0,0);
  }
} /* ssm_parser_qmutex_init */

#endif /* !FEATURE_SSM_PERIPHERAL_SERVER_PROC */

#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC

/**
Writes the specified string into permission file for a given mode.
The function overwrites the file specified if the file is already present.

@param[in] filename                 File to which the contents needs to be written.
@param[in] mode                     mode for which the specified file needs 
                                    to be written.
@param[in] write_buffer             contents to be written to the file.

@return
boolean                             TRUE if file is written, FALSE otherwise. 

@dependencies
None.

*/


boolean ssm_parser_write_input_file
( 
  char * filename,
  uint32 mode,
  char * write_buffer
)
{
  /* Sfs file handle */
  int fp;
  
  /* File name with location to be read */
  char file[120];

  /* Bytes to be written */
  uint32 nbyte;

  /* Bytes written */
  uint32 bytes_written;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( write_buffer == NULL )
  {
    return FALSE;
  }

  int result=-1;
		
  result = efs_mkdir ("/ssm",0755);
  SSM_MSG_MED_1 ( " ssm_parser_write_input_file result = %d", result);
		
  #ifndef FEATURE_SSM_USE_EFS
  result = sfs_mkdir ("/ssm/data_file" );
  #else
  result = efs_mkdir( "/ssm/data_file", 0755);
  #endif
  SSM_MSG_MED_1 ( " ssm_parser_write_input_file result = %d", result);
		
  snprintf( file, sizeof(file), "/ssm/data_file/%s_%u.ssm", 
                           filename, (unsigned int)mode);

  /* Get file handle for data file and check if it is valid */
  #ifndef FEATURE_SSM_USE_EFS
  fp = sfs_open( file, O_CREAT|O_TRUNC|O_WRONLY );
  if ( fp == 0 )
  #else
  fp = efs_open( file, O_CREAT|O_TRUNC|O_WRONLY, 0644 );
  if ( fp < 0 )
  #endif
  {
    SSM_MSG_MED ( " ssm_parser_write_input_file : file open failed ");
    return FALSE ;
  }

  nbyte = strlen(write_buffer);

  while (nbyte > 0) 
  {
    #ifndef FEATURE_SSM_USE_EFS
    bytes_written = sfs_write (fp, (void *) write_buffer, nbyte);
    #else
    bytes_written = efs_write (fp, (void *) write_buffer, nbyte);
    #endif
    
    if (bytes_written <= 0) 
    {
      SSM_MSG_MED ( " ssm_parser_write_input_file : write to file failed ");
      
      /* Close the file handle */
      #ifndef FEATURE_SSM_USE_EFS
      /* Close sfs file */
      sfs_close(fp);
      #else
      efs_close(fp);
      #endif

      return FALSE;
    }

    nbyte -= bytes_written;
    
  }


  /* Close the file handle */
  #ifndef FEATURE_SSM_USE_EFS
  /* Close sfs file */
  sfs_close(fp);
  #else
  efs_close(fp);
  #endif

  return TRUE;

} /* ssm_parser_write_input_file */


/**
Parses the input file and creates a 
structure that can be read from the get function

@param[in] filename                 File that needs to be read
@param[in] pos                      Position in the file to read from
@param[out] size_read               size read
@param[out] read_buffer             Buffer to which data is read into

@return
ssm_err_t                                 err 

@dependencies
None.

*/


static ssm_err_t ssm_parser_read_input_file
( 
  char * filename,
  uint32 pos,
  uint32 * size_read,
  char ** read_buffer
)
{
  /* Sfs file handle */
  int fp;
  
  /* File name with location to be read */
  char file[120];

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_MSG_MED_1 ( " Function : ssm_parser_read_input_file pos =%d", 
		  pos);

  snprintf( file, sizeof(file), "/ssm/data_file/%s_%u.ssm", 
                           filename, (unsigned int)ssm_parser_mode);

  *read_buffer = malloc ( SSM_TEMP_CHAR_ARR_SIZE );

  if ( *read_buffer == NULL )
  {
    return E_SSM_DATA_MALLOC_FAILED;
  }

  /* Get file handle for data file and check if it is valid */
  #ifndef FEATURE_SSM_USE_EFS
  fp = sfs_open( file, O_RDONLY );
  if ( fp == 0 )
  #else
  fp = efs_open( file, O_RDONLY );
  if ( fp < 0 )
  #endif
  {
    return E_SSM_FILE_OPR_FAILED ;
  }

  #ifndef FEATURE_SSM_USE_EFS
  if ( pos != sfs_seek (fp, pos, SEEK_SET) ) 
  #else
  if ( pos != efs_lseek (fp, pos, SEEK_SET) ) 
  #endif
  {
    /* Close the file handle */
    #ifndef FEATURE_SSM_USE_EFS
    /* Close sfs file */
    sfs_close(fp);
    #else
    efs_close(fp);
    #endif

    return E_SSM_FILE_OPR_FAILED;
  }

  #ifndef FEATURE_SSM_USE_EFS
  *size_read = sfs_read( fp, *read_buffer , SSM_TEMP_CHAR_ARR_SIZE );  
  #else
  *size_read = efs_read( fp, *read_buffer, SSM_TEMP_CHAR_ARR_SIZE ); 
  #endif

  if ( size_read < 0 )
  {
    /* Close the file handle */
    #ifndef FEATURE_SSM_USE_EFS
    /* Close sfs file */
    sfs_close(fp);
    #else
    efs_close(fp);
    #endif

    return E_SSM_FILE_OPR_FAILED;
  }
   
  /* Close the file handle */
  #ifndef FEATURE_SSM_USE_EFS
  /* Close sfs file */
  sfs_close(fp);
  #else
  efs_close(fp);
  #endif

  return E_SSM_SUCCESS;
  
} /* ssm_parser_read_input_file */

#endif /* !FEATURE_SSM_PERIPHERAL_SERVER_PROC */



/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/

/** 
  
The function is invoked to update perm file from the buffer passed in argument

@param[in] id                      Id of the client for which buffer is updated
@param[in] len                     Length of the buffer perm_data
@param[in] perm_data               Buffer from which the perm information is read

@return
ssm_err_t                          Returns E_SSM_SUCCESS is succesful

@dependencies
None.
*/


static ssm_err_t ssm_parser_process_input_buffer_append 
(
  uint32 id,
  uint16 len,
  char * perm_data
)
{

  /* Temporary Iterator variables */
  uint32 i =0, j=0, k =0, k_residue =0, k_temp =0;

  /* Temoporary Pointer to iterate through SSM_table */
  ssm_attr_data_t * temp, *temp1;

  /* Temp unsigned integer */
  uint32 overflow_t, old_malloc_buffer_len;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_MSG_MED_2(" ssm_parser_process_input_buffer_append id =%d Buffer_len =%d "
		  ,id, len);
 
  /* Check if id is in the range */
  if ( id > num_clients_registered   )
  {
    return E_SSM_INVALID_DATA;
  }

  /* Check if perm_data id valid */
  if ( perm_data == NULL )
  {
    return E_SSM_INVALID_DATA;
  }

  /* Check for overflow of data */
  if( ssm_parser_clients[id].max_num_of_entries > 
              UINT32_MAX/ SIZEOFATTR_PERM(id) )
  {
    return E_SSM_DATA_OVERFLOW_ERROR ;
  }

  if ( perm_data[ len -1 ] == SSM_LINE_FEED_CHARACTER )
  {
    len-- ;
  }

  /* If ssm_data_ptr is NULL, overflow_t is the size to be allocated 
   * else it is already allocated */
  overflow_t = ssm_parser_clients[id].max_num_of_entries * SIZEOFATTR_PERM(id);

  if ( ssm_parser_clients[id].ssm_data_ptr == NULL )
  {
    /* Allocate a structure to store data from data file */
    ssm_parser_clients[id].ssm_data_ptr = (void *)malloc( overflow_t ) ;

    if ( ssm_parser_clients[id].ssm_data_ptr == NULL )
    {
      return E_SSM_DATA_MALLOC_FAILED ;
    }

    ssm_parser_clients[id].num_of_entries = 0 ;
    ssm_parser_clients[id].current_entry_loc = 0 ;
  
  }

  i = ssm_parser_clients[id].num_of_entries;
  
  /* if max_number of entries have been iterated */
  if( i == ssm_parser_clients[id].max_num_of_entries )
  {
    old_malloc_buffer_len = ssm_parser_clients[id].max_num_of_entries * SIZEOFATTR_PERM(id);

    if ( SSM_MAX_ENTRIES_EXPECTED_INC > UINT32_MAX - ssm_parser_clients[id].max_num_of_entries )
    {
      return E_SSM_DATA_OVERFLOW_ERROR ;
    }
   
    ssm_parser_clients[id].max_num_of_entries += SSM_MAX_ENTRIES_EXPECTED_INC;

    /* Copy pointer to previous buffer */
    temp1 = (ssm_attr_data_t *) ssm_parser_clients[id].ssm_data_ptr;

    /* Check for overflow of data */
    if( ssm_parser_clients[id].max_num_of_entries > 
        UINT32_MAX/ SIZEOFATTR_PERM(id) )
    {
      return E_SSM_DATA_OVERFLOW_ERROR;
    }

    /* Size of data to be allocated */
    overflow_t = ssm_parser_clients[id].max_num_of_entries * SIZEOFATTR_PERM(id);
	
    /* Allocate a bigger strcuture to store data from data file */
    ssm_parser_clients[id].ssm_data_ptr = (void *)malloc( overflow_t ) ;

    if ( ssm_parser_clients[id].ssm_data_ptr == NULL )
    {
      return E_SSM_DATA_MALLOC_FAILED;
    }

    /* copy contents from prev buffer to new buffer */
    if ( old_malloc_buffer_len != memscpy(ssm_parser_clients[id].ssm_data_ptr, 
		  overflow_t, temp1, old_malloc_buffer_len ) )
    {
      return E_SSM_DATA_OVERFLOW_ERROR;

    }
     
    /* free up prev buffer */
    free(temp1);

  }

  
  k_residue=0, k=0;

  /* temp is incremented by ssm_parser_clients[id].current_entry_loc * sizeof ( ssm_data_ptr ) */
  temp = (ssm_attr_data_t *) ssm_parser_clients[id].ssm_data_ptr + ssm_parser_clients[id].current_entry_loc;

  /* Iterate through maximum number of entries allowed */
  while ( i < ( ssm_parser_clients[id].max_num_of_entries ) ) 
  {
    j = ssm_parser_clients[id].current_entry_loc % ( ssm_parser_clients[id].num_of_attr * 2 +1 );

    /* Read Attributes from file for a row */
    for ( ; j < (ssm_parser_clients[id].num_of_attr * 2 + 1 ); j++)
    {
      /* Initialize value pointed by temp as zero */
      *temp = 0;

      if ( ssm_parser_clients[id].residue_buffer != NULL )
      {
        while( k_residue < ssm_parser_clients[id].len_residue_buffer )
	{
	  /* Convert character to integer and update it to temp */
	  *temp = *temp * 10 + 
		( ssm_parser_clients[id].residue_buffer[k_residue] - '0' );

	  /* Increment Iterator */
          k_residue++;
	}

	free ( ssm_parser_clients[id].residue_buffer );
	ssm_parser_clients[id].residue_buffer = NULL;
        ssm_parser_clients[id].len_residue_buffer = 0;
      }
      
      /* Iterate through buffer read from buffer */
      while ( k < len )
      {
	if ( perm_data[k] == SSM_DATA_SEPARATOR )
	{
	  /* Increment Iterator */
	  k++;

	  ssm_parser_clients[id].current_entry_loc++;

	  /* Increment number of entries in the table */
          if ( 0 == ssm_parser_clients[id].current_entry_loc % 
		       (ssm_parser_clients[id].num_of_attr * 2 + 1) ) 
          {
            ssm_parser_clients[id].num_of_entries++ ;
            ++i;

	    /* if max_number of entries have been iterated */
            if( i == ssm_parser_clients[id].max_num_of_entries && k < len )
            {

              
	      old_malloc_buffer_len = ssm_parser_clients[id].max_num_of_entries * SIZEOFATTR_PERM(id);

              if ( SSM_MAX_ENTRIES_EXPECTED_INC > UINT32_MAX - ssm_parser_clients[id].max_num_of_entries )
              {
                return E_SSM_DATA_OVERFLOW_ERROR ;
              }

              ssm_parser_clients[id].max_num_of_entries += SSM_MAX_ENTRIES_EXPECTED_INC;

	      /* Copy pointer to previous buffer */
	      temp1 = (ssm_attr_data_t *) ssm_parser_clients[id].ssm_data_ptr;

              /* Check for overflow of data */
              if( ssm_parser_clients[id].max_num_of_entries > 
                  UINT32_MAX/ SIZEOFATTR_PERM(id) )
              {
                return E_SSM_DATA_OVERFLOW_ERROR;
              }

              /* Size of data to be allocated */
              overflow_t = ssm_parser_clients[id].max_num_of_entries * SIZEOFATTR_PERM(id);

	      /* Allocate a bigger strcuture to store data from data file */
              ssm_parser_clients[id].ssm_data_ptr = (void *)malloc( overflow_t ) ;

	      if ( ssm_parser_clients[id].ssm_data_ptr == NULL )
              {
                return E_SSM_DATA_MALLOC_FAILED;
              }

	      /* copy contents from prev buffer to new buffer */
	      if ( old_malloc_buffer_len != memscpy(ssm_parser_clients[id].ssm_data_ptr, 
			  overflow_t, temp1, old_malloc_buffer_len ) )
	      {
                return E_SSM_DATA_OVERFLOW_ERROR;
              }

	      /* free up prev buffer */
       	      free(temp1);

	      /* Set iterator to the end of the data set in the new buffer */
	      temp = (ssm_attr_data_t *) ssm_parser_clients[id].ssm_data_ptr + 
		                       ssm_parser_clients[id].current_entry_loc;

	      /* Update location pointed by temp to 0 */
	      *temp = 0;

            }
	    else /* if( i == ssm_parser_clients[id].max_num_of_entries && k < len ) */
	    {
              /* Increment temp to next location */
              temp++;
	  
	      /* Update location pointed by temp to 0 */
	      *temp = 0;

	    } /* if( i == ssm_parser_clients[id].max_num_of_entries && k < len ) */
          }
	  else /* if ( 0 == ssm_parser_clients[id].current_entry_loc % (ssm_parser_clients[id].num_of_attr * 2 + 1) ) */
	  {
	    /* Increment temp to next location */
            temp++;
	  
	    /* Update location pointed by temp to 0 */
	    *temp = 0;
	  } /* if ( 0 == ssm_parser_clients[id].current_entry_loc % (ssm_parser_clients[id].num_of_attr * 2 + 1) ) */
		  

	  break;
	}
        else
        {
          /* Convert character to integer and update it to temp */
	  *temp = *temp * 10 + ( perm_data[k] - '0' );

	  /* Increment Iterator */
          k++;

        }
	
      }
      
      k_temp = k - 1;

      if ( k_temp + 1 == len )
      {
        if ( perm_data[ k_temp ] != SSM_DATA_SEPARATOR )
        {
           /* Find last instance of SSM DATA SEPARATOR */
	   while( perm_data[ k_temp ] != SSM_DATA_SEPARATOR )
	   {
             k_temp--;
	   }

	   /* Allocate and Populate residue buffer */
	   ssm_parser_clients[id].len_residue_buffer = k - k_temp - 1 ;
           ssm_parser_clients[id].residue_buffer = malloc(k - k_temp - 1); 

	   if ( ssm_parser_clients[id].residue_buffer == NULL )
	   {
	     return E_SSM_DATA_MALLOC_FAILED;
	   }

	   if ( ssm_parser_clients[id].len_residue_buffer != 
	            memscpy( ssm_parser_clients[id].residue_buffer,
                    ssm_parser_clients[id].len_residue_buffer,
		    perm_data + k_temp + 1 ,
		    ssm_parser_clients[id].len_residue_buffer ) )
	   {
	     return E_SSM_DATA_OVERFLOW_ERROR;
	   }
	}

        break;
      }
    
    } /* for ( j = 0; j < (ssm_parser_clients[id].num_of_attr *2 + 1 ); j++) */

    if ( k == len )
      return E_SSM_SUCCESS ;   
    

  } /* while ( i < ( ssm_parser_clients[id].max_num_of_entries ) ) */
  
  SSM_MSG_MED (" ssm_update_perm_info_from_buffer client_initialized ");

  return E_SSM_SUCCESS;

} /* ssm_parser_process_input_buffer_append */


#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC

extern void ssm_parser_process_remote_read_request(void);

/**
 
There is a chance of failure for initial file reads from RCINIT group 0.
This functions re queues the sfs reads for clients which failed SFS reads 
after first succesful read. 

@param[in]
None

@return
uint32         ---           SSM mode

#dependencies
None.

*/

static void ssm_parser_sfs_ready_retry
(
  uint32 id
)
{
  /* Temp pointer */
  ssm_client_queue_node_struct * temp_node_ptr;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( ssm_parser_sfs_ready_try == FALSE )
    return;

  if ( ssm_parser_sfs_ready_tried == TRUE )
    return;

  ssm_parser_sfs_ready_tried = TRUE;

  id--;

  while ( id != -1 )
  {
    if ( ssm_parser_clients[id].status != SSM_CLIENT_STATUS_INITIALIZED )
    {
      temp_node_ptr = &(ssm_parser_clients[id].q_node);
      
      qurt_pimutex_lock(&ssm_parser_clients[id].mutex);
      if ( temp_node_ptr->in_queue == TRUE  )
      {
        qurt_pimutex_unlock(&ssm_parser_clients[id].mutex);
        continue;
      }
      
      temp_node_ptr->in_queue = TRUE;

      qurt_pimutex_unlock(&ssm_parser_clients[id].mutex);

      if (  ssm_parser_clients[id].ssm_data_ptr )
       free ( ssm_parser_clients[id].ssm_data_ptr );

      ssm_parser_clients[id].max_num_of_entries = SSM_MAX_ENTRIES_EXPECTED;
      ssm_parser_clients[id].num_of_entries = 0;
      ssm_parser_clients[id].current_entry_loc =0;
       
      /* Update ssm_client status */
      ssm_parser_clients[id].status = SSM_CLIENT_STATUS_READ_FILE_IN_QUEUE;
    
      /* Link ssm_client_queue_arr[id] to ssm_perm_file_q */
      (void) q_link( temp_node_ptr, &temp_node_ptr->link );
    
      /* Enqueue node in ssm_perm_file_q */
      q_put( &ssm_perm_file_q, &temp_node_ptr->link); 
    }

    id--;
  }

  ssm_parser_process_remote_read_request();

  /* Signal SSM task to start parsing perm file */
  #ifdef FEATURE_SSM_REX_API_PRESENT
    rex_set_sigs(ssm_tcb_ptr, SSM_PARSE_PERM_FILE_SIG );
  #else /* FEATURE_SSM_REX_API_PRESENT */
    qurt_anysignal_set( &ssm_sig_qurt, SSM_PARSE_PERM_FILE_SIG );
  #endif /* FEATURE_SSM_REX_API_PRESENT */

} /* ssm_parser_sfs_ready_retry */

/**
 
Reads permission file

@param[in]
None

@return
uint32         ---           SSM mode

#dependencies
None.

*/

static ssm_err_t ssm_parser_read_permission_file
(
  uint32 id
)
{
  /* Position from which the file needs to br read */
  uint32 pos = 0;

  /* Length of data read from file */
  uint32 len_read=0 ;

  /* Temp variable to store ssm error */
  ssm_err_t    err;

  /* Read buffer */
  char * read_buffer;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ssm_parser_clients[id].status= SSM_CLIENT_STATUS_READ_FILE_IN_PROGESS;
  
  while (1)
  {
    err = ssm_parser_read_input_file( ssm_parser_clients[id].filename, pos,
		                      &len_read, &read_buffer );

    SSM_MSG_HIGH_1( " SSM read file error code : %d " , err );
    
    /* ssm_parser_read_input_file() returns E_SSM_SUCCESS if the file is present
     * and reads are successful, even if the len_read is 0 */

    if ( E_SSM_SUCCESS == err )
    {
      ssm_parser_sfs_ready_try = TRUE;
      if ( ssm_parser_sfs_ready_try_id == 0xFFFFFFFF )
        ssm_parser_sfs_ready_try_id = id;

      if ( len_read > 0 )
      {
	if ( E_SSM_SUCCESS != 
             ssm_parser_process_input_buffer_append( id, len_read, read_buffer ))
	{
	  /* Intialize SSM client */
	  if ( ssm_parser_clients[id].ssm_data_ptr )
	    free( ssm_parser_clients[id].ssm_data_ptr );

          ssm_parser_clients[id].ssm_data_ptr = NULL;
          ssm_parser_clients[id].status = SSM_CLIENT_STATUS_INITIALIZATION_FAILED; 
        
          /* Callback updates status of the initialization to the client */
          if ( ssm_parser_clients[id].ssm_cb && 
	       ssm_parser_clients[id].status_fb == SSM_CLIENT_STATUS_INITIALIZED )
            ssm_parser_clients[id].ssm_cb ( E_SSM_SUCCESS );
          else if ( ssm_parser_clients[id].ssm_cb )
            ssm_parser_clients[id].ssm_cb ( err );
          
          free( read_buffer );
          break;
	}
	
	if ( pos > UINT32_MAX - len_read )
	{
	  free( read_buffer );
	  return E_SSM_FAIL;
	}
        
        pos += len_read;
      }	 

      if ( len_read < SSM_TEMP_CHAR_ARR_SIZE )
      {
        /* Intialize SSM client */
        ssm_parser_clients[id].status = SSM_CLIENT_STATUS_INITIALIZED; 
        
        /* Callback updates status of the initialization to the client */
        if ( ssm_parser_clients[id].ssm_cb )
          ssm_parser_clients[id].ssm_cb ( E_SSM_SUCCESS );

	free( read_buffer );
	break;
      }

      free( read_buffer );

    } /* if ( len_read > 0 ) */
    else 
    {
      if ( err != E_SSM_DATA_MALLOC_FAILED )
       free ( read_buffer );
      
      if ( ssm_parser_clients[id].ssm_data_ptr )
	    free( ssm_parser_clients[id].ssm_data_ptr );

      ssm_parser_clients[id].ssm_data_ptr = NULL;
      ssm_parser_clients[id].status = SSM_CLIENT_STATUS_INITIALIZATION_FAILED; 
        
      /* Callback updates status of the initialization to the client */
      if ( ssm_parser_clients[id].ssm_cb && 
	 ssm_parser_clients[id].status_fb == SSM_CLIENT_STATUS_INITIALIZED )
        ssm_parser_clients[id].ssm_cb ( E_SSM_SUCCESS );
      else if ( ssm_parser_clients[id].ssm_cb )
        ssm_parser_clients[id].ssm_cb ( err );
      break;
    }
  }

  return err;

} /* ssm_parser_read_permission_file */

#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

/** 
Queues up the clients for parsing perm file in SSM_TASK

@param[in] 
id                               client id to be processed

@return
None

@dependencies
None.
*/


static ssm_err_t ssm_client_queue_read_file
(
  uint32 id
)
{
 
  /* Temp pointer */
  ssm_client_queue_node_struct * temp_node_ptr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_MSG_MED_1 (" ssm_client_queue_read_file for id=%d ", id);

  temp_node_ptr = &(ssm_parser_clients[id].q_node);

  qurt_pimutex_lock(&ssm_parser_clients[id].mutex);

  if ( temp_node_ptr->in_queue == TRUE )
  {
    SSM_MSG_MED_1 (" ssm client requeue not allowed id=%d ", id);
    qurt_pimutex_unlock(&ssm_parser_clients[id].mutex);
    return E_SSM_SUCCESS ;
  }

  temp_node_ptr->in_queue = TRUE;
  qurt_pimutex_unlock(&ssm_parser_clients[id].mutex);
       
  /* Update ssm_client status */
  ssm_parser_clients[id].status = SSM_CLIENT_STATUS_READ_FILE_IN_QUEUE;

  if( ssm_client_queue_init == FALSE )
  {
    q_init(&ssm_perm_file_q);
    ssm_client_queue_init = TRUE;
  }      
    
  /* Link ssm_client_queue_arr[id] to ssm_perm_file_q */
  (void) q_link( temp_node_ptr, &temp_node_ptr->link );
    
  /* Enqueue node in ssm_perm_file_q */
  q_put( &ssm_perm_file_q, &temp_node_ptr->link);

  return E_SSM_SUCCESS;

} /* ssm_client_queue_read_file */


#ifndef FEATURE_SSM_PERIPHERAL_SERVER_PROC

/**
  
The function is invoked by SSM task to request read perm file .

@param[in] 
None

@return
None

@dependencies
None.
*/


static void ssm_parser_send_read_request 
(
  uint32 id
)
{
  
  ssm_err_t err;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_INTLOCK()
  /* Update status of the client */
  ssm_parser_clients[id].status = SSM_CLIENT_STATUS_READ_FILE_IN_PROGESS;
  SSM_INTFREE()
	
  /* Construct message and send it to modem processor */
  err = ssm_ipc_encode_send( SSM_PROC_MODEM,
		       SSM_PTOM_REQ_PERM_FILE, 
		       E_SSM_SUCCESS,
                       ssm_parser_clients[id].filename, 
                       strlen(ssm_parser_clients[id].filename) );

  if ( err == E_SSM_SUCCESS )
  {
    /* Update ssm_parser_clnt_process */
    ssm_parser_clnt_process = id;
  }
  else 
  {
    SSM_MSG_HIGH (" ssm_parser_send_read_request : IPC to modem failed  ");
    /* Intialize SSM client */
    if ( ssm_parser_clients[id].ssm_data_ptr )
      free( ssm_parser_clients[id].ssm_data_ptr );

    ssm_parser_clients[id].ssm_data_ptr = NULL;
    ssm_parser_clients[id].status = SSM_CLIENT_STATUS_INITIALIZATION_FAILED; 
        
    /* Callback updates status of the initialization to the client */
    if ( ssm_parser_clients[id].ssm_cb && 
	 ssm_parser_clients[id].status_fb == SSM_CLIENT_STATUS_INITIALIZED )
      ssm_parser_clients[id].ssm_cb ( E_SSM_SUCCESS );
    else if ( ssm_parser_clients[id].ssm_cb )
      ssm_parser_clients[id].ssm_cb ( err );
    
  }

} /* ssm_parser_send_read_request */

#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */


/**
  
The function is invoked by SSM task to parse perm file based on queue

@param[in] 
None

@return
None

@dependencies
None.
*/


void ssm_parse_perm_file(void)
{
 
  /* Temp pointer */
  ssm_client_queue_node_struct * temp_node_ptr;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_MSG_MED (" Function : ssm_parse_perm_file ");
  
  #ifndef FEATURE_SSM_PERIPHERAL_SERVER_PROC
  
  if ( ssm_is_smdl_ready( SSM_PROC_MODEM ) == FALSE )
    return;

  #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
  
  /* Check if any client is in the middle of ssm_get_perm, If the mode updated 
  has occurred all the clients status would be something other than 
  SSM_CLIENT_STATUS_INITIALIZED. This function is only meant to be called 
  from SSM task */

  if ( ssm_parser_get_perm_counter != 0 )
  {
    /* Signal SSM task to start parsing perm file */
    #ifdef FEATURE_SSM_REX_API_PRESENT
      rex_set_sigs(ssm_tcb_ptr, SSM_PARSE_PERM_FILE_SIG );
    #else /* FEATURE_SSM_REX_API_PRESENT */
      qurt_anysignal_set( &ssm_sig_qurt, SSM_PARSE_PERM_FILE_SIG );
    #endif /* FEATURE_SSM_REX_API_PRESENT */

    return;
  }

  #ifndef FEATURE_SSM_PERIPHERAL_SERVER_PROC

  if ( ssm_parser_clnt_process != SSM_MAX_CLIENTS )
  {
    /* Some client is in process, once the current client is done . it will 
       re-invoke the same function to restart processing of remaining clients */
    return;
  }

  #endif /* !FEATURE_SSM_PERIPHERAL_SERVER_PROC */

  /* Get pointer to queue node */
  temp_node_ptr = (ssm_client_queue_node_struct *)q_get( &ssm_perm_file_q );

  while ( temp_node_ptr != NULL )
  {
	    
    SSM_MSG_MED_1 (" ssm_parse_perm_file File read id=%d ", temp_node_ptr->client_id); 

    #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
      ssm_parser_read_permission_file(temp_node_ptr->client_id);
    #else /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
      /* Read requests are sequential, hence return */
      ssm_parser_send_read_request(temp_node_ptr->client_id);
      free( temp_node_ptr );
      return ;
    #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
    
    qurt_pimutex_lock(&ssm_parser_clients[temp_node_ptr->client_id].mutex);

    temp_node_ptr->in_queue = FALSE;

    qurt_pimutex_unlock(&ssm_parser_clients[temp_node_ptr->client_id].mutex);
    
    temp_node_ptr = (ssm_client_queue_node_struct *)q_get( &ssm_perm_file_q );
    
   } /* while ( temp_node_ptr != NULL ) */

   #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
   ssm_parser_sfs_ready_retry( ssm_parser_sfs_ready_try_id );
   #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

} /* ssm_parse_perm_file */

/** 
Registers a client with ssm module. 

@param[in] filename                       Filename to be parsed.
@param[in] num_of_attr                    Number of attributes in the date file
@param[in] default_perm                   Default permission to be returned when 
                                          entry not found  
@param[in] attr_type                      attribute  types - uint32 or string  
@param[in] ssm_cb_type                    Callback which will be called reporting the status of 
                                          ssm clients
@param[out] uint32 *                      ID of the client
@param[in]  char *                        Fallback buffer if permission file is not present
@param[in] uint32                         Length of flabback buffer

@return
ssm_err_t                                 E_SSM_SUCCESS if client already initialized 
                                          or updated with fallback buffer
                                          E_SSM_CLIENT_INIT_IN_PROCESS in process and not 
					  updated with fallback buffer
                                          E_SSM_FAIL failed
					  E_SSM_CLIENT_MAX_ID_REACHED max id reached
					  E_SSM_CLIENT_ATTR_TYPE_NOT_SUPPORTED attribute not supported
					  E_SSM_DATA_MALLOC_FAILED Dynamic memory allocation failed

@dependencies
None.
*/


ssm_err_t ssm_client_init_func
( 
  char *        filename, 
  uint32        num_of_attr, 
  uint32        default_perm,
  ssm_attr_t    attr_type,
  ssm_cb_type   ssm_cb,
  uint32 *      client_id,
  char   *      fallback_buffer,
  uint16        len_fallback_buffer
)
{
  
  /* Iterator variable */
  int iter;
  
  boolean new_client = TRUE;

  ssm_err_t err = E_SSM_FAIL;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_MSG_MED (" Function : ssm_client_init_func " );

  if ( ssm_parser_clients == NULL )
    return E_SSM_FAIL;

  SSM_INTLOCK()

  /* Get client Id */
  *client_id = num_clients_registered;

  /* Iterate through SSM Clients */
  for ( iter = 0; iter < SSM_MAX_CLIENTS; iter++ )
  {
    /* Check if filename of new client matches to the one of an old client */
    if( strncmp( filename, ssm_parser_clients[iter].filename, MAX_FILE_NAME_SIZE ) == 0 )
    {
      SSM_MSG_MED_1 (" Client is already registered with SSM .. client_id = %d ", \
		      iter);
      
      /* Set new_client to FALSE, as the client was already detected */
      new_client = FALSE;
      if ( ssm_parser_clients[iter].status == SSM_CLIENT_STATUS_READ_FILE_IN_QUEUE
       || ssm_parser_clients[iter].status == SSM_CLIENT_STATUS_READ_FILE_IN_PROGESS
       || ssm_parser_clients[iter].status == SSM_CLIENT_STATUS_INITIALIZED
       || ssm_parser_clients[iter].status_fb == SSM_CLIENT_STATUS_INITIALIZED )
      {	      

	SSM_MSG_MED_1 (" Client is already initialized with SSM .. client_id = %d ", \
			iter);

       /* Dont create a new client. Return old client id */
        *client_id = iter;

       /* There is a chance that read_in_queue might have moved to read_in_progress */
       /* Hence check for initializtion */
       if ( ssm_parser_clients[iter].status == SSM_CLIENT_STATUS_INITIALIZED
	 || ssm_parser_clients[iter].status_fb == SSM_CLIENT_STATUS_INITIALIZED )
       {    
          SSM_INTFREE()
	  return E_SSM_SUCCESS ;
       }
       SSM_INTFREE()
	  return E_SSM_CLIENT_INIT_IN_PROCESS;
      }
      else 
      {
	SSM_MSG_MED_1(" Reinitialize client since previous initialization failed.. \
		        client_id = %d ", iter);
	
	/* Retry initializing the id */
	*client_id = iter;
	break;
      }
    }
  }
  
  /* Check if a new ssm client can be registered */
  if ( *client_id > SSM_MAX_CLIENTS - 1  )
  {
    SSM_INTFREE()

    SSM_MSG_ERROR ( " SSM Max clients limit reached " );

    return E_SSM_CLIENT_MAX_ID_REACHED;
  }

  /* STRING TYPE is not supported */
  if ( attr_type!=SSM_UINT32 )
  {
    SSM_INTFREE()
    
    SSM_MSG_ERROR ( " SSM string type not supported " );
    return E_SSM_CLIENT_ATTR_TYPE_NOT_SUPPORTED;
  }

  if ( fallback_buffer == NULL || len_fallback_buffer == 0 )
  {
    ssm_parser_clients[*client_id].fallback_buffer = NULL ;
    ssm_parser_clients[*client_id].len_fallback_buffer =0 ;
  }
  else
  {
    /* Copy meta data into clients strcuture */
    ssm_parser_clients[*client_id].fallback_buffer = malloc ( len_fallback_buffer + 1 );

    if ( ssm_parser_clients[*client_id].fallback_buffer == NULL )
    {
      SSM_INTFREE()
    
      SSM_MSG_ERROR ( " Dynamic Allocation failed " );
      return E_SSM_DATA_MALLOC_FAILED;
    }

    strlcpy( ssm_parser_clients[*client_id].fallback_buffer, fallback_buffer, 
             len_fallback_buffer +1 );
    ssm_parser_clients[*client_id].len_fallback_buffer = len_fallback_buffer;
  
  }
  
  strlcpy( ssm_parser_clients[*client_id].filename, filename, MAX_FILE_NAME_SIZE);
  ssm_parser_clients[*client_id].num_of_attr = num_of_attr;
  ssm_parser_clients[*client_id].default_perm = default_perm;
  ssm_parser_clients[*client_id].max_num_of_entries= SSM_MAX_ENTRIES_EXPECTED;
  ssm_parser_clients[*client_id].num_of_entries = 0;
  ssm_parser_clients[*client_id].max_num_of_entries_fb = SSM_MAX_ENTRIES_EXPECTED;
  ssm_parser_clients[*client_id].num_of_entries_fb = 0;
  ssm_parser_clients[*client_id].attr_t= attr_type;
  ssm_parser_clients[*client_id].status = SSM_CLIENT_STATUS_READ_FILE_IN_QUEUE;
  ssm_parser_clients[*client_id].ssm_cb = ssm_cb;

  #ifndef FEATURE_SSM_PERIPHERAL_SERVER_PROC
  ssm_parser_clients[*client_id].last_pkt_num = -1;
  #endif /* !FEATURE_SSM_PERIPHERAL_SERVER_PROC */

  if ( fallback_buffer != NULL && len_fallback_buffer != 0 )
  {
    err = ssm_parser_process_input_buffer_append( *client_id, 
                                                  ssm_parser_clients[*client_id].len_fallback_buffer,
                                                  ssm_parser_clients[*client_id].fallback_buffer );

    if ( err == E_SSM_SUCCESS )
    {
      /* Intialize SSM client */
      ssm_parser_clients[*client_id].status_fb = SSM_CLIENT_STATUS_INITIALIZED; 
      ssm_parser_clients[*client_id].max_num_of_entries_fb = 
                  ssm_parser_clients[*client_id].max_num_of_entries;
      ssm_parser_clients[*client_id].num_of_entries_fb = 
                  ssm_parser_clients[*client_id].num_of_entries;
      ssm_parser_clients[*client_id].current_entry_loc_fb = 
                  ssm_parser_clients[*client_id].current_entry_loc;
      ssm_parser_clients[*client_id].ssm_data_ptr_fb = 
                  ssm_parser_clients[*client_id].ssm_data_ptr;
            
    }
    else
    {
      ssm_parser_clients[*client_id].status_fb = SSM_CLIENT_STATUS_INITIALIZATION_FAILED; 
    }

    ssm_parser_clients[*client_id].max_num_of_entries = SSM_MAX_ENTRIES_EXPECTED;
    ssm_parser_clients[*client_id].num_of_entries = 0;
    ssm_parser_clients[*client_id].current_entry_loc = 0;
    ssm_parser_clients[*client_id].ssm_data_ptr = NULL;
  }

  /* Queue up the client to parse perm file from SSM_TASK context*/ 
  if ( E_SSM_FAIL == ssm_client_queue_read_file(*client_id) )
  {
    SSM_INTFREE()
    return err;
  }

  /* Signal SSM task to start parsing perm file */
  #ifdef FEATURE_SSM_REX_API_PRESENT
    rex_set_sigs(ssm_tcb_ptr, SSM_PARSE_PERM_FILE_SIG );
  #else /* FEATURE_SSM_REX_API_PRESENT */
    qurt_anysignal_set( &ssm_sig_qurt, SSM_PARSE_PERM_FILE_SIG );
  #endif /* FEATURE_SSM_REX_API_PRESENT */


  /* Increment number of clients registered, if a new client was detected */
  if ( new_client )
    num_clients_registered++;

  SSM_INTFREE()
  
  SSM_MSG_MED_1 (" Resigtered client with SSM Init in progress ..client_id = %d ", \
		  iter );
  
  if ( err != E_SSM_SUCCESS )
    return E_SSM_CLIENT_INIT_IN_PROCESS;
  else 
    return err;

} /* ssm_client_init_func */


/** 
Checks if the client is initialized or not

@param[in] filename                       Filename of the client. Identifies client.

@return
boolean                                   TRUE if initialized 
                                          FALSE otherwise

@dependencies
None.
*/


boolean ssm_parser_is_client_initialized
( 
  char *        filename
)
{
  
  /* Iterator variable */
  int iter;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_MSG_MED (" Function : ssm_parser_is_client_initialized " );

  if ( ssm_parser_clients == NULL )
    return FALSE;

  /* Iterate through SSM Clients */
  for ( iter = 0; iter < SSM_MAX_CLIENTS; iter++ )
  {
    /* Check if filename of new client matches to the one of an old client */
    if( strncmp( filename, ssm_parser_clients[iter].filename, MAX_FILE_NAME_SIZE ) == 0 )
    {
      return ( ssm_parser_clients[iter].status == SSM_CLIENT_STATUS_INITIALIZED) ? TRUE : FALSE;
    }
  }
  
  return FALSE;

} /* ssm_parser_is_client_initialized */


/** 
Gets the number of entries of perm data if client is initialized

@param[in]  client_id                Id of the client
@param[out] num_ent                  number of entries of perm data 
  
@return
ssm_err_t  - E_SSM_SUCCESS if success
             E_SSM_CLIENT_NOT_INITIALIZED if client not initialized
	     E_SSM_INVALID_ID if arguments are invalid
	     
@dependencies
num_ent will be set to zero if the operation failed

*/

ssm_err_t ssm_get_perm_data_size
( 
  uint32 client_id,
  uint32 * num_ent
)
{
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_INTLOCK()

  /* Check if parameters are valid */
  if ( num_ent == NULL )
  {
    SSM_INTFREE()
    return E_SSM_INVALID_ID;
  }

  /* Validate SSM Client */
  if ( client_id >= num_clients_registered )
  {
    *num_ent = 0;
    SSM_INTFREE()
    return E_SSM_INVALID_ID;
  }
	  
  /* Check if client is initialized */
  if ( ssm_parser_clients[client_id].status != SSM_CLIENT_STATUS_INITIALIZED
    && ssm_parser_clients[client_id].status_fb != SSM_CLIENT_STATUS_INITIALIZED )
  {
    *num_ent = 0;
    SSM_INTFREE()
    return E_SSM_CLIENT_NOT_INITIALIZED;
  }

  /* Get the size of perm data (in number of entries) */
  if ( ssm_parser_clients[client_id].status == SSM_CLIENT_STATUS_INITIALIZED )
    *num_ent = ssm_parser_clients[client_id].num_of_entries ;
  else
    *num_ent = ssm_parser_clients[client_id].num_of_entries_fb ;

  SSM_INTFREE()

  return E_SSM_SUCCESS;

} /* ssm_get_perm_data_size */


/** 
Copies the perm data to buffer allocated by the client and de initailizes 
the client.

@param[in]  client_id                Id of the client
@param[in]  size                     size of the buffer in bytes
@param[in]  perm_data_ptr            address of the buffer
  
@return
ssm_err_t  - E_SSM_SUCCESS if success
             E_SSM_CLIENT_NOT_INITIALIZED if client not initialized
	     E_SSM_INVALID_ID if arguments are invalid

@side_effects
SSM client will be deinitialized from ssm subsystem.

*/

ssm_err_t ssm_copy_perm_data 
( 
  uint32 client_id,
  uint32 size,
  void * perm_data_ptr
)
{
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_INTLOCK()

  /* Check if parameters are valid */
  if ( perm_data_ptr == NULL )
  {
    SSM_INTFREE()
    return E_SSM_INVALID_ID;
  }

  /* Validate SSM Client */
  if ( client_id >= num_clients_registered )
  {
    SSM_INTFREE()
    return E_SSM_INVALID_ID;
  }
	  
  /* Check if client is initialized */
  if( ssm_parser_clients[client_id].status != SSM_CLIENT_STATUS_INITIALIZED
    && ssm_parser_clients[client_id].status_fb != SSM_CLIENT_STATUS_INITIALIZED )
  {
    SSM_INTFREE()
    return E_SSM_CLIENT_NOT_INITIALIZED;
  }

  /* Get the size of perm data in bytes */
  if ( ssm_parser_clients[client_id].status == SSM_CLIENT_STATUS_INITIALIZED &&
       size == ssm_parser_clients[client_id].num_of_entries * SIZEOFATTR_PERM(client_id) )
  {
    /* Copy perm data into allocated buffer */
    memscpy (  perm_data_ptr, size, ssm_parser_clients[client_id].ssm_data_ptr, 
    ssm_parser_clients[client_id].num_of_entries * SIZEOFATTR_PERM(client_id) );
  }
  else if ( ssm_parser_clients[client_id].status_fb == SSM_CLIENT_STATUS_INITIALIZED &&
       size == ssm_parser_clients[client_id].num_of_entries_fb * SIZEOFATTR_PERM(client_id) )
  {
    /* Copy perm data into allocated buffer */
    memscpy (  perm_data_ptr, size, ssm_parser_clients[client_id].ssm_data_ptr_fb, 
    ssm_parser_clients[client_id].num_of_entries_fb * SIZEOFATTR_PERM(client_id) );
  }
  else
  {
    SSM_INTFREE()
    return E_SSM_FAIL;
  }

  SSM_INTFREE()
  return E_SSM_SUCCESS;

} /* ssm_copy_perm_data */


/** 
Returns ssm mode 

@param[in] None                          

@return
uint32                                     SSM mode

@dependencies
None.
*/


uint32 ssm_parser_get_mode( void )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  return ssm_parser_mode;

} /* ssm_parser_get_mode */
  
  
/** 
Update ssm mode and reinitialize clients to read from the new file

@param[in] mode                           New Mode of SSM

@return
boolean TRUE - if success
        FALSE - if fails

@dependencies
None.
*/


void ssm_parser_update_mode
( 
  uint32 mode
)
{
  /* Temporary iter variable */
  uint32                                iter;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_MSG_MED_1 ( " Function : ssm_parser_update_mode mode = %d ", mode);
  
  if ( mode == SSM_DEFAULT_MODE )
  {
    if ( ssm_parser_mode_timer != NULL )
      timer_clr( ssm_parser_mode_timer, T_SEC );
  }
  else
  {
    if ( ssm_parser_mode_timer != NULL )
      timer_set( ssm_parser_mode_timer, SSM_TIMER_EXPIRY_PERIOD_SEC, 0, T_SEC );
  }
  
  SSM_INTLOCK()
	  
  /* return, if new mode is same as old mode */
  if ( ssm_parser_mode == mode )
  {
    SSM_INTFREE()
    SSM_MSG_HIGH_1 ( " Same mode %d ...not udpated ", mode);
    return ;
  }
  
  /* Update new ssm mode */
  ssm_parser_mode = mode;

  /* Update ssm_parser_last_tick with current timetick */
  ssm_parser_last_tick = timetick_get();

  #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
  if ( ssm_parser_remote_perm_file_read.initialized == TRUE )
    free ( ssm_parser_remote_perm_file_read.filename );
  ssm_parser_remote_perm_file_read.initialized     = FALSE;
  ssm_parser_remote_perm_file_read.len_file_read   = 0;
  ssm_parser_remote_perm_file_read.bytes_read_last = 0;
  ssm_parser_remote_perm_file_read.num_pkts_sent   = 0;
  ssm_parser_remote_perm_file_read.data_complete   = FALSE;
  #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

  if ( ssm_parser_clients == NULL )
  {
    SSM_INTFREE()
    return;
  }

  /* Iterate through all the clients */
  for ( iter = 0; iter < num_clients_registered; iter++ )
  {
    /* Free up memory with previous reads */
    free ( ssm_parser_clients[iter].ssm_data_ptr );

    /* Set Initilization attribute to FALSE */
    ssm_parser_clients[iter].status = SSM_CLIENT_STATUS_READ_FILE_IN_QUEUE;
    ssm_parser_clients[iter].ssm_data_ptr = NULL;
    ssm_parser_clients[iter].max_num_of_entries= SSM_MAX_ENTRIES_EXPECTED;
    #ifndef FEATURE_SSM_PERIPHERAL_SERVER_PROC
    ssm_parser_clients[iter].last_pkt_num = -1;
    #endif /* !FEATURE_SSM_PERIPHERAL_SERVER_PROC */
    ssm_parser_clients[iter].num_of_entries = 0;

    /* Queue up the client to parse perm file from SSM_TASK context*/ 
    if ( E_SSM_FAIL == ssm_client_queue_read_file( iter ) )
    {
      SSM_INTFREE()
      return;
    }
  }

  SSM_INTFREE()

  /* This function is already processed in SSM task context, but to have a commom 
     function to serialize all read operation , resignal the SSM_TASK.to start 
     parsing perm file */
  #ifdef FEATURE_SSM_REX_API_PRESENT
    rex_set_sigs(ssm_tcb_ptr, SSM_PARSE_PERM_FILE_SIG );
  #else /* FEATURE_SSM_REX_API_PRESENT */
    qurt_anysignal_set( &ssm_sig_qurt, SSM_PARSE_PERM_FILE_SIG );
  #endif /* FEATURE_SSM_REX_API_PRESENT */

    
} /* ssm_parser_update_mode */


/** 
Timer callback to restore mode to default mode 

@param[in] data                           Data for timer callback. Its NULL
                                          for this timer.

@return
None

@dependencies
None.
*/

void ssm_parser_timer_callback
(
  timer_cb_data_type              data  
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ssm_parser_update_mode( SSM_DEFAULT_MODE );
  
} /* ssm_parser_timer_callback */


/** 
Customized binary search for ssm data read from file and stored in a structure.
Binary Search looks if the search val falls in the range on the lower index and 
then on the upper index. If both the search fails, it searches for the 
middle index. Finally it decides to search any one of the two halves.

@param[in] id                             Id of the smm_client 
@param[in/out] low                        Lower index of the search
@param[in/out] up                         Upper index of the search
@param[in] search_arg_num                 Arg num that is searched for. Used to
                                          offset to reqd location
@param[in] search_val                     Value that is searched for
@param[in] start_ptr                      Buffer in which the value should be searched
@param[in] num_of_entires                 Number of entries in the buffer

@return
None

@dependencies
None.
*/

static void ssm_parser_bin_search
(
  uint32                      id,
  int                       * low,
  int                       * up,
  uint32                      search_arg_num, 
  ssm_attr_data_t             search_val,
  char *                      start_ptr,
  uint32                      num_of_entries
)
{
  /* Size of ssm_data */
  uint32 inc_size;
  
  /* Address to various structure */
  char  *lower_boundary , *upper_boundary, * middle_boundary, *temp_boundary;

  /* Temp values */
  uint32 low_limit, up_limit, argument_offset;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Size of a row of attributes */
  inc_size = SIZEOFATTR_PERM(id);

  /* Store lower index and upper index in temporary values */
  low_limit = *low;
  up_limit = *up;
  
  /* Address offset between start of the index to start of the argument  */
  argument_offset = search_arg_num * 2 * sizeof(ssm_attr_data_t);

  /* Get address of the lower, upper and middle rows of the ssm_data structure */
  lower_boundary = start_ptr + (*low) * inc_size + argument_offset;
  upper_boundary = start_ptr + (*up) * inc_size + argument_offset; 
  middle_boundary = start_ptr + ((*up + *low)/2) * inc_size + argument_offset ;

  /* Start Bin search */

  /* Case when the value being searched for is missing */
  if ( *( ssm_attr_data_t *)lower_boundary > search_val )
  {
    /* Update low and up values as -1 and return */
    *low = -1;
    *up  = -1;
    return;
  }
  
  /* Case when the value being searched for is greater than value at upper boundary */
  else if ( search_val > (*(ssm_attr_data_t *)(upper_boundary + sizeof(ssm_attr_data_t))) )
  {
    /* Update low and up values as -1 and return */
    *low = -1;
    *up  = -1;
    return;
  }

  /* Case when the value being searched falls in the range of values pointed by lower boundary */
  else if ( *(ssm_attr_data_t *)lower_boundary <= search_val 
           && search_val <= (*(ssm_attr_data_t *)(lower_boundary + sizeof(ssm_attr_data_t) ))  )
  {
    /* Update up as low */
    *up = *low;
    
    /* Set temp_boundary to point at low +1 index */
    temp_boundary = lower_boundary + inc_size;

    /* Search for similar range below lower boundary */
    while ( (temp_boundary < start_ptr + inc_size * num_of_entries ) 
     && up_limit > *up 
     && ( *(ssm_attr_data_t *)temp_boundary <= search_val 
     && search_val <= (*(ssm_attr_data_t *)(temp_boundary + sizeof(ssm_attr_data_t) ))) )
    {
      /* If search suceeds, increment up by 1 */
      *up = *up + 1;
      temp_boundary += inc_size;
    } /* while loop */

    /* Return since search is complete */
    return;
  }

  /* Case when the value being searched falls in the range of values pointed by upper boundary */
  else if ( *(ssm_attr_data_t *)upper_boundary <= search_val 
       && search_val <= (*(ssm_attr_data_t *)(upper_boundary + sizeof(ssm_attr_data_t)))  )
  {
    /* Update low as up */
    *low = *up;
    temp_boundary = upper_boundary - inc_size;

    /* Search for similar range above upper boundary */
    while ( temp_boundary > start_ptr && low_limit < *low 
      && *(ssm_attr_data_t *)temp_boundary <= search_val 
      && search_val <= (*(ssm_attr_data_t *)(temp_boundary + sizeof(ssm_attr_data_t))))
    {
      /* If search suceeds, decrement low by 1 */
      *low = *low - 1;
      temp_boundary -= inc_size;
    } /* while loop */

    /* Return since search is complete */
    return;
  }

  /* To stop recursive loop incase value being searched for is not present. 
     Low and up index are consecutive*/
  else if ( (*up + *low)/2 == *low )
  {
    /* Update low and up values as -1 and return */
    *low = -1;
    *up  = -1;
    return;
  }

  /* Check if the value falls in upper half of the structure */
  else if ( *(ssm_attr_data_t *)middle_boundary > search_val )
  {
    /* Set up as middle index */
    *up = (*low + *up)/2 ;

    /* Recursive bin search in uuper half */
    ssm_parser_bin_search(id, low, up, search_arg_num, search_val, start_ptr, num_of_entries);
  }

  /* Case when the value being searched falls in the range of values pointed by middle boundary */
  else if ( *(ssm_attr_data_t *)middle_boundary <= search_val 
          && search_val <= (*(ssm_attr_data_t *)(middle_boundary + sizeof(ssm_attr_data_t)))  )
  {
    /* Update up and low as middle midex */
    *low = (*low + *up)/2 ;
    *up = *low;
    temp_boundary = middle_boundary - inc_size;

    /* Search for similar range above middle boundary */
    while ( temp_boundary > start_ptr && low_limit < *low
     && *(ssm_attr_data_t *)temp_boundary <= search_val 
     && search_val  <= (*(ssm_attr_data_t *)(temp_boundary + sizeof(ssm_attr_data_t))))
    {
      /* If search suceeds, decrement low by 1 */
      *low = *low - 1;
      temp_boundary -= inc_size;
    }
  
    /* Set temp_boundary to point at middle +1 index */
    temp_boundary = middle_boundary + inc_size;

    /* Search for similar range above middle boundary */
    while ( temp_boundary < start_ptr + inc_size * num_of_entries  && up_limit > *up
      && *(ssm_attr_data_t *)temp_boundary <= search_val 
      && search_val <= (*(ssm_attr_data_t *)(temp_boundary + sizeof(ssm_attr_data_t))))
    {
      /* If search suceeds, increment up by 1 */
      *up = *up + 1;
      temp_boundary += inc_size;
    }
    
    /* Return since search is complete */
    return;

  }
  /* Check if the value falls in lower half of the structure */
  else /* if ( (*(uint32 *)(middle_boundary + sizeof(ssm_attr_data_t))) < search_val  ) */
  {
    /* Set low as middle index */
    *low = (*low + *up)/2 ;

    /* Recursive bin search in uuper half */
    ssm_parser_bin_search(id, low, up, search_arg_num, search_val, start_ptr, num_of_entries);
  }
} /* ssm_parser_bin_search */


/** 
Get permission set for a particular ssm attribute 

@param[out] ssm_perm_t                    Corresponding permission
@param[in] arg1                           Number of argument
@param[in] ...                            ellipsis - variable args
                                          in the order of client id, 
					  and argument set 

@return
ssm_err_t    E_SSM_SUCCESS if success
             E_SSM_CLIENT_NOT_INITIALIZED if client not initialized
	     E_SSM_INVALID_ID if arguments are invalid
	     E_SSM_GET_PERM_MAX_COUNTER_REACHED max simulatneous get perm 
	     E_SSM_GET_PERM_CLIENT_DENIED request denied
	     E_SSM_GET_PERM_INVALID_NUM_ARGS invalid num of arguments
	     E_SSM_GET_PERM_MODE_CHANGE_OCCURRED mode change occurred;

@dependencies
None.
*/

ssm_err_t ssm_get_perm
(
  ssm_perm_data_t * perm,
  uint32 arg1,
  ...
)
{
  /* Intially set as low and upper index of table for binary search */
  int low, up;

  /* Tempoary values */
  uint32 level_of_search, iter =0;

  /* Tempoarary void pointers */
  char * boundary;

  /* Variable for variable args */
  va_list attr;

  /* Attribute to be searched */
  ssm_attr_data_t search_val;

  /* Client Id to be looked upon */
  uint32 id;

  /* Captures Initial mode of the system */
  uint32 initial_mode;
  
  /* Captures tick when initial_mode was set */
  timetick_type tick;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  //SSM_MSG_MED (" Function : ssm_get_perm " );

  SSM_INTLOCK()
	
  /* Check if ssm_parser_get_perm_counter has exceeded its max value */	  
  if ( ssm_parser_get_perm_counter == SSM_GET_PERM_MAX )
  {
    SSM_INTFREE()
    *perm= SSM_GET_PERM_ERR;
    SSM_MSG_ERROR ( " Max possible concurrent get_perm calls reached .. \
		      cannot return permission " );
    return E_SSM_GET_PERM_MAX_COUNTER_REACHED;
  }
  
  /* Increment ssm_parser_get_perm_counter */
  ssm_parser_get_perm_counter++;
  
  /* Capture Initial mode of the system */
  initial_mode = ssm_parser_mode;

  /* Captures tick when initial_mode was set */
  tick = ssm_parser_last_tick;

  va_start(attr, arg1);
  
  /* Get id vargs */
  id = va_arg(attr, uint32 );

  if ( id >= num_clients_registered )
  {
    va_end( attr );

    /* Decrement ssm_parser_get_perm_counter */
    ssm_parser_get_perm_counter--;

    SSM_INTFREE()
    *perm= SSM_GET_PERM_ERR;
    SSM_MSG_ERROR_2 ( " Invalid ID %d Max Id possible %d ", id, \
		    num_clients_registered-1 );
    return E_SSM_GET_PERM_INVALID_ID;
  }

  /* Permission to be returned */
  *perm = ssm_parser_clients[id].default_perm;

  /* If client is not yet initialized, return error. 
     Here, client may be in the process of udpating its data members or reading 
     perm file. A retry might result in proper return */
  if ( ssm_parser_clients[id].status != SSM_CLIENT_STATUS_INITIALIZED 
    && ssm_parser_clients[id].status_fb != SSM_CLIENT_STATUS_INITIALIZED )
  {
    va_end( attr );

    /* Decrement ssm_parser_get_perm_counter */
    ssm_parser_get_perm_counter--;

    SSM_INTFREE()
    SSM_MSG_ERROR_1 ( " SSM Client not initialized for client_id %d ", id );

    return E_SSM_GET_PERM_CLIENT_NOT_INITIALIZED;
  }

  /* Size of the row */
  uint32 inc_size = SIZEOFATTR_PERM(id);
  
  /* Get number of arguments */
  level_of_search = ssm_parser_clients[id].num_of_attr ;
  
  /* Check for number of arguments */
  if ( arg1 != 2 + level_of_search )
  {
    va_end( attr );
    
    /* Decrement ssm_parser_get_perm_counter */
    ssm_parser_get_perm_counter--;

    SSM_INTFREE()
    
    SSM_MSG_ERROR_3 ( " Invalid number of arguments for id %d args expected %d args received",\
		    id, arg1,  2 + level_of_search );
    return E_SSM_GET_PERM_INVALID_NUM_ARGS;
  }

  if ( ssm_parser_clients[id].status == SSM_CLIENT_STATUS_INITIALIZED )
  {
    /* Assign low and up values for binary search */
    low = 0;
    up = ssm_parser_clients[id].num_of_entries -1;

    /* Iterate through number of arguments */
    while ( iter < level_of_search )
    {
      /* Get argument to be searched from variable arguments */
      search_val = va_arg ( attr, uint32);
    
      /* Spin Customized Binary search for the search_val */
      ssm_parser_bin_search( id, &low, &up, iter, search_val, 
        ssm_parser_clients[id].ssm_data_ptr, ssm_parser_clients[id].num_of_entries );
    
      /* If no entries found , return default permission */ 
      if ( low == -1 && up == -1 )
      {
        va_end( attr );
      
        /* Decrement ssm_parser_get_perm_counter */
        ssm_parser_get_perm_counter--;

        SSM_INTFREE()
      
        /* Return Default permission */
        return E_SSM_SUCCESS;
      }
      iter++;
    }
  }
  else 
  {
    /* Assign low and up values for binary search */
    low = 0;
    up = ssm_parser_clients[id].num_of_entries_fb -1;

    /* Iterate through number of arguments */
    while ( iter < level_of_search )
    {
      /* Get argument to be searched from variable arguments */
      search_val = va_arg ( attr, uint32);
    
      /* Spin Customized Binary search for the search_val */
      ssm_parser_bin_search( id, &low, &up, iter, search_val, 
        ssm_parser_clients[id].ssm_data_ptr_fb, ssm_parser_clients[id].num_of_entries_fb );
    
      /* If no entries found , return default permission */ 
      if ( low == -1 && up == -1 )
      {
        va_end( attr );
      
        /* Decrement ssm_parser_get_perm_counter */
        ssm_parser_get_perm_counter--;

        SSM_INTFREE()
      
        /* Return Default permission */
        return E_SSM_SUCCESS;
      }
      iter++;
    }
  } 
  
  /* If multiple entries found , return default permission */
  if ( ( low == -1 && up == -1 ) || low != up )
  {
    va_end( attr );
    
    /* Decrement ssm_parser_get_perm_counter */
    ssm_parser_get_perm_counter--;

    SSM_INTFREE()

    /* Return Default permission */
    return E_SSM_SUCCESS;
  }

  /* Get the address of ssm data found */
  if ( ssm_parser_clients[id].status == SSM_CLIENT_STATUS_INITIALIZED )
    boundary =  ssm_parser_clients[id].ssm_data_ptr + low * inc_size + SIZEOFATTR(id);
  else
    boundary =  ssm_parser_clients[id].ssm_data_ptr_fb + low * inc_size + SIZEOFATTR(id);

  va_end( attr );

  /* ssm_parser_mode has changed between execution of this function, The perm value read from 
     the memory might ahev been altered intermediately. Hence, return an error on the
     safe side*/ 
  
  if ( initial_mode != ssm_parser_mode || tick != ssm_parser_last_tick )
  {
    
    /* Decrement ssm_parser_get_perm_counter */
    ssm_parser_get_perm_counter--;
    
    SSM_INTFREE()

    SSM_MSG_ERROR_3 ( " Cannot return permission for id %d.. mode changed in between the \
		    call from %d to %d", id, initial_mode,  ssm_parser_mode );

    return E_SSM_GET_PERM_MODE_CHANGE_OCCURRED;
  }

  /* Decrement ssm_parser_get_perm_counter */
  ssm_parser_get_perm_counter--;
 
  SSM_INTFREE()  

  /* Return the associated permission */
  *perm = *(ssm_perm_data_t *)boundary;

  return E_SSM_SUCCESS;

} /* ssm_get_perm */

/** 
Initializes private static data members of ssm_parser

@param[in] 
None

@return
None

@dependencies
None.
*/

void ssm_parser_init( void )
{
  uint16 iterator =0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
  if ( NULL == pssm_parser_mutex )              
  {                                      
    qurt_pimutex_init(&ssm_parser_mutex);       
    pssm_parser_mutex = &ssm_parser_mutex;             
  }	  
  #else 
  if ( 0 == ssm_parser_mutex )
    ssm_parser_qmutex_init();  

  #endif   /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */	  

  ssm_parser_clients = calloc ( SSM_MAX_CLIENTS, 
		               sizeof ( struct ssm_parser_clients_struct ));

  if ( ssm_parser_clients == NULL )
  {
    SSM_MSG_ERROR ( " SSM DYNAMIC ALLOCATION FAILED ");
    return;
  }
  
  for ( iterator = 0; iterator < SSM_MAX_CLIENTS; iterator++ )
  {
    qurt_pimutex_init(&ssm_parser_clients[iterator].mutex);
    ssm_parser_clients[iterator].q_node.client_id = iterator;    
  }

  if( ssm_client_queue_init == FALSE )
  {
    q_init(&ssm_perm_file_q);
    ssm_client_queue_init = TRUE;
  }

  #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
    /* Initialize Queue */
    q_init( &ssm_parser_remote_read_perm_file_q );
  #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */ 

  #ifdef FEATURE_SSM_REX_API_PRESENT
  ssm_tcb_ptr = rcinit_lookup_rextask("ssm");

  if ( ssm_tcb_ptr == NULL )
  {
    SSM_ERR_FATAL(" SSM task tcb not found", 0 ,0, 0 );
  }
  #endif /* FEATURE_SSM_REX_API_PRESENT */

  ssm_parser_mode_timer = malloc( sizeof( timer_type ));

  if ( ssm_parser_mode_timer == NULL )
    return;
  
  /* Define Timer */
  timer_def( ssm_parser_mode_timer, NULL, NULL,
		0 , ssm_parser_timer_callback , (timer_cb_data_type) NULL);

} /* ssm_parser_init */

#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC

/** 
Retry SFS read if none of the sfs reads have succeeded

@param[in] 
None

@return
None

@dependencies
None.
*/

void ssm_parser_retry_sfs_read( void )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if ( ssm_parser_sfs_ready_try == TRUE )
    return;

  ssm_parser_sfs_ready_try = TRUE;

    ssm_parser_sfs_ready_retry( num_clients_registered );

} /* ssm_parser_retry_sfs_read */


/** 
Post init of ssm_parser

@param[in] 
None

@return
None

@dependencies
None.
*/

void ssm_parser_post_init( void )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  #ifdef FEATURE_SSM_REX_API_PRESENT
    (void)rex_set_sigs ( ssm_tcb_ptr, SSM_PARSER_RETRY_SFS_READ_SIG );
  #else /* FEATURE_SSM_REX_API_PRESENT */
    qurt_anysignal_set( &ssm_sig_qurt, SSM_PARSER_RETRY_SFS_READ_SIG );
  #endif /* FEATURE_SSM_REX_API_PRESENT */
	
} /* ssm_parser_post_init */


/** 

Communicates with SSM on ADSP It transfers the perm file data 
and error codes using secure channel between modem and ADSP. 


@param[in] src_proc                       Proc which requested perm file
@param[in] ipc                            Ipc Message
@param[in] err                            Error Code to be communcated
@param[in] data_complete                  Indicates if the file transfer for this is complete
@param[in] pkt_num                        Denotes pkt_num for this transfer
@param[in] data_size                      Size of the perm file data
@param[in] data                           Perm file data to be transferred

@return
void

@dependencies
None.

*/

static void ssm_parser_read_remote_perm_file_helper
(
  ssm_proc_enum_t      src_proc,
  ssm_ipc_req_t        ipc,
  ssm_err_t            err,
  boolean              data_complete,
  uint32               pkt_num,
  uint32               data_size,
  char *               data
)
{
  
  char * ipc_message;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ipc_message = malloc ( SSM_MTOP_PERM_FILE_BUFFER_LENGTH + data_size );

  if ( ipc_message == NULL )
    return;
  
  /* Create message to be encrypted */
  snprintf ( ipc_message, SSM_MTOP_PERM_FILE_BUFFER_LENGTH , 
  "%10u|%10u|%10u|", (unsigned int )data_complete, (unsigned int )pkt_num, 
                     (unsigned int )data_size );

  ipc_message[SSM_MTOP_PERM_FILE_BUFFER_LENGTH -1] = '|';

  /* Copy data to ipc_message */
  if( data != NULL )
    memscpy( ipc_message + SSM_MTOP_PERM_FILE_BUFFER_LENGTH, data_size, 
             data, data_size );

  ssm_ipc_encode_send( src_proc, ipc, err, ipc_message, 
		       SSM_MTOP_PERM_FILE_BUFFER_LENGTH + data_size );

  free( ipc_message );
  
} /* ssm_parser_read_remote_perm_file_helper */


/** 

Processes reads request for remote read file

@param[in] err                            error code send in IPC request
@param[in] proc                           Proc which requested perm file
@param[in] message                        message which contains fielname
@param[in] len                            length of the message

@return
void

@dependencies
None.

*/

static void ssm_parser_read_remote_perm_file
(
  ssm_err_t            err,
  ssm_proc_enum_t      src_proc,
  char *               message,
  uint32               len
)
{
  uint32 len_read=0;
  
  char * read_buffer;

  timetick_type       tick;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If error code is not success,de-initailize remote perm file structure */
  if ( err != E_SSM_SUCCESS )
  {
    free ( ssm_parser_remote_perm_file_read.filename );
    ssm_parser_remote_perm_file_read.initialized     = FALSE;
    ssm_parser_remote_perm_file_read.len_file_read   = 0;
    ssm_parser_remote_perm_file_read.bytes_read_last = 0;
    ssm_parser_remote_perm_file_read.num_pkts_sent   = 0;
    ssm_parser_remote_perm_file_read.data_complete   = FALSE;

    /* Communicate the failure */
    ssm_parser_read_remote_perm_file_helper( src_proc, 
                                             SSM_MTOP_PERM_FILE, 
                                             err,
				             0, /* data_complete */
				             0, /* pkt_num */
				             0, /* len_data */
				             NULL );

    /* Process remaining remote read requests */
    ssm_parser_process_remote_read_request();

    return;
  }
  
  SSM_MSG_MED ( " Function : ssm_parser_read_remote_perm_file file name generated ");
  
  /* Check if mode information has changed */
  if ( ssm_parser_remote_perm_file_read.mode_used != ssm_parser_mode )
  {
    /* De-initailize remote perm file structure */ 
    free ( ssm_parser_remote_perm_file_read.filename );
    ssm_parser_remote_perm_file_read.initialized = FALSE;
    ssm_parser_remote_perm_file_read.len_file_read = 0;
    ssm_parser_remote_perm_file_read.bytes_read_last = 0;
    ssm_parser_remote_perm_file_read.num_pkts_sent = 0;
    ssm_parser_remote_perm_file_read.data_complete = FALSE;

    /* Communicate the failure */
    ssm_parser_read_remote_perm_file_helper( src_proc, 
                                             SSM_MTOP_PERM_FILE, 
                                             E_SSM_MODE_CHANGED,
				             0, /* data_complete */
				             0, /* pkt_num */
				             0, /* len_data */
				             NULL );
      
    /* Process remaining remote read requests */
    ssm_parser_process_remote_read_request();

    return;
  }

  tick = ssm_parser_last_tick;
  
  err = ssm_parser_read_input_file( ssm_parser_remote_perm_file_read.filename,
		                    ssm_parser_remote_perm_file_read.len_file_read,
		                    &len_read, 
				    &read_buffer );

  if ( E_SSM_SUCCESS == err )
  {
    /* Evaluate data_complete */
    if ( len_read < SSM_TEMP_CHAR_ARR_SIZE )
    {
      ssm_parser_remote_perm_file_read.data_complete = TRUE;
    }
    else 
    {
      ssm_parser_remote_perm_file_read.data_complete = FALSE;
    }
    
    /* Update data members of internal structure */
    ssm_parser_remote_perm_file_read.bytes_read_last = len_read;
    ssm_parser_remote_perm_file_read.len_file_read  += len_read;

  }
  else
  {
    if ( E_SSM_DATA_MALLOC_FAILED != err  )
    {
      free( read_buffer );
    }
    read_buffer = NULL;
  }

  if ( tick != ssm_parser_last_tick )
  {
    err = E_SSM_FAIL;
    ssm_parser_remote_perm_file_read.data_complete   = FALSE;
    ssm_parser_remote_perm_file_read.num_pkts_sent   = 0;
    ssm_parser_remote_perm_file_read.bytes_read_last = 0;
    ssm_parser_remote_perm_file_read.len_file_read   = 0;
  }
   
  /* Communicate the failure/success */
  ssm_parser_read_remote_perm_file_helper( src_proc, 
                                    SSM_MTOP_PERM_FILE, 
                                    err,
   				    ssm_parser_remote_perm_file_read.data_complete, 
				    ssm_parser_remote_perm_file_read.num_pkts_sent,
				    len_read,
				    read_buffer );
  
  if ( E_SSM_SUCCESS == err )
  {
    free( read_buffer );
    
    /* Increment pkt_num */
    ssm_parser_remote_perm_file_read.num_pkts_sent++;
  }
       
} /* ssm_parser_read_remote_perm_file */


/** 

Initiates reading the remote perm file on modem processor

@param[in] 
None

@return
void

@dependencies
None.

*/
void ssm_parser_process_remote_read_request(void)
{
   
  ssm_remote_read_perm_file_node_struct * temp_node;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( ssm_parser_sfs_ready_try == FALSE )
    return;

  /* Check if some other read remote file is in process */
  if ( ssm_parser_remote_perm_file_read.initialized == FALSE )
  {
    /* Dequeue from request queue */
    temp_node = q_get( &ssm_parser_remote_read_perm_file_q);

    if ( temp_node == NULL )
      return;
   
    if ( temp_node->proc < SSM_PROC_MODEM || temp_node->proc >= SSM_PROC_MAX )
      return;
    
    /* Allocate space for remote read filename */
    ssm_parser_remote_perm_file_read.filename = 
	       (char * ) malloc( temp_node->len_filename + 1 );


    if ( ssm_parser_remote_perm_file_read.filename == NULL )
    {
       /* Compile a ipc message to processor for the failure case */
       ssm_parser_read_remote_perm_file( E_SSM_DATA_MALLOC_FAILED, 
		                         temp_node->proc, 
				         temp_node->filename, 
				         temp_node->len_filename );
       
     return;
       
    }

    /* Update data structure for read remote file */
    strlcpy( ssm_parser_remote_perm_file_read.filename, 
	      temp_node->filename, temp_node->len_filename+1 );
    ssm_parser_remote_perm_file_read.initialized = TRUE;
    ssm_parser_remote_perm_file_read.len_file_read = 0;
    ssm_parser_remote_perm_file_read.bytes_read_last = 0;
    ssm_parser_remote_perm_file_read.num_pkts_sent = 0;
    ssm_parser_remote_perm_file_read.mode_used = ssm_parser_mode;
    ssm_parser_remote_perm_file_read.data_complete = FALSE;

    /* Read file and send the encrypted data to Apps processor */
    ssm_parser_read_remote_perm_file( E_SSM_SUCCESS, 
		                      temp_node->proc, 
		                      temp_node->filename, 
				      temp_node->len_filename);

    free( temp_node->filename );
    free( temp_node );
  }

} /* ssm_parser_process_remote_read_request() */


/** 

Initiate the process for a remote read perm file

@param[in] src_proc                       Proc which requested perm file
@param[in] message                        message which contains fielname
@param[in] len                            length of the message

@return
ssm_err_t                                 E_SSM_SUCCESS if successful

@dependencies
None.

*/

ssm_err_t ssm_parser_ptom_req_perm_file
(
  ssm_proc_enum_t      src_proc,
  char *               message,
  uint32               len
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_MSG_MED( "Function : ssm_parser_ptom_req_perm_file " );
  
  if ( src_proc != SSM_PROC_ADSP )
  {
    return E_SSM_MSG_DECODE_ERR;
  }
  
  /* Allocate a node to be queued */
  ssm_remote_read_perm_file_node_struct * temp_node = 
	         malloc( sizeof( ssm_remote_read_perm_file_node_struct ) );

  if ( temp_node == NULL )
  {
    /* Communicate the failure */
    ssm_parser_read_remote_perm_file_helper( src_proc, 
		                      SSM_MTOP_PERM_FILE, 
                                      E_SSM_DATA_MALLOC_FAILED, 
				      0, /* data_complete */
				      0, /* pkt_num */
				      0, /* len_data */
				      NULL 
				      );

    return E_SSM_DATA_MALLOC_FAILED;
  }

  /* Update node */
  temp_node->proc = src_proc;
  temp_node->len_filename = len;
  temp_node->filename = malloc ( temp_node->len_filename +1 );

  if ( temp_node->filename == NULL )
  {
    /* Communicate the failure */
    ssm_parser_read_remote_perm_file_helper( src_proc, 
		                      SSM_MTOP_PERM_FILE, 
                                      E_SSM_DATA_MALLOC_FAILED, 
				      0, /* data_complete */
				      0, /* pkt_num */
				      0, /* len_data */
				      NULL 
				      );
    free ( temp_node );

    return E_SSM_DATA_MALLOC_FAILED;
  }

  /* Copy filename to string */
  strlcpy( temp_node->filename, message, len+1 );

  /* Link temp_node to ssm_parser_remote_read_perm_file_q */
  (void) q_link( temp_node, &temp_node->link );
    
  /* Enqueue node in ssm_parser_remote_read_perm_file_q */
  q_put( &ssm_parser_remote_read_perm_file_q, &temp_node->link);

  /* Process remaining remote read requests */
  ssm_parser_process_remote_read_request();

  return E_SSM_SUCCESS;
  
} /* ssm_parser_ptom_req_perm_file */


/** 

Update remote perm file status for further processing

@param[in] proc                           Proc which requested perm file
@param[in] ipc                            Ipc Message
@param[in] message                        message which contains fielname
@param[in] len                            length of the message

@return
ssm_err_t                                 E_SSM_SUCCESS if successful

@dependencies
None.

*/
  
ssm_err_t ssm_parser_ptom_perm_file_status
(
  ssm_proc_enum_t      proc,
  ssm_err_t            err_code,
  char *               message,
  uint32               len
)
  {
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_MSG_MED( "Function : ssm_process_ptom_perm_file_status " );

  /* Check the error code */
  if ( err_code != E_SSM_SUCCESS )
  {
    /* De-initialize the structure */
    free ( ssm_parser_remote_perm_file_read.filename );
    ssm_parser_remote_perm_file_read.initialized = FALSE;
    ssm_parser_remote_perm_file_read.len_file_read = 0;
    ssm_parser_remote_perm_file_read.bytes_read_last = 0;
    ssm_parser_remote_perm_file_read.num_pkts_sent = 0;
    ssm_parser_remote_perm_file_read.data_complete = FALSE;

    /* Process remaining remote read requests */
    ssm_parser_process_remote_read_request();
  }

  /* Check if some read remote perm file is in process */
  if ( ssm_parser_remote_perm_file_read.initialized == TRUE )
  {
    /* Check if data read is complete */
    if ( ssm_parser_remote_perm_file_read.data_complete == TRUE )
    {
      /* De-initialize the structure */
      free ( ssm_parser_remote_perm_file_read.filename );
      ssm_parser_remote_perm_file_read.initialized = FALSE;
      ssm_parser_remote_perm_file_read.len_file_read = 0;
      ssm_parser_remote_perm_file_read.bytes_read_last = 0;
      ssm_parser_remote_perm_file_read.num_pkts_sent = 0;
      ssm_parser_remote_perm_file_read.data_complete = FALSE;

      /* Process remaining remote read requests */
      ssm_parser_process_remote_read_request();
   }
   else
   {
     /* Read file and send the encrypted data to peripheral processor */
     ssm_parser_read_remote_perm_file( err_code, proc, message, len );
   }
  }

  return E_SSM_SUCCESS;

} /* ssm_parser_ptom_perm_file_status */

#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */


#ifndef FEATURE_SSM_PERIPHERAL_SERVER_PROC

/**
  
The function makes necessary change to data_struture and invokes clients in queue for
further processing .

@param[in] 
None

@return
None

@dependencies
None.
*/

static void ssm_parser_invoke_remaining_client_processing( void )
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Reset ssm_parser_clnt_process to SSM_MAX_CLIENTS */
  ssm_parser_clnt_process = SSM_MAX_CLIENTS;
  
  /* Signal SSM task to start parsing perm file */
  #ifdef FEATURE_SSM_REX_API_PRESENT
    rex_set_sigs(ssm_tcb_ptr, SSM_PARSE_PERM_FILE_SIG );
  #else /* FEATURE_SSM_REX_API_PRESENT */
    qurt_anysignal_set( &ssm_sig_qurt, SSM_PARSE_PERM_FILE_SIG );
  #endif /* FEATURE_SSM_REX_API_PRESENT */

} /* ssm_parser_invoke_remaining_client_processing */


/** 
  
The function take care of mulitple errors and accordingly initializes and de initializes
permission data

@param[in] err                     Used to inform SSm parser of any error in IPC.
@param[in] data_complete           Indicates if data transfer is complete
@param[in] id                      Id of the client in process
@param[id] send_data               boolean value notifying if the modem needs to 
                                   be communicated
@return
void

@dependencies
None.
*/


static void ssm_parser_update_remote_perm_file_helper 
(
  ssm_err_t err,
  boolean   data_complete,
  uint32    id,
  boolean   send_data
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Communicate the failure */
  ssm_ipc_encode_send( SSM_PROC_MODEM, SSM_PTOM_PERM_FILE_STATUS, 
                       err, 0, NULL );
    
  if ( err == E_SSM_SUCCESS )
  {
    if ( data_complete == TRUE )
    {
      
      ssm_parser_clients[id].status= SSM_CLIENT_STATUS_INITIALIZED;
      
      /* Callback updates status of the initialization to the client */
      if ( ssm_parser_clients[id].ssm_cb )
        ssm_parser_clients[id].ssm_cb ( E_SSM_SUCCESS );

      ssm_parser_invoke_remaining_client_processing();
    
    }
  }
  else 
  {
    
    /* De-initialize */
    if ( ssm_parser_clients[ssm_parser_clnt_process].ssm_data_ptr )
      free ( ssm_parser_clients[ssm_parser_clnt_process].ssm_data_ptr );

    ssm_parser_clients[ssm_parser_clnt_process].ssm_data_ptr = NULL;
    
    ssm_parser_clients[id].status= SSM_CLIENT_STATUS_INITIALIZATION_FAILED;
    
    /* Callback updates status of the initialization to the client */
    if ( ssm_parser_clients[id].ssm_cb && 
	 ssm_parser_clients[id].status_fb == SSM_CLIENT_STATUS_INITIALIZED )
      ssm_parser_clients[id].ssm_cb ( E_SSM_SUCCESS );
    else if ( ssm_parser_clients[id].ssm_cb )
      ssm_parser_clients[id].ssm_cb ( err );
        
    ssm_parser_invoke_remaining_client_processing();

  }

} /* ssm_parser_update_remote_perm_file_helper */

/** 
  
The function is invoked to update perm file for a peripheral processor
when modem processor sends permission data securely

@param[in] err                     Used to inform Ssm parser of any error in IPC.
@param[in] data_complete           Indicates if data transfer is complete
@param[in] pkt_num                 Packet number sent from 
                                   peripheral processor
@param[in] perm_data               Permission data 
@param[in] len                     Length of permission data
@param[id] send_data               boolean value notifying if the modem needs to 
                                   be communicated

@return
ssm_err_t                          Returns E_SSM_SUCCESS on success

@dependencies
None.
*/


static ssm_err_t ssm_parser_update_remote_perm_file 
(
  ssm_err_t err,
  boolean   data_complete,
  uint32    pkt_num,
  char *    perm_data,
  uint32    len,
  boolean   send_data
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_MSG_MED_3(" ssm_parser_update_remote_perm_file err =%d data_complete =%d pkt_num =%d "
		  ,err, data_complete, pkt_num );
 
  SSM_MSG_MED_2(" ssm_parser_update_remote_perm_file ssm_parser_clnt_process =%d send_data =%d "
		  , ssm_parser_clnt_process , send_data);

  if ( E_SSM_SUCCESS != err )
  {
    ssm_parser_update_remote_perm_file_helper( err, data_complete, 
		                               ssm_parser_clnt_process, send_data );

    return err;
  }
  
  if ( pkt_num != ssm_parser_clients[ssm_parser_clnt_process].last_pkt_num + 1)
  {
    ssm_parser_update_remote_perm_file_helper( E_SSM_INVALID_DATA, data_complete, 
		                               ssm_parser_clnt_process, send_data );
    return E_SSM_INVALID_DATA;
  }

  err = ssm_parser_process_input_buffer_append( ssm_parser_clnt_process, 
		     len, perm_data );

  if ( err == E_SSM_SUCCESS )
  {
    ssm_parser_clients[ssm_parser_clnt_process].last_pkt_num++;
  }

  ssm_parser_update_remote_perm_file_helper( err, data_complete, 
		                               ssm_parser_clnt_process, send_data );

  return err;

} /* ssm_parser_update_remote_perm_file */

/** 
Process Modem to Peripheral perm file data communcation

@param[in] src_proc                       IPC origin proc
@param[in] err_code                       Error code sent with IPC
@param[in] message_len                    Length of decoded message
@param[in] message                        Decoded message which is further processed

@return
ssm_err_t                        Returns error code

@dependencies
None.

*/

ssm_err_t ssm_parser_mtop_perm_file
(
  ssm_proc_enum_t       src_proc,
  ssm_err_t             err_code,
  char *                message,
  uint32                message_len    
)
{
  /* message is expected to be in the format of 
   * data complete|Packet_number|Length of data|data */

  uint32 len_read       = 0;
  boolean data_complete = FALSE;
  uint32 pkt_num        = 0;
  uint32 len_data       = 0;
  char * data;

  /* Decode info to help break ipc */
  ssm_types_decode_info decode_info[4];
	
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  SSM_MSG_MED_1( " ssm_process_mtop_perm_file  message length = %d ", 
		  message_len );

  /* Check if SSM source proc is Modem */
  if ( src_proc != SSM_PROC_MODEM )
  {
    /* Update SSM parser about failure */
    ssm_parser_update_remote_perm_file( E_SSM_FAIL, FALSE, 0, 0, 0, TRUE );

    /* Return IPC failed */
    return E_SSM_FAIL;
  }

  /* Check if Error code is as expected */
  if ( err_code != E_SSM_SUCCESS )
  {
    /* Update SSM parser about failure */
    ssm_parser_update_remote_perm_file( E_SSM_FAIL, FALSE, 0, 0, 0, FALSE );

    /* Return IPC failed */
    return E_SSM_FAIL;
  }
 
  if ( message == NULL || message_len == 0 )
  {
    /* Update SSM parser about failure */
    ssm_parser_update_remote_perm_file( E_SSM_MSG_DECODE_ERR, 
		                        FALSE, 0, 0, 0, TRUE );

    /* Return Mode Not updated succesful */
    return E_SSM_MSG_DECODE_ERR;
  }

  /* Populate Decode info */
  decode_info[0].type = SSM_TYPE_BOOLEAN ; /* for data_complete */
  decode_info[1].type = SSM_TYPE_UINT32 ; /* for pkt_num */
  decode_info[2].type = SSM_TYPE_UINT32 ; /* for len_data */
  decode_info[3].type = SSM_TYPE_STRING ; /* for data */  

  if ( E_SSM_SUCCESS != 
		  ssm_ipc_decode_break_ipc( decode_info, 4, message, 
			                message_len, 
			                &len_read ) )  
  {
    /* Update SSM parser about failure */
    ssm_parser_update_remote_perm_file( E_SSM_MSG_DECODE_ERR, 
		                        FALSE, 0, 0, 0, TRUE );

    return E_SSM_MSG_DECODE_ERR;
  }
  
  data_complete = (boolean)decode_info[0].val.boolean_val;
  pkt_num       =          decode_info[1].val.uint32_val;
  len_data      =          decode_info[2].val.uint32_val;
  data          =          decode_info[3].val.string_val;
  
  /* Update SSM perm file with Success and data */
  return ssm_parser_update_remote_perm_file( err_code, data_complete, pkt_num, 
		        data , len_data, TRUE );
  
} /* ssm_parser_mtop_perm_file */

#endif /* !FEATURE_SSM_PERIPHERAL_SERVER_PROC */

