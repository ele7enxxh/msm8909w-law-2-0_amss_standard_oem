#ifndef HDRDEBUG_H
#define HDRDEBUG_H

/*===========================================================================

        H D R   D E B U G   D E F I N I T I O N S

DESCRIPTION
  This contains the HDR debug defines.

Copyright(c) 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/common/api/hdrdebug.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/04/16   vko     Reduce F3 msgs
05/31/13   vko     Fixed klocwork issue
09/16/11   wsh     Reduce heap mem usage by moving memory back to static
03/28/07   rmg     Added HIT messages macros
11/30/06   hal     Changed definition of HDR_MSG_SPRINTF_N macros
10/18/06   ljl     Added HDR_MSG_SRCH_SPRINTF_4()
05/11/06   rmg     Added new F3 message categories
09/10/03   mpa     Added new macro sets for new PROT, SRCH, DATA, DRIVERS and
                   IS890 HDR SSIDs
08/21/03    km     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "msg.h"
#include "msgcfg.h"

/*==========================================================================

                        MESSAGE MASK DEFINATIONS

==========================================================================*/
/* the following are required for all subsystems and are defined in msg.h 
 *
 * MSG_LEGACY_LOW      MSG_MASK_0
 * MSG_LEGACY_MED      MSG_MASK_1
 * MSG_LEGACY_HIGH     MSG_MASK_2
 * MSG_LEGACY_ERROR    MSG_MASK_3
 * MSG_LEGACY_FATAL    MSG_MASK_4
 *
 * The remaining bits are sub subsystem specific. MAC specific 
 * ones are defined below. To enable, ensure the bit mask is set
 * correctly in custhdr.h
 */

#define HDR_MSG_DBG_PKT_STATUS          MSG_MASK_5   /* ARQ - Packet Status    */
#define HDR_MSG_DBG_PS_SELECTION        MSG_MASK_6   /* Packet-size selection  */
#define HDR_MSG_DBG_RTCMAC_DECISIONS    MSG_MASK_7   /* RTCMAC Decisions       */
#define HDR_MSG_DBG_PERFORMANCE         MSG_MASK_8   /* Performance            */
#define HDR_MSG_DBG_RAB                 MSG_MASK_9   /* RAB                    */
#define HDR_MSG_DBG_FTC_PKT_INFO        MSG_MASK_10  /* FTC Packet Info        */

/*===========================================================================

                        Macro Definitions

===========================================================================*/
/*===========================================================================

   The macros used to output F3 messages are defined below. They are of
   the format HDR_MSG_n, where and n= 1 | 2 | 3, depending on the number 
   of arguments. These macros are modelled after those defined in msg.h

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*                      GENERIC HDR ASSERT                                 */
/*-------------------------------------------------------------------------*/

#define HDR_ASSERT( xx_exp ) \
   if( !(xx_exp) ) \
   { \
      ERR_FATAL( "Assertion " #xx_exp " failed", 0, 0, 0 ); \
   } 

/*-------------------------------------------------------------------------*/
/*                      GENERIC HDR MSG MACROS                             */
/*-------------------------------------------------------------------------*/

#define HDR_MSG(xx_ss_id, xx_ss_mask, fmt) \
            MSG(xx_ss_id, xx_ss_mask, fmt)

#define HDR_MSG_1(xx_ss_id, xx_ss_mask, fmt, arg1) \
            MSG_1(xx_ss_id, xx_ss_mask, fmt, arg1)

#define HDR_MSG_2(xx_ss_id, xx_ss_mask, fmt, arg1, arg2) \
            MSG_2(xx_ss_id, xx_ss_mask, fmt, arg1, arg2)

#define HDR_MSG_3(xx_ss_id, xx_ss_mask, fmt, arg1, arg2, arg3) \
            MSG_3(xx_ss_id, xx_ss_mask, fmt, arg1, arg2, arg3)

#define HDR_MSG_4(xx_ss_id, xx_ss_mask, fmt, arg1, arg2, arg3, arg4) \
            MSG_4(xx_ss_id, xx_ss_mask, fmt, arg1, arg2, arg3, arg4)

#define HDR_MSG_5(xx_ss_id, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5) \
            MSG_5(xx_ss_id, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5)

#define HDR_MSG_6(xx_ss_id, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, \
                                                                     arg6) \
            MSG_6(xx_ss_id, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, \
                                                                     arg6)

