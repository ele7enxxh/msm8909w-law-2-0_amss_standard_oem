#ifndef CMIDBG_H
#define CMIDBG_H

/*===========================================================================

 C A L L   M A N A G E R   I N T E R N A L D E B U G   H E A D E R   F I L E

DESCRIPTION
  This header file contains debug macros and definitions necessary to
  interface with CMDBG.C

  It is different from cmdbg.h in the way that has no dependency on other cm 
  header files.



Copyright (c) 2005 - 2010 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmidbg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/29/14   jvo     Refactored message macros
01/16/14   jvo     Added CM_CHECK_MAX_AND_DISCARD_ITEMS to help enforce
                   maximum values.
11/13/13   jvo     Added CMDBG_STR_MAP_LOOKUP to validate string map lookups
08/16/11   fc      Added QTF support.
06/08/10   gm      Klocwork/Lint error fixes
02/20/08   ak      Moved cmtest_test_fail_inc and BAD_CHOICE from cmidbg.h
01/24/08   ak      Transferred CM_VERIFY and CM_HELPER_VERIFY to cmtest.h
01/15/07   ak      Asserts removed from verifications
08/07/07   ic      Lint clean up
04/10/06   pk      Lint fixes
03/15/06   sk      Deleted CMDBG_CMD_MSG. Already defined in cmdbg.h.
01/06/06   ic      Lint cleanup  
12/11/05   ic      Lint cleanup
11/18/05   jqi     Initial release.


===========================================================================*/


#include "mmcp_variation.h"
#include "comdef.h"    /* Definition for basic types and macros */
#include "target.h"    /* Target configuration file */
#include "err.h"       /* Interface to error log services */
#include "msg.h"       /* Interface to F3 messages services */

#ifdef FEATURE_MMODE_QTF
#error code not present
#endif /* FEATURE_MMODE_QTF */

#ifdef CM_DEBUG
#error code not present
#endif

/*lint -save -e656 -e641 
** Turn off enum to int / operationuses compatible enums
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
/* General message macros, some are for testing stubs */
#ifndef MSG

#ifdef CM_DEBUG
#error code not present
#else
#define MSG(   SSID, level, str )                 MSG_BASE(level,str, 0, 0, 0)
#define MSG_1( SSID, level, str, a )              MSG_BASE(level,str,a,0,0)
#define MSG_2( SSID, level, str, a, b )           MSG_BASE(level,str,a,b,0)
#define MSG_3( SSID, level, str, a, b, c )        MSG_BASE(level,str,a,b,c)


#endif /* CM_DEBUG */

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
** use CM_MSG_HIGH_2("x:%d, y:%y", x, y) because there are two parameters in
** the format string.
**
** CM_MSG_LOW_x, CM_MSG_MED_x, and CM_MSG_HIGH_x simply print to the message log
**   with their designated message level.
** 
** CM_ERR_x quits during testing or uses MSG_MED on target
**
** CM_ERR_FATAL_x prints to the message log and exits the software.
*/

#define CM_ERR_FATAL_0(str)            ERR_FATAL( "=CM= " str, 0, 0, 0 )
#define CM_ERR_FATAL_1(str, a)         ERR_FATAL( "=CM= " str, (a), 0, 0 )
#define CM_ERR_FATAL_2(str, a, b)      ERR_FATAL( "=CM= " str, (a), (b), 0 )
#define CM_ERR_FATAL_3(str, a, b, c)   ERR_FATAL( "=CM= " str, (a), (b), (c) )

#if defined(CM_DEBUG) || defined(MMOC_DEBUG) || defined(SD_DEBUG)
#error code not present
#else
  #define CM_ERR_0(str)                MSG( MSG_SSID_CM,               \
                                            MMODE_MSG_LEGACY_MED,      \
                                            "=CM= " str )
  #define CM_ERR_1(str, a)             MSG_1( MSG_SSID_CM,             \
                                            MMODE_MSG_LEGACY_MED,      \
                                            "=CM= " str, (a) )
  #define CM_ERR_2(str, a, b)          MSG_2( MSG_SSID_CM,             \
                                            MMODE_MSG_LEGACY_MED,      \
                                            "=CM= " str, (a), (b) )
  #define CM_ERR_3(str, a, b, c)       MSG_3( MSG_SSID_CM,             \
                                            MMODE_MSG_LEGACY_MED,      \
                                            "=CM= " str, (a), (b), (c) )
#endif /* CM_DEBUG || MMOC_DEBUG || SD_DEBUG */


#define CM_MSG_HIGH_0(str)             MSG( MSG_SSID_CM,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=CM= " str )
#define CM_MSG_MED_0(str)              MSG( MSG_SSID_CM,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=CM= " str )
#define CM_MSG_LOW_0(str)              MSG( MSG_SSID_CM,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=CM= " str )

/*-------------------------------------------------------------------------*/

