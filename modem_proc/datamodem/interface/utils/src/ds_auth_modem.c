#include "datamodem_variation.h"
#ifndef IMAGE_APPS_PROC
/******************************************************************************
  @file    ds_auth_modem.c
  @brief   

  DESCRIPTION
  AKAv2 API implementation for modem based clients.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2010 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
****************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ds_auth_modem.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/10   mct     DS Api for AKAv2 algorithm.
===========================================================================*/

#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "amssassert.h"

#include "ds_Utils_DebugMsg.h"
#include "dserrno.h"
#include "ds_auth.h"
#include "ds_auth_platform.h"
#include "ps_utils_aka.h"

/*===========================================================================*/
 /**
 FUNCTION:     DS_AUTH_PLATFORM_INIT

 @brief        This function is called on the library init. It calls the appropriate
               functions to initialize any necessary modem modules.
 
 @param[in]  none

 @dependencies None
 
 @return      None
 
 @sideeffect  None
 */ 
/*===========================================================================*/
void ds_auth_platform_init()
{
  /* Nothing to initialize */
}

/*===========================================================================*/
 /**
 FUNCTION:      DS_AUTH_PLATFORM_RUN_AKA_ALGO

 @brief        Runs AKAv1 or AKAv2 for the client & returns the result in the 
               callback function provided by the client
 
 @param[in]  aka_ver     AKA handle
 @param[in]  rand        RAND value
 @param[in]  rand_len    RAND length
 @param[in]  autn        AUTN value
 @param[in]  autn_len    AUTN length
 @param[in]  aka_algo_cback  Client callback that is called 
                             when the results are available
 @param[in]  user_data   User data, passed to the client callback function
 @param[in]  ds_errno    Any error codes generated are stored 
                         within this variable.
 
 @dependencies None
 
 @return      AKA handle or Failure
 @retval <0   Failure, check ds_errno for more information.
 @retval >=0  AKA handle
 
 @sideeffect  None
 */
/*===========================================================================*/
ds_auth_aka_handle_type ds_auth_platform_run_aka_algo
(
  ds_auth_aka_ver_enum_type   aka_ver,
  uint8*                      rand_ptr,
  uint8                       rand_len,
  uint8*                      autn_ptr,
  uint8                       autn_len,
  ds_auth_aka_callback_type   aka_algo_cback,
  void                       *user_data,
  sint15                     *ds_errno
)
{
  return ps_utils_run_aka_algo
         (
           (ps_utils_aka_ver_enum_type) aka_ver, 
           rand_ptr, 
           rand_len,
           autn_ptr,
           autn_len,
           aka_algo_cback, 
           user_data
         );
}

#endif /* IMAGE_MODEM_PROC */

