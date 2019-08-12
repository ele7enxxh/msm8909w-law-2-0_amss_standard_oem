/*===========================================================================
                       COPYRIGHT INFORMATION

Copyright (c) 2011 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

                       EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_ss_transaction_msgs.c_v   1.4   28 Jun 2002 13:09:34   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_ss_transaction_msgs.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

08/06/01   CD      Replaced mn header files by single include of mn.h

08/14/01   AB      Renamed connection manager references to CNM from CM.

10/16/01   AB      Fixed Lint errors.

06/28/02   AB      In CNM_ss_start_mo_req(), don't Send SS Version Indicator 
                   on process USSD Request Phase compatible retry.

10/02/02   AB      In CNM_start_mo_ss_request(), corrected the message length
                   when SS version indicator is not used.

02/14/03   AB      Removed Lint errors.

03/02/03   AB      Declared the register_msg in CNM_start_mo_ss_request() 
                   global and volatile to prevent any compiler optimization 
                   on the cross phase checking operation.

04/12/05   AB      Removed optional CAUSE IE(CC normal call clearing) from 
                   SS RELEASE COMPLETE message.

07/04/11   AM      Putting range check for fixing buffer overflow of cc_IE_info.

09/07/11   AM      Adding comments in the code.

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"
#include "environ.h"
#include "com_iei_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "cc_cc.h"
#include "ms.h"
#include "ms_cc_mm_v.h"
#include "timers_v.h"
#include "ms_timer_v.h"
#include "cause.h"
#include "cmlib.h"
#include "cnm.h"
#include <stringl/stringl.h>
#include "nasutils.h"


/*
 * global data declaration
 */

/**************** Begin of   CNM_start_mo_ss_request ******************************
 *
 *  Function name: CNM_start_mo_ss_request()
 *  ---------------------------------------
 *
 *  Description:
 *  ------------
 *  This function processes the request from MN to start a MO SS transaction
 *
 *  Parameters:
 *  -----------
 *  SS_input_primitive : IN - input message
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC CNM_start_mo_ss_request ( IMH_T * SS_input_primitive )
{
   static volatile register_msg_T   register_msg;  /* make it global as CNM stack is low and msg > 255 bytes */ 
   connection_id_T            connection_id;
   boolean                    respond_to_event;
   byte                       * data_ptr;
   byte                       *last_octet_data_ptr;

   memset((void*)&register_msg, NULL, sizeof(register_msg_T));  /* clean the buffer */

   connection_id = ((MNSS_BEGIN_REQ_T *)((void *)SS_input_primitive))->connection_id;

   respond_to_event = CNM_ss_respond_to_event( connection_id, MNSS_BEGIN_REQ_event );

   if (respond_to_event != FALSE)
   {
      register_msg.msg_head.IMH.message_set = MS_CC_MM;
      register_msg.msg_head.IMH.message_id = MMCNM_DATA_REQ;

      register_msg.msg_head.ti_pd = (unsigned char) (FORM_TI_PD(connection_id, PD_SS));

      register_msg.msg_head.type = REGISTER;

      data_ptr = (byte *) &register_msg.data[0];
      last_octet_data_ptr = (byte *) &register_msg.data[sizeof(register_msg.data)-1];


      /* length in this memcpy comes from checked value in MN_put_facility() */
      (void)memscpy((void *)data_ptr,
                   (last_octet_data_ptr - data_ptr + 1),
                   (const void *) & ((MNSS_BEGIN_REQ_T *)((void *)SS_input_primitive))->data[0],
                   ((MNSS_BEGIN_REQ_T *)((void *)SS_input_primitive))->data[1] + 2 );

     /*
      * point to next byte to write ss version information to
      */
      data_ptr += (int)(((MNSS_BEGIN_REQ_T *)((void *)SS_input_primitive))->data[1] + 2);

      /* Check for process USSD data operation  */
      if((register_msg.data[7] == OP_CODE_TAG) && 
         (register_msg.data[9] == processUnstructuredSS_Data))
      {
          /* Ignore the SS version indicator if support Phase 1 USSD */
          MSG_HIGH_DS_0(MN_SUB,"=MNCNM= USSD Cross Phase Compatible - skip SS version");
      }
      else 
      {
          *data_ptr = IEI_SS_VERSION_INDICATOR;
          data_ptr++;

          *data_ptr = 0x01;       /* length of this IE */
          data_ptr++;

#ifdef FEATURE_CCBS
          *data_ptr = 0x00; /* Indicates phase 2 support */

          if ( register_msg.data[7] == OP_CODE_TAG )
          {
            switch ( register_msg.data[9] )
            {
              #if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
              case lcs_MOLR:
              #endif
              case eraseCCEntry:
                *data_ptr = 0x01; /* Indicates phase 3 support */
                break;
              default:
                break;
            }
          }
#else
          #if defined ( FEATURE_CM_MMGPS ) || defined ( FEATURE_CM_LCS )
          /* If SS message is lcs_MOLR, SS version needs to indicate phase 3 support. */
          if ( ( register_msg.data[7] == OP_CODE_TAG ) &&
               ( register_msg.data[9] == lcs_MOLR ) )
          {
            *data_ptr = 0x01;       /* Indicates phase 3 support */
          }
          else
          {
            *data_ptr = 0x00;       /* Indicates phase 2 support */
          }
          #else
          #error code not present
#endif
#endif /* FEATURE_CCBS */

          data_ptr++;
      }

      PUT_IMH_LEN(((int)(data_ptr - (byte *) &register_msg) -(int)sizeof(IMH_T)),
                   &register_msg.msg_head.IMH);

      CNM_manage_cnm_service_requests ( SUPPLEMENTARY_SERVICE_ACTIVATION,
                                        connection_id,
                                        (IMH_T *)&register_msg.msg_head.IMH );
   }
   else
   {
      MSG_ERROR_DS_0(MN_SUB,"=MNCNM= SS BEGIN Request unacceptable: respond_to_event = FALSE!\n");
#if defined(FEATURE_DUAL_SIM )
      if(MNCNM_IS_SINGLE_STACK)
      {
        if( (CNM_check_if_any_service_object_exists() == FALSE) &&
            (mn_as_id != SYS_MODEM_AS_ID_NONE) )
        {
          MSG_HIGH_DS_0(MN_SUB,"=MNCNM= =MN= Reset the mn_as_id here\n");
          mn_as_id = SYS_MODEM_AS_ID_NONE;
        }
      }
#endif
   }

}
/**************** End of   CNM_start_mo_ss_request ******************************/


