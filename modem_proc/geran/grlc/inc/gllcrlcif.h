/*****************************************************************************
***
*** TITLE     gllcrlcif.h
***
***  GPRS RLC -> LLC INTERFACE
***
***
*** DESCRIPTION
***
***  Interface definitions for use by GPRS LLC and GPRS RLC modules in the
***  LLC to RLC direction via the GPRS GRR Service Access Point (SAP).
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
*** $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/geran/grlc/inc/gllcrlcif.h#1 $
***
*** when       who     what, where, why
*** --------   ---     ------------------------------------------------------
*** 10-04-01   tjw     Add customer.h and comdef.h to all SU API files
*** mm/dd/yy   xyz     changes
***
*****************************************************************************/

#ifndef INC_GLLCRLCIF_H
#define INC_GLLCRLCIF_H

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


#include "dsm.h"
#include "gprsdef.h"
#include "gprsdef_g.h"
#include "grlci.h"
#include "gllsap.h"
#include "geran_multi_sim.h"


/*****************************************************************************
***
***     Public Type Definitions
***
*****************************************************************************/

/* ---------------------------------------------------------------------------
   GRR Service Primitive label definitions
   ------------------------------------------------------------------------ */

typedef enum
{
  GRR_DATA_REQ                  = 0x00,
  GRR_UNITDATA_REQ              = 0x01,
  GRR_SUSPEND_REQ               = 0x02,
  GRR_PURGE_SUSPENDED_DATA_REQ  = 0x03,
  GRR_RESUME_REQ                = 0x04,
  GRR_TEST_MODE_REQ             = 0x05,
  GRR_PURGE_DATA_REQ            = 0x06
} grr_ul_prim_t;


/* ---------------------------------------------------------------------------
   GRR_DATA_REQ Cause indicator label definitions
   ------------------------------------------------------------------------ */

typedef enum
{
  GRR_CAUSE_LAY3_DATA           = 0x0,
  GRR_CAUSE_GMM_SIGNALLING      = 0x1,
  GRR_CAUSE_LLC_SIGNALLING      = 0x2,
  GRR_CAUSE_GMM_PAGE_RESPONSE   = 0x3,
  GRR_CAUSE_GMM_CELL_UPDATE     = 0x4,
  GRR_CAUSE_GMM_CELL_NOTIFY     = 0x5,
  /* New cause in RLC , transparent to MAC as used for PDU marking.
  ** RLC when sending est_tbf_req translated the cause to GMM_SIGANNALING.
  */
  GRR_CAUSE_SM_SIGNALLING       = 0x6
} grr_data_req_cause_t;


/* ---------------------------------------------------------------------------
   GRR_DATA_REQ PDU Type indicator label definitions
   ------------------------------------------------------------------------ */

#define GRR_LL_PDU_ACKSACK      0 /* LL PDU bears ACK or SACK signalling */
#define GRR_LL_PDU_NON_ACKSACK  1 /* LL PDU doesn't bear ACK or SACK sig */
#define GRR_LL_PDU_U_FR_SRES    2 /* LL PDU bears solicited U-frame response */ 


typedef enum
{
  /* PDU grouping has not been done on this PDU
  */
  PGS_INIT,

  /* PDU grouping is on-going. last_psn holds the PDU in this last on-going group
  */
  PGS_OPEN,

  /* PDU grouping has stopped due to a different PDU group is detected
  */
  PGS_CLOSE

} pdu_group_states_t;


/* ---------------------------------------------------------------------------
   GPRS GRR uplink message type

   Field Applicability:-

    prim              - Data Req & Unitdata Req

    tlli              - Data Req & Unitdata Req
    pfi               - Data Req & Unitdata Req
    peak_throughput   - Data Req & Unitdata Req
    radio_priority    - Data Req & Unitdata Req
    llc_pdu_type      - Data Req only
    cause             - Data Req only
    *pdu_ptr          - Data Req & Unitdata Req

    len               - RLC internal use only
    rem_ind           - RLC internal use only
    rlc_mode          - RLC internal use only
    *dup_pdu_ptr      - RLC internal use only
   ------------------------------------------------------------------------ */

