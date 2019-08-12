/**
 * @file:  SpmiCfg.h
 * @brief: This module provides configuration options for the SPMI controller
 * 
 * Copyright (c) 2013-2015 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2015/09/01 00:30:35 $
 * $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/buses/api/spmi/SpmiCfg.h#1 $
 * $Change: 8935858 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 11/3/14  Automatic channel assignment
 * 10/1/13  Initial Version
 */
#ifndef SPMICFG_H
#define	SPMICFG_H

#include "SpmiTypes.h"

//******************************************************************************
// Constants
//******************************************************************************

#define SPMI_OPEN_OWNER 0xFF

//******************************************************************************
// Data Structures
//******************************************************************************

typedef struct
{
    uint8 slaveId;     /**< Slave id of the SPMI device */
    uint8 periphId;    /**< Peripheral id on the slave device */
    uint16 channel;    /**< The channel number assigned to this peripheral (0 indexed) */
    uint8 intOwner;    /**< The EE that will receive this peripheral's interrupt. */
    uint8 periphOwner; /**< The EE that has write access to this peripheral. */
} SpmiCfg_ChannelCfg;

typedef struct
{
    uint8 slaveId;  /**< Slave id of the SPMI device */
    uint16 address; /**< Address (Periph id + offset) in the device */
} SpmiCfg_Ppid;

typedef struct 
{
    uint8 pvcPortId;              /**< PVC Port id */
    Spmi_AccessPriority priority; /**< SPMI bus command priority */
    SpmiCfg_Ppid* ppids;          /**< Peripheral access table */
    uint32 numPpids;              /**< Number of addresses in the ppids array */
} SpmiCfg_PvcPortCfg;

typedef struct 
{
    uint8 owner;
    uint32 startAddr;
    uint32 size;
    uint8 startIdx;
    uint8 endIdx;
} SpmiCfg_RGConfig;

//******************************************************************************
// Public API Functions
//******************************************************************************

/**
 * @brief Initialize the driver 
 * 
 * This function initializes the Spmi Configuration driver. It is mandatory 
 * to call this function before using any other APIs of this driver
 * 
 * @param[in] initHw  True to initialize static hw configuration
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiCfg_DeInit
 */
Spmi_Result SpmiCfg_Init(boolean initHw);

/**
 * @brief De-initialize the driver 
 * 
 * This function de-initializes the Spmi Configuration driver.
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiCfg_Init
 */
Spmi_Result SpmiCfg_DeInit(void);

/**
 * @brief Gets the ppid and owner information for a SW Channel
 * 
 * This function gets the ppid and owner information of a channel
 * 
 * @param[in] channel  The SW Channel to query
 * @param[out] slaveId   Slave id of the device mapped to the channel
 * @param[out] periphId  Peripheral id of the device mapped to the channel
 * @param[out] owner     The channel's interrupt owner
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiCfg_GetPeripherialInfo
 */
Spmi_Result SpmiCfg_GetChannelInfo(uint16 channel, uint8* slaveId, uint8* periphId, uint8* owner);

/**
 * @brief Gets the channel and owner information of a peripheral
 * 
 * This function gets the channel and owner information of a peripheral
 * 
 * @param[in] slaveId   Slave id of the device
 * @param[in] periphId  Peripheral id
 * @param[out] channel  The peripheral's SW Channel
 * @param[out] owner    The peripheral's interrupt owner
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiCfg_GetChannelInfo
 */
Spmi_Result SpmiCfg_GetPeripherialInfo(uint8 slaveId, uint8 periphId, uint16* channel, uint8* owner);

/**
 * @brief Updates the current channel configuration mode
 * 
 * This function enables or disables the configuration of channels
 * on-the-fly.  If enabled, resources will be configured as peripherals
 * are accessed.  If disabled, a static list of peripherals needs to be 
 * configured via SpmiCfg_ConfigurePeripherals.
 *
 * @param[in] enabled   True, to enable dynamic configuration mode
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * @see SpmiCfg_ConfigurePeripherals
 */
Spmi_Result SpmiCfg_SetDynamicChannelMode(boolean enabled);

