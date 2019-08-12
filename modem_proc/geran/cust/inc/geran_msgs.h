#ifndef GERAN_MSGS_H
#define GERAN_MSGS_H
/*===========================================================================

          G S M   M S G   M A C R O S   H E A D E R   F I L E

GENERAL DESCRIPTION
  This file contains definitions for the macros that map GERAN debug messages
  to MSG2.0 format and prepend gas_id to Dual SIM builds.

Copyright (c) 2012-2014 Qualcomm Technologies, Inc.
===========================================================================*/

/* <EJECT>^L */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/cust/inc/geran_msgs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
25/05/16   rv      CR998395: Excessive F3 logging in  GERAN L1
24/06/14   df      CR672016: Improve GERAN_FATAL comments
05/11/13   ssh     CR570747: Added _4 and _5 macros.
12-10-08   tjw     Initial revision.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "msg.h"
#include "err.h"

/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/

#ifndef GAS_ID_M
#define GAS_ID_M  (gas_id+1)    /* can be redefined to data_ptr->gas_id or whatever is used in the component */
#endif

#ifndef FEATURE_DUAL_SIM
#define MSG_GERAN_LOW_0_G(txt)                            MSG  (MSG_SSID_DFLT,MSG_LEGACY_LOW,txt)
#define MSG_GERAN_LOW_1_G(txt, arg1)                      MSG_1(MSG_SSID_DFLT,MSG_LEGACY_LOW,txt,arg1)
#define MSG_GERAN_LOW_2_G(txt, arg1, arg2)                MSG_2(MSG_SSID_DFLT,MSG_LEGACY_LOW,txt,arg1,arg2)
#define MSG_GERAN_LOW_3_G(txt, arg1, arg2, arg3)          MSG_3(MSG_SSID_DFLT,MSG_LEGACY_LOW,txt,arg1,arg2,arg3)
#define MSG_GERAN_LOW_4_G(txt, arg1, arg2, arg3, arg4)    MSG_4(MSG_SSID_DFLT,MSG_LEGACY_LOW,txt,arg1,arg2,arg3,arg4)
#define MSG_GERAN_LOW_5_G(txt,arg1,arg2,arg3,arg4,arg5)   MSG_5(MSG_SSID_DFLT,MSG_LEGACY_LOW,txt,arg1,arg2,arg3,arg4,arg5)
#else
#define MSG_GERAN_LOW_0_G(txt)                            MSG_1(MSG_SSID_DFLT,MSG_LEGACY_LOW,"gs%d:" txt,GAS_ID_M)
#define MSG_GERAN_LOW_1_G(txt, arg1)                      MSG_2(MSG_SSID_DFLT,MSG_LEGACY_LOW,"gs%d:" txt,GAS_ID_M,arg1)
#define MSG_GERAN_LOW_2_G(txt, arg1, arg2)                MSG_3(MSG_SSID_DFLT,MSG_LEGACY_LOW,"gs%d:" txt,GAS_ID_M,arg1,arg2)
#define MSG_GERAN_LOW_3_G(txt, arg1, arg2, arg3)          MSG_4(MSG_SSID_DFLT,MSG_LEGACY_LOW,"gs%d:" txt,GAS_ID_M,arg1,arg2,arg3)
#define MSG_GERAN_LOW_4_G(txt, arg1, arg2, arg3, arg4)    MSG_5(MSG_SSID_DFLT,MSG_LEGACY_LOW,"gs%d:" txt,GAS_ID_M,arg1,arg2,arg3,arg4)
#define MSG_GERAN_LOW_5_G(txt,arg1,arg2,arg3,arg4,arg5)   MSG_6(MSG_SSID_DFLT,MSG_LEGACY_LOW,"gs%d:" txt,GAS_ID_M,arg1,arg2,arg3,arg4,arg5)
#endif
#ifndef FEATURE_DUAL_SIM
#define MSG_GERAN_MED_0_G(txt)                            MSG  (MSG_SSID_DFLT,MSG_LEGACY_MED,txt)
#define MSG_GERAN_MED_1_G(txt, arg1)                      MSG_1(MSG_SSID_DFLT,MSG_LEGACY_MED,txt,arg1)
#define MSG_GERAN_MED_2_G(txt, arg1, arg2)                MSG_2(MSG_SSID_DFLT,MSG_LEGACY_MED,txt,arg1,arg2)
#define MSG_GERAN_MED_3_G(txt, arg1, arg2, arg3)          MSG_3(MSG_SSID_DFLT,MSG_LEGACY_MED,txt,arg1,arg2,arg3)
#define MSG_GERAN_MED_4_G(txt, arg1, arg2, arg3, arg4)    MSG_4(MSG_SSID_DFLT,MSG_LEGACY_MED,txt,arg1,arg2,arg3,arg4)
#define MSG_GERAN_MED_5_G(txt,arg1,arg2,arg3,arg4,arg5)   MSG_5(MSG_SSID_DFLT,MSG_LEGACY_MED,txt,arg1,arg2,arg3,arg4,arg5)
#else
#define MSG_GERAN_MED_0_G(txt)                            MSG_1(MSG_SSID_DFLT,MSG_LEGACY_MED,"gs%d:" txt,GAS_ID_M)
#define MSG_GERAN_MED_1_G(txt, arg1)                      MSG_2(MSG_SSID_DFLT,MSG_LEGACY_MED,"gs%d:" txt,GAS_ID_M,arg1)
#define MSG_GERAN_MED_2_G(txt, arg1, arg2)                MSG_3(MSG_SSID_DFLT,MSG_LEGACY_MED,"gs%d:" txt,GAS_ID_M,arg1,arg2)
#define MSG_GERAN_MED_3_G(txt, arg1, arg2, arg3)          MSG_4(MSG_SSID_DFLT,MSG_LEGACY_MED,"gs%d:" txt,GAS_ID_M,arg1,arg2,arg3)
#define MSG_GERAN_MED_4_G(txt, arg1, arg2, arg3, arg4)    MSG_5(MSG_SSID_DFLT,MSG_LEGACY_MED,"gs%d:" txt,GAS_ID_M,arg1,arg2,arg3,arg4)
#define MSG_GERAN_MED_5_G(txt,arg1,arg2,arg3,arg4,arg5)   MSG_6(MSG_SSID_DFLT,MSG_LEGACY_MED,"gs%d:" txt,GAS_ID_M,arg1,arg2,arg3,arg4,arg5)
#endif
#ifndef FEATURE_DUAL_SIM
#define MSG_GERAN_HIGH_0_G(txt)                           MSG  (MSG_SSID_DFLT,MSG_LEGACY_HIGH,txt)
#define MSG_GERAN_HIGH_1_G(txt, arg1)                     MSG_1(MSG_SSID_DFLT,MSG_LEGACY_HIGH,txt,arg1)
#define MSG_GERAN_HIGH_2_G(txt, arg1, arg2)               MSG_2(MSG_SSID_DFLT,MSG_LEGACY_HIGH,txt,arg1,arg2)
#define MSG_GERAN_HIGH_3_G(txt, arg1, arg2, arg3)         MSG_3(MSG_SSID_DFLT,MSG_LEGACY_HIGH,txt,arg1,arg2,arg3)
#define MSG_GERAN_HIGH_4_G(txt, arg1, arg2, arg3, arg4)   MSG_4(MSG_SSID_DFLT,MSG_LEGACY_HIGH,txt,arg1,arg2,arg3,arg4)
#define MSG_GERAN_HIGH_5_G(txt,arg1,arg2,arg3,arg4,arg5)  MSG_5(MSG_SSID_DFLT,MSG_LEGACY_HIGH,txt,arg1,arg2,arg3,arg4,arg5)
#else
#define MSG_GERAN_HIGH_0_G(txt)                           MSG_1(MSG_SSID_DFLT,MSG_LEGACY_HIGH,"gs%d:" txt,GAS_ID_M)
#define MSG_GERAN_HIGH_1_G(txt, arg1)                     MSG_2(MSG_SSID_DFLT,MSG_LEGACY_HIGH,"gs%d:" txt,GAS_ID_M,arg1)
#define MSG_GERAN_HIGH_2_G(txt, arg1, arg2)               MSG_3(MSG_SSID_DFLT,MSG_LEGACY_HIGH,"gs%d:" txt,GAS_ID_M,arg1,arg2)
#define MSG_GERAN_HIGH_3_G(txt, arg1, arg2, arg3)         MSG_4(MSG_SSID_DFLT,MSG_LEGACY_HIGH,"gs%d:" txt,GAS_ID_M,arg1,arg2,arg3)
#define MSG_GERAN_HIGH_4_G(txt, arg1, arg2, arg3, arg4)   MSG_5(MSG_SSID_DFLT,MSG_LEGACY_HIGH,"gs%d:" txt,GAS_ID_M,arg1,arg2,arg3,arg4)
#define MSG_GERAN_HIGH_5_G(txt,arg1,arg2,arg3,arg4,arg5)  MSG_6(MSG_SSID_DFLT,MSG_LEGACY_HIGH,"gs%d:" txt,GAS_ID_M,arg1,arg2,arg3,arg4,arg5)
#endif
#ifndef FEATURE_DUAL_SIM
#define MSG_GERAN_ERROR_0_G(txt)                          MSG  (MSG_SSID_DFLT,MSG_LEGACY_ERROR,txt)
#define MSG_GERAN_ERROR_1_G(txt, arg1)                    MSG_1(MSG_SSID_DFLT,MSG_LEGACY_ERROR,txt,arg1)
#define MSG_GERAN_ERROR_2_G(txt, arg1, arg2)              MSG_2(MSG_SSID_DFLT,MSG_LEGACY_ERROR,txt,arg1,arg2)
#define MSG_GERAN_ERROR_3_G(txt, arg1, arg2, arg3)        MSG_3(MSG_SSID_DFLT,MSG_LEGACY_ERROR,txt,arg1,arg2,arg3)
#define MSG_GERAN_ERROR_4_G(txt, arg1, arg2, arg3, arg4)  MSG_4(MSG_SSID_DFLT,MSG_LEGACY_ERROR,txt,arg1,arg2,arg3,arg4)
#define MSG_GERAN_ERROR_5_G(txt,arg1,arg2,arg3,arg4,arg5) MSG_5(MSG_SSID_DFLT,MSG_LEGACY_ERROR,txt,arg1,arg2,arg3,arg4,arg5)
#else
#define MSG_GERAN_ERROR_0_G(txt)                          MSG_1(MSG_SSID_DFLT,MSG_LEGACY_ERROR,"gs%d:" txt,GAS_ID_M)
#define MSG_GERAN_ERROR_1_G(txt, arg1)                    MSG_2(MSG_SSID_DFLT,MSG_LEGACY_ERROR,"gs%d:" txt,GAS_ID_M,arg1)
#define MSG_GERAN_ERROR_2_G(txt, arg1, arg2)              MSG_3(MSG_SSID_DFLT,MSG_LEGACY_ERROR,"gs%d:" txt,GAS_ID_M,arg1,arg2)
#define MSG_GERAN_ERROR_3_G(txt, arg1, arg2, arg3)        MSG_4(MSG_SSID_DFLT,MSG_LEGACY_ERROR,"gs%d:" txt,GAS_ID_M,arg1,arg2,arg3)
#define MSG_GERAN_ERROR_4_G(txt, arg1, arg2, arg3, arg4)  MSG_5(MSG_SSID_DFLT,MSG_LEGACY_ERROR,"gs%d:" txt,GAS_ID_M,arg1,arg2,arg3,arg4)
#define MSG_GERAN_ERROR_5_G(txt,arg1,arg2,arg3,arg4,arg5) MSG_6(MSG_SSID_DFLT,MSG_LEGACY_ERROR,"gs%d:" txt,GAS_ID_M,arg1,arg2,arg3,arg4,arg5)
#endif
#ifndef FEATURE_DUAL_SIM
/* MSG_GERAN_FATAL_x_G macros print a message to logs, they do not cause a crash or panic reset of GL1 */
#define MSG_GERAN_FATAL_0_G(txt)                          MSG  (MSG_SSID_DFLT,MSG_LEGACY_FATAL,txt)
#define MSG_GERAN_FATAL_1_G(txt, arg1)                    MSG_1(MSG_SSID_DFLT,MSG_LEGACY_FATAL,txt,arg1)
#define MSG_GERAN_FATAL_2_G(txt, arg1, arg2)              MSG_2(MSG_SSID_DFLT,MSG_LEGACY_FATAL,txt,arg1,arg2)
#define MSG_GERAN_FATAL_3_G(txt, arg1, arg2, arg3)        MSG_3(MSG_SSID_DFLT,MSG_LEGACY_FATAL,txt,arg1,arg2,arg3)
#define MSG_GERAN_FATAL_4_G(txt, arg1, arg2, arg3, arg4)  MSG_4(MSG_SSID_DFLT,MSG_LEGACY_FATAL,txt,arg1,arg2,arg3,arg4)
#define MSG_GERAN_FATAL_5_G(txt,arg1,arg2,arg3,arg4,arg5) MSG_5(MSG_SSID_DFLT,MSG_LEGACY_FATAL,txt,arg1,arg2,arg3,arg4,arg5)
#else
#define MSG_GERAN_FATAL_0_G(txt)                          MSG_1(MSG_SSID_DFLT,MSG_LEGACY_FATAL,"gs%d:" txt,GAS_ID_M)
#define MSG_GERAN_FATAL_1_G(txt, arg1)                    MSG_2(MSG_SSID_DFLT,MSG_LEGACY_FATAL,"gs%d:" txt,GAS_ID_M,arg1)
#define MSG_GERAN_FATAL_2_G(txt, arg1, arg2)              MSG_3(MSG_SSID_DFLT,MSG_LEGACY_FATAL,"gs%d:" txt,GAS_ID_M,arg1,arg2)
#define MSG_GERAN_FATAL_3_G(txt, arg1, arg2, arg3)        MSG_4(MSG_SSID_DFLT,MSG_LEGACY_FATAL,"gs%d:" txt,GAS_ID_M,arg1,arg2,arg3)
#define MSG_GERAN_FATAL_4_G(txt, arg1, arg2, arg3, arg4)  MSG_5(MSG_SSID_DFLT,MSG_LEGACY_FATAL,"gs%d:" txt,GAS_ID_M,arg1,arg2,arg3,arg4)
#define MSG_GERAN_FATAL_5_G(txt,arg1,arg2,arg3,arg4,arg5) MSG_6(MSG_SSID_DFLT,MSG_LEGACY_FATAL,"gs%d:" txt,GAS_ID_M,arg1,arg2,arg3,arg4,arg5)
#endif

