/**
 * @file:  PmicArb.h
 * @brief: 
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/08/27 05:22:47 $
 * $Header: //components/rel/rpm.bf/2.1.1/core/buses/spmi/src/core/hal/PmicArb.h#2 $
 * $Change: 8908394 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */
#ifndef PMICARB_H
#define	PMICARB_H

#include "SpmiTypes.h"
#include "SpmiUtils.h"
#include "SpmiHal.h"

typedef enum 
{
    PMIC_ARB_CMD_EXTENDED_REG_WRITE_LONG = 0,
    PMIC_ARB_CMD_EXTENDED_REG_READ_LONG = 1,
    PMIC_ARB_CMD_EXTENDED_REG_WRITE = 2,
    PMIC_ARB_CMD_RESET = 3,
    PMIC_ARB_CMD_SLEEP = 4,
    PMIC_ARB_CMD_SHUTDOWN = 5,
    PMIC_ARB_CMD_WAKEUP = 6,
    PMIC_ARB_CMD_AUTHENTICATE = 7,
    PMIC_ARB_CMD_MASTER_READ = 8,
    PMIC_ARB_CMD_MASTER_WRITE = 9,
    PMIC_ARB_CMD_TRANSFER_BUS_OWNERSHIP = 10,
    PMIC_ARB_CMD_DEVICE_DESC_BLOCK_MASTER = 11,
    PMIC_ARB_CMD_DEVICE_DESC_BLOCK_SLAVE = 12,
    PMIC_ARB_CMD_EXTENDED_REGISTER_READ = 13,
    PMIC_ARB_CMD_REGISTER_WRITE = 14,
    PMIC_ARB_CMD_REGISTER_READ = 15,
    PMIC_ARB_CMD_REGISTER_0_WRITE = 16,
    PMIC_ARB_CMD_MAX
} PmicArbCmd;

typedef struct 
{
    uint32 irqStatus;
    uint32 geniStatus;
    uint32 geniCtrl;
} PmicArbHwStatus;

typedef void (*PmicArbIsrCallback)(uint8 channel, uint8 mask);

Spmi_Result PmicArb_Init(PmicArbIsrCallback isrCb);
Spmi_Result PmicArb_ConfigHW(SpmiCfg_ConfigType cfgType);
Spmi_Result PmicArb_FindChannel(uint8 slaveId, uint8 periph, uint8* channel);
Spmi_Result PmicArb_SetInterruptEnabled(uint8 channel, boolean enabled);
Spmi_Result PmicArb_ExecuteCommand(PmicArbCmd cmd,
                                   Spmi_AccessPriority priority,
                                   uint8 slaveId,
                                   uint16 address,
                                   uint8* data,
                                   uint32 dataLen,
                                   uint32* bytesTransacted);

void PmicArb_GetHwStatus(PmicArbHwStatus* status);
boolean PmicArb_HasInterruptSupport(void);
uint32 PmicArb_GetMaxChannelSupport(void);

#endif