#define HDR_MSG_7(xx_ss_id, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, \
                                                               arg6, arg7) \
            MSG_7(xx_ss_id, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, \
                                                               arg6, arg7)

#define HDR_MSG_8(xx_ss_id, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, \
                                                         arg6, arg7, arg8) \
            MSG_8(xx_ss_id, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, \
                                                         arg6, arg7, arg8)

#define HDR_MSG_9(xx_ss_id, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, \
                                                   arg6, arg7, arg8, arg9) \
            MSG_9(xx_ss_id, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, \
                                                   arg6, arg7, arg8, arg9)

#define HDR_MSG_SPRINTF_1(xx_ss_mask, fmt, arg1) \
            MSG_SPRINTF_1(MSG_SSID_HDR_PROT, xx_ss_mask, fmt, arg1)

#define HDR_MSG_SPRINTF_2(xx_ss_mask, fmt, arg1, arg2) \
            MSG_SPRINTF_2(MSG_SSID_HDR_PROT, xx_ss_mask, fmt, arg1, arg2)

#define HDR_MSG_SPRINTF_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
            MSG_SPRINTF_3(MSG_SSID_HDR_PROT, xx_ss_mask, fmt, arg1, arg2, arg3)

#define HDR_MSG_SPRINTF_FMT_VAR_3(xx_ss_id, xx_ss_mask, fmt, arg1, arg2, arg3) \
            MSG_SPRINTF_FMT_VAR_3(xx_ss_id, xx_ss_mask, fmt, arg1, arg2, arg3)

/*-------------------------------------------------------------------------*/
/*                      PROTOCOL MESSAGES MACROS                           */
/*-------------------------------------------------------------------------*/                        
                         
#define HDR_MSG_PROT(xx_ss_mask, fmt) \
          MSG(MSG_SSID_HDR_PROT, xx_ss_mask, fmt)

#define HDR_MSG_PROT_1(xx_ss_mask, fmt, arg1) \
          MSG_1(MSG_SSID_HDR_PROT, xx_ss_mask, fmt, arg1)

#define HDR_MSG_PROT_2(xx_ss_mask, fmt, arg1, arg2) \
          MSG_2(MSG_SSID_HDR_PROT, xx_ss_mask, fmt, arg1, arg2)

#define HDR_MSG_PROT_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
          MSG_3(MSG_SSID_HDR_PROT, xx_ss_mask, fmt, arg1, arg2, arg3)

#define HDR_MSG_PROT_4(xx_ss_mask, fmt, arg1, arg2, arg3, arg4) \
          MSG_4(MSG_SSID_HDR_PROT, xx_ss_mask, fmt, arg1, arg2, arg3, arg4)

#define HDR_MSG_PROT_5(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5) \
          MSG_5(MSG_SSID_HDR_PROT, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                                      arg5)

#define HDR_MSG_PROT_6(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
          MSG_6(MSG_SSID_HDR_PROT, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                                arg5, arg6)

#define HDR_MSG_PROT_7(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                                      arg7) \
          MSG_7(MSG_SSID_HDR_PROT, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                          arg5, arg6, arg7)

#define HDR_MSG_PROT_8(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                                arg7, arg8) \
          MSG_8(MSG_SSID_HDR_PROT, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                    arg5, arg6, arg7, arg8)

#define HDR_MSG_PROT_9(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                          arg7, arg8, arg9) \
          MSG_9(MSG_SSID_HDR_PROT, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                              arg5, arg6, arg7, arg8, arg9)

