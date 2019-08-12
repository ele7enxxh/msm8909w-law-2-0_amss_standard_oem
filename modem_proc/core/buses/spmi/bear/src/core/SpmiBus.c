/**
 * @file:  SpmiBus.c
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/12/13 07:59:23 $
 * $Header: //components/rel/core.mpss/3.5.c12.3/buses/spmi/bear/src/core/SpmiBus.c#1 $
 * $Change: 11985146 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 1/20/15  Multiple bus support
 * 11/3/14  Automatic channel assignment
 * 6/19/14  Log Fatal if bus is busy
 * 05/8/14  To check the clock status for every transaction failure reason
 * 10/1/13  Initial Version
 */

#include "SpmiBus.h"
#include "SpmiOs.h"
#include "SpmiLogs.h"
#include "PmicArb.h"

//******************************************************************************
// Macros / Definitions
//******************************************************************************

#define NUM_DEBUG_TRANSACTIONS 10
#define NUM_DEBUG_FAILURES 5

#define INIT_CHECK() if(!spmiBusInfo.initialized) { return SPMI_FAILURE_NOT_INITIALIZED; }

typedef enum {
    NOT_SET,
    STARTED,
    FINISHED,
} BusDbgState;

typedef struct {
    uint8 vsid;
    uint16 pid;
    uint8 data0;
    Spmi_Result result;
    PmicArbCmd cmd;
    uint8 dataLen;
    uint64 startTime;
    uint32 duration;
    uint8 tries;
    BusDbgState state;
} BusDbgInfo;

typedef struct {
    BusDbgInfo info;
    PmicArbHwStatus hwStatus;
} BusErrDbgInfo;

typedef struct {
    uint8 busId;
    uint8 channel;
    uint8 mask;
    uint64 startTime;
    uint32 duration;
} IsrDbgInfo;

typedef struct
{
    uint8 busIdx;
    uint8 errorIdx;
    uint8 isrIdx;
    BusDbgInfo busTrans[NUM_DEBUG_TRANSACTIONS];
    BusErrDbgInfo errorTrans[NUM_DEBUG_FAILURES];
    IsrDbgInfo isrTrans[NUM_DEBUG_TRANSACTIONS];
    uint16 rsltCnt[SPMI_RESULT_MAX];
    boolean syncFlag;
} SpmiBusDebugInfo;

typedef struct _SpmiBusIsrConfig
{
    SpmiIsr isr;
    const void* ctx;
    uint8 bid;
    uint8 chan;
    uint8 mask;
    uint32 id;
    struct _SpmiBusIsrConfig* next;
} SpmiBusIsrConfig;

typedef struct 
{
    boolean initialized;
    uint32 uid;
    uint32 maxChannels;
    uint32 numBuses;
    SpmiBusIsrConfig*** isrTable;
    SpmiBusDebugInfo debug;
} SpmiBusInfo;

//******************************************************************************
// Global Data
//******************************************************************************

SpmiBusInfo spmiBusInfo = {0};
boolean freeIsrNodes = FALSE;

//******************************************************************************
// Local Helper Functions
//******************************************************************************

static inline void debugTransStart(PmicArbCmd cmd, uint8 vSlaveId, uint16 address, uint32 len)
{
    SPMI_LOG_VERBOSE( "cmd %d / vsid: 0x%02x / addr: %x / len: %d", cmd, vSlaveId, address, len );
    
    spmiBusInfo.debug.busIdx = (spmiBusInfo.debug.busIdx + 1) % NUM_DEBUG_TRANSACTIONS;
    spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].state = STARTED;
    spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].cmd = cmd;
    spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].vsid = vSlaveId;
    spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].pid = address;
    spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].dataLen = len;
    spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].tries = 0;
    spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].startTime = SpmiOs_GetTimeTick();
}

