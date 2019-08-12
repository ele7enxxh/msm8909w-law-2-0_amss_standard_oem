/*===========================================================================
          COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

          EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_sms_check_pending_sms.c_v   1.5   17 Jun 2002 16:01:22   rgurumoo  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_sms_check_pending_sms.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
7/17/01     CD     Added log message when mo retransmit counter changes value
                   Included msg.h

08/07/01   CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

05/23/02   RG      In the CNM_check_for_pending_message() routine, set 
                   CNM_mo_cp_data_retransmit_counter to CP_DATA_RETRANSMIT_COUNT 
                   instead of CP_DATA_RETRANSMIT_COUNT-1 as the CP-DATA for MO 
                   SM has to be retransmitted CP_DATA_RETRANSMIT_COUNT times.

02/14/03   AB      Fixed Lint error.

02/11/06   NR      Fixed lint error
===========================================================================*/

#include "customer.h"
#include "rex.h"
#include "err.h"

#include "environ.h"
#include "mn.h"
#include "cc_cc.h"
#include "timers_v.h"
#include "ms_cc_mm_v.h"
#include "ms_timer_v.h"

#include "cmlib.h"
#include "cnm.h"

#include "nasutils.h"
#include "cm_sms.h"
#include "msg.h"
#include "mnglobal.h"
#include <stringl/stringl.h>


/**************** Begin of   CNM_check_for_pending_message ************************
 *
 *  Function name: CNM_check_for_pending_message()
 *  ---------------------------------------------
 *
 *  Description:
 *  ------------
 *  This function checks the sms request queue. If there are any entries,
 *  then the top entry is extracted and sent to MM as a service request
 *
 *
 *  Parameters:
 *  -----------
 *  none
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_check_for_pending_message (  void )
{
   cp_data_msg_T           mmcnm_data_req;
   connection_id_T         connection_id;


   if ( CNM_read_sms_request_queue(  &mmcnm_data_req, &connection_id )
          != FALSE )
   {
      CNM_manage_cnm_service_requests( SHORT_MESSAGE_SERVICE_ESTABLISHMENT,
                                     connection_id,
                                     ( IMH_T *) &mmcnm_data_req.msg_head.IMH );

      /*
       * store message in case retransmission is required
       */

      (void)memscpy((void *) &CNM_mo_cp_message,
                   sizeof( mmcnm_data_req ),
                   (const void *) &mmcnm_data_req,
                   sizeof( mmcnm_data_req ));

      /*
       * Initialize CNM_mo_cp_data_retransmit_counter
       */

      CNM_mo_cp_data_retransmit_counter = CP_DATA_RETRANSMIT_COUNT;
      MSG_MED_DS_2(MN_SUB,"=MNCNM= SMC-CS- %d MO retx counter = %d",connection_id,CNM_mo_cp_data_retransmit_counter);
   }
}
/**************** End of   CNM_check_for_pending_message ************************/
