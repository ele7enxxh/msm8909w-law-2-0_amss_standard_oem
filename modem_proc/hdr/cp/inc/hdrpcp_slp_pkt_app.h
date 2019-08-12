/*===========================================================================

      H D R   P A C K E T   C O N S O L I D A T I O N   P R O T O C O L
             S L P   P A C K E T   A P P   D E F I N I T I O N S

DESCRIPTION
  This file contains the externalized declarations and definitions for the 
  1x-EVDO Release B Packet Consolidation Protocol's SLP Packet Application.

  Copyright (c) 2007 Qualcomm Technologies Incorporated. All Rights Reserved

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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/cp/inc/hdrpcp_slp_pkt_app.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/07   hal     Initial version

===========================================================================*/

#ifndef HDRPCP_SLP_PKT_APP_H
#define HDRPCP_SLP_PKT_APP_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*-----------------------------------------------------
  Common AMSS include files
-----------------------------------------------------*/

#include "hdr_variation.h"
#include "comdef.h"


/*-----------------------------------------------------
  HDR specific include files
-----------------------------------------------------*/

#ifndef FEATURE_HDRPCP_UNITTEST
#include "hdrhai.h"
#endif /* FEATURE_HDRPCP_UNITTEST */



/*===========================================================================

                    EXTERNAL FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

FUNCTION HDRPCP_SLP_PKT_APP_INIT

DESCRIPTION
  This function performs one-time initialization of the SLP Packet Application
  and registers all necessary callbacks with PCP.

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrpcp_slp_pkt_app_init
( 
  void /* no arguments */
);

/*===========================================================================

FUNCTION HDRPCP_SLP_PKT_APP_INIT_CTRL_BLK

DESCRIPTION
  This function should be called at the beginning of every subframe to 
  properly initialize and dependent instance variables for the SLP
  Packet Application.

DEPENDENCIES
  Must be called by PCP at the beginning of every subframe.

PARAMETERS
  channel              - The channel on which SLP will transmit data
  time_since_prev_req  - The number of PN ROLLs that have triggered

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
extern void hdrpcp_slp_pkt_app_init_ctrl_blk
(
  hdrhai_channel_enum_type channel,
  uint16 time_since_prev_req
);

#endif /* HDRPCP_SLP_PKT_APP_H */
