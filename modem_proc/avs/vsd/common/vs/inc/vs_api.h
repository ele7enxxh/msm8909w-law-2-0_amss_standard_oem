#ifndef __VS_API_H__
#define __VS_API_H__

/**
  @file vs_api.h
  @brief 
   
*/

/*
  ============================================================================

   Copyright (C) 2013 QUALCOMM Technologies Incorporated. All Rights Reserved.
   QUALCOMM Proprietary and Confidential

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/vs/inc/vs_api.h#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  04/16/13  sud   fixed bugs. 
  01/02/13  sud   Initial revision

  ============================================================================
*/

#include "mmdefs.h"

typedef enum vs_callindex_enum_t
{
  /* public */  VS_CALLINDEX_ENUM_UNINITIALIZED = 0,
  /* public */  VS_CALLINDEX_ENUM_INIT = 1,
  /* public */  VS_CALLINDEX_ENUM_POSTINIT = 2,
  /* public */  VS_CALLINDEX_ENUM_PREDEINIT = 3,
  /* public */  VS_CALLINDEX_ENUM_DEINIT = 4,
  /* private */ VS_CALLINDEX_ENUM_RUN = 5,
  /* public */  VS_CALLINDEX_ENUM_INVALID
}
  vs_callindex_enum_t;

#endif /* __VS_API_H__ */

