#ifndef SNS_DEBUG_STR_H
#define SNS_DEBUG_STR_H

/*============================================================================

@file
sns_debug_str.h

@brief
Contains macros and definitions required for printing
debug strings on the DSPS. The file has the same name as the apps
counterpart

Copyright (c) 2010-2016 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

/*===========================================================================

                     EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/debug/inc/sns_debug_str.h#1 $

when        who    what, where, why
(mm/dd/yyyy)
--------   ----    ----------------------------------------------------------
02/09/2016  gju     Remove sns_debug_printf_dsps_string_id
01/22/2015   DK     Added SNS_UMSG_ macros
01/16/2015   VY     Moved uImage logging to debug thread
11/26/2014   VY     Added api to disable logging
2014-07-22   ps     Fix compiler warnings
07/20/2014   VY     Disabled logging in uImage
07/15/2013   vh     Changed MODULE definition FILE to FILENAME
11/21/2012   ps     Added SNS_PRINTF_STRING_<PRI>_X Macros
01/26/2012   ag     Pass only the filename rather than filename with path, in debug messages
12/22/2010   sj     Fixed incorrect array index
12/09/2010   sj     Fixed debug.idl file based on code review comments. Hence changes
                    to C code to match those.
09/28/2010   sj     Compilation fixes
09/27/2010   sj     Fixes for capturing the appropriate
                    filename of caller
09/21/2010   sj     Created to support debug strings from DSPS
===========================================================================*/

/*=====================================================================
                 INCLUDE FILES
=======================================================================*/
#include "sns_debug_interface_v01.h"
#include "sns_debug_api.h"
#include "sns_smgr_ddf_if.h"
#include <stdbool.h>
#include <stddef.h>



#ifdef FAST_PLAYBACK
  #include <stdio.h>
  #define SNS_DEBUG_UIMG

  #define MSG_SSID_SNS    0        // Arbitrarily set values (they are ignored)
  #define DBG_LOW_PRIO    0        // Arbitrarily set values (they are ignored)
  #define DBG_MED_PRIO    0        // Arbitrarily set values (they are ignored)
  #define DBG_HIGH_PRIO   0        // Arbitrarily set values (they are ignored)
  #define DBG_ERROR_PRIO  0        // Arbitrarily set values (they are ignored)
  #define DBG_FATAL_PRIO  0        // Arbitrarily set values (they are ignored)

  // Set the filename width to 25 characters
  // Left-pad the line number and set the width to 5 digits

//#define FAST_PLAYBACK_DEBUG

#ifdef FAST_PLAYBACK_DEBUG
  #define MSG(a,b,c)                  printf("%25s  %05u  " c "\n",__FILENAME__,__LINE__)
  #define MSG_1(a,b,c,d)              printf("%25s  %05u  " c "\n",__FILENAME__,__LINE__,d)
  #define MSG_2(a,b,c,d,e)            printf("%25s  %05u  " c "\n",__FILENAME__,__LINE__,d,e)
  #define MSG_3(a,b,c,d,e,f)          printf("%25s  %05u  " c "\n",__FILENAME__,__LINE__,d,e,f)
  #define MSG_4(a,b,c,d,e,f,g)        printf("%25s  %05u  " c "\n",__FILENAME__,__LINE__,d,e,f,g)
  #define MSG_5(a,b,c,d,e,f,g,h)      printf("%25s  %05u  " c "\n",__FILENAME__,__LINE__,d,e,f,g,h)
#else /* FAST_PLAYBACK_DEBUG */
  #define MSG(a,b,c)
  #define MSG_1(a,b,c,d)
  #define MSG_2(a,b,c,d,e)
  #define MSG_3(a,b,c,d,e,f)
  #define MSG_4(a,b,c,d,e,f,g)
  #define MSG_5(a,b,c,d,e,f,g,h)
#endif /* FAST_PLAYBACK_DEBUG */
  //stub func
  STATIC bool sns_ddf_smgr_is_in_uimage(void){ return false;}
  STATIC uint8_t sns_debug_is_module_disabled(sns_debug_module_id_e module_id){return 0;}
#endif /* FAST_PLAYBACK */

#define __MODULE__ __FILENAME__

