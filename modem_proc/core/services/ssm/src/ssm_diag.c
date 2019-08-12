/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              SSM Diagnostics Packet Processing
             S S M  D I A G   F R A M E W O R K 

General Description
  Diagnostic packet processing routines for SSM operations.

Copyright (c) 2011 - 2014 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_diag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/15/14   din     Added diag test command to test secure channel.
04/03/14   din     Added new apis to get mode and to write to permission file. 
                   Also added diag command to get ssm mode.
04/02/14   psu     Update diag macros using optimized versions.
04/29/13   din     Changes pertaining to ADSP were made.
04/02/13   din     Replaced memcpy with memscpy.
01/15/13   din     Possible buffer overflow issue fixed seen due to usage of strcmp.
11/19/12   din     Resolved klocwork errors.
08/24/12   din     Reinserted EFS based perm file fucntionality within a Feature.
07/19/12   din     Changes done to switch from efs to sfs based permission file.
09/03/11   din     File created.

===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "memheap.h"
#include "msg.h"
#include "ssm_diag_v.h"
#include "ssm.h"
#include "ssm_v.h"
#include "ssm_smdl_v.h"
#include <stringl/stringl.h>
#include <stdio.h>
#include <stdlib.h>
#include "secapi.h"

#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
#include "fs_public.h"
#ifndef FEATURE_SSM_USE_EFS
#include "sfs_api.h"
#endif /* FEATURE_SSM_USE_EFS */
#include "fs_errno.h"
#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

/****************************************************************************\
Example for Command compilation :-
===============================

 - Below is the diag packet req/resp structure for reference :-
   ---------------------------------------------------------
typedef struct
{
  diagpkt_subsys_header_type header;        - Sub System header
  uint16 command;                           - Which command
  uint16 command_data[DIAG_TIME_DATA_SIZE]; - Command parameters
                                              -to be used by function
  uint16 return_stat;                       - Return Status

}SSM_DIAG_F_(req_type/resp_type);
  ----------------------------------------------------------

 - Sample Command building:-
   -----------------------
diagpkt_subsys_header_type header (4 bytes)
------------------------------------------------
Command Class is default   - 1 byte  : 0x4B
Subsystem id for Time_Diag - 1 byte  : 0x49(73 for Time Services)
??                         - 2 bytes : 0x00 0x00
------------------------------------------------

uint16 command (2 bytes)
-----------------------------------------------
Command ID                 - 2 bytes: 0x00 0x00 (Eg for TIME_SET)
-----------------------------------------------

\****************************************************************************/

/*=============================================================================

                         DATA DEFINITIONS
=============================================================================*/

#define SSM_GET_PERM             0
#define SSM_MODE_CHANGE          1
#define SSM_MISC                 2
#define SSM_SMDL_WRITE           3

#ifdef FEATURE_SSM_TEST_DEBUG
#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC

/* The size of the largest possible request packet.  */
#ifdef  DIAG_MAX_RX_PKT_SIZ
  #define LARGEST_DIAG_PACKET     DIAG_MAX_RX_PKT_SIZ
#else
  #define LARGEST_DIAG_PACKET     0x800
#endif

#ifdef DIAG_MAX_TX_PKT_SIZ
  #define LARGEST_DIAG_RESPONSE   DIAG_MAX_TX_PKT_SIZ
#else
  #define LARGEST_DIAG_RESPONSE   0x800
#endif

static char ssm_last_packet[LARGEST_DIAG_PACKET];
static unsigned ssm_last_packet_len;
static char ssm_last_response[LARGEST_DIAG_RESPONSE];
static unsigned ssm_last_response_len;

#define SSM_DIAG_MAX_FDS        10
#define SSM_MAX_FILE_NAME_SIZE  120
/* Maximum size of a read request. */
#define SSM_DIAG_MAX_READ_REQ    1024

typedef enum {
  ERR_SUCCESS =0,
  ERR_MAX_FDS_REACHED,
  ERR_INCORRECT_FD,
  ERR_FILE_OPEN,
  ERR_FILE_CLOSE,
  ERR_FILE_READ,
  ERR_FILE_WRITE,
  ERR_FILE_SEEK,
  ERR_DIRECTORY_OPEN
} ssm_diag_err_type;	   

