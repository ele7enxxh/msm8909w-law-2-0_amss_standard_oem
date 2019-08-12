/*==============================================================================

          e M B M S D A T A - S P E C I F I C   H A N D L E R 


                   D S _ 3 G P P _ E M B M S _ R R C I F . C
GENERAL DESCRIPTION
  This file contains the functions that implement the interface between
  3GPP eMBMS mode handler (eMBMS MH) and LTE RRC. The mode of communicatiob 
  between eMBMS MH and LTE RRC is through message router interface. eMBMS MH
  sends request messages to LTE RRC when it receives an IOCTL from the upper
  layers. eMBMS MH registers for messages from LTE RRC and handles them 
  accordingly. The messages received can be solicited or unsolicited.

EXTERNALIZED FUNCTIONS
  boolean ds_3gpp_embms_rrcif_is_iface_bring_up_allowed(void)


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2014 - 2015 by Qualcomm Technologies Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/embms/src/ds_3gpp_embms_rrcif.c#1 $ 
$DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/19/11    hs     Created file/Initial version.
==============================================================================*/

/*==============================================================================
                          INCLUDE FILES FOR MODULE
==============================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_EMBMS

#include "dsmsgr.h"
#include "lte_rrc_ext_msg.h"
#include "ds_3gpp_embms_hdlr.h"
#include "ds_3gpp_embms_rrcif.h"
#include "ds_3gpp_embms_tmgi_tbl_mgr.h"
#include "ds_3gppi_utils.h"
#include "modem_mem.h"
#include "dsm_item.h"
#include "msgr.h"
#include "ds3gpp_ext_msg.h"
/*==============================================================================
                            VARIABLES
==============================================================================*/

#define DS_3GPP_EMBMS_RRCIF_VALIDATE_NUM_ACT_EMBMS_SESSIONS(num_sessions) \
  (num_sessions <= DS_3GPP_EMBMS_MAX_ACTIVE_SESSIONS)

#define DS_3GPP_EMBMS_RRCIF_VALIDATE_NUM_AVAIL_EMBMS_SESSIONS(num_sessions) \
  (num_sessions <= DS_3GPP_EMBMS_MAX_AVAIL_SESSIONS)

/*------------------------------------------------------------------------------
  Boolean to keep track of the status of eMBMS on the modem. TRUE on this
  boolean indicates that eMBMS is enabled on the modem.

  This boolean will be used to determine if we can allow interface bring up or 
  not. If eMBMS is disabled, interface bring up will be disallowed.
------------------------------------------------------------------------------*/
static boolean ds_3gpp_embms_is_enabled;

/*==============================================================================
        Forward declaration for functions implemented in this module
==============================================================================*/

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_CONVERT_TMGI_INFO_RRC_TO_DS

DESCRIPTION
  This function is used to convert the TMGI info from the RRC format to DS
  format.

  Spec reference: TS 26.346 Section 7.3.2.7
  Based on CR 172 - Applicable to Rel 10 of the spec.

  Note: By aligning the type defs between DS and RRC, these conversion functions
  can be removed

PARAMETERS
  IN
    rrc_info_ptr - pointer to the TMGI in RRC format

  OUT
    ds_info_ptr  - pointer to TMGI in DS format

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL void ds_3gpp_embms_rrcif_convert_tmgi_info_rrc_to_ds
(
  lte_rrc_embms_session_info_s *rrc_info_ptr,
  ps_iface_embms_tmgi_type     *ds_info_ptr
);

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_CONVERT_TMGI_INFO_DS_TO_RRC

DESCRIPTION
  This function is used to convert the TMGI info from the DS format to RRC
  format.

  Spec reference: TS 26.346 Section 7.3.2.7
  Based on CR 172 - Applicable to Rel 10 of the spec.

  Note: By aligning the type defs between DS and RRC, these conversion functions
  can be removed
  
PARAMETERS
  IN
    rrc_info_ptr - pointer to the TMGI in RRC format

  OUT
    ds_info_ptr  - pointer to TMGI in DS format

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL void ds_3gpp_embms_rrcif_convert_tmgi_info_ds_to_rrc
(
  ps_iface_embms_tmgi_type     *ds_info_ptr,
  lte_rrc_embms_session_info_s *rrc_info_ptr
);

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_ACT_TMGI_CNF_HDLR

DESCRIPTION
  This function is used to process the LTE_RRC_EMBMS_ACT_TMGI_CNF message. This
  message is sent to DS as a response to the ACT_TMGI req message.

  The status of activation is a part of this message.

  This function will run under the eMBMS critical section so that there is no
  change to the table until the processing of this message is complete.
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_act_tmgi_cnf_hdlr 
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
);

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_DEACT_TMGI_CNF_HDLR

DESCRIPTION
  This function is used to process the LTE_RRC_EMBMS_DEACT_TMGI_CNF message. The
  message is sent to DS as a response to the DEACT_TMGI req message.

  The status of activation is a part of this message. The status is believed to
  be success always.

  This function will run under the eMBMS critical section so that there is no
  change to the table until the processing of this message is complete.
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_deact_tmgi_cnf_msg_hdlr 
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
);

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_DEACT_TMGI_CNF_HDLR

DESCRIPTION
  This function is used to process the LTE_RRC_EMBMS_DEACT_TMGI_IND message. The
  message is sent when the modem deactivates TMGIs without a request from the
  client to deactivate.

  The status of activation is a part of this message. The status is used to 
  convey more info about the reason for deactivation.

  This function will run under the eMBMS critical section so that there is no
  change to the table until the processing of this message is complete.
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_deact_tmgi_ind_msg_hdlr 
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
);

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_EMBMS_STATUS_CHANGE_MSG_HDLR

DESCRIPTION
  This function is used to handle the LTE_RRC_EMBMS_STATUS_CHANGE_IND message
  from LTE RRC
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_embms_status_change_msg_hdlr 
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
);

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_ACT_TMGI_LIST_MSG_HDLR

DESCRIPTION
  This function is used to handle the LTE_RRC_EMBMS_ACT_TMGI_LIST_IND message
  from LTE RRC

  Currently this function dispatches the indication to every client - in other
  words, if this is a response to a request from a particular client, it is
  still broadcast to all clients. This behaviour should be changed such that we
  send the indication only to the requestor if this is a solicited response.

  If this if an unsolicited response, it will be sent to every client.
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_act_tmgi_list_msg_hdlr
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
);

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_AVAIL_TMGI_LIST_MSG_HDLR

DESCRIPTION
  This function is used to handle the LTE_RRC_EMBMS_AVAIL_TMGI_LIST_IND message
  from LTE RRC

  Currently this function dispatches the indication to every client - in other
  words, if this is a response to a request from a particular client, it is
  still broadcast to all clients. This behaviour should be changed such that we
  send the indication only to the requestor if this is a solicited response.

  If this if an unsolicited response, it will be sent to every client.
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_avail_tmgi_list_msg_hdlr
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
);

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_AVAIL_SAI_LIST_MSG_HDLR

DESCRIPTION
  This function is used to handle the LTE_RRC_EMBMS_AVAIL_SAI_LIST_IND message
  from LTE RRC

  Currently this function dispatches the indication to every client - in other
  words, if this is a response to a request from a particular client, it is
  still broadcast to all clients. This behaviour should be changed such that we
  send the indication only to the requestor if this is a solicited response.

  If this if an unsolicited response, it will be sent to every client.
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_avail_sai_list_msg_hdlr
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
);

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_WARN_TMGI_LIST_MSG_HDLR

DESCRIPTION
  This function is used to handle the LTE_RRC_EMBMS_WARN_TMGI_LIST_IND message
  from LTE RRC

  Currently this function dispatches the indication to every client - in other
  words, if this is a response to a request from a particular client, it is
  still broadcast to all clients. This behaviour should be changed such that we
  send the indication only to the requestor if this is a solicited response.

  If this if an unsolicited response, it will be sent to every client.
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_warn_tmgi_list_msg_hdlr
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
);

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_CONTENT_DESC_CONTROL_MSG_HDLR

DESCRIPTION
  This function is used to handle the DS_LTE_EMBMS_CONTENT_DESC_CONTROL_IND
  message from LTE RLC

  MH relays the info from RLC to QMI
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_content_desc_control_msg_hdlr
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_RRCIF_MAP_CAUSE_CODE

DESCRIPTION
  This function is used to map the deactivation cause code provided by LTE RRC
  into PS error code.

PARAMETERS
  error_code     - Error code specified by LTE RRC
  cause_category - This specifies the category of failure. Please refer to the
                   definition of ps_iface_embms_tmgi_deact_reason_type.

DEPENDENCIES  
  None

RETURN VALUE
  ps_iface_embms_tmgi_deact_reason_type based on incoming cause.
  IFACE_EMBMS_TMGI_DEACTIVATION_UNSPECIFIED is we are unable to map

SIDE EFFECTS  
  None
==============================================================================*/
LOCAL ps_iface_embms_tmgi_event_info_code_type 
                                        ds_3gpp_embms_rrcif_map_cause_code
(
  void        *error_code,
  int         cause_category       
);

/*==============================================================================
                            Functions implemented in this module
==============================================================================*/
/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_RRCIF_IS_IFACE_BRING_UP_ALLOWED

DESCRIPTION   This function returns the status as advertized by LTE RRC. If 
              eMBMS has not been enabled on the modem, interface bring up shall
              be disallowed.

DEPENDENCIES  

RETURN VALUE
  TRUE if eMBMS is enabled on the modem
  FALSE otherwise

SIDE EFFECTS  None
==============================================================================*/
boolean ds_3gpp_embms_rrcif_is_iface_bring_up_allowed(void)
{

/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - -*/
  return ds_3gpp_embms_is_enabled;
} /* ds_3gpp_embms_rrcif_is_iface_bring_up_allowed */

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_EMBMS_STATUS_CHANGE_MSG_HDLR

