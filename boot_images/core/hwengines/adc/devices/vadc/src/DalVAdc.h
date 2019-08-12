#ifndef DALVADC_H
#define DALVADC_H
/*============================================================================
  @file DalVAdc.h

  Function and data structure declarations for VADC DAL


                Copyright (c) 2008-2013 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/boot.bf/3.1.2.c3/boot_images/core/hwengines/adc/devices/vadc/src/DalVAdc.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALFramework.h"
#include "DDIAdcDevice.h"
#include "SpmiBus.h"
#include "AdcScalingUtil.h"
#include "VAdcLog.h"
#include "VAdcHal.h"
#include "pmic.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#define VADC_MAX_NUM_DEVICES 2

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
/*
 * Declaring VAdc data types
 */
typedef enum
{
   VADC_REQUEST_MODE_IDLE,
   VADC_REQUEST_MODE_CALIBRATE,
   VADC_REQUEST_MODE_READ,
   VADC_REQUEST_MODE_RECALIBRATE
} VAdcRequestModeType;

/*
 * VAdcCalibDataType:
 *
 * Here, we store the calibration data for the VAdc. Calibration Data is
 * used to determine the error present the XoADC circuit. Ideally, Ground
 * should give us a raw value of 0x0 but this may not be the case because
 * of error (both internal and external) introduced in the circuit.
 *
 * Here, we measure set of standard reference volatge to determine their
 * raw value. We than use the raw value to make correction on the scaled
 * data.
 *
 * uVrefP            -- Raw ADC Data for VRef voltage.
 * uVrefN            -- Raw ADC Data for Ground.
 * uVref1            -- Raw ADC Data for mvRef1. mvRef1 is defined in bsp.
 * uVref2            -- Raw ADC Data for mvRef2. mvRef2 is defined in bsp.
 * uVref1Percent     --
 * uVref2Percent     --
 */
typedef struct
{
   uint32 uVrefP;
   uint32 uVrefN;
   uint32 uVref1;
   uint32 uVref2;
   uint32 uVref1Percent;
   uint32 uVref2Percent;
} VAdcCalibDataType;

typedef AdcDeviceResultStatusType (*VAdcScalingFuncPtrType)(AdcDeviceResultType *, const VAdcCalibDataType *);

typedef enum
{
   VADC_SCALE_TO_MILLIVOLTS = 0,
   VADC_SCALE_PMIC_SENSOR_TO_MILLIDEGREES,
   VADC_SCALE_INTERPOLATE_FROM_RT_R25,
   VADC_SCALE_INTERPOLATE_FROM_MILLIVOLTS
} VAdcScalingMethodType;

/*
 * VAdcConfigType:
 *
 * pm_xoadc_decimation_ratio_type -- Decimation Ratio for the filter.
 * pm_xoadc_conversion_rate_type  -- XO ADC clock rate.
 * usConversionTime               -- conversion time depends on the above
 *                                   configuration.
 */
typedef struct
{
   VAdcDecimationRatioType eDecimationRatio;
   VAdcClockSelectType eClockSelect;
   uint32 uConversionTime_us;
} VAdcConfigType;

typedef struct
{
   uint32 uNumerator;
   uint32 uDenominator;
} VAdcChannelScalingFactor;

typedef enum
{
   VADC_CHANNEL_MPP_CONFIG_NONE,
   VADC_CHANNEL_MPP_CONFIG_STATIC
} VAdcChannelMppConfigType;

