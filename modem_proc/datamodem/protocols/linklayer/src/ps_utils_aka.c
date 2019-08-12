/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           P S _ U T I L S _ A K A . C

GENERAL DESCRIPTION
  Utility functions used For AKA calculations.
  Most of these functions assume that the caller is in PS task context.

Copyright (c) 2010-2014 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/src/ps_utils_aka.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
12/02/13    fn     Fixed memscpy KW warnings
09/16/13    fn     Added fix to prevent reopening of session for multiple 
                   slots, PS UTILS AKA only supports sessions on SLOT_1
10/23/12    bb     Moved NV item file paths for AKA in SW and AKA AUTH  
06/24/11    scb    Changed CONFIG file name info as part of adding support for
                   EFS NV is_fmc_enabled
03/07/11    op     Compiler warning fix
01/11/11    jee    Handling session re-opening for ISIM
01/11/11    op     Klocwork fix
12/02/10    jee    Fixed a compiler warning & other minor fixes
11/29/10    op     Klocwork fix
10/12/10    jee    Created Initial Version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
//#ifdef FEATURE_DATA_PS
#include "datamodem_variation.h"
#include "ps_utils_aka.h"
#include "ps_utils.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_svc.h"
#include "AKAAlgo.h"
#include "secapi.h"
#include "ps_system_heap.h"
#include "ps_utils.h"
#include "fs_public.h"
#include "IxErrno.h"

#include <stringl/stringl.h>
#include "ds_auth.h"
#include "ps_utils_defs.h"

#ifdef FEATURE_MMGSDI_SESSION_LIB
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#endif /* FEATURE_MMGSDI_SESSION_LIB */
/*===========================================================================
                              DATA DEFINITIONS
===========================================================================*/

/*---------------------------------------------------------------------------
  AKA max instances 
---------------------------------------------------------------------------*/
#define PS_UTILS_AKA_MAX_HANDLES 3
#define PS_UTILS_AKA_INVALID_HANDLE -1

/*---------------------------------------------------------------------------
  Macro to check if the AKA handle is valid
---------------------------------------------------------------------------*/
#define PS_UTILS_AKA_HANDLE_IS_VALID( aka_handle )\
  ( (PS_UTILS_AKA_INVALID_HANDLE != aka_handle) &&    \
    (PS_UTILS_AKA_MAX_HANDLES > aka_handle) &&        \
    (aka_handle_pool[aka_handle].is_assigned == TRUE) \
    ? TRUE : FALSE )

/*---------------------------------------------------------------------------
  Macro to check if the AKA handle is valid
---------------------------------------------------------------------------*/
#define PS_UTILS_AKA_FREE_HANDLE(aka_handle_ptr)\
        (memset (aka_handle_ptr, 0, sizeof (ps_utils_aka_handle_pool_type)))
/*---------------------------------------------------------------------------
  AKA algo type
---------------------------------------------------------------------------*/
#define PS_UTILS_AKA_ALGO_MILENAGE   0x0002

/*---------------------------------------------------------------------------
  Max RAND & AUTN length
---------------------------------------------------------------------------*/
#define PS_UTILS_AKA_MAX_RAND_LEN 16
#define PS_UTILS_AKA_MAX_AUTN_LEN 16

/*---------------------------------------------------------------------------
  OP & K length
---------------------------------------------------------------------------*/
#define PS_UTILS_AKA_OP_LEN 16
#define PS_UTILS_AKA_K_LEN 16

/*-------------------------------------------------------------------------
  Size of AUTN_LEN & RAND_LEN fields
---------------------------------------------------------------------------*/
#define PS_UTILS_AKA_AUTN_SIZE_LEN 1
#define PS_UTILS_AKA_RAND_SIZE_LEN 1

/*---------------------------------------------------------------------------
  Max RES, CK, IK, AUTS & HMAC length
---------------------------------------------------------------------------*/
#define PS_UTILS_AKA_MAX_RES      16
#define PS_UTILS_AKA_CK_LEN       16
#define PS_UTILS_AKA_IK_LEN       16
#define PS_UTILS_AKA_MAX_AUTS     14
#define PS_UTILS_AKA_HMAC_LEN     16

/*---------------------------------------------------------------------------
  USIM auth result data
---------------------------------------------------------------------------*/
#define PS_UTILS_AKA_AUTH_3G_UMTS_SUCCESS 0xDB
#define PS_UTILS_AKA_AUTH_SYNC_FAILURE    0xDC

/*---------------------------------------------------------------------------
  AKA result length - used for AKA in s/w  
---------------------------------------------------------------------------*/
#define PS_UTILS_AKA_RESULT_LEN   60 

/*---------------------------------------------------------------------------
  Number of elements in AKA sequence number array.
---------------------------------------------------------------------------*/
#define PS_UTILS_AKA_SEQ_NUM_ARRAY_ELEMENTS 32

/*---------------------------------------------------------------------------
  Initial sequence number EFS file path
---------------------------------------------------------------------------*/
#define PS_UTILS_AKA_ISN_EFS_FILE_NAME "/datacommon/utils/initial_sequence_number.txt"

#define PS_UTILS_AKA_DIGEST_V2_PASS    "http-digest-akav2-password"

/*===========================================================================
ENUM:      PS_UTILS_AKA_SUB_CMD_ENUM_TYPE

DESCRIPTION:
   Sub command type that needs to be handled in the command handler
=============================================================================*/
typedef enum 
{
  PS_UTILS_AKA_MMGSDI_CLIENT_REG   = 0,
  PS_UTILS_AKA_MMGSDI_CARD_INS_EVT = 1,
  PS_UTILS_AKA_MMGSDI_SESSION_REG  = 2,
  PS_UTILS_AKA_RUN_ALGO_IN_SW      = 3,
  PS_UTILS_AKA_ISIM_AUTH_CB        = 4,
  PS_UTILS_AKA_USIM_AUTH_CB        = 5
} ps_utils_aka_sub_cmd_enum_type;

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*---------------------------------------------------------------------------
  ISIM APP signature
---------------------------------------------------------------------------*/
#define PS_UTILS_AKA_ISIM_APP_SIGN {0xA0, 0x00, 0x00, 0x00, 0x87, 0x10, 0x04}

/*===========================================================================
STRUCT PS_UTILS_AKA_SESSION_INFO_TYPE

DESCRIPTION
 Structure containing the following fields:
  a) session_type   - To indicate the type of session
  b) session_id     - Placeholder for session ID associated with session_type
  c) session_active - Whether session is active or not
  d) session_reopen - Session needs reopening (in case of ISIM)
  e) aid            - App related information
===========================================================================*/
typedef struct 
{
  mmgsdi_session_type_enum_type session_type; 
  mmgsdi_session_id_type        session_id;
  boolean                       session_active;
  boolean                       session_reopen;
  mmgsdi_static_data_type       aid;
} ps_utils_aka_session_info_type;


/*===========================================================================
STRUCT PS_UTILS_AKA_ISIM_AUTH_INFO_TYPE

DESCRIPTION
 Structure containing the following fields:
  a) response_header - MMGSDI response info
  b) res, res_len    - RES value & length
  c) ck, ck_len      - CK value & length
  d) ik, ik_len      - IK value & length
  e) auts, auts_len  - AUTS value & length
===========================================================================*/
typedef struct 
{
  mmgsdi_response_header_type response_header;
  uint8                       res[PS_UTILS_AKA_MAX_RES];
  uint8                       res_len;
  uint8                       ck[PS_UTILS_AKA_CK_LEN];
  uint8                       ck_len;
  uint8                       ik[PS_UTILS_AKA_IK_LEN];
  uint8                       ik_len;
  uint8                       auts[PS_UTILS_AKA_MAX_AUTS];
  uint8                       auts_len;
}ps_utils_aka_isim_auth_info_type;

/*===========================================================================
STRUCT PS_UTILS_AKA_MMGSDI_INFO_TYPE

DESCRIPTION
 Structure containing the following fields:
  a) client_id      - Client ID associated with a particular MMGSDI client
  b) session_info   - Array of mmgsdi_session_info_type structures containing
                      relevant information pertaining to the session
  c) client_ref     - Cookie provided to each client
  d) auth_info      - Auth info returned by ISIM/USIM APP
===========================================================================*/
typedef struct 
{
  mmgsdi_client_id_type             client_id;
  ps_utils_aka_session_info_type    session_info;
  mmgsdi_client_data_type           client_ref;
  union
  {
    ps_utils_aka_isim_auth_info_type  isim;
    mmgsdi_usim_auth_cnf_type         usim;
  }auth_info;
} ps_utils_aka_mmgsdi_info_type;

/*===========================================================================
STRUCT PS_UTILS_AKA_CB_TYPE

DESCRIPTION
  Control block for the AKA instance.
  a) aka_mmgsdi_info - Structure containing the MMGSDI info
===========================================================================*/
typedef struct
{
  ps_utils_aka_mmgsdi_info_type mmgsdi_info;
}ps_utils_aka_cb_type;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================
  Contains the auth result of AKA in s/w
===========================================================================*/
typedef struct
{
  uint8   aka_result[PS_UTILS_AKA_RESULT_LEN];
}ps_utils_aka_client_auth_info_type;

/*===========================================================================
STRUCT PS_UTILS_AKA_HANDLE_POOL_TYPE

DESCRIPTION
  This is to provide a unique handle to the AKA client
  a) is_assigned      - Is the instance assigned
  b) client_cback_ptr - Client callback provided
  c) user_data        - User data of the client
  d) aka_ver          - AKA version
===========================================================================*/
typedef struct
{
  boolean                             is_assigned;
  ps_utils_aka_callback_type          client_cback_ptr;
  void                               *user_data;
  ps_utils_aka_ver_enum_type          aka_ver;
  ps_utils_aka_client_auth_info_type  auth_info;
  ps_utils_aka_sub_cmd_enum_type      sub_cmd;
}ps_utils_aka_handle_pool_type;

/*===========================================================================
STRUCT PS_UTILS_AKA_AUTH_INFO

DESCRIPTION
  To read the OP & K values from the EFS item files
  a) aka_op_val - OP value
  b) aka_k_val  - K value
===========================================================================*/
typedef PACKED struct
{ 
  uint8 aka_op_val[PS_UTILS_AKA_OP_LEN];
  uint8 aka_k_val [PS_UTILS_AKA_K_LEN];
} ps_utils_aka_auth_info;

/*-----------------------------------------------------------------------------
  Structure used to pass the values required to calculate the digest
-----------------------------------------------------------------------------*/
typedef struct
{
  uint8    *res;
  uint8     res_len;
  uint8    *ck;
  uint8     ck_len;
  uint8    *ik;
  uint8     ik_len;
} ps_utils_aka_create_digest_info;

/*-----------------------------------------------------------------------------
  Structure used to parse to store the SEQ no. data from NV
-----------------------------------------------------------------------------*/
typedef struct 
{
  uint64  aka_algo_seqnum_arr[PS_UTILS_AKA_SEQ_NUM_ARRAY_ELEMENTS];
  uint8   aka_algo_seqnum_arr_elements;
} ps_utils_aka_isn_info_type;

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*---------------------------------------------------------------------------
  AKA control block
---------------------------------------------------------------------------*/
static ps_utils_aka_cb_type aka_cb;

/*---------------------------------------------------------------------------
  To determine if ISIM or USIM needs to be opened
---------------------------------------------------------------------------*/
static boolean isim_present;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*---------------------------------------------------------------------------
  AKA handle pool
---------------------------------------------------------------------------*/
static ps_utils_aka_handle_pool_type aka_handle_pool[PS_UTILS_AKA_MAX_HANDLES];

/*---------------------------------------------------------------------------
  AKA sub command type used in task switching
---------------------------------------------------------------------------*/
static ps_utils_aka_sub_cmd_enum_type ps_utils_aka_sub_cmd;

/*===========================================================================
                    LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
#ifdef FEATURE_MMGSDI_SESSION_LIB
static void ps_utils_aka_mmgsdi_session_reg_isim
(
  void
);

static void ps_utils_aka_mmgsdi_session_reg_usim
(
  void
);

static void ps_utils_aka_apps_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
);

void ps_utils_aka_run_isim_algo_cb_task
(
  void *client_data_ptr
);

void ps_utils_aka_run_usim_algo_cb_task
(
  void *client_data_ptr
);

boolean ps_utils_aka_run_algo_on_card
(
  ps_utils_aka_handle_type    aka_handle,
  uint8                      *aka_rand,
  uint8                       rand_len,
  uint8                      *autn,
  uint8                       autn_len
);
#endif /* FEATURE_MMGSDI_SESSION_LIB */

boolean ps_utils_aka_create_digest
(
  ps_utils_aka_ver_enum_type       aka_ver,
  ps_utils_aka_create_digest_info *create_digest_ptr,
  uint8                           *hmac_md5_out_ptr
);

sint15 ps_utils_aka_read_isn_from_efs
(
  ps_utils_aka_isn_info_type *aka_isn_info_ptr
);

boolean ps_utils_aka_create_hmac
(
  secapi_algo_enum_type  algo,
  uint8                 *data,  
  uint16                 data_len,
  uint8                 *key, 
  uint16                 key_len,
  uint8                 *output_data
);

void ps_utils_aka_send_failure_to_client
(
  ps_utils_aka_handle_type    aka_handle
);

