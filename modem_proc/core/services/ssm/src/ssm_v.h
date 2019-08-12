/**
@file ssm_v.h
@brief
This module implements the Secured Services Functionality.
*/
#ifndef SSM_V_H
#define SSM_V_H
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
  Provides SSM headers and other usful data_types

EXTERNALIZED FUNCTIONS
  ssm_client_init_func
    Register a client with SSM

  ssm_get_perm
    Gets permission for a particular set of attributes

INITIALIZATION AND SEQUENCING REQUIREMENTS

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_v.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/02/14   psu     Update diag macros using optimized versions.
04/29/13   din     Changes pertaining to ADSP were done.
08/24/12   din     Removed warnings pertaining to insertion of task.h
08/02/12   din     Updated SSM messages with F3 SSID change.
09/03/11   din     File created.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "ssm.h"
#include "msg.h"
#include "err.h"


/*=============================================================================

                        MACRO DEFINITIONS

=============================================================================*/

#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
  #define SSM_PROC_CURRENT  SSM_PROC_MODEM
  #define FEATURE_SSM_REX_API_PRESENT
  #define SSM_DIAG_F3_MESSAGE_ALLOW
#elif FEATURE_SSM_ADSP_PROC
  #define SSM_PROC_CURRENT  SSM_PROC_ADSP
#elif FEATURE_SSM_RIVA_PROC
  #define SSM_PROC_CURRENT  SSM_PROC_RIVA
#endif

#ifdef SSM_DIAG_F3_MESSAGE_ALLOW
#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC
#define SSM_MSG_LOW( str)    \
        MSG( MSG_SSID_SSM, MSG_LEGACY_LOW , "Modem:" str)
#define SSM_MSG_LOW_1( str, a)    \
        MSG_1( MSG_SSID_SSM, MSG_LEGACY_LOW , "Modem:" str, a)
#define SSM_MSG_LOW_2( str, a, b)    \
        MSG_2  ( MSG_SSID_SSM, MSG_LEGACY_LOW , "Modem:" str, a,b )
#define SSM_MSG_LOW_3( str, a, b, c )    \
        MSG_3  ( MSG_SSID_SSM, MSG_LEGACY_LOW , "Modem:" str, a, b, c )

#define SSM_MSG_MED( str)    \
        MSG( MSG_SSID_SSM, MSG_LEGACY_MED , "Modem:" str)
#define SSM_MSG_MED_1( str, a)    \
        MSG_1( MSG_SSID_SSM, MSG_LEGACY_MED , "Modem:" str, a)
#define SSM_MSG_MED_2( str, a, b)    \
        MSG_2  ( MSG_SSID_SSM, MSG_LEGACY_MED , "Modem:" str, a,b )
#define SSM_MSG_MED_3( str, a, b, c )    \
        MSG_3  ( MSG_SSID_SSM, MSG_LEGACY_MED , "Modem:" str, a, b, c )

#define SSM_MSG_HIGH( str)    \
        MSG( MSG_SSID_SSM, MSG_LEGACY_HIGH , "Modem:" str)
#define SSM_MSG_HIGH_1( str, a)    \
        MSG_1( MSG_SSID_SSM, MSG_LEGACY_HIGH , "Modem:" str, a)
#define SSM_MSG_HIGH_2( str, a, b)    \
        MSG_2  ( MSG_SSID_SSM, MSG_LEGACY_HIGH , "Modem:" str, a,b )
#define SSM_MSG_HIGH_3( str, a, b, c )    \
        MSG_3  ( MSG_SSID_SSM, MSG_LEGACY_HIGH , "Modem:" str, a, b, c )

#define SSM_MSG_ERROR( str)    \
        MSG( MSG_SSID_SSM, MSG_LEGACY_ERROR , "Modem:" str)
#define SSM_MSG_ERROR_1( str, a)    \
        MSG_1( MSG_SSID_SSM, MSG_LEGACY_ERROR , "Modem:" str, a)
#define SSM_MSG_ERROR_2( str, a, b)    \
        MSG_2  ( MSG_SSID_SSM, MSG_LEGACY_ERROR , "Modem:" str, a,b )
#define SSM_MSG_ERROR_3( str, a, b, c )    \
        MSG_3  ( MSG_SSID_SSM, MSG_LEGACY_ERROR , "Modem:" str, a, b, c )

#else /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

#define SSM_MSG_LOW( str)    \
        MSG( MSG_SSID_SSM, MSG_LEGACY_LOW , "ADSP:" str)
#define SSM_MSG_LOW_1( str, a)    \
        MSG_1( MSG_SSID_SSM, MSG_LEGACY_LOW , "ADSP:" str, a)
#define SSM_MSG_LOW_2( str, a, b)    \
        MSG_2  ( MSG_SSID_SSM, MSG_LEGACY_LOW , "ADSP:" str, a,b )
#define SSM_MSG_LOW_3( str, a, b, c )    \
        MSG_3  ( MSG_SSID_SSM, MSG_LEGACY_LOW , "ADSP:" str, a, b, c )

