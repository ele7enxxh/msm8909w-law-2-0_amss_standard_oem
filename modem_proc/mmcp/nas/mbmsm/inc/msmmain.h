#ifndef MSMMAIN_H
#define MSMMAIN_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     MBMS SERVICE MANAGER MAIN MODULE (MSMMAIN.H)

GENERAL DESCRIPTION

This module contains the interface and structure definitions 

EXTERNALIZED FUNCTIONS
 
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001,2002,2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mbmsm/inc/msmmain.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/17/06   vr      Initial version

===========================================================================*/

#include "msmtask.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
#include "sys.h"
#include "sys_v.h"
#define MSM_MAX_MBMS_SERVICES  16

typedef enum
{
  MSM_MBMS_SERVICE_INACTIVE,
  MSM_MBMS_SERVICE_PENDING_ACT,
  MSM_MBMS_SERVICE_ACTIVE_ACQ,
  MSM_MBMS_SERVICE_ACTIVE_TRAFFIC,
  MSM_MBMS_SERVICE_PENDING_DEACT
}msm_service_state_e_type;

typedef struct
{
  msm_service_state_e_type      service_state;
  sys_ip_address_s_type         mip;
  uint16                        port;
  sys_mbms_tmgi_uint_old_s_type tmgi;
  uint64                        tmgi_sdp;
  uint8                         priority; 
  uint32                        log_chan_id;

}msm_mbms_service_entry_s_type;

typedef struct
{
  uint32 num_services;
  msm_mbms_service_entry_s_type services[MSM_MAX_MBMS_SERVICES];
}msm_mbms_services_s_type;

typedef struct
{
  boolean core_nw_capability;
  boolean cell_capability;
}msm_mbms_capabililty_s_type;

/*===========================================================================

FUNCTION MSMMAIN_INIT

DESCRIPTION

  This function initialized service db

DEPENDENCIES

  None.
  
RETURN VALUE

  None

SIDE EFFECTS

  None.
  
===========================================================================*/
void msmmain_init
(
  void
);

/*===========================================================================

FUNCTION MSM_COMMAND_HANDLER

DESCRIPTION

  This function handles all the commands from other layers

DEPENDENCIES

  None.
  
RETURN VALUE

  None

SIDE EFFECTS

  None.
  
===========================================================================*/
void msm_command_handler
(
  msm_ext_cmd_type * msm_cmd_ptr
);

#endif


