#ifndef MSG_QSR_H
#define MSG_QSR_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
         
                EXTENDED DIAGNOSTIC QSHRINK MESSAGE SERVICE HEADER FILE

GENERAL DESCRIPTION

  All the declarations and definitions necessary to support the optimized f3 
  messages for errors and debugging.  This includes support for the 
  extended capabilities as well as the legacy messaging scheme.

Copyright (c) 2010 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR FILE

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/msg_qsr.h#1 $

when       who     what, where, why
--------   ---   ----------------------------------------------------------
06/08/10   vs      Fixed compile error in QSR_MSG_ERR_LOG
05/20/10   vs      Changed QSR_ERR_LOG to QSR_MSG_ERR_LOG
04/08/10   vs      Changes to include memory optimizations for ERR macro
04/01/10   sg      Removed the undefines for QSR_MSG_LOW,MEDIUM,HIGH,ERROR,
                   FATAL
10/11/09   vg      8200 Warning Clean Up
20/05/09   ps      Created new file for qsr feature based on msg.h

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"		       /* Definitions for byte, word, etc. */
#include "msg_pkt_defs.h"
#include "msgcfg.h" 


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
  msg_desc_type desc;	/* ss_mask, line, ss_id */
  uint32 msg_hash;		 /* message hash value */
}
msg_qsr_const_type;

typedef struct
{
  msg_qsr_const_type qsr_const_blk;
  const char *fname;
} err_msg_qsr_const_type;

 /*---------------------------------------------------------------------------
	This is the structure that is stored by the caller's task in msg_send ().
	The DIAG task will expand the constant data into the final packet before
	sending to the external device.
 ---------------------------------------------------------------------------*/
  typedef struct
  {
	msg_hdr_type hdr;
    const msg_qsr_const_type* qsr_const_data_ptr; /* desc */
    uint32 args[1];
  }
  msg_qsr_store_type;

          
  #define XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash) \
	static const msg_qsr_const_type xx_msg_qsr_const = { \
	  {__LINE__, (xx_ss_id), (xx_ss_mask)}, hash}

  #define XX_QSR_MSG_CONST_FMT_VAR(xx_ss_id, xx_ss_mask,hash) \
	const msg_qsr_const_type xx_msg_qsr_const = { \
	  {__LINE__, (xx_ss_id), (xx_ss_mask)}, hash}

  #define XX_ERR_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash) \
	static const err_msg_qsr_const_type xx_err_msg_qsr_const = { \
	  {{__LINE__, (xx_ss_id), (xx_ss_mask)}, hash}, msg_file}



/*---------------------------------------------------------------------------
  These are the message macros that support messages with  variable number
  of parameters and message text of over 40 characters.
  This is the macro for messages with no params but only a text string.
---------------------------------------------------------------------------*/
#define QSR_MSG(hash, xx_ss_id, xx_ss_mask, xx_fmt) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send (&xx_msg_qsr_const); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
				       /* lint +e717 */

/*---------------------------------------------------------------------------
  Macro for messages with 1 parameter.
---------------------------------------------------------------------------*/
#define QSR_MSG_1(hash, xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_1 (&xx_msg_qsr_const, (uint32)(xx_arg1)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
				       /* lint +e717 */

/*---------------------------------------------------------------------------
  Macro for messages with 2 parameters.
---------------------------------------------------------------------------*/
#define QSR_MSG_2(hash,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_2 (&xx_msg_qsr_const,(uint32)(xx_arg1), (uint32)(xx_arg2)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
				       /* lint +e717 */

