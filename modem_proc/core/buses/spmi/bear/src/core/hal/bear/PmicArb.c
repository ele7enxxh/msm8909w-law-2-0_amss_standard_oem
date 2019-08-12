/**
 * @file:  PmicArb.c
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/12/13 07:59:23 $
 * $Header: //components/rel/core.mpss/3.5.c12.3/buses/spmi/bear/src/core/hal/bear/PmicArb.c#1 $
 * $Change: 11985146 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 25/2/15  To fix the compiler warning
 * 12/5/14  Added check to limit the 224 channels for 8952
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

// Max number of bytes we can read/write at a time.
#define MAX_TRANSACTION_LEN 8

typedef struct {
    int8 sid;
    int16 prevIdx;
    int16 nextIdx;
} ChannelMapEntry;

typedef struct {
    uint8 owner;
    boolean hasInterruptSupport;
    PmicArbIsrCallback isrCallback;
    SpmiOs_ReservedChannels* reservedChans[SWIO_MAX_BUSES_SUPPORTED];
    int16 periphMap[SWIO_MAX_BUSES_SUPPORTED][SPMI_MAX_PERIPH_ID + 1];
    ChannelMapEntry channelMap[SWIO_MAX_BUSES_SUPPORTED][SWIO_MAX_CHANNELS_SUPPORTED];
} PmicArbInfo;

//******************************************************************************
// Global Data
//******************************************************************************

PmicArbInfo spmiPmicArbInfo = {0};

//******************************************************************************
// Local Helper Functions
//******************************************************************************

static boolean isInterruptOwner(uint8 bid, uint8 channel)
{
    return spmiPmicArbInfo.owner == SPMI_SWIO_IN_FIELD( bid,
                                                        SWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( bid, channel ),
                                                        SWIO_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER );
}

static void writeSpmiData(uint8 bid, uint8 channel, uint8* data, uint32 bytesToWrite)
{
    uint32 i;
    uint32 tmp = 0;
    
    for(i = 0; i < sizeof(uint32) && bytesToWrite > 0; i++, bytesToWrite--) {
        ((uint8*)&tmp)[i] = data[i];
    }
    
    SPMI_HWIO_OUT( SWIO_PMIC_ARBq_CHNLn_WDATA0_ADDR( bid, spmiPmicArbInfo.owner, channel ), tmp );
    
    if(bytesToWrite > 0)
    {
        tmp = 0;
        for(i = 0; i < sizeof(uint32) && bytesToWrite > 0; i++, bytesToWrite--) {
            ((uint8*)&tmp)[i] = data[i + sizeof(uint32)];
        }
        
        SPMI_HWIO_OUT( SWIO_PMIC_ARBq_CHNLn_WDATA1_ADDR( bid, spmiPmicArbInfo.owner, channel ), tmp );
    }
}

static void readSpmiData(uint8 bid, uint8 channel, uint8* data, uint32 bytesToRead)
{
    uint32 i;
    uint32 spmiData = SPMI_HWIO_IN( SWIO_PMIC_ARB_OBSq_CHNLn_RDATA0_ADDR( bid, spmiPmicArbInfo.owner, channel ) );
        
    for(i = 0; i < sizeof(uint32) && bytesToRead > 0; i++, bytesToRead--) {
        data[i] = ((uint8*)&spmiData)[i];
    }

    if(bytesToRead > 0)
    {
        spmiData = SPMI_HWIO_IN( SWIO_PMIC_ARB_OBSq_CHNLn_RDATA1_ADDR( bid, spmiPmicArbInfo.owner, channel ) );

        for(i = 0; i < sizeof(uint32) && bytesToRead > 0; i++, bytesToRead--) {
            data[i + sizeof(uint32)] = ((uint8*)&spmiData)[i];
        }
    }
}

static Spmi_Result waitForStatusDone(uint8 bid, boolean isObserver, uint8 channel)
{
    uint32 status = 0;
    uint32 timeout = SPMI_TIMEOUT_USEC;
    Spmi_Result result = SPMI_SUCCESS;
    
    while(status == 0 && timeout-- > 0)
    {
        SpmiOs_Wait( 1 );
        status = isObserver ? SPMI_HWIO_IN( SWIO_PMIC_ARB_OBSq_CHNLn_STATUS_ADDR( bid, spmiPmicArbInfo.owner, channel ) ) :
                              SPMI_HWIO_IN( SWIO_PMIC_ARBq_CHNLn_STATUS_ADDR( bid, spmiPmicArbInfo.owner, channel ) );
    }
    
    if(status & SWIO_PMIC_ARBq_CHNLn_STATUS_DROPPED_BMSK( bid )) 
    {
        result = SPMI_FAILURE_TRANSACTION_DROPPED;
    }
    else if(status & SWIO_PMIC_ARBq_CHNLn_STATUS_DONE_BMSK( bid ))
    {
        if(status & SWIO_PMIC_ARBq_CHNLn_STATUS_FAILURE_BMSK( bid )) {
            result = SPMI_FAILURE_TRANSACTION_FAILED;
        }
        else if(status & SWIO_PMIC_ARBq_CHNLn_STATUS_DENIED_BMSK( bid )) {
            result = SPMI_FAILURE_TRANSACTION_DENIED;
        }
    }
    else if(SPMI_SWIO_IN_FIELD( bid, SWIO_SPMI_GENI_CTRL_ADDR( bid ), SWIO_SPMI_GENI_CTRL_M_GENI_DISABLE )) {
        result = SPMI_FAILURE_BUS_DISABLED;
    }
    else {
        result = SPMI_FAILURE_TRANSACTION_TIMEOUT;
    }

    return result;
}

static __inline boolean isTransactionDone(uint8 bid, boolean isObserver, uint8 channel)
{
    if(isObserver) {
        return SPMI_SWIO_IN_FIELD( bid,
                                   SWIO_PMIC_ARB_OBSq_CHNLn_STATUS_ADDR( bid, spmiPmicArbInfo.owner, channel ),
                                   SWIO_PMIC_ARB_OBSq_CHNLn_STATUS_DONE );
    } else {
        return SPMI_SWIO_IN_FIELD( bid,
                                   SWIO_PMIC_ARBq_CHNLn_STATUS_ADDR( bid, spmiPmicArbInfo.owner, channel ),
                                   SWIO_PMIC_ARBq_CHNLn_STATUS_DONE );
    }
}

static __inline void writeCommand(uint8 bid, 
                                  boolean isObserver,
                                  PmicArbCmd cmd,
                                  uint8 channel,
                                  Spmi_AccessPriority priority,
                                  uint8 regOffset,
                                  uint32 len)
{
    if(isObserver) 
    {
        SPMI_HWIO_OUT( SWIO_PMIC_ARB_OBSq_CHNLn_CMD_ADDR( bid, spmiPmicArbInfo.owner, channel ),
                      (cmd                      << SWIO_PMIC_ARB_OBSq_CHNLn_CMD_OPCODE_SHFT(bid))   |
                      (SPMI_ACCESS_PRIORITY_LOW << SWIO_PMIC_ARB_OBSq_CHNLn_CMD_PRIORITY_SHFT(bid)) |
                      (regOffset                << SWIO_PMIC_ARB_OBSq_CHNLn_CMD_ADDRESS_SHFT(bid))  |
                      (len                      << SWIO_PMIC_ARB_OBSq_CHNLn_CMD_BYTE_CNT_SHFT(bid)) );
    }
    else 
    {
        SPMI_HWIO_OUT( SWIO_PMIC_ARBq_CHNLn_CMD_ADDR( bid, spmiPmicArbInfo.owner, channel ),
                      (cmd       << SWIO_PMIC_ARBq_CHNLn_CMD_OPCODE_SHFT(bid))   |
                      (priority  << SWIO_PMIC_ARBq_CHNLn_CMD_PRIORITY_SHFT(bid)) |
                      (regOffset << SWIO_PMIC_ARBq_CHNLn_CMD_ADDRESS_SHFT(bid))  |
                      (len       << SWIO_PMIC_ARBq_CHNLn_CMD_BYTE_CNT_SHFT(bid)) );
    }
}

static void* pmicArbIsr(void* ctx)
{
    uint32 i, k;
    uint8 bid = (uint8)(unsigned)ctx;
    
    for(i = 0; i <= SWIO_SPMI_PIC_OWNERm_ACC_STATUSn_MAXn( bid ); i++) 
    {
        // Each bit in these registers represents a peripheral
        uint32 accStatus = SPMI_HWIO_IN( SWIO_SPMI_PIC_OWNERm_ACC_STATUSn_ADDR( bid, spmiPmicArbInfo.owner, i ) );
        
        for(k = 0; (k < 32) && (accStatus != 0); k++) 
        {
            if((accStatus >> k) & 0x1) 
            {
                // This peripheral (channel) triggered the interrupt.  Check its individual mask
                uint8 channel = (i * 32) + k;
                uint8 mask = SPMI_SWIO_IN_FIELD( bid, 
                                                 SWIO_SPMI_PIC_IRQ_STATUSn_ADDR( bid, channel ), 
                                                 SWIO_SPMI_PIC_IRQ_STATUSn_INT_STATUS );
                
                // Clear the status
                SPMI_SWIO_OUT_FIELD( bid, 
                                     SWIO_SPMI_PIC_IRQ_CLEARn_ADDR( bid, channel ),
                                     SWIO_SPMI_PIC_IRQ_CLEARn_INT_CLEAR, mask );
                
                // Notify the higher layers
                if(spmiPmicArbInfo.isrCallback != NULL) {
                    spmiPmicArbInfo.isrCallback( bid, channel, mask );
                }
            }
        }
    }
    
    return NULL;
}

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result PmicArb_Init(PmicArbIsrCallback isrCb)
{
    uint8 bid;
    Spmi_Result rslt;

    spmiPmicArbInfo.isrCallback = isrCb;
    
    if((rslt = SpmiHal_Init()) != SPMI_SUCCESS) {
        return rslt;
    }
    
    if((rslt = SpmiOs_GetOwnerId( &spmiPmicArbInfo.owner )) != SPMI_SUCCESS) {
        return rslt;
    }

    spmiPmicArbInfo.hasInterruptSupport = FALSE;
    
    for(bid = 0; bid < SWIO_MAX_BUSES_SUPPORTED; bid++)
    {
        spmiPmicArbInfo.reservedChans[bid] = SpmiOs_GetReservedChannels( bid );
                
        if((rslt = SpmiOs_RegisterISR( pmicArbIsr, bid )) == SPMI_SUCCESS) {
            spmiPmicArbInfo.hasInterruptSupport = TRUE;
        }
    }
    
    // Cache all ppid -> channel mappings
    PmicArb_UpdateCache();
    
    return SPMI_SUCCESS;
}

Spmi_Result PmicArb_InvalidateCache()
{
    uint32 i, k;
    
    for(i = 0; i < SWIO_MAX_BUSES_SUPPORTED; i++) 
    {
        for(k = 0; k <= SPMI_MAX_PERIPH_ID; k++) {
            spmiPmicArbInfo.periphMap[i][k] = INVALID_DATA;
        }

        for(k = 0; k < SWIO_MAX_CHANNELS_SUPPORTED; k++) {
            spmiPmicArbInfo.channelMap[i][k].sid = INVALID_SID;
            spmiPmicArbInfo.channelMap[i][k].nextIdx = INVALID_DATA;
            spmiPmicArbInfo.channelMap[i][k].prevIdx = INVALID_DATA;
        }
    }
    
    return SPMI_SUCCESS;
}

Spmi_Result PmicArb_UpdateCache()
{
    uint32 bid, chan;
    PmicArb_InvalidateCache();
    
    for(bid = 0; bid < SWIO_MAX_BUSES_SUPPORTED; bid++) 
    {
        for(chan = 0; chan < SWIO_MAX_CHANNELS_SUPPORTED; chan++) 
        {
            uint32 regChan = SPMI_HWIO_IN( SWIO_PMIC_ARB_REG_CHNLn_ADDR( bid, chan ) );
            uint8 sid = SPMI_SWIO_GET_FIELD_VALUE( bid, regChan, SWIO_PMIC_ARB_REG_CHNLn_SID );
            uint8 addr = SPMI_SWIO_GET_FIELD_VALUE( bid, regChan, SWIO_PMIC_ARB_REG_CHNLn_ADDRESS );

            PmicArb_AddCacheEntry( bid, chan, sid, addr );
        }
    }
    
    return SPMI_SUCCESS;
}

Spmi_Result PmicArb_AddCacheEntry(uint8 bid, uint32 chan, uint8 sid, uint8 periph)
{
    int16 chanIdx;
    boolean newRoot = FALSE;
    ChannelMapEntry* entry;
    ChannelMapEntry* newEntry;
    SpmiOs_ReservedChannels* reserved = spmiPmicArbInfo.reservedChans[bid];
    
    int16* pMap = spmiPmicArbInfo.periphMap[bid];
    ChannelMapEntry* cMap = spmiPmicArbInfo.channelMap[bid];
        
    if(chan >= SWIO_MAX_CHANNELS_SUPPORTED) {
        return SPMI_FAILURE_INVALID_CHAN_NUM;
    }
    
    if(reserved != NULL)
    {
        uint32 i;
        
        for(i = 0; i < reserved->numReserved; i++) 
        {
            if(chan == reserved->reserved[i]) {
                SPMI_LOG_VERBOSE( "Not caching reserved channel: %d", chan);
                return SPMI_FAILURE_CHANNEL_RESERVED;
            }
        }
    }
    
    newEntry = &cMap[chan];
    
    SPMI_LOG_TRACE( "Add to cache - bid: %d, chan %d: sid: %d, periph: %d (0x%x)",
                    bid, chan, sid, periph, periph );

    if(pMap[periph] == INVALID_DATA)
    {
        SPMI_LOG_TRACE("  first hit empty");
        newRoot = TRUE;
        pMap[periph] = chan;
    }
    
    // 1st entry
    chanIdx = pMap[periph];
    entry = &cMap[chanIdx];

    // If this isn't the start of a new chain, follow the existing one
    if(!newRoot)
    {
        int16 tmpIdx = chanIdx;
        
        // go till the end of the chain or we hit the replacement
        do
        {
            chanIdx = tmpIdx;
            entry = &cMap[chanIdx];
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
            cMap[newEntry->nextIdx].prevIdx = newEntry->prevIdx;
        }
        
        if((newEntry->prevIdx & INVALID_DATA) == 0) {
            cMap[newEntry->prevIdx].nextIdx = newEntry->nextIdx;
        }
        else
        {
            // This was the start of a chain -- update periphMap.
            uint32 idx = (newEntry->prevIdx & ~INVALID_DATA);
            pMap[idx] = newEntry->nextIdx;
            SPMI_LOG_TRACE( "  New head ptr - periph: %x, idx: %x", idx, pMap[idx]);
        }
    }

    // Extend the chain to new node
    entry->nextIdx = chan;

    newEntry->sid = sid;
    newEntry->prevIdx = newRoot ? (periph | INVALID_DATA) : chanIdx;
    newEntry->nextIdx = INVALID_DATA;

    return SPMI_SUCCESS;
}

void PmicArb_GetHwStatus(uint8 bid, PmicArbHwStatus* status)
{
    if(status == NULL) {
        return;
    }
    
    status->irqStatus = SPMI_HWIO_IN( SWIO_SPMI_PROTOCOL_IRQ_STATUS_ADDR( bid ) );
    status->geniStatus = SPMI_HWIO_IN( SWIO_SPMI_GENI_STATUS_ADDR( bid ) );
    status->geniCtrl = SPMI_HWIO_IN( SWIO_SPMI_GENI_CTRL_ADDR( bid ) );
}

boolean PmicArb_HasInterruptSupport()
{
    return spmiPmicArbInfo.hasInterruptSupport;
}

uint32 PmicArb_GetMaxChannelSupport()
{
    return SWIO_MAX_CHANNELS_SUPPORTED;
}

uint32 PmicArb_GetNumBusesSupported()
{
    return SWIO_MAX_BUSES_SUPPORTED;
}

Spmi_Result PmicArb_FindChannel(uint8 bid, uint8 slaveId, uint8 periph, uint8* channel)
{
    int16 idx;
    
    if(bid >= SWIO_MAX_BUSES_SUPPORTED) {
        return SPMI_FAILURE_INVALID_BUS_ID;
    }
    
    idx = spmiPmicArbInfo.periphMap[bid][periph];
    
    while (idx != INVALID_DATA)
    {
        ChannelMapEntry* entry = &spmiPmicArbInfo.channelMap[bid][idx];

        if (entry->sid == slaveId) {
            *channel = idx;
            SPMI_LOG_TRACE( "Found chan: bid: %d, sid %d, periph: 0x%x = %d", 
                            bid, slaveId, periph, idx );
            return SPMI_SUCCESS;
        }

        idx = entry->nextIdx;
    }
    
    SPMI_LOG_TRACE( "Chan not found: bid: %d, sid %d, periph: 0x%x",
                    bid, slaveId, periph );
    
    // This peripheral is not supported
    return SPMI_FAILURE_CHANNEL_NOT_FOUND;
}

Spmi_Result PmicArb_SetInterruptEnabled(uint8 bid, uint8 channel, boolean enabled)
{
    if(!spmiPmicArbInfo.hasInterruptSupport) {
        return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
    }
    
    if(!isInterruptOwner( bid, channel )) {
        return SPMI_FAILURE_COMMAND_NOT_ALLOWED;
    }
    /* two sets of parentheses around the constant to silece warning. */
    if( channel > ((SWIO_SPMI_PIC_ACC_ENABLEn_MAXn( bid ))) ) {
       return SPMI_FAILURE_INVALID_CHAN_NUM;
    }
    
    SPMI_LOG_VERBOSE( "ISR enabled: %d, Bus: %d, Chan: %d, ", enabled, bid, channel );
    
    SPMI_SWIO_OUT_FIELD( bid,
                         SWIO_SPMI_PIC_ACC_ENABLEn_ADDR( bid, channel ), 
                         SWIO_SPMI_PIC_ACC_ENABLEn_INT_ACC_ENABLE, 
                         enabled );
    
    return SPMI_SUCCESS;
}