/* Debug string priority defines */
#define SNS_MSG_LOW 0
#define SNS_MSG_MEDIUM 1
#define SNS_MSG_HIGH 2
#define SNS_MSG_FATAL 3
#define SNS_MSG_ERROR 4

/* Originating processor defines */
#define PROCESSOR_DSPS 1

#ifdef UIMG_LOG
#define SNS_DEBUG_LOGPKT_COMMIT_SIG          0x00800000
#define SNS_DEBUG_LOGPKT_DROP_SIG            0x01000000

typedef struct
{
  sns_q_link_s   link;
  void          *log_pkt_msg;
  uint32_t       pkt_type;
  uint32_t       pkt_size;
} logpkt_hold_item_s;

typedef struct
{
  sns_q_s     hold_queue;
  OS_EVENT   *q_mutex_ptr;
} logpkt_hold_buffer_s;
#endif /* UIMG_LOG */

#ifndef FAST_PLAYBACK
uint8_t sns_debug_is_module_disabled(sns_debug_module_id_e module_id);
#endif /* FAST_PLAYBACK */

void sns_logging_status_override(bool log_override);

/* Enable the uImage debug messages by compiling with "-f SNS_UIMG_MSGS" option */

#define SNS_DEBUG_UIMG
#if defined SNS_DEBUG_UIMG
  #define SNS_UMSG_LOW_0(module, msg) SNS_PRINTF_STRING_LOW_0(module, msg)
  #define SNS_UMSG_LOW_1(module, msg, arg0) SNS_PRINTF_STRING_LOW_1(module, msg, arg0)
  #define SNS_UMSG_LOW_2(module, msg, arg0, arg1) SNS_PRINTF_STRING_LOW_2(module, msg, arg0, arg1)
  #define SNS_UMSG_LOW_3(module, msg, arg0, arg1, arg2) SNS_PRINTF_STRING_LOW_3(module, msg, arg0, arg1, arg2)

  #define SNS_UMSG_MEDIUM_0(module, msg) SNS_PRINTF_STRING_MEDIUM_0(module, msg)
  #define SNS_UMSG_MEDIUM_1(module, msg, arg0) SNS_PRINTF_STRING_MEDIUM_1(module, msg, arg0)
  #define SNS_UMSG_MEDIUM_2(module, msg, arg0, arg1) SNS_PRINTF_STRING_MEDIUM_2(module, msg, arg0, arg1)
  #define SNS_UMSG_MEDIUM_3(module, msg, arg0, arg1, arg2) SNS_PRINTF_STRING_MEDIUM_3(module, msg, arg0, arg1, arg2)

  #define SNS_UMSG_HIGH_0(module, msg) SNS_PRINTF_STRING_HIGH_0(module, msg)
  #define SNS_UMSG_HIGH_1(module, msg, arg0) SNS_PRINTF_STRING_HIGH_1(module, msg, arg0)
  #define SNS_UMSG_HIGH_2(module, msg, arg0, arg1) SNS_PRINTF_STRING_HIGH_2(module, msg, arg0, arg1)
  #define SNS_UMSG_HIGH_3(module, msg, arg0, arg1, arg2) SNS_PRINTF_STRING_HIGH_3(module, msg, arg0, arg1, arg2)

  #define SNS_UMSG_ERROR_0(module, msg) SNS_PRINTF_STRING_ERROR_0(module, msg)
  #define SNS_UMSG_ERROR_1(module, msg, arg0) SNS_PRINTF_STRING_ERROR_1(module, msg, arg0)
  #define SNS_UMSG_ERROR_2(module, msg, arg0, arg1) SNS_PRINTF_STRING_ERROR_2(module, msg, arg0, arg1)
  #define SNS_UMSG_ERROR_3(module, msg, arg0, arg1, arg2) SNS_PRINTF_STRING_ERROR_3(module, msg, arg0, arg1, arg2)

  #define SNS_UMSG_FATAL_0(module, msg) SNS_PRINTF_STRING_FATAL_0(module, msg)
  #define SNS_UMSG_FATAL_1(module, msg, arg0) SNS_PRINTF_STRING_FATAL_1(module, msg, arg0)
  #define SNS_UMSG_FATAL_2(module, msg, arg0, arg1) SNS_PRINTF_STRING_FATAL_2(module, msg, arg0, arg1)
  #define SNS_UMSG_FATAL_3(module, msg, arg0, arg1, arg2) SNS_PRINTF_STRING_FATAL_3(module, msg, arg0, arg1, arg2)