/* The descriptor used external to diag is merely an index into this array,
 * not a real SFS descriptor.  We only allow access by diag to files that
 * were opened through diag.  For each opened descriptor, there is a single
 * element here for this.  There will always be an SFS descriptor
 * associated with each open file, and that will sometimes be associated
 * with a buffered SFS_FILE pointer.  A -1 for the fd field indicates that
 * this entry is unused. */
typedef struct {
  int       fd;
  char  file[SSM_MAX_FILE_NAME_SIZE];
} desc_entry;
static desc_entry ssm_desc_table[SSM_DIAG_MAX_FDS];

/* Memoize this packet and response. */
static void ssm_memoize_packet
(PACKED const void * PACKED_POST request, unsigned req_len,
    PACKED const void *  PACKED_POST response, unsigned rsp_len);

/* Invalidate the last memoized packet.  Used for requests that are
 * idempotent to prevent memoization. */
static void ssm_invalidate_memoization (void);

/* Check if this request is the same as a previous one.  Will allocate a
 * response and return a copy of that last response.  Note that this macro
 * has a 'return' in it that will return a result if no work should be
 * done. */
#define SSM_MEMOIZED_PACKET_CHECK(pkt, len, code) \
  do { \
    if ((len) == ssm_last_packet_len && \
        memcmp ((void *)(pkt), ssm_last_packet, ssm_last_packet_len) == 0) \
    { \
      PACKED void *  PACKED_POST __rsp = diagpkt_subsys_alloc (DIAG_SUBSYS_SSM, \
          (code), ssm_last_response_len); \
      if (__rsp != NULL) \
        memscpy ((void *) __rsp, ssm_last_response_len, ssm_last_response, \
			 ssm_last_response_len); \
      return __rsp; \
    } \
  } while (0)

static void
ssm_memoize_packet (PACKED const void * PACKED_POST request, unsigned req_len,
    PACKED const void * PACKED_POST response, unsigned rsp_len)
{
  /* If we somehow get too large of a packet or response, don't remember
   * it. */
  if (req_len > LARGEST_DIAG_PACKET ||
      rsp_len > LARGEST_DIAG_RESPONSE)
  {
    ssm_invalidate_memoization ();
  } 
  else 
  {
    memscpy (ssm_last_packet, LARGEST_DIAG_PACKET, (void *) request, req_len);
    ssm_last_packet_len = req_len;

    memscpy (ssm_last_response, LARGEST_DIAG_RESPONSE, (void *) response, rsp_len);
    ssm_last_response_len = rsp_len;
  }
}

/* Invalidate the last memoized packet.  Used for requests that are
 * idempotent to prevent memoization. */
static void
ssm_invalidate_memoization (void)
{
  ssm_last_packet_len = 0;
  ssm_last_response_len = 0;
}



/**********************************************************************
 * Packet length validation.
 *********************************************************************/

static int ssm_diag_is_valid_pkt_len 
(
  uint32 pkt_len, 
  uint32 min_pkt_len
)
{
  /* Validate pkt_len range */
  if (pkt_len > LARGEST_DIAG_PACKET || pkt_len < min_pkt_len)
  {
    return 0;
  }
  return 1;
}

#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

static uint32 perm;
static ssm_err_t ssm_diag_client_err;

static void ssm_diag_ssm_cb( ssm_err_t ssm_err)
{
  ssm_diag_client_err = ssm_err;
}

/*===========================================================================

FUNCTION SSM_DIAG

DESCRIPTION
 This function would test the Time services.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None


===========================================================================*/

static uint32 ssm_client_id=0;

