#ifndef DIAG_UMTS_CMD_V_H
#define DIAG_UMTS_CMD_V_H
/*==========================================================================

      Diagnostic Services Packet Processing Command Code Defintions

Description
  This file contains packet id definitions for the serial interface to
  the dmss. This file contains packets common to WCDMA and GSM modes (UMTS)

Copyright (c) 1993-2013 Qualcomm Technologies, Inc.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/umtsdiag_v.h#1 $

when       who     what, where, why
--------   ---    -----------------------------------------------------------
12/09/02   ATM    Move IMSI & IMEI logging to common location -- to be
                  triggered by MM
04/10/02   ATM    Added Front-End Filter to CFA
02/13/02   ATM    Updated command set to match UMTS ICD Revision A
12/14/01   ATM    Created the file adapted from diagpkt_wcdma_cmd.h
===========================================================================*/

#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "diagpkt.h"

/**************************************************************************
    PACKET DEFINITIONS
**************************************************************************/

/*---------------------------------------------------------------------------
    UMTS CFA CONFIG -- SUBCOMMAND 9
---------------------------------------------------------------------------*/
/* Request packet  -- modeled after DIAG_LOG_CONFIG_F */
DIAGPKT_SUBSYS_REQ_DEFINE(UMTS, UMTS_CFA_CONFIG_F)
  uint32 operation;  /* CFA specific operation    */
  uint32 op_data[1]; /* Pointer to operation data */
DIAGPKT_RSP_END

/* Response packet */
DIAGPKT_SUBSYS_RSP_DEFINE(UMTS, UMTS_CFA_CONFIG_F)
  uint32 operation;  /* CFA specific operation    */
  uint32 status;
  uint32 op_data[1]; /* Pointer to operation data */
DIAGPKT_RSP_END

#endif
