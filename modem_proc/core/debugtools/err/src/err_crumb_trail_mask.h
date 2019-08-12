#ifndef ERR_CRUMB_TRAIL_MASK_H
#define ERR_CRUMB_TRAIL_MASK_H
/*===========================================================================

                   L O G  P A C K E T S  F O R  E R R

DESCRIPTION
  This header file contains the definitions of crumb_trail_mask
 
Copyright (c) 2007 - 2015   by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/debugtools/err/src/err_crumb_trail_mask.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/16/15   din     File Created

===========================================================================*/


/************************************************************************
 *                         ERR_TRACE_TYPE
 ************************************************************************/

#define ERR_CRUMB_TRAIL_BMSK_ERR_FATAL_PRE                    (1<<0)
#define ERR_CRUMB_TRAIL_BMSK_EXCEPTION_PRE                    (1<<1)
#define ERR_CRUMB_TRAIL_BMSK_JETTISON_CORE_POST               (1<<2)
#define ERR_CRUMB_TRAIL_BMSK_OVERRIDE_PRE                     (1<<3)
#define ERR_CRUMB_TRAIL_BMSK_ERR_FATAL_RAISE_EXCEPTION_PRE    (1<<4)
#define ERR_CRUMB_TRAIL_BMSK_CACHCE_CLEAN_PRE                 (1<<5)
#define ERR_CRUMB_TRAIL_BMSK_DECOMPRESS_MSG_PRE               (1<<6)
#define ERR_CRUMB_TRAIL_BMSK_F3_TRACE_PRE                     (1<<7)
#define ERR_CRUMB_TRAIL_BMSK_COREDUMP_UPDATED_PRE             (1<<8)
#define ERR_CRUMB_TRAIL_BMSK_SMEM_LOG_PRE                     (1<<9)
#define ERR_CRUMB_TRAIL_BMSK_STORE_INFO_PRE                   (1<<10)
#define ERR_CRUMB_TRAIL_BMSK_FORMAT_MESSAGE_PRE               (1<<11)
#define ERR_CRUMB_TRAIL_BMSK_LOG_SSR_PRE                      (1<<12)
#define ERR_CRUMB_TRAIL_BMSK_LOG_EXTERNEL_CBS_PRE             (1<<13)
#define ERR_CRUMB_TRAIL_BMSK_RAISE_TO_KERNEL_PRE              (1<<14)
#define ERR_CRUMB_TRAIL_BMSK_ERR_FATAL_JETTISION_CORE_POST    (1<<15)
#define ERR_CRUMB_TRAIL_BMSK_EMERGENCY_RECOVERY_PRE           (1<<16)
#define ERR_CRUMB_TRAIL_BMSK_DYNAMIC_CALLBACK_PRE             (1<<17)
#define ERR_CRUMB_TRAIL_BMSK_FATAL_NOTIFY_INVOKED             (1<<18)

/*=========================================================================

FUNCTION err_update_crumb_trail_mask

DESCRIPTION
  Updates Crumb trail mask

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void err_update_crumb_trail_mask(uint32 bits);

#endif /* ERR_CRUMB_TRAIL_MASK_H */
