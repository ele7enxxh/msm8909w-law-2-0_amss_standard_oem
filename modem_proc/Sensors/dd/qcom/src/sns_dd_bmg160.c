/*  Date: 2015/06/01  15:00:00
 *  Revision: 2.3.20
 */


/*******************************************************************************
 * Copyright (c) 2014, Bosch Sensortec GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     3. Neither the name of Bosch Sensortec GmbH nor the
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


/*==============================================================================

  S E N S O R S    G Y R O S C O P E    D R I V E R

  DESCRIPTION

  Impelements the driver for the Gyroscope sensor

  EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.




  version         when       who     what, where, why

          03/03/2012 albert  set attr, init, get data,get attr,reset
          07/24/2012 albert  self test
          04/08/2013 albert  set sensor to suspend at reset v1.8
          04/19/2013 albert  support DRI  v1.9
          07/08/2013 albert  support bmg of bmi058 v2.0
          11/11/2013 lk      FIFO wml interrupt
          02/24/2014 lk      add an old way of ODR handling for the customer S
          03/14/2014 lk      add an old way of ODR handling and downsampling for the customer S
          03/14/2014 mw      merge the self test code fix
          03/14/2014 mw      merge the code fix for static analysis from KW
          03/14/2014 lk      merge the code from qcm and bst, update version information
          03/15/2014 lk      interval review by bst
          03/24/2014 lk      use dev_select to handle chips variations
          04/02/2014 lk      heavy testing performed with qcm
          04/11/2014 lk      add SNS_DDF_ATTRIB_RESOLUTION
          04/11/2014 lk      set low power mode to BMG160_MODE_DEEPSUSPEND, add code to detect BMI058 automatically
          04/17/2014 lk      set range to +/-2000 by default
          04/25/2014 lk      add config for fastest odr
          04/25/2014 MW      eliminate compiler warning
          05/04/2014 lk      fix the timestamp issue when wml is 0
          05/12/2014 lk      remove delay(), config_range(), config_bw() in reset()
          06/04/2014 lk      reset(): set mode to deep suspend, added 400Hz ODR support
          06/30/2014 albert  fixed fifo data axis swap issue v2.3.4.9
          07/01/2014 albert  fixed fifo data lost sample time issue v2.3.5
          07/09/2014 lk      use level triggering for FIFO
          07/10/2014 lk      add suport for Dragonboard
          07/29/2014 vy      refactored for uImage support
  2.3.5.3         08/03/2014 lk      work around for missing call of set_attr(SNS_DDF_ATTRIB_FIFO)
  2.3.5.4         08/03/2014 lk      fix the static noise issue
  2.3.5.5         08/20/2014 lk      fix the static noise issue: splitting the valid/invalid samples
  2.3.5.6         08/21/2014 lk      change log types from ERROR to HIGH, disable excessive logs
  2.3.5.7         08/22/2014 lk      move read_wrapper()/write_wrapper()/log_data_fifo() to bigimage part
  2.3.5.8         08/24/14   vy      Used new DDF apis to allocate/free mem supported in uImage
  2.3.5.9         08/27/2014 sd      fix the wrong end_timestamp for 2nd part of a split sensor batch
  2.3.8.1         09/02/2014 lk      use bmg160_sbus_read()/bmg160_sbus_write() for ease of debugging
  2.3.9           09/02/2014 lk      use edge triggering by default
  2.3.10          09/16/2014 MW      Skip data logging (0x14c1 logs) for invalid samples
  2.3.11          09/17/2014 VY      Updated memhandler api in probe fn
  2.3.12          09/24/2014 lk      Don't clear FIFO when FIFO sched is disabled
  2.3.13          10/17/2014 pn      Allows clearing watermark when ODR is already cleared
  2.3.14          11/11/2014 pn      Uses BMG160_CONFIG_SKIP_SAMPLE_CNT_ODRC as is
  2.3.15           02/05/2015 lk      improve on the debug messages for easier debugging
  2.3.16          05/08/2015 albert      added adjust group filter delay
  2.3.17		05/19/2015 albert	     modify set fifo attr for TS gap issue during change wml.
  2.3.18		05/20/2015 albert	     merge log from v2.3.15
  2.3.19		05/25/2015 albert 	     improve design for 2.3.17 's change
  2.3.20		06/01/2015 albert     WA for TS issue,   reset the invalid data to  the first valid data, reset the status to success 
  ----------   ---     -----------------------------------------------------------

  ==============================================================================*/

/*============================================================================

  INCLUDE FILES

  ============================================================================*/


#include "sns_dd_bmg160.h"

#define BMG160_SNS_DDF_ATTRIB_VERSION   2032000

typedef enum {
    BMG160_FIFO_MODE_BYPASS = 0,
    BMG160_FIFO_MODE_FIFO,
    BMG160_FIFO_MODE_STREAM,
    BMG160_FIFO_MODE_RESERVED
} BMG160_FIFO_MODE;

struct bst_val_pair {
    uint32_t l;
    uint32_t r;
};

extern struct bmg160 *p_bmg160;

extern sns_ddf_handle_t bmg160_port_handle;

extern sns_ddf_driver_if_s sns_bmg160_driver_fn_list;

extern float bmg160_lsb_res_rad;

static const uint32_t  sns_dd_gyro_range[5] =
{
    BOSCH_GYRO_SENSOR_RANGE_125,
    BOSCH_GYRO_SENSOR_RANGE_250,
    BOSCH_GYRO_SENSOR_RANGE_500,
    BOSCH_GYRO_SENSOR_RANGE_1000,
    BOSCH_GYRO_SENSOR_RANGE_2000
};

static const uint32_t  sns_dd_gyro_bw[8] =
{
    BOSCH_GYRO_SENSOR_BW_12HZ,
    BOSCH_GYRO_SENSOR_BW_23HZ,
    BOSCH_GYRO_SENSOR_BW_32HZ,
    BOSCH_GYRO_SENSOR_BW_47HZ,
    BOSCH_GYRO_SENSOR_BW_64HZ,
    BOSCH_GYRO_SENSOR_BW_116HZ,
    BOSCH_GYRO_SENSOR_BW_230HZ,
    BOSCH_GYRO_SENSOR_BW_523HZ
};

static const struct bst_val_pair BMG160_MAP_BW_ODR[] = {
    {BOSCH_GYRO_SENSOR_BW_12HZ, 100},
    {BOSCH_GYRO_SENSOR_BW_23HZ, 200},
    {BOSCH_GYRO_SENSOR_BW_32HZ, 100},
    {BOSCH_GYRO_SENSOR_BW_47HZ, 400},
    {BOSCH_GYRO_SENSOR_BW_64HZ, 200},
    {BOSCH_GYRO_SENSOR_BW_116HZ, 1000},
    {BOSCH_GYRO_SENSOR_BW_230HZ, 2000},
    {BOSCH_GYRO_SENSOR_BW_523HZ, 2000}
};

static const uint32_t sns_dd_gyro_freq[8] =
{
    BOSCH_GYRO_SENSOR_BW_VALUE_12HZ,
    BOSCH_GYRO_SENSOR_BW_VALUE_23HZ,
    BOSCH_GYRO_SENSOR_BW_VALUE_32HZ,
    BOSCH_GYRO_SENSOR_BW_VALUE_47HZ,
    BOSCH_GYRO_SENSOR_BW_VALUE_64HZ,
    BOSCH_GYRO_SENSOR_BW_VALUE_116HZ,
    BOSCH_GYRO_SENSOR_BW_VALUE_230HZ,
    BOSCH_GYRO_SENSOR_BW_VALUE_523HZ
};


#if BMG160_CONFIG_ENABLE_TS_ADJUST_FILTER



DD_ATTR_LOCAL const
uint32_t BMG160_FILTER_BWP2_GD_US_LIST_GYR[] =
{
   1250,  //@525HZ 
   2500,  //@240HZ	
   4000,  //@125HZ
   9000,  //@45HZ
   18000,  //@23HZ
   35000, //@12HZ
   6000, //@63HZ
   11000, //@32HZ

};



#endif

extern sns_ddf_status_e sns_dd_bmg160_reg_dump(sns_dd_bmg160_state_t *state);

extern sns_ddf_status_e bmg_read_wrapper(
        uint8_t dev_addr,
        uint8_t reg_addr,
        uint8_t *bosch_read_data,
        uint8_t len);

extern sns_ddf_status_e bmg_write_wrapper(uint8_t dev_addr,
        uint8_t reg_addr,
        uint8_t *bosch_write_buffer,
        uint8_t len);

extern void bmg_delay(uint32_t msec);

static sns_ddf_status_e sns_dd_bmg160_set_attr_fifo(
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        void*                value);

/********************************************************
  FUNCTION DEFINITIONS
 *********************************************************/
static sns_ddf_status_e bmg160_calc_matching_wml(
        uint8_t desired_wml,
        uint8_t downsampling_factor,
        uint8_t *cal_wml)
{
    uint8_t i;

    for (i = desired_wml; i > 0; i--) {
        if ((0 == (desired_wml % i))
                && ((i * downsampling_factor)
                    <= (BMG160_MAX_FIFO_LEVEL - BMG160_FIFO_HEAD_ROOM))) {
            *cal_wml = i * downsampling_factor;
            return SNS_DDF_SUCCESS;
        }
    }

    return SNS_DDF_EINVALID_PARAM;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_set_fifo_tag(unsigned char tag)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        if (tag < C_BMG160_Two_U8X) {
            comres = p_bmg160->BMG160_BUS_READ_FUNC\
                     (p_bmg160->dev_addr,\
                      BMG160_FIFO_CGF1_ADDR_TAG__REG, &v_data_u8r, 1);
            v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
                    BMG160_FIFO_CGF1_ADDR_TAG, tag);
            comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
                     (p_bmg160->dev_addr,\
                      BMG160_FIFO_CGF1_ADDR_TAG__REG, &v_data_u8r, 1);
        } else {
            comres = SNS_DDF_EINVALID_PARAM;
        }
    }
    return comres;
}