/* MSG_GERAN_DEBUG_OPT, used for debug purpose - optional */
#define MSG_GERAN_DEBUG_OPT_0_G(txt)                            if(GL1_HW_EFS_DEBUG_ENABLED(GL1_EFS_DEBUG_OPTIONAL)) \
                                                                MSG_GERAN_HIGH_0_G(txt)
#define MSG_GERAN_DEBUG_OPT_1_G(txt, arg1)                      if(GL1_HW_EFS_DEBUG_ENABLED(GL1_EFS_DEBUG_OPTIONAL)) \
                                                                MSG_GERAN_HIGH_1_G(txt, arg1)
#define MSG_GERAN_DEBUG_OPT_2_G(txt, arg1, arg2)                if(GL1_HW_EFS_DEBUG_ENABLED(GL1_EFS_DEBUG_OPTIONAL)) \
                                                                MSG_GERAN_HIGH_2_G(txt, arg1, arg2)
#define MSG_GERAN_DEBUG_OPT_3_G(txt, arg1, arg2, arg3)          if(GL1_HW_EFS_DEBUG_ENABLED(GL1_EFS_DEBUG_OPTIONAL)) \
                                                                MSG_GERAN_HIGH_3_G(txt, arg1, arg2, arg3)
#define MSG_GERAN_DEBUG_OPT_4_G(txt, arg1, arg2, arg3, arg4)    if(GL1_HW_EFS_DEBUG_ENABLED(GL1_EFS_DEBUG_OPTIONAL)) \
                                                                MSG_GERAN_HIGH_4_G(txt, arg1, arg2, arg3, arg4)
