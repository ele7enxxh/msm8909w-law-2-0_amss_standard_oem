/******************************************************************************
  @file:  loc_conv_locEng_qmiLoc.c
  @brief: Handle conversions between locEng and qmiLoc structures

  DESCRIPTION
  This module translates the QMI_LOC messages to the corresponding
  loc Eng messages. loc Eng messages are IDL based and are intended to be
  used across GNSS engine.

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2013-2015 QUALCOMM Atheros Incorporated.
  All Rights Reserved. QUALCOMM Atheros Proprietary and Confidential.
  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  -----------------------------------------------------------------------------
 ******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
04/06/15   sj       LB 2.0 integration 
01/30/15   sj       Fixed DOP & Sv Used Info transalation for DBT 
01/30/15   sj       Updated position transalation for DBT
03/10/14   ah       Added support for Get Avail WWAN Pos
10/22/12   ns       First revision

$Id: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/src/loc_conv_locEng_qmiLoc.c#1 $
$DateTime: 2016/12/13 07:59:45 $
======================================================================*/
#include "loc_conv_locEng_qmiLoc.h"
#include "loc_api_internal.h"
#include "string.h"
#include "gnss_common.h"

/** copy the value and validity flag of an option field for
 *  IDL generated structures */

#define COPY_OPT_FIELD(TO_FIELD, FROM_FIELD)\
 TO_FIELD = FROM_FIELD;\
 TO_FIELD##_valid = FROM_FIELD##_valid;\

/** copy optional DOP  between loc Eng format and QMI_LOC
 *  format */
#define COPY_LOC_ENG_DOP(DOP_SRC, DOP_DST)\
   DOP_DST##_valid = DOP_SRC##_valid;\
   DOP_DST.HDOP = DOP_SRC.HDOP;\
   DOP_DST.VDOP = DOP_SRC.VDOP;\
   DOP_DST.PDOP = DOP_SRC.PDOP;\

/** copy optional GPS time field between loc Eng and QMI_LOC
 *  format */
#define COPY_LOC_ENG_GPS_TIME(GPS_TIME_SRC, GPS_TIME_DST)\
   GPS_TIME_DST##_valid = GPS_TIME_SRC##_valid;\
   GPS_TIME_DST.gpsTimeOfWeekMs = GPS_TIME_SRC.gpsTimeOfWeekMs;\
   GPS_TIME_DST.gpsWeek = GPS_TIME_SRC.gpsWeek;\

typedef struct
{
  int32_t locEngVal;
  int32_t qmiLocVal;
}locEngQmiLocMapStructT;

typedef struct
{
  uint64 qmiLocMaskVal;
  uint64 locEngMaskVal;
}locEngQmiLocMaskMapStructT;

/* Table to map the sensor usage mask betwen locEng and QMI_LOC */
static struct
{
  qmiLocSensorUsageMaskT_v02 qmiLocUsageMask;
  locEngSensorUsageMaskT_v01 locEngUsageMask;

}sensorUsageMaskMap[] = {
  { QMI_LOC_SENSOR_MASK_USED_ACCEL_V02,
    LOC_ENG_SENSOR_MASK_USED_ACCEL_V01 },

  { QMI_LOC_SENSOR_MASK_USED_GYRO_V02,
    LOC_ENG_SENSOR_MASK_USED_GYRO_V01 }
};

/* Table to map the sensor aided mask betwen locEng and QMI_LOC */
static struct
{
  qmiLocSensorAidedMaskT_v02 qmiLocAidedMask;
  locEngSensorAidedMaskT_v01 locEngAidedMask;
}sensorAidedMaskMap[] = {
  { QMI_LOC_SENSOR_AIDED_MASK_HEADING_V02,
    LOC_ENG_SENSOR_AIDED_MASK_HEADING_V01 },

  { QMI_LOC_SENSOR_AIDED_MASK_SPEED_V02,
    LOC_ENG_SENSOR_AIDED_MASK_SPEED_V01 },

  { QMI_LOC_SENSOR_AIDED_MASK_POSITION_V02,
    LOC_ENG_SENSOR_AIDED_MASK_POSITION_V01 },

  { QMI_LOC_SENSOR_AIDED_MASK_VELOCITY_V02,
    LOC_ENG_SENSOR_AIDED_MASK_VELOCITY_V01 }
};

/* Table to map sensor time source */
static locEngQmiLocMapStructT sensorTimeSourceTable[] =
{
  { (int32_t)eLOC_ENG_SENSOR_TIME_SOURCE_UNSPECIFIED_V01,
    (int32_t)eQMI_LOC_SENSOR_TIME_SOURCE_UNSPECIFIED_V02 },

  { (int32_t)eLOC_ENG_SENSOR_TIME_SOURCE_COMMON_V01,
    (int32_t)eQMI_LOC_SENSOR_TIME_SOURCE_COMMON_V02 }
};

/* Table to map motion state between loc Eng and qmi Loc */
static locEngQmiLocMapStructT motionStateTable[] =
{
  { (int32_t) eLOC_ENG_MOTION_STATE_UNKNOWN_V01,
    (int32_t) eQMI_LOC_MOTION_STATE_UNKNOWN_V02 },

  { (int32_t) eLOC_ENG_MOTION_STATE_STATIONARY_V01,
    (int32_t) eQMI_LOC_MOTION_STATE_STATIONARY_V02 },

  { (int32_t) eLOC_ENG_MOTION_STATE_IN_MOTION_V01,
    (int32_t) eQMI_LOC_MOTION_STATE_IN_MOTION_V02 }
};

/* Table to map motion mode between loc Eng and qmi Loc */
static locEngQmiLocMapStructT motionModeTable[] =
{
  { (int32_t) eLOC_ENG_MOTION_MODE_UNKNOWN_V01,
    (int32_t) eQMI_LOC_MOTION_MODE_UNKNOWN_V02 },

  { (int32_t) eLOC_ENG_MOTION_MODE_STATIONARY_V01,
    (int32_t) eQMI_LOC_MOTION_MODE_STATIONARY_V02 },

  { (int32_t) eLOC_ENG_MOTION_MODE_PEDESTRIAN_UNKNOWN_V01,
    (int32_t) eQMI_LOC_MOTION_MODE_PEDESTRIAN_UNKNOWN_V02 },

  { (int32_t) eLOC_ENG_MOTION_MODE_PEDESTRIAN_WALKING_V01,
    (int32_t) eQMI_LOC_MOTION_MODE_PEDESTRIAN_WALKING_V02 },

  { (int32_t) eLOC_ENG_MOTION_MODE_PEDESTRIAN_RUNNING_V01,
    (int32_t) eQMI_LOC_MOTION_MODE_PEDESTRIAN_RUNNING_V02 },

  { (int32_t) eLOC_ENG_MOTION_MODE_VEHICLE_UNKNOWN_V01,
    (int32_t) eQMI_LOC_MOTION_MODE_VEHICLE_UNKNOWN_V02 }
};

static locEngQmiLocMaskMapStructT techMaskTable[] =
{
  { (uint64_t)QMI_LOC_POS_TECH_MASK_SATELLITE_V02,
    (uint64_t)LOC_ENG_POS_TECH_MASK_SATELLITE_V01 },
  
  { (uint64)QMI_LOC_POS_TECH_MASK_CELLID_V02,
    (uint64)LOC_ENG_POS_TECH_MASK_CELLID_V01 },

  { (uint64)QMI_LOC_POS_TECH_MASK_WIFI_V02,
    (uint64)LOC_ENG_POS_TECH_MASK_WIFI_V01 },

  { (uint64)QMI_LOC_POS_TECH_MASK_SENSORS_V02,
    (uint64)LOC_ENG_POS_TECH_MASK_SENSORS_V01 },

  { (uint64)QMI_LOC_POS_TECH_MASK_REFERENCE_LOCATION_V02,
    (uint64)LOC_ENG_POS_TECH_MASK_REFERENCE_LOCATION_V01 },

  { (uint64)QMI_LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION_V02,
    (uint64)LOC_ENG_POS_TECH_MASK_INJECTED_COARSE_POSITION_V01 }
};





/**countNumberOfBitsIn64BitMask
@brief :      Count the number of bits in a 64 bit mask. 
@param [in]   t_bitMask :  64 bit mask

@retval Number of bits in the mask.
---------------------------------------------------------------------*/
static uint32 countNumberOfBitsIn64BitMask(uint64 t_bitMask)
{
    uint32 valid_bits_count; // accumulates the total number of bits set in the bit mask

    for (valid_bits_count = 0; t_bitMask; valid_bits_count++)
    {
      t_bitMask &= t_bitMask-1; // clears the least significant bit set
    }

    return valid_bits_count;
}

/** convertLocEngEnum
@brief : convert a loc Eng enum to corresponding QMI LOC enum *  
 
@param [in]: mapTable : table mapping loc Eng values to 
       qmiLoc values
@param [in]: size : size of the map table 
@param [in]: locEngVal : loc Eng enumeration 
@param [out]: pQmiLocVal: pointer to qmiLoc output 
@return TRUE: if corresponding qmi value was found ; 
        FALSE : if corresponding qmi value was not found */

static boolean convertLocEngEnum(locEngQmiLocMapStructT mapTable[],
                                 uint32 size,
                                 int32 locEngVal,
                                 int32 *pQmiLocVal)
{
  uint32 idx = 0;
  for (idx = 0; idx < size; idx++)
  {
    if (locEngVal == mapTable[idx].locEngVal)
    {
      *pQmiLocVal = mapTable[idx].qmiLocVal;
      return TRUE;
    }
  }

  return FALSE;
}

/** convertQmiLocEnum
@brief : convert a QMI LOC enum to corresponding locEng enum  
 
@param [in]: mapTable : table mapping loc Eng values to 
       qmiLoc values
@param [in]: size : size of the map table  
@param [in]: qmiLocVal : QMI LOC enumeration 
@param [out]: pLocEngVal: pointer to loc Eng output 
@return TRUE: if corresponding loc eng value was found 
        FALSE : if corresponding loc eng value was not found
 */
 
static boolean convertQmiLocEnum(locEngQmiLocMapStructT mapTable[],
                                 uint32 size,
                                 int32 qmiLocVal,
                                 int32 *pLocEngVal)
{
  int32 idx = 0;
  for (idx = 0; idx < size; idx++)
  {
    if (qmiLocVal == mapTable[idx].qmiLocVal)
    {
      *pLocEngVal = mapTable[idx].locEngVal;
      return TRUE;
    }
  }

  return FALSE;
}


/** convertLocEngMask
@brief : convert a loc Eng Mask to corresponding QMI LOC Mask *  
 
@param [in]: mapTable : table mapping loc Eng values to 
       qmiLoc values
@param [in]: size : size of the map table 
@param [in]: locEngMask : loc Eng Mask
@return qmiLoc Mask calue */

static uint64 convertLocEngMask(
  locEngQmiLocMaskMapStructT mapTable[],
  uint32 size,
  uint64 locEngMask)
{
  uint32 idx = 0;
  uint64 qmiLocMask = 0;
  for (idx = 0; idx < size; idx++)
  {
	if (locEngMask & mapTable[idx].locEngMaskVal)
    {
      qmiLocMask |= mapTable[idx].qmiLocMaskVal;
    }
  }

  return qmiLocMask;
}


/**  convertLocEngReliability
 
@brief : convert reliability from loc Eng common pos format to 
       QMI_LOC format.
@param [in]   srcRel :   Reliability in loc Eng Pos format 
@param [out]  pDstRel :  Reliability in QMI_LOC format

@retval TRUE  Reliability successfully translated
@retval FALSE Reliability was not translated successfully
---------------------------------------------------------------------*/

