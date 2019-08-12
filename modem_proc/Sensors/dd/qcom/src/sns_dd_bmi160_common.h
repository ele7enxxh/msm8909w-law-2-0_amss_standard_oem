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
#ifndef __SNS_DD_BMI160_COMMON_H
#define __SNS_DD_BMI160_COMMON_H

#include "fixed_point.h"

#include "sns_ddf_signal.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"

#include "sns_dd_inertial_test.h"

#include "sns_dd_bmi160_ver.h"


#include <memory.h>
#include <stdbool.h>
#include <string.h>


#ifndef unlikely
#define unlikely(x) (x)
#endif

#ifndef likely
#define likely(x) (x)
#endif

#ifndef BMI160_DD_CHECK_RETVAL
#define BMI160_DD_CHECK_RETVAL(val, val_success)\
    if (likely(val_success == val)) {       \
    } else {                                \
        val = val;                      \
        return val;                     \
    }
#endif


struct bms_platform_data {
    /* this sbus_obj will be used by bus_read() and bus_write() as first parameter */
    void    *sbus_obj;

    int32_t (*bus_read)(
            void            *sbus_obj,
            uint8_t         reg_addr,
            uint8_t         *buf,
            uint32_t        len);

    int32_t (*bus_write)(
            void            *sbus_obj,
            uint8_t         reg_addr,
            uint8_t         *buf,
            uint32_t        len);

    void    (*udelay)(uint32_t us);
};


#endif
