/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
					Diagnostic log filtering using SSM
                   

General Description
This file contains the default event permissions passed in while initializing 
with SSM. 
 
Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagcomm_log_perm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/03/13   sr      Permissions in fallback buffer are mentioned in this file

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#include <string.h>

/*Fallback buffer permissions for events in the format:
  log_id_start|log_id_end|permission|
  permission value 0: allow
  permission value 1" dont allow
  If the permission file is not present in SFS, then these are the default permissions that are used*/
const char * diag_ssm_log_fb = "0|4095|0|4096|8191|0|8192|12287|0|12288|16383|0|16384|20479|0|20480|24575|0|24576|28671|0|28672|32767|0|32768|36863|0|36864|40959|0|40960|45055|0|45056|49151|0|49152|53247|0|53248|57343|0|57344|61439|0|61440|65535|0|";

