/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             P M    NPA   S E R V I C E S

GENERAL DESCRIPTION
  This file contains initialization functions for NPA

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2010           by Qualcomm Technologies, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/systemdrivers/pmic/npa/test/pm_npa_offtarget_test.c#1 $
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/12/11   wra     Uncommeted ULogDump to the pm_core_npa_offtarget_close ()
03/31/10   umr     Created.
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifdef PMIC_OFFTARGET_TESTING

#include "comdef.h"

#include "npa.h"
#include "npa_resource.h"
#include "npa_init.h"
#include "npa_remote_resource.h"
#include "npa_config.h"
#include "npa_remote_lp_protocol.h"
#include "npa_dump.h"

#include "DALSys.h"
#include "ULogDevelopment.h"
//#include "ULogFront.h"
//#include "psdte_8058.h"
/*===========================================================================

                 LOCAL CONSTANT AND MACRO DEFINITIONS

===========================================================================*/

/*===========================================================================

                LOCAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

                VARIABLES DEFINITIONS

===========================================================================*/


/*===========================================================================

                FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION pm_core_npa_offtarget_init

DESCRIPTION
    This function Initilizes the offline NPA lib.

    It does the following:
    * It initializes the NPA Lib.

INPUT PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  NPA Framework should be available in the build being compiled for

SIDE EFFECTS
  NONE.

===========================================================================*/

extern const npa_config *npa_target_config( void );

extern void pm_rpm_proc_sudo_rsrc_init(void);

void
pm_core_npa_offtarget_init (void)
{
  DALSYSConfig SysCfg = {NULL,0};
  npa_config *npa_config_data;

  /* Init NPA and its logging */
  DALSYS_InitMod (&SysCfg);
  ULogDevelopment_LoggerInitialize ("", 1);
  ULogDevelopment_LoggerStart ("NPA Log", 0x10000);
  npa_config_data = (npa_config *)npa_target_config();
  npa_config_data->log_buffer_size = 0x10000;

  npa_init ();
  npa_remote_resource_init ();
  npa_remote_lp_protocol_init ();

//kevin new framework
//  pm_rpm_proc_sudo_rsrc_init();

  rpmclient_init();

//kevin new framework
//  rpmserver_init();
}

void
pm_core_npa_offtarget_close (void)
{
  //ULogDevelopment_LoggerStopAll ();
  ULogHandle npaDumpLog;

  ULogFront_RealTimeInit(&npaDumpLog,            // The log to create
                       "NPA Dump Log",          // The log name
                                           // The amount of memory to 
                                           // allocate
                       65536,
                       ULOG_MEMORY_LOCAL,  // Local memory (normal case)
                       ULOG_LOCK_OS );   // Use a standard OS lock


  npa_dump(npaDumpLog, NULL);
  ULogDevelopment_LoggerPullOne("", "NPA Log");
  ULogDevelopment_LoggerPullOne("", "NPA Dump Log");
}

#endif //PMIC_OFFTARGET_TESTING