#else
  #define SNS_UMSG_LOW_0(module, msg) (void)0
  #define SNS_UMSG_LOW_1(module, msg, arg0) (void)0
  #define SNS_UMSG_LOW_2(module, msg, arg0, arg1) (void)0
  #define SNS_UMSG_LOW_3(module, msg, arg0, arg1, arg2) (void)0

  #define SNS_UMSG_MEDIUM_0(module, msg) (void)0
  #define SNS_UMSG_MEDIUM_1(module, msg, arg0) (void)0
  #define SNS_UMSG_MEDIUM_2(module, msg, arg0, arg1) (void)0
  #define SNS_UMSG_MEDIUM_3(module, msg, arg0, arg1, arg2) (void)0

  #define SNS_UMSG_HIGH_0(module, msg) (void)0
  #define SNS_UMSG_HIGH_1(module, msg, arg0) (void)0
  #define SNS_UMSG_HIGH_2(module, msg, arg0, arg1) (void)0
  #define SNS_UMSG_HIGH_3(module, msg, arg0, arg1, arg2) (void)0

  #define SNS_UMSG_ERROR_0(module, msg) (void)0
  #define SNS_UMSG_ERROR_1(module, msg, arg0) (void)0
  #define SNS_UMSG_ERROR_2(module, msg, arg0, arg1) (void)0
  #define SNS_UMSG_ERROR_3(module, msg, arg0, arg1, arg2) (void)0

  #define SNS_UMSG_FATAL_0(module, msg) (void)0
  #define SNS_UMSG_FATAL_1(module, msg, arg0) (void)0
  #define SNS_UMSG_FATAL_2(module, msg, arg0, arg1) (void)0
  #define SNS_UMSG_FATAL_3(module, msg, arg0, arg1, arg2) (void)0
#endif /* SNS_DEBUG_UIMG */

/* Enable the required Module ID in /data/misc/sensors/sensors_dbg_config.txt */
/* Low Priority */
#define SNS_PRINTF_STRING_LOW_0(mod_id,str) \
       {  \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))  \
          {  \
              MSG(MSG_SSID_SNS,DBG_LOW_PRIO,str);   \
          }  \
       }
#define SNS_PRINTF_STRING_LOW_1(mod_id,str,parameter1) \
       {  \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))  \
          {  \
              MSG_1(MSG_SSID_SNS,DBG_LOW_PRIO,str,parameter1); \
          }  \
       }
#define SNS_PRINTF_STRING_LOW_2(mod_id,str,parameter1,parameter2) \
       { \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))  \
          {  \
              MSG_2(MSG_SSID_SNS, DBG_LOW_PRIO, str,parameter1,parameter2); \
          }  \
       }
#define SNS_PRINTF_STRING_LOW_3(mod_id,str,parameter1,parameter2,parameter3) \
       { \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))  \
          {  \
              MSG_3(MSG_SSID_SNS, DBG_LOW_PRIO, str,parameter1,parameter2,parameter3); \
          }  \
       }

/* Medium priority */
#define SNS_PRINTF_STRING_MEDIUM_0(mod_id,str)  \
       {  \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))  \
          {  \
              MSG(MSG_SSID_SNS, DBG_MED_PRIO, str);  \
          }  \
       }
#define SNS_PRINTF_STRING_MEDIUM_1(mod_id,str,parameter1) \
       {  \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))   \
          {  \
              MSG_1(MSG_SSID_SNS,DBG_MED_PRIO,str,parameter1); \
          }  \
       }
#define SNS_PRINTF_STRING_MEDIUM_2(mod_id,str,parameter1,parameter2) \
       { \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))  \
          {  \
              MSG_2(MSG_SSID_SNS, DBG_MED_PRIO, str,parameter1,parameter2); \
          }  \
       }
#define SNS_PRINTF_STRING_MEDIUM_3(mod_id,str,parameter1,parameter2,parameter3) \
       { \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))  \
          {  \
              MSG_3(MSG_SSID_SNS, DBG_MED_PRIO, str,parameter1,parameter2,parameter3); \
          }  \
       }

