#ifndef DSAT707EXTCTAB_H
#define DSAT707EXTCTAB_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S                
                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor command tables that define
  extended commands specific to IS-707 mode.
  
Copyright (c) 1995 - 2008,2010,2013 - 2014 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/inc/dsat707extctab.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $		      
		     
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc      Added support for Dynamic ATCoP.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
01/12/10   vg      Added support for +CPIN,^CPIN and +CLCK for RUIM cards.
06/24/08   mga     Made additional changes to support CTA timer NV item
06/10/08   mga     Made changes to support CTA timer NV item
02/18/08   sa      Added modification for Passport Feature support.
04/11/07   ua      Modifications as per KDDI requirements. 
04/03/02   rsl     Initial version of file.

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsatctab.h"

/*=========================================================================*/
/* General IS-707 extended format AT command table */
/*=========================================================================*/

extern const dsati_cmd_type dsat707_ext_para_table[ ];
extern const dsati_cmd_ex_type dsat707_ext_para_table_ex[ ];
extern const unsigned int dsat707_ext_para_table_size;

extern const dsati_cmd_type dsat707_ext_action_table[ ];
extern const dsati_cmd_ex_type dsat707_ext_action_table_ex[ ];
extern const unsigned int dsat707_ext_action_table_size;

#ifdef FEATURE_DSAT_CDMA_PIN 
extern const dsat_string_item_type dsat707_clck_fac_tststr[];
extern const mixed_def_s_type * dsat707_cpin_mixed_dfl[2];
#endif /* FEATURE_DSAT_CDMA_PIN */

/* Data declarations for extended commands */
#ifdef FEATURE_DS_IS707A_CMUX
extern const word dsat707_cmux_fwd_table[];
extern const word dsat707_cmux_rev_table[];
#endif /* FEATURE_DS_IS707A_CMUX */

#define DS707_CTA_DEFAULT_VALUE 30
#define DS707_CTA_LOWER_LIMIT 0
#define DS707_CTA_UPPER_LIMIT 255
/*===========================================================================
FUNCTION   

DESCRIPTION :  Original structure looks like this.
 LOCAL const dflm_type dsat707_crm_dflm [] =
{
#ifdef FEATURE_DS_NET_MODEL
  if(jcdma mode)
  { 2  , 2  , 2    }
  else
  { 0  , 0  , 2    },
#else
  { 0  , 0  , 1    },
#endif
 } ;
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void dsat707extctab_update_crm_dflm(void);

#endif /* FEATURE_DATA_IS707  */
#endif /* DSAT707EXTCTAB_H    */