typedef struct
{
  q_link_type           link;
  grr_ul_prim_t         prim;

  uint32                tlli;             /* Temporary Logical Link Id     */
  uint8                 pfi;              /* Packet Flow Identifier        */

  test_mode_T           test_mode;        /* ETSI Test Mode */
  uint8                 ts_offset;        /* DL Timeslot offset */
  uint8                 srb_loopback_mode;  /* loop back blocks on the uplink using either
                                            ** GMSK or 8-PSK modulation
                                            */

  uint8                 peak_throughput;  /* QoS Peak Throughput Class     */
  uint8                 radio_priority;   /* QoS Radio Priority */
  uint8                 reliability_class;/* QoS Reliability Class */
  ll_sndcp_ref_t        sndcp_ref;        /* SNDCP nsapi, npdu num, seg num*/

  uint8                 llc_pdu_type;     /* LLC ACK/SACK sig indicator    */
  grr_data_req_cause_t  cause;            /* PDU origination cause ind     */

  geran_pdu_priority_t  pdu_prio;         /* PDU contains data or control info */

  uint8                 transaction_id;   /* Transaction id of a NAS Message*/
  dsm_item_type         *pdu_ptr;         /* Pointer to LL PDU             */

  /* LLC needs to propagate sapi info down to RLC
  */
  uint8                 sapi;
  /*----------------------------------------
  ** RLC private data. General PDU handling
  **----------------------------------------
  */

  uint16                orig_len;         /* Original octet length of PDU  */
  uint16                len;              /* Working octet length of PDU.  */
                                          /* After changing Coding Scheme  */
                                          /* or handling Contention Resolu.*/
                                          /* 'len' may be shortened        */
  uint16                rem_ind;          /* Index to remaining octets     */
  uint8                 rlc_mode;         /* Acknowledged/Unack mode ind   */
  dsm_item_type         *dup_pdu_ptr;     /* Pointer to the dup'ed payload */
  uint8                 restore_cnt;      /* no of times PDU was restored  */

  /*------------------------------------------
  ** RLC private data. PDU grouping operation
  **------------------------------------------
  */

  /* PDU sequence number assigned by RLC when a PDU is queued. This number comes
  ** from pg.psn which is cleared to 0 and wrapped back to 0 after 65535
  */
  uint16                psn;

  /* The current state of the PDU grouping operaion: INIT, OPEN and CLOSE
  */
  pdu_group_states_t    pg_state;

  /* The number of PDUs and octets tally up so far in this PDU group
  */
  uint16                pg_pdus;
  uint16                pg_octets;

  /* Same as pg_octets less the number of octets transmitted so far, ie.
  ** pg_octets_rem = pg_octets - orig_len + ul.octets_rem.
  */
  uint16                pg_octets_rem;

  /* PSN of the last PDU in this PDU group
  */
  uint16                pg_last_psn;

  /* Number of blocks required for pg_octets taking into account the value of pg_pdus
  */
  uint16                pg_num_blks;

  /* Number of blocks required to complete the current TBF at the end of this PDU
  */
  uint16                pg_num_blks_cur_pdu;

  /* This variable holds the pending reallocation action that shall take place when this
  ** pdu becomes the current pdu. This variable holds RA_NULL at initialisation,
  ** RA_IMMEDIATE for PRR type 1 or RA_XT_TX_COMPLETE for PRR type 2
  */
  realloc_action_t      realloc_action;

  /* Absolute PDU offset from original length. This value starts from 0 and gets
  ** re-assigned everytime the PDU is re-processed
  */
  uint16                pdu_ofs;

  gas_id_t              gas_id;   /* GERAN access stratum id             */

} gprs_grr_ul_mes_t;


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


#endif /* INC_GLLCRLCIF_H */

/*** EOF: don't remove! ***/