static inline void debugTransEnd(PmicArbCmd cmd, uint8 vSlaveId, uint16 address, uint32 len, 
                                 uint8* data, uint8 tries, Spmi_Result rslt)
{
    if(spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].cmd == cmd &&
       spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].vsid == vSlaveId &&
       spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].pid == address &&
       spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].dataLen == len) 
    {
        spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].state = FINISHED;
        spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].result = rslt;
        spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].tries = tries;
        spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].duration = 
        SpmiOs_GetTimeTick() - spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].startTime;
        
        if(data != NULL) {
            spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].data0 = *data;
        }

        if(rslt < SPMI_RESULT_MAX) {
            spmiBusInfo.debug.rsltCnt[rslt]++;
        }
    }
    else {
        spmiBusInfo.debug.syncFlag = TRUE;
    }
    
    if(rslt != SPMI_SUCCESS)
    {
        uint8 bid;
        spmiDecodeVSid( vSlaveId, &bid, NULL );
                
        spmiBusInfo.debug.errorIdx = (spmiBusInfo.debug.errorIdx + 1) % NUM_DEBUG_FAILURES;
        spmiBusInfo.debug.errorTrans[spmiBusInfo.debug.errorIdx].info = spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx];
        
        PmicArb_GetHwStatus( bid, &spmiBusInfo.debug.errorTrans[spmiBusInfo.debug.errorIdx].hwStatus );
        SPMI_LOG_WARNING( "cmd: %d / vsid: 0x%02x / addr: 0x%04x / len: %d / rslt: %d / tries: %d", 
                          cmd, vSlaveId, address, len, rslt,
                          spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].tries);
    }
}

static Spmi_Result debugExecute(PmicArbCmd cmd,
                                Spmi_AccessPriority priority,
                                uint8 vSlaveId,
                                uint16 address,
                                uint8* data,
                                uint32 len,
                                uint32* bytesTransacted)
{
    Spmi_Result rslt;
    boolean retry;
    uint8 slaveId;
    uint8 busId;
    uint8 tries = 0;
    
    INIT_CHECK();
    
    debugTransStart( cmd, vSlaveId, address, len );
    
    spmiDecodeVSid( vSlaveId, &busId, &slaveId );

    do 
    {
        tries++;
        retry = FALSE;        

        rslt = PmicArb_ExecuteCommand( cmd, priority, busId, slaveId, address, data, len, bytesTransacted );

        // Adjust result if a more specific root cause can be found
        if(rslt != SPMI_SUCCESS) 
        {
            if(SpmiOs_GetAhbClkState( busId ) == SPMI_CLK_OFF) {
                rslt = SPMI_FAILURE_AHB_CLK_IS_OFF;
            }
            else if(SpmiOs_GetSerialClkState( busId ) == SPMI_CLK_OFF) {
                rslt = SPMI_FAILURE_SERIAL_CLK_IS_OFF;
            }

            retry = SpmiOs_HandleTransactionError( &rslt, cmd, busId, slaveId, address, tries );
        }

    } while(retry);

    debugTransEnd( cmd, vSlaveId, address, len, data, tries, rslt );
    
    // Fail-fast by catching some irrecoverable errors
    if(rslt == SPMI_FAILURE_TRANSACTION_TIMEOUT) {
        SPMI_LOG_FATAL( "Timeout while executing SPMI transaction" );
    }
    else if(rslt == SPMI_FAILURE_BUS_BUSY) {
        SPMI_LOG_FATAL( "SPMI bus busy" );
    }
    
    return rslt;
}

