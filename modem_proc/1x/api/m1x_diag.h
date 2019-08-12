#ifndef M1X_DIAG_H
#define M1X_DIAG_H
/*==========================================================================

                      1x DIAGnostic services header

  Description:  Interfaces for external subsystems

  Copyright (c) 2003 - 2013 Qualcomm Technologies, Inc.
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/m1x_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/17/16   ab      Enable idle diversity Debug
01/22/15   srk     Make QLIC messages back to debug level.
03/31/14   vks     Temporarily enable qlic debug messages level as med
10/03/13   srk     Created device only DTX debug level message mask.
10/18/12   srk     Created a file for unified 1x debug F3 message masks.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "msg.h"

/*===========================================================================

                                    DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*--------------------------------------------------------------------------
                                                Message Mask Definations
--------------------------------------------------------------------------*/
/* the following are required for all subsystems and are defined in msg.h
 *
 * MSG_LEGACY_LOW      MSG_MASK_0
 * MSG_LEGACY_MED      MSG_MASK_1
 * MSG_LEGACY_HIGH     MSG_MASK_2
 * MSG_LEGACY_ERROR    MSG_MASK_3
 * MSG_LEGACY_FATAL    MSG_MASK_4
 *
 * The remaining bits are sub subsystem specific. 1X search specific
 * ones are defined below. To enable, ensure the bit mask is set
 * correctly in custsrch.h
 */

/***************************************************************************
                              MUX DEBUG MASKS
***************************************************************************/
#define MSG_DBG_DEINT         MSG_MASK_5    /* DEINTERLEAVER              */
#define MSG_DBG_ENC           MSG_MASK_6    /* ENCODER                    */
#define MSG_DBG_M1XTIME       MSG_MASK_7    /* 1X TIME                    */
#define MSG_DBG_DTX           MSG_MASK_8    /* DEVICE ONLY DTX            */
#define MSG_DBG_MDSP          MSG_MASK_9    /* MDSP                       */
#define MSG_DBG_VOC           MSG_MASK_10   /* VOCODER                    */
#define MSG_DBG_RX            MSG_MASK_11   /* MUX RECEIVE TASKS          */
#define MSG_DBG_TX            MSG_MASK_12   /* MUX TRANSMIT TASKS         */
#define MSG_DBG_1XA           MSG_MASK_13   /* 1X ADVANCED                */

/***************************************************************************
                              SRCH DEBUG MASKS
***************************************************************************/
#define MSG_DBG_INIT          MSG_MASK_5   /* Acq, sync, slew             */
#define MSG_DBG_IDLE          MSG_MASK_6   /* Non slotted mode            */
#define MSG_DBG_ZZ_TIMELINE   MSG_MASK_7   /* slotted timelines           */
#define MSG_DBG_QPCH          MSG_MASK_8   /* qpch and qpch channel est   */
#define MSG_DBG_SCHED         MSG_MASK_9   /* search scheduler            */
#define MSG_DBG_TC            MSG_MASK_10  /* traffic state               */
#define MSG_DBG_IDLE_DIV      MSG_MASK_11  /* idle diversity              */
#define MSG_DBG_MISC_DBG      MSG_MASK_14  /* Misc debug messages         */
#define MSG_DBG_XO_TRIM       MSG_MASK_15  /* XO Trim switching algorithm */
#define MSG_DBG_STM           MSG_MASK_16  /* state machine driver module */
#define MSG_DBG_SRCH_SLOT     MSG_MASK_17  /* srch slot module            */
#define MSG_DBG_FING          MSG_MASK_18  /* finger module               */
#define MSG_DBG_TRIAGE        MSG_MASK_19  /* triage module               */
/* #define MSG_DBG_SRCH4      MSG_MASK_20     srch4 messages              */
#define MSG_DBG_RX_DIV        MSG_MASK_21  /* rxdiv messages              */
#define MSG_DBG_SRCH_SECT     MSG_MASK_22  /* search sector module        */
#define MSG_DBG_DV            MSG_MASK_23  /* DV messages                 */
#define MSG_DBG_SRCH_RX       MSG_MASK_24  /* srch_rx.c messages          */
#define MSG_DBG_SRCH_IRM      MSG_MASK_25  /* srch_irm.c messages         */
#define MSG_DBG_AFC           MSG_MASK_26  /* srch_afc_sm.c messages      */
#define MSG_DBG_QLIC          MSG_MASK_27  /* QLIC messages               */
#define MSG_DBG_CGPS          MSG_MASK_28  /* CGPS messages               */
#define MSG_DBG_STRINGS       MSG_MASK_31  /* Msg mask for strings        */


