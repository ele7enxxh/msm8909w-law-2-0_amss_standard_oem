/********************************************************************************
* Copyright (c) 2016, PixArt Imaging Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
* notice, this list of conditions and the following disclaimer in the
* documentation and/or other materials provided with the distribution.
* 3. Neither the name of the PixArt Imaging Inc. nor the
* names of its contributors may be used to endorse or promote products
* derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/*==============================================================================
* Edit History
* 
* This section contains comments describing changes made to the module. Notice
* that changes are listed in reverse chronological order. Please use ISO format
* for dates.
* 
* when       who       what, where, why
* ---------- ---       -----------------------------------------------------------
* 2016-07-07 bell      Initial revision.
==============================================================================*/

#include "sns_dd_pah_driver_8011_reg_array.h"

// pah
#include "sns_dd_pah_driver_8011_reg.h"
#include "sns_dd_pah_comm.h"

// platform support
// #include "sns_dd_pah_platform_functions.h"


/*============================================================================
EXPRESSION MACRO DEFINITIONS
============================================================================*/
#define ARRAY_SIZE(arr)         (sizeof(arr) / sizeof(arr[0]))

/*============================================================================
TYPE DEFINITIONS
============================================================================*/
typedef struct {

  pah8011_reg_array_mode_e    reg_array_mode;

} pah8011_reg_array_state_s;

/*============================================================================
PRIVATE GLOBAL VARIABLES
============================================================================*/
static pah8011_reg_array_state_s    g_state;


/*============================================================================
PRIVATE FUNCTION PROTOTYPES
============================================================================*/
static bool             _pah8011_set_reg_array(const uint8_t reg_array[][2], uint32_t length);


/*============================================================================
PUBLIC FUNCTION DEFINITIONS
============================================================================*/
bool pah8011_reg_array_init(void)
{
  memset(&g_state, 0, sizeof(g_state));
  return true;
}

bool pah8011_reg_array_load_init(void)
{
  debug_printf_1("pah8011_reg_array_load_init(), reg[v%d] \n", PAH_DRIVER_8011_REG_VERSION);

  g_state.reg_array_mode = pah8011_reg_array_mode_none;

  if (!_pah8011_set_reg_array(pah8011_init_register_array, ARRAY_SIZE(pah8011_init_register_array)))
    return false;

  return true;
}

bool pah8011_reg_array_load_mode(pah8011_reg_array_mode_e reg_array_mode)
{
  if (g_state.reg_array_mode != reg_array_mode)
  {
    switch (reg_array_mode)
    {
    case pah8011_reg_array_mode_touch:
      debug_printf("pah8011_reg_array_load_reg_setting(). pah8011_touch_register_array \n");
      if (!_pah8011_set_reg_array(pah8011_touch_register_array, ARRAY_SIZE(pah8011_touch_register_array)))
        goto FAIL;
      break;

    case pah8011_reg_array_mode_ppg_20hz:
      debug_printf("pah8011_reg_array_load_reg_setting(). pah8011_ppg_20hz_register_array \n");
      if (!_pah8011_set_reg_array(pah8011_ppg_20hz_register_array, ARRAY_SIZE(pah8011_ppg_20hz_register_array)))
        goto FAIL;
      break;

    case pah8011_reg_array_mode_ppg_200hz:
      debug_printf("pah8011_reg_array_load_reg_setting(). pah8011_ppg_200hz_register_array \n");
      if (!_pah8011_set_reg_array(pah8011_ppg_200hz_register_array, ARRAY_SIZE(pah8011_ppg_200hz_register_array)))
        goto FAIL;
      break;

    default:
      debug_printf_1("pah8011_reg_array_load_reg_setting(). not implemented, reg_array_mode = %d \n", reg_array_mode);
      goto FAIL;
    }

    g_state.reg_array_mode = reg_array_mode;
  }

  return true;

FAIL:
  debug_printf("_pah8011_init fail \n");
  return false;
}

/*============================================================================
PRIVATE FUNCTION DEFINITIONS
============================================================================*/
static bool _pah8011_set_reg_array(const uint8_t reg_array[][2], uint32_t length)
{
  uint32_t i = 0;

  for (i = 0; i < length; ++i)
  {
    if (!pah_comm_write(reg_array[i][0], reg_array[i][1]))
    {
      debug_printf_1("_pah8011_set_reg_array(). pah_comm_write fail, i = %d \n", i);
      return false;
    }
  }
  return true;
}
