#include "../../utils/inc/checksum.h"
#include "railway.h"
#include "railway_config.h"
#include "CoreVerify.h"


uint16 railway_setting_checksum(uint32 rail_id)
{
    CORE_VERIFY(rail_id < RAILWAY_CONFIG_DATA->num_rails);

    railway_rail_config_t *rail = &(RAILWAY_CONFIG_DATA->rails[rail_id]);

    const checksum_data_t data_arr[] =
    {
        {
            .data = (uint8*)(rail->supported_corners),
            .size = sizeof(railway_corner) * rail->supported_corners_count,
        },
        {
            .data = (uint8*)(rail->default_uvs),
            .size = sizeof(unsigned[RAILWAY_CORNERS_COUNT]),
        },
    };

    return checksum16(data_arr, sizeof(data_arr)/sizeof(data_arr[0]));
}

