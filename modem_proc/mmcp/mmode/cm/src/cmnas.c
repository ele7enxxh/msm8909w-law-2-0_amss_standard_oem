/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

            C A L L   M A N A G E R  N A S  M O D U L E

GENERAL DESCRIPTION

  This module contains the Call Manager NAS Object.
  It is the main implementation of shuffling events from NAS to CM clients.

EXTERNALIZED FUNCTIONS
  Functional Interface:
    cmnas_ptr
    cmnas_client_cmd_proc

INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmsms_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2004 - 2006 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/cm/src/cmnas.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/02/12   gm      Remove unnecessary TMC references.
03/23/12   gm      DIME integration fix: TMC deprecation and others.
04/11/11   am      Cleaning stubs and runnning qtf SUTE
11/02/10   am      Replacing FEATURE_MOB_MMODE_CUTE
05/21/10   ak      Mainlining memory optimization changes.
10/14/09   sg      Do not use "tmc_heap"/gs_free() if feature FEATURE_MODEM_HEAP
                   defined in the build.
03/23/09   rm      Adding _v.h files from NAS
02/11/08   ic      Fixed compiler warnings
09/18/07   rn      Swapped "from ID" & "to ID" in Message ID Range (in correct way)
04/09/07   rp      Changes for SMS domain selection
04/04/07   rp      Changing type for cell_change_type
02/13/07   rp      FEATURE_WMS_CM_NAS_INTERFACES related changes (bound checks, new commands 
                   etc)
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
11/08/05   sk      Separated CM reports and commands.
02/03/05   ic      Added support for FEATURE_UMTS_BMC
12/03/04   ws      Initial Release

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"  /* Customer configuration file */
#include "cm.h"        /* External interface to cm.c */
#include "cm_v.h"
#include "cmi.h"       /* Internal interface to cm.c */
#include "cmdbg.h"

#ifdef CM_GW_SUPPORTED
#ifdef FEATURE_WMS_CM_NAS_INTERFACES
#error code not present
#endif /* FEATURE_WMS_CM_NAS_INTERFACES */
#endif  /* FEATURE_GSM || FEATURE_WCDMA */
