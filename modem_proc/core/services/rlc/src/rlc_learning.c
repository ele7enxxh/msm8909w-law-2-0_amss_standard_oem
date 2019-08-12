/**
@file rlc_learning.c
@brief
This module implements learning for RLC. 
*/
/*=============================================================================
  Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                   R L C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Private RLC functions for Learning.


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/02/14   psu     Update diag macros using optimized versions.
05/10/13   cp      Initial revision.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "rlc.h"
#include "rlc_v.h"
#include "rlc_os.h"
#include "rlc_learning.h"
#include "rlc_cluster.h"
#include "msg_diag_service.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
rlc_error_type rlc_learning_anomaly_detection_init( rlc_attribute_type *attr_ptr, uint32 size, uint32 numDimensions, uint32 numClusters )
{
  uint32 size_int = size - 1;
  rlc_learning_struct *temp_struct = NULL;
  uint32 i=0;

  /* Check to make sure that the pointer is properly allocated */
  if( attr_ptr == NULL || 
      attr_ptr->learning_ptr == NULL )
  {
    return RLC_FAILED;
  }

    /*Verify that the RLC attribute structure type is for learning*/
  if( attr_ptr->attribute_structure != RLC_ATTRIBUTE_LEARN )
  {
	  return RLC_INVALID_TYPE;
  }

  /* Verify that the size is a power of 2 */
  if( size <= 1 || (size & size_int) != 0 )
  {
    /* This value is not divisible by 2! */
    return RLC_INVALID_SIZE;
  }

  /* Verify number of dimensions */
  if( numDimensions < 1 )
  {
	  return RLC_INVALID_DIMENSIONS;
  }

  /* Verify number of clusters */
  if( numClusters < 1 )
  {
	  return RLC_INVALID_PARAMETER;
  }
   
  /* Setup learning struct */

  temp_struct = (rlc_learning_struct*) rlc_malloc (sizeof(rlc_learning_struct));

  if( temp_struct == NULL )
  {
    return RLC_NO_MEMORY;
  }


  temp_struct->data=NULL; 
  temp_struct->clusters=NULL;
  temp_struct->averages = NULL;
  temp_struct->avg_found = 0;
  temp_struct->std_dev_found = 0;
  temp_struct->std_dev = NULL;
  temp_struct->div_by_shift = 0;
  temp_struct->size = size;
  temp_struct->numClusters = numClusters;
  temp_struct->numDimensions = numDimensions;
  temp_struct->current = 0;
  temp_struct->watermark = 0;
  temp_struct->firstTime = 1;
  temp_struct->full = 0;
  temp_struct->bestCluster = 0; //Cluster with most items
  temp_struct->correction_head = NULL;
  temp_struct->on_cluster = 0;
 
  
  /* This the number of 1's in a row should be our shift here */

  while( (size_int & 0x1) != 0 )
  {
    temp_struct->div_by_shift += 1;
    size_int >>= 1;
  }


  /* Allocate memory for data */

  temp_struct->data = (rlc_data *) rlc_malloc (size * sizeof(rlc_data)); //Allocate rlc_data struct for each data point
  if( temp_struct->data == NULL )
  {
    return RLC_NO_MEMORY;
  }

  for( i=0; i < size; i++ )
  {
	  temp_struct->data[i].array_ptr=NULL;
	  temp_struct->data[i].cluster_num=0;
	  temp_struct->data[i].array_ptr=(uint32 *)rlc_malloc(numDimensions * sizeof(uint32)); //Allocate for each dimensions per data point
	  memset(temp_struct->data[i].array_ptr, 0, numDimensions * sizeof(uint32));
	  if( temp_struct->data[i].array_ptr == NULL )
		  return RLC_NO_MEMORY;
  }


   /* Allocate memory for cluster data */

	temp_struct->clusters=(rlc_cluster_struct *)rlc_malloc(sizeof(rlc_cluster_struct)*numClusters);

	if(temp_struct->clusters==NULL)
	{
		return RLC_NO_MEMORY;
	}

	/* Allocate analysis values */

	for( i=0; i < numClusters; i++ )
	{
		temp_struct->clusters[i].location=(uint32 *)rlc_malloc(numDimensions * sizeof(uint32));
		temp_struct->clusters[i].data_pts=(uint32 *)rlc_malloc(size * sizeof(uint32));
		temp_struct->clusters[i].total=(uint32 *)rlc_malloc(numDimensions * sizeof(uint32));

		if(temp_struct->clusters[i].location == NULL || temp_struct->clusters[i].data_pts == NULL || temp_struct->clusters[i].total == NULL)
			return RLC_NO_MEMORY;
		
		temp_struct->clusters[i].numItems=0;
		memset(temp_struct->clusters[i].data_pts, 0, size * sizeof(uint32));
		memset(temp_struct->clusters[i].location, 0, numDimensions * sizeof(uint32));
		memset(temp_struct->clusters[i].total, 0, numDimensions * sizeof(uint32));
	}
  
	/* Allocate memory for averages */

	temp_struct->averages=(uint32 *)rlc_malloc(numDimensions * sizeof(uint32));

	if(temp_struct->averages == NULL)
	{
		return RLC_NO_MEMORY;
	}

	memset(temp_struct->averages, 0, numDimensions * sizeof(uint32));

	/* Allocate memory for standard deviation */

	temp_struct->std_dev=(uint32 *)rlc_malloc(numDimensions * sizeof(uint32));

	if(temp_struct->std_dev == NULL)
	{
		return RLC_NO_MEMORY;
	}

	memset(temp_struct->std_dev, 0, numDimensions * sizeof(uint32));

	/* Link learning handle */
	
	attr_ptr->learning_ptr->learning_handle = (rlc_learning_handle)temp_struct;

    return RLC_SUCCESS;
}

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
rlc_error_type rlc_learning_anomaly_detection_destroy( rlc_attribute_type *attr_ptr )
{
  rlc_learning_struct *temp_struct;
  uint32 i=0;

  /* Check to make sure that the pointer is properly allocated */
  if( attr_ptr == NULL )
  {
    return RLC_FAILED;
  }

	/* Check to make sure that the pointer is properly allocated */
	if( attr_ptr->learning_ptr == NULL )
	{
		return RLC_FAILED;
	}

  /*Verify that the RLC attribute structure type is for learning*/
  if( attr_ptr->attribute_structure != RLC_ATTRIBUTE_LEARN )
  {
	  return RLC_INVALID_TYPE;
  }

  temp_struct = (rlc_learning_struct *)attr_ptr->learning_ptr->learning_handle;

  if( temp_struct->std_dev != NULL )
    rlc_free( temp_struct->std_dev );

  if( temp_struct->averages != NULL )
    rlc_free( temp_struct->averages );
  
  if( temp_struct->data != NULL )
  {
    for( i = 0; i < temp_struct->size; i++ )
    {
      if( temp_struct->data[i].array_ptr != NULL )
        rlc_free( temp_struct->data[i].array_ptr );
    }
    rlc_free( temp_struct->data );
  }

  if( temp_struct->clusters != NULL )
  {
    for( i = 0; i < temp_struct->numClusters; i++ )
    {
      if( temp_struct->clusters[i].data_pts != NULL )
        rlc_free( temp_struct->clusters[i].data_pts );
      if( temp_struct->clusters[i].location != NULL )
        rlc_free( temp_struct->clusters[i].location );
      if( temp_struct->clusters[i].total != NULL )
        rlc_free( temp_struct->clusters[i].total );
    }
    rlc_free( temp_struct->clusters );
  }

  rlc_free( attr_ptr->learning_ptr->learning_handle );
  attr_ptr->learning_ptr->learning_handle = NULL;

  return RLC_SUCCESS;
}

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
rlc_error_type rlc_learning_cluster( rlc_attribute_type *attr_ptr)
{
	rlc_error_type error;
	uint32 i=0;
	uint32 j=0;
	uint32 divide=0;
  uint32 rounding_err = 0;
	rlc_learning_struct *attrstruct=NULL;
	uint32 difference;
	uint32 bestDifference;
	uint32 bestCluster;
	uint32 change=1;
	uint32 bestItems=0;
	uint32 clustBestItems=0;
	uint32 zero_num=0;
	uint32 zero_ind=0;
	uint32 zero_done=1;
	rlc_correction_list *temp=NULL;
	uint32 num_loops=0;

	RLC_PERF_DEF(start,end);

	if( attr_ptr == NULL )
	{
		return RLC_INVALID_PARAMETER;
	}

	/*Verify that the RLC attribute structure type is for learning */

	if( attr_ptr->attribute_structure != RLC_ATTRIBUTE_LEARN )
	{
	  return RLC_INVALID_TYPE;
	}

	RLC_PERF_START(start,end);

  rlc_lock_mutex( &attr_ptr->attr_mutex );

	attrstruct=(rlc_learning_struct *)(attr_ptr->learning_ptr->learning_handle);

  /* Change to learning */
  attrstruct->firstTime = 0;

  /* Set new watermark */
  attrstruct->watermark = attrstruct->current;

	/* Change state to learning state */
	attr_ptr->state = RLC_STATE_LEARNING;

	attrstruct->on_cluster=1; //Clustering is on

	/** Initialize cluster locations **/

	/* Get average from data */
	if(attrstruct->avg_found == 0)
	{
		error=rlc_get_averages(attr_ptr);

		if(error != RLC_SUCCESS)
		{
			attrstruct->on_cluster=0;
			/* Change state back to normal */
			attr_ptr->state = RLC_STATE_OK;
      rlc_unlock_mutex( &attr_ptr->attr_mutex );
			return error;
		}
	}

	/* Turn on Random Number Generator */
	rlc_os_random_init();
	
	if( attrstruct->numClusters & 0x1 ) //If number of clusters is odd
	{
		for(i=0; i < attrstruct->numDimensions; i++) //Place one at average
		{
			attrstruct->clusters[0].location[i] = attrstruct->averages[i];
		}
			
		for(i=1; i <= attrstruct->numClusters/2; i++)
		{
			for(j=0; j < attrstruct->numDimensions; j++)
			{
				attrstruct->clusters[i].location[j] = rlc_os_random() % attrstruct->averages[j];
			}
		}
			
		for(i=attrstruct->numClusters/2+1; i < attrstruct->numClusters; i++)
		{
			
			for(j=0; j < attrstruct->numDimensions; j++)
				{
					attrstruct->clusters[i].location[j] = (rlc_os_random() % attrstruct->averages[j]) + attrstruct->averages[j];
				}
		}

	}
	else
	{
		for(i=0; i < attrstruct->numClusters/2; i++)
		{
			for(j=0; j < attrstruct->numDimensions; j++)
			{
				attrstruct->clusters[i].location[j] = rlc_os_random() % attrstruct->averages[j];
			}
		}
		
		for(i=attrstruct->numClusters/2; i < attrstruct->numClusters; i++)
		{
			for(j=0; j < attrstruct->numDimensions; j++)
				{
					attrstruct->clusters[i].location[j] = (rlc_os_random() % attrstruct->averages[j]) + attrstruct->averages[j];
				}
		}
	}

	/* Turn off Random Number Generator */
	rlc_os_random_deinit();

	while( change & 0x1 && num_loops<50)
	{
		
		change=0;
		/* Clear cluster-data point associations */

		for(i=0; i < attrstruct->numClusters; i++)
		{
			memset(attrstruct->clusters[i].data_pts, 0, sizeof(uint32)*attrstruct->size);
			attrstruct->clusters[i].numItems=0;
			memset(attrstruct->clusters[i].total, 0, sizeof(uint32)*attrstruct->numDimensions);
		}

		bestItems=0;
		clustBestItems=0;

		/* Determine best cluster locations */
		/* TODO: Perform numAnalyze times and average out cluster locations */

		for(i=0; i < attrstruct->size; i++)	// For each data point
		{
			bestDifference=0xFFFF;
			bestCluster=0;


			for(j=0; j < attrstruct->numClusters; j++)		// For each cluster 
			{
				difference=rlc_get_distance(attrstruct->numDimensions, attrstruct->data[i].array_ptr, attrstruct->clusters[j].location);
			
				if( difference < bestDifference )
				{
					bestDifference=difference;
					bestCluster=j;
				}
			}

			/* Add to cluster location calculation */

			attrstruct->clusters[bestCluster].data_pts[i]=1;
			attrstruct->clusters[bestCluster].numItems+=1;
			attrstruct->data[i].cluster_num=bestCluster;
			for(j=0; j < attrstruct->numDimensions; j++)
			{
				attrstruct->clusters[bestCluster].total[j]+=attrstruct->data[i].array_ptr[j];
			}

			if(attrstruct->clusters[bestCluster].numItems > bestItems)
			{
				clustBestItems=bestCluster;
				bestItems=attrstruct->clusters[bestCluster].numItems;
			}
		}

		/* Update cluster locations */
		zero_num=0;
		zero_done=1;
		for( i=0; i < attrstruct->numClusters; i++ )
		{
			for( j=0; j < attrstruct->numDimensions; j++ )
			{
				if(attrstruct->clusters[i].numItems != 0) // Check if the cluster has associated data points (prevent divide by 0)
				{
          rounding_err = attrstruct->clusters[i].numItems >> 2;
          //if( rounding_err != 0 )
            //rounding_err -= 1;
					divide=( attrstruct->clusters[i].total[j] + rounding_err ) / attrstruct->clusters[i].numItems;
					if( attrstruct->clusters[i].location[j] != divide )
						change=1;

					attrstruct->clusters[i].location[j] = divide;
					//MSG_2(MSG_SSID_DFLT, MSG_LEGACY_HIGH,"Cluster Number: %u     Location: %u", i, divide);
				}
				else if(attrstruct->clusters[i].numItems == 0 && zero_num == 0)
				{
					zero_num=1;	
					zero_ind=i;
					change=1;
					break;
				}
			}
			if(zero_num & 0x0001 && zero_done & 0x0001)
			{
				for ( j=0; j < attrstruct->numDimensions; j++ )
				{
					attrstruct->clusters[zero_ind].location[j] = attrstruct->clusters[clustBestItems].location[j] + 1; 
				}
				zero_done=0;
			}

		}
#ifdef RLC_CLUSTER_DEBUG
    if( attrstruct->numDimensions == 1 )
    {
      MSG_HIGH("Cluster 1:%d Cluster 2:%d Cluster 3:%d\n",attrstruct->clusters[0].location[0],attrstruct->clusters[1].location[0],attrstruct->clusters[2].location[0]);
    }
    if( attrstruct->numDimensions == 2 )
    {
      MSG_HIGH("Cluster 1:%d Cluster 2:%d Cluster 3:%d\n",attrstruct->clusters[0].location[0],attrstruct->clusters[1].location[0],attrstruct->clusters[2].location[0]);
      MSG_HIGH("Cluster 1:%d Cluster 2:%d Cluster 3:%d\n",attrstruct->clusters[0].location[1],attrstruct->clusters[1].location[1],attrstruct->clusters[2].location[1]);
    }
    if( attrstruct->numDimensions == 3 )
    {
      MSG_HIGH("Cluster 1:%d Cluster 2:%d Cluster 3:%d\n",attrstruct->clusters[0].location[0],attrstruct->clusters[1].location[0],attrstruct->clusters[2].location[0]);
      MSG_HIGH("Cluster 1:%d Cluster 2:%d Cluster 3:%d\n",attrstruct->clusters[0].location[1],attrstruct->clusters[1].location[1],attrstruct->clusters[2].location[1]);
      MSG_HIGH("Cluster 1:%d Cluster 2:%d Cluster 3:%d\n",attrstruct->clusters[0].location[2],attrstruct->clusters[1].location[2],attrstruct->clusters[2].location[2]);
    }
#endif /*RLC_CLUSTER_DEBUG*/
	  num_loops++;
	}

#ifdef RLC_CLUSTER_DEBUG
  MSG_HIGH("Number of loops: %d\n",num_loops,0,0);
#endif /*RLC_CLUSTER_DEBUG*/

	attrstruct->firstTime = 0;

	// Find the best cluster (TODO: maybe place somewhere more appropriate)
	error=rlc_find_best_clust(attr_ptr);

	if(error != RLC_SUCCESS)
	{
		attrstruct->on_cluster=0;
    rlc_unlock_mutex( &attr_ptr->attr_mutex );
		return RLC_FAILED;
	}

	/* Change state back to normal */
	attr_ptr->state = RLC_STATE_OK;

	/* Add corrections to queue */
	if(attrstruct->correction_head != NULL)
	{
		temp=attrstruct->correction_head;
		do{
			rlc_correct_sc(rlc_map_external_attribute(temp->correct), RLC_CORRECT_ASYNC_MODE);
			temp=temp->next;
		}
		while(temp != NULL);
	}

	RLC_PERF_END("RLC Clustering Perf: Start:0x%x End:0x%x Diff:%d",start,end);

	rlc_c_wake();

	attrstruct->on_cluster=0;

  rlc_unlock_mutex( &attr_ptr->attr_mutex );

  //MSG_HIGH("Clusteringdone!\n",0,0,0);

	return RLC_SUCCESS;
} 

