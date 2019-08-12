/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_sms_mn_primitives.c_v   1.2   14 Aug 2001 15:17:28   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_sms_mn_primitives.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
8/7/01      CD      Rearranged header files

08/14/01   AB      Renamed connection manager references to CNM from CM.

04/09/07    RD      SMS optmization changes are incorporated

10/31/11    PM     Sending is_permanent as TRUE while reporting SMS failure
                   to WMS when MM sends SMS reject cause as MM_REJ_ACCESS_CLASS_BLOCKED

===========================================================================*/


#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"

#include "environ.h"
#include "mn.h"
#include "ms.h"

#include "cmlib.h"
#include "cnm.h" 
#include "mnglobal.h"


/**************** Begin of   CNM_send_MNSMS_ERROR_IND *****************************
 *
 *  Function name: CNM_send_MNSMS_ERROR_IND()
 *  ----------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to form a MNSMS_ERROR_IND message, including the cause value.
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
VOID_FUNC CNM_send_MNSMS_ERROR_IND( connection_id_T  connection_id,
                                   byte cp_cause,
                                   byte lower_layer_cause
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
                                   ,
                                   reject_cause_T reject_cause,
                                   mmcc_domain_name_T domain_name
#endif
                                    )
{
   MNSMS_ERROR_IND_T          *p_mnsms_error_ind;

   p_mnsms_error_ind = (MNSMS_ERROR_IND_T*) CNM_alloc( sizeof(MNSMS_ERROR_IND_T) );


   p_mnsms_error_ind->message_header.message_set = MS_MN_SMS;
   p_mnsms_error_ind->message_header.message_id  = MNSMS_ERROR_IND;
   PUT_IMH_LEN( sizeof( MNSMS_ERROR_IND_T ) - sizeof(IMH_T),
                &p_mnsms_error_ind->message_header);

   p_mnsms_error_ind->connection_id = connection_id;
#ifdef FEATURE_SMS_ENHANCED_DOMAIN_SELECTION
   p_mnsms_error_ind->permanent_error = ((reject_cause == MM_REJ_INVALID_SIM) ||
                                        (reject_cause == MM_REJ_ACCESS_CLASS_BLOCKED))? 1:0;
   p_mnsms_error_ind->domain = domain_name;
#endif

   if (cp_cause != 0)
   {
      p_mnsms_error_ind->cp_cause.Iei  = IEI_CP_CAUSE;
      p_mnsms_error_ind->cp_cause.cause_value  = cp_cause;
      p_mnsms_error_ind->lower_layer_cause = 0;
   }
   else
   {
      p_mnsms_error_ind->cp_cause.Iei  = 0;
      p_mnsms_error_ind->cp_cause.cause_value  = 0;
      p_mnsms_error_ind->lower_layer_cause = lower_layer_cause;
   }
    //Set the lower_layer_cause_value to zero, as this is used in IMS case only
   p_mnsms_error_ind->lower_layer_cause_value = (word)0;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     p_mnsms_error_ind->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif


   CNM_send_message( (IMH_T *)p_mnsms_error_ind, GS_QUEUE_MN_CM );

   CNM_free( p_mnsms_error_ind );
}
/**************** End of   CNM_send_MNSMS_ERROR_IND ****************************/

/**************** Begin of   CNM_send_CPACK_IND *****************************
 *
 *  Function name: CNM_send_CPACK_IND()
 *  ----------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to form a MNSMS_CPACK_IND message, including the error status and cause value.
 *
 *  Parameters:
 *  -----------
 *  error_status  : IN - error type
 *  cause_type : IN - cause value
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/
VOID_FUNC CNM_send_CPACK_IND( connection_id_T  connection_id,
                                   byte error_status,
                                   byte cause_value)
{
   MNSMS_CPACK_IND_T          *p_mnsms_cpack_ind;

   p_mnsms_cpack_ind = (MNSMS_CPACK_IND_T*) CNM_alloc( sizeof(MNSMS_CPACK_IND_T) );


   p_mnsms_cpack_ind->message_header.message_set = MS_MN_SMS;
   p_mnsms_cpack_ind->message_header.message_id  = MNSMS_CPACK_IND;
   PUT_IMH_LEN( sizeof( MNSMS_CPACK_IND_T ) - sizeof(IMH_T),
                &p_mnsms_cpack_ind->message_header);

   p_mnsms_cpack_ind->connection_id = connection_id;
   p_mnsms_cpack_ind->error_status = error_status;
   p_mnsms_cpack_ind->cause_value = cause_value;

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
   {
     p_mnsms_cpack_ind->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
   }
#endif



   CNM_send_message( (IMH_T *)p_mnsms_cpack_ind, GS_QUEUE_MN_CM );

   CNM_free( p_mnsms_cpack_ind );
}
/**************** End of   CNM_send_CPACK_IND ****************************/

