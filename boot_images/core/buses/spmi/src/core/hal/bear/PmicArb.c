/**
 * @file:  PmicArb.c
 * 
 * Copyright (c) 2013-2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/09/01 00:30:35 $
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/spmi/src/core/hal/bear/PmicArb.c#1 $
 * $Change: 8935858 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 12/5/14  Added check to limit the spmi channels equal to spmi interrupts
 * 11/3/14  Automatic channel assignment
 * 05/8/14  Moved the clock status check to other file.
 * 10/1/13  Initial Version
 */

#include "PmicArb.h"
#include "SpmiUtils.h"
#include "SpmiHal.h"
#include "SpmiLogs.h"
#include "SpmiOs.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

#define INVALID_SID -1
#define INVALID_DATA ((int16)0x8000)

#define PMIC_ARB_MAX_PERIPHERAL_SUPPORT (HWIO_PMIC_ARBq_CHNLn_CMD_MAXn + 1)

// Max number of bytes we can read/write at a time.  +1 because 0 means read/write 1 byte
#define MAX_TRANSACTION_LEN ((HWIO_PMIC_ARBq_CHNLn_CMD_BYTE_CNT_BMSK >> HWIO_PMIC_ARBq_CHNLn_CMD_BYTE_CNT_SHFT) + 1)

typedef struct __attribute((__packed__)) {
    int8 sid;
    int16 prevIdx;
    int16 nextIdx;
} ChannelMapEntry;

typedef struct {
    uint8 owner;
    boolean hasInterruptSupport;
    PmicArbIsrCallback isrCallback;
    int16 periphMap[SPMI_MAX_PERIPH_ID + 1];
    ChannelMapEntry channelMap[PMIC_ARB_MAX_PERIPHERAL_SUPPORT];
} PmicArbInfo;

//******************************************************************************
// Global Data
//******************************************************************************

PmicArbInfo spmiPmicArbInfo = {0};

//******************************************************************************
// Local Helper Functions
//******************************************************************************

static boolean isInterruptOwner(uint8 channel)
{
    return spmiPmicArbInfo.owner == SPMI_HWIO_IN_FIELD( HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( channel ),
                                                        HWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER );
}

static void writeSpmiData(uint8 channel, uint8* data, uint32 bytesToWrite)
{
    uint32 i;
    uint32 tmp = 0;
    
    for(i = 0; i < sizeof(uint32) && bytesToWrite > 0; i++, bytesToWrite--) {
        ((uint8*)&tmp)[i] = data[i];
    }
    
    SPMI_HWIO_OUT( HWIO_PMIC_ARBq_CHNLn_WDATA0_ADDR( spmiPmicArbInfo.owner, channel ), tmp );
    
    if(bytesToWrite > 0)
    {
        tmp = 0;
        for(i = 0; i < sizeof(uint32) && bytesToWrite > 0; i++, bytesToWrite--) {
            ((uint8*)&tmp)[i] = data[i + sizeof(uint32)];
        }
        
        SPMI_HWIO_OUT( HWIO_PMIC_ARBq_CHNLn_WDATA1_ADDR( spmiPmicArbInfo.owner, channel ), tmp );
    }
}

static void readSpmiData(uint8 channel, uint8* data, uint32 bytesToRead)
{
    uint32 i;
    uint32 spmiData = SPMI_HWIO_IN( HWIO_PMIC_ARB_OBSq_CHNLn_RDATA0_ADDR( spmiPmicArbInfo.owner, channel ) );
        
    for(i = 0; i < sizeof(uint32) && bytesToRead > 0; i++, bytesToRead--) {
        data[i] = ((uint8*)&spmiData)[i];
    }

    if(bytesToRead > 0)
    {
        spmiData = SPMI_HWIO_IN( HWIO_PMIC_ARB_OBSq_CHNLn_RDATA1_ADDR( spmiPmicArbInfo.owner, channel ) );

        for(i = 0; i < sizeof(uint32) && bytesToRead > 0; i++, bytesToRead--) {
            data[i + sizeof(uint32)] = ((uint8*)&spmiData)[i];
        }
    }
}

