#ifndef __VSS_PARAMS_DEPRECATED_PUBLIC_IF_H__
#define __VSS_PARAMS_DEPRECATED_PUBLIC_IF_H__

/**
  @file vss_params_deprecated_public_if.h
  @brief This file contains deprecated module and parameter IDs supported 
  by Core Voice Drivers.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the
      vss_params_deprecated_public_if group description in the
      CVD_mainpage.dox file.
=============================================================================*/
/*===========================================================================
Copyright (C) 2014 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

===========================================================================*/

/** @addtogroup vss_params_deprecated_public_if
@{ */

/****************************************************************************
 * MODULE ID DEFINES                                                        *
 ****************************************************************************/
/** @name Module ID definitions
  @latexonly \label{module_id_definition} @endlatexonly
  @{ */

/** 
  Wide Voice module ID.

  Parameter for this module ID is:
  #VSS_PARAM_SAMPLING_RATE

  @deprecated
  This module is deprecated. Use
  #VSS_MODULE_FEATURE_CONFIG for configuring features like WideVoice.
*/
#define VSS_MODULE_WIDEVOICE ( 0x00011376 )


/** @} */ /* end_name_group Module ID definitions */


/****************************************************************************
 * PARAM ID DEFINES                                                         *
 ****************************************************************************/

/** @name Param ID definitions
  @latexonly \label{param_id_definition} @endlatexonly
  @{ */

/** 
  Parameter for setting the Wide Voice Rx sampling rate.

  @deprecated
  This parameter is deprecated. Use
  #VSS_PARAM_WIDEVOICE_RX_SAMPLING_RATE to specify Rx Sampling Rate for
  WideVoice.

  @comments
  For setting the Wide Voice Rx sampling rate. The parameter data is 32 bit 
  value in hertz.
*/
#define VSS_PARAM_RX_SAMPLING_RATE ( 0x00011377 )

/** @} */ /* end_name_group Param ID definitions */

/** @} */ /* end_addtogroup vss_params_public_if */

#endif /* __VSS_PARAMS_PUBLIC_IF_H__ */

