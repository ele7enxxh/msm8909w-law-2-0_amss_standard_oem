#ifndef TM_UMTS_CP_CORE_IFACE_H
#define TM_UMTS_CP_CORE_IFACE_H


/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  UMTS_CP / TM core Interface Header File

GENERAL DESCRIPTION

  Prototypes of functions defined in tm_umts_cp.c and used by TmCore.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS
 

  Copyright (c) 2006 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved. 

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/sm/tm/umts_cp/inc/tm_umts_cp_core_iface.h#1 $
  $DateTime: 2016/12/13 07:59:45 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  05/03/07   LT      Initial version.

============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif





extern void tm_umts_cp_prtl_iface_init(void);






#ifdef __cplusplus
}
#endif


#endif
