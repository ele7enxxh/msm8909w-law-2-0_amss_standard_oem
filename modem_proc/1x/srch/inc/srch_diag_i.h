#ifndef SRCH_DIAG_I_H
#define SRCH_DIAG_I_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        S R C H _ D I A G - H E A D E R  F I L E

GENERAL DESCRIPTION
  This module contains functions pertaining to the search diag interface

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2003 - 2015 Qualcomm Technologies, Inc.
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/srch/inc/srch_diag_i.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/18/15   ab      Add Set CHAIN UNLOCK THRESH
04/21/15   srk     Unification of IDLE DIV algorithm for all CRAT modes.
01/09/15   srk     Feature cleanup.
08/15/14   cjb     Diag command to enable/disable new idle div algo for DR mode
06/19/14   as      Implement test hook to extend sleep timelines
06/12/14   sst     Add feature to distinguish between Int and Ext Builds
06/02/14   pap     Adding diag command to refresh NV items.
04/17/14   cjb     Clean-up related to using 1x MSG macros for printing F3s
10/28/13   as      Implemented dynamic timelines
08/16/13   vks     Mainline FEATURE_ZZ2_2
05/10/13   cjb     Fixed mempool capture functionality.
04/19/13   pap     Adding mempool capture capability for diag and fade timers.
04/11/13   cjb     Added DIAG support and ULPN/Long_Code advance/retard
                   capability for full_pn search only for bad meas case.
03/29/13   bph     Mainline RX_DIV_RT_CTRL, DYN_MEM, and RXD_IN_ACCESS
01/23/13   trc     Decouple TSYNC and its QMI interface
10/02/12   adw/dkb Add zz2_2 diag interface.
08/14/12   cjb     Added DIAG interface for SLPC verification in non-sig mode.
04/16/12   bb      Added command to support changing MDSP clock speed
03/07/12   sst     Added RIN enable to API test
02/16/12   srk     Replaced FEATURE_LTE_TO_1X by FEATURE_MODEM_1X_IRAT_LTO1X.
01/23/12   srk     Feature Cleanup
01/13/12   srk     Feature Cleanup: Mainlined Always ON features
12/20/11   srk     Feature clean up: Removed FEATURE_IS95B_MDR
11/04/11   mca     Added TSYNC support
10/31/11   jtm     Feature clean up.
10/13/11   cjb     Changes for IRAT:1x->LTE. Initial working version.
10/07/11   adw     Move types needed by cm api to external header.
08/12/11   adw     Support second chain test [Option-2] capability on NikeL.
05/20/11   mca     Add cmd to query Ec/Io and RSSI values
05/11/11   cjb     Added common API to override chain selection.
04/20/11   cjb     SU API split for "srch_irat.h".
04/15/11   pk      Added support for SRCH_RX_STATUS
03/16/11   mca     Added srch_diag_update_tx_rx_status().
03/16/11   sst     Add support for FTM mode LTE->1x Measurements
01/25/11   adw     Removed redundant definition of sup_walsh_codes data type.
01/14/11   mca     Removed the immed field from SRCH_RX_DIV_OVERRIDE
01/14/11   cjb     Added new api command for runtime RxDiv en/dis.
01/14/11   cjb     Featurized srchlte_tt diag command under FEATURE_LTE_TO_1X.
01/13/11   adw     Enhancements to srchlte_tt diag command.
01/12/11   adw     Added support for srchlte_tt diag command.
12/09/10   mca     Added support for SRCH_API_TEST
11/30/10   cjb     Second_Chain_Test [Option-2] implementation.
07/14/10   adw     Removed featurization around aset_walsh() prototype.
07/08/10   sst     Update to resolve pack issue with walsh diag command
05/19/10   sst     Added new diag command for RxDiv en/dis
01/20/10   mca     Updated PACKED macro
12/11/09   sst/adw Added CGPS diag commands for TT and PPM requests.
04/24/09   adw     Remove MSG_DBG_SRCH4 define already defined in custmcs.h.
04/01/09   adw     Categorized included header files and standardized format.
03/25/09   adw     Include modem_1x_defs.h for 1x specific modem defines.
03/04/09   adw     Added srchmc_v.h include.
11/12/08   adw     Added support to dynamically assign pwr strobe gpio.
08/13/08   vks     Added FEATURE_SRCH_HAS_NO_RTL for ULC
04/23/07   sst     Split up QLIC Support and QLIC Algorithm using defines
02/23/07   sst     Add QLIC support
01/18/07   rkc     Added CGPS debug support
01/17/07   sst     Added xo_trim diag req pkts
08/07/06   rkc     Added srch_diag_request_system_restart() and
                   SRCH_REQUEST_SYSTEM_RESTART.
