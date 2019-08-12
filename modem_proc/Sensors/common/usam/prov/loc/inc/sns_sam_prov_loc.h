#ifndef SNS_SAM_PROV_LOC_H
#define SNS_SAM_PROV_LOC_H

/*============================================================================
  @file sns_sam_prov_loc.h

  @brief
  Data structures and constants to be used in reference to the QMI_LOC
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

#define SNS_SAM_PROV_LOC_POS_SUID 0x47c95382d05949d8
#define SNS_SAM_PROV_LOC_SV_INFO_SUID 0xcc2a108ba82ce3dc

#ifdef SNS_ISLAND_INCLUDE_PROV_LOC
#define SNS_SAM_UIMAGE_CODE __attribute__((section (".text.PROV_LOC")))
#define SNS_SAM_UIMAGE_DATA __attribute__((section (".data.PROV_LOC")))
#else
#define SNS_SAM_UIMAGE_CODE
#define SNS_SAM_UIMAGE_DATA
#endif /* SNS_USES_ISLAND */

/*============================================================================
  Type Declarations
  ===========================================================================*/

#endif /* SNS_SAM_PROV_LOC_H */

