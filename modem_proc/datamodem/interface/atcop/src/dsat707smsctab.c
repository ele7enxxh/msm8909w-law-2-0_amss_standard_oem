/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the SMS related AT modem commands for the IS-707 mode.

  The AT commands here are not specified in 707 spec.  They are vendor
  specific commands for automating sms testing.
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2002 - 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707smsctab.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc      Added support for Dynamic ATCoP.
04/11/12    nc      Fixed operating mode checks for SMS related commands for NikeL targets. 
01/19/12   sk      Feature cleanup.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
07/23/10   nc      Added 1x SMS Support for FUSION Architecture.
05/10/10   kk      Added support for ^HC SMS commands. 
06/18/09   vg      Fixed Lint errors.
11/13/08   nc      Added Support for QCMGW,QCMSS,QCMGL,QCMGF Commands
06/01/07   sa      Mainlining SMS C2K changes
11/28/03   sb      created the module. CDMA SMS additions

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_CDMA_SMS
#include "dsati.h"
#include "dsat707sms.h"
#include "dsat707smsctab.h"
#include "dsatparm.h"
#include "msg.h"


/*=========================================================================*/
/* IS-707 Sms related AT command table */
/*=========================================================================*/
const dsati_cmd_ex_type dsat707_sms_table_ex [] =
{
  { DSAT707_SMS_QCNMI_IDX,    dsat707sms_exec_qcnmi_cmd },
  	
  { DSAT707_SMS_QCSMP_IDX,    dsat707sms_exec_qcsmp_cmd },
  
  { DSAT707_SMS_QCPMS_IDX,    dsat707sms_exec_qcpms_cmd },
  
  { DSAT707_SMS_QCMGR_IDX,    dsat707sms_exec_qcmgr_cmd },
  
  { DSAT707_SMS_QCMGS_IDX,    dsat707sms_exec_qcmgs_cmd },
  
  { DSAT707_SMS_QCMSS_IDX,    dsat707sms_exec_qcmss_cmd },
  
  { DSAT707_SMS_QCMGD_IDX,    dsat707sms_exec_qcmgd_cmd },

  { DSAT707_SMS_QCMGL_IDX,    dsat707sms_exec_qcmgl_cmd },
  
  { DSAT707_SMS_QCMGF_IDX,    dsatparm_exec_param_cmd  },
  
  { DSAT707_SMS_QCMGW_IDX,    dsat707sms_exec_qcmgw_cmd  },

#ifdef FEATURE_DSAT_EXTENDED_SMS
  { DSAT707_SMS_HCMGW_IDX,    dsat707sms_exec_qcmgw_cmd  },

  { DSAT707_SMS_HCMGL_IDX,    dsat707sms_exec_qcmgl_cmd  },

  { DSAT707_SMS_HCMGS_IDX,    dsat707sms_exec_qcmgs_cmd },

  { DSAT707_SMS_HCMGR_IDX,    dsat707sms_exec_qcmgr_cmd },

  { DSAT707_SMS_HSMSSS_IDX,   dsat707sms_exec_hsmsss_cmd  },
#endif /* FEATURE_DSAT_EXTENDED_SMS */
};

#endif /* FEATURE_DS_IS707 */