/*---------------------------------------------------------------------------
  This is the macro for messages with 3 parameters.
---------------------------------------------------------------------------*/
#define QSR_MSG_3(hash,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_3( &xx_msg_qsr_const,(uint32) (xx_arg1), (uint32) (xx_arg2), \
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
#define QSR_MSG_4(hash,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_var (&xx_msg_qsr_const, (uint32)(4), (uint32) (xx_arg1), \
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
#define QSR_MSG_5(hash,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_var(&xx_msg_qsr_const, (uint32)(5), (uint32)(xx_arg1), \
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
#define QSR_MSG_6(hash,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_var (&xx_msg_qsr_const, (uint32)(6), (uint32)(xx_arg1), \
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
#define QSR_MSG_7(hash,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_var(&xx_msg_qsr_const, (uint32)(7), (uint32)(xx_arg1), \
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
#define QSR_MSG_8(hash,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_var(&xx_msg_qsr_const, (uint32)(8), (uint32)(xx_arg1), \
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
#define QSR_MSG_9(hash,xx_ss_id, xx_ss_mask, xx_fmt, xx_arg1, xx_arg2, xx_arg3, \
                                            xx_arg4, xx_arg5, xx_arg6, \
                                            xx_arg7, xx_arg8, xx_arg9) \
  do { \
    /*lint -e506 -e774*/ \
    if (xx_ss_mask & (MSG_BUILD_MASK_ ## xx_ss_id)) { \
    /*lint +e506 +e774*/ \
      XX_QSR_MSG_CONST(xx_ss_id, xx_ss_mask, hash); \
      /*lint -e571 */ \
      qsr_msg_send_var(&xx_msg_qsr_const, (uint32)(9), (uint32)(xx_arg1), \
               (uint32)(xx_arg2),  (uint32)(xx_arg3), (uint32)(xx_arg4), \
                                   (uint32)(xx_arg5), (uint32)(xx_arg6), \
                                   (uint32)(xx_arg7), (uint32)(xx_arg8), \
                                   (uint32)(xx_arg9)); \
      /*lint +e571 */ \
    } \
  /*lint -e717 */ \
  } while (0) \
				       /* lint +e717 */


// TODO: MSG_EXPENSIVE: on target string expansion (name?)

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

#define QSR_MSG_FATAL(hash,x_fmt, a, b, c) \
    QSR_MSG_3 (hash,MSG_SSID_DFLT, MSG_LEGACY_FATAL, x_fmt, a, b, c)

#define QSR_MSG_ERROR(hash, x_fmt, a, b, c) \
    QSR_MSG_3 (hash, MSG_SSID_DFLT, MSG_LEGACY_ERROR, x_fmt, a, b, c)

#define QSR_MSG_HIGH(hash, x_fmt, a, b, c) \
    QSR_MSG_3 (hash, MSG_SSID_DFLT, MSG_LEGACY_HIGH, x_fmt, a, b, c)

#define QSR_MSG_MED(hash,x_fmt, a, b, c) \
    QSR_MSG_3 (hash,MSG_SSID_DFLT, MSG_LEGACY_MED, x_fmt, a, b, c)

#define QSR_MSG_LOW(hash,x_fmt, a, b, c) \
    QSR_MSG_3 (hash,MSG_SSID_DFLT, MSG_LEGACY_LOW, x_fmt, a, b, c)



/* -------------------------------------------------------------------------
   Function Definitions
   ------------------------------------------------------------------------- */

#ifdef __cplusplus
extern "C"
{
#endif

/*===========================================================================

FUNCTION QSR_MSG_SEND

DESCRIPTION
  This will build a new style diagnostic Message with no parameters. 
  Do not call directly; use macro MSG_* defined in msg.h 
  
  Send a message through diag output services.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
  void qsr_msg_send ( const msg_qsr_const_type * xx_msg_const_ptr);

/*===========================================================================

FUNCTION QSR_MSG_SEND_1

DESCRIPTION
  This will build a new style diagnostic Message with 1 parameters. 
  Do not call directly; use macro MSG_* defined in msg.h 
  
  Send a message through diag output services.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
  void qsr_msg_send_1 (const msg_qsr_const_type * xx_msg_const_ptr, uint32 xx_arg1);

/*===========================================================================

FUNCTION QSR_MSG_SEND_2

DESCRIPTION
  This will build a new style diagnostic Message with 2 parameters. 
  Do not call directly; use macro MSG_* defined in msg.h 
  
  Send a message through diag output services.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
  void qsr_msg_send_2 ( const msg_qsr_const_type * xx_msg_const_ptr,uint32 xx_arg1,
    uint32 xx_arg2);

/*===========================================================================

FUNCTION QSR_MSG_SEND_3

DESCRIPTION
  This will build a new style diagnostic Message with 3 parameters. 
  Do not call directly; use macro MSG_* defined in msg.h 
  
  Send a message through diag output services.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
  void qsr_msg_send_3 ( const msg_qsr_const_type * xx_msg_const_ptr, uint32 xx_arg1,
    uint32 xx_arg2, uint32 xx_arg3);

/*===========================================================================

FUNCTION QSR_MSG_SEND_VAR

DESCRIPTION
  This will build a new style diagnostic Message with var # (4 to 6) 
  of parameters. 
  Do not call directly; use macro MSG_* defined in msg.h 
  
  Send a message through diag output services.

DEPENDENCIES
  msg_init() must have been called previously.  A free buffer must
  be available or the message will be ignored (never buffered).

===========================================================================*/
  void qsr_msg_send_var ( const msg_qsr_const_type * xx_msg_const_ptr, uint32 num_args, ...);


/*===========================================================================

FUNCTION       msg_qsrerrlog_

DESCRIPTION    These functions send the desired f3 message in addition to 
        invoking err_put_log. This reduces it one function call for top
		level macro.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void msg_qsrerrlog_3 (const err_msg_qsr_const_type* const_blk, uint32 code1, uint32 code2, uint32 code3);
void msg_qsrerrlog_2 (const err_msg_qsr_const_type* const_blk, uint32 code1, uint32 code2);
void msg_qsrerrlog_1 (const err_msg_qsr_const_type* const_blk, uint32 code1);
void msg_qsrerrlog_0 (const err_msg_qsr_const_type* const_blk);


#define QSR_MSG_ERR_LOG(hash, NbPar, xx_fmt, xx_arg1, xx_arg2, xx_arg3) \
do \
{  \
 /*lint -e506 -e774*/ \
 if (MSG_LEGACY_ERROR & (MSG_BUILD_MASK_MSG_SSID_DFLT)) \
 { \
 /*lint +e506 +e774*/ \
  XX_ERR_QSR_MSG_CONST (MSG_SSID_DFLT, MSG_LEGACY_ERROR, hash); \
   /*lint -e571 */ \
  switch (NbPar)                                           \
  {                                                          \
    case 3:                                                   \
    msg_qsrerrlog_3 (&xx_err_msg_qsr_const, (uint32)xx_arg1,(uint32) xx_arg2,(uint32) xx_arg3);     \
    break;                                                    \
    case 2:                                                   \
    msg_qsrerrlog_2 (&xx_err_msg_qsr_const,(uint32) xx_arg1, (uint32)xx_arg2); \
    break;                                                    \
    case 1:                                                   \
    msg_qsrerrlog_1 (&xx_err_msg_qsr_const, (uint32)xx_arg1);         \
    break;                                                    \
    case 0:                                                   \
    msg_qsrerrlog_0 (&xx_err_msg_qsr_const);                          \
    break;                                                    \
	 default:                                               \
	 break;                                                 \
  }                   \
      /*lint +e571 */ \
 } \
  /*lint -e717 */ \
} while (0) \
/* lint +e717 */


#ifdef __cplusplus
}
#endif

#endif	/* MSG_QSR_H */