boolean convertLocEngReliability(locEngReliabilityEnumT_v01 srcRel,
                                 qmiLocReliabilityEnumT_v02 *pDstRel)
{
  if (NULL == pDstRel)
  {
    LOC_MSG_ERROR(" convertLocEngReliability: Null Dst pointer\n",
                  0, 0, 0);
    return FALSE;
  }
  switch (srcRel)
  {
    case eLOC_ENG_RELIABILITY_NOT_SET_V01:
      *pDstRel = eQMI_LOC_RELIABILITY_NOT_SET_V02;
      break;

    case eLOC_ENG_RELIABILITY_VERY_LOW_V01:
      *pDstRel = eQMI_LOC_RELIABILITY_VERY_LOW_V02;
      break;

    case eLOC_ENG_RELIABILITY_LOW_V01:
      *pDstRel = eQMI_LOC_RELIABILITY_LOW_V02;
      break;

    case eLOC_ENG_RELIABILITY_MEDIUM_V01:
      *pDstRel = eQMI_LOC_RELIABILITY_MEDIUM_V02;
      break;

    case eLOC_ENG_RELIABILITY_HIGH_V01:
      *pDstRel = eQMI_LOC_RELIABILITY_HIGH_V02;
      break;

    default:
      LOC_MSG_ERROR("convertLocEngReliability: Unexpected reliability %d\n",
                    srcRel, 0, 0);
      return FALSE;
  }

  return TRUE;
}

/**  convertLocEngTimeSrc
 
@brief : convert Time source from loc Eng common pos format to 
       QMI_LOC format.
@param [in]   srcTimeSrc :   Time Source in loc Eng Pos format 
@param [out]  pDstTimeSrc :  Time Source in QMI_LOC format

@retval TRUE  Time Source successfully translated
@retval FALSE Time Source was not translated successfully
---------------------------------------------------------------------*/

boolean convertLocEngTimeSrc(locEngTimeSourceEnumT_v01 srcTimeSrc,
                             qmiLocTimeSourceEnumT_v02 *pDstTimeSrc)
{
  if (NULL == pDstTimeSrc)
  {
    LOC_MSG_ERROR(" convertLocEngTimeSrc Null Dst pointer\n",
                  0, 0, 0);
    return FALSE;
  }

  switch (srcTimeSrc)
  {
    case eLOC_ENG_TIME_SRC_INVALID_V01:
      *pDstTimeSrc = eQMI_LOC_TIME_SRC_INVALID_V02;
      break;

    case eLOC_ENG_TIME_SRC_NETWORK_TIME_TRANSFER_V01:
      *pDstTimeSrc = eQMI_LOC_TIME_SRC_NETWORK_TIME_TRANSFER_V02;
      break;

    case eLOC_ENG_TIME_SRC_NETWORK_TIME_TAGGING_V01:
      *pDstTimeSrc = eQMI_LOC_TIME_SRC_NETWORK_TIME_TAGGING_V02;
      break;

    case eLOC_ENG_TIME_SRC_EXTERNAL_INPUT_V01:
      *pDstTimeSrc = eQMI_LOC_TIME_SRC_EXTERNAL_INPUT_V02;
      break;

    case eLOC_ENG_TIME_SRC_TOW_DECODE_V01:
      *pDstTimeSrc = eQMI_LOC_TIME_SRC_TOW_DECODE_V02;
      break;

    case eLOC_ENG_TIME_SRC_TOW_CONFIRMED_V01:
      *pDstTimeSrc = eQMI_LOC_TIME_SRC_TOW_CONFIRMED_V02;
      break;

    case eLOC_ENG_TIME_SRC_TOW_AND_WEEK_CONFIRMED_V01:
      *pDstTimeSrc = eQMI_LOC_TIME_SRC_TOW_AND_WEEK_CONFIRMED_V02;
      break;

    case eLOC_ENG_TIME_SRC_NAV_SOLUTION_V01:
      *pDstTimeSrc = eQMI_LOC_TIME_SRC_NAV_SOLUTION_V02;
      break;

    case eLOC_ENG_TIME_SRC_SOLVE_FOR_TIME_V01:
      *pDstTimeSrc = eQMI_LOC_TIME_SRC_SOLVE_FOR_TIME_V02;
      break;

    case eLOC_ENG_TIME_SRC_GLO_TOW_DECODE_V01:
      *pDstTimeSrc = eQMI_LOC_TIME_SRC_GLO_TOW_DECODE_V02;
      break;

    case eLOC_ENG_TIME_SRC_TIME_TRANSFORM_V01:
      *pDstTimeSrc = eQMI_LOC_TIME_SRC_TIME_TRANSFORM_V02;
      break;

    case eLOC_ENG_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING_V01:
      *pDstTimeSrc = eQMI_LOC_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING_V02;
      break;

    case eLOC_ENG_TIME_SRC_GSM_SLEEP_TIME_TAGGING_V01:
      *pDstTimeSrc = eQMI_LOC_TIME_SRC_GSM_SLEEP_TIME_TAGGING_V02;
      break;

    case eLOC_ENG_TIME_SRC_UNKNOWN_V01:
      *pDstTimeSrc = eQMI_LOC_TIME_SRC_UNKNOWN_V02;
      break;

    case eLOC_ENG_TIME_SRC_SYSTEM_TIMETICK_V01:
      *pDstTimeSrc = eQMI_LOC_TIME_SRC_SYSTEM_TIMETICK_V02;
      break;

    case eLOC_ENG_TIME_SRC_QZSS_TOW_DECODE_V01:
      *pDstTimeSrc = (qmiLocTimeSourceEnumT_v02)eQMI_LOC_TIME_SRC_QZSS_TOW_DECODE_V02;
      break;

    case eLOC_ENG_TIME_SRC_BDS_TOW_DECODE_V01:
      *pDstTimeSrc = (qmiLocTimeSourceEnumT_v02)eQMI_LOC_TIME_SRC_BDS_TOW_DECODE_V02;
      break;
    case eLOC_ENG_TIME_SRC_GAL_TOW_DECODE_V01:
      *pDstTimeSrc = (qmiLocTimeSourceEnumT_v02)eQMI_LOC_TIME_SRC_GAL_TOW_DECODE_V02;
      break;

    default:
      LOC_MSG_ERROR("convertLocEngTimeSrc: Unexpected timesource %d\n",
                    srcTimeSrc, 0, 0);
      return FALSE;
  }

  return TRUE;
}

/**  convertLocEngSensorUsage
 
@brief : convert sensor from loc Eng common format to QMI_LOC
       format.
@param [in]   pSrcSensorUsage : Sensor Usage in loc Eng Pos 
       format
@param [out] pDstSensorUsage :  Sensor Usage in QMI_LOC format

@retval TRUE  Sensor Usage successfully translated
@retval FALSE Sensor Usage was not translated successfully
---------------------------------------------------------------------*/

boolean convertLocEngSensorUsage(
  const locEngSensorUsageIndicatorStructT_v01 *pSrcSensorUsage,
  qmiLocSensorUsageIndicatorStructT_v02 *pDstSensorUsage)
{
  int idx = 0;
  if (NULL == pSrcSensorUsage || NULL == pDstSensorUsage)
  {
    LOC_MSG_ERROR("convertLocEngSensorUsage: Null pointers "
                  "pSrcSensorUsage = %p pDstSensorUsage = %p \n",
                  pSrcSensorUsage, pDstSensorUsage, 0);
    return FALSE;
  }

  for (idx = 0;
       idx < (sizeof(sensorUsageMaskMap) / sizeof(sensorUsageMaskMap[0]));
       idx++)
  {
    if (pSrcSensorUsage->usageMask & sensorUsageMaskMap[idx].qmiLocUsageMask)
    {
      pDstSensorUsage->usageMask |= sensorUsageMaskMap[idx].locEngUsageMask;
    }
  }

  for (idx = 0;
       idx < (sizeof(sensorAidedMaskMap) / sizeof(sensorAidedMaskMap[0]));
       idx++)
  {
    if (pSrcSensorUsage->aidingIndicatorMask &
          sensorAidedMaskMap[idx].qmiLocAidedMask)
    {
      pDstSensorUsage->aidingIndicatorMask |=
        sensorAidedMaskMap[idx].locEngAidedMask;
    }
  }

  return TRUE;
}


/**   convertLocEngPosToBapPos
 
@brief : convert loc Eng common position to QMI_LOC Best available
         position.
@param [in]   pLocEngPosition :  Pointer to Loc Eng position
@param [out]  pBapPosition    :  Pointer to BAP position

@retval TRUE  Position was successfully translated
@retval FALSE Position was not be translated successfully
---------------------------------------------------------------------*/

boolean convertLocEngPosToBapPos
(
  const locEngPositionReportMsgT_v01        *pLocEngPosition,
  qmiLocGetBestAvailablePositionIndMsgT_v02 *pBapPosition
)
{
  /* check NULL input */
  if (NULL == pLocEngPosition || NULL == pBapPosition)
  {
    LOC_MSG_ERROR("convert_pos_report_to_bap: NULL param error, "
                  "position_report_ptr = %p, pBapPosition = %p", pLocEngPosition,
                  pBapPosition, 0);
    return FALSE;
  }

  memset(pBapPosition, 0, sizeof(*pBapPosition));

  /* copy the optional TLV's */
  COPY_OPT_FIELD(pBapPosition->altitudeWrtEllipsoid,
                 pLocEngPosition->altitudeWrtEllipsoid);

  COPY_OPT_FIELD(pBapPosition->altitudeWrtMeanSeaLevel,
                 pLocEngPosition->altitudeWrtMeanSeaLevel);

  COPY_OPT_FIELD(pBapPosition->heading,
                 pLocEngPosition->heading);

  COPY_OPT_FIELD(pBapPosition->headingUnc,
                 pLocEngPosition->headingUnc);

  COPY_OPT_FIELD(pBapPosition->horCircularConfidence,
                 pLocEngPosition->horCircularConfidence);

  COPY_OPT_FIELD(pBapPosition->horEllipticalConfidence,
                 pLocEngPosition->horEllipticalConfidence);

  COPY_OPT_FIELD(pBapPosition->horSpeed,
                 pLocEngPosition->horSpeed);

  COPY_OPT_FIELD(pBapPosition->horSpeedUnc,
                 pLocEngPosition->horSpeedUnc);

  COPY_OPT_FIELD(pBapPosition->horUncCircular,
                 pLocEngPosition->horUncCircular);

  COPY_OPT_FIELD(pBapPosition->horUncEllipseOrientAzimuth,
                 pLocEngPosition->horUncEllipseOrientAzimuth);

  COPY_OPT_FIELD(pBapPosition->horUncEllipseSemiMajor,
                 pLocEngPosition->horUncEllipseSemiMajor);

  COPY_OPT_FIELD(pBapPosition->horUncEllipseSemiMinor,
                 pLocEngPosition->horUncEllipseSemiMinor);

  COPY_OPT_FIELD(pBapPosition->latitude,
                 pLocEngPosition->latitude);

  COPY_OPT_FIELD(pBapPosition->longitude,
                 pLocEngPosition->longitude);

  COPY_OPT_FIELD(pBapPosition->magneticDeviation,
                 pLocEngPosition->magneticDeviation);

  COPY_OPT_FIELD(pBapPosition->technologyMask,
                 pLocEngPosition->technologyMask);

  COPY_OPT_FIELD(pBapPosition->timestampUtc,
                 pLocEngPosition->timestampUtc);

  COPY_OPT_FIELD(pBapPosition->timeUnc,
                 pLocEngPosition->timeUnc);

  COPY_OPT_FIELD(pBapPosition->vertConfidence,
                 pLocEngPosition->vertConfidence);

  COPY_OPT_FIELD(pBapPosition->vertSpeed,
                 pLocEngPosition->vertSpeed);

  COPY_OPT_FIELD(pBapPosition->vertSpeedUnc,
                 pLocEngPosition->vertSpeedUnc);

  COPY_OPT_FIELD(pBapPosition->vertUnc,
                 pLocEngPosition->vertUnc);

  /* convert session status  to indication status */

  switch (pLocEngPosition->sessionStatus)
  {
    case eLOC_ENG_POS_SESS_STATUS_SUCCESS_V01:
      pBapPosition->status = eQMI_LOC_SUCCESS_V02;
      break;
    case eLOC_ENG_SESS_STATUS_GENERAL_FAILURE_V01:
      pBapPosition->status = eQMI_LOC_GENERAL_FAILURE_V02;
      break;
    case eLOC_ENG_SESS_STATUS_TIMEOUT_V01:
      pBapPosition->status = eQMI_LOC_TIMEOUT_V02;
      break;
    default:
      LOC_MSG_ERROR("convert_pos_report_to_bap: Unexpected status %d\n",
                    pLocEngPosition->sessionStatus, 0, 0);
      return FALSE;
  }

  /* Convert hor Reliability */
  pBapPosition->horReliability_valid = pLocEngPosition->horReliability_valid;

  if (pBapPosition->horReliability_valid)
  {
    if (FALSE == convertLocEngReliability(pLocEngPosition->horReliability,
                                          &pBapPosition->horReliability))
    {
      return FALSE;
    }
  }

  /* Convert vert Reliability */
  pBapPosition->vertReliability_valid = pLocEngPosition->vertReliability_valid;

  if (pBapPosition->vertReliability_valid)
  {
    if (FALSE == convertLocEngReliability(pLocEngPosition->vertReliability,
                                          &pBapPosition->vertReliability))
    {
      return FALSE;
    }

  }

  /* Convert time source */
  pBapPosition->timeSrc_valid = pLocEngPosition->timeSrc_valid;

  if (pBapPosition->timeSrc_valid)
  {
    if (FALSE == convertLocEngTimeSrc(pLocEngPosition->timeSrc,
                                      &pBapPosition->timeSrc))
    {
      return FALSE;
    }
  }

  pBapPosition->sensorDataUsage_valid =
    pLocEngPosition->sensorDataUsage_valid;

  if (pBapPosition->sensorDataUsage_valid)
  {

    if (FALSE == convertLocEngSensorUsage(
        &pLocEngPosition->sensorDataUsage,
        &pBapPosition->sensorDataUsage))
    {
      return FALSE;
    }
  }

  /* Copy DOP */
  COPY_LOC_ENG_DOP(pLocEngPosition->DOP, pBapPosition->DOP);

  /* Copy GPS Time */
  COPY_LOC_ENG_GPS_TIME(pLocEngPosition->gpsTime,
                        pBapPosition->gpsTime);

  /* copy the SV used list */
  pBapPosition->gnssSvUsedList_valid = pLocEngPosition->gnssSvUsedList_valid;

  if (pBapPosition->gnssSvUsedList_valid)
  {
    unsigned int idx;

    pBapPosition->gnssSvUsedList_len = pLocEngPosition->gnssSvUsedList_len;

    for (idx = 0; idx < pBapPosition->gnssSvUsedList_len; idx++)
    {
      pBapPosition->gnssSvUsedList[idx] = pLocEngPosition->gnssSvUsedList[idx];
    }
  }

  return TRUE;
}