static void myIsr(uint8 busId, uint8 channel, uint8 mask)
{
    uint64 tick = SpmiOs_GetTimeTick();
	uint64 Final_tick=0;
    
    spmiBusInfo.debug.isrIdx = (spmiBusInfo.debug.isrIdx + 1) % NUM_DEBUG_TRANSACTIONS;
    spmiBusInfo.debug.isrTrans[spmiBusInfo.debug.isrIdx].busId = busId;
    spmiBusInfo.debug.isrTrans[spmiBusInfo.debug.isrIdx].channel = channel;
    spmiBusInfo.debug.isrTrans[spmiBusInfo.debug.isrIdx].mask = mask;
    spmiBusInfo.debug.isrTrans[spmiBusInfo.debug.isrIdx].startTime = tick;
    spmiBusInfo.debug.isrTrans[spmiBusInfo.debug.isrIdx].duration = 0;
    
    // This should never happen since we control the input to this
    if(channel > spmiBusInfo.maxChannels) {
        SPMI_LOG_ERROR( "ISR - Invalid channel: %d", channel );
        return;
    }
    
    SPMI_LOG_VERBOSE( "Bus %d ISR on chan %d", busId, channel );
    
    SpmiBusIsrConfig* userIsr = spmiBusInfo.isrTable[busId][channel];
    
    while(userIsr != NULL)
    {
        if(userIsr->isr != NULL) 
        {
            // Check if they want this particular interrupt
            uint8 userMask = mask & userIsr->mask;
            
            if(userMask)
            {
                SPMI_LOG_VERBOSE( "Calling user ISR with mask %x", userMask );

                uint64 usertick = SpmiOs_GetTimeTick();

                userIsr->isr( (void*)userIsr->ctx, userMask );
				Final_tick=SpmiOs_GetTimeTick() - usertick ;
                SPMI_LOG_VERBOSE( "User ISR took %llx ticks", Final_tick );
            }
            else {
                SPMI_LOG_VERBOSE( "Bus %d ISR (%p) registered for channel %d (user mask %x), but not with this mask: %x ", 
                                  busId, userIsr->isr, channel, userIsr->mask, mask );
            }
        }
        
        userIsr = userIsr->next;
    }
    
    spmiBusInfo.debug.isrTrans[spmiBusInfo.debug.isrIdx].duration = SpmiOs_GetTimeTick() - tick;
}

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result SpmiBus_Init()
{
    Spmi_Result rslt;
    
    if(spmiBusInfo.initialized) {
        return PmicArb_UpdateCache();
    }
    
    SPMI_LOG_INIT();
    
    if((rslt = PmicArb_Init( myIsr )) != SPMI_SUCCESS) {
        return rslt;
    }
    
    spmiBusInfo.maxChannels = PmicArb_GetMaxChannelSupport();
    spmiBusInfo.numBuses = PmicArb_GetNumBusesSupported();

    if(PmicArb_HasInterruptSupport())
    {
        if((rslt = SpmiOs_Malloc( spmiBusInfo.numBuses * sizeof(SpmiBusIsrConfig*),
                                  (void**) &spmiBusInfo.isrTable )) != SPMI_SUCCESS) {
            return rslt;
        }
        
        SPMI_LOG_VERBOSE( "ISR table size: %d", PMIC_ARB_MAX_PERIPHERAL_SUPPORT * sizeof(SpmiBusIsrConfig*) );
    }
    
    spmiBusInfo.initialized = TRUE;
    
    return rslt;
}

Spmi_Result SpmiBus_DeInit()
{
    if(spmiBusInfo.isrTable != NULL) 
    {
        uint32 i, k;

        for(i = 0; i < spmiBusInfo.numBuses; i++)
        {
            SpmiBusIsrConfig** busISRs = spmiBusInfo.isrTable[i];
            
            if(busISRs != NULL)
            {
                for(k = 0; k < spmiBusInfo.maxChannels; k++)
                {
                    SpmiBusIsrConfig* curr = busISRs[k];
                    while(curr != NULL) 
                    {
                        SpmiBusIsrConfig* t = curr->next;
                        SpmiOs_Free( curr );
                        curr = t;
                    }
                }
                
                SpmiOs_Free( busISRs );
            }
        }
        
        SpmiOs_Free( spmiBusInfo.isrTable );
        spmiBusInfo.isrTable = NULL;
    }
    
    spmiBusInfo.initialized = FALSE;
    return SPMI_SUCCESS;
}

