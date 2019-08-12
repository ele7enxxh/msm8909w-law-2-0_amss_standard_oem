#ifndef CPR_LOGGING_H
#define CPR_LOGGING_H
/*===========================================================================
Copyright (c) 2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary and Confidential.
===========================================================================*/
#include "cpr_open_loop.h"
#include "cpr_fuses.h"
#include "cpr_enablement.h"
#include "cpr_device_hw_version.h"
#include "cpr_voltage_ranges.h"
#include "cpr_closed_loop.h"
#if defined(WINSIM) || defined(MPSS_CPR_BUILD)
#include "ULogFront.h"

extern ULogHandle cpr_ulog_h;
extern ULogHandle cpr_ulog_static_h;
#endif

void cpr_logging_init();
char* cpr_logging_get_ascii_rail_id_name(cpr_rail_id_t rail_id);
char* cpr_logging_get_ascii_rail_enablement_funtion(cpr_enablement_init_params_t* cpr_enablement_init_params);
char* cpr_logging_get_ascii_foundary_id_name(uint32 foundry_id);
char* cpr_logging_get_ascii_corner_name(cpr_voltage_mode_t voltage_mode);
void cpr_logging_initialize_stats_log(cpr_closed_loop_rail_t rail, char* cpr_state_info, int disabledByHw, uint32 pmic_version, uint32 cpu_version, int32 fuse_version);
void cpr_logging_update_starting_voltage_stats(uint32 svs_min_uV, uint32 svs2_uV, uint32 svs_uV, uint32 svsplus_uV, uint32 nom_uV, uint32 nomplus_uV, uint32 turbo_uV);
void cpr_logging_update_current_voltage_stats(uint32 svs_min_uV, uint32 svs2_uV, uint32 svs_uV, uint32 svsplus_uV, uint32 nom_uV, uint32 nomplus_uV, uint32 turbo_uV);
void cpr_logging_update_lowest_voltage_stats(uint32 svs_min_uV, uint32 svs2_uV, uint32 svs_uV, uint32 svsplus_uV, uint32 nom_uV, uint32 nomplus_uV, uint32 turbo_uV);
void cpr_logging_update_floor_voltage_stats(uint32 svs_min_uV, uint32 svs2_uV, uint32 svs_uV, uint32 svsplus_uV, uint32 nom_uV, uint32 nomplus_uV, uint32 turbo_uV);
void cpr_logging_update_mode_enable_count_stats(uint8 svs_min_count, uint8 svs2_count, uint8 svs_count, uint8 svsplus_count, uint8 nom_count, uint8 nomplus_count, uint8 turbo_count);
void cpr_logging_update_aging_stats(int32 aging_percentage, int32 aging_static_margin_limit);
void cpr_logging_update_fuse_stats(cpr_rail_id_t rail_id);

#if defined(WINSIM) 
//#define CPR_LOGGING(n,x, ...) printf(x "\n", ##__VA_ARGS__)
#define CPR_LOGGING(n,x, ...)   ULogFront_RealTimePrintf(cpr_ulog_h, n, x, ##__VA_ARGS__)
#elif defined(MPSS_CPR_BUILD) 
  #define CPR_LOGGING(n,x, ...)   ULogFront_RealTimePrintf(cpr_ulog_h, n, x, ##__VA_ARGS__)
#else
  #define CPR_LOGGING(n,x, ...)
#endif


#define CPR_ULOG_STATIC_NUM_SLOTS                 8
#define CPR_ULOG_STATIC_LOG_DRIVER_INFO           0
#define CPR_ULOG_STATIC_LOG_START_VOLTAGE_INFO    1
#define CPR_ULOG_STATIC_LOG_CURR_VOLTAGE_INFO     2
#define CPR_ULOG_STATIC_LOG_MIN_VOLTAGE_INFO      3
#define CPR_ULOG_STATIC_LOG_FLOOR_VOLTAGE_INFO    4
#define CPR_ULOG_STATIC_MODE_ENABLE_COUNT_INFO    5
#define CPR_ULOG_STATIC_LOG_AGING_INFO            6
#define CPR_ULOG_STATIC_LOG_FUSE_INFO             7


#endif //CPR_LOGGING_H