06/06/06   pa      Added MSG_DBG_AFC
02/08/06   awj     Fixed featurization issue for MDR
02/02/06   awj     Redesigned srch_gen_pilot_set_struct_type.
                   Added pilot info imaging/reporting interface.  Moved
                   srch_aset_walsh(), srch_get_pilot_set() here.
01/16/06   awj     Lint cleanup
11/04/05   awj     Added MSG_DBG_SRCH_IRM
11/02/05   awj     Stripped FEATURE_SRCH_PCG, mainlined RTL and IRM
09/26/05   awj     Added FEATURE_SRCH_RTL
08/19/05   ejv     Update deprecated types.
07/14/05   sfm     Mainlined FEATURE_1X_SRCH_USES_ROTATOR_AFC
05/20/05   ejv     Remove srch_fing_driver.h and srch_rpt_ta.
05/18/05   kwo     Relocated srch_rpt_ta() from srch.c
09/16/04   jcm     Featurized AFC with FEATURE_1X_SRCH_USES_ROTATOR_AFC
09/13/04   sst     Added srch_afc overwrite capabilities
07/07/04   ejv     Add T_WINCE.
06/15/04   ejv     Added MSG_DBG_DV
05/28/04   jcm     Added MSG_DBG_SRCH_SECT
02/27/04   ejv     Added MSG_DBG_RX_DIV
02/05/04   sst     Added MSG_DBG_SRCH4
01/21/04   kwo     Added MSG_DBG_SCHED
12/16/03   rng     Added support for sci override
09/19/03   kwo     Changed message levels to match 6100
08/20/03   kwo     Reworked debug message levels
08/13/03   kwo     Added support for MSG_DBG
07/25/03   bt      Added power strobe.
07/22/03   ejv     Include diagpkt.h.
07/16/03   kwo     implementation , first cut
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/

/* Common */
#include "1x_variation.h"
#include "comdef.h"
#include "customer.h"
#include "modem_1x_defs.h"

/* Srch */
#include "srch_diag.h"
#include "srch.h"
#include "srchmc_v.h"
#include "srchmc_i.h"
#include "srch_mdsp_i.h"

/* Other */
#include "diagpkt.h"
#include "m1x_diag.h"


/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/


/* Define if enabling system restart testing */
/* #define SRCH_DIAG_SYSTEM_RESTART_TEST */

#ifdef SRCH_DIAG_SYSTEM_RESTART_TEST
  #error code not present
#else
  #define SRCH_DIAG_SYSTEM_RESTART_CHK(curr_timeline)
#endif /* SRCH_DIAG_SYSTEM_RESTART_TEST */

/*--------------------------------------------------------------------------
                    Subsystem Dispatch Request Response
--------------------------------------------------------------------------*/

/* list of search diag request indeces.  These can be activated via the DM
   command : send_data 75 29 COMMAND 00 DATA */
enum {
  SRCH_FEATURE                = 0x00,
  SRCH_PARM                   = 0x01,
  SRCH_GEN_PILOT_SET          = 0x02,
  SRCH_API_TEST               = 0x03,
  SRCH_RX_STATUS              = 0x04,
  SRCH_RX_STATUS_RAW          = 0x05,

  #ifdef FEATURE_MODEM_1X_TSYNC
  SRCH_TSYNC_CTRL             = 0x06,
  #endif /* FEATURE_MODEM_1X_TSYNC */

