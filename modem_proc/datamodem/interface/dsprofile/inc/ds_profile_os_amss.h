/******************************************************************************
  @file    ds_profile_os_amss.h
  @brief   Operating System specific header

  DESCRIPTION
  This header defines API for OS (AMSS) specific logging, locking mechanisms. 

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  N/A

  ---------------------------------------------------------------------------
  Copyright (C) 2009-2016 Qualcomm Technologies Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  ---------------------------------------------------------------------------
******************************************************************************/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsprofile/inc/ds_profile_os_amss.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/21/10   vk      Support for apn based profile search
04/30/10   ars     Made modem_mem_alloc/free and str fns platform independent
09/30/09   mg      Created the module. First version of the file.
===========================================================================*/

#ifndef DS_PROFILE_OS_AMSS_H
#define DS_PROFILE_OS_AMSS_H

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "rex.h"
#include "data_msg.h"

#include <stringl/stringl.h>
  #include "modem_mem.h"


typedef rex_crit_sect_type  plm_lock_type;

int ds_profile_log_init(  char *lib );
int ds_profile_lock_init( plm_lock_type *lock );

int ds_profile_lock_acq( plm_lock_type *lock );
int ds_profile_lock_rel( plm_lock_type *lock );

#define DS_PROFILE_LOGD(format, arg1) DATA_MSG1( MSG_LEGACY_MED, format, arg1 )
#define DS_PROFILE_LOGE(format, arg1) DATA_MSG1( MSG_LEGACY_ERROR, format, arg1 ) 

#define DS_PROFILE_LOGD_2(format, arg1, arg2) \
  DATA_MSG2( MSG_LEGACY_MED, format, arg1, arg2)
#define DS_PROFILE_LOGD_3(format, arg1, arg2, arg3) \
  DATA_MSG3( MSG_LEGACY_MED, format, arg1, arg2, arg3 )
#define DS_PROFILE_LOGD_4(format, arg1, arg2, arg3, arg4) \
  DATA_MSG4( MSG_LEGACY_MED, format, arg1, arg2, arg3, arg4 )
#define DS_PROFILE_LOGD_5(format, arg1, arg2, arg3, arg4, arg5) \
  DATA_MSG5( MSG_LEGACY_MED, format, arg1, arg2, arg3, arg4, arg5 )

  #define DS_PROFILE_MEM_ALLOC( size, client ) modem_mem_alloc( size, client )
  #define DS_PROFILE_MEM_FREE( ptr, client ) modem_mem_free( ptr, client )

#define DS_PROFILE_STR_LEN( str ) strlen( str )
#define DS_PROFILE_STR_CPY( str1, str2, size ) strlcpy( str1, str2, size )
#define DS_PROFILE_STR_CMP(str1, str2, len ) strncmp( str1, str2, len )
#define DS_PROFILE_STR_CASE_CMP(str1, str2, len ) strncasecmp( str1, str2, len )
#define DS_PROFILE_STR_PRINTF snprintf

#endif /* DS_PROFILE_OS_AMSS_H */