PACK(void *) ssm_diag 
(
  PACK(void *) req_pkt,
  uint16 pkt_len
)
{
  
  SSM_DIAG_F_req_type *req =(SSM_DIAG_F_req_type *) req_pkt;
 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* SSM_DIAG_F_rsp_type *rsp; */
  const int rsp_len = sizeof( SSM_DIAG_F_rsp_type );

  PACK(void *) rsp_ptr = NULL;

  /* Allocate the same length as the request. */
  rsp_ptr = (SSM_DIAG_F_rsp_type *)diagpkt_subsys_alloc
                                         ( DIAG_SUBSYS_SSM,
                                           SSM_DIAG_F,
                                           rsp_len );

  if (rsp_ptr != NULL)
  {
    memscpy ((void *) rsp_ptr, rsp_len, (void *) req_pkt, pkt_len);
  }

  
  switch ( req->command )
  {

     case SSM_GET_PERM :
       
       if ( ssm_diag_client_err == E_SSM_SUCCESS )
         ssm_diag_client_err = ssm_get_perm(&perm, 4,ssm_client_id,1,5);

       SSM_MSG_HIGH_2(" SSM_get_perm =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);

       if ( ssm_diag_client_err == E_SSM_SUCCESS )
         ssm_diag_client_err = ssm_get_perm( &perm, 4, ssm_client_id, 1,4);

       SSM_MSG_HIGH_2(" ssm_get_perm( &perm, 4,ssm_client_id, 1,4) =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);

       if ( ssm_diag_client_err == E_SSM_SUCCESS )
         ssm_diag_client_err = ssm_get_perm( &perm, 4,ssm_client_id, 2,5);

       SSM_MSG_HIGH_2(" ssm_get_perm( &perm, 4,ssm_client_id, 2,5) =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);

       if ( ssm_diag_client_err == E_SSM_SUCCESS )
         ssm_diag_client_err = ssm_get_perm( &perm, 4,ssm_client_id, 1,21);

       SSM_MSG_HIGH_2(" ssm_get_perm( &perm, 4,ssm_client_id, 1,21) =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);

       if ( ssm_diag_client_err == E_SSM_SUCCESS )
         ssm_diag_client_err = ssm_get_perm( &perm, 4,ssm_client_id, 2,24);

       SSM_MSG_HIGH_2(" ssm_get_perm( &perm, 4,ssm_client_id, 2,24) =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);

       if ( ssm_diag_client_err == E_SSM_SUCCESS )
         ssm_diag_client_err = ssm_get_perm( &perm, 4,ssm_client_id, 1,26);

       SSM_MSG_HIGH_2(" ssm_get_perm( &perm, 4,ssm_client_id, 1,26) =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);

       if ( ssm_diag_client_err == E_SSM_SUCCESS )
         ssm_diag_client_err = ssm_get_perm( &perm, 4,ssm_client_id, 3,33);

       SSM_MSG_HIGH_2(" ssm_get_perm( &perm, 4,ssm_client_id, 3,33) =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);

       if ( ssm_diag_client_err == E_SSM_SUCCESS )
         ssm_diag_client_err = ssm_get_perm( &perm, 4,ssm_client_id, 2,38);

       SSM_MSG_HIGH_2(" ssm_get_perm( &perm, 4,ssm_client_id, 2,38) =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);

       if ( ssm_diag_client_err == E_SSM_SUCCESS )
         ssm_diag_client_err = ssm_get_perm( &perm, 4,ssm_client_id, 1,85);

       SSM_MSG_HIGH_2(" ssm_get_perm( &perm, 4,ssm_client_id, 1,85) =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);

       if ( ssm_diag_client_err == E_SSM_SUCCESS )
         ssm_diag_client_err = ssm_get_perm( &perm, 4,ssm_client_id, 1,177);

       SSM_MSG_HIGH_2(" ssm_get_perm( &perm, 4,ssm_client_id, 1,177) =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);
       
       if ( ssm_diag_client_err == E_SSM_SUCCESS )
         ssm_diag_client_err = ssm_get_perm( &perm, 4,ssm_client_id, 2,195);

       SSM_MSG_HIGH_2(" ssm_get_perm( &perm, 4,ssm_client_id, 2,195) =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);
       
       if ( ssm_diag_client_err == E_SSM_SUCCESS )
         ssm_diag_client_err = ssm_get_perm( &perm, 4,ssm_client_id, 2,215);

       SSM_MSG_HIGH_2(" ssm_get_perm( &perm, 4,ssm_client_id, 2,215) =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);
       
       if ( ssm_diag_client_err == E_SSM_SUCCESS )
         ssm_diag_client_err = ssm_get_perm( &perm, 4,ssm_client_id, 1,1192);

       SSM_MSG_HIGH_2(" ssm_get_perm( &perm, 4,ssm_client_id, 1,1192) =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);
       
       if ( ssm_diag_client_err == E_SSM_SUCCESS )
         ssm_diag_client_err = ssm_get_perm( &perm, 4,ssm_client_id, 1,3382);

       SSM_MSG_HIGH_2(" ssm_get_perm( &perm, 4,ssm_client_id, 1,3382) =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);
       
       if ( ssm_diag_client_err == E_SSM_SUCCESS )
	 ssm_diag_client_err = ssm_get_perm( &perm, 4,ssm_client_id, 2,3462);

       SSM_MSG_HIGH_2(" ssm_get_perm( &perm, 4,ssm_client_id, 2,3462) =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);
       
       if ( ssm_diag_client_err == E_SSM_SUCCESS )
         ssm_diag_client_err = ssm_get_perm( &perm, 4,ssm_client_id, 2,4359);

       SSM_MSG_HIGH_2(" ssm_get_perm( &perm, 4,ssm_client_id, 2,4359) =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);
       
       if ( ssm_diag_client_err == E_SSM_SUCCESS )
         ssm_diag_client_err = ssm_get_perm( &perm, 4,ssm_client_id, 2,4207);

       SSM_MSG_HIGH_2(" ssm_get_perm( &perm, 4,ssm_client_id, 2,4207) =%u ssm_diag_client_err = %u" ,
		       perm, ssm_diag_client_err);

       break;

     case SSM_MISC :
       
       /* SSM falbacks to the buffer if the file specified in initialization call is missing */
       ssm_diag_client_err = ssm_client_init_func( "nv", 2, 0, SSM_UINT32, ssm_diag_ssm_cb, &ssm_client_id,
		                                  "0|255|0|65535|0|", 16 );
       
       break;

     case SSM_MODE_CHANGE :
       
       ssm_parser_update_mode(req->command_data[0]);
       
       break;

     case SSM_SMDL_WRITE :
       
       ssm_smdl_handle_tx( SSM_PROC_APPS, "abcdefghij",  10);
       
       break;
    
     default :
     /* for un-implemented and wrong commands */
       SSM_MSG_HIGH_1("ssm_services_test : invalid command - %d",req->command);
       break;
  }

  (void)pkt_len;

  return (rsp_ptr);

} /* ssm_diag */

#endif /* FEATURE_SSM_TEST_DEBUG */

#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC

#ifdef FEATURE_SSM_TEST_DEBUG

/*===========================================================================

FUNCTION SSM_DIAG_OPEN_PERM_FILE_HANDLER

DESCRIPTION
  Handles the SSM_DIAG_OPEN_PERM_FILE packet.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

PACK(void *) ssm_diag_open_perm_file_handler 
( 
  PACK(void *) req_ptr, 
  uint16 pkt_len
)
{
  ssm_diag_open_req_type *request;
  ssm_diag_open_rsp_type *response;
  
  uint16 rsp_len = sizeof (ssm_diag_open_rsp_type);
  
  const char *request_path;
  int32  fd;
  unsigned slot;
  int result;

  char filename[SSM_MAX_FILE_NAME_SIZE];

  /* Check for minimum length of packet */
  if (!ssm_diag_is_valid_pkt_len (pkt_len, sizeof (ssm_diag_open_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  SSM_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, SSM_DIAG_OPEN_PERM_FILE );

  request = (ssm_diag_open_req_type *) req_ptr;
  request_path = (const char *) request->path;


  response = (ssm_diag_open_rsp_type *)
    diagpkt_subsys_alloc (DIAG_SUBSYS_SSM,
                          SSM_DIAG_OPEN_PERM_FILE,
                          rsp_len);
  if (response == NULL)
    return NULL;

  snprintf( filename, sizeof(filename), "/ssm/data_file/%s", request_path);
  
  /* Find a descriptor slot to use. */
  for (slot = 0; slot < SSM_DIAG_MAX_FDS; slot++) 
  {
    if ( (strncmp( filename, ssm_desc_table[slot].file, SSM_MAX_FILE_NAME_SIZE ) == 0 ) 
		            && ssm_desc_table[slot].fd != -1 )
    {
      response->fd =  ssm_desc_table[slot].fd;
      response->diag_errno = ERR_SUCCESS;
      goto open_end;	    
    }
  }

  /* Find a descriptor slot to use. */
  for (slot = 0; slot < SSM_DIAG_MAX_FDS; slot++) 
  {
    if (ssm_desc_table[slot].fd == -1)
      break;
  }

  /* If we are out of diag descriptors, don't try opening, just return the
   * error. */
  if (slot >= SSM_DIAG_MAX_FDS) 
  {
    response->fd = -1;
    response->diag_errno = ERR_MAX_FDS_REACHED;
    goto open_end;
  }

  /* Create SSM directory in the filesystem */
  /* efs_mkdir returns 0 if the directory is not present in the file system and 
     it can create efs directory successfully. It returns -1 if it fails or 
     if the directory is present in the filesystem */
  
  result = efs_mkdir ("/ssm",0755);
  if ( result != 0 )
  {
    SSM_MSG_ERROR ("SSM : SFS Directory creation failed ");  
  }

  #ifndef FEATURE_SSM_USE_EFS
    /* Create a sub-directory within SSM directory in the filesystem */
    result = sfs_mkdir ("/ssm/data_file" );
  #else
  result = efs_mkdir( "/ssm/data_file", 0755);
  #endif
  if ( result != 0 )
    {
      SSM_MSG_ERROR ("SSM : SFS Directory creation failed ");
  }
  
  /* Open the handle. */
  #ifndef FEATURE_SSM_USE_EFS
  fd = sfs_open (filename, request->oflag );
  if (!fd) {
  #else
  fd = efs_open (filename, request->oflag, request->mode);
  if (fd < 0) {
  #endif
    response->fd = -1;
    response->diag_errno = ERR_FILE_OPEN;
    goto open_end;
  }

  /* Update the desc slot. */
  ssm_desc_table[slot].fd = fd;
  strlcpy(ssm_desc_table[slot].file, filename, SSM_MAX_FILE_NAME_SIZE);

  response->fd = slot;
  response->diag_errno = ERR_SUCCESS;

  open_end:
  ssm_memoize_packet (req_ptr, pkt_len, response, rsp_len);
  return response;

} /* ssm_diag_open_perm_file_handler */

/*===========================================================================

FUNCTION SSM_DIAG_CLOSE_PERM_FILE_HANDLER

DESCRIPTION
  Handles the SSM_DIAG_CLOSE_PERM_FILE packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
PACK(void *) ssm_diag_close_perm_file_handler 
(  
   PACK(void *) req_ptr, 
   uint16 pkt_len
)
{
  ssm_diag_close_req_type *request;
  ssm_diag_close_rsp_type *response;
  
  uint16 rsp_len = sizeof (ssm_diag_close_rsp_type);
  
  int result;

  /* Check for exact length of packet */
  if (!ssm_diag_is_valid_pkt_len (pkt_len, sizeof (ssm_diag_close_req_type)))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  SSM_MEMOIZED_PACKET_CHECK (req_ptr, pkt_len, SSM_DIAG_CLOSE_PERM_FILE);

  request = (ssm_diag_close_req_type *) req_ptr;

  response = (ssm_diag_close_rsp_type *)diagpkt_subsys_alloc(DIAG_SUBSYS_SSM,
                                        SSM_DIAG_CLOSE_PERM_FILE,
                                        rsp_len);
  if (response == NULL)
    return NULL;

  response->diag_errno = ERR_SUCCESS;

  /* If this handle is not open, or is out of range, return an error. */
  if (request->fd < 0 || request->fd >= SSM_DIAG_MAX_FDS ||
  #ifndef FEATURE_SSM_USE_EFS
      ssm_desc_table[request->fd].fd == 0)
  #else
  ssm_desc_table[request->fd].fd < 0)
  #endif      
  {
    response->diag_errno = ERR_INCORRECT_FD;
    goto close_done;
  }

  #ifndef FEATURE_SSM_USE_EFS
  result = sfs_close(ssm_desc_table[request->fd].fd);
  #else
  result = efs_close(ssm_desc_table[request->fd].fd);
  #endif

  if (result != 0)
  {
    response->diag_errno = ERR_FILE_CLOSE;
  }
  else
  {
    /* Set Descriptor table */
    ssm_desc_table[request->fd].fd = -1;
  }

  close_done:
  ssm_memoize_packet (req_ptr, pkt_len, response, rsp_len);

  return response;

} /* ssm_diag_close_perm_file_handler */


/*===========================================================================

FUNCTION SSM_DIAG_READ_PERM_FILE_HANDLER

DESCRIPTION
  Handles the SSM_DIAG_READ_PERM_FILE packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

PACK(void *) ssm_diag_read_perm_file_handler 
(
  PACK(void *) req_ptr, 
  uint16 pkt_len
)
{
  ssm_diag_read_req_type *request;
  ssm_diag_read_rsp_type *response;

  desc_entry *slot;
  unsigned base_length = offsetof (ssm_diag_read_rsp_type, data);
  unsigned rsp_len = base_length;
  fs_ssize_t bytes_left;
  fs_ssize_t count;

  ssm_invalidate_memoization ();

  if (!ssm_diag_is_valid_pkt_len (pkt_len, sizeof (ssm_diag_read_req_type)))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (ssm_diag_read_req_type *) req_ptr;

  /* Validate the requested descriptor. */
  if (request->fd < 0 || request->fd >= SSM_DIAG_MAX_FDS ||
      #ifndef FEATURE_SSM_USE_EFS
      ssm_desc_table[request->fd].fd == 0)
      #else
      ssm_desc_table[request->fd].fd < 0)
      #endif  
  {
    response = (ssm_diag_read_rsp_type *)
      diagpkt_subsys_alloc (DIAG_SUBSYS_SSM,
          SSM_DIAG_READ_PERM_FILE, base_length);
    if (response == NULL)
      return NULL;

    response->fd = request->fd;
    response->offset = request->fd;
    response->bytes_read = -1;
    response->diag_errno = ERR_INCORRECT_FD;
    goto read_end;
  }

  slot = &ssm_desc_table[request->fd];

  /* Begin by allocating a full-sized read packet.  We will shorten it if
   * we get an error, or a short read. */
  bytes_left = request->nbyte;
  if (bytes_left > ( SSM_DIAG_MAX_READ_REQ - base_length ) )
    bytes_left = SSM_DIAG_MAX_READ_REQ - base_length ;

  rsp_len += bytes_left;
  response = (ssm_diag_read_rsp_type *)
    diagpkt_subsys_alloc (DIAG_SUBSYS_SSM, SSM_DIAG_READ_PERM_FILE, rsp_len);

  if (response == NULL)
    return NULL;

  response->fd = request->fd;
  response->offset = request->offset;

  /* Seek to the specified location, and attempt the read. */
  #ifndef FEATURE_SSM_USE_EFS
  if ( request->offset != sfs_seek (slot->fd, request->offset, SEEK_SET)) 
  #else
  if ( request->offset != efs_lseek (slot->fd, request->offset, SEEK_SET)) 
  #endif
  {
    response->bytes_read = -1;
    response->diag_errno = ERR_FILE_SEEK;
    diagpkt_shorten (response, base_length);
    goto read_end;
  }

  #ifndef FEATURE_SSM_USE_EFS
  count = sfs_read (slot->fd, (void *) response->data, bytes_left);
  #else
  count = efs_read (slot->fd, (void *) response->data, bytes_left);
  #endif
  
  /* Fixup the response packet, depending on the result. */
  if (count < 0) 
  {
    response->bytes_read = -1;
    response->diag_errno = ERR_FILE_READ;
  } 
  else 
  {
    response->bytes_read = count;
    response->diag_errno = ERR_SUCCESS;
    if (count < bytes_left)
      diagpkt_shorten (response, base_length + count);
  }

  read_end:
  return response;

} /* ssm_diag_read_perm_file_handler */


/*===========================================================================

FUNCTION SSM_DIAG_WRITE_PERM_FILE_HANDLER

DESCRIPTION
  Handles the SSM_DIAG_WRITE_PERM_FILE packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
PACK(void *) ssm_diag_write_perm_file_handler 
(
  PACK(void *) req_ptr, 
  uint16 pkt_len
)
{
  ssm_diag_write_req_type *request;
  ssm_diag_write_rsp_type *response;
  
  uint16 rsp_len = sizeof (ssm_diag_write_rsp_type);
  fs_ssize_t nbyte;
  fs_ssize_t bytes_written = 0;
  fs_ssize_t bytes_left;
  char      *write_locn;
  desc_entry *slot;

  ssm_invalidate_memoization ();
  
  if (pkt_len < sizeof (ssm_diag_write_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  request = (ssm_diag_write_req_type *) req_ptr;

  response = (ssm_diag_write_rsp_type *)
    diagpkt_subsys_alloc (DIAG_SUBSYS_SSM,
                          SSM_DIAG_WRITE_PERM_FILE,
                          rsp_len);

  if (response == NULL)
    return NULL;

  response->fd     = request->fd;
  response->offset = request->offset;

  nbyte = (fs_ssize_t)
    (pkt_len - (uint16) offsetof (ssm_diag_write_req_type, data));

  if ( nbyte > request->nbyte )
    nbyte = request->nbyte;

  if (request->fd < 0 || request->fd >= SSM_DIAG_MAX_FDS ||
      #ifndef FEATURE_SSM_USE_EFS
      ssm_desc_table[request->fd].fd == 0)
      #else
      ssm_desc_table[request->fd].fd < 0)
      #endif 
  {
    response->diag_errno = ERR_INCORRECT_FD;
    response->bytes_written = 0;
    goto write_end;
  }

  slot = &ssm_desc_table[request->fd];

  /* Seek to the specified offset and write the specified number of bytes. */

  #ifndef FEATURE_SSM_USE_EFS
  if (request->offset != sfs_seek (slot->fd, request->offset,SEEK_SET)) 
  #else
  if (request->offset != efs_lseek (slot->fd, request->offset,SEEK_SET)) 
  #endif
  {
    response->bytes_written = -1;
    response->diag_errno = ERR_FILE_SEEK;
    goto write_end;
  }
  
  bytes_left   = nbyte;
  write_locn   = (char *) request->data;

  while (bytes_left > 0) 
  {
    #ifndef FEATURE_SSM_USE_EFS
    bytes_written = sfs_write (slot->fd, (void *) write_locn, bytes_left);
    #else
    bytes_written = efs_write (slot->fd, (void *) write_locn, bytes_left);
    #endif
    
    if (bytes_written <= 0) {
      /* XXX: This is an incorrect response, but I suspect at this point
       * there are external tools that depend on this behavior. */
      response->bytes_written = -1;
      response->diag_errno = ERR_FILE_WRITE;
      goto write_end;
    }

    bytes_left -= bytes_written;
    write_locn += bytes_written;
  }

  /* (bytes_written >= 0) when it gets here */
  response->bytes_written = nbyte;
  response->diag_errno         = ERR_SUCCESS;

  write_end:
  return response;

} /* ssm_diag_write_perm_file_handler */

/*===========================================================================

FUNCTION SSM_DIAG_TEST_SECURE_CHANNEL_HANDLER

DESCRIPTION
  Returns the mode through Diag response packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
PACK(void *) ssm_diag_test_secure_channel_handler 
(
  PACK(void *) req_ptr, 
  uint16 pkt_len
)
{
  ssm_diag_test_secure_channel_rsp_type *response;

  char input_buffer[]="Test buffer";
  char * encyrpted_buffer;
  char *decyrpted_buffer;
  int len_dec_buf= 312, len_enc_buf = 200;;

  uint16 rsp_len = sizeof (ssm_diag_test_secure_channel_rsp_type);
  
  if (pkt_len < sizeof (ssm_diag_test_secure_channel_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (ssm_diag_test_secure_channel_rsp_type *)
   		diagpkt_subsys_alloc ( DIAG_SUBSYS_SSM,
                                       SSM_DIAG_TEST_SECURE_CHANNEL,
                                       rsp_len);

  if (response == NULL)
    return NULL;

  encyrpted_buffer = malloc(200);

  if ( encyrpted_buffer == NULL )
  {
    response->success = 4;
    return response;
  }
  
  decyrpted_buffer = malloc(312);
  if ( decyrpted_buffer == NULL )
  {
    response->success = 4;
    return response;
  }
  
  /* 0 indicates success 
   * 1 indicates encryption failed
   * 2 indicates decryption failed
   * 3 indicates string compare failed
   * 4 malloc failures */
  response->success = 0;

  /* Encrypt temp buffer */
  if ( E_SUCCESS != secapi_secure_message( SC_SSID_MPSS, SC_CID_SSM,
                                          (uint8 * )input_buffer, 
					  sizeof(input_buffer) , 
	                                  (uint8 *)encyrpted_buffer, 
					  &len_enc_buf) )
  {
    SSM_MSG_ERROR ( "Encryption failed : Secapi failed ");
    response->success = 1;
    return response;
  }

  SSM_MSG_HIGH_1 ( " SSM encryption passed, encrypted buf len = %d", len_enc_buf );
  
  /* Currently this fails with Secapi replay check, disable it to proceed */
  if ( E_SUCCESS != 
       secapi_authenticate_decrypt_message( SC_SSID_MPSS, SC_CID_SSM, 
                                (uint8 *)encyrpted_buffer, 
                                len_enc_buf, 
                                (uint8 *)decyrpted_buffer,
				&len_dec_buf ) )
  {
    SSM_MSG_ERROR(" Decode Buffer Decryption failed ");
    response->success = 2;
    return response;
  }
  
  SSM_MSG_HIGH_1 ( " SSM decryption passed, decrypted buf len = %d", len_dec_buf );

  if ( !strncmp(input_buffer, decyrpted_buffer, 12 ) )
  {
    SSM_MSG_ERROR(" Encrypted & Decrypted message dont match ");
    response->success = 3; 
    return response;
  }

   SSM_MSG_HIGH( " Encrypted & Decrypted message matched" );
  
  return response;

} /* ssm_diag_test_secure_channel_handler */

#endif /* FEATURE_SSM_TEST_DEBUG */

#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

/*===========================================================================

FUNCTION SSM_DIAG_GET_MODE_HANDLER

DESCRIPTION
  Returns the mode through Diag response packet.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
PACK(void *) ssm_diag_get_mode_handler 
(
  PACK(void *) req_ptr, 
  uint16 pkt_len
)
{
  ssm_diag_get_mode_rsp_type *response;

  uint16 rsp_len = sizeof (ssm_diag_get_mode_rsp_type);
  
  if (pkt_len < sizeof (ssm_diag_get_mode_req_type))
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);

  response = (ssm_diag_get_mode_rsp_type *)
   		diagpkt_subsys_alloc ( DIAG_SUBSYS_SSM,
                                       SSM_DIAG_GET_MODE,
                                       rsp_len);

  if (response == NULL)
    return NULL;

  response->mode     = ssm_parser_get_mode();

  SSM_MSG_MED_1 ( " SSM Parser mode = %d", response->mode);

  return response;

} /* ssm_diag_get_mode_handler */

/*===========================================================================

              FUNCTION DISPATCH TABLE FOR DIAG COMMANDS

DESCPRITIION  
  SSM_TEST_F needs to be defined to a value in diagcmd.h if
no subsys ID is used to register.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static const diagpkt_user_table_entry_type ssm_diag_tbl[] =
{
#ifdef FEATURE_SSM_TEST_DEBUG
  { SSM_DIAG_F,SSM_DIAG_F,ssm_diag },
#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
  { SSM_DIAG_OPEN_PERM_FILE, SSM_DIAG_OPEN_PERM_FILE,
    ssm_diag_open_perm_file_handler},
  { SSM_DIAG_CLOSE_PERM_FILE, SSM_DIAG_CLOSE_PERM_FILE, 
    ssm_diag_close_perm_file_handler},
  { SSM_DIAG_READ_PERM_FILE, SSM_DIAG_READ_PERM_FILE,
    ssm_diag_read_perm_file_handler},
  { SSM_DIAG_WRITE_PERM_FILE, SSM_DIAG_WRITE_PERM_FILE,
    ssm_diag_write_perm_file_handler},
#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
  { SSM_DIAG_TEST_SECURE_CHANNEL,SSM_DIAG_TEST_SECURE_CHANNEL,
    ssm_diag_test_secure_channel_handler },
#endif /* FEATURE_SSM_TEST_DEBUG */
  { SSM_DIAG_GET_MODE,SSM_DIAG_GET_MODE,ssm_diag_get_mode_handler }
};



/*===========================================================================

FUNCTION SSM_DIAG_INIT

DESCRIPTION
  Registers the diag packet function dispatch table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ssm_diag_init (void)
{
   #ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
   #ifdef FEATURE_SSM_TEST_DEBUG
   int slot =0;
   
   for (slot = 0; slot < SSM_DIAG_MAX_FDS; slot++) 
   {
     ssm_desc_table[slot].fd = -1;
   }

   #endif /* FEATURE_SSM_TEST_DEBUG */
   
    /* Could use the following registration without specifying the processor */
    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_SSM, ssm_diag_tbl);

    #elif FEATURE_SSM_ADSP_PROC

   /* Could use the following registration without specifying the processor */
    DIAGPKT_DISPATCH_TABLE_REGISTER_PROC (DIAG_QDSP6_PROC, DIAG_SUBSYS_SSM, ssm_diag_tbl);
   
   #endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */
    
}

