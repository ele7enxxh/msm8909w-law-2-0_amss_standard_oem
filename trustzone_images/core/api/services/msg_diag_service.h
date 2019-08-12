#ifndef MSG_DIAG_SERVICE_H
#define MSG_DIAG_SERVICE_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
                DIAGNOSTIC MESSAGE SERVICE HEADER FILE

GENERAL DESCRIPTION

  Diag Message macros and functions

Copyright (c) 2008-2010 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================
                        EDIT HISTORY FOR FILE

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/msg_diag_service.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/08/10   vs      Pulled in optimizations for ERR macro
04/02/10   sg      Cleaning up the msg.h inclusion. Moved masks to msg_mask.h
01/22/10   sg      Added Qshrink support 
12/03/09   ps      Fixed compiler warnings & added F3trace support(SW).
09/30/09   as      Ported msg_ts and fixed WINCE featurization 
09/29/09   JV      Removed features FEATURE_ERR_EXTENDED_STORE and
                   FEATURE_SAVE_DEBUG_TRACE from this file
09/08/09   as      Removed support for FEATURE_MSG_IFACE_VIOLATION
08/05/09   JV      Removed the CUST_H featurization around the inclusion of 
                   customer.h.
07/17/09   mad     Merged API headers for commonality.
07/17/09   mad     Un-featurized do_save member of struct msg_const_type
07/17/09   mad     Moved msg_hdr_type and msg_desc_type to this file,removed
                   inclusion of msg_pkt_defs.h
07/17/09   mad     Included customer.h conditionally, included qw.h
05/15/09   mad     Un-featurized do_save member of struct msg_const_type
02/20/09   mad     Added macro MSG_TS and function msg_send_ts(). These let
                   clients pass in a timestamp.(Primarily for use by RETAILMSG
                   or other native-OS Debug message mechanisms that have their
                   own timestamps)
10/23/08   mad     Created from msg.h. Separated out OS-specific re-directions
                   of MSG macros into separate header files.
===========================================================================*/

#if !defined (FEATURE_WINCE)
#include "customer.h"
#endif

#include "comdef.h"            /* Definitions for byte, word, etc. */
#include "qw.h"                /* for qword */
#include "msg_qsr.h"
#include "msgcfg.h"
/*===========================================================================

PACKET       DIAG_EXT_MSG_CONFIG_F

DECRIPTION   This packet configures the extended debug message service.  It
             contains subcommands that are outlined below.
            
===========================================================================*/
#define MSG_EXT_SUBCMD_MIN               (1)

/* query the SSID range of the mobile */
#define MSG_EXT_SUBCMD_GET_RANGES        (1)

/* query the build mask for a specific SSID */
#define MSG_EXT_SUBCMD_GET_BUILD_MASK    (2)

/* query the runtime mask for a specific SSID */
#define MSG_EXT_SUBCMD_GET_RT_MASK       (3)

/* set the runtime mask for a specific SSID */
#define MSG_EXT_SUBCMD_SET_RT_MASK       (4)

/* set the runtime masks for ALL SSIDs */
#define MSG_EXT_SUBCMD_SET_ALL_RT_MASKS  (5)

#define MSG_EXT_SUBCMD_MAX               (5)




/*---------------------------------------------------------------------------
  The extended message packet is defined to be processed as efficiently as
  possible in the caller's context.  Therefore, the packet is divided into
  logical blocks that are aligned w/out declaring the structure as PACK.
  
  A header, static constant block, and argument list are defined to minimize
  the work done by the caller's task.
   
   1. Header
   2. Constant variable length data (format string and filename string, etc).
   3. Arguments ("Variable" variable-length data)
   
  The data is delivered in this order in the packet to simplify the runtime 
  processing of each message.  All constant data is handled in DIAG task 
  context, leaving the caller's task to only process variable data at runtime.
   
  The phone will never process the constant data directly, except to copy 
  format and filename strings. 
---------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------
  All constant information stored for a message.
   
  The values for the fields of this structure are known at compile time. 
  So this is to be defined as a "static " in the MACRO, so it ends up
  being defined and initialized at compile time for each and every message 
  in the software. This minimizes the amount of work to do during run time.
  
  So this structure is to be used in the "caller's" context. "Caller" is the
  client of the Message Services.
---------------------------------------------------------------------------*/
typedef struct
{
  msg_desc_type desc;       /* ss_mask, line, ss_id */
  const char *fmt;      /* Printf style format string */
  const char *fname;        /* Pointer to source file name */
}
msg_const_type;



