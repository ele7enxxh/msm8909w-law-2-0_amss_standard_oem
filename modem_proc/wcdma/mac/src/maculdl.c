/*============================================================================================

         U P L I N K    D O W N L I N K  D A T A   H A N D L E R


GENERAL DESCRIPTION

  This module contains the functions which are used both by the UL MAC and the DL MAC.

Copyright (c) 2001 by Qualcomm Technologies, Inc.  All Rights Reserved.
Copyright (c) 2002, 2005-2006, 2008-2009 by Qualcomm Technologies, Inc.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*===*===*===*===*===*===*===*/

/*==============================================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/msm5200/mac/vcs/maculdl.c_v   1.0   11 Jun 2002 18:38:26   skrishna  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/mac/src/maculdl.c#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------------------------
07/26/14   rkmk    Reduce image size by using right macro depending upon number of parameters
07/13/12   grk     MBMS feature cleanup at L2(MBMS_DEBUG_1, FEATURE_MODEM_MBMS)
03/28/12   zr      Adding feature definitions for Dime
01/02/12   grk     Feature cleanup
01/23/12   kc      Changes for free floating tasks
05/07/09   ssg     Updated Copyright Information
04/29/09   av      Fixed compiler warnings in L2 
04/26/06   gsc     Added support for HSDPA re-config optimizations
12/05/05   sk      Changed the name of l1_post_dl_ul_mac_config_sig_at_l1_failure to
                   l1_post_dl_mac_config_sig_at_l1_failure in the comments section
11/03/05   sk/gsc  Added fucntion l1_post_dl_ul_mac_config_sig_at_l1_failure
03/09/05   sk      Fixed all lint errors.
09/04/02   sk      Added function definition mac_rrc_get_hfn_info()
08/24/02   sk      Removed function l1_post_mac_config_sig() and added functions
                   l1_post_ul_mac_config_sig() and l1_post_dl_mac_config_sig().
08/15/02   sk      Changed cmd_ptr->cmd.mac_config_status to
                   cmd_ptr->cmd.mac_config_cnf.mac_config_status
08/06/02   sk      Changed the name l1_post_ul_mac_config_sig to l1_post_mac_config_sig().
                   changed cmd_id from CPHY_CONFIG_CNF to  CPHY_MAC_CONFIG_CNF
                   and the config_ind to mac_config_status.
06/06/02   sk      Created the file.

==============================================================================================*/


/*==============================================================================================
                     INCLUDE FILES
==============================================================================================*/
#include "wcdma_variation.h"
#include "maculdl.h"
#include "macul.h"

/*==============================================================================================
 FUNCTION NAME  l1_post_ul_mac_config_sig

 FUNCTION   This function is called by physical layer and it posts the config signal to l2 command
            queue.
===============================================================================================*/
void l1_post_ul_mac_config_sig (void)
{
  /* Set UL MAC config signal to UL L2 task */
  (void)rex_set_sigs(wcdma_l2_ul_get_tcb(), MAC_PHY_UL_CONFIG_IND);
}

/*==============================================================================================
 FUNCTION NAME  l1_post_dl_mac_config_sig

 FUNCTION   This function is called by physical layer and it posts the config signal to l2 command
            queue.
===============================================================================================*/
void l1_post_dl_mac_config_sig (void)
{
  /* Set DL MAC config signal to DL L2 task */
  (void)rex_set_sigs(wcdma_l2_dl_get_tcb(), MAC_PHY_DL_CONFIG_IND);
}

/*==============================================================================================
 FUNCTION NAME  l1_post_mac_hs_config_sig

 FUNCTION   This function is called by physical layer and it posts the HS config signal to l2 command
            queue.
===============================================================================================*/
void l1_post_mac_hs_config_sig (void)
{
  /* Set DL MAC config signal to DL L2 task */
  (void)rex_set_sigs(wcdma_l2_dl_get_tcb(), MAC_PHY_HS_CONFIG_IND);
}


/*==============================================================================================
 FUNCTION NAME  l1_post_dl_mac_config_sig_at_l1_failure

 FUNCTION   This function is called by physical layer and it posts the config signal to l2 command
            queue when there is failure to process CPHY_SETUP_REQ and MAC UL or DL sync for MAC
            config is requested
===============================================================================================*/
void l1_post_dl_mac_config_sig_at_l1_failure (void)
{
  /* Set MAC config signal to DL L2 task */
  (void)rex_set_sigs(wcdma_l2_dl_get_tcb(), MAC_PHY_DL_CONFIG_AT_L1_FAILURE_IND);
}


/*==============================================================================================
 FUNCTION NAME  maci_post_l1_config_ind

 FUNCTION   This function posts to L1 the configuration sucess message. This function can be
            called by both the UL and DL MAC to post a configuration sucess message to L1.

===============================================================================================*/
void  maci_post_l1_config_ind
(
  l1_cmd_enum_type cmd_id
)
{
  /* L1 External Command Buffer Pointer */
  l1_ext_cmd_type   *cmd_ptr;

  /* Get External Command Buffer Pointer */
  cmd_ptr = l1_get_cmd_buf();

  /* L1 External Command Buffer Pointer is NULL */
  if (cmd_ptr == NULL){
    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_HIGH , "CPHY_MAC_CONFIG_CNF command buffer allocation failed");
  }
  else {
    /* Populate Command Header fields */
    cmd_ptr->cmd_hdr.cmd_id = cmd_id;

    /* Populate command data fields */
    cmd_ptr->cmd.mac_config_cnf.mac_config_status = TRUE;

    /* Put the command in L1 External Command Queue */
    l1_put_cmd (cmd_ptr);

    MSG(MSG_SSID_WCDMA_MAC, MSG_LEGACY_LOW , "CPHY_MAC_CONFIG_CNF sent");

  }
}

/*-------------------------------------------------------------------
FUNCTION mac_rrc_get_hfn_info

DESCRIPTION
 Function interface which will provide RRC with the information of CS
 and PS HFN numbers during the time of RRC connection release
DEPENDENCIES
  None

RETURN VALUE
  Boolean ( TRUE indicates MAC was able to populate the cs_hfn and
  ps_hfn values.FALSE indicates the data was not populated.

SIDE EFFECTS
  None
--------------------------------------------------------------------*/
boolean
mac_rrc_get_hfn_info(mac_hfn_info_type *hfn_info)
{
  boolean rv = TRUE;

  L2_ACQ_CIPHER_PARAM_LOCK();
  hfn_info->cs_hfn = cur_ul_cipher_info[0].hfn;
  hfn_info->ps_hfn = cur_ul_cipher_info[1].hfn;
  L2_ACQ_CIPHER_PARAM_UNLOCK();
  return rv;
}

