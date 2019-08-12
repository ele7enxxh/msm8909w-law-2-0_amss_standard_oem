#ifndef __UBSP_RPM_H__
#define __UBSP_RPM_H__

/*=============================================================================
  FILE:         ubsp_rpm.h

  OVERVIEW:     This file provides the public externs and definitions that
                are exported by the rpm module for Pronto ubsp.

                Copyright (c) 2012 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary

$Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/power/rpm/client/inc/ubsp_rpm.h#1 $

when       who     what, where, why
--------   ---     --------------------------------------------------------
05/21/12   CA      Create initial version for uBSP
=============================================================================*/

/*--------------------------------------------------------------------------
**                          INCLUDES
---------------------------------------------------------------------------*/
#include "comdef.h"

/*--------------------------------------------------------------------------
                       DEFINITIONS AND MACROS
---------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                       TYPES AND ENUMERATIONS
---------------------------------------------------------------------------*/
/**
* These are all the error codes returned by UBSP RPM API
*/
typedef enum 
{
  UBSP_RPM_ERR_FLAG__FAILURE =0,
  UBSP_RPM_ERR_FLAG__SUCCESS,
}ubsp_rpm_err_type;

/*--------------------------------------------------------------------------
                         FUNCTION PROTOTYPES
---------------------------------------------------------------------------*/

/**----------------------------------------------------------------
  FUNCTION   ubsp_rpm_setTimedTrigger 
 
  DESCRIPTION
  Sets up the Time Trigger to RPM to wake up on time for the next BT
  or WLAN hard timer. 
 
  PARAMETERS  
  wakeuptime

  DEPENDENCIES
 

  RETURN VALUE
  None

  SIDE EFFECTS
  None              
---------------------------------------------------------------**/
void ubsp_rpm_setTimedTrigger(uint64 wakeuptime);


/**----------------------------------------------------------------
  FUNCTION ubsp_rpm_send
  
  DESCRIPTION
 
  PARAMETERS  
  None

  DEPENDENCIES
  None

  RETURN VALUE
  ubsp_rpm_err_type

  SIDE EFFECTS
  None
---------------------------------------------------------------**/
ubsp_rpm_err_type ubsp_rpm_send (uint32 resource_type, uint32 id, uint32 set, uint8 *kvps, uint32 len);

/**----------------------------------------------------------------
  FUNCTION ubsp_rpm_send_halt

  DESCRIPTION

  PARAMETERS
  None

  DEPENDENCIES
  None

  RETURN VALUE
  None

  SIDE EFFECTS
  None
---------------------------------------------------------------**/
void ubsp_rpm_send_halt(void);

#endif /* __UBSP_SLEEP_H__ */
