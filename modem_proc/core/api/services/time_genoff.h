#ifndef TIME_GENOFF_H
#define TIME_GENOFF_H

/**
@file time_genoff.h
@brief
This module implements the Generic framework to handle time bases.
*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ATS_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ATS_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the time_genoff group 
      description in the ATS_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 2009-2015 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

 T I M E   S E R V I C E   G E N E R I C  F R A M E W O R K  S U B S Y S T E M

GENERAL DESCRIPTION
 Implements Generic framework to handle Time bases. 

EXTERNALIZED FUNCTIONS

  time_genoff_opr
   Does time set/get operation on the basis of Inputs

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Should be initialized with following sequence

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/api/services/time_genoff.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
09/01/15   abh     Changes for correctly handling ATS_PRIVATE bases.
11/29/10   scd     (Tech Pubs) Edited Doxygen markup and comments.
10/27/10   EBR     Doxygenated File.
06/23/10   din     Added new time bases and attached a proxy base for a base.
05/18/10   din     Shifted time_remote_opr_type from time_genoff_v.h
10/07/09   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include "time_types.h"
#include "time_osal.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

/* The maximun file name string length to be used by clients
  * for external genoff initialization to use in efs read/write operations */
#define MAX_EFS_FILE_NAME_SIZE              16 

/*--------------------------------------------------------------------------
  Linting...
--------------------------------------------------------------------------*/

/*lint -esym(715,unused_*)  parameters prefixed with unused_ are not used */


/*-----------------------------------------------------------------------------
  Macros used for converting to/from ts_timestamp format
-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
  Time Bases static information
-----------------------------------------------------------------------------*/

/** @addtogroup time_genoff
@{ */

/** Various time bases.
*/
typedef enum time_bases
{
  ATS_RTC = 0, /**< Real time clock timebase.*/
  ATS_TOD,     /**< Proxy base for number of bases.*/
  ATS_USER,    /**< User timebase. */
  ATS_SECURE,  /**< Secure timebase. */
  ATS_DRM,     /**< Digital rights management timebase. */
  ATS_USER_UTC,   /**< Universal Time Coordinated user timebase. */   
  ATS_USER_TZ_DL, /**< Global time zone user timebase. */
  ATS_GPS,    /**< Base for GPS time. \n
                   @note1hang When ATS_GSTK is modified, changes are also 
                   reflected on ATS_TOD. */
  ATS_1X,     /**< Base for 1X time. \n
                   @note1hang When ATS_1X is modified, changes are also 
                   reflected on ATS_TOD. */
  ATS_HDR,    /**< Base for HDR time. \n
                   @note1hang When ATS_HDR is modified, changes are also 
                   reflected on ATS_TOD. */
  ATS_WCDMA,  /**< Base for WCDMA time. \n
                   @note1hang When ATS_WCDMA is modified, changes are also 
                   reflected on ATS_TOD. */
  ATS_MFLO,   /**< Base for MediaFLO time. \n
                   @note1hang When ATS_MFLO is modified, changes are also 
                   reflected on ATS_TOD. */
  ATS_3GPP, /**< LTE timebase. */
  ATS_UTC,    /**< ATS_UTC is on apps */
  ATS_LTE_HR, /**< Base for storing LTE SIB16 UTC Time. SIB16 has the same 
                 granularity as 1x. It will be handled similar to ATS_3GPP */
  ATS_LTE_HR_GPS, /**< Base for storing LTE SIB16 GMT Time. SIB16 has the same 
                 granularity as 1x. It will also have TOD as a proxy base */
  ATS_PRIVATE = 0x1000000, /**< Holder for Private Bases that are declared 
                                outside of generic time framework */
  ATS_INVALID = 0x10000000
} time_bases_type;

#define ATS_MAX ( ATS_LTE_HR_GPS + 1 )

