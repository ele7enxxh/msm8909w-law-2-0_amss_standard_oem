/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  Session Manager Utility Module

GENERAL DESCRIPTION
  This module has SM translation function to convert internal nav solution
  structures to gps/gnss structures.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
 
  Copyright (c) 2015 - 2016 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/common/src/sm_util.c#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
8/26/15    mv      CR 895699 - Fixed assistance deletion for BDS and QZSS
06/30/15    jv     Added QZSS Support
09/15/14   jv      Galio GNSS Support
08/21/14   jv      16-bit SVID Support
5/20/14    hs      TDP feature. Rename the gnss- with bcn_ where bcn stands for
                   Beacon
12/04/12   rk      CR 408753 - Use extended SV parameters in FixReport to update 
                   the translation functions. 
10/27/11   rk      CR 308120 - Fix incorrect position engine type in SM generated 
                   0x1476 log.

============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "gps_variation.h"
#include "sm_api.h"
#include "sm_util.h"
#include "mgp_api.h"

static sm_ConstellationCfgType sm_runtime_ccfg = 0;
static sm_ConstellationCfgType sm_compiletime_ccfg = 0;


/**
* @brief  Query supported runtime constellations from MGP and initialize supported SM constellation configuration.
* 
* @return  void
* @sideeffects None
* @see
* 
*/

void sm_init_constellation_config()
{
  uint32 q_GnssCfg = 0;
  uint32 q_SbasCfg = 0;
  sm_ConstellationCfgType cfg = 0;

  if (mgp_GnssGetConstellationConfig( &q_GnssCfg, &q_SbasCfg) == TRUE)
  {
    if ( 0 != (q_GnssCfg & C_RCVR_GNSS_CONFIG_GPS_ENABLED) )
    {
      cfg |= SM_CCFG_RT_GPS;
    }

    if ( 0 != (q_GnssCfg & C_RCVR_GNSS_CONFIG_GLO_ENABLED) )
    {
      cfg |= SM_CCFG_RT_GLO;
    }

    if ( 0 != (q_GnssCfg & (C_RCVR_GNSS_CONFIG_BDS_ENABLED_OUTSIDE_OF_US | C_RCVR_GNSS_CONFIG_BDS_ENABLED_WORLDWIDE) ) )
    {
      cfg |= SM_CCFG_RT_BDS;
    }

    if( 0 != (q_GnssCfg & (C_RCVR_GNSS_CONFIG_GAL_ENABLED_OUTSIDE_OF_US | C_RCVR_GNSS_CONFIG_GAL_ENABLED_WORLDWIDE) ) )
    {
      cfg |= SM_CCFG_RT_GAL;
    }

    if( 0 != (q_GnssCfg & (C_RCVR_GNSS_CONFIG_QZSS_ENABLED_OUTSIDE_OF_US | C_RCVR_GNSS_CONFIG_QZSS_ENABLED_WORLDWIDE) ) )
    {
      cfg |= SM_CCFG_RT_QZSS;
    }
  }
  else
  {
      /*something went wrong, fallback to GPS only */
      cfg=SM_CCFG_RT_GPS;
  }

  sm_set_constellation_config(cfg);
  return;
}


/**
* @brief  Sets supported GNSS constellations
* 
* @return  void
* @sideeffects None
* @see
* 
*/

void 
sm_set_constellation_config(sm_ConstellationCfgType cfg_mask)
{
    sm_compiletime_ccfg = 0;
    sm_compiletime_ccfg |=sm_set_constellation_config_gps();
    sm_compiletime_ccfg |=sm_set_constellation_config_glo();
    sm_compiletime_ccfg |=sm_set_constellation_config_bds();
    sm_compiletime_ccfg |=sm_set_constellation_config_gal();  
    sm_compiletime_ccfg |=SM_CCFG_CT_QZSS; /* Always enabled at compile time */ 

    /*Set only supported configurations */
    sm_runtime_ccfg = cfg_mask&sm_compiletime_ccfg;
 
    if (sm_runtime_ccfg != cfg_mask)
    {
       /* Compile time config is in conflict with runtime config! */
       /* Fallback to GPS only mode */
       sm_runtime_ccfg = SM_CCFG_RT_GPS;
    }
  
    return;
}

/**
* @brief  check if GNSS constellation is supported.
* 
* @return  true if supported, false not supported.
* @sideeffects None
* @see
* 
*/
boolean
sm_is_constellation_supported(sm_ConstellationCfgType cfg_mask)
{
   boolean retval = FALSE;

   if( (cfg_mask&(sm_runtime_ccfg&sm_compiletime_ccfg)) == cfg_mask )
   {   
      retval = TRUE;
   }

   return retval;
}

