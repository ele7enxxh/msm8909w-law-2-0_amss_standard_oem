#ifndef WL_COMMON_H
#define WL_COMMON_H
/*============================================================================
  Wifi Location Common Header File
 
  This file defines common structures used in Wifi Location information 
  creation and use that are common between high level modules, notably 
  LE, WLM & SM

               Copyright (c) 2015 Qualcomm Atheros, Inc.
               All Rights Reserved.
               Qualcomm Atheros Confidential and Proprietary.

               Copyright (c) 2015 Qualcomm Technologies, Inc. 
               All Rights Reserved.
               Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

/*=============================================================================
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/common/inc/wl_common.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $
=============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "com_dtypes.h" /* for uint8 */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*=============================================================================
   Wifi Fix Results Type usable by WLM, LE & SM
=============================================================================*/
typedef struct 
{  
  struct
  {
    uint16 v_LowiNotPresent :1;     /** LOWI-LP SW/HW not found on device by
                                        WLM – multiple attempts to init. LOWI-LP
                                        have failed. Flag shouldn’t be needed 
                                        given no passive fixes are expected, 
                                        and no active clients 
                                        should ever have been registered? But 
                                        perhaps in error conditions, such as an 
                                        unexpected active request, this could 
                                        occur? */
    uint16 v_LowiNotReady :1;       /** When request received, WLM / LOWI-LP 
                                        init. is in-progress – this is a
                                        transitional state. This
                                        shouldn’t happen often given                   
                                        deregistration of WLM from GM, 
                                        but could in race conditions. */
    uint16 v_NoResponse :1;         /** e.g. LOWI measurement request time-out
                                        Reached.  Should only happen in case of 
                                        LOWI bug. */
    uint16 v_ResponseTempFailure :1; /** LOWI replied there’s a 
                                         temporary failure of Wifi HW/SW, 
                                         including LOWI-LP status values of:
                                         BUSY, DRIVER_ERROR, DRIVER_TIMEOUT,
                                         INTERNAL_ERROR, LOWI_LP_SERVICE_ERROR 
                                     */
    uint16 v_ResponsePermFailure :1; /** LOWI-LP was asked for a MeasBlk, but 
                                         replied with a Permanent Failure, e.g.
                                         going away.  This
                                         shouldn’t happen often given                   
                                         deregistration, but could in race 
                                         conditions.  Note this (nor any other 
                                         flag in this field) should not be
                                         considered a deregistration - that is
                                         managed separately. Expected LOWI-LP 
                                         status values that map to this are:
                                         INVALID_REQ, NOT_SUPPORTED */
    uint16 v_NoMeasFound :1;        /** likely far from all APs – perhaps 
                                        outdoors? TechSel should try GNSS ?? */
    // 10 bits remain
  } w_Me; // Wifi Measurement Engine related bits

  struct
  {
    uint8 v_CacheEmpty :1;       /** shouldn’t typically be needed in report
                                     back to GM, given 
                                     de-registration, but could happen in
                                     race conditions */
    uint8 v_CacheNoMatch : 1;    /** Of all (non-suspect) Wifi measurements
                                     found, none were found in Wifi AD Cache,
                                     so no Wifi fix can be produced.
                                     Going to AP, server, or 3rd party AD
                                     may result in more AD, that may then 
                                     result in a successful Wifi location. */
    uint8 v_CacheFewMatches : 1; /** Of all (non-suspect) Wifi measurements
                                     found, only a few were found in Wifi AD 
                                     Cache.  Typically, a Wifi fix can still
                                     be produced, however, going to 
                                     AP, server, or 3rd party AD source
                                     may result in more AD, that may then 
                                     result in a more accurate Wifi location. 
                                     */
    // 5 bits remain
  } u_Ad; // Wifi Assistance Data related bits

  struct
  {
    uint8 v_WlmReqInProg :1;       /** Additional request(s) were rcvd. before 
                                       reply to this request completed.  
                                       (They were dropped.) */
    uint8 v_WlmMalformedRequest :1; // Request for action was rejected. 
    // 6 bits remain
  } u_Wlm; // Wifi Location Manager related bits

  struct
  {
    uint8 v_InsufficientMeas :1; /** Not enough measurements (e.g. Wifi APs
                                     w/matching AD available) to do a fix */
    uint8 v_IntraWifiCheckFailed :1; /** Wifi vs. current Wifi meas or previous
                                         Wifi location self-check failure */
    uint8 v_WifiVsOtherCheckFailed :1; /** Wifi vs. other source (e.g. WWAN) 
                                           failure */
    uint8 v_AlgorithmError :1; /** Calculation aborted due to unexpected error
                                   like NULL pointer check failure. */
    uint8 v_OldVsNewWifiCheckFailed :1; // Jumped too far between fixes
    
    /** Note that following flag is for future use - not expected to be set
        as of LPW for 08.75 - should be set when hooked up to GF 3.5 */
    uint8 v_ExistingAleFixBetterThanWifi :1; /** Wifi fix didn’t replace 
                                                 best fix. Often benign in 
                                                 passive cases.  Sometimes 
                                                 wasteful in power-sensitive 
                                                 active cases (GM.) */

    /** Note that following flag is for future use - not expected to be set
        as of LPW for 08.75 - should be set when hooked up to GF 3.5 */
    uint8 v_BetterFixNotSent :1; /** if forced to send Wifi when better fix
                                     was available */
    uint8 v_IsGoodFix :1; /** If WiLE successfully calc'ed a location that was
                              accepted by ALE. As of 9/24/2015, this is the only
                              bit that should be set on an esp. clean good fix. 
                              */
    // 0 bits remain
  } u_Le; // Location Estimation related bits

  // 40 bits allocated above
} wl_ResultFlagsType; // Used in IPC - change (size) with IPC rev. only

// source of active request for Wifi Location
typedef enum
{
  WL_POS_REQ_SRC_NONE,            /** No direct request - e.g. passive scan 
                                      for ALE update only. */
  WL_POS_REQ_SRC_BG_LOC_ENG,       // Background Locaiton Engine
  WL_POS_REQ_SRC_MAX = 0xFFFFFFFF  // For Internal Use, enum sizing
} wl_PosReqSrcType; // Used in IPC - change with IPC rev. only

#endif // WL_COMMON_H