static BMG160_RETURN_FUNCTION_TYPE bmg160_set_fifo_watermark_enable(unsigned char fifo_wn_en)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL)
    {
        comres = SNS_DDF_EINVALID_PARAM;
    }
    else
    {
        if (fifo_wn_en < C_BMG160_Two_U8X)
        {
            comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr, BMG160_INT_4_FIFO_WM_EN__REG, &v_data_u8r, 1);
            v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r, BMG160_INT_4_FIFO_WM_EN, fifo_wn_en);
            comres = p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_INT_4_FIFO_WM_EN__REG, &v_data_u8r, 1);
        }
        else
        {
            comres = SNS_DDF_EINVALID_PARAM;
        }
    }
    return comres;
}


static BMG160_RETURN_FUNCTION_TYPE bmg160_enable_int_drdy(unsigned char data_en)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        comres = p_bmg160->BMG160_BUS_READ_FUNC\
                 (p_bmg160->dev_addr,\
                  BMG160_INT_ENABLE0_DATAEN__REG, &v_data_u8r, 1);
        v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
                BMG160_INT_ENABLE0_DATAEN, data_en);
        comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
                 (p_bmg160->dev_addr,\
                  BMG160_INT_ENABLE0_DATAEN__REG, &v_data_u8r, 1);
    }
    return comres;
}


static BMG160_RETURN_FUNCTION_TYPE bmg160_enable_int_fifo(unsigned char fifo_en)
{
    BMG160_RETURN_FUNCTION_TYPE comres= SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL)
    {
        comres = SNS_DDF_EINVALID_PARAM;
    }
    else
    {
        if (fifo_en < C_BMG160_Two_U8X)
        {
            comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr, BMG160_INT_ENABLE0_FIFOEN__REG, &v_data_u8r, 1);
            v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r, BMG160_INT_ENABLE0_FIFOEN, fifo_en);
            comres = p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_INT_ENABLE0_FIFOEN__REG, &v_data_u8r, 1);
        }
        else
        {
            comres = SNS_DDF_EINVALID_PARAM;
        }
    }
    return comres;
}


static sns_ddf_status_e bmg160_enable_int_fifo_wm(
        sns_ddf_handle_t port_handle,
        int en)
{
    sns_ddf_status_e status;


    status = bmg160_set_fifo_watermark_enable(!!en);

    return status;
}


static BMG160_RETURN_FUNCTION_TYPE bmg160_map_int1_fifo(unsigned char fifo_int1)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL)
    {
        comres = SNS_DDF_EINVALID_PARAM;
    }
    else
    {
        if (fifo_int1 < C_BMG160_Two_U8X)
        {
            comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr, BMG160_MAP_1_INT1_FIFO__REG, &v_data_u8r, 1);
            v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r, BMG160_MAP_1_INT1_FIFO, fifo_int1);
            comres = p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_MAP_1_INT1_FIFO__REG, &v_data_u8r, 1);
        }
        else
        {
            comres = SNS_DDF_EINVALID_PARAM;
        }
    }
    return comres;
}


static BMG160_RETURN_FUNCTION_TYPE bmg160_map_int1_drdy(unsigned char en)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL)
    {
        comres = SNS_DDF_EINVALID_PARAM;
    }
    else
    {
        comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr, BMG160_MAP_1_INT1_DATA__REG, &v_data_u8r, 1);
        v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r, BMG160_MAP_1_INT1_DATA, !!en);
        comres = p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_MAP_1_INT1_DATA__REG, &v_data_u8r, 1);
    }
    return comres;
}


BMG160_RETURN_FUNCTION_TYPE bmg160_init(struct bmg160 *bmg160)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;

    p_bmg160 = bmg160;

    return comres;
}


BMG160_RETURN_FUNCTION_TYPE bmg160_set_bw(unsigned char bandwidth)
{
    BMG160_RETURN_FUNCTION_TYPE comres;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL)
    {
        comres = SNS_DDF_EINVALID_PARAM;
    }
    else
    {
        if (bandwidth < C_BMG160_Eight_U8X)
        {
            comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,
                    BMG160_DSP_BW_ADDR_BW__REG, &v_data_u8r, 1);
            v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,
                    BMG160_DSP_BW_ADDR_BW, bandwidth);
            comres = p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr,
                    BMG160_DSP_BW_ADDR_BW__REG, &v_data_u8r, 1);
        }
        else
        {
            comres = SNS_DDF_EINVALID_PARAM;
        }
    }
    return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_set_range_reg(unsigned char range)
{
    BMG160_RETURN_FUNCTION_TYPE comres;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL)
    {
        comres = SNS_DDF_EINVALID_PARAM;
    }
    else
    {
        if (range < C_BMG160_Five_U8X)
        {
            comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr, BMG160_DSP_RANGE_ADDR_RANGE__REG, &v_data_u8r, 1);
            v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r, BMG160_DSP_RANGE_ADDR_RANGE, range);
            comres = p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_DSP_RANGE_ADDR_RANGE__REG, &v_data_u8r, 1);
        }
        else
        {
            comres = SNS_DDF_EINVALID_PARAM;
        }
    }
    return comres;
}


BMG160_RETURN_FUNCTION_TYPE bmg160_set_soft_reset()
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_SoftReset_u8r;
    v_SoftReset_u8r = 0xB6;
    if (p_bmg160 == BMG160_NULL)
    {
        comres = SNS_DDF_EINVALID_PARAM;
    }
    else
    {
        comres = p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_BA_SOFTRESET_ADDR, &v_SoftReset_u8r, 1);
    }
    return comres;
}


BMG160_RETURN_FUNCTION_TYPE bmg160_set_mode(uint8_t mode)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char data1,data2;
    if (p_bmg160 == BMG160_NULL)
    {
        comres = SNS_DDF_EINVALID_PARAM;
    }
    else
    {
        if (mode < C_BMG160_Six_U8X)
        {
            comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr, BMG160_MODE_CTRL_REG, &data1, C_BMG160_One_U8X );
            comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr, BMG160_PMU_LPW1_ADDR, &data2, C_BMG160_One_U8X );
            switch (mode)
            {
                case BMG160_MODE_NORMAL:
                    data1  = BMG160_SET_BITSLICE(data1, BMG160_MODE_CTRL, C_BMG160_Zero_U8X);
                    data2  = BMG160_SET_BITSLICE(data2, BMG160_PMU_LPW1_ADDR_FAST_POWERUP, C_BMG160_Zero_U8X);
                    break;
                case BMG160_MODE_LOWPOWERNORMAL:
                    data1  = BMG160_SET_BITSLICE(data1, BMG160_MODE_CTRL, C_BMG160_Two_U8X);
                    data2  = BMG160_SET_BITSLICE(data2, BMG160_PMU_LPW1_ADDR_FAST_POWERUP, C_BMG160_Zero_U8X);
                    break;
                case BMG160_MODE_SUSPENDNORMAL:
                    data1  = BMG160_SET_BITSLICE(data1, BMG160_MODE_CTRL, C_BMG160_Four_U8X);
                    data2  = BMG160_SET_BITSLICE(data2, BMG160_PMU_LPW1_ADDR_FAST_POWERUP, C_BMG160_Zero_U8X);
                    break;
                case BMG160_MODE_DEEPSUSPEND:
                    data1  = BMG160_SET_BITSLICE(data1, BMG160_MODE_CTRL, C_BMG160_One_U8X);
                    data2  = BMG160_SET_BITSLICE(data2, BMG160_PMU_LPW1_ADDR_FAST_POWERUP, C_BMG160_Zero_U8X);
                    break;
                default:
                    break;
            }
            comres += p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_MODE_CTRL_REG, &data1, C_BMG160_One_U8X);
            p_bmg160->delay_msec(1);
            comres += p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_PMU_LPW1_ADDR, &data2, C_BMG160_One_U8X);
        }
        else
        {
            comres = SNS_DDF_EINVALID_PARAM ;
        }
    }
    return comres;
}


