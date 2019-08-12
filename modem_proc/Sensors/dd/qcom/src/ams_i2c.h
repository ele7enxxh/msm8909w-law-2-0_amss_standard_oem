/********************************************************************************
* Copyright (c) 2014, "ams AG"
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     1. Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*     2. Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     3. Neither the name of "ams AG" nor the
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
/******************************************************************************
* Copyright (c) 2014, "ams AG"
* All rights reserved.
* THIS SOFTWARE IS PROVIDED FOR USE ONLY IN CONJUNCTION WITH AMS PRODUCTS.
* USE OF THE SOFTWARE IN CONJUNCTION WITH NON-AMS-PRODUCTS IS EXPLICITLY
* EXCLUDED.
*******************************************************************************/

#ifndef _AMS_I2C_H
#define _AMS_I2C_H

sns_ddf_status_e ams_i2c_write_byte
(
 sns_dd_state_t* state,
 REG_NAMES       reg,
 uint8_t         byte_val
);

sns_ddf_status_e ams_i2c_write_word
(
 sns_dd_state_t* state,
 REG_NAMES       reg,
 uint16_t        wordToWrite
);


sns_ddf_status_e ams_i2c_write_word_db
(
 sns_dd_state_t* state,
 REG_NAMES       reg,
 uint16_t        word_val
);


sns_ddf_status_e ams_i2c_read_buf
(
 sns_dd_state_t* state,
 REG_NAMES       startReg,
 uint8_t         byteCnt,
 uint8_t*        buf
);


sns_ddf_status_e ams_i2c_write_buf
(
 sns_dd_state_t* state,
 REG_NAMES       startReg,
 uint8_t         byteCnt,
 uint8_t*        buf
);


sns_ddf_status_e ams_i2c_write_buf_db
(
 sns_dd_state_t* state,
 uint8_t         byteCnt,
 uint8_t*        buf
);


sns_ddf_status_e ams_i2c_read_byte
(
 sns_dd_state_t* state,
 REG_NAMES       reg,
 uint8_t*        byte_val
);


sns_ddf_status_e ams_i2c_read_word
(
 sns_dd_state_t* state,
 REG_NAMES       regName,
 uint16_t*       word_val
);


sns_ddf_status_e ams_i2c_write_spec_func
(
 sns_ddf_handle_t port_handle,
 uint8_t          spec_func
);


uint8_t ams_usecToRegVal
(
 sns_dd_state_t*  state,
 uint32_t         time_us
);


uint16_t ams_setBits(uint16_t val, uint16_t bits);

uint16_t ams_getBits(uint16_t val, uint16_t bits);

uint16_t ams_clrBits(uint16_t val, uint16_t bits);

uint16_t ams_toggleBits(uint16_t val, uint16_t bits);

void ams_setBits_s(sns_dd_state_t* state, REG_NAMES reg, uint16_t bits);

uint16_t ams_getBits_s(sns_dd_state_t* state, REG_NAMES reg, uint16_t bits);

void ams_clrBits_s(sns_dd_state_t* state, REG_NAMES reg, uint16_t bits);

void ams_toggleBits_s(sns_dd_state_t* state, REG_NAMES reg, uint16_t bits);

void ams_setBits_r(sns_dd_state_t* state, REG_NAMES reg, uint16_t bits);

uint16_t ams_getBits_r(sns_dd_state_t* state, REG_NAMES reg, uint16_t bits);

void ams_clrBits_r(sns_dd_state_t* state, REG_NAMES reg, uint16_t bits);

void ams_toggleBits_r(sns_dd_state_t* state, REG_NAMES reg, uint16_t bits);

uint16_t ams_setField
(
 TMG399X_FIELD_NAMES fName,
 uint16_t            regVal,
 uint16_t            fVal
);

uint16_t ams_getField
(
 TMG399X_FIELD_NAMES fName,
 uint16_t            regVal
);

uint16_t ams_setField_s
(
 sns_dd_state_t*     state,
 TMG399X_FIELD_NAMES fName,
 uint16_t            fVal
);

uint16_t ams_getField_s
(
 sns_dd_state_t*     state,
 TMG399X_FIELD_NAMES fName
);

uint16_t ams_getField_r
(
 sns_dd_state_t*     state,
 TMG399X_FIELD_NAMES fName,
 sns_ddf_status_e*   status
);

sns_ddf_status_e ams_i2cWriteField_s
(
 sns_dd_state_t*     state,
 TMG399X_FIELD_NAMES fName,
 uint16_t            fVal
);

sns_ddf_status_e ams_i2cWriteField_r
(
 sns_dd_state_t*     state,
 TMG399X_FIELD_NAMES fName,
 uint16_t            fVal
);

sns_ddf_status_e ams_i2cWriteChangedShadowRegisters
(
 sns_dd_state_t* state
);

void ams_clearShadowChangedFlag
(
 sns_dd_state_t* state
);

#endif  // _AMS_I2C_H