/*===========================================================================
FUNCTION PS_UTILS_AKA_CREATE_EFS_FILE

DESCRIPTION
  Create the file in EFS which stores the sequence number

DEPENDENCIES
  None

PARAMETERS
  conf_file_path - File path

RETURN VALUE
  0         - Success
 -1         - Non-EFS related Failures
  efs_errno - EFS related failures. Meaning of this value can be
              found in fs_errno.h

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_utils_aka_create_efs_file
(
  const char *isn_file_path
)
{
  int32                 config_fd;
  int32                 result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( NULL == isn_file_path )
  {
    LOG_MSG_ERROR_0("ps_utils_aka_create_efs_file(): "
                  "isn_file_path is NULL");
    return -1;
  }
  
  /*-------------------------------------------------------------------------
    Create common directories if needed.
  -------------------------------------------------------------------------*/
  LOG_MSG_INFO2_0("ps_utils_aka_create_efs_file(): "
                "EFS: Creating ISN file if necessary");
  result = ps_path_is_directory("/datacommon");
  if( 0 != result )
  {
    /* Directory doesn't exist yet */
    LOG_MSG_INFO1_0("ps_utils_aka_create_efs_file(): "
                  "Create /datacommon dir in EFS");
    result = mcfg_mkdir( "/datacommon", 
                         S_IREAD|S_IWRITE|S_IEXEC, 
                         MCFG_FS_TYPE_EFS);
    if ( -1 == result )
    {
      LOG_MSG_ERROR_1("ps_utils_aka_create_efs_file(): "
                    "Create EFS Dir Failed: error %d", 
                    mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return mcfg_fs_errno(MCFG_FS_TYPE_EFS);
    } 
  }

  result = ps_path_is_directory("/datacommon/utils");
  if( 0 != result )
  {
    /* Directory doesn't exist yet */
    LOG_MSG_INFO1_0("ps_utils_aka_create_efs_file(): "
                  "Create /datacommon/utils dir in EFS");
    result = mcfg_mkdir( "/datacommon/utils", 
                         S_IREAD|S_IWRITE|S_IEXEC, 
                         MCFG_FS_TYPE_EFS);
    if ( -1 == result )
    {
      LOG_MSG_ERROR_1("ps_utils_aka_create_efs_file(): "
                    "Create EFS Dir Failed: error %d", 
                    mcfg_fs_errno(MCFG_FS_TYPE_EFS));
      return mcfg_fs_errno(MCFG_FS_TYPE_EFS);
    }
  }

  /*-------------------------------------------------------------------------
    Open or create the file if does not exist. Resulting file is truncated 
    to zero bytes.
  -------------------------------------------------------------------------*/  
  config_fd = mcfg_fopen (isn_file_path, 
                          O_WRONLY|O_CREAT|O_TRUNC, 
                          ALLPERMS, 
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);
  if ( 0 > config_fd )
  {
    LOG_MSG_ERROR_1("ps_utils_aka_create_efs_file(): "
                  "Error creating config file, error %d", 
                  mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return mcfg_fs_errno(MCFG_FS_TYPE_EFS);
  }

  result = mcfg_fclose (config_fd, MCFG_FS_TYPE_EFS);
  if ( 0 != result )
  {
    LOG_MSG_ERROR_1("ps_utils_aka_create_efs_file(): "
                  "Error closing config file, error %d", 
                  mcfg_fs_errno(MCFG_FS_TYPE_EFS));
    return mcfg_fs_errno(MCFG_FS_TYPE_EFS);
  }
  return 0;

} /* ps_utils_aka_create_efs_file() */

/*===========================================================================
FUNCTION      PS_UTILS_AKA_STORE_ISN()

DESCRIPTION   Utility function to write the SQN no. in the EFS

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ps_utils_aka_store_isn
(
 uint64 *seqnum_arr,
 uint8   seqnum_arr_num_elements
)
{
  int fd; 
  uint8 i;
  char temp_64[17];
  char seperator = ';';
  int result;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_aka_store_isn(): "
                "EFS store sequence number", 0, 0, 0);
  if (NULL == seqnum_arr)
  {
    LOG_MSG_ERROR("ps_utils_aka_store_isn(): "
                  "seqnum_arr is NULL", 0, 0, 0);
    return;
  }

  if(PS_UTILS_AKA_SEQ_NUM_ARRAY_ELEMENTS != seqnum_arr_num_elements)
  {
    LOG_MSG_ERROR("ps_utils_aka_store_isn(): "
                  "Invalid size %d", seqnum_arr_num_elements, 0, 0);
    return;
  }
  (void)mcfg_fs_truncate (PS_UTILS_AKA_ISN_EFS_FILE_NAME, 
                          0, 
                          MCFG_FS_TYPE_EFS,
                          MCFG_FS_SUBID_NONE);
  fd = mcfg_fopen (PS_UTILS_AKA_ISN_EFS_FILE_NAME, 
                   O_WRONLY,0, 
                   MCFG_FS_TYPE_EFS,
                   MCFG_FS_SUBID_NONE);
  
  if(0 > fd) 
  {
    if (ENOENT == mcfg_fs_errno(MCFG_FS_TYPE_EFS)) /* ISN file does not exist */
    {
      LOG_MSG_ERROR("ps_utils_aka_store_isn(): "
                    "EFS: ISN file not present", 0, 0, 0);
      LOG_MSG_INFO1("ps_utils_aka_store_isn(): "
                    "EFS: Creating ISN file", 0, 0, 0);
      result = ps_utils_aka_create_efs_file(PS_UTILS_AKA_ISN_EFS_FILE_NAME);
      if ( 0 != result )
      {
        LOG_MSG_ERROR("ps_utils_aka_store_isn(): "
                      "EFS: Error creating ISN file, error %d",
                      mcfg_fs_errno(MCFG_FS_TYPE_EFS), 0, 0);
        return;
      }
	  
      fd = mcfg_fopen (PS_UTILS_AKA_ISN_EFS_FILE_NAME, 
                       O_RDWR|O_APPEND,0, 
                       MCFG_FS_TYPE_EFS,
                       MCFG_FS_SUBID_NONE);
      if ( 0 > fd )
      {
        LOG_MSG_ERROR("ps_utils_aka_store_isn(): "
                      "EFS: Error opening ISN file, error %d",
                       mcfg_fs_errno(MCFG_FS_TYPE_EFS), 0, 0);
        return;
      }
    }

    else /* Could not open ISN file for some other reason */
    {
      LOG_MSG_ERROR("ps_utils_aka_store_isn(): "
                    "Error opening ISN file, error %d",
                    mcfg_fs_errno(MCFG_FS_TYPE_EFS), 0, 0);
      return;
    }
  }

  /*-----------------------------------------------------------------------
    Store the Sequence no. into EFS 
  -------------------------------------------------------------------------*/
  for(i = 0; i< seqnum_arr_num_elements; i++)
  {
    memset(temp_64,0,sizeof(temp_64));
    /*lint -save -e545 Passing array and &array is the same if the cast is correct */
    (void)snprintf ((char *)&temp_64, 17, "%016llx", seqnum_arr[i]);
    (void)mcfg_fwrite (fd, (char *)&temp_64, 16, MCFG_FS_TYPE_EFS);
    /*lint -restore */
    (void)mcfg_fwrite (fd, (char *)&seperator, 1, MCFG_FS_TYPE_EFS);
  }
  (void) mcfg_fclose (fd, MCFG_FS_TYPE_EFS);

  return;
} /* ps_utils_aka_store_isn() */

/*===========================================================================
FUNCTION PS_UTILS_AKA_RUN_ALGO_IN_SW

DESCRIPTION  Will call execute AKA algorithm on a given rand in software.
             Will retrieve res and kc.

DEPENDENCIES  None.

PARAMETERS  user_info_ptr - User data

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ps_utils_aka_run_algo_in_sw
(
  void *user_info_ptr  
)
{
  uint8   data_len;
  uint8   status;
  uint8   offset = 0;
  boolean ret_val;
  uint8   hmac_md5_output[PS_UTILS_AKA_HMAC_LEN];
  uint8   hmac_md5_len = PS_UTILS_AKA_HMAC_LEN;
  uint8   *aka_result_ptr = NULL;
  uint8   aka_cb_cnt;
  ps_utils_aka_create_digest_info   create_digest;
  ps_utils_aka_handle_type          aka_handle = PS_UTILS_AKA_INVALID_HANDLE;
  ps_utils_aka_handle_pool_type    *aka_handle_pool_ptr = NULL;
  ps_utils_aka_isn_info_type        isn_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_aka_run_algo_in_sw() called", 0, 0, 0);

  /*-----------------------------------------------------------------------
   Sanity Check
  -------------------------------------------------------------------------*/
  if (NULL == user_info_ptr)
  {
    LOG_MSG_ERROR("ps_utils_aka_run_algo_in_sw(): "
                  "user_info_ptr is NULL", 0, 0, 0);
    return;
  }

  aka_handle_pool_ptr = (ps_utils_aka_handle_pool_type *)user_info_ptr;
  /*-----------------------------------------------------------------------
   Retrieve the aka_handle based on the address passed as client_data
  -------------------------------------------------------------------------*/
  for(aka_cb_cnt = 0; aka_cb_cnt < PS_UTILS_AKA_MAX_HANDLES; aka_cb_cnt++)
  {
    if (aka_handle_pool_ptr == &aka_handle_pool[aka_cb_cnt])
    {
      aka_handle = aka_cb_cnt;
    }
  }

  if (TRUE != PS_UTILS_AKA_HANDLE_IS_VALID(aka_handle))
  {
    LOG_MSG_ERROR("ps_utils_aka_run_algo_in_sw(): "
                  "Invalid AKA handle", 0, 0, 0);
    return;
  }

  if (NULL == aka_handle_pool[aka_handle].client_cback_ptr)
  {
    LOG_MSG_ERROR("ps_utils_aka_run_algo_in_sw(): "
                  "Client callback pointer is NULL", 0, 0, 0);
    PS_UTILS_AKA_FREE_HANDLE(&aka_handle_pool[aka_handle]);
    return;

  }

  aka_result_ptr = aka_handle_pool[aka_handle].auth_info.aka_result;
  /*-------------------------------------------------------------------------
    The first byte in the response buffer is the status byte
  -------------------------------------------------------------------------*/
  status = aka_result_ptr[offset];
  offset += sizeof(status);

  switch ( status )
  {
    case AKAALGO_ERR_SQN:
    {
      LOG_MSG_INFO1 ("ps_utils_aka_run_algo_in_sw(): "
                     "AKA Algo CB - Sync failure", 0, 0, 0);
      
      /*-----------------------------------------------------------------------
        aka_data = auts_len + auts
      -----------------------------------------------------------------------*/
      aka_result_ptr[0] = PS_UTILS_AKA_MAX_AUTS;
      aka_handle_pool[aka_handle].client_cback_ptr (aka_handle, 
                 (ds_auth_aka_status_enum_type) PS_UTILS_AKA_SYNC_FAILURE,
                 NULL, 
                 0, 
                 aka_result_ptr,
                 PS_UTILS_AKA_MAX_AUTS + 1,
                 aka_handle_pool[aka_handle].user_data);

      /*-----------------------------------------------------------------------
        Free the aka_handle
      -----------------------------------------------------------------------*/
      PS_UTILS_AKA_FREE_HANDLE (&aka_handle_pool[aka_handle]);
      break;
    }

    case AKAALGO_SUCCESS:
    {
      /*----------------------------------------------------------------------
        The 3G RES, CK & IK length has to be less than or equal to 16 
        according to TS 31.102 7.1.2
      -----------------------------------------------------------------------*/
      create_digest.res_len = aka_result_ptr[offset]; /*res len */
      if (PS_UTILS_AKA_MAX_RES < create_digest.res_len)
      {
        LOG_MSG_ERROR("ps_utils_aka_run_algo_in_sw(): "
                      "Invalid RES_LEN %d", create_digest.res_len , 0, 0);
        ps_utils_aka_send_failure_to_client(aka_handle);
        return;
      }

      create_digest.res = (aka_result_ptr + offset + sizeof(uint8));
      offset += sizeof(uint8) + create_digest.res_len;

      create_digest.ck_len = aka_result_ptr[offset]; /*ck len */
      if (PS_UTILS_AKA_CK_LEN < create_digest.ck_len)
      {
        LOG_MSG_ERROR("ps_utils_aka_run_algo_in_sw(): "
                      "Invalid CK_LEN %d", create_digest.ck_len, 0, 0);
        ps_utils_aka_send_failure_to_client(aka_handle);
        return;
      }

      create_digest.ck = (aka_result_ptr + offset + sizeof(uint8));
      offset += sizeof(uint8) + create_digest.ck_len;

      create_digest.ik_len = aka_result_ptr[offset]; /*ik len */
      if (PS_UTILS_AKA_IK_LEN < create_digest.ik_len)
      {
        LOG_MSG_ERROR("ps_utils_aka_run_algo_in_sw(): "
                      "Invalid IK_LEN %d", create_digest.ik_len, 0, 0);
        ps_utils_aka_send_failure_to_client(aka_handle);
        return;
      }

      create_digest.ik = (aka_result_ptr + offset + sizeof(uint8));
      offset += sizeof(uint8) + create_digest.ik_len;

      /*-----------------------------------------------------------------------
        For AKAv1 just copy RES as the HMAC output
      -----------------------------------------------------------------------*/
      if (PS_UTILS_AKA_V1 == aka_handle_pool_ptr->aka_ver)
      {
        memscpy(hmac_md5_output, sizeof(hmac_md5_output), 
                create_digest.res, create_digest.res_len);
        hmac_md5_len = create_digest.res_len;
      }

      else
      {
        ret_val = ps_utils_aka_create_digest(aka_handle_pool[aka_handle].aka_ver,
                                             &create_digest,
                                             hmac_md5_output);
        if (FALSE == ret_val)
        {
          LOG_MSG_ERROR("ps_utils_aka_run_algo_in_sw(): "
                        "Creating HMAC_MD5 digest failed", 0, 0, 0);
          ps_utils_aka_send_failure_to_client(aka_handle);
          return;
        }
      }

      data_len = offset;
      
      LOG_MSG_INFO1("ps_utils_aka_run_algo_in_sw(): "
                    "AKA s/w Algo CB - Success", 0, 0, 0);
      aka_handle_pool[aka_handle].client_cback_ptr (aka_handle, 
                 (ds_auth_aka_status_enum_type) PS_UTILS_AKA_SUCCESS,
                 hmac_md5_output,
                 hmac_md5_len,
                 aka_result_ptr + sizeof(uint8),
                 data_len, 
                 aka_handle_pool[aka_handle].user_data);

      /*-----------------------------------------------------------------------
        Free the aka_handle
      -----------------------------------------------------------------------*/
      PS_UTILS_AKA_FREE_HANDLE(&aka_handle_pool[aka_handle]);

      /*-----------------------------------------------------------------------
        Retrieve SQN no. & store it in the EFS 
      -----------------------------------------------------------------------*/
      isn_info.aka_algo_seqnum_arr_elements 
                                      = PS_UTILS_AKA_SEQ_NUM_ARRAY_ELEMENTS;
      AKAAlgo_Get_SEQ (isn_info.aka_algo_seqnum_arr,
                       isn_info.aka_algo_seqnum_arr_elements);

      ps_utils_aka_store_isn (isn_info.aka_algo_seqnum_arr,
                              isn_info.aka_algo_seqnum_arr_elements);
      break;
    }

    case AKAALGO_ERR_MACA:
    default:
    {
      LOG_MSG_ERROR ("ps_utils_aka_run_algo_in_sw(): "
                     "Undefined AKA status %d", status, 0, 0);
      ps_utils_aka_send_failure_to_client(aka_handle);
      return;
    }
  }

  return;
}/* ps_utils_aka_run_algo_in_sw() */

