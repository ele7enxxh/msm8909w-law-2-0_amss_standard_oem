#ifndef RXDIV_H
#define RXDIV_H

/*============================================================================
                                  R X D I V . H
DESCRIPTION

Copyright (c) 2006 - 2010 Qualcomm Technologies, Inc.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/l1/vcs/rxdiv.h_v   1.2   18 Jul 2002 09:25:44   vpaliwal  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/wcdma/api/rxdiv.h#1 $
$DateTime: 2016/12/13 08:00:12 $
$Author: mplcsds1 $

when        who     what, where, why
--------    ---     --------------------------------------------------------
10/01/13    sm      Enable NV read when coming Online in FTM Mode
04/25/11    geg     Separated rxdiv.h into rxdiv.h and rxdiv_v.h
04/25/11    geg     Moved this file to /wcdma/api from /l1/inc
01/28/10    vsr     Added support for RxD RSCP based enable/disable
                    Updated chain swap API to accept required cfg
01/14/11    ks      Added RxD support for FACH state and DPCH signalling
12/13/10    vsr     Backup RxD lockmode params during HHO
12/08/10    vsr     Added support for Rx chain swap
10/14/10    vb      Added API to update quality/capacity based thresholds to trigger rxd
08/08/10    sj      Added unified PLT support
03/30/10    rvs     Remove FEATURE_WCDMA_CME_CONTROLLER.
09/22/09    sup     Removed log code assignments and placed them in
                    log_codes_wcdma_int.h
07/30/09    vsr     Changes to use dB10 notation instead of dB12
07/15/09    rvs     Edited feature flags to include
                    FEATURE_WCDMA_QICE_CONTROLLER.
05/20/09    yh      Add API to check if RxD is active or going active.
03/12/09    rc      Added HS callback function to update RxD status.
10/03/08    hk      Bringing in 7k Mailine fixes
08/27/08    vsr     Added CME capability under feature FEATURE_WCDMA_CME_CONTROLLER
08/08/08    ks      Enabling RXD_OPS_SRCH_BMSK
06/10/08    ms      Added PACKED_POST attribute for structures to make it
                    compatible for GNU compiler.
10/24/07    vsr     Mainlining MBMS and Enhanced DL changes 
08/13/07    vp      Moved event handlers from dltimeline to individual modules.
07/09/07    vsr/vb  changes to support modified rxd i/f for rxd meas config update
                    changes to support quality based rxd triggering only for dynamic rxd and 
                    provided facility to turn on capacity based rxd triggering through NV value 17
05/22/07    mmr     Introducing a debug feature FEATURE_WCDMA_RXD_DEBUG_CHAIN_STUCK
                    to catch the diversity chain stuck state.
04/28/07    vp/vsr  Changes for RxD log packet
02/06/07    mc      Added support for Type3 receiver
01/19/07    kps     Add new reason for setting FOFF bit, RXD_DIV_CHAIN_FOR_TRM.
                    This can be same as RXD_DIV_CHAIN_FOR_GPS, since both will never
                    be used in the same build.
12/04/06    vb      modified Maximum number of valid NV items value to 15
10/16/06    vip     JTT disabled by default through NV control
09/25/06    vip      Changes for capacity threshold value calculation
09/19/06    vip     RxD phase 2 changes:
                    (a) APIs rxd_cap_meas_update(), rxd_qual_meas_update(), 
                        rxd_cap_conf_update() for capacity and quality measurements
                    (b) Log packet for RxD measurements
                    (c) NV enhancements for MMSE, JTT, JFT, dynamic PS configurations
                    (d) State operation changes to facilitate continuous measurements
07/13/06    vip     Disabling the searches on the diversity antenna and do 
                    primary antenna searches only, may enable in future
07/03/06    vip     (a) RxD controller log packet code change 0x4727 -> 0x4014
                    (b) Fix for reset problem due to div chain init when MDSP is down
06/19/06    vip     Added number of WCDMA frequency bands to be scanned for RxD support
05/22/06    vip     Added client mechanism for forcing RxD mode to turn off for
                    GPS and CM
04/13/06    vip     Event triggered logging changes
04/04/06    vip     (a) Added callback handler for hardware modules associated
                    with RxD controller
                    (b) Two more NV item type additions and maximum NV item types
                    (c) Band dependent RxD functionality
03/31/06    vip     Enhancements to support the following:
                    (a) FTM API rxd_overide_nv_settings() for enforcing FTM imposed behavior
                    (b) API rxd_set_forced_off() for GPS request for the diversity RF chain
                    (c) APIs rxd_enable() and rxd_disable() to handle WPLT request for forcing
                        RxD control
                    (d) AGC antenna decision changes and rxd_config_update() enhancements
                    (e) Table for Nv settings
                    (f) Featurization changes
10/08/05    vip     Created the file for supporting two RF receive chains and the receive
                    diversity feature
===========================================================================*/

