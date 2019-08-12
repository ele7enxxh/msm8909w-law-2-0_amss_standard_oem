/*===========================================================================
                      DS_ANDSF_CM_IF.H

DESCRIPTION
Interfacing to Call Manager event header file for ANDSF.

EXTERNALIZED FUNCTIONS

 Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who                            what, where, why
--------    -------                ----------------------------------------
04/12/2015    Youjunf                          First version of file
===========================================================================*/
#ifndef DS_ANDSF_CM_IF_H
#define DS_ANDSF_CM_IF_H

/*===========================================================================
                           INCLUDE FILES
===========================================================================*/
#include "datamodem_variation.h"
#ifdef FEATURE_DATA_ANDSF_RULE_MGR

#include "ds_sys.h"
/*===========================================================================
                   EXTERNAL DEFINITIONS AND TYPES
===========================================================================*/

/*-----------------------------------------------------------------
  Gives whether the UE is roaming or in the home network.
  is_roaming_on = TRUE  : UE is roaming
  is_roaming_on = FALSE : UE is in the home network   
---------------------------------------------------------------------*/
typedef struct
{
  ds_sys_subscription_enum_type subs_id;
  boolean                       is_roaming_on;
}ds_andsf_roaming_status_s_type;


/*===========================================================================
                           FUNCTION DECLARATIONS
===========================================================================*/

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------------
  Post the roaming information to ANDSF rule manager
------------------------------------------------------------------------------*/
void ds_andsf_notify_update_roaming_status(const ds_andsf_roaming_status_s_type *roaming_status_ptr);

#ifdef __cplusplus
}
#endif

#endif   /*FEATURE_DATA_ANDSF_RULE_MGR*/
#endif /* DS_ANDSF_CM_IF_H */
