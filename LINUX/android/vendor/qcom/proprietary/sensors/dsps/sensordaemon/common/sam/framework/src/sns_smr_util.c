#define __SNS_MODULE__ SNS_SMR

#ifndef SNS_SMR_C
#define SNS_SMR_C

/*============================================================================

  @file sns_smr_util.c

  @brief

  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
============================================================================*/

/*============================================================================

                                INCLUDE FILES

============================================================================*/
#include "sns_osa.h"
#include "sns_memmgr.h"
#include "sns_smr_util.h"
#include "sns_common.h"
#include "sns_debug_api.h"
#include "sns_queue.h"
#include "sns_em.h"
#include "qmi_idl_lib.h"
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

/*===========================================================================

                              PREPROCESSOR DEFINITIONS AND CONSTANTS

===========================================================================*/

#define SMR_MAX_MSG_LEN         4096        /* 4KB */
#define SMR_RSVD_HEADER_LEN     64
#define SMR_MAX_BODY_LEN        (SMR_MAX_MSG_LEN - SMR_RSVD_HEADER_LEN) /* The max body length
                                                     allowed for sns_smr_msg_alloc() */
#define SNS_SMR_QMI_CLI_SIG 0x04
#define SNS_SMR_QMI_TIMER_SIG 0x08

/*===========================================================================

                              Function Definitions

===========================================================================*/

/*===========================================================================

  FUNCTION:   sns_smr_set_hdr

===========================================================================*/
/*!
  @brief This function sets message header information with the parameters delivered using
         sns_smr_header_s structure type.
         The address of the message header is calculated from body_ptr.

  @param[i] header_type_ptr: A pointer to the header structure type variable which includes all parameters.
            The message header is identified by body_ptr.
  @param[i] body_ptr: A pointer to the message body allocated by sns_smr_msg_alloc

  @return
   - SNS_SUCCESS if the message header was set successfully.
   - All other values indicate an error has occurred.

*/
/*=========================================================================*/
sns_err_code_e sns_smr_set_hdr(const sns_smr_header_s * header_type_ptr, void * body_ptr)
{
  SNS_OS_MEMCOPY((void*)GET_SMR_MSG_HEADER_PTR(body_ptr), (const void *)header_type_ptr,
         sizeof(sns_smr_header_s));
  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_smr_get_hdr

===========================================================================*/
/*!
  @brief This function gets message header information into sns_smr_header_s structure type.

  @param[o] header_type_ptr: A pointer to the header structure type in which
            the header informaiton will be retrieved
  @param[i] body_ptr: A pointer to the message body allocated by sns_smr_msg_alloc

  @return
   - SNS_SUCCESS if the message header was gotten successfully.
   - All other values indicate an error has occurred.

*/
/*=========================================================================*/
sns_err_code_e sns_smr_get_hdr(sns_smr_header_s * header_type_ptr, const void * body_ptr)
{
  SNS_OS_MEMCOPY((void *)header_type_ptr, (const void*)GET_SMR_MSG_HEADER_PTR(body_ptr),
         sizeof(sns_smr_header_s));
  return SNS_SUCCESS;
}

/*===========================================================================

  FUNCTION:   sns_smr_msg_alloc

===========================================================================*/
/*!
  @brief this function allocates message body and header, and returns the body pointer.

  @param[i] body_size is the message body size to be allocated

  @return
  NULL if failed, or a pointer to the newly allocated message body

*/
/*=========================================================================*/
void * sns_smr_msg_alloc (sns_debug_module_id_e src_module, uint16_t body_size)
{
  UNREFERENCED_PARAMETER(src_module);
  smr_msg_s * msg_ptr;

  if ( SMR_MAX_BODY_LEN < body_size )
  {
    SNS_ASSERT ( false );
  }

  msg_ptr = (smr_msg_s*)
  SNS_OS_MALLOC(src_module, (uint16_t)(SMR_MSG_HEADER_BLK_SIZE + body_size));
  if (msg_ptr != NULL)
  {
    sns_q_link(msg_ptr, &msg_ptr->q_link);
    SNS_OS_MEMSET((void *)msg_ptr->body, 0, body_size);
    return ((void *)msg_ptr->body);
  }
  else
  {
    return NULL;
  }
}

/*===========================================================================

  FUNCTION:   sns_smr_msg_free

===========================================================================*/
/*!
  @brief This function frees the message header and body allocated by sns_smr_msg_alloc().

  @param[i] body_ptr: A pointer variable to the message body to be freed

  @return
   None

*/
/*=========================================================================*/
void sns_smr_msg_free (void * body_ptr)
{
  if( NULL != body_ptr )
  {
    SNS_OS_FREE( GET_SMR_MSG_PTR(body_ptr) );
  }
}

#endif /* SNS_SMR_C */
