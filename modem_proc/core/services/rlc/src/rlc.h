#ifndef RLC_H
#define RLC_H
/**
@file rlc.h
@brief
This module implements the Reporting, Learning, and Correction modules. 
*/
/*=============================================================================
  Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                   R L C   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Top level include file for all RLC modules.


=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
7/16/13    as	   Added learning functionality
11/15/12   cp      Initial revision.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include <stdarg.h>


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

/** Defines the maximum length of variable names in characters */
#define RLC_VARIABLE_NAME_SIZE 16

/** Defines the maximum length of handle names in characters */
#define RLC_HANDLE_NAME_MAX_SIZE 16

/** Defines the maximum length of report buffers */
#define RLC_REPORT_BUF_SIZE 128

/** Defines the maximum length of report buffers */
#define RLC_ATTR_TYPE_SET_LENGTH 1024

/** Defines the possible values that rlc_boolean can take on */
#define M_B_FALSE 0
#define M_B_TRUE  1

/** Defines the rlc_boolean datatype used for boolean-style return types */
typedef uint32 rlc_boolean;

/** Defines an enumeration for different types of reporting that are possible 
    for reporting functions     
*/
typedef enum{
  RLC_REPORT_OUT_NONE = 0,
  RLC_REPORT_OUT_PRINTF,
  RLC_REPORT_OUT_FILE,
  RLC_REPORT_OUT_QXDM,
  RLC_REPORT_OUT_EFS,
  RLC_REPORT_OUT_CALLBACK,
  RLC_REPORT_OUT_QDSS_TRACER,
  RLC_REPORT_OUT_DATA_PACKET,
  RLC_REPORT_OUT_SMD
}
rlc_report_output_type;

/** Defines an enumeration for different types of learning that are possible 
    for rlc
*/
typedef enum{
  RLC_LEARNING_CLUSTERING = 0,
  RLC_LEARNING_STATISTICS,
}
rlc_learning_type;

/** Defines an enumeration for internal processing for Learning
*/
typedef enum{
  RLC_LEARNING_DATA = 0,
  RLC_LEARN,
}
rlc_learning_internal_type;

/** Defines an enumeration for different states for RLC
*/
typedef enum{
  RLC_STATE_OK = 0,
  RLC_STATE_LEARNING,
}
rlc_attr_state;

/** Defines an enumeration for different types of dynamic correction */
typedef enum{
  RLC_CORR_NONE = 0,
  RLC_CORR_PERFORM_CALLBACK,
  RLC_CORR_SET_VAR_BC, //Set best cluster
  RLC_CORR_SET_VAR_WC, //Set worst cluster (NOT IMPLEMENTED YET)
}
rlc_dynamic_correction_type;

/** Defines an opaque handle for client attributes */
typedef void* rlc_external_attribute_type;

/** Defines common datatype for 64-bit data passed into or returned from RLC */
typedef unsigned long long rlc_data_type_64;

/** Defines the external handle for clients using RLC */
typedef struct{
  void                            *internal_handle;
  char                            name[RLC_HANDLE_NAME_MAX_SIZE];           
}
rlc_handle_type;

/** Defines a pointer to the client handle */
typedef rlc_handle_type* rlc_handle_ptr;

/** Defines the error type returned by RLC APIs */
typedef enum{
  RLC_FAILED = 0,
  RLC_SUCCESS,
  RLC_NO_MEMORY,
  RLC_INVALID_PARAMETER,
  RLC_NOT_IMPLEMENTED,
  RLC_FS_ERROR,
  RLC_INVALID_SIZE,
  RLC_INVALID_DIMENSIONS,
  RLC_INVALID_TYPE,
  RLC_FULL,
  RLC_EMPTY,
  RLC_LEARNING
}
rlc_error_type;

