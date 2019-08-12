/**
 * @file:  SpmiMaster.c
 * 
 * Copyright (c) 2013 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2016/06/17 14:31:11 $
 * $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/buses/spmi/src/core/hal/badger/SpmiMaster.c#1 $
 * $Change: 10727476 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 10/1/13  Initial Version
 */

#include "SpmiMaster.h"
#include "SpmiUtils.h"
#include "SpmiHal.h"
#include "SpmiLogs.h"

//******************************************************************************
// Local Helper Functions
//******************************************************************************

/*
 * From HPG 4.3.3.3.1  Boot minimal initialization
 */
static Spmi_Result loadMin(uint8 masterId, boolean disableSecurity)
{
    // 0. Don't make assumptions - set ser_clk_sel to cc_spmi_ahb_clk for RAM programming
    SPMI_HWIO_OUT_CLEAR( HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_SER_CLK_SEL_BMSK );
    
    // 1. FW Image download
    SpmiMaster_LoadFirmware();
    
    // 2. Initiate FORCE-DEFAULT command
    SPMI_HWIO_OUT_SET( HWIO_PMIC_ARB_SPMI_GENI_FORCE_DEFAULT_REG_ADDR, HWIO_PMIC_ARB_SPMI_GENI_FORCE_DEFAULT_REG_FORCE_DEFAULT_BMSK );
    
    // 3. Switch PROGRAM-RAM clock muxing to serial clock
    SPMI_HWIO_OUT_SET( HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_SER_CLK_SEL_BMSK );
    
    // 4. Set all security features
    SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_ADDR, HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_DISABLE_SECURITY, disableSecurity );
    
    // 5: Disable forced High-Z driven to GENI outputs. Set bits SOE0_EN and SOE1_EN of GENI_OUTPUT_CTRL_REG register to 1
    SPMI_HWIO_OUT_SET( HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_ADDR, HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_SOE0_EN_BMSK |
                                                        HWIO_PMIC_ARB_SPMI_GENI_OUTPUT_CTRL_SOE1_EN_BMSK );
    
    // 6. Configure SPMI_MID_REG register
    SPMI_HWIO_OUT_FIELD( HWIO_PMIC_ARB_SPMI_MID_REG_ADDR, HWIO_PMIC_ARB_SPMI_MID_REG_MID, masterId );
    
    // 7. Set fields FORCE_MASTER_WRITE_ON_ERROR and ARBITER_ENABLE in SPMI_CFG_REG
    SPMI_HWIO_OUT_SET( HWIO_PMIC_ARB_SPMI_CFG_REG_ADDR, HWIO_PMIC_ARB_SPMI_CFG_REG_FORCE_MASTER_WRITE_ON_ERROR_BMSK );
    SPMI_HWIO_OUT_SET( HWIO_PMIC_ARB_SPMI_CFG_REG_ADDR, HWIO_PMIC_ARB_SPMI_CFG_REG_ARBITER_ENABLE_BMSK );
    
    
    // *Not in the HPG* -- Core resets in bypass mode: disable bypass
    SPMI_HWIO_OUT_CLEAR( HWIO_PMIC_ARB_SPMI_CFG_REG_ADDR, HWIO_PMIC_ARB_SPMI_CFG_REG_ARBITER_BYPASS_BMSK );
    
    return SPMI_SUCCESS;
}

/*
 * From HPG 4.3.3.3.3 
 *          Fully operational initialization (without minimal boot initialization)
 */
