#ifndef TMS_DIAGPKT_H
#define TMS_DIAGPKT_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              Defnitions for diagpkts used by various TMS modules

GENERAL DESCRIPTION
  Common internal defniitions for diag command handling by TMS 

INITIALIZATION AND SEQUENCING REQUIREMENTS
  No requirements

Copyright (c) 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*==========================================================================

when       who     what, where, why
--------   ---     ------------------------------------------------------------
02/25/15   abh     Added changes to supress medium level warnings (CR799478)
11/04/14   dav     File created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "diagpkt.h"
#include <stdint.h>
#include "DALSys.h"
#include "DALPropDef.h"
#include "msg.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

#define TMS_DIAG_TBLSIZE 3

// DO NOT ARBITRARILY RESIZE THIS ENUMERATION : LEGACY OPERATION REQUIRES THAT
// UNTIL DIAG_F3_TRACE IS IT'S OWN DISTINCT SUBSYSTEM, ITS COMMANDS ARE INTERLEAVED
// WITH THOSE OF ERR.
typedef enum
{
  TMS_DIAGPKT_DEPRECATED_0, 
  TMS_DIAGPKT_DEPRECATED_1,
  TMS_DIAGPKT_DEPRECATED_2,   /* Delete type trace file          */
  TMS_DIAGPKT_ERR_CORE_DUMP,
  TMS_DIAGPKT_DEPRECATED_3,   /* Get number of type saved        */
  TMS_DIAGPKT_DEPRECATED_4,   /* Get next type saved             */
  TMS_DIAGPKT_DEPRECATED_5,   /* Get names of all trace files       */
  TMS_DIAGPKT_DEPRECATED_6,   /* Get number of type saved from file */
  TMS_DIAGPKT_DEPRECATED_7,   /* Get next type saved from file      */
  TMS_DIAGPKT_DEPRECATED_8,   /* Delete specified trace file        */
  TMS_DIAGPKT_DEPRECATED_9,
  TMS_DIAGPKT_SYS_M_SHUTDOWN_DELAY, /* Delay shutdown completion to allow diag additional time to flush data*/
  TMS_DIAGPKT_SYS_M_RESTART,
}CMD;

typedef PACK(struct) 
{
  diagpkt_subsys_header_type hdr;
} tms_diag_req_type;

diagpkt_user_table_entry_type tms_diag_build_table_entry(CMD cmd, PACK(void*)(*cb)(PACK(void *), uint16));

void tms_diag_send_generic_response(PACK(void*) req);

#endif //TMS_DIAGPKT_H
