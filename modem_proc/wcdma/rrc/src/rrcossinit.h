#ifndef RRCOSSINIT_H
#define RRCOSSINIT_H
/*===========================================================================
                          R R C O S S I N I T

DESCRIPTION
  This header file contains function prototypes for the functions in 
  rrcossinit.c.

Copyright (c) 2000,2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/ossarm7ads11/vcs/rrcossinit.h_v   1.0   18 Jun 2001 09:24:52   sorenson  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcossinit.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/12   geg     Upmerge WCDMA.MPSS.1.0 to WCDMA.MPSS.2.0
04/10/12   gv      Made changes to remove unused code
09/21/11   gv      Added prototype for rrc_init_osys_context
11/29/10   sks     Fixed bugs found during testing.
11/03/10   rl      Fixed review comments
10/30/10   rl      Objective systems migration changes check in 
                   on code base VU_MODEM_WCDMA_RRC.01.80.00
05/13/10   ss      Fixed compiler warnings.
05/12/10   ss      Fixed compile errors.
05/07/10   ss      Fixed compiler warnings.
05/08/09   ss      Updated Copyright Information
12/14/00   jrs     Updated the header file to match the C file.
12/13/00   jrs     Created file

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

#include "uecomdef.h"

/* -----------------------------------------------------------------------
** Include Files
** ----------------------------------------------------------------------- */



/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */


/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */



/* =======================================================================
**                          Macro Definitions
** ======================================================================= */



/* =======================================================================
**                        Function Declarations
** ======================================================================= */


/*===========================================================================

FUNCTION rrc_oss_init

DESCRIPTION
  This function initializes the OSS ASN.1 subsystem. It also sets the encoding
  flags, decoding flags, and encoding rules.

DEPENDENCIES
  None.

RETURN VALUE
  SUCCESS if the OSS ASN.1 subsystem was initialized successfully, otherwisw 
  FAILURE. When FAILURE is returned, the UE should go offline because there
  is somethign drastically wrong with the ASN.1 subsystem and RRC will be 
  unable to process signalling messages.

SIDE EFFECTS
  None.

===========================================================================*/

void rrc_obj_sys_init(void);

/*===========================================================================
  
FUNCTION rrc_osys_set_mem_functions

DESCRIPTION
  This function is an empty function, to support compilation dependencies
  
DEPENDENCIES

RETURN VALUE
Void

SIDE EFFECTS
None.
  
===========================================================================*/

void rrc_osys_set_mem_functions(void);

/*===========================================================================

FUNCTION rrc_init_osys_context

DESCRIPTION
  This function inits asn1 context 

DEPENDENCIES

RETURN VALUE
  Non-zero if an error occurred; else zero.

SIDE EFFECTS
  None.
  
===========================================================================*/
int rrc_init_osys_context
(
  OSCTXT* pctxt
);
#endif /* RRCOSSINIT_H */
