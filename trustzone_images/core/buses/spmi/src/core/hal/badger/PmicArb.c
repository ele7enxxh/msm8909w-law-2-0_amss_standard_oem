/**
 * @file:  PmicArb.c
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/06/17 14:31:11 $
 * $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spmi/src/core/hal/badger/PmicArb.c#1 $
 * $Change: 10727476 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
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

#define SPMI_MAX_RADIX_TREE_STEPS 12

// Max number of bytes we can read/write at a time.  +1 because 0 means read/write 1 byte
#define MAX_TRANSACTION_LEN ((HWIO_PMIC_ARB_CHNLn_CMD_BYTE_CNT_BMSK >> HWIO_PMIC_ARB_CHNLn_CMD_BYTE_CNT_SHFT) + 1)

typedef struct {
    boolean valid;
    uint8 channel;
} ChannelMapEntry;

typedef struct {
    uint8 owner;
    boolean hasInterruptSupport;
    PmicArbIsrCallback isrCallback;
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
    return spmiPmicArbInfo.owner == SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_ADDR( channel ),
                                                        HWIO_PMIC_ARB_SPMI_PERIPHm_2OWNER_TABLE_REG_PERIPH2OWNER );
}

static void writeSpmiData(uint8* data, uint32 bytesToWrite)
{
    uint32 i;
    uint32 tmp;
    
    for(i = 0; i < sizeof(uint32) && bytesToWrite > 0; i++, bytesToWrite--) {
        ((uint8*)&tmp)[i] = data[i];
    }
    
    SPMI_HWIO_OUT( HWIO_PMIC_ARB_CHNLn_WDATA0_ADDR( spmiPmicArbInfo.owner ), tmp );
    
    if(bytesToWrite > 0)
    {
        tmp = 0;
        for(i = 0; i < sizeof(uint32) && bytesToWrite > 0; i++, bytesToWrite--) {
            ((uint8*)&tmp)[i] = data[i + sizeof(uint32)];
        }
        
        SPMI_HWIO_OUT( HWIO_PMIC_ARB_CHNLn_WDATA1_ADDR( spmiPmicArbInfo.owner ), tmp );
    }
}

static void readSpmiData(uint8* data, uint32 bytesToRead)
{
    uint32 i;
    uint32 spmiData = SPMI_HWIO_IN( HWIO_PMIC_ARB_CHNLn_RDATA0_ADDR( spmiPmicArbInfo.owner ) );
        
    for(i = 0; i < sizeof(uint32) && bytesToRead > 0; i++, bytesToRead--) {
        data[i] = ((uint8*)&spmiData)[i];
    }

    if(bytesToRead > 0)
    {
        spmiData = SPMI_HWIO_IN( HWIO_PMIC_ARB_CHNLn_RDATA1_ADDR( spmiPmicArbInfo.owner ) );

        for(i = 0; i < sizeof(uint32) && bytesToRead > 0; i++, bytesToRead--) {
            data[i + sizeof(uint32)] = ((uint8*)&spmiData)[i];
        }
    }
}

static Spmi_Result waitForStatusDone()
{
    uint32 status = 0;
    uint32 timeout = SPMI_TIMEOUT_USEC;
    Spmi_Result result = SPMI_SUCCESS;
    
    while(status == 0 && timeout-- > 0) 
    {
        SpmiOs_Wait( 1 );
        status = SPMI_HWIO_IN( HWIO_PMIC_ARB_CHNLn_STATUS_ADDR( spmiPmicArbInfo.owner ) );
    }

    if(status & HWIO_PMIC_ARB_CHNLn_STATUS_DROPPED_BMSK) {
        result = SPMI_FAILURE_TRANSACTION_DROPPED;
    } 
    else if(status & HWIO_PMIC_ARB_CHNLn_STATUS_DONE_BMSK) 
    {
        if(status & HWIO_PMIC_ARB_CHNLn_STATUS_FAILURE_BMSK) {
            result = SPMI_FAILURE_TRANSACTION_FAILED;
        } 
        else if(status & HWIO_PMIC_ARB_CHNLn_STATUS_DENIED_BMSK) {
            result = SPMI_FAILURE_TRANSACTION_DENIED;
        }
    }
    else if (SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_SPMI_GENI_CTRL_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CTRL_M_GENI_DISABLE )) {
        result = SPMI_FAILURE_BUS_DISABLED;
    }
    else {
        result = SPMI_FAILURE_TRANSACTION_TIMEOUT;
    }

    if(result != SPMI_SUCCESS)
    {
        SpmiOs_ClkStatus serialClkState = SpmiOs_GetSerialClkState();
        SpmiOs_ClkStatus ahbClkState = SpmiOs_GetAhbClkState();

        if(serialClkState == SPMI_CLK_OFF) {
            result = SPMI_FAILURE_SERIAL_CLK_IS_OFF;
        } 
        else if(ahbClkState == SPMI_CLK_OFF) {
            result = SPMI_FAILURE_AHB_CLK_IS_OFF;
        }

        SPMI_LOG_VERBOSE( "WaitForStatusDone. Result: %d, Channel Status: 0x%x, Serial Clk: %d, AHB Clk: %d, IRQ Status: 0x%lx",
                          result,
                          status,
                          serialClkState,
                          ahbClkState,
                          SPMI_HWIO_IN( HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_ADDR ) );
    }
    
    return result;
}

static __inline boolean isTransactionDone()
{
    return SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_CHNLn_STATUS_ADDR( spmiPmicArbInfo.owner ), HWIO_PMIC_ARB_CHNLn_STATUS_DONE );
}

static __inline void writeCommand(PmicArbCmd cmd,
                                  uint8 slaveId,
                                  Spmi_AccessPriority priority,
                                  uint16 addr,
                                  uint32 len)
{
    SPMI_HWIO_OUT( HWIO_PMIC_ARB_CHNLn_CMD_ADDR( spmiPmicArbInfo.owner ),
                   ((slaveId << HWIO_PMIC_ARB_CHNLn_CMD_SID_SHFT) |
                    (cmd << HWIO_PMIC_ARB_CHNLn_CMD_OPCODE_SHFT) |
                    (priority << HWIO_PMIC_ARB_CHNLn_CMD_PRIORITY_SHFT) |
                    (addr << HWIO_PMIC_ARB_CHNLn_CMD_ADDRESS_SHFT) |
                    (len << HWIO_PMIC_ARB_CHNLn_CMD_BYTE_CNT_SHFT)) );
}

static void* pmicArbIsr(void* ctx)
{
    uint32 i, k;
    
    for(i = 0; i <= HWIO_PMIC_ARB_SPMI_PIC_OWNERm_ACC_STATUSn_MAXn; i++) 
    {
        // Each bit in these registers represents a peripheral
        uint32 accStatus = SPMI_HWIO_IN( HWIO_PMIC_ARB_SPMI_PIC_OWNERm_ACC_STATUSn_ADDR( spmiPmicArbInfo.owner, i ) );
        
        for(k = 0; k < 32; k++) 
        {
            if((accStatus >> k) & 0x1) 
            {
                // This peripheral (channel) triggered the interrupt.  Check its individual mask
                uint8 channel = (i * 32) + k;
                uint8 mask = SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_SPMI_PIC_IRQ_STATUSn_ADDR( channel ), HWIO_PMIC_ARB_SPMI_PIC_IRQ_STATUSn_INT_STATUS );
                
                // Clear the status
                SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_SPMI_PIC_IRQ_CLEARn_ADDR( channel ), HWIO_PMIC_ARB_SPMI_PIC_IRQ_CLEARn_INT_CLEAR, mask );
                
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
        SPMI_LOG_WARNING( "SPMI interrupts not supported: %d", rslt );
        spmiPmicArbInfo.hasInterruptSupport = FALSE;
    }
    else {
        spmiPmicArbInfo.hasInterruptSupport = TRUE;
    }
        
    SPMI_LOG_VERBOSE( "SPMI IRQ status at init: 0x%lx", SPMI_HWIO_IN( HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_ADDR ) );
    
    return SPMI_SUCCESS;
}

void PmicArb_GetHwStatus(PmicArbHwStatus* status)
{
    if(status == NULL) {
        return;
    }
    
    status->irqStatus = SPMI_HWIO_IN( HWIO_PMIC_ARB_SPMI_PROTOCOL_IRQ_STATUS_ADDR );
    status->geniStatus = SPMI_HWIO_IN( HWIO_PMIC_ARB_SPMI_GENI_STATUS_ADDR );
    status->geniCtrl = SPMI_HWIO_IN( HWIO_PMIC_ARB_SPMI_GENI_CTRL_ADDR );
}

boolean PmicArb_HasInterruptSupport()
{
    return spmiPmicArbInfo.hasInterruptSupport;
}

Spmi_Result PmicArb_FindChannel(uint8 slaveId, uint8 periph, uint8* channel)
{
    uint32 ppid = SLAVE_PERIPH_TO_PPID( slaveId, periph );
    uint32 steps = 0;
    uint32 entryIndex = 0;
    uint32 entryValue;
    uint32 bitIndex;
    uint32 bitValue;
    int32 branchOrResult;
    uint32 branchOrResultFlag;

    /* The longest possible radix tree (e.g. right hanging thread) requires
    exactly 12 nodes to be visited (each iteration visits one node) */
    while (steps < SPMI_MAX_RADIX_TREE_STEPS)
    {
        ++steps;

        entryValue = SPMI_HWIO_IN( HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_ADDR(entryIndex) );

        /* Determine the action in current entry */
        bitIndex = (entryValue & HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BIT_INDEX_BMSK) >>
            HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BIT_INDEX_SHFT;
        bitValue = (ppid & (1 << bitIndex));

        if(bitValue)
        {
            branchOrResult = ((entryValue &
                HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_1_BMSK) >>
                HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_1_SHFT);
            branchOrResultFlag = ((entryValue &
                HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_1_FLAG_BMSK) >>
                HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_1_FLAG_SHFT);
        }
        else
        {
            branchOrResult = ((entryValue &
                HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_0_BMSK) >>
                HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_0_SHFT);
            branchOrResultFlag = ((entryValue &
                HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_0_FLAG_BMSK) >>
                HWIO_PMIC_ARB_SPMI_MAPPING_TABLE_REGk_BRANCH_RESULT_0_FLAG_SHFT);
        }

        if(branchOrResultFlag) /* This is Branch */
        {
            entryIndex = branchOrResult;
        }
        else /* This is result */
        {
            *channel = branchOrResult;
            return SPMI_SUCCESS;
        }
    }

    /* If reaches here, we were not able to find the APID in the Map table */
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
    
    SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_ADDR( channel ), HWIO_PMIC_ARB_SPMI_PIC_ACC_ENABLEn_INT_ACC_ENABLE, enabled );
    
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
        // Check if the Done bit is set. If not, the previous transaction is stuck
        if(!isTransactionDone()) {
            return SPMI_FAILURE_BUS_BUSY;
        }
    
        uint32 len = dataLen > MAX_TRANSACTION_LEN ? MAX_TRANSACTION_LEN : dataLen;
        
        if(doWrite) {
            writeSpmiData( data + *bytesTransacted, len );
        }
        
        // This is the signal to start the transaction
        // len - 1 because value of 0 means 1 byte and increments from there
        writeCommand( cmd, slaveId, priority, address + *bytesTransacted, len - 1 );
        
        Spmi_Result result = waitForStatusDone();
        if(result != SPMI_SUCCESS) {
            return result;
        }
        
        if(doRead) {
            readSpmiData( data + *bytesTransacted, len );
        }
        
        dataLen -= len;
        *bytesTransacted += len;
    }
    
    return SPMI_SUCCESS;
}