/* Error return type for the time-services API's */
typedef enum
{
ATS_SUCCESS,
ATS_EARLY_INIT_CALLED,
ATS_INVALID_BASE_NUM,	
ATS_INVALID_INPUT,
ATS_MALLOC_FAIL,
ATS_PRIVATE_BASE_EXISTS,
ATS_QDI_COPY_FAILED,
ATS_FAILURE,
} 
ats_error_type;


/** Time unit type. 
*/
typedef enum time_unit
{
  TIME_STAMP,      /**< Time is in timestamp format. */
  TIME_MSEC,       /**< Time is in millisecond format. */
  TIME_SECS,       /**< Time is in second format. */
  TIME_JULIAN,     /**< Time is in Julian calendar format. */
  TIME_20MS_FRAME, /**< Time is in 20-millisecond frames format. */
  TIME_NONE        /**< Time format is undefined. */
} time_unit_type ;

/** Time the generic offset operation is to be done. 
*/
typedef enum time_genoff_opr
{
  T_SET = 0x1, /**< Genoff_Opr sets the time. */
  T_GET = 0x2, /**< Genoff_Opr gets the time. */

  T_MAX = 0x8000  /**< Placeholder for maximum enumerator value. */
} time_genoff_opr_type;

#define TIME_GENOFF_OPR_SET_MASK	  (T_SET)
#define TIME_GENOFF_OPR_GET_MASK	  (T_GET)

/* Time Generic Offset 'type 1' callback function */
typedef void (*time_genoff_t1_cb_type)(void);

/* Time Generic Offset 'type 2' callback function */
typedef void (*time_genoff_t2_cb_type)
(
  /* Base that has registered for callback */
  void       *registered_genoff_ptr,
  
  /* data to pass to this callback function */
  int64      data
);


/*-----------------------------------------------------------------------------
  Generic offset information
-----------------------------------------------------------------------------*/
/** Base source from which to get the time.
*/
typedef enum base_time_source
{
  TIME_SCLK,
  TIMETICK_SCLK64
} base_time_source_type ;

/**
@brief Structure to be passed as the argument to set and get functions. 
*/
typedef struct time_genoff_args
{
  time_bases_type                 base;   /**< Pointer to genoff in 
                                               consideration. */  
  void *                          ts_val; /**< Time to be passed/returned. */
  time_unit_type                  unit;   /**< Time unit being used. */
  time_genoff_opr_type            operation; /**< Time operation to be done. */
  base_time_source_type           base_source; /**< Base source type. */
  uint32                          dal_timetick_val_valid; /**< Flag to state whether a timestamp
                                                               dal_timetick_val is populated with
                                                               QTimer count from DalTimetick. Populate
                                                               with TIME_GENOFF_TIMETICK_VALID if valid */
  uint64                          dal_timetick_val; /**< QTimer cnt value at which timestamp was valid */

} time_genoff_args_struct_type;

/**
@brief Pointer type of time_genoff_args_struct_type.
*/
typedef struct time_genoff_args *time_genoff_args_ptr;

/**
@brief Time type to be passed as the remote type.
*/
typedef union time_remote_opr
{
  time_type                       timestamp; /**< Time to be passed/returned
                                                  in timestamp format. */
  time_julian_type                julian;    /**< Time to be passed/returned
                                                  in time_julian format. */
}time_remote_opr_type;
/*~ CASE TIME_STAMP TIME_MSEC TIME_SECS TIME_20MS_FRAME time_remote_opr.timestamp */ 
/*~ CASE TIME_JULIAN time_remote_opr.julian */

/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/

/** 
Initializes Generic offset argument structure to be used later in time_genoff_opr..

@param[in] pargs    Pointer to the arguments.

@return
None.

@dependencies
None.
*/
void time_genoff_args_init
(
  time_genoff_args_ptr pargs
);

/** 
Performs the time set/get operation based on the inputs passed in.

@param[in] pargs Pointer to the arguments.

@return
None.

@note
For GET/SET operations on ATS PRIVATE base, the caller needs
to pass the base number which he gets after calling the ext init
function, i.e. time_genoff_private_base_init().

@dependencies
None.
*/
void time_genoff_opr
(
  time_genoff_args_ptr         pargs
);

