#ifndef __MVS_LOG_PACKET_H__
#define __MVS_LOG_PACKET_H__

/*===========================================================================

                  M U L T I M O D E   V O I C E   S E R V I C E S

DESCRIPTION
  None.

REFERENCES
  None.

Copyright (C) 2011-2013 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/common/mvs/inc/mvs_log_packet.h#1 $
$Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/26/13   sud     Updated Copyright.
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "mvssup.h"
#include "apr_comdef.h"


/*===========================================================================

F U N C T I O N   D E F I N I T I O N S

===========================================================================*/
int32_t LogUMTSRxPacket(mvs_mode_type mvs_mode, mvs_frame_info_type mvs_frame_info,
                            void *vocoderPacket);
int32_t LogUMTSTxPacket(mvs_mode_type mvs_mode,
                            mvs_frame_info_type mvs_frame_info,void *vocoderPacket);

int32_t LogCDMATxPacket(mvs_mode_type mvs_mode, mvs_voc_rate_type frame_rate,
                                    void *vocoderPacket);
int32_t LogCDMARxPacket(mvs_mode_type mvs_mode, mvs_voc_rate_type frame_rate,
                                    void *vocoderPacket);

#endif /*__MVS_LOG_PACKET_H__ */

