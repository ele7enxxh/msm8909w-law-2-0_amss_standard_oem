#ifndef RABM_RR_H
#define RABM_RR_H

/*===========================================================================
                        R A B M - R R Header File

DESCRIPTION

   Definition of Primitive message types between Session Management Entity
   and Mobile Network layer (RABM <-> RR)

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/rabm/vcs/rabm_rr.h_v   1.1   25 Apr 2001 14:54:18   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/rabm_rr.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/26/00   ks      Created file.
            
===========================================================================*/

#include "sys.h"

/* --------------------------------------------------------------------------
** Enumeration used by RRC for querying a validity or a particular RAB.
** ------------------------------------------------------------------------*/
typedef enum
{
  RAB_ESTABLISH_QUERY,
  RAB_MODIFY_QUERY,
  RAB_RELEASE_QUERY
} rab_query_T;

/* ---------------------------------------------------------------------------
* Message Primitive Types between Radio Access Bearer Manager (RABM) and the
* Access Stratum layer (AS) at the service RABMAS-SAP (UMTS only).
* These are as defined in TR 24.007, table 9.3.3
* --------------------------------------------------------------------------*/

/* --------------------------------------------------------------------------
** Data types used in the Primitives in RABMAS-SAP
** ------------------------------------------------------------------------*/
typedef byte  rabid_T;
typedef byte  rab_cause_T;  // Exact enum. will be defined soon

/**************************************************************************
**           MESSAGES FROM AS TO RABM , DEFINED IN Table 9.3.3 in TS 24.007
**************************************************************************/                                                                  

/* --------------------------------------------------------------------------
** Primitive   : RABMAS_RAB_ESTABLISH_IND.
** Description : RRC informs RABM after establishing a RAB.
** ------------------------------------------------------------------------*/
typedef struct rabmas_rab_establish_ind
{ 
  rabid_T       rabid;

  sys_modem_as_id_e_type    as_id;

} rabmas_rab_establish_ind_T;

/* --------------------------------------------------------------------------
** Primitive   : RABMAS_RAB_RELEASE_IND.
** Description : RRC informs RABM if a RAB has been released. 
** ------------------------------------------------------------------------*/
typedef struct rabmas_rab_release_ind
{ 
  rabid_T       rabid;

  sys_modem_as_id_e_type    as_id;

} rabmas_rab_release_ind_T;

/* --------------------------------------------------------------------------
** Primitive   : RABMAS_STATUS_IND.
** Description : RRC informs RABM if a RAB setup/modification is failed.
** ------------------------------------------------------------------------*/
typedef struct rabmas_status_ind
{ 
  rabid_T       rab_id;
  rab_cause_T   cause;

  sys_modem_as_id_e_type    as_id;

} rabmas_status_ind_T;

/*-------------------------------------------------------------------
RRC to SM: RABMAS_FORCE_CLOSE_SESSION_IND
Used by RRC to abort PS session if PDP states allow.
--------------------------------------------------------------------*/
typedef struct
{
  sys_modem_as_id_e_type     as_id;
}rabmas_force_close_session_ind_type;

#endif

