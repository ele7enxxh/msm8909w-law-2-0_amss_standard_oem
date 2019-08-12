/**
 * @file:  SpmiCfg.c
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/09/01 00:30:35 $
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/spmi/src/core/hal/bear/SpmiCfg.c#1 $
 * $Change: 8935858 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 11/3/14  Automatic channel assignment
 * 9/2/14   Enabled security features when channel info is set
 * 10/1/13  Initial Version
*/

#include <stdlib.h>
#include "SpmiCfg.h"
#include "SpmiCfgInternal.h"
#include "SpmiUtils.h"
#include "SpmiLogs.h"
#include "SpmiHal.h"
#include "SpmiMaster.h"
#include "PmicArb.h"
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
static uint16 nextChanIdx = 0;
static boolean dynamicChannelMode = FALSE;

//******************************************************************************
// Local Helper Functions
//******************************************************************************

typedef boolean (*CmprFunc)(const SpmiCfg_ChannelCfg*, const SpmiCfg_ChannelCfg*);

static boolean ppidCmpr(const SpmiCfg_ChannelCfg* c1, const SpmiCfg_ChannelCfg* c2)
{
    if(c2->slaveId == c1->slaveId) {
        return c2->periphId > c1->periphId;
    }
    
    return c2->slaveId > c1->slaveId;
}

static boolean ownerCmpr(const SpmiCfg_ChannelCfg* c1, const SpmiCfg_ChannelCfg* c2)
{    
    if(c1->periphOwner == c2->periphOwner) {
        return c1->intOwner > c2->intOwner;
    }
    
    return c1->periphOwner > c2->periphOwner;
}

static void shellSort(SpmiCfg_ChannelCfg* entries, uint32 numEntries, CmprFunc cmpr)
{
    // https://en.wikipedia.org/wiki/Shellsort
    static const uint8 gaps[] = {57, 23, 10, 4, 1};
    uint32 i, k;
    
    for(i = 0; i < sizeof(gaps) / sizeof(uint8); i++) 
    {
        uint8 gap = gaps[i];
        
        for(k = gap; k < numEntries; k++)
        {
            SpmiCfg_ChannelCfg tmp = entries[k];
            uint32 j = k;
            
            while(j >= gap && cmpr( &entries[j - gap], &tmp ))
            {
                entries[j] = entries[j-gap];
                j -= gap;
            }
            
            entries[j] = tmp;
        }
    }
}

static Spmi_Result assignChannel(SpmiCfg_ChannelCfg* entry, boolean autoAssignChan)
{
    if(autoAssignChan) {
        entry->channel = nextChanIdx;
    }
    
    if(entry->channel > HWIO_PMIC_ARB_REG_CHNLn_MAXn) 
    {
        if(autoAssignChan && dynamicChannelMode) 
        {
            SPMI_LOG_VERBOSE( "Exhausted channels. Rolling over." );
            entry->channel = 0;
        }
        else {
            return SPMI_FAILURE_INVALID_CHAN_NUM;
        }
    }
    
    nextChanIdx = entry->channel + 1;
    
    return SPMI_SUCCESS;
}

