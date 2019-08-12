#ifndef __CVA_IF_H__
#define __CVA_IF_H__

/**
  @file  cva_if.h
  @brief This is the public header file that clients of CVA should include.
         This file includes all other CVA public header files and contains
         single entry point into the CVA.
*/

/*
  ============================================================================

   Copyright (C) 2015 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/vadapter/inc/protected/cva_if.h#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------


  ============================================================================
*/


/*----------------------------------------------------------------------------
  Include files for Module
----------------------------------------------------------------------------*/

#include "cva_icommon_if.h"


/*----------------------------------------------------------------------------
  CVA Call function.
----------------------------------------------------------------------------*/

/**
 * Provides a single API entry point into the GSM Voice Adapter.
 *
 * @param[in] event_id event identifier to execute.
 * @param[in] params event parameters.
 * @param[in] size Size of the event parameters in bytes.V
 *
 * @return
 * None
 */
CVA_EXTERNAL uint32_t cva_call (

  uint32_t cmd_id,
  void* params,
  uint32_t size

);

#endif /* __CVA_IF_H__ */

