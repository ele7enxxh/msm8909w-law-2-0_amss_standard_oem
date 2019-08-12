#ifndef TZBSP_COUNTER_H
#define TZBSP_COUNTER_H

/**
@file tzbsp_counter.h
@brief This is an implementation of counter service in tzbsp
*/

/*===========================================================================
   Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/securemsm/trustzone/qsee/include/tzbsp_counter.h#1 $
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
05/25/13   yh      initial version

===========================================================================*/
#include <comdef.h>

/* tzbsp_counter_set_folder must be called 
   1. Only once from sfs init function before any other counter functions 
   2. Any tzbsp counter call before SFS API call must set root_folder value
      same as core\securemsm\trustzone\qsapps\libs\custom\src\qsee_oem_utils.c
      qsee_sfs_set_root_path return value */
int tzbsp_counter_set_folder(const char *root_folder);
int tzbsp_create_counter(uint8 *identifier, uint64 initial_value);
int tzbsp_query_counter(uint8 *identifier, uint64 *current_value);
int tzbsp_increment_counter(uint8 *identifier);
int tzbsp_delete_counter(uint8 *identifier);
int tzbsp_query_rpmb_version(uint64 *current_value);

#endif /* TZBSP_COUNTER_H */