/*===========================================================================
FUNCTION PS_UTILS_RUN_AKA_ALGO()

DESCRIPTION  Runs AKAv1 or AKAv2 for the client & returns the result in the  
             callback function provided by the client

DEPENDENCIES None

PARAMETERS  
        aka_ver            - AKA version
        rand, rand_len     - RAND value & length
        autn, autn_len     - AUTN value & length
        run_aka_algo_cback - Client callback that is called when the 
                             results are available
        user_data          - User data, passed to the client callback function

RETURN VALUE  -1     - Failure
              handle - Success (>= 0)

SIDE EFFECTS  None
===========================================================================*/
ps_utils_aka_handle_type ps_utils_run_aka_algo
(
  ps_utils_aka_ver_enum_type  aka_ver,
  uint8                      *aka_rand,
  uint8                       rand_len,
  uint8                      *autn,
  uint8                       autn_len,
  ps_utils_aka_callback_type  run_aka_algo_cback,
  void                       *user_data
)
{
  uint8                    aka_cb_cnt = 0;
  ps_utils_aka_handle_type aka_handle = PS_UTILS_AKA_INVALID_HANDLE;
  boolean                  aka_in_sw = FALSE;
  ps_utils_aka_auth_info   aka_auth_info;
  int                      retval;
  const char* item_file_path1 = {PS_NV_CONF_ITEM_INFO(PS_NV_CONF_AKA_IN_SW)};
  const char* item_file_path2 = {PS_NV_CONF_ITEM_INFO(PS_NV_CONF_AKA_SW_AUTH)};
  ps_utils_aka_isn_info_type aka_isn_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_run_aka_algo() called", 0, 0, 0);

  /*-----------------------------------------------------------------------
   Sanity Check
  -------------------------------------------------------------------------*/
  if ((NULL == aka_rand) || (NULL == autn) || (NULL == run_aka_algo_cback))
  {
    LOG_MSG_ERROR("ps_utils_run_aka_algo(): "
                  "Input parameters are NULL", 0, 0, 0);
    return aka_handle;
  }

  if ((rand_len > PS_UTILS_AKA_MAX_RAND_LEN)
      || (autn_len > PS_UTILS_AKA_MAX_AUTN_LEN))
  {
    LOG_MSG_ERROR("ps_utils_run_aka_algo(): "
                  "Invalid RAND_LEN %d or AUTN_LEN %d", rand_len, autn_len, 0);
    return aka_handle;
  }

  if ((PS_UTILS_AKA_V1 != aka_ver) && (PS_UTILS_AKA_V2 != aka_ver))
  {
    LOG_MSG_ERROR("ps_utils_run_aka_algo(): "
                  "Invalid AKA version %d", aka_ver, 0, 0);
    return aka_handle;
  }

  for(aka_cb_cnt = 0; aka_cb_cnt < PS_UTILS_AKA_MAX_HANDLES; aka_cb_cnt++)
  {
    if(FALSE == aka_handle_pool[aka_cb_cnt].is_assigned)
    {
      aka_handle_pool[aka_cb_cnt].is_assigned = TRUE;
      aka_handle = aka_cb_cnt;
      break;
    }
  }

  if (TRUE != PS_UTILS_AKA_HANDLE_IS_VALID(aka_handle))
  {
    LOG_MSG_ERROR("ps_utils_run_aka_algo(): "
                  "Invalid AKA handle", 0, 0, 0);
    return PS_UTILS_AKA_INVALID_HANDLE;
  }

  /*-------------------------------------------------------------------------
   Store the client data for later use
  -------------------------------------------------------------------------*/
  aka_handle_pool[aka_handle].client_cback_ptr = run_aka_algo_cback;
  aka_handle_pool[aka_handle].user_data        = user_data;
  aka_handle_pool[aka_handle].aka_ver          = aka_ver;

  /*-------------------------------------------------------------------------
    Read EFS item file to determine if AKA has to be done in s/w or on card
  -------------------------------------------------------------------------*/
  retval = ps_read_efs_nv(item_file_path1, &aka_in_sw, sizeof(aka_in_sw));
  if ( 0 != retval )
  {
    LOG_MSG_ERROR("ps_utils_run_aka_algo(): "
                  "AKA in s/w or on card unknown, NV item not set. "
                  "Defaulting to card mode", 0, 0, 0);
  }

  /*-------------------------------------------------------------------------
   Read the NV item to decide if AKA needs to be done in NV mode or on card
   If in NV mode read OP, K & initial Sequence no. values from EFS
  -------------------------------------------------------------------------*/
  if (TRUE == aka_in_sw)
  {
    /*-------------------------------------------------------------------------
      Read OP & K values from the EFS
    -------------------------------------------------------------------------*/
    retval = ps_read_efs_nv(item_file_path2, &aka_auth_info, 
                            sizeof(aka_auth_info));
    if (0 != retval)
    {
      LOG_MSG_ERROR("ps_utils_run_aka_algo(): "
                    "Cannot read OP & K values from EFS", 0, 0, 0);
      PS_UTILS_AKA_FREE_HANDLE(&aka_handle_pool[aka_handle]);
      return PS_UTILS_AKA_INVALID_HANDLE;
    }

    /*-------------------------------------------------------------------------
      Read sequence number from the EFS
      If the Sequence no. length is not correct or if it is not possible to 
      read it then set the sequence no. array to 0 & try AKA with that.
      If AKA succeeds then the new sequence no. will be stored in the EFS file
    -------------------------------------------------------------------------*/
    memset (&aka_isn_info, 0, sizeof(aka_isn_info));
    LOG_MSG_INFO2("ps_utils_run_aka_algo(): "
                  "Reading initial sequence number from EFS", 0, 0, 0);  
    if (ps_utils_aka_read_isn_from_efs(&aka_isn_info) == (sint15)E_SUCCESS)
    {
        if(aka_isn_info.aka_algo_seqnum_arr_elements 
                                  != PS_UTILS_AKA_SEQ_NUM_ARRAY_ELEMENTS)
        {
          LOG_MSG_ERROR("ps_utils_run_aka_algo(): "
                        "Invalid sequence no. array size %d. "
                        "Continuing with SQN no. array set to 0", 
                        aka_isn_info.aka_algo_seqnum_arr_elements, 0, 0);
          aka_isn_info.aka_algo_seqnum_arr_elements 
                                      = PS_UTILS_AKA_SEQ_NUM_ARRAY_ELEMENTS;
        }

        else
        {
          LOG_MSG_INFO2("ps_utils_run_aka_algo(): "
                        "Initial sequence number from EFS success", 0, 0, 0);
        }
    }
    else
    {
      LOG_MSG_ERROR("ps_utils_run_aka_algo(): "
                    "Reading initial sequence number from EFS failed"
                    "Continuing with SQN no. array set to 0", 0, 0, 0);
      aka_isn_info.aka_algo_seqnum_arr_elements 
                                      = PS_UTILS_AKA_SEQ_NUM_ARRAY_ELEMENTS;
    }

    /*-------------------------------------------------------------------------
      Setup the AKA & do the task switch to PS again. This is required
      so that client gets the aka_handle before the client callback is called.
      This is to make the AKA in s/w async similar to AKA on card.
    -------------------------------------------------------------------------*/
    AKAAlgo_Init(PS_UTILS_AKA_ALGO_MILENAGE,
                 aka_auth_info.aka_op_val,
                 PS_UTILS_AKA_OP_LEN);

    AKAAlgo_Set_Subscriber_Key(aka_auth_info.aka_k_val,
                               PS_UTILS_AKA_K_LEN);

    AKAAlgo_Set_SEQ(aka_isn_info.aka_algo_seqnum_arr, 
                    aka_isn_info.aka_algo_seqnum_arr_elements);

    /*-----------------------------------------------------------------------
     Perform AKA is s/w
    -------------------------------------------------------------------------*/
    AKAAlgo_Perform_AKA(autn, 
                        aka_rand, 
                        aka_handle_pool[aka_handle].auth_info.aka_result);

    LOG_MSG_INFO1("ps_utils_run_aka_algo(): "
                  "Switching Task Ctx to make the s/w call async", 0, 0, 0);
    
    aka_handle_pool[aka_handle].sub_cmd = PS_UTILS_AKA_RUN_ALGO_IN_SW;
    ps_send_cmd (PS_UTILS_AKA_CMD, (void *)&aka_handle_pool[aka_handle]);

    return aka_handle;
  }

  /*-------------------------------------------------------------------------
   AKA will be done on card (Either ISIM or USIM)
  -------------------------------------------------------------------------*/
  else
  {
#ifdef FEATURE_MMGSDI_SESSION_LIB
    LOG_MSG_INFO1("ps_utils_run_aka_algo(): "
                  "Running AKA Algo on card", 0, 0, 0);
    if (FALSE == ps_utils_aka_run_algo_on_card(aka_handle, aka_rand, 
                                               rand_len, autn, autn_len))
    {
      LOG_MSG_ERROR("ps_utils_run_aka_algo(): "
                    "Run AKA algo on card failed, freeing the AKA handle", 
                    0, 0, 0);
      PS_UTILS_AKA_FREE_HANDLE(&aka_handle_pool[aka_handle]);
      return PS_UTILS_AKA_INVALID_HANDLE;
    }
#endif /* FEATURE_MMGSDI_SESSION_LIB */
  }

  return aka_handle;
} /* ps_utils_run_aka_algo() */

/*===========================================================================
FUNCTION PS_UTILS_AKA_CREATE_HMAC

DESCRIPTION  This function is used by the AKA for computing HMAC_ALGO, where  
             ALGO is currently MD5

DEPENDENCIES None

PARAMETERS  algo: Algo type (MD5, SHA...)
  data, data_len: the data over which key computation will occur & length
  key, key_len: the authentication key used in HMAC/MD5 computation & length
  output_data: the resulting output value

RETURN VALUE  True if success.

SIDE EFFECTS  None  
===========================================================================*/
boolean ps_utils_aka_create_hmac
(
  secapi_algo_enum_type  algo,
  uint8                 *data,  
  uint16                 data_len,
  uint8                 *key, 
  uint16                 key_len,
  uint8                 *output_data
)
{
  secapi_handle_type          hash_handle = NULL;
  secapi_hsh_param_data_type  hash_param;
  secerrno_enum_type          status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_aka_create_hmac() called", 0, 0, 0);

  if ((NULL == data) || (NULL == key) || (NULL == output_data))
  {
    LOG_MSG_ERROR("ps_utils_aka_create_hmac(): "
                  "Invalid input parameters", 0, 0, 0);
    return FALSE;
  }

  /*----------------------------------------------------------------------- 
    Create a new instance 
  -----------------------------------------------------------------------*/
  status = secapi_new(&hash_handle, algo);
  if(E_SUCCESS != status)
  {
    LOG_MSG_ERROR("ps_utils_aka_create_hmac(): "
                  "Failed to create hash handle", 0, 0, 0);
    return status;
  }
  
  /*-----------------------------------------------------------------------
    Set the execution platform to ARM
  -----------------------------------------------------------------------*/
  hash_param.common.exec_platform = SECAPI_EXEC_PLATFORM_ARM;
  status = secapi_hsh_set_param(hash_handle,
                                algo,
                                SECAPI_EXEC_PLATFORM,
                                &hash_param );
  if(E_SUCCESS != status)
  {
    LOG_MSG_ERROR("ps_utils_aka_create_hmac(): "
                  "Unable to use ARM platform", 0 , 0, 0);
    return FALSE;  
  }

  /*-----------------------------------------------------------------------
    Set the mode to "atomic" the default processor is accelerator prefered 
  -----------------------------------------------------------------------*/
  hash_param.mode = SECAPI_HSH_INPUT_MODE_ATOMIC;
  status = secapi_hsh_set_param( hash_handle,
                                 algo,
                                 SECAPI_HSH_INPUT_MODE,
                                 &hash_param );
  if(E_SUCCESS != status)
  {
    LOG_MSG_ERROR("ps_utils_aka_create_hmac(): "
                  "Unable to use ARM platform", 0 , 0, 0);
    return FALSE;  
  }

  /*-----------------------------------------------------------------------
    Compute hmac 
  -------------------------------------------------------------------------*/
  status = secapi_hsh_create_hmac( hash_handle,
                                   algo,
                                   data,
                                   data_len,
                                   key,
                                   key_len,
                                   output_data );


  if ( E_SUCCESS != status )
  {
    LOG_MSG_ERROR("ps_utils_aka_create_hmac(): "
                  "secapi_hsh_create_hmac() Failed", 0 , 0, 0);
    return FALSE;  
  }
  
  /*-----------------------------------------------------------------------
   Instance no longer needed - delete 
  -------------------------------------------------------------------------*/
  status = secapi_delete(&hash_handle);
  if ( E_SUCCESS != status )
  {
    LOG_MSG_ERROR("ps_utils_aka_create_hmac(): "
                  "Failed to delte hash handle", 0 , 0, 0);
    return FALSE;  
  }

  return TRUE;
}/* ps_utils_aka_create_hmac() */

