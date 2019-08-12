#ifndef __VS_H__
#define __VS_H__

/**
  @file vs.h
  @brief This is the public header file that Clients of Voice 
    	 Services (VS) should include. This file includes all
    	 other VS public header files.
*/

/*
  ============================================================================

   Copyright (C) 2012, 2014 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/vs/vs.h#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  12/05/12  sud   Initial revision

  ============================================================================
*/

/*----------------------------------------------------------------------------
 * VS VERSION
 *--------------------------------------------------------------------------*/

/** @addtogroup vs_common
@{ */

/** VS major version. */
#define VS_VERSION_MAJOR ( 1 )
/** VS minor version. */
#define VS_VERSION_MINOR ( 0 )

/** @} */ /* end_addtogroup vs_common */

/*----------------------------------------------------------------------------
 * Include files for Module
 *--------------------------------------------------------------------------*/

#include "vs_common.h"
#include "vs_voc.h"
#include "vs_vocamr.h"
#include "vs_vocamrwb.h"
#include "vs_vocfr.h"
#include "vs_vocefr.h"
#include "vs_vochr.h"
#include "vs_vocevrca.h"
#include "vs_vocevrcb.h"
#include "vs_vocevrcnw.h"
#include "vs_vocevrcnw2k.h"
#include "vs_vocevrcwb.h"
#include "vs_vocg711alaw.h"
#include "vs_vocg711mulaw.h"
#include "vs_vocg729.h"
#include "vs_vocqcelp13k.h"

#endif /* __VS_H__ */

