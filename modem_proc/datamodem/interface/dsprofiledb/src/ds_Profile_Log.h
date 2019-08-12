#ifndef DS_PROFILE_DB_LOG_H
#define DS_PROFILE_DB_LOG_H
/*===========================================================================
  @file ds_Profile_Log.h

  This file defines macros used for logging throughout DS Profile DB code
  
                Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-07-13 am Created module  

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

    
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_Utils_DebugMsg.h"
#include "msg.h"

#define DB_LOG_FORMAT ""

/* error messages */
#define PROFILE_DB_LOG_ERROR(format) \
  PRINT_MSG_0(MSG_LEGACY_ERROR, DB_LOG_FORMAT format);

#define PROFILE_DB_LOG_ERROR_1(format, a1) \
  PRINT_MSG_1(MSG_LEGACY_ERROR, DB_LOG_FORMAT format, a1);

#define PROFILE_DB_LOG_ERROR_2(format, a1, a2) \
  PRINT_MSG_2(MSG_LEGACY_ERROR, DB_LOG_FORMAT format, a1, a2);

#define PROFILE_DB_LOG_ERROR_3(format, a1, a2, a3) \
  PRINT_MSG_3(MSG_LEGACY_ERROR, DB_LOG_FORMAT format, a1, a2, a3);

#define PROFILE_DB_LOG_ERROR_4(format, a1, a2, a3, a4) \
  PRINT_MSG_4(MSG_LEGACY_ERROR, DB_LOG_FORMAT format, a1, a2, a3, a4);

#define PROFILE_DB_LOG_ERROR_5(format, a1, a2, a3, a4, a5) \
  PRINT_MSG_5(MSG_LEGACY_ERROR, DB_LOG_FORMAT format, a1, a2, a3, a4, a5);

#define PROFILE_DB_LOG_ERROR_6(format, a1, a2, a3, a4, a5, a6) \
  PRINT_MSG_6(MSG_LEGACY_ERROR, DB_LOG_FORMAT format, a1, a2, a3, a4, a5, a6);

#define PROFILE_DB_LOG_ERROR_7(format, a1, a2, a3, a4, a5, a6, a7) \
  PRINT_MSG_7(MSG_LEGACY_ERROR, DB_LOG_FORMAT format, a1, a2, a3, a4, a5, a6, a7);

#define PROFILE_DB_LOG_ERROR_8(format, a1, a2, a3, a4, a5, a6, a7, a8) \
  PRINT_MSG_8(MSG_LEGACY_ERROR, DB_LOG_FORMAT format, a1, a2, a3, a4, a5, a6, a7, a8);

#define PROFILE_DB_LOG_ERROR_9(format, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
  PRINT_MSG_9(MSG_LEGACY_ERROR, DB_LOG_FORMAT format, a1, a2, a3, a4, a5, a6, a7, a8, a9);

/* high info messages */
#define PROFILE_DB_LOG_HIGH(format) \
  PRINT_MSG_0(MSG_LEGACY_HIGH, DB_LOG_FORMAT format);

#define PROFILE_DB_LOG_HIGH_1(format, a1) \
  PRINT_MSG_1(MSG_LEGACY_HIGH, DB_LOG_FORMAT format, a1);

#define PROFILE_DB_LOG_HIGH_2(format, a1, a2) \
  PRINT_MSG_2(MSG_LEGACY_HIGH, DB_LOG_FORMAT format, a1, a2);

#define PROFILE_DB_LOG_HIGH_3(format, a1, a2, a3) \
  PRINT_MSG_3(MSG_LEGACY_HIGH, DB_LOG_FORMAT format, a1, a2, a3);

#define PROFILE_DB_LOG_HIGH_4(format, a1, a2, a3, a4) \
  PRINT_MSG_4(MSG_LEGACY_HIGH, DB_LOG_FORMAT format, a1, a2, a3, a4);

#define PROFILE_DB_LOG_HIGH_5(format, a1, a2, a3, a4, a5) \
  PRINT_MSG_5(MSG_LEGACY_HIGH, DB_LOG_FORMAT format, a1, a2, a3, a4, a5);