static Spmi_Result waitForStatusDone(boolean isObserver, uint8 channel)
{
    uint32 status = 0;
    uint32 timeout = SPMI_TIMEOUT_USEC;
    Spmi_Result result = SPMI_SUCCESS;
    
    while(status == 0 && timeout-- > 0)
    {
        SpmiOs_Wait( 1 );
        status = isObserver ? SPMI_HWIO_IN( HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_ADDR( spmiPmicArbInfo.owner, channel ) ) :
                              SPMI_HWIO_IN( HWIO_PMIC_ARBq_CHNLn_STATUS_ADDR( spmiPmicArbInfo.owner, channel ) );
    }
    
    if(status & HWIO_PMIC_ARBq_CHNLn_STATUS_DROPPED_BMSK) {
        result = SPMI_FAILURE_TRANSACTION_DROPPED;
    } 
    else if(status & HWIO_PMIC_ARBq_CHNLn_STATUS_DONE_BMSK) 
    {
        if(status & HWIO_PMIC_ARBq_CHNLn_STATUS_FAILURE_BMSK) {
            result = SPMI_FAILURE_TRANSACTION_FAILED;
        } 
        else if(status & HWIO_PMIC_ARBq_CHNLn_STATUS_DENIED_BMSK) {
            result = SPMI_FAILURE_TRANSACTION_DENIED;
        }
    }
    else if (SPMI_HWIO_IN_FIELD( HWIO_SPMI_GENI_CTRL_ADDR, HWIO_SPMI_GENI_CTRL_M_GENI_DISABLE )) {
        result = SPMI_FAILURE_BUS_DISABLED;
    }
    else {
        result = SPMI_FAILURE_TRANSACTION_TIMEOUT;
    }

    return result;
}

static __inline boolean isTransactionDone(boolean isObserver, uint8 channel)
{
    if(isObserver) {
        return SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_ADDR( spmiPmicArbInfo.owner, channel ),
                                   HWIO_PMIC_ARB_OBSq_CHNLn_STATUS_DONE );
    } else {
        return SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARBq_CHNLn_STATUS_ADDR( spmiPmicArbInfo.owner, channel ),
                                   HWIO_PMIC_ARBq_CHNLn_STATUS_DONE );
    }
}

static __inline void writeCommand(boolean isObserver,
                                  PmicArbCmd cmd,
                                  uint8 channel,
                                  Spmi_AccessPriority priority,
                                  uint8 regOffset,
                                  uint32 len)
{
    if(isObserver) 
    {
        SPMI_HWIO_OUT( HWIO_PMIC_ARB_OBSq_CHNLn_CMD_ADDR( spmiPmicArbInfo.owner, channel ),
                      (cmd                      << HWIO_PMIC_ARB_OBSq_CHNLn_CMD_OPCODE_SHFT)   |
                      (SPMI_ACCESS_PRIORITY_LOW << HWIO_PMIC_ARB_OBSq_CHNLn_CMD_PRIORITY_SHFT) |
                      (regOffset                << HWIO_PMIC_ARB_OBSq_CHNLn_CMD_ADDRESS_SHFT)  |
                      (len                      << HWIO_PMIC_ARB_OBSq_CHNLn_CMD_BYTE_CNT_SHFT) );
    }
    else 
    {
        SPMI_HWIO_OUT( HWIO_PMIC_ARBq_CHNLn_CMD_ADDR( spmiPmicArbInfo.owner, channel ),
                      (cmd       << HWIO_PMIC_ARBq_CHNLn_CMD_OPCODE_SHFT)   |
                      (priority  << HWIO_PMIC_ARBq_CHNLn_CMD_PRIORITY_SHFT) |
                      (regOffset << HWIO_PMIC_ARBq_CHNLn_CMD_ADDRESS_SHFT)  |
                      (len       << HWIO_PMIC_ARBq_CHNLn_CMD_BYTE_CNT_SHFT) );
    }
}

