#ifndef SDDBG_H
#define SDDBG_H
/*===========================================================================

                 S Y S T E M   D E T E R M I N A T I O N
 
                    D E B U G   H E A D E R   F I L E

DESCRIPTION
  This header file contains SD debug macros.
   

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/sd/src/sddbg.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/29/14   jvo     Refactored message macros
07/17/12   jh      Added LTE Available File BSR support
08/16/11   fc      Added QTF support.
02/10/11   am      Migrating SUTE to QTF
12/02/10   gm      Added fixes for MOB naming conflicts.
10/05/10   gm      Added MSG_SPRINTF_1 for enhanced debug msg
08/20/10   dqu     Added SD_MSG_CONSTR and SD_MSG_SCRIPT as additions to 
                   debug messages. They are used as tools to toggle on and
                   off the F3 message enhancements and are used in sdssscr.c
                   and sdss.c functions to enhance script and construct 
                   messages.
01/04/07   jqi     Lint fixes for barred API -sprintf.
12/02/05   jqi     Lint fixes for Lint ver8.
08/22/01   RC      Change SD_CASE_NOT_IMP() to ERR() when SD_DEBUG is off.
03/27/01   RC      Added debug macros.
11/17/00   RC      Initial release.

===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "target.h"    /* Target configuration file */
#include "err.h"       /* Interface to error log services */
#include "msg.h"       /* Interface to F3 messages services */

#ifdef FEATURE_MMODE_QTF
#error code not present
#endif /* FEATURE_MMODE_QTF */

#ifdef SD_DEBUG
#error code not present
#endif /* SD_DEBUG */

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*---------------------------------------------------------------------------
      Compile SD with debug information, asserts and messages.
---------------------------------------------------------------------------*/

#ifndef __FILENAME__
#define __FILENAME__ __FILE__
#endif

//#define SD_DEBUG

/* General message macros, some are for testing stubs */
#ifndef MSG

#ifdef SD_DEBUG
#error code not present
#else
#define MSG(   SSID, level, str )                 MSG_BASE(level,str, 0, 0, 0)
#define MSG_1( SSID, level, str, a )              MSG_BASE(level,str,a,0,0)
#define MSG_2( SSID, level, str, a, b )           MSG_BASE(level,str,a,b,0)
#define MSG_3( SSID, level, str, a, b, c )        MSG_BASE(level,str,a,b,c)
#define MSG_4( SSID, level, str, a, b, c, d )
#define MSG_5( SSID, level, str, a, b, c, d, e )
#define MSG_6( SSID, level, str, a, b, c, d, e, f )
#endif /* SD_DEBUG */

#define MMODE_MSG_LEGACY_FATAL         MSG_LVL_FATAL
#define MMODE_MSG_LEGACY_ERROR         MSG_LVL_ERROR
#define MMODE_MSG_LEGACY_HIGH          MSG_LVL_HIGH
#define MMODE_MSG_LEGACY_MED           MSG_LVL_MED
#define MMODE_MSG_LEGACY_LOW           MSG_LVL_LOW
#endif /* ! MSG */

#ifndef MMODE_MSG_LEGACY_FATAL

#define MMODE_MSG_LEGACY_FATAL         MSG_LEGACY_FATAL
#define MMODE_MSG_LEGACY_ERROR         MSG_LEGACY_ERROR
#define MMODE_MSG_LEGACY_HIGH          MSG_LEGACY_HIGH
#define MMODE_MSG_LEGACY_MED           MSG_LEGACY_MED
#define MMODE_MSG_LEGACY_LOW           MSG_LEGACY_LOW

#endif /* MMODE_MSG_LEGACY_FATAL */



/*-------------------------------------------------------------------------*/

/* MAIN MESSAGE MACROS */
/* Messages have numbers to designate the number of parameters, not counting
** the string parameter.  For example, if the format string is "x:%d, y:%y",
** use SD_MSG_HIGH_2("x:%d, y:%y", x, y) because there are two parameters in
** the format string.
**
** SD_MSG_LOW_x, SD_MSG_MED_x, and SD_MSG_HIGH_x simply print to the message log
**   with their designated message level.
** 
** SD_ERR_x quits during testing or uses MSG_MED on target
**
** SD_ERR_FATAL_x prints to the message log and exits the software.
*/

