/*=============================================================================

                   T I M E   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements functionality to save offsets into efs item file.

EXTERNALIZED FUNCTIONS
  time_efs_rw 
    Local function that provides IO access for generic offsets in EFS
  
INITIALIZATION AND SEQUENCING REQUIREMENTS
    None

Copyright (c) 2009 - 2015 by QUALCOMM Technologies Incorporated.  All Rights Reserved.


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/time/src/time_efs.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/15   abh     Changes for correctly handling ATS_PRIVATE bases.
08/06/15   abh     Changes to  handle file read write based on queue operations.
11/23/10   din     Changed to handle allocation failure in malloc.
04/27/10   din     Fixed compiler warnings.
04/19/10   din     Prevented Multiple efs_writes for same file. Reinsertion of 
                   same element was prevented.
03/11/10   din     Moved efs write to Time IPC task.
03/10/10   din     Removed efs_unlink() as efs_put() overwites the item file.
17/10/09   din     File created. 

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "rcinit.h"
#include "time_efs.h"
#include "time_genoff_v.h"
#include "time_ipc.h"
#include "queue.h"
#include "ats_v.h"
#include "fs_public.h"
#include "time_fn_map.h"
#include <string.h>
#include <stringl/stringl.h>
#include <stdio.h>
#include "rcinit_rex.h"

/*=============================================================================

                         DATA DEFINITIONS
=============================================================================*/

/* Size to be written, 64 bits for all offsets */
#define ITEM_SIZE                        8 

/* Queue to store base to be remoted through TIME IPC TASK */
static q_type                            efs_rw_q;
static q_type                            efs_rw_free_q;

static time_fn_map_fp_type * time_fn_map_efs_fp;

typedef enum queue_enum
{
NOT_IN_QUEUE = 0,
IN_QUEUE
} queue_q;

typedef struct efs_rw_struct_type
{
  /* Link for the next queue */
  q_link_type                            link;

  /* Name of the file to be written "absolute path" */
  char                                   file_name[TIME_GENOFF_MAX_FILE_NAME_LENGTH];

  /* Pointer to data to be read/written */
  int64                                   offset;

} efs_rw_struct;

/* Number of elements in the efs read write queue */
#define MAX_EFS_RW_ARR_SIZE             (ATS_MAX + ATS_PRIVATE_MAX)
static efs_rw_struct                     efs_rw [MAX_EFS_RW_ARR_SIZE];

static boolean time_efs_init_flag = FALSE;

static rex_tcb_type *time_ipc_tcb_ptr = NULL;


/*=============================================================================

                         FUNCTION DEFINITIONS
=============================================================================*/

/*=============================================================================

FUNCTION TIME_EFS_INIT

DESCRIPTION
  Initializes the efs read/write queues.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_efs_init ( void )
{
  /* Array index iterator */
  uint32          index;

  /* Queue link pointer */
  efs_rw_struct * efs_rw_ptr ;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize the queue for time EFS read/write */
  (void) q_init( &efs_rw_q);

  /* Initialize the time EFS free queue */
  (void) q_init( &efs_rw_free_q);

  /* Add the EFS read/write queuelinks to this free queue */
  for ( index=0; index < MAX_EFS_RW_ARR_SIZE; index++ )
  {
    efs_rw_ptr = (efs_rw_struct *) &efs_rw[index];
    (void) q_link( efs_rw_ptr, &efs_rw_ptr->link );
    q_put( &efs_rw_free_q, &efs_rw_ptr->link);
  }
    
  /* Initialize the time efs read/write function pointer */
  time_fn_map_efs_fp = time_fn_map_get_fp();

  /* Mark flag for indicating the queue init operation is completed */
  time_efs_init_flag = TRUE;  

} /* time_efs_init */



