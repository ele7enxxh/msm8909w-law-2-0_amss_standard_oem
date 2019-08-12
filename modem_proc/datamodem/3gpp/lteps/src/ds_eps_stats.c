
/*!
  @file
  ds_eps_stats.c

  @brief
  REQUIRED brief one-sentence description of this C module.

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2009 - 2014 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/lteps/src/ds_eps_stats.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/02/15   ms      create this file for modem power statistics
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include <stringl/stringl.h>

#ifdef FEATURE_DATA_LTE

#ifdef TEST_FRAMEWORK
#error code not present
#endif
#include "ds_eps_stats.h"
#include "dsmsgr.h"
#include "dsmsgrrecv_msg.h"
#include "lte_cphy_msg.h"
#include "ds_eps_bearer_context.h"
#include "ds3gstats.h"


/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*----------------------------------------------------------------------------
  Array to store ML1 tx power values temporarily, when it's time to report
  3s3gstats will use this value
----------------------------------------------------------------------------*/
uint32 ds_eps_tx_modem_power_stats[DS3G_TX_POWER_BIN_MAX];


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================
FUNCTION DS_EPS_CONFIG_MODEM_POWER_STATS

DESCRIPTION
  This function informs lte ml1 to enable/disable modem power stats
 
PARAMETERS
  boolean - enable_modem_power_stats 
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void ds_eps_config_modem_power_stats
(
  sys_modem_as_id_e_type subs_id,
  boolean enable_modem_power_stats
)
{
  lte_cphy_tx_power_report_ind_s lte_cphy_tx_power_report_ind;

  /*-------------------------------------------------------------------------*/

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
     return ;
  }

  DS_LTE_MSG1_HIGH("Sending power stats ind to LTE ML1: %d", 
                   enable_modem_power_stats);

  lte_cphy_tx_power_report_ind.enable = enable_modem_power_stats;

  (void)dsmsgrsnd_msg_send_ext(
     LTE_CPHY_TX_POWER_REPORT_IND,
     MSGR_DS_3GPP, 
     (msgr_hdr_struct_type*)(&lte_cphy_tx_power_report_ind),
     sizeof(lte_cphy_tx_power_report_ind_s)
  );
  
}/*ds_eps_config_modem_power_stats*/


/*===========================================================================
FUNCTION DS_EPS_REQUEST_MODEM_POWER_STATS

DESCRIPTION
  This function requests lte ml1 to send modem power stats
 
PARAMETERS
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void ds_eps_request_modem_power_stats
(
  sys_modem_as_id_e_type subs_id
)
{
  lte_cphy_tx_power_report_info_req_s lte_cphy_tx_power_report_info_req;
  
  /*-------------------------------------------------------------------------*/

  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
     return ;
  }

  DS_LTE_MSG0_LOW("Sending power stats req to LTE ML1");
  (void)dsmsgrsnd_msg_send_ext(
     LTE_CPHY_TX_POWER_REPORT_INFO_REQ,
     MSGR_DS_3GPP, 
     (msgr_hdr_struct_type*)(&lte_cphy_tx_power_report_info_req),
     sizeof(lte_cphy_tx_power_report_info_req_s)
  );
  
}/*ds_eps_request_modem_power_stats*/


/*===========================================================================
FUNCTION DS_EPS_GET_MODEM_POWER_STATS

DESCRIPTION
  This function returns cached tx modem power stats
 
PARAMETERS
  boolean - enable_modem_power_stats 
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

=============================================================================*/
void ds_eps_get_modem_power_stats
(
  sys_modem_as_id_e_type subs_id,
  uint32 tx_power_bin[DS3G_TX_POWER_BIN_MAX] 
)
{
  if(!ds3gsubsmgr_is_subs_id_valid(subs_id))
  {
     return ;
  }
  
  memscpy((void *)tx_power_bin, sizeof(uint32)*DS3G_TX_POWER_BIN_MAX,
            (void *)ds_eps_tx_modem_power_stats,
            sizeof(uint32)*DS3G_TX_POWER_BIN_MAX);

  memset((void *)ds_eps_tx_modem_power_stats, 0, 
            sizeof(uint32)*DS3G_TX_POWER_BIN_MAX);

} /*ds_eps_get_modem_power_stats*/


/*===========================================================================
FUNCTION DS_EPS_STATS_MODEM_POWER_INFO_CNF_HDLR

DESCRIPTION
  This function is used to receive LTE ML1 power statistics information and
  cache it loacally for ds3g to query

PARAMETERS
  msgrtype  - UMID of the message sent
  dsmsg_ptr - Pointer to the payload

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - if the message header is correct and we can handle the message
  FALSE - otherwise

  Note: We will not return FALSE for errors in the message payload

SIDE EFFECTS
  None.
===========================================================================*/
boolean ds_eps_stats_modem_power_info_cnf_hdlr
(
  msgr_umid_type              msgrtype,
  const msgr_hdr_struct_type  *dsmsg_ptr
)
{
  boolean                              ret_val = FALSE;
  lte_cphy_tx_power_report_info_cnf_s *payload_ptr = NULL;
  sys_modem_as_id_e_type               subs_id = SYS_MODEM_AS_ID_NONE;
  uint8                                index = 0;
  ds3gsubsmgr_subs_id_e_type           ds3g_subs_id = DS3GSUBSMGR_SUBS_ID_MIN;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  DS_LTE_MSG0_LOW("Received modem_power_stats from LTE");
  if(msgrtype == LTE_CPHY_TX_POWER_REPORT_INFO_CNF && dsmsg_ptr != NULL)
  {
    ret_val = TRUE;
    payload_ptr = (lte_cphy_tx_power_report_info_cnf_s*)dsmsg_ptr;
    subs_id = payload_ptr->subs_id; 
    if (!ds3gsubsmgr_is_subs_id_valid(subs_id))
    {
      return FALSE;
    }

    for (index = DS3G_TX_POWER_BIN_1; index <DS3G_TX_POWER_BIN_MAX ; index++ ) 
    {
      DS_LTE_MSG2_ERROR("LTE bin[%d] = %d ",index, payload_ptr->tx_power_info[index]);
    }

    memscpy((void *)ds_eps_tx_modem_power_stats, sizeof(uint32)*DS3G_TX_POWER_BIN_MAX,
            (void *)(payload_ptr->tx_power_info),
            sizeof(uint32)*DS3G_TX_POWER_BIN_MAX);

    /*-------------------------------------------------------------------------
    Send report to ps framework 
    -------------------------------------------------------------------------*/
    ds3g_subs_id = ds3gsubsmgr_subs_id_cm_to_ds3g(subs_id);
    ds3g_report_modem_power_stats(ds3g_subs_id);
  }

  else
  {
    DS_LTE_MSG2_LOW("Error in incoming message. Msg id: %d, Msg_ptr 0x%x",
                       msgrtype,dsmsg_ptr);
  }

  return ret_val;
} /* ds_eps_stats_modem_power_info_cnf_hdlr */

#endif /* FEATURE_DATA_LTE */

