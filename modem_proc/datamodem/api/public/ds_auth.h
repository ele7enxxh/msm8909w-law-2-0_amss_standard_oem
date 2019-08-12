#ifndef DS_AUTH_H
#define DS_AUTH_H
/******************************************************************************
  @file    ds_auth.h
  @brief   

  DESCRIPTION
  Authentication and Key Agreement version 2 (AKAv2) API wrapper implementation 
  for modem and Qualcomm Messaging Interface (QMI) based clients. This API is 
  used by clients as a cryptographic challenge/response authentication 
  mechanism.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ds_auth.h#1 $ $DateTime: 2016/12/13 08:00:03 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/10   mct     DS Api for AKAv2 algorithm.
===========================================================================*/

#include "comdef.h"

/*=============================================================================

  TYPE DEFINITIONS

=============================================================================*/
typedef int32 ds_auth_aka_handle_type;

typedef enum 
{
  DS_AUTH_AKA_SUCCESS          = 0,
  DS_AUTH_AKA_SYNC_FAILURE     = 1,
  DS_AUTH_AKA_FAILURE          = 2,
  DS_AUTH_AKA_MAX_STATUS       = 3
} ds_auth_aka_status_enum_type;


/**
* ENUM:      DS_AUTH_AKA_VER_ENUM_TYPE AKA version (currently v1
* or v2)
*/
typedef enum 
{
  DS_AUTH_AKA_V1            = 0,
  DS_AUTH_AKA_V2            = 1,
  DS_AUTH_AKA_MAX_VER       = 2
} ds_auth_aka_ver_enum_type;

/*===========================================================================*/
 /**
 FUNCTION:      DS_AUTH_AKA_CALLBACK_TYPE

 @brief        The client callback function.  The parameter to the
                callback function contains the result of the AKA procedures
                performed as the result of the client's request.
 
 @detail    

  Success case:
                   status   = PS_UTILS_AKA_SUCCESS
                   digest   = PRF (RES|CK|IK)
                   aka_data = RES_LEN + RES + CK_LEN + CK + IK_LEN + IK

     Set the aka_data as specified below
     Bytes:    1        L1         1         L2        1        L3
          ---------------------------------------------------------------
          |RES_LEN|     RES     |CK_LEN|     CK     |IK_LEN|     IK     |  
          ---------------------------------------------------------------

     Byte(s)                    Description          Length
     -------------------------------------------------------
     1                          Length of RES (L1)   1
     2 to (L1+1)                RES                  L1
     (L1+2)                     Length of CK(L2)     1
     (L1+3) to (L1+L2+2)	      CK                   L2
     (L1+L2+3)                  Length of IK(L3)     1
     (L1+L2+4) to (L1+L2+L3+3)  IK                   L3
     -------------------------------------------------------

  Sync failure case:
                    status   = DS_AUTH_AKA_SYNC_FAILURE
                    digest   = NULL
                    aka_data = AUTS_LEN + AUTS

     Set the aka_data as specified below
     Bytes:    1        L1         
          -------------------------
          |AUTS_LEN|     AUTS     |
          -------------------------

     Byte(s)                    Description          Length
     -------------------------------------------------------
     1                          Length of AUTS (L1)   1
     2 to (L1+1)                AUTS                  L1
     -------------------------------------------------------

  Failure case:
                    status   = DS_AUTH_AKA_FAILURE
                    digest   = NULL
                    aka_data = NULL
                     
 @param[out]  aka_handle    AKA handle
 @param[out]  status        Result of the AKA operation
 @param[out]  digest        HMAC digest 
 @param[out]  digest_len    Digest length
 @param[out]  aka_data      AKA data (RES, CK, IK or AUTS)
 @param[out]  aka_data_len  AKA data length
 @param[out]  user_data     User data passed by the client

 @dependencies None
 
 @return       None
 
 @sideeffect  The data from the callback needs to be copied into a local buffer 
              before it can be used. AKA will free the buffer once the control
              returns after the callback is called
 */
/*===========================================================================*/
typedef void (*ds_auth_aka_callback_type) 
(
  ds_auth_aka_handle_type        aka_handle,
  ds_auth_aka_status_enum_type   status,
  uint8                         *digest,
  uint8                          digest_len,
  uint8                         *aka_data,
  uint8                          aka_data_len,
  void                          *user_data
);

/*=============================================================================

  EXTERNAL FUNCTIONS

=============================================================================*/

/*===========================================================================*/
 /**
 FUNCTION:      DS_AUTH_RUN_AKA_ALGO

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
ds_auth_aka_handle_type ds_auth_run_aka_algo
(
  ds_auth_aka_ver_enum_type   aka_ver,
  uint8*                      rand,
  uint8                       rand_len,
  uint8*                      autn,
  uint8                       autn_len,
  ds_auth_aka_callback_type   aka_algo_cback,
  void                       *user_data,
  int16                      *ds_errno
);
#endif /* DS_AUTH_H */