DESCRIPTION
  This function is used to handle the LTE_RRC_EMBMS_STATUS_CHANGE_IND message
  from LTE RRC
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_embms_status_change_msg_hdlr 
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
)
{
  boolean                            ret_val = FALSE;
  lte_rrc_embms_status_change_ind_s  *msg_ptr= NULL;
  sys_modem_as_id_e_type              subs_id = SYS_MODEM_AS_ID_NONE;
  ds_3gpp_embms_ctrl_blk_type        *ctrl_blk_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - -*/
  
  if((msgrtype == LTE_RRC_EMBMS_STATUS_CHANGE_IND) && (dsmsg_ptr != NULL))
  {
    msg_ptr = (lte_rrc_embms_status_change_ind_s*)dsmsg_ptr;

    subs_id = INST_ID_TO_SYS_AS_ID(msg_ptr->msg_hdr.inst_id);

    /*--------------------------------------------------------------------------
      Set the data variable reflecting eMBMS status to the value advertized
      by LTE RRC
    --------------------------------------------------------------------------*/
    ds_3gpp_embms_is_enabled = msg_ptr->enabled_status;

    DS_EMBMS_MSG2_HIGH("EMBMS Status change Ind. Status: %d, debug_trace_id %d",
                       ds_3gpp_embms_is_enabled, msg_ptr->debug_trace_id);

    if(ds_3gpp_embms_is_enabled == FALSE)
    {
      DS_EMBMS_MSG0_HIGH("EMBMS disabled. Tearing down the interface");
      /*------------------------------------------------------------------------
        We have received a notification from LTE RRC that eMBMS is disabled.
        We should tear down the eMBMS iface and phys link if it is in UP state
      ------------------------------------------------------------------------*/

      ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(subs_id);

      if (ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
      {
        ds_3gpp_embms_tear_down(ctrl_blk_p);
      }
      else
      {
        ret_val = TRUE;
      }
    }
  }
  else
  {
    DS_EMBMS_MSG1_ERROR("Unexpected message type: %d",msgrtype );
  }

  return ret_val;
} /* ds_3gpp_embms_rrcif_embms_status_change_msg_hdlr */

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_CONVERT_TMGI_INFO_DS_TO_RRC

DESCRIPTION
  This function is used to convert the TMGI info from the DS format to RRC
  format.

  Spec reference: TS 26.346 Section 7.3.2.7
  Based on CR 172 - Applicable to Rel 10 of the spec.

  Note: By aligning the type defs between DS and RRC, these conversion functions
  can be removed
  
PARAMETERS
  IN
    rrc_info_ptr - pointer to the TMGI in RRC format

  OUT
    ds_info_ptr  - pointer to TMGI in DS format

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL void ds_3gpp_embms_rrcif_convert_tmgi_info_ds_to_rrc
(
  ps_iface_embms_tmgi_type     *ds_info_ptr,
  lte_rrc_embms_session_info_s *rrc_info_ptr
)
{
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT((rrc_info_ptr != NULL) && (ds_info_ptr != NULL));

  /*----------------------------------------------------------------------------
    Fill in the fields of the message using the incoming information
  ----------------------------------------------------------------------------*/
  rrc_info_ptr->session_id_present = ds_info_ptr->session_id_present;

  if(ds_info_ptr->session_id_present == TRUE)
  {
    rrc_info_ptr->session_id = ds_info_ptr->session_id;
  }

  /*----------------------------------------------------------------------------
    From the incoming 6 byte array for TMGI, copy the first 3 into Service id
    of the RRC message. The next 3 bytes are to be copied into the TMGI.

    This information is based on TS 26.346 Section 7.3.2.7
    Based on CR 172 - Applicable to Rel 10 of the spec.
  ----------------------------------------------------------------------------*/
  memscpy((void*)&(rrc_info_ptr->tmgi.service_id),
          DS_3GPP_EMBMS_RRCIF_SVC_ID_SIZE,
          (void*)(ds_info_ptr->tmgi),
          DS_3GPP_EMBMS_RRCIF_SVC_ID_SIZE);

  memscpy((void*)&(rrc_info_ptr->tmgi.plmn_id.identity),
  	   DS_3GPP_EMBMS_RRCIF_PLMN_ID_SIZE,
          (void*)(ds_info_ptr->tmgi+DS_3GPP_EMBMS_RRCIF_PLMN_OFFSET),
          DS_3GPP_EMBMS_RRCIF_PLMN_ID_SIZE);

  return;
} /* ds_3gpp_embms_rrcif_convert_tmgi_info_ds_to_rrc */

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_CONVERT_TMGI_INFO_RRC_TO_DS

DESCRIPTION
  This function is used to convert the TMGI info from the RRC format to DS
  format.

  Spec reference: TS 26.346 Section 7.3.2.7
  Based on CR 172 - Applicable to Rel 10 of the spec.

  Note: By aligning the type defs between DS and RRC, these conversion functions
  can be removed
      
PARAMETERS
  IN
    rrc_info_ptr - pointer to the TMGI in RRC format

  OUT
    ds_info_ptr  - pointer to TMGI in DS format

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL void ds_3gpp_embms_rrcif_convert_tmgi_info_rrc_to_ds
(
  lte_rrc_embms_session_info_s *rrc_info_ptr,
  ps_iface_embms_tmgi_type     *ds_info_ptr
)
{
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT((rrc_info_ptr != NULL) && (ds_info_ptr != NULL));

  ds_info_ptr->session_id_present = rrc_info_ptr->session_id_present;

  if(ds_info_ptr->session_id_present == TRUE)
  {
    ds_info_ptr->session_id = rrc_info_ptr->session_id;
  }

  memscpy((void*)ds_info_ptr->tmgi, 
  	   sizeof(ds_info_ptr->tmgi),
           (void*)&(rrc_info_ptr->tmgi.service_id),
           DS_3GPP_EMBMS_RRCIF_SVC_ID_SIZE);

  memscpy((void*)(ds_info_ptr->tmgi + DS_3GPP_EMBMS_RRCIF_PLMN_OFFSET), 
  	   DS_3GPP_EMBMS_RRCIF_PLMN_ID_SIZE ,
           (void*)&(rrc_info_ptr->tmgi.plmn_id.identity),
           DS_3GPP_EMBMS_RRCIF_PLMN_ID_SIZE);

  return;

} /* ds_3gpp_embms_rrcif_convert_tmgi_info_rrc_to_ds */

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_DEACT_TMGI_CNF_HDLR

DESCRIPTION
  This function is used to process the LTE_RRC_EMBMS_DEACT_TMGI_IND message. The
  message is sent when the modem deactivates TMGIs without a request from the
  client to deactivate.

  The status of activation is a part of this message. The status is used to 
  convey more info about the reason for deactivation.

  This function will run under the eMBMS critical section so that there is no
  change to the table until the processing of this message is complete.
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_deact_tmgi_ind_msg_hdlr 
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
)
{
  boolean                                    ret_val = FALSE;
  lte_rrc_embms_deact_tmgi_ind_s             *msg_ptr= NULL;
  rex_crit_sect_type                         *crit_section_ptr = NULL;
  ps_iface_embms_tmgi_type                   tmgi_info;
  uint8                                      index = 
                                                DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE;
  ps_iface_embms_tmgi_deactivation_info_type deact_ind_info;
  uint8                                      loop_index = 0;
  void*                                      d_mem_ptr = NULL;
  sys_modem_as_id_e_type                     subs_id = SYS_MODEM_AS_ID_NONE;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */


  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  if((msgrtype == LTE_RRC_EMBMS_DEACT_TMGI_IND) && (dsmsg_ptr != NULL))
  {
    /*--------------------------------------------------------------------------
      Set the retval to TRUE. This can be done as we are processing the right
      message
    --------------------------------------------------------------------------*/
    ret_val = TRUE;

    /*--------------------------------------------------------------------------
      Typecast the incoming message payload to the right type
    --------------------------------------------------------------------------*/
    msg_ptr = (lte_rrc_embms_deact_tmgi_ind_s*)dsmsg_ptr;

    subs_id = INST_ID_TO_SYS_AS_ID(msg_ptr->msg_hdr.inst_id);

    DS_EMBMS_MSG1_HIGH("TMGI DEACT IND Processing, debug_trace_id: %d",
                        msg_ptr->debug_trace_id);
    /*--------------------------------------------------------------------------
      Memset the deact_ind payload needed for the sending the indications.

      Explicitly set the client id in the deact_ind payload to 0xFFFF. 
      The function that sends out indication uses this info to figure out that 
      indication has to be sent to all the clients.
    --------------------------------------------------------------------------*/
    memset((void*)&deact_ind_info, 0, sizeof(deact_ind_info));

    deact_ind_info.client_id = (void*)PS_IFACE_EMBMS_RESERVED_CLIENT_ID;

    deact_ind_info.debug_trace_id = msg_ptr->debug_trace_id;
    /*--------------------------------------------------------------------------
      We will populate PS cause code and send iface indication.
    --------------------------------------------------------------------------*/
    deact_ind_info.info_code = 
      ds_3gpp_embms_rrcif_map_cause_code((void*)(msg_ptr->reason),
                                         PS_IFACE_EMBMS_CAUSE_CATEGORY_3);

    /*--------------------------------------------------------------------------
      Copy the field that specifies the number of TMGIs that have been 
      deactivated
    --------------------------------------------------------------------------*/
    deact_ind_info.tmgi_list.num_tmgi =  msg_ptr->num_tmgi_deactivated;

    DS_3GPP_MSG2_HIGH("TMGI Deact Ind. Cause: %d, Num TMGI: %d",
                      deact_ind_info.info_code,
                      deact_ind_info.tmgi_list.num_tmgi);

    /*--------------------------------------------------------------------------
      Allocate memory for the number of TMGIs deactivated.

      If mem allocation fails, there is no need to send indication but
      remove the TMGIs from the table.
    --------------------------------------------------------------------------*/
     d_mem_ptr = modem_mem_alloc(
                 msg_ptr->num_tmgi_deactivated*sizeof(ps_iface_embms_tmgi_type),
                                                         MODEM_MEM_CLIENT_DATA);

     if(d_mem_ptr == NULL)
     {
       DS_3GPP_MSG0_ERROR("Mem allocation for deactivated TMGI fail");
     } /* Memory allocation failed */

     deact_ind_info.tmgi_list.list = (ps_iface_embms_tmgi_type*)d_mem_ptr;
    /*--------------------------------------------------------------------------
      Loop over all the TMGIs in the RRC payload and copy the content to the
      mem block allocated.

      Remove the TMGI from the table as we process each TMGI
    --------------------------------------------------------------------------*/
    for(loop_index = 0; 
                       loop_index < msg_ptr->num_tmgi_deactivated; loop_index++)
    {
      /*------------------------------------------------------------------------
        Memset the TMGI on the stack and set the Session id to the invalid value
      ------------------------------------------------------------------------*/
      memset((void*)&tmgi_info, 0, sizeof(ps_iface_embms_tmgi_type));
      tmgi_info.session_id = PS_IFACE_EMBMS_INVALID_SESSION_ID;

      /*------------------------------------------------------------------------
        Convert the TMGI info in the message to the format that we maintain in
        TMGI table.
      ------------------------------------------------------------------------*/
      ds_3gpp_embms_rrcif_convert_tmgi_info_rrc_to_ds(
                           &(msg_ptr->deact_info_list[loop_index].session_info),
                                                                    &tmgi_info);

      /*------------------------------------------------------------------------
        Print the response. This involves printing the TMGI, session id of the 
        request
      ------------------------------------------------------------------------*/
      ds_3gpp_embms_print_tmgi_session_info(&tmgi_info);

      if(deact_ind_info.tmgi_list.list != NULL)
      {
        /*----------------------------------------------------------------------
          Memory allocation successful. Copy the contents of the local TMGI
          variable into the mem block in the right place
        ----------------------------------------------------------------------*/
        memscpy((void*)(deact_ind_info.tmgi_list.list+loop_index),
                 sizeof(ps_iface_embms_tmgi_type),
                 &tmgi_info, sizeof(ps_iface_embms_tmgi_type));
      }/* If mem was allocated */

      if(ds_3gpp_embms_tmgi_tbl_mgr_find_tmgi
           (&tmgi_info, TRUE, &index, subs_id) == TRUE)
      {
        /*----------------------------------------------------------------------
          Assert that the index returned in valid
        ----------------------------------------------------------------------*/
        ASSERT(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index));

        /*--------------------------------------------------------------------
          Clean up the TMGI entry in the table
        --------------------------------------------------------------------*/
        ds_3gpp_embms_tmgi_tbl_mgr_remove_entry(index, subs_id);
      } /* if TMGI is present in the table */

    }/* loop to get all TMGI info*/
  
    if(deact_ind_info.tmgi_list.list != NULL)
    {

      ds_3gpp_embms_tmgi_deactivated_gen_ind(&deact_ind_info, index, subs_id);

      /*-------------------------------------------------------------------
        Free the memory as the contents would be copied into a local struct
        while sending indications to clients
      -------------------------------------------------------------------*/
      modem_mem_free(
                (void*)deact_ind_info.tmgi_list.list,
                       MODEM_MEM_CLIENT_DATA);
    } /* mem allocation success */

    /*--------------------------------------------------------------------
      Perform RLC related operations. This function will de-reg RLC WM
      only if this is the last TMGI to be deactivated.
    --------------------------------------------------------------------*/
    ds_3gpp_embms_bring_down_rlc_to_ps_wm(subs_id);
  }
  else
  {
    DS_EMBMS_MSG1_ERROR("DEACT TMGI IND: Invalid args, message type: %d",msgrtype );
  }

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
  return ret_val;
} /* ds_3gpp_embms_rrcif_deact_tmgi_ind_msg_hdlr */

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_ACT_DEACT_TMGI_CNF_HDLR

DESCRIPTION
  This function is used to process the LTE_RRC_EMBMS_ACT_DEACT_TMGI_CNF message. The
  message is sent to DS as a response to the ACT_DEACT_TMGI req message.

  The status of activation/deactivation is a part of this message. 

  This function will run under the eMBMS critical section so that there is no
  change to the table until the processing of this message is complete.
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_act_deact_tmgi_cnf_hdlr 
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
)
{
  boolean                                     ret_val = FALSE;
  lte_rrc_embms_act_deact_tmgi_cnf_s          *msg_ptr= NULL;
  rex_crit_sect_type                          *crit_section_ptr = NULL;
  uint8                                       act_index;
  uint8                                       deact_index;
  ps_iface_embms_tmgi_activation_deactivation_info_type
                                              act_deact_cnf_info;
  boolean                                     act_tmgi_entry_found;
  boolean                                     deact_tmgi_entry_found;
  boolean                                     remove_act_entry = FALSE;
  sys_modem_as_id_e_type                      subs_id = SYS_MODEM_AS_ID_NONE;
  ds_3gpp_embms_ctrl_blk_type                 *ctrl_blk_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */
  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  if((msgrtype == LTE_RRC_EMBMS_ACT_DEACT_TMGI_CNF) && (dsmsg_ptr != NULL))
  {
    /*--------------------------------------------------------------------------
      Set the retval to TRUE. This can be done as we are processing the right
      message
    --------------------------------------------------------------------------*/
    ret_val = TRUE;

    /*--------------------------------------------------------------------------
      Typecast the incoming message payload to the right type
    --------------------------------------------------------------------------*/
    msg_ptr = (lte_rrc_embms_act_deact_tmgi_cnf_s*)dsmsg_ptr;

    DS_EMBMS_MSG3_HIGH("TMGI ACT/DEACT CNF - ACT Status: %d, DEACT Status: %d "
                       "debug_trace_id: %d", msg_ptr->act_status, 
					   msg_ptr->deact_status, msg_ptr->debug_trace_id);

    subs_id = INST_ID_TO_SYS_AS_ID(msg_ptr->msg_hdr.inst_id);

    ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(subs_id);

    if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
    {
      DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
      return FALSE;
    }

    /*--------------------------------------------------------------------------
      Memset the TMGI on the stack and set the Session id to the invalid value
    --------------------------------------------------------------------------*/
    memset((void*)&act_deact_cnf_info, 0, sizeof(act_deact_cnf_info));

    act_deact_cnf_info.act_tmgi_session_info.session_id = 
	                                          PS_IFACE_EMBMS_INVALID_SESSION_ID;
    act_deact_cnf_info.deact_tmgi_session_info.session_id = 
	                                           PS_IFACE_EMBMS_INVALID_SESSION_ID;
    /*--------------------------------------------------------------------------
      Convert the TMGI info in the message to the format that we maintain in
      TMGI table.
    --------------------------------------------------------------------------*/
    ds_3gpp_embms_rrcif_convert_tmgi_info_rrc_to_ds(
                                           &(msg_ptr->act_bearer_info.session_info),
                                           &(act_deact_cnf_info.act_tmgi_session_info));
    ds_3gpp_embms_rrcif_convert_tmgi_info_rrc_to_ds(
                                           &(msg_ptr->deact_bearer_info.session_info),
                                           &(act_deact_cnf_info.deact_tmgi_session_info));
    /*--------------------------------------------------------------------------
      Print the response. This involves printing the TMGI, session id and status
      of request
    --------------------------------------------------------------------------*/
    ds_3gpp_embms_print_tmgi_session_info(&(act_deact_cnf_info.act_tmgi_session_info));
    ds_3gpp_embms_print_tmgi_session_info(&(act_deact_cnf_info.deact_tmgi_session_info));


    /*--------------------------------------------------------------------------
    Handle activation response
    --------------------------------------------------------------------------*/

    if((act_tmgi_entry_found = 
         ds_3gpp_embms_tmgi_tbl_mgr_find_tmgi
           (&(act_deact_cnf_info.act_tmgi_session_info), 
             TRUE, &act_index, subs_id)) == TRUE)
    {

      act_deact_cnf_info.debug_trace_id = msg_ptr->debug_trace_id;

      switch(msg_ptr->act_status)
      {
        /*--------------------------------------------------------------------
          Fill the TMGI info and the extended info about activation to the fn
          ds_3gpp_embms_rrcif_tmgi_act_success_gen_ind. This fn generates the 
          tmgi activated indication to the clients that have requested that 
          the TMGI be activated.
        --------------------------------------------------------------------*/
      case LTE_EMBMS_SUCCESS_ALREADY_DONE:
      case LTE_EMBMS_SUCCESS_CONN_RADIO_TUNE:
      case LTE_EMBMS_SUCCESS_IDLE_RADIO_TUNE:
      case LTE_EMBMS_SUCCESS:
        act_deact_cnf_info.act_info_code = 
          ds_3gpp_embms_rrcif_map_cause_code((void*)msg_ptr->act_status,
                                             PS_IFACE_EMBMS_CAUSE_CATEGORY_0);
        /*--------------------------------------------------------------------
          Set the TMGI state to ACTIVE
        --------------------------------------------------------------------*/
        ds_3gpp_embms_tmgi_tbl_mgr_set_state
          (act_index, DS_3GPP_EMBMS_TMGI_STATE_ACTIVE, subs_id);

        /*--------------------------------------------------------------------
          Store the MRB Id.
        --------------------------------------------------------------------*/
        ds_3gpp_embms_tmgi_tbl_mgr_store_mrb_id
          (act_index, (uint8) msg_ptr->act_bearer_info.mrb_id, subs_id);

        /*--------------------------------------------------------------------
          Update the session info if LTE RRC has returned session id.
          This has to be done only if RRC indicates that the session id is
          valid
        --------------------------------------------------------------------*/
        if(act_deact_cnf_info.act_tmgi_session_info.session_id_present == TRUE)
        {
          ds_3gpp_embms_tmgi_tbl_mgr_store_session_id
            (act_index, act_deact_cnf_info.act_tmgi_session_info.session_id, subs_id);
        }

        /*--------------------------------------------------------------------------
          Set up the RLC - PS watermark.
        --------------------------------------------------------------------------*/
        ds_3gpp_embms_set_up_rlc_to_ps_wm(ctrl_blk_p);

        break;

      default:

        /*--------------------------------------------------------------------
          All other cases are treated as failures. We will populate PS cause
          code and send iface indication to reflect the same
        --------------------------------------------------------------------*/          
        act_deact_cnf_info.act_info_code = 
          ds_3gpp_embms_rrcif_map_cause_code((void*)msg_ptr->act_status,
                                             PS_IFACE_EMBMS_CAUSE_CATEGORY_1);
        remove_act_entry = TRUE;
      }
    }
    else
    {
      DS_EMBMS_MSG0_ERROR("Could not find act TMGI entry in table");
    } /* Could not find table entry */

    /*--------------------------------------------------------------------------
    Handle deactivation response
    --------------------------------------------------------------------------*/
    if((deact_tmgi_entry_found = 
       ds_3gpp_embms_tmgi_tbl_mgr_find_tmgi
        (&(act_deact_cnf_info.deact_tmgi_session_info), TRUE, 
         &deact_index, subs_id)) == TRUE)
    {
      switch(msg_ptr->deact_status)
      {
      case LTE_EMBMS_SUCCESS:
      case LTE_EMBMS_SUCCESS_ALREADY_DONE:
        DS_3GPP_MSG1_HIGH("TMGI DEACT CNF - Status: %d",msg_ptr->deact_status);
        break;
  
      default:
        DS_3GPP_MSG1_HIGH("TMGI DEACT CNF - Unexpected Status: %d",
                          msg_ptr->deact_status);
        break;
      } /* switch based on activation status */
  
      /*--------------------------------------------------------------------
        We will populate PS cause code and send iface indication.
      --------------------------------------------------------------------*/        
      act_deact_cnf_info.deact_info_code = 
                             IFACE_EMBMS_TMGI_DEACTIVATION_CLIENT_INITIATED;
      /*--------------------------------------------------------------------
        Perform RLC related operations. This function will de-reg RLC WM
        only if this is the last TMGI to be deactivated.
      --------------------------------------------------------------------*/
      ds_3gpp_embms_bring_down_rlc_to_ps_wm(subs_id);
    }/* Table entry found */

    if(act_tmgi_entry_found && deact_tmgi_entry_found)
    {
	  /* Use client ID from deact_index since ref count == 1*/
       ds_3gpp_embms_tmgi_act_deact_response_gen_ind(&act_deact_cnf_info,
                                                      deact_index, subs_id);
    }
	  /*--------------------------------------------------------------------
      Clean up the TMGI entry in the table
    --------------------------------------------------------------------*/
  	if(remove_act_entry)
  	{
       ds_3gpp_embms_tmgi_tbl_mgr_remove_entry(act_index, subs_id);
  	}
    ds_3gpp_embms_tmgi_tbl_mgr_remove_entry(deact_index, subs_id);
  }
  else
  {
    DS_EMBMS_MSG1_ERROR("Unexpected message type: %d",msgrtype );
  }

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
  return ret_val;
} /* ds_3gpp_embms_rrcif_act_deact_tmgi_cnf_hdlr */

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_DEACT_TMGI_CNF_HDLR

