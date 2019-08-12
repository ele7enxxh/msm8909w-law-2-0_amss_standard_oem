#ifndef RRCPG2_H
#define RRCPG2_H
/*===========================================================================
                  R R C  P A G I N G  T Y P E 2

DESCRIPTION
  This header file contains prototype declarations for Paging Type 2 
  procedure.

Copyright (c) 2002,2003,2007,2008 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/RRC/ref/rrcpg2.h_v   1.0   14 Feb 2002 18:35:38   kiranc  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rrc/src/rrcpg2.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/08/09   ss     Updated Copyright Information
02/07/02   rj     Created file.

===========================================================================*/


/* ==========================================================================
** Includes and Public Data Declarations
** ========================================================================*/

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

FUNCTION rrcpg2_init_procedure

DESCRIPTION
  This function initializes everything that the Paging Type 2 procedure
  needs to have initialized.

DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcpg2_init_procedure(void);

/*===========================================================================

FUNCTION rrcpg2_procedure_event_handler

DESCRIPTION
  This procedure is the event handler for the Paging Type 2 procedure.
  The following events are handled by this procedure:

    RRC_DOWNLINK_SDU_IND Primitive (received from RLC)


DEPENDENCIES
  None.

RETURN VALUE
  None.

===========================================================================*/

void rrcpg2_procedure_event_handler(rrc_cmd_type *cmd_ptr);


#endif /* RRCPG2_H */

