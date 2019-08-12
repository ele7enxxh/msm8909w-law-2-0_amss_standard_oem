#ifndef DSWCSD_H
#define DSWCSD_H
/*===========================================================================

             W C D M A   C I R C U I T - S W I T C H E D   D A T A

                          H E A D E R   F I L E

DESCRIPTION
  This file contains common data declarations and function prototypes for WCDMA 
  Circuit-Switched Data Services. 

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001 - 2003 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dswcsd.h_v   1.9   06 May 2002 14:52:46   degangy  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/csd/inc/dswcsd.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/03   dgy     Added support for UMTS CS Data handlers. Removed
                   FEATURE_DATA_WCDMA_ASYNC_T_CS. Moved some data definitions
                   and declaration to dswcsdi.h, dswcsdul.h, and 
                   dswcsdultask.c.
03/07/03   dwp     Make sync test app runtime configurable.
01/01/03   dwp     Add initial V.80 support. Update copyright.
11/18/02   dgy     Removed some defines and renamed a define.
05/06/02   dgy     Added variables for Asynchronous CS Data.
03/15/02   dgy     Added Synchronous Transparent CS Data test application 
                   under FEATURE_DATA_WCDMA_SYNC_T_CS_TEST_APP.
03/12/02   dgy     Added a definition.
01/15/02   dgy     Added and featurized 40msec TTI for 64k.
11/07/01   smp     Renamed default_bc_udi to dswcsd_default_bc_udi and
                   changed the type to cm_bearer_capability_s_type.
10/30/01   dgy     Added definitions for maximum number of SDUs per TTI.
09/20/01   dgy     Removed the command DSWCSD_PROC_DL_RLC_DATA_CMD.
08/31/01   dgy     Changed include file and MMI data type names.
08/29/01   dgy     Changed FEATURE_WCDMA_CS_DATA to FEATURE_DATA_WCDMA_CS.
08/17/01   dgy     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"


#if defined (FEATURE_WCDMA) || defined (FEATURE_TDSCDMA)

#ifdef FEATURE_DATA_WCDMA_CS
#include "comdef.h"
#include "dsm.h"

#ifdef FEATURE_DATA_V80
#include "dswcsdv80.h"
#endif


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  WCDMA CS Data Task Command Definitions
---------------------------------------------------------------------------*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Commands that may be sent to the WCDMA CS Data Uplink/Downlink Tasks.
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef enum
{
  DSWCSD_CONNECT_CALL_CMD,          /* Notification that call is connected */
  DSWCSD_V80_CONNECT_CALL_CMD, /* Notification that V.80 call is connected */
  DSWCSD_END_CALL_CMD,                         /* Notification of call end */
  DSWCSD_V80_SEND_DTE_CMD,       /* Notify DL task to send V.80 cmd to DTE */
  DSWCSD_CMD_MAX          /* Max number of commands. Must be the last item */
} dswcsd_cmd_enum_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command Header type
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
  q_link_type          link;                            /* Queue link type */
  dswcsd_cmd_enum_type cmd_id;                       /* Command identifier */
  sys_modem_as_id_e_type subs_id;
} dswcsd_cmd_hdr_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command Data types
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
{
  dsm_item_type **dsm_item_ptr;                  /* Pointer to an DSM item */
} dswcsd_dsm_item_type;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
  Command type
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
typedef struct
{
  dswcsd_cmd_hdr_type    hdr;

  union
  {
    dswcsd_dsm_item_type    dsm_item;
#ifdef FEATURE_DATA_V80
    dswcsdv80_cmd_enum_type v80_cmd;
#endif
  } cmd;

} dswcsd_cmd_type;

#endif /* FEATURE_DATA_WCDMA_CS */

#endif /* FEATURE_WCDMA || FEATURE_TDSCDMA */


#endif /* DSWCSD_H */
