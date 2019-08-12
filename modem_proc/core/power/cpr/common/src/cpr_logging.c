/*===========================================================================

Copyright (c) 2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary and Confidential.

===========================================================================*/
#include "cpr_logging.h"

#if defined(MPSS_CPR_BUILD) || defined(WINSIM)
ULogHandle cpr_ulog_h;   
ULogHandle cpr_ulog_static_h;
#endif


void cpr_logging_init()
{
 #if defined(MPSS_CPR_BUILD) || defined(WINSIM)
    //create the log and log handle
    if(cpr_ulog_h==NULL){
        ULogFront_RealTimeInit(&cpr_ulog_h, "CprLog", 8192, ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
    }

    if (cpr_ulog_static_h == NULL){
        //create the static log and log handle for statistics
        ULogFront_RealTimeStaticInit(&cpr_ulog_static_h, "CprStatsLog", CPR_ULOG_STATIC_NUM_SLOTS, ULOG_MEMORY_LOCAL, ULOG_LOCK_OS);
    }
#else

#endif
}


char* cpr_logging_get_ascii_rail_id_name(cpr_rail_id_t rail_id){
    switch (rail_id){
        case CPR_RAIL_MX:
            return "CPR_RAIL_MX";
        case CPR_RAIL_CX:
            return "CPR_RAIL_CX";
        case CPR_RAIL_MSS:
            return "CPR_RAIL_MSS";
        case CPR_RAIL_GFX:
            return "CPR_RAIL_GFX";
        case CPR_RAIL_VDDA_EBI:
            return "CPR_RAIL_VDDA_EBI";
      // APC rails
        case CPR_RAIL_APC0:
            return "CPR_RAIL_APC0";
        case CPR_RAIL_APC1:
            return "CPR_RAIL_APC1";
      // SSC rails
        case CPR_RAIL_SSC_MX:
            return "CPR_RAIL_SSC_MX";
        case CPR_RAIL_SSC_CX:
            return "CPR_RAIL_SSC_CX";
        default:
            return "unmatched";
  }
}


char* cpr_logging_get_ascii_rail_enablement_funtion(cpr_enablement_init_params_t* cpr_enablement_init_params){
    if (cpr_enablement_init_params == &CPR_ENABLE_GLOBAL_CEILING_VOLTAGE){
        return "CPR_ENABLE_GLOBAL_CEILING_VOLTAGE";
    }
    else if (cpr_enablement_init_params == &CPR_ENABLE_READ_CONFIG_FROM_SMEM){
        return "CPR_ENABLE_READ_CONFIG_FROM_SMEM";
    }
    else if (cpr_enablement_init_params == &CPR_ENABLE_OPEN_LOOP){
        return "CPR_ENABLE_OPEN_LOOP";
    }
    else if (cpr_enablement_init_params == &CPR_ENABLE_CLOSED_LOOP){
        return "CPR_ENABLE_CLOSED_LOOP";
    }
    return "not recognized??";
}


char* cpr_logging_get_ascii_foundary_id_name(uint32 foundry_id){
    if (foundry_id == CPR_FOUNDRY_TSMC){
        return "CPR_FOUNDRY_TSMC";
    }
    if (foundry_id == CPR_FOUNDRY_GF){
        return "CPR_FOUNDRY_GF";
    }
    if (foundry_id == CPR_FOUNDRY_SS){
        return "CPR_FOUNDRY_SS";
    }
    if (foundry_id == CPR_FOUNDRY_IBM){
        return "CPR_FOUNDRY_IBM";
    }
    if (foundry_id == CPR_FOUNDRY_UMC){
        return "CPR_FOUNDRY_UMC";
    }
    return "unknown";
}



char* cpr_logging_get_ascii_corner_name(cpr_voltage_mode_t voltage_mode){
    if (voltage_mode == CPR_VOLTAGE_MODE_OFF){
        return "CPR_VOLTAGE_MODE_OFF";
    }
    if (voltage_mode == CPR_VOLTAGE_MODE_RETENTION){
        return "CPR_VOLTAGE_MODE_RETENTION";
    }
    if (voltage_mode == CPR_VOLTAGE_MODE_SVS_MIN){
        return "CPR_VOLTAGE_MODE_SVS_MIN";
    }
    if (voltage_mode == CPR_VOLTAGE_MODE_SVS2){
        return "CPR_VOLTAGE_MODE_SVS2";
    }
    if (voltage_mode == CPR_VOLTAGE_MODE_SVS){
       return "CPR_VOLTAGE_MODE_SVS";
    }
    if (voltage_mode == CPR_VOLTAGE_MODE_SVS_L1){
       return "CPR_VOLTAGE_MODE_SVS_L1";
    }
    if (voltage_mode == CPR_VOLTAGE_MODE_SVS_L1){
       return "CPR_VOLTAGE_MODE_SVS_L1";
    }
    if (voltage_mode == CPR_VOLTAGE_MODE_NOMINAL){
        return "CPR_VOLTAGE_MODE_NOMINAL";
    }
    if (voltage_mode == CPR_VOLTAGE_MODE_NOMINAL_L1){
        return "CPR_VOLTAGE_MODE_NOMINAL_L1";
    }
    if (voltage_mode == CPR_VOLTAGE_MODE_TURBO){
        return "CPR_VOLTAGE_MODE_TURBO";
    }
    if (voltage_mode == CPR_VOLTAGE_MODE_SUPER_TURBO){
        return "CPR_VOLTAGE_MODE_SUPER_TURBO";
    }
    if (voltage_mode == CPR_VOLTAGE_MODE_SUPER_TURBO_NO_CPR){
        return "CPR_VOLTAGE_MODE_SUPER_TURBO_NO_CPR";
    }
    return "unknown";
}


#ifdef WINSIM

char* cpr_logging_get_ascii_fuse_type(cpr_fuse_type fuse_type){
    if (fuse_type == CPR_FUSE_NO_FUSE){
        return "CPR_FUSE_NO_FUSE";
    }
    if (fuse_type == CPR_FUSE_SVS2){
        return "CPR_FUSE_SVS2";
    }
    if (fuse_type == CPR_FUSE_SVS){
        return "CPR_FUSE_SVS";
    }
    if (fuse_type == CPR_FUSE_NOMINAL){
        return "CPR_FUSE_NOMINAL";
    }
    if (fuse_type == CPR_FUSE_TURBO){
        return "CPR_FUSE_TURBO";
    }
    if (fuse_type == CPR_FUSE_SUTUR){
        return "CPR_FUSE_SUTUR";
    }
    if (fuse_type == CPR_FUSE_AGING){
        return "CPR_FUSE_AGING";
    }
    return "unknown";
}
#endif





void cpr_logging_initialize_stats_log(cpr_closed_loop_rail_t rail, char* cpr_state_info, int disabledByHw, uint32 pmic_version, uint32 cpu_version, int32 fuse_version){
    ULOG_RT_STATIC_PRINTF_5(cpr_ulog_static_h, CPR_ULOG_STATIC_LOG_DRIVER_INFO, 
        "MSS CPR %s  (DisabledByHW: %d) (PmicVer: %d) (CPUVersion: %x) (DriverFuseConfigVersion: %d)", cpr_state_info, disabledByHw, pmic_version, cpu_version, fuse_version);

}


void cpr_logging_update_current_voltage_stats(uint32 svs_min_uV, uint32 svs2_uV, uint32 svs_uV, uint32 svsplus_uV, uint32 nom_uV, uint32 nomplus_uV, uint32 turbo_uV){
    ULOG_RT_STATIC_PRINTF_7(cpr_ulog_static_h, CPR_ULOG_STATIC_LOG_CURR_VOLTAGE_INFO,
                            "Current Voltages (SVS_MIN: %d) (SVS2: %d) (SVS: %d) (SVS+: %d) (NOM: %d) (NOM+: %d) (TUR: %d)",
                            svs_min_uV, svs2_uV, svs_uV, svsplus_uV, nom_uV, nomplus_uV, turbo_uV);
}


void cpr_logging_update_starting_voltage_stats(uint32 svs_min_uV, uint32 svs2_uV, uint32 svs_uV, uint32 svsplus_uV, uint32 nom_uV, uint32 nomplus_uV, uint32 turbo_uV){
    ULOG_RT_STATIC_PRINTF_7(cpr_ulog_static_h, CPR_ULOG_STATIC_LOG_START_VOLTAGE_INFO,
                            "Starting Voltages (SVS_MIN: %d) (SVS2: %d) (SVS: %d) (SVS+: %d) (NOM: %d) (NOM+: %d) (TUR: %d)",
                            svs_min_uV, svs2_uV, svs_uV, svsplus_uV, nom_uV, nomplus_uV, turbo_uV);
}



void cpr_logging_update_lowest_voltage_stats(uint32 svs_min_uV, uint32 svs2_uV, uint32 svs_uV, uint32 svsplus_uV, uint32 nom_uV, uint32 nomplus_uV, uint32 turbo_uV){
    ULOG_RT_STATIC_PRINTF_7(cpr_ulog_static_h, CPR_ULOG_STATIC_LOG_MIN_VOLTAGE_INFO,
                            "Min Voltages (SVS_MIN: %d) (SVS2: %d) (SVS: %d) (SVS+: %d) (NOM: %d) (NOM+: %d) (TUR: %d)",
                            svs_min_uV, svs2_uV, svs_uV, svsplus_uV, nom_uV, nomplus_uV, turbo_uV);
}

void cpr_logging_update_floor_voltage_stats(uint32 svs_min_uV, uint32 svs2_uV, uint32 svs_uV, uint32 svsplus_uV, uint32 nom_uV, uint32 nomplus_uV, uint32 turbo_uV){
    ULOG_RT_STATIC_PRINTF_7(cpr_ulog_static_h, CPR_ULOG_STATIC_LOG_FLOOR_VOLTAGE_INFO,
        "Floor Voltages (SVS_MIN: %d) (SVS2: %d) (SVS: %d) (SVS+: %d) (NOM: %d) (NOM+: %d) (TUR: %d)",
        svs_min_uV, svs2_uV, svs_uV, svsplus_uV, nom_uV, nomplus_uV, turbo_uV);
}


void cpr_logging_update_mode_enable_count_stats(uint8 svs_min_count, uint8 svs2_count, uint8 svs_count, uint8 svsplus_count, uint8 nom_count, uint8 nomplus_count, uint8 turbo_count){
    ULOG_RT_STATIC_PRINTF_7(cpr_ulog_static_h, CPR_ULOG_STATIC_MODE_ENABLE_COUNT_INFO,
                            "Mode_Enable_Count (SVS_MIN: %d) (SVS2: %d) (SVS: %d) (SVS+: %d) (NOM: %d) (NOM+: %d) (TUR: %d)",
                            svs_min_count, svs2_count, svs_count, svsplus_count, nom_count, nomplus_count, turbo_count);
}

void cpr_logging_update_aging_stats(int32 aging_percentage, int32 aging_static_margin_limit){
    ULOG_RT_STATIC_PRINTF_2(cpr_ulog_static_h, CPR_ULOG_STATIC_LOG_AGING_INFO,
                            "Aging (aging_percentage: %d)  (aging_static_margin_limit: %d)",
                            aging_percentage, aging_static_margin_limit);
                            
}



void cpr_logging_update_fuse_stats(cpr_rail_id_t rail_id){
    ULOG_RT_STATIC_PRINTF_7(cpr_ulog_static_h, CPR_ULOG_STATIC_LOG_FUSE_INFO,
                            "Debug (SVS2: %d)  (SVS: %d) (NOM: %d) (TURBO: %d) (AGING: %d) (NOM offset: %d) (TURBO offset: %d)",
                             cpr_fuses_get_fuse_value(rail_id, CPR_FUSE_SVS2),
                             cpr_fuses_get_fuse_value(rail_id, CPR_FUSE_SVS),
                             cpr_fuses_get_fuse_value(rail_id, CPR_FUSE_NOMINAL),
                             cpr_fuses_get_fuse_value(rail_id, CPR_FUSE_TURBO),
                             cpr_fuses_get_fuse_value(rail_id, CPR_FUSE_AGING),
			     cpr_fuses_get_fuse_value(rail_id, CPR_FUSE_NOMINAL_OFFSET),
			     cpr_fuses_get_fuse_value(rail_id, CPR_FUSE_TURBO_OFFSET));
}




#if 0
//V3 debug code
#include "HAL_cpr_v3.h"
#include "HALhwio.h"
#include "HAL_cpr.h"
#include "HAL_cpr_hwio.h"

void cpr_logging_dump_cprLogRegistersToLUog(cpr_closed_loop_rail_t rail)
{
  const cpr_closed_loop_rail_config_t* rail_config = rail->rail_config;
  const HAL_cpr_rail_s* hal_handle = &rail_config->hal_handle;
 
  ULogFront_RealTimePrintf(cpr_ulog_h, 8,"(MSS_RBCPR_LOG 0 to 6: (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x)",
                                 CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,0)),
                                 CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,1)),
                                 CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,2)),
                                 CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,3)),
                                 CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,4)),
                                 CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,5)),
                                 CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,6)),
                                 CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,7)));

                                 
 // ULogFront_RealTimePrintf(cpr_ulog_h, 1,"Debug 0 address is %x",  HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,0));
}