/**
* sm_is_meas_blk_active
*
* @brief 
*  This function checks if measurement block source is supported by
*  current configuration. Function returns true if eMeasBlkSrc1 == eMeasBlkSrc2
*  and constellation is enabled by software configuration. 
* @param e_MeasBlkSrc1 - first measurement block source for comparison
* @param e_MeasBlkSrc2 - second measurement block source for comparison
* @return
*  TRUE or FALSE
* @sideeffects None
*
*/

boolean sm_is_meas_blk_active(uint32 e_MeasBlkSrc1,uint32 e_MeasBlkSrc2)
{
    boolean b_ret = FALSE;
    sm_ConstellationCfgType cfg = SM_CCFG_NONE;
    
    if(e_MeasBlkSrc1 == e_MeasBlkSrc2)
    {
      if (GNSS_MEAS_BLK_SRC_QZSS_SBAS == e_MeasBlkSrc1)
      {
        /*QZSS and SBAS measurements are tracked for cross-correlation
        even when the constellation is disabled in configuration*/
        b_ret = TRUE;
      }
      else
      {
        switch(e_MeasBlkSrc1)
        {
        case GNSS_MEAS_BLK_SRC_GPS: cfg = SM_CCFG_RT_GPS;break;
        case GNSS_MEAS_BLK_SRC_GLO: cfg = SM_CCFG_RT_GLO;break;
        case GNSS_MEAS_BLK_SRC_BDS: cfg = SM_CCFG_RT_BDS;break;
        case GNSS_MEAS_BLK_SRC_GAL: cfg = SM_CCFG_RT_GAL;break;

        default:
          break;
        }
        if (SM_CCFG_NONE != cfg)
        {
          b_ret = sm_is_constellation_supported(cfg);
        }
      }     
    }

    return b_ret;
}




/**
 * Converts newer gnss time struct to older gps time struct
 * 
 * @param pDest Gen7 GPS time struct type
 * @param pSrc  Gen8 GNSS time struct type
 * 
 * @return boolean TRUE on success, FALSE otherwise
 */
boolean 
sm_translate_gnss_time_to_gps_time(sm_ReportGpsTimeStructType *pDest, 
                                   const sm_ReportGnssTimeStructType *pSrc)
{
   boolean u_retval = (pDest && pSrc);
   if(u_retval)
   {

      pDest->source = pSrc->source; 

      pDest->GpsClock.q_NumClockResets = pSrc->gnssClock.q_NumClockResets;
      pDest->GpsClock.q_RefFCount = pSrc->gnssClock.q_RefFCount;
      pDest->GpsClock.t_SystemRtcMs = pSrc->gnssClock.t_SystemRtcMs;
      pDest->GpsClock.u_SystemRtcValid = pSrc->gnssClock.u_SystemRtcValid;

      pDest->GpsClock.z_Freq.f_ClkFreqBias = pSrc->gnssClock.z_Freq.f_ClkFreqBias;
      pDest->GpsClock.z_Freq.f_ClkFreqUnc = pSrc->gnssClock.z_Freq.f_ClkFreqUnc;

      pDest->GpsClock.z_Time = pSrc->gnssClock.z_Time.z_GpsTime;

   } 

   return u_retval;
} /* end sm_translate_gnss_time_to_gps_time */

/** Translates GNSS assist data to internal "backwards
 *  compatible" struct. This translation is required to minimize
 *  the impact of the changes while providing a cleaner external
 *  interface
 * 
 * @param p_internalAssistData
 * @param p_gnssAssistData
 * 
 * @return boolean
 */
