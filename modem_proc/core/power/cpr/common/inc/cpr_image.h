/*===========================================================================

  Copyright (c) 2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef CPR_IMAGE_H
#define CPR_IMAGE_H

#include "cpr_defs.h"
#include "cpr_closed_loop.h"
#include <stddef.h>

// This file contains function prototypes for all functions which must be implemented by the image-specific part of the driver.
size_t cpr_image_memscpy(void *dst, size_t dst_size, const void *src, size_t src_size);

void cpr_image_cancel_task(cpr_closed_loop_rail_t rail);

void cpr_image_enable_clocks(cpr_closed_loop_rail_t rail);

cpr_image_closed_loop_rail_t cpr_image_alloc_image_rail_state(void);

void cpr_image_clear_interrupt_at_interrupt_controller(uint32 interrupt_id);

void cpr_image_init_image_closed_loop(cpr_closed_loop_rail_t rail);

const cpr_corner_params_t* cpr_image_get_current_corner_params(cpr_closed_loop_rail_t rail);

void cpr_image_prepare_for_voltage_settling(void);

//Voltage control APIs
cpr_image_rail_voltage_control_handle_t cpr_image_alloc_voltage_control_handle(cpr_rail_id_t cpr_rail_id);
void cpr_image_set_rail_mode(cpr_image_rail_voltage_control_handle_t voltage_control_handle, cpr_voltage_mode_t voltage_mode);
cpr_voltage_mode_t cpr_image_get_rail_mode(cpr_image_rail_voltage_control_handle_t voltage_control_handle);
uint32 cpr_image_get_rail_mode_voltage(cpr_image_rail_voltage_control_handle_t voltage_control_handle, cpr_voltage_mode_t voltage_mode);
void cpr_image_rail_transition_voltage(cpr_image_rail_voltage_control_handle_t voltage_control_handle);
void cpr_image_set_rail_mode_voltage(cpr_image_rail_voltage_control_handle_t voltage_control_handle, cpr_voltage_mode_t voltage_mode, uint32 voltage_uv);

uint32 cpr_image_get_chip_info_version(void);

cpr_foundry_id cpr_image_get_foundry_info(void);
void cpr_image_prepare_to_access_smem(void);
void cpr_image_smem_access_complete(void);

#endif //CPR_IMAGE_H
