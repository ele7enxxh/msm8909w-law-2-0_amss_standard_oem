/*============================================================================
  @file aon_dbttransac.cpp

  @brief
    This file contains the implementation for the DBT transaction
 
               Copyright (c) 2014 - 2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               QUALCOMM Proprietary and Confidential.
		  Copyright (c) 2015-2016 Qualcomm Technologies, Inc. All Rights Reserved.
               Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/aon/src/aon_dbttransac.cpp#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  ----------------------------------------------------------- 
01/29/16    ss   Fixed an issue where DBT fixes were not being reported if needOriginLocation is set to FALSE
08/13/15    sj    Added default configuration of GM from NV 
06/11/15    sj    LB 2.0 integration 
04/17/15    sj    Fixed origin location reporting when using cached seed 
03/18/15    sj    Fixed an yield issue with DBT through radius adjustment
03/11/15    sj    Fixed an overflow issue in the reqId 
01/26/15    sj    Added correct transalation for positionType field  
01/19/15    sj    Canned responsiveness to UHR for maxLatency = 1 sec
01/14/15    sj    Removed bread crumb references 
09/17/14    sj    Initial creation of file.


=============================================================================*/
/*****************************************************************************
 * Include Files
 * *************************************************************************/
#include <math.h>
#include "aon_internal.h"
#include "aon_dbttransac.h"
#include "aon_gmproxy.h"
#include "aon_db.h"

AonDbtTransac:: AonDbtTransac(uint32 reqId, uint32 minDistance, 
                  aonDbtDistanceType distType,
                  boolean needOriginLocation,
                  uint32 maxLatency, aonDbtUsageType usageType,
                  AonDatabase* aonDbHdl, AonGmProxy *aonGmProxy,
		          aonLocationCallback locCb, aonStatusCallback statusCb)
                  :AonTransac(reqId, aonDbHdl, aonGmProxy, locCb, statusCb),
                   mMinDistance(minDistance),mDistType(distType),
                   mNeedOriginLocation(needOriginLocation),
                   mMaxLatency(maxLatency), mUsageType(usageType),
				   mLastFixHorSpeed(0), mLastFixHorSpeedIsValid(FALSE)
                      
  { }

/* 
    @brief Retrieve the equivalent background engine client config for AON
    @param[in] gmConfig Background engine client level config object
    @return TRUE or FALSE
    */
boolean AonDbtTransac:: getEqGmClientConfig(AonGmClientConfig &gmConfig)
{
  const aonGmNvDefaultConfigType z_nvConfig = mAonGmProxy->getGmNvDefaultConfig();
  gmConfig.mGnssUnavailAggresiveExit = TRUE;
  gmConfig.mGmBackOffMax = mMaxLatency;
  gmConfig.mGmBackOffMin = mMaxLatency;
  gmConfig.mSessionTimeout = z_nvConfig.q_gnssPositionQosSessionTimeout;
  return TRUE;
}
/* 
    @brief Retrieve the equivalent geofence config for the AON transaction
    @param[out] gfConfig GF Config object that needs to be populated
    @return TRUE or FALSE
    */
boolean AonDbtTransac::getEqGeofenceConfig(AonGeofenceConfig &gfConfig)
{
  gfConfig.mGfResponsivenessInSec = mMaxLatency;
  gfConfig.mResponsiveness = GEOFENCE_RESPONSIVENESS_CUSTOM;
  
  gfConfig.mGfBreachEventType = (sm_gm_geofence_event_type)( 
	  GEOFENCE_EVENT_ENTERING_AREA |
	  GEOFENCE_EVENT_LEAVING_AREA  |
	  GEOFENCE_EVENT_INSIDE_AREA   |
	  GEOFENCE_EVENT_OUTSIDE_AREA  );
  
  AON_MSG_MED("getEqGeofenceConfig invoked. mMinDistance: %d responsiveness sec = %d", 
              mMinDistance,gfConfig.mGfResponsivenessInSec ,0);

  calculateGfRadius(gfConfig.mGfRadius);
  AON_MSG_MED("getEqGeofenceConfig gmConfig radius = %d, HorSpeed * 10k %d ",
	           gfConfig.mGfRadius,(uint32) (mLastFixHorSpeed * 10000), 0);
  return TRUE;
}
/* 
    @brief Breach response handler fn invoked by gmproxy 
    @param[in] gfId Geofence Id
    @param[in] breachType Type of breatch
    @param[in] location  Lcoation object instance adssocited witht his breach
    @return TRUE or FALSE
    */
