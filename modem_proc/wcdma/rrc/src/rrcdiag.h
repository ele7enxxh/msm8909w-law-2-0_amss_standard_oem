#ifndef RRCDIAG_H
#define RRCDIAG_H
/*==========================================================================

                      RRC Diagnostic Packet Definitions

  Description: Packet definitions between the RRC diagnostic subsystem
  and the external device.

Copyright (c) 2003-2004, 2007-2008 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

when       who    what, where, why
--------   ---    ----------------------------------------------------------
05/08/09   ss     Updated Copyright Information
09/03/04   vm     Created file

===========================================================================*/

#include "comdef.h"
#include "diagpkt.h"


/* Subsystem command codes */
#define RRCDIAG_RRC_GET_STATE_F 0


/* -------------------------------------------------------------------------
** Packet definitions.
** ------------------------------------------------------------------------- */

/*==========================================================================

PACKET   RRCDIAG_RRC_GET_STATE

PURPOSE  Request packet sent from the DM to the DMSS to return the 
         current state of RRC 
         
============================================================================*/
typedef struct {
  diagpkt_subsys_header_type header;         /* Sub System header */
} rrcdiag_rrc_get_state_req_type;


typedef PACKED struct {
  diagpkt_subsys_header_type header;         /* Sub System header */
  uint8                      rrc_curr_state; /* rrc current state */
} rrcdiag_rrc_get_state_rsp_type;


/*---------------------------------------------------------------------------
                         FUNCTION PROTOTYPE
---------------------------------------------------------------------------*/
void rrcdiag_init (void);

#endif /* RRCDIAG_H */