static Spmi_Result loadFull(uint8 masterId, boolean minAlreadyLoaded)
{
    // Insure bus is idle
    if(SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_SPMI_GENI_STATUS_ADDR, HWIO_PMIC_ARB_SPMI_GENI_STATUS_M_GENI_CMD_ACTIVE )) {
        return SPMI_FAILURE_BUS_BUSY;
    }
    
    if(minAlreadyLoaded) {
        // 4. Enable all security features -- disabled by minimal load
        SPMI_HWIO_OUT_CLEAR( HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_ADDR, HWIO_PMIC_ARB_SPMI_SEC_DISABLE_REG_DISABLE_SECURITY_BMSK );
    }
    else
    {
        // 1 - 7. 
        Spmi_Result rslt = loadMin( masterId, FALSE );
        if(rslt != SPMI_SUCCESS) {
            return rslt;
        }        
    }
    
    // 7a. Set SSC_WINDOW_EN 
    SPMI_HWIO_OUT_SET( HWIO_PMIC_ARB_SPMI_CFG_REG_ADDR, HWIO_PMIC_ARB_SPMI_CFG_REG_SSC_WINDOW_EN_BMSK );
    
    // 8. RPU initialization N/A
    
    // 9. Mapping Table (radix tree - set later)
    
    // 10. SPMI_PERIPHm_2OWNER_TABLE_REG (owner table APID2PPID - set later)
    
    // 11. Set SPMI_CMPR_EN_REG.CMPR_ENABLE 
    SPMI_HWIO_OUT_SET( HWIO_PMIC_ARB_SPMI_CMPR_EN_REG_ADDR, HWIO_PMIC_ARB_SPMI_CMPR_EN_REG_CMPR_ENABLE_BMSK );
    
    // 12. SPMI_PERIPHm_2OWNER_TABLE_REG (owner table PERIPH2OWNER - set later)
    
    // 13. Enable global interrupt flag
    SPMI_HWIO_OUT_SET( HWIO_PMIC_ARB_SPMI_MWB_ENABLE_REG_ADDR, HWIO_PMIC_ARB_SPMI_MWB_ENABLE_REG_MWB_ENABLE_BMSK );
    
    // 14. Change ROM and RAM mux to output RAM Data.
    SPMI_HWIO_OUT_CLEAR( HWIO_PMIC_ARB_SPMI_GENI_PROG_ROM_CTRL_REG_ADDR, HWIO_PMIC_ARB_SPMI_GENI_PROG_ROM_CTRL_REG_PROG_ROM_EN_BMSK );
    
    // 15. Enabling HW control CGC
    SPMI_HWIO_OUT_CLEAR( HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_PROG_RAM_SCLK_OFF_BMSK );
    SPMI_HWIO_OUT_SET( HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_ADDR, HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_PROG_RAM_HCLK_OFF_BMSK |
                                                     HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_EXT_CLK_CGC_ON_BMSK |
                                                     HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_RX_CLK_CGC_ON_BMSK |
                                                     HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_TX_CLK_CGC_ON_BMSK |
                                                     HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_SCLK_CGC_ON_BMSK |
                                                     HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_DATA_AHB_CLK_CGC_ON_BMSK |
                                                     HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_CFG_AHB_WR_CLK_CGC_ON_BMSK |
                                                     HWIO_PMIC_ARB_SPMI_GENI_CGC_CTRL_CFG_AHB_CLK_CGC_ON_BMSK );
    
    return SPMI_SUCCESS;
}

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result SpmiMaster_ConfigHW(uint8 masterId, SpmiCfg_ConfigType cfgType)
{
    SPMI_LOG_INFO( "Configuring SPMI Master. Id: %d, Type: %d", masterId, cfgType );
    
    if(masterId > SPMI_MAX_MASTER_ID) {
        return SPMI_FAILURE_INVALID_MASTER_ID;
    }
    
    switch(cfgType)
    {
        case SPMI_CFG_MINIMAL:
            return loadMin( masterId, TRUE );
            
        case SPMI_CFG_FULL:
            // If the serial clock is enabled, we already did a minimal config (step 3)
            return loadFull( masterId, SPMI_HWIO_IN_FIELD( HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_ADDR, 
                                                           HWIO_PMIC_ARB_SPMI_GENI_CLK_CTRL_SER_CLK_SEL ) != 0 );
    }
    
    SPMI_LOG_FATAL( "Invalid config type for PMI Master: %d", cfgType );
    return SPMI_FAILURE_INIT_FAILED;
}