boolean 
sm_translate_gnss_assist_data_to_internal_assist_data( sm_InternalAssistDataStatusStructType*   p_internalAssistData,
                                                       const sm_GnssAssistDataStatusStructType* p_gnssAssistData)
{
   boolean b_retval = FALSE;
   uint8   u_numSV = 0;

   if(p_gnssAssistData && p_internalAssistData)
   {
      b_retval = TRUE;
   }

   if(b_retval)
   {

      memset(p_internalAssistData, 0, sizeof(*p_internalAssistData));


      p_internalAssistData->u_Valid = p_gnssAssistData->w_Valid;
      /* convert GPS data if valid */
      if(SM_ASSIST_DATA_VALID_MASK_GPS & p_gnssAssistData->w_Valid)
      {
         p_internalAssistData->q_AlmHaveList = p_gnssAssistData->z_GpsAssistData.q_AlmHaveList ;
         p_internalAssistData->q_AlmNeedList = p_gnssAssistData->z_GpsAssistData.q_AlmNeedList ;
         p_internalAssistData->q_EphHaveList = p_gnssAssistData->z_GpsAssistData.q_EphHaveList ;
         p_internalAssistData->q_EphNeedList = p_gnssAssistData->z_GpsAssistData.q_EphNeedList ;
         p_internalAssistData->q_SvNoExistMask = p_gnssAssistData->z_GpsAssistData.q_SvNoExistMask ;
         p_internalAssistData->q_VisibleSVList = p_gnssAssistData->z_GpsAssistData.q_VisibleSVList ;
         p_internalAssistData->q_WishListMask = p_gnssAssistData->z_GpsAssistData.q_WishListMask ;
         p_internalAssistData->u_EphInfoIncl = p_gnssAssistData->z_GpsAssistData.u_EphInfoIncl ;
         p_internalAssistData->u_NumSvs = p_gnssAssistData->z_GpsAssistData.u_NumSvs ;

         /* fill up health masks */
         p_internalAssistData->q_HealthHaveList = p_gnssAssistData->z_GpsAssistData.q_HealthHaveList; /* known good list */
         p_internalAssistData->q_HealthBadList = p_gnssAssistData->z_GpsAssistData.q_HealthBadList;   /* known bad list */
         p_internalAssistData->q_HealthNoExList = p_gnssAssistData->z_GpsAssistData.q_SvNoExistMask;  /* known non-existant */
         p_internalAssistData->q_HealthNeedList = p_gnssAssistData->z_GpsAssistData.q_HealthNeedList; /* unknown */

         /* translate structs */

         p_internalAssistData->w_GpsWeek = 
         p_gnssAssistData->z_GpsAssistData.z_AssistDataTime.z_GpsEphTime.w_Week ;

         p_internalAssistData->w_Toe = 
         p_gnssAssistData->z_GpsAssistData.z_AssistDataTime.z_GpsEphTime.w_Toe;

         p_internalAssistData->w_ToeLimit = 
         p_gnssAssistData->z_GpsAssistData.z_AssistDataTime.z_GpsEphTime.w_ToeLimit ;

         /* ok to copy following structs as definitions are the same and do not contain references */
         p_internalAssistData->z_PosUnc = p_gnssAssistData->z_GpsAssistData.z_PosUnc;
         p_internalAssistData->z_TimeUnc = p_gnssAssistData->z_GpsAssistData.z_TimeUnc ;

         for(u_numSV = 0; 
            u_numSV < GNSS_ARRAY_SIZEOF(p_gnssAssistData->z_GpsAssistData.z_SvEphIode); 
            ++u_numSV)
         {
            p_internalAssistData->z_SvEphIode[u_numSV] = p_gnssAssistData->z_GpsAssistData.z_SvEphIode[u_numSV] ;
         }
      } /* end if(SM_ASSIST_DATA_VALID_MASK_GPS & p_gnssAssistData->w_Valid) */

      /* convert GLONASS data if valid */
      if(SM_ASSIST_DATA_VALID_MASK_GLONASS & p_gnssAssistData->w_Valid)
      {
         /* ok to bit-blit as struct definitins are consistent and don't contain pointers */
         p_internalAssistData->z_GloAssistData = p_gnssAssistData->z_GloAssistData;

      } /* end if(SM_ASSIST_DATA_VALID_MASK_GLONASS & p_gnssAssistData->u_Valid) */

      /* convert BDS data if valid */
      if(SM_ASSIST_DATA_VALID_MASK_BDS & p_gnssAssistData->w_Valid)
      {
         /* ok to bit-blit as struct definitins are consistent and don't contain pointers */
         p_internalAssistData->z_BdsAssistData = p_gnssAssistData->z_BdsAssistData;

      } /* end if(SM_ASSIST_DATA_VALID_MASK_BDS & p_gnssAssistData->w_Valid) */

      /* convert QZSS data if valid */
      if(SM_ASSIST_DATA_VALID_MASK_QZSS & p_gnssAssistData->w_Valid)
      {
         /* ok to bit-blit as struct definitins are consistent and don't contain pointers */
         p_internalAssistData->z_QzssAssistData = p_gnssAssistData->z_QzssAssistData;

      } /* end if(SM_ASSIST_DATA_VALID_MASK_QZSS & p_gnssAssistData->w_Valid) */

      /* convert Gal data if valid */
      if(SM_ASSIST_DATA_VALID_MASK_GAL & p_gnssAssistData->w_Valid)
      {
         /* ok to bit-blit as struct definitins are consistent and don't contain pointers */
         p_internalAssistData->z_GalAssistData = p_gnssAssistData->z_GalAssistData;

      } /* end if(SM_ASSIST_DATA_VALID_MASK_GAL & p_gnssAssistData->u_Valid) */


   }

   return b_retval;
} /* end sm_translate_gnss_assist_data_to_internal_assist_data */