#define SSM_MSG_MED( str)    \
        MSG( MSG_SSID_SSM, MSG_LEGACY_MED , "ADSP:" str)
#define SSM_MSG_MED_1( str, a)    \
        MSG_1( MSG_SSID_SSM, MSG_LEGACY_MED , "ADSP:" str, a)
#define SSM_MSG_MED_2( str, a, b)    \
        MSG_2  ( MSG_SSID_SSM, MSG_LEGACY_MED , "ADSP:" str, a,b )
#define SSM_MSG_MED_3( str, a, b, c )    \
        MSG_3  ( MSG_SSID_SSM, MSG_LEGACY_MED , "ADSP:" str, a, b, c )

#define SSM_MSG_HIGH( str)    \
        MSG( MSG_SSID_SSM, MSG_LEGACY_HIGH , "ADSP:" str)
#define SSM_MSG_HIGH_1( str, a)    \
        MSG_1( MSG_SSID_SSM, MSG_LEGACY_HIGH , "ADSP:" str, a)
#define SSM_MSG_HIGH_2( str, a, b)    \
        MSG_2  ( MSG_SSID_SSM, MSG_LEGACY_HIGH , "ADSP:" str, a,b )
#define SSM_MSG_HIGH_3( str, a, b, c )    \
        MSG_3  ( MSG_SSID_SSM, MSG_LEGACY_HIGH , "ADSP:" str, a, b, c )

#define SSM_MSG_ERROR( str)    \
        MSG( MSG_SSID_SSM, MSG_LEGACY_ERROR , "ADSP:" str)
#define SSM_MSG_ERROR_1( str, a)    \
        MSG_1( MSG_SSID_SSM, MSG_LEGACY_ERROR , "ADSP:" str, a)
#define SSM_MSG_ERROR_2( str, a, b)    \
        MSG_2  ( MSG_SSID_SSM, MSG_LEGACY_ERROR , "ADSP:" str, a,b )
#define SSM_MSG_ERROR_3( str, a, b, c )    \
        MSG_3  ( MSG_SSID_SSM, MSG_LEGACY_ERROR , "ADSP:" str, a, b, c )


#endif /* FEATURE_SSM_PERIPHERAL_SERVER_PROC */

#define SSM_ERR_FATAL( str, a, b, c )     ERR_FATAL( " *** SSM *** " str, a, b, c )

#else  /* SSM_DIAG_F3_MESSAGE_ALLOW */

#define SSM_MSG_LOW( str)
#define SSM_MSG_LOW_1( str, a)
#define SSM_MSG_LOW_2( str, a, b)
#define SSM_MSG_LOW_3( str, a, b, c )

#define SSM_MSG_MED( str)
#define SSM_MSG_MED_1( str, a)
#define SSM_MSG_MED_2( str, a, b)
#define SSM_MSG_MED_3( str, a, b, c )

#define SSM_MSG_HIGH( str)
#define SSM_MSG_HIGH_1( str, a)
#define SSM_MSG_HIGH_2( str, a, b)
#define SSM_MSG_HIGH_3( str, a, b, c )

#define SSM_MSG_ERROR( str)
#define SSM_MSG_ERROR_1( str, a)
#define SSM_MSG_ERROR_2( str, a, b)
#define SSM_MSG_ERROR_3( str, a, b, c )

#endif /* SSM_DIAG_F3_MESSAGE_ALLOW */




/** SSM Proc enum  */
typedef enum 
{
  /** Indicates Modem Proc  */
  SSM_PROC_MODEM = 0,

  /** Indicates APPS Proc  */
  SSM_PROC_APPS,

  /** Indicates ADSP Proc  */
  SSM_PROC_ADSP,

  /** Indicates RIVA Proc  */
  SSM_PROC_RIVA,

  /** MAX PROC */
  SSM_PROC_MAX,

  SSM_PROC_UINT32_MAX = 0xFFFFFFFF

} ssm_proc_enum_t ;






/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/


/** 
Reinitialize ssm clients to read from the new file

@param[in] 
None

@return
boolean TRUE - if success
        FALSE - if fails

@dependencies
None.
*/


void ssm_update_mode_thru_ssm_task(void);

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
);


/**
  
The function is invoked by SSM task to parse perm file based on queue

@param[in] 
None

@return
None

@dependencies
None.
*/


void ssm_parse_perm_file(void);

/** 
Initializes private static data members of ssm_parser

@param[in] 
None

@return
None

@dependencies
None.
*/

void ssm_parser_init( void );

/**
 
Returns SSM mode

@param[in]
None

@return
uint32         ---           SSM mode

#dependencies
None.

*/

uint32 ssm_get_mode(void);

/** 
Retry SFS read if none of the sfs reads have succeeded

@param[in] 
None

@return
None

@dependencies
None.
*/

void ssm_parser_retry_sfs_read( void );

#ifdef FEATURE_SSM_PERIPHERAL_SERVER_PROC

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
);

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
);

#else

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
);

#endif /* !FEATURE_SSM_PERIPHERAL_SERVER_PROC */


#endif /* SSM_V_H */


/** @} */ /* end_addtogroup ssm */
