#ifndef _QCMAP_WLAN_SCM_H_
#define _QCMAP_WLAN_SCM_H_

/*====================================================

FILE:  QCMAP_WLAN_SCM.h

SERVICES:
   QCMAP Connection Manager WLAN SCM Class

=====================================================

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

=====================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  03/09/16   tbw        Created
======================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/select.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include "stringl.h"
#include "ds_util.h"
#include "comdef.h"
#include "qcmap_cm_api.h"
#include "qti_wlan_scm_msgr_v01.h"

class QCMAP_WLAN_SCM
{
  private:
    static bool flag;
    static QCMAP_WLAN_SCM *object;
    QCMAP_WLAN_SCM();

    /* QMI SCM service information. */
    qmi_client_type           qmi_scm_handle;
    qmi_client_type           qmi_scm_notifier;
    qmi_cci_os_signal_type    qmi_scm_os_params;

  public:
    static QCMAP_WLAN_SCM* Get_Instance(boolean obj_create=false);

    ~QCMAP_WLAN_SCM();

    /*----------------------MobileAP WLAN Execution-----------------------*/

    //ALL VARIABLES HERE//
    boolean SCMEnableWLAN(int mode);
    int SCMDisableWLAN();
    static boolean DualWifiEnabled();
    int SCMReconfig(scm_msgr_wlan_if_type_enum_v01 mode, scm_msgr_wlan_if_control_enum_v01 control_option);

    int QCMAP_WLAN_SCM::SCMProcessDynamicEnable(int iface_num, int type);
    int QCMAP_WLAN_SCM::SCMProcessDynamicDisable(int type);
};
#endif