/*--------------------------------------------------------------------------
                  Message Macro 1x Re-Definitions
--------------------------------------------------------------------------*/
/* Message macro's to be used within 1x should all follow the following
   format:
     values  -- M1X_MSG( SSID, Msg Level, String, Arg1, Arg2, Arg3, ....)
     strings -- M1X_STR( SSID, Msg Level, String, Arg1, Arg2, Arg3, ....)
       Note: the macro will count the number of arguments, but from diag
             MSG's allow only 0-9 arguments, and
             STR's allow only 1-8 arguments.
   where:
      SSID      = SSID of tech, minus "MSG_SSID_1X"
                    valid "techs" are: ACP, DCP, DEC, ENC, MUX, SRCH
      Msg Level = Message level of the requested string, minus "MSG_"
                    valid levels are: MSG_LEGACY_[LOW|MED|HIGH|ERROR|FATAL]
                                      DBG_[*]
      String    = The string literal
      Arg1, ... = # Arguments
                    can be strings if macro used is "M1X_STR_#"

   */

/* Macro's to determine the number of arguments passed in to the macro,
   note because M1X_ARG_N and M1X_RSEQ_N are one off, this will actually
   return the number of arguments -1. Since I can't pass in zero parameters
   for M1X_NARG, I'm using the "string" as the first argument, hence the need
   to count "-1". M1X_RSEQ_N is the macro name MSG(), MSG_1(), MSG_2(), ...

   Example of how this works:
   str + 2 arguments
   M1X_NARG(str, a1, b2)
    M1X_NARG_(__VA_ARGS__, M1X_RSEQ_N())
              |||||||||||
              vvvvvvvvvvv
    M1X_NARG_(str, a1, b2, M1X_RSEQ_N())
                           \|||||||||||\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
                            vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
    M1X_NARG_(str, a1, b2,  MSG_9, MSG_8, MSG_7, MSG_6, MSG_5, MSG_4, MSG_3, MSG_2, MSG_1, MSG)
              \\|||||||||\\ \|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\
                Vvvvvvvvvvv  vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
      M1X_ARG_N(str, a1, b2, MSG_9, MSG_8, MSG_7, MSG_6, MSG_5, MSG_4, MSG_3, MSG_2, MSG_1, MSG)
                ||/  ||  ||  ||///  ||///  ||///  ||///  ||///  ||///  |||//  |///   |||///////
                vv   vv  vv  vv     vv     vv     vv     vv     vv     vv     v      vvv
               (_1,  _2, _3, _4,    _5,    _6,    _7,    _8,    _9,    _10,   N,     ...        ) N
       which returns "N", which in this case:
       N = "MSG_2"

   More Examples:
   str + 0 arguments
   M1X_NARG(str)
    M1X_NARG_(__VA_ARGS__, M1X_RSEQ_N())
    M1X_NARG_(str, M1X_RSEQ_N())
    M1X_NARG_(str, MSG_9, MSG_8, MSG_7, MSG_6, MSG_5, MSG_4, MSG_3, MSG_2, MSG_1, MSG)
      M1X_ARG_N(str, MSG_9, MSG_8, MSG_7, MSG_6, MSG_5, MSG_4, MSG_3, MSG_2, MSG_1, MSG)
                                                                                    |//
                                                                                    v
               (_1,  _2,    _3,    _4,    _5,    _6,    _7,    _8,    _9,    _10,   N,     ...        ) N
       which returns "N", which in this case:
       N = "MSG"

   str + 4 arguments
   M1X_NARG(str, a1, a2, a3, a4)
    M1X_NARG_(__VA_ARGS__, M1X_RSEQ_N())
    M1X_NARG_(str, a1, a2, a3, a4, M1X_RSEQ_N())
    M1X_NARG_(str, a1, a2, a3, a4, MSG_9, MSG_8, MSG_7, MSG_6, MSG_5, MSG_4, MSG_3, MSG_2, MSG_1, MSG)
      M1X_ARG_N(str, a1, a2, a3, a4, MSG_9, MSG_8, MSG_7, MSG_6, MSG_5, MSG_4, MSG_3, MSG_2, MSG_1, MSG)
                                                                        |////
                                                                        v
               (_1,  _2, _3, _4, _5, _6,    _7,    _8,    _9,    _10,   N,     ...                      ) N
       which returns "N", which in this case:
       N = "MSG_4"
 */

