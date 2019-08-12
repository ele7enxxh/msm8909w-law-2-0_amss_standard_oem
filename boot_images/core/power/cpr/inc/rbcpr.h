/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/


#ifndef RBCPR_H
#define RBCPR_H

#include "DALStdDef.h"
#include "railway.h"
#include "HAL_rbcpr.h"
#include "DDIChipInfo.h"
#include "rbcpr_fuses.h"

// The max # of latest recommendations for stats
#define RBCPR_ROSC_COUNT            8

// The max # of rails included in setting checksum
// i.e mx, cx, vdd_ebi
#define RBCPR_MAX_CHECKSUMS         3
#define RAIL_NOT_SUPPORTED_BY_RBCPR -1

//Settings that are tied to the HW version.
typedef struct
{
  uint16                rosc_target[RBCPR_ROSC_COUNT];
  railway_corner        corner;
  rbcpr_fuse_type       fuse;
  int8                  unused;
  uint32                voltage_ceil;
  uint32                voltage_floor;
} rbcpr_corner_params_type;

struct rbcpr_bsp_rail_info;
typedef void (*rbcpr_custom_fuse_function)(rbcpr_corner_params_type*, struct rbcpr_bsp_rail_info*);

typedef struct
{
    rbcpr_corner_params_type    corner_params;
    rbcpr_custom_fuse_function  fuse_function;
    int32                       fuse_correlation_factor;
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
  rbcpr_enablement_type           rbcpr_enablement;
  uint8                           number_of_target_params;
  uint8                           number_of_corners_to_settle_at;
  railway_corner*                 corners_to_settle_at;
} rbcpr_bsp_rail_params_type;

typedef struct
{
    rbcpr_bsp_rail_params_type* bsp_params;
	uint32 process_id;
	uint32 foundry_id;
} rbcpr_fab_id_settings;

typedef struct
{
  rbcpr_fab_id_settings* rbcpr_bsp_params_fab;
  uint32 supported_hw_after_version;
  uint32 fab_process_id_count;
} rbcpr_versioned_bsp_rail_params_type;

typedef struct
{
  int32 hw_version_count;
  rbcpr_versioned_bsp_rail_params_type* hw_version_array;
  DalChipInfoFamilyType supported_chipset;
} rbcpr_bsp_revision_array_type;

typedef struct
{
  rbcpr_bsp_revision_array_type** hw_version_specific_settings;
  int32 hw_version_specific_settings_count;
} rbcpr_supported_hw_array_type;

extern const rbcpr_supported_hw_array_type rbcpr_supported_hw_array;

typedef struct
{
  const char *rail_name;
  uint32 pmic_step_size_uv;
  uint32 efuse_target_voltage_multipler;
  uint8 idle_clocks;
  uint8 step_quot;
  uint8 up_threshold;
  uint8 dn_threshold;
  uint8 consecutive_up;
  uint8 consecutive_dn;
  uint8 gcnt;
  uint8 efuse_target_voltage_step_multipler;
  HAL_rbcpr_count_mode count_mode;
} rbcpr_bsp_rail_const_info;

typedef struct rbcpr_bsp_rail_info
{
  const rbcpr_bsp_rail_params_type* bsp_data;
  const rbcpr_bsp_rail_const_info* rail_const_info;
  int32 railway_rail_id;
  HAL_rbcpr_rail_t hal_handle;
  void* task;
  int32 corner_lookup_index[RAILWAY_CORNERS_COUNT];
} rbcpr_bsp_rail_info;

typedef struct
{
  rbcpr_bsp_rail_info* rails;
  int32                num_rails;
} rbcpr_bsp_type;

void init_rbcpr_resource(void);
void rbcpr_disable_lpass_q6_sensor(boolean disable_sensor);

extern rbcpr_bsp_type rbcpr_bsp_data;

/*
 * checksum config types
 */
typedef struct
{
    const char *rail_name;
    int32 rbcpr_rail_id;
    int32 railway_rail_id;
} rbcpr_checksum_config_t;

typedef struct
{
    rbcpr_checksum_config_t * const configs;
    const uint8 num_configs;
} rbcpr_checksum_config_arr_t;

extern rbcpr_checksum_config_arr_t rbcpr_checksum_config_data;


#endif /* RBCPR_H */
