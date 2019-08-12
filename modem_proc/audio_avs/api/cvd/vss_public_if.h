#ifndef __VSS_PUBLIC_IF_H__
#define __VSS_PUBLIC_IF_H__

/**
  @file vss_public_if.h
  @brief This file includes all the CVD header files.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.
=============================================================================*/
/*===========================================================================
Copyright (C) 2011-2014 QUALCOMM Technologies Incorporated
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/audio_avs/api/cvd/vss_public_if.h#1 $
  $Author: mplcsds1 $


===========================================================================*/

/**
  Active interfaces. \n
  These interfaces are fully supported.
*/
#include "vss_common_public_if.h"
#include "vss_params_public_if.h"
#include "vss_iavsync_public_if.h"
#include "vss_iavtimer_public_if.h"
#include "vss_ihdvoice_public_if.h"
#include "vss_imemory_public_if.h"
#include "vss_imvm_public_if.h"
#include "vss_inotify_public_if.h"
#include "vss_ipktexg_public_if.h"
#include "vss_iplayback_public_if.h"
#include "vss_irecord_public_if.h"
#include "vss_istream_public_if.h"
#include "vss_itty_public_if.h"
#include "vss_ittyoob_public_if.h"
#include "vss_iversion_public_if.h"
#include "vss_ivocproc_public_if.h"
#include "vss_ivolume_public_if.h"
#include "vss_ivp3_public_if.h"
#include "vss_ivpcm_public_if.h"
#include "vss_isoundfocus_public_if.h"
#include "vss_isourcetrack_public_if.h"

/**
  Deprecated interfaces. \n
  These interfaces are scheduled for deletion and will no longer be
  supported.
*/
#include "vss_common_deprecated_public_if.h"
#include "vss_params_deprecated_public_if.h"
#include "vss_imvm_deprecated_public_if.h"
#include "vss_istream_deprecated_public_if.h"
#include "vss_ivocproc_deprecated_public_if.h"
#include "vss_iwidevoice_deprecated_public_if.h"

/**
  Deleted interfaces. \n
  These interfaces are not supported. These interfaces were kept to 
  avoid compilation failures. Implementation will not exist.
*/
#include "vss_deleted_public_if.h"

#endif /* __VSS_PUBLIC_IF_H__ */

