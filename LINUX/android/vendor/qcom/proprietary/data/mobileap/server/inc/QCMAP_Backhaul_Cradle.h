#ifndef _QCMAP_BACKHAUL_CRADLE_H_
#define _QCMAP_BACKHAUL_CRADLE_H_

/*======================================================

FILE:  QCMAP_Backhaul_Cradle.h

SERVICES:
   QCMAP Connection Manager Backhaul Cradle Class

=======================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

======================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  07/04/14   ka           Created
======================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "qcmap_cm_api.h"
#include "QCMAP_Tethered_Backhaul.h"

class QCMAP_Backhaul_Cradle : public QCMAP_Tethered_Backhaul
{
private:
   static bool flag;
   static QCMAP_Backhaul_Cradle *object;

   QCMAP_Backhaul_Cradle();

public:
  static QCMAP_Backhaul_Cradle *Get_Instance(boolean obj_create=false);

  ~QCMAP_Backhaul_Cradle();

  /* ---------------------------Cradle Execution---------------------------*/


   /*V4 connection available on Cradle*/
   boolean                     cradle_v4_available;
   /*V6 connection available on Cradle*/
   boolean                     cradle_v6_available;
   /* Track cradle bringup */
   boolean                     cradle_connect_in_progress;
   qcmap_cm_cradle_conf_t cradle_cfg;
   /* Use to store the gateway information from RA when acting as backhaul*/
   struct in6_addr ipv6_gateway_addr;

   static boolean IsCradleBridgeActivated(void);
   static boolean IsCradleWANBridgeActivated(void);
   static boolean IsCradleBackhaulAvailableV4();
   static boolean IsCradleBackhaulAvailableV6();
   /* Cradle Mode */
   static boolean GetCradleMode( qcmap_msgr_cradle_mode_v01 *mode,
                                        qmi_error_type_v01 *qmi_err_num );

   static boolean SetCradleMode( qcmap_msgr_cradle_mode_v01 mode,
                                       void *softApHandle,
                                       qmi_error_type_v01 *qmi_err_num );

   /*Handle Address Assignment during Cradle Association*/
   static void ProcessCradleAddrAssign(char* devname,
                                       ip_version_enum_type ip_vsn);

   /* Handle Cradle Disassociation event. */
   void ProcessCradleDisAssoc();
   void DisableCradle(qmi_error_type_v01 *qmi_err_num);
   void SwitchToCradleBackhaul();
};
#endif