/*===========================================================================
FUNCTION PS_UTILS_RUN_AKA_ALGO()

DESCRIPTION  Runs AKAv1 or AKAv2 for the client & returns the result in the 
             callback function provided by the client

DEPENDENCIES  None

PARAMETERS
  res, res_len     - RES value & length
  ck, ck_len       - CK value & length
  hmac_md5_out_ptr - Provides the HMAC_MD5 digest

RETURN VALUE  TRUE  - Success
              FALSE - Failure

SIDE EFFECTS  None
===========================================================================*/
boolean ps_utils_aka_create_digest
(
  ps_utils_aka_ver_enum_type       aka_ver,
  ps_utils_aka_create_digest_info *create_digest_ptr,
  uint8                           *hmac_md5_out_ptr
)
{
  uint8  *key          = NULL;
  uint8   offset = 0;
  uint8   ret_val;
  char    hmac_data[]  = PS_UTILS_AKA_DIGEST_V2_PASS;
  uint8   key_len;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((NULL == create_digest_ptr) || (NULL == hmac_md5_out_ptr))
  {
    LOG_MSG_ERROR("ps_utils_aka_create_digest(): "
                  "Input parameters are NULL", 0, 0, 0);
    return FALSE;
  }

  if ((NULL == create_digest_ptr->res) || (NULL == create_digest_ptr->ck) 
       || (NULL == create_digest_ptr->ik))
  {
    LOG_MSG_ERROR("ps_utils_aka_create_digest(): "
                  "Input parameters are NULL", 0, 0, 0);
    return FALSE;
  }

  if ((PS_UTILS_AKA_MAX_RES < create_digest_ptr->res_len)
       || (PS_UTILS_AKA_CK_LEN < create_digest_ptr->ck_len) 
       || (PS_UTILS_AKA_IK_LEN < create_digest_ptr->ik_len))
  {
    LOG_MSG_ERROR("ps_utils_aka_create_digest(): "
                  "Invlid RES %d, CK %d or IK %d length", 
                  create_digest_ptr->res_len, 
                  create_digest_ptr->ck_len, 
                  create_digest_ptr->ik_len);
    return FALSE;
  }

  key_len = create_digest_ptr->res_len + create_digest_ptr->ck_len 
            + create_digest_ptr->ik_len;

  key = (uint8 *)ps_system_heap_mem_alloc(key_len);
  if (NULL == key)
  {
    LOG_MSG_ERROR("ps_utils_aka_create_digest(): "
                  "PS sytem heap alloc() failed", 0, 0, 0);
    return FALSE;
  }

  memset (key, 0, key_len);
  /*---------------------------------------------------------------------
    key = (RES||IK||CK) in case of AKAv2
  -----------------------------------------------------------------------*/
  memscpy (key + offset, 
           key_len - offset,
           create_digest_ptr->res, 
           create_digest_ptr->res_len);

  offset += create_digest_ptr->res_len;

  memscpy (key + offset, 
           key_len - offset,
           create_digest_ptr->ik, 
           create_digest_ptr->ik_len);

  offset += create_digest_ptr->ik_len;
      
  memscpy (key + offset, 
           key_len - offset,
           create_digest_ptr->ck, 
           create_digest_ptr->ck_len);

  ret_val = ps_utils_aka_create_hmac(SECAPI_MD5,
                                     (uint8 *)hmac_data,
                                     sizeof(hmac_data)-1,
                                     key,
                                     key_len,
                                     hmac_md5_out_ptr);
  if (FALSE == ret_val)
  {
    LOG_MSG_ERROR("ps_utils_aka_create_digest(): "
                  "ps_utils_aka_create_hmac() failed", 0, 0, 0);
    PS_SYSTEM_HEAP_MEM_FREE (key);
    return FALSE;
  }

  PS_SYSTEM_HEAP_MEM_FREE (key);
  return TRUE;
} /* ps_utils_aka_create_digest() */

/*===========================================================================
FUNCTION PS_UTILS_AKA_SEND_FAILURE_TO_CLIENT

DESCRIPTION  Calls the client callback with status as AKA Failure

DEPENDENCIES  None

PARAMETERS  aka_handle -  AKA handle

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ps_utils_aka_send_failure_to_client
(
  ps_utils_aka_handle_type    aka_handle
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_aka_send_failure_to_client() called", 0, 0, 0);
  
  if (TRUE != PS_UTILS_AKA_HANDLE_IS_VALID(aka_handle))
  {
    LOG_MSG_ERROR("ps_utils_aka_send_failure_to_client(): "
                  "Invalid AKA handle", 0, 0, 0);
    return;
  }

  if (NULL == aka_handle_pool[aka_handle].client_cback_ptr)
  {
    LOG_MSG_ERROR("ps_utils_aka_send_failure_to_client(): "
                  "Client callback ptr is NULL", 0, 0, 0);
    return;
  }

  /*-----------------------------------------------------------------------
   Call the client callback with status as FAILURE
  -------------------------------------------------------------------------*/
  aka_handle_pool[aka_handle].client_cback_ptr(aka_handle, 
             (ds_auth_aka_status_enum_type) PS_UTILS_AKA_FAILURE,
             NULL, 
             0, 
             NULL, 
             0,
             aka_handle_pool[aka_handle].user_data);
  
  /*-----------------------------------------------------------------------
   Free the aka_handle
  -------------------------------------------------------------------------*/
  PS_UTILS_AKA_FREE_HANDLE(&aka_handle_pool[aka_handle]);

  return;
} /* ps_utils_aka_send_failure_to_client() */

/*===========================================================================
FUNCTION PS_UTILS_AKA_CMD_HDLR()

DESCRIPTION  This function handles the PS UTILS AKA commands

PARAMETERS  cmd_name:       command that is being handled.
           *user_info_ptr  User data passed during the task switch

RETURN VALUE  None

DEPENDENCIES  This function must be registered as a ps command handler

SIDE EFFECTS  None
===========================================================================*/
void ps_utils_aka_cmd_hdlr
(
  ps_cmd_enum_type    cmd_name,
  void               *user_info_ptr
)
{
#ifdef FEATURE_MMGSDI_SESSION_LIB
  mmgsdi_return_enum_type ret_val;
#endif /* FEATURE_MMGSDI_SESSION_LIB */
  ps_utils_aka_sub_cmd_enum_type sub_cmd;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_aka_cmd_hdlr() called", 0, 0, 0);

  if (PS_UTILS_AKA_CMD != cmd_name)
  {
    LOG_MSG_ERROR("ps_utils_aka_cmd_hdlr(): "
                  "Invalid command posted", 0, 0, 0);
    return;
  }
  
  if (NULL == user_info_ptr)
  {
    sub_cmd = ps_utils_aka_sub_cmd;
  }

  else
  {
    sub_cmd = ((ps_utils_aka_handle_pool_type *)user_info_ptr)->sub_cmd;
  }

  switch(sub_cmd)
  {
#ifdef FEATURE_MMGSDI_SESSION_LIB
    case PS_UTILS_AKA_MMGSDI_CARD_INS_EVT:
      ret_val = mmgsdi_get_all_available_apps (aka_cb.mmgsdi_info.client_id,
                                               ps_utils_aka_apps_cb,
                                               (mmgsdi_client_data_type)0);
      if (MMGSDI_SUCCESS != ret_val)
      {
        LOG_MSG_ERROR("ps_utils_aka_cmd_hdlr(): "
                      "mmgsdi_get_all_available_apps() failed, error %d ", 
                      ret_val, 0, 0);
        return;
      }
      break;

    case PS_UTILS_AKA_MMGSDI_SESSION_REG:
      /*-----------------------------------------------------------------------
        re-opening of session applies only for ISIM. It is a no-op for USIM
      -----------------------------------------------------------------------*/
      if (TRUE == aka_cb.mmgsdi_info.session_info.session_reopen)
      {
        LOG_MSG_INFO2("ps_utils_aka_cmd_hdlr(): "
                      "Trying to re-open session for ISIM", 0, 0, 0);
        if(TRUE == isim_present)
        {
          ps_utils_aka_mmgsdi_session_reg_isim();
        }

        else
        {
          LOG_MSG_ERROR("ps_utils_aka_cmd_hdlr(): "
                        "ISIM not present, session re-open failed", 0, 0, 0);
        }
        aka_cb.mmgsdi_info.session_info.session_reopen = FALSE;
      }

      else
      {
        if(TRUE == isim_present)
        {
          ps_utils_aka_mmgsdi_session_reg_isim();
        }
        else
        {
          ps_utils_aka_mmgsdi_session_reg_usim();
        }
      }
      break;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

    case PS_UTILS_AKA_RUN_ALGO_IN_SW:
      ps_utils_aka_run_algo_in_sw(user_info_ptr);
      break;

#ifdef FEATURE_MMGSDI_SESSION_LIB
    case PS_UTILS_AKA_ISIM_AUTH_CB:
      ps_utils_aka_run_isim_algo_cb_task(user_info_ptr);
      break;

    case PS_UTILS_AKA_USIM_AUTH_CB:
      ps_utils_aka_run_usim_algo_cb_task(user_info_ptr);
      break;
#endif /* FEATURE_MMGSDI_SESSION_LIB */

    default:
      LOG_MSG_ERROR("ps_utils_aka_cmd_hdlr(): "
                    "Invalid sub command during task switch, cmd %d ", 
                     ps_utils_aka_sub_cmd, 0, 0);
      break;
  }

  return;
} /* ps_utils_aka_cmd_hdlr() */

#ifdef FEATURE_MMGSDI_SESSION_LIB
/*===========================================================================
FUNCTION PS_UTILS_AKA_RUN_ISIM_ALGO_CB_TASK

DESCRIPTION  To process ISIM auth result after the task switch from MMGSDI

DEPENDENCIES  None

PARAMETERS  client_data_ptr - Client data

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ps_utils_aka_run_isim_algo_cb_task
(
  void *client_data_ptr
)
{
  uint8   aka_cb_cnt;
  uint8   aka_data_len;
  uint8   offset = 0;
  boolean status;
  uint8                             hmac_md5_output[PS_UTILS_AKA_HMAC_LEN];
  uint8                             hmac_md5_len = PS_UTILS_AKA_HMAC_LEN;
  ps_utils_aka_handle_type          aka_handle = PS_UTILS_AKA_INVALID_HANDLE;
  ps_utils_aka_handle_pool_type    *aka_handle_pool_ptr = NULL;
  ps_utils_aka_isim_auth_info_type *auth_info_ptr       = NULL;
  uint8                            *aka_data_ptr        = NULL;
  ps_utils_aka_create_digest_info   create_digest;
  uint8                             res_len = 0, ck_len = 0, ik_len =0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_aka_run_isim_algo_cb_task() called", 0, 0, 0);

  if (NULL == client_data_ptr)
  {
    LOG_MSG_ERROR("ps_utils_aka_run_isim_algo_cb_task(): "
                  "client_data_ptr is NULL", 0, 0, 0);
    return;
  }

  aka_handle_pool_ptr = (ps_utils_aka_handle_pool_type *)client_data_ptr;
  /*-----------------------------------------------------------------------
   Retrieve the aka_handle based on the address passed as client_data
  -------------------------------------------------------------------------*/
  for(aka_cb_cnt = 0; aka_cb_cnt < PS_UTILS_AKA_MAX_HANDLES; aka_cb_cnt++)
  {
    if (aka_handle_pool_ptr == &aka_handle_pool[aka_cb_cnt])
    {
      aka_handle = aka_cb_cnt;
    }
  }

  if (TRUE != PS_UTILS_AKA_HANDLE_IS_VALID(aka_handle))
  {
    LOG_MSG_ERROR("ps_utils_aka_run_isim_algo_cb_task(): "
                  "Invalid AKA handle", 0, 0, 0);
    return;
  }
  
  if (NULL == aka_handle_pool_ptr->client_cback_ptr)
  {
    LOG_MSG_ERROR("ps_utils_aka_run_isim_algo_cb_task(): "
                  "client callback pointer is NULL", 0, 0, 0);
    PS_UTILS_AKA_FREE_HANDLE(&aka_handle_pool[aka_handle]);
    return;  
  }

  auth_info_ptr = &aka_cb.mmgsdi_info.auth_info.isim;
  /*-----------------------------------------------------------------------
    Client needs to be notified of both success and failure
  -------------------------------------------------------------------------*/
  if(MMGSDI_AUTS_FAIL == auth_info_ptr->response_header.mmgsdi_status)
  {
    if ( auth_info_ptr->auts_len <= PS_UTILS_AKA_MAX_AUTS )
    {
      LOG_MSG_INFO1("ps_utils_aka_run_isim_algo_cb_task(): "
                    "ISIM Algo CB - sync failure", 0, 0, 0);
      
      aka_data_len = sizeof(auth_info_ptr->auts_len) 
                            + auth_info_ptr->auts_len;
      aka_data_ptr = (uint8 *)ps_system_heap_mem_alloc(aka_data_len);
      if (NULL == aka_data_ptr)
      {
        LOG_MSG_ERROR("ps_utils_aka_run_isim_algo_cb_task(): "
                      "PS sytem heap alloc() failed", 0, 0, 0);
        ps_utils_aka_send_failure_to_client (aka_handle);
        return;
      }
      aka_data_ptr[0] = auth_info_ptr->auts_len;
      memscpy (aka_data_ptr + 1, auth_info_ptr->auts_len, 
               auth_info_ptr->auts, auth_info_ptr->auts_len);

      aka_handle_pool_ptr->client_cback_ptr (aka_handle, 
                 (ds_auth_aka_status_enum_type) PS_UTILS_AKA_SYNC_FAILURE,
                 NULL, 
                 0, 
                 aka_data_ptr, 
                 aka_data_len,
                 aka_handle_pool_ptr->user_data);

      /*---------------------------------------------------------------------
        Free the aka_data_ptr & aka_handle
      -----------------------------------------------------------------------*/
      PS_SYSTEM_HEAP_MEM_FREE (aka_data_ptr);
      PS_UTILS_AKA_FREE_HANDLE(&aka_handle_pool[aka_handle]);
      return;
    }
    else
    {
      LOG_MSG_ERROR("ps_utils_aka_run_isim_algo_cb_task(): "
                    "Invalid AUTS", 0, 0, 0);
      ps_utils_aka_send_failure_to_client (aka_handle);
      return;
    }
  }

  if(MMGSDI_SUCCESS == auth_info_ptr->response_header.mmgsdi_status)
  {
    res_len = auth_info_ptr->res_len;
    ck_len  = auth_info_ptr->ck_len;
	  ik_len  = auth_info_ptr->ik_len;
    
    /*---------------------------------------------------------------------
      The 3G RES, CK & IK length has to be less than or equal to 16 according
      to TS 31.102 7.1.2
    -----------------------------------------------------------------------*/
    if ( res_len > PS_UTILS_AKA_MAX_RES || 
         ck_len > PS_UTILS_AKA_CK_LEN   || 
         ik_len > PS_UTILS_AKA_IK_LEN )
    {
      LOG_MSG_ERROR("ps_utils_aka_run_isim_algo_cb_task(): "
                    "Invalid auth values, RES_LEN %d, CK_LEN %d, IK_LEN %d", 
        auth_info_ptr->res_len , auth_info_ptr->ck_len, auth_info_ptr->ik_len);
      ps_utils_aka_send_failure_to_client (aka_handle);
      return;
    }

    /*-----------------------------------------------------------------------
      For AKAv1 just copy RES as the HMAC output
    -----------------------------------------------------------------------*/
    if (PS_UTILS_AKA_V1 == aka_handle_pool_ptr->aka_ver)
    {
      memscpy(hmac_md5_output, sizeof(hmac_md5_output),
              auth_info_ptr->res, auth_info_ptr->res_len);
      hmac_md5_len = auth_info_ptr->res_len;
    }

    else
    {
      /*---------------------------------------------------------------------
        Copy the auth data into create digest ptr & calculate HMAC_MD5
      -----------------------------------------------------------------------*/
      create_digest.res     = auth_info_ptr->res;
      create_digest.res_len = auth_info_ptr->res_len;
      create_digest.ck      = auth_info_ptr->ck;
      create_digest.ck_len  = auth_info_ptr->ck_len;
      create_digest.ik      = auth_info_ptr->ik;
      create_digest.ik_len  = auth_info_ptr->ik_len;
  
      status = ps_utils_aka_create_digest (aka_handle_pool_ptr->aka_ver,
                                           &create_digest,
                                           hmac_md5_output);
      if (FALSE == status)
      {
        LOG_MSG_ERROR("ps_utils_aka_run_isim_algo_cb_task(): "
                      "Creating HMAC_MD5 digest failed", 0, 0, 0);
        ps_utils_aka_send_failure_to_client (aka_handle);
        return;
      }
    }

    aka_data_len = sizeof(res_len) + res_len 
                   + sizeof(ck_len) + ck_len
                   + sizeof(ik_len) + ik_len;

    aka_data_ptr = (uint8 *)ps_system_heap_mem_alloc(aka_data_len);
    if (NULL == aka_data_ptr)
    {
      LOG_MSG_ERROR("ps_utils_aka_run_isim_algo_cb_task(): "
                    "PS sytem heap alloc() failed", 0, 0, 0);
      ps_utils_aka_send_failure_to_client (aka_handle);
      return;
    }

    /*---------------------------------------------------------------------
     Set the aka_data as specified below
     Bytes:    1        L1         1         L2        1        L3
          ---------------------------------------------------------------
          |RES_LEN|     RES     |CK_LEN|     CK     |IK_LEN|     IK     |  
          ---------------------------------------------------------------

     Byte(s)                    Description          Length
     -------------------------------------------------------
     1                          Length of RES (L1)   1
     2 to (L1+1)                RES                  L1
     (L1+2)                     Length of CK(L2)     1
     (L1+3) to (L1+L2+2)	      CK                   L2
     (L1+L2+3)                  Length of IK(L3)     1
     (L1+L2+4) to (L1+L2+L3+3)  IK                   L3
     -------------------------------------------------------
    -----------------------------------------------------------------------*/
    offset = 0;
    aka_data_ptr[offset] = res_len;
    offset += sizeof(res_len);

    if( (aka_data_len - offset) >= res_len )
    {
       memscpy (aka_data_ptr + offset, res_len,
                auth_info_ptr->res, res_len);
       offset += res_len;
    }

    aka_data_ptr[offset] = ck_len;
    offset += sizeof(ck_len);

    if( (aka_data_len - offset) >= ck_len )
    {
       memscpy (aka_data_ptr + offset, ck_len,
                auth_info_ptr->ck, ck_len);
       offset += ck_len;
    }

    aka_data_ptr[offset] =  ik_len;
    offset += sizeof(ik_len);

    if( (aka_data_len - offset) >= ik_len )
    {
       memscpy (aka_data_ptr + offset, ik_len,
                auth_info_ptr->ik, ik_len);
       offset += ik_len;
    }

    LOG_MSG_INFO2("ps_utils_aka_run_isim_algo_cb_task(): "
                  "ISIM Algo CB - Success", 0, 0, 0);
    aka_handle_pool_ptr->client_cback_ptr (aka_handle, 
               (ds_auth_aka_status_enum_type) PS_UTILS_AKA_SUCCESS,
               hmac_md5_output,
               hmac_md5_len,
               aka_data_ptr,
               aka_data_len, 
               aka_handle_pool_ptr->user_data);

    /*---------------------------------------------------------------------
      Free the allocated memory & the aka_handle
    -----------------------------------------------------------------------*/
    PS_SYSTEM_HEAP_MEM_FREE (aka_data_ptr);
    PS_UTILS_AKA_FREE_HANDLE(&aka_handle_pool[aka_handle]);
    return;
  }

  /*---------------------------------------------------------------------
    If reached here means it is an error, ISIM Algo failed - notify client
  -----------------------------------------------------------------------*/
  LOG_MSG_ERROR("ps_utils_aka_run_isim_algo_cb_task(): "
                "ISIM Algo CB - failed, error %d", 
                auth_info_ptr->response_header.mmgsdi_status, 0, 0);
  ps_utils_aka_send_failure_to_client (aka_handle);

  return;
} /* ps_utils_aka_run_isim_algo_cb_task() */

