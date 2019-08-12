#ifndef HDRBCOVHDI_H
#define HDRBCOVHDI_H

/*===========================================================================

       B R O A D C A S T     P R O T O C O L       S U I T E
                I N T E R N A L  D E F I N I T I O N S

DESCRIPTION
  This contains the declarations for the Broadcast Overhead Protocol 

Copyright (c) 2005, 2006, 2007, 2008 
              by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/hdr/bc/inc/hdrbcovhdi.h#1 $ $DateTime: 2016/12/13 07:58:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/23/09   wsh     CMI: featurized CMI under FEATURE_CMI
11/10/08   smd     Replaced errno.h with IxErrno.h  
08/28/05   dna     Created module.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

             INCLUDE FILES FOR MODULE

===========================================================================*/

#include "hdr_variation.h"
#include "comdef.h"
#include "IxErrno.h"
#include "dsm.h"
#include "sys.h"

#ifdef FEATURE_CMI
#include "sys_v.h"
#endif

#include "hdrovhd.h"

/*===========================================================================

                  MACROS

===========================================================================*/

/*===========================================================================

                      TYPE DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------------
                        OVERHEAD MESSAGE

  Following data structures are used to store the received 
  BroadcastOverhead message.
---------------------------------------------------------------------------*/
/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION HDRBCOVHD_POWERUP_INIT

DESCRIPTION
  Powerup initialization for the HDRBCOVHD module.
  
DEPENDENCIES
  None.
  
PARAMETERS
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  Initializes static data on power up.
  
===========================================================================*/
extern void hdrbcovhd_powerup_init( void );

/*===========================================================================
FUNCTION HDRBCOVHD_UPDATE_OVHD_INFO

DESCRIPTION
  Updates the currently stored version of the BroadcstOverhead message.
  
DEPENDENCIES
  
PARAMETERS
  msg_ptr      - Pointer to the DSM item chain holding the message 
  bc_msg_ptr   - Pointer to the cache entry.

RETURN VALUE
  
SIDE EFFECTS
  None.
===========================================================================*/
extern void hdrbcovhd_update_ovhd_info
(
  dsm_item_type            *msg_ptr,
  hdrbcovhd_bc_msg_type    *bc_msg_ptr
);

/*=============================================================================

FUNCTION HDROVHD_UNPACK_OVHD_MSG

DESCRIPTION
  This function unpacks parts of the received Broadcast message into the
  overhead message cache. It saves indexes into the message in the cache
  so other routines can extract values on an as-needed basis without needing
  to re-parse the entire message.


DEPENDENCIES

PARAMETERS
  msg_ptr      - Pointer to the DSM item chain holding the message 
  bc_msg_ptr - Pointer to the cache entry.

RETURN VALUE
  E_SUCCESS  - If extraction & unpacking of message succeeds.
  E_NO_DATA  - Otherwise.

SIDE EFFECTS
  None

=============================================================================*/
extern errno_enum_type hdrbcovhd_unpack_ovhd_msg
(
  dsm_item_type            *msg_ptr,
  hdrbcovhd_bc_msg_type    *bc_msg_ptr
);

/*=============================================================================
FUNCTION HDRBCOVHD_UNPACK_SOFT_COMBINE_INFO

DESCRIPTION
  Helper

DEPENDENCIES
  None

PARAMETERS
  item_ptr       - DSM item pointer to received message
  sp_msg_ptr     - Pointer to SP message in cache
  neighbor_count - Number of neighbors in corresponding SectorParameters msg
  info_ptr       - Pointer where to place unpapcked info
  soft_combine_offset - soft combine offset

RETURN VALUE
  E_SUCCESS if the flow record was parsed successfully
  E_NO_DATA if there was an error extracting a field
  E_DATA_INVALID if there were more soft combine neighbors than expected
  
  If E_SUCCESS is returned, info_ptr has the soft combine neighbor info for the 
  given flow.  Otherwise info_ptr may have inconsistent data.

SIDE EFFECTS
  None
=============================================================================*/

extern errno_enum_type hdrbcovhd_unpack_soft_combine_info
(
   dsm_item_type         *item_ptr,
   hdrovhd_sp_msg_type   *sp_msg_ptr,
   uint8                 neighbor_count,
   hdrbcovhd_soft_combine_info_type *info_ptr,
   uint16                soft_combine_offset
);
#endif /* HDRBCOVHDI_H */
