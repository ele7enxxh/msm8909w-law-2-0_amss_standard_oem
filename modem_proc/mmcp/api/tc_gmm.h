#ifndef TC_GMM_H
#define TC_GMM_H

/*===========================================================================
                        T C - G M M  Header File

DESCRIPTION

   Definition of Primitive message types between Test Control Entity
   and GPRS Mobility Management Entity (TC <-> GMM)

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/ctest/vcs/tc_gmm.h_v   1.3   11 Jul 2002 11:36:42   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/tc_gmm.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/11/02   ks      Renamed the SIG_CONNECTION_RELEASE to TCGMM_SIG_CONNECTION_RELEASE.
01/30/02   mks     Removed GMMTC_ESTABLISH_REQ, GMMTC_ESTABLISH_CNF &
                   GMMTC_ESTABLISH_REJ. 
09/17/01   ks      Created file.
            
===========================================================================*/
#include "sys_stru.h"
#include "sys_cnst.h"
#include "sys_type.h"

/* ---------------------------------------------------------------------------
* Message Primitive Types between Test Contol (TC) entity and the GMM.
* --------------------------------------------------------------------------*/
                              
/* Primitives from TC to GMM */

/* Note: This definition will be removed once GMM defines in one of its headers */
#define GMMTC_UNITDATA_REQ      0x11

/* --------------------------------------------------------------------------
** The following are the Error causes between GMM and TC. 
** ------------------------------------------------------------------------*/
#define TCGMM_SIG_CONNECTION_RELEASE  0x20

/* Commands from GMM to TC */
/* These are defined in the TCTASK.H file */

/* --------------------------------------------------------------------------
** Data types used in the Primitives in gmmtc-SAP
** ------------------------------------------------------------------------*/

/**************************************************************************
          Messages from TC to GMM.
**************************************************************************/ 
                                                                 
/* --------------------------------------------------------------------------
** Primitive : GMMTC_UNITDATA_REQ, used by TC to send UL messages to SS 
** ------------------------------------------------------------------------*/
typedef struct gmmtc_unitdata_req
{ 
  IMH_T             header;
  byte              data[MAX_OCTETS_L3_MESSAGE];
} gmmtc_unitdata_req_T;

/**************************************************************************
            Messages from GMM to TC , defined in table 9.5.1 in ts 24.007
**************************************************************************/

/* --------------------------------------------------------------------------
** Primitive : GMMTC_ERROR_IND, used to indicate errors from GMM to TC.
** ------------------------------------------------------------------------*/
typedef struct gmmtc_error_ind 
{
  cc_cause_T      cause;    /* defined in sys_type.h as byte */
} gmmtc_error_ind_T;

/* --------------------------------------------------------------------------
** Primitive : GMMTC_UNITDATA_IND, used by GMM to send DL SS messages to TC 
** ------------------------------------------------------------------------*/
typedef struct gmmtc_unitdata_ind
{ 
  byte          message_len;             /* Length of received TC-PDU */
  byte          data[MAX_OCTETS_L3_MESSAGE];
} gmmtc_unitdata_ind_T;

#endif