#define HDR_MSG_PROT_SPRINTF_1(xx_ss_mask, fmt, arg1) \
        MSG_SPRINTF_1(MSG_SSID_HDR_PROT, xx_ss_mask, fmt, arg1)

#define HDR_MSG_PROT_SPRINTF_2(xx_ss_mask, fmt, arg1, arg2) \
        MSG_SPRINTF_2(MSG_SSID_HDR_PROT, xx_ss_mask, fmt, arg1, arg2)

#define HDR_MSG_PROT_SPRINTF_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
        MSG_SPRINTF_3(MSG_SSID_HDR_PROT, xx_ss_mask, fmt, arg1, arg2, arg3)

#define HDR_MSG_PROT_SPRINTF_FMT_VAR_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
        MSG_SPRINTF_FMT_VAR_3(MSG_SSID_HDR_PROT, xx_ss_mask, fmt, arg1, arg2, \
                                                                        arg3)
                                                                        
/*-------------------------------------------------------------------------*/
/*                      SEARCHER MESSAGES MACROS                           */
/*-------------------------------------------------------------------------*/                        
                         
#define HDR_MSG_SRCH(xx_ss_mask, fmt) \
          MSG(MSG_SSID_HDR_SRCH, xx_ss_mask, fmt)

#define HDR_MSG_SRCH_1(xx_ss_mask, fmt, arg1) \
          MSG_1(MSG_SSID_HDR_SRCH, xx_ss_mask, fmt, arg1)

#define HDR_MSG_SRCH_2(xx_ss_mask, fmt, arg1, arg2) \
          MSG_2(MSG_SSID_HDR_SRCH, xx_ss_mask, fmt, arg1, arg2)

#define HDR_MSG_SRCH_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
          MSG_3(MSG_SSID_HDR_SRCH, xx_ss_mask, fmt, arg1, arg2, arg3)

#define HDR_MSG_SRCH_4(xx_ss_mask, fmt, arg1, arg2, arg3, arg4) \
          MSG_4(MSG_SSID_HDR_SRCH, xx_ss_mask, fmt, arg1, arg2, arg3, arg4)

#define HDR_MSG_SRCH_5(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5) \
          MSG_5(MSG_SSID_HDR_SRCH, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                                      arg5)

#define HDR_MSG_SRCH_6(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
          MSG_6(MSG_SSID_HDR_SRCH, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                                arg5, arg6)

#define HDR_MSG_SRCH_7(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                                      arg7) \
          MSG_7(MSG_SSID_HDR_SRCH, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                          arg5, arg6, arg7)

#define HDR_MSG_SRCH_8(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                                arg7, arg8) \
          MSG_8(MSG_SSID_HDR_SRCH, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                    arg5, arg6, arg7, arg8)

#define HDR_MSG_SRCH_9(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                          arg7, arg8, arg9) \
          MSG_9(MSG_SSID_HDR_SRCH, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                              arg5, arg6, arg7, arg8, arg9)

#define HDR_MSG_SRCH_SPRINTF_1(xx_ss_mask, fmt, arg1) \
        MSG_SPRINTF_1(MSG_SSID_HDR_SRCH, xx_ss_mask, fmt, arg1)

#define HDR_MSG_SRCH_SPRINTF_2(xx_ss_mask, fmt, arg1, arg2) \
        MSG_SPRINTF_2(MSG_SSID_HDR_SRCH, xx_ss_mask, fmt, arg1, arg2)

#define HDR_MSG_SRCH_SPRINTF_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
        MSG_SPRINTF_3(MSG_SSID_HDR_SRCH, xx_ss_mask, fmt, arg1, arg2, arg3)

#define HDR_MSG_SRCH_SPRINTF_4(xx_ss_mask, fmt, arg1, arg2, arg3, arg4) \
        MSG_SPRINTF_4(MSG_SSID_HDR_SRCH, xx_ss_mask, fmt, arg1, arg2, arg3, arg4)

