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

/*============================================================================
                                INCLUDE FILES
============================================================================*/
#include <string.h>

#include "ams_tmg399x.h"
#include "sns_dd_ams_tmg399x_priv.h"
#include "ams_i2c.h"


sns_ddf_status_e ams_i2c_write_byte
(
 sns_dd_state_t* state,
 REG_NAMES       regName,
 uint8_t         byte_val
)
{
    sns_ddf_status_e status;
    uint8_t          write_count;
    uint8_t          byteVal = byte_val;

    /* Sanity check input param */
    if(regName >= AMS_MAX_NUM_OF_REGS)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    status = sns_ddf_write_port(state->port_handle,
                                regAddr[regName],
                                &byteVal,
                                1,
                                &write_count
                                );
    if(status == SNS_DDF_SUCCESS)
    {
        state->chip.shadow.val[regName]     = byte_val;
        state->chip.shadow.changed[regName] = 0;
    }

    return status;
}


sns_ddf_status_e ams_i2c_write_word
(
 sns_dd_state_t* state,
 REG_NAMES       regName,
 uint16_t        wordToWrite
)
{
    sns_ddf_status_e status;
    uint8_t          write_count;
    uword_u          word_val;

    /* Sanity check input param */
    if(regName >= (AMS_MAX_NUM_OF_REGS - 1))
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    word_val.word = wordToWrite;

    status = sns_ddf_write_port(state->port_handle,
                                regAddr[regName],
                                &word_val.byte[0],
                                2,
                                &write_count
                                );
    if(status == SNS_DDF_SUCCESS)
    {
        state->chip.shadow.val[regName]     = word_val.byte[0];  // Lo byte
        state->chip.shadow.val[regName + 1] = word_val.byte[1];  // Hi byte
        state->chip.shadow.changed[regName]     = 0;
        state->chip.shadow.changed[regName + 1] = 0;
    }

    return status;
}


sns_ddf_status_e ams_i2c_write_word_db
(
 sns_dd_state_t* state,
 REG_NAMES       regName,
 uint16_t        word_val
)
{
    sns_ddf_status_e status;
    uint8_t          write_count;
    uword_u          wordVal;

    /* Sanity check input param */
    if(regName >= AMS_MAX_NUM_OF_REGS)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    wordVal.word = word_val;

    status = sns_ddf_write_port(state->port_handle,
                                regAddr[regName],
                                &wordVal.byte[0],
                                2,
                                &write_count
                                );

    return status;
}


sns_ddf_status_e ams_i2c_read_buf
(
 sns_dd_state_t* state,
 REG_NAMES       startReg,
 uint8_t         byteCnt,
 uint8_t*        buf)
{
    sns_ddf_status_e status;
    uint8_t          read_count;

    status = sns_ddf_read_port(state->port_handle,
                               regAddr[startReg],
                               buf,
                               byteCnt,
                               &read_count
                               );
    return status;
}


sns_ddf_status_e ams_i2c_write_buf
(
 sns_dd_state_t* state,
 REG_NAMES       startReg,
 uint8_t         byteCnt,
 uint8_t*        buf)
{
    sns_ddf_status_e status;
    uint8_t          read_count;

    status = sns_ddf_write_port(state->port_handle,
                               regAddr[startReg],
                               buf,
                               byteCnt,
                               &read_count
                               );
    return status;
}


sns_ddf_status_e ams_i2c_write_buf_db
(
 sns_dd_state_t* state,
 uint8_t         byteCnt,
 uint8_t*        buf)
{
    sns_ddf_status_e status;


    status = ams_i2c_write_buf(state, AMS_REG_I2C_DB, byteCnt, buf);

    return status;
}


sns_ddf_status_e ams_i2c_read_byte
(
 sns_dd_state_t* state,
 REG_NAMES       regName,
 uint8_t*        byte_val
)
{
    sns_ddf_status_e status;
    uint8_t          read_count;


    /* Sanity check input param */
    if(regName >= AMS_MAX_NUM_OF_REGS)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    status = sns_ddf_read_port(state->port_handle,
                               regAddr[regName],
                               byte_val,
                               1,
                               &read_count
                               );
    return status;
}


