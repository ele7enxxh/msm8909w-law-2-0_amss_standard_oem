#ifndef DSAT707PSTATSCTAB_H
#define DSAT707PSTATSCTAB_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S                
                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor command tables that define
  Protocol Statistics related commands specific to IS-707 mode of
  operation.
  
Copyright (c) 1995,1996,1997 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1998 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1999, 2000, 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707pstatsctab.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
		      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc      Added support for Dynamic ATCoP.
01/19/12   sk      Feature cleanup.
04/03/02   rsl     Initial version of file.

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsatctab.h"

/*=========================================================================*/
/* IS-707 Protocol Statistics related AT command table */
/*=========================================================================*/

extern const dsati_cmd_type dsat707_pstats_table[ ];
extern const dsati_cmd_type dsat707_pstats_table_ex[ ];
extern const unsigned int   dsat707_pstats_table_size;

#endif /* FEATURE_DATA_IS707 */
#endif /* DSAT707PSTATSCTAB_H */
