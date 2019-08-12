#ifndef HDRDS_API_H
#define HDRDS_API_H
/*=========================================================================

          H D R  D A T A  S E R V I C E S  T A S K  H A N D L E R

                  E X T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This module is the task level handler for HDR data service modules. The
  module handles queuing of events (messages, commands & indications) for the
  ds task and de-queuing and calling the appropriate processing functions
  from the ds task.

Copyright (c)2009 - 2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath: O:/src/asw/COMMON/vcs/hdrds.h_v   1.4   09 Dec 2001 20:05:50   vramaswa  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/hdrds_api.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
                     
when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/11   vpk     HDR SU API cleanup
09/04/09   ss      New file created for CMI SU level changes.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "hdrcom_api.h"
#include "dsm.h"

/*===========================================================================

                      FUNCTION  DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION HDRDS_QUEUE_IND_EXT

DESCRIPTION
  This function is a wrapper to hdrds_queue_ind with the indication info (third
  argument) restricted to hdrcom_ind_data_union_type. hdrds_queue_ind_ext should 
  be in the APP Side and will be called from Modem side

DEPENDENCIES
  None.

PARAMETERS
  protocol_name - Who the indication is for
  ind_name - Global enumeration of indications
  ind_data_ptr - Pointer to the indication data

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hdrds_queue_ind_ext
(
  hdrcom_hai_protocol_name_enum_type protocol_name,
  hdrcom_ind_name_enum_type ind_name,
  hdrcom_ind_data_union_type ind_data
);

#endif /* HDRDS_API_H */
