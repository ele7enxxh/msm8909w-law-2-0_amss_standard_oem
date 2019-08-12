#ifndef DSAT707MIPCTAB_H
#define DSAT707MIPCTAB_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

                M I P   C O M M A N D   T A B L E S                
                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor command tables that define
  Mobile IP commands specific to IS-707 mode of operation.
  

Copyright (c) 1995 - 2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707mipctab.h_v   1.5   07 Feb 2003 10:24:12   sramacha  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/inc/dsat707mipctab.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc      Added support for Dynamic ATCoP.
01/19/12   sk      Feature cleanup.
09/04/09   ss      CMI SU level modifications.
04/28/04   jd      Include dsmip.h for length constants, remove unused vars
09/18/02   jay     Removed AT test feature around QCMIPT
04/03/02   rsl     Initial version of file.

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsatctab.h"
#include "dsmip_v.h"

/*=========================================================================*/
/* Mobile IP related AT command table */
/*=========================================================================*/

#ifdef FEATURE_DS_MOBILE_IP
extern const dsati_cmd_type dsat707_mip_table[ ];
extern const dsati_cmd_ex_type dsat707_mip_table_ex[ ];
extern const unsigned int dsat707_mip_table_size;
#endif /* FEATURE_DS_MOBILE_IP */
#endif /* FEATURE_DATA_IS707     */  

#endif /* DSAT707MIPCTAB_H     */
