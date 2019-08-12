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
  
  when        version  who       what, where, why
  ----------  ------   ---       -----------------------------------------------------------
  2016-04-29   1001    bell      - Edit pah8011_init_register_array
                                 - Add pah8011_touch_register_array, pah8011_ppg_20hz_register_array, pah8011_ppg_200hz_register_array
  2016-04-14   1000    bell      - Add version
  2016-04-12           bell      - Add license information and revision information
                                 - Modify to reduce the reaction time of touch/no-touch detect.
                                 - Add comment
  2016-04-07           bell      Initial revision.
==============================================================================*/

#ifndef __pah_driver_8011_reg_h__
#define __pah_driver_8011_reg_h__


// c
#include <stdint.h>


#define PAH_DRIVER_8011_REG_VERSION		1001


// pah_driver_8011_reg_20160428.h
static const uint8_t pah8011_init_register_array[][2] = {
	{ 0x7F, 0x00 },	//change to bank0
	{ 0x14, 0x1B },
	{ 0x15, 0x06 },
	{ 0x16, 0xA0 },
	{ 0x17, 0x00 },
	{ 0x19, 0x2E },	
	{ 0x1D, 0x08 },
	{ 0x26, 0x00 },
	{ 0x27, 0x0C },
	{ 0x28, 0x00 },
	{ 0x29, 0x04 },
	{ 0x33, 0x1B },
	{ 0x34, 0x06 },
	{ 0x35, 0xA0 },
	{ 0x36, 0x00 },
	{ 0x38, 0x2E },	
	{ 0x3C, 0x08 },
	{ 0x45, 0x00 },
	{ 0x46, 0x0C },
	{ 0x47, 0x00 },
	{ 0x48, 0x04 },
	{ 0x52, 0x1B },
	{ 0x53, 0x06 },
	{ 0x54, 0xA0 },
	{ 0x55, 0x00 },
	{ 0x57, 0x2E },	
	{ 0x5B, 0x08 },
	{ 0x64, 0x00 },
	{ 0x65, 0x0C },
	{ 0x66, 0x00 },
	{ 0x67, 0x04 },
	{ 0x70, 0x01 },	
	{ 0x71, 0x02 },	
	{ 0x72, 0x04 },	
	{ 0x73, 0x0F },	
	{ 0x74, 0x0C },	
	{ 0x75, 0x03 },	
	{ 0x2A, 0x3F },	
	{ 0x49, 0x3F },	
	{ 0x68, 0x3F },	
	{ 0x7F, 0x04 },	//change to bank4
	{ 0x07, 0x00 },	
	{ 0x08, 0x00 },	
	{ 0x0B, 0x01 },	
	{ 0x0C, 0x01 },	
	{ 0x0F, 0x00 },	
	{ 0x10, 0x00 },	
	{ 0x15, 0x69 },	
	{ 0x2B, 0xFE },	
	{ 0x34, 0x01 },	
	{ 0x7F, 0x05 },	//change to bank5
	{ 0x04, 0x06 },	
	{ 0x5C, 0x08 },	
	{ 0x5D, 0x08 },	
	{ 0x5E, 0x00 },	
	{ 0x60, 0x35 },	
	{ 0x64, 0x14 },	
	{ 0x7F, 0x01 },	//change to bank1 
	{ 0x00, 0x03 },	
	{ 0x0B, 0x00 },		
	{ 0x0E, 0x10 },	
	{ 0x14, 0x03 },	
	{ 0x1B, 0x01 },
	{ 0x38, 0x01 },
	{ 0x39, 0x06 },
	{ 0x40, 0x01 },
	{ 0x41, 0x06 },
	{ 0x48, 0x01 },
	{ 0x49, 0x06 },
	{ 0x59, 0x81 },
	{ 0x21, 0x68 },		
	{ 0x23, 0x69 },	
	{ 0x25, 0x69 },	
	{ 0x75, 0x03 },	
	//{ 0x24, 0x01 },	//update flag
};

static const uint8_t pah8011_touch_register_array[][2] = {
	{ 0x7F, 0x05 },	//change to bank5
	{ 0x44, 0x1F },
	{ 0x7F, 0x01 },	//change to bank1
	{ 0x1C, 0x04 },
	{ 0x1D, 0x04 },
	{ 0x26, 0xA0 },
	{ 0x27, 0x0F },
	{ 0x12, 0x01 },
	{ 0x3C, 0x80 },
	{ 0x3D, 0x02 },
	{ 0x44, 0x80 },
	{ 0x45, 0x02 },
	{ 0x4C, 0x80 },
	{ 0x4D, 0x02 },
};

static const uint8_t pah8011_ppg_20hz_register_array[][2] = {
	{ 0x7F, 0x05 },	//change to bank5
	{ 0x44, 0x1F },
	{ 0x7F, 0x01 },	//change to bank1
	{ 0x1C, 0x04 },
	{ 0x1D, 0x04 },
	{ 0x26, 0x40 },
	{ 0x27, 0x06 },
	{ 0x12, 0x14 },
	{ 0x3C, 0x80 },
	{ 0x3D, 0x02 },
	{ 0x44, 0x80 },
	{ 0x45, 0x02 },
	{ 0x4C, 0x80 },
	{ 0x4D, 0x02 },
};

static const uint8_t pah8011_ppg_200hz_register_array[][2] = {
	{ 0x7F, 0x05 },	//change to bank5
	{ 0x44, 0x04 },
	{ 0x7F, 0x01 },	//change to bank1
	{ 0x1C, 0x01 },
	{ 0x1D, 0x01 },
	{ 0x26, 0xA0 },
	{ 0x27, 0x00 },
	{ 0x12, 0xC8 },
	{ 0x3C, 0xA0 },
	{ 0x3D, 0x00 },
	{ 0x44, 0xA0 },
	{ 0x45, 0x00 },
	{ 0x4C, 0xA0 },
	{ 0x4D, 0x00 },
	
};


#endif	// header guard