#define MSG_GERAN_DEBUG_OPT_5_G(txt,arg1,arg2,arg3,arg4,arg5)   if(GL1_HW_EFS_DEBUG_ENABLED(GL1_EFS_DEBUG_OPTIONAL)) \
                                                                MSG_GERAN_HIGH_5_G(txt,arg1,arg2,arg3,arg4,arg5)

#define MSG_GERAN_LOW_0(txt)                              MSG  (MSG_SSID_DFLT,MSG_LEGACY_LOW,txt)
#define MSG_GERAN_LOW_1(txt, arg1)                        MSG_1(MSG_SSID_DFLT,MSG_LEGACY_LOW,txt,arg1)
#define MSG_GERAN_LOW_2(txt, arg1, arg2)                  MSG_2(MSG_SSID_DFLT,MSG_LEGACY_LOW,txt,arg1,arg2)
#define MSG_GERAN_LOW_3(txt, arg1, arg2, arg3)            MSG_3(MSG_SSID_DFLT,MSG_LEGACY_LOW,txt,arg1,arg2,arg3)
#define MSG_GERAN_LOW_4(txt, arg1, arg2, arg3, arg4)      MSG_4(MSG_SSID_DFLT,MSG_LEGACY_LOW,txt,arg1,arg2,arg3,arg4)
#define MSG_GERAN_LOW_5(txt,arg1,arg2,arg3,arg4,arg5)     MSG_5(MSG_SSID_DFLT,MSG_LEGACY_LOW,txt,arg1,arg2,arg3,arg4,arg5)