/**

@function time_genoff_get_sum_genoff_ts

@brief
  Returns sum generic offset value, its sign of a given time base
  in timestamp format

@param[in]
  base - time base for which offset details are required.

@return
  boolean - For negative sum_genoff_ts val this will be True
  Provides sum generic offset value of required base as part of sum_genoff_ts_val

@dependencies
None

**/
boolean time_genoff_get_sum_genoff_ts
( 
    time_bases_type  base,
    time_type        sum_genoff_ts_val     /* Returns with sum_genoff_ts value */
);


/**

@function 
  time_genoff_get_ts_from_offset_and_timetick

@brief
  Calculates timestamp from passed offset and timetick.

@dependencies
  Need to pas offset details that are obtained from time_genoff_get_sum_genoff_ts

@return
  time in timestamp format in ts_val

**/

void time_genoff_get_ts_from_offset_and_timetick
(
  /* Generic offset in timestamp format */
  time_type           sum_genoff_ts,
  /* Sign of offset. FALSE if +ve, True if -ve */
  boolean             is_offset_negative,
  /* timetick at which timestamp is required */
  uint64              timetick,
  /* Calculated timestamp is returned */
  time_type           ts_val  
);


/** 
Performs the initialization of the external genoff.
This external genoff is a private (to the client) structure
which the caller identifies via a unique filename.
This init function then return a base number which needs to be
used in later set / get / deinit operations.
The private base serves the purpose of a time-services client
to maintain his own separate time base.

@param[in] f_name   The filename to associate with ext genoff.
@param[in] ret_base Encoded base number returned to the caller.

@return
ats_error_type  Indicates the error based on the performed operation.

@note
The filename should not exceed MAX_EFS_FILE_NAME_SIZE characters.

@dependencies
None.
*/
ats_error_type time_genoff_private_base_init
(
  char *              f_name,
  uint32 *            ret_base
);

/** 
Performs the de-initialization of the external genoff.

@param[in] base     Base which was returned by time_genoff_private_base_init().
@param[in] f_name The filename to associate with ext genoff..

@return
ATS_SUCCESS                            If the passed base & filename were successfully deinited.
ATS_FAILURE                              If there was an error in the deinitialization.

@note
For deinit operation , the caller needs to pass the same
base number which he gets after calling the ext init
function, i.e. time_genoff_private_base_init().

@dependencies
None.
*/
ats_error_type time_genoff_private_base_deinit
(
  uint32              base,
  char *              f_name
);

/** 
Syncronizes the TOD genoff data with the Julian time.

@param[in] ts_val Timestamp with which to synchronize.

@return
None.
 
@dependencies  
None.
*/
void time_genoff_sync_with_tod
(
  time_type ts_val
);

/**
This function logs the radio technology going out of coverage

@param[in] time offset corresponding to Radio Technology that is
           going  
@return
None.

@dependencies  
None.
*/
void time_genoff_report_out_of_coverage
(
  time_bases_type base
);

/**
Determines whether the remote modem is to be updated.

@param[in] update_remote_modem Boolean value to determine whether the remote 
                               modem is to be updated.

@return
None.

@dependencies  
None.
*/
void time_genoff_set_remote_modem_update
(
  boolean update_remote_modem
);

/**
Returns whether the remote modem is being updated.

@return
TRUE  -- Remote modem is being updated. \n
FALSE -- Remote modem is not being updated.
      
@dependencies  
None.
*/
boolean time_genoff_get_remote_modem_update
(
  void
);

#ifdef FEATURE_QMI_TIME_REMOTE_CLNT
/*=============================================================================

FUNCTION TIME_GENOFF_GET_SYS_TIME_IN_SEC_WITH_LP_SEC

DESCRIPTION
  This function sets gets the system time alongwith leap second info

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
uint32 time_genoff_get_sys_time_in_sec_with_lp_sec
(
  void
);
#endif
/** @} */ /* end_addtogroup time_genoff */

#endif /* TIME_GENOFF_H */
