#ifndef LOCATION_COMMON_H
#define LOCATION_COMMON_H
/*=============================================================================

                             Location Common Header file

GENERAL DESCRIPTION
  This file contains the structure definitions and function prototypes 
  relevant for the entire GPS module (essentially the shared structures).

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  


Copyright (c) 2010 - 2012 Qualcomm Technologies Incorporated.
Qualcomm Confidential and Proprietary. All Rights Reserved.

Copyright (c) 2013-2015 Qualcomm Atheros, Inc.
All Rights Reserved.
Qualcomm Atheros Confidential and Proprietary.

Copyright (c) 2015 Qualcomm Technologies, Inc. 
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

=============================================================================*/

/*=============================================================================
       Include Files
=============================================================================*/
#include "gnss_common.h"

/*=============================================================================

                     DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/

/*=============================================================================

       Constants

=============================================================================*/


/*=============================================================================

       Macros

=============================================================================*/
/* Maximum altitude uncertainty in meters */
#define LOC_ALT_MAX_UNC_METERS   (6000.0)
  /* Unknown position uncertainty (m). */
#define LOC_UNKNOWN_UNC          ((FLT)9999000.0)
#define LOC_GPS_WEEK_UNKNOWN     0xFFFF
#define LOC_TIMETICKMSEC_INVALID (0)


/* The following scale factors are used to convert the 2D error ellipse of the 
  EPI to a 39% confidence 2D error ellipse. On arrival in NF, the EPI is a 99% 
  confidence uncertainty. 
  
  In the cases of a CPI and Cell DB a circular uncertainty is received. The 
  scale factor to convert this to a 39% confidence 2D error ellipse is 
  (1/2.14/1.41). The (1/1.41) portion of the scale factor is executed when the
  circular uncertainty is stored as a 2D error ellipse, and the (1/2.14) 
  portion is executed prior to usage of the EPI as a constraint. 
    
  In case of a reference location, a 2D error ellipse is received directly. The 
  scale factor to convert this to a 39% confidence 2D error ellipse is 
  (1/2.14/1.41). 

  For details of the scaling, check ALE QWiki page:
	  http://qwiki.qualcomm.com/qct-gpsone/Uncertainty_Propagation
  */
#define LOC_68_TO_99_PCTCONF_1D_ERR_SF                      (2.57)
#define LOC_99_ERR_ELLIPSE_TO_39_PCTCONF_2D_ERR_ELLIPSE_SF  (1./3.03)
#define LOC_63_CIRCULAR_TO_95_PCTCONF_2D_CIRCULAR_SF        (1.73)
#define LOC_63_CIRCULAR_TO_99_PCTCONF_2D_CIRCULAR_SF        (2.14)
#define LOC_63_CIRCULAR_TO_999_PCTCONF_2D_CIRCULAR_SF       (3.0)
#define LOC_99_CIRCULAR_TO_63_PCTCONF_2D_CIRCULAR_SF        (1.0/LOC_63_CIRCULAR_TO_99_PCTCONF_2D_CIRCULAR_SF)

/* Inverse of the square root of 2 */
#define LOC_INV_SQRT_2              (0.70710678)

/* Inverse of the square root of 3 */
#define LOC_INV_SQRT_3              (0.57735027)

#define LOC_MIN_SIGMA_METERS        (2.5)
#define LOC_MAX_SIGMA_METERS        (10000.0)
#define LOC_MIN_SIGMA_MPS           (0.01)
#define LOC_MAX_SIGMA_MPS           (1000.0)
#define LOC_MIN_SIGMA_MS            (1)
#define LOC_MAX_SIGMA_MS            (100000)

/* Minimum value for propagated position uncertainty */
#define LOC_MIN_PUNC_METERS         (2.5)

/* RSS velocity uncertainty in 3D for external position inputs */
#define LOC_EXTERNAL_VUNC_3D_MPS    (25.0)

/* Air speed model for uncertainty propagation in meters/second */
#define LOC_PROP_AIR_SPEED_MPS      ((FLT)(250.0))

#define LOC_SMALL_DOUBLE (1.0E-18)

// Part of a longer series defined in MGP/PE - this term is used more widely
#define   TWO_TO_6    64.0

//95% conversion factor table constants
#define UNCERTAINTY_CONV_FACTOR_TABLE_ROW_SIZE 6
#define UNCERTAINTY_CONV_FACTOR_TABLE_COL_SIZE 3
#define UNCERTAINTY_CONV_CONFIDENCE_UBOUND 100
#define UNCERTAINTY_CONV_CONFIDENCE_DESIRED 95