#define CM_MSG_HIGH_1(str, a)        MSG_1( MSG_SSID_CM,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=CM= " str, (a) )
#define CM_MSG_MED_1(str, a)         MSG_1( MSG_SSID_CM,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=CM= " str, (a) )
#define CM_MSG_LOW_1(str, a)         MSG_1( MSG_SSID_CM,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=CM= " str, (a) )
/*-------------------------------------------------------------------------*/

#define CM_MSG_HIGH_2(str, a, b)     MSG_2( MSG_SSID_CM,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=CM= " str, (a), (b) )
#define CM_MSG_MED_2(str, a, b)      MSG_2( MSG_SSID_CM,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=CM= " str, (a), (b) )
#define CM_MSG_LOW_2(str, a, b)      MSG_2( MSG_SSID_CM,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=CM= " str, (a), (b) )

/*-------------------------------------------------------------------------*/

#define CM_MSG_HIGH_3(str, a, b, c)  MSG_3( MSG_SSID_CM,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=CM= " str, (a), (b), (c) )
#define CM_MSG_MED_3(str, a, b, c)   MSG_3( MSG_SSID_CM,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=CM= " str, (a), (b), (c) )
#define CM_MSG_LOW_3(str, a, b, c)   MSG_3( MSG_SSID_CM,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=CM= " str, (a), (b), (c) )
/*-------------------------------------------------------------------------*/

#define CM_MSG_HIGH_4(str, a, b, c, d)  MSG_4( MSG_SSID_CM,               \
                                         MMODE_MSG_LEGACY_HIGH,            \
                                         "=CM= " str, (a), (b), (c), (d) )
#define CM_MSG_MED_4(str, a, b, c, d)   MSG_4( MSG_SSID_CM,               \
                                         MMODE_MSG_LEGACY_MED,             \
                                         "=CM= " str, (a), (b), (c), (d) )
#define CM_MSG_LOW_4(str, a, b, c, d)   MSG_4( MSG_SSID_CM,               \
                                         MMODE_MSG_LEGACY_LOW,             \
                                         "=CM= " str, (a), (b), (c), (d) )
/*-------------------------------------------------------------------------*/

#define CM_MSG_HIGH_5(str, a, b, c, d, e)  MSG_5( MSG_SSID_CM,               \
                                           MMODE_MSG_LEGACY_HIGH,            \
                                           "=CM= " str, (a), (b), (c), (d), (e) )
#define CM_MSG_MED_5(str, a, b, c, d, e)   MSG_5( MSG_SSID_CM,               \
                                           MMODE_MSG_LEGACY_MED,             \
                                           "=CM= " str, (a), (b), (c), (d), (e) )
#define CM_MSG_LOW_5(str, a, b, c, d, e)   MSG_5( MSG_SSID_CM,               \
                                           MMODE_MSG_LEGACY_LOW,             \
                                           "=CM= " str, (a), (b), (c), (d), (e) )
/*-------------------------------------------------------------------------*/

#define CM_MSG_HIGH_6(str, a, b, c, d, e, f)  MSG_6( MSG_SSID_CM,               \
                                              MMODE_MSG_LEGACY_HIGH,            \
                                              "=CM= " str, (a), (b), (c), (d), (e), (f) )
#define CM_MSG_MED_6(str, a, b, c, d, e, f)   MSG_6( MSG_SSID_CM,               \
                                              MMODE_MSG_LEGACY_MED,             \
                                              "=CM= " str, (a), (b), (c), (d), (e), (f) )
#define CM_MSG_LOW_6(str, a, b, c, d, e, f)   MSG_6( MSG_SSID_CM,               \
                                              MMODE_MSG_LEGACY_LOW,             \
                                              "=CM= " str, (a), (b), (c), (d), (e), (f) )


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* For use in cmdbg.c only! */

const char *cmdbg_fail_ut_wrapper
(
  const char *err_msg,
  const char *fmt_msg,
  int val1,
  int val2,
  int val3
);

#ifdef CMDEBUG_STRING_MAPS_ACTIVE
/*
** Macro - CMDBG_STR_MAP_LOOKUP
**
** This macro is meant to be used in a parameter list, to verify a string map
** index is valid and non-NULL before we send it to output.
**
** val     - the enum value to use
** max_val - 1 more than the highest valid enum value
** str_map - the string map to use (type should be const char **)
**
** This macro gives an error if any of these are true:
**  * val < 0
**  * val >= max_val
**  * str_map[ val ] == NULL
**
** Example usage:
**
** cmdbg_print_message("- favored_hybr_pref = %d(%s)",
**                     call_ptr->favored_hybr_pref,
**                     CMDBG_STR_MAP_LOOKUP(call_ptr->favored_hybr_pref,
**                                          CM_HYBR_PREF_MAX,
**                                          cm_hybr_pref_e_type_string_map));
**
** Notice how every branch of the ternary operator ( cnd ? t-val : f-val )
** results in a const char * value.  This allows the macro to be used in a
** parameter list.  In the CM_DEBUG case, calls to cmdbg_fail_ut_wrapper
** return a const char * value.
*/
#ifdef CM_DEBUG
#error code not present
#else
#define CMDBG_STR_MAP_LOOKUP(val, max_val, str_map)                         \
  (( (val) < 0 )                        ? ("OUT_OF_BNDS_LO") :              \
   ( (val) >= (max_val))                ? ("OUT_OF_BNDS_HI") :              \
   ( (str_map[(val)]) == NULL)          ? ("NULL_STR_MAP")   :              \
   ( (str_map[(val)])))
