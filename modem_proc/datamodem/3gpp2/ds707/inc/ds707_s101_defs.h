#ifndef DS707_S101_DEFS_H
#define DS707_S101_DEFS_H

/** 
  @file ds707_s101_defs.h
  @brief 
   S101 related defines
*/

/*===========================================================================

                            D S 7 0 7 _ S101
GENERAL DESCRIPTION
  This file contains definitions that other modules use

EXTERNALIZED FUNCTIONS


 Copyright (c) 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_s101_defs.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/05/11   Ack     Created module
===========================================================================*/

/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/
//#include "comdef.h"
#include "datamodem_variation.h"
#include "ps_sys_ioctl.h"

/*===========================================================================
                            VARIABLES
===========================================================================*/
#ifdef FEATURE_DATA_OPTHO
/*
Note: This union is used by dstask.c to post command to S101.
We cannot define this in ds707_s101.h since it causes interdependency 
between dstask_v.h and ds707_s101.h and won't compile. 
i.e dstask_v.h needs ds707_s101_process_cmd_info_type in ds_cmd_type
to post the cmd and ds707_s101.h needs ds_cmd_type to process the cmd.
Hence we need to define this struct in a header file that doesn't 
include dstask_v.h (or) define this in dstask_v.h before we
define ds_cmd_type
*/

/** Generic union that groups all S101 data. Used in
 *  dstask_v.h by ds_cmd_type */
typedef union
{
  ps_sys_ioctl_3gpp2_prereg_handoff_complete_cmd_type prereg_handoff_info;
} ds707_s101_process_cmd_info_type;
#endif /* FEATURE_DATA_OPTHO */
#endif /* DS707_S101_DEFS_H */
