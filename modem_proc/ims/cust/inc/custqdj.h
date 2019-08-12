#ifndef CUSTQDJ_H
#define CUSTQDJ_H
/*===========================================================================

DESCRIPTION
  Configuration for QDJ (Qualcomm DeJitter library).

  Copyright (c) 2005  by Qualcomm Technologies Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/ims/cust/inc/custqdj.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/18/08   mc      Removed QDTX feature.
08/04/08   mc      Removed TIMEWARP feature and other commented part of code
07/28/08   mc      Featurized TIMEWARP to disable in SC2X,6290/6246 builds
05/22/08    grk    Added FEATURE_CRD_ENHANCEMENTS
05/30/06    ac     Added support for FEATURE_QDJ_BEGIN_END_ENHANCEMENTS
05/05/06    ac     Added support for FEATURE_QDJTEST
09/26/05   waq     Initial version.

===========================================================================*/
/* -------------------------------------------------------------------------
** QDJ
** ----------------------------------------------------------------------- */

#define FEATURE_QDJ
//top level feature removed: #define FEATURE_CRD_ENHANCEMENTS

/* -------------------------------------------------------------------------
** QDTX
** ----------------------------------------------------------------------- */

//#define FEATURE_QDTX

#endif /* CUSTQDJ_H */
