#ifndef CURVE_FIT_H
#define CURVE_FIT_H

/*============================================================================
@file curve_fit.h

Curve fit header file 

Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/scm/algo/inc/curve_fit.h#1 $ */
/* $DateTime: 2016/12/13 08:00:23 $ */
/* $Author: mplcsds1 $ */

/*============================================================================
EDIT HISTORY

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order. 

when        who  what, where, why
----------  ---  -----------------------------------------------------------
2015-03-26  ps   Updated license header
2013-06-13  ks   initial version

============================================================================*/

#include <stdint.h>
#include <math.h>
#include "stdbool.h"
#include "float.h"
#include "matrix_float.h"

#define CURVE_FIT_3D_COL (3)

/*=========================================================================
FUNCTION:  curve_fit_spheroid
=========================================================================*/
/*!
@brief
Estimate a spheroid that best fits the triaxial sensor data points

@param[i] data: pointer to pointer to float containing N * 4 array
of triaxial sensor data points for which the spheroidal fit needs
to be computed. The 3 columns should contain measurements from the
to the X,Y,Z axes of the sensor. The last column should contain 1.0
@param[i] data_length: the number of sensor data points.
@param[o] offset: offset of the estimated spheroid from origin
@param[o] radius: radius of the estimated sphere
@param[o] rel_rad_std: standard deviation of the fit residual error relative to radius
@param[o] rel_rad_std: span of the fit residual error relative to radius
@param[o] cov_mat: offset adjusted data covariance

@return boolean set to true if spheroidal fit was succesful, false otherwise
*/
/*=======================================================================*/

bool curve_fit_spheroid(float **data, float ** measurements, 
                        float **curve_fit_buffer_16_by_4,
                        matrix_inversion_buffer_s *matrix_inversion_buff_p,
                        uint32_t data_length,
                        float offset[CURVE_FIT_3D_COL], float *radius,
                        float *rel_rad_std, float *rel_rad_span
#ifdef COMPUTE_HI_COV_MAT
                        ,float cov_mat[CURVE_FIT_3D_COL+1][CURVE_FIT_3D_COL+1]
#endif
);

#endif /* CURVE_FIT_H */

