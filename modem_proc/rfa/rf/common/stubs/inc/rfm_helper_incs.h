#ifndef RFM_HELPER_INCS_H
#define RFM_HELPER_INCS_H

/*
   @file
   rfm_helper_incs.h

   @brief
   these includes for other(outside of RF) modules to compile, which
   indirectly includes these files via rfm.h file : which need to be fixed for
   future builds.

*/

/*===========================================================================
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/stubs/inc/rfm_helper_incs.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
01/03/12   sty     Added p4 header 
                   mainlined RFM_DEBUG 
============================================================================*/ 

#include "rfa_variation.h"
#include "err.h"
#include "msg.h"
#include "nv_items.h"
#include "nv.h"
#ifdef FEATURE_WCDMA
#include "rfumts.h"
#endif
#ifdef FEATURE_CDMA1X
#include "db.h"
#endif
#ifdef FEATURE_GSM
#include "rfllgsm.h"
#endif



/*
"..\..\modem\1x\srch\src\traffic\srchtc_sm.c", line 2774: Error:  #20: identifier "RFC_CFS_TUNE_WAIT" is undefined
    uint16 cfs_sw_time = EXPECTED_MAX_OFF_TIME_FWD - 2*RF_CFS_TUNE_WAIT;
*/
#define RFC_CFS_TUNE_WAIT (5)

#define RFM_CMD_NUM_BUFFS 2000

typedef struct {
  void * func_ptr;
  uint32 args[4];
} rfm_cmd_type;

typedef struct {
  int32 last_entry;
  rfm_cmd_type entry[RFM_CMD_NUM_BUFFS];

} rfm_cmd_buffer_type;

rfm_cmd_buffer_type rfm_cmd_buffs;

/* circular buffer */
#define RFM_DEBUG_EN_Q(x,a,b,c,d)                   \
{                                                   \
  rfm_cmd_buffs.last_entry++;                       \
  rfm_cmd_buffs.last_entry = rfm_cmd_buffs.last_entry % RFM_CMD_NUM_BUFFS;    \
  rfm_cmd_buffs.entry[rfm_cmd_buffs.last_entry].func_ptr = (void*)(x); \
  rfm_cmd_buffs.entry[rfm_cmd_buffs.last_entry].args[0] = (uint32)(a);    \
  rfm_cmd_buffs.entry[rfm_cmd_buffs.last_entry].args[1] = (uint32)(b);    \
  rfm_cmd_buffs.entry[rfm_cmd_buffs.last_entry].args[2] = (uint32)(c);    \
  rfm_cmd_buffs.entry[rfm_cmd_buffs.last_entry].args[3] = (uint32)(d);    \
}                                                                         \

#endif