BMG160_RETURN_FUNCTION_TYPE bmg160_set_int2_fifo(unsigned char fifo_int2)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL)
    {
        comres = SNS_DDF_EINVALID_PARAM;
    }
    else
    {
        if (fifo_int2 < C_BMG160_Two_U8X)
        {
            comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr, BMG160_MAP_1_INT2_FIFO__REG, &v_data_u8r, 1);
            v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r, BMG160_MAP_1_INT2_FIFO, fifo_int2);
            comres = p_bmg160->BMG160_BUS_WRITE_FUNC(p_bmg160->dev_addr, BMG160_MAP_1_INT2_FIFO__REG, &v_data_u8r, 1);
        }
        else
        {
            comres = SNS_DDF_EINVALID_PARAM;
        }
    }
    return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_set_int_data(unsigned char axis, unsigned char int_data)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        switch (axis) {
            case BMG160_INT1_DATA:
                comres = p_bmg160->BMG160_BUS_READ_FUNC\
                         (p_bmg160->dev_addr,\
                          BMG160_MAP_1_INT1_DATA__REG, &v_data_u8r, 1);
                v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
                        BMG160_MAP_1_INT1_DATA, int_data);
                comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
                         (p_bmg160->dev_addr,\
                          BMG160_MAP_1_INT1_DATA__REG, &v_data_u8r, 1);
                break;
            case BMG160_INT2_DATA:
                comres = p_bmg160->BMG160_BUS_READ_FUNC\
                         (p_bmg160->dev_addr,\
                          BMG160_MAP_1_INT2_DATA__REG, &v_data_u8r, 1);
                v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
                        BMG160_MAP_1_INT2_DATA, int_data);
                comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
                         (p_bmg160->dev_addr,\
                          BMG160_MAP_1_INT2_DATA__REG, &v_data_u8r, 1);
                break;
            default:
                comres = SNS_DDF_EINVALID_PARAM;
                break;
        }
    }
    return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_tag(unsigned char *tag)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,\
                BMG160_FIFO_CGF1_ADDR_TAG__REG, &v_data_u8r, 1);
        *tag = BMG160_GET_BITSLICE(v_data_u8r,\
                BMG160_FIFO_CGF1_ADDR_TAG);
    }
    return comres;
}


BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_watermarklevel(
        unsigned char *water_mark_level)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,\
                BMG160_FIFO_CGF1_ADDR_WML__REG, &v_data_u8r, 1);
        *water_mark_level = BMG160_GET_BITSLICE(v_data_u8r,\
                BMG160_FIFO_CGF1_ADDR_WML);
    }
    return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_set_fifo_watermarklevel(
        unsigned char water_mark_level)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        if (water_mark_level < C_BMG160_OneTwentyEight_U8X) {
            comres = p_bmg160->BMG160_BUS_READ_FUNC\
                     (p_bmg160->dev_addr,\
                      BMG160_FIFO_CGF1_ADDR_WML__REG, &v_data_u8r, 1);
            v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
                    BMG160_FIFO_CGF1_ADDR_WML, water_mark_level);
            comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
                     (p_bmg160->dev_addr,\
                      BMG160_FIFO_CGF1_ADDR_WML__REG, &v_data_u8r, 1);
        } else {
            comres = SNS_DDF_EINVALID_PARAM;
        }
    }
    return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_data_reg(unsigned char *fifo_data)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,\
                BMG160_FIFO_DATA_ADDR, &v_data_u8r, 1);
        *fifo_data = v_data_u8r;
    }
    return comres;
}


BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_overrun(
        unsigned char *fifo_overrun)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,\
                BMG160_FIFO_STATUS_OVERRUN__REG, &v_data_u8r, 1);
        *fifo_overrun = BMG160_GET_BITSLICE(v_data_u8r,\
                BMG160_FIFO_STATUS_OVERRUN);
    }
    return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_mode(unsigned char *mode)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,\
                BMG160_FIFO_CGF0_ADDR_MODE__REG, &v_data_u8r, 1);
        *mode = BMG160_GET_BITSLICE(v_data_u8r,\
                BMG160_FIFO_CGF0_ADDR_MODE);
    }
    return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_set_fifo_mode(unsigned char mode)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        if (mode < C_BMG160_Four_U8X) {
            comres = p_bmg160->BMG160_BUS_READ_FUNC\
                     (p_bmg160->dev_addr,\
                      BMG160_FIFO_CGF0_ADDR_MODE__REG, &v_data_u8r, 1);
            v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
                    BMG160_FIFO_CGF0_ADDR_MODE, mode);
            v_data_u8r |= (0x03 << 2); /* WA */
            comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
                     (p_bmg160->dev_addr,\
                      BMG160_FIFO_CGF0_ADDR_MODE__REG, &v_data_u8r, 1);
        } else {
            comres = SNS_DDF_EINVALID_PARAM;
        }
    }
    return comres;
}

BMG160_RETURN_FUNCTION_TYPE bmg160_get_fifo_data_sel(unsigned char *data_sel)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        comres = p_bmg160->BMG160_BUS_READ_FUNC(p_bmg160->dev_addr,\
                BMG160_FIFO_CGF0_ADDR_DATA_SEL__REG, &v_data_u8r, 1);
        *data_sel = BMG160_GET_BITSLICE(v_data_u8r,\
                BMG160_FIFO_CGF0_ADDR_DATA_SEL);
    }
    return comres;
}

/*****************************************************************************
 * Description: *//**\brief This API is used to set the status of fifo data
 * sel

                   *  \param unsigned char data_sel
                   *         data_sel --> [0:3]
                   *         0 --> X,Y and Z (DEFAULT)
                   *         1 --> X only
                   *         2 --> Y only
                   *         3 --> Z only

                   *  \return communication results
                   *

                   *****************************************************************************/

BMG160_RETURN_FUNCTION_TYPE bmg160_set_fifo_data_sel(unsigned char data_sel)
{
    BMG160_RETURN_FUNCTION_TYPE comres = SNS_DDF_SUCCESS;
    unsigned char v_data_u8r;
    if (p_bmg160 == BMG160_NULL) {
        comres = SNS_DDF_EINVALID_PARAM;
    } else {
        if (data_sel < C_BMG160_Four_U8X) {
            comres = p_bmg160->BMG160_BUS_READ_FUNC\
                     (p_bmg160->dev_addr,\
                      BMG160_FIFO_CGF0_ADDR_DATA_SEL__REG, &v_data_u8r, 1);
            v_data_u8r = BMG160_SET_BITSLICE(v_data_u8r,\
                    BMG160_FIFO_CGF0_ADDR_DATA_SEL, data_sel);
            comres = p_bmg160->BMG160_BUS_WRITE_FUNC\
                     (p_bmg160->dev_addr,\
                      BMG160_FIFO_CGF0_ADDR_DATA_SEL__REG, &v_data_u8r, 1);
        } else {
            comres = SNS_DDF_EINVALID_PARAM;
        }
    }
    return comres;
}

static sns_ddf_status_e bmg160_config_fifo_mode(
        sns_ddf_handle_t port_handle,
        int wml)
{
    sns_ddf_status_e status;
    uint8_t fifo_mode;

    if (wml <= 0) {
        fifo_mode = BMG160_FIFO_MODE_BYPASS;
    } else {
        fifo_mode = BMG160_FIFO_MODE_STREAM;
    }

#if BMG160_CONFIG_DRI_MOCKUP_WMI
    if (1 == wml) {
        fifo_mode = BMG160_FIFO_MODE_BYPASS;
    }
#endif
    status = bmg160_set_fifo_mode(fifo_mode);

    return status;
}

/*===========================================================================
FUNCTION:   sns_dd_gyro_log_fifo_backend
===========================================================================*/
/*!
  @brief log accel sensor data in fifo

  @detail
  - Logs latest set of sensor data sampled from the sensor.

  @param[in] accel_data_ptr: ptr to the driver data
  */
/*=========================================================================*/
void sns_dd_gyro_log_fifo_backend(sns_ddf_sensor_data_s *gyro_data_ptr)
{
#if BMG160_CONFIG_FIFO_LOG
    sns_err_code_e                  err_code;
    sns_log_sensor_data_pkt_s       *log_struct_ptr;
    uint16 idx                      = 0;
    log_pkt_t                       log_pkt_type;


#if BMG160_CONFIG_RUN_ON_OSSC || BMG160_CONFIG_ENABLE_COMPILE_CHECK
    log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA_PUBLIC;
#else
    log_pkt_type = SNS_LOG_CONVERTED_SENSOR_DATA;
#endif
    //Allocate log packet
    err_code = sns_logpkt_malloc(log_pkt_type,
            sizeof(sns_log_sensor_data_pkt_s) + (gyro_data_ptr->num_samples - 1)*sizeof(int32_t),
            (void**)&log_struct_ptr);

    if ((err_code == SNS_SUCCESS) && (log_struct_ptr != NULL))
    {
        log_struct_ptr->version = SNS_LOG_SENSOR_DATA_PKT_VERSION;
        log_struct_ptr->sensor_id = SNS_DDF_SENSOR_GYRO;
        log_struct_ptr->vendor_id = SNS_DDF_VENDOR_BOSCH;

        //Timestamp the log with sample time
        log_struct_ptr->timestamp = gyro_data_ptr->timestamp;
        log_struct_ptr->end_timestamp = gyro_data_ptr->end_timestamp;

        log_struct_ptr->num_data_types = SDD_GYRO_NUM_AXES;
        log_struct_ptr->num_samples = gyro_data_ptr->num_samples / SDD_GYRO_NUM_AXES;

        //Log the sensor fifo data
        log_struct_ptr->data[0]  = gyro_data_ptr->samples[0].sample;
        log_struct_ptr->data[1]  = gyro_data_ptr->samples[1].sample;
        log_struct_ptr->data[2]  = gyro_data_ptr->samples[2].sample;

        for(idx=0; idx<gyro_data_ptr->num_samples; idx++)
        {
            log_struct_ptr->samples[idx]  = gyro_data_ptr->samples[idx].sample;
        }

        //Commit log (also frees up the log packet memory)
        (void) sns_logpkt_commit(log_pkt_type,
                log_struct_ptr);
    }
#endif
}


