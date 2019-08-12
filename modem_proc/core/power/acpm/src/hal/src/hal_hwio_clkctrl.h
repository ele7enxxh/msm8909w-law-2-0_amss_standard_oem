/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/
/*
 * hal_hwio_clkctrl.h - header file for direct HWIO control module
 *
 *  Created on: Dec 16, 2013
 *      Author: yrusakov
 */

#ifndef HAL_HWIO_CLKCTRL_H_
#define HAL_HWIO_CLKCTRL_H_
#include "adsppm.h"

Adsppm_Status HalHwIo_Init(void);
Adsppm_Status HalHwIo_EnableClock(AdsppmClkIdType clk);
Adsppm_Status HalHwIo_DisableClock(AdsppmClkIdType clk);
Adsppm_Status HalHwIo_EnableHWClockCntl(AdsppmClkIdType clk);
Adsppm_Status HalHwIo_DisableHWClockCntl(AdsppmClkIdType clk);

#endif /* HAL_HWIO_CLKCTRL_H_ */
