#ifndef TIME_GENOFF_H
#define TIME_GENOFF_H
/*=============================================================================

 T I M E   S E R V I C E   G E N E R I C  F R A M E W O R K  S U B S Y S T E M

GENERAL DESCRIPTION
 Implements Generic framework to handle Time bases. 

EXTERNALIZED FUNCTIONS

  time_genoff_opr
   Does time set/get operation on the basis of Inputs

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Should be initialized with following sequence
    

Copyright (c) 2009-10 by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/services/time_genoff.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/23/10   din     Added new time bases and attached a proxy base for a base.
05/18/10   din     Shifted time_remote_opr_type from time_genoff_v.h
10/07/09   din     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include "time_types.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/


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

typedef enum time_bases
{
  ATS_RTC = 0,
  ATS_TOD,    /* proxy base for number of bases */
  ATS_USER,
  ATS_SECURE,
  ATS_DRM,
  ATS_USER_UTC,   
  ATS_USER_TZ_DL,
  ATS_GPS,    /* If ATS_GSTK is modified, changes are also reflected on ATS_TOD */
  ATS_1X,     /* If ATS_1X is modified, changes are also reflected on ATS_TOD */
  ATS_HDR,    /* If ATS_HDR is modified, changes are also reflected on ATS_TOD */
  ATS_WCDMA,  /* If ATS_WCDMA is modified, changes are also reflected on ATS_TOD */
  ATS_MFLO,   /* If ATS_MFLO is modified, changes are also reflected on ATS_TOD	  */
  ATS_INVALID = 0x10000000
} time_bases_type;

#define ATS_MAX ( ATS_MFLO + 1 )

/* time unit type */
typedef enum time_unit
{
  TIME_STAMP,
  TIME_MSEC,
  TIME_SECS,
  TIME_JULIAN,
  TIME_20MS_FRAME,
  TIME_NONE
} time_unit_type ;

/* time gneoff operation to be done */
typedef enum time_genoff_opr
{
  T_SET,
  T_GET,
  T_MAX
} time_genoff_opr_type;

/*-----------------------------------------------------------------------------
  Generic offset information
-----------------------------------------------------------------------------*/
typedef enum base_time_source
{
  TIME_SCLK,
  TIMETICK_SCLK64
} base_time_source_type ;

/* Structure to be passed as argument to set and get functions */
typedef struct time_genoff_args
{
  /* Pointer to Genoff in consideration */
  time_bases_type                 base;

  /* Time to be passed/returned */
  void *                          ts_val;

  /* Time unit */
  time_unit_type                  unit;

  /* Time operation to be done */
  time_genoff_opr_type            operation;     

  /* Base source Type */
  base_time_source_type           base_source;

} time_genoff_args_struct_type;

/* Pointer type of the time_genoff_args_struct_type */
typedef struct time_genoff_args *time_genoff_args_ptr;

/* Type to be passed as remote type */
typedef union time_remote_opr
{
  /* Time to be passed/returned in timestamp format */
  time_type                       timestamp;

  /* Time to be passed/returned in time_julian format */
  time_julian_type                julian;

}time_remote_opr_type;
/*~ CASE TIME_STAMP TIME_MSEC TIME_SECS TIME_20MS_FRAME time_remote_opr.timestamp */ 
/*~ CASE TIME_JULIAN time_remote_opr.julian */


/*=============================================================================

                           FUNCTION DEFINITIONS

=============================================================================*/


/*=============================================================================

FUNCTION TIME_GENOFF_OPR

DESCRIPTION
  Does time set/get operation on the basis of Inputs.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void time_genoff_opr
(
  /* Pointer to time_genoff_args_type with genoff_ptr in consideration */
  time_genoff_args_ptr         pargs
);

/** 
@ingroup Functions for Time Genoff
         
@brief  
 This function will sync the tod genoff data with the julian time       \n\n 

@param[in]                
   time_type ts_val -  Time stamp                                       \n\n 
      
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 None                                                                   \n\n 
 
@sideffects 
 None                                                                   \n\n 

*/
void time_genoff_sync_with_tod
(
  time_type ts_val
);

/** 
@ingroup Functions for Time Genoff
         
@brief  
 This function sets the variable for updating remote modem              \n\n 

@param[in]                
   uint8 - boolean value to set                                         \n\n 
      
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 None                                                                   \n\n 
 
@sideffects 
 None                                                                   \n\n 

*/
void time_genoff_set_remote_modem_update
(
  boolean update_remote_modem
);

/** 
@ingroup Functions for Time Genoff
         
@brief  
 This function gets the variable for updating remote modem              \n\n 

@param[in]                
 None                                                                   \n\n 
      
@dependencies  
 None                                                                   \n\n 
 
@param[out]
 boolean - whether the remote modem proc is to be updated or not        \n\n 
 
@sideffects 
 None                                                                   \n\n 

*/
boolean time_genoff_get_remote_modem_update
(
  void
);
#endif /* TIME_GENOFF_H */
