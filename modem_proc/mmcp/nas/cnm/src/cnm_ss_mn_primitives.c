/*===========================================================================
           COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

           EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_ss_mn_primitives.c_v   1.3   16 Oct 2001 10:34:06   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_ss_mn_primitives.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/06/01   CD      Replaced mn header files by single include of mn.h

08/14/01   AB      Renamed connection manager references to CNM from CM.

10/12/01   AB      Fixed Lint errors
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include "mn.h"
#include "mnglobal.h"
#include "ms.h"
#include "cmlib.h"
#include "cnm.h"


/**************** Begin of   CNM_send_MNSS_REJ_IND ********************************
 *
 *  Function name: CNM_send_MNSS_REJ_IND()
 *  ----------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to form a MNSS_REJ_IND message, including the cause value.
 *
 *  Parameters:
 *  -----------
 *  Service type  : IN
 *  cause : IN - cause value
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/
VOID_FUNC CNM_send_MNSS_REJ_IND( connection_id_T connection_id,
                                byte cause)
{

   MNSS_REJ_IND_T       *p_mnss_rej_ind;

   p_mnss_rej_ind = (MNSS_REJ_IND_T *)CNM_alloc( sizeof( MNSS_REJ_IND_T ));

   p_mnss_rej_ind->message_header.message_set = MS_MN_SS;
   p_mnss_rej_ind->message_header.message_id  = MNSS_REJ_IND;
   PUT_IMH_LEN( sizeof( MNSS_REJ_IND_T ) - sizeof(IMH_T),
                &p_mnss_rej_ind->message_header);

   p_mnss_rej_ind->connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM)
   if(MNCNM_IS_SINGLE_STACK)
   {
     p_mnss_rej_ind->as_id = ss_transaction_information[connection_id].as_id;
   }
#if defined(FEATURE_DUAL_ACTIVE)
   else
   {
     p_mnss_rej_ind->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif   
#endif

   p_mnss_rej_ind->lower_layer_cause = 0;
   p_mnss_rej_ind->mm_cause = cause;

   CNM_send_message( (IMH_T *)p_mnss_rej_ind, GS_QUEUE_MN_CM );

   CNM_free( (void *)p_mnss_rej_ind );
}
/**************** End of   CNM_send_MNSS_REJ_IND *******************************/
