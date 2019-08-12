#ifndef SSM_IPC_V_H
#define SSM_IPC_V_H
/**
@file ssm_ipc.h
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
  Copyright (c) 2011 - 2013 by QUALCOMM Technologies Incorporated. 
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

$Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_ipc_v.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/29/13   din     Changes pertaining to ADSP were done.
07/20/12   din     Removed key exchange code from SSM.
09/03/11   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "ssm_v.h"


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
/** Various type of attributes .
*/


#define SSM_SINGLE_MESSAGE_CODE_LENGTH        10
#define SSM_SINGLE_MESSAGE_WITH_DELIM_LENGTH  (SSM_SINGLE_MESSAGE_CODE_LENGTH+1)
#define SSM_IPC_ENC_ADD_BUFFER_LENGTH         100

#define SSM_RAW_IPC_HEADER_LENGTH        (4*SSM_SINGLE_MESSAGE_WITH_DELIM_LENGTH)
#define SSM_RAW_IPC_HEADER_LENGTH_APPS   (2*SSM_SINGLE_MESSAGE_WITH_DELIM_LENGTH)
#define SSM_ENC_IPC_HEADER_LENGTH        SSM_SINGLE_MESSAGE_WITH_DELIM_LENGTH
#define SSM_MTOP_PERM_FILE_BUFFER_LENGTH (3*SSM_SINGLE_MESSAGE_WITH_DELIM_LENGTH)

/** @addtogroup ssm
@{ */

typedef enum ssm_ipc_req
{
   SSM_IPC_MIN = 0x0000AAAB,
   SSM_ATOM_MODE_UPDATE,              /* Send encypted Mode to modem */
   SSM_MTOP_MODE_UPDATE,              /* Send encypted Mode to peripheral */
   SSM_PTOM_MODE_UPDATE_STATUS,       /* Updates status of mode updates to Modem processor */
   SSM_MTOA_MODE_UPDATE_STATUS,       /* Updates status of mode updates to Apps processor */
   SSM_PTOM_REQ_PERM_FILE,            /* Request perm file from modem processor */
   SSM_MTOP_PERM_FILE,                /* Send perm file information from modem  to peripheral */ 
   SSM_PTOM_PERM_FILE_STATUS,         /* Send perm file exchange status from peripheral to modem */ 
   SSM_IPC_MAX,
   SSM_IPC_UINT32_MAX = 0xFFFFFFFF   
} ssm_ipc_req_t ;

/* Following typedefs are used in breaking IPC message */
typedef enum 
{
  SSM_TYPE_UINT32,
  SSM_TYPE_BOOLEAN,
  SSM_TYPE_STRING,
  SSM_TYPE_UINT8_MAX = 0xff
} ssm_types_decode_info_enum ;

typedef union
{
  uint32          uint32_val;
  boolean         boolean_val;
  char *          string_val;
} ssm_types_decode_union;

typedef struct 
{
  ssm_types_decode_info_enum   type;
  ssm_types_decode_union       val;
} ssm_types_decode_info;


/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/** 

A common function to encode and send ipcs.

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
);


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
);

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
);

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
);

/** @} */ /* end_addtogroup ssm */

#endif /* SSM_IPC_V_H*/
