#ifndef DSAT707VENDCTAB_H
#define DSAT707VENDCTAB_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S
                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor command tables that define
  vendor specific commands specific to IS-707 mode of operation.

   Copyright (c) 1995-2002, 2010,2013-2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707vendctab.h_v   1.5   08 Jan 2003 12:28:00   sramacha  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/inc/dsat707vendctab.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/27/14    sc     Added support for Dynamic ATCoP.
03/13/12    dvk    Merged enabling mdr val update always during powerup.
01/11/11    ad     Remove extern usage.used get/set API for command associated  
                   value pointers.
05/10/10    kk     Added support for $$GPS* and $SP* commands.
10/15/02    ak     Added struct/vars for qcchs, which is for control/hold.
10/15/02    ak     Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
09/24/02    atp    Added support for QOS. (1x Release-A feature).
04/03/02    rsl    Initial version of file.

===========================================================================*/


/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsatctab.h"
#include "ds707.h"


/*===========================================================================
                           VARIABLE DECLARATIONS
===========================================================================*/
/*=========================================================================*/
/* IS-707 Vendor specific AT command table */
/*=========================================================================*/

extern const dsati_cmd_type dsat707_vend_para_table[ ];
extern const dsati_cmd_ex_type dsat707_vend_para_table_ex[ ];
extern const unsigned int dsat707_vend_para_table_size;

extern const dsati_cmd_type dsat707_vend_action_table[ ];
extern const dsati_cmd_ex_type dsat707_vend_action_table_ex[ ];
extern const unsigned int dsat707_vend_action_table_size;
/*---------------------------------------------------------------------------
   Declarations of constant default, lower bound and upper bound for 
   arguments of AT command with numeric arguments.                         
---------------------------------------------------------------------------*/

#if defined(FEATURE_DSAT_SP_CMDS)
/* Data declarations for MIP commands */
extern dflm_type dsat707_spmiperror_dflm [];
#endif /* defined(FEATURE_DSAT_SP_CMDS) */

#endif /* FEATURE_DATA_IS707 */
#endif /* DSAT707VENDCTAB_H  */