sns_ddf_status_e ams_i2c_read_word
(
 sns_dd_state_t* state,
 REG_NAMES       regName,
 uint16_t*       word_val
)
{
    sns_ddf_status_e status;
    uint8_t          read_count;
    uword_u          word_tmp;


    /* Sanity check input param */
    if(regName >= AMS_MAX_NUM_OF_REGS)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    status = sns_ddf_read_port(state->port_handle,
                               regAddr[regName],
                               &word_tmp.byte[0],
                               2,
                               &read_count
                               );
    if(status == SNS_DDF_SUCCESS)
    {
        *word_val = word_tmp.word;
    }
    return status;
}


/*!
 ***************************************
 * @brief Write special function to the Tmg399x command register
 *
 * Will write special function byte to slave device, and dummy byte will be ignored.
 *
 * @param port_handle : Handle to port data
 * @param spec_func   : Special function to write to command register
 * - 0x00  - Normal operation
 * - 0x05  - Proximity interrupt clear
 * - 0x06  - ALS interrupt clear
 * - 0x07  - Proximity and ALS interrupt clear
 * - other - Reserved, do NOT write
 *
 * @return status
 * - SNS_DDF_SUCCESS    - No error
 * - SNS_DDF_EDEVICE    - Comm error
 * - SNS_DDF_EBUS       - Comm error
 * - SNS_EINVALID_PARAM - Incorrect Config parameters
 ***************************************
 */
sns_ddf_status_e ams_i2c_write_spec_func
(
 sns_ddf_handle_t port_handle,
 uint8_t          intrClr
)
{
    sns_ddf_status_e status;
    uint8_t          rw_count;

    status = sns_ddf_write_port(port_handle,
				 intrClr,
				 NULL,
				 0,
				 &rw_count
				 );

    return status;
}


uint8_t ams_usecToRegVal
(
 sns_dd_state_t*  state,
 uint32_t         time_us
)
{
    uint8_t  integrationCycles;
    uint32_t minIntegTime_us = state->chip.Min_Integ_Time_us;

    if(time_us < minIntegTime_us)
    {
        time_us = minIntegTime_us;
    }

    integrationCycles = (((time_us * 16) / minIntegTime_us) + 8) / 16;

    return (256 - integrationCycles);
}


uint16_t ams_setBits(uint16_t val, uint16_t bits)
{
    return (val | bits);
}


uint16_t ams_getBits(uint16_t val, uint16_t bits)
{
    return (val & bits);
}


uint16_t ams_clrBits(uint16_t val, uint16_t bits)
{
    return (val & ~bits);
}


uint16_t ams_toggleBits(uint16_t val, uint16_t bits)
{
    return (val ^ bits);
}


void ams_setBits_s(sns_dd_state_t* state, REG_NAMES reg, uint16_t bits)
{
    state->chip.shadow.val[reg] = ams_setBits(state->chip.shadow.val[reg], bits);
}


uint16_t ams_getBits_s(sns_dd_state_t* state, REG_NAMES reg, uint16_t bits)
{
    return ams_getBits(state->chip.shadow.val[reg], bits);
}


void ams_clrBits_s(sns_dd_state_t* state, REG_NAMES reg, uint16_t bits)
{
    state->chip.shadow.val[reg] = ams_clrBits(state->chip.shadow.val[reg], bits);
}


void ams_toggleBits_s(sns_dd_state_t* state, REG_NAMES reg, uint16_t bits)
{
    state->chip.shadow.val[reg] = ams_toggleBits(state->chip.shadow.val[reg], bits);
}


void ams_setBits_r(sns_dd_state_t* state, REG_NAMES reg, uint16_t bits)
{
    uint8_t regVal;

    ams_i2c_read_byte(state, reg, &regVal);
    ams_i2c_write_byte(state, reg, ams_setBits(regVal, bits));
}