/*
 * VAdcChannelConfigType:
 *
 * pName                      -- Channel name as defined in core/api/AdcInputs.
 * uAdcHardwareChannel        -- pmic hardware channel number.
 * nConfigIdx                 -- VAdc Filter Configuration used for this
 *                               channel. Refer to VAdcConfigType for more
 *                               details.
 * eSettlingDelay             -- Amount of time to wait for the signal to settle
 *                               after requesting the conversion
 * eFastAverageMode           -- If enabled, will automatically take multiple
 *                               readings and average them
 * bUseSequencer              -- Whether or not to use the conversion sequencer
 *                               for readings on this channel
 * uSequencerIdx              -- Sequencer index to use if we’re using the
 *                               sequencer
 * scalingFactor              -- Since the inputs are prescaled, we must know
 *                               the hardware’s scaling factor so that we can
 *                               multiply by the appropriate amount to get the
 *                               correct (unscaled) reading value
 * eScalingMethod             -- Scaling method for converting the reading to
 *                               the appropriate units
 * pInterpolationTable        -- Scaling function (if required by the scaling
 *                               method) to convert a reading to units other
 *                               than millivolts.
 * uInterpolationTableLength  -- the length of the interpolation table
 * eCalMethod                 -- The calibration type we'll use for a
 *                               particular channel.
 *
 * eMppConfig                 -- How to map the mpp – statically or not at all
 * eMpp                       -- The source MPP to use.
 * eChSelect                  -- The AMUX channel to route the MPP to.
 *
 * Suggested MPP routing:
 *         mpp -> amux
 *          1 -> 5
 *          2 -> 6
 *          3 -> 7
 *          4 -> 8
 *          5 -> 5
 *          6 -> 6
 *          7 -> 7
 *          8 -> 8
 */
typedef struct
{
   const char *pName;
   VAdcAmuxChannelSelectType uAdcHardwareChannel;
   uint32 uConfigIdx;
   VAdcSettlingDelay eSettlingDelay;
   VAdcFastAverageModeType eFastAverageMode;
   DALBOOL bUseSequencer;
   uint32 uSequencerIdx;
   VAdcChannelScalingFactor scalingFactor;
   VAdcScalingMethodType eScalingMethod;
   const AdcMapPtInt32toInt32Type *pInterpolationTable;
   uint32 uInterpolationTableLength;
   enum
   {
      VADC_CAL_METHOD_RATIOMETRIC,
      VADC_CAL_METHOD_ABSOLUTE
   }  eCalMethod;
   VAdcChannelMppConfigType eMppConfig;
   pm_mpp_perph_index eMpp;
   pm_mpp_ain_ch_type eChSelect;
} VAdcChannelConfigType;

/*
 * VAdcBspType
 */
typedef struct
{
   const SpmiBus_AccessPriorityType eAccessPriority;
   const uint32 uSlaveId;
   const uint32 uPmicDevice;
   const uint32 uPeripheralID;
   const uint32 uMasterID;
   const DALBOOL bUsesInterrupts;
   const uint32 uMinDigMinor;
   const uint32 uMinDigMajor;
   const uint32 uMinAnaMinor;
   const uint32 uMinAnaMajor;
   const uint32 uPerphType;
   const uint32 uPerphSubType;
   const uint32 uVrefP_mv;
   const uint32 uVrefN_mv;
   const uint32 uVref1_mv;
   const uint32 uVref2_mv;
   const uint32 uNumPeriodsTimeout;
   const uint32 uNumSequencerConfigs;
   const VAdcSequencerParametersType *paSequencerParams;
   const uint32 uNumConfigs;
   const VAdcConfigType *paConfigs;
   const uint32 uNumChannels;
   const VAdcChannelConfigType *paChannels;
   const uint32 uNumCalChannels;
   const VAdcChannelConfigType *paCalChannels;
} VAdcBspType;

/*
 * Declaring a "VAdc" Driver,Device and Client Context
 */
typedef struct VAdcDrvCtxt VAdcDrvCtxt;
typedef struct VAdcDevCtxt VAdcDevCtxt;
typedef struct VAdcClientCtxt VAdcClientCtxt;

/*
 * Declaring a private "VAdc" Vtable
 */
typedef struct VAdcDALVtbl VAdcDALVtbl;
struct VAdcDALVtbl
{
  int (*VAdc_DriverInit)(VAdcDrvCtxt *);
  int (*VAdc_DriverDeInit)(VAdcDrvCtxt *);
};

typedef struct
{
   VAdcDevCtxt *pDevCtxt;
} VAdcHalInterfaceCtxtType;

