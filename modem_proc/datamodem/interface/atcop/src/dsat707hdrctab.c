/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the HDR related AT modem commands for the IS-707 mode.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 1995-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707hdrctab.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
			
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc      Added support for Dynamic ATCoP.
01/19/12   sk      Feature cleanup.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
04/15/10   spn     Added support for HDR REVB Enhanced rates
10/30/06   spn     Changed Featurization from DS_AT_TEST_ONLY to 
                   HDR_AT_TEST_ONLY
09/29/05   spn     Changed the Default value of dsat707_qchdrr_list to 12
08/31/05   spn     Added DRC rates for DO-REV-A
4/3/01     rsl     Initial release.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsat707hdr.h"
#include "dsat707hdrctab.h"
#include "dsatparm.h"
#include "msg.h"

#ifdef FEATURE_HDR

/*=========================================================================*/
/* IS-707 HDR related AT command table */
/*=========================================================================*/

/*lint -save -e785 */
#ifdef FEATURE_HDR_AT_TEST_ONLY
const dsati_cmd_ex_type dsat707_hdr_table_ex [] =
{

/*-------------------------------------------------------------------------*/
  { DSAT707_HDR_QCHDRT_IDX,    dsat707_exec_qchdrt_cmd },
/*-------------------------------------------------------------------------*/  
  { DSAT707_HDR_QCHDRC_IDX,    dsat707_exec_qchdrc_cmd },
/*-------------------------------------------------------------------------*/  
  { DSAT707_HDR_QCHDRR_IDX,    dsat707_exec_qchdrr_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_HDR_QCHDRET_IDX,   dsat707_exec_qchdret_cmd }

};

#endif /* FEATURE_HDR_AT_TEST_ONLY */
/*lint -restore */


#endif /* FEATURE_HDR */

#endif /* FEATURE_DATA_IS707 */
