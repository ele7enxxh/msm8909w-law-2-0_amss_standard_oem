#ifndef HDRHIT_API_H
#define HDRHIT_API_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

						H D R   H I T   A P I

GENERAL DESCRIPTION

   This file enums for hdrhit messages.

Copyright (c) 2000-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */

/*===========================================================================

                        EDIT HISTORY FOR MODULE                   

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/26/2011 smd     Removed including ftm_cmdi.h
08/26/2011 smd     Removed including hdrsrchtypes.h and feature HDR.
07/20/2011 ssu	   Created new file.

===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include "msgr.h"
#include "msgr_umid.h"
#include <sys.h>
#include "hdrmsg.h"

#include "sys_v.h"

#ifdef __cplusplus
}  // extern "C"
#endif


/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/


typedef enum 
{
  HDRHIT_HDR_FTM_DIVERSITY_DISABLE,
  HDRHIT_HDR_FTM_DIVERSITY_ENABLE,
  hdrhit_hdr_ftm_diversity_size = 0x7FFFFFFF
} hdrhit_hdr_ftm_diversity_mode_type;


typedef enum
{
  HDRHITMSG_HDR_FTM_ENTER,                  /* FTM entry state */
  HDRHITMSG_HDR_FTM_PIL_ACQ,                /* FTM pilot channel acquisition */
  HDRHITMSG_HDR_FTM_SYNC_STATE,            /* FTM sync channel processing, combines
                                 CDMA_SYNC_ACQ, CDMA_SYNC_MSG */
  HDRHITMSG_HDR_FTM_IDLE,                   /* FTM idle state, we have acquired system
                                 time and are demodulating the paging 
                                 channel. */
  HDRHITMSG_HDR_FTM_FWD_TRAFFIC_ONLY,  
  HDRHITMSG_HDR_FTM_TRAFFIC,                /* FTM traffic state */
  HDRHITMSG_HDR_FTM_SYS_ACC,                /* FTM System Access state */
  HDRHITMSG_HDR_FTM_REV_TRAFFIC,            /* FTM reverse traffic state */
  HDRHITMSG_HDR_FTM_MAX_STATE
} hdrhitmsg_hdr_ftm_state_enum_type;

#endif /* HDRHIT_API_H */