uint16_t ams_getBits_r(sns_dd_state_t* state, REG_NAMES reg, uint16_t bits)
{
    uint8_t regVal;

    ams_i2c_read_byte(state, reg, &regVal);
    return ams_getBits(regVal, bits);
}


void ams_clrBits_r(sns_dd_state_t* state, REG_NAMES reg, uint16_t bits)
{
    uint8_t regVal;

    ams_i2c_read_byte(state, reg, &regVal);
    ams_i2c_write_byte(state, reg, ams_clrBits(regVal, bits));
}


void ams_toggleBits_r(sns_dd_state_t* state, REG_NAMES reg, uint16_t bits)
{
    uint8_t regVal;

    ams_i2c_read_byte(state, reg, &regVal);
    ams_i2c_write_byte(state, reg, ams_toggleBits(regVal, bits));
}



uint16_t ams_setField
(
 TMG399X_FIELD_NAMES fName,
 uint16_t            regVal,
 uint16_t            fVal
)
{
    uint16_t bitMask;
    uint16_t regHole;
    uint16_t newVal;

    uint8_t  numBits = tmg399x[fName].numBits;
    uint8_t  bitPos  = tmg399x[fName].bitPos;

    bitMask = bitFieldMask[numBits - 1];
    regHole = regVal & ~(bitMask << bitPos);
    newVal  = regHole | ((fVal & bitMask) << bitPos);

    return newVal;
}


uint16_t ams_getField
(
 TMG399X_FIELD_NAMES fName,
 uint16_t            regVal
)
{
    uint16_t bitMask;
    uint16_t regHole;
    uint16_t fVal;

    uint8_t  numBits = tmg399x[fName].numBits;
    uint8_t  bitPos  = tmg399x[fName].bitPos;

    bitMask = bitFieldMask[numBits - 1];
    regHole = regVal >> bitPos;
    fVal    = regHole & bitMask;

    return fVal;
}


uint16_t ams_setField_s
(
 sns_dd_state_t*     state,
 TMG399X_FIELD_NAMES fName,
 uint16_t            fVal
)
{
    uint16_u  new;
    REG_NAMES regName = tmg399x[fName].regName;
    uint8_t   numBits = tmg399x[fName].numBits;

    if(regName >= AMS_MAX_NUM_OF_REGS)
    {
        return 0xFFFF; 
    }

    if((numBits > 8) && (regName >= (AMS_MAX_NUM_OF_REGS - 1)))
    {
        return 0xFFFF; 
    }

    new.val = ams_setField(fName, state->chip.shadow.val[regName], fVal);

    state->chip.shadow.val[regName]     = new.byte[0];
    state->chip.shadow.changed[regName] = 1;

    if(numBits > 8)
    {
        state->chip.shadow.val[regName + 1]     = new.byte[1];
        state->chip.shadow.changed[regName + 1] = 1;
    }

    return new.val;
}


uint16_t ams_getField_s
(
 sns_dd_state_t*     state,
 TMG399X_FIELD_NAMES fName
)
{
    uint16_u fVal;
    uint16_u new;

    REG_NAMES regName = tmg399x[fName].regName;
    uint8_t   numBits = tmg399x[fName].numBits;

    if(regName >= AMS_MAX_NUM_OF_REGS)
    {
        return 0xFFFF; 
    }

    if((numBits > 8) && (regName >= (AMS_MAX_NUM_OF_REGS - 1)))
    {
        return 0xFFFF; 
    }

    fVal.val     = 0;
    fVal.byte[0] = state->chip.shadow.val[regName];

    if(numBits > 8)
    {
        fVal.byte[1] = state->chip.shadow.val[regName + 1];
    }

    new.val = ams_getField(fName, fVal.val);

    return new.val;
}


