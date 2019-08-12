/********************************************************************************

         S E N S O R S    A C C E L E R O M E T E R    D R I V E R  
* Copyright (c) 2013, "Memsic Inc."

* All rights reserved.

*

* Redistribution and use in source and binary forms, with or without

* modification, are permitted provided that the following conditions are met:

*     1. Redistributions of source code must retain the above copyright

*      notice, this list of conditions and the following disclaimer.

*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the

*       documentation and/or other materials provided with the distribution.

  
*     3. Neither the name of "Memsic Inc." nor the

*       names of its contributors may be used to endorse or promote products

*       derived from this software without specific prior written permission.

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

* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.)

*******************************************************************************/

#ifndef _SNS_DD_MMC3XXX_REF_H
#define _SNS_DD_MMC3XXX_REF_H

#define MMC3XXX_SENSOR_I2C_ADDR             0x30
#define MMC3XXX_SENSOR_ID_REG1_ADDR         0x10
#define MMC3XXX_SENSOR_ID_REG2_ADDR         0x20

#define MMC3XXX_SENSOR_CTRL0_REG_ADDR 	    0x07
#define MMC3XXX_SENSPR_FILLCAP              0x80
#define MMC3XXX_SENSOR_RESET       			0x40     /* B6 */
#define MMC3XXX_SENSOR_SET 				    0x20     /* B4 */
#define MMC3XXX_SENSOR_TM               	0x01     /* B0 */
#define MMC3XXX_SENSOR_CONTMODE             0x02
#define MMC3XXX_SENSOR_FREQ_50HZ            0x00
#define MMC3XXX_SENSOR_FREQ_25HZ            0x04
#define MMC3XXX_SENSOR_FREQ_12HZ            0x80
#define MMC3XXX_SENSOR_FREQ_1_5HZ           0xc0

#define MMC3XXX_SENSOR_CTRL1_REG_ADDR       0x8
#define MMC3XXX_SENSOR_BW_16_SLOW           0x00     
#define MMC3XXX_SENSOR_BW_16_FAST           0x01     
#define MMC3XXX_SENSOR_BW_14                0x02

#define MMC3XXX_SENSOR_STATUS_REG_ADDR      0x6
#define MMC3XXX_SENSOR_MEASURE_DONE         0x1
#define MMC3XXX_SENSOR_CHARGE_PUMP          0x2
#define MMC3XXX_SENSOR_READ_OTP             0x4

#define MMC3XXX_SENSOR_DATA_REG_ADDR        0x0

#define MMC3XXX_SENSOR_OPERATION_RANGE      16
#define MMC3XXX_SENSOR_BIT_LEN              13

#define MMC3XXX_SENSOR_TM_DELAY             10000

#define MMC3XXX_MAX_NUM                     3
#define SNS_DD_MAGNETIC_MAX_RANGE           3



typedef enum {
  MMC3280MA,
  MMC33160MT,
  MMC34160PJ,
  MMC3XXX
}MMC3XXX_TYPE;

typedef struct {
  char name[16];
  MMC3XXX_TYPE type;  
  int  bias;
  int  bit_len;
  int  freq;
  int  scale;
  int  range;
} MMC3XXX_PRODUCT_ID;

#endif /* End include guard _SNS_DD_MAGNETIC_REF_H */
