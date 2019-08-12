#ifndef SM_SNDCP_H
#define SM_SNDCP_H

/*===========================================================================
                        S M - S N D C P Header File

DESCRIPTION

   Definition of Primitive message types between Session Management Entity
   and Mobile Network layer (SM <- SNDCP)

   Note : This file contains the Messages that are sent from SNDCP to SM only.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/sm/vcs/sm_sndcp.h_v   1.3   02 Jul 2002 17:40:32   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/sm_sndcp.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
02/13/12   us      Added new cause SNSM_MODIFY_FAILURE in snsm_cause_T 
10/08/02   ks      Added sapi in snsm_status_req_T primitive
07/01/02   ks      Added sm_tlli extern variable
01/26/00   ks      Created file.

===========================================================================*/

/*--------------------------------------------------------------------
* SM Cause type Enum definitions.
* -----------------------------------------------------------------*/
typedef enum
{
  SNSM_NORMAL_RELEASE,
  SNSM_NO_PEER_RESP,
  SNSM_DM_RECEIVED,
  SNSM_INVALID_XID_RESP,
  SNSM_MODIFY_FAILURE
}snsm_cause_T; 


/**********************************************************************
 *******           MESSAGES FROM SNDCP TO SM                    *******
 ********************************************************************** 
*/

/*--------------------------------------------------------------------
 * Primitive : SNSM_ACTIVATE_RES, PDP activation response. 
 * -----------------------------------------------------------------*/
typedef struct snsm_activate_rsp
{
  uint32      tlli;
  byte        nsapi;
  sys_modem_as_id_e_type    as_id;

} snsm_activate_rsp_T;

/*--------------------------------------------------------------------
 * Primitive : SNSM_MODIFY_RES, PDP modification response.
 * -----------------------------------------------------------------*/
typedef struct snsm_modify_rsp
{
  uint32      tlli;
  byte        nsapi;

  sys_modem_as_id_e_type    as_id;

} snsm_modify_rsp_T;

/*--------------------------------------------------------------------
 * Primitive : SNSM_DEACTIVATE_RES, PDP deactivation response.
 * -----------------------------------------------------------------*/
typedef struct snsm_deactivate_rsp
{
  uint32      tlli;
  byte        nsapi;

  sys_modem_as_id_e_type    as_id;

} snsm_deactivate_rsp_T;

/*--------------------------------------------------------------------
 * Primitive : SNSM_SEQUENCE_RES, 
               Used during an inter-SGSN routeing area update 
 * -----------------------------------------------------------------*/
typedef struct snsm_sequence_rsp
{
  uint32      tlli;
  byte        nsapi;
  uint16      rec_n_pdu;

  sys_modem_as_id_e_type    as_id;

} snsm_sequence_rsp_T;

/*--------------------------------------------------------------------
 * Primitive : SNSM_STATUS_REQ
 * -----------------------------------------------------------------*/
typedef struct snsm_status_req
{
  uint32          tlli;
  uint8           sapi;
  snsm_cause_T    cause;

  sys_modem_as_id_e_type    as_id;

} snsm_status_req_T;



#endif


