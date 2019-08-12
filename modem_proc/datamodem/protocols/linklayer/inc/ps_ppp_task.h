#ifndef PS_PPP_TASK_H
#define PS_PPP_TASK_H
/*===========================================================================
                          P S _ P P P _ T A S K . H

DESCRIPTION
  Header file containing the PPP task specific information.  This is intended
  to isolate all of the task specific information to a single module.

Copyright (c) 2002-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_task.h_v   1.4   31 Jan 2003 18:45:14   ubabbar  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/linklayer/inc/ps_ppp_task.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/26/12    vpk    Use new core apis for tcb
10/17/11    ssh    eHRPD Always On support
06/06/11    ash    Added support for PPP Partial Context
12/05/10    ssh    Added ppp_ncp_abort()
12/05/10    ssh    Added ppp_ncp_resync()
08/31/09    mga    Merged from eHRPD branch
05/07/09    pp     CMI Phase-4: SU Level API Effort.
04/02/07    ssh    Added PPP vendor send command define
09/22/04    jd     Renamed PS_PPP_RM_RX_SIG to PS_RM_RX_SIG
07/15/04    ifk    Added PPP resync command define
05/03/04    sy     Added PPP_CURR_TASK() macro.
10/27/03    ifk    Added ppp_task_init() prototype
04/07/03    usb    Added PPP_INJECT_PACKET_CMD to replay an out of order PPP
                   packet for PPP receive processing.
01/27/03    usb    Added PPP_SET_MODE_CMD to set PPP mode in PS context only
09/23/02    mvl    Modified signal usage.
09/12/02    mvl    Removed netmodel featurization.
08/02/02    mvl    Added macro for setting a signal handler in PS.
07/25/02    mvl    Updates for PPP renaming.
07/11/02    mvl    Updates for new PPP architecture.
07/01/02    mvl    Added macros to make rest of PPP more independent of task.
05/22/02    mvl    created file
===========================================================================*/
#include "datamodem_variation.h"
#include "ps_svc.h"
#include "ps_ppp_defs.h"
#include "rcinit_rex.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                   MACROS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*---------------------------------------------------------------------------
  Define the task functions in terms of the equivalent functions for the
  hosting task: in this case PS
---------------------------------------------------------------------------*/
#define PPP_SET_SIG_HANDLER( signal, cback, user_data ) \
  ps_set_sig_handler((signal),(cback), (user_data))

#define PPP_SEND_CMD( cmd, data_ptr )           \
  ps_send_cmd((cmd), (void *) (data_ptr))

#define PPP_ENABLE_SIG( signal )                \
  ps_enable_sig( (signal) )

#define PPP_DISABLE_SIG( signal )               \
  ps_disable_sig( (signal) )

#ifdef FEATURE_MODEM_RCINIT_PHASE2
  #define PPP_IS_CURR_TASK_OWNER()                \
    (rex_self() == rcinit_lookup_rextask("ps"))
#else
  #define PPP_IS_CURR_TASK_OWNER()                \
    (rex_self() == &ps_tcb)
#endif

#define PPP_CURR_TASK()                \
  ((uint32)rex_self())


/*---------------------------------------------------------------------------
  define all of the PPP related commands that can be sent in terms of the
  commands for the hosting task: in this case PS
---------------------------------------------------------------------------*/
#define PPP_RESYNC_CMD          PS_PPP_RESYNC_CMD
#define PPP_START_CMD           PS_PPP_START_CMD
#define PPP_STOP_CMD            PS_PPP_STOP_CMD
#define PPP_ABORT_CMD           PS_PPP_ABORT_CMD
#define PPP_SET_MODE_CMD        PS_PPP_SET_MODE_CMD
#define PPP_INJECT_PKT_CMD      PS_PPP_INJECT_PKT_CMD
#define PPP_VENDOR_SEND_CMD     PS_PPP_VENDOR_SEND_CMD
#define PPP_NCP_START_CMD       PS_PPP_NCP_START_CMD
#define PPP_NCP_STOP_CMD        PS_PPP_NCP_STOP_CMD
#define PPP_NCP_RESYNC_CMD      PS_PPP_NCP_RESYNC_CMD
#define PPP_NCP_ABORT_CMD       PS_PPP_NCP_ABORT_CMD
#define PPP_DEVICE_STOP_CMD     PS_PPP_DEVICE_STOP_CMD
#define PPP_TRIGGER_LSD_CMD     PS_PPP_TRIGGER_LSD_CMD


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                             EXTERNAL FUNCTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION PPP_TASK_POWERUP_INIT()

DESCRIPTION
  This function does the powerup initialization for the PPP module.  This
  includes registering handlers with PS (the task executing it) for the PPP
  RX signals.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_powerup_init( void );


/*===========================================================================
FUNCTION PPP_TASK_INIT()

DESCRIPTION
  This function does the startup initialization for the PPP module.  This
  includes configuring the PPP configurable parameters.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_init( void );

/*===========================================================================
FUNCTION PPP_TASK_NV_INIT()

DESCRIPTION
  This function configures the PPP configurable parameters.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  Sets configurable protocol specific device constants
===========================================================================*/
void ppp_task_nv_init( void );

#endif /* PS_PPP_TASK_H */
