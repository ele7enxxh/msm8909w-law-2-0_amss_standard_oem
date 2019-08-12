#ifndef SNS_SAM_PROV_SMGRI_H
#define SNS_SAM_PROV_SMGRI_H

/*============================================================================
  @file sns_sam_prov_smgri.h

  @brief
  Data structures and constants to be used in reference to the SMGRI
  Provider.

  @note
  File line length should generally be limited to <= 80 columns.

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

#define SNS_ISLAND_INCLUDE_PROV_SMGRI
#ifdef SNS_ISLAND_INCLUDE_PROV_SMGRI
#define SNS_SAM_UIMAGE_CODE __attribute__((section (".text.PROV_SMGRI")))
#define SNS_SAM_UIMAGE_DATA __attribute__((section (".data.PROV_SMGRI")))
#else
#define SNS_SAM_UIMAGE_CODE
#define SNS_SAM_UIMAGE_DATA
#endif /* SNS_USES_ISLAND */

/*============================================================================
  Type Declarations
  ===========================================================================*/

#endif /* SNS_SAM_PROV_SMGRI_H */