DESCRIPTION
  This function is used to process the LTE_RRC_EMBMS_DEACT_TMGI_CNF message. The
  message is sent to DS as a response to the DEACT_TMGI req message.

  The status of activation is a part of this message. The status is believed to
  be success always.

  This function will run under the eMBMS critical section so that there is no
  change to the table until the processing of this message is complete.
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_deact_tmgi_cnf_msg_hdlr 
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
)
{
  boolean                                    ret_val = FALSE;
  lte_rrc_embms_deact_tmgi_cnf_s             *msg_ptr= NULL;
  rex_crit_sect_type                         *crit_section_ptr = NULL;
  ps_iface_embms_tmgi_type                   tmgi_info;
  uint8                                      index = 
                                                DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE;
  ps_iface_embms_tmgi_deactivation_info_type deact_ind_info;

  ds_3gpp_embms_tmgi_state_e                 tmgi_state = 
                                               DS_3GPP_EMBMS_TMGI_STATE_INVALID;
  sys_modem_as_id_e_type                     subs_id = SYS_MODEM_AS_ID_NONE;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */
  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  if((msgrtype == LTE_RRC_EMBMS_DEACT_TMGI_CNF) && (dsmsg_ptr != NULL))
  {
    /*--------------------------------------------------------------------------
      Set the retval to TRUE. This can be done as we are processing the right
      message
    --------------------------------------------------------------------------*/
    ret_val = TRUE;

    /*--------------------------------------------------------------------------
      Typecast the incoming message payload to the right type
    --------------------------------------------------------------------------*/
    msg_ptr = (lte_rrc_embms_deact_tmgi_cnf_s*)dsmsg_ptr;

    subs_id = INST_ID_TO_SYS_AS_ID(msg_ptr->msg_hdr.inst_id);

    DS_EMBMS_MSG2_HIGH("TMGI DEACT CNF - Status: %d, debug_trace_id: %d",
                       msg_ptr->deact_status, msg_ptr->debug_trace_id );
    /*--------------------------------------------------------------------------
      Memset the TMGI on the stack and set the Session id to the invalid value
    --------------------------------------------------------------------------*/
    memset((void*)&tmgi_info, 0, sizeof(ps_iface_embms_tmgi_type));
    tmgi_info.session_id = PS_IFACE_EMBMS_INVALID_SESSION_ID;

    /*--------------------------------------------------------------------------
      Convert the TMGI info in the message to the format that we maintain in
      TMGI table.
    --------------------------------------------------------------------------*/
    ds_3gpp_embms_rrcif_convert_tmgi_info_rrc_to_ds(
                                           &(msg_ptr->bearer_info.session_info),
                                           &tmgi_info);

    /*--------------------------------------------------------------------------
      Print the response. This involves printing the TMGI, session id and status
      of request
    --------------------------------------------------------------------------*/
    ds_3gpp_embms_print_tmgi_session_info(&tmgi_info);

    if(ds_3gpp_embms_tmgi_tbl_mgr_find_tmgi
         (&tmgi_info, TRUE, &index, subs_id) == TRUE)
    {
      /*------------------------------------------------------------------------
        Assert that the index returned in valid
      ------------------------------------------------------------------------*/
      ASSERT(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index));

      tmgi_state = ds_3gpp_embms_tmgi_tbl_mgr_get_state(index, subs_id);

      if(tmgi_state == DS_3GPP_EMBMS_TMGI_STATE_DEACTIVATING)
      {
        /*----------------------------------------------------------------------
          Memset the deact_ind payload needed for the sending the indications.

          This memset also ensures that the client_id field in the deact_ind
          Payload is set to 0. The function that sends out indication uses this
          info to figure out that indication has to be sent to the client by
          looking up the client info from the TMGI table.
        ----------------------------------------------------------------------*/
        memset((void*)&deact_ind_info, 0, sizeof(deact_ind_info));

        switch(msg_ptr->deact_status)
        {
        case LTE_EMBMS_SUCCESS:
        case LTE_EMBMS_SUCCESS_ALREADY_DONE:
          DS_3GPP_MSG1_HIGH("TMGI DEACT CNF - Status: %d",msg_ptr->deact_status);
          break;

        default:
          DS_3GPP_MSG1_HIGH("TMGI DEACT CNF - Unexpected Status: %d",
                            msg_ptr->deact_status);
          break;
        } /* switch based on activation status */

        /*--------------------------------------------------------------------
          We will populate PS cause code and send iface indication.
        --------------------------------------------------------------------*/
        deact_ind_info.debug_trace_id = msg_ptr->debug_trace_id;
        deact_ind_info.info_code = 
                               IFACE_EMBMS_TMGI_DEACTIVATION_CLIENT_INITIATED;

        /*--------------------------------------------------------------------
          Allocate memory for one TMGI info. This message is sent only on req
          and should not specify more than one TMGI.

          Note: This is needed because the same indication is used for deact
          response as well as modem initated deactivation. Since the number
          of TMGIs may differ between these two events, we keep only a pointer
          to the TMGI list
        --------------------------------------------------------------------*/
        deact_ind_info.tmgi_list.num_tmgi = 1;

        deact_ind_info.tmgi_list.list = 
                            (ps_iface_embms_tmgi_type*)
                             modem_mem_alloc(sizeof(ps_iface_embms_tmgi_type),
                                             MODEM_MEM_CLIENT_DATA);

        if(deact_ind_info.tmgi_list.list != NULL)
        {
          memscpy((void*)deact_ind_info.tmgi_list.list,
                 sizeof(ps_iface_embms_tmgi_type),
                 (void*)&tmgi_info, 
                 sizeof(ps_iface_embms_tmgi_type));

          ds_3gpp_embms_tmgi_deactivated_gen_ind(&deact_ind_info, index, subs_id);

          /*-------------------------------------------------------------------
            Free the memory as the contents would be copied into a local struct
            while sending indications to clients
          -------------------------------------------------------------------*/
          modem_mem_free(
                (void*)deact_ind_info.tmgi_list.list,
                       MODEM_MEM_CLIENT_DATA);
        } /* mem allocation success */
        else
        {
          DS_3GPP_MSG0_ERROR("Modem heap alloc fail. TMGI deact cnf event");
        }

        /*--------------------------------------------------------------------
          Clean up the TMGI entry in the table
        --------------------------------------------------------------------*/
        ds_3gpp_embms_tmgi_tbl_mgr_remove_entry(index, subs_id);

        /*--------------------------------------------------------------------
          Perform RLC related operations. This function will de-reg RLC WM
          only if this is the last TMGI to be deactivated.
        --------------------------------------------------------------------*/
        ds_3gpp_embms_bring_down_rlc_to_ps_wm(subs_id);
      }/* TMGI state is DEACTIVATING*/
      else
      {
        DS_EMBMS_MSG1_ERROR("TMGI state is not as expected. State: %d. Ignore",
                            tmgi_state );
      }

    }/* Table entry found */
    else
    {
      DS_EMBMS_MSG0_ERROR("Could not find entry in table. Ignore deact_cnf msg");
    } /* Could not find table entry */
  }
  else
  {
    DS_EMBMS_MSG1_ERROR("Unexpected message type: %d",msgrtype );
  }

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
  return ret_val;
} /* ds_3gpp_embms_rrcif_deact_tmgi_cnf_msg_hdlr */

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_ACT_TMGI_CNF_HDLR