Spmi_Result SpmiBus_ReadLong(uint32 vSlaveId,
                             Spmi_AccessPriority priority,
                             uint32 address,
                             uint8* data,
                             uint32 len,
                             uint32* bytesRead)
{
    return debugExecute( PMIC_ARB_CMD_EXTENDED_REG_READ_LONG, priority, vSlaveId, address, data, len, bytesRead );
}

Spmi_Result SpmiBus_WriteLong(uint32 vSlaveId,
                              Spmi_AccessPriority priority,
                              uint32 address,
                              uint8* data,
                              uint32 len)
{
    return debugExecute( PMIC_ARB_CMD_EXTENDED_REG_WRITE_LONG, priority, vSlaveId, address, data, len, NULL );
}

Spmi_Result SpmiBus_Command(uint32 vSlaveId, Spmi_AccessPriority priority, Spmi_Command cmd)
{
    PmicArbCmd pmicArbCmd;
    
    switch(cmd)
    {
        case SPMI_COMMAND_RESET:    pmicArbCmd = PMIC_ARB_CMD_RESET; break;
        case SPMI_COMMAND_SLEEP:    pmicArbCmd = PMIC_ARB_CMD_SLEEP; break;
        case SPMI_COMMAND_SHUTDOWN: pmicArbCmd = PMIC_ARB_CMD_SHUTDOWN; break;
        case SPMI_COMMAND_WAKEUP:   pmicArbCmd = PMIC_ARB_CMD_WAKEUP; break;
        default:
            return SPMI_FAILURE_COMMAND_NOT_SUPPORTED;
    }
    
    return debugExecute( pmicArbCmd, priority, vSlaveId, 0, NULL, 0, NULL );
}

Spmi_Result SpmiBus_RegisterIsr(uint8 vSlaveId,
                                uint8 periph,
                                uint8 mask,
                                const SpmiIsr isr,
                                const void* ctx,
                                SpmiToken* token)
{
    INIT_CHECK();
    
    uint8 channel;
    Spmi_Result rslt;
    uint8 slaveId;
    uint8 busId;
    
    if(!PmicArb_HasInterruptSupport()) {
        return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
    }
    
    if(mask == 0 || isr == NULL || token == NULL) {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }
    
    spmiDecodeVSid( vSlaveId, &busId, &slaveId );
    
    if((rslt = PmicArb_FindChannel( busId, slaveId, periph, &channel )) != SPMI_SUCCESS) {
        return rslt;
    }
    
    if(channel > spmiBusInfo.maxChannels) {
        SPMI_LOG_WARNING( "Couldn't get channel to set ISR. vSlaveId: %d, periph %d, channel %d", vSlaveId, periph, channel );
        return SPMI_FAILURE_CHANNEL_NOT_FOUND;
    }
    
    SpmiBusIsrConfig*** busISRs = &spmiBusInfo.isrTable[busId];
    
    if(*busISRs == NULL)
    {
        SPMI_LOG_VERBOSE( "Allocating buf for bus #%d ISRs", busId );
        if((rslt = SpmiOs_Malloc( spmiBusInfo.maxChannels * sizeof(SpmiBusIsrConfig*),
                                  (void**) busISRs )) != SPMI_SUCCESS) {
            return rslt;
        }
    }
    
    SpmiBusIsrConfig** curr = &((*busISRs)[channel]);
        
    while(*curr != NULL && (*curr)->isr != NULL) 
    {
        curr = &(*curr)->next;
    }
    
    // If curr is null we are adding a node, if it is not null we are reusing a node that was 
    // unregistered, but not freed
    if(*curr == NULL)
    {
        SPMI_LOG_VERBOSE( "Registering new ISR node (bid: %d, chan: %d, mask: %d)", busId, channel, mask );
        if ((rslt = SpmiOs_Malloc( sizeof(SpmiBusIsrConfig), (void**) curr )) != SPMI_SUCCESS) {
            return rslt;
        }
    }
    else {
        SPMI_LOG_VERBOSE( "Registering existing ISR node (bid: %d, chan: %d, mask: %d)", busId, channel, mask );
    }
        
    (*curr)->isr = isr;
    (*curr)->ctx = ctx;
    (*curr)->bid = busId;
    (*curr)->chan = channel;
    (*curr)->mask = mask;
    (*curr)->id = ++spmiBusInfo.uid;
    
    token->id = (*curr)->id;
    token->bid = busId;
    token->chan = channel;
    
    return PmicArb_SetInterruptEnabled( busId, channel, TRUE );
}