/* Maximum number of valid NV items */
#define RXD_NV_VALUE_MAX 18

/*===========================================================================
FUNCTION rxd_overide_nv_settings

DESCRIPTION
  This function overides the NV values and the antenna assignment mode

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void rxd_overide_nv_settings(uint8 overide_nv_val);

/*===========================================================================
FUNCTION rxd_set_active_rx_chain

DESCRIPTION
  API used to indicate the Rx chain setting to be used for next traffic state.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean rxd_set_active_rx_chain(boolean chain0, boolean chain1);

/*===========================================================================
FUNCTION rxd_rscp_meas_thresh_update

DESCRIPTION
  This function updates threshold RSCP value to be considered for RSCP based Rx Diversity triggers. 
  It takes input parameter RSCP in range of 20 to 121 value corresponds to (-20dBm to -121dBm) . If input 
  argument is out of range, default RSCP as set by NV is kept. Once if the value is updated it will overwite 
  value set by NV and stay there until UE is power cycled or leaves and enters WCDMA mode.

DEPENDENCIES
 None

INPUT VALUE
  RSCP range from 20 to 121 integer value

RETURN VALUE
None

SIDE EFFECTS
  RSCP thresh NV value will get overwritten
===========================================================================*/
extern void rxd_rscp_meas_thresh_update(uint8 rscp_value);

/*===========================================================================
FUNCTION rxd_qual_meas_thresh_update

DESCRIPTION
  This function updates threshold Bler to be considered for Quality based Rx Diversity triggers. 
  It takes input parameter % of bler range of 0 to 500 dB10 (0 % to 50 %) value. if any input argument 
  is out of range default BLER of 12.5% is taken. Once if the value is updated it will stay there until 
  UE is power cycled

DEPENDENCIES
None

INPUT VALUE
  Bler range from 0 to 500 integer value

RETURN VALUE
None

SIDE EFFECTS
  None
===========================================================================*/
void rxd_qual_meas_thresh_update(uint16 bler_percentage);

/*===========================================================================
FUNCTION rxd_update_fullload_cpich_ior_ratio

DESCRIPTION
  This function updates the assumed cpich/Ior value under full load condition, which is used for computation of 
  thresholds for N/W capaicty based trigger to enable RX Diversity. It take input argument cpich/Ior range db4 value 
  from -60 to -20, which corresponds to -15db to -5db. If i/p argument is out of range default value (-40) corresponds to -10db 
  is taken and overwrites any previous value. Once if the value is programmed it will stay there until UE is power cycled.

DEPENDENCIES
None

INPUT VALUE
this function takes db4 value of cpich/Ior integer value ranging -60 to -20. 

RETURN VALUE
None

SIDE EFFECTS
  None
===========================================================================*/
void rxd_update_fullload_cpich_ior_ratio(int8 min_cpich_to_ior_ratio_db4);

/*===========================================================================
FUNCTION rxd_temporary_disable
         
DESCRIPTION
  This function is used to temporarily disable RxD in order to save UE
  current consumption. The argument specifies the number of seconds for
  which RxD is to be disabled. The function accepts any value between
  5 and 60 seconds.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  RxD gets disabled. Timer is set so that RxD can be renabled.
===========================================================================*/
extern void rxd_temporary_disable(uint8 time_in_sec);
#endif /* ifdef RXDIV_H */