/**   convertLocEngPosToAvailWwanPos
 
@brief : convert loc Eng common position to QMI_LOC Available WWAN
         position.
@param [in]   pLocEngPosition : Pointer to Loc Eng position
@param [out]  pAvWwanPosition : Pointer to Available WWAN position

@retval TRUE  Position was successfully translated
@retval FALSE Position was not be translated successfully
---------------------------------------------------------------------*/

boolean convertLocEngPosToAvailWwanPos
(
  const locEngPositionReportMsgT_v01    *pLocEngPosition,
  qmiLocGetAvailWwanPositionIndMsgT_v02 *pAvWwanPosition
)
{
  /* check NULL input */
  if (NULL == pLocEngPosition || NULL == pAvWwanPosition)
  {
    LOC_MSG_ERROR("convertLocEngPosToAvailWwanPos: NULL param error, "
                  "pLocEngPosition = %p, pAvWwanPosition = %p", pLocEngPosition,
                  pAvWwanPosition, 0);
    return FALSE;
  }

  memset(pAvWwanPosition, 0, sizeof(*pAvWwanPosition));

  /* copy the optional TLV's */
  COPY_OPT_FIELD(pAvWwanPosition->altitudeWrtEllipsoid,
                 pLocEngPosition->altitudeWrtEllipsoid);

  COPY_OPT_FIELD(pAvWwanPosition->altitudeWrtMeanSeaLevel,
                 pLocEngPosition->altitudeWrtMeanSeaLevel);

  COPY_OPT_FIELD(pAvWwanPosition->horCircularConfidence,
                 pLocEngPosition->horCircularConfidence);

  COPY_OPT_FIELD(pAvWwanPosition->horEllipticalConfidence,
                 pLocEngPosition->horEllipticalConfidence);

  COPY_OPT_FIELD(pAvWwanPosition->horUncCircular,
                 pLocEngPosition->horUncCircular);

  COPY_OPT_FIELD(pAvWwanPosition->horUncEllipseOrientAzimuth,
                 pLocEngPosition->horUncEllipseOrientAzimuth);

  COPY_OPT_FIELD(pAvWwanPosition->horUncEllipseSemiMajor,
                 pLocEngPosition->horUncEllipseSemiMajor);

  COPY_OPT_FIELD(pAvWwanPosition->horUncEllipseSemiMinor,
                 pLocEngPosition->horUncEllipseSemiMinor);

  COPY_OPT_FIELD(pAvWwanPosition->latitude,
                 pLocEngPosition->latitude);

  COPY_OPT_FIELD(pAvWwanPosition->longitude,
                 pLocEngPosition->longitude);

  COPY_OPT_FIELD(pAvWwanPosition->timestampUtc,
                 pLocEngPosition->timestampUtc);

  COPY_OPT_FIELD(pAvWwanPosition->timeUnc,
                 pLocEngPosition->timeUnc);

  COPY_OPT_FIELD(pAvWwanPosition->vertConfidence,
                 pLocEngPosition->vertConfidence);

  COPY_OPT_FIELD(pAvWwanPosition->vertUnc,
                 pLocEngPosition->vertUnc);

  /* convert session status to indication status */

  switch (pLocEngPosition->sessionStatus)
  {
    case eLOC_ENG_POS_SESS_STATUS_SUCCESS_V01:
      pAvWwanPosition->status = eQMI_LOC_SUCCESS_V02;
      break;
    case eLOC_ENG_SESS_STATUS_GENERAL_FAILURE_V01:
      pAvWwanPosition->status = eQMI_LOC_GENERAL_FAILURE_V02;
      break;
    case eLOC_ENG_SESS_STATUS_TIMEOUT_V01:
      pAvWwanPosition->status = eQMI_LOC_TIMEOUT_V02;
      break;
    default:
      LOC_MSG_ERROR("convert_pos_report_to_bap: Unexpected status %d\n",
                    pLocEngPosition->sessionStatus, 0, 0);
      return FALSE;
  }

  /* Convert hor Reliability */
  pAvWwanPosition->horReliability_valid = pLocEngPosition->horReliability_valid;

  if (pAvWwanPosition->horReliability_valid)
  {
    if (FALSE == convertLocEngReliability(pLocEngPosition->horReliability,
                                          &pAvWwanPosition->horReliability))
    {
      return FALSE;
    }
  }

  /* Convert vert Reliability */
  pAvWwanPosition->vertReliability_valid = pLocEngPosition->vertReliability_valid;

  if (pAvWwanPosition->vertReliability_valid)
  {
    if (FALSE == convertLocEngReliability(pLocEngPosition->vertReliability,
                                          &pAvWwanPosition->vertReliability))
    {
      return FALSE;
    }

  }

  /* Convert time source */
  pAvWwanPosition->timeSrc_valid = pLocEngPosition->timeSrc_valid;

  if (pAvWwanPosition->timeSrc_valid)
  {
    if (FALSE == convertLocEngTimeSrc(pLocEngPosition->timeSrc,
                                      &pAvWwanPosition->timeSrc))
    {
      return FALSE;
    }
  }

  /* Copy GPS Time */
  COPY_LOC_ENG_GPS_TIME(pLocEngPosition->gpsTime,
                        pAvWwanPosition->gpsTime);

  return TRUE;
}

/**   convertLocEngPosToAvailablePosition
 
@brief : convert loc Eng common position to QMI_LOC Available GNSS
         position.
@param [in]   pLocEngPosition : Pointer to Loc Eng position
@param [out]  pAvailablePosition : Pointer to Available GNSS position

@retval TRUE  Position was successfully translated
@retval FALSE Position was not be translated successfully
---------------------------------------------------------------------*/

boolean convertLocEngPosToAvailablePosition
(
  const locEngPositionReportMsgT_v01    *pLocEngPosition,
  qmiLocGetAvailablePositionIndParamMsgT_v02 *pAvailablePosition
)
{
  /* check NULL input */
  if (NULL == pLocEngPosition || NULL == pAvailablePosition)
  {
    LOC_MSG_ERROR("convertLocEngPosToAvailGnssPos: NULL param error, "
                  "pLocEngPosition = %p, pAvailablePosition = %p", pLocEngPosition,
                  pAvailablePosition, 0);
    return FALSE;
  }

  memset(pAvailablePosition, 0, sizeof(*pAvailablePosition));

  /* copy the optional TLV's */
  COPY_OPT_FIELD(pAvailablePosition->altitudeWrtEllipsoid,
                 pLocEngPosition->altitudeWrtEllipsoid);

  COPY_OPT_FIELD(pAvailablePosition->altitudeWrtMeanSeaLevel,
                 pLocEngPosition->altitudeWrtMeanSeaLevel);

  COPY_OPT_FIELD(pAvailablePosition->horCircularConfidence,
                 pLocEngPosition->horCircularConfidence);

  COPY_OPT_FIELD(pAvailablePosition->horEllipticalConfidence,
                 pLocEngPosition->horEllipticalConfidence);

  COPY_OPT_FIELD(pAvailablePosition->horUncCircular,
                 pLocEngPosition->horUncCircular);

  COPY_OPT_FIELD(pAvailablePosition->horUncEllipseOrientAzimuth,
                 pLocEngPosition->horUncEllipseOrientAzimuth);

  COPY_OPT_FIELD(pAvailablePosition->horUncEllipseSemiMajor,
                 pLocEngPosition->horUncEllipseSemiMajor);

  COPY_OPT_FIELD(pAvailablePosition->horUncEllipseSemiMinor,
                 pLocEngPosition->horUncEllipseSemiMinor);

  COPY_OPT_FIELD(pAvailablePosition->latitude,
                 pLocEngPosition->latitude);

  COPY_OPT_FIELD(pAvailablePosition->longitude,
                 pLocEngPosition->longitude);

  COPY_OPT_FIELD(pAvailablePosition->timestampUtc,
                 pLocEngPosition->timestampUtc);

  COPY_OPT_FIELD(pAvailablePosition->timeUnc,
                 pLocEngPosition->timeUnc);

  COPY_OPT_FIELD(pAvailablePosition->vertConfidence,
                 pLocEngPosition->vertConfidence);

  COPY_OPT_FIELD(pAvailablePosition->vertUnc,
                 pLocEngPosition->vertUnc);

  COPY_OPT_FIELD(pAvailablePosition->heading,
                 pLocEngPosition->heading);

  COPY_OPT_FIELD(pAvailablePosition->headingUnc,
                 pLocEngPosition->headingUnc);
 
  COPY_OPT_FIELD(pAvailablePosition->horSpeed,
                 pLocEngPosition->horSpeed);

  COPY_OPT_FIELD(pAvailablePosition->horSpeedUnc,
                 pLocEngPosition->horSpeedUnc);

  COPY_OPT_FIELD(pAvailablePosition->magneticDeviation,
                 pLocEngPosition->magneticDeviation);

  COPY_OPT_FIELD(pAvailablePosition->technologyMask,
                 pLocEngPosition->technologyMask);

  COPY_OPT_FIELD(pAvailablePosition->vertSpeed,
                 pLocEngPosition->vertSpeed);

  COPY_OPT_FIELD(pAvailablePosition->vertSpeedUnc,
                 pLocEngPosition->vertSpeedUnc);

  /* convert session status to indication status */

  switch (pLocEngPosition->sessionStatus)
  {
    case eLOC_ENG_POS_SESS_STATUS_SUCCESS_V01:
      pAvailablePosition->status = eQMI_LOC_SUCCESS_V02;
      break;
    case eLOC_ENG_SESS_STATUS_GENERAL_FAILURE_V01:
      pAvailablePosition->status = eQMI_LOC_GENERAL_FAILURE_V02;
      break;
    case eLOC_ENG_SESS_STATUS_TIMEOUT_V01:
      pAvailablePosition->status = eQMI_LOC_TIMEOUT_V02;
      break;
    default:
      LOC_MSG_ERROR("convert_pos_report_to_gnss: Unexpected status %d\n",
                    pLocEngPosition->sessionStatus, 0, 0);
      return FALSE;
  }

  /* Convert hor Reliability */
  pAvailablePosition->horReliability_valid = pLocEngPosition->horReliability_valid;

  if (pAvailablePosition->horReliability_valid)
  {
    if (FALSE == convertLocEngReliability(pLocEngPosition->horReliability,
                                          &pAvailablePosition->horReliability))
    {
      return FALSE;
    }
  }

  /* Convert vert Reliability */
  pAvailablePosition->vertReliability_valid = pLocEngPosition->vertReliability_valid;

  if (pAvailablePosition->vertReliability_valid)
  {
    if (FALSE == convertLocEngReliability(pLocEngPosition->vertReliability,
                                          &pAvailablePosition->vertReliability))
    {
      return FALSE;
    }

  }

  /* Convert time source */
  pAvailablePosition->timeSrc_valid = pLocEngPosition->timeSrc_valid;

  if (pAvailablePosition->timeSrc_valid)
  {
    if (FALSE == convertLocEngTimeSrc(pLocEngPosition->timeSrc,
                                      &pAvailablePosition->timeSrc))
    {
      return FALSE;
    }
  }

  /* Copy GPS Time */
  COPY_LOC_ENG_GPS_TIME(pLocEngPosition->gpsTime,
                        pAvailablePosition->gpsTime);

  pAvailablePosition->sensorDataUsage_valid =
    pLocEngPosition->sensorDataUsage_valid;

  if (pAvailablePosition->sensorDataUsage_valid)
  {

    if (FALSE == convertLocEngSensorUsage(
        &pLocEngPosition->sensorDataUsage,
        &pAvailablePosition->sensorDataUsage))
    {
      return FALSE;
    }
  }

  /* Copy DOP */
  COPY_LOC_ENG_DOP(pLocEngPosition->DOP, pAvailablePosition->DOP);


  /* copy the SV used list */
  pAvailablePosition->gnssSvUsedList_valid = pLocEngPosition->gnssSvUsedList_valid;

  if (pAvailablePosition->gnssSvUsedList_valid)
  {
    int idx;

    pAvailablePosition->gnssSvUsedList_len = pLocEngPosition->gnssSvUsedList_len;

    for (idx = 0; idx < pAvailablePosition->gnssSvUsedList_len; idx++)
    {
      pAvailablePosition->gnssSvUsedList[idx] = pLocEngPosition->gnssSvUsedList[idx];
    }
  }

  LOC_MSG_ERROR("convert_pos_report_to_gnss: Confidence  valid %d  %d\n",
				pAvailablePosition->horCircularConfidence_valid, pAvailablePosition->horCircularConfidence, 0);

  return TRUE;
}
/**   convertLocEngStatus
 
@brief : convert loc Eng status to QMI_LOC status.
@param [in]  locEngStatus :  Status in locEng format
 
@retval  corresponding qmiLoc status
------------------------------------------------------------------*/