  #ifdef FEATURE_1X_ADAPTIVE_FW_CLK
  /* MDSP clock speed override request */
  SRCH_MDSP_CLK_SPEED_OVERRIDE = 0x7,
  #endif /* FEATURE_1X_ADAPTIVE_FW_CLK */

  SRCH_DIAG_MAX_EXTERNAL_ID,

  /* all internal commands are featurized out in External builds */
  SRCH_DIAG_START_INTERNAL_ID = 0xE0,

  SRCH_RX_DIV_OVERRIDE        = 0xE1,
  SRCH_RX_CHAIN_SELECT        = 0xE2,

  #ifdef FEATURE_MODEM_1X_IRAT_LTO1X
  SRCH_LTO1X_MEAS_TEST        = 0xE3,
  SRCH_LTE_1X_TT_DEBUG        = 0xE4,
  #endif /* FEATURE_MODEM_1X_IRAT_LTO1X */

  #ifdef FEATURE_1X_TO_LTE
  SRCH_1XTOL_MEAS_TEST        = 0xE5,
  #endif /* FEATURE_1X_TO_LTE */

  SRCH_IDLE_DIV_TEST          = 0xE6,

  /*dumps mempool data based on the chain requested by user*/
  SRCH_MEMPOOL_DUMP           = 0XE7,

  /* Adjusts the range for valid slack */
  SRCH_TL_SLACK_SET           = 0xE8,

  /* NV refresh diag command */
  SRCH_NV_READ                = 0xE9,

  SRCH_SET_CHAIN_UNLOCK_THRESH = 0xEA,

  SRCH_TIMELINE_TEST          = 0xEF,

  SRCH_SLEEP_TEST             = 0xF0,

  SRCH_PWR_STROBE             = 0xF1,
  SRCH_SNOOPER                = 0xF2,

  #ifdef FEATURE_SCI_OVERRIDE
  SRCH_SCI_OVERRIDE           = 0xF3,
  #endif /* FEATURE_SCI_OVERRIDE */

  SRCH_REQUEST_RTL            = 0xF4,

  #ifdef SRCH_DIAG_SYSTEM_RESTART_TEST
  #error code not present
#endif /* SRCH_DIAG_SYSTEM_RESTART_TEST */

  #ifdef FEATURE_SRCH_CGPS_IF
  /* Used to test sending internal CGPS interface command via srchcom_sm */
  SRCH_CGPS_API_TEST          = 0xF6,
  #endif /* FEATURE_SRCH_CGPS_IF */

  SRCH_QLIC_OVERRIDE          = 0xF7,

  #ifdef FEATURE_XO
  SRCH_AFC_READ_XO_TRIM       = 0xF8,
  SRCH_AFC_WRITE_XO_TRIM      = 0xF9,
  #endif /* FEATURE_XO */
  SRCH_AFC_READ_ACCS          = 0xFA,
  SRCH_AFC_WRITE_ACCS         = 0xFB,
  SRCH_AFC_WRITE_MODE_GAINS   = 0xFC,

  #ifdef FEATURE_SRCH_CGPS_IF
  SRCH_REQ_CGPS               = 0xFD,
  #endif /* FEATURE_SRCH_CGPS_IF */
  SRCH_SECOND_CHAIN_TEST      = 0xFE,

  SRCH_DIAG_MAX_ID
};


/*--------------------------------------------------------------------------
             GENERALIZED PILOT SET REQUEST REPORT STRUCTURE

This structure is used in the called interface when the caller desires to
get a read on how many and what pilots are in the Search sets.
--------------------------------------------------------------------------*/

/* The following packed structures are used to comply with diag packet
   specifications */

/* Collection of pilot counts. */
typedef PACK( struct )
{
  uint16 pilot_inc;             /* Pilot increment for remaining sets */
  uint8  active_cnt;            /* Count of pilots in active set */
  uint8  cand_cnt;              /* Count of pilots in candidate set */
  uint8  neigh_cnt;             /* Count of pilots in neighbor set */
} srch_diag_pilot_set_count_type;

