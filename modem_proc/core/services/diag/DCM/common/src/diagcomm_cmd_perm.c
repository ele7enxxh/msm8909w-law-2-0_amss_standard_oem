/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
					Diagnostic cmd filtering using SSM
                   

General Description
This file contains the default cmd permissions passed in while initializing 
with SSM.
 
Copyright (c) 2013 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagcomm_cmd_perm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/03/13   sr      Permissions in fallback buffer are mentioned in this file

===========================================================================*/

#include "core_variation.h"
#include "comdef.h"
#include <string.h>

/*Default diag cmd permissions in the format:
  subssys_id_start|subsys_id_end|subsys_cmd_code_start|subsys_cmd_code_end|permission|
  permission value 0: allow
  permission value 1" dont allow*/
const char * diag_ssm_cmd_fb = "0|255|0|65535|0|";