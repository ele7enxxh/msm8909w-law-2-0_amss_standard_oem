#ifndef DS707_DIAG_H
#define DS707_DIAG_H
/*===========================================================================

                        D S 7 0 7 _ D I A G 
GENERAL DESCRIPTION
  This file contains the functionality for the IS-707 interface to QXDM/DIAG.
  This implements the commands a user can issue from QXDM to the mobile, 
  which are specific to 1X data.

EXTERNALIZED FUNCTIONS
 ds707_diag_init()
   Registers the functions in this file with DIAG.
   
INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

 Copyright (c) 2003 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_diag.h#1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/11   ttv     Added changes to compute rlp and 707 statistics.
04/22/04   ak      Lint changes - made stats uint64.
05/01/03   ak      First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "diagcmd.h"
#include "diagpkt.h"
#include "dsrlp_v.h"



/*===========================================================================
                                DIAG_SUB_CMDS
===========================================================================*/

/*---------------------------------------------------------------------------
  The sub-system id's for the different commands we handle.  There is a 
  higher-level command defined in DIAG for DIAG_SUBSYS_DATA1X
---------------------------------------------------------------------------*/

#define DS707_DIAG_RLP_STATS        0
#define DS707_DIAG_RLP_STATS_EXT    1
#define DS707_DIAG_707_STATS        2

/*===========================================================================
                                DIAG_SUB_PKT_IDS
===========================================================================*/

/*---------------------------------------------------------------------------
  Sub packet IDs for DS707_DIAG_RLP_STATS_EXT
---------------------------------------------------------------------------*/

#define DS707_DIAG_RLP_STATS_EXT_SUB_PKT_RLP_STATS 1

/*---------------------------------------------------------------------------
  Sub packet IDs for DS707_DIAG_707_STATS
---------------------------------------------------------------------------*/
#define DS707_DIAG_707_STATS_SUB_PKT_CTA_VAL       1

/*===========================================================================
                         DIAG RLP STATS STRUCTS
===========================================================================*/

/*---------------------------------------------------------------------------
  Definition of query packet from DIAG.  Note that "DATA1X" refers to 
  DIAG_SUBSYS_DATA1X in diagcmd.h.  The macro prepends the "DIAG_SUBSYS"
  part.
---------------------------------------------------------------------------*/
DIAGPKT_SUBSYS_REQ_DEFINE(DATA1X, DS707_DIAG_RLP_STATS)
DIAGPKT_REQ_END

/*---------------------------------------------------------------------------
  Definition of response packet to DIAG.
---------------------------------------------------------------------------*/
DIAGPKT_SUBSYS_RSP_DEFINE(DATA1X, DS707_DIAG_RLP_STATS)
  uint16    last_pkt_so;          /* last packet so that connected         */
  uint32    rlp_rx_bytes;         /* RLP I/II rx'ed bytes                  */
  uint32    rlp_tx_bytes;         /* RLP I/II tx'ed bytes                  */
  uint64    rlp3_rx_bytes;        /* RLP III rx'ed bytes  (1x)             */
  uint64    rlp3_tx_bytes;        /* RLP III tx'ed bytes  (1x)             */
  uint64    rlp_hdr_SN_rx_bytes;  /* HDR RLP (SN) rx'ed bytes              */
  uint64    rlp_hdr_SN_tx_bytes;  /* HDR RLP (SN) tx'ed bytes)             */
  uint64    rlp_hdr_AN_rx_bytes;  /* HDR RLP (AN) rx'ed bytes              */
  uint64    rlp_hdr_AN_tx_bytes;  /* HDR RLP (AN) tx'ed bytes)             */
DIAGPKT_RSP_END

/*---------------------------------------------------------------------------
  Definition of query packet from DIAG.  Note that "DATA1X" refers to 
  DIAG_SUBSYS_DATA1X in diagcmd.h.  The macro prepends the "DIAG_SUBSYS"
  part. This is extension of DS707_DIAG_RLP_STATS.
---------------------------------------------------------------------------*/
DIAGPKT_SUBSYS_REQ_DEFINE(DATA1X, DS707_DIAG_RLP_STATS_EXT)
DIAGPKT_REQ_END

