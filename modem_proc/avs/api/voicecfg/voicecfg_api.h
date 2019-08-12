#ifndef __VOICECFG_API_H__
#define __VOICECFG_API_H__

/**
  @file voicecfg_api.h
  @brief This file contains the definitions of the constants,
    	 data structures, and interfaces for voice configuration.
*/

/*
  ============================================================================

   Copyright (C) 2013 QUALCOMM Technologies Incorporated.
   All Rights Reserved.
   QUALCOMM Proprietary and Confidential

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/voicecfg/voicecfg_api.h#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  10/11/13  sre   Initial revision

  ============================================================================
*/

/* 
  NOTE : The client is expected to include the header file apr_errorcodes.h to
  interpret the error codes correctly. 
*/

/*----------------------------------------------------------------------------
  Include files
----------------------------------------------------------------------------*/

#include "apr_comdef.h"
#include "voicecfg_items.h"

/** @addtogroup voicecfg_api
@{ */

/*----------------------------------------------------------------------------
  Core Controls
----------------------------------------------------------------------------*/

/**
  Retrieves a Boolean value from the voice configuration binary corresponding
  to a given item id.

  @par Payload
  #voicecfg_cmd_get_bool_item_t

  @return
  None.

  @dependencies
  None.
*/
#define VOICECFG_CMD_GET_BOOL_ITEM ( 0x00012E5A )

/** @brief VOICECFG_CMD_GET_BOOL_ITEM parameter structure.
*/
typedef struct voicecfg_cmd_get_bool_item_t
{
  uint32_t id;  
    /**< The item ID to retrieve. */
  bool_t ret_value;
    /**< The storage to receive the Boolean item. */
}
  voicecfg_cmd_get_bool_item_t;

/*----------------------------------------------------------------------------
  Core Routines
----------------------------------------------------------------------------*/

/**
 * Provides a single API entry-point into the voice driver.
 *
 * \param[in] cmd_id The command identifier to execute.
 * \param[in] params The command parameters.
 * \param[in] size The size of the command parameters in bytes.
 *
 * \return APR_EOK(0) when successful.
 */
APR_EXTERNAL uint32_t voicecfg_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
);

/** @} */ /* end_addtogroup vociecfg_api */

#endif  /* __VOICECFG_API_H__ */