qmiLocStatusEnumT_v02 convertLocEngStatus(
  locEngStatusEnumT_v01 locEngStatus)
{
  qmiLocStatusEnumT_v02 qmiLocStatus = eQMI_LOC_GENERAL_FAILURE_V02;
  switch (locEngStatus)
  {
    case eLOC_ENG_SUCCESS_V01:
      qmiLocStatus = eQMI_LOC_SUCCESS_V02;
      break;
    case eLOC_ENG_INVALID_PARAMETER_V01:
      qmiLocStatus = eQMI_LOC_INVALID_PARAMETER_V02;
      break;
    case eLOC_ENG_CONFIG_NOT_SUPPORTED_V01:
      qmiLocStatus = eQMI_LOC_CONFIG_NOT_SUPPORTED_V02;
      break;
    case eLOC_ENG_ENGINE_BUSY_V01:
      qmiLocStatus = eQMI_LOC_ENGINE_BUSY_V02;
      break;
    case eLOC_ENG_INSUFFICIENT_MEMORY_V01:
      qmiLocStatus = eQMI_LOC_INSUFFICIENT_MEMORY_V02;
      break;
    case eLOC_ENG_PHONE_OFFLINE_V01:
      qmiLocStatus = eQMI_LOC_PHONE_OFFLINE_V02;
      break;
    case eLOC_ENG_TIMEOUT_V01:
      qmiLocStatus = eQMI_LOC_TIMEOUT_V02;
      break;
    case eLOC_ENG_UNSUPPORTED_V01:
      qmiLocStatus = eQMI_LOC_UNSUPPORTED_V02;
      break;
    case eLOC_ENG_GENERAL_FAILURE_V01:
    default:
      qmiLocStatus = eQMI_LOC_GENERAL_FAILURE_V02;
      break;

  }

  LOC_MSG_MED("convertLocEngStatus: in %d, out %d",
              locEngStatus, qmiLocStatus, 0);
  return qmiLocStatus;
}


/**   convertLocEngEventPedometerControlInd
 
@brief : convert locEng Pedometer Control event to QMI_LOC. 
@param [in]   pLocEngEventPedometerControl :  Pointer to Loc Eng 
       Pedometer Control event payload.
@param [out]  pEventPedometerControl :  Pointer to QMI_LOC 
       Pedometer control event payload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/

boolean convertLocEngEventPedometerControlInd(
  const locEngEventPedometerControlIndMsgT_v01 *pLocEngEventPedometerControl,
  qmiLocEventPedometerControlIndMsgT_v02 *pEventPedometerControl)

{
  if (NULL == pLocEngEventPedometerControl ||
      NULL == pEventPedometerControl)
  {
    return FALSE;
  }

  memset(pEventPedometerControl, 0, sizeof(*pEventPedometerControl));

  pEventPedometerControl->requestPedometerData =
    pLocEngEventPedometerControl->requestPedometerData;

  COPY_OPT_FIELD(pEventPedometerControl->resetStepCount,
                 pLocEngEventPedometerControl->resetStepCount);

  COPY_OPT_FIELD(pEventPedometerControl->stepCountThreshold,
                 pLocEngEventPedometerControl->stepCountThreshold);
  return TRUE;
}


/**  convertLocEngEventMotionDataControlInd
@brief : convert locEng Motion Data Control event to QMI_LOC. 
@param [in]   pLocEngEventMotionControl :  Pointer to Loc Eng 
       Motion Data Control event payload.
@param [out]  pEventMotionControl :  Pointer to QMI_LOC Motion 
       Data Control event payload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/

boolean convertLocEngEventMotionDataControlInd(
  const locEngEventMotionDataControlIndMsgT_v01 *pLocEngEventMotionControl,
  qmiLocEventMotionDataControlIndMsgT_v02 *pEventMotionControl)
{
  if (NULL == pLocEngEventMotionControl ||
      NULL == pEventMotionControl)
  {
    return FALSE;
  }

  pEventMotionControl->requestMotionData =
    pLocEngEventMotionControl->requestMotionData;

  return TRUE;
}


/** convertQmiLocPedomterReportReq
@brief : convert QMI LOC pedometer report request to Loc Eng. 
@param [in]   pQmiLocPedometerReportReq :  Pointer to QMI LOC 
              pedometer report request payload.
@param [out]  pPedometerReportReq :  Pointer to Loc Eng 
       pedometer report request payload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/

boolean convertQmiLocPedomterReportReq(
  const qmiLocPedometerReportReqMsgT_v02 *pQmiLocPedometerReportReq,
  locEngPedometerReportReqMsgT_v01 *pPedometerReportReq)
{
  if (NULL == pQmiLocPedometerReportReq ||
      NULL == pPedometerReportReq)
  {
    return FALSE;
  }

  memset(pPedometerReportReq, 0, sizeof(*pPedometerReportReq));

  pPedometerReportReq->modemTimeTickMsec = cgps_TimeTickGetMsec();

  pPedometerReportReq->timestamp = pQmiLocPedometerReportReq->timestamp;
  pPedometerReportReq->timeInterval = pQmiLocPedometerReportReq->timeInterval;

  pPedometerReportReq->stepCount = pQmiLocPedometerReportReq->stepCount;

  pPedometerReportReq->messageSource = eLOC_ENG_MSG_SOURCE_LOC_MW_V01;

  COPY_OPT_FIELD(pPedometerReportReq->stepConfidence,
                 pQmiLocPedometerReportReq->stepConfidence);

  COPY_OPT_FIELD(pPedometerReportReq->stepCountUncertainty,
                 pQmiLocPedometerReportReq->stepCountUncertainty);

  COPY_OPT_FIELD(pPedometerReportReq->stepRate,
                 pQmiLocPedometerReportReq->stepRate);

  return (convertLocEngEnum( sensorTimeSourceTable,
                             GNSS_ARRAY_SIZEOF(sensorTimeSourceTable),
                             (int32_t)pQmiLocPedometerReportReq->timeSource,
                             (int32_t*) &(pPedometerReportReq->timeSource) ) );
}

/** convertQmiLocInjectMotionDataReq
@brief : convert QMI LOC inject motion data request to Loc Eng. 
@param [in]   pQmiLocMotionDataReq :  Pointer to QMI LOC 
              inject motion data request payload.
@param [out]  pMotionDataReq :  Pointer to Loc Eng inject motion
       data request payload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/

boolean convertQmiLocInjectMotionDataReq(
  const qmiLocInjectMotionDataReqMsgT_v02 *pQmiLocMotionDataReq,
  locEngInjectMotionDataReqMsgT_v01 *pMotionDataReq)
{
  boolean retVal = FALSE;
  if (NULL == pQmiLocMotionDataReq ||
      NULL == pMotionDataReq)
  {
    return retVal;
  }

  memset(pMotionDataReq, 0, sizeof(*pMotionDataReq));

  pMotionDataReq->modemTimeTickMsec = cgps_TimeTickGetMsec();

  pMotionDataReq->messageSource = eLOC_ENG_MSG_SOURCE_LOC_MW_V01;

  pMotionDataReq->motion_data.age = pQmiLocMotionDataReq->motion_data.age;

  pMotionDataReq->motion_data.probability_of_state =
    pQmiLocMotionDataReq->motion_data.probability_of_state;

  pMotionDataReq->motion_data.timeout =
    pQmiLocMotionDataReq->motion_data.timeout;

  retVal = convertQmiLocEnum( 
    motionStateTable,
    GNSS_ARRAY_SIZEOF (motionStateTable),
    (int32_t) pQmiLocMotionDataReq->motion_data.motion_state,
    (int32_t *)&(pMotionDataReq->motion_data.motion_state));

  if (TRUE == retVal)
  {
    retVal = convertQmiLocEnum( 
      motionModeTable,
      GNSS_ARRAY_SIZEOF (motionModeTable),
      (int32_t)pQmiLocMotionDataReq->motion_data.motion_mode,
      (int32_t *)&(pMotionDataReq->motion_data.motion_mode));
  }

 return retVal;
}


/** convertToLocEngInjectVehicleSensorDataReq
@brief : convert QMI LOC vehicle sensor inject data 
       request to Loc Eng.
@param [in]   pQmiLocVehicleSensorReq :  Pointer to QMI LOC 
              vehicle sensor inject data request payload.
@param [out]  pLocEngVehicleSensorReq :  Pointer to Loc Eng 
       vehicle sensor inject data request payload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/

boolean convertToLocEngInjectVehicleSensorDataReq(
  const qmiLocInjectVehicleSensorDataReqMsgT_v02 *pQmiLocVehicleSensorReq,
  locEngInjectVehicleSensorDataReqMsgT_v01 *pLocEngVehicleSensorReq)
{
  uint8 i,j;
  uint32 valid_bits_count;

  /* validate input pointers */
  if (NULL == pQmiLocVehicleSensorReq ||
      NULL == pLocEngVehicleSensorReq)
  {
    return FALSE;
  }

  memset(pLocEngVehicleSensorReq, 0, sizeof(*pLocEngVehicleSensorReq));

  /* proxy might overwrite the timestamp */
  pLocEngVehicleSensorReq->modemTimeTickMsec = cgps_TimeTickGetMsec();
  pLocEngVehicleSensorReq->messageSource = eLOC_ENG_MSG_SOURCE_LOC_MW_V01;

  /* accelData */
  pLocEngVehicleSensorReq->accel_data_valid = pQmiLocVehicleSensorReq->accelData_valid;
  if (pQmiLocVehicleSensorReq->accelData_valid)
  {
    pLocEngVehicleSensorReq->accel_data.axes_validity = pQmiLocVehicleSensorReq->accelData.axesValidity; /* qmiLocAxesMaskT_v02 --> locEngAxesMaskT_v01 */
    pLocEngVehicleSensorReq->accel_data.sensor_sample_time_base = pQmiLocVehicleSensorReq->accelData.sampleTimeBase; /* uint32 */
    pLocEngVehicleSensorReq->accel_data.sensorData_len = pQmiLocVehicleSensorReq->accelData.sensorData_len; /* uint32 */


    valid_bits_count = countNumberOfBitsIn64BitMask(pLocEngVehicleSensorReq->accel_data.axes_validity);
    for (i = 0; i < pLocEngVehicleSensorReq->accel_data.sensorData_len; i++)
    {
      pLocEngVehicleSensorReq->accel_data.sensorData[i].sample_timetag_offset = pQmiLocVehicleSensorReq->accelData.sensorData[i].timeOffset; /* uint32 */
      pLocEngVehicleSensorReq->accel_data.sensorData[i].axis_sample_len = pQmiLocVehicleSensorReq->accelData.sensorData[i].axisSample_len; /* uint32 */

      if (valid_bits_count < pLocEngVehicleSensorReq->accel_data.sensorData[i].axis_sample_len)
      {
        return FALSE;
      }

      for (j = 0; j < pLocEngVehicleSensorReq->accel_data.sensorData[i].axis_sample_len; j++)
      {
        pLocEngVehicleSensorReq->accel_data.sensorData[i].axis_sample[j] = pQmiLocVehicleSensorReq->accelData.sensorData[i].axisSample[j]; /* float */
      }
    }
  }

  /* angRotationData */
  pLocEngVehicleSensorReq->ang_rotation_data_valid = pQmiLocVehicleSensorReq->angRotationData_valid;
  if (pQmiLocVehicleSensorReq->angRotationData_valid)
  {
    pLocEngVehicleSensorReq->ang_rotation_data.sensor_sample_time_base = pQmiLocVehicleSensorReq->angRotationData.sampleTimeBase; /* uint32 */
    pLocEngVehicleSensorReq->ang_rotation_data.axes_validity = pQmiLocVehicleSensorReq->angRotationData.axesValidity; /* qmiLocAxesMaskT_v02 --> locEngAxesMaskT_v01 */
    pLocEngVehicleSensorReq->ang_rotation_data.sensorData_len = pQmiLocVehicleSensorReq->angRotationData.sensorData_len; /* uint32 */

    valid_bits_count = countNumberOfBitsIn64BitMask(pLocEngVehicleSensorReq->ang_rotation_data.axes_validity);
    for (i=0; i < pLocEngVehicleSensorReq->ang_rotation_data.sensorData_len; i++)
    {
      pLocEngVehicleSensorReq->ang_rotation_data.sensorData[i].sample_timetag_offset = pQmiLocVehicleSensorReq->angRotationData.sensorData[i].timeOffset; /* uint32 */
      pLocEngVehicleSensorReq->ang_rotation_data.sensorData[i].axis_sample_len = pQmiLocVehicleSensorReq->angRotationData.sensorData[i].axisSample_len; /* uint32 */

      if (valid_bits_count < pLocEngVehicleSensorReq->ang_rotation_data.sensorData[i].axis_sample_len)
      {
        return FALSE;
      }

      for (j=0; j < pLocEngVehicleSensorReq->ang_rotation_data.sensorData[i].axis_sample_len; j++)
      {
        pLocEngVehicleSensorReq->ang_rotation_data.sensorData[i].axis_sample[j] = pQmiLocVehicleSensorReq->angRotationData.sensorData[i].axisSample[j]; /* float */
      }
    }
  }

  /* changeInTimeScales */
  pLocEngVehicleSensorReq->change_in_time_scales_valid = pQmiLocVehicleSensorReq->changeInTimeScales_valid;
  pLocEngVehicleSensorReq->change_in_time_scales = pQmiLocVehicleSensorReq->changeInTimeScales; /* int32 */

  /* odometryData */
  pLocEngVehicleSensorReq->odometry_data_valid = pQmiLocVehicleSensorReq->odometryData_valid;
  if (pQmiLocVehicleSensorReq->odometryData_valid)
  {
    pLocEngVehicleSensorReq->odometry_data.sensor_sample_time_base = pQmiLocVehicleSensorReq->odometryData.sampleTimeBase; /* uint32 */
    pLocEngVehicleSensorReq->odometry_data.flags = 
      pQmiLocVehicleSensorReq->odometryData.flags; /* qmiLocVehicleOdometryMeasDeviationMaskType_v02 --> locEngVehicleOdometryMeasDeviationMaskT_v01 */

    pLocEngVehicleSensorReq->odometry_data.wheel_flags = 
      pQmiLocVehicleSensorReq->odometryData.wheelFlags; /* qmiLocVehicleOdometryWheelFlagsMaskT_v02 --> locEngVehicleOdometryWheelFlagsMaskT_v01 */

    pLocEngVehicleSensorReq->odometry_data.distance_travelled_base = pQmiLocVehicleSensorReq->odometryData.distanceTravelledBase; /* uint32 */
    pLocEngVehicleSensorReq->odometry_data.odometry_data_len = pQmiLocVehicleSensorReq->odometryData.odometryData_len; /* uint32 */

    valid_bits_count = countNumberOfBitsIn64BitMask(pLocEngVehicleSensorReq->odometry_data.wheel_flags);
    for (i=0; i < pLocEngVehicleSensorReq->odometry_data.odometry_data_len; i++)
    {
      pLocEngVehicleSensorReq->odometry_data.odometry_data[i].sample_timetag_offset = pQmiLocVehicleSensorReq->odometryData.odometryData[i].timeOffset; /* uint32 */
      pLocEngVehicleSensorReq->odometry_data.odometry_data[i].distance_travelled_len = pQmiLocVehicleSensorReq->odometryData.odometryData[i].distanceTravelled_len; /* uint32 */

      if (valid_bits_count < pLocEngVehicleSensorReq->odometry_data.odometry_data[i].distance_travelled_len)
      {
        return FALSE;
      }

      for (j=0; j < pLocEngVehicleSensorReq->odometry_data.odometry_data[i].distance_travelled_len; j++)
      {
        pLocEngVehicleSensorReq->odometry_data.odometry_data[i].distance_travelled[j] = pQmiLocVehicleSensorReq->odometryData.odometryData[i].distanceTravelled[j]; /* uint32 */
      }
    }
  }

  // user data is filled in the caller function

   return TRUE;
}