/*---------------------------------------------------------------------------
  Definition of response packet to DIAG.
---------------------------------------------------------------------------*/
typedef PACK (struct)
{
  uint16 last_pkt_so;               /* last packet so that connected         */
  uint32 rlp_rx_bytes;              /* RLP I/II rx'ed bytes                  */
  uint32 rlp_tx_bytes;              /* RLP I/II tx'ed bytes                  */
  uint64 rlp_hdr_SN_rx_bytes;       /* HDR RLP (SN) rx'ed bytes              */
  uint64 rlp_hdr_SN_tx_bytes;       /* HDR RLP (SN) tx'ed bytes)             */
  uint64 rlp_hdr_AN_rx_bytes;       /* HDR RLP (AN) rx'ed bytes              */
  uint64 rlp_hdr_AN_tx_bytes;       /* HDR RLP (AN) tx'ed bytes)             */

  uint32 rlp3_at_reset_request_count;
  uint32 rlp3_an_reset_ack_count;
  uint32 rlp3_an_reset_request_count;  
  uint64 rlp3_rx_dup_fr_cnt;        /* # of received duplicate frames        */
  uint32 rlp3_rx_rexmit_fr_cnt;
  uint32 rlp3_rx_new_data_fr_cnt;        
  uint32 rlp3_rx_bytes;             /* RLP III rx'ed bytes  (1x)             */
  uint32 rlp3_tx_bytes;             /* RLP III tx'ed bytes  (1x)             */
  uint32 rlp3_tx_rexmit_fr_cnt;     /*# transmitted re-transmitted frames    */
  uint32 rlp3_tx_new_data_fr_cnt;
  uint32 rlp3_rx_idle_fr_cnt;       /* # of received idle frames             */
  uint32 rlp3_rx_fill_fr_cnt;       /* # of received fill frames             */
  uint32 rlp3_rx_blank_fr_cnt;      /* # of blank frames received            */
  uint32 rlp3_rx_null_fr_cnt;       /* # of null frames received             */
  uint32 rlp3_rx_20ms_fr_cnt;       /* # of received Fundamental data        */
  uint32 rlp3_tx_idle_fr_cnt;       /* # of transmitted idle frames          */
  uint32 rlp3_tx_20ms_fr_cnt;       /* # of txed Fundamental data frames     */
  uint32 rlp3_naks_received ;       /* total # of nak frames received        */
  uint32 rlp3_rx_rlp_erasures;      /* # of rlp erasures received            */
  uint32 rlp3_rx_mux_erasures;      /* # of mux erasures received            */
  uint8  rlp3_resets;               /* # of resets                           */
  uint16 rlp3_state;                /* current state of the rlp              */
} ds707_diag_rlp_stats_ext_params_type;


typedef PACK (struct)
{
  /* Subpacket ID */
  byte              sub_pkt_id;
  /* Subpacket version */
  byte              sub_pkt_ver;
  /* subpacket size */
  uint16            sub_pkt_size;
  /* Parameters */
  ds707_diag_rlp_stats_ext_params_type  rlp_params;
} ds707_diag_sub_pkt_rlp_stats_ext_type;

typedef PACK (struct)
{
  /* Header: CMD_Code, Subsystem ID and subsystem cmd code */
  diagpkt_subsys_header_type  hdr;
  /* Genlog packet version */
  byte                        pkt_ver;
  /* Number of sub packets */
  byte                        num_sub_pkts;
  /* 2 bytes reserved */
  short                       reserved;
  /* Subpacket structure*/
  ds707_diag_sub_pkt_rlp_stats_ext_type sub_pkt_rlp_stats;
} ds707_diag_rlp_stats_ext_rsp_type;


/*===========================================================================
                               DIAG 707 STATS
===========================================================================*/

/*---------------------------------------------------------------------------
  Definition of query packet from DIAG.  Note that "DATA1X" refers to 
  DIAG_SUBSYS_DATA1X in diagcmd.h.  The macro prepends the "DIAG_SUBSYS"
  part.
---------------------------------------------------------------------------*/
DIAGPKT_SUBSYS_REQ_DEFINE(DATA1X, DS707_DIAG_707_STATS)
DIAGPKT_REQ_END

/*---------------------------------------------------------------------------
  Definition of response packet to DIAG.
---------------------------------------------------------------------------*/
typedef PACK (struct)
{
  /* Subpacket ID */
  byte              sub_pkt_id;
  /* Subpacket version */
  byte              sub_pkt_ver;
  /* subpacket size */
  uint16            sub_pkt_size;
  /* Parameters */
  dword             ds707_cta_val;
} ds707_diag_sub_pkt_707_stats_type;

typedef PACK (struct)
{
  /* Header: CMD_Code, Subsystem ID and subsystem cmd code */
  diagpkt_subsys_header_type  hdr;
  /* Genlog packet version */
  byte                        pkt_ver;
  /* Number of sub packets */
  byte                        num_sub_pkts;
  /* 2 bytes reserved */
  short                       reserved;
  /* Subpacket structure*/
  ds707_diag_sub_pkt_707_stats_type sub_pkt_707_stats;
} ds707_diag_707_stats_rsp_type;

/*===========================================================================
                          EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_DIAG_INIT

DESCRIPTION   Registers functions with DIAG.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ds707_diag_init(void);

#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_DIAG_H */

