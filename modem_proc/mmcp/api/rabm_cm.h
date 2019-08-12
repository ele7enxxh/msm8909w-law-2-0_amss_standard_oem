#ifndef RABM_CM_H
#define RABM_CM_H

/*===========================================================================
                        R A B M - C M  Header File

DESCRIPTION

   Definition of Primitive message types between Radio Access Bearer Manager 
   and Call Manager layer (RABM <-> CM)

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/rabm/vcs/rabm_cm.h_v   1.4   23 Apr 2002 16:29:28   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/rabm_cm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
09/26/12   am      Back off timer code
02/16/12   us      Adding new field in interface structure-> "rabm_rab_reestablish_ind_T" to send rat mode in RAB ESTABLISHMENT INDICATOR  
07/30/08   rm      CMI defeaturisation
04/22/02   ks      Added RABM_RAB_ESTABLISH_REJ in rabm_cm_enum_T, Added
                   rabm_rab_reestablish_fail_T for PDP context preservation.
11/21/01   ks      Added connection_id in the SM to CM primitives.
01/26/01   ks      Created file.
            
===========================================================================*/

#include "cm_gw.h"             /* Defintions from NAS required by clients */


#include "sys.h"

/* ---------------------------------------------------------------------------
* Message Primitive Types between RABM and CM.
* --------------------------------------------------------------------------*/

/* Note: Messages from CM to RABM are defined in rabmtask.h*/

/* Messages from RABM to CM */                                 
/* Note: These will be removed once CM defines in one of its headers */

typedef enum
{
  RABM_RAB_REESTABLISH_CNF,
  RABM_RAB_REESTABLISH_REJ,
  RABM_RAB_ESTABLISH_IND, 
  RABM_RAB_RELEASE_IND,
  RABM_RAB_ESTABLISH_REJ
} rabm_cm_enum_T;

/* --------------------------------------------------------------------------
 Enumeration for RAB re-estb rejections
** ------------------------------------------------------------------------*/
typedef enum
{
  REASON_NONE,
  RABM_APN_BLOCKED
}rabm_cm_status_T;

/**************************************************************************
      MESSAGES FROM CM TO RABM .
**************************************************************************/ 

/* --------------------------------------------------------------------------
** Primitive  : RABM_RAB_REESTABLISH_REQ
** Description: Mobile originated RAB establishment request.
** ------------------------------------------------------------------------*/
typedef struct rabm_rab_reestablish_req
{ 
  rabm_rabid_T                rabid;

  sys_modem_as_id_e_type    as_id;

} rabm_rab_reestablish_req_T;


/**************************************************************************
      MESSAGES FROM RABM to CM
**************************************************************************/ 

/* ------------------------------------------------------------------------
** Primitive  : RABM_RAB_REESTABLISH_CNF
** Description: Reestablish confirmation for the MO RAB reestablish command.
** -----------------------------------------------------------------------*/
typedef struct rabm_rab_reestablish_cnf
{
  byte                 connection_id;
  rabm_rabid_T         rabid;

  sys_modem_as_id_e_type    as_id;

} rabm_rab_reestablish_cnf_T;

/* ---------------------------------------------------------------------
** Primitive  : RABM_RAB_REESTABLISH_REJ,
** Description: Reject for the MO RAB reestablish Request.
** -------------------------------------------------------------------*/
typedef struct rabm_rab_reestablish_rej
{
  byte               connection_id;
  rabm_rabid_T       rabid;
  rabmcm_cause_T   cause;

  sys_modem_as_id_e_type    as_id;

} rabm_rab_reestablish_rej_T;

/* ------------------------------------------------------------------------
** Primitive  : RABM_RAB_ESTABLISH_IND
** Description: Network initiated RAB Reestablish indication.
** ----------------------------------------------------------------------*/
typedef struct rabm_rab_establish_ind
{
  byte               connection_id;
  rabm_rabid_T       rabid;

  sys_modem_as_id_e_type    as_id;
  sys_radio_access_tech_e_type sys_rat;

} rabm_rab_reestablish_ind_T;

/* ------------------------------------------------------------------------
** Primitive  : RABM_RAB_RELEASE_IND
** Description: Network initiated RAB release indication.
** ----------------------------------------------------------------------*/
typedef struct rabm_rab_release_ind
{
  byte               connection_id;
  rabm_rabid_T       rabid;

  sys_modem_as_id_e_type    as_id;

} rabm_rab_release_ind_T;

/* ------------------------------------------------------------------------
** Primitive  : RABM_RAB_REESTABLISH_CNF
** Description: Reestablish confirmation for the MO RAB reestablish command.
** -----------------------------------------------------------------------*/
typedef struct rabm_rab_reestablish_fail
{
  byte                 connection_id;
  rabm_rabid_T         rabid;

  sys_modem_as_id_e_type    as_id;

} rabm_rab_reestablish_fail_T;


#endif