/**
 * Fill the SV info into the data arrays of NAV solution structure
 * @param pz_NavSolution: pointer to destination structure to write
 * @param pz_NavSvInfo:   pointer to source input structure to read
 * @param u_GnssIdx:      index indicates the GNSS specific type
 * 
 * @return none
 * @see 
 */
static void FillSvInfoToNavSol
(
  gnss_NavSolutionStructType* pz_NavSolution,
  const sm_GnssNavSvInfo*     pz_NavSvInfo,
  bcn_IdxEnumType            u_GnssIdx
)
{
  const sm_GnssSvInfoType* pz_GnssSvInfo = NULL;
  uint8 u_OffsetMeas   = 0;
  uint8 u_OffsetPosFix = 0;
  uint8 u_OffsetVelFix = 0;
  uint8 u_Length = 0;
  uint8 u_Idx = 0;
  sm_ConstellationCfgType cfg = 0;
 
  /* Sanity check of function arguments */
  if ( (NULL == pz_NavSolution) ||
       (NULL == pz_NavSvInfo)   ||
       (BCN_IDX_GNSS_TOTAL <= u_GnssIdx) )
  {
    return;
  }

  switch (u_GnssIdx)
  {
    case BCN_IDX_GPS:
      pz_GnssSvInfo = &pz_NavSvInfo->z_GpsSvInfo[0];
      u_Length      = ARR_SIZE(pz_NavSvInfo->z_GpsSvInfo);
      cfg = SM_CCFG_RT_GPS;
      break;

    case BCN_IDX_GLO:
      pz_GnssSvInfo = &pz_NavSvInfo->z_GloSvInfo[0];
      u_Length      = ARR_SIZE(pz_NavSvInfo->z_GloSvInfo);
      cfg = SM_CCFG_RT_GLO;
      break;

    case BCN_IDX_BDS:
      pz_GnssSvInfo = &pz_NavSvInfo->z_BdsSvInfo[0];
      u_Length      = ARR_SIZE(pz_NavSvInfo->z_BdsSvInfo);
      cfg = SM_CCFG_RT_BDS;
      break;

    case BCN_IDX_QZSS:
      pz_GnssSvInfo = &pz_NavSvInfo->z_QzssSvInfo[0];
      u_Length      = ARR_SIZE(pz_NavSvInfo->z_QzssSvInfo);
      cfg = SM_CCFG_RT_QZSS;
      break;

    case BCN_IDX_GAL:
      pz_GnssSvInfo = &pz_NavSvInfo->z_GalSvInfo[0];
      u_Length      = ARR_SIZE(pz_NavSvInfo->z_GalSvInfo);
      cfg = SM_CCFG_RT_GAL;
      break;

    default:
      /* error case, not supported */
      return;
  }

  if (sm_is_constellation_supported(cfg) == FALSE)
  {
      return;
  }

  /* calculating Meas/PrMeas/PrrMeas offset */
  for (u_Idx = 0; u_Idx < u_GnssIdx; u_Idx++)
  {
    u_OffsetMeas   += pz_NavSolution->u_NumMeas[u_Idx];
    u_OffsetPosFix += pz_NavSolution->u_NumPrMeas[u_Idx];
    u_OffsetVelFix += pz_NavSolution->u_NumPrrMeas[u_Idx];
  }

  pz_NavSolution->u_NumMeas[u_GnssIdx]    = 0;
  pz_NavSolution->u_NumPrMeas[u_GnssIdx]  = 0;
  pz_NavSolution->u_NumPrrMeas[u_GnssIdx] = 0;

  /* loop through the SV struct to populate arrays */
  for (u_Idx = 0; u_Idx < u_Length; u_Idx++)
  {
    /* measurement block data */
    if ( (pz_GnssSvInfo[u_Idx].q_SvInfoMask & PDSM_GNSS_SV_INFO_MEAS_VALID) &&
         (u_OffsetMeas <  ARR_SIZE(pz_NavSolution->w_MeasSv)) )
    {
      pz_NavSolution->w_MeasSv[u_OffsetMeas]      = pz_GnssSvInfo[u_Idx].w_SvId;  
      pz_NavSolution->b_MeasFreqNum[u_OffsetMeas] = pz_GnssSvInfo[u_Idx].b_FreqNum;
      u_OffsetMeas++;
      pz_NavSolution->u_NumMeas[u_GnssIdx]++;
    }

    /* PR meas for pos fix */
    if ( (pz_GnssSvInfo[u_Idx].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_POS_FIX) &&
         (u_OffsetPosFix <  ARR_SIZE(pz_NavSolution->w_PrMeasSv)) )
    {
      pz_NavSolution->w_PrMeasSv[u_OffsetPosFix]        = pz_GnssSvInfo[u_Idx].w_SvId; 
      pz_NavSolution->f_PrMeasResMeters[u_OffsetPosFix] = pz_GnssSvInfo[u_Idx].f_PrResMeters;
      pz_NavSolution->f_PrMeasUncMeters[u_OffsetPosFix] = pz_GnssSvInfo[u_Idx].f_PrUncMeters;
      pz_NavSolution->b_PrMeasFreqNum[u_OffsetPosFix]   = pz_GnssSvInfo[u_Idx].b_FreqNum;
      pz_NavSolution->w_PrMeasIode[u_OffsetPosFix]      = (u_GnssIdx == BCN_IDX_GLO)  ?
                                                          pz_GnssSvInfo[u_Idx].u_GloTb :
                                                          pz_GnssSvInfo[u_Idx].w_IODE;
      u_OffsetPosFix++;
      pz_NavSolution->u_NumPrMeas[u_GnssIdx]++;
    }

    /* PRR meas for velocity fix */
    if ( (pz_GnssSvInfo[u_Idx].q_SvInfoMask & PDSM_GNSS_SV_INFO_USED_IN_VEL_FIX) &&
         (u_OffsetVelFix <  ARR_SIZE(pz_NavSolution->w_PrrMeasSv)) )
    {
      pz_NavSolution->w_PrrMeasSv[u_OffsetVelFix]     = pz_GnssSvInfo[u_Idx].w_SvId;  
      pz_NavSolution->f_PrrMeasResMps[u_OffsetVelFix] = pz_GnssSvInfo[u_Idx].f_PrrResMps;
      pz_NavSolution->f_PrrMeasUncMps[u_OffsetVelFix] = pz_GnssSvInfo[u_Idx].f_PrrUncMps;
      u_OffsetVelFix++;
      pz_NavSolution->u_NumPrrMeas[u_GnssIdx]++;
    }
  }
}

