#ifndef TC_MM_H
#define TC_MM_H

/*===========================================================================
                        T C -  M M  Header File

DESCRIPTION

   Definition of Primitive message types between Test Control Entity
   and Mobility Management Entity (TC <-> MM)

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/ctest/vcs/tc_mm.h_v   1.3   11 Jul 2002 11:36:36   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/tc_mm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
07/11/02   ks      Renamed the SIG_CONNECTION_RELEASE to TCMM_SIG_CONNECTION_RELEASE.
01/30/02   mks     Removed MMTC_ESTABLISH_REQ, MMTC_ESTABLISH_CNF &
                   MMTC_ESTABLISH_REJ.
09/17/01   ks      Created file.
            
===========================================================================*/
#include "sys_stru.h"
#include "sys_cnst.h"
#include "sys_type.h"

/* ---------------------------------------------------------------------------
* Message Primitive Types between Test Control (TC) entity and the MM.
* --------------------------------------------------------------------------*/
                              
/* Primitives from TC to MM */

/* Note: This definition will be removed once MM defines in one of its headers */
#define MMTC_UNITDATA_REQ      0x01

/* --------------------------------------------------------------------------
** The following are the Error causes between MM and TC. 
** More cause values will be added as necessary.
** ------------------------------------------------------------------------*/
#define TCMM_SIG_CONNECTION_RELEASE  0x20

/* Commands from MM to TC */
/* These are defined in the TCTASK.H file */

/* --------------------------------------------------------------------------
** Data types used in the Primitives in MMTC-SAP
** ------------------------------------------------------------------------*/

/**************************************************************************
          Messages from TC to MM.
**************************************************************************/                                                                  

/* --------------------------------------------------------------------------
** Primitive : MMTC_UNITDATA_REQ, used by TC to send UL messages to SS 
** ------------------------------------------------------------------------*/
typedef struct mmtc_unitdata_req
{ 
  IMH_T             header;
  byte              data[MAX_OCTETS_L3_MESSAGE];
} mmtc_unitdata_req_T;


/**************************************************************************
            Messages from MM to TC.
**************************************************************************/

/* --------------------------------------------------------------------------
** Primitive : MMTC_ERROR_IND, used to indicate errors from MM to TC.
** ------------------------------------------------------------------------*/
typedef struct mmtc_error_ind 
{
  cc_cause_T      cause;    /* defined in sys_type.h as byte */
} mmtc_error_ind_T;

/* --------------------------------------------------------------------------
** Primitive : MMTC_UNITDATA_IND, used by MM to send DL SS messages to TC
** ------------------------------------------------------------------------*/
typedef struct mmtc_unitdata_ind
{ 
  byte          message_len;             /* Length of received TC-PDU */
  byte          data[MAX_OCTETS_L3_MESSAGE];
} mmtc_unitdata_ind_T;

/* --------------------------------------------------------------------------
** Primitive : MMTC_RAB_SETUP_IND, used by MM to notify TC about CS RAB setup
** ------------------------------------------------------------------------*/
typedef struct mmtc_rab_setup_ind
{ 
  byte      rab_id;    /* RAB-ID for the CS domain */
} mmtc_rab_setup_ind_T;

/* --------------------------------------------------------------------------
** Primitive : MMTC_RAB_RELEASE_IND, used by MM to notify CS RAB release to TC
** ------------------------------------------------------------------------*/
typedef struct mmtc_rab_rel_ind
{ 
  byte      rab_id;    /* RAB-ID for the CS domain */
} mmtc_rab_rel_ind_T;

#endif
