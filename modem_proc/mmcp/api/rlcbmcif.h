#ifndef BMC_RLC_H
#define BMC_RLC_H
/*===========================================================================

                    L2 --> BMC INTERFACE HEADER FILE

DESCRIPTION
  This file contains types, function prototypes and data declarations for
  BMC task

Copyright (c) 2001,2002,2003 Qualcomm Technologies, Incorporated and its licensors.  All 
Rights Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/rlcbmcif.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/14/05   kvk     Removed cell id field to store the cell id.
03/30/05   kvk     New field cell id defined to store the cell id which the 
                   PDU was received
06/24/03   vdr     Initial version


===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "customer.h"

#include "comdef.h"

#include "wcdmamvsif.h"

#define CELL_ID_VALUE_LENGTH 4
#define BMC_BLOCK_LEN 55
typedef enum
{
 DL_BMC_BLOCK_IND
}dl_bmc_message_id;

typedef struct  
{
   IMH_T     message_header; 
   word      pdu_cnt;     /*Count to store the PDUs*/
}dl_bmc_block_ind_T;

typedef struct
{
  rlc_lc_id_type      rlc_lc_id;         /* RLC Downlink Logical Channel ID */
  q_type              rlc_rx_q;          /* RLC receive Queue               */
  dsm_watermark_type  rlc_rx_wm_item;    /* RLC receive watermark           */
  boolean             rlc_wm_reg_flag;   /* Boolean flag to indicated BMC registered
                                            with WM or not*/
  boolean             sent_close_to_rlc;
}
bmc_rlc_state_type;

void cb_rlc_ctch_pdu_dropped(dword pdu_sfn);



#endif/*BMC_RLC_H*/



