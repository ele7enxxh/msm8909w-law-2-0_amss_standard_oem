#ifndef RLCDLTM_H
#define RLCDLTM_H
/*===========================================================================
                  D O W N L I N K   T R A N S P A R E N T   M O D E   

              R A D I O   L I N K   C O N T R O L : H E A D E R  F I L E

DESCRIPTION
  This file contains function prototypes associated with the
  Downlink Transparent Mode RLC module.
  
Copyright (c) 2001 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

Copyright (c) 2002, 2008-2009 Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/rlc/vcs/rlcdltm.h_v   1.3   22 May 2001 14:28:56   rchar  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/rlc/src/rlcdltm.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/01/12   grk     Feature cleanup
05/11/09   ssg     Updated Copyright Information
03/24/08   pj      Added support for data path optimizations under 
                   FEATURE_WCDMA_UL_DATA_PATH_OPT & FEATURE_WCDMA_DL_DATA_PATH_OPT
10/06/02   ttl     Modified rlci_dl_reassemble_tm_pdus() function type.
05/21/01   rc      Added more comments.
01/31/01   rc      Changed function prototype name from 
                   rlci_reassemble_dl_tm_pdus() to rlci_dl_reassemble_tm_pdus().
01/08/01   rc      Added description for the protoype.
11/22/00   rc      Created file.

===========================================================================*/


/*===========================================================================
** Includes and Public Data Declarations
**=========================================================================*/

/*---------------------------------------------------------------------------
** Include Files
**-------------------------------------------------------------------------*/

#include "rlcdl.h"
#include "macrlcif.h"


/*===========================================================================

FUNCTION rlci_dl_reassemble_tm_pdus

DESCRIPTION
  Enqueues the Downlink Transparent Mode PDUs that arrive from MAC into 
  the downlink queue for that channel and calls a callback function, if 
  the higher layer has registered for it.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_dl_reassemble_tm_pdus
(       
  rlci_dl_ctrl_blk_type *ctrl_blk, /* Pointer to Downlink Control block    */
  dsm_item_type         *pdu_ptr,  /* Packet Data Unit                     */
  uint16                pdu_len,   /* PDU length                           */
  rlc_lc_id_type        rlc_id     /* RLC logical channel ID               */
);

/*===========================================================================

FUNCTION rlci_enh_dl_reassemble_tm_pdus

DESCRIPTION
  Enqueues the Downlink Transparent Mode PDUs that arrive from MAC into 
  the downlink queue for that channel and calls a callback function, if 
  the higher layer has registered for it.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
void rlci_enh_dl_reassemble_tm_pdus(rlci_dl_ctrl_blk_type *ctrl_blk_ptr, 
                                    complete_pdu_type *complete_pdu_ptr);

#endif
