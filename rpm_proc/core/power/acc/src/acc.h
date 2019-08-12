/*===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "railway.h"

#define ACC_MODE_NOT_SUPPORTED -1

typedef enum {
    ACC_REGISTER_SVS_SETTING,
    ACC_REGISTER_NOM_SETTING,
    ACC_REGISTER_TURBO_SETTING,
    ACC_REGISTER_NUM_SETTING
} acc_register_state;

typedef struct {
    const char* rail_name;
    const int*  acc_register_setting;
    volatile unsigned* acc_register_address;
	volatile unsigned* apc_acc_register_address;
	int merged_rail_config;
} acc_control_config;

typedef struct {
    acc_control_config* config;
    int number_of_acc_controls;
} acc_target_config;

typedef struct {
    acc_register_state current_acc_setting;
    const acc_control_config* config;
} acc_setting_control;

extern const acc_target_config* ACC_TARGET_CONFIG;
void acc_target_init (int, acc_control_config*);