/**************** Begin of   CNM_mo_end_ss_request *********************************
 *
 *  Function name: CNM_mo_end_ss_request()
 *  -------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles the request to terminate the ss transaction received
 *  from MN. A release complete message is sent to the network, with a facility
 *  IE present if there was ine in the input message
 *
 *  Parameters:
 *  -----------
 *  SS_input_primitive : IN
 *
 *  Returns:
 *  --------
 *  none 
 *
 ******************************************************************************/

VOID_FUNC   CNM_mo_end_ss_request(  IMH_T *SS_input_primitive )
{
   connection_id_T            connection_id;
   boolean                    respond_to_event;
   int                        length;
   ie_facility_T              *p_facility;


   connection_id = ((MNSS_END_REQ_T *)((void *) SS_input_primitive))->connection_id;

   respond_to_event = CNM_ss_respond_to_event( connection_id,
                                              MNSS_END_REQ_event);
   
   if (respond_to_event != FALSE)
   {
      GET_IMH_LEN( length, SS_input_primitive);

      /*
       * Check if there is a facility IE present
       */

      if ( ( length > (sizeof(connection_id_T) + sizeof(sys_modem_as_id_e_type)) ) && (((MNSS_END_REQ_T *) ((void *)SS_input_primitive))->data[0] == IEI_FACILITY))
      {

          p_facility = (ie_facility_T *)
                     &((MNSS_END_REQ_T *) ((void *)SS_input_primitive))-> data[0];

          MSG_HIGH_DS_3(MN_SUB,"=MNCNM= SS component=%d, operation=%d,%d",p_facility->data[2], p_facility->data[7] , p_facility->data[9]);

      }
      else
      {
          p_facility = NULL;
      }

      if(ss_transaction_information[connection_id].offline != TRUE)
      {
          CNM_send_release_complete( connection_id,
                                     PD_SS,
                                     NULL,
                                     p_facility );
      }
   }
   else
   {
      MSG_HIGH_DS_0(MN_SUB,"=MNCNM= MNSS_END_REQ msg: respond_to_event = FALSE\n");
   }

}
/**************** End of   CNM_mo_end_ss_request **********************************/


