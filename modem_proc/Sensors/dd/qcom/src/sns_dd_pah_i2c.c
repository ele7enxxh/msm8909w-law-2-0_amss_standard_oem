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
  Edit History
  
  This section contains comments describing changes made to the module. Notice
  that changes are listed in reverse chronological order. Please use ISO format
  for dates.
  
  when       who       what, where, why
  ---------- ---       -----------------------------------------------------------
  
  2016-07-12 bell 	   Add pah_plat_get_last_error() function. In the future, it would be moved to other header if more than one platform modules are going to be used.
  2016-04-12 bell      Add license information and revision information
  2016-04-07 bell      Initial revision.
==============================================================================*/

#include "sns_dd_pah_i2c.h"



/*============================================================================
STATIC VARIABLE DEFINITIONS
============================================================================*/
static sns_ddf_handle_t  _i2c_port_handle = NULL;
static sns_ddf_status_e _last_error = SNS_DDF_SUCCESS;


sns_ddf_status_e  pah_plat_get_last_error(void)
{
  return _last_error;
}

bool i2c_init(void* port_handle)
{
  _i2c_port_handle = port_handle;
  return true;
}
void i2c_deinit(void)
{
  _i2c_port_handle = NULL;
}

bool i2c_write_reg(uint8_t addr, uint8_t data)
{
  uint8_t write_count = 0;

    _last_error = sns_ddf_write_port(
    _i2c_port_handle,
    addr,
    &data,
    1,
    &write_count);

  return _last_error == SNS_DDF_SUCCESS;
}
bool i2c_read_reg(uint8_t addr, uint8_t *data)
{
  uint8_t read_count = 0;

    _last_error = sns_ddf_read_port(
    _i2c_port_handle,
    addr,
    data,
    1,
    &read_count);

  return _last_error == SNS_DDF_SUCCESS;
}
bool i2c_burst_read_reg(uint8_t addr, uint8_t *data, uint32_t rx_size)
{
  uint32_t read_count = 0;
  uint32_t total_read_count = 0;
  uint32_t size_to_read = rx_size;
    do
    {
      _last_error = sns_ddf_read_port_u32_count(
    _i2c_port_handle,
    addr,
    data,
    size_to_read,
    &read_count);
    
    total_read_count +=read_count;
    data +=read_count;
    size_to_read = rx_size -total_read_count;
  }while(_last_error == SNS_DDF_SUCCESS && total_read_count < rx_size);
  
  if(total_read_count != rx_size)
     _last_error = SNS_DDF_EFAIL;

  return _last_error == SNS_DDF_SUCCESS;
}
