/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   RF REX MSGR Interface

GENERAL DESCRIPTION
  This file contains functions specific/internal to the RF REX MSGR Interface.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright(c) 2009-2014 by Qualcomm Technologies, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/task/rex_msgr/src/rf_rex_msgr_interface.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/15/14   aro     Deprecated old WCDMA cmd dispatcher
10/03/13   sb      Add changes to receive MSGR cmd from MCS to switch to filtered/unfiltered 
                   path for SGLTE coexistence (GSM DCS +LTE B39/TDS B39/B34)
07/29/13   tws     Add new RF task RF_TASK_1. Move GSM to RF_TASK_1.
06/20/13   tws     Register dispatchers for RF_TASK_3
03/13/13   jj      call rfwcdma_msgr_register() and fwcdma_msgr_deregister(), add #include "rfwcdma_mc.h"
03/11/13   sar     Updated file for APQ, GNSS only target.
02/05/13   aca     RFRPE Task registration
01/31/13   sc      Add iRAT meas msgr registration and de-registration
01/10/13   av      Create new REQ and CNF ids for subscriber 2 and handle them in rf_task_2
11/01/12   pl      Fix warnings
10/05/12   av      Expanded rf task architecture to include more tasks. 
08/09/12   sc      Enable rfgsm msgr registration
08/02/12   sar     Moved rf_task_dispatch_init() above tdscdma call to 
                   resolve null pointer crash
07/06/12   kb      Portrd RCINIT changes from RFA3/main/1.0
03/12/12   ggs     Added GSM FTM module to MSGR
06/19/12   sc      Added GSM to MSGR
04/04/12   pl      Add support for LTE FW messages
03/22/12   aro     Deleted cdma_rf_task file
03/22/12   aro     Mainlined new Dispatch for RF_TASK
03/22/12   aro     Deleted old RF_TASK based dispatching of CDMA
                   firmware responses
03/13/12   aro     Added new RF_TASK dispatcher init and deinit. Code
                   is protected with debug flag
02/15/12   sar     Replaced FEATURE_RF_HAS_LTE with FEATURE_LTE.
01/04/12   aki     Added calls to CDMA cleanup functions
12/07/11   aro     Deleted unused CDMA files
11/21/11   aro     Mainlined debug variable to use new RF task dispatch
11/10/11   aro     Added check to bypass old CDMA dispatch init
07/20/11   jhe     Add TDS msg registration
05/25/11   sty     Renamed APIs
05/19/11   sty     Removed debug flags, featurized code for non-multimode
                   builds
08/16/10   mkv     Declaring extern function prototype to fix compiler warning.
11/19/09   mkv     Adding rf_rex_msgr_interface.h 
11/09/09   pl      Adding rf_cap.h to temporarily define FEATURE_RF_HAS_LTE
11/06/09   mkv     Moving MSGR registration and de-registration required for RF Task 
                                     into LTE files.
10/06/09   mkv     Updated LTE RF Calibration command definitions.
09/01/09   can     Changes to L1 Interface.
08/26/09   mkv     Integrating REX MSGR interworking into RFA2 
07/16/09   mkv     Initial revision
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include "rfa_variation.h"
#include "rex.h"
#include "msgr_rex.h"
#include "err.h"
#include "rf_rex_msgr_interface.h"
#include "rf_cmd_interface.h"
#include "rf_cap.h"
#include "rfm_umid_list.h"
#include "rfcommon_cmd_processing.h"
#ifdef FEATURE_LTE
#include "rf_lte_ftm_cmd_proc.h"
#include "rflte_mc.h"
#endif /*FEATURE_LTE*/
#ifdef FEATURE_RF_HAS_TDSCDMA
#include "rf_tdscdma_mc.h"
#endif /*FEATURE_RF_HAS_TDSCDMA*/
#ifdef FEATURE_GSM
#include "rfgsm_mc.h"
#include "rfgsm_ftm_cmd_proc.h"
#endif /*FEATURE_GSM*/
#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#ifdef FEATURE_RF_MEAS
#include "rfmeas_mc.h"
#endif /*FEATURE_RF_MEAS*/
#include "rf_1x_rf_task.h"
#include "rf_hdr_rf_task.h"
#endif
#include "rfm_internal.h"
#include "rf_task.h"
#include "rf_qmi_rfrpe_rf_task.h"
#ifdef FEATURE_WCDMA
#include "rfwcdma_mc.h"
#endif /*FEATURE_WCDMA*/


/*! @brief Holds RF task msgr related information. 
*/
typedef struct
{

  msgr_client_t  client; /*!< All RF interaction with the message router 
                                       include this client as a parameter. */
  msgr_id_t      mb;         /*!< Mailbox: This mailbox 
                                      allows RF task to receive messages from a message 
                                      router based task.*/
}rf_task_msgr_s;


static rf_task_msgr_s rf_task_rex_msgr[RF_MAX_TASK]; // RF Task MSGR Client and Mailbox