#define MSG_GERAN_MED_0(txt)                              MSG  (MSG_SSID_DFLT,MSG_LEGACY_MED,txt)
#define MSG_GERAN_MED_1(txt, arg1)                        MSG_1(MSG_SSID_DFLT,MSG_LEGACY_MED,txt,arg1)
#define MSG_GERAN_MED_2(txt, arg1, arg2)                  MSG_2(MSG_SSID_DFLT,MSG_LEGACY_MED,txt,arg1,arg2)
#define MSG_GERAN_MED_3(txt, arg1, arg2, arg3)            MSG_3(MSG_SSID_DFLT,MSG_LEGACY_MED,txt,arg1,arg2,arg3)
#define MSG_GERAN_MED_4(txt, arg1, arg2, arg3, arg4)      MSG_4(MSG_SSID_DFLT,MSG_LEGACY_MED,txt,arg1,arg2,arg3,arg4)
#define MSG_GERAN_MED_5(txt,arg1,arg2,arg3,arg4,arg5)     MSG_5(MSG_SSID_DFLT,MSG_LEGACY_MED,txt,arg1,arg2,arg3,arg4,arg5)

#define MSG_GERAN_HIGH_0(txt)                             MSG  (MSG_SSID_DFLT,MSG_LEGACY_HIGH,txt)
#define MSG_GERAN_HIGH_1(txt, arg1)                       MSG_1(MSG_SSID_DFLT,MSG_LEGACY_HIGH,txt,arg1)
#define MSG_GERAN_HIGH_2(txt, arg1, arg2)                 MSG_2(MSG_SSID_DFLT,MSG_LEGACY_HIGH,txt,arg1,arg2)
#define MSG_GERAN_HIGH_3(txt, arg1, arg2, arg3)           MSG_3(MSG_SSID_DFLT,MSG_LEGACY_HIGH,txt,arg1,arg2,arg3)
#define MSG_GERAN_HIGH_4(txt, arg1, arg2, arg3, arg4)     MSG_4(MSG_SSID_DFLT,MSG_LEGACY_HIGH,txt,arg1,arg2,arg3,arg4)
#define MSG_GERAN_HIGH_5(txt,arg1,arg2,arg3,arg4,arg5)    MSG_5(MSG_SSID_DFLT,MSG_LEGACY_HIGH,txt,arg1,arg2,arg3,arg4,arg5)