/*===========================================================================
FUNCTION PS_UTILS_AKA_RUN_ISIM_ALGO_CB

DESCRIPTION  Will be registered with the ISIM module and will be called as a 
             response to retrieve RUN ISIM auth request.

DEPENDENCIES  ISIM module up and running

PARAMETERS
  status, cnf - success or failure, confirmation type
  cnf_ptr - response - union member isim_auth_cnf - will contain valid data

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ps_utils_aka_run_isim_algo_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  mmgsdi_isim_auth_cnf_type               *isim_auth_cnf   = NULL;
  mmgsdi_response_header_type             *response_header = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_aka_run_isim_algo_cb() called", 0, 0, 0);

  /*-----------------------------------------------------------------------
   Sanity Check
  -------------------------------------------------------------------------*/
  if ( NULL == cnf_ptr ) 
  {
    LOG_MSG_ERROR("ps_utils_aka_run_isim_algo_cb(): "
                  "cnf_ptr NULL", 0, 0, 0);
    return;
  }

  if( MMGSDI_SUCCESS != status )
  {
    LOG_MSG_ERROR("ps_utils_aka_run_isim_algo_cb(): "
                  "MMGSDI AKA ISIM cb, failure status %d", status, 0, 0);
  }

  if( MMGSDI_ISIM_AUTH_CNF != cnf )
  {
    LOG_MSG_ERROR("ps_utils_aka_run_isim_algo_cb(): "
                  "Improper command confirmation, cnf %d", cnf, 0, 0);
    return;
  }

  isim_auth_cnf = (mmgsdi_isim_auth_cnf_type *) &(cnf_ptr->isim_auth_cnf);
  response_header = (mmgsdi_response_header_type*) 
                                    &(isim_auth_cnf->response_header);

  if (NULL == ((ps_utils_aka_handle_pool_type *)response_header->client_data))
  {
    LOG_MSG_ERROR("ps_utils_aka_run_isim_algo_cb(): "
                  "Client data is NULL", 0, 0, 0);
    return;
  }

  /*---------------------------------------------------------------------
    Copy all the ISIM authentication data if valid & switch task
  -----------------------------------------------------------------------*/
  if ((isim_auth_cnf->res.data_len > PS_UTILS_AKA_MAX_RES)
      || (isim_auth_cnf->ck.data_len > PS_UTILS_AKA_CK_LEN)
      || (isim_auth_cnf->ik.data_len > PS_UTILS_AKA_IK_LEN)
      || (isim_auth_cnf->auts.data_len > PS_UTILS_AKA_MAX_AUTS))
  {
    LOG_MSG_ERROR("ps_utils_aka_run_isim_algo_cb(): "
                  "Invalid auth info, RES_LEN %d, CK_LEN %d, IK_LEN %d",
                  isim_auth_cnf->res.data_len, 
                  isim_auth_cnf->ck.data_len, 
                  isim_auth_cnf->ik.data_len);
    LOG_MSG_ERROR("ps_utils_aka_run_isim_algo_cb(): "
                  "AUTS_LEN %d", isim_auth_cnf->auts.data_len, 0, 0);
    return;
  }

  memscpy ( &aka_cb.mmgsdi_info.auth_info.isim.response_header, 
            sizeof(mmgsdi_response_header_type),
            &isim_auth_cnf->response_header, 
            sizeof(mmgsdi_response_header_type) );

  if (NULL != isim_auth_cnf->res.data_ptr && isim_auth_cnf->res.data_len > 0)
  {
    memscpy (aka_cb.mmgsdi_info.auth_info.isim.res,
             sizeof(uint8)*PS_UTILS_AKA_MAX_RES,
             isim_auth_cnf->res.data_ptr, 
             isim_auth_cnf->res.data_len);
    aka_cb.mmgsdi_info.auth_info.isim.res_len 
                                      = (uint8)isim_auth_cnf->res.data_len;
  }

  if ((NULL != isim_auth_cnf->ck.data_ptr) && (isim_auth_cnf->ck.data_len > 0))
  {
    memscpy (aka_cb.mmgsdi_info.auth_info.isim.ck,
             sizeof(uint8)*PS_UTILS_AKA_CK_LEN,
             isim_auth_cnf->ck.data_ptr, 
             isim_auth_cnf->ck.data_len);
    aka_cb.mmgsdi_info.auth_info.isim.ck_len 
                                        = (uint8)isim_auth_cnf->ck.data_len;
  }

  if ((NULL != isim_auth_cnf->ik.data_ptr) && (isim_auth_cnf->ik.data_len > 0))
  {
    memscpy (aka_cb.mmgsdi_info.auth_info.isim.ik,
             sizeof(uint8)*PS_UTILS_AKA_IK_LEN,
             isim_auth_cnf->ik.data_ptr, 
             isim_auth_cnf->ik.data_len);
    aka_cb.mmgsdi_info.auth_info.isim.ik_len 
                                      = (uint8)isim_auth_cnf->ik.data_len;
  }

  if ((NULL != isim_auth_cnf->auts.data_ptr) 
       && (isim_auth_cnf->auts.data_len > 0))
  {
    memscpy (aka_cb.mmgsdi_info.auth_info.isim.auts,
             sizeof(uint8)*PS_UTILS_AKA_MAX_AUTS,
             isim_auth_cnf->auts.data_ptr, 
             isim_auth_cnf->auts.data_len);
    aka_cb.mmgsdi_info.auth_info.isim.auts_len 
                                        = (uint8)isim_auth_cnf->auts.data_len;
  }

  LOG_MSG_INFO1("ps_utils_aka_run_isim_algo_cb(): "
                "MMGSDI ISIM Algo CB - Switching Task Ctx", 0, 0, 0);

  ((ps_utils_aka_handle_pool_type *)response_header->client_data)->sub_cmd 
                                                 = PS_UTILS_AKA_ISIM_AUTH_CB;
  ps_send_cmd (PS_UTILS_AKA_CMD, (void*)response_header->client_data);
  return;
}/* ps_utils_aka_run_isim_algo_cb() */

