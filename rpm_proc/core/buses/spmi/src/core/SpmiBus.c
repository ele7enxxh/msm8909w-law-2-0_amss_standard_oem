/**
 * @file:  SpmiBus.c
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/08/27 05:22:47 $
 * $Header: //components/rel/rpm.bf/2.1.1/core/buses/spmi/src/core/SpmiBus.c#2 $
 * $Change: 8908394 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
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

// This could be a config item, if needed
#define FREE_UNUSED_NODES FALSE

#define INIT_CHECK() do { if(!spmiBusInfo.initialized) { return SPMI_FAILURE_NOT_INITIALIZED; } } while(FALSE)

typedef enum {
    NOT_SET,
    STARTED,
    FINISHED,
} BusDbgState;

typedef struct {
    uint8 sid;
    uint16 pid;
    uint8 data0;
    Spmi_Result result;
    PmicArbCmd cmd;
    uint8 dataLen;
    uint64 startTime;
    uint32 duration;
    BusDbgState state;
} BusDbgInfo;

typedef struct {
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
    BusDbgInfo errorTrans[NUM_DEBUG_FAILURES];
    IsrDbgInfo isrTrans[NUM_DEBUG_TRANSACTIONS];
    uint16 errorCnt[PMIC_ARB_CMD_MAX];
    PmicArbHwStatus hwStatus;
    boolean syncFlag;
} SpmiBusDebugInfo;

typedef struct _SpmiBusIsrConfig
{
    SpmiIsr isr;
    const void* ctx;
    uint8 mask;
    uint32 id;
    struct _SpmiBusIsrConfig* next;
} SpmiBusIsrConfig;

typedef struct 
{
    boolean initialized;
    uint32 uid;
    uint32 maxChannels;

    SpmiBusIsrConfig** isrTable;
    SpmiBusDebugInfo debug;
} SpmiBusInfo;

//******************************************************************************
// Global Data
//******************************************************************************

SpmiBusInfo spmiBusInfo = {0};

//******************************************************************************
// Local Helper Functions
//******************************************************************************

static void debugTransStart(PmicArbCmd cmd, uint8 slaveId, uint16 address, uint32 len)
{
    SPMI_LOG_INFO( "Execute - cmd %d / slave: %d / addr: %x / len: %d", cmd, slaveId, address, len );
    
    spmiBusInfo.debug.busIdx = (spmiBusInfo.debug.busIdx + 1) % NUM_DEBUG_TRANSACTIONS;
    spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].state = STARTED;
    spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].cmd = cmd;
    spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].sid = slaveId;
    spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].pid = address;
    spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].dataLen = len;
    spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].startTime = SpmiOs_GetTimeTick();
}

static void debugTransEnd(PmicArbCmd cmd, uint8 slaveId, uint16 address, uint32 len, uint8* data, Spmi_Result rslt)
{
    if(spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].cmd == cmd &&
       spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].sid == slaveId &&
       spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].pid == address &&
       spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].dataLen == len) 
    {
        spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].state = FINISHED;
        spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].result = rslt;
        
        if(data != NULL) {
            spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].data0 = *data;
        }
        
        uint64 start = spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].startTime;
        spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx].duration = SpmiOs_GetTimeTick() - start;
    }
    else {
        spmiBusInfo.debug.syncFlag = TRUE;
    }
    
    if(rslt != SPMI_SUCCESS)
    {
        // CHANNEL_NOT_FOUND errors happen during normal operation, so ignore them for counting purposes
        if(rslt != SPMI_FAILURE_CHANNEL_NOT_FOUND) {
            spmiBusInfo.debug.errorCnt[cmd]++;
        }
        
        spmiBusInfo.debug.errorIdx = (spmiBusInfo.debug.errorIdx + 1) % NUM_DEBUG_FAILURES;
        spmiBusInfo.debug.errorTrans[spmiBusInfo.debug.errorIdx] = spmiBusInfo.debug.busTrans[spmiBusInfo.debug.busIdx];
        
        PmicArb_GetHwStatus( &spmiBusInfo.debug.hwStatus );
        SPMI_LOG_WARNING( "Error executing cmd %d / slave: %d / addr: %x / len: %d / rslt: %d", 
                          cmd, slaveId, address, len, rslt );
    }
}

static Spmi_Result debugExecute(PmicArbCmd cmd,
                                Spmi_AccessPriority priority,
                                uint8 slaveId,
                                uint16 address,
                                uint8* data,
                                uint32 len,
                                uint32* bytesTransacted)
{
    INIT_CHECK();
    
    debugTransStart( cmd, slaveId, address, len );
    Spmi_Result rslt = PmicArb_ExecuteCommand( cmd, priority, slaveId, address, data, len, bytesTransacted );
    
    // Adjust result if a more specific root cause can be found
    if(rslt != SPMI_SUCCESS)
    {
        if(SpmiOs_GetAhbClkState() == SPMI_CLK_OFF) {
            rslt = SPMI_FAILURE_AHB_CLK_IS_OFF;
        }
        else if(SpmiOs_GetSerialClkState() == SPMI_CLK_OFF) {
            rslt = SPMI_FAILURE_SERIAL_CLK_IS_OFF;
        }
    }
    
    debugTransEnd( cmd, slaveId, address, len, data, rslt );
        
    // Fail-fast by catching some irrecoverable errors
    if(rslt == SPMI_FAILURE_TRANSACTION_TIMEOUT) {
        SPMI_LOG_FATAL( "Timeout while executing SPMI transaction" );
    }
    else if(rslt == SPMI_FAILURE_BUS_BUSY) {
        SPMI_LOG_FATAL( "SPMI bus busy" );
    }
    
    return rslt;
}

static void myIsr(uint8 channel, uint8 mask)
{
    uint64 tick = SpmiOs_GetTimeTick();
    
    spmiBusInfo.debug.isrIdx = (spmiBusInfo.debug.isrIdx + 1) % NUM_DEBUG_TRANSACTIONS;
    spmiBusInfo.debug.isrTrans[spmiBusInfo.debug.isrIdx].channel = channel;
    spmiBusInfo.debug.isrTrans[spmiBusInfo.debug.isrIdx].mask = mask;
    spmiBusInfo.debug.isrTrans[spmiBusInfo.debug.isrIdx].startTime = tick;
    spmiBusInfo.debug.isrTrans[spmiBusInfo.debug.isrIdx].duration = 0;
    
    // This should never happen since we control the input to this
    if(channel > spmiBusInfo.maxChannels) {
        SPMI_LOG_ERROR( "ISR - Invalid channel: %d", channel );
        return;
    }
    
    SpmiBusIsrConfig* userIsr = spmiBusInfo.isrTable[channel];
    
    while(userIsr != NULL)
    {
        if(userIsr->isr != NULL) 
        {
            // Check if they want this particular interrupt
            if(mask & userIsr->mask)
            {
                SPMI_LOG_VERBOSE( "Calling user ISR. chan %d", channel );

                uint64 usertick = SpmiOs_GetTimeTick();

                userIsr->isr( (void*)userIsr->ctx, mask & userIsr->mask );

                SPMI_LOG_INFO( "User ISR took %llx ticks", SpmiOs_GetTimeTick( ) - usertick );
            }
            else {
                SPMI_LOG_VERBOSE( "ISR (%p) registered for channel %d (user mask %x), but not with this mask: %x ", 
                                  userIsr->isr, channel, userIsr->mask, mask );
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
        return SPMI_SUCCESS;
    }
    
    SPMI_LOG_INIT();
    
    if((rslt = PmicArb_Init( myIsr )) != SPMI_SUCCESS) {
        return rslt;
    }
    
    spmiBusInfo.maxChannels = PmicArb_GetMaxChannelSupport();
    

    if(PmicArb_HasInterruptSupport())
    {
        if((rslt = SpmiOs_Malloc( spmiBusInfo.maxChannels * sizeof(SpmiBusIsrConfig*), 
                                  (void**)&spmiBusInfo.isrTable )) != SPMI_SUCCESS) 
        {
            return rslt;
        }
        
        SPMI_LOG_VERBOSE( "ISR table size: %d", spmiBusInfo.maxChannels * sizeof(SpmiBusIsrConfig*) );
    }
    
    spmiBusInfo.initialized = TRUE;
    
    return rslt;
}

Spmi_Result SpmiBus_DeInit()
{
    if(spmiBusInfo.isrTable != NULL)
    {
        uint32 i;
        
        for(i = 0; i < spmiBusInfo.maxChannels; i++) 
        {
            SpmiBusIsrConfig* curr = spmiBusInfo.isrTable[i];
            while(curr != NULL)
            {
                SpmiBusIsrConfig* t = curr->next;
                SpmiOs_Free( curr );
                curr = t;
            }
        }
        
        SpmiOs_Free( spmiBusInfo.isrTable );
        spmiBusInfo.isrTable = NULL;
    }
    
    spmiBusInfo.initialized = FALSE;
    return SPMI_SUCCESS;
}

Spmi_Result SpmiBus_ReadLong(uint32 slaveId,
                             Spmi_AccessPriority priority,
                             uint32 address,
                             uint8* data,
                             uint32 len,
                             uint32* bytesRead)
{
    return debugExecute( PMIC_ARB_CMD_EXTENDED_REG_READ_LONG, priority, slaveId, address, data, len, bytesRead );
}

Spmi_Result SpmiBus_WriteLong(uint32 slaveId,
                              Spmi_AccessPriority priority,
                              uint32 address,
                              uint8* data,
                              uint32 len)
{
    return debugExecute( PMIC_ARB_CMD_EXTENDED_REG_WRITE_LONG, priority, slaveId, address, data, len, NULL );
}

Spmi_Result SpmiBus_Command(uint32 slaveId, Spmi_AccessPriority priority, Spmi_Command cmd)
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
    
    return debugExecute( pmicArbCmd, priority, slaveId, 0, NULL, 0, NULL );
}

Spmi_Result SpmiBus_RegisterIsr(uint8 slaveId,
                                uint8 periph,
                                uint8 mask,
                                const SpmiIsr isr,
                                const void* ctx,
                                SpmiToken* token)
{
    INIT_CHECK();
    
    uint8 channel;
    Spmi_Result rslt;
    
    if(spmiBusInfo.isrTable == NULL) {
        return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
    }
    
    if(mask == 0 || isr == NULL || token == NULL) {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }
    
    if((rslt = PmicArb_FindChannel( slaveId, periph, &channel )) != SPMI_SUCCESS) {
        return rslt;
    }
    
    if(channel > spmiBusInfo.maxChannels) {
        SPMI_LOG_WARNING( "Couldn't get channel to set ISR. slaveId: %d, periph %d, channel %d", slaveId, periph, channel );
        return SPMI_FAILURE_CHANNEL_NOT_FOUND;
    }
    
    SpmiBusIsrConfig** curr = &spmiBusInfo.isrTable[channel];
        
    while(*curr != NULL && (*curr)->isr != NULL) 
    {
        curr = &(*curr)->next;
    }
    
    // If curr is null we are adding a node, if it is not null we are reusing a node that was 
    // unregistered, but not freed
    if(*curr == NULL)
    {
        SPMI_LOG_VERBOSE( "Registering new ISR node (chan: %d, mask: %d)", channel, mask );
        if ((rslt = SpmiOs_Malloc( sizeof(SpmiBusIsrConfig), (void**) curr )) != SPMI_SUCCESS) {
            return rslt;
        }
    }
    else {
        SPMI_LOG_VERBOSE( "Registering existing ISR node (chan: %d, mask: %d)", channel, mask );
    }
        
    (*curr)->isr = isr;
    (*curr)->ctx = ctx;
    (*curr)->mask = mask;
    (*curr)->id = ++spmiBusInfo.uid;
    
    token->id = (*curr)->id;
    token->chan = channel;
    
    return PmicArb_SetInterruptEnabled( channel, TRUE );
}

Spmi_Result SpmiBus_UnregisterIsr(SpmiToken* token, uint8 mask)
{
    INIT_CHECK();
    
    boolean keepEnabled = FALSE;
    Spmi_Result rslt = SPMI_FAILURE_INVALID_TOKEN;
        
    if(spmiBusInfo.isrTable == NULL) {
        return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
    }
    
    if(token == NULL) {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }
        
    if(token->chan >= spmiBusInfo.maxChannels) {
        SPMI_LOG_ERROR( "Invalid channel: %d", token->chan );
        return SPMI_FAILURE_CHANNEL_NOT_FOUND;
    }
    
    SpmiBusIsrConfig* curr = spmiBusInfo.isrTable[token->chan];
    SpmiBusIsrConfig* prev = NULL;
    
    while(curr != NULL) 
    {
        if(curr->id == token->id) 
        {
            SPMI_LOG_VERBOSE( "Unregistering ISR (chan: %d, mask: %d)", token->chan, mask );
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
                if(FREE_UNUSED_NODES) 
                {
                    if(prev == NULL) 
                    {
                        spmiBusInfo.isrTable[token->chan] = curr->next;
                        SpmiOs_Free( curr );
                        curr = spmiBusInfo.isrTable[token->chan];
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
        
        if(curr->isr != NULL) {
            keepEnabled = TRUE;
        }

        prev = curr;
        curr = curr->next;
    }
    
    if(!keepEnabled) {
        return PmicArb_SetInterruptEnabled( token->chan, FALSE );
    }
    
    return rslt;
}

Spmi_Result SpmiBus_ReadModifyWriteLongByte(uint32 slaveId,
                                            Spmi_AccessPriority priority,
                                            uint32 address,
                                            uint32 data,
                                            uint32 mask,
                                            uint8 *byteWritten)
{
    Spmi_Result rslt;
    uint8 readData;

    if((rslt = SpmiBus_ReadLong( slaveId, priority, address, &readData, 1, NULL )) != SPMI_SUCCESS) {
        return rslt;
    }

    readData &= ~mask;
    readData |= data & mask;

    if((rslt = SpmiBus_WriteLong( slaveId, priority, address, &readData, 1 )) != SPMI_SUCCESS) {
        return rslt;
    }

    *byteWritten = readData;

    return SPMI_SUCCESS;
}