extern rf_cmd_struct_type* rf_cmd_get_queue_address( rf_task_num_type task_num ); /*Extern prototype declaration of function defined in rf_cmd_interface.c */
boolean rf_task_msgr_register( msgr_client_t *client_id, msgr_id_t id );
boolean rf_task_msgr_deregister( msgr_client_t *client_id );


/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/

/*===========================================================================

  FUNCTION:  rf_task_msgr_register

===========================================================================*/
/*!
  @brief
  This function is used by RF Task to register for any message router commands
  that it intends to recieve.


  @return
  TRUE if all registrations were successful, FALSE otherwise
*/
/*=========================================================================*/
boolean rf_task_msgr_register( msgr_client_t *client_id, msgr_id_t id )
{
  boolean ret_status = TRUE;
#ifdef FEATURE_LTE
  ret_status = rf_lte_msgr_register(client_id, id);
  ret_status &= rf_lte_ftm_cal_msgr_register(client_id, id);
#endif /*FEATURE_LTE*/

#ifdef FEATURE_CDMA1X
  /* register the RF-CDMA module with msg_router */
  ret_status &= rf_1x_rf_task_dispatch_init( client_id, id );
  ret_status &= rf_hdr_rf_task_dispatch_init( client_id, id );
#endif 

#ifdef FEATURE_MODEM_RCINIT
  ret_status &= rf_task_dispatch_init(client_id, id);
#endif

#ifdef FEATURE_RF_HAS_TDSCDMA
  ret_status &= rf_tdscdma_l1_msgr_register(client_id, id);
#endif

#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#ifdef FEATURE_RF_MEAS
  ret_status &= rfmeas_msgr_register(client_id, id);
#endif /*FEATURE_RF_MEAS*/
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */

  /*RFRPE task- used by QMI server*/
  ret_status &= rf_qmi_rfrpe_rf_task_dispatch_init( client_id, id );

  ret_status &= rfcommon_msgr_register( client_id, id );

  return ret_status;

}
/*===========================================================================

  FUNCTION:  rf_task_msgr_deregister

===========================================================================*/
/*!
  @brief
  This function is used by RF Task to de-register any message router commands
  that it had registered for.


  @return
  TRUE if de-registration is successful, FALSE otherwise
*/
/*=========================================================================*/
boolean rf_task_msgr_deregister( msgr_client_t *client_id )
{
    boolean ret_status = TRUE;
#ifdef FEATURE_LTE
  ret_status = rf_lte_msgr_deregister(client_id);
  ret_status &= rf_lte_ftm_cal_msgr_deregister(client_id);
#endif /*FEATURE_LTE*/

#ifdef FEATURE_CDMA1X
  rf_1x_rf_task_dispatch_deinit(client_id);
  rf_hdr_rf_task_dispatch_deinit(client_id);
#endif

#ifdef FEATURE_RF_HAS_TDSCDMA
  ret_status &= rf_tdscdma_l1_msgr_deregister(client_id);
#endif

#ifdef FEATURE_GSM
  ret_status &= rfgsm_msgr_deregister(client_id);
  ret_status &= rfgsm_ftm_msgr_deregister(client_id);
#endif /*FEATURE_GSM*/

#ifndef FEATURE_GNSS_ONLY_NO_WWAN
#ifdef FEATURE_RF_MEAS
  ret_status &= rfmeas_msgr_deregister(client_id);
#endif /*FEATURE_RF_MEAS*/
#endif /* FEATURE_GNSS_ONLY_NO_WWAN */

#ifdef FEATURE_MODEM_RCINIT
    ret_status &= rf_task_dispatch_deinit(client_id);
#endif

ret_status &= rfcommon_msgr_deregister(client_id);


  return ret_status;
}

/*===========================================================================

  FUNCTION:  rf_create_msgr_client_add_rex_queue

===========================================================================*/
/*!
  @brief
  Creates the RF REX Message router client and adds the REX Queue to the MSGR client.

  @return
  True if Client  are created successfully
*/
/*=========================================================================*/
boolean rf_create_msgr_client_add_rex_queue
( 
  rf_task_num_type task_num, rex_tcb_type *p_rf_tcb,rex_sigs_type sig
)
{
  errno_enum_type ret_status;
  boolean         rc         = TRUE;
  rf_cmd_struct_type *ptr_rf_cmd;

  /* Create RF Task message router client for receiving messages from SW entities such L1, FTM Cal etc.  */
  ret_status = msgr_client_create(&(rf_task_rex_msgr[task_num].client));
  if (ret_status != E_SUCCESS)
  {
    MSG_ERROR("Unable to create RF message router client.",0,0,0);  
    rc = FALSE;
  }
  /*Fetch the address of the rf command pointer.*/
  ptr_rf_cmd =  rf_cmd_get_queue_address(task_num);


  /*! @brief Create message queue to receive message router messages from other LTE SW entities.
      RF is a REX Task and it waits on REX signals. Hence a REX message queue has to
      be added to the message router client created above. The parameter list to
      msgr_client_add_rex_q is as follows:
      msgr_client_t             *msgr_client_id,   //!< Message Client Handle
      rex_tcb_type              *tcb,              //!< REX TCB pointer
      rex_sigs_type              sig,              //!< REX signal to set
      q_type                    *msg_queue,        //!< queue to place the msg
      q_type                    *free_msg_queue,   //!< queue to get empty buffer
      uint16                     msgr_hdr_offset,  //!< offset to msgr_hdr
      uint16                     cmd_type_offset,  //!< offset to legacy cmd_type
      uint16                     max_msg_size,     //!< max size of msgs
      msgr_id_t                 *queue_id          //!< queue identifier
   */

  if (ret_status == E_SUCCESS)
  {
    ret_status = msgr_client_add_rex_q(
                                      &(rf_task_rex_msgr[task_num].client),
                                      p_rf_tcb,
                                      sig, 
                                      &(ptr_rf_cmd->cmd_q),
                                      &(ptr_rf_cmd->free_q),
                                      sizeof(rf_cmd_rex_hdr_type),
                                      sizeof(q_link_type),
                                      sizeof(rf_cmd_type),
                                      &(rf_task_rex_msgr[task_num].mb)
                                      );

    if (ret_status != E_SUCCESS)
    {
      MSG_ERROR("Unable to add RF REX MSGR client.",0,0,0);
      rc = FALSE;
    }
  }

  return(rc);
}