/*===========================================================================
FUNCTION PS_UTILS_AKA_RUN_USIM_ALGO_CB_TASK

DESCRIPTION  To process USIM auth result after the task switch from MMGSDI

DEPENDENCIES  None

PARAMETERS  client_data_ptr - Client data

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ps_utils_aka_run_usim_algo_cb_task
(
  void *client_data_ptr
)
{
  uint8   aka_cb_cnt;
  uint8   data_len;
  uint8   res_len;
  uint8   ck_len;
  uint8   ik_len;
  boolean status;
  mmgsdi_return_enum_type           mmgsdi_status;
  uint8                             hmac_md5_output[PS_UTILS_AKA_HMAC_LEN];
  uint8                             hmac_md5_len = PS_UTILS_AKA_HMAC_LEN;
  ps_utils_aka_handle_type          aka_handle = PS_UTILS_AKA_INVALID_HANDLE;
  ps_utils_aka_handle_pool_type    *aka_handle_pool_ptr = NULL;
  mmgsdi_usim_auth_cnf_type        *usim_info_ptr       = NULL;
  uint8                            *auth_data_ptr       = NULL;
  ps_utils_aka_create_digest_info   create_digest;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_aka_run_usim_algo_cb_task() called", 0, 0, 0);

  if (NULL == client_data_ptr)
  {
    LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb_task(): "
                  "client_data_ptr is NULL", 0, 0, 0);
    return;
  }

  aka_handle_pool_ptr = (ps_utils_aka_handle_pool_type *)client_data_ptr;
  /*-----------------------------------------------------------------------
   Retrieve the aka_handle based on the address passed as client_data
  -------------------------------------------------------------------------*/
  for(aka_cb_cnt = 0; aka_cb_cnt < PS_UTILS_AKA_MAX_HANDLES; aka_cb_cnt++)
  {
    if (aka_handle_pool_ptr == &aka_handle_pool[aka_cb_cnt])
    {
      aka_handle = aka_cb_cnt;
    }
  }

  if (TRUE != PS_UTILS_AKA_HANDLE_IS_VALID(aka_handle))
  {
    LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb_task(): "
                  "Invalid AKA handle", 0, 0, 0);
    return;
  }
  
  usim_info_ptr = &(aka_cb.mmgsdi_info.auth_info.usim);
  auth_data_ptr = aka_cb.mmgsdi_info.auth_info.usim.auth_response.data_ptr;

  /*-----------------------------------------------------------------------
    Client needs to be notified of both success and failure
  -------------------------------------------------------------------------*/
  mmgsdi_status = usim_info_ptr->response_header.mmgsdi_status;
  if (MMGSDI_SUCCESS != mmgsdi_status)
  {
    LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb_task(): "
                  "MMGSDI returned error, %d", mmgsdi_status, 0, 0);
    ps_utils_aka_send_failure_to_client (aka_handle);
    return;
  }

  if ((NULL == auth_data_ptr)
      || (0 == usim_info_ptr->auth_response.data_len))
  {
    LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb_task(): "
                  "auth_data_ptr is NULL or auth_data_len is 0", 0, 0, 0);
    ps_utils_aka_send_failure_to_client (aka_handle);
    return;
  }

  /*-----------------------------------------------------------------------
    Check if client callback function is not NULL
  -------------------------------------------------------------------------*/
  if (NULL == aka_handle_pool_ptr->client_cback_ptr)
  {
    LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb_task(): "
                  "Client callback ptr is NULL", 0, 0, 0);
    PS_SYSTEM_HEAP_MEM_FREE (auth_data_ptr);
    PS_UTILS_AKA_FREE_HANDLE(&aka_handle_pool[aka_handle]);
    return;
  }

  if(PS_UTILS_AKA_AUTH_SYNC_FAILURE == *auth_data_ptr)
  {
    data_len = *(uint8*)(auth_data_ptr + 1);

    if (data_len <= PS_UTILS_AKA_MAX_AUTS)
    {
      LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb_task(): "
                    "USIM Algo CB - sync failure", 0, 0, 0);
      
      /*---------------------------------------------------------------------
        Skip the first status byte when sending the sync failure data to 
        the client
      ---------------------------------------------------------------------*/
      aka_handle_pool_ptr->client_cback_ptr (aka_handle, 
                 (ds_auth_aka_status_enum_type) PS_UTILS_AKA_SYNC_FAILURE,
                 NULL, 
                 0, 
                 (auth_data_ptr + 1), 
                 data_len + 1,
                 aka_handle_pool_ptr->user_data);

      /*---------------------------------------------------------------------
        Free the aka_handle & the auth data memory
      ---------------------------------------------------------------------*/
      PS_SYSTEM_HEAP_MEM_FREE (auth_data_ptr);
      PS_UTILS_AKA_FREE_HANDLE(&aka_handle_pool[aka_handle]);
      return;
    }
    else
    {
      LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb_task(): "
                    "Invalid AUTS", 0, 0, 0);
      PS_SYSTEM_HEAP_MEM_FREE (auth_data_ptr);
      ps_utils_aka_send_failure_to_client (aka_handle);
      return;
    }
  }

  if(PS_UTILS_AKA_AUTH_3G_UMTS_SUCCESS == *(auth_data_ptr))
  {
    /*---------------------------------------------------------------------
     The auth_data obtained will be as specified below
     Bytes:    1      1        L1         1       L2        1        L3
          ----------------------------------------------------------------
          |Status |RES_LEN|    RES    |CK_LEN|    CK    |IK_LEN|    IK    |  
          ----------------------------------------------------------------

      The 3G RES, CK & IK length has to be less than or equal to 16 according
      to TS 31.102 7.1.2
    -----------------------------------------------------------------------*/
    res_len = *(auth_data_ptr + 1); /*res len */
    if (res_len > PS_UTILS_AKA_MAX_RES)
    {
      LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb_task(): "
                    "Invalid 3G RES length (%d)", res_len , 0, 0) ;
      PS_SYSTEM_HEAP_MEM_FREE (auth_data_ptr);
      ps_utils_aka_send_failure_to_client (aka_handle);
      return;
    }

    create_digest.res = (auth_data_ptr + 1 + sizeof(res_len));
    create_digest.res_len = res_len;

    ck_len = *(auth_data_ptr + 1 + sizeof(res_len) + res_len);
    if (ck_len > PS_UTILS_AKA_CK_LEN)
    {
      LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb_task(): "
                    "Invalid 3G CK length (%d)", ck_len , 0, 0) ;
      PS_SYSTEM_HEAP_MEM_FREE (auth_data_ptr);
      ps_utils_aka_send_failure_to_client (aka_handle);
      return;
    }

    create_digest.ck = (auth_data_ptr + 1 + sizeof(res_len) + res_len
                                                + sizeof(ck_len));
    create_digest.ck_len = ck_len;

    ik_len = *(auth_data_ptr + 1 + sizeof(res_len) + res_len
                                 + sizeof(ck_len) + ck_len);
    if (ik_len > PS_UTILS_AKA_IK_LEN)
    {
      LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb_task(): "
                    "Invalid 3G IK length (%d)", ik_len , 0, 0) ;
      PS_SYSTEM_HEAP_MEM_FREE (auth_data_ptr);
      ps_utils_aka_send_failure_to_client (aka_handle);
      return;
    }

    create_digest.ik = (auth_data_ptr + 1 + sizeof(res_len) + res_len
                         + sizeof(ck_len) + ck_len + sizeof(ik_len));
    create_digest.ik_len = ik_len;

    data_len = sizeof(res_len) + res_len + sizeof(ck_len) + ck_len +
               sizeof(ik_len) + ik_len;

    /*-----------------------------------------------------------------------
      For AKAv1 just copy RES as the HMAC output
    -----------------------------------------------------------------------*/
    if (PS_UTILS_AKA_V1 == aka_handle_pool_ptr->aka_ver)
    {
      memscpy(hmac_md5_output, sizeof(hmac_md5_output),
              create_digest.res, create_digest.res_len);
      hmac_md5_len = create_digest.res_len;
    }
    
    else
    {
      /*---------------------------------------------------------------------
        Create the HAMC_MD5 digest
      -----------------------------------------------------------------------*/
      status = ps_utils_aka_create_digest (aka_handle_pool_ptr->aka_ver,                                 
                                           &create_digest,
                                           hmac_md5_output);
      if (FALSE == status)
      {
        LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb_task(): "
                      "Creating HMAC_MD5 digest failed", 0, 0, 0);
        PS_SYSTEM_HEAP_MEM_FREE (auth_data_ptr);
        ps_utils_aka_send_failure_to_client (aka_handle);
        return;
      }
    }

    /*---------------------------------------------------------------------
      Skip the first status byte when sending success result to client
    -----------------------------------------------------------------------*/
    LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb_task(): "
                  "USIM Algo CB - Success", 0, 0, 0);
    aka_handle_pool_ptr->client_cback_ptr (aka_handle, 
               (ds_auth_aka_status_enum_type) PS_UTILS_AKA_SUCCESS,
               hmac_md5_output,
               hmac_md5_len,
               auth_data_ptr + 1,
               data_len, 
               aka_handle_pool_ptr->user_data);

    /*-----------------------------------------------------------------------
      Free the aka_handle & the auth data memory
    -------------------------------------------------------------------------*/
    PS_SYSTEM_HEAP_MEM_FREE (auth_data_ptr);
    PS_UTILS_AKA_FREE_HANDLE(&aka_handle_pool[aka_handle]);
    return;
  }

  /*-----------------------------------------------------------------------
    if you get here there is an error somewhere
  -------------------------------------------------------------------------*/
  LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb_task(): "
                "USIM Algo CB - Failure = 0x%x", 
                *((uint8*)auth_data_ptr), 0, 0);
  PS_SYSTEM_HEAP_MEM_FREE (auth_data_ptr);
  ps_utils_aka_send_failure_to_client (aka_handle);

  return;
} /* ps_utils_aka_run_usim_algo_cb_task() */

/*===========================================================================
FUNCTION PS_UTILS_AKA_RUN_USIM_ALGO_CB

DESCRIPTION  Will be registered with the USIM module and will be called as a 
             response to retrieve RUN USIM auth request.

DEPENDENCIES  USIM module up and running

PARAMETERS
  status, cnf - success or failure, confirmation type
  cnf_ptr - response - union member usim_auth_cnf - will contain valid data

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ps_utils_aka_run_usim_algo_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  mmgsdi_usim_auth_cnf_type    *usim_auth_cnf   = NULL;
  mmgsdi_response_header_type  *response_header = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_aka_run_usim_algo_cb() called", 0, 0, 0);

  /*-----------------------------------------------------------------------
   Sanity Check
  -------------------------------------------------------------------------*/
  if ( NULL == cnf_ptr ) 
  {
    LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb(): "
                  "cnf_ptr NULL", 0, 0, 0);
    return;
  }

  if( MMGSDI_SUCCESS != status )
  {
    LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb(): "
                  "MMGSDI AKA USIM cb, failure status %d", status, 0, 0);
  }

  if( MMGSDI_USIM_AUTH_CNF != cnf )
  {
    LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb(): "
                  "Improper command confirmation, cnf %d", cnf, 0, 0);
    return;
  }

  usim_auth_cnf = (mmgsdi_usim_auth_cnf_type *) &(cnf_ptr->usim_auth_cnf);
  response_header = (mmgsdi_response_header_type*) 
                                    &(usim_auth_cnf->response_header);

  if (NULL == (void *)response_header->client_data)
  {
    LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb(): "
                  "Client data is NULL", 0, 0, 0);
    return;
  }

  /*---------------------------------------------------------------------
    Copy all the USIM authentication data if valid & switch task
  -----------------------------------------------------------------------*/
  memscpy ( &aka_cb.mmgsdi_info.auth_info.usim,
            sizeof(mmgsdi_usim_auth_cnf_type),
            usim_auth_cnf,
            sizeof(mmgsdi_usim_auth_cnf_type) );

  if ((0 != usim_auth_cnf->auth_response.data_len)
      && (NULL != usim_auth_cnf->auth_response.data_ptr
      && (MMGSDI_SUCCESS == response_header->mmgsdi_status)))
  {
    aka_cb.mmgsdi_info.auth_info.usim.auth_response.data_ptr
        = (uint8 *)ps_system_heap_mem_alloc(
           usim_auth_cnf->auth_response.data_len);

    if (NULL == aka_cb.mmgsdi_info.auth_info.usim.auth_response.data_ptr)
    {
      LOG_MSG_ERROR("ps_utils_aka_run_usim_algo_cb(): "
                    "PS sytem heap alloc() failed", 0, 0, 0);
      return;
    }

    memscpy (aka_cb.mmgsdi_info.auth_info.usim.auth_response.data_ptr,
            usim_auth_cnf->auth_response.data_len,
            usim_auth_cnf->auth_response.data_ptr,
            usim_auth_cnf->auth_response.data_len);
  }

  LOG_MSG_INFO1("ps_utils_aka_run_usim_algo_cb(): "
                "MMGSDI USIM Algo CB - Switching Task Ctx", 0, 0, 0);

  ((ps_utils_aka_handle_pool_type *)response_header->client_data)->sub_cmd 
                                                 = PS_UTILS_AKA_USIM_AUTH_CB;
  ps_send_cmd (PS_UTILS_AKA_CMD, (void*)response_header->client_data);
  return;

}/* ps_utils_aka_run_usim_algo_cb() */

/*===========================================================================
FUNCTION PS_UTILS_AKA_RUN_ALGO_ON_CARD

DESCRIPTION Will call ISIM/USIM module to execute aka algorithm on a given rand

DEPENDENCIES  None.

PARAMETERS  aka_handle       - AKA pool handle
            rand, rand_len   - RAND value & length
            autn, autn_len   - AUTN value & length

RETURN VALUE  TRUE  - Success
              FALSE - Failure

SIDE EFFECTS  None
===========================================================================*/
boolean ps_utils_aka_run_algo_on_card
(
  ps_utils_aka_handle_type    aka_handle,
  uint8                      *aka_rand,
  uint8                       rand_len,
  uint8                      *autn,
  uint8                       autn_len
)
{
  mmgsdi_data_type            auth_data;
  mmgsdi_return_enum_type     retval;
  uint8  databuf[PS_UTILS_AKA_AUTN_SIZE_LEN + PS_UTILS_AKA_MAX_AUTN_LEN 
                 + PS_UTILS_AKA_RAND_SIZE_LEN + PS_UTILS_AKA_MAX_RAND_LEN];
  uint8  offset = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_aka_run_algo_on_card() called", 0, 0, 0);

  /*-----------------------------------------------------------------------
   Sanity Check
  -------------------------------------------------------------------------*/
  if (TRUE != PS_UTILS_AKA_HANDLE_IS_VALID(aka_handle))
  {
    LOG_MSG_ERROR("ps_utils_aka_run_algo_on_card(): "
                  "Invalid AKA handle", 0, 0, 0);
    return FALSE;
  }
  
  if ((NULL == aka_rand) || (NULL == autn)
      || (rand_len > PS_UTILS_AKA_MAX_RAND_LEN)
      || (autn_len > PS_UTILS_AKA_MAX_AUTN_LEN))
  {
    LOG_MSG_ERROR("ps_utils_aka_run_algo_on_card(): "
                  "Invalid RAND or AUTN parameters", 0, 0, 0);
    return FALSE;
  }

  /*-----------------------------------------------------------------------
   Set the rand_data as specified by MMGSDI team (per standard)
   Byte(s)               Description          Length
   1                     Length of RAND (L1)   1
   2 to (L1+1)           RAND                  L1
   (L1+2)                Length of AUTN        1
   (L1+3) to (L1+L2+2)	 AUTN                  L2
  -------------------------------------------------------------------------*/
  auth_data.data_len = rand_len + autn_len + (2*sizeof(uint8));
  auth_data.data_ptr = databuf;

  memscpy(databuf, sizeof(databuf), &rand_len, sizeof(uint8));
  offset += sizeof(uint8);

  memscpy(databuf + offset, sizeof(databuf) - offset, aka_rand, rand_len);
  offset += rand_len;

  memscpy(databuf + offset, sizeof(databuf) - offset, &autn_len, sizeof(uint8));
  offset += sizeof(uint8);

  memscpy(databuf + offset, sizeof(databuf) - offset, autn, autn_len);

  /*-----------------------------------------------------------------------
   Based on isim_present & session active decide ISIM or USIM
  -------------------------------------------------------------------------*/
  if((TRUE == isim_present)
     && (TRUE == aka_cb.mmgsdi_info.session_info.session_active))
  {
    LOG_MSG_INFO1("ps_utils_aka_run_algo_on_card(): "
                  "Running MMGSDI ISIM AKA Algo", 0, 0, 0);
    retval = mmgsdi_session_isim_authenticate(
                                    aka_cb.mmgsdi_info.session_info.session_id,
                                    auth_data,
                                    ps_utils_aka_run_isim_algo_cb,
                                    (uint32)(&(aka_handle_pool[aka_handle])));
  }

  else if((FALSE == isim_present)
           && (TRUE == aka_cb.mmgsdi_info.session_info.session_active))
  {
    LOG_MSG_INFO1("ps_utils_aka_run_algo_on_card(): "
                  "Running MMGSDI USIM AKA Algo", 0, 0, 0);
    retval = mmgsdi_session_usim_authenticate(
                                    aka_cb.mmgsdi_info.session_info.session_id,
                                    MMGSDI_AUTN_USIM_3G_CONTEXT,
                                    auth_data,
                                    ps_utils_aka_run_usim_algo_cb,
                                    (uint32)(&(aka_handle_pool[aka_handle])));  
  }

  else
  {
    LOG_MSG_ERROR("ps_utils_aka_run_algo_on_card(): "
                  "Neither ISIM nor USIM is present", 0, 0, 0);
    return FALSE;
  }

  /*-----------------------------------------------------------------------
   Notify failure to the caller - if success, result will come async
  -------------------------------------------------------------------------*/
  if (MMGSDI_SUCCESS != retval) 
  {
    LOG_MSG_ERROR("ps_utils_aka_run_algo_on_card(): "
                  "mmgsdi session authenticate() returned error = %d", 
                  retval, 0, 0);
    return FALSE;
  }

  return TRUE;
}/* ps_utils_aka_run_algo_on_card() */

