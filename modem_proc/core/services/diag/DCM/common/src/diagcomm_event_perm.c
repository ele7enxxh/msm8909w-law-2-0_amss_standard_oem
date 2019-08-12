/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
					Diagnostic event filtering using SSM
                   

General Description
This file contains the default event permissions passed in while initializing 
with SSM. 
 
Copyright (c) 2013-14 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagcomm_event_perm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/01/14   ph      EVENT_POWERUP now appears on boot up.
07/03/13   sr      Permissions in fallback buffer are mentioned in this file

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#include "event_defs.h"
#include <string.h>

/*Fallback buffer permissions for events in the format:
  event_id_start|event_id_end|permission|
  permission value 0: allow
  permission value 1" dont allow
  If the permission file is not present in SFS, then these are the default permissions that are used*/

const char * diag_ssm_event_fb = "0|4095|0|";
/*Add the events that are to be seen before event mask is set */
/* Add new event before EVENT_MAX_ID as it indicates the end of event array  */
/*WARNING: Events in this array will by-pass SSM security before SSM callback is invoked */
const event_id_enum_type event_ids_pre_boot[]={EVENT_POWERUP, EVENT_MAX_ID};