#define HDR_MSG_SRCH_SPRINTF_5(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5) \
        MSG_SPRINTF_5(MSG_SSID_HDR_SRCH, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5)

#define HDR_MSG_SRCH_SPRINTF_FMT_VAR_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
        MSG_SPRINTF_FMT_VAR_3(MSG_SSID_HDR_SRCH, xx_ss_mask, fmt, arg1, arg2, \
                                                                        arg3)
                                                                        
/*-------------------------------------------------------------------------*/
/*                      DRIVERS MESSAGES MACROS                            */
/*-------------------------------------------------------------------------*/                        
                         
#define HDR_MSG_DRIVERS(xx_ss_mask, fmt) \
          MSG(MSG_SSID_HDR_DRIVERS, xx_ss_mask, fmt)

#define HDR_MSG_DRIVERS_1(xx_ss_mask, fmt, arg1) \
          MSG_1(MSG_SSID_HDR_DRIVERS, xx_ss_mask, fmt, arg1)

#define HDR_MSG_DRIVERS_2(xx_ss_mask, fmt, arg1, arg2) \
          MSG_2(MSG_SSID_HDR_DRIVERS, xx_ss_mask, fmt, arg1, arg2)

#define HDR_MSG_DRIVERS_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
          MSG_3(MSG_SSID_HDR_DRIVERS, xx_ss_mask, fmt, arg1, arg2, arg3)

#define HDR_MSG_DRIVERS_4(xx_ss_mask, fmt, arg1, arg2, arg3, arg4) \
          MSG_4(MSG_SSID_HDR_DRIVERS, xx_ss_mask, fmt, arg1, arg2, arg3, arg4)

#define HDR_MSG_DRIVERS_5(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5) \
          MSG_5(MSG_SSID_HDR_DRIVERS, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                                     arg5)

#define HDR_MSG_DRIVERS_6(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
          MSG_6(MSG_SSID_HDR_DRIVERS, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                               arg5, arg6)

#define HDR_MSG_DRIVERS_7(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                                     arg7) \
          MSG_7(MSG_SSID_HDR_DRIVERS, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                         arg5, arg6, arg7)

#define HDR_MSG_DRIVERS_8(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                                arg7, arg8) \
          MSG_8(MSG_SSID_HDR_DRIVERS, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                   arg5, arg6, arg7, arg8)

#define HDR_MSG_DRIVERS_9(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                         arg7, arg8, arg9) \
          MSG_9(MSG_SSID_HDR_DRIVERS, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                             arg5, arg6, arg7, arg8, arg9)

#define HDR_MSG_DRIVERS_SPRINTF_1(xx_ss_mask, fmt, arg1) \
        MSG_SPRINTF_1(MSG_SSID_HDR_DRIVERS, xx_ss_mask, fmt, arg1)

#define HDR_MSG_DRIVERS_SPRINTF_2(xx_ss_mask, fmt, arg1, arg2) \
        MSG_SPRINTF_2(MSG_SSID_HDR_DRIVERS, xx_ss_mask, fmt, arg1, arg2)

#define HDR_MSG_DRIVERS_SPRINTF_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
        MSG_SPRINTF_3(MSG_SSID_HDR_DRIVERS, xx_ss_mask, fmt, arg1, arg2, arg3)

#define HDR_MSG_DRIVERS_SPRINTF_FMT_VAR_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
        MSG_SPRINTF_FMT_VAR_3(MSG_SSID_HDR_DRIVERS, xx_ss_mask, fmt, arg1, arg2, \
                                                                        arg3)

/*-------------------------------------------------------------------------*/
/*                      DATA MESSAGES MACROS                               */
/*-------------------------------------------------------------------------*/                        
                         
#define HDR_MSG_DATA(xx_ss_mask, fmt) \
          MSG(MSG_SSID_HDR_DATA, xx_ss_mask, fmt)

