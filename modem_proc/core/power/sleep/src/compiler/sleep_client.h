#ifndef SLEEP_CLIENT_H
#define SLEEP_CLIENT_H

/*=============================================================================

  FILE:            sleep_client.h
  
  OVERVIEW:        This file contains the definition of a class that
                   enables/disables the modes requested by NPA client for a
                   particular resource.
 
                   Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
                   All Rights Reserved.
                   Qualcomm Confidential and Proprietary

  $Header: //source/qcom/qct/core/power/sleep/rel/1h10/src/common/compiler/offline_synthesis/sleep_client.h

=============================================================================*/

#include "CLpr.h"
#include "npa_resource.h"
#include "CoreVerify.h"
#include "sleep_log.h"
#include "sleep_lpri.h"
#include "SleepLPR_synth_modes.h"

/*
===============================================================================

                             CLASS DEFINITION

===============================================================================
*/

typedef struct SleepClient_s
{
  /* Pointer to the LPR associated with this SleepClient */
  sleep_lpr* lpr_ptr;

  /* Last request made for the LPR through this SleepClient */
  npa_resource_state m_lastRequest;

} SleepClient;
 
/**
 * SleepClient_create
 *
 * @brief Constructor that will create a SleepClient object for input 
 *        sleep_lpr struct.
 *
 * @param lpr: Pointer to sleep_lpr for which we want to create sleep client
 *             that will enable/disable the modes as requested.
 * @param cLpr: Pointer to CLpr for the above sleep_lpr that is registered
 *              with the sleep registry.
 */
extern void SleepClient_create( SleepClient *hClient, sleep_lpr *lpr );

/**
 * SleepClient_destroy
 *
 * @brief Destructor for the SleepClient class.
 */
extern void SleepClient_destroy( SleepClient *hClient );


/**
 * request
 *
 * @brief Enables/disables the modes of corresponding LPR as requested.
 *
 * @param request: Bitmask containing 1's for the modes we want to enable
 *                 and 0's for the modes we want to disable.
 */
extern void SleepClient_request( SleepClient *hClient, npa_resource_state request);


#endif /* !SLEEP_CLIENT_H */