#endif /* CM_DEBUG */
#endif /* CMDEBUG_STRING_MAPS_ACTIVE */


/* For use in cmdbg.c only! */
#ifdef CMDEBUG_STRING_MAPS_ACTIVE
  #define CMDBG_RPT_MSG(rpttype, cmd, offset, max_cmd, strmap, num1)        \
          cmdbg_print_message( "CM<< " rpttype " %d (%s): %d",              \
                               cmd,                                         \
                               CMDBG_STR_MAP_LOOKUP(cmd - offset - 1,       \
                                                    max_cmd,                \
                                                    strmap),                \
                               num1)
#else
  #define CMDBG_RPT_MSG(rpttype, cmd, offset, max_cmd, strmap, num1)        \
          CM_MSG_HIGH_2( "CM<< " rpttype " %d: %d", cmd, num1 )
#endif


/* For use in cmdbg.c only! */
#ifdef CMDEBUG_STRING_MAPS_ACTIVE
  #define CMDBG_CMD_MSG(cmdtype, cmd, max_cmd, strmap, client_id)           \
          cmdbg_print_message( ">>CM " cmdtype " %d (%s): %d",              \
                               cmd,                                         \
                               CMDBG_STR_MAP_LOOKUP(cmd,                    \
                                                    max_cmd,                \
                                                    strmap),                \
                               cmclient_get_client_type(client_id))
#else
  #define CMDBG_CMD_MSG(cmdtype, cmd, max_cmd, strmap, client_id)           \
          CM_MSG_HIGH_2( ">>CM " cmdtype " %d: %d", cmd,                      \
                       cmclient_get_client_type(client_id))
#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Debug/PC independent macros.
*/

#define CM_FUNC_NOT_IMP( func ) \
    CM_MSG_HIGH_0( #func " NOT Implemented!" )

#define CM_CASE_NOT_IMP( _case ) \
    CM_ERR_0( #_case " case NOT Implemented" )



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Debug and PC dependent macros.
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Asserts and verifications macros.
*/

/* 
** 506: Constant value Boolean
** 774: Boolean within 'if' always evaluates to True
** This takes care of things like CM_ASSERT(FALSE)
*/ 
#if defined(CM_DEBUG) && !defined(FEATURE_MMODE_QTF)
#error code not present
#else
  #define CM_ASSERT( exp )
#endif


#ifdef CM_DEBUG
  #error code not present
#else
  #define CM_ASSERT_ERR( exp )                         \
    if( !(exp) )                                       \
    {                                                  \
      CM_ERR_0( "=CM= ASSERTION FAILED! " #exp );      \
    }
#endif


#define CM_NONE_DBG_ASSERT( exp )                      \
    if( !(exp) )                                       \
    {                                                  \
      CM_ERR_FATAL_0( "ASSERTION FAILED! " #exp );    \
    }


#ifdef CM_DEBUG
  #error code not present
#else
  #define CM_ASSERT_ENUM_IS_INRANGE( val, max )
#endif


#ifdef CM_DEBUG
  #error code not present
#else
  #define CM_ASSERT_ENUM_IS_INRANGE_INC( val, max )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


#ifdef CM_DEBUG
  #error code not present
#else
  #define CM_ASSERT_ENUM_IS_INRANGE_OR( val, max, equal )
#endif


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/* CM_CHECK_MAX_AND_DISCARD_ITEMS
** If the value (val) is greater than the maximum (max), set the value equal
** to the maximum and print an error message indicating that (description)
** items will be discarded.
**
** Note: value must be writeable (not a constant)
**
** Example:
** --------------------------------------------------------------------------
** #define MAX_REPORTS 5
**
** report_s_type myReports[10];
** int numReports = 10;
**
** CM_CHECK_MAX_AND_DISCARD_ITEMS( numReports, MAX_REPORTS, "Reports" );
**
** This will print:
** "Number of [Reports] : 10 > MAX (== 5), discarding extras"
** it will also set numReports = 5l
** --------------------------------------------------------------------------
** Originally created for use in cmstats.c
*/
#define CM_CHECK_MAX_AND_DISCARD_ITEMS( val, max, descr )                   \
  {                                                                         \
    if((val) > (max))                                                       \
    {                                                                       \
      CM_ERR_2("Number of [" descr "] : %d > MAX (== %d), discarding extras", \
           (val), (max));                                                \
      (val) = (max);                                                        \
    }                                                                       \
  }

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


/* Enum print macros.
*/
#ifdef CM_DEBUG
  #error code not present
#else
  #define CMDBG_PRT_FUNC( prt_func, enum_val )
#endif

/*lint -restore */          
#endif /* CMIDBG_H */