boolean AonDbtTransac::breachRspHandler(uint32 gfId, aonGMBreachType breachType, locEngPositionReportMsgT_v01 &location)

{
    boolean retVal = FALSE;
	AON_MSG_MED("breachRspHandler invoked. gfId %d breachType %d mNeedOriginLocation %d", 
                gfId, breachType, mNeedOriginLocation);
    //Report location only if 1.First fix pending & client needs origin loc or 2. this is not first fix
    aonPositionType positionType;

    if( breachType == AON_GEOFENCE_EVENT_LEAVING_AREA || 
		breachType ==  AON_GEOFENCE_EVENT_OUTSIDE_AREA)
	{
        if(!mFirstFixPending)
		{
            positionType = AON_POSITION_TYPE_TRACKING;
			mAonLocCb(mReqId, &location, positionType);
		}
        
        //Cache a copy of the speed from the last fix for radius adjustment
        if (location.technologyMask_valid && 
            (location.technologyMask & LOC_ENG_POS_TECH_MASK_SATELLITE_V01))
        {   
            //Consider speed valid only if GNSS fix 
            mLastFixHorSpeed = location.horSpeed;
            mLastFixHorSpeedIsValid = TRUE;
        }

		//Repalce the GF with updated params
		retVal = replaceGF(gfId, location.latitude, location.longitude);

	}

	//Report the origin location for any kind of breach
    if(mFirstFixPending && mNeedOriginLocation)
	{
        positionType = AON_POSITION_TYPE_ORIGIN;
		mAonLocCb(mReqId, &location, positionType);
	}

	/*mFirstFixPending is set to TRUE when first GF is added. 
	Since the breach was received, mFirstFixPending can now be set to False.*/
	mFirstFixPending = FALSE;		
	return retVal;

}

/* -----------------------------------------------------------------------*//**
@brief
  This function is used internally by DBT transaction object to determine the optimum 
  value to use for the radius parameter to be sent to the GM engine for the next geofence.
  Through performance tuning it was determined that in order to maintain the distance 
  traversed between position reports within + or - 15% of the minDistance specified by 
  the client the radius should be calculated as follows
  1. If speed is not valid or less than 1m/sec then
         1.1 Final radius is minDistance reduced by 15% of the minDistance so that we force GM to breach conservatively at the lower bound
           
  2. Else If speed is valid and above 1m sec
         2.1 Calculate the earliest time to the next breach based on the last speed reported. This is actually the whole second before
             or at which the next breach could happen
         2.2 Use this to calculate the estimated distance to this breach event
         2.3 Determine the undershoot radius by calculating the 85% threshold for the estimated distance
         2.4 Use this and speed to calculate the estimated time to breach the undershoot radius and round it up to the next second
             This is becuase we want to align our estimate to the GPS second boundary of position reports
         2.5 Determine the distance travelled to breach the undershoot radius using the estimated time from step 2.4
         2.5 We then evaluate the overshoot radius if we wait for one second after covering the undershoot radius. The idea is to
             not be too conservative and relax our radius constraint if we can still remain within the 115% upper bound
         2.6 If the overshoot radius is lesser than the 115% upper bound we choose the overshoot radius or the undershoot radius
  3. For both cases the radius needs to be reduced by 2m to account for integer arithmetic being exclusively used in GM for radius 
     calculations wherein a few meters maybe lost in round off errors.

@retval  radius 
*//* ------------------------------------------------------------------------*/