static Spmi_Result configChannels(SpmiCfg_ChannelCfg* entries, uint32 numEntries, 
                                  SpmiCfg_RGConfig* rgCfg, uint32* rgCount, 
                                  boolean autoAssignChan, boolean configHW, boolean legacySorted)
{
    uint32 i;
    uint32 rgIdx = 0;
    Spmi_Result rslt;
    SpmiCfg_RGConfig channelRG;
    SpmiCfg_RGConfig interruptRG;
    SpmiCfg_ChannelCfg* entry = NULL;
    
    INIT_CHECK();
    
    if(entries == NULL || numEntries == 0 || (rgCfg != NULL && rgCount == NULL)) {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }
    
    if(numEntries > HWIO_PMIC_ARB_REG_CHNLn_MAXn + 1) {
        return SPMI_FAILURE_TOO_MANY_ENTRIES;
    }
    
    if(autoAssignChan) {
        nextChanIdx = 0;
    }
    
    if(configHW)
    {
        // Zero table
        for (i = 0; i <= HWIO_PMIC_ARB_REG_CHNLn_MAXn; i++) {
            SPMI_HWIO_OUT( HWIO_PMIC_ARB_REG_CHNLn_ADDR( i ), 0 );
        }
    }
    
    if(rgCfg != NULL)
    {
        shellSort(entries, numEntries, ownerCmpr);

        channelRG.owner = entries[0].periphOwner;
        channelRG.startIdx = 0;
        channelRG.startAddr = (uint32) HWIO_PMIC_ARBq_CHNLn_CMD_ADDR( 0, 0 );

        interruptRG.owner = entries[0].intOwner;
        interruptRG.startIdx = 0;
        interruptRG.startAddr = (uint32) HWIO_SPMI_PIC_ACC_ENABLEn_ADDR( 0 );
    }
    
    for (i = 0; i < numEntries; i++)
    {
        entry = &entries[i];

        if(entry->slaveId > SPMI_MAX_SLAVE_ID) {
            return SPMI_FAILURE_INVALID_SLAVE_ID;
        }
        
        if(entry->intOwner > HWIO_PMIC_ARBq_CHNLn_CMD_MAXq) {
            return SPMI_FAILURE_INVALID_OWNER;
        }
        
        if(rgCfg != NULL &&
           entry->periphOwner > HWIO_PMIC_ARBq_CHNLn_CMD_MAXq && 
           entry->periphOwner != SPMI_OPEN_OWNER) 
        {
            return SPMI_FAILURE_INVALID_OWNER;
        }
        
        if(configHW) {
            rslt = SpmiCfg_ConfigureChannel( entry, autoAssignChan );
        }
        else {
            rslt = assignChannel( entry, autoAssignChan );
        }
        
        if(rslt != SPMI_SUCCESS) {
            return rslt;
        }
        
        if(rgCfg != NULL)
        {
            if(entry->periphOwner != channelRG.owner)
            {
                if(rgIdx == *rgCount) {
                    return SPMI_FAILURE_NOT_ENOUGH_RGS;
                }
                
                channelRG.endIdx = entry->channel - 1;
                channelRG.size = (uint32) HWIO_PMIC_ARBq_CHNLn_CMD_ADDR( 0, entry->channel ) - channelRG.startAddr;
                rgCfg[rgIdx++] = channelRG;

                channelRG.owner = entry->periphOwner;
                channelRG.startIdx = entry->channel;
                channelRG.startAddr = (uint32) HWIO_PMIC_ARBq_CHNLn_CMD_ADDR( 0, entry->channel );
            }

            if(entry->intOwner != interruptRG.owner)
            {
                if(rgIdx == *rgCount) {
                    return SPMI_FAILURE_NOT_ENOUGH_RGS;
                }
                
                interruptRG.endIdx = entry->channel - 1;
                interruptRG.size = (uint32) HWIO_SPMI_PIC_ACC_ENABLEn_ADDR( entry->channel ) - interruptRG.startAddr;
                rgCfg[rgIdx++] = interruptRG;

                interruptRG.owner = entry->intOwner;
                interruptRG.startIdx = entry->channel;
                interruptRG.startAddr = (uint32) HWIO_SPMI_PIC_ACC_ENABLEn_ADDR( entry->channel );
            }
        }
    }

    if(rgCfg != NULL)
    {
        channelRG.endIdx = entry->channel;
        channelRG.size = (uint32) HWIO_PMIC_ARBq_CHNLn_CMD_ADDR( 0, entry->channel + 1 ) - channelRG.startAddr;

        if(channelRG.owner != SPMI_OPEN_OWNER)
        {
            if(rgIdx == *rgCount) {
                return SPMI_FAILURE_NOT_ENOUGH_RGS;
            }
            
            rgCfg[rgIdx++] = channelRG;
        }

        if(rgIdx == *rgCount) {
            return SPMI_FAILURE_NOT_ENOUGH_RGS;
        }

        interruptRG.endIdx = entry->channel;
        interruptRG.size = (uint32) HWIO_SPMI_PIC_ACC_ENABLEn_ADDR( entry->channel + 1 ) - interruptRG.startAddr;
        rgCfg[rgIdx++] = interruptRG;
    }
    
    if(rgCount != NULL) {
        *rgCount = rgIdx;
    }
    
    if(configHW && !dynamicChannelMode)
    {
        // Load the SPMI radix tree
        if(!legacySorted) {
            shellSort(entries, numEntries, ppidCmpr);
        }
        SpmiMaster_LoadRadixTree( entries, numEntries );
    
        // Enable security now that the tables are configured
        SPMI_HWIO_OUT_SET( HWIO_SPMI_CMPR_EN_REG_ADDR, HWIO_SPMI_CMPR_EN_REG_CMPR_ENABLE_BMSK );
        SPMI_HWIO_OUT_CLEAR( HWIO_SPMI_SEC_DISABLE_REG_ADDR, HWIO_SPMI_SEC_DISABLE_REG_DISABLE_SECURITY_BMSK );

        // Enable interrupt processing
        SPMI_HWIO_OUT_SET( HWIO_SPMI_MWB_ENABLE_REG_ADDR, HWIO_SPMI_MWB_ENABLE_REG_MWB_ENABLE_BMSK );
    }
    
    return SPMI_SUCCESS;
}

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result SpmiCfg_Init(boolean initHw)
{
    Spmi_Result rslt;
    
    if (!initialized) 
    {
        SPMI_LOG_INIT();

        if ((rslt = SpmiHal_Init()) != SPMI_SUCCESS) {
            return rslt;
        }
        
        initialized = TRUE;
    }
    
    if(initHw)
    {
        if((rslt = SpmiMaster_ConfigHW( DEFAULT_MASTER_ID, SPMI_CFG_FULL )) != SPMI_SUCCESS) {
            return rslt;
        }

        if((rslt = PmicArb_ConfigHW( SPMI_CFG_FULL )) != SPMI_SUCCESS) {
            return rslt;
        }
    }

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
    
    if(channel > HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_MAXm) {
        return SPMI_FAILURE_INVALID_CHAN_NUM;
    }
    
    *slaveId = SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_REG_CHNLn_ADDR( channel ), HWIO_PMIC_ARB_REG_CHNLn_SID );
    *periphId = SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_REG_CHNLn_ADDR( channel ), HWIO_PMIC_ARB_REG_CHNLn_ADDRESS );
    *owner = SPMI_HWIO_IN_FIELD( HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( channel ), HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER );
    
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

    for(i = 0; i <= HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_MAXm; i++)
    {
        if(ppid == SPMI_HWIO_IN_FIELD( HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( i ), HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_APID2PPID ))
        {
            *channel = i;
            *owner = SPMI_HWIO_IN_FIELD( HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( i ), HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER );
            return SPMI_SUCCESS;
        }   
    }

    return SPMI_FAILURE_CHANNEL_NOT_FOUND;
}

