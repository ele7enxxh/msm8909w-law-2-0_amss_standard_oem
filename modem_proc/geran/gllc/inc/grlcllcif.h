/*****************************************************************************
***
*** TITLE   grlcllcif.h
***
***  GPRS RLC -> LLC INTERFACE
***
***
*** DESCRIPTION
***
***  Interface definitions for use by GPRS LLC and GPRS RLC modules in the
***  RLC to LLC direction via the GPRS GRR Service Access Point (SAP).
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/gllc/inc/grlcllcif.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/

#ifndef INC_GRLCLLCIF_H
#define INC_GRLCLLCIF_H


/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "geran_variation.h"
#ifndef CUSTOMER_H
  #include "customer.h"
#endif

#ifndef COMDEF_H
  #include "comdef.h"
#endif

#include "gllcrlcif.h"

#include "dsm.h"
#include "queue.h"
#include "geran_multi_sim.h"

#include "geran_multi_sim.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/* ---------------------------------------------------------------------------
   GRR Service Primitive labels for use in the uplink direction.
   ------------------------------------------------------------------------ */

typedef enum
{
  GRR_DATA_IND            = 0x80,
  GRR_UNITDATA_IND        = 0x81,
  GRR_STATUS_IND          = 0x82
  ,GRR_RESUME_CNF         = 0x84
} gprs_grr_dl_prim_t;


/* ---------------------------------------------------------------------------
   GRR_STATUS_IND Service Primitive structure def for use in the downlink.
   ------------------------------------------------------------------------ */

typedef enum
{
  GRR_RLCMAC_FAILURE      = 0x0    /* Failure of the RLC/MAC Layer    */
  ,
  GRR_PDU_TX_SUCCESS      = 0x1    /* PDU transmission success by L2  */
} gprs_grr_status_cause_t;


/* ---------------------------------------------------------------------------
   GPRS GRR downlink message type.
   ------------------------------------------------------------------------ */

typedef struct
{
  q_link_type             link;
  gprs_grr_dl_prim_t      prim;
  uint32                  tlli;     /* Data Ind & Unitdata Ind             */
  dsm_item_type           *ll_pdu;  /* Data Ind & Unitdata Ind             */
  gprs_grr_status_cause_t cause;          /* pdu tx status               */
  grr_data_req_cause_t    pdu_cause;      /* pdu_cause                    */
  uint8                   transaction_id; /* Transaction ID in DL NAS
                                             signalling message           */
  gas_id_t                gas_id;   /* GERAN access stratum id             */
} gprs_grr_dl_mes_t;


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


#endif /* INC_GRLCLLCIF_H */

/*** EOF: don't remove! ***/