Spmi_Result PmicArb_ExecuteCommand(PmicArbCmd cmd,
                                   Spmi_AccessPriority priority,
                                   uint8 bid,
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
    
    if(bid >= SWIO_MAX_BUSES_SUPPORTED) {
        return SPMI_FAILURE_INVALID_SLAVE_ID;
    }
    
    if(slaveId > SPMI_MAX_SLAVE_ID) {
        return SPMI_FAILURE_INVALID_SLAVE_ID;
    }
         
    if(data == NULL || dataLen == 0) {
        return SPMI_FAILURE_INVALID_BUFFER;
    }
    
    if(bytesTransacted == NULL) {
        bytesTransacted = &tmp;
    }

    *bytesTransacted = 0;    
    
    if(PmicArb_FindChannel( bid, slaveId, REG_ADDR_TO_PERIPH_ID( address ), &channel ) != SPMI_SUCCESS) {
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
        if(!isTransactionDone( bid, doRead, channel )) {
            return SPMI_FAILURE_BUS_BUSY;
        }
    
        uint32 len = dataLen > MAX_TRANSACTION_LEN ? MAX_TRANSACTION_LEN : dataLen;
        
        if(doWrite) {
            writeSpmiData( bid, channel, data + *bytesTransacted, len );
        }
        
        // This is the signal to start the transaction
        // len - 1 because value of 0 means 1 byte and increments from there
        writeCommand( bid, doRead, cmd, channel, priority, REG_ADDR_TO_REG_OFFSET( address ) + *bytesTransacted, len - 1 );
        
        if((result = waitForStatusDone( bid, doRead, channel )) != SPMI_SUCCESS) {
            return result;
        }
        
        if(doRead) {
            readSpmiData( bid, channel, data + *bytesTransacted, len );
        }
        
        dataLen -= len;
        *bytesTransacted += len;
    }
    
    return SPMI_SUCCESS;
}
