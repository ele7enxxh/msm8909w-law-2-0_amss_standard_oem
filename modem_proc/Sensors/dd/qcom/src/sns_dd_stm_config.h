/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
*
* File Name         : stm_sensor_config.h
* Authors           : Jianjian Huo (samuel.huo@st.com)
* Version           : V 0.9.1
* Date              : 05/05/2011
* Description       : Header file to configure axis and polarity for each sensor.
*
********************************************************************************
* Copyright (c) 2011, STMicroelectronics.
* All rights reserved.
* 
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of the STMicroelectronics nor the
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
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

#ifndef __STM_CONFIG__
#define __STM_CONFIG__

/**
 * Map phone's SAE coordinates to accelerometer LIS3DH's axis settings
 * Value for each axis can be 0, 1 or 2.
 * 0 stands for X axis of LIS3DH device coordinate.
 * 1 stands for Y axis of LIS3DH device coordinate.
 * 2 stands for Z axis of LIS3DH device coordinate.
 */
#define STM_ACC_AXIS_MAP_X     0  
#define STM_ACC_AXIS_MAP_Y     1
#define STM_ACC_AXIS_MAP_Z     2

/**
 * Map phone's SAE coordinates to accelerometer LIS3DH's polarity settings
 * Value for each axis can be -1 or 1.
 * -1 stands for negative axis direction of LIS3DH's coordinate.
 * 1 stands for positive axis direction of LIS3DH's coordinate.
 */
#define STM_ACC_POLARITY_X    1
#define STM_ACC_POLARITY_Y    1
#define STM_ACC_POLARITY_Z    1

/**
 * Map device's SAE coordinates to magnetometer LSM303DLHC's axis settings
 * Value for each axis can be 0, 1 or 2.
 * 0 stands for X axis of LSM303DLHC's coordinate.
 * 1 stands for Y axis of LSM303DLHC's coordinate.
 * 2 stands for Z axis of LSM303DLHC's coordinate.
 */
#define STM_MAG_AXIS_MAP_X     0
#define STM_MAG_AXIS_MAP_Y     1
#define STM_MAG_AXIS_MAP_Z     2

/**
 * Map device's SAE coordinates to magnetometer LSM303DLHC's polarity settings
 * Value for each axis can be -1 or 1.
 * -1 stands for negative axis direction of LSM303DLHC's coordinate.
 * 1 stands for positive axis direction of LSM303DLHC's coordinate.
 */
#define STM_MAG_POLARITY_X     1
#define STM_MAG_POLARITY_Y     1
#define STM_MAG_POLARITY_Z     1

/**
 * Map device's SAE coordinates to gyroscope L3GD20's axis settings
 * Value for each axis can be 0, 1 or 2.
 * 0 stands for X axis of L3GD20's coordinate.
 * 1 stands for Y axis of L3GD20's coordinate.
 * 2 stands for Z axis of L3GD20's coordinate.
 */
#define STM_GYR_AXIS_MAP_X     0
#define STM_GYR_AXIS_MAP_Y     1
#define STM_GYR_AXIS_MAP_Z     2

/**
 * Map device's SAE coordinates to gyroscope L3GD20's polarity settings
 * Value for each axis can be -1 or 1.
 * -1 stands for negative axis direction of L3GD20's coordinate.
 * 1 stands for positive axis direction of L3GD20's coordinate.
 */
#define STM_GYR_POLARITY_X     1
#define STM_GYR_POLARITY_Y     1
#define STM_GYR_POLARITY_Z     1

#endif  /* __STM_CONFIG__ */
