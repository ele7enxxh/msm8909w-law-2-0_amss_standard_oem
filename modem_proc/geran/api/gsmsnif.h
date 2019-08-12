/*****************************************************************************
***
*** TITLE  gsmsnif.h
***
***  GPRS SNDCP - SM INTERFACE HEADER FILE
***
***
*** DESCRIPTION
***
***  SNSM SAP inter-task message definitions for the SM to SNDCP direction.
***
***
*** Copyright (c) 2001-2013 Qualcomm Technologies, Inc.
*** All Rights Reserved. QUALCOMM Proprietary.
***
*** Export of this technology or software is regulated by the U.S.
*** Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/api/gsmsnif.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/

#ifndef INC_GSMSNIF_H
#define INC_GSMSNIF_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#ifndef CUSTOMER_H
  #include "customer.h"
#endif
#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "smtask.h"   /* Access to qos_T in sm_common.h via smtask.h */
#include "smtask_v.h"
#include "sys.h"
#include "sys_v.h"
#include "geran_dual_sim.h"

/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/


/* SM -> SNDCP service primitive message identifier definition. */

/* Packet data protocol type */
typedef enum
{
  GSNSM_PDP_IPV4    = 0x00,
  GSNSM_PDP_IPV6    = 0x01,
  GSNSM_PDP_INVALID = 0xff
}gsnsm_pdp_type;

typedef enum
{
  GSNSM_ACTIVATE_IND    = 0x00,                /* gsnsm_activ_ind_type   */
  GSNSM_DEACTIVATE_IND  = 0x01,                /* gsnsm_deactiv_ind_type */
  GSNSM_MODIFY_IND      = 0x02,                /* gsnsm_modify_ind_type  */
  GSNSM_SEQUENCE_IND    = 0x03                 /* gsnsm_seq_ind_type     */

} gsnsm_prim_type;

/* ===================================================================== */
/* SNSM-ACTIVATE-IND message type definition. PDP context activation
   request*/

typedef struct
{
  gsnsm_prim_type    prim;           /* GSNSM_ACTIVATE_IND               */

  uint32       tlli;           /* Temporary Logical Link Identifier*/
  uint8              nsapi;          /* NSAP Identifier Address          */
  uint8              sapi;           /* LL SAP Identifier Addr 3,5,9,11  */
  qos_T              qos;            /* Quality of Service QoS Profile   */
  uint8              radio_priority; /* Radio priority level             */
  sys_pdp_header_comp_e_type pcomp_alg; /* IP hdr compression alg type */
  sys_pdp_data_comp_e_type   dcomp_alg; /* Data compression alg type */
  gsnsm_pdp_type     pdp_type;          /* PDP type */
  uint8              pfi;            /* Packet Flow Identifier           */

  sys_modem_as_id_e_type    as_id;   /* Access stratum identity          */
} gsnsm_activ_ind_type;

/* ===================================================================== */
/* SNSM-DEACTIVATE-IND message type definition. PDP context deactivation
   request */

typedef struct
{
  gsnsm_prim_type    prim;           /* GSNSM_DEACTIVATE_IND             */

  uint32             tlli;           /* Temporary Logical Link Identifier*/
  uint8              num_nsapi;      /* Number of NSAPIs to deactivate   */
  uint8              nsapi[16];      /* NSAPI address list to deactivate */
  boolean            local;          /* TRUE if LLC is to locally release*/

//ifdef F EATURE_DUAL_SIM
  sys_modem_as_id_e_type    as_id;   /* Access stratum identity          */
//endif /* F EATURE_DUAL_SIM */

} gsnsm_deactiv_ind_type;

/* ===================================================================== */
/* SNSM-MODIFY-IND message type definition. PDP context modification
   request*/

typedef struct
{
  gsnsm_prim_type    prim;           /* GSNSM_MODIFY_IND                 */

  uint32             tlli;           /* Temporary Logical Link Identifier*/
  uint8              nsapi;          /* NSAP Identifier Address          */
  uint8              sapi;           /* LL SAP Identifier Addr 3,5,9,11  */
  qos_T              qos;            /* Quality of Service QoS Profile   */
  uint8              radio_priority; /* Radio priority level             */
  uint8              snd_npdu_num;   /* Send N-PDU number                */
  uint8              rcv_npdu_num;   /* Receive N-PDU number             */
  sys_pdp_header_comp_e_type pcomp_alg; /* IP header compression alg type */
  sys_pdp_data_comp_e_type   dcomp_alg; /* Data compression alg type */
  gsnsm_pdp_type             pdp_type;  /* PDP type */
  uint8              pfi;            /* Packet Flow Identifier           */

//ifdef F EATURE_DUAL_SIM
  sys_modem_as_id_e_type    as_id;   /* Access stratum identity          */
//endif /* F EATURE_DUAL_SIM */

} gsnsm_modify_ind_type;

/* ===================================================================== */
/* SNSM-SEQUENCE-IND message type definition. */

typedef struct
{
  gsnsm_prim_type    prim;           /* GSNSM_SEQUENCE_IND               */
  uint32             tlli;           /* Temporary Logical Link Identifier*/
  uint8              nsapi;          /* NSAP Identifier Address          */
  uint8              rcv_npdu_num;   /* Receive N-PDU number             */

//ifdef F EATURE_DUAL_SIM
  sys_modem_as_id_e_type    as_id;   /* Access stratum identity          */
//endif /* F EATURE_DUAL_SIM */

} gsnsm_seq_ind_type;


/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/


#endif /* INC_GSMSNIF_H */

/*** EOF: don't remove! ***/