/* Each pilot has a pn/strength pair.  An dynamically sized array of these
   is included in diag packets/UI update requests */
typedef PACK( struct )
{
  uint16 pn;
  uint16 strength;
} srch_diag_pilot_info_type;

/* Max sized array of pn/strength pairs */
typedef PACK( struct )
{
  /* Sets of pilots (in the above order), plus one Remaining pilot */
  srch_diag_pilot_info_type sets[SRCH_MAX_SETS];
} srch_diag_pilot_set_info_type;

/* Complete pilot set type; includes counts and pn/strength data */
typedef PACK( struct )
{
  srch_diag_pilot_set_count_type count;
  srch_diag_pilot_set_info_type  info;
} srch_diag_pilot_set_type;

/* The RF portion of certain diag requests.  This includes srch_state for
   compliance with the packet structure */
typedef PACK( struct )
{
  uint16 cdma_freq;             /* Current CDMA channel */
  uint16 band_class;            /* Current band class */
  uint8  rx_agc;                /* Current value of rx agc register */
  uint8  tx_adj;                /* Current value of tx gain adjust register */
  uint8  tx_agc;                /* Current value of tx agc register */

  uint16 ref_inx;               /* Latest reference sector index */
  uint16 srch_state;            /* Latest state Searcher is in */
} srch_diag_rf_info_type;

/* Each active should have an offsect and a number 0-7 of supplemental
   channels.  Walsh codes for each possible channel (fundamental + 7
   supplemental) are included */

typedef PACK( struct )
{
  /* Pilot offset used by search */
  uint16 pilot_off;

  /* Number of supplemental channels */
  uint8 num_sup_chans;
} srch_diag_walsh_data_type;

/* Latest Walsh code information; the number of actives and an array consisting
   of the walsh data for each */
typedef struct
{
  /* number of active set */
  uint16                    num_aset;

  srch_diag_walsh_data_type data[ SRCH_ASET_MAX ];

} srch_diag_walsh_info_type;

/* This type is only used to direct DIAG to look beyond a packet header.  It
   should not be used for any other purpose.  Explanation follows:

   This type is only used to support the DIAG architectural model for packet
   layout.  The size of the packet is dynamically determined, so sizeof() is
   insufficient.  The reason it is preserved is to support the packet model, which is

struct
{
  uint32 packet_header;
  type*  packet_data;
};

  The size of this type is never checked, and a variable of this type should
  never be instantiated.  It is simply a pointer which will be cast to void*
  in srch_diag and will point to a diag packet allocated elsewhere.
  </warning> */
typedef void* srch_gen_pilot_set_struct_type;

#ifdef SRCH_DIAG_SYSTEM_RESTART_TEST
#error code not present
#endif /* SRCH_DIAG_SYSTEM_RESTART_TEST */

/* Gen Pilot Sets diag request packet definition */

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_GEN_PILOT_SET)
DIAGPKT_REQ_END

/* The Gen Pilot Sets response packet definition */

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_GEN_PILOT_SET)
  srch_gen_pilot_set_struct_type pilot_set_response;
DIAGPKT_RSP_END


/*--------------------------------------------------------------------------
                             SEARCH PARMS

This interface provides a way for a test tool to query and clear search
parameters.
--------------------------------------------------------------------------*/

enum {
  SRCH_DIAG_PARM_INVALID = -1,  /* invalid param */
  /* the following parms are from stored values */
  SRCH_DIAG_PARM_ACQ_TOOK         = 0x00, /* how long acq took in ms */
  SRCH_DIAG_PARM_ACQ_FAIL_CNT     = 0x01, /* number of time acq has failed */

  SRCH_DIAG_PARM_MAX_STORED       = 0x02,