Spmi_Result SpmiBus_UnregisterIsr(SpmiToken* token, uint8 mask)
{
    INIT_CHECK();
    
    boolean keepEnabled = FALSE;
    Spmi_Result rslt = SPMI_FAILURE_INVALID_TOKEN;
        
    if(!PmicArb_HasInterruptSupport()) {
        return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
    }
    
    if(token == NULL) {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }

    if(token->chan >= spmiBusInfo.maxChannels) 
    {
        SPMI_LOG_ERROR( "Invalid channel: %d", token->chan );
        return SPMI_FAILURE_CHANNEL_NOT_FOUND;
    }
    
    if(token->bid >= spmiBusInfo.numBuses) 
    {
        SPMI_LOG_ERROR( "Invalid busId: %d", token->bid );
        return SPMI_FAILURE_INVALID_BUS_ID;
    }

    SpmiBusIsrConfig* curr = spmiBusInfo.isrTable[token->bid][token->chan];
    SpmiBusIsrConfig* prev = NULL;
    
    while(curr != NULL) 
    {
        if(curr->id == token->id) 
        {
            SPMI_LOG_VERBOSE( "Unregistering ISR (bid %d: chan: %d, mask: %d)", token->bid, token->chan, mask );
            rslt = SPMI_SUCCESS;
            
            // Turn off any interrupts for this node
            curr->mask &= ~mask;

            if(curr->mask == 0) 
            {    
                curr->isr = NULL;
                curr->ctx = NULL;
                curr->id = 0;
                
                // Depending on the usage of register/unregister it might be
                // more efficient to not free the node and just reuse it when needed
                if(freeIsrNodes) 
                {
                    if(prev == NULL) 
                    {
                        spmiBusInfo.isrTable[token->bid][token->chan] = curr->next;
                        SpmiOs_Free( curr );
                        curr = spmiBusInfo.isrTable[token->bid][token->chan];
                    }
                    else 
                    {
                        prev->next = curr->next;
                        SpmiOs_Free( curr );
                        curr = prev->next;
                    }
                    
                    // This is just to continue checking if we need to disable 
                    // the actual interrupt in the pmic arb. Otherwise we could quit.
                    continue; 
                }
            }
        }
        
        if(curr->bid == token->bid && 
           curr->chan == token->chan && 
           curr->isr != NULL) 
        {
            keepEnabled = TRUE;
        }

        prev = curr;
        curr = curr->next;
    }
    
    if(!keepEnabled) {
        return PmicArb_SetInterruptEnabled( token->bid, token->chan, FALSE );
    }
    
    return rslt;
}

Spmi_Result SpmiBus_ReadModifyWriteLongByte(uint32 vSlaveId,
                                            Spmi_AccessPriority priority,
                                            uint32 address,
                                            uint32 data,
                                            uint32 mask,
                                            uint8 *byteWritten)
{
    Spmi_Result rslt;
    uint8 readData;

    if((rslt = SpmiBus_ReadLong( vSlaveId, priority, address, &readData, 1, NULL )) != SPMI_SUCCESS) {
        return rslt;
    }

    readData &= ~mask;
    readData |= data & mask;

    if((rslt = SpmiBus_WriteLong( vSlaveId, priority, address, &readData, 1 )) != SPMI_SUCCESS) {
        return rslt;
    }

    *byteWritten = readData;

    return SPMI_SUCCESS;
}