#define SD_ERR_FATAL_0(str)            ERR_FATAL( "=SD= " str, 0, 0, 0 )
#define SD_ERR_FATAL_1(str, a)         ERR_FATAL( "=SD= " str, (a), 0, 0 )
#define SD_ERR_FATAL_2(str, a, b)      ERR_FATAL( "=SD= " str, (a), (b), 0 )
#define SD_ERR_FATAL_3(str, a, b, c)   ERR_FATAL( "=SD= " str, (a), (b), (c) )

#if defined(CM_DEBUG) || defined(MMOC_DEBUG) || defined(SD_DEBUG)
#error code not present
#else
#define SD_ERR_0(str)                MSG( MSG_SSID_SD,               \
                                          MMODE_MSG_LEGACY_MED,      \
                                          "=SD= " str )
#define SD_ERR_1(str, a)             MSG_1( MSG_SSID_SD,             \
                                          MMODE_MSG_LEGACY_MED,      \
                                          "=SD= " str, (a) )
#define SD_ERR_2(str, a, b)          MSG_2( MSG_SSID_SD,             \
                                          MMODE_MSG_LEGACY_MED,      \
                                          "=SD= " str, (a), (b) )
#define SD_ERR_3(str, a, b, c)       MSG_3( MSG_SSID_SD,             \
                                          MMODE_MSG_LEGACY_MED,      \
                                          "=SD= " str, (a), (b), (c) )

#endif /* CM_DEBUG || MMOC_DEBUG || SD_DEBUG */


#if 0
#define SD_ERR_0(str)                printf("\nERR,%s,%d)\n",  \
                                             __FILENAME__, __LINE__);
#define SD_ERR_1(str, a)             SD_ERR_0(str)
#define SD_ERR_2(str, a, b)          SD_ERR_0(str)
#define SD_ERR_3(str, a, b, c)       SD_ERR_0(str)
#endif


#define SD_MSG_HIGH_0(str)             MSG( MSG_SSID_SD,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=SD= " str )
#define SD_MSG_MED_0(str)              MSG( MSG_SSID_SD,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=SD= " str )
#define SD_MSG_LOW_0(str)              MSG( MSG_SSID_SD,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=SD= " str )

/*-------------------------------------------------------------------------*/

#define SD_MSG_HIGH_1(str, a)        MSG_1( MSG_SSID_SD,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=SD= " str, (a) )
#define SD_MSG_MED_1(str, a)         MSG_1( MSG_SSID_SD,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=SD= " str, (a) )
#define SD_MSG_LOW_1(str, a)         MSG_1( MSG_SSID_SD,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=SD= " str, (a) )
/*-------------------------------------------------------------------------*/

#define SD_MSG_HIGH_2(str, a, b)     MSG_2( MSG_SSID_SD,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=SD= " str, (a), (b) )
#define SD_MSG_MED_2(str, a, b)      MSG_2( MSG_SSID_SD,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=SD= " str, (a), (b) )
#define SD_MSG_LOW_2(str, a, b)      MSG_2( MSG_SSID_SD,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=SD= " str, (a), (b) )

/*-------------------------------------------------------------------------*/

#define SD_MSG_HIGH_3(str, a, b, c)  MSG_3( MSG_SSID_SD,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=SD= " str, (a), (b), (c) )
#define SD_MSG_MED_3(str, a, b, c)   MSG_3( MSG_SSID_SD,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=SD= " str, (a), (b), (c) )
#define SD_MSG_LOW_3(str, a, b, c)   MSG_3( MSG_SSID_SD,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=SD= " str, (a), (b), (c) )
/*-------------------------------------------------------------------------*/

#define SD_MSG_HIGH_4(str, a, b, c, d)  MSG_4( MSG_SSID_SD,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=SD= " str, (a), (b), (c), (d) )
#define SD_MSG_MED_4(str, a, b, c, d)   MSG_4( MSG_SSID_SD,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=SD= " str, (a), (b), (c), (d) )
#define SD_MSG_LOW_4(str, a, b, c, d)   MSG_4( MSG_SSID_SD,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=SD= " str, (a), (b), (c), (d) )
/*-------------------------------------------------------------------------*/

#define SD_MSG_HIGH_5(str, a, b, c, d, e)  MSG_5( MSG_SSID_SD,               \
                                           MMODE_MSG_LEGACY_HIGH,            \
                                           "=SD= " str, (a), (b), (c), (d), (e) )