#define MSG_GERAN_ERROR_0(txt)                            MSG  (MSG_SSID_DFLT,MSG_LEGACY_ERROR,txt)
#define MSG_GERAN_ERROR_1(txt, arg1)                      MSG_1(MSG_SSID_DFLT,MSG_LEGACY_ERROR,txt,arg1)
#define MSG_GERAN_ERROR_2(txt, arg1, arg2)                MSG_2(MSG_SSID_DFLT,MSG_LEGACY_ERROR,txt,arg1,arg2)
#define MSG_GERAN_ERROR_3(txt, arg1, arg2, arg3)          MSG_3(MSG_SSID_DFLT,MSG_LEGACY_ERROR,txt,arg1,arg2,arg3)
#define MSG_GERAN_ERROR_4(txt, arg1, arg2, arg3, arg4)    MSG_4(MSG_SSID_DFLT,MSG_LEGACY_ERROR,txt,arg1,arg2,arg3,arg4)
#define MSG_GERAN_ERROR_5(txt,arg1,arg2,arg3,arg4,arg5)   MSG_5(MSG_SSID_DFLT,MSG_LEGACY_ERROR,txt,arg1,arg2,arg3,arg4,arg5)

/* MSG_GERAN_FATAL_x macros print a message to logs, they do not cause a crash or panic reset of GL1 */
#define MSG_GERAN_FATAL_0(txt)                            MSG  (MSG_SSID_DFLT,MSG_LEGACY_FATAL,txt)
#define MSG_GERAN_FATAL_1(txt, arg1)                      MSG_1(MSG_SSID_DFLT,MSG_LEGACY_FATAL,txt,arg1)
#define MSG_GERAN_FATAL_2(txt, arg1, arg2)                MSG_2(MSG_SSID_DFLT,MSG_LEGACY_FATAL,txt,arg1,arg2)
#define MSG_GERAN_FATAL_3(txt, arg1, arg2, arg3)          MSG_3(MSG_SSID_DFLT,MSG_LEGACY_FATAL,txt,arg1,arg2,arg3)

