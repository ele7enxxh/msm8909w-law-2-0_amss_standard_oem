#ifndef SNS_SAM_PROV_SMGR_H
#define SNS_SAM_PROV_SMGR_H

/*============================================================================
  @file sns_sam_prov_smgr.h

  @brief
  Data structures and constants to be used in reference to the SMGR
  Provider.

  @note
  File line length should generally be limited to <= 80 columns.

  @note
  Dynamically linked algorithms should be compiled with SNS_QDSS_SWE undefined.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "qmi_idl_lib.h"
#include "sns_sam_prov_api.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/
#define SNS_ISLAND_INCLUDE_PROV_SMGR
#ifdef SNS_ISLAND_INCLUDE_PROV_SMGR
#define SNS_SAM_UIMAGE_CODE __attribute__((section (".text.PROV_SMGR")))
#define SNS_SAM_UIMAGE_DATA __attribute__((section (".data.PROV_SMGR")))
#else
#define SNS_SAM_UIMAGE_CODE
#define SNS_SAM_UIMAGE_DATA
#endif /* SNS_USES_ISLAND */

/*============================================================================
  Type Declarations
  ===========================================================================*/

#endif /* SNS_SAM_PROV_SMGR_H */

