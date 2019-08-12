#ifndef __VS_COMMON_H__
#define __VS_COMMON_H__

/**
  @file  vs_common.h
  @brief This file contains common definitions and single API entry point into 
         the Voice Services
*/

/*
  ============================================================================

   Copyright (C) 2012, 2014 Qualcomm Technologies, Inc. 
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc. 

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/vs/vs_common.h#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  12/05/12  sud   Initial revision

  ============================================================================
*/

/*----------------------------------------------------------------------------
  Include files for Module
----------------------------------------------------------------------------*/

#include "mmdefs.h"

#include "vs_errcodes.h"

/*----------------------------------------------------------------------------
  Export Macros
----------------------------------------------------------------------------*/

#ifdef __cplusplus
  #define VS_EXTERNAL extern "C"
#else
  #define VS_EXTERNAL extern
#endif /* __cplusplus */
  /* This marks an external API that is intended for public use. */

#ifdef __cplusplus
  #define VS_INTERNAL extern "C"
#else
  #define VS_INTERNAL extern
#endif /* __cplusplus */
  /* This marks an internal API that is intended for internal use. */


/*----------------------------------------------------------------------------
  Start of Structures
----------------------------------------------------------------------------*/

/** @addtogroup vs_common
@{ */

/**
 * Command response event structure.
 */
#define VS_COMMON_EVENT_CMD_RESPONSE ( 0x00012E23 )

typedef struct vs_common_event_cmd_response_t vs_common_event_cmd_response_t;

/** Common event command response. */
struct vs_common_event_cmd_response_t
{
  uint32_t cmd_id;
    /**< ID of the command that completed. */
  uint32_t status_id;
    /**< Status code returned. See Chapter @xref{hdr:errorCodes}. */
  void* client_context;
    /**< Client context pointer returned from the asynchronous command call. */
};

/**
 * eAMR rate change event structure.
 */
#define VS_COMMOM_EVENT_EAMR_MODE_CHANGE ( 0x000130E1 )	 

typedef struct vs_common_event_eamr_mode_t vs_common_event_eamr_mode_t;

/** eAMR modes.
*/
struct vs_common_event_eamr_mode_t
{
  uint32_t mode;
    /**< 
      * VS_COMMON_EAMR_MODE_NARROWBAND -- 8000 Hz \n
      * VS_COMMON_EAMR_MODE_WIDEBAND -- 16000 Hz 
      */
};

/** eAMR Narrowband mode. */
#define VS_COMMON_EAMR_MODE_NARROWBAND ( 0 )

/** eAMR Wideband mode. */
#define VS_COMMON_EAMR_MODE_WIDEBAND ( 1 )

/**
 * Feature ready event structure. There are no parameters for this event.
 */
#define VS_COMMON_EVENT_READY ( 0x00012E24 )

/**
 * Feature not ready event structure. There are no parameters for this event.
 */
#define VS_COMMON_EVENT_NOT_READY ( 0x00012E25 )

/*----------------------------------------------------------------------------
  Event Callback Function
----------------------------------------------------------------------------*/

/**
 * Event callback provided by a client of the voice driver.
 *
 * The client returns VS_EOK from the callback when the client was succesful
 * in queuing the event to its own thread context for processing.
 *
 * All other error codes (see Chapter @xref{hdr:errorCodes}) indicate a problem.
 *
 * The event parameter pointer is valid only within the callback context.
 */
typedef uint32_t ( *vs_common_event_callback_fn_t ) (
  uint32_t event_id,
    /**< Event ID. See VS_XXX_EVENT_YYY. */
  void* params,
    /**< Pointer to an associated event data structure. */
  uint32_t size,
    /**< Size of the event data structure. */
  void* session_context
    /**< Client-provided session context pointer from the Open command. */
);

/*----------------------------------------------------------------------------
  VS Call function
----------------------------------------------------------------------------*/

/**
 * Provides a single API entry point into the voice driver.
 *
 * @param[in] cmd_id Command identifier to execute.
 * @param[in] params Command parameters.
 * @param[out] size Size of the command parameters in bytes.
 *
 * @return
 *  VS_EOK (0) when successful.
 */
VS_EXTERNAL uint32_t vs_call
(
  uint32_t cmd_id,
  void* params,
  uint32_t size
);

/** @} */ /* end_addtogroup vs_common */

#endif /* __VS_COMMON_H__ */