DESCRIPTION
  This function is used to process the LTE_RRC_EMBMS_ACT_TMGI_CNF message. This
  message is sent to DS as a response to the ACT_TMGI req message.

  The status of activation is a part of this message.

  This function will run under the eMBMS critical section so that there is no
  change to the table until the processing of this message is complete.
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_act_tmgi_cnf_hdlr 
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
)
{
  boolean                                    ret_val = FALSE;
  lte_rrc_embms_act_tmgi_cnf_s               *msg_ptr= NULL;
  rex_crit_sect_type                         *crit_section_ptr = NULL;
  ps_iface_embms_tmgi_type                   tmgi_info;
  uint8                                      index = 
                                                DS_3GPP_EMBMS_TMGI_TBL_MAX_SIZE;
  ps_iface_embms_tmgi_deactivation_info_type deact_ind_info;
  ps_iface_embms_tmgi_activation_info_type   activate_ind_info;
  sys_modem_as_id_e_type                     subs_id = SYS_MODEM_AS_ID_NONE;
  ds_3gpp_embms_ctrl_blk_type                *ctrl_blk_p = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */
  /*----------------------------------------------------------------------------
    Get the handle to the critical section that has to be locked while operating
    on the TMGI table
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  if((msgrtype == LTE_RRC_EMBMS_ACT_TMGI_CNF) && (dsmsg_ptr != NULL))
  {
    /*--------------------------------------------------------------------------
      Set the retval to TRUE. This can be done as we are processing the right
      message
    --------------------------------------------------------------------------*/
    ret_val = TRUE;

    /*--------------------------------------------------------------------------
      Typecast the incoming message payload to the right type
    --------------------------------------------------------------------------*/
    msg_ptr = (lte_rrc_embms_act_tmgi_cnf_s*)dsmsg_ptr;

    subs_id = INST_ID_TO_SYS_AS_ID(msg_ptr->msg_hdr.inst_id);

    ctrl_blk_p = ds_3gpp_embms_get_ctrl_blk(subs_id);

    if (!ds_3gpp_embms_validate_ctrl_blk(ctrl_blk_p))
    {
      DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
      return FALSE;
    }

    /*--------------------------------------------------------------------------
      Memset the TMGI on the stack and set the Session id to the invalid value
    --------------------------------------------------------------------------*/
    memset((void*)&tmgi_info, 0, sizeof(ps_iface_embms_tmgi_type));
    tmgi_info.session_id = PS_IFACE_EMBMS_INVALID_SESSION_ID;

    /*--------------------------------------------------------------------------
      Convert the TMGI info in the message to the format that we maintain in
      TMGI table.
    --------------------------------------------------------------------------*/
    ds_3gpp_embms_rrcif_convert_tmgi_info_rrc_to_ds(
                                           &(msg_ptr->bearer_info.session_info),
                                           &tmgi_info);

    /*--------------------------------------------------------------------------
      Print the response. This involves printing the TMGI, session id and status
      of request
    --------------------------------------------------------------------------*/
    ds_3gpp_embms_print_tmgi_session_info(&tmgi_info);

    /* TODO by rraghave
       Fix debug_trace_id for multiple client scenario.
     */
    DS_EMBMS_MSG2_HIGH("TMGI ACT CNF - Status: %d, debug_trace_id: %d",
                       msg_ptr->act_status, msg_ptr->debug_trace_id );

    if(ds_3gpp_embms_tmgi_tbl_mgr_find_tmgi
         (&tmgi_info, TRUE, &index, subs_id) == TRUE)
    {
      /*------------------------------------------------------------------------
        Assert that the index returned in valid
      ------------------------------------------------------------------------*/
      ASSERT(DS_3GPP_EMBMS_TMGI_TBL_VALIDATE_INDEX(index));

        /*----------------------------------------------------------------------
          Memset the payloads needed for the indications. Since we will not know
          which of the two payloads is needed till we examine the status, we can
          memset both. Also there should be no impact to stack usage since the 
          payloads only have pointers and the actual info is from the heap.
        ----------------------------------------------------------------------*/
        memset((void*)&activate_ind_info, 0, sizeof(activate_ind_info));
        memset((void*)&deact_ind_info, 0, sizeof(deact_ind_info));

      /* TODO by rraghave
          Fix debug_trace_id for multiple client scenario.
      */
      activate_ind_info.debug_trace_id = msg_ptr->debug_trace_id;

        switch(msg_ptr->act_status)
        {
          /*--------------------------------------------------------------------
            Fill the TMGI info and the extended info about activation to the fn
            ds_3gpp_embms_rrcif_tmgi_act_success_gen_ind. This fn generates the 
            tmgi activated indication to the clients that have requested that 
            the TMGI be activated.
          --------------------------------------------------------------------*/
      case LTE_EMBMS_SUCCESS_ALREADY_DONE:
      case LTE_EMBMS_SUCCESS_IDLE_RADIO_TUNE:
      case LTE_EMBMS_SUCCESS_CONN_RADIO_TUNE:
        case LTE_EMBMS_SUCCESS:
          activate_ind_info.act_info_code = 
            ds_3gpp_embms_rrcif_map_cause_code((void*)msg_ptr->act_status,
                                               PS_IFACE_EMBMS_CAUSE_CATEGORY_0);

          memscpy((void*)&(activate_ind_info.tmgi_session_info),
                 sizeof(ps_iface_embms_tmgi_type),
                 (void*)&tmgi_info, 
                 sizeof(ps_iface_embms_tmgi_type));

          /*--------------------------------------------------------------------
            Set the TMGI state to ACTIVE
          --------------------------------------------------------------------*/
          ds_3gpp_embms_tmgi_tbl_mgr_set_state
            (index, DS_3GPP_EMBMS_TMGI_STATE_ACTIVE, subs_id);

          /*--------------------------------------------------------------------
            Store the MRB Id.
          --------------------------------------------------------------------*/
          ds_3gpp_embms_tmgi_tbl_mgr_store_mrb_id
            (index, (uint8) msg_ptr->bearer_info.mrb_id, subs_id);

          /*--------------------------------------------------------------------
            Update the session info if LTE RRC has returned session id.
            This has to be done only if RRC indicates that the session id is
            valid
          --------------------------------------------------------------------*/
          if(activate_ind_info.tmgi_session_info.session_id_present == TRUE)
          {
            ds_3gpp_embms_tmgi_tbl_mgr_store_session_id
              (index, activate_ind_info.tmgi_session_info.session_id, subs_id);
          }
          /*--------------------------------------------------------------------------
            Set up the RLC - PS watermark.
          --------------------------------------------------------------------------*/
          ds_3gpp_embms_set_up_rlc_to_ps_wm(ctrl_blk_p);

          ds_3gpp_embms_tmgi_act_success_gen_ind
            (&activate_ind_info, index, subs_id);
          break;

        default:
          /*--------------------------------------------------------------------
            All other cases are treated as failures. We will populate PS cause
            code and send iface indication to reflect the same
          --------------------------------------------------------------------*/
          deact_ind_info.info_code = 
            ds_3gpp_embms_rrcif_map_cause_code((void*)msg_ptr->act_status,
                                               PS_IFACE_EMBMS_CAUSE_CATEGORY_1);
          /* TODO by rraghave
            Fix debug_trace_id for multiple client scenario.
          */
          deact_ind_info.debug_trace_id = msg_ptr->debug_trace_id;
          /*--------------------------------------------------------------------
            Allocate memory for one TMGI info. This message is sent only on req
            and should not specify more than one TMGI.

            Note: This is needed because the same indication is used for deact
            response as well as modem initated deactivation. Since the number
            of TMGIs may differ between these two events, we keep only a pointer
            to the TMGI list
          --------------------------------------------------------------------*/
          deact_ind_info.tmgi_list.num_tmgi = 1;

          deact_ind_info.tmgi_list.list = 
                              (ps_iface_embms_tmgi_type*)
                               modem_mem_alloc(sizeof(ps_iface_embms_tmgi_type),
                                               MODEM_MEM_CLIENT_DATA);

          if(deact_ind_info.tmgi_list.list != NULL)
          {
            memscpy((void*)deact_ind_info.tmgi_list.list,
                   sizeof(ps_iface_embms_tmgi_type),
                   (void*)&tmgi_info, 
                   sizeof(ps_iface_embms_tmgi_type));

            ds_3gpp_embms_tmgi_deactivated_gen_ind(&deact_ind_info, index, subs_id);

            /*-------------------------------------------------------------------
              Free the memory as the contents would be copied into a local struct
              while sending indications to clients
            -------------------------------------------------------------------*/
            modem_mem_free(
                  (void*)deact_ind_info.tmgi_list.list,
                         MODEM_MEM_CLIENT_DATA);
          } /* mem allocation success */
          else
          {
            DS_EMBMS_MSG0_ERROR("Modem heap alloc fail. TMGI act cnf fail ev");
          }

          /*--------------------------------------------------------------------
            Clean up the TMGI entry in the table
          --------------------------------------------------------------------*/
          ds_3gpp_embms_tmgi_tbl_mgr_remove_entry(index, subs_id);
          break;

        } /* switch based on activation status */      

    }/* Table entry found */
    else
    {
      DS_EMBMS_MSG0_ERROR("Could not find entry in table. Ignoring message");
    } /* Could not find table entry */
  }
  else
  {
    DS_EMBMS_MSG1_ERROR("Unexpected message type: %d",msgrtype);
  }

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
  return ret_val;
} /* ds_3gpp_embms_rrcif_act_tmgi_cnf_hdlr */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_RRCIF_INIT

DESCRIPTION   This function is used to perform the steps needed at power up
              initialization. This mainly registers for the response messages
              and indications that we expect from RRC.

DEPENDENCIES  
  None

RETURN VALUE
  None

SIDE EFFECTS  
  None
==============================================================================*/
void ds_3gpp_embms_rrcif_init(void)
{
  
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */
  /*----------------------------------------------------------------------------
    Register for all messages that DS is interested in receiving from LTE RRC
    for eMBMS purposes
  ----------------------------------------------------------------------------*/
  dsmsgrrcv_reg_hdlr(LTE_RRC_EMBMS_ACT_TMGI_CNF,
               (dsmsgrcv_msg_hdlr_f)ds_3gpp_embms_rrcif_act_tmgi_cnf_hdlr);

  dsmsgrrcv_reg_hdlr(LTE_RRC_EMBMS_DEACT_TMGI_CNF,
              (dsmsgrcv_msg_hdlr_f)ds_3gpp_embms_rrcif_deact_tmgi_cnf_msg_hdlr);

  dsmsgrrcv_reg_hdlr(LTE_RRC_EMBMS_ACT_DEACT_TMGI_CNF,
              (dsmsgrcv_msg_hdlr_f)ds_3gpp_embms_rrcif_act_deact_tmgi_cnf_hdlr);

  dsmsgrrcv_reg_hdlr(LTE_RRC_EMBMS_DEACT_TMGI_IND,
              (dsmsgrcv_msg_hdlr_f)ds_3gpp_embms_rrcif_deact_tmgi_ind_msg_hdlr);

  dsmsgrrcv_reg_hdlr(LTE_RRC_EMBMS_AVAIL_TMGI_LIST_IND,
             (dsmsgrcv_msg_hdlr_f)ds_3gpp_embms_rrcif_avail_tmgi_list_msg_hdlr);

  dsmsgrrcv_reg_hdlr(LTE_RRC_EMBMS_AVAIL_SAI_LIST_IND,
             (dsmsgrcv_msg_hdlr_f)ds_3gpp_embms_rrcif_avail_sai_list_msg_hdlr);

  dsmsgrrcv_reg_hdlr(LTE_RRC_EMBMS_ACT_TMGI_LIST_IND,
               (dsmsgrcv_msg_hdlr_f)ds_3gpp_embms_rrcif_act_tmgi_list_msg_hdlr);

  dsmsgrrcv_reg_hdlr(LTE_RRC_EMBMS_STATUS_CHANGE_IND,
         (dsmsgrcv_msg_hdlr_f)ds_3gpp_embms_rrcif_embms_status_change_msg_hdlr);

  dsmsgrrcv_reg_hdlr(LTE_RRC_EMBMS_OOS_WARN_IND,
             (dsmsgrcv_msg_hdlr_f)ds_3gpp_embms_rrcif_warn_tmgi_list_msg_hdlr);

  dsmsgrrcv_reg_hdlr(DS_LTE_EMBMS_CONTENT_DESC_CONTROL_IND,
        (dsmsgrcv_msg_hdlr_f)ds_3gpp_embms_rrcif_content_desc_control_msg_hdlr);

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

} /* ds_3gpp_embms_rrcif_init */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_RRCIF_DEACTIVATE_TMGI

DESCRIPTION
  This function is used to deactivate a TMGI. It copies the incoming payload into
  a message router packet and sends the same to LTE RRC.

PARAMETERS
  deact_tmgi_req_ptr - Pointer to TMGI deactivate request structure
  subs_id            - Subscription Id

DEPENDENCIES  
  None

RETURN VALUE
  TRUE - if sending the message was successful
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_rrcif_deactivate_tmgi
(
  ps_iface_embms_tmgi_deact_req_type *deact_tmgi_req_ptr,
  sys_modem_as_id_e_type              subs_id
)
{
  boolean                        ret_val = FALSE;
  lte_rrc_embms_deact_tmgi_req_s deact_tmgi_req_msg;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  /*----------------------------------------------------------------------------
    Memset the message to 0
  ----------------------------------------------------------------------------*/
  memset((void*)&deact_tmgi_req_msg, 0 , sizeof(deact_tmgi_req_msg));

  /*----------------------------------------------------------------------------
    Fill the payload by calling the function that converts the incoming params
    into a format that RRC understands.
  ----------------------------------------------------------------------------*/
  ds_3gpp_embms_rrcif_convert_tmgi_info_ds_to_rrc
    ( 
      &(deact_tmgi_req_ptr->tmgi_session_info),
      &(deact_tmgi_req_msg.session_info)
    );

  deact_tmgi_req_msg.debug_trace_id = deact_tmgi_req_ptr->debug_trace_id;

  ret_val = dsmsgrsnd_per_subs_msg_send
              (LTE_RRC_EMBMS_DEACT_TMGI_REQ, 
		          (msgr_hdr_struct_type*)(&deact_tmgi_req_msg),
               sizeof(lte_rrc_embms_deact_tmgi_req_s),
               SYS_AS_ID_TO_INST_ID(subs_id));

  if(ret_val == FALSE) 
  {
    DS_EMBMS_MSG0_ERROR(" Unable to send message to DSMSGR");
  }

  return ret_val;
} /* ds_3gpp_embms_rrcif_deactivate_tmgi */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_RRCIF_ACTIVATE_TMGI

DESCRIPTION
  This function is used to activate a TMGI. It copies the incoming payload into
  a message router packet and sends the same to LTE RRC.

PARAMETERS
  act_tmgi_req_ptr - Pointer to TMGI activate request structure
  subs_id          - Subscription Id

DEPENDENCIES  
  None

RETURN VALUE
  TRUE - if sending the message was successful
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_rrcif_activate_tmgi
(
  ps_iface_embms_tmgi_act_req_type *act_tmgi_req_ptr,
  sys_modem_as_id_e_type            subs_id
)
{
  boolean                      ret_val = FALSE;
  lte_rrc_embms_act_tmgi_req_s act_tmgi_req_msg;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  /*----------------------------------------------------------------------------
    Memset the message to 0
  ----------------------------------------------------------------------------*/
  memset((void*)&act_tmgi_req_msg, 0 , sizeof(act_tmgi_req_msg));

  /*----------------------------------------------------------------------------
    Fill the payload by calling the function that converts the incoming params
    into a format that RRC understands.
  ----------------------------------------------------------------------------*/
  ds_3gpp_embms_rrcif_convert_tmgi_info_ds_to_rrc
    ( 
      &(act_tmgi_req_ptr->tmgi_session_info),
      &(act_tmgi_req_msg.session_info)
    );

  act_tmgi_req_msg.debug_trace_id = act_tmgi_req_ptr->debug_trace_id;
  act_tmgi_req_msg.preempt_priority = act_tmgi_req_ptr->preempt_priority;
  act_tmgi_req_msg.num_earfcn_to_search = 
      (act_tmgi_req_ptr->num_earfcn_to_search < 
          PS_IFACE_EMBMS_MAX_NUM_FREQUENCY_TO_SRCH) ? 
                         act_tmgi_req_ptr->num_earfcn_to_search :
                                PS_IFACE_EMBMS_MAX_NUM_FREQUENCY_TO_SRCH;

  memscpy(act_tmgi_req_msg.earfcn_list, 
  	  sizeof (ps_iface_earfcn_type) * act_tmgi_req_msg.num_earfcn_to_search,
  	  act_tmgi_req_ptr->earfcn_list,
          sizeof (ps_iface_earfcn_type) * act_tmgi_req_msg.num_earfcn_to_search);

  act_tmgi_req_msg.num_sai_to_search = 
    (act_tmgi_req_ptr->num_sai_to_search < 
          PS_IFACE_EMBMS_MAX_NUM_SAI_TO_SRCH) ? 
                         act_tmgi_req_ptr->num_sai_to_search :
                                PS_IFACE_EMBMS_MAX_NUM_SAI_TO_SRCH;
  memscpy(act_tmgi_req_msg.sai_list, 
          sizeof (ps_iface_embms_sai_type) * PS_IFACE_EMBMS_MAX_NUM_SAI_TO_SRCH,
          act_tmgi_req_ptr->sai_list,
          sizeof (ps_iface_embms_sai_type) * act_tmgi_req_msg.num_sai_to_search);

  ret_val = dsmsgrsnd_per_subs_msg_send
              (LTE_RRC_EMBMS_ACT_TMGI_REQ, 
		          (msgr_hdr_struct_type*)(&act_tmgi_req_msg),
               sizeof(lte_rrc_embms_act_tmgi_req_s),
               SYS_AS_ID_TO_INST_ID(subs_id));

  if(ret_val == FALSE) 
  {
    DS_EMBMS_MSG0_ERROR(" Unable to send message to DSMSGR");
  }

  return ret_val;
} /* ds_3gpp_embms_rrcif_activate_tmgi */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_RRCIF_ACTIVATE_DEACTIVATE_TMGI

