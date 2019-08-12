#ifndef DS_3GPP_EMBMS_RRCIF_H
#define DS_3GPP_EMBMS_RRCIF_H
/*==============================================================================

          e M B M S D A T A - S P E C I F I C   H A N D L E R 


                   D S _ 3 G P P _ E M B M S _ R R C I F . C
GENERAL DESCRIPTION
  This file contains the functions that are used to interface between the 
  DS_3GPP_EMBMS_RRCIF and DS_3GPP_EMBMS_HDLR components.

EXTERNALIZED FUNCTIONS
  boolean ds_3gpp_embms_rrcif_is_iface_bring_up_allowed(void)


INITIALIZATION AND SEQUENCING REQUIREMENTS


  Copyright (c) 2013 - 2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================
                           EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/embms/inc/ds_3gpp_embms_rrcif.h#1 $ 
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
#include "ps_iface_embms.h"
#include "sys.h"

#ifdef FEATURE_DATA_EMBMS

#define DS_3GPP_EMBMS_RRCIF_PLMN_OFFSET 3
#define DS_3GPP_EMBMS_RRCIF_PLMN_ID_SIZE 3
#define DS_3GPP_EMBMS_RRCIF_SVC_ID_SIZE 3

#define DS_3GPP_EMBMS_MAX_ACTIVE_SESSIONS LTE_EMBMS_MAX_ACTIVE_SESSIONS
#define DS_3GPP_EMBMS_MAX_AVAIL_SESSIONS LTE_EMBMS_MAX_AVAIL_SESSIONS
/*==============================================================================
                            Externalized Functions
==============================================================================*/
/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_RRCIF_IS_IFACE_BRING_UP_ALLOWED

DESCRIPTION   This function returns the status as advertized by LTE RRC. If 
              eMBMS has not been enabled on the modem, interface bring up shall
              be disallowed.

              This will not be enforced as of now as there is a possibility that
              there could be race conditions between the receipt of Iface up cmd
              and the indication from RRC about enabling eMBMS.

DEPENDENCIES  

RETURN VALUE
  TRUE if eMBMS is enabled on the modem
  FALSE otherwise

SIDE EFFECTS  None
==============================================================================*/
boolean ds_3gpp_embms_rrcif_is_iface_bring_up_allowed(void);

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
void ds_3gpp_embms_rrcif_init(void);

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
);

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

);

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
);
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
  ps_iface_embms_tmgi_related_query_type* query_ptr,
  sys_modem_as_id_e_type                  subs_id
);

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
);

/*==============================================================================

FUNCTION      DS_3GPP_EMBMS_RRCIF_CONTENT_DESC_UPDATE

DESCRIPTION
  This function is used to get send to RLC the embms content update message
  DS_3GPP_EMBMS_CONTENT_DESC_UPDATE_REQ

PARAMETERS
  

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
);

#endif /* FEATURE_DATA_EMBMS */
#endif /* DS_3GPP_EMBMS_RRCIF_H */
