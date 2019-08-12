#ifndef PS_UTILS_AKA_H
#define PS_UTILS_AKA_H

/*=========================================================================*/
/*!
  @file 
  ps_utils_aka.h

  @brief
  This file provides functions that are used to perform AKA operations

  @detail
  This file provides methods to do AKA operations. Based on the client's 
  requirement either AKAv1 or AKAv2 is performed & the results are returned
 
  Exported functions:
  ps_utils_run_aka_algo() 
  This is called by the client to run the AKA algo on the specified values

  ps_utils_aka_cback_type()
  This is the callback function that will be provided by the clients & will 
  be called once the result of AKA is available.

  @note
  MMGSDI ISIM session needs to be opened if AKA has to be performed on the 
  card.

  Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_utils_aka.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  10/26/12   bb   Removed API to create AKA EFS NV item file path
  10/11/10   jee  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_auth.h"

/*===========================================================================

                    LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/** AKA handle type */
typedef ds_auth_aka_handle_type ps_utils_aka_handle_type;

/**
* ENUM:      PS_UTILS_AKA_STATUS_ENUM_TYPE 
* Indication of return status of running AKA
*/
typedef enum 
{
  PS_UTILS_AKA_SUCCESS      = DS_AUTH_AKA_SUCCESS,
  PS_UTILS_AKA_SYNC_FAILURE = DS_AUTH_AKA_SYNC_FAILURE,
  PS_UTILS_AKA_FAILURE      = DS_AUTH_AKA_FAILURE,
  PS_UTILS_AKA_MAX_STATUS
} ps_utils_aka_status_enum_type;


/**
* ENUM:      PS_UTILS_AKA_VER_ENUM_TYPE
* AKA version (currently v1 or v2)
*/
typedef enum 
{
  PS_UTILS_AKA_V1            = DS_AUTH_AKA_V1,
  PS_UTILS_AKA_V2            = DS_AUTH_AKA_V2,
  PS_UTILS_AKA_MAX_VER
} ps_utils_aka_ver_enum_type;

/*===========================================================================*/
 /**
 FUNCTION:      PS_UTILS_AKA_CALLBACK_TYPE

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
                    status   = PS_UTILS_AKA_SYNC_FAILURE
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
                    status   = PS_UTILS_AKA_FAILURE
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
typedef ds_auth_aka_callback_type ps_utils_aka_callback_type;

/*===========================================================================*/
 /**
 FUNCTION:      PS_UTILS_RUN_AKA_ALGO

 @brief        Runs AKAv1 or AKAv2 for the client & returns the result in the 
               callback function provided by the client
 
 @param[in]  aka_ver     AKA handle
 @param[in]  rand        RAND value
 @param[in]  rand_len    RAND length
 @param[in]  autn        AUTN value
 @param[in]  autn_len    AUTN length
 @param[in]  run_aka_algo_cback  Client callback that is called when the 
                                  results are available
 @param[in]  user_data   User data, passed to the client callback function

 @dependencies None
 
 @return      AKA handle or Failure
 @retval <0   Failure
 @retval >=0  AKA handle
 
 @sideeffect  None
 */
/*===========================================================================*/
ps_utils_aka_handle_type ps_utils_run_aka_algo
(
  ps_utils_aka_ver_enum_type  aka_ver,
  uint8*                      rand,
  uint8                       rand_len,
  uint8*                      autn,
  uint8                       autn_len,
  ps_utils_aka_callback_type  run_aka_algo_cback,
  void                       *user_data
);

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================*/
 /**
 FUNCTION:  PS_UTILS_AKA_REG_MMGSDI_CLIENT_ID()

 @brief  This function is used to regsiter a client id for the 
         authentication mechanism that uses the mmgsdi module. As
         a part of the authentication procedure the MMGDSI
         library is called to register a client id.
 
 @dependencies None
 
 @return      None
 
 @sideeffect  None
 */
/*===========================================================================*/
void ps_utils_aka_reg_mmgsdi_client_id
(
  void
);

#ifdef __cplusplus
}
#endif
#endif /* PS_UTILS_AKA_H */