static sns_ddf_status_e sns_dd_bmg160_config_range(
        sns_dd_bmg160_state_t *state)
{
    uint8_t bosch_write_buffer;
    sns_ddf_status_e stat;

    bosch_write_buffer = state->range ;
    if ((stat =  BMG_CALL_API(set_range_reg)(bosch_write_buffer)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    return SNS_DDF_SUCCESS;
}

static sns_ddf_status_e sns_dd_bmg160_config_bw(
        sns_dd_bmg160_state_t *state)
{
    uint8_t bosch_write_buffer;
    sns_ddf_status_e stat;
    uint32_t i;
    bosch_write_buffer = state->lowpass_bw;

    for (i = 0; i < BST_ARRARY_SIZE(BMG160_MAP_BW_ODR); i++) {
        if (BMG160_MAP_BW_ODR[i].l == bosch_write_buffer) {
            break;
        }
    }

    if (i >= BST_ARRARY_SIZE(BMG160_MAP_BW_ODR)) {
        return SNS_DDF_EINVALID_PARAM;
    }

    if ((stat = BMG_CALL_API(set_bw)(bosch_write_buffer)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    state->odr_hw = BMG160_MAP_BW_ODR[i].r;

    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_bmg160_flush_fifo_data(
        sns_dd_bmg160_state_t *state)
{
    sns_ddf_status_e status;

    uint8_t f_count = 0;

    status = BMG_CALL_API(get_fifo_framecount)(&f_count);
    if (SNS_DDF_SUCCESS != status) {
        return status;
    }

    if (f_count > 0) {
        bmg160_fifo_data_read_out_frames(state, f_count, 0);

        if (state->f_frames_cache.num_samples > 0) {
            status = sns_ddf_smgr_notify_data(state->smgr_handle,
                    &state->f_frames_cache, 1);
            if(state->f_frames_cache.status == SNS_DDF_SUCCESS) {
                sns_dd_gyro_log_fifo(&state->f_frames_cache, DD_DATA_LOG_CONTEXT_FIFO_FLUSH);
            }
        }
    }

    return SNS_DDF_SUCCESS;
}

static sns_ddf_status_e sns_dd_bmg160_config_odr(
        sns_dd_bmg160_state_t *state,
        uint32_t odr_value)
{
    sns_ddf_status_e stat;
    uint8_t bosch_write_buffer;
    uint8_t flag_downsampling = false;
    uint8_t regv;
    uint8_t out;


#if BMG160_CONFIG_BW_SEL_OLD_WAY
    /* For Normal and UI */
    if (odr_value <= 5) { /* NORMAL */
        bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_12HZ;
        state->odr_reported = 5;
        state->odr_hw = 100;
        flag_downsampling = true;
    } else if (odr_value <= 20) { /* UI */
        bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_12HZ;
        state->odr_reported = 20;
        state->odr_hw = 100;
        flag_downsampling = true;
    } else if (odr_value <= 50) {   /* GAME */
        bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_23HZ;
        state->odr_reported = 50;
        state->odr_hw = 200;
        flag_downsampling = true;
    } /* for FASTEST */
    else if (odr_value < 1000) {
        bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_116HZ;
        state->odr_reported = BMG160_CONFIG_FASTEST_ODR;
        state->odr_hw = 1000;
        flag_downsampling = true;
    }
    else {
        bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_230HZ;
        state->odr_reported = 2000;
    }

#else
    if (odr_value < 101)
    {
        bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_12HZ;
        state->odr_reported = 100;
    }
    else if (odr_value < 201)
    {
        bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_23HZ;
        state->odr_reported = 200;
    }
    else if (odr_value < 401)
    {
        bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_47HZ;
        state->odr_reported = 400;
    }
    else if (odr_value < 1001)
    {
        bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_116HZ;
        state->odr_reported = 1000;
    }

    else
    {
        bosch_write_buffer = BOSCH_GYRO_SENSOR_BW_230HZ;
        state->odr_reported = 2000;
    }
#endif
    if (!flag_downsampling) {
        state->odr_hw = state->odr_reported;
    }

#if BMG160_CONFIG_ENABLE_TS_ADJUST_FILTER

        
                
    if ((bosch_write_buffer >= BOSCH_GYRO_SENSOR_BW_523HZ) &&
        (bosch_write_buffer < BOSCH_GYRO_SENSOR_BW_32HZ )){
    
        state->ts_adjust_gyr = BMG160_SSC_US_2_TICKS(BMG160_FILTER_BWP2_GD_US_LIST_GYR[bosch_write_buffer]);
            

    } else {
        state->ts_adjust_gyr = 0;
    }
                
                
            
#endif

    stat = bmg160_sbus_read(state->port_handle,
            BMG160_DSP_BW_ADDR,
            &regv, 1, &out);
    if (SNS_DDF_SUCCESS != stat) {
        return stat;
    }

    if (regv != bosch_write_buffer) {
        sns_dd_bmg160_flush_fifo_data(state);

        if ((stat = BMG_CALL_API(set_bw)(bosch_write_buffer)) != SNS_DDF_SUCCESS)
        {
            return stat;
        }

        state->sample_cnt_to_skip_odrc = BMG160_CONFIG_SKIP_SAMPLE_CNT_ODRC + 1;
    }

    state->itvl_drdy                = (uint32_t)(state->ticks_per_ms * (1000.0 / state->odr_hw));
    state->itvl_drdy_tolerance      = (uint32_t)(state->ticks_per_ms * (1000.0 / state->odr_hw)
            * BMG160_CONFIG_TOLERANCE_DRYD_TIME_ACCURACY);

    //this happens only when downsampling is used
#if BMG160_CONFIG_BW_SEL_OLD_WAY
    //work around in case SMGR doesn't call the set_attr(SNS_DDF_ATTRIB_FIFO)
    uint8_t         desired_wml = state->f_wml_req;
    sns_dd_bmg160_set_attr_fifo(state, SNS_DDF_SENSOR_GYRO, &desired_wml);
#endif

    return SNS_DDF_SUCCESS;
}


/*============================================================================

  Global Data Definitions

  ============================================================================*/


static sns_ddf_sensor_e my_sensors[] =
{
    SNS_DDF_SENSOR_GYRO,
};


static sns_ddf_status_e sns_dd_bmg160_softreset_and_cfg_default(sns_dd_bmg160_state_t *state)
{
    sns_ddf_status_e stat;
    sns_ddf_time_t time_start;
    sns_ddf_time_t time_end;

    BMG_CALL_API(init)(&(state->device));

    state->range = BOSCH_GYRO_SENSOR_RANGE_2000;
    //this BW setting is for SELF_TEST
    state->lowpass_bw = BOSCH_GYRO_SENSOR_BW_23HZ;

    if ((stat = BMG_CALL_API(set_soft_reset)()) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    time_start = sns_ddf_get_timestamp();
    sns_ddf_delay(40000);//40ms
    time_end = sns_ddf_get_timestamp();

    /* estimate the ticks_per_ms */
    state->ticks_per_ms = (uint32_t)(((int32_t)time_end - (int32_t)time_start) / 40.0);

    if ((stat = sns_dd_bmg160_config_range(state)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    if ((stat = sns_dd_bmg160_config_bw(state)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }


    if ((stat = bmg160_set_int_od(BMG160_INT1,
                    INT_PIN_PUSH_PULL)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }

    sns_ddf_delay(5000);//5ms

    /*! Set to suspend */
    if (!state->test_init) {
        if ((stat = BMG_CALL_API(set_mode)(BMG160_MODE_DEEPSUSPEND))
                != SNS_DDF_SUCCESS) {
            return stat;
        }
    }

    sns_ddf_delay(1000);
    sns_dd_bmg160_reg_dump(state);

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION:   sns_dd_bmg160_reset

===========================================================================*/
/*!
  @brief Resets the driver and device so they return to the state they were
  in after init() was called.

  @detail


  @param[in] state: Ptr to the driver structure

  @return
  The error code definition within the DDF
  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Check if open and close port operation is required here?
  */
/*=========================================================================*/
sns_ddf_status_e sns_dd_bmg160_reset(sns_ddf_handle_t dd_handle)
{
    sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;
    sns_ddf_status_e stat;

#ifdef BMG160_DEBUG
    sns_ddf_axes_map_s      *axes_map = &state->axes_map;

    BMG160_MSG_3_P(HIGH,"55555565 %d %d %d", axes_map->indx_x, axes_map->indx_y, axes_map->indx_z);
    BMG160_MSG_3_P(HIGH,"55555565 %d %d %d", axes_map->sign_x, axes_map->sign_y, axes_map->sign_z);
#endif

    BMG160_MSG_3(HIGH, "<bmg160_if_ reset> 0x%x gpio: %d dev_select: %d",
            dd_handle, state->gpio_num, state->dev_select);

    BMG160_MSG_1(HIGH, "ticks_per_ms: %d", state->ticks_per_ms);

    state->odr_reported = 0;
    state->range = BOSCH_GYRO_SENSOR_RANGE_2000;

    if ((stat = BMG_CALL_API(set_mode)(BMG160_MODE_DEEPSUSPEND))
            != SNS_DDF_SUCCESS) {
        return stat;
    }

#ifdef BMG160_DEBUG
    sns_dd_bmg160_reg_dump(state);
#endif

    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e bmg_check_chip_id(sns_dd_bmg160_state_t *state)
{
    uint8_t chip_id = 0;
    sns_ddf_status_e stat;
    uint8_t  out;

    if ((stat = bmg160_sbus_read(state->port_handle,
                    BMG160_CHIP_ID,
                    &chip_id,
                    1,
                    &out)) != SNS_DDF_SUCCESS)
    {
        return stat;
    }


    if ((chip_id & 0xff) != SENSOR_CHIP_ID_BMG) {
        stat = SNS_DDF_EDEVICE;
    }

    return stat;
}

static sns_ddf_status_e bmg160_get_dev_model_info(
        sns_dd_bmg160_state_t *state)
{
    sns_ddf_status_e stat;
    uint8_t reg_val;
    uint8_t out;
    uint8_t bit0;
    uint8_t bit1;
    uint8_t bit2;

    /* check the part info further */
    reg_val = 0x8A;
    if ((stat = bmg160_sbus_write(state->port_handle,
                    0x35,
                    &reg_val,
                    1,
                    &out)) != SNS_DDF_SUCCESS) {
        return stat;
    }
    sns_ddf_delay(2000);
    reg_val = 0x8A;
    if ((stat = bmg160_sbus_write(state->port_handle,
                    0x35,
                    &reg_val,
                    1,
                    &out)) != SNS_DDF_SUCCESS) {
        return stat;
    }
    sns_ddf_delay(2000);

    if ((stat = bmg160_sbus_read(state->port_handle,
                    0x4C,
                    &reg_val,
                    1,
                    &out)) != SNS_DDF_SUCCESS) {
        return stat;
    }
    sns_ddf_delay(500);
    if ((stat = bmg160_sbus_read(state->port_handle,
                    0x4C,
                    &reg_val,
                    1,
                    &out)) != SNS_DDF_SUCCESS) {
        return stat;
    }
    bit0 = reg_val & (1<<7);

    if ((stat = bmg160_sbus_read(state->port_handle,
                    0x56,
                    &reg_val,
                    1,
                    &out)) != SNS_DDF_SUCCESS) {
        return stat;
    }
    sns_ddf_delay(500);

    if ((stat = bmg160_sbus_read(state->port_handle,
                    0x56,
                    &reg_val,
                    1,
                    &out)) != SNS_DDF_SUCCESS) {
        return stat;
    }
    bit1 = reg_val & (1<<7);

    if ((stat = bmg160_sbus_read(state->port_handle,
                    0x60,
                    &reg_val,
                    1,
                    &out)) != SNS_DDF_SUCCESS) {
        return stat;
    }
    sns_ddf_delay(500);

    if ((stat = bmg160_sbus_read(state->port_handle,
                    0x60,
                    &reg_val,
                    1,
                    &out)) != SNS_DDF_SUCCESS) {
        return stat;
    }
    bit2 = reg_val & (1<<7);

    reg_val = 0x0A;
    if ((stat = bmg160_sbus_write(state->port_handle,
                    0x35,
                    &reg_val,
                    1,
                    &out)) != SNS_DDF_SUCCESS) {
        return stat;
    }
    sns_ddf_delay(2000);

    if ((bit0) && (!bit1) && (!bit2)) {
        state->dev_select = BMG160_DEV_SEL_NUM_BMI058;
    }
    /* end of check */

    return stat;
}



/**
 * @brief Parses the nv_params and fills in appropriate values for the axes_map
 *
 */
sns_ddf_status_e sns_dd_bmg160_parse_nv_params(
        sns_ddf_nv_params_s         *entire_nv_params,
        sns_dd_bmg160_state_t       *state)
{
    sns_ddf_axes_map_s      *axes_map = &state->axes_map;

    // If there are no nv_params handed in, then set all to default values
    if (NULL == entire_nv_params) {
        BMG160_MSG_3_P(HIGH, "%d %d %d",
                55555701, entire_nv_params, axes_map);
        sns_ddf_axes_map_init(axes_map, NULL);
    }
    // Else, parse the nv_params and validate each entry
    else {
        /*const sns_dd_bmg160_nv_params_s* nv_params; */
        if (NULL == entire_nv_params->data) {
            BMG160_MSG_3_P(HIGH,"sns_dd_bmg160_parse_nv_params no nv ram data %d %d %d", 0,1,1);
            return SNS_DDF_EFAIL;
        }
        /*nv_params = (sns_dd_bmg160_nv_params_s*)entire_nv_params->data;*/
        // Initialize the axes-map
        sns_ddf_axes_map_init(axes_map, entire_nv_params->data);
        BMG160_MSG_3_P(HIGH,"%d %d %d", axes_map->indx_x, axes_map->indx_y, axes_map->indx_z);
        BMG160_MSG_3_P(HIGH,"%d %d %d", axes_map->sign_x, axes_map->sign_y, axes_map->sign_z);
    }

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION:   sns_dd_bmg160_init

===========================================================================*/
/*!
  @brief Initializes Bosch magnetometer bmg160
  determines the device to use

  @detail
  - Allocates memory for driver state structure.
  Opens the device port by calling sns_ddf_open_port routine
  Calls sns_dd_bmg160_reset routine

  @param[out] dd_handle_ptr  Pointer that this function must malloc and
  populate. This is a handle to the driver
  instance that will be passed in to all
  other functions.
  @param[in]  smgr_handle    Handle used to identify this driver when it
  calls into Sensors Manager functions.
  @param[in]  nv_params      NV parameters retrieved for the driver.
  @param[in]  device_info    Information describing each of the physical
  devices controlled by this driver. This is
  used to configure the bus and talk to the
  device.
  @param[in]  memhandler     Memory handler used to dynamically allocate
  output parameters, if applicable. NB: Do
  not use memhandler to allocate memory for
  @a dd_handle_ptr.
  @param[in]  num_devices    Length of @a device_info.
  @param[out] sensors        List of supported sensors, populated and
  returned by this function.
  @param[out] num_sensors    Length of @a sensors.

  @return
  The error code definition within the DDF
  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
  */
/*=========================================================================*/
sns_ddf_status_e sns_dd_bmg160_init(
        sns_ddf_handle_t*        dd_handle_ptr,
        sns_ddf_handle_t         smgr_handle,
        sns_ddf_nv_params_s*     nv_params,
        sns_ddf_device_access_s  device_info[],
        uint32_t                 num_devices,
        sns_ddf_memhandler_s*    memhandler,
        sns_ddf_sensor_e*        sensors[],
        uint32_t*                num_sensors)
{
    sns_ddf_status_e stat;
    sns_dd_bmg160_state_t *ptr;

    sns_ddf_status_e err = SNS_DDF_SUCCESS;

    /* Update smgr sensor data for the driver to indicate uImage support  */
#if BMG160_CONFIG_ENABLE_UIMAGE
    sns_ddf_smgr_set_uimg_refac(smgr_handle);
#endif

    bmg160_lsb_res_rad = (PI / 180 / 65536);

    if (sns_ddf_malloc_ex((void **)&ptr, sizeof(sns_dd_bmg160_state_t), smgr_handle) != SNS_DDF_SUCCESS)
    {
        return SNS_DDF_ENOMEM;
    }
    ptr->smgr_handle = smgr_handle;
    ptr->gpio_num = device_info->first_gpio;


#if 0
    if ((stat = sns_ddf_signal_register(
                    ptr->gpio_num,
                    ptr,
                    &sns_bmg160_driver_fn_list,
                    SNS_DDF_SIGNAL_IRQ_RISING))
            != SNS_DDF_SUCCESS)
    {
        BMG160_MSG_0(ERROR, "fail to register ISR for bmg160");
        return stat;
    }
#endif

    if ((stat = sns_ddf_open_port(&(ptr->port_handle) , &(device_info->port_config))) != SNS_DDF_SUCCESS)
    {
        return stat;
    }
    bmg160_port_handle = ptr->port_handle;

    *num_sensors = 1;
    *sensors = my_sensors;
    *dd_handle_ptr = (sns_ddf_handle_t)ptr;

    err = bmg_check_chip_id(ptr);
    if (!err) {
    } else {
        err = SNS_DDF_EDEVICE;
        return err;
    }

    sns_dd_bmg160_parse_nv_params(nv_params, ptr);

    /* h/w init */
    ptr->device.bus_read = bmg_read_wrapper;
    ptr->device.bus_write = bmg_write_wrapper;
    ptr->device.delay_msec = bmg_delay;

    stat = bmg160_get_dev_model_info(ptr);
    // Resets the bmg160
    ptr->test_init = 0;
    sns_dd_bmg160_softreset_and_cfg_default(ptr);

    stat = sns_ddf_malloc_ex((void**)&(((sns_dd_bmg160_state_t *)ptr)->f_frames_cache.samples),
            sizeof(sns_ddf_sensor_sample_s)
            * SDD_GYRO_NUM_AXES* BMG160_MAX_FIFO_LEVEL, smgr_handle);
    if (stat != SNS_DDF_SUCCESS)
    {
        return stat;
    }
    return stat;
}
/*===========================================================================

FUNCTION:   sns_dd_bmg160_config_mode

===========================================================================*/
/*!
  @brief Sets the BOSCH magnetometer  to the passed in operating mode

  @detail
  - Set the operational mode (sleep or suspend) by programming
  BMC050_POWER_CNTL register

  @param[in] state: Ptr to the driver structure
  @param[in] mode:

  @return
  The error code definition within the DDF
  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO: Get away with read modify write
  */
/*=========================================================================*/
static sns_ddf_status_e sns_dd_bmg160_config_mode(
        sns_dd_bmg160_state_t *state,
        sns_ddf_powerstate_e  mode)
{
    sns_ddf_status_e status;
    uint8_t bosch_write_buffer;

    switch (mode)
    {
        case SNS_DDF_POWERSTATE_ACTIVE:
            bosch_write_buffer = BMG160_MODE_NORMAL;
            break;
        case SNS_DDF_POWERSTATE_LOWPOWER:
            //deep suspend
            bosch_write_buffer = BMG160_MODE_DEEPSUSPEND;
            break;
        default:
            return SNS_DDF_EINVALID_PARAM;

    }

    /* Set opmode */
    if ((status = BMG_CALL_API(set_mode)(bosch_write_buffer)) != SNS_DDF_SUCCESS)
    {
        return status;
    }


    return SNS_DDF_SUCCESS;
}


static sns_ddf_status_e sns_dd_bmg160_set_attr_fifo(
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        void*                value)
{
    sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;
    sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;
    uint8_t         desired_wml = *((uint8_t *)value);
    uint8_t         hw_wml;
    uint8_t         downsampling_factor = 1;
    uint8_t 		f_wml_old;

    BMG160_MSG_3_F(HIGH, "SNS_DDF_ATTRIB_FIFO %d %d %d",
            state->odr_reported, state->odr_hw, desired_wml);

    if ( ((0 == state->odr_reported) && (0 != desired_wml))
            || (desired_wml >= BMG160_MAX_FIFO_LEVEL)) {
        return SNS_DDF_EINVALID_PARAM;
    }

    downsampling_factor = (state->odr_hw / state->odr_reported);
    /* we are using streaming FIFO MODE, thus max FIFO depth is 99 */

    if (desired_wml > 0) {
        if (downsampling_factor > 1) {
            ret_val = bmg160_calc_matching_wml(desired_wml, downsampling_factor, &hw_wml);
            if (SNS_DDF_SUCCESS != ret_val) {
                return ret_val;
            }
        } else {
            hw_wml = desired_wml;
        }
    } else {
        hw_wml = 0;
    }

    /* This assumes that ATTRIB_ODR is configured before ATTRIB_FIFO */
    if (hw_wml > 0) {
        ret_val = bmg160_set_fifo_watermarklevel((uint8_t)hw_wml);

        if (SNS_DDF_SUCCESS != ret_val) {
            BMG160_MSG_3_F(HIGH, "SNS_DDF_ATTRIB_FIFO %d %d %d",
                    desired_wml, hw_wml, ret_val);
            return ret_val;
        }

    }
    f_wml_old = state->f_wml;

    state->f_wml = (uint8_t)hw_wml;
    state->f_wml_req = desired_wml;

    
#if BMG160_CONFIG_DRI_MOCKUP_WMI
    if((f_wml_old > 1)&&(state->f_wml > 1)) {
#else
    if((f_wml_old > 0)&&(state->f_wml > 0)) {	
#endif
    
        BMG160_MSG_3_F(HIGH, "does not change fifo mode %d %d %d",
            f_wml_old, state->f_wml, state->f_wml_req);	

            state->ts_start_first_sample = state->f_frames_cache.end_timestamp;
            
    } else {
            ret_val = bmg160_config_fifo_mode(state->port_handle, state->f_wml);
            if (SNS_DDF_SUCCESS != ret_val) {
                BMG160_MSG_3_F(HIGH, "SNS_DDF_ATTRIB_FIFO %d %d %d",
                        desired_wml, hw_wml, ret_val);
                return ret_val;
            }

            if (state->f_wml > 0) {
                state->ts_start_first_sample = sns_ddf_get_timestamp();
            }

    }

    

    return SNS_DDF_SUCCESS;
}

/*===========================================================================

FUNCTION:   sns_dd_bmg160_set_attr

===========================================================================*/
/*!
  @brief Sets an attribute of the bosch magnetometer

  @detail
  - Called by SMGR to set certain device attributes that are
  programmable. Curently its the power mode.

  @param[in] dd_handle   Handle to a driver instance.
  @param[in] sensor Sensor for which this attribute is to be set.
  @param[in] attrib      Attribute to be set.
  @param[in] value      Value to set this attribute.

  @return
  The error code definition within the DDF
  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS or
  SNS_DDF_EINVALID_PARAM


*/
/*=========================================================================*/
sns_ddf_status_e sns_dd_bmg160_set_attr(
        sns_ddf_handle_t     dd_handle,
        sns_ddf_sensor_e     sensor,
        sns_ddf_attribute_e  attrib,
        void*                value)
{
    sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;
    sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;

    BMG160_MSG_4(HIGH, "<bmg160_if_ set_attrib> 0x%x sensor: %d attr: %d val: %d",
            dd_handle, sensor, attrib, *(uint8_t *)value);

    switch (attrib)
    {
        case SNS_DDF_ATTRIB_POWER_STATE:
            /* set power mode */
            ret_val = sns_dd_bmg160_config_mode((sns_dd_bmg160_state_t*)dd_handle,
                    *((sns_ddf_powerstate_e *)value));
            break;
        case SNS_DDF_ATTRIB_RANGE:
            if (*((uint8_t *)value) >= SNS_DD_GYRO_MAX_RANGE)
            {
                return SNS_DDF_EINVALID_ATTR;
            }
            else
            {
                ((sns_dd_bmg160_state_t*)dd_handle)->range = sns_dd_gyro_range[*((uint8_t *)value)];
            }

            ret_val = sns_dd_bmg160_config_range((sns_dd_bmg160_state_t*)dd_handle);

            break;

        case SNS_DDF_ATTRIB_LOWPASS:


            if (*((uint8_t *)value) > BOSCH_GYRO_SENSOR_BW_32HZ)
            {
                return SNS_DDF_EINVALID_ATTR;
            }
            else
            {
                ((sns_dd_bmg160_state_t*)dd_handle)->lowpass_bw = sns_dd_gyro_bw[*((uint8_t *)value)];
            }

            ret_val = sns_dd_bmg160_config_bw((sns_dd_bmg160_state_t*)dd_handle);

            break;

        case SNS_DDF_ATTRIB_ODR:
            {
                sns_ddf_odr_t odr_value = *(sns_ddf_odr_t *)value;

                if (0 == odr_value) {
                    state->odr_reported = 0;
                } else {
                    if ((ret_val = sns_dd_bmg160_config_odr(
                                    state,
                                    odr_value))
                            != SNS_DDF_SUCCESS)
                    {
                        return ret_val;
                    }

                }
                return SNS_DDF_SUCCESS;
            }

        case SNS_DDF_ATTRIB_RESOLUTION_ADC:
            break;

        case SNS_DDF_ATTRIB_FIFO:
            if (sensor == SNS_DDF_SENSOR_GYRO) {
                return sns_dd_bmg160_set_attr_fifo(dd_handle, sensor, value);
            } else {
                return SNS_DDF_EINVALID_PARAM;
            }

            break;

        default:

            return SNS_DDF_EINVALID_PARAM;
    }

    return (ret_val);
}


/*===========================================================================

FUNCTION:   sns_dd_bmg160_get_attr

===========================================================================*/
/*!
  @brief Called by the SMGR to retrieves the value of an attribute of
  the sensor.

  @detail
  - resolution info is from the device data sheet.

  @param[in]  dd_handle   Handle to a driver instance.
  @param[in] sensor       Sensor whose attribute is to be retrieved.
  @param[in]  attrib      Attribute to be retrieved.
  @param[in] memhandler  Memory handler used to dynamically allocate
  output parameters, if applicable.
  @param[out] value      Pointer that this function will allocate or set
  to the attribute's value.
  @param[out] num_elems  Number of elements in @a value.

  @return
  The error code definition within the DDF
  SNS_DDF_SUCCESS on success; Otherwise SNS_DDF_EBUS

  $TODO:
  */
/*=========================================================================*/

sns_ddf_status_e sns_dd_bmg160_get_attr(
        sns_ddf_handle_t       dd_handle,
        sns_ddf_sensor_e       sensor,
        sns_ddf_attribute_e    attrib,
        sns_ddf_memhandler_s*  memhandler,
        void**                 value,
        uint32_t*              num_elems)
{
    sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;

    BMG160_MSG_3(HIGH, "<bmg160_if_ get_attrib> 0x%x sensor: %d attr: %d",
            dd_handle, sensor, attrib);

    switch (attrib)
    {
        case SNS_DDF_ATTRIB_POWER_INFO:
            {
                sns_ddf_power_info_s* power_attrib;
                if ((*value = sns_ddf_memhandler_malloc_ex(memhandler,
                                sizeof(sns_ddf_power_info_s),state->smgr_handle)) == NULL)
                {
                    return SNS_DDF_ENOMEM;
                }
                power_attrib = *value;
                power_attrib->active_current = 5000;//5mA
                power_attrib->lowpower_current = 5;//5uA max
                *num_elems = 1;
            }
            break;

        case SNS_DDF_ATTRIB_RANGE:
            {
                sns_ddf_range_s *device_ranges;


                if ((*value = sns_ddf_memhandler_malloc_ex(memhandler,
                                SNS_DD_GYRO_MAX_RANGE*sizeof(sns_ddf_range_s),state->smgr_handle))
                        == NULL)
                {
                    return SNS_DDF_ENOMEM;
                }
                device_ranges = *value;
                *num_elems = 5;
                device_ranges[0].min = BOSCH_GYRO_SENSOR_RANGE_125_MIN;
                device_ranges[0].max = BOSCH_GYRO_SENSOR_RANGE_125_MAX;
                device_ranges[1].min = BOSCH_GYRO_SENSOR_RANGE_250_MIN;
                device_ranges[1].max = BOSCH_GYRO_SENSOR_RANGE_250_MAX;
                device_ranges[2].min = BOSCH_GYRO_SENSOR_RANGE_500_MIN;
                device_ranges[2].max = BOSCH_GYRO_SENSOR_RANGE_500_MAX;
                device_ranges[3].min = BOSCH_GYRO_SENSOR_RANGE_1000_MIN;
                device_ranges[3].max = BOSCH_GYRO_SENSOR_RANGE_1000_MAX;
                device_ranges[4].min = BOSCH_GYRO_SENSOR_RANGE_2000_MIN;
                device_ranges[4].max = BOSCH_GYRO_SENSOR_RANGE_2000_MAX;
            }
            break;


        case SNS_DDF_ATTRIB_LOWPASS:
            {
                uint8_t i;
                sns_ddf_lowpass_freq_t *freq_set;
                *num_elems = 8;
                if ((*value = sns_ddf_memhandler_malloc_ex(memhandler,
                                (*num_elems)*sizeof(sns_ddf_lowpass_freq_t),state->smgr_handle)) == NULL)
                {
                    return SNS_DDF_ENOMEM;
                }
                freq_set = *value;


                *num_elems = 8;
                for(i=0; i<*num_elems; i++)
                {
                    *(freq_set+i) = sns_dd_gyro_freq[i];
                }
            }
            break;

        case SNS_DDF_ATTRIB_RESOLUTION_ADC:
            {
                sns_ddf_resolution_adc_s *device_res;
                if ((*value = sns_ddf_memhandler_malloc_ex(memhandler,
                                sizeof(sns_ddf_resolution_adc_s),state->smgr_handle)) == NULL)
                {
                    return SNS_DDF_ENOMEM;
                }

                device_res = *value;

                *num_elems = 1;

                device_res->bit_len = 16;
                device_res->max_freq = BMG160_CONFIG_MAX_FREQ;
            }
            break;

        case SNS_DDF_ATTRIB_RESOLUTION:
            {
                sns_ddf_resolution_t *res;

                res = sns_ddf_memhandler_malloc_ex(memhandler,
                        sizeof(sns_ddf_resolution_t),state->smgr_handle);
                if (NULL == res)
                    return SNS_DDF_ENOMEM;

                *res = FX_FLTTOFIX_Q16((4000>>state->range) * PI / 180 / 65536);
                *value = res;
                *num_elems = 1;

                return SNS_DDF_SUCCESS;
            }


        case SNS_DDF_ATTRIB_DELAYS:
            {
                sns_ddf_delays_s *device_delay;
                if ((*value = sns_ddf_memhandler_malloc_ex(memhandler,
                                sizeof(sns_ddf_delays_s),state->smgr_handle)) == NULL)
                {
                    return SNS_DDF_ENOMEM;
                }
                device_delay = *value;
                *num_elems = 1;

                device_delay->time_to_active = 30; // 30ms is the startup time
                device_delay->time_to_data = 30;   //30 ms
            }
            break;


        case SNS_DDF_ATTRIB_DEVICE_INFO:
            {
                sns_ddf_device_info_s *device_info;

                if ((*value = sns_ddf_memhandler_malloc_ex(memhandler,
                                sizeof(sns_ddf_device_info_s),state->smgr_handle)) == NULL)
                {
                    return SNS_DDF_ENOMEM;
                }
                device_info = *value;
                *num_elems = 1;
                if (BMG160_DEV_SEL_NUM_BMI058 == state->dev_select) {
                    device_info->model = "BMI058";
                } else if (BMG160_DEV_SEL_NUM_BMI055 == state->dev_select) {
                    device_info->model = "BMI055";
                } else {
                    device_info->model = "BMG160";
                }
                device_info->vendor = "BOSCH";
                device_info->name = "Gyroscope";
                device_info->version = BMG160_SNS_DDF_ATTRIB_VERSION;
            }
            break;

        case SNS_DDF_ATTRIB_BIAS:
            {
                q16_t *bias;
                uint32_t i;
                bias = sns_ddf_memhandler_malloc_ex(memhandler,
                                sizeof(q16_t) * 3,
                                state->smgr_handle);
                if (NULL == bias) {
                    return SNS_DDF_ENOMEM;
                }


                if (SNS_DDF_SENSOR_GYRO != sensor) {
                    return SNS_DDF_EINVALID_PARAM;
                }

                for (i = 0; i < 3; i++) {
                    bias[i] = state->bias[i];
                }

                *value = bias;
                *num_elems = 3;

                return SNS_DDF_SUCCESS;
            }

        case SNS_DDF_ATTRIB_ODR:
            {
                uint32_t *odr_ptr;

                odr_ptr = &state->odr_reported;
                *value = odr_ptr;
                *num_elems = 1;

                return SNS_DDF_SUCCESS;
            }

        case SNS_DDF_ATTRIB_FIFO:
            {
                if (sensor == SNS_DDF_SENSOR_GYRO)
                {
                    // for GYRO
                    sns_ddf_fifo_attrib_get_s *fifo_attrib_info = sns_ddf_memhandler_malloc_ex(
                            memhandler ,sizeof(sns_ddf_fifo_attrib_get_s),state->smgr_handle);
                    if (NULL == fifo_attrib_info)
                        return SNS_DDF_ENOMEM;

                    fifo_attrib_info->is_supported = true;
                    fifo_attrib_info->is_sw_watermark = false;
                    fifo_attrib_info->max_watermark = 100;
                    fifo_attrib_info->share_sensor_cnt = 0;
                    fifo_attrib_info->share_sensors[0] =  NULL;
                    *value = fifo_attrib_info;
                    *num_elems = 1;

                    return SNS_DDF_SUCCESS;
                }
                else
                {
                    return SNS_DDF_EINVALID_ATTR;
                }

                break;
            }

        default:
            return SNS_DDF_EINVALID_PARAM;
    }

    return SNS_DDF_SUCCESS;
}


static void sns_dd_gyro_bmg160_test_notify(
        sns_ddf_handle_t handle,
        sns_ddf_sensor_e sensor,
        sns_ddf_status_e status,
        uint32_t err,
        q16_t* biases)
{
    static const uint8_t num_axes = 3;
    sns_dd_bmg160_state_t *state = handle;
    q16_t* biases_ptr;
    uint8_t i;

    sns_ddf_smgr_notify_test_complete(state->smgr_handle,
            sensor,
            status,err);
    if (status == SNS_DDF_SUCCESS)
    {
        biases_ptr = state->bias;

        for (i = 0; i < num_axes; i++)
            biases_ptr[i] = biases[i];

        sns_ddf_smgr_notify_event(state->smgr_handle,
                sensor,
                SNS_DDF_EVENT_BIAS_READY);
    }

}

static sns_ddf_status_e sns_dd_gyro_bmg160_run_test_self(
        sns_dd_bmg160_state_t   *state,
        sns_ddf_sensor_e        sensor_type,
        uint32_t                *err)
{
    sns_ddf_status_e        status = SNS_DDF_SUCCESS;
    uint8_t                 si_buf = 0;
    uint8_t                 bytes_read;

    if ((status = bmg160_sbus_read(state->port_handle,
                    BMG160_CHIP_ID,
                    &si_buf,
                    1,
                    &bytes_read)) != SNS_DDF_SUCCESS) {
        return status;
    }

    if (!(SENSOR_CHIP_ID_BMG == si_buf)) {
        *err = si_buf;
        return SNS_DDF_EDEVICE;
    }

    *err = 0;
    return SNS_DDF_SUCCESS;
}

sns_ddf_status_e sns_dd_gyro_bmg160_self_test(
        sns_ddf_handle_t dd_handle,
        sns_ddf_sensor_e sensor,
        sns_ddf_test_e test,
        uint32_t* err)
{
    sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;
    sns_ddf_status_e stat;
    uint8_t out;

    unsigned char read_value = 0;
    unsigned char timeout = 0;
    uint8_t bosch_write_buffer = 0;
    sns_ddf_status_e status;
    sns_dd_inertial_test_config_s test_config;

    *err = 0;

    BMG160_MSG_3(HIGH, "<bmg160_if_ self_test> 0x%x sensor: %d test: %d",
            dd_handle, sensor, test);

    if (test == SNS_DDF_TEST_OEM)
    {
        test_config.sample_rate =  100;
        test_config.num_samples =  64;
        test_config.max_variance = (int64_t)(FX_CONV((FX_FLTTOFIX_Q16(0.1)),16,32));
        test_config.bias_thresholds[0] = FX_FLTTOFIX_Q16(0.087);
        test_config.bias_thresholds[1] = FX_FLTTOFIX_Q16(0.087);
        test_config.bias_thresholds[2] = FX_FLTTOFIX_Q16(0.087);

        state->test_init = 1;
        sns_dd_bmg160_softreset_and_cfg_default(dd_handle);

        status = sns_dd_inertial_test_run(
                sensor,
                dd_handle,
                &sns_bmg160_driver_fn_list,
                &test_config,
                &sns_dd_gyro_bmg160_test_notify);

        if (status != SNS_DDF_SUCCESS)
            return status;

        return SNS_DDF_PENDING;

    }
    else if (test == SNS_DDF_TEST_SELF_SW)
    {
        if ((stat = BMG_CALL_API(set_soft_reset)()) != SNS_DDF_SUCCESS)
        {
            return stat;
        }
        sns_ddf_delay(40000);//40ms

        stat = SNS_DDF_EINVALID_TEST;
        if ((stat = bmg160_sbus_read(state->port_handle,
                        BMG160_FPGA_REV_ADDR,
                        &read_value,
                        1,
                        &out)) != SNS_DDF_SUCCESS)
        {
            *err = 1; return stat;
        }

        if ((read_value&0x10)==0x10){

            bosch_write_buffer = 0x01;
            if ((stat = bmg160_sbus_write(state->port_handle,
                            BMG160_FPGA_REV_ADDR,
                            &bosch_write_buffer,
                            1,
                            &out)) != SNS_DDF_SUCCESS)
            {
                *err = 2; return stat;
            }

            do {
                sns_ddf_delay(2000);
                if ((stat = bmg160_sbus_read(state->port_handle,
                                BMG160_FPGA_REV_ADDR,
                                &read_value,
                                1,
                                &out)) != SNS_DDF_SUCCESS)
                {
                    *err = 3;
                    return stat;
                }

                timeout++;
                if (timeout == 100) {
                    stat = SNS_DDF_EFAIL;
                    return stat;
                };

            } while ((read_value&0x02)!=0x02);

            if ((stat = bmg160_sbus_read(state->port_handle,
                            BMG160_FPGA_REV_ADDR,
                            &read_value,
                            1,
                            &out)) != SNS_DDF_SUCCESS)
            {
                *err = 4;
                return stat;
            }

            if ((read_value&0x04)!=0x04) {

                stat = SNS_DDF_SUCCESS;

            } else {
                stat = SNS_DDF_EFAIL;
            }

        } else {
            stat = SNS_DDF_EFAIL;
        }

        state->test_init = 0;
        sns_dd_bmg160_softreset_and_cfg_default(dd_handle);

        return stat;
    }
    else if (test == SNS_DDF_TEST_SELF) {
        stat = sns_dd_gyro_bmg160_run_test_self(state, sensor, err);
        return stat;
    }

    else
    {
        return SNS_DDF_EINVALID_TEST;
    }
}


sns_ddf_status_e sns_dd_bmg160_probe(
        sns_ddf_device_access_s*  device_info,
        sns_ddf_memhandler_s*     memhandler,
        uint32_t*                 num_sensors,
        sns_ddf_sensor_e**        sensors)
{
    sns_ddf_status_e status;
    sns_ddf_handle_t port_handle;

    uint8_t si_buf;
    uint8_t bytes_read;
    *num_sensors = 0;
    *sensors = NULL;

    status = sns_ddf_open_port(&port_handle, &(device_info->port_config));
    if (status != SNS_DDF_SUCCESS)
    {
        return status;
    }

    if ((status = bmg160_sbus_read(port_handle,
                    BMG160_CHIP_ID,
                    &si_buf,
                    1,
                    &bytes_read)) != SNS_DDF_SUCCESS)
    {
        sns_ddf_close_port(port_handle);
        return status;
    }

    sns_ddf_close_port(port_handle);

    if (SENSOR_CHIP_ID_BMG == si_buf)
    {
        *num_sensors = 1;
        *sensors = sns_ddf_memhandler_malloc(memhandler,
                sizeof(sns_ddf_sensor_e) * (*num_sensors));
        if (*sensors != NULL)
        {
            (*sensors)[0] = SNS_DDF_SENSOR_GYRO;
            status = SNS_DDF_SUCCESS;
        }
        else
        {
            status = SNS_DDF_ENOMEM;
        }
        return status;
    }
    else
    {
        /* Unknown device ID. Return now with nothing detected */
        return SNS_DDF_SUCCESS;
    }
}


sns_ddf_status_e sns_dd_bmg160_enable_sched_data(
        sns_ddf_handle_t dd_handle,
        sns_ddf_sensor_e sensor,
        bool             enable)
{
    sns_ddf_status_e ret_val = SNS_DDF_SUCCESS;
    sns_dd_bmg160_state_t *state = (sns_dd_bmg160_state_t *)dd_handle;
    uint32_t signal_registered = 0;

    uint8_t retry = 0;
    uint8_t readbyte = 0;
    uint8_t out = 0;
    uint8_t dri_mockup_wmi;

    uint32_t irq_trigger_type = BMG160_IRQ_TRIGGER_TYPE_DEFAULT;

    BMG160_MSG_3(HIGH, "<bmg160_if_ enable_sched_data> sensor: %d enable: %d wml: %d",
            sensor, enable, state->f_wml);

    BMG160_MSG_3(MEDIUM, "[enable_sched_data] %d %d 0x%x",
            state->odr_reported, state->odr_hw,
            state->en_dri | (state->en_fifo_int << 8));

    if (!sns_ddf_signal_irq_enabled())
    {
        return SNS_DDF_EINVALID_PARAM;
    }

    signal_registered = state->en_dri || state->en_fifo_int;

    dri_mockup_wmi = 0;
#if BMG160_CONFIG_DRI_MOCKUP_WMI
    if (enable && (1 == state->f_wml)) {
        dri_mockup_wmi = 1;
    }
#endif

    if (enable) {
        if (!signal_registered) {
#if BMG160_CONFIG_WMI_TRIGGER_LEVEL
            if ((state->f_wml > 0) && !dri_mockup_wmi) {
                irq_trigger_type = SNS_DDF_SIGNAL_IRQ_HIGH;
            }
#endif
            /*! Register to receive notification when the interrupt fires */
            if ((ret_val = sns_ddf_signal_register(
                            state->gpio_num,
                            dd_handle,
                            &sns_bmg160_driver_fn_list,
                            irq_trigger_type)) != SNS_DDF_SUCCESS)
            {
                return ret_val;
            }
            BMG160_MSG_3_F(MEDIUM, "[enable_sched_data] register handler %d %d %d",
                    5555590, state->gpio_num, 2);
        }
    } else {
        if (signal_registered) {
            if ((ret_val = sns_ddf_signal_deregister(state->gpio_num))
                    != SNS_DDF_SUCCESS)
            {
                return ret_val;
            }
            BMG160_MSG_3_F(MEDIUM, "[enable_sched_data] deregister handler %d %d %d",
                    5555590, state->gpio_num, 3);
        }
    }

    if (enable)
    {
        while (retry++ < 5) {
            bmg160_sbus_read(state->port_handle,
                    BMG160_INT_ENABLE1,
                    &readbyte,
                    1,
                    &out);
            if (BMG160_INT_EN_1_EXPECTED_VAL_INT1_PUSHPULL_ACTIVE_HIGH == readbyte) {
                break;
            } else {
                if ((ret_val = bmg160_set_int_od(BMG160_INT1, INT_PIN_PUSH_PULL))
                        != SNS_DDF_SUCCESS)
                {
                    return ret_val;
                }
                sns_ddf_delay(1000 * 1);
            }
        }
        BMG160_MSG_3_F(MEDIUM, "[enable_sched_data] %d %d %d",
                5555592, retry, readbyte);

        if (state->f_wml > 0) {
            if (!dri_mockup_wmi) {
                if ((ret_val = bmg160_enable_int_fifo(1))
                        != SNS_DDF_SUCCESS)
                {
                    return ret_val;
                }

                if ((ret_val = bmg160_enable_int_fifo_wm(state->port_handle, 1))
                        != SNS_DDF_SUCCESS)
                {
                    return ret_val;
                }

                /* maps interrupt to INT1 pin */
                if ((ret_val = bmg160_map_int1_fifo(1)) != SNS_DDF_SUCCESS)
                {
                    return ret_val;
                }

            } else {
                //wml == 1, implicit dri
                if ((ret_val = bmg160_enable_int_drdy(INT_ENABLE)) != SNS_DDF_SUCCESS) {
                    return ret_val;
                }
                state->ts_start_first_sample = sns_ddf_get_timestamp();
                state->num_irq_drdy = 0;
                bmg160_map_int1_drdy(1);

                bmg160_map_int1_fifo(0);
            }

            state->en_fifo_int = true;
        } else {
            //wml == 0, explicit dri
            if ((ret_val = bmg160_enable_int_drdy(INT_ENABLE)) != SNS_DDF_SUCCESS) {
                return ret_val;
            }
            state->ts_start_first_sample = sns_ddf_get_timestamp();
            state->num_irq_drdy = 0;
            bmg160_map_int1_drdy(INT_ENABLE);

            state->en_dri = true;
        }

    }
    else
    {
        state->en_fifo_int = false;
        state->en_dri = false;

        bmg160_enable_int_fifo_wm(state->port_handle, 0);
        bmg160_enable_int_fifo(0);
        bmg160_enable_int_drdy(0);
        bmg160_map_int1_fifo(0);
        bmg160_map_int1_drdy(0);
    }

    if (enable) {
        if ((state->en_fifo_int) && !dri_mockup_wmi) {
            ret_val = bmg160_sbus_read(state->port_handle,
                    BMG160_INT_MAP_1,
                    &state->regv_int_map_1, 1, &out);

            if (SNS_DDF_SUCCESS != ret_val) {
                state->regv_int_map_1 = 0;
                state->regv_int_map_1 |= (1 << 2);
            }
        }
    }

    BMG160_MSG_3(HIGH, "sns_dd_bmg160_enable_sched_data ts_start_first_sample: %d %d %d",
            state->ts_start_first_sample, state->en_fifo_int, state->en_dri);

    sns_dd_bmg160_reg_dump(state);

    return ret_val;
}


