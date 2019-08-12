#ifndef HDRBCCPFSI_H
#define HDRBCCPFSI_H

/*===========================================================================

                            P R O T O C O L
    D E F I N I T I O N S   F O R   S T A T E   C H A N G E   M O D U L E

DESCRIPTION
  This contains the declarations for regional types and variables used
  in the state change module of the BCCP Flows

Copyright (c) 2005,2006,2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2008 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrbccpfs.h_v   1.7   08 May 2001 18:59:52   dandrus  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/api/hdrbccpfsi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/08   mpa     Added HDRBCCPFS_DESIRED_MAX_MONITORED state for BCMCS 2.0
08/28/05   mpa     Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "hdr_variation.h"
#include "comdef.h"
#include "hdrbit.h"

/* <EJECT> */ 
/*===========================================================================

             DEFINITIONS  FROM  THE  HDR  AIR  INTERFACE  SPEC

===========================================================================*/
                                 
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/* 
   Note: The state enum values are fixed and they are used by the QXDM database
   New enum tag should be added after the current enum tag and also
   assign a fixed value
*/

/* The next new BCCPF state must be added as state #4 */
#ifdef FEATURE_HDR_BCMCS_2p0
#error code not present
#else
#define HDRBCCPFS_NUM_STATES 5
#endif /* FEATURE_HDR_BCMCS_2p0 */
typedef enum
{
  HDRBCCPFS_NULL                  = 0, /* Transition state when entry is created 
                                        or purged */
  HDRBCCPFS_UNDESIRED             = 1, /* Undesired State */
  HDRBCCPFS_DESIRED_UNAVAILABLE   = 2, /* Desired Unavailable State */
  HDRBCCPFS_DESIRED_MONITORED     = 3, /* Desired Monitored State */
  HDRBCCPFS_DESIRED_UNMONITORED   = 4 /* Desired Unmonitored State */
#ifdef FEATURE_HDR_BCMCS_2p0
  #error code not present
#endif /* FEATURE_HDR_BCMCS_2p0 */
} hdrbccpfs_state_enum_type;       

#endif               /* HDRBCCPFSI_H */
