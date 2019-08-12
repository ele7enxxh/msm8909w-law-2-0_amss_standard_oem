#ifndef BUILDFEATURES_PKGS_H
#define BUILDFEATURES_PKGS_H

/*==============================================================================
  @file BuildFeatures_Pkgs.h

  This file defines the default featureset for 
  DCVS implementations.

  Copyright (c) 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
============================================================================*/
/* $Header: //components/rel/core.mpss/3.5.c12.3/power/dcvs/src/Env/Default/BuildFeatures_Pkgs.h#1 $ */

// Features are removed rather than added.  This file provides some 
// defines to simplify target customization.

/**************************************************************************
 * Full Listing of available Features
 *
 *
 ***********************************************************************/

#if defined(DCVS_PKG_ALTERNATE)
// This package means that all standard source files will be compiled out.
// The selection of the alternate implementation is outside this file.  This 
// section MUST list all features.
// 
#define DEFEATURE_DCVS_ALGO_CLASSIC
#define DEFEATURE_DCVS_ALGO_MDM_CLASSIC
#define DEFEATURE_DCVS_ALGO_QDSP_CLASSIC
#define DEFEATURE_DCVS_ALGO_DISABLED
#define DEFEATURE_DCVS_ALGO_FIXED
#define DEFEATURE_DCVS_ALGO_REQUESTS
#define DEFEATURE_CPU_MON_QDSP
#define DEFEATURE_CPU_MON_ARM

#elif defined(DCVS_PKG_FULL)
// This package defines the maximum set of functionality.  

#elif defined (DCVS_PKG_SMALL)
// This package defines the minimum set of functionality.  It is 
// used on images with extreme memory requirements.
//

#elif defined (DCVS_PKG_QDSP)
// This package defines the functionality for QDSP. It compiles 
// the QDSP versions of core DCVS files.
#define DEFEATURE_CPU_MON_ARM
#define DEFEATURE_DCVS_ALGO_CLASSIC

#elif defined (DCVS_PKG_ARM)
// This package defines the functionality for ARM. It compiles 
// the QDSP versions of core DCVS files.
#define DEFEATURE_CPU_MON_QDSP
#define DEFEATURE_DCVS_ALGO_QDSP_CLASSIC

#endif

#endif // BUILDFEATURES_PKGS_H