/* High priority */
#define SNS_PRINTF_STRING_HIGH_0(mod_id,str) \
       {  \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))  \
          {  \
              MSG(MSG_SSID_SNS, DBG_HIGH_PRIO, str); \
          }  \
       }
#define SNS_PRINTF_STRING_HIGH_1(mod_id,str,parameter1) \
       {  \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))   \
          {  \
              MSG_1(MSG_SSID_SNS,DBG_HIGH_PRIO,str,parameter1); \
          }  \
       }
#define SNS_PRINTF_STRING_HIGH_2(mod_id,str,parameter1,parameter2) \
       { \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))  \
          {  \
              MSG_2(MSG_SSID_SNS, DBG_HIGH_PRIO, str,parameter1,parameter2); \
          }  \
       }
#define SNS_PRINTF_STRING_HIGH_3(mod_id,str,parameter1,parameter2,parameter3) \
       { \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))  \
          {  \
              MSG_3(MSG_SSID_SNS, DBG_HIGH_PRIO, str,parameter1,parameter2,parameter3); \
          }  \
       }

/* Fatal Priority */
#define SNS_PRINTF_STRING_FATAL_0(mod_id,str) \
       {  \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))  \
          {  \
              MSG(MSG_SSID_SNS, DBG_FATAL_PRIO, str); \
          }  \
       }
#define SNS_PRINTF_STRING_FATAL_1(mod_id,str,parameter1) \
       {  \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))   \
          {  \
              MSG_1(MSG_SSID_SNS,DBG_FATAL_PRIO,str,parameter1); \
          }  \
       }
#define SNS_PRINTF_STRING_FATAL_2(mod_id,str,parameter1,parameter2) \
       { \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))  \
          {  \
              MSG_2(MSG_SSID_SNS, DBG_FATAL_PRIO, str,parameter1,parameter2); \
          }  \
       }
#define SNS_PRINTF_STRING_FATAL_3(mod_id,str,parameter1,parameter2,parameter3) \
       { \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))  \
          {  \
              MSG_3(MSG_SSID_SNS, DBG_FATAL_PRIO, str,parameter1,parameter2,parameter3); \
          }  \
       }

/* Error priority */
#define SNS_PRINTF_STRING_ERROR_0(mod_id,str) \
       {  \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))  \
          {  \
              MSG(MSG_SSID_SNS, DBG_ERROR_PRIO, str); \
          }  \
       }
#define SNS_PRINTF_STRING_ERROR_1(mod_id,str,parameter1) \
       {  \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))   \
          {  \
              MSG_1(MSG_SSID_SNS,DBG_ERROR_PRIO,str,parameter1); \
          }  \
       }
#define SNS_PRINTF_STRING_ERROR_2(mod_id,str,parameter1,parameter2) \
       { \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))  \
          {  \
              MSG_2(MSG_SSID_SNS, DBG_ERROR_PRIO, str,parameter1,parameter2); \
          }  \
       }
#define SNS_PRINTF_STRING_ERROR_3(mod_id,str,parameter1,parameter2,parameter3) \
       { \
          if(!sns_ddf_smgr_is_in_uimage() && !sns_debug_is_module_disabled(mod_id))  \
          {  \
              MSG_3(MSG_SSID_SNS, DBG_ERROR_PRIO, str,parameter1,parameter2,parameter3); \
          }  \
       }


/*
 * The debug printf macros for DSPS and Apps processor where only the string ID
 * is provided. These macros send a QMI indication message to apps processor to
 * print out the debug string.
 *
 * These macros are DEPRECATED, and should not be used in new code.
 */

#define SNS_PRINTF_STRING_ID_LOW_0(mod_id,str_id) \
  { SNS_PRINTF_STRING_LOW_0(mod_id,""); }
#define SNS_PRINTF_STRING_ID_LOW_1(mod_id,str_id,parameter1) \
  { SNS_PRINTF_STRING_LOW_1(mod_id,"%i",parameter1); }
#define SNS_PRINTF_STRING_ID_LOW_2(mod_id,str_id,parameter1,parameter2) \
  { SNS_PRINTF_STRING_LOW_2(mod_id,"%i %i",parameter1,parameter2); }
