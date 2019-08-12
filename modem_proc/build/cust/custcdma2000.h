#ifndef CUSTCDMA2000_H
#define CUSTCDMA2000_H
/*===========================================================================

            " C D M A 2 0 0 0 "   H E A D E R   F I L E

DESCRIPTION
  Configuration for IS-856 support on MSM6800 targets.

  Copyright (c) 2001, 2002, 2003
                2004, 2005, 2006 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/build/cust/custcdma2000.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/23/06   dnn     Created the file. Added FEATURE_IS683_OTASP for MM 

===========================================================================*/

/*===========================================================================

                         PUBLIC DATA DECLARATIONS

===========================================================================*/
/**************************************************/
/*                      CDMA                      */
/**************************************************/
//#define T_POSEIDON2
//top level feature removed: #define FEATURE_MULTIMODE_RFAPI
//top level feature removed: #define FEATURE_MULTI_RX_CHAIN
#define FEATURE_MMOC_SUPPORTS_HYBRID
/* Radio resource arbitration.
*/
#define FEATURE_SYSTEM_ARBITRATION
//top level feature removed: #define FEATURE_PLCM_ENHANCEMENTS
#define FEATURE_HHO_ENHANCEMENTS
//top level feature removed: #define FEATURE_BUILD_CDMA_TRIMODE 
#define FEATURE_TRANS_MANAGER 
/**************************************************/
/*                  SRCH / SLEEP                  */
/**************************************************/

//top level feature removed: #define FEATURE_MSM6500_DEC_DYNAMIC_CLK_OFF
/**************************************************/
/*                      HDR                       */
/**************************************************/
/* Minimal UATI Mode feature */
//top level feature removed: #define FEATURE_HDR_MIN_UATI_MODE
#ifdef FEATURE_HDR_MIN_UATI_MODE
  /* When maintain EVDO session is FALSE, whether to explicitly
   * close session (ECS) by sending SessionClose message to AN */
  #undef FEATURE_HDR_MIN_UATI_MODE_ECS
#endif

/* Control of FEATURE_HDR_CUSTOM_CONFIG has been moved to the build file */


/**************************************************/
/*                  Multimode                     */
/**************************************************/
#ifdef FEATURE_IS683C_PRL
#define FEATURE_IS683C_OTASP
#endif
/**************************************************/
/*                  MDSP                          */
/**************************************************/

/**************************************************/
/*                      RF                        */
/**************************************************/
//top level feature removed: #define FEATURE_UART_TCXO_CLK_FREQ
/**************************************************/
/*                      GPSONE                    */
/**************************************************/

/**************************************************/
/*                      DATA                      */
/**************************************************/
#define FEATURE_SNM_ENHANCEMENTS
//top level feature removed: #define FEATURE_DATA_SERIALIZER
// #define FEATURE_DATA_PS_DATA_LOGGING_PARTIAL_PPP


/* FEATURE_CDMA feature should be defined only in C2k 
 * and multimode (UMTS + CDMA) builds. 
 * support for IS-95/IS2000/1X Data Services   */
//top level feature removed: #define FEATURE_CDMA

#endif /* CUSTCDMA2000_H */

