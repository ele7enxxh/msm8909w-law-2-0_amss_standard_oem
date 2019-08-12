#ifndef __CVA_ICOMMON_IF_H__
#define __CVA_ICOMMON_IF_H__

/**
  @file  iva_icommon_if.h
  @brief This file contains common interface definitions of the IMS Voice
         Adapter.
*/

/*
  ============================================================================

   Copyright (C) 2015 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/vadapter/inc/protected/cva_icommon_if.h#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------

  ============================================================================
*/


/*----------------------------------------------------------------------------
  Include files for Module
----------------------------------------------------------------------------*/

#include "mmdefs.h"


/*----------------------------------------------------------------------------
  Export Macros
----------------------------------------------------------------------------*/
#ifdef __cplusplus
  #define CVA_EXTERNAL extern "C"
#else
  #define CVA_EXTERNAL extern
#endif /* __cplusplus */
  /* This marks an external API that is intended for public use. */

#ifdef __cplusplus
  #define CVA_INTERNAL extern "C"
#else
  #define CVA_INTERNAL extern
#endif /* __cplusplus */
  /* This marks an internal API that is intended for internal use. */


/****************************************************************************
 * GSM VOICE ADAPTER COMMON DEFINITION                                      *
 ****************************************************************************/

/**
 * Event callback provided by a client of the server.
 *
 * The client returns APR_EOK from the callback when it was successful in
 * queuing the event to its own thread context for processing.
 *
 * All other error codes indicate a problem.
 *
 * The event parameter pointer is valid only within the callback context.
 */
typedef uint32_t ( *cva_icommon_event_callback_fn_t ) (

  void* session_context,
    /**< Client-provided session_context pointer from the OPEN command. */

  uint32_t event_id,
    /**< Event ID. See CVA_IXXX_EVENT_YYY. */

  void* params,
    /**< Pointer to an associated event data structure. */

  uint32_t size
    /**< Size of the event data structure. */

);

#endif /* __CVA_ICOMMON_IF_H__ */