#define PROFILE_DB_LOG_HIGH_6(format, a1, a2, a3, a4, a5, a6) \
  PRINT_MSG_6(MSG_LEGACY_HIGH, DB_LOG_FORMAT format, a1, a2, a3, a4, a5, a6);

#define PROFILE_DB_LOG_HIGH_7(format, a1, a2, a3, a4, a5, a6, a7) \
  PRINT_MSG_7(MSG_LEGACY_HIGH, DB_LOG_FORMAT format, a1, a2, a3, a4, a5, a6, a7);

#define PROFILE_DB_LOG_HIGH_8(format, a1, a2, a3, a4, a5, a6, a7, a8) \
  PRINT_MSG_8(MSG_LEGACY_HIGH, DB_LOG_FORMAT format, a1, a2, a3, a4, a5, a6, a7, a8);

#define PROFILE_DB_LOG_HIGH_9(format, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
  PRINT_MSG_9(MSG_LEGACY_HIGH, DB_LOG_FORMAT format, a1, a2, a3, a4, a5, a6, a7, a8, a9);

#define PROFILE_DB_LOG_HIGH_STR_1(format, a1) \
  PRINT_MSG_STR_1(MSG_LEGACY_ERROR, DB_LOG_FORMAT format, a1);


/* med info messages */
#define PROFILE_DB_LOG_MED(format) \
  PRINT_MSG_0(MSG_LEGACY_MED, DB_LOG_FORMAT format);

#define PROFILE_DB_LOG_MED_1(format, a1) \
  PRINT_MSG_1(MSG_LEGACY_MED, DB_LOG_FORMAT format, a1);

#define PROFILE_DB_LOG_MED_2(format, a1, a2) \
  PRINT_MSG_2(MSG_LEGACY_MED, DB_LOG_FORMAT format, a1, a2);

#define PROFILE_DB_LOG_MED_3(format, a1, a2, a3) \
  PRINT_MSG_3(MSG_LEGACY_MED, DB_LOG_FORMAT format, a1, a2, a3);

#define PROFILE_DB_LOG_MED_4(format, a1, a2, a3, a4) \
  PRINT_MSG_4(MSG_LEGACY_MED, DB_LOG_FORMAT format, a1, a2, a3, a4);

#define PROFILE_DB_LOG_MED_5(format, a1, a2, a3, a4, a5) \
  PRINT_MSG_5(MSG_LEGACY_MED, DB_LOG_FORMAT format, a1, a2, a3, a4, a5);

#define PROFILE_DB_LOG_MED_6(format, a1, a2, a3, a4, a5, a6) \
  PRINT_MSG_6(MSG_LEGACY_MED, DB_LOG_FORMAT format, a1, a2, a3, a4, a5, a6);

#define PROFILE_DB_LOG_MED_7(format, a1, a2, a3, a4, a5, a6, a7) \
  PRINT_MSG_7(MSG_LEGACY_MED, DB_LOG_FORMAT format, a1, a2, a3, a4, a5, a6, a7);

#define PROFILE_DB_LOG_MED_8(format, a1, a2, a3, a4, a5, a6, a7, a8) \
  PRINT_MSG_8(MSG_LEGACY_MED, DB_LOG_FORMAT format, a1, a2, a3, a4, a5, a6, a7, a8);

#define PROFILE_DB_LOG_MED_9(format, a1, a2, a3, a4, a5, a6, a7, a8, a9) \
  PRINT_MSG_9(MSG_LEGACY_MED, DB_LOG_FORMAT format, a1, a2, a3, a4, a5, a6, a7, a8, a9);


/* additional helper macros */
#define PROFILE_DB_LOG_ERROR_ON_EXIT(res) \
  if (DB_RESULT_SUCCESS != res) { \
    PROFILE_DB_LOG_ERROR_1("error occured %d: ", res); \
  } 

#endif /* DS_PROFILE_DB_PLATFORM_MODEM_H */
