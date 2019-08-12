#ifndef CALIB_EVAL_H
#define CALIB_EVAL_H

/*============================================================================
@file calib_eval.h

Calibration evaluation header file 

Copyright (c) 2013, 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.
============================================================================*/

/* $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/Sensors/scm/algo/inc/calib_eval.h#1 $ */
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

#define CALIB_EVAL_COVMAT_COL_NUM (4)

/*=========================================================================
FUNCTION:  calib_eval_statistics
=========================================================================*/
/*!
@brief
Computes statistics associated with the quality of hard iron calibration

@param[i] matX: pointer to pointer to float containing N * 3 array
of triaxial sensor data points for which the quality of
calibration needs to be evaluated
@param[i] rowsX: the number of sensor data points.
@param[i] radius: radius of the estimated sphere
@param[i] mag_noise_std_for_variance: magnetometer noise standard deviation
@param[i] cov_mat: offset adjusted sensor data covariance
@param[o] min_3d_span: computed minimum 3D span
@param[o] max_3d_span: computed maximum 3D span
@param[o] rel_data_span_min: computed relative minimum 3D span to radius
@param[o] rel_data_span_max: computed minimum 3D span to radius

@return boolean set to true if evaluation was succesful,
false otherwise
*/
/*=======================================================================*/
bool calib_eval_statistics(float **matX, uint32_t rowsX, 
                           float radius, 
#ifdef COMPUTE_HI_COV_MAT
                           float mag_noise_std_for_variance,
                           float cov_mat[CALIB_EVAL_COVMAT_COL_NUM][CALIB_EVAL_COVMAT_COL_NUM],
#endif
                           float *min_3d_span, float *max_3d_span,
                           float *rel_data_span_min, float *rel_data_span_max);
#endif /* CALIB_EVAL_H */
