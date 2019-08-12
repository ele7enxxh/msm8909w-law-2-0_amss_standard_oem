#ifndef ULTIMELINE_H
#define ULTIMELINE_H

/*===========================================================================
              U L   T I M E L I N E . H

DESCRIPTION
  This file defines the generic TX event dispatcher and the individual TX 
  events that will be handled by the ENC driver. 

Copyright (c) 2000,2013 by Qualcomm Technologies, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/ultimeline.h_v   1.6   09 Jan 2002 10:42:04   halbhavi  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/l1/offline/src/ultimeline.h#1 $ $DateTime: 2016/12/13 08:00:12 $ $Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
02/20/13    hk      Mainlined FEATURE_WCDMA_DL_ENHANCED
02/08/12    vs      Feature cleanup.
02/01/12    gnk     Nikel feature cleanup
11/29/11    gnk     Removed some unused features
03/30/11    gnk     Mainlined several features
01/25/07    vb      featurized to bring in support for MCAL/dl enhanced
08/14/06    yiz     Mainline HSUPA code
05/12/05    gs      Added frame boundry event enum type
08/30/04    gs      Added HS channel event
                    Added TX_GEN_UL_MAINT_EVENT event
01/09/01    sh      Put back stripped out code for FEATURE_VST.
12/18/01    sh      Added TX_GEN_GET_DATA2_EVENT called when mac is busy.
12/03/01    sk/sh   Added TX_GEN_PC_ENABLE_EVENT to enable closed
                    loop power control at start of DPCH frame.
                    Removed TX_GEN_BUILD_FRAME_EVENT.
09/22/00    sk      file created.
                    
===========================================================================*/
#include "wcdma_variation.h"

#endif // ULTIMELINE_H