/*---------------------------------------------------------------------------
  This is the structure that is stored by the caller's task in msg_send ().
  The DIAG task will expand the constant data into the final packet before
  sending to the external device.
---------------------------------------------------------------------------*/
typedef struct
{
  msg_hdr_type hdr;
  const msg_const_type *const_data_ptr; /* desc, file_name, fmt */
  uint32 args[1];
}
msg_ext_store_type;



/*---------------------------------------------------------------------------
  The purpose of this macro is to define the constant part of the message
  that can be initialized at compile time and stored in ROM. This will 
  define and initialize a msg_const_type for each call of a message macro.
  The "static" limits the scope to the file the macro is called from while
  using the macro in a do{}while() guarantees the uniqueness of the name.
---------------------------------------------------------------------------*/

#define XX_MSG_CONST_SAVE(xx_ss_id, xx_ss_mask, xx_fmt) \
    static const msg_const_type xx_msg_const = { \
      {__LINE__, (xx_ss_id), (xx_ss_mask)}, (xx_fmt), msg_file}

  #define XX_MSG_CONST(xx_ss_id, xx_ss_mask, xx_fmt) \
    static const msg_const_type xx_msg_const = { \
	  {__LINE__, (xx_ss_id), (xx_ss_mask)}, (xx_fmt), msg_file}



/*---------------------------------------------------------------------------
  This Macro is used when format string xx_fmt is passed as a variable.at
  runtime instead of a literal.
---------------------------------------------------------------------------*/

#define XX_MSG_CONST_FMT_VAR(xx_ss_id, xx_ss_mask, xx_fmt) \
    const msg_const_type xx_msg_const = { \
	  {__LINE__, (xx_ss_id), (xx_ss_mask)}, (xx_fmt), msg_file}