static void* pmicArbIsr(void* ctx)
{
    uint32 i, k;
    
    for(i = 0; i <= HWIO_SPMI_PIC_OWNERm_ACC_STATUSn_MAXn; i++) 
    {
        // Each bit in these registers represents a peripheral
        uint32 accStatus = SPMI_HWIO_IN( HWIO_SPMI_PIC_OWNERm_ACC_STATUSn_ADDR( spmiPmicArbInfo.owner, i ) );
        
        for(k = 0; (k < 32) && (accStatus != 0); k++) 
        {
            if((accStatus >> k) & 0x1) 
            {
                // This peripheral (channel) triggered the interrupt.  Check its individual mask
                uint8 channel = (i * 32) + k;
                uint8 mask = SPMI_HWIO_IN_FIELD( HWIO_SPMI_PIC_IRQ_STATUSn_ADDR( channel ), HWIO_SPMI_PIC_IRQ_STATUSn_INT_STATUS );
                
                // Clear the status
                SPMI_HWIO_OUT_FIELD( HWIO_SPMI_PIC_IRQ_CLEARn_ADDR( channel ), HWIO_SPMI_PIC_IRQ_CLEARn_INT_CLEAR, mask );
                
                // wait for the status to clear
                //while(SPMI_HWIO_IN_FIELD( HWIO_SPMI_PIC_IRQ_STATUSn_ADDR( channel ), HWIO_SPMI_PIC_IRQ_STATUSn_INT_STATUS ) != 0);
                
                // Notify the higher layers
                if(spmiPmicArbInfo.isrCallback != NULL) {
                    spmiPmicArbInfo.isrCallback( channel, mask );
                }
            }
        }
    }
    
    return NULL;
}

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result PmicArb_ConfigHW(SpmiCfg_ConfigType cfgType)
{
    // Nothing to do
    return SPMI_SUCCESS;
}

Spmi_Result PmicArb_Init(PmicArbIsrCallback isrCb)
{
    Spmi_Result rslt;

    spmiPmicArbInfo.isrCallback = isrCb;
    
    if((rslt = SpmiHal_Init()) != SPMI_SUCCESS) {
        return rslt;
    }
    
    if((rslt = SpmiOs_GetOwnerId( &spmiPmicArbInfo.owner )) != SPMI_SUCCESS) {
        return rslt;
    }

    if((rslt = SpmiOs_RegisterISR( pmicArbIsr, NULL )) != SPMI_SUCCESS) {
        SPMI_LOG_INFO( "SPMI interrupts not supported: %d", rslt );
        spmiPmicArbInfo.hasInterruptSupport = FALSE;
    }
    else {
        spmiPmicArbInfo.hasInterruptSupport = TRUE;
    }
    
    // Cache all ppid -> channel mappings
    PmicArb_UpdateCache();
    
    SPMI_LOG_VERBOSE( "SPMI IRQ status at init: 0x%lx", SPMI_HWIO_IN( HWIO_SPMI_PROTOCOL_IRQ_STATUS_ADDR ) );
    
    return SPMI_SUCCESS;
}

Spmi_Result PmicArb_InvalidateCache()
{
    uint32 i;
    
    for(i = 0; i <= SPMI_MAX_PERIPH_ID; i++) {
        spmiPmicArbInfo.periphMap[i] = INVALID_DATA;
    }
    
    for(i = 0; i < PMIC_ARB_MAX_PERIPHERAL_SUPPORT; i++) {
        spmiPmicArbInfo.channelMap[i].sid = INVALID_SID;
        spmiPmicArbInfo.channelMap[i].nextIdx = INVALID_DATA;
        spmiPmicArbInfo.channelMap[i].prevIdx = INVALID_DATA;
    }
    
    return SPMI_SUCCESS;
}

Spmi_Result PmicArb_UpdateCache()
{
    uint32 i;
    PmicArb_InvalidateCache();
    
    for(i = 0; i < PMIC_ARB_MAX_PERIPHERAL_SUPPORT; i++) 
    {
        uint32 regChan = SPMI_HWIO_IN( HWIO_PMIC_ARB_REG_CHNLn_ADDR( i ) );
        uint8 sid = SPMI_HWIO_GET_FIELD_VALUE( regChan, HWIO_PMIC_ARB_REG_CHNLn_SID );
        uint8 addr = SPMI_HWIO_GET_FIELD_VALUE( regChan, HWIO_PMIC_ARB_REG_CHNLn_ADDRESS );
        
        PmicArb_AddCacheEntry( i, sid, addr );
    }
    
    return SPMI_SUCCESS;
}