/*===========================================================================
FUNCTION PS_UTILS_AKA_APPS_CB

DESCRIPTION  CB function registered with mmgsdi to obtain all the APPS info

DEPENDENCIES  None

PARAMETERS  status  - success or failure
            cnf     - type of result being retrieved
            cnf_ptr - the result being retrieved

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ps_utils_aka_apps_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  mmgsdi_get_all_available_apps_cnf_type *all_apps_info_ptr = NULL;
  uint8 app_cnt, app_num;
  uint8 isim_app_sign[] = PS_UTILS_AKA_ISIM_APP_SIGN;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_aka_apps_cb() called", 0, 0, 0);

  if( NULL == cnf_ptr )
  {
    LOG_MSG_ERROR("ps_utils_aka_apps_cb(): "
                  "cnf_ptr is NULL", 0, 0, 0);
    return;
  }

  all_apps_info_ptr = (mmgsdi_get_all_available_apps_cnf_type *) 
                       &(cnf_ptr->get_all_available_apps_cnf);

  LOG_MSG_INFO2("ps_utils_aka_apps_cb(): "
                "MMGSDI get all APPS info callback", 0, 0, 0);

  if ((MMGSDI_GET_ALL_AVAILABLE_APPS_CNF != cnf) 
       && (MMGSDI_GET_ALL_AVAILABLE_APPS_CNF 
           != all_apps_info_ptr->response_header.response_type))
  {
    LOG_MSG_ERROR("ps_utils_aka_apps_cb(): "
                  "Improper command confirmation cnf %d response_type %d", 
                  cnf, all_apps_info_ptr->response_header.response_type, 0);
    return; 
  }

  if ((MMGSDI_SUCCESS != status) 
      && (MMGSDI_SUCCESS != all_apps_info_ptr->response_header.mmgsdi_status))
  {
    LOG_MSG_ERROR("ps_utils_aka_apps_cb(): "
                  "MMGSDI-CLIENT ID Reg failure status %d "
                  "and mmgsdi_status %d are ", status, 
                  all_apps_info_ptr->response_header.mmgsdi_status, 0);
    return;
  }

  /*-----------------------------------------------------------------------
   Copy the required APP info if success
  -------------------------------------------------------------------------*/
  app_num = (uint8)all_apps_info_ptr->card1_num_app;

  for (app_cnt = 0; app_cnt < app_num; app_cnt++)
  {
    /*---------------------------------------------------------------------
      If present ISIM APP will be set to APP_UNKNOWN type & will have the 
      app data set to "A0 00 00 00 87 10 04"
    ---------------------------------------------------------------------*/
    if (MMGSDI_APP_UNKNOWN 
          == all_apps_info_ptr->card1_app_info[app_cnt].app_data.app_type)
    {
      if (0 == memcmp(
            all_apps_info_ptr->card1_app_info[app_cnt].app_data.aid.data_ptr,
            isim_app_sign,
            sizeof(isim_app_sign)))
      {
        isim_present = TRUE;
        break;
      }
    }
  } /* end for(app_cnt..) */

  /*-------------------------------------------------------------------------
    Copy the APP data if ISIM APP is present. Post a command so the MMGSDI
    ISIM session can be opened with the app data available.
  -------------------------------------------------------------------------*/
  if(TRUE == isim_present)
  {
    aka_cb.mmgsdi_info.session_info.aid.data_len 
         = all_apps_info_ptr->card1_app_info[app_cnt].app_data.aid.data_len;
    if(MMGSDI_MAX_AID_LEN < aka_cb.mmgsdi_info.session_info.aid.data_len)
    {
      LOG_MSG_ERROR("ps_utils_aka_apps_cb(): "
                    "Invalid APP data", 0, 0, 0);
      return;
    }

    memscpy(aka_cb.mmgsdi_info.session_info.aid.data_ptr,
            aka_cb.mmgsdi_info.session_info.aid.data_len,
            all_apps_info_ptr->card1_app_info[app_cnt].app_data.aid.data_ptr,
            aka_cb.mmgsdi_info.session_info.aid.data_len);

    aka_cb.mmgsdi_info.session_info.session_type 
                                        = MMGSDI_NON_PROV_SESSION_SLOT_1;

  }

  /*-------------------------------------------------------------------------
    If no ISIM is present, fallback to USIM
  -------------------------------------------------------------------------*/
  ps_utils_aka_sub_cmd = PS_UTILS_AKA_MMGSDI_SESSION_REG;
  ps_send_cmd (PS_UTILS_AKA_CMD, NULL);

  return;
}

/*===========================================================================
FUNCTION PS_UTILS_AKA_MMGSDI_SESSION_OPEN_CB()

DESCRIPTION  Session open callback

PARAMETERS  status       - success or failure 
            cnf, cnf_ptr - type & result being retrieved 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ps_utils_aka_mmgsdi_session_open_cb 
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type    cnf,
  const mmgsdi_cnf_type  *cnf_ptr
)
{
  const mmgsdi_session_open_cnf_type *ssn_open_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_aka_mmgsdi_session_open_cb() called", 0, 0, 0);

  if( NULL == cnf_ptr )
  {
    LOG_MSG_ERROR("ps_utils_aka_mmgsdi_session_open_cb(): "
                  "MMGSDI session open failure as cnf_ptr is NULL", 0, 0, 0);
    return;
  }

  ssn_open_ptr = &(cnf_ptr->session_open_cnf);
  
  /*-------------------------------------------------------------------------
    Copy the session ID into a variable local to the file if valid
  -------------------------------------------------------------------------*/
  if ((MMGSDI_SESSION_OPEN_CNF != cnf) 
       && (MMGSDI_SESSION_OPEN_CNF 
           != ssn_open_ptr->response_header.response_type))
  {
    LOG_MSG_ERROR("ps_utils_aka_mmgsdi_session_open_cb(): "
                  "Improper command confirmation cnf %d response_type %d", 
                  cnf, ssn_open_ptr->response_header.response_type, 0);
    return;
  }

  if ((MMGSDI_SUCCESS != status) 
       && (MMGSDI_SUCCESS != ssn_open_ptr->response_header.mmgsdi_status))
  {
    LOG_MSG_ERROR("ps_utils_aka_mmgsdi_session_open_cb(): "
                  "Session open cb failure status %d and mmgsdi_status %d ", 
                  status, ssn_open_ptr->response_header.mmgsdi_status, 0);
    return;
  }

  aka_cb.mmgsdi_info.session_info.session_id = ssn_open_ptr->session_id;
  /*-----------------------------------------------------------------------
    Set session active only for ISIM, in case of USIM it will be set when 
    SUBSCRIPTION_READY event is obtained
  -------------------------------------------------------------------------*/
  if (TRUE == isim_present)
  {
    aka_cb.mmgsdi_info.session_info.session_active = TRUE;
  }

  return;
} /* ps_utils_aka_mmgsdi_session_open_cb() */

/*===========================================================================
FUNCTION PS_UTILS_AKA_MMGSDI_EVT_CB

DESCRIPTION  Function registered with the MMGSDI events, needed in order to know
            the state of ISIM.

DEPENDENCIES  None.

PARAMETERS  event - the event information

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ps_utils_aka_mmgsdi_evt_cb
(
  const mmgsdi_event_data_type *event
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_aka_mmgsdi_evt_cb() called", 0, 0, 0);

  /*-----------------------------------------------------------------------
    Sanity Check
  -------------------------------------------------------------------------*/
  if ( NULL == event )
  {
    LOG_MSG_ERROR("ps_utils_aka_mmgsdi_evt_cb(): "
                  "Input parameter is NULL", 0, 0, 0);
    return;
  }
  /*-----------------------------------------------------------------------
    For now only interested in CARD_INSERTED
  -------------------------------------------------------------------------*/
  switch ( event->evt )
  {
    case MMGSDI_CARD_INSERTED_EVT:

      if ( MMGSDI_SLOT_1 != event->data.card_inserted.slot )
      {
        LOG_MSG_ERROR("Ignoring card insert event for slot %d",
                        event->data.card_inserted.slot,0,0);
        return;
      }
      ps_utils_aka_sub_cmd = PS_UTILS_AKA_MMGSDI_CARD_INS_EVT;
      ps_send_cmd (PS_UTILS_AKA_CMD, NULL);
      break;

    default:
      LOG_MSG_INFO2("ps_utils_aka_mmgsdi_evt_cb(): "
                    "Received event : %d", event->evt, 0, 0);
      break;
  }
 
  return;
}/*ps_utils_aka_mmgsdi_evt_cb()*/

/*===========================================================================
FUNCTION PS_UTILS_AKA_MMGSDI_SESSION_REFRESH_COMP_CB()

DESCRIPTION  Callback for MMGSDI session refresh complete event

PARAMETERS   status       - success or failure 
             cnf, cnf_ptr - type & result being retrieved 

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ps_utils_aka_mmgsdi_session_refresh_comp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
)
{
  LOG_MSG_INFO2 ("ps_utils_aka_mmgsdi_session_refresh_comp_cb(): "
                 "MMGSDI response call back for cnf=%d with status=%d",
                 cnf, status,0);
  return;
}/* ps_utils_aka_mmgsdi_session_refresh_comp_cb */

/*===========================================================================
FUNCTION PS_UTILS_AKA_MMGSDI_SESSION_EVT_CB()

DESCRIPTION  Callback for MMGSDI GW session related events

PARAMETERS  event : Pointer to the structure conatining info about the event

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ps_utils_aka_mmgsdi_session_evt_cb
(
  const mmgsdi_event_data_type *event
)
{
  if (NULL == event)
  {
    LOG_MSG_ERROR("ps_utils_aka_mmgsdi_session_evt_cb(): "
                  "Event pointer is NULL", 0, 0, 0);
    return;
  }

  switch (event->evt)
  {
    case MMGSDI_SUBSCRIPTION_READY_EVT:
      LOG_MSG_INFO2("ps_utils_aka_mmgsdi_session_evt_cb(): "
                    "MMGSDI_SUBSCRIPTION_READY_EVT for client_id %d", 
                    event->client_id, 0, 0);
      /*-----------------------------------------------------------------------
        Copy the App info based on GW or 1X session
      -----------------------------------------------------------------------*/
      if (MMGSDI_APP_USIM 
          == event->data.subscription_ready.app_info.app_data.app_type)
      {
        aka_cb.mmgsdi_info.session_info.session_active = TRUE;
      }
      
      else
      {
        LOG_MSG_ERROR("ps_utils_aka_mmgsdi_session_evt_cb(): "
                      "Unable to open GW session with USIM", 0, 0, 0);
      }
      break;

    case MMGSDI_SESSION_CLOSE_EVT:
      LOG_MSG_INFO2("ps_utils_aka_mmgsdi_session_evt_cb(): "
                    "MMGSDI_SESSION_CLOSE_EVT for client_id %d", 
                     event->client_id, 0, 0);

      /*-----------------------------------------------------------------------
        Need to set the session_active to false in case of ISIM. The session 
        will be opened again when card inserted event is obtained
      -----------------------------------------------------------------------*/
      if(TRUE == isim_present)
      {
        aka_cb.mmgsdi_info.session_info.session_active = FALSE;
        aka_cb.mmgsdi_info.session_info.session_reopen = TRUE;
        isim_present = FALSE;
      }
      break;

    case MMGSDI_REFRESH_EVT:
      LOG_MSG_INFO2("ps_utils_aka_mmgsdi_session_evt_cb(): "
                    "MMGSDI_REFRESH_EVT for client_id %d, refresh stage %d, " 
                    "mode %d", event->client_id, 
                    event->data.refresh.stage, 
                    event->data.refresh.mode);
      if (MMGSDI_REFRESH_STAGE_START == event->data.refresh.stage)
      {
        mmgsdi_session_refresh_complete(
                                aka_cb.mmgsdi_info.session_info.session_id, 
                                TRUE, 
                                ps_utils_aka_mmgsdi_session_refresh_comp_cb,
                                0);
      }

      else
      {
        LOG_MSG_INFO1 ("ps_utils_aka_mmgsdi_session_evt_cb(): "
                       "Invalid refresh stage %d", 
                       event->data.refresh.stage, 0, 0);
      }
      break;

    default:
      LOG_MSG_INFO2("ps_utils_aka_mmgsdi_session_evt_cb(): "
                    "Ignoring MMGSDI session event %d for client_id %d", 
                    event->evt, event->client_id, 0);
      break;
  }

  return;
} /* ps_utils_aka_mmgsdi_session_evt_cb() */

/*===========================================================================
FUNCTION PS_UTILS_AKA_MMGSDI_SESSION_REG_ISIM()

DESCRIPTION  To register the ISIM MMGSDI session

PARAMETERS None

RETURN VALUE  None

DEPENDENCIES  MMGSDI client ID should be available

SIDE EFFECTS  None
===========================================================================*/
static void ps_utils_aka_mmgsdi_session_reg_isim
(
  void
)
{
  mmgsdi_return_enum_type         status;
  mmgsdi_non_prov_app_info_type   app_info;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_aka_mmgsdi_session_reg_isim() called", 0, 0, 0);

  /*-----------------------------------------------------------------------
    Copy the ISIM APP data
  -----------------------------------------------------------------------*/
  memset (&app_info, 0, sizeof(mmgsdi_non_prov_app_info_type));
  app_info.app_id_data.data_len = aka_cb.mmgsdi_info.session_info.aid.data_len;
  app_info.app_id_data.data_ptr = aka_cb.mmgsdi_info.session_info.aid.data_ptr;
  
  /*-----------------------------------------------------------------------
    open non provisioning mmgsdi session with ISIM APP
  -----------------------------------------------------------------------*/
  status = mmgsdi_session_open(aka_cb.mmgsdi_info.client_id,
                               aka_cb.mmgsdi_info.session_info.session_type, 
                               &app_info, 
                               ps_utils_aka_mmgsdi_session_evt_cb,
                               FALSE, 
                               ps_utils_aka_mmgsdi_session_open_cb, 
                               (mmgsdi_client_data_type) 0);

  if (MMGSDI_SUCCESS != status)
  {
    LOG_MSG_ERROR("ps_utils_aka_mmgsdi_session_reg_isim(): "
                  "MMGSDI ISIM session open failed, %d", status, 0, 0);
  }

  return;
} /* ps_utils_aka_mmgsdi_session_reg_isim() */

