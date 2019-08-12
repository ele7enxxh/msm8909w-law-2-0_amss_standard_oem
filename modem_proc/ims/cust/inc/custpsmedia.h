#ifndef CUSTPSMEDIA_H
#define CUSTPSMEDIA_H
/*===========================================================================

DESCRIPTION
  Configuration for PS Media Path and Codec as part of QVideoPhone

  Copyright (c) 2005 - 2006 by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/ims/cust/inc/custpsmedia.h#1 $ $DateTime: 2016/12/13 08:00:34 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/04/09   apr     Adding FEATURE_QVP_RTP_GAN_DSCP for GAN
08/04/08   mc      Added QVP_SDP_DEAD_CODE feature.
05/09/08   mc      Initial Creation.

===========================================================================*/

/*---------------------------------------------------------------------------
                       PS MEDIA PATH SERVICE FLAGS
---------------------------------------------------------------------------*/
#ifdef FEATURE_QDSP6
#undef QVP_RTP_ERROR_CONCEAL_PATTERN
#define FEATURE_QVPHONE_SIP_USE_TOS
#define FEATURE_QVPHONE_SDP
#define FEATURE_QVPHONE_RTP
//top level feature removed: #define FEATURE_IMS_QVP_RTP
#endif //FEATURE_QDSP6


#endif /* CUSTPSMEDIA_H */