  /* the following parms are from queried values */
  SRCH_DIAG_PARM_FIRST_QUERIED    = 0x10,
                                          /* following querries are return
                                             value of api: */
  SRCH_DIAG_PARM_RAW_ECIO         = 0x10, /*  srch_get_cdma_raw_ecio           */
  SRCH_DIAG_PARM_RAW_ECIO_COMB    = 0x11, /*  srch_get_cdma_raw_ecio_all_chains*/
  SRCH_DIAG_PARM_RAW_ECIO_RX0     = 0x12, /*  srch_get_cdma_raw_ecio_all_chains*/
  SRCH_DIAG_PARM_RAW_ECIO_RX1     = 0x13, /*  srch_get_cdma_raw_ecio_all_chains*/
  SRCH_DIAG_PARM_RSSI             = 0x14, /*  srch_get_cdma_rssi               */
  SRCH_DIAG_PARM_RSSI_COMB        = 0x15, /*  srch_get_cdma_rssi_all_chains    */
  SRCH_DIAG_PARM_RSSI_RX0         = 0x16, /*  srch_get_cdma_rssi_all_chains    */
  SRCH_DIAG_PARM_RSSI_RX1         = 0x17, /*  srch_get_cdma_rssi_all_chains    */
  SRCH_DIAG_PARM_BEST_RSSI        = 0x18, /*  srch_get_best_rssi               */

  SRCH_DIAG_PARM_MAX_PARM
};

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_PARM)
  int16 parm_id;  /* the id of the requested parameter */
  int8  clear;    /* 0 = get without clearing, 1 = get and clear
                     applies only to stored values */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_PARM)
  int16 parm_id;  /* the id of the requested parameter */
  int16 value;    /* value of the parameter */
DIAGPKT_RSP_END



/*--------------------------------------------------------------------------
                             SEARCH API TEST

This interface provides a way for a test tool to test Searcher's external
API's. Functions under test can be found by s/SRCH_DIAG_API_TEST/SRCH/
--------------------------------------------------------------------------*/

enum {
  SRCH_DIAG_API_TEST_INVALID                      = -1,  /* invalid param */
  SRCH_DIAG_API_TEST_GET_CDMA_RAW_ECIO            =  0,
  SRCH_DIAG_API_TEST_GET_CDMA_RSSI                =  1,
  SRCH_DIAG_API_TEST_GET_BEST_RSSI                =  2,
  SRCH_DIAG_API_TEST_GET_CDMA_RAW_ECIO_ALL_CHAINS =  3,
  SRCH_DIAG_API_TEST_GET_CDMA_RSSI_ALL_CHAINS     =  4,
  SRCH_DIAG_API_TEST_RX_RIN_DIV_CTRL              =  5,
  SRCH_DIAG_API_TEST_MAX
};

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_API_TEST)
  int16 api_id;                     /* the id of the requested API */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_API_TEST)
  int16 api_id;                     /* the id of the requested API */
DIAGPKT_RSP_END


/*--------------------------------------------------------------------------
                             SEARCH RX STATUS

This interface provides information about AGC of various chains owned by
SRCH

--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_RX_STATUS)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_RX_STATUS)
  uint8 chain_owned;
  int16 rf_path_agc_0;
  int16 rf_path_agc_1;
  int8  rx_home_agc;
DIAGPKT_RSP_END

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_RX_STATUS_RAW)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_RX_STATUS_RAW)
  uint8 chain_owned;
  int16 rf_path_agc_0_raw;
  int16 rf_path_agc_1_raw;
  int8  rx_home_agc_raw;
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             RX DIV OVERRIDE

This interface provides a way for a test tool to enable and disable Rx Div
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_RX_DIV_OVERRIDE)
  uint8  en_ovrd;    /* 0 = allow div, 1 = disallow div */
  uint8  immed;      /* 0 = next call, 1 = now */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_RX_DIV_OVERRIDE)
DIAGPKT_RSP_END


/*--------------------------------------------------------------------------
                             RX CHAIN SELECT

This interface provides a way for a test tool to override chain selection
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_RX_CHAIN_SELECT)
  uint8  chain_select;    /* Bit 0 => RX0, Bit 1 => RX1, ... */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_RX_CHAIN_SELECT)
DIAGPKT_RSP_END


/*--------------------------------------------------------------------------
                             NERO STROBE

This interface provides a way for a test tool to enable and disable the
nero strobe
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_PWR_STROBE)
  uint8  enable;    /* 0 = disable, 1 = enable */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_PWR_STROBE)