typedef enum {
  RLC_ATTR_DATA_MIN = 0,
  /* Report Specific Data */
  RLC_ATTR_DATA_R_MIN = RLC_ATTR_DATA_MIN,
  RLC_ATTR_DATA_R_MAX = RLC_ATTR_DATA_MIN + RLC_ATTR_TYPE_SET_LENGTH - 1,

  /* Learning Specific Data */
  RLC_ATTR_DATA_L_MIN = RLC_ATTR_DATA_R_MAX + 1,
  RLC_ATTR_DATA_L_GET_AVERAGE = RLC_ATTR_DATA_L_MIN,
  RLC_ATTR_DATA_L_GET_STD_DEV,
  RLC_ATTR_DATA_L_GET_CLUST_LOCS,
  RLC_ATTR_DATA_L_GET_BEST_CLUST,
  RLC_ATTR_DATA_L_GET_WORST_CLUST,
  RLC_ATTR_DATA_L_GET_NUM_CLUSTS,
  RLC_ATTR_DATA_L_MAX = RLC_ATTR_DATA_L_MIN + RLC_ATTR_TYPE_SET_LENGTH - 1,

  /* Correction Specific Data */
  RLC_ATTR_DATA_C_MIN = RLC_ATTR_DATA_L_MAX + 1,
  RLC_ATTR_DATA_C_MAX = RLC_ATTR_DATA_C_MIN + RLC_ATTR_TYPE_SET_LENGTH - 1,

  RLC_ATTR_DATA_MAX = RLC_ATTR_DATA_C_MAX
}
rlc_attr_data_type;

/** Defines the type of mutex passed into RLC functions */
typedef enum{
  RLC_MUTEX_NONE = 0,
  RLC_MUTEX_QURT_PIMUTEX,
  RLC_MUTEX_QURT_RMUTEX,
  RLC_MUTEX_REX_INTLOCK,
}
rlc_mutex_type;

/** Defines the various callback/output types passed into RLC functions for instrumentation */
#define RLC_SYNC_CALLBACK   0x10000000uL
#define RLC_ASYNC_CALLBACK  0x20000000uL
#define RLC_SYNC_OUTPUT     0x40000000uL
#define RLC_ASYNC_OUTPUT    0x80000000uL
typedef uint32 rlc_instrumentation_type;

/** Defines synchonous reporting/learning/correction, to be done within client task context */
#define RLC_REPORT_SYNC_MODE  0x1uL
#define RLC_LEARN_SYNC_MODE  0x1uL
#define RLC_CORRECT_SYNC_MODE  0x1uL
/** Defines asynchronous reporting/learning/correction, to be done in RLC task context */
#define RLC_REPORT_ASYNC_MODE 0x2uL
#define RLC_LEARN_ASYNC_MODE 0x2uL
#define RLC_CORRECT_ASYNC_MODE 0x2uL
/** Defines the different modes for reporting, populated by either sync or async mode */
typedef uint32 rlc_mode_type;

/** Defines the peripheral processors that RLC can report to when required */
typedef enum {
  RLC_REPORT_TO_MIN = 0,
  RLC_REPORT_TO_APPS = RLC_REPORT_TO_MIN,
  RLC_REPORT_TO_MPSS,
  RLC_REPORT_TO_WCNSS,
  RLC_REPORT_TO_ADSP,
  RLC_REPORT_TO_MAX
}
rlc_report_to_peripherals;

/** Defines the masks and bitshifts required for reporting to remote processors */
#define RLC_REPORT_TO_MASK        0xFF000000uL
#define RLC_REPORT_TO_BITSHIFT    24

/** Defines the internal masks needed for reporting to remote processors */
#define RLC_REPORT_TO_APPS_INTERNAL_MASK    (0x1 << RLC_REPORT_TO_APPS)
#define RLC_REPORT_TO_MPSS_INTERNAL_MASK    (0x1 << RLC_REPORT_TO_MPSS)
#define RLC_REPORT_TO_WCNSS_INTERNAL_MASK   (0x1 << RLC_REPORT_TO_WCNSS)
#define RLC_REPORT_TO_ADSP_INTERNAL_MASK    (0x1 << RLC_REPORT_TO_ADSP)

