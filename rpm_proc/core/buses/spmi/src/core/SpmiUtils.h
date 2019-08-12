/**
 * @file:  SpmiUtils.h
 * @brief: Helper functions, macros, and types used internally by the SPMI driver
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/02/25 02:53:43 $
 * $Header: //components/rel/rpm.bf/2.1.1/core/buses/spmi/src/core/SpmiUtils.h#1 $
 * $Change: 7538753 $
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */
#ifndef SPMIUTILS_H
#define	SPMIUTILS_H

#define SPMI_TIMEOUT_USEC 400

#define SPMI_MAX_MASTER_ID   0x03
#define SPMI_MAX_SLAVE_ID    0x0F
#define SPMI_MAX_PERIPH_ID   0xFF
#define SPMI_MAX_REG_OFFSET  0xFF

#define SPMI_MAX_REG_ADDRESS 0xFFFF

#define REG_ADDR_TO_PERIPH_ID(addr)  (((addr) >> 8) & 0xFF)
#define REG_ADDR_TO_REG_OFFSET(addr) ((addr) & 0xFF)

#define SLAVE_PERIPH_TO_PPID(slave, periph) (((slave) << 8) | (periph))

#define PPID_TO_SLAVE_ID(ppid)  (((ppid) >> 8) & SPMI_MAX_SLAVE_ID)
#define PPID_TO_PERIPH_ID(ppid) ((ppid) & SPMI_MAX_PERIPH_ID)

typedef enum
{
    SPMI_CFG_NONE,   /** No static configuration will be done */
    SPMI_CFG_FULL,   /** All supported features will be enabled */
    SPMI_CFG_MINIMAL /** A minimal number of features will be enabled */
} SpmiCfg_ConfigType;

#endif