/** convertTolocEngSetNavigationConfigReq
@brief : convert QMI LOC set sensor properties data 
       request to Loc Eng Set Navigation Config type.
@param [in]   pQmiLocSetSensorPropertiesReq :  Pointer to QMI 
              LOC set sensor properties data request payload.
@param [out]  pLocEngSetNavConfigReq :  Pointer to Loc Eng Set 
       Navigation config data request payload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/

boolean convertTolocEngSetNavigationConfigReq(
  const qmiLocSetSensorPropertiesReqMsgT_v02 *pQmiLocSetSensorPropertiesReq,
  locEngSetNavigationConfigReqMsgT_v01 *pLocEngSetNavConfigReq)
{
  /* validate input pointers */
  if (NULL == pQmiLocSetSensorPropertiesReq ||
      NULL == pLocEngSetNavConfigReq)
  {
    return FALSE;
  }
  
  memset(pLocEngSetNavConfigReq, 0, sizeof(*pLocEngSetNavConfigReq));

  /* proxy might overwrite the timestamp */
  pLocEngSetNavConfigReq->modemTimeTickMsec = cgps_TimeTickGetMsec();
  pLocEngSetNavConfigReq->messageSource = eLOC_ENG_MSG_SOURCE_LOC_MW_V01;

  pLocEngSetNavConfigReq->acceleration_random_walk_spectral_density_valid = pQmiLocSetSensorPropertiesReq->accelerationRandomWalkSpectralDensity_valid;
  pLocEngSetNavConfigReq->acceleration_random_walk_spectral_density = pQmiLocSetSensorPropertiesReq->accelerationRandomWalkSpectralDensity; /* float */

  pLocEngSetNavConfigReq->angle_random_walk_spectral_density_valid = pQmiLocSetSensorPropertiesReq->angleRandomWalkSpectralDensity_valid;
  pLocEngSetNavConfigReq->angle_random_walk_spectral_density = pQmiLocSetSensorPropertiesReq->angleRandomWalkSpectralDensity; /* float */

  pLocEngSetNavConfigReq->gyro_bias_random_walk_valid = pQmiLocSetSensorPropertiesReq->gyroBiasVarianceRandomWalk_valid;
  pLocEngSetNavConfigReq->gyro_bias_random_walk = pQmiLocSetSensorPropertiesReq->gyroBiasVarianceRandomWalk; /* float */

  pLocEngSetNavConfigReq->rate_random_walk_spectral_density_valid = pQmiLocSetSensorPropertiesReq->rateRandomWalkSpectralDensity_valid;
  pLocEngSetNavConfigReq->rate_random_walk_spectral_density = pQmiLocSetSensorPropertiesReq->rateRandomWalkSpectralDensity; /* float */
  
  pLocEngSetNavConfigReq->vehicle_accel_random_walk_spectral_density_valid = pQmiLocSetSensorPropertiesReq->vehicleAccelRandomWalkSpectralDensity_valid;
  pLocEngSetNavConfigReq->vehicle_accel_random_walk_spectral_density = pQmiLocSetSensorPropertiesReq->vehicleAccelRandomWalkSpectralDensity; /* float */

  pLocEngSetNavConfigReq->vehicle_angle_random_walk_spectral_density_valid = pQmiLocSetSensorPropertiesReq->vehicleAngleRandomWalkSpectralDensity_valid;
  pLocEngSetNavConfigReq->vehicle_angle_random_walk_spectral_density = pQmiLocSetSensorPropertiesReq->vehicleAngleRandomWalkSpectralDensity; /* float */

  pLocEngSetNavConfigReq->vehicle_angular_rate_random_walk_spectral_density_valid = pQmiLocSetSensorPropertiesReq->vehicleAngularRateRandomWalkSpectralDensity_valid;
  pLocEngSetNavConfigReq->vehicle_angular_rate_random_walk_spectral_density = pQmiLocSetSensorPropertiesReq->vehicleAngularRateRandomWalkSpectralDensity; /* float */

  pLocEngSetNavConfigReq->vehicle_data_use_valid = pQmiLocSetSensorPropertiesReq->vehicleDataUse_valid;
  pLocEngSetNavConfigReq->vehicle_data_use = pQmiLocSetSensorPropertiesReq->vehicleDataUse; /* qmiLocVehicleDataUseControlMaskT_v02 --> locEngVehicleDataUseControlMaskT_v01 */

  pLocEngSetNavConfigReq->vehicle_odometry_scale_factor_random_walk_spectral_density_valid = pQmiLocSetSensorPropertiesReq->vehicleOdometryScaleFactorRandomWalkSpectralDensity_valid;
  pLocEngSetNavConfigReq->vehicle_odometry_scale_factor_random_walk_spectral_density = 
    pQmiLocSetSensorPropertiesReq->vehicleOdometryScaleFactorRandomWalkSpectralDensity; /* float */

  pLocEngSetNavConfigReq->vehicle_odometry_variance_valid = pQmiLocSetSensorPropertiesReq->vehicleOdometryVariance_valid;
  pLocEngSetNavConfigReq->vehicle_odometry_variance = pQmiLocSetSensorPropertiesReq->vehicleOdometryVariance; /* float */

  pLocEngSetNavConfigReq->vehicle_velocity_random_walk_spectral_density_valid = pQmiLocSetSensorPropertiesReq->vehicleVelocityRandomWalkSpectralDensity_valid;
  pLocEngSetNavConfigReq->vehicle_velocity_random_walk_spectral_density = pQmiLocSetSensorPropertiesReq->vehicleVelocityRandomWalkSpectralDensity; /* float */

  pLocEngSetNavConfigReq->velocity_random_walk_spectral_density_valid = pQmiLocSetSensorPropertiesReq->velocityRandomWalkSpectralDensity_valid;
  pLocEngSetNavConfigReq->velocity_random_walk_spectral_density = pQmiLocSetSensorPropertiesReq->velocityRandomWalkSpectralDensity; /* float */

  // user data is filled in the caller function

  return TRUE;
}


