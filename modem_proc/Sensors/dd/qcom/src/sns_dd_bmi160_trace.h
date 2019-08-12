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


#ifndef __LK_DEBUG_H
#define __LK_DEBUG_H
#include <stdint.h>



#define CONFIG_ENABLE_TP 0

#if CONFIG_ENABLE_TP

#include "fixed_point.h"

#include "qurt_elite_diag.h"
#include "sns_ddf_signal.h"
#include "sns_ddf_attrib.h"
#include "sns_ddf_comm.h"
#include "sns_ddf_common.h"
#include "sns_ddf_driver_if.h"
#include "sns_ddf_memhandler.h"
#include "sns_ddf_signal.h"
#include "sns_ddf_smgr_if.h"
#include "sns_ddf_util.h"
#include "stdbool.h"
#include <string.h>
#include "sns_log_api_public.h"
#include "sns_log_types_public.h"

#define INSERT_TRACE_POINT(mod_tag, tnum) MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "trace: %d\n", tnum)
#define INSERT_TRACE_POINT2(mod_tag, tnum1, tnum2) MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "trace: %d %d\n", tnum1, tnum2)
#define INSERT_TRACE_POINT3(mod_tag, tnum1, tnum2, tnum3) MSG_3(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "trace: %d %d %d\n", tnum1, tnum2, tnum3);
#else
#define INSERT_TRACE_POINT(mod_tag, tnum)
#define INSERT_TRACE_POINT2(mod_tag, tnum1, tnum2)
#define INSERT_TRACE_POINT3(mod_tag, tnum1, tnum2, tnum3)
#endif

#endif


#define INSERT_TRACE_POINT_T(mod_tag, tnum)
#define INSERT_TRACE_POINT2_T(mod_tag, tnum1, tnum2)
#define INSERT_TRACE_POINT3_T(mod_tag, tnum1, tnum2, tnum3)
#define INSERT_TRACE_POINT4_T(mod_tag, tnum1, tnum2, tnum3, tnum4)

#define INSERT_TRACE_POINT_P(mod_tag, ptr)