#define HDR_MSG_DATA_1(xx_ss_mask, fmt, arg1) \
          MSG_1(MSG_SSID_HDR_DATA, xx_ss_mask, fmt, arg1)

#define HDR_MSG_DATA_2(xx_ss_mask, fmt, arg1, arg2) \
          MSG_2(MSG_SSID_HDR_DATA, xx_ss_mask, fmt, arg1, arg2)

#define HDR_MSG_DATA_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
          MSG_3(MSG_SSID_HDR_DATA, xx_ss_mask, fmt, arg1, arg2, arg3)

#define HDR_MSG_DATA_4(xx_ss_mask, fmt, arg1, arg2, arg3, arg4) \
          MSG_4(MSG_SSID_HDR_DATA, xx_ss_mask, fmt, arg1, arg2, arg3, arg4)

#define HDR_MSG_DATA_5(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5) \
          MSG_5(MSG_SSID_HDR_DATA, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                                      arg5)

#define HDR_MSG_DATA_6(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
          MSG_6(MSG_SSID_HDR_DATA, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                                arg5, arg6)

#define HDR_MSG_DATA_7(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                                      arg7) \
          MSG_7(MSG_SSID_HDR_DATA, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                          arg5, arg6, arg7)

#define HDR_MSG_DATA_8(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                                arg7, arg8) \
          MSG_8(MSG_SSID_HDR_DATA, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                    arg5, arg6, arg7, arg8)

#define HDR_MSG_DATA_9(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                          arg7, arg8, arg9) \
          MSG_9(MSG_SSID_HDR_DATA, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                              arg5, arg6, arg7, arg8, arg9)

#define HDR_MSG_DATA_SPRINTF_1(xx_ss_mask, fmt, arg1) \
        MSG_SPRINTF_1(MSG_SSID_HDR_DATA, xx_ss_mask, fmt, arg1)

#define HDR_MSG_DATA_SPRINTF_2(xx_ss_mask, fmt, arg1, arg2) \
        MSG_SPRINTF_2(MSG_SSID_HDR_DATA, xx_ss_mask, fmt, arg1, arg2)

#define HDR_MSG_DATA_SPRINTF_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
        MSG_SPRINTF_3(MSG_SSID_HDR_DATA, xx_ss_mask, fmt, arg1, arg2, arg3)

#define HDR_MSG_DATA_SPRINTF_FMT_VAR_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
        MSG_SPRINTF_FMT_VAR_3(MSG_SSID_HDR_DATA, xx_ss_mask, fmt, arg1, arg2, \
                                                                        arg3)   
                                                                        
/*-------------------------------------------------------------------------*/
/*                      IS890 MESSAGES MACROS                              */
/*-------------------------------------------------------------------------*/                        
                         
#define HDR_MSG_IS890(xx_ss_mask, fmt) \
          MSG(MSG_SSID_HDR_IS890, xx_ss_mask, fmt)

#define HDR_MSG_IS890_1(xx_ss_mask, fmt, arg1) \
          MSG_1(MSG_SSID_HDR_IS890, xx_ss_mask, fmt, arg1)

#define HDR_MSG_IS890_2(xx_ss_mask, fmt, arg1, arg2) \
          MSG_2(MSG_SSID_HDR_IS890, xx_ss_mask, fmt, arg1, arg2)

#define HDR_MSG_IS890_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
          MSG_3(MSG_SSID_HDR_IS890, xx_ss_mask, fmt, arg1, arg2, arg3)

#define HDR_MSG_IS890_4(xx_ss_mask, fmt, arg1, arg2, arg3, arg4) \
          MSG_4(MSG_SSID_HDR_IS890, xx_ss_mask, fmt, arg1, arg2, arg3, arg4)

#define HDR_MSG_IS890_5(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5) \
          MSG_5(MSG_SSID_HDR_IS890, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                                       arg5)

