/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the Protocol Statistics related AT modem commands for the
  IS-707 mode.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2002 - 2014 by Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707pstatsctab_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
			
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc     Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_DS_PSTATS
#include "dsati.h"
#include "dsat707pstats.h"
#include "dsat707pstatsctab.h"
#include "dsatparm.h"
#include "msg.h"


/*=========================================================================*/
/* IS-707 Protocol Statistics related AT command table */
/*=========================================================================*/
const dsati_cmd_type dsat707_pstats_table [] =
{
  { "$QCRLPD",  
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_QCRLPD,
      0,
      DSAT707_PSTATS_QCRLPD_IDX,
      NULL},
/*-------------------------------------------------------------------------*/  
  { "$QCRLPR",
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_QCRLPR,
      0,
      DSAT707_PSTATS_QCRLPR_IDX,
      NULL},
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DS_RLP3
  { "$QCRL3D",
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_QCRLPD33,
      0, 
      DSAT707_PSTATS_QCRL3D_IDX,
      NULL},
/*-------------------------------------------------------------------------*/
  { "$QCRL3R",
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_QCRLPR33,
      0,
      DSAT707_PSTATS_QCRL3R_IDX,
      NULL},
/*-------------------------------------------------------------------------*/
#endif /* FEATURE_DS_RLP3 */

  { "$QCPPPD",
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_QCPPPD,
      0, 
      DSAT707_PSTATS_QCPPPD_IDX,
      NULL},
/*-------------------------------------------------------------------------*/
  { "$QCPPPR", 
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_QCPPPR,
      0,
      DSAT707_PSTATS_QCPPPR_IDX,
      NULL},
/*-------------------------------------------------------------------------*/
  { "$QCIPD",
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_QCIPD,
      0,
      DSAT707_PSTATS_QCIPD_IDX,
      NULL},
/*-------------------------------------------------------------------------*/
  { "$QCIPR", 
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_QCIPR,
      0,
      DSAT707_PSTATS_QCIPR_IDX,
      NULL},
/*-------------------------------------------------------------------------*/
  { "$QCUDPD",
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_QCUDPD,
      0,
      DSAT707_PSTATS_QCUDPD_IDX,
      NULL},
/*-------------------------------------------------------------------------*/
  { "$QCUDPR",
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_QCUDPR,
      0,
      DSAT707_PSTATS_QCUDPR_IDX,
      NULL},
/*-------------------------------------------------------------------------*/

  { "$QCTCPD",
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_QCTCPD, 
      0,
      DSAT707_PSTATS_QCTCPD_IDX,
      NULL},
/*-------------------------------------------------------------------------*/
  { "$QCTCPR",
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_QCTCPR,
      0,
      DSAT707_PSTATS_QCTCPR_IDX,
      NULL}
/*-------------------------------------------------------------------------*/
};

/* Size of protocol statistics command table */
const unsigned int dsat707_pstats_table_size = ARR_SIZE
( dsat707_pstats_table );

#endif /* FEATURE_DS_PSTATS */
#endif /* FEATURE_DATA_IS707 */