/**
 * Translates sm's internal nav solution structures to gnss
 * version so that it may be used to generate a position fix log
 * @param p_dest
 * @param p_src
 * 
 * @return boolean
 * @see gnss_LogPositionReport
 */
boolean
sm_translate_sm_fix_report_to_gnss_nav_solution
(
  gnss_NavSolutionStructType*   pz_NavSolution,
  const sm_GpsFixRptStructType* pz_FixReport,
  boolean                       b_FinalFix,
  boolean                       b_backgroundFix
)
{
   boolean b_retval = pz_NavSolution && pz_FixReport;
   if (b_retval)
   {
      /* Translate the gnss_NavSolutionStructType to sm_GnssFixRptStructType */
      pz_NavSolution->w_GpsWeek   = pz_FixReport->z_NavPos.w_GpsWeek;
      pz_NavSolution->q_GpsTimeMs = pz_FixReport->z_NavPos.q_GpsTimeMs;

      // GLONASS time
      pz_NavSolution->u_GloNumFourYear       = pz_FixReport->z_NavPos.u_GloNumFourYear;
      pz_NavSolution->w_GloNumDaysInFourYear = pz_FixReport->z_NavPos.w_GloNumDaysInFourYear;
      pz_NavSolution->q_GloTimeMs            = pz_FixReport->z_NavPos.q_GloTimeMs;

      //pz_FixReport->z_NavPos.q_PosCounter = 0; //no equivalent member?

      pz_NavSolution->q_RefFcount = pz_FixReport->z_NavPos.q_RefFCount;
      pz_NavSolution->q_UtcTimeMs = pz_FixReport->z_NavPos.q_UtcTimeMs;

      memscpy(pz_NavSolution->d_PosEcef, sizeof(pz_NavSolution->d_PosEcef),
             pz_FixReport->z_NavPos.d_PosEcef, 
             sizeof(pz_FixReport->z_NavPos.d_PosEcef) );

      memscpy(pz_NavSolution->d_PosLla, sizeof(pz_NavSolution->d_PosLla), 
             pz_FixReport->z_NavPos.d_PosLla, 
             sizeof(pz_FixReport->z_NavPos.d_PosLla) );

      pz_NavSolution->f_FilteredAlt      = pz_FixReport->z_NavPos.f_FilteredAlt;
      pz_NavSolution->f_FilteredAltUnc   = pz_FixReport->z_NavPos.f_FilteredAltUnc;
      pz_NavSolution->f_UnfilteredAlt    = pz_FixReport->z_NavPos.f_UnfilteredAlt;
      pz_NavSolution->f_UnfilteredAltUnc = pz_FixReport->z_NavPos.f_UnfilteredAltUnc;

      pz_NavSolution->f_Punc3dMeters  = pz_FixReport->z_NavPos.f_Punc3dMeters;
      memscpy(pz_NavSolution->f_PuncLatLonMeters, sizeof(pz_NavSolution->f_PuncLatLonMeters),
             pz_FixReport->z_NavPos.f_PuncLatLonMeters, 
             sizeof(pz_FixReport->z_NavPos.f_PuncLatLonMeters));
      pz_NavSolution->f_PuncVertMeters = pz_FixReport->z_NavPos.f_PuncVertMeters;
      memscpy(pz_NavSolution->f_ErrorEllipse, sizeof(pz_NavSolution->f_ErrorEllipse),
             pz_FixReport->z_NavPos.f_ErrorEllipse, 
             sizeof(pz_FixReport->z_NavPos.f_ErrorEllipse));

      memscpy(pz_NavSolution->d_VelEcef, sizeof(pz_NavSolution->d_VelEcef),
             pz_FixReport->z_NavPos.d_VelEcef, 
             sizeof(pz_FixReport->z_NavPos.d_VelEcef) );
      pz_NavSolution->d_VelEnu[0]     = pz_FixReport->z_NavPos.f_VelEnu[0];
      pz_NavSolution->d_VelEnu[1]     = pz_FixReport->z_NavPos.f_VelEnu[1];
      pz_NavSolution->d_VelEnu[2]     = pz_FixReport->z_NavPos.f_VelEnu[2] ;

      pz_NavSolution->f_Vunc3dMps           = pz_FixReport->z_NavPos.f_Vunc3dMps;
      pz_NavSolution->f_VuncEastNorthMps[0] = pz_FixReport->z_NavPos.f_VuncEastNorthMps[0];
      pz_NavSolution->f_VuncEastNorthMps[1] = pz_FixReport->z_NavPos.f_VuncEastNorthMps[1];
      pz_NavSolution->f_VuncVertMps         = pz_FixReport->z_NavPos.f_VuncVertMps;

      pz_NavSolution->f_HeadingRad    = pz_FixReport->z_NavPos.f_HeadingRad;  
      pz_NavSolution->f_HeadingUncRad = pz_FixReport->z_NavPos.f_HeadingUncRad;
      pz_NavSolution->f_GnssHeadingRad      = pz_FixReport->z_NavPos.f_GnssHeadingRad;
      pz_NavSolution->f_GnssHeadingUncRad   = pz_FixReport->z_NavPos.f_GnssHeadingUncRad;

      pz_NavSolution->f_ClockBiasMs         = pz_FixReport->z_NavPos.f_ClockBias;
      pz_NavSolution->f_ClockBiasUncMs      = pz_FixReport->z_NavPos.f_ClockBiasUncMs;
      pz_NavSolution->f_ClockFreqBiasMps    = pz_FixReport->z_NavPos.f_ClockDriftRate;
      pz_NavSolution->f_ClockFreqBiasUncMps = pz_FixReport->z_NavPos.f_ClockDriftRateUncMps;
      pz_NavSolution->f_FilteredAlt         = pz_FixReport->z_NavPos.f_FilteredAlt;
      pz_NavSolution->f_FilteredAltUnc      = pz_FixReport->z_NavPos.f_FilteredAltUnc;

      memscpy(pz_NavSolution->z_InterGnssTb, sizeof(pz_NavSolution->z_InterGnssTb),
              pz_FixReport->z_NavPos.z_InterGnssTb, sizeof(pz_FixReport->z_NavPos.z_InterGnssTb));

      pz_NavSolution->f_HDOP          = pz_FixReport->z_NavPos.f_HDOP;
      pz_NavSolution->f_VDOP          = pz_FixReport->z_NavPos.f_VDOP;
      pz_NavSolution->f_PDOP          = pz_FixReport->z_NavPos.f_PDOP;

      pz_NavSolution->f_MagDevDegrees = pz_FixReport->z_NavPos.f_MagDeviation;

      pz_NavSolution->b_HaveFirstFix = (pz_FixReport->z_NavPos.u_FirstFix != 0);
      //pz_FixReport->z_NavPos.u_IonoCorrected = TRUE; //no equivalent member
      //pz_FixReport->z_NavPos.u_MagDevGood = FALSE; //no equivalent member
      //pz_FixReport->z_NavPos.u_TropoCorrected = TRUE;       // no equivalent member
      //pz_FixReport->z_NavPos.u_DriftEstimated = FALSE; //no equivalent member

      {
        bcn_IdxEnumType e_SysIdx = BCN_IDX_GPS;
        for (e_SysIdx = BCN_IDX_GPS; e_SysIdx < BCN_IDX_GNSS_TOTAL; e_SysIdx++)
        {
          FillSvInfoToNavSol(pz_NavSolution,
            &pz_FixReport->z_NavSvInfo, e_SysIdx);
        }
      }

      /* copy over existing Nav Solution Flags */
      pz_NavSolution->z_NavSolutionFlags = pz_FixReport->z_NavPos.z_PosFlags;

      /* indicate that this is a final fix or not */
      pz_NavSolution->z_NavSolutionFlags.b_IsFinal = SM_UTIL_MAKE_BOOL(b_FinalFix);

      /* indicate that this fix was NOT generated by PE */
      pz_NavSolution->z_NavSolutionFlags.b_IsPeGenerated = FALSE;

      /* indicate that this fix is NOT generated by ME */
      pz_NavSolution->z_NavSolutionFlags.b_IsMeGenerated = FALSE;

      /* indicate that this fix is a background fix (ODP) or not */
      pz_NavSolution->z_NavSolutionFlags.b_IsBackground = SM_UTIL_MAKE_BOOL(b_backgroundFix);

      pz_NavSolution->q_SensorAidMask       = (uint32)pz_FixReport->z_NavPos.w_SensorAidingIndicator;
      pz_NavSolution->q_SensorDataUsageMask = (uint32)pz_FixReport->z_NavPos.w_SensorDataUsageMask;
      pz_NavSolution->e_HoriRelIndicator    = pz_FixReport->z_NavPos.e_HoriRelIndicator;
      pz_NavSolution->e_VertRelIndicator    = pz_FixReport->z_NavPos.e_VertRelIndicator;
      
      /* SV extension fields */
      pz_NavSolution->z_SvUsageInfo.z_UsedGnssSvMask.q_GpsPrnMask = pz_FixReport->z_NavSvInfo.q_GpsSvMaskUsed;
      pz_NavSolution->z_SvUsageInfo.z_UsableGnssSvMask.q_GpsPrnMask = pz_FixReport->z_NavSvInfo.q_GpsSvMaskUsable;

      pz_NavSolution->z_SvUsageInfo.z_UsedGnssSvMask.q_GloSlotIdMask = pz_FixReport->z_NavSvInfo.q_GloSvMaskUsed;
      pz_NavSolution->z_SvUsageInfo.z_UsableGnssSvMask.q_GloSlotIdMask = pz_FixReport->z_NavSvInfo.q_GloSvMaskUsable;

      pz_NavSolution->z_SvUsageInfo.z_UsedGnssSvMask.t_BdsPrnMask = pz_FixReport->z_NavSvInfo.t_BdsSvMaskUsed;
      pz_NavSolution->z_SvUsageInfo.z_UsableGnssSvMask.t_BdsPrnMask = pz_FixReport->z_NavSvInfo.t_BdsSvMaskUsable;

      pz_NavSolution->z_SvUsageInfo.z_UsedGnssSvMask.u_QzssPrnMask = pz_FixReport->z_NavSvInfo.u_QzssSvMaskUsed;
      pz_NavSolution->z_SvUsageInfo.z_UsableGnssSvMask.u_QzssPrnMask = pz_FixReport->z_NavSvInfo.u_QzssSvMaskUsable;

      pz_NavSolution->z_SvUsageInfo.z_UsedGnssSvMask.t_GalPrnMask = pz_FixReport->z_NavSvInfo.t_GalSvMaskUsed;
      pz_NavSolution->z_SvUsageInfo.z_UsableGnssSvMask.t_GalPrnMask = pz_FixReport->z_NavSvInfo.t_GalSvMaskUsable;

      /* Sensor (gyro/accel) bias parameter struct */
      pz_NavSolution->z_OnDeviceSensorParam.w_SeqenceNumber = pz_FixReport->z_OnDeviceSensorParam.w_SeqenceNumber;
      if ( pz_FixReport->z_OnDeviceSensorParam.z_AccelBias.w_SensorValidMask != 0 )
      {
        pz_NavSolution->z_OnDeviceSensorParam.z_AccelBias.w_SensorValidMask = pz_FixReport->z_OnDeviceSensorParam.z_AccelBias.w_SensorValidMask;
        pz_NavSolution->z_OnDeviceSensorParam.z_AccelBias.f_XaxisValue = pz_FixReport->z_OnDeviceSensorParam.z_AccelBias.f_XaxisValue;
        pz_NavSolution->z_OnDeviceSensorParam.z_AccelBias.f_YaxisValue = pz_FixReport->z_OnDeviceSensorParam.z_AccelBias.f_YaxisValue;
        pz_NavSolution->z_OnDeviceSensorParam.z_AccelBias.f_ZaxisValue = pz_FixReport->z_OnDeviceSensorParam.z_AccelBias.f_ZaxisValue;
      }

      if ( pz_FixReport->z_OnDeviceSensorParam.z_GyroBias.w_SensorValidMask != 0 )
      {
        pz_NavSolution->z_OnDeviceSensorParam.z_GyroBias.w_SensorValidMask = pz_FixReport->z_OnDeviceSensorParam.z_GyroBias.w_SensorValidMask;
        pz_NavSolution->z_OnDeviceSensorParam.z_GyroBias.f_XaxisValue = pz_FixReport->z_OnDeviceSensorParam.z_GyroBias.f_XaxisValue;
        pz_NavSolution->z_OnDeviceSensorParam.z_GyroBias.f_YaxisValue = pz_FixReport->z_OnDeviceSensorParam.z_GyroBias.f_YaxisValue;
        pz_NavSolution->z_OnDeviceSensorParam.z_GyroBias.f_ZaxisValue = pz_FixReport->z_OnDeviceSensorParam.z_GyroBias.f_ZaxisValue;
      }
   }

   return b_retval;
}

