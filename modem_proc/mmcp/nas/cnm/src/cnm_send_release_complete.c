/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_send_release_complete.c_v   1.2   12 Mar 2002 16:07:46   vtawker  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_send_release_complete.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why

--------   ---     ----------------------------------------------------------
08/14/01   AB      Renamed connection manager references to CNM from CM.
03/12/02   VT      Removed Lint errors.
02/14/03   AB      Changed the facility_len to unsigned int per Lint.
01/12/05   AB      Ignore cause IE for NULL cause for PD_SS.

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"

#include "environ.h"
#include "sys_type.h"
#include "sys_cnst_v.h"
#include "sys_stru_v.h"
#include "com_iei_v.h"
#include "cc_iei_v.h"
#include "cc_cc.h"

#include "nasutils.h"
#include "cmlib.h"
#include "cnm.h"
#include "err.h"
#include <stringl/stringl.h>
#include "mnglobal.h"

/**************** Begin of   CNM_send_release_complete ****************************
 *
 *  Function name: CNM_send_release_complete()
 *  -----------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to form a release complete message and send it to mm
 *
 *  Parameters:
 *  -----------
 *  connection_id : IN
 *  pd : IN
 *  cause : IN - cause value
 *  p_facility : IN - pointer to facility ie to include in message, 0 if none
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_send_release_complete(  connection_id_T connection_id,
                                       byte pd,
                                       byte cause,
                                       ie_facility_T * p_facility)
{
   release_complete_msg_T     *p_release_complete;
   unsigned int               release_complete_length, facility_len;
   byte                       *p_write_point;
   dword                      len;
   byte                       no_of_bytes;

   p_release_complete = (release_complete_msg_T*) CNM_alloc( sizeof(release_complete_msg_T) );

   p_release_complete->msg_head.type = RELEASE_COMPLETE;

   /*
    * Set up a pointer to the end of the message so fare and
    * add a cause IE
    */

   p_write_point = &(p_release_complete->data[0]);

   if((pd == PD_SS) && (cause == NULL))
   {
#ifdef FEATURE_CAUSE_IN_SS_RELEASE_COMP
       no_of_bytes = CNM_write_cause_ie( p_write_point, NORMAL_CALL_CLEARING );
#else
       no_of_bytes = 0;
#endif
   }
   else
   {
       no_of_bytes = CNM_write_cause_ie( p_write_point, cause );
   }

   /* calculate message len, so far */
   p_write_point = ((byte *)(p_write_point)) + no_of_bytes;

   len = (unsigned long)( (byte *)p_write_point -
                          (byte *)&(p_release_complete->msg_head.ti_pd));
    
   PUT_IMH_LEN( len, &(p_release_complete->msg_head.IMH));

   /*
    * Check if there is a facility IE present
    */
   if ( p_facility != NULL )
   {
      GET_IMH_LEN(release_complete_length, &p_release_complete->msg_head.IMH);

       /*
       * copy the FACILITY IE to the release complete message and
       * adjust the length
       */
       facility_len = (release_complete_length - sizeof(msg_head_T) )+ sizeof(IMH_T);
       if((facility_len+p_facility->Length+2) >=  (MAX_OCTETS_L3_MESSAGE - 2))
       {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds and buffer overflow");
       }
       else
       {
         (void)memscpy((void *)&(p_release_complete->data[facility_len]),
                               (sizeof(p_release_complete->data) - facility_len),
                               (const void *) p_facility,
                                p_facility->Length+2 ); /* ti_pd + type */
          release_complete_length = release_complete_length + p_facility->Length+2; /* tag + length */
          PUT_IMH_LEN( release_complete_length, &p_release_complete->msg_head.IMH);
       }

      
   }

   CNM_send_primitive_to_mm(connection_id,
                            pd,
                            (IMH_T *)p_release_complete);

   CNM_free(p_release_complete);
}
/**************** End of   CNM_send_release_complete ****************************/
