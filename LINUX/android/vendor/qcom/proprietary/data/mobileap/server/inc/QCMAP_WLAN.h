#ifndef _QCMAP_WLAN_H_
#define _QCMAP_WLAN_H_

/*====================================================

FILE:  QCMAP_WLAN.h

SERVICES:
   QCMAP Connection Manager WLAN Class

=====================================================

  Copyright (c) 2011-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

=====================================================*/
/*======================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.
    when       who        what, where, why
  --------   ---        -------------------------------------------------------
  08/14/14   ka           Created
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

#include "QCMAP_Backhaul_WWAN.h"
#include "QCMAP_Backhaul_WLAN.h"
#include "QCMAP_Backhaul_Cradle.h"
#include "QCMAP_Tethered_Backhaul.h"

class QCMAP_WLAN
{
  private:
    static bool flag;
    static QCMAP_WLAN *object;
    QCMAP_WLAN();


  public:
    static QCMAP_WLAN* Get_Instance(boolean obj_create=false);

    ~QCMAP_WLAN();

    /*----------------------MobileAP WLAN Execution-----------------------*/

    //ALL VARIABLES HERE//
    qcmap_cm_wlan_conf_t wlan_cfg;
    /* Which mode is wifi brought up in */
    qcmap_cm_debug_conf_t debug_config;
    qcmap_msgr_wlan_mode_enum_v01 wifi_mode;
    qcmap_msgr_access_profile_v01 prev_guest_profile;
    boolean activate_wlan_in_progress;
    /* eth device number for first AP iface */
    static bool  priv_client_restart_wlan;

    /* Variable to Store the STAOnlyMode enabled state */
    boolean is_sta_only_mode_enabled;
    qcmap_msgr_wlan_status_ind_msg_v01 current_wlan_status;

    boolean AssociateWLANtoBridge(int wlan_index);

    boolean ReadWLANConfigFromXML();

    static boolean ReadWLANConfigPathsFromXML(qcmap_cm_wlan_paths_conf_t *wlan_paths);

    static boolean GetsetWLANConfigFromXML( qcmap_wlan_config_enum conf,
                                            qcmap_action_type action,
                                            uint32 *value );

    inline boolean IsWlanModeUpdated()
                        {return (this->wifi_mode != this->wlan_cfg.wlan_mode); }

    static boolean EnableWLAN( qmi_error_type_v01 *qmi_err_num,
                                     boolean enable_wlan_onbootup = false,
                                     boolean privileged_client    = false );

    boolean EnableWLANModule(char *,boolean bootup);

    /* Bring down Linux LAN. */
    static boolean DisableWLAN( qmi_error_type_v01 *qmi_err_num,
                                      boolean privileged_client    = false );
    inline boolean IsSSRInProgress(void)
                                  {return QCMAP_WLAN::priv_client_restart_wlan;}

    boolean DisableWLANModule(void);
    /* Set WLAN Mode. */
    static boolean SetWLANConfig
                           ( qcmap_msgr_wlan_mode_enum_v01 wlan_mode,
                             qcmap_msgr_access_profile_v01 guest_access_profile,
                             qcmap_msgr_station_mode_config_v01 *station_config,
                             qmi_error_type_v01 *qmi_err_num );

    /* Get Configured LAN Mode */
    static boolean GetWLANConfig
                          ( qcmap_msgr_wlan_mode_enum_v01 *wlan_mode,
                            qcmap_msgr_access_profile_v01 *guest_access_profile,
                            qcmap_msgr_station_mode_config_v01 *station_config,
                            qmi_error_type_v01 *qmi_err_num );

    /* Actiavte WLAN Request. */
    static boolean ActivateWLAN(qmi_error_type_v01 *qmi_err_num);

    /* Restart WLAN Request. */
    static boolean RestartWLAN(void);


    /* Get the Current LAN Status. */
    static boolean GetWLANStatus( qcmap_msgr_wlan_mode_enum_v01 *wlan_mode,
                                         qmi_error_type_v01 *qmi_err_num );

    /* Activate HostapdConfig Request.*/
    static boolean ActivateHostapdConfig
                       ( qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
                         qcmap_msgr_activate_hostapd_action_enum_v01 action_type,
                         qmi_error_type_v01 *qmi_err_num );

    boolean ActivateHostapdActionStart
                               ( qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
                                 int ap_pid,
                                 int guest_ap_pid,
                                 qmi_error_type_v01 *qmi_err_num );

    boolean ActivateHostapdActionStop
                               ( qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
                                 int ap_pid,
                                 int guest_ap_pid,
                                 qmi_error_type_v01 *qmi_err_num);

    boolean ActivateHostapdActionRestart
                               ( qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type,
                                 int ap_pid,
                                 int guest_ap_pid,
                                 qmi_error_type_v01 *qmi_err_num);

    /* Checks if hostapd is running. */
    boolean IsHostapdRunning(int iface);

    /* Checks if wpa_supplicant is running. */
    boolean IsSupplicantRunning(void);

    /* Kills the appropriate instance of hostapd_cli*/
    void StopHostapdCli(qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type);

    /* Starts the appropriate instance of hostapd_cli*/
    void StartHostapdCli(qcmap_msgr_activate_hostapd_ap_enum_v01 ap_type);

    /* Configure, start and stop the Linux HostAPD server. */
    boolean StopHostAPD(void);
    boolean StartHostAPD(int intf,int wlan_dev_index);

    boolean IsGuestProfileUpdated();
    void UpdateAccessProfileRules(void);
    void InstallGuestAPAccessRules(void);
    void DeleteGuestAPAccessRules(void);
    boolean IsHostapdkilled(int omit_pid);

    /* Updates the IPACM_cfg file */
    boolean SetIPAWLANMode(int mode);

    /* Enable STA Only Mode.
       Putting it in WLAN Class so that we dont have to create any additional objects */
    static boolean EnableSTAOnlyMode(qmi_error_type_v01 *qmi_err_num);
    static boolean DisableSTAOnlyMode(qmi_error_type_v01 *qmi_err_num);
    /* Send WLAN Status IND */
    void SendWLANStatusIND(void);
};
#endif