struct VAdcDevCtxt
{
   //Base Members
   uint32   dwRefs;
   DALDEVICEID DevId;
   uint32   dwDevCtxtRefIdx;
   VAdcDrvCtxt  *pVAdcDrvCtxt;
   DALSYS_PROPERTY_HANDLE_DECLARE(hProp);
   uint32 Reserved[16];
   //VAdc Dev state can be added by developers here
   VAdcDebugInfoType vAdcDebug;                     /* debug structure */
   const VAdcBspType   *pBsp;                       /* pointer to the BSP */
   DALBOOL bHardwareSupported;                      /* flag to indicate if the hardware is supported */
   VAdcHalInterfaceCtxtType VAdcHalInterfaceCtxt;   /* VADC HAL interface context */
   VAdcHalInterfaceType iVAdcHalInterface;          /* VADC HAL interface */
   DALSYSSyncHandle     hSync;                      /* synchronization object */
   DALSYS_SYNC_OBJECT(syncObject);
   VAdcRevisionInfoType revisionInfo;               /* VADC revision info */
   DALBOOL bCalibrated;                             /* indicates if the VADC has been calibrated */
   VAdcCalibDataType   *paCalibData;                /* pointer to measured VADC calibration data */
   VAdcRequestModeType eCurrentRequestMode;         /* type of current request */
   uint32 uCurrentChannelIdx;                       /* index to current device channel being processed */
   DalDeviceHandle      *phSpmiDev;                 /* handle to the SPMI DAL device */
   DALSYSEventHandle    hClientEvent;               /* client event to be called when current conversion or calibration is complete */
   DALSYSWorkLoopHandle hAdcOpWorkLoop;             /* work loop used to perform ADC operations */
   DALSYSEventHandle    hAdcOpEvent;                /* work loop event to perform ADC operations */
   uint32 uEventParam;                              /* event parameter value to pass to client when conversion is complete */
   AdcDeviceChannelStatusType guardedDeviceStatus;  /* the current status of the VADC device channels - protect with hSync */
   DALSYSEventHandle ahEocEvent[2];                 /* end-of-conversion event - EOC signal & a timeout */
   uint32 uFabID;                                   /* PMIC fab ID */
};

struct VAdcDrvCtxt
{
   //Base Members
   VAdcDALVtbl VAdcDALVtbl;
   uint32  dwNumDev;
   uint32  dwSizeDevCtxt;
   uint32  bInit;
   uint32  dwRefs;
   VAdcDevCtxt VAdcDevCtxt[VADC_MAX_NUM_DEVICES];
   //VAdc Drv state can be added by developers here
};

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/*
 * Declaring a "VAdc" Client Context
 */
struct VAdcClientCtxt
{
   //Base Members
   uint32  dwRefs;
   uint32  dwAccessMode;
   void *pPortCtxt;
   VAdcDevCtxt *pVAdcDevCtxt;
   DalAdcDeviceHandle DalAdcDeviceHandle;
   //VAdc Client state can be added by developers here
};

DALResult VAdc_DriverInit(VAdcDrvCtxt *);
DALResult VAdc_DriverDeInit(VAdcDrvCtxt *);
DALResult VAdc_DeviceInit(VAdcClientCtxt *);
DALResult VAdc_DeviceDeInit(VAdcClientCtxt *);
DALResult VAdc_Reset(VAdcClientCtxt *);
DALResult VAdc_PowerEvent(VAdcClientCtxt *, DalPowerCmd, DalPowerDomain);
DALResult VAdc_Open(VAdcClientCtxt *, uint32);
DALResult VAdc_Close(VAdcClientCtxt *);
DALResult VAdc_Info(VAdcClientCtxt *,DalDeviceInfo *, uint32);
DALResult VAdc_InheritObjects(VAdcClientCtxt *, DALInheritSrcPram *, DALInheritDestPram *);

/*
 * Functions specific to DalAdcDevice interface
 */
DALResult VAdc_GetDeviceProperties(VAdcClientCtxt *, AdcDeviceDevicePropertiesType *);
DALResult VAdc_GetChannelStatus(VAdcClientCtxt *, uint32, AdcDeviceChannelStatusType *);
DALResult VAdc_Calibrate(VAdcClientCtxt *, const DALSYSEventHandle, uint32);
DALResult VAdc_RecalibrateChannel(VAdcClientCtxt *, uint32, const DALSYSEventHandle, uint32);
DALResult VAdc_StartConversion(VAdcClientCtxt *, uint32, const DALSYSEventHandle, uint32);
DALResult VAdc_GetChannel(VAdcClientCtxt *, const char *, uint32 *);
DALResult VAdc_RecalibrateChannelSync(VAdcClientCtxt *, uint32, AdcDeviceRecalibrateResultType *);
DALResult VAdc_ReadChannelSync(VAdcClientCtxt *, uint32, AdcDeviceResultType *);

#endif /* #ifndef DALVADC_H */

