#ifndef CPR_INTERNAL_H
#define CPR_INTERNAL_H
/*===========================================================================
cpr_internal.h

Internal data types. Should NOT be included by the HAL layer.

Copyright © 2013,2015 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
QUALCOMM Confidential and Proprietary.

$Header: //components/rel/core.mpss/3.5.c12.3/power/rbcpr/inc/cpr_internal.h#1 $

=============================================================================*/
#include "HAL_cpr.h"
#include "npa.h"
#include "ULogFront.h"
#include "ULog.h"
#include "rex.h"
#include "VCSDefs.h"

/** Increment this when driver functionality changes. It's logged to help
    assist debug only.  */
                           //2  - Older versions where the version isn't logged.  
                           //3  - Added version logging.  Speedpush masking impovements. 
                           //20 - New version for 9x35 and beyond.  Using new shared PMIC mode enums. 
                           //21 - Added floor voltage calculations, statistics logging. 
#define CPR_DRIVER_VERSION  21

/** Default Ulog buffer size can be overridden with the target config */
#define CPR_ULOG_DEFAULT_BUFFER_SIZE_BYTES     16384    /* Bytes (Switch to 65536 if more debug info needed) */

// Must be used in pairs - will allow compiler to catch incomplete pairs.
#define CPR_MUTEX_ACQUIRE() { DALSYS_SyncEnter(cprContext.mutexHandle);
#define CPR_MUTEX_RELEASE()   DALSYS_SyncLeave(cprContext.mutexHandle); }



/**
 * Dynamic CPR measurement data per voltage mode
 */
typedef struct
{
  uint32   currentVoltage; // microVolts

} CprMeasurementInfoType;


/**
 * Configuration info for a given voltage mode.
 */
typedef struct
{
  uint32   voltageCeiling;
  uint32   voltageFloor;

  // Number of PMIC Vdd step sizes by which the default operating voltage
  // should be corrected
  int8     initVoltOffsetSteps;

  } CprVmodeBoundsAndOffsetCfgType;


/**
 * Target specific configuration info (things that are not needed/usable
 * directly at the HAL layer).
 */
typedef struct
{
  // CPR IRQ
  uint32           irq;

  // Milliseconds by which we want to delay the next measurement
  uint32           delayBeforeNextMeas;

  // Number of modes this driver has quotients for. 
  uint32 numVmodeQuotientsAvail;

  // PMIC voltage step size in microVolts
  uint32           voltageStepSize;

  // Characterization data for each voltage mode. (ceiling, floor, intial offset...)
  // An Array of pointers to be indexed by the PMIC_NPA_MODE_ID_CORE_RAIL_xx indexes. 
  // NULL if no supported mode, non NULL points to an available vmode.
  CprVmodeBoundsAndOffsetCfgType*  vmodeBoundsAndOffsetCfgPtrs[CPR_MAX_VMODE_COUNT];

} CprTargetCfgType;
#define CPR_ULOG_STATIC_NUM_SLOTS                 8
#define CPR_ULOG_STATIC_SLOT0_DRIVER_INFO         0
#define CPR_ULOG_STATIC_SLOT1_START_VOLTAGE_INFO  1
#define CPR_ULOG_STATIC_SLOT2_CURR_VOLTAGE_INFO   2
#define CPR_ULOG_STATIC_SLOT3_MIN_VOLTAGE_INFO 	  3
#define CPR_ULOG_STATIC_SLOT4_MODECOUNT_INFO      4
#define CPR_ULOG_STATIC_SLOT5_UPISR_INFO          5
#define CPR_ULOG_STATIC_SLOT6_DOWNISR_INFO        6
#define CPR_ULOG_STATIC_SLOT7_FINFO               7
/**
 * Container for ALL the CPR driver data.
 */