/**************** Begin of   CNM_start_mt_ss_request ******************************
 *
 *  Function name: CNM_start_mt_ss_request()
 *  ---------------------------------------
 *
 *  Description:
 *  ------------
 *  This function handles a REGISTER message received from the network
 *
 *  Parameters:
 *  -----------
 *  SS_input_primitive : IN
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_start_mt_ss_request(  IMH_T *SS_input_primitive )
{
   byte                       *p_IE;
   byte                       IEI;
   byte                       IE_count;
   byte                       IE_length ;
   boolean                    syntax_ok;
   connection_id_T            connection_id;
   static MNSS_BEGIN_IND_T    mnss_begin_ind;  /* make it global to save stack space */
   boolean                    respond_to_event;
   byte                       type;
   boolean                    facility_found;
   byte                       *write_ptr;
   byte                       *last_octet_write_ptr;
   int                        length;

   p_IE = (byte *) 0;
   facility_found = FALSE;

   IE_count = 2;              /* start at first IE after message type */

   connection_id = CNM_get_connection_id((mmcnm_data_ind_T *) ((void *)SS_input_primitive));

   syntax_ok = TRUE;

   type = ((msg_head_T *)((void *)SS_input_primitive))->type;

   memset(&mnss_begin_ind, NULL, sizeof(MNSS_BEGIN_IND_T));

   if (type == REGISTER)
   {
      mnss_begin_ind.message_header.message_set = MS_MN_SS;
      mnss_begin_ind.message_header.message_id = MNSS_BEGIN_IND;
      mnss_begin_ind.connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE) 
      {
        mnss_begin_ind.as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
      }
#endif
      write_ptr = &mnss_begin_ind.data[0];
      last_octet_write_ptr = &mnss_begin_ind.data[sizeof(mnss_begin_ind.data) -1];

      if ( no_of_IEs > MAXNO_IES_IN_MESSAGE )
      {
        ERR("no_of_IEs exceeds MAXNO_IES_IN_MESSAGE", 0, 0, 0);
        no_of_IEs = MAXNO_IES_IN_MESSAGE;
      }

      /*
       * set up write pointer ready to append the IEs
       */

      while (( syntax_ok != FALSE ) && ( IE_count < no_of_IEs ))
      {

         /*
          * get information of the next IE from global store
          */

         IEI       = cc_IE_info[IE_count].IEI;
         p_IE      = cc_IE_info[IE_count].start;
         IE_length = cc_IE_info[IE_count].length;

         syntax_ok = CNM_check_IE_syntax(IEI, p_IE, IE_count);

         if ( syntax_ok != FALSE )
         {
            switch (IEI)
            {
               /*
                * accept the following IEs
                */
               case IEI_FACILITY:

                  if (facility_found == FALSE )
                  {

                     facility_found = TRUE;     /* ignore second facility */

                     /*
                      * Form mnss_begin_ind message with data
                      */


                     (void)memscpy((void *) write_ptr, (last_octet_write_ptr - write_ptr + 1) ,(const void *) p_IE, IE_length );

                     write_ptr += IE_length;

                  }
                  break;

               default:                            /* this cannot happen! */
                  ERR("Unexpected IE in facility msg: received IEI = %d\n",
                       (dword) IEI, 0, 0);

                  syntax_ok = FALSE;
                  break;
            }
         }        /* ENDIF syntax is OK */

         IE_count++;       /* index next IE in the message */

      }   /* END WHILE more IEs and Syntax is OK */

      if ( syntax_ok != FALSE )
      {
         if(connection_id >= MAXNO_SS_TRANSACTIONS)
         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds and buffer overflow");
            respond_to_event = FALSE;
         }
         else
         {
            respond_to_event =
               CNM_ss_respond_to_event( connection_id, MMSS_EST_IND_event );
         }

         if ( respond_to_event != FALSE )
         {
            length = (int) ((char *) write_ptr -
                            (char *)&mnss_begin_ind) -
                            (int ) sizeof(IMH_T);

            PUT_IMH_LEN( length, &mnss_begin_ind.message_header);

            CNM_send_message( (IMH_T *)&mnss_begin_ind, GS_QUEUE_MN_CM );
         }
         else
         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= MMSS_EST_IND(REGISTER) unexpected in this state: respond_to_event = FALSE\n");

            CNM_send_release_complete(
                           connection_id,
                           PD_SS,
                           MESSAGE_TYPE_NOT_COMPATIBLE_WITH_PROT_STATE,
                           (ie_facility_T *) 0 );
         }
      }
      else
      {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Syntax error in MMSS_EST_IND(REGISTER) msg\n");

         CNM_send_release_complete(  connection_id,
                                    PD_SS,
                                    INVALID_MANDATORY_INFORMATION,
                                    (ie_facility_T *) 0 );

      }
   }
}

/**************** End of   CNM_start_mt_ss_request ******************************/