boolean SpmiCfg_InDynamicChannelMode()
{
    return dynamicChannelMode;
}

Spmi_Result SpmiCfg_SetDynamicChannelMode(boolean enabled)
{
    // If we are changing the channel -> ppid mapping, the security 
    // and interrupt tables wont be in sync (assuming they were populated)
    // and we dont have the information to update them; so interrupts and security aren't supported.
    if(enabled) 
    {
        SPMI_LOG_WARNING( "Disabling security and interrupts" );
        SPMI_HWIO_OUT_CLEAR( HWIO_SPMI_CMPR_EN_REG_ADDR, HWIO_SPMI_CMPR_EN_REG_CMPR_ENABLE_BMSK );
        SPMI_HWIO_OUT_SET( HWIO_SPMI_SEC_DISABLE_REG_ADDR, HWIO_SPMI_SEC_DISABLE_REG_DISABLE_SECURITY_BMSK );
        SPMI_HWIO_OUT_CLEAR( HWIO_SPMI_MWB_ENABLE_REG_ADDR, HWIO_SPMI_MWB_ENABLE_REG_MWB_ENABLE_BMSK );
    }
    
    dynamicChannelMode = enabled;
    
    return SPMI_SUCCESS;
}

Spmi_Result SpmiCfg_ConfigureChannel(SpmiCfg_ChannelCfg* entry, boolean autoAssignChan)
{
    Spmi_Result rslt;
    uint32 reg;
    
    if((rslt = assignChannel( entry, autoAssignChan )) != SPMI_SUCCESS) {
        return rslt;
    }
    
    // Update the channel address translation table - used by owner and observer channels
    reg = SPMI_HWIO_SET_FIELD_VALUE( entry->slaveId, HWIO_PMIC_ARB_REG_CHNLn_SID );
    reg |= SPMI_HWIO_SET_FIELD_VALUE( entry->periphId, HWIO_PMIC_ARB_REG_CHNLn_ADDRESS );
    
    SPMI_HWIO_OUT( HWIO_PMIC_ARB_REG_CHNLn_ADDR( entry->channel ), reg );

    if(!dynamicChannelMode)
    {
        // Update peripheral interrupt ownership table
        reg = SPMI_HWIO_SET_FIELD_VALUE( SLAVE_PERIPH_TO_PPID( entry->slaveId, entry->periphId ),
                                         HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_APID2PPID );

        reg |= SPMI_HWIO_SET_FIELD_VALUE( entry->intOwner,
                                          HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER );

        SPMI_HWIO_OUT( HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( entry->channel ), reg );
    }
    
    SPMI_LOG_VERBOSE( "%sPPID %01X %02X (intOwner %d, chanOwner %d) @ chan %d",
                      dynamicChannelMode ? "*" : "",
                      entry->slaveId, entry->periphId, entry->intOwner, 
                      entry->periphOwner, entry->channel );
    
    return SPMI_SUCCESS;
}

Spmi_Result SpmiCfg_ConfigureChannels(SpmiCfg_ChannelCfg* entries, uint32 numEntries)
{
    return configChannels( entries, numEntries, NULL, NULL, FALSE, TRUE, TRUE );
}

Spmi_Result SpmiCfg_ConfigurePeripherals(SpmiCfg_ChannelCfg* entries, uint32 numEntries, 
                                         SpmiCfg_RGConfig* rgCfg, uint32* rgCount)
{
    return configChannels( entries, numEntries, rgCfg, rgCount, TRUE, TRUE, FALSE );
}

Spmi_Result SpmiCfg_CalculateRGConfig(SpmiCfg_ChannelCfg* entries, uint32 numEntries, 
                                      SpmiCfg_RGConfig* rgCfg, uint32* rgCount)
{
    return configChannels( entries, numEntries, rgCfg, rgCount, TRUE, FALSE, FALSE );
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
    SPMI_HWIO_OUT_SET( HWIO_PMIC_ARB_PVC_PORTn_CTL_ADDR( portCfg->pvcPortId ), HWIO_PMIC_ARB_PVC_PORTn_CTL_PVC_PORT_EN_BMSK );

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