void cpr_logging_dump_cprRegistersToLog(cpr_closed_loop_rail_t rail)
{

  const cpr_closed_loop_rail_config_t* rail_config = rail->rail_config;
  const HAL_cpr_rail_s* hal_handle = &rail_config->hal_handle;
  
  ULogFront_RealTimePrintf(cpr_ulog_h, 1," cpr_ctl : 0x%x", CPR_HWIO_IN( HWIO_CPR_FSM_CTL_ADDR( hal_handle->hw_base_address ) ));
  ULogFront_RealTimePrintf(cpr_ulog_h, 1," cpr_result_0 : 0x%x", CPR_HWIO_IN( HWIO_CPR_RESULT0_t_ADDR( hal_handle->hw_base_address, 0 ) ));
  ULogFront_RealTimePrintf(cpr_ulog_h, 1," cpr_debug0 : 0x%x", CPR_HWIO_IN( HWIO_CPR_DEBUG_ADDR( hal_handle->hw_base_address ) ));
  ULogFront_RealTimePrintf(cpr_ulog_h, 1," rbif_irq_en_0 : 0x%x", CPR_HWIO_IN( HWIO_CPR_IRQ_EN_t_ADDR( hal_handle->hw_base_address, 0 ) ));
  
   
 
  ULogFront_RealTimePrintf(cpr_ulog_h, 8,"(MSS_RBCPR_LOG 0 to 6: (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x)",
                                 CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,0)),
                                 CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,1)),
                                 CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,2)),
                                 CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,3)),
                                 CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,4)),
                                 CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,5)),
                                 CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,6)),
                                 CPR_HWIO_IN( HWIO_CPR_DEBUG_LOG_n_ADDR(hal_handle->hw_base_address,7)));

}

