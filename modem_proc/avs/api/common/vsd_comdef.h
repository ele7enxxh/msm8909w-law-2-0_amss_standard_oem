#ifndef __VSD_COMDEF_H__
#define __VSD_COMDEF_H__

/**
  @file vsd_comdef.h
  @brief This file contains VSD (Voice Services and Drivers) common
  definitions.
*/

/*===========================================================================
Copyright (C) 2013 QUALCOMM Technologies, Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/common/vsd_comdef.h#1 $
  $Author: mplcsds1 $

===========================================================================*/

#include "mmdefs.h"

/** @addtogroup vsd_comdef
@{ */

/****************************************************************************
 * EXPORT MACROS                                                            *
 ****************************************************************************/

#ifdef __cplusplus
  #define VSD_PRIVATE extern "C"
#else
  #define VSD_PRIVATE extern
#endif /* __cplusplus */
  /**< This marks an internal API that is not intended for public use. */

#ifdef __cplusplus
  #define VSD_PUBLIC extern "C"
#else
  #define VSD_PUBLIC extern
#endif /* __cplusplus */
  /**< This marks an external API that is intended for public use. */


/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/
 
/** @brief Type used for voice return codes.
*/
typedef uint32_t vsd_status_t;


/** @} */ /* end_addtogroup vsd_comdef */

#endif /* __VSD_COMDEF_H__ */