void AonDbtTransac::calculateGfRadius(uint32 &radius)
{
    //Intialize the radius to the minDistance and adjust it only for special cases
    int32 adjustedRadius = mMinDistance;

    if (mMinDistance < AON_GF_RADIUS_ADJUSTMENT_THRESHOLD_MAX)
    {
        //Adjust the minDist threshold for the first session or if we have a low speed. 
        if ( !mLastFixHorSpeedIsValid  ||
             (mLastFixHorSpeedIsValid && (mLastFixHorSpeed < 1)))
        {
            //Reduce the radius to the lower bound eg. 85% of requested distnace
            adjustedRadius = ((int32)mMinDistance ) - (((int32)mMinDistance * (100-AON_DBT_DISTANCE_THREHOLD_LOWERBOUND_PERCENTAGE))/ 100) ;

            AON_MSG_MED("calculateGfRadius. at or Nearly static. mMinDistance %d adjustedRadius: %d mLastFixHorSpeedIsValid %d", 
                mMinDistance, adjustedRadius, mLastFixHorSpeedIsValid);  	  
        }
        else 
        //All other cases have horizontal speed which needs to be factored into calcuating the new radius
        {
            uint32 earliestTimeToBreachinSecond = (uint32)( (float)mMinDistance / mLastFixHorSpeed);

            uint32 distTravelledForEarliestBreach = (uint32)((float) earliestTimeToBreachinSecond * mLastFixHorSpeed);

            //Need to get the 85% lower bound of the calculated distance to determine the least radius thredhold we could set to undershoot
            // by 15%. Uint32 is safe here since this can never underflow
            uint32 undershootRadius  = distTravelledForEarliestBreach - 
                                      (distTravelledForEarliestBreach * (100-AON_DBT_DISTANCE_THREHOLD_LOWERBOUND_PERCENTAGE)) / 100;

            //Estimated time to breach adjusted upward to the next second to align our estimate to the GPS second boundary of position reports
            uint32 timeTBreachUndershootRadius   = (uint32) (ceil((float)distTravelledForEarliestBreach / mLastFixHorSpeed));
            //Determine the distance that needs to be travelled to breach at the undershoot radius
            float distanceTravUndershootBreach = (float)timeTBreachUndershootRadius * mLastFixHorSpeed;
            //Then determine at which distance or overshoot radius we would breach if we wait for an additional second
            float overshootRadius  = distanceTravUndershootBreach + (mLastFixHorSpeed * 1);

            //Determine the maximum +15% overshoot limit within which we should report a position
            uint32 distanceOvershootLimit  =  (mMinDistance * AON_DBT_DISTANCE_THREHOLD_UPPERBOUND_PERCENTAGE) / 100 ;

            //Choose the overshoot radius if it is within the max upper bound for the distance specified
            if ( overshootRadius < distanceOvershootLimit)
            {
                adjustedRadius = (int32)overshootRadius;
            }
            else
            {
                adjustedRadius = (int32)undershootRadius;
            }

            AON_MSG_MED("calculateGfRadius. undershootRadius: %d "
                        " overshootRadius: %d*10k distanceOvershootLimit: %d" ,
                        undershootRadius,
                        (overshootRadius * 10000),
                        distanceOvershootLimit);
        }
        /* Due to the integer arithmetic being exclusively used in GM for radius calculations a few meters
             *  maybe lost in round off errors. Becuase of stringent yield requirements for DBT this adjustment
             *  is done before feeding the radius into GM
             */ 
        adjustedRadius -= AON_GM_RADIUS_ROUNDOFF_PREADJUSTMENT;
    }

    radius = (adjustedRadius > AON_MIN_GF_RADIUS)? adjustedRadius: AON_MIN_GF_RADIUS;
    AON_MSG_MED("calculateGfRadius. radius: %d " ,radius, 0, 0);
}





 