#define SNS_PRINTF_STRING_ID_LOW_3(mod_id,str_id,parameter1,parameter2,parameter3) \
  { SNS_PRINTF_STRING_LOW_3(mod_id,"%i %i %i",parameter1,parameter2,parameter3); }

#define SNS_PRINTF_STRING_ID_MEDIUM_0(mod_id,str_id) \
  { SNS_PRINTF_STRING_MEDIUM_0(mod_id,""); }
#define SNS_PRINTF_STRING_ID_MEDIUM_1(mod_id,str_id,parameter1) \
  { SNS_PRINTF_STRING_MEDIUM_1(mod_id,"%i",parameter1); }
#define SNS_PRINTF_STRING_ID_MEDIUM_2(mod_id,str_id,parameter1,parameter2) \
  { SNS_PRINTF_STRING_MEDIUM_2(mod_id,"%i %i",parameter1,parameter2); }
#define SNS_PRINTF_STRING_ID_MEDIUM_3(mod_id,str_id,parameter1,parameter2,parameter3) \
  { SNS_PRINTF_STRING_MEDIUM_3(mod_id,"%i %i %i",parameter1,parameter2,parameter3); }

#define SNS_PRINTF_STRING_ID_HIGH_0(mod_id,str_id) \
  { SNS_PRINTF_STRING_HIGH_0(mod_id,""); }
#define SNS_PRINTF_STRING_ID_HIGH_1(mod_id,str_id,parameter1) \
  { SNS_PRINTF_STRING_HIGH_1(mod_id,"%i",parameter1); }
#define SNS_PRINTF_STRING_ID_HIGH_2(mod_id,str_id,parameter1,parameter2) \
  { SNS_PRINTF_STRING_HIGH_2(mod_id,"%i %i",parameter1,parameter2); }
#define SNS_PRINTF_STRING_ID_HIGH_3(mod_id,str_id,parameter1,parameter2,parameter3) \
  { SNS_PRINTF_STRING_HIGH_3(mod_id,"%i %i %i",parameter1,parameter2,parameter3); }

#define SNS_PRINTF_STRING_ID_ERROR_0(mod_id,str_id) \
  { SNS_PRINTF_STRING_ERROR_0(mod_id,""); }
#define SNS_PRINTF_STRING_ID_ERROR_1(mod_id,str_id,parameter1) \
  { SNS_PRINTF_STRING_ERROR_1(mod_id,"%i",parameter1); }
#define SNS_PRINTF_STRING_ID_ERROR_2(mod_id,str_id,parameter1,parameter2) \
  { SNS_PRINTF_STRING_ERROR_2(mod_id,"%i %i",parameter1,parameter2); }
#define SNS_PRINTF_STRING_ID_ERROR_3(mod_id,str_id,parameter1,parameter2,parameter3) \
  { SNS_PRINTF_STRING_ERROR_3(mod_id,"%i %i %i",parameter1,parameter2,parameter3); }

#define SNS_PRINTF_STRING_ID_FATAL_0(mod_id,str_id) \
  { SNS_PRINTF_STRING_FATAL_0(mod_id,""); }
#define SNS_PRINTF_STRING_ID_FATAL_1(mod_id,str_id,parameter1) \
  { SNS_PRINTF_STRING_FATAL_1(mod_id,"%i",parameter1); }
#define SNS_PRINTF_STRING_ID_FATAL_2(mod_id,str_id,parameter1,parameter2) \
  { SNS_PRINTF_STRING_FATAL_2(mod_id,"%i %i",parameter1,parameter2); }
#define SNS_PRINTF_STRING_ID_FATAL_3(mod_id,str_id,parameter1,parameter2,parameter3) \
  { SNS_PRINTF_STRING_FATAL_3(mod_id,"%i %i %i",parameter1,parameter2,parameter3); }

#ifdef UIMG_LOG
extern sns_err_code_e sns_debug_logpkt_commit_cntl(void *qmi_log_pkt_msg,
                                                   uint32_t pkt_type,
                                                   uint32_t pkt_size);

extern void sns_debug_signal_me(uint32_t sig);

extern void sns_debug_unsignal_me(uint32_t sig);

extern void sns_debug_logpkt_drop(void);

extern void sns_debug_logpkt_commit_send(void);

#endif /* UIMG_LOG */

#endif /* SNS_DEBUG_STR_H */