/**
 * @brief Maps SW channels to a peripherals and sets interrupt ownership
 * @deprecated Use SpmiCfg_ConfigurePeripherals instead
 * 
 * This function will map logical SW Channels to the peripherals and set ownership.  
 * The mapping will exist for both the owner and the observer SW channels.
 * 
 * @Note Each owner's channels should be consecutive so they can be grouped together
 *       during SMMU/XPU configuration.
 * 
 * @param[in] entries  An array containing the mapping information.
 * @param[in] numEntries  Number of entries in the array
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * 
 * @see SpmiCfg_ConfigurePeripherals
 */
Spmi_Result SpmiCfg_ConfigureChannels(SpmiCfg_ChannelCfg* entries, uint32 numEntries);

/**
 * @brief Maps SW channels to peripherals and sets interrupt ownership.
 * 
 * This function will map peripherals to SW Channels and set related ownership information.  
 * It will override any existing configuration.
 * 
 * @param[in] entries     An array containing the mapping information.
 * @param[in] numEntries  Number of entries in the array
 * @param[out] rgCfg      An array of Resource Groups that will be populated.
 *                        Can be NULL if this info isn't wanted.
 * @param[in/out] rgCount [in]  Pointer to number of Resource Groups available
 *                        [out] Pointer to number of Resource Groups used
 *                        Can be NULL if rgCfg is NULL.
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * @see SpmiCfg_SetDynamicChannelMode
 */
Spmi_Result SpmiCfg_ConfigurePeripherals(SpmiCfg_ChannelCfg* entries, uint32 numEntries, 
                                         SpmiCfg_RGConfig* rgCfg, uint32* rgCount);

/**
 * @brief Calculates resource group configuration
 * 
 * This function will perform identically to SpmiCfg_ConfigurePeripherals, given the
 * same input, but without modifying any HW configuration.
 * 
 * @param[in] entries     An array containing the mapping information.
 * @param[in] numEntries  Number of entries in the array
 * @param[out] rgCfg      An array of Resource Groups that will be populated.
 *                        Can be NULL if this info isn't wanted.
 * @param[in/out] rgCount [in]  Pointer to number of Resource Groups available
 *                        [out] Pointer to number of Resource Groups used
 *                        Can be NULL if rgCfg is NULL.
 * 
 * @return  SPMI_SUCCESS on success, error code on error
 * @see SpmiCfg_ConfigurePeripherals
 */
Spmi_Result SpmiCfg_CalculateRGConfig(SpmiCfg_ChannelCfg* entries, uint32 numEntries, 
                                      SpmiCfg_RGConfig* rgCfg, uint32* rgCount);

/**
 * @brief Configures the PVC ports of the PMIC Arbiter 
 * 
 * This function configures the PMIC Arbiter for various PVC 
 * ports. It also enables the listed PVC ports. 
 * 
 * @param[in] portCfg  The port configuration information
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 */
Spmi_Result SpmiCfg_ConfigurePvcPort(const SpmiCfg_PvcPortCfg* portCfg);

/**
 * @brief Configures the ports of the PMIC Arbiter
 * 
 * The PMIC Arbiter has 1 SW port, n number of PVC ports, and possibly other
 * ports for things such as debug.  Each of these ports contend with each other
 * for access to the SPMI bus.  This function will set the priority of these 
 * ports relative to each other.
 * 
 * @param[in] ports  Array containing the priority info.  The priority 
 *                   is used as the array index and the highest priority is 0.
 *                   e.g. ports[0] = 2 (port 2 has the highest priority)
 *                        ports[1] = 5 (port 5 has the next highest priority)
 * @param[in] numPorts  Number of ports in the array
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 */
Spmi_Result SpmiCfg_SetPortPriorities(uint8* ports, uint32 numPorts);

/**
 * @brief Enables or disables all PVC ports
 * 
 * This function will enable or disable the HW PVC ports.  Called after
 * the ports have been configured.
 * 
 * @param[in] enable  TRUE to enable, FALSE to disable
 * 
 * @return  SPMI_SUCCESS on success, error code on failure
 * 
 * @see SpmiCfg_ConfigurePvcPort
 */
Spmi_Result SpmiCfg_SetPVCPortsEnabled(boolean enable);

#endif