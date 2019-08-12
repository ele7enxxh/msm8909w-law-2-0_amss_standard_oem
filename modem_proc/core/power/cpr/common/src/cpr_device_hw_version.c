/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "cpr_device_hw_version.h"
#include "cpr_image.h"
#include "CoreVerify.h"
#include "DALSys.h"
#include "DALSysTypes.h"
#include "HAL_cpr_v2.h"


uint8 cpr_char_rev;
uint32 cpr_chip_version;
cpr_foundry_id cpr_foundry;

void cpr_device_hw_version_init(void)
{
    static bool cpr_device_hw_version_init_done = false;
    if(cpr_device_hw_version_init_done)
    {
        return;
    }
    cpr_device_hw_version_init_done = true;

    cpr_chip_version = cpr_image_get_chip_info_version();

    cpr_foundry = cpr_image_get_foundry_info();
  
    cpr_char_rev = HAL_cpr_get_cpr_rev();
}

bool cpr_device_hw_version_matches_this_device(const cpr_config_hw_version_range* hw_range)
{
    cpr_device_hw_version_init();
    DalChipInfoFamilyType familyVersion;
    
    CORE_VERIFY(familyVersion = DalChipInfo_ChipFamily());

    for(int j=0; j<hw_range->foundry_range_count; j++)
    {
        const cpr_config_foundry_range* foundry_range = &hw_range->foundry_range[j];
        if((cpr_foundry == foundry_range->foundry_id) &&
           (cpr_chip_version >= foundry_range->min_version) && (cpr_chip_version < foundry_range->max_version) &&
           (cpr_char_rev >= foundry_range->min_char_rev) && (cpr_char_rev < foundry_range->max_char_rev))
        {
            if(!foundry_range->ChipfamilyVersion || familyVersion == foundry_range->ChipfamilyVersion)
              return true;
        }
    }
    return false;
}

