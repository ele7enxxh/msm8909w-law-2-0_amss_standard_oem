#ifndef DS_AUTH_PLATFORM_H
#define DS_AUTH_PLATFORM_H
/******************************************************************************
  @file    ds_auth_platform.h
  @brief   

  DESCRIPTION
  AKAv2 API platform header for modem and QMI based clients.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ds_auth_platform.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/10   mct     DS Api for AKAv2 algorithm.
===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#include "comdef.h"
#include "ds_auth.h"

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
  uint8*                      rand,
  uint8                       rand_len,
  uint8*                      autn,
  uint8                       autn_len,
  ds_auth_aka_callback_type   aka_algo_cback,
  void                       *user_data,
  sint15                     *ds_errno
);

/*===========================================================================*/
 /**
 FUNCTION:     DS_AUTH_INIT

 @brief        This function is called by the library init. It 
               is the wrapper for the platform intialization function.
 
 @param[in]    None

 @dependencies None
 
 @return       None
 
 @sideeffect   None
 */   
/*===========================================================================*/
void ds_auth_init
(
  void
);

/*===========================================================================*/
 /**
 FUNCTION:     DS_AUTH_PLATFORM_INIT

 @brief        This function is called by the library init. It 
               performs the specific modem or QMI-based API
	       initialization.
 
 @param[in]    None

 @dependencies None
 
 @return       None
 
 @sideeffect   None
 */     
/*===========================================================================*/
void ds_auth_platform_init
(
  void
);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* DS_AUTH_PLATFORM_H */