DIAGPKT_RSP_END


#ifdef FEATURE_SCI_OVERRIDE
/*--------------------------------------------------------------------------
                             SCI OVERRIDE

This interface provides a way for a test tool to override the current SCI
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_SCI_OVERRIDE)
  uint8  enable;    /* 0 = disable, 1 = enable */
  int8   sci_override;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_SCI_OVERRIDE)
DIAGPKT_RSP_END

#endif /* FEATURE_SCI_OVERRIDE */

/*--------------------------------------------------------------------------
                             REQUEST_RTL

This interface provides a way for a test tool to transfer in and out of RTL
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_REQUEST_RTL)
  uint8  rtl_enable;    /* 0 = disable, 1 = enable */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_REQUEST_RTL)
DIAGPKT_RSP_END

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_REQ_CGPS)
  uint8  tt_or_ppm;      /* 0 = ppm, 1 = tt */
  uint8  cgps_enable;    /* 0 = disable, 1 = enable, 3 = send data */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_REQ_CGPS)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             AFC DEBUG

This interface provides functions to read/write trk_lo_adj (ft_freq) and the
AFC gains
--------------------------------------------------------------------------*/

#ifdef FEATURE_XO
DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_AFC_READ_XO_TRIM)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_AFC_READ_XO_TRIM)
    uint8  xo_trim;
DIAGPKT_RSP_END


DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_AFC_WRITE_XO_TRIM)
    uint8  xo_trim;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_AFC_WRITE_XO_TRIM)
DIAGPKT_RSP_END
#endif /* FEATURE_XO */


DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_AFC_READ_ACCS)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_AFC_READ_ACCS)
    int16  acc_vco;
    int16  acc_rot;
DIAGPKT_RSP_END


DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_AFC_WRITE_ACCS)
    int16  acc_vco;
    int16  acc_rot;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_AFC_WRITE_ACCS)
DIAGPKT_RSP_END


DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_AFC_WRITE_MODE_GAINS)
    uint16 gain_vco;
    uint16 gain_rot;
    uint8  config_mode;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_AFC_WRITE_MODE_GAINS)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             REQUEST_SYSTEM_RESTART

This interface provides a way for a test tool to restart current timeline
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_REQUEST_SYSTEM_RESTART)
    uint8 tl_point;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_REQUEST_SYSTEM_RESTART)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             SRCH CGPS API TEST

This interface provides a way for a test tool to test SRCH CGPS API
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_CGPS_API_TEST)
    uint8 internal_cmd;
    uint8 priority;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_CGPS_API_TEST)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                             QLIC OVERRIDE

This interface provides a way for a test tool to enable/disable QLIC
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_QLIC_OVERRIDE)
  uint8  override;    /* 1 = disable QLIC, 0 = enable QLIC */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_QLIC_OVERRIDE)
DIAGPKT_RSP_END


/*--------------------------------------------------------------------------
                            FTM SECOND CHAIN TEST

This interface provides a way for a test tool to test second chain [DRx]
Configuration / Test set-up:
  Primary chain   -> Tx only
  Secondary chain -> Rx only
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_SECOND_CHAIN_TEST)
  uint8  enable;    /* 1 = enable second chain, 0 = disable second chain */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_SECOND_CHAIN_TEST)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                  Dynamic Timeline Slack Adjustment


--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_TL_SLACK_SET)
  uint8  option;      /* 2 = enable/disable, 1 = max , 0 = min */
  uint8  value;       /* Value in microseconds */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_TL_SLACK_SET)
DIAGPKT_RSP_END

#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*--------------------------------------------------------------------------
                            LTE TO 1X TT DEBUG

This interface provides a way for a test tool to enable srchlte_tt debug
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_LTE_1X_TT_DEBUG)
  uint8  full_pn_enable;    /* 1 = enable full pn srch, 0 = disable */
  int16  recovery_pn;       /* PN to recover */
  int8   ulpn_adv_rolls;    /* Number of rolls to advance ulpn */
  uint8  bad_meas_full_pn;  /* 1 = enable full pn srch for bad meas cases */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_LTE_1X_TT_DEBUG)