/**
 * Translates sm's new SVInfo structure into the old array which
 * is defined in gnss nav solution structure so 
 * that it may be used to generate old-style log 
 * @param p_destSVInfo
 * @param p_srcSVInfo 
 * @param q_validBitmask  all bits must be present (if more than 
 *                        one bit)
 * 
 * @return uint32  number of SVs populated in destSVInfo array
 */

uint32
sm_generate_sv_array_from_svinfo_struct(
   uint16 *p_destSV,   
   uint32 q_destMaxNum,
   const  sm_GnssSvInfoType *p_srcSVInfo,
   uint32 q_srcMaxNum,
   uint32 q_validBitmask) 
{
  uint32 i, counter = 0;

  if (q_destMaxNum == 0 || p_destSV == NULL || q_srcMaxNum == 0 || p_srcSVInfo == NULL ||
      q_validBitmask == 0)
  {
    return 0;  /* invalid input, nothing will be populated */
  }

  for (i = 0; i < q_srcMaxNum; i++)
  {
    if ((p_srcSVInfo[i].q_SvInfoMask & q_validBitmask) == q_validBitmask)
    {
      p_destSV[counter] = p_srcSVInfo[i].w_SvId; 
      if (++counter >= q_destMaxNum)
      {
        break;
      }
    } // if
  }

  return counter;
};