DESCRIPTION
  This function is used to simultaneously activate/ deactivate a TMGI. TMGI to be
  activated must not have been activated before. TMGI to be deactivated must be
  activated before. 
  A message router packet is populated and sent to LTE RRC.

PARAMETERS
  tmgi_req_ptr - Pointer to the TMGI 
  subs_id          - Subscription Id

DEPENDENCIES  
  None

RETURN VALUE
  TRUE - if sending the message was successful
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_rrcif_act_deact_tmgi
(
  ps_iface_embms_tmgi_act_deact_req_type *tmgi_req_ptr,
  sys_modem_as_id_e_type                  subs_id
)
{
  boolean                      ret_val = FALSE;
  lte_rrc_embms_act_deact_tmgi_req_s tmgi_req_msg;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */
  /*----------------------------------------------------------------------------
    Memset the message to 0
  ----------------------------------------------------------------------------*/
  memset((void*)&tmgi_req_msg, 0 , sizeof(tmgi_req_msg));

  /*----------------------------------------------------------------------------
    Fill the payload by calling the function that converts the incoming params
    into a format that RRC understands.
  ----------------------------------------------------------------------------*/
  ds_3gpp_embms_rrcif_convert_tmgi_info_ds_to_rrc
    ( 
      &(tmgi_req_ptr->act_tmgi_session_info),
      &(tmgi_req_msg.act_session_info)
    );
  ds_3gpp_embms_rrcif_convert_tmgi_info_ds_to_rrc
    ( 
      &(tmgi_req_ptr->deact_tmgi_session_info),
      &(tmgi_req_msg.deact_session_info)
    );

  tmgi_req_msg.debug_trace_id = tmgi_req_ptr->debug_trace_id;
  tmgi_req_msg.preempt_priority = tmgi_req_ptr->preempt_priority;
  tmgi_req_msg.num_earfcn_to_search = tmgi_req_ptr->num_earfcn_to_search;

  /*----------------------------------------------------------------------------
     P1 KW Fix: Replacing memcpy to memscpy to handle the possibility
     of buffer overflow
  ----------------------------------------------------------------------------*/
  if (tmgi_req_ptr->num_earfcn_to_search > PS_IFACE_EMBMS_MAX_NUM_FREQUENCY_TO_SRCH ) 
  {
     DS_EMBMS_MSG0_ERROR(" tmgi_req_ptr->num_earfcn_to_search is greater "
                         "than PS_IFACE_EMBMS_MAX_NUM_FREQUENCY_TO_SRCH");
     ret_val = FALSE;
     goto exit_function;
  }
  memscpy(tmgi_req_msg.earfcn_list,
          sizeof (ps_iface_earfcn_type)*PS_IFACE_EMBMS_MAX_NUM_FREQUENCY_TO_SRCH,
          tmgi_req_ptr->earfcn_list,
          sizeof (ps_iface_earfcn_type)* tmgi_req_ptr->num_earfcn_to_search);

  tmgi_req_msg.num_sai_to_search = tmgi_req_ptr->num_sai_to_search;
  if (tmgi_req_ptr->num_sai_to_search > PS_IFACE_EMBMS_MAX_NUM_FREQUENCY_TO_SRCH ) 
  {
     DS_EMBMS_MSG0_ERROR(" tmgi_req_ptr->num_sai_to_search is greater "
                         "than PS_IFACE_EMBMS_MAX_NUM_FREQUENCY_TO_SRCH");
     ret_val = FALSE;
     goto exit_function;
  }


  memscpy(tmgi_req_msg.act_sai_list,
          sizeof (ps_iface_embms_sai_type)*PS_IFACE_EMBMS_MAX_NUM_FREQUENCY_TO_SRCH,
          tmgi_req_ptr->sai_list,
          sizeof (ps_iface_embms_sai_type)* tmgi_req_ptr->num_sai_to_search);

  ret_val = dsmsgrsnd_per_subs_msg_send
              (LTE_RRC_EMBMS_ACT_DEACT_TMGI_REQ, 
		          (msgr_hdr_struct_type*)(&tmgi_req_msg),
               sizeof(lte_rrc_embms_act_deact_tmgi_req_s),
               SYS_AS_ID_TO_INST_ID(subs_id));

exit_function:
  if(ret_val == FALSE) 
  {
    DS_EMBMS_MSG0_ERROR(" Unable to send message to DSMSGR");
  }

  return ret_val;
} /* ds_3gpp_embms_rrcif_act_deact_tmgi */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_RRCIF_MAP_CAUSE_CODE

DESCRIPTION
  This function is used to map the deactivation cause code provided by LTE RRC
  into PS error code.

PARAMETERS
  cause_code     - Cause code specified by LTE RRC. This can be of type
                   lte_embms_status_e (or) lte_errno_e (or)
                   lte_rrc_embms_deact_reason_e
  cause_category - This specifies the Success (or) Failure categoies.


DEPENDENCIES  
  None

RETURN VALUE
  ps_iface_embms_tmgi_event_info_code_type based on incoming cause.
  IFACE_EMBMS_TMGI_EVENT_INFO_CODE_INVALID is when we are unable to map

SIDE EFFECTS  
  None
==============================================================================*/
LOCAL ps_iface_embms_tmgi_event_info_code_type 
                                        ds_3gpp_embms_rrcif_map_cause_code
(
  void        *cause_code,
  int         cause_category       
)
{
  ps_iface_embms_tmgi_event_info_code_type ps_status_code = 
                                      IFACE_EMBMS_TMGI_EVENT_INFO_CODE_INVALID;
  lte_embms_status_e                    success_status_code;
  lte_embms_status_e                    generic_error_code;
  lte_rrc_embms_deact_reason_e          embms_specific_cause_code;
  lte_rrc_embms_warn_reason_e           warning_reason_code;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  /*----------------------------------------------------------------------------
    Switch based on the cause category and then based on the incoming cause.
    Please refer to the cause codes defined in lte_rrc_embms_act_tmgi_cnf_s,
    lte_rrc_embms_deact_tmgi_cnf_s and lte_rrc_embms_deact_tmgi_ind_s
  ----------------------------------------------------------------------------*/
  switch(cause_category)
  {
  case PS_IFACE_EMBMS_CAUSE_CATEGORY_0:
      success_status_code = (lte_embms_status_e)cause_code;
      switch (success_status_code)
      {
      case LTE_EMBMS_SUCCESS_ALREADY_DONE:
          ps_status_code = IFACE_EMBMS_TMGI_ACTIVATION_SUCCESS_INFO_DUPLICATE;
          break;
      case LTE_EMBMS_SUCCESS_IDLE_RADIO_TUNE:
          ps_status_code = IFACE_EMBMS_TMGI_ACTIVATION_SUCCESS_IDLE_RADIO_TUNE;
          break;
      case LTE_EMBMS_SUCCESS_CONN_RADIO_TUNE:
         ps_status_code = IFACE_EMBMS_TMGI_ACTIVATION_SUCCESS_CONN_RADIO_TUNE;
          break;
      default:
      DS_EMBMS_MSG1_LOW("Incoming generic success code: %d", 
                       success_status_code);
          ps_status_code = IFACE_EMBMS_TMGI_ACTIVATION_SUCCESS;
          break;

      }
      break;
  case PS_IFACE_EMBMS_CAUSE_CATEGORY_1:
    /*--------------------------------------------------------------------------
      Convert the incoming void pointer to the generic cause code
    --------------------------------------------------------------------------*/
    generic_error_code = (lte_embms_status_e)cause_code;

    switch(generic_error_code)
    {
    case LTE_EMBMS_FAIL_UNKNOWN:
      ps_status_code = IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_UNKNOWN;
      break;
    
    case LTE_EMBMS_FAIL_NOT_ALLOWED:
      ps_status_code = IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_NOT_ALLOWED;
      break;

    case LTE_EMBMS_FAIL_MISSING_CTRL_INFO:
      ps_status_code = IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_MISSING_CONTROL_INFO;
      break;

    case LTE_EMBMS_FAIL_TMGI_NOT_FOUND:
      ps_status_code = IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_MISSING_TMGI;
      break;

    case LTE_EMBMS_FAIL_MCAST_OOC:
      ps_status_code = IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_MULTICAST_OOS;
      break;

    case LTE_EMBMS_FAIL_UCAST_OOS:
      ps_status_code = IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_UNICAST_OOS;
      break;

    case LTE_EMBMS_FAIL_FREQ_CONFLICT:
      ps_status_code = IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_CAMPED_ON_OTHER_FREQ;
      break;

    default:
      DS_EMBMS_MSG1_LOW("Incoming generic error code: %d", 
                       generic_error_code);
      ps_status_code = IFACE_EMBMS_TMGI_ACTIVATION_FAILURE_UNKNOWN;
      break;
    } /* switch based on incoming error code */
    break;

  case PS_IFACE_EMBMS_CAUSE_CATEGORY_2:
    ps_status_code = IFACE_EMBMS_TMGI_DEACTIVATION_CLIENT_INITIATED;
    break;

  case PS_IFACE_EMBMS_CAUSE_CATEGORY_3:
    /*--------------------------------------------------------------------------
      Convert the incoming void pointer to the generic cause code
    --------------------------------------------------------------------------*/
    embms_specific_cause_code = (lte_rrc_embms_deact_reason_e)cause_code;
    switch(embms_specific_cause_code)
    {
    case LTE_RRC_EMBMS_DEACT_LTE_STOPPED:
      ps_status_code = IFACE_EMBMS_TMGI_DEACTIVATION_LTE_STOPPED;
      break;

    case LTE_RRC_EMBMS_DEACT_SESSION_END:
      ps_status_code = IFACE_EMBMS_TMGI_DEACTIVATION_SESSION_END;
      break;

    case LTE_RRC_EMBMS_DEACT_OUT_OF_COVERAGE:
      ps_status_code = IFACE_EMBMS_TMGI_DEACTIVATION_OUT_OF_COVERAGE;
      break;

    default:
      DS_EMBMS_MSG1_HIGH("Incoming embms specific error code: %d", 
                       embms_specific_cause_code);
      break;
    } /* switch based on incoming error code */
    break;

  case PS_IFACE_EMBMS_CAUSE_CATEGORY_4:
    warning_reason_code = (lte_rrc_embms_warn_reason_e)cause_code;
    switch(warning_reason_code)
    {
    case LTE_RRC_EMBMS_WARN_REASON_UCAST_OOS:
      ps_status_code = IFACE_EMBMS_TMGI_WARNING_UNICAST_OOS;
      break;
    case LTE_RRC_EMBMS_WARN_REASON_MCAST_OOS:
      ps_status_code = IFACE_EMBMS_TMGI_WARNING_MULITCAST_OOS;
      break;
    case LTE_RRC_EMBMS_WARN_REASON_OOS_CLEARED:
      ps_status_code = IFACE_EMBMS_TMGI_WARNING_OOS_CLEARED;
      break;
    default:
      DS_EMBMS_MSG1_HIGH("Incoming embms specific error code: %d", 
                       warning_reason_code);
    }
    break;

  default:
    DS_EMBMS_MSG1_LOW("Invalid incoming cause category. Category: %d", 
                     cause_category);
    
  } /* switch based on cause category */

  return ps_status_code;
} /* ds_3gpp_embms_rrcif_map_cause_code */

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_ACT_TMGI_LIST_MSG_HDLR