#endif

#if 0
//V2 debug code
#include "HAL_cpr_v2.h"
#include "HALhwio.h"
#include "HAL_cpr.h"
#include "HAL_cpr_hwio.h"

void cpr_logging_dump_cprLogRegistersToLUog(cpr_closed_loop_rail_t cl_rail)
{
    HAL_cpr_rail_s* rail = &(cl_rail->rail_config->hal_handle);

    ULogFront_RealTimePrintf(cpr_ulog_h, 8, "(MSS_RBCPR_LOG 0 to 6: (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x) (0x%x)",
        HWIO_INX(rail->hw_base_address, RBCPR_LOG_0),
        HWIO_INX(rail->hw_base_address, RBCPR_LOG_1),
        HWIO_INX(rail->hw_base_address, RBCPR_LOG_2),
        HWIO_INX(rail->hw_base_address, RBCPR_LOG_3),
        HWIO_INX(rail->hw_base_address, RBCPR_LOG_4),
        HWIO_INX(rail->hw_base_address, RBCPR_LOG_5),
        HWIO_INX(rail->hw_base_address, RBCPR_LOG_6),
        HWIO_INX(rail->hw_base_address, RBCPR_LOG_7));
}



void cpr_logging_dump_cprRegistersToLog(cpr_closed_loop_rail_t cl_rail)
{
    HAL_cpr_rail_s* rail = &(cl_rail->rail_config->hal_handle);

    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "cpr_timer_interval : %x", HWIO_INX(rail->hw_base_address, RBCPR_TIMER_INTERVAL));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "rbif_timer_adjust : %x", HWIO_INX(rail->hw_base_address, RBIF_TIMER_ADJUST));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "cpr_ctl : %x", HWIO_INX(rail->hw_base_address, RBCPR_CTL));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "cpr_result_0 : %x", HWIO_INX(rail->hw_base_address, RBCPR_RESULT_0));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "cpr_result_1 : %x", HWIO_INX(rail->hw_base_address, RBCPR_RESULT_1));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "rbif_irq_en_0 : %x", HWIO_INX(rail->hw_base_address, RBIF_IRQ_EN_0));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "rbif_irq_status : %x", HWIO_INX(rail->hw_base_address, RBIF_IRQ_STATUS));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "cpr_debug0 : %x", HWIO_INX(rail->hw_base_address, RBCPR_DEBUG0));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "cpr_debug1 : %x", HWIO_INX(rail->hw_base_address, RBCPR_DEBUG1));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "cpr_debug2 : %x", HWIO_INX(rail->hw_base_address, RBCPR_DEBUG2));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "cpr_log_0 : %x", HWIO_INX(rail->hw_base_address, RBCPR_LOG_0));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "cpr_log_1 : %x", HWIO_INX(rail->hw_base_address, RBCPR_LOG_1));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "cpr_log_2 : %x", HWIO_INX(rail->hw_base_address, RBCPR_LOG_2));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "cpr_log_3 : %x", HWIO_INX(rail->hw_base_address, RBCPR_LOG_3));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "cpr_log_4 : %x", HWIO_INX(rail->hw_base_address, RBCPR_LOG_4));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "cpr_log_5 : %x", HWIO_INX(rail->hw_base_address, RBCPR_LOG_5));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "cpr_log_6 : %x", HWIO_INX(rail->hw_base_address, RBCPR_LOG_6));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "cpr_log_7 : %x", HWIO_INX(rail->hw_base_address, RBCPR_LOG_7));
    ULogFront_RealTimePrintf(cpr_ulog_h, 1, "htol_age : %x", HWIO_INX(rail->hw_base_address, RBCPR_HTOL_AGE));
}

#endif


