#ifndef DSMIO_STATS_H
#define DSMIO_STATS_H
/*===========================================================================

                        D S M I P _ S T A T S

GENERAL DESCRIPTION
  This file contains the functionality for the MIP interface to QXDM/DIAG.
  This implements the commands a user can issue from QXDM to the mobile, 
  which are specific to 1X data.

EXTERNALIZED FUNCTIONS
 dsmip_stats_init()
   Registers the functions in this file with DIAG.
   
INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

 Copyright (c) 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/src/dsmip_stats.h#1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/11   ttv     Fixed linker error.
04/19/11   ttv     First version of MIP statistics file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "diagcmd.h"
#include "diagpkt.h"


/*===========================================================================
                                DIAG_SUB_CMDS
===========================================================================*/

/*---------------------------------------------------------------------------
  The sub-system id's for the different commands we handle.  There is a 
  higher-level command defined in DIAG for DIAG_SUBSYS_DATA1X.
  1 to 30 are used for 707 statistics.
---------------------------------------------------------------------------*/

#define DSMIP_DIAG_MA_INFO        31
#define DSMIP_DIAG_MIP_STATS      32

/*---------------------------------------------------------------------------
  Sub packet IDs for DSMIP_DIAG_MA_INFO
---------------------------------------------------------------------------*/

#define DSMIP_DIAG_MA_INFO_SUB_PKT_MA_INFO       1

/*---------------------------------------------------------------------------
  Sub packet IDs for DSMIP_DIAG_MIP_STATS
---------------------------------------------------------------------------*/
#define DSMIP_DIAG_MIP_STATS_SUB_PKT_MIP_STATS   1

/*===========================================================================
                         DIAG MA INFO STRUCTS
===========================================================================*/

/*---------------------------------------------------------------------------
  Definition of query packet from DIAG.  Note that "DATA1X" refers to 
  DIAG_SUBSYS_DATA1X in diagcmd.h.  The macro prepends the "DIAG_SUBSYS"
  part.
---------------------------------------------------------------------------*/
DIAGPKT_SUBSYS_REQ_DEFINE(DATA1X, DSMIP_DIAG_MA_INFO)
DIAGPKT_REQ_END

typedef PACK (struct)
{
  uint32   ma_addr;             /* IP address of mobility agent            */
  uint16   ma_lifetime;         /* registration lifetime granted by MA     */
  uint16   last_seq_num;        /* sequence number of last agent adv.      */
  byte     mn_AAA_SPI[4];       /* Security Parameter Index for AAA auth   */
  boolean  ma_supp_rev_tun;     /* MA supports reverse tunneling           */
  byte     ma_prefix_len;       /* subnet prefix len of ma_addr            */
}dsmip_diag_ma_info_params_type;

typedef PACK (struct)
{
  /* Subpacket ID */
  byte              sub_pkt_id;
  /* Subpacket version */
  byte              sub_pkt_ver;
  /* subpacket size */
  uint16            sub_pkt_size;
  /* Parameters */
  dsmip_diag_ma_info_params_type  ma_info_params;
}dsmip_diag_sub_pkt_ma_info_type;

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
  dsmip_diag_sub_pkt_ma_info_type sub_pkt_ma_info;
}dsmip_diag_ma_info_rsp_type;


/*===========================================================================
                               DIAG MIP STATS STRUCTS
===========================================================================*/

/*---------------------------------------------------------------------------
  Definition of query packet from DIAG.  Note that "DATA1X" refers to 
  DIAG_SUBSYS_DATA1X in diagcmd.h.  The macro prepends the "DIAG_SUBSYS"
  part.
---------------------------------------------------------------------------*/
DIAGPKT_SUBSYS_REQ_DEFINE(DATA1X, DSMIP_DIAG_MIP_STATS)
DIAGPKT_REQ_END

/*---------------------------------------------------------------------------
  Definition of response packet to DIAG.
---------------------------------------------------------------------------*/
  typedef PACK (struct)
  {
    uint32 rrp_cnt;
    uint32 rrq_cnt;
    uint32 sol_cnt;
    uint32 adv_cnt;
    uint32 mip_to_sip_cnt;
    byte   mip_rrp_code;            /* mobility agent response code         */
  }dsmip_diag_mip_stats_params_type;
  
  
  typedef PACK (struct)
  {
    /* Subpacket ID */
    byte              sub_pkt_id;
    /* Subpacket version */
    byte              sub_pkt_ver;
    /* subpacket size */
    uint16            sub_pkt_size;
    /* Parameters */
    dsmip_diag_mip_stats_params_type  mip_stats_params;
  }dsmip_diag_sub_pkt_mip_stats_type;
  
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
    dsmip_diag_sub_pkt_mip_stats_type sub_pkt_mip_stats;
  }dsmip_diag_mip_stats_rsp_type;


/*===========================================================================
                          EXTERNAL FUNCTION DECLARATIONS
===========================================================================*/

#endif /* FEATURE_DS_MOBILE_IP */

#endif /* DSMIO_STATS_H */

