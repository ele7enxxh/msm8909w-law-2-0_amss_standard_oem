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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707pstatsctab.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
			
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc      Added support for Dynamic ATCoP.
01/19/12   sk      Feature cleanup.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
12/12/08   ua      Fixed Off target lint errors.
4/3/01     rsl     Initial release.

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
const dsati_cmd_ex_type dsat707_pstats_table_ex [] =
{
  { DSAT707_PSTATS_QCRLPD_IDX,  dsat707_exec_qcrlpd_cmd },
/*-------------------------------------------------------------------------*/  
  { DSAT707_PSTATS_QCRLPR_IDX,  dsat707_exec_qcrlpr_cmd},
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DS_RLP3
  { DSAT707_PSTATS_QCRL3D_IDX,  dsat707_exec_qcrlpd33_cmd},
/*-------------------------------------------------------------------------*/
  { DSAT707_PSTATS_QCRL3R_IDX,  dsat707_exec_qcrlpr33_cmd},
/*-------------------------------------------------------------------------*/
#endif /* FEATURE_DS_RLP3 */

  { DSAT707_PSTATS_QCPPPD_IDX,  dsat707_exec_qcpppd_cmd},
/*-------------------------------------------------------------------------*/
  { DSAT707_PSTATS_QCPPPR_IDX,  dsat707_exec_qcpppr_cmd},
/*-------------------------------------------------------------------------*/
  { DSAT707_PSTATS_QCIPD_IDX,   dsat707_exec_qcipd_cmd},
/*-------------------------------------------------------------------------*/
  { DSAT707_PSTATS_QCIPR_IDX,   dsat707_exec_qcipr_cmd},
/*-------------------------------------------------------------------------*/
  { DSAT707_PSTATS_QCUDPD_IDX,  dsat707_exec_qcudpd_cmd},
/*-------------------------------------------------------------------------*/
  { DSAT707_PSTATS_QCUDPR_IDX,  dsat707_exec_qcudpr_cmd},
/*-------------------------------------------------------------------------*/

  { DSAT707_PSTATS_QCTCPD_IDX,  dsat707_exec_qctcpd_cmd},
/*-------------------------------------------------------------------------*/
  { DSAT707_PSTATS_QCTCPR_IDX,  dsat707_exec_qctcpr_cmd}
/*-------------------------------------------------------------------------*/
};

#endif /* FEATURE_DS_PSTATS */
#endif /* FEATURE_DATA_IS707 */