DESCRIPTION
  This function is used to handle the LTE_RRC_EMBMS_ACT_TMGI_LIST_IND message
  from LTE RRC

  Currently this function dispatches the indication to every client - in other
  words, if this is a response to a request from a particular client, it is
  still broadcast to all clients. This behaviour should be changed such that we
  send the indication only to the requestor if this is a solicited response.

  If this if an unsolicited response, it will be sent to every client.
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_act_tmgi_list_msg_hdlr
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
)
{
  boolean                                    ret_val = FALSE;
  lte_rrc_embms_act_tmgi_list_ind_s          *msg_ptr = NULL;
  ps_iface_embms_tmgi_list_info_type   ind_info;
  boolean                                    send_indication_flag = TRUE;
  ps_iface_embms_tmgi_type                   tmgi_info;
  uint8                                      loop_index = 0;
  sys_modem_as_id_e_type                     subs_id = SYS_MODEM_AS_ID_NONE;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  /*----------------------------------------------------------------------------
    Since there is no operation with the TMGI table, we need not acquire the 
    crit section. We allocate memory for the TMGI list and send out the ind.
  ----------------------------------------------------------------------------*/
  if((msgrtype == LTE_RRC_EMBMS_ACT_TMGI_LIST_IND) && (dsmsg_ptr != NULL))
  {
    /*--------------------------------------------------------------------------
      Set return value to TRUE as we need to fail this API only if the above
      conditions fail.
    --------------------------------------------------------------------------*/
    ret_val = TRUE;

    /*--------------------------------------------------------------------------
      Typecast the incoming payload into the correct type.
    --------------------------------------------------------------------------*/
    msg_ptr = (lte_rrc_embms_act_tmgi_list_ind_s*)dsmsg_ptr;

    subs_id = INST_ID_TO_SYS_AS_ID(msg_ptr->msg_hdr.inst_id);

    DS_EMBMS_MSG1_HIGH("ACT TMGI LIST IND processing, debug_trace_id: %d",
                       msg_ptr->debug_trace_id);
    /*--------------------------------------------------------------------------
      Get the number of active TMGIs and ensure that the value is right.
    --------------------------------------------------------------------------*/
    ind_info.tmgi_list.num_tmgi = msg_ptr->num_active_embms_sessions;
    ind_info.debug_trace_id = msg_ptr->debug_trace_id;
    ASSERT(DS_3GPP_EMBMS_RRCIF_VALIDATE_NUM_ACT_EMBMS_SESSIONS(
                                                  ind_info.tmgi_list.num_tmgi));

    DS_EMBMS_MSG1_HIGH("Num active TMGIs: %d", ind_info.tmgi_list.num_tmgi);

    /*--------------------------------------------------------------------------
      Set the client ID to Reserved client id. This will result in a broadcast
      of this indication. When the LTE RRC interface is changed to allow the 
      passing of the opaque handle, we can set the client id to the one that we
      get in the message
    --------------------------------------------------------------------------*/
    ind_info.client_id = (void*)PS_IFACE_EMBMS_RESERVED_CLIENT_ID;

    /*--------------------------------------------------------------------------
      Set the info code as invalid as we do not expect to pass any for this
      indication
    --------------------------------------------------------------------------*/
    ind_info.info_code = IFACE_EMBMS_TMGI_EVENT_INFO_CODE_INVALID;

    /*--------------------------------------------------------------------------
      Set the pointer to the TMGI list to NULL.
    --------------------------------------------------------------------------*/
    ind_info.tmgi_list.list = NULL;

    /*--------------------------------------------------------------------------
      Allocate memory if the number of tmgi in the active list is greater than 0
    --------------------------------------------------------------------------*/
    if(ind_info.tmgi_list.num_tmgi > 0)
    {
      ind_info.tmgi_list.list = (ps_iface_embms_tmgi_type*)
             modem_mem_alloc(ind_info.tmgi_list.num_tmgi*sizeof(ps_iface_embms_tmgi_type),
                             MODEM_MEM_CLIENT_DATA);

      if(ind_info.tmgi_list.list != NULL)
      {
        for(loop_index = 0; loop_index < ind_info.tmgi_list.num_tmgi; loop_index++) 
        {
          /*--------------------------------------------------------------------
            Memset the stack variable that will be used to hold the TMGI in the
            format that is used to send indication. The contents of this will
            be copied into the memory allocated.

            This process will be repeated for every TMGI in the active list.
          --------------------------------------------------------------------*/
          memset((void*)&tmgi_info, 0, sizeof(ps_iface_embms_tmgi_type));
          tmgi_info.session_id = PS_IFACE_EMBMS_INVALID_SESSION_ID;

          ds_3gpp_embms_rrcif_convert_tmgi_info_rrc_to_ds(
                              &(msg_ptr->session_info[loop_index].session_info),
                              &tmgi_info);

          /*--------------------------------------------------------------------
            Copy the contents of the stack variable into the memory allocated.
            Copying will have to happen based on the loop_index
          --------------------------------------------------------------------*/
          memscpy((void*)(ind_info.tmgi_list.list+loop_index),
                  sizeof(ps_iface_embms_tmgi_type),
                  (void*)&tmgi_info, sizeof(ps_iface_embms_tmgi_type));

        } /* loop over all TMGIs in active list */
      } /* Memory allocation succeeded */
      else
      {
        send_indication_flag = FALSE;

        DS_EMBMS_MSG1_ERROR("Modem heap alloc failed. Block Size: %d. "
                           "No Act TMGI list ind",
                  ind_info.tmgi_list.num_tmgi*sizeof(ps_iface_embms_tmgi_type));
      } /* Memory allocation failed */

    } /* Num TMGI greater than zero */
    else
    {
      DS_EMBMS_MSG0_HIGH("Num Active TMGI is zero. Sending indication");
    }

    if(send_indication_flag == TRUE)
    {

      /*----------------------------------------------------------------------
        Send the indication
      ----------------------------------------------------------------------*/
      ds_3gpp_embms_send_ind(IFACE_EMBMS_ACTIVE_TMGI_LIST_EV,
                             (void *)&ind_info, subs_id);

      if(ind_info.tmgi_list.list != NULL)
      {
        /*----------------------------------------------------------------------
          Free the memory that was use for the ACT TMGI list
        ----------------------------------------------------------------------*/
        modem_mem_free((void*)ind_info.tmgi_list.list,MODEM_MEM_CLIENT_DATA);
      }
    }
  } /* input params valid */
  else
  {
    DS_EMBMS_MSG0_ERROR("Active TMGI LIST IND - Invalid input params");
  }
  return ret_val;
} /* ds_3gpp_embms_rrcif_act_tmgi_list_msg_hdlr */

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_AVAIL_SAI_LIST_MSG_HDLR

DESCRIPTION
  This function is used to handle the LTE_RRC_EMBMS_AVAIL_SAI_LIST_IND message
  from LTE RRC

  Currently this function dispatches the indication to every client - in other
  words, if this is a response to a request from a particular client, it is
  still broadcast to all clients. This behaviour should be changed such that we
  send the indication only to the requestor if this is a solicited response.

  If this if an unsolicited response, it will be sent to every client.
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_avail_sai_list_msg_hdlr
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
)
{
  boolean                                    ret_val = FALSE;
  lte_rrc_embms_avail_sai_list_ind_s         *msg_ptr = NULL;
  ps_iface_embms_sai_info_type               ind_info;  
  uint8                                      freq_cnt = 0;
  rex_crit_sect_type                         *crit_section_ptr = NULL;
  void                                       *client_id = NULL;
  msgr_attach_struct_type                    *att_ptr = NULL;
  dsm_item_type                              *dsm_item_ptr = NULL;
  uint32                                     dsm_pkt_len;
  uint32                                     dsm_item_size;
  uint8                                       index = 0;
  sys_modem_as_id_e_type                     subs_id = SYS_MODEM_AS_ID_NONE;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  if((msgrtype != LTE_RRC_EMBMS_AVAIL_SAI_LIST_IND) || (dsmsg_ptr == NULL))
  {
    DS_EMBMS_MSG0_ERROR("Avail AVAILABLE SAI LIST IND - Invalid input params");
    return ret_val;
  }

  /*----------------------------------------------------------------------------
    Since we are dealing with a list of clients that have requested the info
    about avail tmgi list and we have to generate indications to them, we want
    the list to be consistent while we are processing this from 
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  /*--------------------------------------------------------------------------
    Typecast the incoming payload into the correct type.
  --------------------------------------------------------------------------*/
  msg_ptr = (lte_rrc_embms_avail_sai_list_ind_s*)dsmsg_ptr;

  subs_id = INST_ID_TO_SYS_AS_ID(msg_ptr->msg_hdr.inst_id);

  DS_EMBMS_MSG1_HIGH("AVAILABLE SAI LIST IND processing, debug_trace_id: %d",
                     msg_ptr->debug_trace_id);

 /*--------------------------------------------------------------------------
    Memset the indication info
  --------------------------------------------------------------------------*/
  memset((void *)&ind_info, 0, sizeof(ps_iface_embms_sai_info_type));

  /*--------------------------------------------------------------------------
    Get the number of available SAIs and ensure that the value is right.
  --------------------------------------------------------------------------*/
  // Get number of frequencies
  ind_info.debug_trace_id = msg_ptr->debug_trace_id;
   
  if((client_id = ds_3gpp_embms_get_avail_sai_list_client()) == NULL)
  {
    /*------------------------------------------------------------------------
      There is no client expecting a response from RRC. Set the client if so
      that this indication is broadcast to all clients
    ------------------------------------------------------------------------*/
  ind_info.client_id = (void*)PS_IFACE_EMBMS_RESERVED_CLIENT_ID;

  }
  else
  {
    /*------------------------------------------------------------------------
      We have a client that is expecting a response for the Available TMGI
      list IOCTL. Set the client id in the indication and reset our info about
      the client
    ------------------------------------------------------------------------*/
    ind_info.client_id = client_id;
    ds_3gpp_embms_reset_avail_sai_list_client();
  }
  
  DS_EMBMS_MSG1_HIGH("SAI AVAL LIST IND: Client:%d", ind_info.client_id);


  /*--------------------------------------------------------------------------
     Dummy do while {} loop. Break to end of loop on error
  --------------------------------------------------------------------------*/
  do
  {
    /*--------------------------------------------------------------------------
     Get the DSM item ptr from RRC payload
    --------------------------------------------------------------------------*/ 
    att_ptr = msgr_get_attach(&(msg_ptr->msg_hdr), 0);
    if(att_ptr == NULL)
    {
      DS_EMBMS_MSG0_ERROR("Att_ptr is NULL in RRC TMGI AVAL LIST IND");
      ret_val = FALSE;
      break;
    }

    msgr_get_dsm_attach(att_ptr, &dsm_item_ptr);
    
    dsm_pkt_len = dsm_length_packet(dsm_item_ptr); 
    if(dsm_pkt_len == 0)
    {
      DS_EMBMS_MSG0_ERROR("DSM packet len is 0 in RRC SAI AVAL LIST IND");
      ret_val = FALSE;
      break;
    }

    /* Read num_freq_sai_info */
    dsm_item_size = dsm_pullup(&dsm_item_ptr, &ind_info.num_frequencies, 
                                         sizeof(ind_info.num_frequencies));
    
    DS_EMBMS_MSG1_HIGH("SAI AVAL LIST IND: num_freq:%d", 
                       ind_info.num_frequencies);
    /*Allocate memory for the array of ps_iface_embms_frequency_sai_info */
    ind_info.freq_sai_info = DS_3GPP_MEM_ALLOC(ind_info.num_frequencies
                                  * sizeof(ps_iface_embms_frequency_sai_info));

    if (ind_info.freq_sai_info == NULL)
    {
      DS_EMBMS_MSG0_ERROR("zero num_freq (or)Memory alloc failure in "
                          "processing RRC SAI AVAL LIST IND");
      ret_val = FALSE;
      break;
    }

    /* Now fill the list of SAIs for each frequency */    
    for(freq_cnt = 0; (freq_cnt < ind_info.num_frequencies)
                                && (dsm_item_ptr != NULL); freq_cnt++) 
    {     

      /* Read freq field */
      if((dsm_item_size = dsm_pullup(&dsm_item_ptr, 
                                 &ind_info.freq_sai_info[freq_cnt].frequency, 
                                 sizeof(ind_info.freq_sai_info[freq_cnt].frequency)))
                      != sizeof(ind_info.freq_sai_info[freq_cnt].frequency))
      {
        DS_EMBMS_MSG0_ERROR("DSM pullup error in reading frequency field "
                          "RRC SAI AVAL LIST IND");
        ret_val = FALSE;
        break;

      }
      else
      {
        DS_EMBMS_MSG1_HIGH("SAI AVAL LIST IND: frequency %d",
                             ind_info.freq_sai_info[freq_cnt].frequency);
      }

      /* Read is_serving_freq field */
      if((dsm_item_size = dsm_pullup(&dsm_item_ptr, 
                                 &ind_info.freq_sai_info[freq_cnt].is_serving_frequency, 
                                 sizeof(ind_info.freq_sai_info[freq_cnt].is_serving_frequency))) 
         != sizeof(ind_info.freq_sai_info[freq_cnt].is_serving_frequency))
      {
        DS_EMBMS_MSG0_ERROR("DSM pullup error in reading is_serving_freq "
                          "RRC SAI AVAL LIST IND");
        ret_val = FALSE;
        break;
      }
      else
      {
        DS_EMBMS_MSG1_HIGH("SAI AVAL LIST IND: is_serving_freq %d",
                            ind_info.freq_sai_info[freq_cnt].is_serving_frequency);
      }

      /* Get the number of SAIs for this frequency */
      if((dsm_item_size = dsm_pullup(&dsm_item_ptr, 
                                 &ind_info.freq_sai_info[freq_cnt].num_sais, 
                                 sizeof(ind_info.freq_sai_info[freq_cnt].num_sais)))
                       != sizeof(ind_info.freq_sai_info[freq_cnt].num_sais))
      {
        DS_EMBMS_MSG0_ERROR("DSM pullup error in reading num_sais "
                          "RRC SAI AVAL LIST IND");
        ret_val = FALSE;
        break;
      }
      else
      {
        DS_EMBMS_MSG1_HIGH("SAI AVAL LIST IND: num_sai's %d",
                             ind_info.freq_sai_info[freq_cnt].num_sais);
      }

      if((dsm_item_size = dsm_pullup(&dsm_item_ptr, ind_info.freq_sai_info[freq_cnt].sai_list, 
              ind_info.freq_sai_info[freq_cnt].num_sais *sizeof(ps_iface_embms_sai_type)))
            != ind_info.freq_sai_info[freq_cnt].num_sais *sizeof(ps_iface_embms_sai_type))
      {
        DS_EMBMS_MSG0_ERROR("DSM pullup error in reading sai_list"
                          "RRC SAI AVAL LIST IND");
        ret_val = FALSE;
        break;

      }
      else
      {

        /*---------------------------------------------------------------------
          Debug to print SAI's. Remove if not reqd later
          ---------------------------------------------------------------------*/
        DS_EMBMS_MSG0_HIGH("SAI AVAL LIST IND: freq_sai_info");
        for(index=0;index<ind_info.freq_sai_info[freq_cnt].num_sais;index++)
        {
          DS_EMBMS_MSG1_HIGH("%d",ind_info.freq_sai_info[freq_cnt].sai_list[index]);
        }
      }
    } /* End for*/
    ret_val = TRUE;
  } while (0); /* end do while */


    /*----------------------------------------------------------------------
     Send the indication
    ----------------------------------------------------------------------*/
  ds_3gpp_embms_send_ind
    (IFACE_EMBMS_AVAILABLE_SAI_LIST_EV,(void *)&ind_info, subs_id);

  /* cannot just return here. cleanup any memory we allocated */
  if (ind_info.freq_sai_info != NULL)
  {
    DS_3GPP_MEM_FREE(ind_info.freq_sai_info);
  }

  if(dsm_item_ptr != NULL)
  {
    /* Discard any extra dsm items */
    dsm_free_packet(&dsm_item_ptr);
  }

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr); 
  return ret_val;
} /* ds_3gpp_embms_rrcif_avail_sai_list_msg_hdlr */

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_AVAIL_TMGI_LIST_MSG_HDLR

