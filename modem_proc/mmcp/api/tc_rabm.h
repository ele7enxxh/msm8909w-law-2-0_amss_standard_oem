#ifndef TC_RABM_H
#define TC_RABM_H

/*===========================================================================
                        T C - R A B M  Header File

DESCRIPTION

   Definition of Primitive message types between Test Control Entity
   and Radio Access Bearer Management Entity (TC <-> RABM)

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/ctest/vcs/tc_rabm.h_v   1.2   11 Jul 2002 11:36:44   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/tc_rabm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/11/02   ks      Renamed the SIG_CONNECTION_RELEASE to TCRABM_SIG_CONNECTION_RELEASE.
09/17/01   ks      Created file.
            
===========================================================================*/
#include "sys_stru.h"
#include "sys_cnst.h"
#include "sys_type.h"

/* ---------------------------------------------------------------------------
* Message Primitive Types between Session Management (TC) and the RABM.
* --------------------------------------------------------------------------*/
             
// Enumeration for UE mode                 
typedef enum
{
  UE_NORMAL_MODE,
  UE_LB_MODE1,
  UE_LB_MODE2
} ue_mode_type_T;

/* Command types from TC to RABM are defined in RABMTASK.H*/

/* --------------------------------------------------------------------------
** Primitive : TC_MODE_CHANGE, used to indicate a PS RAB establishment.
** ------------------------------------------------------------------------*/
typedef struct tc_mode_change 
{
  ue_mode_type_T  mode_type;   // Current mode type
} tc_mode_change_T;


/* --------------------------------------------------------------------------
** The following are the Error causes between RABM and TC. 
** ------------------------------------------------------------------------*/
#define TCRABM_SIG_CONNECTION_RELEASE  0x20

/* Commands from RABM to TC */
/* These are defined in the TCTASK.H file */

/* --------------------------------------------------------------------------
** Data types used in the Primitives in RABMtc-SAP
** ------------------------------------------------------------------------*/

/**************************************************************************
            messages from RABM to TC , defined in table 9.5.1 in ts 24.007
**************************************************************************/

/* --------------------------------------------------------------------------
** Primitive : RABMTC_RAB_SETUP_IND, used to indicate a PS RAB establishment.
** ------------------------------------------------------------------------*/
typedef struct rabmtc_rab_setup_ind 
{
  byte            rab_id;   /* Rab_id that is established */
  cc_cause_T      cause;    /* defined in sys_type.h as byte */
} rabmtc_rab_setup_ind_T;

/* --------------------------------------------------------------------------
** Primitive : RABMTC_RAB_REL_IND, used to indicate a PS RAB Release.
** ------------------------------------------------------------------------*/
typedef struct rabmtc_rab_rel_ind
{ 
  byte            rab_id;   /* Rab_id that is released */
  cc_cause_T      cause;    /* defined in sys_type.h as byte */
} rabmtc_rab_rel_ind_T;

#endif

