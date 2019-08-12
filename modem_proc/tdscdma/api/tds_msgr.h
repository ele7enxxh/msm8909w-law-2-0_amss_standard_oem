#ifndef TDS_MSGR_H


#define TDS_MSGR_H


/*===========================================================================


                      WCDMA TECH MODULE DEFINITIONS





DESCRIPTION





  This file contains the WCDMA Tech module to support the MSGR





Copyright (c) 2000-2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.


QUALCOMM Proprietary.  Export of this technology or software is regulated


by the U.S. Government. Diversion contrary to U.S. law prohibited.





===========================================================================*/








/*===========================================================================





                        EDIT HISTORY FOR MODULE





  This section contains comments describing changes made to the module.


  Notice that changes are listed in reverse chronological order.





$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/tdscdma/api/tds_msgr.h#1 $    $DateTime: 2016/12/13 07:58:19 $ $Author: mplcsds1 $





when       who     what, where, why


--------   ---     ----------------------------------------------------------


02/26/10   rm      Added new line at EOF


01/28/10   rm      Added WCDMA RRC tech module definition


===========================================================================*/








#include "msgr.h"





#define MSGR_MODULE_TDSRRC   0x31 /* TDS RRC starts from 0x30 */


#define MSGR_TDSCDMA_RRC     MSGR_TECH_MODULE( MSGR_TECH_TDSCDMA, MSGR_MODULE_TDSRRC )





#endif /* #ifndef WCDMA_MSGR_H */





