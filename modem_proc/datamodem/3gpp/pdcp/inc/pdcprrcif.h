#ifndef PDCPRRCIF_H
#define PDCPRRCIF_H
/*===========================================================================

                           P D C P  R R C  I F

                           H E A D E R   F I L E


DESCRIPTION
  This file contains data declarations and function prototypes for wcdma
  that are shared between PDCP and RRC layers as part of the control path.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2005 - 2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/pdcp/inc/pdcprrcif.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/31/09   ss      SU level CMI modifications.
03/04/09   sa      AU level CMI modifications.
08/31/06   rsl     Featurized PDCP
===========================================================================*/


/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "comptask_v.h"
#ifdef FEATURE_DATA_WCDMA_PS
#include "sys.h"
#include "errno.h"
#include "ps_svc.h"
#include "ps_rohc.h"
#include "ps_iphc.h"
#include "pdcpi.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION PDCP_RESET_PID_TABLE

DESCRIPTION This function resets pid entries to invalid values, the ones at
            powerup.

PARAMETERS
          pdcp_inst - the index of pdcp ctl block array.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void pdcp_reset_pid_tables
(
  uint8 pdcp_inst
);

/*===========================================================================
FUNCTION PDCP_GET_NEXT_DL_RX_SN

DESCRIPTION
  This primitive is used by RRC to get the next_rx_sn on the DL when a 
  RLC PDU size change is received from UTRAN through an RB_RECONFIG message.
  
PARAMETERS
  pdcp_instance - Index into the pdcp control block
  next_rx_sn - parameter that will have SN value

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS If next_dl_rx_sn retrieval succeeded.
  E_FAILURE If next_dl_rx_sn retrieval failed.

SIDE EFFECTS
  None

===========================================================================*/
errno_enum_type pdcp_get_next_dl_rx_sn
(
  uint8     pdcp_inst, 
  uint32    *next_rx_sn
);

/*===========================================================================
FUNCTION PDCP_RRC_CFG_REQ_CMD_HDLR

DESCRIPTION
  This function will be reigstered as a callback for PDCP CFG REQ CMD
  with PS task.
  
PARAMETERS
  cmd: the CMD itself being processed by pdcp-task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
CCC: why extern here?
===========================================================================*/
void pdcp_rrc_cfg_req_cmd_hdlr
(
  const comp_cmd_type  *cmd_ptr                             /* Pointer to command */
);

/*===========================================================================
FUNCTION PDCP_REGISTER_WM_HDLR

DESCRIPTION  This function registers given tx and rx wm with PDCP. These
              wm are owned by DS mode handler to enqueue or dequeue data.
  
DEPENDENCIES
  None

RETURN VALUE
  True - if wm reg succeeded
  false - if wm reg failed

SIDE EFFECTS
  None
===========================================================================*/

boolean  pdcp_register_wm_hdlr
(
  const comp_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION PDCP_DEREGISTER_WM_HDLR

DESCRIPTION
  This function deregisters the WMs between PS & PDCP.

DEPENDENCIES
  None

RETURN VALUE
  TRUE - if parameter validation passes
  FALSE - if parameter validation fails
  
SIDE EFFECTS
  Can result in WM dereg with RLC if PDCP was initially in transparent state.
===========================================================================*/

void pdcp_deregister_wm_hdlr
( 
  const comp_cmd_type *cmd_ptr
);
/*===========================================================================
FUNCTION PDCP_SET_FLOW_MASK_HDLR

DESCRIPTION
  This function handles the set flow mask command posted by the mode 
  handler / RLC.

DEPENDENCIES
  None

RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/

void pdcp_set_flow_mask_hdlr
( 
  const comp_cmd_type *cmd_ptr
);

/*===========================================================================
FUNCTION PDCP_GET_PDCP_INST_GIVEN_LC_ID

DESCRIPTION
  
PARAMETERS

DEPENDENCIES
  None

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
uint8 pdcp_get_pdcp_inst_given_lc_id
(
  sys_modem_as_id_e_type subs_id,
  uint8          rlc_dl_id,
  uint8          rlc_ul_id
);

/*===========================================================================
FUNCTION PDCP_UPDATE_DL_WM_LEVELS

DESCRIPTION
  This function gets invoked when RRC forwards RAB_Reconfig request to PDCP.
This function dynamically updates DL WM levels based on RAB rates.

PARAMETERS
  rab_dl_rate - new RAB DL rate to tune the WM levels to.
  wm_ptr - WM to be tuned.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pdcp_update_dl_wm_levels
(
  uint32                    rab_dl_rate,
  dsm_watermark_type        *wm_ptr
);

#endif /* FEATURE_DATA_WCDMA_PS */



#endif /* FEATURE_DATA_PDCP */