uint16_t ams_getField_r
(
 sns_dd_state_t*     state,
 TMG399X_FIELD_NAMES fName,
 sns_ddf_status_e*   status
)
{
    uint16_u  reg;
    uint16_u  field;

    REG_NAMES regName = tmg399x[fName].regName;
    uint8_t   numBits = tmg399x[fName].numBits;

    reg.val = 0;
    *status = ams_i2c_read_byte(state, regName, &reg.byte[0]); if(SNS_DDF_SUCCESS != *status) return 0;

    if(numBits > 8)
    {
        *status = ams_i2c_read_byte(state, (regName + 1), &reg.byte[1]); if(SNS_DDF_SUCCESS != *status) return 0;
    }

    field.val = ams_getField(fName, reg.val);

    return field.val;
}


sns_ddf_status_e ams_i2cWriteField_s
(
 sns_dd_state_t*     state,
 TMG399X_FIELD_NAMES fName,
 uint16_t            fVal
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    uint16_u original;
    uint16_u new;

    /* Sanity check input param */
    if(fName >= MAX_NUM_OF_FIELDS)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    REG_NAMES regName = tmg399x[fName].regName;
    uint8_t   numBits = tmg399x[fName].numBits;

    if(regName >= AMS_MAX_NUM_OF_REGS)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    if(numBits > MAX_NUM_OF_BITS)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    if((numBits > 8) && (regName >= (AMS_MAX_NUM_OF_REGS - 1)))
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    original.val     = 0;
    original.byte[0] = state->chip.shadow.val[regName];

    if(numBits > 8)
    {
        original.byte[1] = state->chip.shadow.val[regName + 1];
    }

    new.val = ams_setField(fName, original.val, fVal);

    if(new.val != original.val)
    {
        if(numBits > 8)
        {
            status = ams_i2c_write_word(state, regName, new.val);
        }
        else
        {
            status = ams_i2c_write_byte(state, regName, new.byte[0]);
        }
    }

    return status;
}


sns_ddf_status_e ams_i2cWriteField_r
(
 sns_dd_state_t*     state,
 TMG399X_FIELD_NAMES fName,
 uint16_t            fVal
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    uint16_u  original;
    uint16_u  new;

    /* Sanity check input param */
    if(fName >= MAX_NUM_OF_FIELDS)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    REG_NAMES regName = tmg399x[fName].regName;
    uint8_t   numBits = tmg399x[fName].numBits;

    if(regName >= AMS_MAX_NUM_OF_REGS)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    if(numBits > MAX_NUM_OF_BITS)
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    original.val = 0;

    if(numBits > 8)
    {
        status = ams_i2c_read_word(state, regName, &original.val);
    }
    else
    {
        status = ams_i2c_read_byte(state, regName, &original.byte[0]);
    }
    if(SNS_DDF_SUCCESS != status) return status;

    new.val = ams_setField(fName, original.val, fVal);

    if(new.val != original.val)
    {
        if(numBits > 8)
        {
            status = ams_i2c_write_word(state, regName, new.val);
        }
        else
        {
            status = ams_i2c_write_byte(state, regName, new.byte[0]);
        }
    }

    return status;
}


sns_ddf_status_e ams_i2cWriteChangedShadowRegisters
(
 sns_dd_state_t* state
)
{
    sns_ddf_status_e status = SNS_DDF_SUCCESS;

    REG_NAMES regName;

    for(regName = 1; regName < AMS_MAX_NUM_OF_REGS; ++regName)
    {
        if(state->chip.shadow.changed[regName])
        {
            status = ams_i2c_write_byte(state, regName, state->chip.shadow.val[regName]);
            if(SNS_DDF_SUCCESS != status) return status;
        }
    }

    return status;
}


void ams_clearShadowChangedFlag
(
 sns_dd_state_t* state
)
{
    REG_NAMES regName;

    for(regName = 0; regName < AMS_MAX_NUM_OF_REGS; ++regName)
    {
        state->chip.shadow.changed[regName] = 0;
    }
}