typedef struct
{
  //
  // There are 3 "enableFlags" to configure the driver.
  //

  // Turns on all the closed loop ISR features.
  boolean                    enableFlags_ClosedLoopEnabled;

   // Set to TRUE if eFuse has been blown to disable the use of CPR.
  boolean                    enableFlags_DisabledByFuse;


  // New feature April 2014 Floor calculations will be enabled after validation
  boolean                    enableFlags_OpenLoop_FloorVoltageEnabled;


  //set to TRUE if the down interrupt in the current mode has been disabled. 
  boolean                    downIntEnabled;

  //set to TRUE if the up interrupt in the current mode has been disabled. 
  boolean                    upIntEnabled;


  // Set to TRUE if clkrgm is in the process of switching voltage modes.
  boolean                    switchingVmode;

  // Set to TRUE at init.  Cleared after the first time the PostSwitch is called to enable CPR.
  boolean                    clearedAfterFirstUse;

  // Voltage mode we're currently at
  uint32                     currentVmode;

  // Mode specific measurements based on CPR HW. (current voltage)
  CprMeasurementInfoType     measurements[CPR_MAX_VMODE_COUNT];

  // CPR version specific HW config that is independent of which target we're on.
  HalCprTargetConfigType   * halTargetCfg;

  // Target specific data that is independent of the CPR HW version.
  CprTargetCfgType         * targetCfg;

  // Mutex handle
  DALSYSSyncHandle           mutexHandle;

  // Clock handle
  DalDeviceHandle          * clkHandle;

  // VCS handle
  DalDeviceHandle          * vcsHandle;

  // Ulog handle
  ULogHandle                 ulogHandle;

  // Ulog handle for statitics
  ULogHandle                 ulogStaticHandle;
  
  uint32                     debugTimesInEachMode[CPR_MAX_VMODE_COUNT];
  uint32                     debugUpIsrInEachMode[CPR_MAX_VMODE_COUNT];
  uint32                     debugDownIsrInEachMode[CPR_MAX_VMODE_COUNT];
  uint32                     debugMinVoltageInEachMode[CPR_MAX_VMODE_COUNT];
  //detected PMIC for selecting the right quotients and step size on some targets.
  uint32                     detectedPMIC;

  //detected chip version for selecting the right quotients
  uint32                     detectedChipVer;
  
  //record what the disable and redundant fuses were.
  uint32                     fuse_disable;
  uint32                     fuse_redund;

  //chip info like foundary and id
  uint32                     fuse_reference_value;


  // NPA Event handle for receiving notification when the reference clock
  // frequency changes.
  npa_event_handle           refClkChgNpaHandle;

  // Current reference clock rate in kHz
  uint32                     currentRefClkRate;

  // LDO is in use for QDSP6 if TRUE - we need to disable the sensor on the
  // QDSP6 if TRUE;
  boolean                    cpuNotUsingMssRail;

  // Handler for low power modes
  void (*lpFunc)(boolean enteringLowPowerMode);

  // Mutex object
  DALSYSSyncObj syncObj;

} CprContextType;


typedef struct CprApiType
{
  DALResult (*Enable)( VCSRailType eRail );
  DALResult (*Disable)( VCSRailType eRail );
  DALResult (*GetRailVoltageRecommendation)( VCSRailType eRail, VCSCornerType eCorner, uint32 *nVoltageuV );
  DALResult (*MeasureNow)( VCSRailType nRail, VCSCornerType eCorner, uint32 *nVoltageuV );
} CprApiType;


/**
 * <!-- CprGetTargetConfiguration -->
 * 
 * @brief Get target-specific configuration info, including reading efuses etc.
 *
 * This is NOT the place to do any HW initialization.
 * 
 * cprContext->targetCfg and cprContext->halTargetCfg will
 * will be set to non-NULL pointers.
 */
void CprGetTargetConfiguration( void );


/**
 * <!-- CprGetTargetCeilingVoltageForMode -->
 *
 *
 */
DALResult CprGetTargetCeilingVoltageForMode( uint32 vmode, uint32* voltageUV );


/**
 *  <!-- CprGetTargetFloorVoltageForMode -->
 *
 *
 */
DALResult CprGetTargetFloorVoltageForMode( uint32 vmode, uint32* voltageUV );


/**
 * <!-- CprGetFloorAndCeiling -->
 *
 * @brief Get the floor and ceiling voltage for the specified mode. 
 *
 * @return DAL_SUCCESS one or 
 */
DALResult CprGetFloorAndCeiling( VCSRailType rail_id, VCSCornerType mode, uint32* floorVoltageUV, uint32* ceilingVoltageUV );



/**
 * <!-- CprInitEnabledMode -->
 *
 * @brief Initialize the driver so that CPR HW can be used to manager the voltages.
 */
void CprInitEnabledMode( CprApiType *pCprApi );


/**
 * <-- CprInitDisabledMode -->
 *
 * @brief Setup the driver to handle voltage requirements in disabled mode.
 *
 * Initialize the driver so that CPR HW is not used. The driver will still
 * continue to correct clkrgm's default voltage with the offset
 * values blown into the efuses.
 */
void CprInitDisabledMode( CprApiType *pCprApi );


/**
 * <!-- CprInitLowPowerMgr -->
 * 
 * @brief Register an LPRM with Sleep so that we can disable CPR during 
 * power collapse. 
 */
void  CprInitLowPowerMgr( void );

#endif // CPR_INTERNAL_H