/** convertTolocEngGetNavigationConfigReq
@brief : convert QMI LOC get sensor properties data request to 
       Loc Eng get Navigation Config type.
@param [in]   pQmiLocGtSensorPropertiesReq :  Pointer to QMI 
              LOC get sensor properties data request payload.
@param [out]  pLocEngGetNavConfigReq :  Pointer to Loc Eng Get 
       Navigation config data request payload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/

boolean convertTolocEngGetNavigationConfigReq(
  const qmiLocGetSensorPropertiesReqMsgT_v02 *pQmiLocGetSensorPropertiesReq,
  locEngGetNavigationConfigReqMsgT_v01 *pLocEngGetNavConfigReq)
{
  /* validate input pointers */
  if (NULL == pQmiLocGetSensorPropertiesReq ||
      NULL == pLocEngGetNavConfigReq)
  {
    return FALSE;
  }
  
  memset(pLocEngGetNavConfigReq, 0, sizeof(*pLocEngGetNavConfigReq));

  /* proxy might overwrite the timestamp */
  pLocEngGetNavConfigReq->modemTimeTickMsec = cgps_TimeTickGetMsec();
  pLocEngGetNavConfigReq->messageSource = eLOC_ENG_MSG_SOURCE_LOC_MW_V01;

  // user data is filled in the caller function

  return TRUE;
}


/**  convertFromlocEngGetNavigationConfigResp
@brief : convert locEng Get Navigation Config Resp to QMI_LOC.
@param [in]   pLocEngEventMotionControl :  Pointer to Loc Eng 
       Motion Data Control event payload.
@param [out]  pEventMotionControl :  Pointer to QMI_LOC Motion 
       Data Control event payload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/

boolean convertFromlocEngGetNavigationConfigResp(
  const locEngGetNavigationConfigRespMsgT_v01 *pLocEngGetNavConfigResp,
  qmiLocGetSensorPropertiesIndMsgT_v02 *pQmiLocGetSensorPropertiesInd,
  qmiLocSensorPropertiesMaskT_v02 sensorPropertiesMask)
{
  if (NULL == pLocEngGetNavConfigResp ||
      NULL == pQmiLocGetSensorPropertiesInd)
  {
    return FALSE;
  }

  if ((FALSE != pLocEngGetNavConfigResp->acceleration_random_walk_spectral_density_valid) &&
      (sensorPropertiesMask & QMI_LOC_SENSOR_PROPERTIES_MASK_ACCELERATION_RANDOM_WALK_SPECTRAL_DENSITY_V02))
  {
    pQmiLocGetSensorPropertiesInd->accelerationRandomWalkSpectralDensity_valid = TRUE;
    pQmiLocGetSensorPropertiesInd->accelerationRandomWalkSpectralDensity = pLocEngGetNavConfigResp->acceleration_random_walk_spectral_density; /* float */
  }
  else
  {
    pQmiLocGetSensorPropertiesInd->accelerationRandomWalkSpectralDensity_valid = FALSE;
  }

  if ((FALSE != pLocEngGetNavConfigResp->angle_random_walk_spectral_density_valid) &&
      (sensorPropertiesMask & QMI_LOC_SENSOR_PROPERTIES_MASK_ANGLE_RANDOM_WALK_SPECTRAL_DENSITY_V02))
  {
    pQmiLocGetSensorPropertiesInd->angleRandomWalkSpectralDensity_valid = TRUE;
    pQmiLocGetSensorPropertiesInd->angleRandomWalkSpectralDensity = pLocEngGetNavConfigResp->angle_random_walk_spectral_density; /* float */
  }
  else
  {
    pQmiLocGetSensorPropertiesInd->angleRandomWalkSpectralDensity_valid = FALSE;
  }

  if ((FALSE != pLocEngGetNavConfigResp->gyro_bias_random_walk_valid) &&
      (sensorPropertiesMask & QMI_LOC_SENSOR_PROPERTIES_MASK_GYRO_BIAS_VARIANCE_RANDOM_WALK_V02))
  {
    pQmiLocGetSensorPropertiesInd->gyroBiasVarianceRandomWalk_valid = TRUE;
    pQmiLocGetSensorPropertiesInd->gyroBiasVarianceRandomWalk = pLocEngGetNavConfigResp->gyro_bias_random_walk; /* float */
  }
  else
  {
    pQmiLocGetSensorPropertiesInd->gyroBiasVarianceRandomWalk_valid = FALSE;
  }

  if ((FALSE != pLocEngGetNavConfigResp->rate_random_walk_spectral_density_valid) &&
      (sensorPropertiesMask & QMI_LOC_SENSOR_PROPERTIES_MASK_RATE_RANDOM_WALK_SPECTRAL_DENSITY_V02))
  {
    pQmiLocGetSensorPropertiesInd->rateRandomWalkSpectralDensity_valid = TRUE;
    pQmiLocGetSensorPropertiesInd->rateRandomWalkSpectralDensity = pLocEngGetNavConfigResp->rate_random_walk_spectral_density; /* float */
  }
  else
  {
    pQmiLocGetSensorPropertiesInd->rateRandomWalkSpectralDensity_valid = FALSE;
  }

  if ((FALSE != pLocEngGetNavConfigResp->vehicle_accel_random_walk_spectral_density_valid) &&
      (sensorPropertiesMask & QMI_LOC_SENSOR_PROPERTIES_MASK_VEHICLE_ACCEL_RWSD_V02))
  {
    pQmiLocGetSensorPropertiesInd->vehicleAccelRandomWalkSpectralDensity_valid = TRUE;
    pQmiLocGetSensorPropertiesInd->vehicleAccelRandomWalkSpectralDensity = pLocEngGetNavConfigResp->vehicle_accel_random_walk_spectral_density; /* float */
  }
  else
  {
    pQmiLocGetSensorPropertiesInd->vehicleAccelRandomWalkSpectralDensity_valid = FALSE;
  }

  if ((FALSE != pLocEngGetNavConfigResp->vehicle_angle_random_walk_spectral_density_valid) &&
      (sensorPropertiesMask & QMI_LOC_SENSOR_PROPERTIES_MASK_VEHICLE_ANGLE_RWSD_V02))
  {
    pQmiLocGetSensorPropertiesInd->vehicleAngleRandomWalkSpectralDensity_valid = TRUE;
    pQmiLocGetSensorPropertiesInd->vehicleAngleRandomWalkSpectralDensity = pLocEngGetNavConfigResp->vehicle_angle_random_walk_spectral_density; /* float */
  }
  else
  {
    pQmiLocGetSensorPropertiesInd->vehicleAngleRandomWalkSpectralDensity_valid = FALSE;
  }

  if ((FALSE != pLocEngGetNavConfigResp->vehicle_angular_rate_random_walk_spectral_density_valid) &&
      (sensorPropertiesMask & QMI_LOC_SENSOR_PROPERTIES_MASK_VEHICLE_ANGULAR_RATE_RWSD_V02))
  {
    pQmiLocGetSensorPropertiesInd->vehicleAngularRateRandomWalkSpectralDensity_valid = TRUE;
    pQmiLocGetSensorPropertiesInd->vehicleAngularRateRandomWalkSpectralDensity = pLocEngGetNavConfigResp->vehicle_angular_rate_random_walk_spectral_density; /* float */
  }
  else
  {
    pQmiLocGetSensorPropertiesInd->vehicleAngularRateRandomWalkSpectralDensity_valid = FALSE;
  }

  if ((FALSE != pLocEngGetNavConfigResp->vehicle_data_use_valid) &&
      (sensorPropertiesMask & QMI_LOC_SENSOR_PROPERTIES_MASK_VEHICLE_DATA_USE_CONTROL_V02))
  {
    pQmiLocGetSensorPropertiesInd->vehicleDataUse_valid = TRUE;
    pQmiLocGetSensorPropertiesInd->vehicleDataUse = pLocEngGetNavConfigResp->vehicle_data_use; /* locEngVehicleDataUseControlMaskT_v01 --> qmiLocVehicleDataUseControlMaskT_v02 */
  }
  else
  {
    pQmiLocGetSensorPropertiesInd->vehicleDataUse_valid = FALSE;
  }

  if ((FALSE != pLocEngGetNavConfigResp->vehicle_odometry_scale_factor_random_walk_spectral_density_valid) &&
      (sensorPropertiesMask & QMI_LOC_SENSOR_PROPERTIES_MASK_VEHICLE_ODOMETRY_SCALE_RWSD_V02))
  {
    pQmiLocGetSensorPropertiesInd->vehicleOdometryScaleFactorRandomWalkSpectralDensity_valid = TRUE;
    pQmiLocGetSensorPropertiesInd->vehicleOdometryScaleFactorRandomWalkSpectralDensity = pLocEngGetNavConfigResp->vehicle_odometry_scale_factor_random_walk_spectral_density; /* float */
  }
  else
  {
    pQmiLocGetSensorPropertiesInd->vehicleOdometryScaleFactorRandomWalkSpectralDensity_valid = FALSE;
  }

  if ((FALSE != pLocEngGetNavConfigResp->vehicle_odometry_variance_valid) &&
      (sensorPropertiesMask & QMI_LOC_SENSOR_PROPERTIES_MASK_VEHICLE_ODOMETRY_VARIANCE_V02))
  {
    pQmiLocGetSensorPropertiesInd->vehicleOdometryVariance_valid = TRUE;
    pQmiLocGetSensorPropertiesInd->vehicleOdometryVariance = pLocEngGetNavConfigResp->vehicle_odometry_variance; /* float */
  }
  else
  {
    pQmiLocGetSensorPropertiesInd->vehicleOdometryVariance_valid = FALSE;
  }

  if ((FALSE != pLocEngGetNavConfigResp->vehicle_velocity_random_walk_spectral_density_valid) &&
      (sensorPropertiesMask & QMI_LOC_SENSOR_PROPERTIES_MASK_VEHICLE_VELOCITY_RWSD_V02))
  {
    pQmiLocGetSensorPropertiesInd->vehicleVelocityRandomWalkSpectralDensity_valid = TRUE;
    pQmiLocGetSensorPropertiesInd->vehicleVelocityRandomWalkSpectralDensity = pLocEngGetNavConfigResp->vehicle_velocity_random_walk_spectral_density; /* float */
  }
  else
  {
    pQmiLocGetSensorPropertiesInd->vehicleVelocityRandomWalkSpectralDensity_valid = FALSE;
  }

  if ((FALSE != pLocEngGetNavConfigResp->velocity_random_walk_spectral_density_valid) &&
      (sensorPropertiesMask & QMI_LOC_SENSOR_PROPERTIES_MASK_VELOCITY_RANDOM_WALK_SPECTRAL_DENSITY_V02))
  {
    pQmiLocGetSensorPropertiesInd->velocityRandomWalkSpectralDensity_valid = TRUE;
    pQmiLocGetSensorPropertiesInd->velocityRandomWalkSpectralDensity = pLocEngGetNavConfigResp->velocity_random_walk_spectral_density; /* float */
  }
  else
  {
    pQmiLocGetSensorPropertiesInd->velocityRandomWalkSpectralDensity_valid = FALSE;
  }

  /* userdata is extracted outside */

  return TRUE;
}