/** 
Determines the standard deviation of the given data set.

@param[in]	attr_ptr	Pointer to an RLC learning attribute structure.

@return
std_dev -- The standard deviation of the data set.

@dependencies
1) Data set must be filled to size.
*/
rlc_error_type rlc_learning_std_dev( rlc_attribute_type *attr_ptr)
{
	rlc_learning_struct *attrstruct=NULL;
	rlc_error_type error=RLC_FAILED;
	uint32 i;
	uint32 j;
	uint32 sum_deviation;
	uint32 diff;

	/* Check to make sure that the pointer is properly allocated */
	if( attr_ptr == NULL || 
		attr_ptr->learning_ptr == NULL )
	{
	return RLC_FAILED;
	}

	/*Verify that the RLC attribute structure type is for learning*/
	if( attr_ptr->attribute_structure != RLC_ATTRIBUTE_LEARN )
	{
		return RLC_INVALID_TYPE;
	}

	attrstruct=(rlc_learning_struct *)(attr_ptr->learning_ptr->learning_handle);

  rlc_lock_mutex( &attr_ptr->attr_mutex );

  attrstruct->std_dev_found = 0;

	/*Verify that RLC data has been filled*/
	if( attrstruct->full != 1 )
	{
    rlc_unlock_mutex( &attr_ptr->attr_mutex );
		return RLC_FAILED;
	}

	/*Check if averages have been found or not*/
	if(attrstruct->avg_found == 0)
	{
		error=rlc_get_averages(attr_ptr);
		
		if(error != RLC_SUCCESS)
		{
			return error;
		}
	}

	for(i=0; i < attrstruct->numDimensions; i++)
	{
		sum_deviation=0;
		for(j=0; j < attrstruct->size; j++)
		{
			diff=absdiff(attrstruct->data[j].array_ptr[i], attrstruct->averages[i]);
			sum_deviation+=diff*diff;
		}
		sum_deviation=sum_deviation >> attrstruct->div_by_shift;
		attrstruct->std_dev[i]=rlc_os_sqrt(sum_deviation);
	}

  attrstruct->std_dev_found = 1;

  rlc_unlock_mutex( &attr_ptr->attr_mutex );

	return RLC_SUCCESS;

}

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
								uint32 *data)
{	rlc_learning_struct *attrstruct=NULL;
	uint32 i=0;
	rlc_error_type error=RLC_FAILED;


	if(attr_ptr == NULL || size < 1 || data == NULL)
	{
		return RLC_FAILED;
	}

	attrstruct=(rlc_learning_struct *)(attr_ptr->learning_ptr->learning_handle); 
		
	if(attrstruct->numDimensions != size)
		return RLC_FAILED;

	rlc_lock_mutex(&attr_ptr->attr_mutex);
	for(i=0; i < attrstruct->numDimensions; i++)
	{
		attrstruct->data[attrstruct->current].array_ptr[i] = data[i];

	}

  attrstruct->current = ( (attrstruct->current + 1) & (attrstruct->size - 1) );

	/* Check if the size of the data has reached capacity */
	if(attrstruct->current == attrstruct->watermark) //OR if signal sent from correction module to 
	{
		attrstruct->full=1;
		if(attrstruct->on_cluster == 0)
		{
			
			//error=rlc_learning_cluster(attr_ptr); //Add to command queue but not more than 1. check for flag, if 0 cluster, if 1, ignore clustering and move on.
      error=rlc_learn(rlc_map_external_attribute(attr_ptr), RLC_LEARN_ASYNC_MODE);
			if(error != RLC_SUCCESS)
			{
				rlc_unlock_mutex(&attr_ptr->attr_mutex);
				return RLC_FAILED;
			}
		}
		attrstruct->current=0;
	}
	rlc_unlock_mutex(&attr_ptr->attr_mutex);
	return RLC_SUCCESS;
}

