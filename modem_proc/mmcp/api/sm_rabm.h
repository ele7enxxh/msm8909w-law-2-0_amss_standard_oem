#ifndef SM_RABM_H
#define SM_RABM_H

/*===========================================================================
                        S M - R A B M Header File
DESCRIPTION

   Definition of Primitive message types between Session Management Entity
   and Radio Access Bearer Management (RABM) Entity (SM <-> RABM)

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/sm/vcs/sm_rabm.h_v   1.1   25 Apr 2001 14:40:00   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sm_rabm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/13/05   kvk     Added nsapi to rabm_status_req_T structure
01/26/00   ks      Created file.
            
===========================================================================*/

/* --------------------------------------------------------------------------
** data types defined.
** ------------------------------------------------------------------------*/
typedef byte rabm_nsapi_T;
typedef byte rabm_cause_T;

/* ---------------------------------------------------------------------------
* Message Primitive Types between Radio Access Bearer Manager (RABM) and the
* Session Managment (SM) at the service RABMAS-SAP (UMTS only).
* These are as defined in TR 24.007, table 9.5.1
* --------------------------------------------------------------------------*/

/**************************************************************************
          messages from SM to RABM , defined in table 6.5.3 in ts 24.007
**************************************************************************/                                                                  

/* --------------------------------------------------------------------------
** This data structure is used by Primitive : RABMSM_ACTIVATE_IND.
** ------------------------------------------------------------------------*/
typedef struct rabmsm_activate_ind
{ 
  rabm_nsapi_T       nsapi;

  sys_modem_as_id_e_type    as_id;

} rabmsm_activate_ind_T;

/* --------------------------------------------------------------------------
** Primitive : RABMSM_DEACTIVATE_IND.
** ------------------------------------------------------------------------*/
typedef struct rabmsm_deactivate_ind
{ 
  rabm_nsapi_T       nsapi;  

  sys_modem_as_id_e_type    as_id;

} rabmsm_deactivate_ind_T;

/* --------------------------------------------------------------------------
** Primitive : RABMSM_MODIFY_IND.
** ------------------------------------------------------------------------*/
typedef struct rabmsm_modify_ind
{ 
  rabm_nsapi_T       nsapi;

  sys_modem_as_id_e_type    as_id;

} rabmsm_modify_ind_T;

/* --------------------------------------------------------------------------
** Primitive : RABMSM_SIG_CON_RELEASE_IND.
** ------------------------------------------------------------------------*/
typedef struct rabmsm_release_ind
{ 

  sys_modem_as_id_e_type    as_id;

} rabmsm_release_ind_T;

/* --------------------------------------------------------------------------
** Primitive : RABMSM_RAT_CHANGE_IND.
** ------------------------------------------------------------------------*/
typedef struct rabmsm_rat_change_ind
{ 

  sys_modem_as_id_e_type    as_id;

} rabmsm_rat_change_ind_T;

/* --------------------------------------------------------------------------
** Primitive : RABMSM_RAT_RESUME_IND.
** ------------------------------------------------------------------------*/
typedef struct rabmsm_rat_resume_ind
{ 

  sys_modem_as_id_e_type    as_id;

} rabmsm_rat_resume_ind_T;

/**************************************************************************
            messages from RABM to SM , defined in table 6.5.3 in ts 24.007
**************************************************************************/

/* --------------------------------------------------------------------------
** Primitive : RABMSM_ACTIVATE_RSP.
** ------------------------------------------------------------------------*/
typedef struct rabmsm_activate_rsp
{ 
  rabm_nsapi_T       nsapi;

  sys_modem_as_id_e_type    as_id;

} rabmsm_activate_rsp_T;

/* --------------------------------------------------------------------------
** Primitive : RABMSM_DEACTIVATE_RSP.
** ------------------------------------------------------------------------*/
typedef struct rabmsm_deactivate_rsp
{ 
  rabm_nsapi_T       nsapi; 

  sys_modem_as_id_e_type    as_id;

} rabmsm_deactivate_rsp_T;

/* --------------------------------------------------------------------------
** Primitive : RABMSM_MODIFY_RSP.
** ------------------------------------------------------------------------*/
typedef struct rabmsm_modify_rsp
{ 
  rabm_nsapi_T       nsapi; 

  sys_modem_as_id_e_type    as_id;

} rabmsm_modify_rsp_T;

/* --------------------------------------------------------------------------
** Primitive : RABMSM_STATUS_REQ.
** ------------------------------------------------------------------------*/
typedef struct rabmsm_status_req
{ 
  rabm_nsapi_T       nsapi;
  rabm_cause_T       cause;

  sys_modem_as_id_e_type    as_id;

} rabmsm_status_req_T;
/* --------------------------------------------------------------------------
** Primitive : RABMSM_REEST_IND.
** ------------------------------------------------------------------------*/
typedef struct rabmsm_reest_ind
{ 

  sys_modem_as_id_e_type    as_id;

} rabmsm_reest_ind_T;

/* --------------------------------------------------------------------------
** Various Cause values between the AS and the NAS (RABM)
** These are defined in section. 9.2.1.4 of TS 25.413 
** ------------------------------------------------------------------------*/

#endif