/* ERR_FATAL handling that can reduce memory impact.
   Don't use ERR_FATAL every time (~ 50 bytes) instead call a function that
   calls ERR_FATAL but only prints the line number where the error occurred.
   Less information but looking up the line in the source should be sufficient
   for such a rare occurrence.
   Middle way option expands to MSG_GERAN_FATAL then call local __error_fatal.
*/

/* Uncomment one of these three options: */
/* 1) Macro expands to original ERR_FATAL (with gas_id prefix if selected) */
#define _ERR_GERAN_FATAL_FULL
/* Add this if required */
#define _ERR_GERAN_FATAL_FULL_PREFIX_GAS_ID
/* 2) Macro expands to MSG_GERAN_FATAL then call local __error_fatal fn */
//#define ERR_FATAL_MSG_PLUS_FN
/* 3) Macro expands to call ERR_FATAL but only print the line number where the error occurred */
//#define _ERR_GERAN_FATAL_FN_ONLY

/* Add this if required */
/* call ERR_FATAL if ptr is null but only print the line number where the error occurred */
#define _ERR_GERAN_FATAL_NULL_FN_ONLY

#if defined (_ERR_GERAN_FATAL_FULL)
  /* 1) Original ERR_FATAL */
  #if defined(FEATURE_DUAL_SIM) && defined(_ERR_GERAN_FATAL_FULL_PREFIX_GAS_ID)
    /* prefix gas_id (where possible - ERR_FATAL only permits 3 args) */
    #define ERR_GERAN_FATAL_0_G(txt)                      \
      ERR_FATAL("gs%d:" txt, GAS_ID_M, 0, 0)
    #define ERR_GERAN_FATAL_1_G(txt, arg1)                \
      ERR_FATAL("gs%d:" txt, GAS_ID_M,arg1, 0)
    #define ERR_GERAN_FATAL_2_G(txt, arg1, arg2)          \
      ERR_FATAL("gs%d:" txt, GAS_ID_M,arg1, arg2)
    #define ERR_GERAN_FATAL_3_G(txt, arg1, arg2, arg3)    \
      ERR_FATAL(txt, arg1, arg2, arg3)
  #else
    #define ERR_GERAN_FATAL_0_G(txt)                      \
      ERR_FATAL(txt, 0, 0, 0)
    #define ERR_GERAN_FATAL_1_G(txt, arg1)                \
      ERR_FATAL(txt, arg1, 0, 0)
    #define ERR_GERAN_FATAL_2_G(txt, arg1, arg2)          \
      ERR_FATAL(txt, arg1, arg2, 0)
    #define ERR_GERAN_FATAL_3_G(txt, arg1, arg2, arg3)    \
      ERR_FATAL(txt, arg1, arg2, arg3)
  #endif
  /* Non-gas_id macros */
  #define ERR_GERAN_FATAL_0(txt)                          \
    ERR_FATAL(txt, 0, 0, 0)
  #define ERR_GERAN_FATAL_1(txt, arg1)                    \
    ERR_FATAL(txt, arg1, 0, 0)
  #define ERR_GERAN_FATAL_2(txt, arg1, arg2)              \
    ERR_FATAL(txt, arg1, arg2, 0)
  #define ERR_GERAN_FATAL_3(txt, arg1, arg2, arg3)        \
    ERR_FATAL(txt, arg1, arg2, arg3)

