#ifndef RLC_LEARNING_H
#define RLC_LEARNING_H
/**
@file rlc_learning.h
@brief
This module implements the Learning module in RLC. 
*/
/*=============================================================================
  Copyright (c) 2013 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                   R L C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Learning functions for the RLC module.


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/10/13   cp      Initial revision.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "rlc.h"
#include "rlc_v.h"



/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/



typedef struct {
	uint32 *location; // [dimensions]
	uint32 *data_pts; //Number of data points 0/1 on/off [size]
	uint32 numItems;
	uint32 *total; // [dimensions]
}
rlc_cluster_struct;

typedef struct {
	uint32 *array_ptr; //array_ptr[dimensions]
	uint32 cluster_num;
}
rlc_data;


typedef struct _rlc_correction_list{
	rlc_attribute_type *correct;
	struct _rlc_correction_list *next;
}
rlc_correction_list;

typedef struct {
  uint32  size;
  uint32  numDimensions;
  uint32  numClusters;
  uint32  div_by_shift;
  rlc_data *data; //data[size]
  uint32  *averages;
  uint32  avg_found;
  uint32  std_dev_found;
  rlc_cluster_struct *clusters;
  uint32  *std_dev; //std_dev[dimensions]
  rlc_boolean  firstTime;
  rlc_boolean  full;
  uint32 current;
  uint32 watermark;
  uint32 bestCluster;
  rlc_correction_list *correction_head;
  uint32 on_cluster;
}
rlc_learning_struct;



/*=============================================================================

								MACROS

=============================================================================*/



/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/** 
Sets up learning information for a learning attribute.

@param[in]  attr_ptr		Pointer to an RLC learning attribute structure.
@param[in]  size			Size of data needed to be stored.
@param[in]	numDimensions	Number of dimensions.
@param[in]	numClusters		Number of clusters

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_NO_MEMORY -- No memory available to create the handle 
RLC_INVALID_SIZE -- Invalid size passed in, failure case
 
@dependencies  
None.
*/
rlc_error_type rlc_learning_anomaly_detection_init( rlc_attribute_type *attr_ptr, uint32 size, uint32 numDimensions, uint32 numClusters);

/** 
Destroys anomaly detection attribute.

@param[in]  attr_ptr		Pointer to an RLC learning attribute structure.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_NO_MEMORY -- No memory available to create the handle 
RLC_INVALID_SIZE -- Invalid size passed in, failure case
 
@dependencies  
None.
*/
rlc_error_type rlc_learning_anomaly_detection_destroy( rlc_attribute_type *attr_ptr );

/**
Determines cluster locations based upon given data set.

@param[in]  attr_ptr  Pointer to an RLC learning attribute structure.

@return
RLC_SUCCESS
RLC_NO_MEMORY
RLC_FAILURE

@dependencies
1) Data set must be filled to size.
*/
rlc_error_type rlc_learning_cluster( rlc_attribute_type *attr_ptr);

/** 
Determines the standard deviation of the given data set.

@param[in]	attr_ptr	Pointer to an RLC learning attribute structure.

@return
RLC_SUCCESS
RLC_FAILURE

@dependencies
1) Data set must be filled to size.
*/
rlc_error_type rlc_learning_std_dev( rlc_attribute_type *attr_ptr);

/**
Adds data to attribute

@param[in]	attr_ptr	Pointer to an RLC learning attribute structure.
@param[in]	size		Size of data to be added
@param[in]  data		Pointer to data to be added.

@return
RLC_SUCCESS
RLC_FAILURE

@dependencies
1) Data set must not be filled.
*/
rlc_error_type rlc_learning_add( rlc_attribute_type *attr_ptr,
								uint32 size,
								uint32 *data);

/**
Finds cluster with most items.

@param[in]	attr_ptr	Pointer to an RLC learning attribute structure.

@return
RLC_SUCCESS
RLC_FAILURE

@dependencies
1) Clustering performed already
*/

rlc_error_type rlc_find_best_clust( rlc_attribute_type *attr_ptr);

/**
Gets cluster location dimension value

@param[in]	attr_ptr	Pointer to an RLC learning attribute structure
@param[in]	clusterNum	Cluster number
@param[in]	dimension	Dimension of which value you want (first=0)

@return
uint32 -- Value of cluster location at specified dimension

@dependencies
1) Chosen dimension must be less than the number of dimensions
*/

uint32 rlc_learning_get_cluster_loc( rlc_attribute_type *attr_ptr, uint32 clusterNum, uint32 dimension );


/**
Gets averages of n-dimensional array

@param[in]	data			2-Dimensional array containing the data values for each dimension.
@param[in]	numDimensions	The number of dimensions of data values.
@param[in]	size			Number of data points.
@param[in]	div_shift		Number of times to divide by to get average.
@param[out] averages		Array storing the resulting averages.

@return
RLC_SUCCESS
RLC_NO_MEMORY

@dependencies
1) Need data
*/

rlc_error_type rlc_get_averages(rlc_attribute_type *attr_ptr);

#endif /*RLC_LEARNING_H*/