#define HDR_MSG_IS890_6(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
          MSG_6(MSG_SSID_HDR_IS890, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                                 arg5, arg6)

#define HDR_MSG_IS890_7(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                                       arg7) \
          MSG_7(MSG_SSID_HDR_IS890, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                           arg5, arg6, arg7)

#define HDR_MSG_IS890_8(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                                 arg7, arg8) \
          MSG_8(MSG_SSID_HDR_IS890, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                     arg5, arg6, arg7, arg8)

#define HDR_MSG_IS890_9(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                           arg7, arg8, arg9) \
          MSG_9(MSG_SSID_HDR_IS890, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                               arg5, arg6, arg7, arg8, arg9)

#define HDR_MSG_IS890_SPRINTF_1(xx_ss_mask, fmt, arg1) \
        MSG_SPRINTF_1(MSG_SSID_HDR_IS890, xx_ss_mask, fmt, arg1)

#define HDR_MSG_IS890_SPRINTF_2(xx_ss_mask, fmt, arg1, arg2) \
        MSG_SPRINTF_2(MSG_SSID_HDR_IS890, xx_ss_mask, fmt, arg1, arg2)

#define HDR_MSG_IS890_SPRINTF_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
        MSG_SPRINTF_3(MSG_SSID_HDR_IS890, xx_ss_mask, fmt, arg1, arg2, arg3)

#define HDR_MSG_IS890_SPRINTF_FMT_VAR_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
        MSG_SPRINTF_FMT_VAR_3(MSG_SSID_HDR_IS890, xx_ss_mask, fmt, arg1, arg2, \
                                                                         arg3)

/*-------------------------------------------------------------------------*/
/*                      DEBUG MESSAGES MACROS                               */
/*-------------------------------------------------------------------------*/                        
                         
#define HDR_MSG_DEBUG(xx_ss_mask, fmt) \
          MSG(MSG_SSID_HDR_DEBUG, xx_ss_mask, fmt)

#define HDR_MSG_DEBUG_1(xx_ss_mask, fmt, arg1) \
          MSG_1(MSG_SSID_HDR_DEBUG, xx_ss_mask, fmt, arg1)

#define HDR_MSG_DEBUG_2(xx_ss_mask, fmt, arg1, arg2) \
          MSG_2(MSG_SSID_HDR_DEBUG, xx_ss_mask, fmt, arg1, arg2)

#define HDR_MSG_DEBUG_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
          MSG_3(MSG_SSID_HDR_DEBUG, xx_ss_mask, fmt, arg1, arg2, arg3)

#define HDR_MSG_DEBUG_4(xx_ss_mask, fmt, arg1, arg2, arg3, arg4) \
          MSG_4(MSG_SSID_HDR_DEBUG, xx_ss_mask, fmt, arg1, arg2, arg3, arg4)

#define HDR_MSG_DEBUG_5(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5) \
          MSG_5(MSG_SSID_HDR_DEBUG, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                                      arg5)

#define HDR_MSG_DEBUG_6(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
          MSG_6(MSG_SSID_HDR_DEBUG, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                                arg5, arg6)

#define HDR_MSG_DEBUG_7(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                                      arg7) \
          MSG_7(MSG_SSID_HDR_DEBUG, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                          arg5, arg6, arg7)

#define HDR_MSG_DEBUG_8(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                                arg7, arg8) \
          MSG_8(MSG_SSID_HDR_DEBUG, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                    arg5, arg6, arg7, arg8)

#define HDR_MSG_DEBUG_9(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                          arg7, arg8, arg9) \
          MSG_9(MSG_SSID_HDR_DEBUG, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                              arg5, arg6, arg7, arg8, arg9)

#define HDR_MSG_DEBUG_SPRINTF_1(xx_ss_mask, fmt, arg1) \
        MSG_SPRINTF_1(MSG_SSID_HDR_DEBUG, xx_ss_mask, fmt, arg1)

