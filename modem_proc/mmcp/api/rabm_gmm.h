#ifndef RABM_GMM_H
#define RABM_GMM_H

/*===========================================================================
                        R A B M - G M M Header File

DESCRIPTION

   Definition of Primitive message types between Radio Access Bearer Management
   (RABM) Entity and GPRS Mobility Management Entity (RABM <-> GMM)

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/rabm/vcs/rabm_gmm.h_v   1.5   18 Sep 2001 10:11:34   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/rabm_gmm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/07   vsm    Added a new primitive from RABM to GMM gmmrabm_rab_rel_ind to indicate
                           GMM when an a rab belonging to an active PDP is released by AS.
11/21/05   kvk     Added traffic_class fiels in gmmrabm_reestablish_req_T primitive
02/28/03   ks      Added cause field in gmmrabm_reestablish_req_T primitive.
                   for setting the right RRC cause during connection establishment.
09/16/01   ks      Removed gmmrabm_reestablish_rej_T as this primitive is not
                   needed for the corresponding command from GMM.
09/06/01   ks      Added IMH_T header in gmmrabm_reestablish_req_T for 
                   inter-task communication between SM-GMM. 
06/04/01   ks      Added definition for MAX_RAB_NUM.
01/26/01   ks      Created file.
            
===========================================================================*/
#include "sys_stru.h"
#include "comdef.h"
#include "rrcmmif.h"

#include "sys.h"


/* ---------------------------------------------------------------------------
* Message Primitive Types between Radio Access Bearer Managment (RABM) and 
*  the GMM. These are as defined in TR 24.007, table 9.5.5
* --------------------------------------------------------------------------*/

/* Primitives from RABM to GMM */
/* Note: This will be removed if GMM includes it in one of its header files */

#define GMMRABM_REESTABLISH_REQ     0x01

#define GMMRABM_RAB_REL_IND             0x02
#define GMMRABM_RAB_EST_IND             0x03


#define GMM_PDP_CONTEXT_STATUS_LENGTH      0x02

typedef struct
{
  byte length;
  byte value[GMM_PDP_CONTEXT_STATUS_LENGTH];
}gmm_ie_pdp_context_status_T;
/* Primitives from GMM to RABM */
/* Note: These command IDs are defined in SMTASK.H file */

/* --------------------------------------------------------------------------
** Data types used in the Primitives in GMMSM-SAP
** ------------------------------------------------------------------------*/

/**************************************************************************
          messages from RABM to GMM , defined in table 9.5.5 in ts 24.007
**************************************************************************/                                                                  

/* --------------------------------------------------------------------------
** Primitive : GMMRABM_REESTABLISH_REQ.
** ------------------------------------------------------------------------*/
typedef struct gmmrabm_reestablish_req
{ 
  IMH_T         header;                /* Header for inter-task communication */
  byte          traffic_class;         /* Traffic class of the PDP context */
  rrc_establish_cause_e_type cause;    /* Establish cause for RRC connection */  

  gmm_ie_pdp_context_status_T uplink_data_status; /*RAB to be re-established*/


  sys_modem_as_id_e_type    as_id;

} gmmrabm_reestablish_req_T;


/* --------------------------------------------------------------------------
** Primitive : GMMRABM_RAB_REL_IND.
** ------------------------------------------------------------------------*/
typedef struct gmmrabm_rab_rel_ind
{ 
  IMH_T         header;                /* Header for inter-task communication */

  sys_modem_as_id_e_type    as_id;

} gmmrabm_rab_rel_ind_T;

/* --------------------------------------------------------------------------
** Primitive : GMMRABM_RAB_EST_IND.
** ------------------------------------------------------------------------*/
typedef struct gmmrabm_rab_est_ind
{ 
  IMH_T         header;                /* Header for inter-task communication */

  sys_modem_as_id_e_type    as_id;

} gmmrabm_rab_est_ind_T;


/**************************************************************************
            messages from GMM to RABM , defined in table 9.5.1 in ts 24.007
**************************************************************************/

/* --------------------------------------------------------------------------
** Primitive : GMMRABM_REESTABLISH_RSP. Defined in rabmtask.h
** ------------------------------------------------------------------------*/

/* --------------------------------------------------------------------------
** Primitive : GMMRABM_REESTABLISH_REJ.
** ------------------------------------------------------------------------*/
typedef struct gmmrabm_reestablish_rej
{ 
  byte                  cause;

  sys_modem_as_id_e_type    as_id;

} gmmrabm_reestablish_rej_T;



/* --------------------------------------------------------------------------
** Primitive : GMMRABM_REESTABLISH_RSP. Defined in rabmtask.h
** ------------------------------------------------------------------------*/
typedef struct gmmrabm_reestablish_rsp
{ 
  sys_modem_as_id_e_type    as_id;
} gmmrabm_reestablish_rsp_T;




#endif