DIAGPKT_RSP_END
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */


#ifdef FEATURE_MODEM_1X_IRAT_LTO1X
/*--------------------------------------------------------------------------
                            LTE TO 1X MEASUREMENT TEST

This interface provides a way for a test tool to enable measurement testing
--------------------------------------------------------------------------*/
enum {
  SRCH_DIAG_LTO1X_MEAS_TEST__STATE_INIT = 1,
  SRCH_DIAG_LTO1X_MEAS_TEST__STATE_EXEC = 2,
  SRCH_DIAG_LTO1X_MEAS_TEST__STATE_EXIT = 3
};
enum {
  SRCH_DIAG_LTO1X_MEAS_TEST__MODE_IDLE = 1,
  SRCH_DIAG_LTO1X_MEAS_TEST__MODE_CONN = 2
};

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_LTO1X_MEAS_TEST)
  uint8   state;             /* see enum above */
  uint8   mode;              /* see enum above */
  uint8   band;              /* CDMA band */
  uint16  channel;           /* CDMA channel */
  int16   rot_val;           /* initial rotator value in ppb */
  uint8   rf_device;         /* see rfcom_device_enum_type */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_LTO1X_MEAS_TEST)
DIAGPKT_RSP_END
#endif /* FEATURE_MODEM_1X_IRAT_LTO1X */


#ifdef FEATURE_1X_TO_LTE
/*--------------------------------------------------------------------------
                            1X TO LTE MEASUREMENT TEST

This interface provides a way for a test tool to enable measurement testing
--------------------------------------------------------------------------*/
DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_1XTOL_MEAS_TEST)
  uint8  enable;    /* 1 = send a fake ATIM request, 0 = Don't send fake req */
  uint8  skip_treselect;  /* Skip setting Treselect timer */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_1XTOL_MEAS_TEST)
DIAGPKT_RSP_END
#endif /* FEATURE_1X_TO_LTE */

/*--------------------------------------------------------------------------
                            TIMELINE TEST

This interface provides a way for a test tool to verify sleep timelines
--------------------------------------------------------------------------*/
extern boolean force_sleep_delay;
extern uint16  sleep_delay_ms;
extern uint8   fake_qpch_erasure;
extern uint8   fake_reacq_failure;

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_TIMELINE_TEST)
  uint8  option;
  uint8  param0;
  uint8  param1;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_TIMELINE_TEST)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                            SLEEP TEST

This interface provides a way for a test tool to verify sleep timelines
--------------------------------------------------------------------------*/
DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_SLEEP_TEST)
  uint8  enable;    /* 1 = enter sleep_tl, 0 = stop sleep */
  uint8  sleep_tl;  /* Sleep timeline to be tested */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_SLEEP_TEST)
DIAGPKT_RSP_END


#ifdef FEATURE_MODEM_1X_TSYNC
/*--------------------------------------------------------------------------
                             TSYNC CONTROL

This interface provides a way to control TSYNC behavior.
--------------------------------------------------------------------------*/

enum {
  SRCH_DIAG_TSYNC_CTRL_INVALID = -1,  /* invalid param */
  SRCH_DIAG_TSYNC_CTRL_DISABLE =  0,
  SRCH_DIAG_TSYNC_CTRL_ENABLE  =  1,

  SRCH_DIAG_TSYNC_CTRL_MAX
};

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_TSYNC_CTRL)
  int16 cmd;                         /* the requested command */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_TSYNC_CTRL)
  int16 cmd;                         /* the requested command */
DIAGPKT_RSP_END
#endif /* FEATURE_MODEM_1X_TSYNC */

#ifdef FEATURE_1X_ADAPTIVE_FW_CLK
/*--------------------------------------------------------------------------
                            MDSP Clock speed override

This interface provides a way for a test tool to run MDSP clock to HIGH speed
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_MDSP_CLK_SPEED_OVERRIDE)
  uint8  mdsp_speed_override;    /* 1 = high speed, 0 = default speed */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_MDSP_CLK_SPEED_OVERRIDE)
