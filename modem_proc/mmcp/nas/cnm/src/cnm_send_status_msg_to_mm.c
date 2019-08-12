/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_send_status_msg_to_mm.c_v   1.7   12 Jul 2002 17:18:26   abanh  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_send_status_msg_to_mm.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

08/07/01   CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

26-Oct-2001  VT    DTMF: In CNM_send_status(), removed the comments about
                   U10_DTMF. Also, changed the bit-mask used to fill the
                   call_state IE, from 7f to 3f.

10-Jan-2002  AB    Fixed seting of aux_hold_state in CNM_send_status() 
                   in response to status enquiring.  The call held state should
                   be shift by 2 bit not 1. 

25-Feb-2002  AB    Removed the checking of active connection/available
                   before sending status response to the network becuase 
                   MM will now check for availibility of the  
                   RRC connection.

12-Mar-2002  VT    Removed Lint errors.

06/27/02    CD     Fixed lint errors 

08/20/02    AB     Added status description details to F3 message for
                   debugging purposes. 

===========================================================================*/

#include "customer.h"
#include "rex.h"
#include "err.h"

#include "environ.h"
#include "mn.h"
#include "mnglobal.h"
#include "cc_cc.h"
#include "timers_v.h"
#include "ms_cc_mm_v.h"
#include "ms_timer_v.h"

#include "nasutils.h"
#include "cmlib.h"
#include "cnm.h"


const char cnm_aux_states[8][18] = {"MPTY IDLE       \n",
                                    "MPTY REQUEST    \n",
                                    "CALL IN MPTY    \n",
                                    "SPLIT REQUEST   \n",
                                    "HOLD IDLE       \n",
                                    "HOLD REQUEST    \n",
                                    "CALL HELD       \n",
                                    "RETRIEVE REQUEST\n"};



/**************** Begin of   CNM_send_status **************************************
 *
 *  Function name: CNM_send_status()
 *  -------------------------------
 *
 *  Description:
 *  ------------
 *  Function to form a status message and send it to mm
 *
 *  Parameters:
 *  -----------
 *  connection_id : INPUT
 *
 *  Returns:
 *  --------
 *  none
 *
 ******************************************************************************/

VOID_FUNC   CNM_send_status( connection_id_T connection_id,
                             byte pd,
                             byte cause)
{
   status_msg_T               status_msg;
   int                        length;
   byte                       *p_write_point;
   byte                       index;

   hold_auxiliary_state_T     hold_auxiliary_state = HOLD_IDLE;
   mpty_auxiliary_state_T     mpty_auxiliary_state = MPTY_IDLE;


   if(connection_id < MAXNO_CC_TRANSACTIONS) 
   {
      if(cc_transaction_information_p[connection_id] != NULL)
      {
        hold_auxiliary_state = cc_transaction_information_p[connection_id]->
                                   hold_auxiliary_state;

        mpty_auxiliary_state = cc_transaction_information_p[connection_id]->
                                   mpty_auxiliary_state;
      }
      else
      {
        MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot update CC AUX states in STATUS msg as cc_transaction_information_p[%d] is NULL",
                                   connection_id);
      }

      /*
       * Fill in message type
       */

      status_msg.msg_head.type = STATUS_MSG;

      /*
       * add the cause information.
       */

      p_write_point = (byte *)(&status_msg.data[0]);
      *(p_write_point++) = 2;             /* 2 bytes follow */
      *(p_write_point++) = 0xe0;  /* standard GSM   */
      *(p_write_point) = (unsigned char)(cause | 0x80);


      /*
       * Fill in call state (least significant 6 bits), and set coding 
       *   standard to GSM (11 in most significant 2 bits in the same octet)
       *
       * First make pointer to location to store the state information. This
       * is beyond the end of the cause IE which is the octet for the call
       * state IE 
       */

      index = (unsigned char)(status_msg.data[0] + 1);

      p_write_point = (byte *)(&(status_msg.data[index]));

      if(cc_transaction_information_p[connection_id] != NULL)
      {

        *p_write_point = (unsigned char)
           ((cc_transaction_information_p[connection_id]->cc_state & 0x3f) | 0xc0);

        p_write_point++;           /* point at next free byte */

        if (((cc_transaction_information_p[connection_id]->cc_state & 0x3f) == CC_STATE_U10)
               ||
            ((cc_transaction_information_p[connection_id]->cc_state & 0x3f) == CC_STATE_U26))
        {
           if (( hold_auxiliary_state != HOLD_IDLE ) ||
               ( mpty_auxiliary_state != MPTY_IDLE ) )
           {
            
             /*
              * add the IEI for the aux states to the status message
              */

             *p_write_point = IEI_AUXILIARY_STATES;
             p_write_point++;

             /*
              * Add the length of the remaining bytes of the IE
              */

             *p_write_point = 1;
             p_write_point++;

             /*
              * add the values of the aux states
              */

             *p_write_point = (unsigned char)(0x80 |
                                ((hold_auxiliary_state & 0x03) << 2) |
                                (mpty_auxiliary_state & 0x03)) ;

             p_write_point++;

           }
        }
      }
      else
      {
        *p_write_point = (unsigned char)
           ((CC_STATE_U0 & 0x3f) | 0xc0);

        p_write_point++;           /* point at next free byte */

        MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Updating CC state as U0 in STATUS msg as cc_transaction_information_p[%d] is NULL",
                                   connection_id);
      }

      /*
       * Set up the message length
       */

      length = (int) (((byte *)(p_write_point) - (byte *)(&status_msg))
                             - (int)(sizeof(IMH_T)));

      PUT_IMH_LEN(length,  &status_msg.msg_head.IMH);

      CNM_send_primitive_to_mm(connection_id, pd, (IMH_T *)&status_msg);

      if(cc_transaction_information_p[connection_id] != NULL)
      {
        MSG_HIGH_DS_3(MN_SUB,"=MNCNM= STATUS: ID=%x, CC=%x AUX=%x", connection_id, 
                  cc_transaction_information_p[connection_id]->cc_state,
                  ((hold_auxiliary_state & 0x03) << 2) | (mpty_auxiliary_state & 0x03));
      }
      else
      {
        MSG_HIGH_DS_2(MN_SUB,"=MNCNM= STATUS: ID=%x, CC=%x", connection_id,CC_STATE_U0);
      }
   }
   else
   {
      MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Received Invalid STATUS_ENQUIRY = %d\n", connection_id);
   }
}

/**************** End of   CNM_send_status ****************************************/
