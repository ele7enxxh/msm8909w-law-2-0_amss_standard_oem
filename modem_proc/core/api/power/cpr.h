#ifndef __CPR_H__
#define __CPR_H__
/*
  ====================================================================

  Copyright (c) 2014-2015 Qualcomm Technologies Incorporated.  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  cpr.h
  
  Public API for the CPR driver. 
  
  $Header: //components/rel/core.mpss/3.5.c12.3/api/power/cpr.h#1 $

  ====================================================================
*/
#include "VCSDefs.h"     //core/api/systemdrivers/VCSDefs.h
#include "ClockDefs.h"

#define CPR_MAX_VMODE_COUNT VCS_CORNER_NUM_OF_CORNERS 

/**
 * <!-- CPR_Enable -->
 *
 * @brief  
 * 
 * 
 * 
 * @param s
 *
 * @return 
 */ 
 DALResult CPR_Enable( VCSRailType eRail );
 
 
 /**
 * <!-- CPR_Disable -->
 *
 * @brief  
 * 
 * 
 * 
 * @param s
 *
 * @return 
 */ 
 DALResult CPR_Disable( VCSRailType eRail );
 
 
#if 0
 /**
 * <!-- CPR_UpdateVoltageTable -->
 *
 * @brief  
 * 
 * 
 * 
 * @param s
 *
 * @return 
 */ 
 DALResult CPR_UpdateVoltageTable( VCSRailType eRail );
#endif
 
 
/**
 * <!-- CPR_GetRailVoltageRecommendation -->
 *
 * @brief  
 * 
 * 
 * 
 * @param s
 *
 * @return 
 */  
 DALResult CPR_GetRailVoltageRecommendation( VCSRailType eRail, VCSCornerType eCorner, uint32 *nVoltageuV );

 
/**
 * <!-- CPR_MeasureNow -->
 *
 * @brief  The system is at the specified corner, take measurements and update the voltage for this corner.
 * 
 * @param nRail : Which rail to measure
 * @param eCorner : Which corner the system voltage is set to
 * @param nVoltageuV : Contains the current voltage and will be filled updated by this function. 
 * 
 * @param DAL_SUCCESS if the the measurements were done.  The updated voltage is returned in nVoltageuV
 *
 * @return 
 */ 
 DALResult CPR_MeasureNow( VCSRailType nRail, VCSCornerType eCorner, uint32 *nVoltageuV );
 
 

/**
 * <!-- CPR_EnableSensor -->
 *
 * @brief  Enable a specific CPR sensor section. This function is only called while CPR is disabled.
 * 
 * CPUs or parts of the chip may be powered down of subregulated by an LDO.  This function 
 * communicates those changes to CPR so the CPR hardware can be reconfigured. 
 * 
 * @param nSensorCpuId : Value indicating which sensor to disable.
 *
 * @return DAL_SUCCESS if the enable is done and values passed all look good. 
 */ 
 DALResult CPR_EnableSensor( uint32 nSensorCpuId );

 
/**
 * <!-- CPR_DisableSensor -->
 *
 * @brief  - Disable a specific CPR sensor section. This function is only called while CPR is disabled.
 * 
 * CPUs or parts of the chip may be powered down of subregulated by an LDO.  This function 
 * communicates those changes to CPR so the CPR hardware can be reconfigured. 
 * 
 * @param nSensorCpuId : Value indicating which sensor to disable.
 *
 * @return DAL_SUCCESS if the disable is done and values passed all look good. 
 */
 DALResult CPR_DisableSensor( uint32 nSensorCpuId );

 


 /**
 * <!-- CprGetFloorAndCeiling -->
 *
 * @brief Get the floor and ceiling voltage for the specified mode. 
 *
 * @param rail_id : CLOCK_VREG_VDD_MSS (or VCS_RAIL_MSS)
 * @param mode : enum for svs, svs+, nom, turbo etc. 
 * @param floorVoltageUV : pointer to a uint32 to be filled in with a voltage in uV
 * @param ceilingVoltageUV : pointer to a uint32 to be filled in with a voltage in uV
 *
 * @return DAL_SUCCESS if there are no issues filling the provided pointers. 
 */
 DALResult CprGetFloorAndCeiling( VCSRailType rail_id, VCSCornerType mode, uint32* floorVoltageUV, uint32* ceilingVoltageUV );
 
 
 #endif