Spmi_Result PmicArb_AddCacheEntry(uint32 chan, uint8 sid, uint8 periph)
{
    int16 chanIdx;
    boolean newRoot = FALSE;
    ChannelMapEntry* entry;
    ChannelMapEntry* newEntry;

    if(chan >= PMIC_ARB_MAX_PERIPHERAL_SUPPORT) {
        return SPMI_FAILURE_INVALID_CHAN_NUM;
    }
    
    newEntry = &spmiPmicArbInfo.channelMap[chan];
    
    SPMI_LOG_TRACE("Add to cache - chan %d: sid: %d, periph: %d (0x%x)", chan, sid, periph, periph);

    if(spmiPmicArbInfo.periphMap[periph] == INVALID_DATA)
    {
        SPMI_LOG_TRACE("  first hit empty");
        newRoot = TRUE;
        spmiPmicArbInfo.periphMap[periph] = chan;
    }
    
    // 1st entry
    chanIdx = spmiPmicArbInfo.periphMap[periph];
    entry = &spmiPmicArbInfo.channelMap[chanIdx];

    // If this isn't the start of a new chain, follow the existing one
    if(!newRoot)
    {
        int16 tmpIdx = chanIdx;
        
        // go till the end of the chain or we hit the replacement
        do
        {
            chanIdx = tmpIdx;
            entry = &spmiPmicArbInfo.channelMap[chanIdx];
            tmpIdx = entry->nextIdx;
            
            if(entry->sid == sid) {
                SPMI_LOG_TRACE( "  dup @ chan %d", chanIdx );
            }

            if(chanIdx == chan) {
                SPMI_LOG_TRACE( "  overwriting chan %d (in chain)", chan );
                newEntry->sid = sid;
                return SPMI_SUCCESS;
            }

        } while((entry->nextIdx & INVALID_DATA) == 0);
    }
        
    if(newEntry->sid != INVALID_SID) 
    {
        SPMI_LOG_TRACE( "  overwriting chan %d (out of chain)", chan);
        
        // pull newEntry out of its chain
        if((newEntry->nextIdx & INVALID_DATA) == 0) {
            spmiPmicArbInfo.channelMap[newEntry->nextIdx].prevIdx = newEntry->prevIdx;
        }
        
        if((newEntry->prevIdx & INVALID_DATA) == 0) {
            spmiPmicArbInfo.channelMap[newEntry->prevIdx].nextIdx = newEntry->nextIdx;
        }
        else
        {
            // This was the start of a chain -- update periphMap.
            uint32 idx = (newEntry->prevIdx & ~INVALID_DATA);
            spmiPmicArbInfo.periphMap[idx] = newEntry->nextIdx;
            SPMI_LOG_TRACE( "  New head ptr - periph: %x, idx: %x", idx, spmiPmicArbInfo.periphMap[idx]);
        }
    }

    // Extend the chain to new node
    entry->nextIdx = chan;

    newEntry->sid = sid;
    newEntry->prevIdx = newRoot ? (periph | INVALID_DATA) : chanIdx;
    newEntry->nextIdx = INVALID_DATA;

    return SPMI_SUCCESS;
}

void PmicArb_GetHwStatus(PmicArbHwStatus* status)
{
    if(status == NULL) {
        return;
    }
    
    status->irqStatus = SPMI_HWIO_IN( HWIO_SPMI_PROTOCOL_IRQ_STATUS_ADDR );
    status->geniStatus = SPMI_HWIO_IN( HWIO_SPMI_GENI_STATUS_ADDR );
    status->geniCtrl = SPMI_HWIO_IN( HWIO_SPMI_GENI_CTRL_ADDR );
}

boolean PmicArb_HasInterruptSupport()
{
    return spmiPmicArbInfo.hasInterruptSupport;
}

uint32 PmicArb_GetMaxChannelSupport()
{
    return PMIC_ARB_MAX_PERIPHERAL_SUPPORT;
}