#elif defined (ERR_FATAL_MSG_PLUS_FN)
  /* 2) MSG_GERAN_FATAL then call local __error_fatal fn */
  /* (MSG_GERAN_FATAL_x_G handles DUAL_SIM differences)  */
  #define ERR_GERAN_FATAL_0_G(txt)                        \
    MSG_GERAN_FATAL_0_G(txt);                             \
    __error_fatal(__LINE__)
  #define ERR_GERAN_FATAL_1_G(txt, arg1)                  \
    MSG_GERAN_FATAL_1_G(txt, arg1);                       \
    __error_fatal(__LINE__)
  #define ERR_GERAN_FATAL_2_G(txt, arg1, arg2)            \
    MSG_GERAN_FATAL_2_G(txt, arg1, arg2);                 \
    __error_fatal(__LINE__)
  #define ERR_GERAN_FATAL_3_G(txt, arg1, arg2, arg3)      \
    MSG_GERAN_FATAL_3_G(txt, arg1, arg2, arg3);           \
    __error_fatal(__LINE__)
  /* Non-gas_id macros */
    __error_fatal(__LINE__)
  #define ERR_GERAN_FATAL_0(txt)                          \
    MSG_GERAN_FATAL_0(txt);                               \
    __error_fatal(__LINE__)
  #define ERR_GERAN_FATAL_1(txt, arg1)                    \
    MSG_GERAN_FATAL_1(txt, arg1);                         \
    __error_fatal(__LINE__)
  #define ERR_GERAN_FATAL_2(txt, arg1, arg2)              \
    MSG_GERAN_FATAL_2(txt, arg1, arg2);                   \
    __error_fatal(__LINE__)
  #define ERR_GERAN_FATAL_3(txt, arg1, arg2, arg3)        \
    MSG_GERAN_FATAL_3(txt, arg1, arg2, arg3);             \
    __error_fatal(__LINE__)

#else
/* 3) call ERR_FATAL but only print the line number where the error occurred: */
  #define ERR_GERAN_FATAL_0_G(txt)                        \
    __error_fatal(__LINE__)
  #define ERR_GERAN_FATAL_1_G(txt, arg1)                  \
    __error_fatal(__LINE__)
  #define ERR_GERAN_FATAL_2_G(txt, arg1, arg2)            \
    __error_fatal(__LINE__)
  #define ERR_GERAN_FATAL_3_G(txt, arg1, arg2, arg3)      \
    __error_fatal(__LINE__)
  /* Non-gas_id macros */
  #define ERR_GERAN_FATAL_0(txt)                          \
    __error_fatal(__LINE__)
  #define ERR_GERAN_FATAL_1(txt, arg1)                    \
    __error_fatal(__LINE__)
  #define ERR_GERAN_FATAL_2(txt, arg1, arg2)              \
    __error_fatal(__LINE__)
  #define ERR_GERAN_FATAL_3(txt, arg1, arg2, arg3)        \
    __error_fatal(__LINE__)
#endif

#if defined (_ERR_GERAN_FATAL_NULL_FN_ONLY)
/* call ERR_FATAL but only print the line number where the error occurred: */
  #define NULL_CHECK_FATAL(pp)                            \
    if (pp == NULL)                                       \
    {                                                     \
      __error_fatal(__LINE__);                            \
    }
#else /* Use ERR_GERAN_FATAL to handle it */
  #define NULL_CHECK_FATAL(pp)                            \
    if (pp == NULL)                                       \
    {                                                     \
      ERR_GERAN_FATAL_0("Null ptr #pp");                  \
    }
#endif


/* Calling this function will generate an F3 with the file name and the line
   number where this function is defined. The arg gives the line where the
   actual error occurred.
*/
/*
   The compiler will optimise out this function if it's not used by the file
   including this header. Suppress the warning if it's not used.
*/
#ifdef __GNUC__
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wunused-function"
#endif
static void __error_fatal(uint32 lineNum)
{
  ERR_FATAL("Fatal error at line %d", lineNum, 0, 0);
}
#ifdef __GNUC__
  #pragma clang diagnostic pop
#endif

#endif /* GERAN_MSGS_H */
