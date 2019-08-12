/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       D A T A   S E R V I C E S

              S E R V I C E   O P T I O N   C O N T R O L

GENERAL DESCRIPTION
  This file contains a handler function to parse and process service option
  control messages related to data service options.

EXTERNALIZED FUNCTIONS
  ds707_socm_parse_socm
    Called when a SOCM (service option control message) is received from
    the base station.  Function parses SOCM to see if it has info regarding
    either PZID or the dormant (holddown) timer. 

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2002 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_socm.c_v   1.1   19 Nov 2002 18:41:40   akhare  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_socm.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/05/05   gr      Integrated EPZID functionality onto the main line
08/27/03   vr      Do not process SOCM if we are in a HDR system
11/17/02   ak      Updated file header comments.

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707

#include "amssassert.h"
#include "cai.h"
#include "err.h"
#include "msg.h"
#include "data_msg.h"
#include "snm.h"

#include "ds707.h"
#include "ds707_epzid.h"
#include "ds707_dorm_timer.h"
#include "dstask.h"


#ifdef FEATURE_HDR
#include "ds707_pkt_mgr.h"
#endif /* FEATURE_HDR */

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*---------------------------------------------------------------------------
  Field type codes for Packet Data service option control messages.
---------------------------------------------------------------------------*/
#define SOCMI_DORM_CNTL_FIELD_TYPE     0x03

/*---------------------------------------------------------------------------
  Field type codes for Packet Data service option control messages related
  to optional PZID based reconnection
---------------------------------------------------------------------------*/
#define SOCMI_EPZID_CNTL_FIELD_TYPE     0x04

/*---------------------------------------------------------------------------
  Mask to get the field type bits.
---------------------------------------------------------------------------*/
#define SOCMI_FIELD_TYPE_MASK          0x07

/*---------------------------------------------------------------------------
  Union of all the Packet Data Service Option Control Messages.
---------------------------------------------------------------------------*/
typedef PACKED union PACKED_POST
{
  byte                           cntl;        /* Control & Field Type      */
  ds707_dorm_timer_socm_msg_type dorm_timer;  /* Pkt Data dormant timer ctl*/
  ds707_epzid_socm_msg_type      epzid;       /* EPZID related info.       */
} socmi_msg_type;


/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                        INTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION      SOCMI_PARSE_EPZID_SOCM

DESCRIPTION   This function parses the service option control message. 
              What it really does is does a quick error check, and then
              posts this message to DS, so it's processed in DS task.

DEPENDENCIES  It is assumed that the message is already known to be a EPZID
              message.

RETURN VALUE  Returns indication if message was okay or not okay.

SIDE EFFECTS  None
===========================================================================*/
snm_socm_return_type socmi_parse_epzid_socm
(
  byte                        cntl,            /* SOCM CNTL field           */
  ds707_epzid_socm_msg_type*  epzid_socm_msg    /* SOCM with EPZID info       */
)
{
  ds_cmd_type               *cmd_ptr          = NULL;       /* DS Task msg */
  ds707_epzid_socm_msg_type *ds707_epzid_socm_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(epzid_socm_msg);

  if (((cntl & DS707_EPZID_CNTL_MASK) == DS707_EPZID_CNTL_DISABLE)    ||
      ((cntl & DS707_EPZID_CNTL_MASK) == DS707_EPZID_CNTL_ENABLE)     ||
      ((cntl & DS707_EPZID_CNTL_MASK) == DS707_EPZID_CNTL_CLEAR_LIST)
     )
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "PZID SOCM - post msg to DS");

    cmd_ptr = ds_allocate_cmd_buf(sizeof(ds707_epzid_socm_msg_type));
    if((NULL == cmd_ptr) || (NULL == cmd_ptr->cmd_payload_ptr))
    {
      ASSERT(0);
      return SNM_INVALID_MSG_TYP;
    }

    cmd_ptr->hdr.cmd_id = DS_CMD_707_PKT_EPZID_SOCM;

    ds707_epzid_socm_ptr = (ds707_epzid_socm_msg_type *)cmd_ptr->cmd_payload_ptr;
    ds707_epzid_socm_ptr->cntl     = cntl;
    ds707_epzid_socm_ptr->list_len = epzid_socm_msg->list_len;

    ds_put_cmd( cmd_ptr );
    return(SNM_VALID);
  }
  else
  {
    return(SNM_INVALID_MSG_TYP);
  }
} /* socmi_parse_epzid_socm() */
/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION      DS707_SOCM_PARSE_SOCM

DESCRIPTION  This function processes service option control messages related
             to data service options. Currently, the only Service Option
             Control Message that is supported is for Packet Data Dormant
             Timer control.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
snm_socm_return_type ds707_socm_parse_socm
(
  word  length,          /* Length of the type-specific fields (in bytes)  */
  byte *soctl_msg,       /* Pointer to the type-specific fields            */
  word  service_option   /* The service option that is currently connected */
)
{
  socmi_msg_type    *pkt_socm_msg;      /* Service Option Control Message  */
                                        /* for Packet data service options */
  byte                  field_type;     /* Field type of Service Option    */
                                        /* Control Message                 */  
  snm_socm_return_type  result = SNM_VALID; /* return value                */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -esym(715, length) */

  /*-------------------------------------------------------------------------
    For now, Service Option Control Messages are processed only for Packet
    Data service options.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_HDR
  /*-------------------------------------------------------------------------
    Don't process if we receive this event when in a HDR system.
  -------------------------------------------------------------------------*/
  if(ds707_pkt_get_current_data_sess_network() == SYS_SYS_MODE_HDR) 
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "SOCTL msg rcvd in HDR system!!!");
    result = SNM_UNSPEC;
  }
  else
#endif /* FEATURE_HDR */
  if( DS_ISDATA_PPP_PKT(service_option) )
  {
    pkt_socm_msg = (socmi_msg_type *)soctl_msg;

    /*-----------------------------------------------------------------------
      Get the Field Type bits.
    -----------------------------------------------------------------------*/
    field_type = pkt_socm_msg->cntl & SOCMI_FIELD_TYPE_MASK;

    switch(field_type)
    {
      case SOCMI_DORM_CNTL_FIELD_TYPE:
        result = ds707_dorm_timer_parse_socm(&(pkt_socm_msg->dorm_timer));
        break;

      /*---------------------------------------------------------------------
        Service option control message for optional Packet Zone ID based 
        reconnection.
      ---------------------------------------------------------------------*/
      case SOCMI_EPZID_CNTL_FIELD_TYPE:
        result = socmi_parse_epzid_socm(pkt_socm_msg->cntl, 
                                        &(pkt_socm_msg->epzid));
        break;

      default:
        DATA_IS707_MSG1(MSG_LEGACY_MED, "Unknown SO Ctl Msg w/ FIELD_TYPE: 0x%x", field_type);
        result = SNM_INVALID_MSG_TYP;
        break;
    }  /* switch */
  }
  else
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "SO Ctl Msg rx'ed for Non-Packet SO");
    result = SNM_MESSAGE_NOT_SUPPORTED;
  }

  return(result);

} /* ds707_socm_parse_socm() */




#endif /* FEATURE_DATA_IS707 */