#define HDR_MSG_DEBUG_SPRINTF_2(xx_ss_mask, fmt, arg1, arg2) \
        MSG_SPRINTF_2(MSG_SSID_HDR_DEBUG, xx_ss_mask, fmt, arg1, arg2)

#define HDR_MSG_DEBUG_SPRINTF_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
        MSG_SPRINTF_3(MSG_SSID_HDR_DEBUG, xx_ss_mask, fmt, arg1, arg2, arg3)

#define HDR_MSG_DEBUG_SPRINTF_FMT_VAR_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
        MSG_SPRINTF_FMT_VAR_3(MSG_SSID_HDR_DEBUG, xx_ss_mask, fmt, arg1, arg2, \
                                                                         arg3)   

/*-------------------------------------------------------------------------*/
/*                       HIT MESSAGES MACROS                               */
/*-------------------------------------------------------------------------*/                        
                         
#define HDR_MSG_HIT(xx_ss_mask, fmt) \
          MSG(MSG_SSID_HDR_HIT, xx_ss_mask, fmt)

#define HDR_MSG_HIT_1(xx_ss_mask, fmt, arg1) \
          MSG_1(MSG_SSID_HDR_HIT, xx_ss_mask, fmt, arg1)

#define HDR_MSG_HIT_2(xx_ss_mask, fmt, arg1, arg2) \
          MSG_2(MSG_SSID_HDR_HIT, xx_ss_mask, fmt, arg1, arg2)

#define HDR_MSG_HIT_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
          MSG_3(MSG_SSID_HDR_HIT, xx_ss_mask, fmt, arg1, arg2, arg3)

#define HDR_MSG_HIT_4(xx_ss_mask, fmt, arg1, arg2, arg3, arg4) \
          MSG_4(MSG_SSID_HDR_HIT, xx_ss_mask, fmt, arg1, arg2, arg3, arg4)

#define HDR_MSG_HIT_5(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5) \
          MSG_5(MSG_SSID_HDR_HIT, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                                      arg5)

#define HDR_MSG_HIT_6(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6) \
          MSG_6(MSG_SSID_HDR_HIT, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                                arg5, arg6)

#define HDR_MSG_HIT_7(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                                      arg7) \
          MSG_7(MSG_SSID_HDR_HIT, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                          arg5, arg6, arg7)

#define HDR_MSG_HIT_8(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                                arg7, arg8) \
          MSG_8(MSG_SSID_HDR_HIT, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                                    arg5, arg6, arg7, arg8)

#define HDR_MSG_HIT_9(xx_ss_mask, fmt, arg1, arg2, arg3, arg4, arg5, arg6, \
                                                          arg7, arg8, arg9) \
          MSG_9(MSG_SSID_HDR_HIT, xx_ss_mask, fmt, arg1, arg2, arg3, arg4, \
                                              arg5, arg6, arg7, arg8, arg9)

#define HDR_MSG_HIT_SPRINTF_1(xx_ss_mask, fmt, arg1) \
        MSG_SPRINTF_1(MSG_SSID_HDR_HIT, xx_ss_mask, fmt, arg1)

#define HDR_MSG_HIT_SPRINTF_2(xx_ss_mask, fmt, arg1, arg2) \
        MSG_SPRINTF_2(MSG_SSID_HDR_HIT, xx_ss_mask, fmt, arg1, arg2)

#define HDR_MSG_HIT_SPRINTF_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
        MSG_SPRINTF_3(MSG_SSID_HDR_HIT, xx_ss_mask, fmt, arg1, arg2, arg3)

#define HDR_MSG_HIT_SPRINTF_FMT_VAR_3(xx_ss_mask, fmt, arg1, arg2, arg3) \
        MSG_SPRINTF_FMT_VAR_3(MSG_SSID_HDR_HIT, xx_ss_mask, fmt, arg1, arg2, \
                                                                         arg3)   

#endif /* HDRDEBUG_H */