/** Defines the modes needed to pass to RLC to report to remote processors */
#define RLC_REPORT_TO_APPS_MODE   (RLC_REPORT_TO_APPS_INTERNAL_MASK << RLC_REPORT_TO_BITSHIFT)
#define RLC_REPORT_TO_MPSS_MODE   (RLC_REPORT_TO_MPSS_INTERNAL_MASK << RLC_REPORT_TO_BITSHIFT)
#define RLC_REPORT_TO_WCNSS_MODE  (RLC_REPORT_TO_WCNSS_INTERNAL_MASK << RLC_REPORT_TO_BITSHIFT)
#define RLC_REPORT_TO_ADSP_MODE   (RLC_REPORT_TO_ADSP_INTERNAL_MASK << RLC_REPORT_TO_BITSHIFT)

/** Defines a pointer to a mutex */
typedef void* rlc_mutex_ptr;

/** Defines a pointer to a semaphore */
typedef void* rlc_sig_ptr;

/** Defines the callback function format for functionality that requires callbacks */
typedef void (*rlc_callback_fn(void* data));

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/** 
Registers a handle with RLC.

@param[out] handle  Pointer to a pointer to a handle that will be populated by the function.
@param[in]  name    Client name to be attached to the handle.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
None.
*/
rlc_error_type rlc_register_handle( rlc_handle_ptr  *handle, const char* name );


/** 
Unregisters a handle from RLC.

@param[out] handle  Pointer to an RLC handle.

@return
None.
 
@dependencies  
None.
*/
void rlc_unregister_handle( rlc_handle_ptr handle );


/** 
Adds a reporting attribute to the client handle within RLC.

@param[in]  handle          Pointer to a client handle.
@param[in]  report_output   Type of reporting to perform.
@param[in]  reporting_data  Reporting-specific data.
@param[out] attr_ret        Client name to be attached to the handle.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
1) handle has successfully been registered.
*/
rlc_error_type rlc_add_reporting_attribute( rlc_handle_ptr              handle,
                                                    rlc_report_output_type      report_output,
                                                    uint32                          reporting_data,
                                                    rlc_external_attribute_type *attr_ret /* Get attribute pointer */
                                                  );


/** 
Schedules a timed reporting.

NOTE: Not used right now, not required at the moment.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
1) handle has successfully been registered.

rlc_error_type rlc_schedule_timed_rlc(  rlc_handle_ptr              handle,
                                                    rlc_external_attribute_type attr,
                                                    uint32                          time_in_ms
                                                    );
*/
                                            

/** 
Reports on the attribute that has previously been added to the client handle.

@param[in]  attr    External attribute that has been returned from adding report attribute.
@param[in]  mode    Bitmask of the mode that will determine how the reporting is done.
@param[in]  string  Buffer filled with reporting-specific data.
@param[in]  size    Total number of bytes to report in the buffer.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
1) Attribute attr has successfully been added to the handle.
*/
rlc_error_type rlc_report(  rlc_external_attribute_type attr,
                                    rlc_mode_type        mode,
                                    void                            *string,
                                    uint32                          size
                                    );


/** 
Adds a learning attribute to the client handle within RLC.

@param[in]  handle          RLC handle to attach attribute to.
@param[in]  learning_type   Enumeration stating what type of learning is to be done on the data passed in.
@param[in]  size            Size of data, must be a power of 2!
@param[in]	numDimensions	Number of dimensions.
@param[in]	numClusters		Number of clusters
@param[out] attr_ret        Pointer to the external attribute handle.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
1) Attribute attr has successfully been added to the handle.
*/
                                                    
rlc_error_type rlc_add_learning_attribute( rlc_handle_ptr              handle,
                                           rlc_learning_type           learning_type,
                                           uint32                      size,
										   uint32					   numDimensions,
										   uint32					   numClusters,
                                           rlc_external_attribute_type *attr_ret /* Get attribute pointer */
                                           );

/** 
Performs learning on an attribute that has previously been added to the client handle.

@param[in]  attr    External attribute that has been returned from adding learning attribute.
@param[in]  mode	Whether to learn synchronously or asynchronously.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_FAILED  -- Wrong data or size.
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
1) Attribute attr has successfully been added to the handle.
2) Learning must have been initialized previously
3) Number of data arguments must match up with dimensions.
*/
rlc_error_type rlc_learn(rlc_external_attribute_type	attr,
						             uint32                       mode);

