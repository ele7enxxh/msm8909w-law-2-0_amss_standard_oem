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
  2016-04-12 bell      Add license information and revision information
  2016-04-07 bell      Initial revision.
==============================================================================*/

#include <stdio.h>
#include "fixed_point.h"


#include "sns_dd_pah_comm.h"

// platform support
#include "sns_dd_pah_i2c.h"

// c
//#include <stdlib.h>
//#include <string.h>


/*============================================================================
STATIC VARIABLE DEFINITIONS
============================================================================*/
static uint8_t    _curr_bank = 0xFF;    // 0x00 ~ 0x03

/*============================================================================
STATIC FUNCTION PROTOTYPES
============================================================================*/


bool pah_comm_write(uint8_t addr, uint8_t data)
{
  if (addr == 0x7F)
  {
    if (_curr_bank == data)
      return true;

    if (!i2c_write_reg(0x7F, data))
      return false;
    
    _curr_bank = data;
    return true;
  }

  return i2c_write_reg(addr, data);
}

bool pah_comm_read(uint8_t addr, uint8_t *data)
{
  return i2c_read_reg(addr, data);
}

bool pah_comm_burst_read(uint8_t addr, uint8_t *data, uint16_t num)
{
  return i2c_burst_read_reg(addr, data, num);
}