/**
 * Translates sm's new SVInfo structure into the 8-bit old array which
 * is defined in gnss nav solution structure so 
 * that it may be used to generate old-style log 
 * @param p_destSVInfo
 * @param p_srcSVInfo 
 * @param q_validBitmask  all bits must be present (if more than 
 *                        one bit)
 * 
 * @return uint32  number of SVs populated in destSVInfo array
 */

uint32
sm_generate_sv_array8_from_svinfo_struct(
   uint8 *p_destSV,
   uint32 q_destMaxNum,
   const  sm_GnssSvInfoType *p_srcSVInfo,
   uint32 q_srcMaxNum,
   uint32 q_validBitmask)
{
  uint32 i, counter = 0;

  if (q_destMaxNum == 0 || p_destSV == NULL || q_srcMaxNum == 0 || p_srcSVInfo == NULL ||
      q_validBitmask == 0)
  {
    return 0;  /* invalid input, nothing will be populated */
  }

  for (i = 0; i < q_srcMaxNum; i++)
  {
    if ((p_srcSVInfo[i].q_SvInfoMask & q_validBitmask) == q_validBitmask)
    {
      p_destSV[counter] = (uint8)p_srcSVInfo[i].w_SvId;
      if (++counter >= q_destMaxNum)
      {
        break;
      }
    } // if
  }

  return counter;
};