/*=============================================================================

       Typedefs

=============================================================================*/
typedef struct
{
  U16 w_GpsWeek;
  U32 q_GpsTimeMs;

  // (angle-rad, meters-along, meters-cross)                                                
  FLT f_ErrorEllipse[3];

  /*Pos Unc in Lat/Lon/Vertical*/
  FLT f_PuncLatLonMeters[2];
  FLT f_PuncVertUncMeters;

  /*Vel Unc in East/North/Vertical*/
  FLT f_VuncEastNorthMps[2];
  FLT f_VuncVertMps;
}loc_InitUncDataStructType;

/*=============================================================================

  FUNCTION      loc_PropagatePuncVelUnc()

  DESCRIPTION   Propagate the initial position and velocity uncertainties of 
                the navigation solution to a specific time using a piecewise
                acceleration & velocity propagation model. The propagated 
                uncertainties given are the 2D error ellipse, RSS of the 3D 
                position uncertainty, and RSS of the 3D velocity uncertainty.

                The confidence levels of all inputs are assumed at 39%. 
                They are all scaled to 99% during propagation and then 
                scaled back to 39% after propagation for output purpose.

  DEPENDENCIES  None

  PARAMETERS    f_PropagatedErrEllipseMeters[]: Propagated Punc as a 2D error 
                                                ellipse ([3]: angle-rad, 
                                                meters-along, meters-cross)
                                                output
                pf_PropagatedPunc3dMeters: 3D RSS propagated Punc, output
                pf_PropagatedVunc3dMps: 3D RSS propagated Vunc. Can be set to 
                                        NULL if not required. If set to NULL, 
                                        other 3  input arguments should be
                                        set to NULL. output
                f_PropagatedVuncEastNorthMps[]: Propagated East and North Vunc.
                                                Can be set to NULL if not 
                                                required. If set to NULL, other
                                                3 input arguments should 
                                                be set to NULL. output
                pf_PropagatedVuncVertMps: Propagated Vertical Vunc. Can be set 
                                          to NULL if not required. If set to 
                                          NULL, other 3 input arguments 
                                          should be set to NULL. output
                f_PropagatedPuncLatLonMeters[]: Propagated Lat/Long uncertainty
                                          This can be NULL as well along with 
                                          other 3 input arguments.  
                f_TimeDiffSec: Time difference of propagation in seconds,
                               input
                pz_InitUnc: Init Uncertainties, input
                            The error ellipse is at 39%
                            The Punc is at 1-sigma
                            The Vunc is at 1-sigma

  RETURN VALUE  None

  SIDE EFFECTS  None

=============================================================================*/
void loc_PropagatePuncVelUnc( FLT f_PropagatedErrEllipseMeters[],
                              FLT *pf_PropagatedPunc3dMeters,
                              FLT *pf_PropagatedVunc3dMps,
                              FLT f_PropagatedVuncEastNorthMps[],
                              FLT *pf_PropagatedVuncVertMps,
                              FLT f_PropagatedPuncLatLonMeters[],
                              FLT f_TimeDiffSec,
                              const loc_InitUncDataStructType *pz_InitUnc );

/*
******************************************************************************
*
 * FUNCTION:
 * loc_GetGeoidalSeparation
 *
 * DESCRIPTION:
 *  This function takes a lat/long in degrees and returns the geoidal separation 
 *  for that point in meter.
 *
 * PARAMETERS:
 *  d_latitude:     Latitude in degrees
 *  d_longitude:    Longitude in degrees
 *
 * RETURN VALUE:
 *  float - geoidal separation
 *
 ******************************************************************************
*/
float loc_GetGeoidalSeparation 
( double d_latitude, 
  double d_longitude
);

/*
 ******************************************************************************
 *
 * FUNCTION:
 * loc_GetConversionFactor
 *
 * DESCRIPTION:
 *  Utility function to calculate the conversion factor to be used to
 *  adjust uncertainties to a 95% confidence level.
 *
 * PARAMETERS:
 *  xValueConfidenceLevel:     Confidence level
 *
 * RETURN VALUE:
 *  float - conversion factor
 *
 ******************************************************************************
*/

float loc_GetConversionFactor(
  uint32 q_ConfidenceLevel);

/*
 ******************************************************************************
 *
 * FUNCTION:
 * loc_CalcDistBetLatLongHaversine
 *
 * DESCRIPTION:
 *  Utility function to calculate the distance between two (lat, lons) using
 *  the haversine formula.
 *
 * PARAMETERS:
 * reg_lat1, reg_long1: Lat, Lon of point one.
 * reg_lat2, reg_long2: Lat, Lon of point two.
 *
 *
 * RETURN VALUE:
 *  distance:
 *
 ******************************************************************************
*/
uint32 loc_CalcDistBetLatLongHaversine(
  int32 reg_lat1, int32 reg_long1,
  int32 reg_lat2, int32 reg_long2
);

#endif /* LOCATION_COMMON_H */