DESCRIPTION
  This function is used to handle the LTE_RRC_EMBMS_AVAIL_TMGI_LIST_IND message
  from LTE RRC

  Currently this function dispatches the indication to every client - in other
  words, if this is a response to a request from a particular client, it is
  still broadcast to all clients. This behaviour should be changed such that we
  send the indication only to the requestor if this is a solicited response.

  If this if an unsolicited response, it will be sent to every client.
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_avail_tmgi_list_msg_hdlr
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
)
{
  boolean                                    ret_val = FALSE;
  lte_rrc_embms_avail_tmgi_list_ind_s        *msg_ptr = NULL;
  lte_rrc_embms_session_info_s               tmgi_session_info;
  ps_iface_embms_tmgi_list_info_type         ind_info;
  ps_iface_embms_tmgi_type                   tmgi_info;
  uint8                                      cnt = 0;
  rex_crit_sect_type                         *crit_section_ptr = NULL;
  void                                       *client_id = NULL;
  msgr_attach_struct_type                    *att_ptr = NULL;
  dsm_item_type                              *dsm_item_ptr = NULL;
  uint32                                     dsm_pkt_len;
  uint32                                     dsm_item_size;
  sys_modem_as_id_e_type                     subs_id = SYS_MODEM_AS_ID_NONE;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  if((msgrtype != LTE_RRC_EMBMS_AVAIL_TMGI_LIST_IND) || (dsmsg_ptr == NULL))
  {
    DS_EMBMS_MSG0_ERROR("Avail AVAILABLE TMGI LIST IND - Invalid input params");
    return ret_val;
  }

  /*----------------------------------------------------------------------------
    Since we are dealing with a list of clients that have requested the info
    about avail tmgi list and we have to generate indications to them, we want
    the list to be consistent while we are processing this from 
  ----------------------------------------------------------------------------*/
  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  /*--------------------------------------------------------------------------
    Typecast the incoming payload into the correct type.
  --------------------------------------------------------------------------*/
  msg_ptr = (lte_rrc_embms_avail_tmgi_list_ind_s*)dsmsg_ptr;

  subs_id = INST_ID_TO_SYS_AS_ID(msg_ptr->msg_hdr.inst_id);

  DS_EMBMS_MSG1_HIGH("AVAILABLE TMGI LIST IND processing, debug_trace_id: %d",
                     msg_ptr->debug_trace_id);
  /*--------------------------------------------------------------------------
    Get the number of active TMGIs and ensure that the value is right.
  --------------------------------------------------------------------------*/
  ind_info.tmgi_list.num_tmgi = msg_ptr->num_available_embms_sessions;
  ind_info.debug_trace_id = msg_ptr->debug_trace_id;

  ASSERT(DS_3GPP_EMBMS_RRCIF_VALIDATE_NUM_AVAIL_EMBMS_SESSIONS(
                                                       ind_info.tmgi_list.num_tmgi));

  DS_EMBMS_MSG1_HIGH("Num avail TMGIs: %d", ind_info.tmgi_list.num_tmgi);

  if((client_id = ds_3gpp_embms_get_avail_tmgi_list_client()) == NULL)
  {
    /*------------------------------------------------------------------------
      There is no client expecting a response from RRC. Set the client if so
      that this indication is broadcast to all clients
    ------------------------------------------------------------------------*/
  ind_info.client_id = (void*)PS_IFACE_EMBMS_RESERVED_CLIENT_ID;

  }
  else
  {
    /*------------------------------------------------------------------------
      We have a client that is expecting a response for the Available TMGI
      list IOCTL. Set the client id in the indication and reset our info about
      the client
    ------------------------------------------------------------------------*/
    ind_info.client_id = client_id;
    ds_3gpp_embms_reset_avail_list_tmgi_client();
  }
  /*--------------------------------------------------------------------------
    Set the info code as invalid as we do not expect to pass any for this
    indication
  --------------------------------------------------------------------------*/
  ind_info.info_code = IFACE_EMBMS_TMGI_EVENT_INFO_CODE_INVALID;

  /*--------------------------------------------------------------------------
    Set the pointer to the TMGI list to NULL.
  --------------------------------------------------------------------------*/
  ind_info.tmgi_list.list = NULL;

  /*--------------------------------------------------------------------------
     Dummy do while {} loop. Break to end of loop on error
  --------------------------------------------------------------------------*/
  do
  {
    /*--------------------------------------------------------------------------
      Allocate memory if the number of tmgi in the active list is greater than 0
    --------------------------------------------------------------------------*/
    if(ind_info.tmgi_list.num_tmgi > 0)
    {
  
      /*--------------------------------------------------------------------------
       Get the DSM item ptr from RRC payload
      --------------------------------------------------------------------------*/ 
      att_ptr = msgr_get_attach(&(msg_ptr->msg_hdr), 0);
      if(att_ptr == NULL)
      {
         DS_EMBMS_MSG0_ERROR("Att_ptr is NULL in RRC TMGI AVAL LIST IND");
        break;
      }
      msgr_get_dsm_attach(att_ptr, &dsm_item_ptr);
      
      /*--------------------------------------------------------------------------
       Verify RRC payload has all the TMGIs
      --------------------------------------------------------------------------*/
      dsm_pkt_len = dsm_length_packet(dsm_item_ptr); 
      if(dsm_pkt_len == 0)
      {
        DS_EMBMS_MSG0_ERROR("DSM packet len is 0 in RRC TMGI AVAL LIST IND");
        break;
      }
      if (dsm_pkt_len != (ind_info.tmgi_list.num_tmgi * 
                               sizeof(lte_rrc_embms_session_info_s)))
      {
        DS_EMBMS_MSG2_ERROR("DSM packet len (%d) does not match with number of"
                               " TMGIs (%d) in RRC payload", 
                               dsm_pkt_len, ind_info.tmgi_list.num_tmgi);
      }

      /*--------------------------------------------------------------------------
       Allocate memory for TMGI list
      --------------------------------------------------------------------------*/
      ind_info.tmgi_list.list = (ps_iface_embms_tmgi_type*)
                                  modem_mem_alloc((ind_info.tmgi_list.num_tmgi* 
                                          sizeof(ps_iface_embms_tmgi_type)),
                           MODEM_MEM_CLIENT_DATA);
      if (ind_info.tmgi_list.list == NULL)
      {
        DS_EMBMS_MSG1_ERROR("Modem heap alloc failed. Block Size: %d. "
                           "No Avail TMGI list ind",
                           ind_info.tmgi_list.num_tmgi*sizeof(ps_iface_embms_tmgi_type));
        break;
      }
  
      /*-------------------------------------------------------------------- 
        Copy over TMGIs from DSM list to Avail TMGI list ind message
      --------------------------------------------------------------------*/ 
      for(cnt = 0; (cnt < ind_info.tmgi_list.num_tmgi)
                                  && (dsm_item_ptr != NULL); cnt++) 
      {
       memset((void*)&tmgi_info, 0, sizeof(ps_iface_embms_tmgi_type));

       tmgi_info.session_id = PS_IFACE_EMBMS_INVALID_SESSION_ID;
      
       dsm_item_size = dsm_pullup(&dsm_item_ptr, &tmgi_session_info, 
                                           sizeof(tmgi_session_info));
      
       if(dsm_item_size == sizeof(tmgi_session_info))
       { 
         ds_3gpp_embms_rrcif_convert_tmgi_info_rrc_to_ds(
                                 &tmgi_session_info, &tmgi_info);
      
         memscpy((void*)(ind_info.tmgi_list.list+cnt),
                 sizeof(ps_iface_embms_tmgi_type),
                 (void*)&tmgi_info, sizeof(ps_iface_embms_tmgi_type));
       }
       else
       {
         DS_EMBMS_MSG3_ERROR("DSM item size (%d)does not match with size of"
                            " TMGI (%d) at index (%d)", dsm_item_size, 
                            sizeof(tmgi_session_info), cnt);
       }     
      } /* loop over all TMGIs in active list */
  
      /* Discard any extra dsm items */
      dsm_free_packet(&dsm_item_ptr);
    } /* Num TMGI greater than zero */
    else
    {
      DS_EMBMS_MSG0_HIGH("Num Avail TMGI is zero. Sending indication");
    }
    ret_val = TRUE;    
  } while (0); /* end do while */
    
    /*----------------------------------------------------------------------
     Send the indication
    ----------------------------------------------------------------------*/
    ds_3gpp_embms_send_ind(IFACE_EMBMS_AVAILABLE_TMGI_LIST_EV,
                           (void *)&ind_info, subs_id);

    if(ind_info.tmgi_list.list != NULL)
    {
      /*----------------------------------------------------------------------
        Free the memory that was use for the ACT TMGI list
      ----------------------------------------------------------------------*/
      modem_mem_free((void*)ind_info.tmgi_list.list,MODEM_MEM_CLIENT_DATA);
    }

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);
  return ret_val;
} /* ds_3gpp_embms_rrcif_avail_tmgi_list_msg_hdlr */

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_WARN_TMGI_LIST_MSG_HDLR

DESCRIPTION
  This function is used to handle the LTE_RRC_EMBMS_WARN_TMGI_LIST_IND message
  from LTE RRC

  Currently this function dispatches the indication to every client - in other
  words, if this is a response to a request from a particular client, it is
  still broadcast to all clients. This behaviour should be changed such that we
  send the indication only to the requestor if this is a solicited response.

  If this if an unsolicited response, it will be sent to every client.
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_warn_tmgi_list_msg_hdlr
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
)
{
  boolean                                    ret_val = FALSE;
  lte_rrc_embms_oos_warn_ind_s               *msg_ptr = NULL;
  ps_iface_embms_tmgi_list_info_type         ind_info;
  boolean                                    send_indication_flag = TRUE;
  ps_iface_embms_tmgi_type                   tmgi_info;
  uint8                                      loop_index = 0;
  sys_modem_as_id_e_type                     subs_id = SYS_MODEM_AS_ID_NONE;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  DS_EMBMS_MSG0_HIGH("WARNING TMGI LIST IND processing");

  /*----------------------------------------------------------------------------
    Since there is no operation with the TMGI table, we need not acquire the 
    crit section. We allocate memory for the TMGI list and send out the ind.
  ----------------------------------------------------------------------------*/
  if((msgrtype == LTE_RRC_EMBMS_OOS_WARN_IND) && (dsmsg_ptr != NULL))
  {
    /*--------------------------------------------------------------------------
      Set return value to TRUE as we need to fail this API only if the above
      conditions fail.
    --------------------------------------------------------------------------*/
    ret_val = TRUE;

    /*--------------------------------------------------------------------------
      Typecast the incoming payload into the correct type.
    --------------------------------------------------------------------------*/
    msg_ptr = (lte_rrc_embms_oos_warn_ind_s*)dsmsg_ptr;

    subs_id = INST_ID_TO_SYS_AS_ID(msg_ptr->msg_hdr.inst_id);

    /*--------------------------------------------------------------------------
      Get the number of active TMGIs and ensure that the value is right.
    --------------------------------------------------------------------------*/
    ind_info.tmgi_list.num_tmgi = msg_ptr->num_sessions_affected;

    ASSERT(DS_3GPP_EMBMS_RRCIF_VALIDATE_NUM_ACT_EMBMS_SESSIONS(
                                                  ind_info.tmgi_list.num_tmgi));

    DS_EMBMS_MSG1_HIGH("Num affected TMGIs: %d", ind_info.tmgi_list.num_tmgi);

    /*--------------------------------------------------------------------------
      Set the client ID to Reserved client id. This will result in a broadcast
      of this indication. When the LTE RRC interface is changed to allow the 
      passing of the opaque handle, we can set the client id to the one that we
      get in the message
    --------------------------------------------------------------------------*/
    ind_info.client_id = (void*)PS_IFACE_EMBMS_RESERVED_CLIENT_ID;

    ind_info.debug_trace_id = msg_ptr->debug_trace_id;
    /*--------------------------------------------------------------------------
      Set the warning reason. The warning reason has to be mapped using a
      mapping function.
    --------------------------------------------------------------------------*/
    ind_info.info_code = 
      ds_3gpp_embms_rrcif_map_cause_code((void*)msg_ptr->reason,
                                         PS_IFACE_EMBMS_CAUSE_CATEGORY_4);

    /*--------------------------------------------------------------------------
      Set the pointer to the TMGI list to NULL.
    --------------------------------------------------------------------------*/
    ind_info.tmgi_list.list = NULL;

    /*--------------------------------------------------------------------------
      Allocate memory if the number of tmgi in the active list is greater than 0
    --------------------------------------------------------------------------*/
    if(ind_info.tmgi_list.num_tmgi > 0)
    {
      ind_info.tmgi_list.list = (ps_iface_embms_tmgi_type*)
             modem_mem_alloc(ind_info.tmgi_list.num_tmgi*sizeof(ps_iface_embms_tmgi_type),
                             MODEM_MEM_CLIENT_DATA);

      if(ind_info.tmgi_list.list != NULL)
      {
        for(loop_index = 0; loop_index < ind_info.tmgi_list.num_tmgi; loop_index++) 
        {
          /*--------------------------------------------------------------------
            Memset the stack variable that will be used to hold the TMGI in the
            format that is used to send indication. The contents of this will
            be copied into the memory allocated.

            This process will be repeated for every TMGI in the active list.
          --------------------------------------------------------------------*/
          memset((void*)&tmgi_info, 0, sizeof(ps_iface_embms_tmgi_type));
          tmgi_info.session_id = PS_IFACE_EMBMS_INVALID_SESSION_ID;

          ds_3gpp_embms_rrcif_convert_tmgi_info_rrc_to_ds(
                              &(msg_ptr->affected_session_info[loop_index].session_info),
                              &tmgi_info);

          /*--------------------------------------------------------------------
            Copy the contents of the stack variable into the memory allocated.
            Copying will have to happen based on the loop_index
          --------------------------------------------------------------------*/
          memscpy((void*)(ind_info.tmgi_list.list+loop_index),
                  sizeof(ps_iface_embms_tmgi_type),
                 (void*)&tmgi_info, sizeof(ps_iface_embms_tmgi_type));

        } /* loop over all TMGIs in active list */
      } /* Memory allocation succeeded */
      else
      {
        send_indication_flag = FALSE;

        DS_EMBMS_MSG1_ERROR("Modem heap alloc failed. Block Size: %d. "
                           "No Warn TMGI list ind",
                           ind_info.tmgi_list.num_tmgi *sizeof(ps_iface_embms_tmgi_type));
      } /* Memory allocation failed */

    } /* Num TMGI greater than zero */
    else
    {
      DS_EMBMS_MSG0_HIGH("Num Warning TMGI is zero. Sending indication");
    }

    if(send_indication_flag == TRUE)
    {

      /*----------------------------------------------------------------------
        Send the indication
      ----------------------------------------------------------------------*/
      ds_3gpp_embms_send_ind(IFACE_EMBMS_WARNING_TMGI_LIST_EV,
                            (void *)&ind_info, subs_id);

      if(ind_info.tmgi_list.list!= NULL)
      {
        /*----------------------------------------------------------------------
          Free the memory that was use for the WARN TMGI list
        ----------------------------------------------------------------------*/
        modem_mem_free((void*)ind_info.tmgi_list.list,MODEM_MEM_CLIENT_DATA);
      }
    }
  } /* input params valid */
  else
  {
    DS_EMBMS_MSG0_ERROR("WARNING TMGI LIST IND - Invalid input params");
  }
  return ret_val;
} /* ds_3gpp_embms_rrcif_warn_tmgi_list_msg_hdlr */

