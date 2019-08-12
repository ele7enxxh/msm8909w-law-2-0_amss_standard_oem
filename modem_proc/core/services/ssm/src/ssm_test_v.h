#ifndef SSM_TEST_H
#define SSM_TEST_H
/*==========================================================================

               Diagnostic Packet Definitions for SSM

  Description: Packet definitions between the diagnostic subsystem
  and the external device.

  !!! NOTE: All member structures of diag packets must be PACKED.

  !!! WARNING: Each command code number is part of the externalized
  diagnostic command interface.  This number *MUST* be assigned
  by a member of QCT's tools development team.

Copyright (c) 2012 by QUALCOMM Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            Edit History

  $Header: //components/rel/core.mpss/3.5.c12.3/services/ssm/src/ssm_test_v.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/13   din     Changes pertaining to ADSP were done.
09/03/11   din     File created.

===========================================================================*/

#include "comdef.h"

#include "diagcmd.h"
#include "diagpkt.h"
#include "diagi.h"
#include "log_codes.h"
#include "feature.h"
#include "msg.h"


/* -------------------------------------------------------------------------
** Packet Definitions
** ------------------------------------------------------------------------- */
#if defined(T_WINNT) || defined(FEATURE_WINCE)
#error code not present
#endif

#define DIAG_TIME_DATA_SIZE 7

#define SSM_TEST_START_F  0
#define SSM_TEST_END_F  255


/*==========================================================================

PACKET   SSM_TEST_F

PURPOSE  Sent from the DM to the DMSS to request execution of a Time Service
         command. Usage: send_data 75 69 <SSCmd code> 0

============================================================================*/

typedef struct
{
  diagpkt_subsys_header_type header;        /* Sub System header */
  uint16 command;                           /* Which command */
  uint16 command_data[DIAG_TIME_DATA_SIZE]; /* Command parameters -to be used by function */
  uint16 return_stat;                       /* Return Status */

}SSM_TEST_F_req_type;

typedef struct
{
  diagpkt_subsys_header_type header;        /* Sub System header */
  uint16 command;                           /* Which command */
  uint16 command_data[DIAG_TIME_DATA_SIZE]; /* Command parameters -to be used by function */
  uint16 return_stat;                       /* Return Status */

}SSM_TEST_F_rsp_type;

void ssm_test_init( void );

#endif /* SSM_TEST_H */
