
#ifndef TM_LTE_NAS_MSGR_IFACE_H
#define TM_LTE_NAS_MSGR_IFACE_H

#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                            TM_LTE_NAS_MSGR_IFACE

DESCRIPTION


  Copyright (c) 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/src/tm_lte_nas_msgr_iface.h#1 $

  when        who     what, where, why
  --------    ---     ----------------------------------------------------------
  6/20/12      gk     Initial checkin
===========================================================================*/

#include "comdef.h"
#include "custcgps.h"
 


#include "msgr.h"
#include "msgr_rex.h"
#include "msgr_umid.h"
#include "msgr_umid.h"
#ifdef  FEATURE_CGPS_LTE_CELLDB   
#include "lte_tlb.h"
#endif /* LTE */

#include "cm.h"
#include "cm_msgr_msg.h"
#include "tm_data.h"

#define MSGR_ID_REX             MSGR_ID_NONE

/* Create TECH MODULE MACRO FOR GNSS TASKS */
#define MSGR_GPS_SM_TM   MSGR_TECH_MODULE(MSGR_TECH_GPS, THREAD_ID_SM_TM)

/* MSGR RECEIVE QUEUE SIZE */
#define TM_MSGR_RCV_QUEUE_SIZE  10

typedef struct
{
  q_link_type  qlink;

} tm_msgr_rcv_msg_hdr_type;

/* Messages which can be received over MSGR queue. We can receive messages over MSGR 
 * NAS only for now
 */

typedef union
{
    /*! ************* system general external messages *************/
    /*! Message header */
    msgr_hdr_struct_type                hdr;            

#ifdef  FEATURE_CGPS_LTE_CELLDB   
    /*! ********* external Messages ************/
    /* TLB message from NAS */
    lte_tlb_dl_info_ind_s               dl_info;
#endif

    /*! ********* external Messages ************/
    /* E911 Enter message from CM */
    cm_ac_info_ind_s_type emergency_enter_info;
    /*! ************ internal Messages *****************/
    /* E911 Exit message from CM */
    cm_ac_info_ind_s_type emergency_exit_info;
} tm_msgr_ctrl_msg_u_type;


/* Format of messages received over MSGR */
typedef struct
{
  tm_msgr_rcv_msg_hdr_type  msg_hdr;
  tm_msgr_ctrl_msg_u_type msg;
} tm_msgr_rcv_msg_type;


typedef struct
{
  tm_msgr_rcv_msg_type   q_items[ TM_MSGR_RCV_QUEUE_SIZE ];
  q_type                   msg_q;
  q_type                   free_q;
} tm_msgr_rcv_buffer_type;


/*===========================================================================

FUNCTION tm_msgr_init

DESCRIPTION
  This function is to initialize MSGR data structures.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_msgr_init(void);


/*===========================================================================

FUNCTION tm_handle_msgr_msg

DESCRIPTION
  This function is to handle MSGR messages to TM task

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
extern void tm_handle_msgr_msg(void );

#endif /* IFNDEF */