#define SD_MSG_MED_5(str, a, b, c, d, e)   MSG_5( MSG_SSID_SD,               \
                                           MMODE_MSG_LEGACY_MED,             \
                                           "=SD= " str, (a), (b), (c), (d), (e) )
#define SD_MSG_LOW_5(str, a, b, c, d, e)   MSG_5( MSG_SSID_SD,               \
                                           MMODE_MSG_LEGACY_LOW,             \
                                           "=SD= " str, (a), (b), (c), (d), (e) )
/*-------------------------------------------------------------------------*/

#define SD_MSG_HIGH_6(str, a, b, c, d, e, f)  MSG_6( MSG_SSID_SD,               \
                                              MMODE_MSG_LEGACY_HIGH,            \
                                              "=SD= " str, (a), (b), (c), (d), (e), (f) )
#define SD_MSG_MED_6(str, a, b, c, d, e, f)   MSG_6( MSG_SSID_SD,               \
                                              MMODE_MSG_LEGACY_MED,             \
                                              "=SD= " str, (a), (b), (c), (d), (e), (f) )
#define SD_MSG_LOW_6(str, a, b, c, d, e, f)   MSG_6( MSG_SSID_SD,               \
                                              MMODE_MSG_LEGACY_LOW,             \
                                              "=SD= " str, (a), (b), (c), (d), (e), (f) )
/*-------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* <EJECT> */
/*---------------------------------------------------------------------------
                  SD_DEBUG/T_PC independent macros.
---------------------------------------------------------------------------*/

/* Message macro wrapper used to print out appropriate log message depending
** on whether the feature to print enhanced F3 messages for activated scripts
** has been enabled.
*/
#ifdef FEATURE_SD_ENHANCED_DEBUG_MSG
#define SD_MSG_SCRIPT(string, string_array) \
          MSG_SPRINTF_1(MSG_SSID_DFLT, MSG_LEGACY_HIGH, string "%s **", string_array)
#endif

/* Message macro wrapper used to print out appropriate log message depending
** on whether the feature to print enhanced F3 messages for activated
** constructs has been enabled.
*/
#ifndef FEATURE_SD_ENHANCED_DEBUG_MSG
#define SD_MSG_CONSTR(string, a, b, c, string_array) \
                                           SD_MSG_LOW_3(string "%-3d", a, b, c)

#else
#define SD_MSG_CONSTR(string, a, b, c, string_array) \
          MSG_SPRINTF_3(MSG_SSID_DFLT, MSG_LEGACY_LOW, string "%s", a, b, string_array)
#endif


#ifdef FEATURE_SD_ENHANCED_DEBUG_MSG
#define SD_MSG_SPRINT_HIGH(string, a, b, c) \
        MSG_SPRINTF_3(MSG_SSID_DFLT, MSG_LEGACY_HIGH, string, a, b, c)

#endif

/* <EJECT> */
/*---------------------------------------------------------------------------
                   SD_DEBUG/T_PC dependent macros.
---------------------------------------------------------------------------*/

  #define SD_FUNC_NOT_IMP( func ) \
      SD_ERR_0( #func " NOT Implemented!" )

  #define SD_CASE_NOT_IMP( _case ) \
      SD_ERR_0( #_case " case NOT Implemented" )


/*---------------------------------------------------------------------------
                  Asserts and verifications macros
---------------------------------------------------------------------------*/

#if defined(SD_DEBUG) && !defined(FEATURE_MMODE_QTF)
    #error code not present
#elif( TG==T_PC )

  #define SD_ASSERT( exp ) (exp);

#else

  #define SD_ASSERT( exp )

#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef SD_DEBUG
  #error code not present
#else
  #define SD_ASSERT_ENUM_IS_INRANGE( val, max )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef SD_DEBUG
  #error code not present
#else
  #define SD_ASSERT_ENUM_IS_INRANGE_INC( val, max )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef SD_DEBUG
  #error code not present
#else
  #define SD_ASSERT_ENUM_IS_INRANGE_OR( val, max, equal )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* vsnprintf is not part of ANSI C standard, so visual studio defines it 
** as _vsnprintf
*/
#if( TG != T_PC )

#if !defined(vsnprintf)
#define sd_vsnprintf _vsnprintf
#endif 

#endif

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/



#endif /* SDDBG_H */





