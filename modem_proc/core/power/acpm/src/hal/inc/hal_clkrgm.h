/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * hal_clkrgm.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef HAL_CLKRGM_H_
#define HAL_CLKRGM_H_

#include "adsppm.h"
#include "asic.h"

typedef enum{
    Hal_ClkRgmDisable = 0,
    Hal_ClkRgmEnable
} Hal_ClkRgmEnableDisableType;

typedef enum{
    Hal_ClkRgmMemCollapse = 0,
    Hal_ClkRgmMemRetain
} Hal_ClkRgmMemStateType;

typedef union{
    Hal_ClkRgmEnableDisableType pwrState;
    Hal_ClkRgmMemStateType memState;
} Hal_ClkRgmPwrState;


Adsppm_Status ClkRgm_Init(void);
Adsppm_Status ClkRgm_SetClock(AdsppmClkIdType clk, uint32 freq);
Adsppm_Status ClkRgm_EnableClock(AdsppmClkIdType clk, Hal_ClkRgmEnableDisableType state);
Adsppm_Status ClkRgm_RequestPower(AsicPowerDomainType blk, Hal_ClkRgmEnableDisableType state);
Adsppm_Status ClkRgm_MemoryControl(AdsppmMemIdType memCore, Hal_ClkRgmMemStateType pwrState);
Adsppm_Status ClkRgm_GetClockFrequency(AdsppmClkIdType clk, uint32 *pFreq);
Adsppm_Status ClkRgm_CalcClockFrequency(AdsppmClkIdType clk, uint32 *pFreq);
/**
 * @fn ClkRgm_EnableHWClockCntl - enables/disables HW clock control for the specified clock
 * Function will return success if the clock doesn't support DCG
 * @param clk - clock ID
 * @param state - specifies whether or not HW control should be enabled
 * @return completion status
 */
Adsppm_Status ClkRgm_EnableHWClockCntl(AdsppmClkIdType clk, Hal_ClkRgmEnableDisableType state);

Adsppm_Status clkrgm_SharedClkControl(Hal_ClkRgmEnableDisableType stateRequested);

#endif /* CLKRGM_H_ */


