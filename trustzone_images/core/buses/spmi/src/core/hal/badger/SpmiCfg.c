/**
 * @file:  SpmiCfg.c
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/06/17 14:31:11 $
 * $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spmi/src/core/hal/badger/SpmiCfg.c#1 $
 * $Change: 10727476 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Badger Version
*/

#include "SpmiCfg.h"
#include "SpmiUtils.h"
#include "SpmiLogs.h"
#include "SpmiHal.h"
#include "SpmiMaster.h"
#include "SpmiOs.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define DEFAULT_MASTER_ID 0
#define INIT_CHECK() do { if(!initialized) { return SPMI_FAILURE_NOT_INITIALIZED; } } while(FALSE)

//******************************************************************************
// Global Data
//******************************************************************************

static boolean initialized = FALSE;

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result SpmiCfg_Init(boolean initHw)
{
    Spmi_Result rslt;
    
    if(initialized) {
        return SPMI_SUCCESS;
    }
    
    SPMI_OS_LOG_INIT();
    
    if((rslt = SpmiHal_Init()) != SPMI_SUCCESS) {
        return rslt;
    }
    
    if(initHw)
    {
        if((rslt = SpmiMaster_ConfigHW( DEFAULT_MASTER_ID, SPMI_CFG_FULL )) != SPMI_SUCCESS) {
            return rslt;
        }
    }
    
    initialized = TRUE;
    return SPMI_SUCCESS;
}

Spmi_Result SpmiCfg_DeInit()
{
    initialized = FALSE;
    return SPMI_SUCCESS;
}

Spmi_Result SpmiCfg_GetChannelInfo(uint16 channel, uint8* slaveId, uint8* periphId, uint8* owner)
{
    INIT_CHECK();
    
    if(channel > HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_MAXm) {
        return SPMI_FAILURE_INVALID_CHAN_NUM;
    }
    
    uint32 ppid = SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( channel ), HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_APID2PPID );
    
    *slaveId = PPID_TO_SLAVE_ID( ppid );
    *periphId = PPID_TO_PERIPH_ID( ppid );
    *owner = SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( channel ), HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER );
                             
    return SPMI_SUCCESS;
}

Spmi_Result SpmiCfg_GetPeripherialInfo(uint8 slaveId, uint8 periphId, uint16* channel, uint8* owner)
{
    uint32 i;    
    INIT_CHECK();
    
    if(slaveId > SPMI_MAX_SLAVE_ID) {
        return SPMI_FAILURE_INVALID_SLAVE_ID;
    }
    
    uint16 ppid = SLAVE_PERIPH_TO_PPID( slaveId, periphId );

    for(i = 0; i <= HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_MAXm; i++)
    {
        if(ppid == SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( i ), HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_APID2PPID ))
        {
            *channel = i;
            *owner = SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( i ), HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER );
            return SPMI_SUCCESS;
        }   
    }

    return SPMI_FAILURE_CHANNEL_NOT_FOUND;
}

Spmi_Result SpmiCfg_ConfigureChannels(SpmiCfg_ChannelCfg* entries, uint32 numEntries)
{
    uint32 i;
    INIT_CHECK();
    
    for (i = 0; i < numEntries; i++) 
    {
        SpmiCfg_ChannelCfg* entry = &entries[i];

        if(entry->channel > HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_MAXm) {
            return SPMI_FAILURE_INVALID_CHAN_NUM;
        }

        if(entry->intOwner > (HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER_BMSK >> 
                              HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER_SHFT)) 
        {
            return SPMI_FAILURE_INVALID_OWNER;
        }

        if(entry->slaveId > SPMI_MAX_SLAVE_ID) {
            return SPMI_FAILURE_INVALID_SLAVE_ID;
        }
        
        //
        // Update peripheral interrupt ownership table
        //
        SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( entry->channel ),
                             HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_APID2PPID, SLAVE_PERIPH_TO_PPID( entry->slaveId, entry->periphId ) );
        
        SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( entry->channel ),
                             HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER, entry->intOwner );
                    
    }
        
    //
    // Load the SPMI radix tree
    //
    SpmiMaster_LoadRadixTree( entries, numEntries );
    
    return SPMI_SUCCESS;
}

Spmi_Result SpmiCfg_ConfigurePvcPort(const SpmiCfg_PvcPortCfg* portCfg)
{
    uint32 i;
    INIT_CHECK();

    if(portCfg->pvcPortId > HWIO_PMIC_ARB_PVC_PORTn_CTL_MAXn || portCfg->pvcPortId > HWIO_PMIC_ARB_PVCn_ADDRm_MAXn) {
        return SPMI_FAILURE_INVALID_PORT_ID;
    }
    
    SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_PVC_PORTn_CTL_ADDR( portCfg->pvcPortId ), 
                         HWIO_PMIC_ARB_PVC_PORTn_CTL_SPMI_PRIORITY, portCfg->priority );

    if(portCfg->numPpids > HWIO_PMIC_ARB_PVCn_ADDRm_MAXm + 1) {
        return SPMI_FAILURE_TOO_MANY_ENTRIES;
    }
    
    for(i = 0; i < portCfg->numPpids; i++) 
    {
        if(portCfg->ppids[i].slaveId > SPMI_MAX_SLAVE_ID) {
            return SPMI_FAILURE_INVALID_SLAVE_ID;
        }

        SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_PVCn_ADDRm_ADDR( portCfg->pvcPortId, i ), HWIO_PMIC_ARB_PVCn_ADDRm_SID, portCfg->ppids[i].slaveId );
        SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_PVCn_ADDRm_ADDR( portCfg->pvcPortId, i ), HWIO_PMIC_ARB_PVCn_ADDRm_ADDRESS, portCfg->ppids[i].address );
    }

    // Enable the individual port
    SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_PVC_PORTn_CTL_ADDR( portCfg->pvcPortId ), HWIO_PMIC_ARB_PVC_PORTn_CTL_PVC_PORT_EN, 0x01 );

    return SPMI_SUCCESS;
}

Spmi_Result SpmiCfg_SetPortPriorities(uint8* ports, uint32 numPorts)
{
    uint32 i;
    INIT_CHECK();
    
    if(numPorts > HWIO_PMIC_ARB_PRIORITIESn_MAXn + 1) {
        return SPMI_FAILURE_TOO_MANY_ENTRIES;
    }
    
    for(i = 0; i < numPorts; i++) 
    {
        if(ports[i] > (HWIO_PMIC_ARB_PRIORITIESn_PORT_BMSK >> HWIO_PMIC_ARB_PRIORITIESn_PORT_SHFT)) {
            return SPMI_FAILURE_INVALID_PORT_ID;
        }

        SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_PRIORITIESn_ADDR( i ), HWIO_PMIC_ARB_PRIORITIESn_PORT, ports[i] );
    }
    
    return SPMI_SUCCESS;
}

Spmi_Result SpmiCfg_SetPVCPortsEnabled(boolean enable)
{
    INIT_CHECK();
    
    SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_PVC_INTF_CTL_ADDR, HWIO_PMIC_ARB_PVC_INTF_CTL_PVC_INTF_EN, enable );
    return SPMI_SUCCESS;
}