/** 
Performs learning on an attribute that has previously been added to the client handle.

@param[in]  attr    External attribute that has been returned from adding learning attribute.
@param[in]  mode	Whether to learn synchronously or asynchronously.
@param[in]  type    Internal learning type.
@param[in]  numArgs Number of arguments to be read from var args.
@param[in]  data... New data obtained.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_FAILED  -- Wrong data or size.
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
1) Attribute attr has successfully been added to the handle.
2) Learning must have been initialized previously
3) Number of data arguments must match up with dimensions.
*/
rlc_error_type rlc_learn_internal(rlc_external_attribute_type		attr,
						                      uint32                        mode,
                                  rlc_learning_internal_type    type);



/** 
Adds data to a learning attribute.

@param[in]  attr    External attribute that has been returned from adding learning attribute.
@param[in]  mode	Whether to learn data synchronously or asynchronously.
@param[in]  numArgs Number of arguments to be read from var args.
@param[in]  data... New data obtained.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_FAILED  -- Wrong data or size.
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
1) Attribute attr has successfully been added to the handle.
2) Learning must have been initialized previously
3) Number of data arguments must match up with dimensions.
*/
rlc_error_type rlc_learn_data(	rlc_external_attribute_type 	attr,
			    		                  uint32                      	mode,
                                uint32					              numArgs, 
                                ... );

/**
Adds a set cluster location correcting attribute to the client handle within RLC

@param[in]  handle			RLC handle attribute is attached to.
@param[in]  correcting_type	RLC dynamic correction type.
@param[in]	func			Pointer to function used for correction
@param[in]	attr_learn		External learning attribute with the learned data.
@param[out] attr_ret        Pointer to the external attribute handle.

@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_FAILED  -- Wrong data or size.
RLC_NO_MEMORY -- No memory available to create the handle 
*/

rlc_error_type rlc_add_correcting_attribute_sc( rlc_handle_ptr              handle,
										   rlc_dynamic_correction_type correcting_type,
										   void (*func)(uint32),
										   rlc_external_attribute_type attr_learn,
                                           rlc_external_attribute_type *attr_ret /* Get attribute pointer */
										   );

/** 
Performs set cluster correction on the attribute that has previously been added to the client handle.

@param[in]  attr    External attribute that has been returned from adding correction attribute.
@param[in]  mode    Bitmask of the mode that will determine how the correcting is done.



@return
RLC_SUCCESS -- Handle successfully created and pointer to the handle is returned through 'handle'
RLC_NO_MEMORY -- No memory available to create the handle 
 
@dependencies  
1) Attribute attr has successfully been added to the handle.
*/
                                                    
rlc_error_type rlc_correct_sc(  rlc_external_attribute_type attr,
                                    rlc_mode_type        mode
                                    );

/**
Gets the dimension of best cluster location

@param[in]	attr	Pointer to external RLC correction attribute
@param[in]	dim		Dimension of cluster

@return
uint32 -- Value at dimension of best cluster location

@dependencies
1) Clustering performed
2) Dimension must be within valid range in data
*/

uint32 rlc_get_best( rlc_external_attribute_type attr, uint32 dim );

/**
Gets cluster location dimension value (API)

@param[in]	attr_ptr	Pointer to an RLC learning attribute structure
@param[in]	clusterNum	Cluster number
@param[in]	dimension	Dimension of which value you want (first=0)

@return
uint32 -- Value of cluster location at specified dimension

@dependencies
1) Chosen dimension must be less than the number of dimensions
*/

uint32 rlc_get_cluster_location( rlc_external_attribute_type attr_ptr, uint32 clusterNum, uint32 dimension );


/** 
Get data associated with an attribute.

@param[in]  attr          	Attribute to get data from.
@param[in]  attr_data_type	Data type which is being requested from attribute.
@param[in]  buffer_size	Size of the client supplied buffer.
@param[out] buffer		Client supplied buffer to fill requested data with.

@return
RLC_SUCCESS – Data correctly populated in buffer
RLC_FAILED – Error retrieving data
 
@dependencies  
1) Attributes must already be created.
*/

rlc_error_type rlc_get_attr_data( rlc_external_attribute_type 	attr,
                                  rlc_attr_data_type 			      attr_data_type,
                                  uint32 				                buffer_size,
                                  void* 					              buffer );

#endif /*RLC_H*/