/**   convertLocEngStatusToQmiError
 
@brief : convert loc Eng status to QMI error .
@param [in]  locEngStatus :  Status in locEng format
 
@retval  corresponding QMI error
------------------------------------------------------------------*/
qmi_error_type_v01 convertLocEngStatusToQmiError(
   locEngStatusEnumT_v01 locEngStatus)
{
  qmi_error_type_v01 qmiErr = QMI_ERR_GENERAL_V01;
  switch (locEngStatus)
  {
     case eLOC_ENG_SUCCESS_V01:
       qmiErr = QMI_ERR_NONE_V01;
       break;
     case eLOC_ENG_INVALID_PARAMETER_V01:
       qmiErr = QMI_ERR_MALFORMED_MSG_V01;
       break;
     case eLOC_ENG_CONFIG_NOT_SUPPORTED_V01:
       qmiErr = QMI_ERR_NOT_SUPPORTED_V01;
       break;
     case eLOC_ENG_ENGINE_BUSY_V01:
       qmiErr = QMI_ERR_DEVICE_IN_USE_V01;
       break;
     case eLOC_ENG_INSUFFICIENT_MEMORY_V01:
       qmiErr =  QMI_ERR_NO_MEMORY_V01;
       break;
     case eLOC_ENG_PHONE_OFFLINE_V01:
       qmiErr = QMI_ERR_NO_RADIO_V01;
       break;
     case eLOC_ENG_TIMEOUT_V01:
       qmiErr = QMI_ERR_INTERNAL_V01;
       break;
     case eLOC_ENG_UNSUPPORTED_V01:
       qmiErr = QMI_ERR_NOT_SUPPORTED_V01;
       break;
     case eLOC_ENG_GENERAL_FAILURE_V01:
     default:
       qmiErr = QMI_ERR_GENERAL_V01;
       break;
  }

  LOC_MSG_MED("convertLocEngToQmiError: in %d, out %d",
              locEngStatus, qmiErr, 0);
  return qmiErr;
}


/** convertQmiLocToLocEngWiFiApDataMask
@brief : convert QMI wifi AP device type enum to Loc Eng. 
@param [in]   apDataMask :  QMI LOC type mask.
@param [out]  pConvertedType :  Pointer to Loc Eng type.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/
// Routine to translate QMI-LOC WiFi AP data mask to LocEng type.
boolean convertQmiLocToLocEngWiFiApDataMask(qmiLocWifiApDataMaskT_v02   apDataMask,
                                            locEngWifiMaskT_v01         *pConvertedType)
{
  boolean retVal = TRUE;

  // pConvertedType is already memseted to 0 in the caller function
  
  if (apDataMask & QMI_LOC_WIFI_APDATA_MASK_AP_TRANSMIT_POWER_V02)
  {
    *pConvertedType |= LOC_ENG_WIFI_APINFO_MASK_AP_TRANSMIT_POWER_V01;
  }

  if (apDataMask & QMI_LOC_WIFI_APDATA_MASK_AP_ANTENNA_GAIN_V02)
  {
    *pConvertedType |= LOC_ENG_WIFI_APINFO_MASK_AP_ANTENNA_GAIN_V01;
  }
  
  if (apDataMask & QMI_LOC_WIFI_APDATA_MASK_AP_SNR_V02)
  {
    *pConvertedType |= LOC_ENG_WIFI_APINFO_MASK_AP_SNR_V01;
  }
  
  if (apDataMask & QMI_LOC_WIFI_APDATA_MASK_AP_DEVICE_TYPE_V02)
  {
    *pConvertedType |= LOC_ENG_WIFI_APINFO_MASK_AP_DEVICE_TYPE_V01;
  }

  if (apDataMask & QMI_LOC_WIFI_APDATA_MASK_AP_RSSI_V02)
  {
    *pConvertedType |= LOC_ENG_WIFI_APINFO_MASK_AP_RSSI_V01;
  }
  
  if (apDataMask & QMI_LOC_WIFI_APDATA_MASK_AP_CHANNEL_V02)
  {
    *pConvertedType |= LOC_ENG_WIFI_APINFO_MASK_AP_CHANNEL_V01;
  }
  
  if (apDataMask & QMI_LOC_WIFI_APDATA_MASK_AP_ROUNDTRIP_DELAY_V02)
  {
    *pConvertedType |= LOC_ENG_WIFI_APINFO_MASK_AP_ROUNDTRIP_DELAY_V01;
  }
  
  if (apDataMask & QMI_LOC_WIFI_APDATA_MASK_AP_ROUNDTRIP_DELAY_ACCURACY_V02)
  {
    *pConvertedType |= LOC_ENG_WIFI_APINFO_MASK_AP_ROUNDTRIP_DELAY_ACCURACY_V01;
  }
  
  if (apDataMask & QMI_LOC_WIFI_APDATA_MASK_MOBILE_SNR_V02)
  {
    *pConvertedType |= LOC_ENG_WIFI_APINFO_MASK_MOBILE_SNR_V01;
  }

  if (apDataMask & QMI_LOC_WIFI_APDATA_MASK_MOBILE_RSSI_V02)
  {
    *pConvertedType |= LOC_ENG_WIFI_APINFO_MASK_MOBILE_RSSI_V01;
  }
  
  return retVal;
}


/** convertQmiLocToLocEngWiFiDeviceTypeEnum
@brief : convert QMI wifi AP device type enum to Loc Eng. 
@param [in]   deviceType :  QMI LOC type enum.
@param [out]  pConvertedType :  Pointer to Loc Eng type.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/
// Routine to translate QMI-LOC WiFi device type enum to LocEng type.
boolean convertQmiLocToLocEngWiFiDeviceTypeEnum(qmiLocWifiApDataDeviceTypeEnumT_v02  deviceType,
                                                locEngWifiApDeviceTypeEnumT_v01      *pConvertedType)
{
  boolean retVal = TRUE;

  switch (deviceType)
  {
     case eQMI_LOC_WIFI_AP_DATA_DEVICE_TYPE_WLAN_802_11_A_V02:
       *pConvertedType = eLOC_ENG_WIFI_AP_DEVICE_TYPE_WLAN_802_11_A_V01;
       break;
     case eQMI_LOC_WIFI_AP_DATA_DEVICE_TYPE_WLAN_802_11_B_V02:
       *pConvertedType = eLOC_ENG_WIFI_AP_DEVICE_TYPE_WLAN_802_11_B_V01;
       break;
     case eQMI_LOC_WIFI_AP_DATA_DEVICE_TYPE_WLAN_802_11_G_V02:
       *pConvertedType = eLOC_ENG_WIFI_AP_DEVICE_TYPE_WLAN_802_11_G_V01;
       break;
     default:
       retVal = FALSE;
       break;
  }
  return retVal;
}


/** convertQmiLocToLocEngWiFiRTDTypeEnum
@brief : convert QMI wifi AP RTD type enum to Loc Eng. 
@param [in]   rtdType :  QMI LOC type enum.
@param [out]  pConvertedType :  Pointer to Loc Eng type.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/
// Routine to translate QMI-LOC WiFi RTD type enum to LocEng type.
boolean convertQmiLocToLocEngWiFiRTDTypeEnum(qmiLocWifiApDataRtdUnitTypeEnumT_v02  rtdType,
                                             locEngWifiApRtdUnitTypeEnumT_v01      *pConvertedType)
{
  boolean retVal = TRUE;

  switch (rtdType)
  {
     case eQMI_LOC_WIFI_AP_DATA_RTD_UNIT_MICROSEC_V02:
       *pConvertedType = eLOC_ENG_WIFI_AP_RTD_UNIT_MICROSEC_V01;
       break;
     case eQMI_LOC_WIFI_AP_DATA_RTD_UNIT_HUNDREDS_OF_NANOSEC_V02:
       *pConvertedType = eLOC_ENG_WIFI_AP_RTD_UNIT_HUNDREDS_OF_NANOSEC_V01;
       break;
     case eQMI_LOC_WIFI_AP_DATA_RTD_UNIT_TENS_OF_NANOSEC_V02:
       *pConvertedType = eLOC_ENG_WIFI_AP_RTD_UNIT_TENS_OF_NANOSEC_V01;
       break;
     case eQMI_LOC_WIFI_AP_DATA_RTD_UNIT_NANOSEC_V02:
       *pConvertedType = eLOC_ENG_WIFI_AP_RTD_UNIT_NANOSEC_V01;
       break;
     case eQMI_LOC_WIFI_AP_DATA_RTD_UNIT_TENTH_OF_NANOSEC_V02:
       *pConvertedType = eLOC_ENG_WIFI_AP_RTD_UNIT_TENTH_OF_NANOSEC_V01;
       break;
     default:
       retVal = FALSE;
       break;
  }
  return retVal;
}


/** convertQmiLocInjectWifiApDataReq
@brief : convert QMI wifi AP scan data request to Loc Eng. 
@param [in]   pQmiLocWifiApDataReq :  Pointer to QMI LOC 
              inject wifi AP data listpayload.
@param [out]  pMotionDataReq :  Pointer to Loc Eng inject wifi 
       AP data listpayload.

@retval TRUE  Successful Translation
@retval FALSE Failed Translation
---------------------------------------------------------------------*/
boolean convertQmiLocInjectWifiApDataReq(const qmiLocWifiApDataStructT_v02  *pQmiLocWifiApDataReq,
                                         locEngWifiApInfoStructT_v01        *pWifiApDataReq,
                                         uint32                             numberOfAps)
{
  uint32 i = 0;
  boolean resultVal = FALSE;

  /* validate input pointers */
  if (NULL == pQmiLocWifiApDataReq ||
      NULL == pWifiApDataReq || 0 == numberOfAps)
  {
    return FALSE;
  }

  memset(pWifiApDataReq, 0, (sizeof(*pWifiApDataReq) * numberOfAps));

  for (i = 0; i < numberOfAps; i++)
  {
    /* use conversion function for bit mask */
    convertQmiLocToLocEngWiFiApDataMask(pQmiLocWifiApDataReq[i].wifiApDataMask,
                                        &pWifiApDataReq[i].wifiInfoMask);

    /* copy mac address */
    (void)memscpy(&pWifiApDataReq[i].mac_address, sizeof(pWifiApDataReq[i].mac_address), &pQmiLocWifiApDataReq[i].macAddress, sizeof(pQmiLocWifiApDataReq[i].macAddress));
    
    pWifiApDataReq[i].ap_transmit_power = pQmiLocWifiApDataReq[i].apTransmitPower;
    pWifiApDataReq[i].ap_antenna_gain = pQmiLocWifiApDataReq[i].apAntennaGain;
    pWifiApDataReq[i].ap_signal_to_noise = pQmiLocWifiApDataReq[i].apSignalToNoise;

    if (pWifiApDataReq[i].wifiInfoMask & LOC_ENG_WIFI_APINFO_MASK_AP_DEVICE_TYPE_V01)
    {
      resultVal = convertQmiLocToLocEngWiFiDeviceTypeEnum(pQmiLocWifiApDataReq[i].apDeviceType,
                                                          &pWifiApDataReq[i].ap_device_type);
      /* if conversion failed, reset the bit in mask */
      if ( FALSE == resultVal )
      {
        pWifiApDataReq[i].wifiInfoMask =
          pWifiApDataReq[i].wifiInfoMask & ~(LOC_ENG_WIFI_APINFO_MASK_AP_DEVICE_TYPE_V01);
      }
    }
    
    pWifiApDataReq[i].ap_rssi = pQmiLocWifiApDataReq[i].apRssi;
    pWifiApDataReq[i].ap_channel = pQmiLocWifiApDataReq[i].apChannel;
    pWifiApDataReq[i].ap_round_trip_delay = pQmiLocWifiApDataReq[i].apRoundTripDelay;

    if (pWifiApDataReq[i].wifiInfoMask & LOC_ENG_WIFI_APINFO_MASK_AP_ROUNDTRIP_DELAY_V01)
    {
      resultVal = convertQmiLocToLocEngWiFiRTDTypeEnum(pQmiLocWifiApDataReq[i].apRoundTripDelayUnit,
                                                       &pWifiApDataReq[i].ap_round_trip_delay_unit);
      /* if conversion failed, reset the bit in mask */
      if ( FALSE == resultVal )
      {
        pWifiApDataReq[i].wifiInfoMask =
          pWifiApDataReq[i].wifiInfoMask & ~(LOC_ENG_WIFI_APINFO_MASK_AP_ROUNDTRIP_DELAY_V01);
      }
    }
    
    pWifiApDataReq[i].ap_round_trip_delay_accuracy = pQmiLocWifiApDataReq[i].apRoundTripDelayAccuracy;
    pWifiApDataReq[i].mobile_signal_to_noise = pQmiLocWifiApDataReq[i].mobileSignalToNoise;
    pWifiApDataReq[i].mobile_rssi = pQmiLocWifiApDataReq[i].mobileRssi;
  }

  return TRUE;
}

