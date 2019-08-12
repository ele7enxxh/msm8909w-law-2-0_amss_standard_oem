#ifndef TZBSP_SECAPI_H
#define TZBSP_SECAPI_H

/**
@file tzbsp_secapi.h
@brief TZ security level/state header file for struct definition in TZ
*/

/*===========================================================================
   Copyright (c) 2014 by QUALCOMM Technology, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: 
  $DateTime: 2016/06/17 14:31:11 $
  $Author: pwbldsvc $

when       who      what, where, why
--------   ---      ------------------------------------
03/20/14    hw      Initial version.

===========================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <comdef.h>

typedef struct {
  uint32 address;
  uint32 lsb_value;
  uint32 msb_value;
  uint32 lsb_bit_pattern;
  uint32 msb_bit_pattern;
} secctrl_check_fuse_info_t;

#endif /* TZBSP_SECAPI_I_H */
