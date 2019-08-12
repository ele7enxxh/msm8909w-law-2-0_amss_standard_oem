#ifndef _CFA_H_
#define _CFA_H_
/*===========================================================================
 
                    Callflow Analysis Logging Header File
 
DESCRIPTION
   This header file includes the appropriate enums for CFA logging.
   It can be shared by the TARGET (MSM) and the analyzer (QCAT).
 
Copyright (c) 2000, 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
 
                      EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/nas/vcs/cfa.h_v   1.3   10 May 2002 15:18:46   amcintos  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/api/cfa.h#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/02   ATM     Added GSM ANALYZE interface functions
05/10/02   ATM     Added cfa_log_ex function 
04/10/02   ATM     Added Front-End Filter to CFA
11/21/01   ATM     Removed enums - using defines from generic services (gs.h)
08/21/01   ATM     Initial Revision
===========================================================================*/

#include "comdef.h"   /* byte */
  
/*******************************************************************************
 *
 *  Function name: cfa_log_packet_ex
 *  ------------------------------
 *  Description:
 *  ------------
 *  Alternative Form -- for messages that don't have IMH_T, we will generate
 *                      one from the three params: MSG_SET, MSG_ID, LENGTH
 *
 *  Parameters:
 *  -----------
 *     queue     -- queueid of recipient (enum in gs.h)
 *     msg_set   -- message category     (enum in ms.h)
 *     msg_ptr   -- message id (specific to category)
 *     length    -- size of message pointed to by msg_ptr
 *     msg_ptr   -- pointer to the message payload
 *
 *  Returns:
 *  --------
 *     None.
 ******************************************************************************/

void cfa_log_packet_ex( byte  queue,
                        byte  msg_set,
                        byte  msg_id,
                        word  length,
                        byte *msg_ptr );


#endif /*ifndef _CFA_H_ */

