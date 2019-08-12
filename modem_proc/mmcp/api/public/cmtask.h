#ifndef CMTASK_H
#define CMTASK_H
/*===========================================================================

         C A L L   M A N A G E R   T A S K   H E A D E R   F I L E
 
===========================================================================*/ 


/*===========================================================================
Copyright (c) 2001 - 2010 by Qualcomm Technologies INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/

/**
  @file cmtask.h
  Definitions for interfacing with CMTASK.C.
*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/public/cmtask.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/10   kl/gm   (Tech Pubs) Edited/added Doxygen comments and markup.
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
09/16/08   pk      Added support for WLAN UOoS
09/04/08   dm/sv   Added callback function for the WPS result
07/18/08   ks      Added new info for Current Operator's Cell ID and 
                   Neighbors Cell IDs
04/01/06   ka      Changes for emerg serv category during orig, aborting 
                   manual search and returning current band with srv ind.                  
01/30/06   sk      Added debug info to help catch CM cmd buffer memory leaks.
01/06/06   ic      Lint cleanup  
12/07/05   ka      Exporting cm_si_network_list_cb
10/13/05   ic      Externalized cmtask_crit_sect_ptr()
09/08/04   jqi     More ps dormant optimization.
08/27/04   jqi     Added ps dormant optimization.
06/29/04   jqi     Avoid using the value of common signals.
06/14/04   jqi     Made CM MMGSDI calls to be asynchronouse.
06/09/04   dk      Added CM Heap support.
04/13/04   ic      Added prototype of cm_si_acq_fail_cb()
03/24/04   ic      Added prototype of cm_si_ok_to_orig_cb()
02/06/04   ws      Initial jaguar/mainline merge.
01/21/03   ws      Updated copyright information for 2003
11/01/02   vt      Rearranged CM signals.
10/07/02   vt      Defined signals for regproxy.
07/08/02   vt      Changed the definitions of SIG's to avoid conflicts with
                   TASK signal definitions.
04/16/02   AT      Merged Advice of Charge with GSDI and UI.
01/18/02   ic      Merged MSM_CM.05.03.33 on top of tip CCS CM
                   Diffed MSM_CM.05.03.32 with MSM_CM.05.03.33 and only the changes
                   between those two were merged with CCS CM
12/13/01   ic      Merged MSM_CM.05.03.32 onto SVD CM tip.
                   CCS CM code started from CM VU MSM_CM.05.03.25 + WCDMA/GSM
                   stuff from SIMBA team merged on top of it.
                   We diff'ed MSM_CM.05.03.25 with MSM_CM.05.03.32 (CM VU in N*2165)
                   that we needed to merge with and changes between those two VUs
                   were merged onto CCS CM tip.
10/23/01   VT/IC   Baseline for CCS
09/13/01   RI      Initial Release
===========================================================================*/


#include "rex.h"

/*---------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/

/** @addtogroup CM_TASK Call Manager Task
    @{ */

/**
  Timer durations.
*/
#define CM_TIME            1000
/* Time to update CM */

/*===========================================================================
FUNCTION CM_TASK
===========================================================================*/
/**
  The main call manager function.

  This is the big loop outside of the state machine that handles signals
  and a variety of inputs.

@param[in] dummy A dummy doubleword.

@return
  Never exits.

@dependencies
  REX is running.
*/
extern void cm_task
(
  dword dummy
);

/*===========================================================================

FUNCTION cm_init_before_task_start

DESCRIPTION
  Initialize CM queues and other objects that needs to be initialized
  before we report back to MC that CM is ready to run by calling on
  mc_task_start().

  NOTE! Any initialization that must be done before other tasks or clients
  tries to contact CM, must be done in this initialization function,
  as opposed to the cm_init_after_task_start() function.

  This function must be called before any other CM function is ever called.

DEPENDENCIES
  none

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
void cm_init_before_task_start( void );

/** @} */ /* end_addtogroup CM_TASK Call Manager Task */

#endif  /* CMTASK_H */