/*!=========================================================================
  @brief
  RF Rex MSGR interface initialization..

  @details
  This funciton creates the message router client on the RF TCB, registers for
  all the MSGR commands that RF task wishes to recieve.

  @param p_rf_tcb
  Pointer to the RF TCB.
 
  @param rf_rex_sig
  Signal to be set when a message router command is recieved by the RF task.
 
  @retval void
==========================================================================*/
boolean rf_rex_msgr_interface_init(rf_task_num_type task_num, rex_tcb_type *p_rf_tcb,rex_sigs_type rf_rex_sig)
{
  boolean ret_status =  TRUE;
  /*Create message router client and add REX queue. */
  ret_status = rf_create_msgr_client_add_rex_queue(task_num, p_rf_tcb,rf_rex_sig);

  if (ret_status)
  {
    MSG_LOW("RF MSGR client created and REX queue added .",0,0,0);
  }
  else
  {
    MSG_ERROR("RF MSGR client creation and REX queue addition failed.",0,0,0);
  }
  return ret_status;
}

/*===========================================================================

  FUNCTION:  rf_rex_msgr_interface_deinit

===========================================================================*/
/*!
  @brief
  RF Message Router Tear down

  @return
  None.
*/
boolean rf_rex_msgr_interface_deinit(rf_task_num_type task_num)
{
  boolean ret_status = TRUE;

    /*De-register the UMIDs.*/
  ret_status = rf_task_msgr_deregister(&(rf_task_rex_msgr[task_num].client));

  /*Release all mailboxes associated with RF client */
  if (msgr_client_delete(&(rf_task_rex_msgr[task_num].client)) == E_SUCCESS)
  {
    ret_status = TRUE; 
  }
  else
  {
    ret_status = FALSE;
  }
  return ret_status;
}

/*===========================================================================

  FUNCTION:  rf_rex_task_msgr_register

===========================================================================*/
/*!
  @brief
  Register messages with MSGR

  @return
  None.
*/
boolean rf_rex_task_msgr_register( rf_task_num_type task_num)
{
  boolean ret_status =  TRUE;
  if (task_num == RF_TASK_MAIN)
  {
    ret_status = rf_task_msgr_register(&(rf_task_rex_msgr[task_num].client), rf_task_rex_msgr[task_num].mb);
  }
  else if (task_num == RF_TASK_1 )
  {
#ifdef FEATURE_GSM
    ret_status &= rfgsm_msgr_register(&(rf_task_rex_msgr[task_num].client), rf_task_rex_msgr[task_num].mb);
    ret_status &= rfgsm_ftm_msgr_register(&(rf_task_rex_msgr[task_num].client), rf_task_rex_msgr[task_num].mb);
#endif /*FEATURE_GSM*/
  }
  else if (task_num == RF_TASK_2)
  {
#ifdef FEATURE_GSM
/* for rf_task_2 which is for subscriber 2 a different set of UMIDs need to be registered for GL1<->RF communication*/
    ret_status &= rfgsm_msgr_register_sub2(&(rf_task_rex_msgr[task_num].client), rf_task_rex_msgr[task_num].mb);
    ret_status &= rfgsm_ftm_msgr_register(&(rf_task_rex_msgr[task_num].client), rf_task_rex_msgr[task_num].mb);
#endif /*FEATURE_GSM*/
  }
  else if (task_num == RF_TASK_3)
  {
#ifdef FEATURE_GSM
  /* for rf_task_3 which is for subscriber 3 a different set of UMIDs need to be registered for GL1<->RF communication*/
    ret_status &= rfgsm_msgr_register_sub3(&(rf_task_rex_msgr[task_num].client), rf_task_rex_msgr[task_num].mb);
#endif /*FEATURE_GSM*/
  }
  return ret_status;
}
