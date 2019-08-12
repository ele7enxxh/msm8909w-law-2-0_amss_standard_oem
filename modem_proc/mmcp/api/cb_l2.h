#ifndef CB_L2_H
#define CB_L2_H
/*===========================================================================

                    L2 --> CB INTERFACE HEADER FILE

DESCRIPTION
  This file contains types, function prototypes and data declarations for
  Cell Broadcast task

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All 
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/cb_l2.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/24/03   vdr     Initial version


===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "customer.h"



#include "comdef.h"
#include "sys.h"


/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
#define CB_BLOCK_LEN 23

typedef enum 
{
  DL_CB_BLOCK_IND
} dl_cb_message_id ;

typedef struct  
{
   IMH_T   message_header;                  
   byte    cb_block[CB_BLOCK_LEN];     
   dword   Frame_no;

   sys_modem_as_id_e_type as_id;

}
dl_cb_block_ind_T;




#endif /* #ifndef CB_L2_H */


