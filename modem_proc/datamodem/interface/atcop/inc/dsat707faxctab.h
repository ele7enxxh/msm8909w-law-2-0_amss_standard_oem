#ifndef DSAT707FAXCTAB_H
#define DSAT707FAXCTAB_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S                
                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor command tables that define
  FAX related commands specific to IS-707 mode of operation.
  
Copyright (c) 1995 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/inc/dsat707faxctab.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
		      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc      Added support for Dynamic ATCoP.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
04/11/07   ua      Modifications as per KDDI requirements. 
04/03/02   rsl     Initial version of file.

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707_ASYNC
#include "dsati.h"
#include "dsatctab.h"

/*=========================================================================*/
/* IS-707 Fax related AT command table */
/*=========================================================================*/

/*  Flag to indicate that an AT+FDT command has been   */
/*  and that a land to mobile FAX image is about to start */
extern boolean dsat707_fdt_seen;

/* Size of the function value. */
extern const unsigned int      dsat707_sizeof_fns_val; 

extern const dsati_cmd_type dsat707_fax_table[ ];
extern const dsati_cmd_ex_type dsat707_fax_table_ex[ ];
extern const unsigned int dsat707_fax_table_size;

/* Data declarations for fax commands */



/*===========================================================================

FUNCTION   

DESCRIPTION
 For JCDMA 
  { 0  , 0  , 1    },  // VR: R8*3.85l/mm, R8*7.7l/mm 
  { 1  , 0  , 5    },  // BR: 2400,4800,7200,9600,12000,14400bps 
  { 0  , 0  , 2    },  // WD: 1728,2048,2432d/mm 
  { 0  , 0  , 2    },  // LN: A4(297mm), B4(364mm), Unlimited 
  { 0  , 0  , 1    },  // DF: MH, MR 
  { 0  , 0  , 0    },  // EC: disable 
  { 0  , 0  , 0    },  // BF: disable 
  { 0  , 0  , 7    },  // ST: 0,5,10,20,40ms 

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void dsat707faxctab_update_fcc_dflm(void);


#endif /* FEATURE_DATA_IS707_ASYNC */
#endif /* DSAT707FAXCTAB_H   */