Spmi_Result PmicArb_FindChannel(uint8 slaveId, uint8 periph, uint8* channel)
{
    int16 idx = spmiPmicArbInfo.periphMap[periph];

    while (idx != INVALID_DATA)
    {
        ChannelMapEntry* entry = &spmiPmicArbInfo.channelMap[idx];

        if (entry->sid == slaveId) {
            *channel = idx;
            SPMI_LOG_TRACE( "Found chan: sid %d, periph: 0x%x = %d", slaveId, periph, idx );
            return SPMI_SUCCESS;
        }

        idx = entry->nextIdx;
    }
    
    // This peripheral is not supported
    return SPMI_FAILURE_CHANNEL_NOT_FOUND;
}

Spmi_Result PmicArb_SetInterruptEnabled(uint8 channel, boolean enabled)
{
    if(!spmiPmicArbInfo.hasInterruptSupport) {
        return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
    }
    
    if(!isInterruptOwner( channel )) {
        return SPMI_FAILURE_COMMAND_NOT_ALLOWED;
    }
    
	if(channel > HWIO_SPMI_PIC_ACC_ENABLEn_MAXn){
       return SPMI_FAILURE_INVALID_CHAN_NUM;
    }
	
    SPMI_HWIO_OUT_FIELD( HWIO_SPMI_PIC_ACC_ENABLEn_ADDR( channel ), HWIO_SPMI_PIC_ACC_ENABLEn_INT_ACC_ENABLE, enabled );
    
    return SPMI_SUCCESS;
}

Spmi_Result PmicArb_ExecuteCommand(PmicArbCmd cmd,
                                   Spmi_AccessPriority priority,
                                   uint8 slaveId,
                                   uint16 address,
                                   uint8* data,
                                   uint32 dataLen,
                                   uint32* bytesTransacted)
{
    uint32 tmp;
    uint8 channel;
    boolean doRead = FALSE;
    boolean doWrite = FALSE;
    
    if(slaveId > SPMI_MAX_SLAVE_ID) {
        return SPMI_FAILURE_INVALID_SLAVE_ID;
    }
    
    if(address > SPMI_MAX_REG_ADDRESS) {
        return SPMI_FAILURE_INVALID_ADDRESS;
    }
            
    if(data == NULL || dataLen == 0) {
        return SPMI_FAILURE_INVALID_BUFFER;
    }
    
    if(bytesTransacted == NULL) {
        bytesTransacted = &tmp;
    }

    *bytesTransacted = 0;    
    
    if(PmicArb_FindChannel( slaveId, REG_ADDR_TO_PERIPH_ID( address ), &channel ) != SPMI_SUCCESS) {
        return SPMI_FAILURE_CHANNEL_NOT_FOUND;
    }
    
    switch(cmd)
    {
        case PMIC_ARB_CMD_EXTENDED_REG_READ_LONG:
            doRead = TRUE;
            break;
            
        case PMIC_ARB_CMD_EXTENDED_REG_WRITE_LONG:
            doWrite = TRUE;
            break;

        default:
            return SPMI_FAILURE_COMMAND_NOT_SUPPORTED;
    }
    
    while(dataLen > 0)
    {
        Spmi_Result result;
        
        // Check if the Done bit is set. If not, the previous transaction is stuck
        if(!isTransactionDone( doRead, channel )) {
            return SPMI_FAILURE_BUS_BUSY;
        }
    
        uint32 len = dataLen > MAX_TRANSACTION_LEN ? MAX_TRANSACTION_LEN : dataLen;
        
        if(doWrite) {
            writeSpmiData( channel, data + *bytesTransacted, len );
        }
        
        // This is the signal to start the transaction
        // len - 1 because value of 0 means 1 byte and increments from there
        writeCommand( doRead, cmd, channel, priority, REG_ADDR_TO_REG_OFFSET( address ) + *bytesTransacted, len - 1 );
        
        if((result = waitForStatusDone( doRead, channel )) != SPMI_SUCCESS) {
            return result;
        }
        
        if(doRead) {
            readSpmiData( channel, data + *bytesTransacted, len );
        }
        
        dataLen -= len;
        *bytesTransacted += len;
    }
    
    return SPMI_SUCCESS;
}
