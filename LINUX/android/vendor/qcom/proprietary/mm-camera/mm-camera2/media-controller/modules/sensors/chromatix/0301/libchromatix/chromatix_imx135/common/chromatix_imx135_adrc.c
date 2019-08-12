
/*============================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

/*============================================================================
 *                      INCLUDE FILES
 *===========================================================================*/
#include "chromatix_adrc.h"
#include "sensor_dbg.h"

static chromatix_adrc_type chromatix_imx135_adrc_parms = {
#include "chromatix_imx135_adrc.h"
};

/*============================================================================
 * FUNCTION    - load_chromatix -
 *
 * DESCRIPTION:
 *==========================================================================*/
void *load_chromatix(void)
{
  SLOW("chromatix ptr %p", &chromatix_imx135_adrc_parms);
  return &chromatix_imx135_adrc_parms;
}