#ifdef T_WINNT
 #error code not present
#endif /* T_WINNT */

#define M1X_ARG_N( _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, N, ...) \
         N
#define M1X_ARG_N_(wkarnd) M1X_ARG_N wkarnd
#define M1X_NARG_(...) \
         M1X_ARG_N_( (__VA_ARGS__) )

/* Numerical Printing */
#define M1X_RSEQ_N() \
          MSG_9, MSG_8, MSG_7, MSG_6, MSG_5, MSG_4, MSG_3, MSG_2, MSG_1, MSG
#define M1X_NARG(...) \
          M1X_NARG_(__VA_ARGS__,M1X_RSEQ_N())

#define M1X_SRSEQ_N() \
          MSG_SPRINTF_9, MSG_SPRINTF_8, MSG_SPRINTF_7, MSG_SPRINTF_6, MSG_SPRINTF_5, MSG_SPRINTF_4, MSG_SPRINTF_3, MSG_SPRINTF_2, MSG_SPRINTF_1, MSG
/* String Printing
   Note MSG_SPRINTF_0() is replaced by MSG(), since without parameters,
    they are identical */
#define M1X_SNARG(...) \
          M1X_NARG_(__VA_ARGS__,M1X_SRSEQ_N())

/* New 1X Message Macro's */
/* using the macro's to count arguments, the "count" can be concatenated onto the
   known MSG Macro's name, so that the correct MSG or MSG_SPRINTF macro is used.
   From the examples above: "MSG_2", "MSG", "MSG_4" */
#define M1X_MSG( ssid, lvl, ... )                                          \
  M1X_NARG(__VA_ARGS__)(                                                   \
       MSG_SSID_1X_##ssid,                                                 \
       MSG_##lvl,                                                          \
       __VA_ARGS__                                                         \
      )

#define M1X_STR( ssid, lvl, ... )                                          \
  M1X_SNARG(__VA_ARGS__)(                                                  \
       MSG_SSID_1X_##ssid,                                                 \
       MSG_##lvl,                                                          \
       __VA_ARGS__                                                         \
      )


#define M1X_MSG_SSID( ssid, lvl, ... )                                     \
  M1X_NARG(__VA_ARGS__)(                                                   \
       MSG_SSID_##ssid,                                                    \
       MSG_##lvl,                                                          \
       __VA_ARGS__                                                         \
      )

#define M1X_STR_SSID( ssid, lvl, ... )                                     \
  M1X_SNARG(__VA_ARGS__)(                                                  \
       MSG_SSID_##ssid,                                                    \
       MSG_##lvl,                                                          \
       __VA_ARGS__                                                         \
      )


/* define the older message macros into something that won't compile, so as to
   enforce the use of the new macros
*/
#ifdef MSG_LOW
  #undef MSG_LOW
  #define MSG_LOW(...)                                                     \
    MSG_LOW___MacroDeprecated___Use___M1X_MSG( __VA_ARGS__)
#endif
#ifdef MSG_MED
  #undef MSG_MED
  #define MSG_MED(...)                                                     \
    MSG_MED___MacroDeprecated___Use___M1X_MSG( __VA_ARGS__)
#endif
#ifdef MSG_HIGH
  #undef MSG_HIGH
  #define MSG_HIGH(...)                                                     \
    MSG_HIGH___MacroDeprecated___Use___M1X_MSG( __VA_ARGS__)
#endif
#ifdef MSG_ERROR
  #undef MSG_ERROR
  #define MSG_ERROR(...)                                                     \
    MSG_ERROR___MacroDeprecated___Use___M1X_MSG( __VA_ARGS__)
#endif
#ifdef MSG_FATAL
  #undef MSG_FATAL
  #define MSG_FATAL(...)                                                     \
    MSG_FATAL___MacroDeprecated___Use___M1X_MSG( __VA_ARGS__)
#endif

#ifdef ERR
  #undef ERR
  #define ERR(...)                                                           \
    ERR___MacroDeprecated___Use___M1X_MSG( __VA_ARGS__)
#endif
#endif /* M1X_DIAG_H */
