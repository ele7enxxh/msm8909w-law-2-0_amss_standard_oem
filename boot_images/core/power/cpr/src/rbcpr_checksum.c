#include "rbcpr.h"
#include "../../utils/inc/checksum.h"
#include "railway.h"
#include "CoreVerify.h"


extern bool regular_init_done;


static uint16 rbcpr_setting_checksum(uint32 rail_id)
{
    rbcpr_bsp_rail_info *rail = NULL;

    CORE_VERIFY(rail_id < rbcpr_bsp_data.num_rails);

    rail = &(rbcpr_bsp_data.rails[rail_id]);

    CORE_VERIFY(rail->bsp_data);
    CORE_VERIFY(rail->bsp_data->rbcpr_enablement != RBCPR_DISABLED);

    const rbcpr_bsp_rail_params_type * const rail_params = rail->bsp_data;
    const rbcpr_bsp_rail_const_info * const const_info   = rail->rail_const_info;

        const checksum_data_t data_arr[] =
        {
            {
                .data = (uint8*)(rail_params->target_params),
                .size = sizeof(rbcpr_corner_params_type) * rail_params->number_of_target_params,
            },
            {
                .data = (uint8*)&(rail_params->rbcpr_enablement),
                .size = sizeof(rbcpr_enablement_type),
            },
            {
                .data = (uint8*)&(rail_params->number_of_corners_to_settle_at),
                .size = sizeof(uint8),
            },
            {
                .data = (uint8*)(rail_params->corners_to_settle_at),
                .size = sizeof(railway_corner) * rail_params->number_of_corners_to_settle_at,
            },
            {
                .data = (uint8*)&(const_info->step_quot),
                .size = sizeof(uint8),
            },
            {
                .data = (uint8*)&(const_info->up_threshold),
                .size = sizeof(uint8),
            },
            {
                .data = (uint8*)&(const_info->dn_threshold),
                .size = sizeof(uint8),
            },
            {
                .data = (uint8*)&(const_info->count_mode),
                .size = sizeof(HAL_rbcpr_count_mode),
            }
        };

    return checksum16(data_arr, sizeof(data_arr)/sizeof(data_arr[0]));
}

uint32 rbcpr_cx_mx_settings_checksum(void)
{
    uint16 data[RBCPR_MAX_CHECKSUMS] = {0};
    uint32 i;

    /*
     * This function MUST be used after rbcpr_init()
     */
    CORE_VERIFY(regular_init_done);
    CORE_VERIFY(rbcpr_checksum_config_data.num_configs <= RBCPR_MAX_CHECKSUMS);

    for(i = 0; i < rbcpr_checksum_config_data.num_configs; i++)
    {
        if(rbcpr_checksum_config_data.configs[i].rbcpr_rail_id != RAIL_NOT_SUPPORTED_BY_RBCPR)
        {
            data[i] = rbcpr_setting_checksum(rbcpr_checksum_config_data.configs[i].rbcpr_rail_id);
        }
        else
        {
            data[i] = railway_setting_checksum(rbcpr_checksum_config_data.configs[i].railway_rail_id);
        }
    }

    return checksum32(data, i);
}