/*==============================================================================
FUNCTION  DS_3GPP_EMBMS_RRCIF_CONTENT_DESC_CONTROL_MSG_HDLR

DESCRIPTION
  This function is used to handle the DS_LTE_EMBMS_CONTENT_DESC_CONTROL_IND
  message from LTE RLC

  MH relays the info from RLC to QMI
  
PARAMETERS
  msgrtype - UMID of the message
  dsmsg    - pointer to the message payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE is processing was successful
  False otherwise

SIDE EFFECTS
  None.

==============================================================================*/
LOCAL boolean ds_3gpp_embms_rrcif_content_desc_control_msg_hdlr
(
  msgr_umid_type             msgrtype,
  const msgr_hdr_struct_type *dsmsg_ptr
)
{
  boolean                                    ret_val = FALSE;
  ds_3gpp_embms_content_desc_control_msg_s  *msg_ptr = NULL;
  ps_iface_embms_content_desc_update_type    ind_info;
  msgr_attach_struct_type                   *att_ptr = NULL;
  dsm_item_type                             *dsm_item_ptr = NULL;
  uint32                                     dsm_pkt_len = 0;
  uint32                                     dsm_item_size = 0;
  sys_modem_as_id_e_type                     subs_id = SYS_MODEM_AS_ID_NONE;
  rex_crit_sect_type                        *crit_section_ptr = NULL;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  DS_EMBMS_MSG0_HIGH("LTE EMBMS CONTENT DESC CONTROL IND processing");

  crit_section_ptr = ds_3gpp_embms_get_crit_section();

  DS_3GPP_ENTER_CRIT_SECTION(crit_section_ptr);

  if((msgrtype == DS_LTE_EMBMS_CONTENT_DESC_CONTROL_IND)&& (dsmsg_ptr != NULL))
  {
    /*--------------------------------------------------------------------------
      Typecast the incoming payload into the correct type.
    --------------------------------------------------------------------------*/
    msg_ptr = (ds_3gpp_embms_content_desc_control_msg_s*)dsmsg_ptr;

    subs_id = INST_ID_TO_SYS_AS_ID(msg_ptr->msg_hdr.inst_id);

    memset(&ind_info,0,sizeof(ps_iface_embms_content_desc_update_type));

    do
    {
      /*--------------------------------------------------------------------------
       Get the DSM item ptr from RLC payload
      --------------------------------------------------------------------------*/ 
      att_ptr = msgr_get_attach(&(msg_ptr->msg_hdr), 0);
      if(att_ptr == NULL)
      {
        DS_EMBMS_MSG0_ERROR("Att_ptr is NULL in "
                            "LTE EMBMS CONTENT DESC CONTROL IND");
        ret_val = FALSE;
        break;
      }

      msgr_get_dsm_attach(att_ptr, &dsm_item_ptr);
      
      dsm_pkt_len = dsm_length_packet(dsm_item_ptr); 
      if(dsm_pkt_len == 0)
      {
        DS_EMBMS_MSG0_ERROR("DSM packet len is 0 in "
                            "LTE EMBMS CONTENT DESC CONTROL IND");
        ret_val = FALSE;
        break;
      }

      /* Read the payload size */
      if((dsm_item_size = 
          dsm_pullup(&dsm_item_ptr, &ind_info.size,sizeof(uint32)))
         != sizeof(uint32))
      {
        DS_EMBMS_MSG0_ERROR("DSM pullup error in reading sai_list"
                            "LTE EMBMS CONTENT DESC CONTROL IND");
        ret_val = FALSE;
        break;
      }

      ind_info.content_description 
        = modem_mem_alloc(ind_info.size,MODEM_MEM_CLIENT_DATA);

      if (ind_info.content_description == NULL)
      {
        DS_EMBMS_MSG0_ERROR("Memory allocation failed for"
                            "LTE EMBMS CONTENT DESC CONTROL IND");
        ret_val = FALSE;
        break;
      }

      /* Read the payload */
      if((dsm_item_size = 
        dsm_pullup(&dsm_item_ptr, 
                   ind_info.content_description,
                   (uint16)ind_info.size))
         != ind_info.size)
      {
        DS_EMBMS_MSG0_ERROR("DSM pullup error in reading sai_list"
                            "LTE EMBMS CONTENT DESC CONTROL IND");
        modem_mem_free(ind_info.content_description,MODEM_MEM_CLIENT_DATA);
        ret_val = FALSE;
        break;
      }

      /*----------------------------------------------------------------------
        Send the indication
      ----------------------------------------------------------------------*/
      ds_3gpp_embms_send_ind(IFACE_EMBMS_CONTENT_DESC_CONTROL_EV,
                             (void *)&ind_info, subs_id);

      modem_mem_free(ind_info.content_description,MODEM_MEM_CLIENT_DATA);

      ret_val = TRUE;
    } while (0); 
  }
  else
  {
    DS_EMBMS_MSG0_ERROR("LTE EMBMS CONTENT DESC CONTROL IND - "
                        "Invalid input params");
  }

  if(dsm_item_ptr != NULL)
  {
    /* Discard any extra dsm items */
    dsm_free_packet(&dsm_item_ptr);
  }

  DS_3GPP_LEAVE_CRIT_SECTION(crit_section_ptr);

  return ret_val;
} /* ds_3gpp_embms_rrcif_content_desc_control_msg_hdlr */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_RRCIF_GET_AVAIL_TMGI_LIST

DESCRIPTION
  This function is used to get the Available TMGI list from RRC. This function
  sends a message to LTE RRC requesting for available TMGI list

PARAMETERS
  query_ptr - pointer to the available TMGI list
  subs_id   - subscription id

DEPENDENCIES  
  None

RETURN VALUE
  TRUE - if sending the message was successful
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_rrcif_get_avail_tmgi_list
(
  ps_iface_embms_tmgi_related_query_type* tmgi_related_query_ptr,
  sys_modem_as_id_e_type                  subs_id
)
{
  lte_rrc_embms_avail_tmgi_list_req_s avail_tmgi_req_msg;
  boolean                             ret_val = FALSE;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  if(tmgi_related_query_ptr->client_id != NULL)
  {
    /*--------------------------------------------------------------------------
      Memset the message to be sent to RRC
    --------------------------------------------------------------------------*/
    memset(&avail_tmgi_req_msg, 0, sizeof(avail_tmgi_req_msg));
    
    avail_tmgi_req_msg.debug_trace_id = tmgi_related_query_ptr->debug_trace_id;
    /*--------------------------------------------------------------------------
      Send the message to RRC via DSMSGR
    --------------------------------------------------------------------------*/
    ret_val = dsmsgrsnd_per_subs_msg_send
               (LTE_RRC_EMBMS_AVAIL_TMGI_LIST_REQ, 
		            (msgr_hdr_struct_type*)(&avail_tmgi_req_msg),
                sizeof(lte_rrc_embms_avail_tmgi_list_req_s),
                SYS_AS_ID_TO_INST_ID(subs_id));

    if(ret_val == FALSE) 
    {
      DS_EMBMS_MSG0_ERROR(" Unable to send message to DSMSGR");
    }

  }
  else
  {
    DS_EMBMS_MSG0_ERROR("NULL Client ID while querying AVAIL TMGI list");
  }

  return ret_val;
} /* ds_3gpp_embms_rrcif_get_avail_tmgi_list */


/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_RRCIF_GET_SAI_LIST

DESCRIPTION
  This function is used to get the SAI list from RRC. This function
  sends a message to LTE RRC requesting for available SAI list.

PARAMETERS
  sai_list_query_ptr - Pointer to the SAI list
  is_mdm_originated  - flag to indicated if this request is initiated
                        by the client (or) internally by the modem
  subs_id            - Subscription Id

DEPENDENCIES  
  None

RETURN VALUE
  TRUE - if sending the message was successful
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_rrcif_get_sai_list
(
  ps_iface_embms_sai_list_req_type* sai_list_query_ptr,
  boolean                           is_mdm_originated,
  sys_modem_as_id_e_type            subs_id
)
{
  lte_rrc_embms_avail_sai_list_req_s sai_list_req_msg;
  boolean                            ret_val = FALSE;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  /*---------------------------------- 
    Validate Input SAI list Query
    ----------------------------------*/
  if(sai_list_query_ptr == NULL)
  {
    DS_EMBMS_MSG0_ERROR("SAI List query ptr is NULL");
    return ret_val;
  }
  /*--------------------------------------------------------------------------
    Soon after the EMBMS iface is brought up, Mode handler will query for the
    available SAI list to broadcast it back to all clients, even though
    there may be no one specifically requesting for SAI list.
    --------------------------------------------------------------------------*/
  if((sai_list_query_ptr->client_id != NULL)||
     (is_mdm_originated == TRUE))
  {
    /*--------------------------------------------------------------------------
      Memset the message to be sent to RRC
    --------------------------------------------------------------------------*/
    memset(&sai_list_req_msg, 0, sizeof(sai_list_req_msg));
    
    sai_list_req_msg.debug_trace_id = sai_list_query_ptr->debug_trace_id;
    sai_list_req_msg.mdm_initiated  = is_mdm_originated;
    /*--------------------------------------------------------------------------
      Send the message to RRC via DSMSGR
    --------------------------------------------------------------------------*/
    ret_val = dsmsgrsnd_per_subs_msg_send
                (LTE_RRC_EMBMS_AVAIL_SAI_LIST_REQ, 
		            (msgr_hdr_struct_type*)(&sai_list_req_msg),
                 sizeof(lte_rrc_embms_avail_tmgi_list_req_s),
                 SYS_AS_ID_TO_INST_ID(subs_id));

    if(ret_val == FALSE) 
    {
      DS_EMBMS_MSG0_ERROR("Unable to send AVAIL_SAI_LIST_REQ msg to DSMSGR");
    }

  }
  else
  {
    DS_EMBMS_MSG0_ERROR("NULL Client ID in SAI List Query");
  }

  return ret_val;
} /* ds_3gpp_embms_rrcif_get_sai_list */

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_RRCIF_CONTENT_DESC_UPDATE

DESCRIPTION
  This function is used to get send to RLC the embms content update message
  DS_LTE_EMBMS_CONTENT_DESC_UPDATE_REQ

PARAMETERS
  *content_desc_update_ptr - pointer to the PS payload to be sent
   subs_id - subs id on which message needs to be sent

DEPENDENCIES  
  None

RETURN VALUE
  TRUE - if sending the message was successful
  FALSE otherwise

SIDE EFFECTS  
  None
==============================================================================*/
boolean ds_3gpp_embms_rrcif_content_desc_update
(
  ps_iface_embms_content_desc_update_type *content_desc_update_ptr,
  sys_modem_as_id_e_type                   subs_id
)
{
  errno_enum_type                          errnum = E_FAILURE;
  dsm_item_type                           *dsm_ptr = NULL;	
  msgr_attach_struct_type                 *att_ptr = NULL;
  ds_3gpp_embms_content_desc_update_msg_s  embms_content_desc_msg;
  uint16                                   dsm_item_size = 0;
  boolean                                  ret_val = FALSE;
/* - - - - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - */

  memset(&embms_content_desc_msg,0,
         sizeof(ds_3gpp_embms_content_desc_update_msg_s));

  msgr_init_hdr_attach
  (
    &embms_content_desc_msg.msg_hdr, 
    MSGR_DS_3GPP, 
    DS_LTE_EMBMS_CONTENT_DESC_UPDATE_REQ,
    0,
    1
  );

  msgr_set_hdr_inst(&embms_content_desc_msg.msg_hdr,
                    SYS_AS_ID_TO_INST_ID(subs_id));

  /*-----------------------------------------------
    Start Pushing items in reverse order
  ------------------------------------------------*/
  /* Push down the payload */
  if((dsm_item_size = dsm_pushdown(&dsm_ptr,
                                   content_desc_update_ptr->content_description,
                                   (uint16)content_desc_update_ptr->size,
                                   DSM_DS_SMALL_ITEM_POOL))
     != content_desc_update_ptr->size)
  {
    DS_EMBMS_MSG2_ERROR("DSM pushdown failed size pushed %d, size returned %d",
                        content_desc_update_ptr->size, dsm_item_size);
    dsm_free_packet(&dsm_ptr);
    return ret_val;
  }

  /* Push down the payload size */
  if((dsm_item_size = dsm_pushdown(&dsm_ptr,
                                   &content_desc_update_ptr->size,
                                   sizeof(uint32),
                                   DSM_DS_SMALL_ITEM_POOL))
     != sizeof(uint32))
  {
    DS_EMBMS_MSG2_ERROR("DSM pushdown failed size pushed %d, size returned %d",
                        sizeof(uint32), dsm_item_size);
    dsm_free_packet(&dsm_ptr);
    return ret_val;
  }

  att_ptr = msgr_get_attach(&embms_content_desc_msg.msg_hdr, 0);
  if ((msgr_get_attach(&embms_content_desc_msg.msg_hdr,0)) == NULL)
  {
    DS_EMBMS_MSG0_ERROR("msgr attach ptr is NULL");
    dsm_free_packet(&dsm_ptr);
    return ret_val;
  }

  msgr_set_dsm_attach(att_ptr, dsm_ptr);

  errnum = msgr_send(&embms_content_desc_msg.msg_hdr, 
                     sizeof(ds_3gpp_embms_content_desc_update_msg_s));

  if (errnum != E_SUCCESS)
  {
    DS_EMBMS_MSG0_ERROR("Failed to send DS_LTE_EMBMS_CONTENT_DESC_UPDATE_REQ");
    dsm_free_packet(&dsm_ptr);
    return ret_val;
  }

  ret_val = TRUE;

  return ret_val;
} /* ds_3gpp_embms_rrcif_content_desc_update */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

#endif /* FEATURE_DATA_EMBMS */