DIAGPKT_RSP_END
#endif /* FEATURE_1X_ADAPTIVE_FW_CLK */


/*--------------------------------------------------------------------------
                            IDLE DIVERSITY TEST

This interface provides a way for a test tool to issue idle div test cmds.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_IDLE_DIV_TEST)
  uint8  cond;   /* 0xFF = reinitialize idle div (full reset + param reload)

                    0xEy = reset idle div, where "y" = the initial desired
                           device on next wakeup:
                             0xE0 = Rx0
                             0xE1 = Rx1
                             0xE2 = Div

                    0xX  = spoof the setting of any desired dyn rid condition,
                           where "X" = the enum value of the condition which
                           one desires to be set.

                           For example, to spoof a PCH decode failure, set
                           "X" to SRCH_DYN_RID_PAGE_DECODE_FAIL.

                           See srch_dyn_rid_bad_cond_enum_type for complete
                           list of available conditions and their corresponding
                           values. */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_IDLE_DIV_TEST)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         MEMPOOL DATA DUMP SELECT

This interface provides a way for a test tool to obtain chain selection for
mempool capture.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_MEMPOOL_DUMP)
  uint8  enable; /* Enable mempool dump capture     */
  uint8  ant;    /* Bit 0 => RX0, Bit 1 => RX1, ... */
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_MEMPOOL_DUMP)
DIAGPKT_RSP_END

/*--------------------------------------------------------------------------
                         REFRESH NV DATA

Trigger NV data to be read from the efs files.
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_NV_READ)
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_NV_READ)
DIAGPKT_RSP_END


/*--------------------------------------------------------------------------
                         SET CHAIN UNLOCK THRESH

Sets IDLE chain unlock Params from DIAG
--------------------------------------------------------------------------*/

DIAGPKT_SUBSYS_REQ_DEFINE(SRCH1X, SRCH_SET_CHAIN_UNLOCK_THRESH)
  uint8  thresh;
DIAGPKT_REQ_END

DIAGPKT_SUBSYS_RSP_DEFINE(SRCH1X, SRCH_SET_CHAIN_UNLOCK_THRESH)
DIAGPKT_RSP_END

/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION SRCH_DIAG_INIT

DESCRIPTION    This function is called at startup to load Diag with the
      SRCH logging dispatch table.

DEPENDENCIES   srch_diag_tbl.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_diag_init (void);

/*===========================================================================

FUNCTION SRCH_DIAG_GET_PILOT_SET

DESCRIPTION    This function fills up a diagnostic pilot set status
               packet with current set populations and memberships.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_diag_get_pilot_set (srch_pilot_set_struct_type *pkt_ptr);

/*===========================================================================

FUNCTION SRCH_DIAG_SET_PARM

DESCRIPTION    This function sets a parameter from the search parm table.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_diag_set_parm
(
  int16   parm_id, /* paramater id */
  int16   value    /* value to set parm to */
);

/*===========================================================================

FUNCTION SRCH_DIAG_GET_PARM

DESCRIPTION    This function gets a parameter from the search parm table.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern int16 srch_diag_get_parm
(
  int16   parm_id /* paramater id */
);

#ifdef FEATURE_SCI_OVERRIDE
/*===========================================================================

FUNCTION SRCH_DIAG_GET_SCI_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the sci override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_diag_get_sci_override_enable( void );

/*===========================================================================

FUNCTION SRCH_DIAG_GET_SCI_OVERRIDE

DESCRIPTION    This function gets the sci override value.

DEPENDENCIES   None.

RETURN VALUE   INT8.

SIDE EFFECTS   None.

===========================================================================*/
extern int8 srch_diag_get_sci_override( void );
#endif /* FEATURE_SCI_OVERRIDE */

#ifdef SRCH_DIAG_SYSTEM_RESTART_TEST
#error code not present
#endif /* SRCH_DIAG_SYSTEM_RESTART_TEST */

#endif /* SRCH_DIAG_I_H */
