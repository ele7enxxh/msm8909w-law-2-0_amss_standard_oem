#ifndef SRCH_DIAG_H
#define SRCH_DIAG_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

        S R C H _ D I A G - H E A D E R  F I L E

GENERAL DESCRIPTION
  This module contains functions pertaining to the search diag interface

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2003 - 2013 Qualcomm Technologies, Inc. 
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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/api/srch_diag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/14   pk      Modify srch_diag_update_tx_rx_status for SVLTE devices.
10/18/12   sst     Create funcs and vars to get current message level enabled
                    flags from diag.
                   Move message level defines to m1x_diag.h
12/20/11   srk     Feature clean up: Removed FEATURE_IS95B_MDR
10/21/11   srk     Copied definition of SRCH_MAX_SETS from srchmc_i.h.
10/20/11   ab      Moved "srch_sect_pilot_info_image_type" str declaration
                   from "1x/srch/main/latest/inc/srch_sect.h" to this file
10/18/11   ppr     Externalized srch_diag_get_pilot_image() fn
10/07/11   adw     Export types needed by cm api.
01/25/11   adw     Updated pack macro in diag_sup_walsh_codes data type.
11/15/10   jtm     Initial SU API split for CP.
11/09/10   jtm     Initial SU API split.
07/14/10   adw     Removed featurization around aset_walsh() prototype.
07/08/10   sst     Update to resolve pack issue with walsh diag command
05/19/10   sst     Added new diag command for RxDiv en/dis
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
#include "comdef.h"

/* Srch */
#include "srch.h"
#include "srchmc_v.h"

/* Other */
#include "diagpkt.h"
#include "msg.h"
#include "m1x_diag.h"
/* note the include of m1x_diag.h may not be included by other srch files,
   so this is inherited through this include */

/*===========================================================================

                  DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/* Maximum number of all sets, including RSET of 1 */
#define SRCH_MAX_SETS             ( SRCH_ASET_MAX + SRCH_CSET_MAX + \
                                    SRCH_NSET_MAX + 1 )

typedef PACK( struct )
{
  word num_aset;
    /* number of active set */

  PACK( struct )
  {
    int2 pilot_off;
      /* Pilot offset used by serach */

    byte num_sup_channels;
      /* Number of supplemental channels */

    word walsh_code[ SRCH_SUP_CHANNEL_MAX + 1 ];
      /* Specifies which walsh code is being used, including fundamental */

  } ch_data [ SRCH_ASET_MAX ];
  /* One for each Aset */
} srch_diag_sup_walsh_codes_data_type;

/* Macro for creating a bitmask by shifting '1' left by a
   Specified number indicated by 'val' */
#define SRCH_DIAG_BM( val ) ( 1<< ((int)(val)) )

/* Enumeration of RX chains supported */
typedef enum
{
  SRCH_DIAG_CHAIN_NUM_0 = 0,
  SRCH_DIAG_CHAIN_NUM_1,
  SRCH_DIAG_CHAIN_NUM_2,
  SRCH_DIAG_CHAIN_NUM_3,
  SRCH_DIAG_CHAIN_NUM_MAX
} srch_diag_chain_enum;

/* Enumeration of RX chain diversity info */
typedef enum
{
  SRCH_DIAG_CHAIN_UNAVAIL = 0,
    /* Signal info not available on all RX chains */
  SRCH_DIAG_CHAIN_0_AVAIL = SRCH_DIAG_BM( SRCH_DIAG_CHAIN_NUM_0 ),
    /* Signal info on SRCH_DIAG_CHAIN_0 is available */
  SRCH_DIAG_CHAIN_1_AVAIL = SRCH_DIAG_BM( SRCH_DIAG_CHAIN_NUM_1 ),
    /* Signal info on SRCH_DIAG_CHAIN_1 is available */
  SRCH_DIAG_CHAIN_2_AVAIL = SRCH_DIAG_BM( SRCH_DIAG_CHAIN_NUM_2 ),
    /* Signal info on SRCH_DIAG_CHAIN_2 is available */
  SRCH_DIAG_CHAIN_3_AVAIL = SRCH_DIAG_BM( SRCH_DIAG_CHAIN_NUM_3 ),
    /* Signal info on SRCH_DIAG_CHAIN_3 is available */
  SRCH_DIAG_CHAIN_ALL_AVAIL = SRCH_DIAG_BM( SRCH_DIAG_CHAIN_NUM_0 ) |
                              SRCH_DIAG_BM( SRCH_DIAG_CHAIN_NUM_1 ),
    /* Signal info on RX chains 0 & 1 are available */
  SRCH_DIAG_CHAIN_ALL_AVAIL_SV = SRCH_DIAG_BM( SRCH_DIAG_CHAIN_NUM_2 ) |
                                SRCH_DIAG_BM( SRCH_DIAG_CHAIN_NUM_3 )
  /* Signal info on RX chains 2 & 3 are available */
} srch_diag_diversity_bit_mask_enum;