/*=============================================================================

FUNCTION TIME_EFS_RW

DESCRIPTION
  Local function that provides IO access for generic offsets in EFS

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  For writes: Expect a overwrite of the entire value of what ever size

=============================================================================*/
time_efs_rw_stat_enum_type time_efs_rw 
(
  /* Name of the file to be written "absolute path" */
  const char                * file_name,

  /* Pointer to data to be read/written */
  void                      * data_ptr,

  /* Read/write command */
  time_efs_rw_cmd_enum_type rd_wr
)
{
  /* Efs_* return size variable */	
  int return_size;

  /* file name with absolute address */
  char f_name[64]={0};

  /* Initialize the return status with unexpected error */
  time_efs_rw_stat_enum_type status = TIME_EFS_RW_STAT_IO_ERR;

  /* Data to be enqeued in queue */
  efs_rw_struct * efs_rw_ptr ;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Return if the time efs is not yet initialized */
  if ( time_efs_init_flag != TRUE )
  {
    ATS_ERR("time_efs_rw: Time efs not initialized yet");
    return TIME_EFS_RW_STAT_IO_ERR;
  }

  /* Incase of write */
  if(rd_wr == TIME_EFS_RW_CMD_WR)
  {
    /* Get one queue element */
	efs_rw_ptr = ( efs_rw_struct * ) q_get( &efs_rw_free_q );

	/* Crash if the efs rw free queue is exhausted */
	if (efs_rw_ptr == NULL)
    {
	  ERR_FATAL("time_efs_rw: efs_rw_free_q=0x%x exhausted, queue_size=%d",
	  	         &efs_rw_free_q,MAX_EFS_RW_ARR_SIZE,0);
	}
       
       /* Copy data_ptr into allocated memory */
       efs_rw_ptr->offset = (*(int64*)data_ptr);

    /* Copy the filename */
	snprintf( efs_rw_ptr->file_name , TIME_GENOFF_MAX_FILE_NAME_LENGTH, 
		   "%s", file_name );

       /* Link efs_rw_ptr to efs_rw_q */
       (void) q_link( efs_rw_ptr, &efs_rw_ptr->link );
    
       /* Enqueue node in efs_rw_q */
       q_put( &efs_rw_q, &efs_rw_ptr->link);

       /* Set signal to TIME_IPC task for further processing */
       #ifdef FEATURE_TIME_IPC_TASK
       if (time_ipc_tcb_ptr != NULL)
       {
         (void) rex_set_sigs( time_ipc_tcb_ptr, TIME_IPC_EFS_WRITE );
       }
       else
       {
         time_ipc_tcb_ptr = rcinit_lookup_rextask("time_ipc");
         (void) rex_set_sigs( time_ipc_tcb_ptr, TIME_IPC_EFS_WRITE );
       }
       #endif /*FEATURE_TIME_IPC_TASK*/

    status = TIME_EFS_RW_STAT_OK;	  
  }

  /* Incase of read */
  else if(rd_wr == TIME_EFS_RW_CMD_RD)
  {
    /* Generate the absolute path for the file */
    (void) snprintf( f_name, sizeof(f_name), 
		   "/nvm/alpha/item_file/time/%s", file_name );
  
    /* Obtain the contents from EFS */
    if(time_fn_map_efs_fp && 
	   time_fn_map_efs_fp->efs_get_fp)
    {
      return_size = time_fn_map_efs_fp->efs_get_fp(f_name, data_ptr, ITEM_SIZE);
    }
    else
    {
      return_size = -1;
    }
      
    if (return_size == -1)
    {
      /* EFS read err */
      status = TIME_EFS_RW_STAT_RD_ERR;
    }
    else
    {
      status = TIME_EFS_RW_STAT_OK;
    }
  }

  return status;

} /* time_efs_rw */



/*=============================================================================

FUNCTION TIME_EFS_WRITE

DESCRIPTION
  Called from Time IPC task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  For writes: Expect a overwrite of the entire value of what ever size

=============================================================================*/

void time_efs_write()
{
   /* Element to be dequeued from queue */
   efs_rw_struct                  *efs_rw_ptr;

   /* Efs_* return size variable */	
   int                            return_size;

   /* Absolute path of efs_item file */
   char                           file_name[64];
   
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   /* Get pointer to queue node */
   efs_rw_ptr = ( efs_rw_struct * ) q_get( &efs_rw_q );

   while ( efs_rw_ptr != NULL )
   {

    /* Attach absolute path to filename */
    snprintf( file_name , sizeof(file_name), 
		   "/nvm/alpha/item_file/time/%s", efs_rw_ptr->file_name );

    /* Write into efs_item file */
    if(time_fn_map_efs_fp && 
	   time_fn_map_efs_fp->efs_put_fp)		
    {
      return_size = time_fn_map_efs_fp->efs_put_fp( file_name,
                                                    (void *)&efs_rw_ptr->offset,
                                                    ITEM_SIZE,
                                                    O_CREAT|O_AUTODIR,
                                                    0777 );
    }
    else
    {
      return_size = -1;
    }
        
    if (return_size == -1)
    {
      /* EFS write err, Time base restoration will fail in next power cycle */
      ATS_MSG_SPRINTF_1(" Time_Debug: EFS write to file %s failed ", &file_name[0]);
    }

	/* Return this node to the free queue for efs read/write */
    (void) q_link( efs_rw_ptr, &efs_rw_ptr->link );
    q_put( &efs_rw_free_q, &efs_rw_ptr->link);

	/* Get the next node pointer from the efs_rw_q */	
    efs_rw_ptr = ( efs_rw_struct * ) q_get( &efs_rw_q );
    
   } /* while ( efs_rw_ptr != NULL ) */

} /* time_efs_write */


/*=============================================================================

FUNCTION TIME_EFS_DEL_FILE

DESCRIPTION
  Called to delete EFS file node or a directory

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Post deletion, reading the efs file item will return error

=============================================================================*/

time_efs_rw_stat_enum_type time_efs_del_file
(
  /* Name of the file or directory to be deleted */
  const char                   * file_name
)
{
   /* Efs_* return size variable */	
   int                            ret_val;

   /* Absolute path of efs_item file */
   char                           f_name[64];

   /* Initialize the return status with unexpected error */
   time_efs_rw_stat_enum_type status = TIME_EFS_RW_STAT_IO_ERR;   
   
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   /* Return if the time efs is not yet initialized.
       * We shouldn't even be here if it is not initialied.
       */
   if ( time_efs_init_flag != TRUE )
   {
     ATS_ERR("time_efs_del_file: Time efs not initialized yet");
     return TIME_EFS_RW_STAT_IO_ERR;
   }

    /* Attach absolute path to filename */
    snprintf( f_name , sizeof(file_name), 
		   "/nvm/alpha/item_file/time/%s", file_name );   

   /* Delete the file / directory passed */
   if(time_fn_map_efs_fp && 
   	  time_fn_map_efs_fp->efs_deltree_fp)
   {
	 ret_val = time_fn_map_efs_fp->efs_deltree_fp(f_name);
   }
   else
   {
	 ret_val = -1;
   }

   if (ret_val == -1)
   {
	 /* EFS deltree err */
	 ATS_MSG_SPRINTF_1(" Time_Debug: EFS Delete for file:%s failed ", &file_name[0]);
   }
   else
   {
     status = TIME_EFS_RW_STAT_OK;
   }

   return status;

} /* time_efs_del_file */



