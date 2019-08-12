/*===========================================================================

         S E R V I C E   O P T I O N S   D E F I N I T I O N S

GENERAL DESCRIPTION
  This module contains definitions of service options other than data.

  Copyright (c) 2000 - 2015 Qualcomm Technologies, Inc. 
  All Rights Reserved Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccso.h_v   1.0.2.0   30 Nov 2001 17:30:26   fchan  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/1x/cp/src/mccso.h#1 $ $DateTime: 2016/12/13 08:00:08 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/19/15   agh     Changes to support Voice Adapter based Audio architecture
03/01/11   ag      Fix to release MVS before MUX is commanded to release SO.
01/04/11   ag      Mainlined FEATURE_MVS_MIGRATE.
09/14/10   ag      Featurized voice support under FEATURE_MODEM_1X_VOICE_SUPPORT.
05/15/08   bb      Code changes for "MVS API Update" feature
11/02/00   lcc     Initial release for FEATURE_COMPLETE_SNM.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "modem_1x_defs.h"

#include "1x_variation.h"
#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
#include "mvs.h"
#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

/*===========================================================================

                           FUNCTION PROTOTYPES

===========================================================================*/

#ifdef FEATURE_MODEM_1X_VOICE_SUPPORT
/*===========================================================================

FUNCTION mccso_cdmacodec_mvs_cb_func

DESCRIPTION
  This function initializes all the service options defined in mccso.c.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccso_cdmacodec_mvs_cb_func(mvs_event_type *event);

/*===========================================================================

FUNCTION MCC_IS_MVS_MODE_SUPPORTED

DESCRIPTION
  This function checks if a particular MVS mode is supported or not for
  old Audio architecture. 
  For new Audio architecture, all modes are supported on current targets
  as per inputs from Audio team so this API should always return TRUE.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE  - Mode is supported
  FALSE - Mode is not supported

SIDE EFFECTS
  None.

===========================================================================*/

boolean mccso_is_mvs_mode_supported ( mvs_mode_type mode );

/*===========================================================================

FUNCTION MCC_SET_AVG_ENCODER_RATE

DESCRIPTION
  This function provides the average encoder to MVS or the CDMA Voice Adapter.
  Depending on the audio architecture being used, this function will invoke MVS
  API or provide required EVENT to CDMA Voice Adapter.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccso_set_avg_encoder_rate ( byte rate ) ;

#endif /* FEATURE_MODEM_1X_VOICE_SUPPORT */

/*===========================================================================

FUNCTION MCCSO_RELEASE_MVS

DESCRIPTION
  This function releases MVS if it was acquired for CDMA voice services.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mccso_release_mvs ( void );

/*===========================================================================

FUNCTION mccso_initialize

DESCRIPTION
  This function initializes all the service options defined in mccso.c.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void mccso_initialize( void );