/* Structure of RX signal info parameters per each RX chain */
typedef struct
{
  boolean             is_radio_tuned;
    /* Whether RX is tuned to a channel. If radio is tuned,
       instantenuous values will be set for below signal info
       fields.If radio is not tuned, delayed or invalid values
       will be set. */

  int32               rx_pwr;
    /* RX power value in 1/10 dbm resolution */

  int32               ecio;
    /* Ec/Io in negtive 1/10 dbm */
} srch_diag_chain_info_type;

/* Structure of RX power related info */
typedef struct
{
  srch_diag_diversity_bit_mask_enum       rx_diversity_indicator;
    /* Diversity indicator to show which RX chain has valid
       signal info. Used to indicate RX power available info
       of below 2 fields */

  srch_diag_chain_info_type
                           rx_pwr_chain[SRCH_DIAG_CHAIN_NUM_MAX];
    /* Rx signal info */
} srch_diag_rx_pwr_info_type;

/* Structure of TX power related info */
typedef struct
{
  boolean             is_in_traffic;
    /* Whether device is in traffic. tx_pwr is only meanful
       when in traffic. If no traffic tx_pwr is invalid */

  int32               tx_pwr;
    /* TX power value in 1/10 dbm */
} srch_diag_tx_pwr_info_type;

/* Structure to hold pilot set snapshot */
typedef struct
{
  uint16 pilot_inc;             /* Pilot increment for remaining sets */
  uint16 ref_index;             /* Reference index */
  uint8  active_cnt;            /* Count of pilots in active set */
  uint8  cand_cnt;              /* Count of pilots in candidate set */
  uint8  neigh_cnt;             /* Count of pilots in neighbor set */
  uint8  srch_state;            /* Searcher state */

  struct
  {
    uint16 pn;
    uint16 strength;
    srch_rx_band_type band;
    srch_rx_chan_type chan;
  } pilot_info[SRCH_MAX_SETS];
} srch_sect_pilot_info_image_type;

/*===========================================================================

                    FUNCTION DECLARATIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION       SRCH_DIAG_UPDATE_TX_RX_STATUS

DESCRIPTION    This function updates the status of various TX and RX
               parameters for CM.

DEPENDENCIES   None.

RETURN VALUE   INT8.

SIDE EFFECTS   None.

===========================================================================*/
extern void srch_diag_update_tx_rx_status
(
  srch_diag_rx_pwr_info_type* rx_info,  /* RX information */
  srch_diag_tx_pwr_info_type* tx_info   /* TX information */
);

/*=============================================================================

FUNCTION       SRCH_DIAG_GET_PILOT_IMAGE

DESCRIPTION    Takes a pilot snapshot for external use.

DEPENDENCIES   If sect is not loaded, this function will call
               sect_diag_legacy_get_pilot_image.

RETURN VALUE   None

SIDE EFFECTS   None

=============================================================================*/
extern void srch_diag_get_pilot_image
(
  srch_sect_pilot_info_image_type *pilot_set_info
);

/*===========================================================================

FUNCTION       SRCH_DIAG_MSG_UPDATE_MASK

DESCRIPTION    This function determines if specific message level are enabled

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   This is to be called rarely based on the MSG functions header

===========================================================================*/
extern void srch_diag_msg_update_mask( void );

/*===========================================================================

FUNCTION       SRCH_DIAG_MSG_GET_MASK_EN

DESCRIPTION    This function returns if the specified message level is enabled

DEPENDENCIES   None.

RETURN VALUE   boolean

SIDE EFFECTS   None.

===========================================================================*/
extern boolean srch_diag_msg_get_mask_en
(
  uint32  msg_lvl
);


#endif /* SRCH_DIAG_H */