/*===========================================================================
FUNCTION PS_UTILS_AKA_MMGSDI_SESSION_REG_USIM()

DESCRIPTION  To register the USIM MMGSDI session

PARAMETERS  None

RETURN VALUE  None

DEPENDENCIES  MMGSDI client ID should be available

SIDE EFFECTS  None
===========================================================================*/
static void ps_utils_aka_mmgsdi_session_reg_usim
(
  void
)
{
  mmgsdi_return_enum_type         status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_aka_mmgsdi_session_reg_usim() called", 0, 0, 0);

  /*-----------------------------------------------------------------------
    Set session type tp GW
  -----------------------------------------------------------------------*/
  aka_cb.mmgsdi_info.session_info.session_type = MMGSDI_GW_PROV_PRI_SESSION;
  
  /*-----------------------------------------------------------------------
    open mmgsdi 1x session
  -----------------------------------------------------------------------*/
  status = mmgsdi_session_open(aka_cb.mmgsdi_info.client_id,
                               MMGSDI_GW_PROV_PRI_SESSION, 
                               NULL, 
                               ps_utils_aka_mmgsdi_session_evt_cb,
                               FALSE, 
                               ps_utils_aka_mmgsdi_session_open_cb, 
                               (mmgsdi_client_data_type) 0);

  if (MMGSDI_SUCCESS != status)
  {
    LOG_MSG_ERROR("ps_utils_aka_mmgsdi_session_reg_usim(): "
                  "MMGSDI ISIM session open failed, %d", status, 0, 0);
  }

  return;
} /* ps_utils_aka_mmgsdi_session_reg_usim() */

/*===========================================================================
FUNCTION PS_UTILS_AKA_ID_REG_CB

DESCRIPTION  CB function registered with mmgsdi to obtain client id.

DEPENDENCIES  None

PARAMETERS  status - success or failure
            cnf - type of result being retrieved
            cnf_ptr - the result being retrieved

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ps_utils_aka_id_reg_cb
(
  mmgsdi_return_enum_type  status,
  mmgsdi_cnf_enum_type     cnf,
  const mmgsdi_cnf_type   *cnf_ptr
)
{
  mmgsdi_client_id_and_evt_reg_cnf_type  *client_reg_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( NULL == cnf_ptr )
  {
    LOG_MSG_ERROR("ps_utils_aka_id_reg_cb(): "
                  "MMGSDI Client ID Reg failure as cnf_ptr is NULL", 0, 0, 0);
    return ;
  }

  client_reg_ptr = (mmgsdi_client_id_and_evt_reg_cnf_type *) 
                    &(cnf_ptr->client_id_and_evt_reg_cnf);

  LOG_MSG_INFO2("ps_utils_aka_id_reg_cb(): "
                "MMGSDI ID & Event Registration Callback", 0, 0, 0);

  /*-----------------------------------------------------------------------
   Copy the client id if registration was successful 
  -------------------------------------------------------------------------*/
  if ((MMGSDI_CLIENT_ID_AND_EVT_REG_CNF == cnf) 
       && (MMGSDI_CLIENT_ID_AND_EVT_REG_CNF 
           == client_reg_ptr->response_header.response_type))
  {
    if ((MMGSDI_SUCCESS == status) 
        && (MMGSDI_SUCCESS == client_reg_ptr->response_header.mmgsdi_status))
    {
      aka_cb.mmgsdi_info.client_id 
                    = client_reg_ptr->response_header.client_id;
    }

    else
    {
      LOG_MSG_ERROR("ps_utils_aka_id_reg_cb(): "
                    "MMGSDI-CLIENT ID Reg failure status %d "
                    "and mmgsdi_status %d are ", status, 
                    client_reg_ptr->response_header.mmgsdi_status, 0);
    }
  }

  else
  {
    LOG_MSG_ERROR("ps_utils_aka_id_reg_cb(): "
                  "Improper command confirmation cnf %d response_type %d", 
                  cnf, client_reg_ptr->response_header.response_type, 0);
  }

  return;
}/* ps_utils_aka_id_reg_cb() */

/*===========================================================================
FUNCTION PS_UTILS_AKA_REG_MMGSDI_CLIENT_ID()

DESCRIPTION
  This function is used to regsiter a client id for the authentication 
  mechanism that uses the mmgsdi module. As a part of the authentication 
  procedure the MMGDSI library is called to register a client id.

RETURN VALUE  None

DEPENDENCIES None

SIDE EFFECTS  None
===========================================================================*/
void ps_utils_aka_reg_mmgsdi_client_id
(
  void
)
{
  mmgsdi_return_enum_type      retval;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  LOG_MSG_INFO2("ps_utils_aka_reg_mmgsdi_client_id() called", 0, 0, 0);

  (void) ps_set_cmd_handler(PS_UTILS_AKA_CMD,
                            ps_utils_aka_cmd_hdlr);

  retval = mmgsdi_client_id_and_evt_reg ( ps_utils_aka_mmgsdi_evt_cb,
                                          ps_utils_aka_id_reg_cb,
                                          (mmgsdi_client_data_type) 0);
  if(MMGSDI_SUCCESS != retval) 
  {
    LOG_MSG_ERROR("ps_utils_aka_reg_mmgsdi_client_id(): "
                  "mmgsdi_client_id_and_evt_reg() error = %d", retval, 0, 0);
  }

  return;
} /* ps_utils_aka_reg_mmgsdi_client_id() */
#endif /* FEATURE_MMGSDI_SESSION_LIB */

/*===========================================================================
FUNCTION      PS_UTILS_AKA_PRASE_DATA

DESCRIPTION   Reads and parses a record from the passed EFS file. The 
              input is considerd to be 64 bit and in hex format

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
int ps_utils_aka_parse_data
(
  char *from,
  char *to, 
  uint64 *data_read
)
{
 
  int count =0; /* to make sure that it is 64 bit maximum */
  uint8 value = 0;
  uint64 temp = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ((NULL == from) || (NULL == to) || ((NULL == data_read)))
  {
    LOG_MSG_ERROR ("ps_utils_aka_parse_data(): "
                   "Input parameters are NULL", 0, 0, 0);
    return -1;
  }

  *data_read = 0;
  memset(&temp,0,8);

  while(from != to)
  {
    if(count >=16) /* can not exceed 8 bytes (16 nibesls */
    {
      LOG_MSG_INFO2 ("ps_utils_aka_parse_data(): "
                     "Invalid value. Abort", 0, 0, 0);
      return -1;
    }
    
    switch(*from)
    {
      case '0':
        value = 0;
        break;
      case '1':
        value = 1;
        break;
      case '2':
        value = 2;
        break;
      case '3':
        value = 3;
        break;
      case '4':
        value = 4;
        break;
      case '5':
        value = 5;
        break;
      case '6':
        value = 6;
        break;
      case '7':
        value = 7;
        break;
      case '8':
        value = 8;
        break;
      case '9':
        value = 9;
        break;
      case 'A':
      case 'a':
        value = 10;
        break;
      case 'B':
      case 'b':
        value = 11;
        break;
      case 'C':
      case 'c':
        value = 12;
        break;
      case 'D':
      case 'd':
        value = 13;
        break;
      case 'E':
      case 'e':
        value = 14;
        break;
      case 'F':
      case 'f':
        value = 15;
        break;
      default:
        LOG_MSG_INFO1("ps_utils_aka_parse_data(): "
                      "Invalid value. Abort", 0, 0, 0);
        return -1;
    }
    temp = temp * 16;
    temp = temp + value;

    count++;   
    from++;
  }
  *data_read = temp;
  return 0; /* success */
}/* ps_utils_aka_prase_data()*/

/*===========================================================================
FUNCTION      PS_UTILS_AKA_READ_RECORD_FROM_EFS

DESCRIPTION   Reads and parses a record from the passed EFS file.

DEPENDENCIES  file_handle must point to a valid file in the EFS that has 
              been opened for READ access by the calling function.

RETURN VALUE  If successfully able to read the record from EFS and populate
              the database structure, returns 0 and *err_num is set to 
              E_SUCCESS.

              If an error is encountered while reading from the EFS or
              while parsing, returns -1 and *err_num is set to one of

              E_EOF           End of file reached
              E_INVALID_ARG   I/O parameter is invalid
              E_BAD_DATA      Parse error in the EFS text record
              E_OVERFLOW      Parsing exceed the max record size


SIDE EFFECTS  If successfully able to read and parse the text record from
              the EFS file, the file position in the EFS is set to the
              beginning of the next record. If a parse error is encountered,
              the file position is undefined.
===========================================================================*/
int ps_utils_aka_read_record_from_efs
(
  ps_efs_token_type          *efs_db_ptr,
  sint15                     *err_num,
  ps_utils_aka_isn_info_type *aka_isn_info_ptr
)
{
  uint64 data_read; /* 64 bit value stored */
  char *from, *to;
  ps_efs_token_parse_status_enum_type ret_val = PS_EFS_TOKEN_PARSE_SUCCESS;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Sanity checks on input parameters
  -------------------------------------------------------------------------*/
  if(NULL == err_num)
  {
    LOG_MSG_ERROR("ps_utils_aka_read_record_from_efs(): "
                  "Input parameters are NULL!", 0, 0, 0);
    return -1;
  }

  if((NULL == efs_db_ptr) || (NULL == aka_isn_info_ptr))
  {
    LOG_MSG_ERROR("ps_utils_aka_read_record_from_efs(): "
                  "Input parameters are NULL!", 0, 0, 0);
    *err_num = (sint15) E_INVALID_ARG;
    return -1;
  }

  *err_num = (sint15) E_FAILURE;
  aka_isn_info_ptr->aka_algo_seqnum_arr_elements = 0;

  /*-------------------------------------------------------------------------
    Parse the record tokens from the EFS file and assign them to the 
    appropriate record field. If there is a parse error, this will set the
    err_num and return. If the record is parsed successfully, set the database
    entry valid flag to true. The tokens in a record may be separated either
    by '\n' or by ';'
  -------------------------------------------------------------------------*/  
  while (PS_EFS_TOKEN_PARSE_EOL 
          != (ret_val = ps_efs_tokenizer(efs_db_ptr, &from, &to )))
  {
    /*------------------------------------------------------------------------
      Token being read. from points to the beginning of the token and 
      to point to the end of the token.

      The tokenizer automatically skips blank lines and comments (lines 
      beginning with #, so no need to check for them here).
    ------------------------------------------------------------------------*/
    if((from == to) || (PS_EFS_TOKEN_PARSE_EOL == ret_val))
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      continue;
    }     
    else if(PS_EFS_TOKEN_PARSE_SUCCESS == ret_val)
    {
      if(ps_utils_aka_parse_data(from, to, &data_read) == -1)
      {
        LOG_MSG_ERROR("ps_utils_aka_read_record_from_efs(): "
                      "Error in reading. Abort", 0, 0, 0);
        *err_num = (sint15) E_FAILURE;
        return -1;
      }
      else
      {
        if(aka_isn_info_ptr->aka_algo_seqnum_arr_elements 
                                      >= PS_UTILS_AKA_SEQ_NUM_ARRAY_ELEMENTS)
        {
          LOG_MSG_ERROR("ps_utils_aka_read_record_from_efs(): "
                        "Too many elements to parse. Array overflow", 0, 0, 0);
          *err_num = (sint15) E_FAILURE;
          return -1;
        }

        aka_isn_info_ptr->aka_algo_seqnum_arr[
              aka_isn_info_ptr->aka_algo_seqnum_arr_elements] = data_read;
        aka_isn_info_ptr->aka_algo_seqnum_arr_elements++;
      }
    }
    else
    {
      ASSERT(PS_EFS_TOKEN_PARSE_FAILURE == ret_val);
      *err_num = (sint15) E_FAILURE;
      return -1;
    }
  } /* Big while Parsed the file*/

  LOG_MSG_INFO1("ps_utils_aka_read_record_from_efs(): "
                "Done Parsing record EFS: Arry size %d",
                aka_isn_info_ptr->aka_algo_seqnum_arr_elements, 0, 0);
  *err_num = (sint15) E_EOF;
  return -1;
}

/*===========================================================================
FUNCTION       PS_UTILS_AKA_READ_ISN_FROM_EFS

DESCRIPTION    This funcation reads the EFS data base
               
DEPENDENCIES    1. The input parameter of the function
                2. NV read

RETURN VALUE   E_SUCCESS - Sucess
               E_FAILURE - Failure
===========================================================================*/
sint15 ps_utils_aka_read_isn_from_efs
(
  ps_utils_aka_isn_info_type *aka_isn_info_ptr
)
{
  ps_efs_token_type  efs_db;
  sint15  err_num;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(NULL == aka_isn_info_ptr)
  {
    LOG_MSG_INFO1("ps_utils_aka_read_isn_from_efs(): "
                  "Input parameters are NULL!", 0, 0, 0);
    return (sint15) E_FAILURE;
  }

  if(ps_efs_file_init(PS_UTILS_AKA_ISN_EFS_FILE_NAME, &efs_db) == -1)
  {
    LOG_MSG_INFO1("ps_utils_aka_read_isn_from_efs(): "
                  "Error opening file ISN from EFS", 0, 0, 0);
    return (sint15) E_FAILURE;
  }

  /*-------------------------------------------------------------------------
    Read each record from EFS. 
    If the EFS file is too small, this will result in returning E_SUCCESS
    but only some of the entries in the database will be valid.
    If there is a parse error this will return E_FAILURE.
  -------------------------------------------------------------------------*/
  if(ps_utils_aka_read_record_from_efs( &efs_db, 
                                        &err_num, 
                                        aka_isn_info_ptr ) == -1)
  {
    ps_efs_file_close(&efs_db);
    if(err_num != (sint15) E_EOF)
    {
      LOG_MSG_INFO1("ps_utils_aka_read_isn_from_efs(): "
                    "Error %d parsing record from EFS.Aborting", 
                    err_num, 0, 0);
      return (sint15) E_FAILURE;
    }
    else
    {
      LOG_MSG_INFO1("ps_utils_aka_read_isn_from_efs(): "
                    "EOF reached reading record from EFS", 0, 0, 0);
      return (sint15) E_SUCCESS;
    }
  }

  ps_efs_file_close(&efs_db);
  LOG_MSG_INFO1("ps_utils_aka_read_isn_from_efs(): "
                "EFS file read successfully", 0, 0, 0);

  return (sint15) E_SUCCESS;
} /* ps_utils_aka_read_isn_from_efs() */

//#endif /* FEATURE_DATA_PS */
