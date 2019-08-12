#ifndef DSAT707SMSCTAB_H
#define DSAT707SMSCTAB_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S                
                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor command tables that define
  SMS related commands specific to IS-707 mode of operation.
  
Copyright (c) 2002 - 2008, 2010, 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707smsctab.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
		      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc      Added support for Dynamic ATCoP.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
05/10/10   kk      Added support for ^HC SMS commands. 
11/13/08   nc      Added Support for QCMGW,QCMSS,QCMGL,QCMGF Commands
06/01/07   sa      Mainlining SMS C2K changes
11/28/03   sb      created the module. CDMA SMS additions

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_CDMA_SMS
#include "dsati.h"

/*=========================================================================*/
/* IS-707 Sms related AT command table */
/*=========================================================================*/
extern const unsigned int       dsat707_sms_table_size;
extern const dsati_cmd_type     dsat707_sms_table [];
extern const dsati_cmd_ex_type     dsat707_sms_table_ex [];

/* Data declarations for sms commands */

extern const dsat_string_item_type  qcsmp_tid_valstr [][8];

extern const def_list_type      dsat707sms_qcpms_list[];

#endif /* FEATURE_CDMA_SMS */
#endif /* DSAT707SMSCTAB_H   */

