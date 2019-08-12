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

#include "sns_dd_pah_system_clock.h"
#include <stdio.h>
#include "fixed_point.h"

// qcom
#include <sns_ddf_util.h>


/*============================================================================
STATIC VARIABLE DEFINITIONS
============================================================================*/
static const uint32_t  MICROSEC_PER_SEC = 1000 * 1000;
static const uint32_t  MICROSEC_PER_MILLISEC = 1000;
static uint32_t    _ticks_per_sec = 1;

bool system_clock_init(void)
{
  _ticks_per_sec = sns_ddf_convert_usec_to_tick(MICROSEC_PER_SEC);
  return true;
}

uint32_t get_tick_count(void)
{
  return (uint32_t)(((float)sns_ddf_get_timestamp() * MICROSEC_PER_SEC / _ticks_per_sec) / MICROSEC_PER_MILLISEC);
}

void delay_ms(uint32_t ms)
{
  sns_ddf_delay(ms * MICROSEC_PER_MILLISEC);
}
