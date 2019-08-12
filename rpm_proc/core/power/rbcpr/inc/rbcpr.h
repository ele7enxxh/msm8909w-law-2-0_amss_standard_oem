/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/


#ifndef RBCPR_H
#define RBCPR_H

#include <comdef.h>
#include <DALSysTypes.h>
#include "rbcpr_comdef.h"
#include "pm.h"
#include "railway.h"
#include "HAL_rbcpr.h"
#include "npa.h"
#include "Clock.h"
#include "Chipinfo.h"
#include "rbcpr_fuses.h"

// The max # of latest recommendations for stats
#define RBCPR_LATEST_RECOMMENDS     3
#define RBCPR_ROSC_COUNT            8


struct rbcpr_bsp_rail_info;
//Settings that are tied to the HW version.
typedef struct
{
  uint16                rosc_target[RBCPR_ROSC_COUNT];
  railway_corner        corner;
  rbcpr_fuse_type       fuse;
  int8                  unused;         //Left in to maintain binary compatibility between SBL and RPM.
  uint32                voltage_ceil;
  uint32                voltage_floor;
  int32                 static_margin; // value in microvolts
  signed                static_margin_adjust; 
  int32                 ss_static_margin_adjust; // value in microvolts
} rbcpr_corner_params_type;

struct rbcpr_bsp_rail_info;
typedef void (*rbcpr_custom_fuse_function)(rbcpr_corner_params_type*, struct rbcpr_bsp_rail_info*, int);

typedef struct
{
    rbcpr_corner_params_type corner_params;
    rbcpr_custom_fuse_function fuse_function;
    int32                      fuse_correlation_factor;
} rbcpr_corner_params_and_fuse_type;

typedef enum
{
    RBCPR_DISABLED,
    RBCPR_ENABLED_OPEN_LOOP,
    RBCPR_ENABLED_CLOSED_LOOP,
} rbcpr_enablement_type;

typedef struct
{
  rbcpr_corner_params_and_fuse_type*    target_params;
  rbcpr_enablement_type                 rbcpr_enablement;
  uint8                                 number_of_target_params;
  bool                                  sbl_init_done;
  uint8                                 number_of_corners_to_settle_at;
  railway_corner*                       corners_to_settle_at;
  uint32                                voltage_scaling_vector[RBCPR_ROSC_COUNT];
} rbcpr_bsp_rail_params_type;

typedef struct
{
    rbcpr_bsp_rail_params_type* bsp_params;
	uint32 process_id;
	uint32 foundry_id;
} rbcpr_tsmc_fab_id_settings;

typedef struct
{
  const rbcpr_tsmc_fab_id_settings* rbcpr_bsp_params_tsmc_fab;
  uint32 supported_hw_after_version;
  uint32 fab_process_id_count;
} rbcpr_versioned_bsp_rail_params_type;

typedef struct
{
  int32 hw_version_count;
  const rbcpr_versioned_bsp_rail_params_type* hw_version_array;
  ChipInfoFamilyType supported_chipset;
} rbcpr_bsp_revision_array_type;

typedef struct
{
  const rbcpr_bsp_revision_array_type** hw_version_specific_settings;
  int32 hw_version_specific_settings_count;
} rbcpr_supported_hw_array_type;

extern const rbcpr_supported_hw_array_type rbcpr_supported_hw_array;

typedef enum
{
    REF_CLK,
    AHB_CLK,
    CLK_COUNT
} rbcpr_clock_id;

typedef struct
{
  ClockIdType clk_id;
  bool        enabled;
} rbcpr_clock_state;

typedef struct
{
  const char *ref_clk_resource;
  const char *ahb_clk_resource;
  const char *rail_name;
  uint32 pmic_step_size_uv;
  uint32 efuse_target_voltage_multipler;
  uint8 idle_clocks;
  uint8 step_quot;
  uint8 interrupt_id;
  uint8 up_threshold;
  uint8 dn_threshold;
  uint8 consecutive_up;
  uint8 consecutive_dn;
  uint8 gcnt;
  HAL_rbcpr_count_mode count_mode;
  const uint8 *sensors_to_mask;
  uint8 number_of_sensors_to_mask;
  bool disable_fused_floor;
} rbcpr_bsp_rail_const_info;

typedef struct rbcpr_bsp_rail_info
{
  rbcpr_bsp_rail_params_type* bsp_data;
  const rbcpr_bsp_rail_const_info* rail_const_info;
  rbcpr_clock_state clock[CLK_COUNT];
  int railway_rail_id;
  HAL_rbcpr_rail_t hal_handle;
  boolean block_enabled;
  uint8  stats_rail_id;
  boolean cpr_handler_in_progress;
  railway_settings railway_setting;
  void* task;
  int corner_lookup_index[RAILWAY_CORNERS_COUNT];
  HAL_rbcpr_log_type hal_log;
  uint16 post_switch_counter;
  uint16 cpr_voltage_change_cnt;
  uint16 cpr_voltage_hit_floor;
  uint16 cpr_voltage_hit_ceiling;
  uint16 step_up_count;
  uint16 step_down_count;
  uint16 ignored_readings;
} rbcpr_bsp_rail_info;

typedef struct
{
  rbcpr_bsp_rail_info* rails;
  int                  num_rails;
} rbcpr_bsp_type;

typedef struct
{
  int       rail_id;
  int       sensors_count;
  const int *sensors;
} rbcpr_sensor_set;

extern const rbcpr_sensor_set lpass_q6_sensors;

void init_rbcpr_resource(void);
void rbcpr_disable_lpass_q6_sensor(boolean disable_sensor);
void rbcpr_bsp_target_init(void);

extern rbcpr_bsp_type rbcpr_bsp_data;
boolean rbcpr_internalize_state(void);

rbcpr_corner_params_type* rbcpr_corner_params(rbcpr_bsp_rail_info* rail, railway_corner corner);
uint32 rbcpr_corner_index(rbcpr_bsp_rail_info* rail, railway_corner corner);
#endif /* RBCPR_H */