/**   convertLocEngPosToBatchPos
 
@brief : convert loc Eng common position to QMI_LOC batched
         position structure.
@param [in]   pLocEngPosition :  Pointer to Loc Eng position
@param [out]  pBatchedPosition    :  Pointer to BAP position

@retval TRUE  Position was successfully translated
@retval FALSE Position was not be translated successfully
---------------------------------------------------------------------*/

boolean convertLocEngPosToBatchedPos
(
  const locEngPositionReportMsgT_v01       *pLocEngPosition,
  qmiLocBatchedReportStructT_v02     *const pBatchedPosition
)
{
  /* check NULL input */
  if (NULL == pLocEngPosition || NULL == pBatchedPosition)
  {
    LOC_MSG_ERROR("convertLocEngPosToBatchedPos: NULL param error, "
                  "pLocEngPosition = %p, pBatchedPosition = %p", 
				  pLocEngPosition, pBatchedPosition, 0);
    return FALSE;
  }

  memset(pBatchedPosition, 0, sizeof(*pBatchedPosition));

  /* copy the optional TLV's */

  if( TRUE == pLocEngPosition->latitude_valid)
  {
	pBatchedPosition->latitude = 
	  pLocEngPosition->latitude;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_LATITUDE_V02;
  }

  if( TRUE == pLocEngPosition->longitude_valid)
  {
	pBatchedPosition->longitude = 
	  pLocEngPosition->longitude;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_LONGITUDE_V02;
  }

  if( TRUE == pLocEngPosition->horUncCircular_valid)
  {
	pBatchedPosition->horUncCircular = 
	  pLocEngPosition->horUncCircular;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_HOR_CIR_UNC_V02;
  }

  if( TRUE == pLocEngPosition->horSpeed_valid)
  {
	pBatchedPosition->speedHorizontal = 
	  pLocEngPosition->horSpeed;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_SPEED_HOR_V02;
  }

 
  if(TRUE ==  pLocEngPosition->speedUnc3d_valid)
  {
	pBatchedPosition->speedUnc = pLocEngPosition->speedUnc3d;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_SPEED_UNC_V02;
  }


  if( TRUE == pLocEngPosition->altitudeWrtEllipsoid_valid)
  {
    pBatchedPosition->altitudeWrtEllipsoid = 
	  pLocEngPosition->altitudeWrtEllipsoid;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_ALT_WRT_ELP_V02;
  }

  if( TRUE == pLocEngPosition->vertSpeed_valid)
  {
	pBatchedPosition->speedVertical = 
	  pLocEngPosition->vertSpeed;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_SPEED_VER_V02;
  }

  if( TRUE == pLocEngPosition->heading_valid)
  {
	pBatchedPosition->heading = pLocEngPosition->heading;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_HEADING_V02;
  }
   
  if( TRUE == pLocEngPosition->headingUnc_valid)
  {
	pBatchedPosition->headingUnc = pLocEngPosition->headingUnc;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_HEADING_UNC_V02;
  }

  if( TRUE == pLocEngPosition->technologyMask_valid)
  {
	pBatchedPosition->technologyMask = convertLocEngMask(
	  techMaskTable, 
	  GNSS_ARRAY_SIZEOF(techMaskTable), 
	  pLocEngPosition->technologyMask );
	
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_TECH_MASK_V02;
  }

  if( TRUE == pLocEngPosition->timestampUtc_valid)
  {
	pBatchedPosition->timestampUtc = pLocEngPosition->timestampUtc;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_TIMESTAMP_UTC_V02;
  }

  if( TRUE == pLocEngPosition->timeUnc_valid)
  {
	pBatchedPosition->timeUnc = pLocEngPosition->timeUnc;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_TIME_UNC_V02;
  }

  if( TRUE == pLocEngPosition->magneticDeviation_valid)
  {
	pBatchedPosition->magneticDeviation = pLocEngPosition->magneticDeviation;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_MAGNETIC_DEV_V02;
  }

  if( TRUE == pLocEngPosition->vertUnc_valid)
  {
	pBatchedPosition->vertUnc = pLocEngPosition->vertUnc;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_VERT_UNC_V02;
  }
 
  if( TRUE == pLocEngPosition->horCircularConfidence_valid)
  {
	pBatchedPosition->horConfidence = pLocEngPosition->horCircularConfidence;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_HOR_CONF_V02;
  }

  if( TRUE == pLocEngPosition->gpsTime_valid)
  {
	pBatchedPosition->gpsTime.gpsTimeOfWeekMs = 
	  pLocEngPosition->gpsTime.gpsTimeOfWeekMs;
	pBatchedPosition->gpsTime.gpsWeek = pLocEngPosition->gpsTime.gpsWeek;
	pBatchedPosition->validFields |= 
	  QMI_LOC_BATCHED_REPORT_MASK_VALID_TIMESTAMP_GPS_V02;
  }

  return TRUE;
}

/**   convertLocEngPosToDbtPosInd
 
@brief : convert loc Eng common position to QMI_LOC DBT
         position structure.
@param [in]   pLocEngPosition :  Pointer to Loc Eng position 
@param [in]   positionType    :  Origin or Tracking Position 
@param [in]   locReqId        :  Request Id associated 
@param [out]  pDbtPositionInd :  Pointer to DBT position 
                                 Indication

@retval TRUE  Position was successfully translated
@retval FALSE Position was not be translated successfully
---------------------------------------------------------------------*/

extern boolean convertLocEngPosToDbtPosInd
(
  const locEngPositionReportMsgT_v01            *pLocEngPosition,
  aonPositionType                               positionType,
  uint32                                        locReqId,
  qmiLocEventDbtPositionReportIndMsgT_v02       *const pDbtPositionInd
  
)
{
  /* check NULL input */
  if (NULL == pLocEngPosition || NULL == pDbtPositionInd)
  {
    LOC_MSG_ERROR("convertLocEngPosToDbtPos: NULL param error, "
                  "pLocEngPosition = %p, pDbtPosition = %p", 
				  pLocEngPosition, pDbtPositionInd, 0);
    return FALSE;
  }

  memset(pDbtPositionInd, 0, sizeof(*pDbtPositionInd));

  if(eLOC_ENG_POS_SESS_STATUS_SUCCESS_V01 == pLocEngPosition->sessionStatus)
  {
    if(pLocEngPosition->timestampUtc_valid)
	   pDbtPositionInd->dbtPosition.timestampUtc = pLocEngPosition->timestampUtc;

	if(pLocEngPosition->latitude_valid)
	   pDbtPositionInd->dbtPosition.latitude = pLocEngPosition->latitude;

	if(pLocEngPosition->longitude_valid)
	   pDbtPositionInd->dbtPosition.longitude = pLocEngPosition->longitude;

	if(pLocEngPosition->horUncEllipseSemiMinor_valid)
	   pDbtPositionInd->dbtPosition.horUncEllipseSemiMinor = 
        pLocEngPosition->horUncEllipseSemiMinor;

	if(pLocEngPosition->horUncEllipseSemiMajor_valid)
	   pDbtPositionInd->dbtPosition.horUncEllipseSemiMajor = 
        pLocEngPosition->horUncEllipseSemiMajor;

	if(pLocEngPosition->horUncEllipseOrientAzimuth_valid)
	   pDbtPositionInd->dbtPosition.horUncEllipseOrientAzimuth = 
        pLocEngPosition->horUncEllipseOrientAzimuth;

	pDbtPositionInd->dbtPosition.speedHorizontal_valid = pLocEngPosition->horSpeed_valid;
	pDbtPositionInd->dbtPosition.speedHorizontal = pLocEngPosition->horSpeed;

	pDbtPositionInd->dbtPosition.altitudeWrtEllipsoid_valid = pLocEngPosition->altitudeWrtEllipsoid_valid;
	pDbtPositionInd->dbtPosition.altitudeWrtEllipsoid = pLocEngPosition->altitudeWrtEllipsoid;

	pDbtPositionInd->dbtPosition.vertUnc_valid = pLocEngPosition->vertUnc_valid ;
	pDbtPositionInd->dbtPosition.vertUnc = pLocEngPosition->vertUnc;

	pDbtPositionInd->dbtPosition.speedVertical_valid = pLocEngPosition->vertSpeed_valid;
	pDbtPositionInd->dbtPosition.speedVertical = pLocEngPosition->vertSpeed;

	pDbtPositionInd->dbtPosition.heading_valid = pLocEngPosition->heading_valid;
	pDbtPositionInd->dbtPosition.heading = pLocEngPosition->heading;
    //Need to add the additional fields required in the indication
    pDbtPositionInd->reqId = locReqId;
	pDbtPositionInd->headingUnc_valid =  pLocEngPosition->headingUnc_valid;
	pDbtPositionInd->headingUnc = pLocEngPosition->headingUnc;
	pDbtPositionInd->speedUnc_valid = pLocEngPosition->speedUnc3d_valid;
    pDbtPositionInd->speedUnc = pLocEngPosition->speedUnc3d;	
	pDbtPositionInd->horConfidence_valid = pLocEngPosition->horEllipticalConfidence_valid;
	pDbtPositionInd->horConfidence = pLocEngPosition->horEllipticalConfidence;
	pDbtPositionInd->vertConfidence_valid = pLocEngPosition->vertConfidence_valid;
	pDbtPositionInd->vertConfidence = pLocEngPosition->vertConfidence;
    pDbtPositionInd->positionType = (AON_POSITION_TYPE_TRACKING == positionType)?
                                        eQMI_LOC_DBT_POSITION_TYPE_TRACKING_V02: 
                                        eQMI_LOC_DBT_POSITION_TYPE_ORIGIN_V02;

    pDbtPositionInd->DOP_valid = pLocEngPosition->DOP_valid;
	pDbtPositionInd->DOP.PDOP = pLocEngPosition->DOP.PDOP;
	pDbtPositionInd->DOP.HDOP = pLocEngPosition->DOP.HDOP;
	pDbtPositionInd->DOP.VDOP = pLocEngPosition->DOP.VDOP;
	pDbtPositionInd->gnssSvUsedList_valid = pLocEngPosition->gnssSvUsedList_valid;
	pDbtPositionInd->gnssSvUsedList_len = pLocEngPosition->gnssSvUsedList_len;
	/* SV  used info */
    memscpy(&pDbtPositionInd->gnssSvUsedList, 
            sizeof(pDbtPositionInd->gnssSvUsedList),
		    &pLocEngPosition->gnssSvUsedList, sizeof(pLocEngPosition->gnssSvUsedList));
    if(pLocEngPosition->technologyMask_valid)
    {
        pDbtPositionInd->positionSrc_valid =  pLocEngPosition->technologyMask_valid;

        if (pLocEngPosition->technologyMask & LOC_ENG_POS_TECH_MASK_SATELLITE_V01)
        {
            pDbtPositionInd->positionSrc = eQMI_LOC_POSITION_SRC_GNSS_V02;
        }
        else if(pLocEngPosition->technologyMask & LOC_ENG_POS_TECH_MASK_CELLID_V01)
        {
            pDbtPositionInd->positionSrc = eQMI_LOC_POSITION_SRC_CELLID_V02;
        }
        else if(pLocEngPosition->technologyMask & LOC_ENG_POS_TECH_MASK_WIFI_V01)
        {
            pDbtPositionInd->positionSrc = eQMI_LOC_POSITION_SRC_WIFI_V02;
        }
        else if(pLocEngPosition->technologyMask & LOC_ENG_POS_TECH_MASK_SENSORS_V01)
        {
            pDbtPositionInd->positionSrc = eQMI_LOC_POSITION_SRC_OTHER_V02;
        }
        else if(pLocEngPosition->technologyMask & LOC_ENG_POS_TECH_MASK_REFERENCE_LOCATION_V01)
        {
            pDbtPositionInd->positionSrc = eQMI_LOC_POSITION_SRC_CELLID_V02;
        }
        else if(pLocEngPosition->technologyMask & LOC_ENG_POS_TECH_MASK_INJECTED_COARSE_POSITION_V01)
        {
            pDbtPositionInd->positionSrc = eQMI_LOC_POSITION_SRC_TERRESTRIAL_V02;
        }
     }
    return TRUE;
  } 
  else 
  {
	
	LOC_MSG_ERROR("convertLocEngPosToDbtPos: Incorrect session status, "
                  "sessionStatus = %d", 
				  pLocEngPosition->sessionStatus, 0, 0);
    return FALSE;
  }
}
