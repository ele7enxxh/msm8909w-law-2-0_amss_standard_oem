#ifndef _QCMAP_TETHERED_BACKHAUL_H_
#define _QCMAP_TETHERED_BACKHAUL_H_

/*======================================================

FILE:  QCMAP_Tethered_Backhaul.h

SERVICES:
   QCMAP Connection Manager Tethered Backhaul Class

=======================================================

  Copyright (c) 2011-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

======================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  02/17/16   jc           Created
======================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "qcmap_cm_api.h"


/* Cradle flags */
#define CradleMode_TAG                  "CradleMode"
#define EthBackhaulMode_TAG             "EthBackhaulMode"
#define MAX_IFACE_NAME_LENGTH           5
typedef enum {
  QCMAP_TETH_CRADLE_BACKHAUL = 0,
  QCMAP_TETH_ETH_BACKHAUL
}QCMAP_Tethered_backhaul_type;

class QCMAP_Tethered_Backhaul
{

public:

  QCMAP_Tethered_Backhaul();
  ~QCMAP_Tethered_Backhaul();

  /* ----------------------Teth Backhaul Execution---------------------------*/
   static boolean GetSetConfigFromXML(qcmap_action_type action,
                               QCMAP_Tethered_backhaul_type backhaul_type,
                               void *mode);

   static void DisableV4V6RulesOnOtherBackhaul(
                                   qcmap_backhaul_type backhaul_type);

   void ProcAddrDisAssocOnBackhaul(
                                   qcmap_backhaul_type backhaul_type);
   /* For JO there is no IPA. */
#ifndef FEATURE_DATA_TARGET_MDM9607
   static boolean SetIPACMCategory(char* iface_name,
                                 char* category,
                                 char* mode);
#endif /* FEATURE_DATA_TARGET_MDMFERRUM */
};
#endif

