#ifndef __VSS_PARAMS_DEPRECATED_PUBLIC_IF_H__
#define __VSS_PARAMS_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_params_deprecated_public_if.h
  @brief This file contains deprecated module and parameter IDs supported 
  by Core Voice Drivers.
*/

/*===========================================================================
NOTE: The @brief description above does not appear in the PDF.
      The description that displays in the PDF is located in the
      CVD_mainpage.dox file. Contact Tech Pubs for assistance.
=============================================================================*/
/*===========================================================================
Copyright (c) 2014 Qualcomm Technologies, Inc.
All rights reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

when        who    what, where, why
-------     ---    ----------------------------------------------------------
08/27/14     sw    (Tech Pubs) Updated with Doxygen comments from 2.0.

===========================================================================*/

/** @addtogroup cvd_widevoice_deprecated
@{ */

/****************************************************************************
 * MODULE ID DEFINES                                                        *
 ****************************************************************************/

/** Deprecated. Use #VSS_MODULE_FEATURE_CONFIG for configuring features like
  the WideVoice module.

  WideVoice module ID.
  This module ID uses one parameter.
*/
#define VSS_MODULE_WIDEVOICE ( 0x00011376 )


/****************************************************************************
 * PARAM ID DEFINES                                                         *
 ****************************************************************************/

/** Deprecated. Use #VSS_PARAM_HDVOICE_CONFIG.
 
  ID of the parameter used by VSS_MODULE_WIDEVOICE to set the WideVoice Rx
  sampling rate. The parameter data is a 32-bit value in Hertz.
*/
#define VSS_PARAM_RX_SAMPLING_RATE ( 0x00011377 )

/** @} */ /* end_addtogroup cvd_widevoice_deprecated */

#endif /* __VSS_PARAMS_DEPRECATED_PUBLIC_IF_H__ */