/*---------------------------------------------------------------------------
  These are the message macros that support messages with  variable number
  of parameters and message text of over 40 characters.
  This is the macro for messages with no params but only a text string.
---------------------------------------------------------------------------*/
#define MSG(xx_ss_id, xx_ss_mask, xx_fmt) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send (&xx_msg_const); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */
/* This is currently supported only on WM */
#define MSG_TS(xx_ss_id, xx_ss_mask, xx_fmt, xx_ts) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_ts (&xx_msg_const, xx_ts); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0)

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameter.
---------------------------------------------------------------------------*/
#define MSG_1(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_1 (&xx_msg_const, (uint32) (xx_arg1)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define MSG_2(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_2 (&xx_msg_const, (uint32)(xx_arg1), (uint32)(xx_arg2)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define MSG_3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_3 (&xx_msg_const, (uint32) (xx_arg1), (uint32) (xx_arg2), \
                                 (uint32) (xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

#define MSG_3_SAVE(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, do_sav) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST_SAVE (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_3 (&xx_msg_const, (uint32) (xx_arg1), (uint32) (xx_arg2), \
                                 (uint32) (xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for messages with 4 parameters. In this case the function
  called needs to have more than 4 parameters so it is going to be a slow 
  function call.  So for this case the  msg_send_var() uses var arg list 
  supported by the compiler.
---------------------------------------------------------------------------*/
#define MSG_4(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(4), (uint32) (xx_arg1), \
               (uint32) (xx_arg2), (uint32) (xx_arg3), (uint32) (xx_arg4)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for messages with 5 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#define MSG_5(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var(&xx_msg_const, (uint32)(5), (uint32)(xx_arg1), \
               (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                  (uint32)(xx_arg5)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for messages with 6 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#define MSG_6(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(6), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for messages with 7 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#define MSG_7(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(7), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0)                                                             \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for messages with 8 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#define MSG_8(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(8), (uint32)(xx_arg1), \
                (uint32)(xx_arg2), (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for messages with 9 parameters. msg_send_var() uses var 
  arg list supported by the compiler.
---------------------------------------------------------------------------*/
#define MSG_9(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8, xx_arg9) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_send_var (&xx_msg_const, (uint32)(9), (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8), \
                                   (uint32)(xx_arg9)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 1 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define MSG_SPRINTF_1(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 2 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define MSG_SPRINTF_2(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define MSG_SPRINTF_3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 4 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define MSG_SPRINTF_4(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 5 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define MSG_SPRINTF_5(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 6 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define MSG_SPRINTF_6(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5), \
               (uint32)(xx_arg6)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 7 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define MSG_SPRINTF_7(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6, \
                                                    xx_arg7 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5), \
               (uint32)(xx_arg6),  (uint32)(xx_arg7)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 8 parameters. msg_sprintf() 
  uses var arg list supported by the compiler.This Macro is used when xx_fmt
  is passed as a literal.
---------------------------------------------------------------------------*/
#define MSG_SPRINTF_8(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                                    xx_arg4, xx_arg5, xx_arg6, \
                                                    xx_arg7, xx_arg8 ); \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const,  (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), \
               (uint32)(xx_arg4),  (uint32)(xx_arg5), \
               (uint32)(xx_arg6),  (uint32)(xx_arg7), \
               (uint32)(xx_arg8)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */


/*---------------------------------------------------------------------------
  This is the macro for sprintf messages with 3 parameters. This Macro is 
  used when xx_fmt is passed at runtime.
---------------------------------------------------------------------------*/
#define MSG_SPRINTF_FMT_VAR_3(xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_MSG_CONST_FMT_VAR (xx_ss_id, xx_ss_mask, xx_fmt); \
      /*lint -e571 */ \
      msg_sprintf (&xx_msg_const, (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
                       /* lint +e717 */

/*---------------------------------------------------------------------------
  The following MACROs are for LEGACY diagnostic messages support.  
---------------------------------------------------------------------------*/

/*===========================================================================

MACRO MSG_FATAL, MSG_ERROR, MSG_HIGH, MSG_MED, MSG_LOW

DESCRIPTION
  Output a message to be sent to be picked up by the Diag Task.  The
  message levels are controlled by selecting the proper macro function.

    MSG_FATAL   fatal
    MSG_ERROR   error
    MSG_HIGH    high
    MSG_MED     medium
    MSG_LOW     low

  Generation of message generating code is controlled by the setting
  of the MSG_LEVEL #define (defined above and on the 'cl' command line).

PARAMETERS
  x_fmt Format string for message (printf style).  Note, this parameter
        must be a string literal (e.g. "Tuned to %lx"), not a variable.
        This is because the value is compiled into ROM, and can clearly
        not be a dynamic data type.
  a     1st parameter for format string
  b     2nd parameter for format string
  c     3rd parameter for format string

DEPENDENCIES
  msg_init() must be called prior to this macro referencing msg_put().
  This macro, is multiple C expressions, and cannot be used as if it
  is a single expression.

RETURN VALUE
  None

SIDE EFFECTS
  On the target hardware, the file and format strings are placed in ROM.

===========================================================================*/
#define MSG_FATAL(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_FATAL, x_fmt, a, b, c)

#define MSG_ERROR(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_ERROR, x_fmt, a, b, c)

#define MSG_HIGH(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_HIGH, x_fmt, a, b, c)

#define MSG_MED(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_MED, x_fmt, a, b, c)

#define MSG_LOW(x_fmt, a, b, c) \
    MSG_3 (MSG_SSID_DFLT, MSG_LEGACY_LOW, x_fmt, a, b, c)


/*===========================================================================

MACRO MSG_FATAL_NO_SAVE, MSG_ERROR_NO_SAVE, MSG_HIGH_NO_SAVE, MSG_MED_NO_SAVE, MSG_LOW_NO_SAVE

DESCRIPTION
  Output a message to be sent to be picked up by the Diag Task.  The
  message levels are controlled by selecting the proper macro function.

    MSG_FATAL_NO_SAVE   fatal
    MSG_ERROR_NO_SAVE   error
    MSG_HIGH_NO_SAVE    high
    MSG_MED_NO_SAVE     medium
    MSG_LOW_NO_SAVE     low

  Generation of message generating code is controlled by the setting
  of the MSG_LEVEL #define (defined above and on the 'cl' command line).

  The use of these macros will cause the const type to not be saved to the
  RAM buffer.

PARAMETERS
  x_fmt Format string for message (printf style).  Note, this parameter
        must be a string literal (e.g. "Tuned to %lx"), not a variable.
        This is because the value is compiled into ROM, and can clearly
        not be a dynamic data type.
  a     1st parameter for format string
  b     2nd parameter for format string
  c     3rd parameter for format string

DEPENDENCIES
  msg_init() must be called prior to this macro referencing msg_put().
  This macro, is multiple C expressions, and cannot be used as if it
  is a single expression.

RETURN VALUE
  None

SIDE EFFECTS
  On the target hardware, the file and format strings are placed in ROM.

===========================================================================*/
#define MSG_FATAL_NO_SAVE(x_fmt, a, b, c) MSG_FATAL (x_fmt, a, b, c)

#define MSG_ERROR_NO_SAVE(x_fmt, a, b, c) MSG_ERROR (x_fmt, a, b, c)

#define MSG_HIGH_NO_SAVE(x_fmt, a, b, c)  MSG_HIGH (x_fmt, a, b, c)

#define MSG_MED_NO_SAVE(x_fmt, a, b, c)   MSG_MED (x_fmt, a, b, c)

#define MSG_LOW_NO_SAVE(x_fmt, a, b, c)   MSG_LOW (x_fmt, a, b, c)

/* Legacy stop-gap solutions to the need for string transport in MSG services. */
#define MSG_STR_ERROR(x_fmt, s, a, b, c)
#define MSG_STR_HIGH(x_fmt, s, a, b, c)
#define MSG_STR_MED(x_fmt, s, a, b, c)
#define MSG_STR_LOW(x_fmt, s, a, b, c)


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* -------------------------------------------------------------------------
   Function Definitions
   ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================

FUNCTION MSG_INIT

DESCRIPTION
  Initialize the message reporting services

DEPENDENCIES
  Can only be called by one task.

RETURN VALUE
  None

SIDE EFFECTS
  Sets the counts to 0, and throws away any buffered messages.

===========================================================================*/
    void msg_init (void);

/*===========================================================================

FUNCTION MSG_SEND

DESCRIPTION
  This will build a new style diagnostic Message with no parameters. 
  Do not call directly; use macro MSG_* defined in msg.h 
  
  Send a message through diag output services.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
  void msg_send (const msg_const_type * xx_msg_const_ptr);

/*===========================================================================

FUNCTION MSG_SEND_1

DESCRIPTION
  This will build a new style diagnostic Message with 1 parameters. 
  Do not call directly; use macro MSG_* defined in msg.h 
  
  Send a message through diag output services.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
  void msg_send_1 (const msg_const_type * xx_msg_const_ptr, uint32 xx_arg1);

/*===========================================================================

FUNCTION MSG_SEND_2

DESCRIPTION
  This will build a new style diagnostic Message with 2 parameters. 
  Do not call directly; use macro MSG_* defined in msg.h 
  
  Send a message through diag output services.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
  void msg_send_2 (const msg_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2);

/*===========================================================================

FUNCTION MSG_SEND_3

DESCRIPTION
  This will build a new style diagnostic Message with 3 parameters. 
  Do not call directly; use macro MSG_* defined in msg.h 
  
  Send a message through diag output services.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
  void msg_send_3 (const msg_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3);

/*===========================================================================

FUNCTION MSG_SEND_VAR

DESCRIPTION
  This will build a new style diagnostic Message with var # (4 to 6) 
  of parameters. 
  Do not call directly; use macro MSG_* defined in msg.h 
  
  Send a message through diag output services.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
  void msg_send_var (const msg_const_type * xx_msg_const_ptr, uint32 num_args,
    ...);

/*===========================================================================

FUNCTION MSG_SPRINTF

DESCRIPTION
  This will build a message sprintf diagnostic Message with var # (1 to 6) 
  of parameters.
  Do not call directly; use macro MSG_SPRINTF_* defined in msg.h 
  
  Send a message through diag output services.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
  void msg_sprintf(const msg_const_type * const_blk,...);

/*===========================================================================

FUNCTION MSG_SEND_TS

DESCRIPTION
  This will build a message with no parameter, and allow user to pass in a timestamp.
  Limited usage: only for Native Debug messages like WM RETAILMSG etc
  Do not call directly, use MSG_TS macro
  
DEPENDENCIES
  msg_init() and Diag_LSM_Init() must have been called previously.  
  A free buffer must be available or the message will be ignored 
  (never buffered).

===========================================================================*/
  void msg_send_ts(const msg_const_type *const_blk, uint64 timestamp);

/*===========================================================================

FUNCTION MSG_SAVE_3
 
DESCRIPTION
  Saves the F3 message to the RAM buffer.  Also called from the ERR_FATAL
  macro.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void msg_save_3(const msg_const_type*,
                uint32, uint32, uint32, msg_ext_store_type*);


/*===========================================================================

FUNCTION       msg_errlog_

DESCRIPTION  These functions send the desired f3 message in addition to 
        invoking err_put_log. This reduces it one function call for top
		level macro.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void msg_errlog_3 (const msg_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3);
void msg_errlog_2 (const msg_const_type* const_blk, uint32 code1, uint32 code2);
void msg_errlog_1 (const msg_const_type* const_blk, uint32 code1);
void msg_errlog_0 (const msg_const_type* const_blk);


#define MSG_ERR_LOG(NbPar, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
do                                                             \
{                                                              \
 /*lint -e506 -e774*/                                          \
 if (MSG_LEGACY_ERROR & (MSG_BUILD_MASK_MSG_SSID_DFLT))        \
 {                                                             \
   /*lint +e506 +e774*/                                        \
   XX_MSG_CONST (MSG_SSID_DFLT, MSG_LEGACY_ERROR, xx_fmt);     \
   /*lint -e571 */                                             \
   switch (NbPar)                                         \
   {                                                      \
     case 3:                                               \
     msg_errlog_3 (&xx_msg_const, (uint32)(xx_arg1), (uint32)(xx_arg2), (uint32)(xx_arg3)); \
     break;                                                \
     case 2:                                               \
     msg_errlog_2 (&xx_msg_const, (uint32)(xx_arg1), (uint32)(xx_arg2));                       \
     break;                                                \
     case 1:                                               \
     msg_errlog_1 (&xx_msg_const, (uint32)(xx_arg1));                \
     break;                                                \
     case 0:                                               \
     msg_errlog_0 (&xx_msg_const);                                   \
     break;                                                \
     default:                                               \
	 break;                                                 \
   }   \
    /*lint +e571 */                                                                         \
 }                                                                                          \
  /*lint -e717 */                                                                           \
} while (0)                                                                                 \
/* lint +e717 */



#ifdef __cplusplus
}
#endif

#endif /* MSG_DIAG_SERVICE_H */
