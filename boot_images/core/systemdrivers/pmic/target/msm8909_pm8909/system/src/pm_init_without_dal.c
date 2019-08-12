#include "com_dtypes.h"
#include "pm_target_information.h"
#include "SpmiCfg.h"
#include "pm_sbl_boot.h"
#include "SpmiBus.h"

pm_err_flag_type pm_pon_channel_config(void)
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    // 0x0: slave Id, 0x08: peripheral Id, 4: channel, 0: Owner, APPS
    SpmiCfg_ChannelCfg pon_cfg = {0x0, 0x08, 1, 0}; // copied from msm8909/pm_8909/pm_config_target.c, PON
    Spmi_Result spmi_err_flag = SPMI_BUS_SUCCESS;

    do
    {
        spmi_err_flag = SpmiCfg_Init(TRUE);
        if ( spmi_err_flag != SPMI_SUCCESS )
        {
            err_flag = PM_ERR_FLAG__SPMI_OPT_ERR;
            break;
        }

        spmi_err_flag |= SpmiCfg_ConfigureChannels(&pon_cfg, 1);
        if ( spmi_err_flag != SPMI_SUCCESS )
        {
            err_flag = PM_ERR_FLAG__SPMI_OPT_ERR;
            break;
        }

        spmi_err_flag = SpmiBus_Init();
        if ( spmi_err_flag != SPMI_SUCCESS )
        {
            err_flag = PM_ERR_FLAG__SPMI_OPT_ERR;
            break;
        }
    } while(0);

    return err_flag;
}

