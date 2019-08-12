#ifndef RR_CGPS_DEFS_H
#define RR_CGPS_DEFS_H
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2002-2013 Qualcomm Technologies, Inc.
Reserved.  QUALCOMM Proprietary.  Export of this technology or software
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/rr_cgps_defs.h#1 $ $DateTime: 2016/12/13 08:00:21 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10-04-01   tjw     Add customer.h and comdef.h to all SU API files
5/7/2007   kgj     derived from the old lsm_rr.h file
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "sys_type.h"
#include "nas_exp.h"
#include "sys.h"

#define MAX_APDU_DATA_LEN 500

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

typedef enum
{
  /* CGPS to RR message Types */
  RR_APDU_REQ,
  RR_CGPS_EVENT_REQ
  /* RR to CGPS message Types */
} cgps_rr_message_id_enum_T;

/************************************************************************/
/* RR_APDU_REQUEST primitive                                            */
/* FROM : GPS                                                           */
/* TO   : RR                                                            */
/* DESCRIPTION :                                                        */
/* Is used by GPS to request the sending of apdu data to its peer       */
/* entity on the network side                                           */
/************************************************************************/

typedef struct
{
   IMH_T                   message_header;
   uint16                  apdu_len;
   byte                    apdu_data[MAX_APDU_DATA_LEN];
   sys_modem_as_id_e_type  as_id;
} rr_apdu_req_T;

typedef struct
{
   IMH_T                   message_header;
   /* TRUE to start notifying GPS on RR managament procedures; FALSE to stop */
   boolean                 send_rr_event;
   sys_modem_as_id_e_type  as_id;
} rr_cgps_event_req_T;

#endif  /* RR_CGPS_DEFS_H */