/**
Finds cluster with most items.

@param[in]	attr_ptr	Pointer to an RLC learning attribute structure.

@return
RLC_SUCCESS
RLC_FAILURE

@dependencies
1) Clustering performed already
*/

rlc_error_type rlc_find_best_clust( rlc_attribute_type *attr_ptr)
{
	rlc_learning_struct *attrstruct=NULL;
	uint32 i=0;
	uint32 best=0;

	if( attr_ptr == NULL )
		return RLC_INVALID_PARAMETER;

	attrstruct=(rlc_learning_struct *)(attr_ptr->learning_ptr->learning_handle);

	if(attrstruct == NULL)
	{
		return RLC_FAILED;
	}
	
	rlc_lock_mutex(&attr_ptr->attr_mutex);

	while( i < attrstruct->numClusters )
	{
		if(attrstruct->clusters[i].numItems > best)
		{
			best=attrstruct->clusters[i].numItems;
			attrstruct->bestCluster=i;
		}
		i++;
	}
	i=rlc_learning_get_cluster_loc(attr_ptr,attrstruct->bestCluster,0);

	rlc_unlock_mutex(&attr_ptr->attr_mutex);

	return RLC_SUCCESS;

}

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

uint32 rlc_learning_get_cluster_loc( rlc_attribute_type *attr_ptr, uint32 clusterNum, uint32 dimension )
{
	rlc_learning_struct *attrstruct=NULL;

	if( attr_ptr == NULL )
		return RLC_INVALID_PARAMETER;

	attrstruct=(rlc_learning_struct *)(attr_ptr->learning_ptr->learning_handle);

	if(attrstruct == NULL)
	{
		return RLC_FAILED;
	}

	if(dimension >= attrstruct->numDimensions || clusterNum >= attrstruct->numClusters)
	{
		return RLC_INVALID_PARAMETER;
	}

	return attrstruct->clusters[clusterNum].location[dimension];
}
