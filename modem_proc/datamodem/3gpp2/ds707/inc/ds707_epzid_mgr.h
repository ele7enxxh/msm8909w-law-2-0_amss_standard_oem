#ifndef DS707_EPZID_MGR_H
#define DS707_EPZID_MGR_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S   E P Z I D

                                 M A N A G E R

GENERAL DESCRIPTION
  This file contains functionality of EPZID Manager. The EPZID Manager 
  maintains the EPZID list and provides an interface to the other modules.
  All the PZID/SID/NID commands from DS707 are funneled through EPZID 
  Manager

EXTERNALIZED FUNCTIONS
  DS707_MGR_EPZID_PARSE_SOCM
    Used for parsing the service option control message which contains the
    EPZID reconnect parameters.

  DS707_MGR_PROCESS_CMD
    Processes DS task commands specific to EPZID.  
    
  DS707_MGR_EPZID_INIT
    Registers with for 707 events.  

INITIALIZATION AND SEQUENCING REQUIREMENTS
    Must call ds707_epzid_mgr_init() at startup.

  Copyright (c) 2002 - 2009 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_epzid.h_v   1.1   18 Nov 2002 21:33:56   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds707_epzid_mgr.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/09/09   sa      AU level CMI modifications.
04/05/05   gr      Initial version of the file to support IS707-B

===========================================================================*/

/*===========================================================================
                       INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "dstask_v.h"

/*===========================================================================
                                TYPEDEFS
===========================================================================*/

/*===========================================================================
                       EXTERN VARIABLES
===========================================================================*/

/* Minimum EPZID list size to start HT on the last EPZID entry             */
#define DS707_EPZID_MIN_LIST_SIZE_TO_START_HT  2 
/*===========================================================================
                                 EXTERNS
===========================================================================*/
/*===========================================================================
                        EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION       DS707_EPZID_MGR_INIT

DESCRIPTION    Initialize EPZID Manager module.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None.
===========================================================================*/
void ds707_epzid_mgr_init(void);

/*===========================================================================
FUNCTION      DS707_EPZID_MGR_REG_RECON

DESCRIPTION   This function registers PZID based reconnection for packet
              calls.  It registers the pzid change callbacks and
              initializes the PZID list to one entry.
  
              This function is called when (see sec.2.2.7 of IS-707.A.5):
              (a) A packet call (sockets/relay/network) is initiated (packet
              service goes into active state).
  
              Note, the idle change handler will reoriginate a packet call on
              detecting a PZID change.
          
              This function can be called from the DS task.  Note that this 
              can be called multiple times in a data call, as the IFACE goes
              between ROUTEABLE and UP (could be Rm MIP call).  Protect
              against losing PZID list by making callback hasn't been
              registered by checking max_pzid_list_len.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_epzid_mgr_reg_recon(void);

/*===========================================================================
FUNCTION      DS707_EPZID_MGR_DEREG_RECON

DESCRIPTION   This function deregisters PZID based reconnection for packet
              calls.  It sets the PZID change handlers to NULL and resets
              the visited PZID list.
   
              This function is called when (see sec.2.2.7 of IS-707.A.5):
              (a) A packet call (sockets/relay/network) is ended (packet
              service goes into Inactive state) - not called when packet data
              service goes dormant.
   
              This function can be called from the DS task

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void  ds707_epzid_mgr_dereg_recon(void);

/*===========================================================================
FUNCTION      DS707_EPZID_MGR_PROCESS_CMD

DESCRIPTION   This function processes PZID specific commands.

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_epzid_mgr_process_cmd
(
  ds_cmd_type   *ds_cmd_ptr
);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_EPZID_MGR_H */

