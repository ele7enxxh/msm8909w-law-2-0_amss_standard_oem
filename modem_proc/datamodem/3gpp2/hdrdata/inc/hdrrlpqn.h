#ifndef HDRRLPQN_H
#define HDRRLPQN_H

/*===========================================================================

                      H I G H     D A T A     R A T E

    Q U I C K   N A K   P R O T O C O L   L A Y E R   D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the HDR QuickNak Layer.
  
Copyright (c) 2003 - 2011 by Qualcomm Technologies, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/inc/hdrrlpqn.h#1 $
                     
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
05/23/07   da      Created Module

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "errno.h"

#ifdef FEATURE_HDR_QOS

#include "dsm.h"
#include "dsrlpi.h"
#include "hdrrlpi.h"
#include "queue.h"
#include "errno.h"
#include "ds707_qosdefs.h"

#ifndef FEATURE_HDR
#error "FEATURE_HDR_QOS defined without FEATURE_HDR being defined"
#endif /* FEATURE_HDR */

/* <EJECT> */

/*===========================================================================

             DEFINITIONS  FOR THE MODULE

===========================================================================*/


/*===========================================================================

                        DATA  DECLARATIONS

===========================================================================*/


/*===========================================================================

                        FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION HDRRLPQN_RESET

DESCRIPTION
  This function resets the state variables of this QN instance

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void hdrrlpqn_reset
(
  dsrlp_qn_ctl_type    *qn_ptr         /* QN-specific ctl block    */
);

/*===========================================================================

FUNCTION HDRRLPQN_ACTIVATE

DESCRIPTION
  This function activates this QN instance

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void hdrrlpqn_activate
(
  dsrlp_qn_ctl_type    *qn_ptr,         /* QN-specific ctl block    */
  uint64                qn_id,          /* QN ID */
  uint8                 schdgrp_id,     /* Scheduler group id for this QN instance */
  uint8                 num_carriers    /* Number of carriers associated with this QN instance */
);

/*===========================================================================

FUNCTION HDRRLPQN_DEACTIVATE

DESCRIPTION
  This function deactivates this QN instance and resets the state

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void hdrrlpqn_deactivate
(
  dsrlp_qn_ctl_type    *qn_ptr         /* QN-specific ctl block    */
);

/*===========================================================================

FUNCTION HDRRLPQN_PROCESS_RX_PKT

DESCRIPTION
  This function processes the QN header of a received RLP data 
  packet.  The function is called on a per packet basis.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void hdrrlpqn_process_rx_pkt
(
  dsrlp_qn_ctl_type       *qn_ptr,        /* QN-specific ctl block    */
  dsrlpi_rlp_ctl_blk_type *rrcb_ptr,      /* RLP-specific ctl block   */
  dsm_item_type           *fwd_pkt_ptr,   /* Fwd RLP pkt received     */
  dsrlpi_header_type      *rlp_header_ptr /* RLP Header info          */
);

/*===========================================================================

FUNCTION HDRRLPQN_SCHDGRPID_LOOKUP

DESCRIPTION
  This function looks up a scheduler_group_id within the scheduler info 
  structure of type hdrsrchschdgrp_info_type maintained by searcher.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if it finds the gid in the list, FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean hdrrlpqn_schdgrpid_lookup
( 
  hdrl1_srch_schdgrp_rpt_type  *schdgrp_info_ptr,
  uint8                      schdgrp_id
);

/*===========================================================================

FUNCTION HDRRLPQN_PROCESS_SCHEDULER_GROUP_CHANGED_IND

DESCRIPTION
  This function processes the HDRIND_LMAC_SCHEDULER_GROUP_CHANGED indication
  and queries for the new sched group info from the lmac layer. Based 
  on the new and old sched_group_info, old QN instances are deactivated 
  and new ones are activated.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void hdrrlpqn_process_scheduler_group_changed_ind( void );

#endif /* FEATURE_HDR_QOS */

#endif /* HDRRLPQN_H */
