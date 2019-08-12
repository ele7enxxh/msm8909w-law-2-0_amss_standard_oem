/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                V E N D O R   S P E C I F I C

                A T    C O M M A N D   
                
                P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes AT commands. It implements execution of common 
  vendor specific AT commands.

EXTERNALIZED FUNCTIONS

  dsatvend_exec_qcclr_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcclr command.
    at$qcclr command is used for clearing mobile error log.

  dsatvend_exec_qcdmg_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdmg command.
    at$qcdmg is used to transition the serial port to diagnostic
    monitor.

  dsatvend_exec_qcdmr_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdmr command.
    at$qcdmr is used to set the DM baud rate.

  dsatvend_exec_qcdom_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdom command.
    at$qcdom is used to set domain name address.

  dsatvend_exec_qcdnsp_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdnsp command.
    at$qcdnsp is used to set primary DNS IP address.

  dsatvend_exec_qcdnss_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdnss command.
    at$qcdnss is used to set secondary DNS IP address.

  dsatvend_exec_qcter_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcter command.
    at$qcter is used to set TE-DCE baud rate.

  dsatvend_exec_brew_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdmg command.
    at$brew is used to transition the serial port to diagnostic
    monitor.

  dsatvend_exec_qcpwrdn_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcpwrdn command.
    at$qcpwrdn is used to power down the terminal.
  
  dsatvend_exec_qcdgen_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcdgen command.
    at$qcdgen is used to send a specified quantity of PS data as IP packet(s) 
    over a primary PDP-IP context already activated by +CGACT command.

  dsatvend_exec_qcslip_cmd
    This function brings up the SLIP Iface by posting SLIP_IFACE_UP_EV to the
    DS task. It reserves a serial port that will be used by the slip stack.

  dsatvend_exec_prefmode_cmd
    This function takes the result from the command line parser
    and executes it. It executes at^prefmode command. This command provides 
    the ability to set/get the network mode preferences.

  dsatvend_exec_sysinfo_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT^SYSINFO command. It provides
    the current SYSTEM information of <srv_status>, <srv_domain>
    <roam_status>, <sys_mode>,<sim_status>. 

  dsatvend_exec_pacsp_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT+PACSP command.
  This function returns the current PLMN mode bit setting of EF CSP file.

  dsatvend_exec_qcpdpcfge_cmd
    This function takes the result from the command line parser
    and executes it. It executes at$qcpdpcfge command. 
    at$qcpdpcfge is used to edit values in APN table. 

  dsatvend_exec_qcpdpimscfge_cmd
  This function takes the result from the command line parser
  and executes it. It executes at$qcpdpimscfge command.
  at$qcpdpimscfge is used to edit PDP profile registry . 

  dsatvend_exec_qcdrx_cmd
    This function takes the result from the command line parser
    and executes it. It executes AT^QCDRX command.
    This command provides the ability to set/get the drx coefficient.

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2001 - 2016 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE    

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatvend.c_v   1.3   11 Oct 2002 10:27:58   sbandaru  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatvend.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/20/16   skc     Fixed issue with AT$QCMRUC for DSDS case.
12/10/15   skc     Replacing TUPLE_TYPE_IPv4_254 with TUPLE_TYPE_IPv4 to remove
                   IP limitation for DNS
12/08/15   skc     Fixed compilation issue due to FEATURE_DUAL_SIM disabled
10/06/15   skc     Fixed KW Issues
01/29/15   tk      Feature wrapped $QCVOIPM command with FEATURE_IMS.
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
08/12/14   pg      Modified DS Profile APIs to per subs.
06/27/14   tk      Added support for Dynamic ATCoP.
02/15/14   pg      Added support for SGLTE+G.
04/17/14   sc      Fixed ATCoP KW errors.
03/17/14   pg      Fixed number of SW resets in Ni/Di targets.
03/18/14   pg      Fixed $QCMRUE SD LTE scan issue.
03/26/14   sc      Fixed SYSINFO display issue on SUB2.
03/14/14   tk/sc   Optimized debug macros usage in ATCoP.
02/06/14   sc      Fixed $QCRSRP display issue.
01/23/14   sc      Added support for $QCPDPCFGEXT command.
01/07/14   sc      Converted high usage ATCoP macros to functions.
12/30/13   sc      Used cm_user* api instead of sys_sel* api
11/22/13   sc      Fixed Bolt MOB3.0 compiler warnings.
11/13/13   sc      Added support for $QCCSGCOPS command.
10/23/13   sc      Adding support for $QCACQDBC command to clear acq_DB
10/23/13   sc      Fixed $QCRSRP and $QCRSRQ result display issue.
10/11/13   sc      Added support for $QCNSP, $QCSIMT, $QCRCIND commands and
                   REMOTE CALL END, REMOTE RING, REMOTE ANSWER
                   unsolicited result codes.
10/03/13   tk      ATCoP changes for L+G DSDS.
09/13/13   sc      Fixed KW errors.
08/26/13   pg      Fixed $QCRMCALL ip invalid issue.
08/21/13   pg      Added support for emergency calls, operator res pco, mcc and mnc fields in $QCPDPCFGE.
06/05/13   tk      ATCoP changes for SGLTE support on Dime Plus.
05/28/13   tk      Fixed $QCBANDPREF issue with WLAN France 5000 band.
05/17/13   sk      Added data retry fields to $QCPDPCFGE command.
03/27/13   tk      Fixed issues in dual stack commands.
03/18/13   tk      Fixed $QCMRUE issue with invalid input PLMN ID length.
01/04/13   tk      ATCoP changes for DSDX C/G+G with True World Mode Support.
12/20/12   tk      Fixed $QCRMCALL command memory leak issue.
10/11/12   tk      Fixed $QCRMCALL command issue.
09/24/12   sk      Added $QCRSRP and $QCRSRQ command Support.
09/24/12   tk      Migrated to CM APIs for MRU table access.
08/22/12   sk      Mainlining Nikel Changes.
08/06/12   tk      Added support for $QCDRX command.
07/26/12   sb      Fixed KW Errors.
06/21/12   sk      Fixed $QCPDPCFGE apn bearer issue.
06/12/12   tk      Extended $QCSYSMODE command for 3GPP2 modes.
02/25/12   sb      Fixed compiler Warnings.
05/18/12   tk      Migrated to MSG 2.0 macros
05/18/12   sk      Added APN bearer field to $QCPDPCFGE.
05/14/12   sb      Fixed KW Warnings.
04/10/12   nc      Fixed Read concurrency issues for MRU commands.
03/29/12   sk      Fixed errors & warnings when FEATURE_DSAT_EXTENDED_CMD is enabled.
03/15/12   nc      Initialized mru_entry before writing onto NV.
02/17/11   ua      Added support for $QCRMCALL.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
02/06/12   ad      Added CSTF/CEN/CGPIAF/CVMOD command support.
01/19/12   sk      Feature cleanup.
01/04/12   sb      Fixed KW Warnings.
11/28/11   sb      Fixed $QCMRUE command to not show entries with NO_SRV as mode
11/03/11   nc      Replaced the deprecated API 'hw_partnum_version' with 'DalChipInfo_GetChipId'.
10/17/11   ad      Added +CECALL support.
09/27/11   mk      Added support for $QCCLAC command.
09/06/10   nc      Fixed the RSSI and RSCP display when 
                   FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY is enabled
07/05/11   nc      Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
06/07/11   nc      Added support for $QCAPNE command.
05/26/11   dvk     Global Variables Cleanup
05/13/11   ad      Added $QCPDPIMSCFGE support.
04/29/11   bs      Fixed $QCDGEN command to use COMPTASK.
03/14/11   ad      Added APN class support in $QCPDPCFGE command.
01/11/11   ad      Removed pending variable .
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
10/06/10   ad      Added Error msg before Assert(0).
10/04/10   ad      Generates PS data on the context ID when ID is less then  SYS_MAX_PDP_CONTEXTS.  
08/31/10   sa      Added support for default pdp type as IPv4v6.
09/13/10   sa      Fixed QCDGEN handler to send more data.
08/06/10   nc      Using vbatt_read_pmic_temperature instead of 
                   adc_read(ADC_MSM_THERM_DEGC).
07/28/10   bs      Fixed $QCDGEN command for LTE builds.
07/13/10   ad      Fixed CNTI issue.
07/08/10   ad      Added support for $ECALL command.
06/03/10   sn      Added FEATURE_BT_MODEM featurization for BT specific 
                   functionality. This feature will be enabled in MODEM proc 
                   on multi-proc targets.
05/31/10   ad      Fixed lint error.
05/28/10   sa      Fixed crash during $QCPDPCFGE execution.
05/20/10   kk      Fixed on-target compilation error.
05/17/10   ad      Added Support for $QCPDPCFGE.
05/10/10   kk      Added support for $QCVOLT, $QCHWREV, $QCBOOTVER, $QCTEMP, 
                   $QCAGC, $QCALLUP.
03/18/10   nc      Added Support for $QCDGEN for LTE.
01/18/10   bs      Added support for +PACSP.
06/10/09   ua      Added support for ^PREFMODE command. 
12/15/09   nc      Featurisation changes for LTE.
11/02/09   sa      Replacing snprintf with std_snprintf.
09/22/09   vrk     Merged LTE changes.
09/09/09   ua      Added support for EONS (spec 22.101).
09/18/09   ss      DataCommon CMI SU level impacts.
09/13/09   pp      AT$QCSLIP posts CallP UP command to DCC.
07/04/09   pp      hton/ntoh* macros replaced with ps_hton/ntoh*.
06/30/09   bs      Migration to modified diag interface.
06/12/09   ua      Added support for $QCBANDPREF command.
04/29/09   ua      Fixed compiler warnings. 
12/12/08   ua      Fixed Off target lint errors.
11/07/08   sa      Modified transition of serial port to diagnostic
                   monitor using $BREW command to be non-persistent.
10/18/08   pp      Added support for SLIP interface (AT$QCSLIP)
03/24/08   bs      Fixed Klocwork criticals.
02/18/08   sa      Added modification for Passport Feature support.
02/08/08   sa      Added support for $QCSQ command.
01/11/08   sa      Added support for $QCANTE and $QCRPW commands.
12/04/07   pp      Fixed RVCT 2.2 Compiler warning.
11/24/07   ua      Fixed Lint-lows. 
07/18/07   sa      Fixed feature wrap errors for GSM 1x builds.
05/02/07   ua      Fixed lint medium errors. 
02/15/07   ss      Fixed lint high errors
01/30/07   ss      Replaced banned string API calls
11/03/06   snb     Dual processor changes.
09/26/06   sa      Inter RAT handover support for $QCDGEN.
05/22/06   rsl     Add support for at$qcsysmode to return current sysmode, 
                   possible values returned: HSDPA/HSUPA/HSDPA+HSUPA/WCDMA/GSM.
03/10/06   snb     Verify datagen_info_idx before use.
01/26/06   snb     Have $QCDGEN use DSM_DS_SMALL_ITEM_POOL,
                   correct size of and indexing into datagen_info array,
                   Lint fixes.
09/08/05   ar      Add support for $QCSDFTEST vendor command.
08/10/05   snb     Allow secondary contexts for $QCDGEN.
08/02/05   snb     Add WCDMA-mode $QCDGEN support.
08/01/05   snb     Fix for 6500-GSM builds: use legacy functions instead of
                   ps_ip4_hdr functionality.
07/18/05   snb     Use PS functions for dummy IP frame construction.
07/08/05   snb     Call the registration function to SNDCP in the same way as
                   the DS UMTS PS handler for consistent operation.
06/27/05   snb     Add support for $QCDGEN sending data simultaneously on 
                   multiple contexts.
05/16/05   ar      Merge in serialzer port support to $QCDMG 
05/10/05   snb     Add flow control to $QCDGEN GPRS data send.
04/28/05   snb     Fix 6550 GSM-CDMA build issues.
02/24/05   snb     Add $QCDGEN command support.
11/19/03   ar      Replace dsatutil_format_ipaddr with dsatutil_convert_tuple
10/21/03   ar      Added $QCPWRDN command support.
10/08/03   ar      Add wrapper FEATURE_DSAT_BREW_SUPPORT to BREW commands.
                   Lint corrections.
07/07/03   sb      Added AT$BREW command
12/06/02   ak      Removed include of nv_items.h under UNFINISHED, and
                   instead include nv.h
10/09/02   sb      ERROR check to see if it is a valid baud in $QCTER
04/17/02   rc      Removed FEATURE wrap FEATURE_DS_SOCKETS around dns 
                   functions.
12/13/01   sb      Changed the rdm_data_got_at$qcdmg() function name to 
                   rdm_data_got_atqcdmg()
08/27/01   sb      ATCOP code is now partitioned into multiple VUs. As a 
                   result of the partitioning, some of the files have to be
                   renamed. So making the file/interface name changes.
08/10/01   sjd     Initial release to MSM5200 archives.
05/15/01   sb      Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>


#if defined(T_WINNT)
#error code not present
#endif /* WINNT */

#include "dstaski.h"
#include "dsati.h"
#include "dsatcmif.h"
#include "dsatctab.h"
#include "dsatparm.h"
#include "dsatvend.h"
#include "dsatme.h"
#include "msg.h"
#include "err.h"
#include "diag.h"
#include "nv.h"
#include "dsutil.h"
#ifdef FEATURE_ECALL_APP
#include "ecall_app.h"
#endif /* FEATURE_ECALL_APP */

#include <stdio.h>
#include <string.h>

#ifdef FEATURE_DATA_PS_SLIP
extern ds3g_siolib_portid_e_type slip_ser_port;
#include "slip_iface_ext.h"
#endif /* FEATURE_DATA_PS_SLIP */

#ifdef FEATURE_RUNTIME_DEVMAP
#include "rdevmap.h"
#include "diagcomm.h"
#endif /* FEATURE_RUNTIME_DEVMAP */

#ifdef FEATURE_DATA_IS707
#include "ds707_jcdma_m51.h"
#include "ds707_pkt_mgr.h"
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_DSAT_ETSI_DATA
#define FEATURE_DSM_WM_CB
#include "dsm.h"
#include "mn_cm_exp.h"
#include "dsatetsipkt.h"
#include "dsatetsictab.h"
#include "ps_in.h"
#include "ds_profile_3gppi.h"

#include "comptask_v.h"

#ifndef FEATURE_DSAT_LEGACY_GCF_SUPPORT          
#include "ps_ip4_hdr.h"
#else  /* FEATURE_DSAT_LEGACY_GCF_SUPPORT */
/* If legacy GCF feature is defined, Q304Maint branch of MMDATA_PS is being
   used and provides no ps_ip4_hdr functionality. */
#include "ps_iputil.h"
#endif /* FEATURE_DSAT_LEGACY_GCF_SUPPORT */

#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DATA_WCDMA_PS
#include "rrcdata.h"
#include "wcdmamvsif.h"
#endif /* FEATURE_DATA_WCDMA_PS */

#ifdef FEATURE_GSM_GPRS
#include "gsndcp.h"
#endif /* FEATURE_GSM_GPRS */

#include "dsatetsime.h"

#include "vbatt.h"
#include "DDIChipInfo.h"
#include "DALDeviceId.h"

#ifdef FEATURE_HDR_AT_TEST_ONLY
#include "dsat707hdrctab.h"
#endif /* FEATURE_HDR_AT_TEST_ONLY */
#ifdef FEATURE_DS_PSTATS
#include "dsatpstatsctab.h"
#endif /* FEATURE_DS_PSTATS */
#ifdef FEATURE_CDMA_SMS
#include "dsat707smsctab.h"
#endif /* FEATURE_CDMA_SMS */
#ifdef FEATURE_DATA_IS707
#include "dsat707vendctab.h"
#include "dsat707mipctab.h"
#include "dsat707extctab.h"
#include "dsat707faxctab.h"
#endif /*FEATURE_DATA_IS707*/

#ifdef FEATURE_DSAT_GOBI_MAINLINE 
#include "ftm.h"
#include "ftm_diagpkt.h"
#include "gobi_nv.h"
#include "gobi_im_common.h"
#include "ftm_rf_cmd.h"
#include "bsp.h"
/* Variable to find out what command is being processed currently */
#endif /* FEATURE_DSAT_GOBI_MAINLINE */

#include "ds_rmnet_defs.h"
#include "ds_qmi_defs.h"
#include "ds_qmux.h"
#include "ds_profile.h"
#include "ps_iface.h"
#include "ps_iface_defs.h"
#include "sd.h"
#include "ds3gsiolib_ex.h"

#include "lte_rrc_ext_api.h"

#include "sys_m_reset.h"
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

#define RMNET_NUM_LAPTOP_INSTANCES            (RMNET_DUAL_IP_MAX)
#define RMNET_IS_VALID_LAPTOP_INSTANCE(inst)  (TRUE)
#define RMNET_GET_LAPTOP_INSTANCE(inst)       ((inst) / 2)
#define RMNET_INSTANCE_LAPTOP_PROC_MIN        (0)

#define MAX_PARAM_BUF                  100
#define DEF_MAX_APN_DISABLE_VAL        1
#define DEF_MAX_PDN_CONN_PER_BLK       1023
#define DEF_MAX_PDN_REQ_WAIT_INTERVAL  1023
#define DEF_MAX_PDN_CONN_TIME          300 //300 seconds 
#define DEF_PDN_REQ_WAIT_TIME          0 //0 seconds
#define DEF_MAX_INACTIVITY_TIMER_VAL   122820
#define MAX_PDN_CONN_TIME              3600
#define MIN_OPRT_RESV_PCO              65280 /* FF00H to FFFFH reserved for operator specific use spec 3GPP TS 24.008 Table 10.5.154*/
#define MAX_OPRT_RESV_PCO              65535
#define MAX_MNC_MCC_VAL                999

#define MAX_DNS_SIZE 18
dsat_string_item_type dsat_qcdnssec_val[MAX_DNS_SIZE];

/* RSSI antenna level for $QCANTE*/
typedef enum
{
  RSSI_LEVEL_INVALID = -1,
  RSSI_LEVEL_0 = 0,
  RSSI_LEVEL_1,
  RSSI_LEVEL_2,
  RSSI_LEVEL_3,
  RSSI_LEVEL_4
}dsat_ante_rssi_level_e_type;

/* PACSP command  */
extern boolean dsat_pcasp_cmd_pending;
/* PREFMODE global variable */

dsat_qcrmcall_s_type dsat_qcrmcall_info;

/* ==================================MRU Related======================================= */

#define DSAT_CDMA_RAT 0
#define DSAT_HDR_RAT 1
#define DSAT_GSM_RAT 2
#define DSAT_WCDMA_RAT 3
#define DSAT_LTE_RAT 4

#define DSAT_MRU_BAND_INPUT_3GPP2_BC2 2
#define DSAT_MRU_BAND_INPUT_3GPP2_MAX 20

typedef struct
{
  dsat_num_item_type index;
  dsat_num_item_type rat;
  dsat_num_item_type band;
  char band_pref[100];
  dsat_num_item_type channel;
  sys_plmn_id_s_type     plmn;
  boolean is_plmn_valid;
}dsat_mru_input_s_type;

dsat_mru_input_s_type dsat_mru_input;

LOCAL dsat_num_item_type dsat_mru_read_index;

LOCAL dsat_result_enum_type dsat_mru_result;

const LOCAL dsat_mru_band_pref_map_gw_s_type mru_band_pref_map_gw[] =
{
  { DSAT_MRU_BAND_PREF_GSM_450            ,     "0"},
  { DSAT_MRU_BAND_PREF_GSM_480            ,     "1"},
  { DSAT_MRU_BAND_PREF_GSM_750            ,     "2"},
  { DSAT_MRU_BAND_PREF_GSM_850            ,     "3"},
  { DSAT_MRU_BAND_PREF_GSM_EGSM_900       ,     "4"},
  { DSAT_MRU_BAND_PREF_GSM_PGSM_900       ,     "5"},
  { DSAT_MRU_BAND_PREF_GSM_RGSM_900       ,     "6"},
  { DSAT_MRU_BAND_PREF_GSM_DCS_1800       ,     "7"},
  { DSAT_MRU_BAND_PREF_GSM_PCS_1900       ,     "8"},
  { DSAT_MRU_BAND_PREF_WCDMA_I_IMT_2000   ,     "0"},
  { DSAT_MRU_BAND_PREF_WCDMA_II_PCS_1900  ,     "1"},
  { DSAT_MRU_BAND_PREF_WCDMA_III_1700     ,     "2"},
  { DSAT_MRU_BAND_PREF_WCDMA_IV_1700      ,     "3"},
  { DSAT_MRU_BAND_PREF_WCDMA_V_850        ,     "4"},
  { DSAT_MRU_BAND_PREF_WCDMA_VI_800       ,     "5"},
  { DSAT_MRU_BAND_PREF_WCDMA_VII_2600     ,     "6"},
  { DSAT_MRU_BAND_PREF_WCDMA_VIII_900     ,     "7"},
  { DSAT_MRU_BAND_PREF_WCDMA_IX_1700      ,     "8"},
  { DSAT_MRU_BAND_PREF_WCDMA_XI_1500      ,     "9"}
};

const LOCAL dsat_mru_band_pref_map_lte_s_type mru_band_pref_map_lte[] =
{
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_1       ,     "0"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_2       ,     "1"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_3       ,     "2"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_4       ,     "3"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_5       ,     "4"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_6       ,     "5"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_7       ,     "6"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_8       ,     "7"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_9       ,     "8"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_10      ,     "9"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_11      ,     "10"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_12      ,     "11"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_13      ,     "12"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_14      ,     "13"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_17      ,     "14"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_18      ,     "15"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_19      ,     "16"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_20      ,     "17"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_21      ,     "18"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_33      ,     "19"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_34      ,     "20"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_35      ,     "21"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_36      ,     "22"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_37      ,     "23"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_38      ,     "24"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_39      ,     "25"},
  { DSAT_MRU_BAND_PREF_LTE_EUTRAN_40      ,     "26"}
};

/* ==================================MRU Related======================================= */

/*-------------------------------------------------------------------------
    Function Definitions:
-------------------------------------------------------------------------*/

/* ==================================MRU Related functions======================================= */
LOCAL uint32 vend_convert_band_pref_to_sys_band32
(
  uint32 dsat_band_pref,
  char *band_ptr,
  uint32 src_size
);
LOCAL uint32 vend_convert_lte_band_pref_to_sys_band
(
  uint64 dsat_band_pref,
  char *band_ptr,
  uint32 src_size
);

LOCAL boolean vend_convert_mru
(
  cm_mmode_mru_table_entry_u_type *dsat_mru_entry,
  dsat_mru_input_s_type           *mru_data
);

LOCAL uint32 vend_convert_mru_band_input_to_gsm_band_pref
(
  dsat_mru_band_input_gsm_e_type band
);

LOCAL uint32 vend_convert_mru_band_input_to_wcdma_band_pref
(
  dsat_mru_band_input_wcdma_e_type band
);

LOCAL uint64 vend_convert_mru_band_input_to_lte_band_pref
(
  dsat_mru_band_input_lte_e_type band
);

LOCAL boolean vend_validate_mru_inputs
(
  dsat_num_item_type index,
  dsat_num_item_type rat,
  dsat_num_item_type band
);

LOCAL boolean vend_mru_change_sys_oprt_mode
(
  sys_oprt_mode_e_type new_mode
);

LOCAL dsat_result_enum_type dsatvend_mru_read_cm_cmd
(
  dsat_num_item_type index
);

/* ==================================MRU Related functions======================================= */

LOCAL dsat_result_enum_type vend_convert_ph_pref_type
(
  boolean to_at_state,
  dsat_sysconfig_type *at_pref_ptr,
  ds_at_cm_ph_pref_type *cm_pref_ptr
);

LOCAL void clr_err_log( void);

/* PREFMODE global variable */
#define DSAT_BANDPREF_NON_PERS 0
#define DSAT_BANDPREF_PERS 1

dsat_num_item_type dsat_bandpref_pers_val;
dsat_bandpref_e_type  dsat_bandpref_val[(uint32)DSAT_MAX_BANDPREF+1];

LOCAL const char *dsat_qcsysmode_text[DSAT_QCSYSMODE_ID_MAX] =
{
  "NOT DETERMINED",
  "CDMA",
  "GSM",
  "HDR",
  "WCDMA",
  "LTE",
  "TDS",
  "HDR - REV0",
  "HDR - REVA",
  "HDR - REVB",
  "HDR - EMPA EHRPD",
  "HDR - MMPA EHRPD",
  "CDMA and HDR",
  "CDMA and LTE",
};

LOCAL const char *dsat_qcsysmode_hs_ind[DSAT_QCSYSMODE_HS_IND_MAX] =
{
  "",
  " - HSDPA",
  " - HSUPA",
  " - HSDPA and HSUPA",
  " - HSDPA+",
  " - HSDPA+ and HSUPA",
  " - DC HSDPA+",
  " - DC HSDPA+ and HSUPA",
  " - 64QAM HSDPA+",
  " - 64QAM HSDPA+ and HSUPA",
  " - DC HSDPA+ and DC HSUPA"
};

#ifdef FEATURE_DSAT_GOBI_MAINLINE
typedef struct dsatdvend_proc_qcagc_s
{
   int16            ftm_dispatch_id;
   int16            ftm_dispatch_div_id;
   int8             enable_rx_diversity;
   ftm_mode_id_type mode_band;
   uint16           lna; 
   uint16           path;
   union
   {
     struct
     {
       uint16           tx_chan;        
       uint16           rx_chan;
   int16            vga_gain_offset;
   int8             vga_vs_chan_freq;
     } etsi;
     struct
     {
       uint16           chan;        
   int16            rx_agc_range;
   int16            rx_min_rssi;
     } cdma;
   } sys_mode;
} dsatdvend_proc_qcagc_type;

/*============================================================================

   Local definitions and declarations used by dsatvend_ftm* functions
   
===========================================================================*/
typedef dsat_result_enum_type (dsatvend_ftm_diagpkt_cb_f_type) 
                              (ftm_pkt_type *ftm_pkt, void *rsp_arg);

typedef struct dsatvend_ftm_diagpkt_rsp_params_s {
   ftm_pkt_type   ftm_diagpkt;
   dsatvend_ftm_diagpkt_cb_f_type 
                 *rsp_cb;
   void          *rsp_arg;
} dsatvend_ftm_diagpkt_rsp_params_type;

static dsatvend_ftm_diagpkt_rsp_params_type 
            dsatvend_ftm_diagpkt_rsp_params = {NULL, NULL};

typedef void (ftm_diagpkt_rsp_cb_f_type)(char *rsp_ftm_pkt);

void dsatvend_ftm_diagpkt_fwd_cb(char *rsp_ftm_pkt);


/*===========================================================================

Local definitions and declarations used by dsatvend_exec_qcallup_* functions

============================================================================*/
typedef struct dsatdvend_proc_qcallup_s {
   int              ftm_dispatch_id;
   ftm_mode_id_type mode_band;
   int8             tx_enable;
   unsigned int     tx_chan;        
   unsigned int     pdm;
} dsatdvend_proc_qcallup_type;

/* Variable to find out what command is being processed currently */
#endif /* FEATURE_DSAT_GOBI_MAINLINE */

/*==============================================================================
             Profile Related Structures and Mappings
================================================================================*/

#define DSAT_3GPP2_PARAM_TYPE_MAX_LENGTH 10
#define DSAT_3GPP2_PARAM_VALUE_MAX_LENGTH 150

/* $QCPRFCRT Values */
#define DSAT_3GPP2_PROFILE_CREATE                  1
#define DSAT_3GPP2_PROFILE_DELETE                  2

#define DSAT_NON_PERSISTENT 0
#define DSAT_PERSISTENT 1

#define DSAT_MAX_PROFILE_PARAM_VAL_LEN 150
#define DSAT_MAX_PROFILE_PARAM_TYPE_LEN 20

#define MAX_MNC_MCC_VAL                 999
#define RANGE_END  0
#define MIN_OPRT_RESV_PCO             65280 /* FF00H to FFFFH reserved for operator specific use spec 3GPP TS 24.008 Table 10.5.154*/
#define MAX_OPRT_RESV_PCO             65535
#define DSAT_MAX_PDP_TYPE_NUM                 4
#define DSAT_MAX_PDP_TYPE_LEN                 8

typedef enum
{
  DSAT_PROFILE_CREATE_MASK_NO_PARAM            =   0,
  DSAT_PROFILE_CREATE_MASK_OPRT                =   1 << 0,   /* 1 */
  DSAT_PROFILE_CREATE_MASK_TECH                =   1 << 1,   /* 2 */
  DSAT_PROFILE_CREATE_MASK_PERSIST             =   1 << 2,   /* 4 */
  DSAT_PROFILE_CREATE_MASK_PROFILE_ID          =   1 << 3,   /* 8 */
  DSAT_PROFILE_CREATE_MASK_APN_NAME            =   1 << 4,   /* 16 */
  DSAT_PROFILE_CREATE_MASK_PDP_TYPE            =   1 << 5,   /* 32 */
  DSAT_PROFILE_CREATE_MASK_ALL                 =   0XFF
  
}dsat_profile_create_param_present_mask_enum_type;

/*------------------------------------------------------------------------ 
         Structure to store info related to profile creation
--------------------------------------------------------------------------*/
typedef struct
{
  dsat_profile_create_param_present_mask_enum_type  param_present;
  uint8                                             oprt_val;
  uint8                                             tech_val;
  uint8                                             persistent_val;
  uint8                                             profile_id_val;
  char                                              pdp_type[DSAT_MAX_PDP_TYPE_LEN+2];                  /*"" + NULL charaacter*/
  char                                              apn_name[DS_PROFILE_3GPP2_APN_MAX_VAL_LEN+3]; /*"" + NULL charaacter*/
}dsat_3gpp2_profile_create_parameter_s_type;

typedef enum
{
  DSAT_3GPP2_PARAM_OPRT_TYPE          = 0,
  DSAT_3GPP2_PARAM_TECH_TYPE          = 1,
  DSAT_3GPP2_PARAM_PERSISTENT         = 2,
  DSAT_3GPP2_PARAM_PROFILE_ID         = 3,
  DSAT_3GPP2_PARAM_APN_NAME           = 4,
  DSAT_3GPP2_PARAM_PDP_TYPE           = 5
}dsat_3gpp2_profile_create_param_e_type;


typedef struct
{
  char*                                    param_abbrv;
  dsat_3gpp2_profile_create_param_e_type   param_type;  
}dsat_3gpp2_profile_param_abbrv_type_map_s_type;

const dsat_3gpp2_profile_param_abbrv_type_map_s_type dsat_3gpp2_profile_param_abbrv_type[]=
{
  {  "OPRT"        ,   DSAT_3GPP2_PARAM_OPRT_TYPE  },
  {  "TECH"        ,   DSAT_3GPP2_PARAM_TECH_TYPE  }, 
  {  "PERSIST"     ,   DSAT_3GPP2_PARAM_PERSISTENT }, 
  {  "PID"         ,   DSAT_3GPP2_PARAM_PROFILE_ID },
  {  "APNNAME"     ,   DSAT_3GPP2_PARAM_APN_NAME },
  {  "PDPTYPE"     ,   DSAT_3GPP2_PARAM_PDP_TYPE }
};

const dsat_string_item_type dsat_pp_pdptype [5][8] =
  { "IP", "PPP", "IPV6","IPV4V6","" };

const dsat_string_item_type dsat_p2_pdptype [4][8] =
  { "IP", "IPV6","IPV4V6","" };


const dsat_string_item_type dsat_pp_failure_timers [11][10] =
  { "PPFLRTMR1", "PPFLRTMR2","PPFLRTMR3","PPFLRTMR4",
     "PPFLRTMR5","PPFLRTMR6","PPFLRTMR7","PPFLRTMR8",
     "PPFLRTMR9","PPFLRTMR10","" };

typedef struct
{
  uint32 lower;
  uint32 upper;
}dsat_profile_range_s_type;

dsat_profile_range_s_type boolean_range[] =
{
  {FALSE,TRUE},
  {RANGE_END , RANGE_END}
};

dsat_profile_range_s_type apn_class_range[]=
{
  {DSAT_APN_CLASS_ONE, DSAT_APN_CLASS_SIX},
  {DSAT_APN_CLASS_MAX, DSAT_APN_CLASS_MAX},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type uint8_range[]=
{
  {0, 255},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type uint32_range[]=
{
  {0, 0xFFFFFFFF},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type uint16_range[]=
{
  {0, 0xFFFF},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type mnc_mcc_range[]=
{
  {0, MAX_MNC_MCC_VAL},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type auth_type_range[]=
{
  {DS_PROFILE_3GPP2_AUTH_PROTOCOL_NONE, DS_PROFILE_3GPP2_AUTH_PROTOCOL_PAP_CHAP},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type pdp_header_comp_e_type_range[]=
{
  {DS_PROFILE_3GPP_PDP_HEADER_COMP_OFF, DS_PROFILE_3GPP_PDP_HEADER_COMP_RFC3095},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type data_comp_e_type_range[]=
{
  {DS_PROFILE_3GPP_PDP_DATA_COMP_OFF, DS_PROFILE_3GPP_PDP_DATA_COMP_V44},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type ipv4_addr_alloc_range[]=
{
  {DS_PROFILE_3GPP_PDP_IPV4_ADDR_ALLOC_NAS, DS_PROFILE_3GPP_PDP_IPV4_ADDR_ALLOC_DHCPV4},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type pdn_inactivity_to_range[]=
{
  {0, 71582}, /* Provided by 3GPP2*/
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type datarate_range[]=
{
  {DS_PROFILE_3GPP2_DATARATE_LOW, DS_PROFILE_3GPP2_DATARATE_HIGH},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type datamode_range[]=
{
  {DS_PROFILE_3GPP2_DATAMODE_CDMA_HDR, DS_PROFILE_3GPP2_DATAMODE_HDR_ONLY},
  {RANGE_END, RANGE_END}
};

/*This is max range of length*/
dsat_profile_range_s_type pdn_type_range[]=
{
  {DSAT_MAX_PDP_TYPE_LEN, DSAT_MAX_PDP_TYPE_LEN},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type rat_range[]=
{
  {DS_PROFILE_3GPP2_RAT_TYPE_HRPD, DS_PROFILE_3GPP2_RAT_TYPE_HRPD_EHRPD},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type op_pco_range[]=
{
  {MIN_OPRT_RESV_PCO, MAX_OPRT_RESV_PCO},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type apn_str_len[]=
{
  {DS_PROFILE_3GPP2_APN_MAX_VAL_LEN + 1, DS_PROFILE_3GPP2_APN_MAX_VAL_LEN + 1},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type uid_3gpp2_str_len[]=
{
  {DS_PROFILE_3GPP2_PPP_MAX_USER_ID_LEN + 1, DS_PROFILE_3GPP2_PPP_MAX_USER_ID_LEN + 1},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type pswd_3gpp2_str_len[]=
{
  {DS_PROFILE_3GPP2_PPP_MAX_PASSWD_LEN + 1, DS_PROFILE_3GPP2_PPP_MAX_PASSWD_LEN + 1},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type qcpdp_3gpp_str_len[]=
{
  {DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN + 1, DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN + 1},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type max_v4_addr_len[]=
{
  {MAX_V4_ADDR_LEN , MAX_V4_ADDR_LEN},
  {RANGE_END, RANGE_END}
};

dsat_profile_range_s_type max_v6_addr_len[]=
{
  {MAX_IPADDR_STR_LEN, MAX_IPADDR_STR_LEN},
  {RANGE_END, RANGE_END}
};

typedef struct
{
  char*                               abbrv;
  ds_profile_identifier_type          identifier;
  uint16                              len;
  value_type_enum_type                type;
  dsat_profile_range_s_type           *range;
}dsat_profile_parameter_details_s_type;

typedef struct
{
  ds_profile_identifier_type          identifier;
  uint16                              len;
  dsat_num_item_type                  max_value;
}dsat_qcprofile_param_details_s_type;

/*==============================================================================
             EPC Profile Related Global Mapping
================================================================================*/
/*
General Abbribiation rule

Long Name
 - First two char <- From First word
 - One character  <- From intermediate word
Short Name
 - 2 or 3 chars from first word

Fixed Key words
TMR <- Timer
TOUT <- Time Out
PROT <- PROTOCOL
PR  <- PRIMARY
SE  <- SECONDARY
UID <- USER ID
PWD <- PASSWORD
PRM <- PARAMETER
FLR <- FAILURE
P2 <- PP2
P <-PP
EPC  <- EPC
C <- COM
*/
dsat_profile_parameter_details_s_type profile_3gpp2_parameter_details_list[]=
{
  /* 3GPP2 params of 3GPP2 profiles */

  {  "APPPRT"         ,     DS_PROFILE_3GPP2_PROFILE_PARAM_APP_PRIORITY,
      sizeof(ds_profile_3gpp2_app_priority_type),   NUM_TYPE,   &uint8_range[0]    },

  {  "APPTYPE"        ,     DS_PROFILE_3GPP2_PROFILE_PARAM_APP_TYPE,
      sizeof(ds_profile_3gpp2_app_type_type),   NUM_TYPE,       &uint32_range[0]    },

  {  "AUTHRCNT"        ,     DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_RETRY_COUNT,
     sizeof(ds_profile_3gpp2_auth_retry_count_type),  NUM_TYPE, &uint8_range[0]    },

  {  "AUTHTO"          ,      DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_TIMEOUT,
      sizeof(ds_profile_3gpp2_auth_timeout_type),   NUM_TYPE,   &uint16_range[0]    },	 

  {  "DMODE"          ,      DS_PROFILE_3GPP2_PROFILE_PARAM_DATA_MODE,
     sizeof(ds_profile_3gpp2_data_mode_type),   NUM_TYPE,   &datamode_range[0]  },
 
  {  "DRATE"          ,      DS_PROFILE_3GPP2_PROFILE_PARAM_DATA_RATE,
      sizeof(ds_profile_3gpp2_data_rate_type),   NUM_TYPE,   &datarate_range[0] },
 
  {  "DISATMR1"        ,        DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_1,
     sizeof(ds_profile_3gpp2_disallow_timer_type),   NUM_TYPE,   &uint32_range[0]  },
 
  {  "DISATMR2"        ,        DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_2,
     sizeof(ds_profile_3gpp2_disallow_timer_type),   NUM_TYPE,   &uint32_range[0]  },
 
  {  "DISATMR3"        ,        DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_3,
     sizeof(ds_profile_3gpp2_disallow_timer_type),   NUM_TYPE,   &uint32_range[0]  },
 
  {  "DISATMR4"        ,        DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_4,
     sizeof(ds_profile_3gpp2_disallow_timer_type),   NUM_TYPE,   &uint32_range[0]  },
  
  {  "DISATMR5"        ,        DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_5,
      sizeof(ds_profile_3gpp2_disallow_timer_type),   NUM_TYPE,   &uint32_range[0]  },
  
  {  "DISATMR6"        ,        DS_PROFILE_3GPP2_PROFILE_PARAM_DISALLOW_TIMER_6,
     sizeof(ds_profile_3gpp2_disallow_timer_type),   NUM_TYPE,   &uint32_range[0]   },

  {  "FLRTMR1"          ,        DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_1,
     sizeof(ds_profile_3gpp2_failure_timer_type),   NUM_TYPE,  &uint32_range[0]   },
 
  {  "FLRTMR2"          ,        DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_2 ,
     sizeof(ds_profile_3gpp2_failure_timer_type),   NUM_TYPE,  &uint32_range[0] },
 
  {  "FLRTMR3"          ,        DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_3,
     sizeof(ds_profile_3gpp2_failure_timer_type),   NUM_TYPE,  &uint32_range[0]  },
 
  {  "FLRTMR4"          ,        DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_4,
     sizeof(ds_profile_3gpp2_failure_timer_type),   NUM_TYPE,  &uint32_range[0] },
 
  {  "FLRTMR5"          ,        DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_5,
     sizeof(ds_profile_3gpp2_failure_timer_type),   NUM_TYPE,  &uint32_range[0]  },
 
  {  "FLRTMR6"          ,        DS_PROFILE_3GPP2_PROFILE_PARAM_FAILURE_TIMER_6,
     sizeof(ds_profile_3gpp2_failure_timer_type),   NUM_TYPE,  &uint32_range[0]  },
   
  {  "IPACK_TO"          ,        DS_PROFILE_3GPP2_PROFILE_PARAM_IPCP_ACK_TIMEOUT,
     sizeof(ds_profile_3gpp2_ipcp_ack_timeout_type),   NUM_TYPE,   &uint16_range[0]  },
 
  {  "IPCRCNT"          ,        DS_PROFILE_3GPP2_PROFILE_PARAM_IPCP_CREQ_RETRY_COUNT,
      sizeof(ds_profile_3gpp2_ipcp_creq_retry_count_type),   NUM_TYPE,   &uint8_range[0]  },	
  
  {  "LACKTO"           ,        DS_PROFILE_3GPP2_PROFILE_PARAM_LCP_ACK_TIMEOUT,
      sizeof(ds_profile_3gpp2_lcp_ack_timeout_type),   NUM_TYPE,    &uint16_range[0]  },

  {  "LCRCNT"           ,        DS_PROFILE_3GPP2_PROFILE_PARAM_LCP_CREQ_RETRY_COUNT,
     sizeof(ds_profile_3gpp2_lcp_creq_retry_count_type),   NUM_TYPE,   &uint8_range[0]   },
 
  {  "DNSFLG"             ,         DS_PROFILE_3GPP2_PROFILE_PARAM_NEGOTIATE_DNS_SERVER,
     sizeof(ds_profile_3gpp2_negotiate_dns_server_type),    NUM_TYPE,   &boolean_range[0]  },	 
     
  {  "PINACTTO"        ,         DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_INACTIVITY_TIMEOUT,
     sizeof(uint32),    NUM_TYPE,    &pdn_inactivity_to_range[0]  },  
 
  {  "PDNLBL"        ,         DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_LABEL,
      0,     STR_TYPE,   &apn_str_len[0]   },
  
  {  "PDNAPROT"       ,        DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PROTOCOL,
      sizeof(uint8),      NUM_TYPE,     &auth_type_range[0]},	 
  
  {  "PDNAPWD"        ,        DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_LEVEL_AUTH_PASSWORD,
      0,     STR_TYPE,   &pswd_3gpp2_str_len[0]   },
  
  {  "PDNUID"         ,       DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_LEVEL_USER_ID,
      0,     STR_TYPE,   &uid_3gpp2_str_len[0]   },
  
  {  "PDNTYPE"         ,       DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_TYPE,
     0                 ,       STR_TYPE, &pdn_type_range[0]}, 
    
  {  "RAT"          ,         DS_PROFILE_3GPP2_PROFILE_PARAM_RAT_TYPE,
     sizeof(ds_profile_3gpp2_rat_type_enum_type),   NUM_TYPE,   &rat_range[0] },
 
  {  "SCT1X"          ,      DS_PROFILE_3GPP2_PROFILE_PARAM_SESSION_CLOSE_TIMER_1X,
      sizeof(uint32),   NUM_TYPE,    &uint32_range[0]  },
  
  {  "SCTDO"          ,      DS_PROFILE_3GPP2_PROFILE_PARAM_SESSION_CLOSE_TIMER_DO,
      sizeof(uint32),   NUM_TYPE,    &uint32_range[0]},
  
  /* 3GPP2 common params of EPC and 3GPP2 profiles.
     Names of param(s) start with P2 + <param_name>. This list include all 3GPP2 params
     that have a similar param in common and in 3GPP set*/

    /*P2+ <PARAM_NAME> START*/
  {  "P2APNCLASS"          ,   DS_PROFILE_3GPP2_PROFILE_PARAM_APN_CLASS ,
     sizeof(ds_profile_apn_class_type),    NUM_TYPE,    &apn_class_range[0] },

  {  "P2APNFLAG"          ,   DS_PROFILE_3GPP2_PROFILE_PARAM_APN_ENABLED,
     sizeof(ds_profile_apn_disable_flag_type),   NUM_TYPE,   &boolean_range[0] },

  {  "P2AUPWD"          ,   DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_PASSWORD,
     0  ,    STR_TYPE,    &pswd_3gpp2_str_len[0] },

  {  "P2AUPROT"          ,   DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_PROTOCOL ,
     sizeof(uint8),    NUM_TYPE,   &auth_type_range[0]  },

  {  "P2LNGRTMR"          ,   DS_PROFILE_3GPP2_PROFILE_PARAM_LINGER_PARAMS ,
      sizeof(ds_profile_linger_params_type),    NUM_TYPE,    &uint16_range[0]   },  
    
  {  "P2MCC"          ,   DS_PROFILE_3GPP2_PROFILE_PARAM_MCC,
     sizeof(uint16),    NUM_TYPE,    &mnc_mcc_range[0]  },

  {  "P2MNC"          ,   DS_PROFILE_3GPP2_PROFILE_PARAM_MNC,
     sizeof(ds_profile_3gpp2_mnc_type),   NUM_TYPE,  &mnc_mcc_range[0]  },

  {  "P2OPRPCO"     ,   DS_PROFILE_3GPP2_PROFILE_PARAM_OP_PCO_ID,
     sizeof(uint16),    NUM_TYPE,    &op_pco_range[0]  }, 

  {  "P2PCRFLG"       ,   DS_PROFILE_3GPP2_PROFILE_PARAM_IS_PCSCF_ADDR_NEEDED,
     sizeof(boolean),   NUM_TYPE,   &boolean_range[0] },

  {  "P2UID"         ,   DS_PROFILE_3GPP2_PROFILE_PARAM_USER_ID ,
     0  ,    STR_TYPE,    &uid_3gpp2_str_len[0] },

  {  "P2V4DNSPR"          ,   DS_PROFILE_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY ,
     sizeof(ds_profile_in_addr_type),    ADDR_V4_TYPE,  &max_v4_addr_len[0]   },

  {  "P2V4DNSSE"          ,   DS_PROFILE_3GPP2_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY ,
      sizeof(ds_profile_in_addr_type),    ADDR_V4_TYPE,  &max_v4_addr_len[0]  },

  {  "P2V6DNSPR"          ,   DS_PROFILE_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_PRIMARY,
      sizeof(ds_profile_in6_addr_type),    ADDR_P2_V6_TYPE,  &max_v6_addr_len[0]   },
	
  {  "P2V6DNSSE"          ,   DS_PROFILE_3GPP2_PROFILE_PARAM_V6_DNS_ADDR_SECONDARY ,
      sizeof(ds_profile_in6_addr_type),    ADDR_P2_V6_TYPE,  &max_v6_addr_len[0]   },

  {  "P2UAPPDATA"          ,    DS_PROFILE_3GPP2_PROFILE_PARAM_USER_APP_DATA,
      sizeof(uint32),    NUM_TYPE,  &uint32_range[0]  }

  /*P2+ <PARAM_NAME> END*/
}; /*profile_3gpp2_parameter_details_list*/

/*==============================================================================
EPC Profile Related Global Mapping: EPC will contain common,3gpp and 3gpp2 param
================================================================================*/


dsat_profile_parameter_details_s_type profile_epc_parameter_details_list[]=
{
    /*MAX Size is same as 3GPP2 PDP ENUM*/
  {  "CPDPTYPE"          ,    DS_PROFILE_TECH_COMMON_PROFILE_PARAM_PDP_TYPE, 
      0                  ,    STR_TYPE,  &pdn_type_range[0]  },

  {  "CAPNCLASS"          ,   DS_PROFILE_TECH_COMMON_PROFILE_PARAM_APN_CLASS,
     sizeof(ds_profile_apn_class_type),    NUM_TYPE,    &apn_class_range[0] },
 
  {  "CAPNFLAG"          ,   DS_PROFILE_TECH_COMMON_PROFILE_PARAM_APN_DISABLED ,
     sizeof(ds_profile_apn_disable_flag_type),   NUM_TYPE,   &boolean_range[0] },  
     
  {  "CAUPWD"          ,   DS_PROFILE_TECH_COMMON_PROFILE_PARAM_AUTH_PASSWORD ,
     0  ,    STR_TYPE,    &qcpdp_3gpp_str_len[0] },   
 
  {  "CAUPROT"          ,   DS_PROFILE_TECH_COMMON_PROFILE_PARAM_AUTH_PROTOCOL,
     sizeof(uint8),    NUM_TYPE,   &auth_type_range[0]  },
 
  {  "CLNGRTMR"          ,   DS_PROFILE_TECH_COMMON_PROFILE_PARAM_LINGER_PARAMS ,
      sizeof(ds_profile_linger_params_type),    LNGR_TMR_TYPE,    &uint16_range[0]   },   
  
  {  "CMCC"          ,   DS_PROFILE_TECH_COMMON_PROFILE_PARAM_MCC,
     sizeof(ds_profile_mcc_type),    NUM_TYPE,    &mnc_mcc_range[0] } ,
 
  {  "CMNC"          ,   DS_PROFILE_TECH_COMMON_PROFILE_PARAM_MNC,
     sizeof(ds_profile_mnc_type),    NUM_TYPE,    &mnc_mcc_range[0]},
 
  {  "COPRPCO"      ,  DS_PROFILE_TECH_COMMON_PROFILE_PARAM_OPERATOR_RESERVED_PCO,
     sizeof(ds_profile_op_pco_id_type),    NUM_TYPE,    &op_pco_range[0]  }, 
 
  {  "CPCRFLG"        ,   DS_PROFILE_TECH_COMMON_PROFILE_PARAM_REQUEST_PCSCF_REQ_FLAG,
     sizeof(ds_profile_is_pcscf_req_flag_type),   NUM_TYPE,   &boolean_range[0] },
 
  {  "CUID"          ,   DS_PROFILE_TECH_COMMON_PROFILE_PARAM_USER_ID ,
     0  ,    STR_TYPE,    &qcpdp_3gpp_str_len[0]},  
 
  {  "CV4DNSPR"          ,   DS_PROFILE_TECH_COMMON_PROFILE_PARAM_V4_DNS_ADDR_PRIMARY ,
     sizeof(ds_profile_in_addr_type),    ADDR_V4_TYPE,  &max_v4_addr_len[0] },  
  
  {  "CV4DNSSE"          ,   DS_PROFILE_TECH_COMMON_PROFILE_PARAM_V4_DNS_ADDR_SECONDARY ,
      sizeof(ds_profile_in_addr_type),    ADDR_V4_TYPE,  &max_v4_addr_len[0]    },
    
  {  "CV6DNSPR"          ,   DS_PROFILE_TECH_COMMON_PROFILE_PARAM_V6_DNS_ADDR_PRIMARY ,
      sizeof(ds_profile_in6_addr_type),    ADDR_C_V6_TYPE,  &max_v6_addr_len[0]  }, 
   
  {  "CV6DNSSE"          ,   DS_PROFILE_TECH_COMMON_PROFILE_PARAM_V6_DNS_ADDR_SECONDARY ,
     sizeof(ds_profile_in6_addr_type),    ADDR_C_V6_TYPE,   &max_v6_addr_len[0]  },

  {  "CUAPPDATA"          ,    DS_PROFILE_TECH_COMMON_PROFILE_PARAM_USER_APP_DATA,
      sizeof(uint32),    NUM_TYPE,  &uint32_range[0]  }
};/*profile_epc_parameter_details_list*/

dsat_profile_parameter_details_s_type profile_3gpp_parameter_details_list[]=
{
  {  "PDPCOPID"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PRIMARY_ID,
      sizeof(uint8),    NUM_TYPE,  &uint8_range[0]  },

  {  "PDPCOSFLG"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_SECONDARY_FLAG,
      sizeof(boolean),    NUM_TYPE,  &boolean_range[0]  },

  {  "PDNCOBLK"          ,    DS_PROFILE_3GPP_PROFILE_MAX_PDN_CONN_PER_BLOCK,
      sizeof(uint16),    NUM_TYPE,  &uint16_range[0]  },

  {  "PDNCOTMR"          ,    DS_PROFILE_3GPP_PROFILE_MAX_PDN_CONN_TIMER,
      sizeof(uint16),    NUM_TYPE,  &uint16_range[0]  },

  {  "APNBR"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_APN_BEARER,
      sizeof(uint8),    NUM_TYPE,  &uint8_range[0]  },

  {  "EMERCALL"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_EMERGENCY_CALLS_SUPPORTED,
      sizeof(boolean),    NUM_TYPE,  &boolean_range[0]  },

  {  "IMCNFLAG"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_IM_CN_FLAG,
      sizeof(boolean),    NUM_TYPE,  &boolean_range[0]  },

  {  "INACTTO"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_INACTIVITY_TIMER_VAL,
      sizeof(uint32),    NUM_TYPE,  &uint32_range[0]  },

  {  "PCSCFDHCP"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG,
      sizeof(boolean),    NUM_TYPE,  &boolean_range[0]  },

  {  "PDPDCOMP"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_D_COMP,
      sizeof(ds_profile_3gpp_pdp_data_comp_e_type),    NUM_TYPE,  &data_comp_e_type_range[0]  },

  {  "PDPHCOMP"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_H_COMP,
      sizeof(ds_profile_3gpp_pdp_header_comp_e_type),  NUM_TYPE,  &pdp_header_comp_e_type_range[0]  },

  {  "PDPCONUM"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_NUMBER,
      sizeof(uint8),    NUM_TYPE,  &uint8_range[0]  },

  {  "PDPV4"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4,
      sizeof(ds_profile_in_addr_type),    ADDR_V4_TYPE,  &max_v4_addr_len[0]  },

  {  "PDPV6"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V6,
      sizeof(ds_profile_in6_addr_type),    ADDR_C_V6_TYPE,  &max_v6_addr_len[0]  },

  {  "PDPTYPE"        ,    DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE, 
      0               ,    STR_TYPE,  &pdn_type_range[0] },

  {  "RODISALLW"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_ROAMING_DISALLOWED,
      sizeof(boolean),    NUM_TYPE,  &boolean_range[0]  },

  {  "PDNREQTMR"          ,    DS_PROFILE_3GPP_PROFILE_PDN_REQ_WAIT_TIMER,
      sizeof(uint16),    NUM_TYPE,  &uint16_range[0]  },

  {  "PDNDTMR"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_PDN_DISCON_WAIT_TIME,
      sizeof(uint8),    NUM_TYPE,  &uint8_range[0]  },

  {  "V4ALLOC"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_IPV4_ADDR_ALLOC,
      sizeof(ds_profile_3gpp_pdp_ipv4_addr_alloc_e_type),    NUM_TYPE,  &ipv4_addr_alloc_range[0]  },

  {  "IWLANTOLTERO"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_IWLAN_TO_LTE_ROAMING_HO_FLAG,
      sizeof(boolean)        ,    NUM_TYPE,  &boolean_range[0]  },

  {  "LTETOIWLANRO"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_LTE_TO_IWLAN_ROAMING_HO_FLAG,
      sizeof(boolean)        ,    NUM_TYPE,  &boolean_range[0]  },

  {  "OVRRIDEHOPDP"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE, 
      0                      ,    STR_TYPE,  &pdn_type_range[0]  },
   
   /*Using PP+FLRTMR to differentiate between 3GPP2 failure timers and 3GPP PDN throttling timers*/
  {  (char *)dsat_pp_failure_timers[0]          ,    DS_PROFILE_3GPP_PROFILE_PARAM_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[1]          ,    DS_PROFILE_3GPP_PROFILE_PARAM_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[2]          ,    DS_PROFILE_3GPP_PROFILE_PARAM_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[3]          ,    DS_PROFILE_3GPP_PROFILE_PARAM_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[4]          ,    DS_PROFILE_3GPP_PROFILE_PARAM_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[5]          ,    DS_PROFILE_3GPP_PROFILE_PARAM_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[6]          ,    DS_PROFILE_3GPP_PROFILE_PARAM_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[7]          ,    DS_PROFILE_3GPP_PROFILE_PARAM_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[8]          ,    DS_PROFILE_3GPP_PROFILE_PARAM_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

  {  (char *)dsat_pp_failure_timers[9]          ,    DS_PROFILE_3GPP_PROFILE_PARAM_FAILURE_TIMERS,
     sizeof(uint32),   NUM_TYPE,  &uint32_range[0]  },

   
  {  "LTEROPDP"              ,    DS_PROFILE_3GPP_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE, 
      0                      ,    STR_TYPE,  &pdn_type_range[0]  },

  {  "UMTSROPDP"             ,    DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE, 
      0                      ,    STR_TYPE,  &pdn_type_range[0]  },

  /* 3GPP common params of an EPC profile (to avoid same name conflict with 3GPP params)
     Names of param(s) start with P + <param_name>. This list include all 3GPP params
     that have a similar param in common and in 3GPP2 set*/

  /*P+ <PARAM_NAME> START*/
  {  "PAPNCLASS"          ,   DS_PROFILE_3GPP_PROFILE_PARAM_APN_CLASS ,
     sizeof(ds_profile_3gpp_apn_class_type),    NUM_TYPE,    &apn_class_range[0] },
     
  {  "PAPNFLAG"          ,   DS_PROFILE_3GPP_PROFILE_PARAM_APN_DISABLE_FLAG,
     sizeof(ds_profile_apn_disable_flag_type),   NUM_TYPE,   &boolean_range[0] },

  {  "PAUPWD"          ,   DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_PASSWORD ,
     0  ,    STR_TYPE,    &qcpdp_3gpp_str_len[0] },
 
  {  "PAUPROT"          ,   DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_TYPE,
     sizeof(ds_profile_3gpp_auth_pref_type),    NUM_TYPE,   &auth_type_range[0]  },
 
  {  "PLNGRTMR"          ,   DS_PROFILE_3GPP_PROFILE_PARAM_LINGER_PARAMS ,
     sizeof(ds_profile_linger_params_type),    LNGR_TMR_TYPE,    &uint16_range[0]   },  
  
  {  "PMCC"          ,   DS_PROFILE_3GPP_PROFILE_PARAM_MCC,
     sizeof(uint16),    NUM_TYPE,    &mnc_mcc_range[0]    },
  
  {  "PMNC"          ,   DS_PROFILE_3GPP_PROFILE_PARAM_MNC,
     sizeof(ds_profile_3gpp_mnc_type),   NUM_TYPE,  &mnc_mcc_range[0]  },

  {  "POPRPCO"      ,   DS_PROFILE_3GPP_PROFILE_PARAM_OPERATOR_RESERVED_PCO,
     sizeof(uint16),    NUM_TYPE,    &op_pco_range[0]},

  {  "PPCRFLG"        ,   DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_REQ_FLAG,
     sizeof(ds_profile_3gpp_request_pcscf_address_flag_type),   NUM_TYPE,   &boolean_range[0] }, 

  {  "PUID"          ,   DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_USERNAME ,
     0  ,    STR_TYPE,    &qcpdp_3gpp_str_len[0] }, 
 
  {  "PV4DNSPR"          ,   DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_PRIMARY,
     sizeof(ds_profile_in_addr_type),    ADDR_V4_TYPE,  &max_v4_addr_len[0]   },
  
  {  "PV4DNSSE"          ,   DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V4_SECONDARY,
      sizeof(ds_profile_in_addr_type),    ADDR_V4_TYPE,  &max_v4_addr_len[0]    },
  
  {  "PV6DNSPR"          ,   DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V6_PRIMARY,
      sizeof(ds_profile_in6_addr_type),    ADDR_P_V6_TYPE,  &max_v6_addr_len[0]   },

  {  "PV6DNSSE"          ,   DS_PROFILE_3GPP_PROFILE_PARAM_DNS_ADDR_V6_SECONDARY,
      sizeof(ds_profile_in6_addr_type),    ADDR_P_V6_TYPE,  &max_v6_addr_len[0]  },

  {  "PUAPPDATA"          ,    DS_PROFILE_3GPP_PROFILE_USER_APP_DATA,
      sizeof(uint32),    NUM_TYPE,  &uint32_range[0]  }
  /*P+ <PARAM_NAME> END*/

};/*profile_3gpp_parameter_details_list*/

dsat_profile_parameter_details_s_type profile_general_parameter_details_list[]=
{
  {  "PAPNNAME"          ,    DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
      0,    STR_TYPE,  &apn_str_len[0]  },

  {  "P2APNNAME"  ,           DS_PROFILE_3GPP2_PROFILE_PARAM_APN_STRING, 
     0  ,   STR_TYPE  ,     &apn_str_len[0] }
};/*profile_general_parameter_details_list*/

dsat_qcprofile_param_details_s_type profile_qcpdpcfge_param_details_list[] = 
{
  {
    DS_PROFILE_3GPP_PROFILE_PARAM_APN_DISABLE_FLAG, 
    sizeof(ds_profile_3gpp_apn_disable_flag_type),
    DEF_MAX_APN_DISABLE_VAL
  },
  {
    DS_PROFILE_3GPP_PROFILE_PARAM_INACTIVITY_TIMER_VAL,
    sizeof(ds_profile_3gpp_inactivity_timer_val_type),
    DEF_MAX_INACTIVITY_TIMER_VAL
  },
  {
    DS_PROFILE_3GPP_PROFILE_PARAM_APN_CLASS,
    sizeof(ds_profile_3gpp_apn_class_type),
    DSAT_APN_CLASS_MAX
  },
  {
    DS_PROFILE_3GPP_PROFILE_PARAM_APN_BEARER,
    sizeof(uint8),
    DS_PROFILE_3GPP_APN_BEARER_FLAG_ALL
  },
  {
    DS_PROFILE_3GPP_PROFILE_MAX_PDN_CONN_PER_BLOCK,
    sizeof(uint16),
    DEF_MAX_PDN_CONN_PER_BLK
  },
  {
    DS_PROFILE_3GPP_PROFILE_MAX_PDN_CONN_TIMER,
    sizeof(uint16),
    MAX_PDN_CONN_TIME
  },
  {
    DS_PROFILE_3GPP_PROFILE_PDN_REQ_WAIT_TIMER,
    sizeof(uint16),
    DEF_MAX_PDN_REQ_WAIT_INTERVAL
  },
  {
    DS_PROFILE_3GPP_PROFILE_PARAM_EMERGENCY_CALLS_SUPPORTED,
    sizeof(boolean),
    1
  },
  /*Special case, check for min and max in the code*/
  {
    DS_PROFILE_3GPP_PROFILE_PARAM_OPERATOR_RESERVED_PCO,
    sizeof(uint16),
    MAX_OPRT_RESV_PCO
  },
  {
    DS_PROFILE_3GPP_PROFILE_PARAM_MCC,
    sizeof(uint16),
    MAX_MNC_MCC_VAL
  },
  {
    DS_PROFILE_3GPP_PROFILE_PARAM_MNC,
    sizeof(ds_profile_3gpp_mnc_type),
    MAX_MNC_MCC_VAL
  }
}; /*profile_qcpdpcfge_param_details_list*/

dsat_qcprofile_param_details_s_type profile_qcapne_param_details_list[] = 
{
  /*Argument 1*/
  {
    DS_PROFILE_3GPP_PROFILE_PARAM_APN_CLASS,
    sizeof(ds_profile_3gpp_apn_class_type),
    DSAT_APN_CLASS_MAX
  },
  /*Argument 5*/
  {
    DS_PROFILE_3GPP_PROFILE_PARAM_APN_DISABLE_FLAG, 
    sizeof(ds_profile_3gpp_apn_disable_flag_type),
    DEF_MAX_APN_DISABLE_VAL
  },
  /*Argument 6*/
  {
    DS_PROFILE_3GPP_PROFILE_PARAM_INACTIVITY_TIMER_VAL,
    sizeof(ds_profile_3gpp_inactivity_timer_val_type),
    DEF_MAX_INACTIVITY_TIMER_VAL
  }
}; /*profile_qcapne_param_details_list*/

dsat_qcprofile_param_details_s_type profile_qcpdpcfgext_param_details_list[] = 
{
  {
    DS_PROFILE_3GPP_PROFILE_USER_APP_DATA, 
    sizeof(ds_profile_3gpp_user_app_data),
    0xFFFFFFFF
  },
  {
    DS_PROFILE_3GPP_PROFILE_PARAM_ROAMING_DISALLOWED, 
    sizeof(boolean),
    1
  },
  {
    DS_PROFILE_3GPP_PROFILE_PARAM_PDN_DISCON_WAIT_TIME, 
    sizeof(uint8),
    255
  }
}; /*profile_qcpdpcfgext_param_details_list*/
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCCLR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcclr command.
  at$qcclr command is used for clearing mobile error log.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    OK : if the command has been successfully executed
    ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcclr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;

  /*---------------------------------------
   Processing command with no args
   ----------------------------------------*/
  if (tok_ptr->op == (NA) ) 
  {
    clr_err_log();
    result = DSAT_OK;
  }
  /*--------------------------------------
  Standard processing for a TEST command:
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    /*  NO-OP but prevents ERROR --                     */
    /*  Code at top will output                         */
    /*  the command name for at TEST command.           */
    result = DSAT_OK;
  }
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsatvend_exec_qcclr_cmd () */


/*===========================================================================
FUNCTION CLR_ERR_LOG

DESCRIPTION
  This function clears the NV error log whenever the command AT$QCCLR is
  issued.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void clr_err_log( void)
{
  byte i;
  
  /*-----------------------------------------------------------------------
    Loop through all entries in the error log and clear them.
  -------------------------------------------------------------------------*/
  for (i=0; i < NV_MAX_ERR_LOG; i++ )
  {
    (void) err_clr_log( i);
  }

} /* clr_err_log () */


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDMR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdmr command.
  at$qcdmr is used to set the DM baud rate.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdmr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  
  result = dsatparm_exec_param_cmd(
                                    mode,
                                    parse_table,
                                    tok_ptr,
                                    res_buff_ptr
                                  );
  return result;
}  /* dsatvend_exec_qcdmr_cmd() */

/*  ------------------------------------------------------------------  */



/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDNSP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdnsp command.
  at$qcdnsp is used to set primary DNS IP address.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdnsp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  uint32                ds707_qcdns_val[2]={0,0};
  
#ifdef FEATURE_DATA_IS707
  ds707_pkt_mgr_get_1x_profile_val(DS707_QCDNS_VAL,(void *)ds707_qcdns_val,2);
#else
  if((tok_ptr->op == (NA|QU)) && (NV_DONE_S == dsatutil_get_nv_item( NV_PRIMARY_DNS_I,&ds_nv_item )))
  {
    ds707_qcdns_val[0] = ds_nv_item.primary_dns;
  }
#endif /*FEATURE_DATA_IS707*/
  result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );
  
  if (DSAT_OK == result) 
  {
    if (tok_ptr->op == (NA|EQ|AR))
    {
      /*---------------------------------------------------------------
      Check for validity of the IP address & convert to internal format
      ---------------------------------------------------------------*/
      if (DSAT_OK == dsatutil_convert_tuple (STRING_TO_INT,
					     TUPLE_TYPE_IPv4,
                                             &ds707_qcdns_val[0],
					     (byte*)dsatutil_get_val(parse_table->cmd_id,0,0,STR_TYPE),
					     MAX_DNS_SIZE,
					     DSAT_CONVERT_IP) )
      {
        /*---------------------------------------------------------------
        Write Primary DNS item into the NV 
        ---------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707
        ds707_pkt_mgr_set_1x_profile_val(DS707_QCDNS_VAL,(void *)ds707_qcdns_val,2);
#endif /*FEATURE_DATA_IS707*/
        ds_nv_item.primary_dns  = ds707_qcdns_val[0];
        (void)dsatutil_put_nv_item( NV_PRIMARY_DNS_I, &ds_nv_item );
        result = DSAT_OK;
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
    else if (tok_ptr->op == (NA))
    {
      result = DSAT_OK;
    }
    else if (tok_ptr->op == (NA|QU))
    {
      /*---------------------------------------------------------------
      Format NV data to 4-tuple string
      ---------------------------------------------------------------*/
      if (DSAT_OK == dsatutil_convert_tuple (INT_TO_STRING,
                                             TUPLE_TYPE_IPv4,
                                             &ds707_qcdns_val[0],
                                             (byte*)dsatutil_get_val(
                                             DSAT_VENDOR_QCDNSP_IDX,0,0,STR_TYPE),
                                             MAX_DNS_SIZE,
                                             DSAT_CONVERT_IP) )
      {
        /* Use extended format on value query */
         res_buff_ptr->used +=
              (word)snprintf ((char*)res_buff_ptr->data_ptr,
                                    res_buff_ptr->size,
                                    "%s: %s",
                                    (char*)parse_table->name,
                                    (char*)dsatutil_get_val(
                                    DSAT_VENDOR_QCDNSP_IDX,0,0,STR_TYPE));

         result = DSAT_OK;
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
    else if (tok_ptr->op == (NA|EQ|QU))
    {
      res_buff_ptr->used =
           (word)snprintf ((char*)res_buff_ptr->data_ptr,
                                 res_buff_ptr->size,
                                 "%s:",
                                 (char*)parse_table->name);      
      result = DSAT_OK;
    }
    else
    {
      result = DSAT_ERROR;
    }
  }
  return result;
}  /* dsatvend_exec_qcdnsp_cmd() */


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDNSS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdnss command.
  at$qcdnss is used to set secondary DNS IP address.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdnss_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  uint32                ds707_qcdns_val[2]={0,0};

#ifdef FEATURE_DATA_IS707
  ds707_pkt_mgr_get_1x_profile_val(DS707_QCDNS_VAL,(void *)ds707_qcdns_val,2);
#else
  if((tok_ptr->op == (NA|QU)) && (NV_DONE_S == dsatutil_get_nv_item( NV_SECONDARY_DNS_I,&ds_nv_item )))
  {
    ds707_qcdns_val[1] = ds_nv_item.secondary_dns;
  }
#endif /*FEATURE_DATA_IS707*/

  result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );

  if (DSAT_OK == result)
  {
    if (tok_ptr->op == (NA|EQ|AR))
    {
      /*---------------------------------------------------------------
      Check for validity of the IP address & convert to internal format
      ---------------------------------------------------------------*/
      if (DSAT_OK == dsatutil_convert_tuple (STRING_TO_INT,
                                             TUPLE_TYPE_IPv4,
                                             &ds707_qcdns_val[1],
                                             (byte*)dsatutil_get_val(parse_table->cmd_id,0,0,STR_TYPE),
                                             sizeof(dsat_qcdnssec_val),
                                             DSAT_CONVERT_IP) )
      {
        /*---------------------------------------------------------------
        Write Primary DNS item into the NV 
        ---------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707
        ds707_pkt_mgr_set_1x_profile_val(DS707_QCDNS_VAL,(void *)ds707_qcdns_val,2);
#endif /*FEATURE_DATA_IS707*/
        ds_nv_item.secondary_dns  = ds707_qcdns_val[1];
        (void)dsatutil_put_nv_item( NV_SECONDARY_DNS_I, &ds_nv_item );
        result = DSAT_OK;
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
    else if (tok_ptr->op == (NA))
    {
      result = DSAT_OK;
    }
    else if (tok_ptr->op == (NA|QU))
    {
      /*---------------------------------------------------------------
      Format NV data to 4-tuple string
      ---------------------------------------------------------------*/
      if (DSAT_OK == dsatutil_convert_tuple (INT_TO_STRING,
                                             TUPLE_TYPE_IPv4,
                                             &ds707_qcdns_val[1],
                                             dsat_qcdnssec_val,
                                             sizeof(dsat_qcdnssec_val),
                                             DSAT_CONVERT_IP) )
      {
        /* Use extended format on value query */
         res_buff_ptr->used +=
              (word)snprintf ((char*)res_buff_ptr->data_ptr,
                                    res_buff_ptr->size,
                                    "%s: %s",
                                    (char*)parse_table->name,
                                     (char*)dsat_qcdnssec_val);
         result = DSAT_OK;
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
    else if (tok_ptr->op == (NA|EQ|QU))
    {
       res_buff_ptr->used =
             (word)snprintf ((char*)res_buff_ptr->data_ptr,
                                   res_buff_ptr->size,
                                   "%s:",
                                   (char*)parse_table->name);    
      result = DSAT_OK;
    }
    else
    {
      result = DSAT_ERROR;
    }
  }
  return result;
} /* dsatvend_exec_qcdnss_cmd() */


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCTER_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcter command.
  at$qcter is used to set current and default TE-DCE baud rate.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcter_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  dsat_num_item_type    old_qcter_val;
  dsat_num_item_type    temp_val;
  old_qcter_val = (dsat_num_item_type)dsatutil_get_val(
                                DSAT_VENDOR_QCTER_IDX,0,0,NUM_TYPE); 
                                 /* Retain old value for error checking */
  
  result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );
#ifdef FEATURE_DATA_IS707
  if( (result == DSAT_OK) && (IS_JCDMA_MODE()))
  {
    if((tok_ptr->op == (NA|EQ|AR) &&
       (SIO_BITRATE_115200 != (dsat_num_item_type)dsatutil_get_val(
                                DSAT_VENDOR_QCTER_IDX,0,0,NUM_TYPE))) )
    {
      /* Baudrate not allowed in JCDMA mode, 
       * tell user "ERROR".  Restore old IPR value. 
       */
      result = DSAT_ERROR;
      DSATUTIL_SET_VAL(DSAT_VENDOR_QCTER_IDX,0,0,0,old_qcter_val,NUM_TYPE)
      
    }
    else if (tok_ptr->op == (NA|EQ|QU))
    {
      res_buff_ptr->used =
        (word) snprintf((char*)res_buff_ptr->data_ptr,
                               res_buff_ptr->size,
                               "$QCTER:(115200)");
    }
  }
#endif /* FEATURE_DATA_IS707 */
  if (DSAT_OK == result) 
  {
    if (tok_ptr->op == (NA|EQ|AR)) 
    { 
      temp_val = (dsat_num_item_type)dsatutil_get_val(
                                          DSAT_VENDOR_QCTER_IDX,0,0,NUM_TYPE);
      if (sio_is_baud_valid ((sio_bitrate_type)temp_val) == FALSE)
      {
        /* Baudrate not allowed, tell user "ERROR" */
        DSATUTIL_SET_VAL(DSAT_VENDOR_QCTER_IDX,0,0,0,old_qcter_val,NUM_TYPE)
        result = DSAT_ERROR;
      }
      else
      {
#ifdef FEATURE_DATA_IS707
        /*--------------------------------------------------------------------
          Note that this if statement does the function call internally, which
          will result in the baud rate being changed, if the passed in 
          qcter_val is okay
        --------------------------------------------------------------------*/
        temp_val = (dsat_num_item_type) dsatutil_get_val(
                                           DSAT_VENDOR_QCTER_IDX,0,0,NUM_TYPE);
        if ((IS_JCDMA_MODE()) && 
            (ds707_jcdma_chng_m513_via_at((sio_bitrate_type)temp_val)  == FALSE))
        {
          DSATUTIL_SET_VAL(DSAT_VENDOR_QCTER_IDX,0,0,0,old_qcter_val,NUM_TYPE)
          result = DSAT_ERROR;
        }
        else
#endif /* FEATURE_DATA_IS707 */
        {
          /* Change current baud and default in NV */
          temp_val = (dsat_num_item_type)dsatutil_get_val(
                                            DSAT_VENDOR_QCTER_IDX,0,0,NUM_TYPE);
          dsat_change_baud( (sio_bitrate_type)temp_val , FALSE);
        }
      }  /* brackets else in sio_is_baud_valid() == FALSE */
    }
  }
  return result;
}  /* dsatvend_exec_qcter_cmd() */


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPWRDN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcpwrdn command.
  at$qcpwrdn is used to power down the terminal.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Phone is powered off.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcpwrdn_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  /* function return                    */
  dsat_result_enum_type result = DSAT_OK;

  /*---------------------------------------
   Processing command with no args
   ----------------------------------------*/
  if (tok_ptr->op == (NA) ) 
  {
     if(TRUE == cm_rpm_check_reset_allowed())
    {
     sys_m_initiate_poweroff();
    }
    else
    {
      DS_AT_MSG0_HIGH("PowerDown is blocked");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    /*  NO-OP but prevents ERROR --                     */
    result = DSAT_OK;
  }
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsatvend_exec_qcpwrdn_cmd () */



#ifdef FEATURE_DSAT_BREW_SUPPORT

/*===========================================================================

FUNCTION DSATVEND_EXEC_BREW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$brew command.
  at$brew is used to transition the serial port to diagnostic
  monitor.This port change will be non-persistent. The port
  change will not be written on to EFS.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Transitions the data port to Brew.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_brew_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  /* function return                    */
  dsat_result_enum_type result;

  /*result for diagcomm_sio_qcdmg       */
  boolean               brew_ret = FALSE;

  /*---------------------------------------
   Processing command with no args
   ----------------------------------------*/
  if (tok_ptr->op == (NA) ) 
  {
    rdm_device_enum_type device;

    DS_AT_MSG0_HIGH("Transitioning to BREW");
    
    /* Query the RDM for the port currently being used by Data Services */
    device = rdm_get_device(RDM_DATA_SRVC);
  
    /* Relinquish the port to BREW */
    brew_ret = rdm_assign_port_tmp ( RDM_BREW_SRVC, device, NULL );

    if ( brew_ret )
    {
      result = DSAT_DO_DM;
    }
    else
    {
      result = DSAT_ERROR;
    }
  }
  /*--------------------------------------
  Processing for a TEST command:
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    /*  NO-OP but prevents ERROR --                     */
    result = DSAT_OK;
  }
  else  /*  Wrong Operator  */
  {
    result = DSAT_ERROR;
  }

  return result;

} /* dsatvend_exec_brew_cmd () */

#endif /* FEATURE_DSAT_BREW_SUPPORT */


#ifdef FEATURE_DSAT_ETSI_DATA

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDGEN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdgen command.
  at$qcdgen is used to send a specified quantity of PS data as IP packet(s) 
  over a primary PDP-IP context already activated by +CGACT command.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if async processing to start data send in progress

SIDE EFFECTS
  Phone is powered off.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcdgen_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD; /* default function return */
  dsat_num_item_type cid = 0;             /* context ID */
  if (tok_ptr->op == (NA|EQ|AR) ) 
  {
    /* Write Command */

    /* If <cid> parameter in range 1-DSAT_UMTS_MAX_PDP_PROFILE_NUM has not been
       provided exit with diag message and ERROR result code */
    if ( !VALID_TOKEN(0) ||
         ATOI_OK != dsatutil_atoi(&cid, tok_ptr->arg[0], 10) ||
         !DSAT_IS_PROFILE_ID_WITHIN_RANGE(cid))
    {
      DS_AT_MSG1_HIGH("Invalid <cid> specified: %d",cid);
      result = DSAT_ERROR;
    }
    else
    {
        dsat_num_item_type num_of_bytes = 0;
        
        if ( !VALID_TOKEN(1) ||
             ATOI_OK != dsatutil_atoi(&num_of_bytes, 
                                      tok_ptr->arg[1], 
                                      10)||
                            num_of_bytes < DSAT_DGEN_MIN_DATA_LEN )
        {
          DS_AT_MSG1_HIGH("Invalid <data_len> specified: %d",num_of_bytes);
          result = DSAT_ERROR;
        }
        else
        {
          SET_PENDING(DSAT_VENDOR_QCDGEN_IDX ,0, DSAT_PENDING_TRUE)
          
          result = ds_3gpp_rmsm_atif_qcdgen_handler ( (uint32)cid,
                                                      (sys_modem_as_id_e_type)dsat_get_current_subs_id(FALSE),
                                                      (uint32)num_of_bytes,NULL);
          if(result != DSAT_ASYNC_CMD)
          {
            DS_AT_MSG0_HIGH("Unsuccessful Result from RMSM");
            SET_PENDING(DSAT_VENDOR_QCDGEN_IDX ,0, DSAT_PENDING_FALSE)
            result = DSAT_ERROR;
          }
        }
     }
   } /* End write command */
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    /* Test Command */
    result = DSAT_OK;
  }
  else  
  {
    /* Read Command */
    result = DSAT_ERROR;
  }

  return result;
} /* dsatvend_exec_qcdgen_cmd () */

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPDPCFGEXT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcpdpcfgext command.
  at$qcpdpcfgext is used for setting and getting MBIM context type.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcpdpcfgext_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  #define MBIM_CONTEXT_MAX_VAL_BYTES 4

  dsat_num_item_type                 profile_id        = 0;
  dsat_num_item_type                 profile_params    = 0;
  dsat_num_item_type                 token_idx         = 0;
  dsat_num_item_type                 min_val           = 0;
  dsat_num_item_type                 buf_curr_len      = 0;
  uint8                              i                 = 0;
  uint8                              index             = 0;
  dsat_result_enum_type              result            = DSAT_OK;
  boolean                            valid_status      = FALSE;
  void                              *value             = NULL;
  dsat_error_info_s_type             err_info;
  char                               profile_param_data[MAX_PARAM_BUF];

  dsat_qcprofile_param_details_s_type *param_list = &profile_qcpdpcfgext_param_details_list[0];

  err_info.errval  = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if (tok_ptr->op == (NA|EQ|AR) ) 
  {
    /* Check the number of arguments.*/
    if ( tok_ptr->args_found > parse_table->compound)
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }
/* Profile validation :- Arg 0 should present and have valid value*/

    VALID_NUM_TOKEN(TOKEN_IDX_0,0,DSAT_EXT_PROFILE_NUM_MAX, &profile_id)

    if(valid_status && DSAT_IS_PROFILE_ID_WITHIN_RANGE(profile_id))
    {
      if(FALSE == dsatetsipkt_is_profile_context_valid( profile_id,
                                                        DS_PROFILE_TECH_3GPP,
                                                        dsat_get_current_ds_profile_subs_id()))

      {
        err_info.errval = DSAT_ERR_NO_VALID_PROFILE;
        err_info.arg_num = 0;
        goto send_error;
      }
    }
    else
    {
      err_info.errval  = DSAT_ERR_NO_VALID_PROFILE;
      err_info.arg_num = 0;
      goto send_error;
    }
/*At least one valid arg should present */

    if( !( VALID_TOKEN(1) || VALID_TOKEN(2)|| VALID_TOKEN(3))) 
    {
      err_info.errval = DSAT_ERR_PARAMETER_MANDATORY;
      goto send_error;
    }

    /*token_index 0 is used for profile_id*/
    for (i=0;i<ARR_SIZE(profile_qcpdpcfgext_param_details_list);i++)
    {
      token_idx      = i+1;
      min_val        = 0;
      profile_params = 0;

      VALID_NUM_TOKEN(token_idx, min_val, param_list[i].max_value,&profile_params)

      if (valid_status)
      {
        value = &profile_params;

        if(dsatetsipkt_read_write_profile_param((uint16)profile_id,
                                                WRITE_COMMAND,
                                                param_list[i].identifier, 
                                                value,
                                                param_list[i].len) != DSAT_OK)
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = token_idx;
          goto send_error;
        }
      }
    }
    result = DSAT_OK;
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                               res_buff_ptr->size,
                                          "$QCPDPCFGEXT: (%d-%d,%d-%d),%d,(0-1),(0-255)\n",
                                           DSAT_LEGACY_PROFILE_NUM_MIN,
                                           DSAT_LEGACY_PROFILE_NUM_MAX,
                                           DSAT_EXT_PROFILE_NUM_MIN,
                                           DSAT_EXT_PROFILE_NUM_MAX,
                                           MBIM_CONTEXT_MAX_VAL_BYTES);

  }
  else if(tok_ptr->op == (NA|QU))
  {
    uint16    i = 0;
    uint8     profile_list[DSAT_MAX_ALLOWED_PROFILES] = {0};
    uint8     total_profiles = 0;
    res_buff_ptr->used = 0;
    if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                               sizeof(profile_list), &total_profiles,dsat_get_current_ds_profile_subs_id() ))
    {
      DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
    }

    for ( index = 0; index < total_profiles; index++)
    {
      /*Reset profile parameter data and length*/
      memset(&profile_param_data[0],0x0,MAX_PARAM_BUF);
      buf_curr_len = 0;
      
      /*Start iterating through the param table*/   
      for (i=0;i<ARR_SIZE(profile_qcpdpcfgext_param_details_list);i++)
      {
        profile_params = 0;
        if(DSAT_OK == dsatetsipkt_read_write_profile_param((uint16)profile_list[index],
                                                           READ_COMMAND,
                                                           param_list[i].identifier,
                                                           &profile_params,
                                                           param_list[i].len))
        {
          buf_curr_len += snprintf(&profile_param_data[buf_curr_len],MAX_PARAM_BUF - buf_curr_len,"%d",profile_params);
        }
        buf_curr_len += snprintf(&profile_param_data[buf_curr_len],MAX_PARAM_BUF - buf_curr_len,",");
      }
      /*Remove the last extra comma*/
      buf_curr_len--;
      profile_param_data[buf_curr_len] = '\0';

      res_buff_ptr->used += 
             (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                              res_buff_ptr->size - res_buff_ptr->used,
                              "$QCPDPCFGEXT: %d,%s\n",
                              profile_list[index],
                              profile_param_data);
      
    }

    if(res_buff_ptr->used == 0)
    {
      res_buff_ptr->used =(word) snprintf((char*)&res_buff_ptr->data_ptr[0],
                                            res_buff_ptr->size,
                                            "$QCPDPCFGEXT: ");
    }
  }
  else
  {
    /* Not Supported */
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }

  return result;

send_error:
  DS_AT_MSG2_ERROR("$QCPDPCFGEXT: Error: %d, Arg:%d",
                   err_info.errval,
                   err_info.arg_num);
  return DSAT_ERROR;
}

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPDPCFGE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcpdpcfge command.
  at$qcpdpcfge is used to edit APN values in APN table. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcpdpcfge_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_num_item_type                        profile_id    = 0;
  dsat_num_item_type                        profile_params    = 0;
  ds_profile_3gpp_mnc_type                  mnc;           /*Indicates Mnc to be used in the PCO*/
  dsat_error_info_s_type                    err_info;
  dsat_qcprofile_param_details_s_type *param_list = &profile_qcpdpcfge_param_details_list[0];
  boolean             valid_status = FALSE;
  void                *value = NULL;

  uint8               i = 0;
  uint8               token_idx = 0;
  uint8               profile_list[DSAT_MAX_ALLOWED_PROFILES] = {0};
  uint8               total_profiles = 0;
  uint16              index = 0;
  dsat_num_item_type  buf_curr_len = 0;
  dsat_num_item_type  min_val = 0;
  char                profile_param_data[MAX_PARAM_BUF];

  err_info.errval  = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if (tok_ptr->op == (NA|EQ|AR) ) 
  {
    /* Check the number of arguments.*/
    if ( tok_ptr->args_found > parse_table->compound)
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }

/* Profile validation :- Arg 0 should present and have valid value*/
    VALID_NUM_TOKEN(TOKEN_IDX_0,0,DSAT_EXT_PROFILE_NUM_MAX, &profile_id)
    if (valid_status && DSAT_IS_PROFILE_ID_WITHIN_RANGE(profile_id))
    {
      if(FALSE == dsatetsipkt_is_profile_context_valid( profile_id,
                                                        DS_PROFILE_TECH_3GPP,
                                                        dsat_get_current_ds_profile_subs_id()))
      {
        err_info.errval  = DSAT_ERR_INVALID_ARGUMENT;
        err_info.arg_num = 0;
        goto send_error;
      }
    }
    else
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 0;
      goto send_error;
    }

    for (i=0;i<ARR_SIZE(profile_qcpdpcfge_param_details_list);i++)
    {
      token_idx      = i+1;
      min_val        = 0;
      profile_params = 0;

      /*Special case of reserved pco where min is not 0*/
      if (token_idx == TOKEN_IDX_9)
      {
        min_val = MIN_OPRT_RESV_PCO;
      }

      VALID_NUM_TOKEN(token_idx, min_val, param_list[i].max_value,&profile_params)
      if (valid_status)
      {
        value = &profile_params;

        if(token_idx == TOKEN_IDX_11)
        {
          mnc.mnc_includes_pcs_digit = FALSE;
          if(PLMN_STR_MNC_LEN == strlen((char*)tok_ptr->arg[11]))
          {
            mnc.mnc_includes_pcs_digit = TRUE;
          }
          mnc.mnc_digits = (uint16)profile_params;
        }

        if(dsatetsipkt_read_write_profile_param((uint16)profile_id,
                                                WRITE_COMMAND,
                                                param_list[i].identifier, 
                                                value,
                                                param_list[i].len) != DSAT_OK)
        {
          err_info.errval  = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = token_idx;
          goto send_error;
        }
      }
    }    
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
/*  
  APN bearer is a bit mask and the input can be a combination of bit masks below. 
  
  #define  DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_G   (0x1)   
  #define  DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_W   (0x2)  
  #define  DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_L   (0x4)   
  #define  DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_T   (0x8)
  #define  DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_ALL (0xff)  

  Hence the allowed range is 0x1 - 0xF(G+W+L+T) and 0xFF for all.
  
*/
    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                               res_buff_ptr->size,
                                               "$QCPDPCFGE: (%d-%d,%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d,%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d)\n",
                                               DSAT_LEGACY_PROFILE_NUM_MIN ,
                                               DSAT_LEGACY_PROFILE_NUM_MAX,
                                               DSAT_EXT_PROFILE_NUM_MIN,
                                               DSAT_EXT_PROFILE_NUM_MAX,
                                               0,1,
                                               0,DEF_MAX_INACTIVITY_TIMER_VAL,
                                               DSAT_APN_CLASS_ONE,DSAT_APN_CLASS_MAX,
                                               DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_G,15,DS_UMTS_PDP_PROFILE_APN_BEARER_FLAG_ALL,
                                               0,DEF_MAX_PDN_CONN_PER_BLK,
                                               0,MAX_PDN_CONN_TIME,
                                               0,DEF_MAX_PDN_REQ_WAIT_INTERVAL,
                                               0,1,
                                               MIN_OPRT_RESV_PCO,MAX_OPRT_RESV_PCO,
                                               0,MAX_MNC_MCC_VAL,
                                               0,MAX_MNC_MCC_VAL);


  }
  else if(tok_ptr->op == (NA|QU))
  {
    res_buff_ptr->used = 0;
    if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                               sizeof(profile_list), &total_profiles,
                                               dsat_get_current_ds_profile_subs_id()) )
    {      
      DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
    }
    for ( index = 0; index < total_profiles; index++)
    {
      /*Reset profile parameter data and length*/
      memset(&profile_param_data[0],0x0,MAX_PARAM_BUF);
      buf_curr_len = 0;
      
      /*Start for 1st position, leave 0th index above*/
      
      for (i=0;i<ARR_SIZE(profile_qcpdpcfge_param_details_list);i++)
      {
        profile_params = 0;
        if(DSAT_OK == dsatetsipkt_read_write_profile_param((uint16)profile_list[index],
                                                           READ_COMMAND,
                                                           param_list[i].identifier,
                                                           &profile_params,
                                                           param_list[i].len))
        {
          buf_curr_len += snprintf(&profile_param_data[buf_curr_len],MAX_PARAM_BUF - buf_curr_len,"%d",profile_params);
        }
        buf_curr_len += snprintf(&profile_param_data[buf_curr_len],MAX_PARAM_BUF - buf_curr_len,",");
      }
      /*Remove the last extra comma*/
      buf_curr_len--;
      profile_param_data[buf_curr_len] = '\0';

      res_buff_ptr->used += 
             (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                              res_buff_ptr->size - res_buff_ptr->used,
                              "$QCPDPCFGE: %d,%s\n",
                              profile_list[index],
                              profile_param_data);
      
    }
    if(0 == res_buff_ptr->used )
    {
       res_buff_ptr->used = 
             (word) snprintf((char*)&res_buff_ptr->data_ptr[0],
                              res_buff_ptr->size,
                              "$QCPDPCFGE: ");
    }
    
  }
  else
  {
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }

  return DSAT_OK;

send_error:
  DS_AT_MSG2_ERROR("Error:%d, Arg:%d",err_info.errval, err_info.arg_num);
  return DSAT_ERROR;

}/*dsatvend_exec_qcpdpcfge_cmd*/

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPDPIMSCFGE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcpdpimscfge command.
  at$qcpdpimscfge is used to edit PDP profile registry . 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcpdpimscfge_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_num_item_type       profile_id       = 0;
  dsat_num_item_type       flag_i_val[3]    = {0,0,0};
  boolean                  flag_val[3]      = {FALSE,FALSE,FALSE};
  dsat_result_enum_type    result = DSAT_OK;
  dsat_error_info_s_type   err_info;

  err_info.errval  = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if (tok_ptr->op == (NA|EQ|AR) ) 
  {
    /* Check the number of arguments. */
    if ( (tok_ptr->args_found > parse_table->compound) || (tok_ptr->args_found < 2) )
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }
    if( !(VALID_TOKEN(0) && (VALID_TOKEN(1) || VALID_TOKEN(2) || VALID_TOKEN(3))))
    {
      err_info.errval = DSAT_ERR_INVALID_TOKENS;
      goto send_error;
    }
    if((ATOI_OK ==dsatutil_atoi(&profile_id,tok_ptr->arg[0],10)) && 
        ( DSAT_IS_PROFILE_ID_WITHIN_RANGE(profile_id)))
    {
      if(!dsatetsipkt_is_profile_context_valid( profile_id,
                                                DS_PROFILE_TECH_3GPP,
                                                dsat_get_current_ds_profile_subs_id()))
      {
        DS_AT_MSG1_ERROR("Context %d is not valid",profile_id);
        return DSAT_ERROR;
      }
      if(VALID_TOKEN(1) && (ATOI_OK ==dsatutil_atoi((dsat_num_item_type*)&flag_i_val[0],tok_ptr->arg[1],10)))
      {
        if(0 == flag_i_val[0] || 1 == flag_i_val[0])
        {
          result = dsatetsipkt_read_write_profile_param((uint16)profile_id,WRITE_COMMAND,
                                   DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_REQ_FLAG,
                                   &flag_i_val[0],sizeof(boolean));
          if(DSAT_OK != result )
          {
            DS_AT_MSG1_ERROR("$QCPDPIMSCFGE: Address flag write failed  %d", result);
            return DSAT_ERROR;
          }
        }
        else
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 1;
          goto send_error;
        }
      }
      if(VALID_TOKEN(2) && (ATOI_OK ==dsatutil_atoi((dsat_num_item_type*)&flag_i_val[1],tok_ptr->arg[2],10)))
      {
        if(0 == flag_i_val[1] || 1 == flag_i_val[1])
        {
          result = dsatetsipkt_read_write_profile_param((uint16)profile_id,WRITE_COMMAND,
                                   DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG,
                                   &flag_i_val[1],sizeof(boolean));

          if(DSAT_OK != result )
          {
            DS_AT_MSG1_ERROR("$QCPDPIMSCFGE: DHCP flag write failed  %d", result);
            return DSAT_ERROR;
          }
        }
        else
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 2;
          goto send_error;
         }
      }
      if(VALID_TOKEN(3) && (ATOI_OK ==dsatutil_atoi((dsat_num_item_type*)&flag_i_val[2],tok_ptr->arg[3],10)))
      {
        if(0 == flag_i_val[2] || 1 == flag_i_val[2])
        {
          result = dsatetsipkt_read_write_profile_param((uint16)profile_id,WRITE_COMMAND,
                                     DS_PROFILE_3GPP_PROFILE_PARAM_IM_CN_FLAG,
                                     &flag_i_val[2],sizeof(boolean));

          if(DSAT_OK != result )
          {
            DS_AT_MSG1_ERROR("$QCPDPIMSCFGE: IM CN flag write failed  %d", result);
            return DSAT_ERROR;
          }

        }
        else
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 3;
          goto send_error;
        }
      }
    }
    else
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 0;
      goto send_error;
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,
                                               "$QCPDPIMSCFGE: (%d-%d,%d-%d),(0-1),(0-1),(0-1)\n",
                                                DSAT_LEGACY_PROFILE_NUM_MIN,
                                                DSAT_LEGACY_PROFILE_NUM_MAX,
                                                DSAT_EXT_PROFILE_NUM_MIN,
                                                DSAT_EXT_PROFILE_NUM_MAX);
    
  }
  else  if(tok_ptr->op == (NA|QU))
  {
    uint16  i = 0;
    uint8   profile_list[DSAT_MAX_ALLOWED_PROFILES] = {0};
    uint8   total_profiles = 0;
    res_buff_ptr->used = 0;
    
    if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                               sizeof(profile_list), &total_profiles,
                                               dsat_get_current_ds_profile_subs_id()) )
    {      
      DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
    }
    
    for(i = 0; i < total_profiles; i++)
    {
      result = dsatetsipkt_read_write_profile_param((uint16)profile_list[i],READ_COMMAND,
                                        DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_REQ_FLAG,
                                        &flag_val[0],sizeof(boolean));

      if(DSAT_OK == result )
        {
        result = dsatetsipkt_read_write_profile_param((uint16)profile_list[i],READ_COMMAND,
                                       DS_PROFILE_3GPP_PROFILE_PARAM_PCSCF_DHCP_REQ_FLAG,
                                       &flag_val[1],sizeof(boolean));
        if(DSAT_OK == result )
        {
          result = dsatetsipkt_read_write_profile_param((uint16)profile_list[i],READ_COMMAND,
                                       DS_PROFILE_3GPP_PROFILE_PARAM_IM_CN_FLAG,
                                       &flag_val[2],sizeof(boolean));

          if(DSAT_OK == result )
          {
            res_buff_ptr->used += 
             (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                    res_buff_ptr->size - res_buff_ptr->used,
                                   "$QCPDPIMSCFGE: %d , %d , %d , %d\n",
                                    profile_list[i],
                                    flag_val[0],
                                    flag_val[1],
                                    flag_val[2]);

           }
         }
      }
    }
    if(0 == res_buff_ptr->used )
    {
      res_buff_ptr->used = 
         (word) snprintf((char*)&res_buff_ptr->data_ptr[0],
                                 res_buff_ptr->size,
                                 "$QCPDPIMSCFGE: ");
    }

  }
  else
  {
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }
  return DSAT_OK;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_ARGUMENT )
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  else
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  return DSAT_ERROR;

} /*dsatvend_exec_qcpdpimscfge_cmd*/

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCAPNE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcapne command.
  at$qcapne is used to edit APN values in APN table. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcapne_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
#define MAX_APNE_OUTPUT 150

  ds_profile_3gpp_apn_class_type          pre_apn_class = DSAT_APN_CLASS_ONE; /* Used to restore an APN  Class value     .*/
  ds_profile_3gpp_pdp_context_type        pdata;
  ds_profile_3gpp_apn_disable_flag_type   flag_val      = FALSE;              /* Used to Update an APN  flag value      */


  void                       *value            = NULL;
  uint8                      token_idx         = 0;
  uint16                     i                 = 0;
  uint16                     index             = 0;
  boolean                    valid_status = FALSE;
  dsat_num_item_type         profile_params    = 0;
  dsat_num_item_type         min_val           = 0;
  dsat_num_item_type         profile_id    = 0;
  dsat_string_item_type      apnb_type[10];
  dsat_string_item_type      pdptype_val[8];
  byte                       temp_buff[8];
  byte                       apn_buff[DS_PROFILE_3GPP_MAX_APN_STRING_LEN+1];

  dsat_qcprofile_param_details_s_type     *param_list = &profile_qcapne_param_details_list[0];
  dsat_error_info_s_type     err_info;

  err_info.errval  = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if (tok_ptr->op == (NA|EQ|AR) ) 
  {
    /* Check the number of arguments. */
    if ( tok_ptr->args_found != parse_table->compound )
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }

    /* Profile ID  */

    VALID_NUM_TOKEN(TOKEN_IDX_0,0,DSAT_EXT_PROFILE_NUM_MAX, &profile_id)
    if (valid_status && DSAT_IS_PROFILE_ID_WITHIN_RANGE(profile_id))
    {
      if(FALSE == dsatetsipkt_is_profile_context_valid(profile_id, 
                                                       DS_PROFILE_TECH_3GPP,
                                                       dsat_get_current_ds_profile_subs_id()))
      {
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        goto send_error;
      }
    }
    else
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      goto send_error;
    }
	
    /* APN CLASS */

    for (i=0;i<ARR_SIZE(profile_qcapne_param_details_list);i++)
    {
      min_val        = 0;
      profile_params = 0;

      if (i == 0)
      {
        token_idx = TOKEN_IDX_1;
      }

      else if (i == 1)
      {
        token_idx = TOKEN_IDX_5;
      }

      else if (i == 2)
      {
        token_idx = TOKEN_IDX_6;
      }

      VALID_NUM_TOKEN(token_idx, min_val, param_list[i].max_value,&profile_params)
      value = &profile_params;

      if (valid_status)
      {
        if(dsatetsipkt_read_write_profile_param((uint16)profile_id,
                                                WRITE_COMMAND,
                                                param_list[i].identifier, 
                                                value,
                                                param_list[i].len) != DSAT_OK)
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          goto send_error;
        }
      }
    }

    memset((void*)&pdata, 0, sizeof(pdata));
   
    /* Try getting the profile info from EFS */
    if(dsatetsipkt_read_write_param_table(context_param_tbl,
                                          sizeof(context_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
                                          profile_id,
                                          READ_COMMAND,
                                          (byte*)&pdata,sizeof(pdata),
                                          dsat_get_current_ds_profile_subs_id()) != DSAT_OK)
    {
      DS_AT_MSG1_HIGH("Could not get profile %d info",profile_id);
    }
    /*         APN Name          */

    VALID_STRING_TOKEN(TOKEN_IDX_2, tok_ptr->arg[2], apn_buff,sizeof(apn_buff))
    if (valid_status)
    {
      (void)strlcpy((char *)pdata.apn,(const char *)apn_buff,DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1);
    }

    /*         PDP Type          */
    VALID_STRING_TOKEN(TOKEN_IDX_3, tok_ptr->arg[3], pdptype_val,sizeof(pdptype_val))
    if (valid_status)
    {
      if( !dsatutil_strcmp_ig_sp_case((const byte *)pdptype_val, (const byte *)"IPV6") )
      {
        pdata.pdp_type = DS_PROFILE_3GPP_PDP_IPV6;
      }
      else if( !dsatutil_strcmp_ig_sp_case((const byte *)pdptype_val, (const byte *)"IPV4V6") )
      {
        pdata.pdp_type = DS_PROFILE_3GPP_PDP_IPV4V6;
      }
      else
      {
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        err_info.arg_num = 3;
        goto send_error;
      }
    }

    /*         APN Bearer type          */

    VALID_STRING_TOKEN(TOKEN_IDX_4, tok_ptr->arg[4], apnb_type,sizeof(apnb_type))
    if (valid_status)
    {
      if( dsatutil_strcmp_ig_sp_case((const byte *)apnb_type, (const byte *)"LTE") )
      {
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        goto send_error;
      }
    }

    /* WRITE to PDP REG */	

    /* Write to EFS */
    if(dsatetsipkt_read_write_param_table(context_param_tbl,
                                          sizeof(context_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
                                          profile_id,
                                          WRITE_COMMAND,
                                          (byte*)&pdata,sizeof(pdata),
                                          dsat_get_current_ds_profile_subs_id()) != DSAT_OK)
    {
      err_info.errval = DSAT_ERR_NO_VALID_PROFILE;
      goto send_error;
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU) )
  {
    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                               res_buff_ptr->size,
                                               "$QCAPNE: (%d-%d,%d-%d),(%d-%d),,(%s and %s),\n%s,(%d-%d ),(%d-%d)\n",
                                               DSAT_LEGACY_PROFILE_NUM_MIN,
                                               DSAT_LEGACY_PROFILE_NUM_MAX,
                                               DSAT_EXT_PROFILE_NUM_MIN,
                                               DSAT_EXT_PROFILE_NUM_MAX,
                                               DSAT_APN_CLASS_ONE,DSAT_APN_CLASS_MAX,
                                               "IPV6","IPV4V6","LTE",0,1,
                                               0,DEF_MAX_INACTIVITY_TIMER_VAL);

  }
  else if(tok_ptr->op == (NA|QU))
  {
    ds_profile_3gpp_inactivity_timer_val_type  inac_timer;
    uint8     profile_list[DSAT_MAX_ALLOWED_PROFILES] = {0};
    uint8     total_profiles = 0;
    
    if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                               sizeof(profile_list), &total_profiles,
                                               dsat_get_current_ds_profile_subs_id()) )
    {
      DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
    }
    for(index = 0; index < total_profiles ;index++)
    {
      memset((void*)&pdata, 0, sizeof(pdata));
      /* Try getting the profile info from EFS */
      if(dsatetsipkt_read_write_param_table(context_param_tbl,
                                            sizeof(context_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
                                            profile_list[index],
                                            READ_COMMAND,
                                            (byte*)&pdata,
                                            sizeof(pdata),
                                            dsat_get_current_ds_profile_subs_id()) != DSAT_OK)
      {
        DS_AT_MSG1_HIGH("Could not get profile %d info",profile_list[i]);
        continue;
      }
      else
      {
        for (i=0;i<ARR_SIZE(profile_qcapne_param_details_list);i++)
        {
          if (i == 0)
          {
            value = &pre_apn_class;
          }

          else if (i == 1)
          {
            value = &flag_val;
          }

          else if (i == 2)
          {
            value = &inac_timer;
          }

          if(DSAT_OK != dsatetsipkt_read_write_profile_param((uint16)profile_list[index],
                                                             READ_COMMAND,
                                                             param_list[i].identifier,
                                                             value,
                                                             param_list[i].len))
          {
            err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
            goto send_error;
          }
        }
        memset((void *)temp_buff,0,sizeof(temp_buff));
        switch(pdata.pdp_type)
        {
          case DS_PROFILE_3GPP_PDP_IPV6:
            strlcpy((char*)temp_buff,"IPV6",sizeof(temp_buff));
            break;
          case DS_PROFILE_3GPP_PDP_IPV4V6:
            strlcpy((char*)temp_buff,"IPV4V6",sizeof(temp_buff));
            break;
          default:
            continue;
        }

        /* Check to see if a new DSM item is required */
        CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - MAX_APNE_OUTPUT) );

        res_buff_ptr->used += 
          (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                res_buff_ptr->size - res_buff_ptr->used,
                                "$QCAPNE: %d , %d ,\"%s\",\"%s\",\"%s\", %d, %u\n",
                                profile_list[index],
                                pre_apn_class,
                                pdata.apn,
                                temp_buff,
                                "LTE",
                                flag_val,
                                (unsigned int)inac_timer );
      }
    }
    if(res_buff_ptr->used == 0)
    {
      res_buff_ptr->used = 
            (word) snprintf((char*)&res_buff_ptr->data_ptr[0],
                             res_buff_ptr->size,
                             "$QCAPNE: ");
    }
  }
  else
  {
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }

  return DSAT_OK;

send_error:
  DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  return DSAT_ERROR;
}/*dsatvend_exec_qcapne_cmd*/

#endif /* FEATURE_DSAT_ETSI_DATA */

/*===========================================================================

FUNCTION  VEND_CONVERT_MRU

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  Returns a boolean that describes the result of the command execution.
    FALSE : if there was any problem in execution.
    TRUE  : if it is a success. 

SIDE EFFECTS
  None

===========================================================================*/
boolean vend_convert_mru
(
  cm_mmode_mru_table_entry_u_type *mru_entry, 
  dsat_mru_input_s_type *mru_data
)
{
#ifdef FEATURE_DSAT_LTE
  uint64                lte_mru_band_pref;
#endif/* FEATURE_DSAT_LTE */
  boolean               invalid_data = FALSE;
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_DSAT_LTE)
  uint32                mru_band_pref;
  uint32                size;
#endif  /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) || defined(FEATURE_DSAT_LTE) */
  
  if ( ( mru_entry == NULL ) || ( mru_data == NULL ) )
  {
    DS_AT_MSG0_ERROR("NULL pointer argument");
    return FALSE;
  }

  DS_AT_MSG1_HIGH("Found mode - %d ",mru_entry->mode);

  switch ( mru_entry->mode )
  {
    case SYS_SYS_MODE_CDMA:
      mru_data->rat = DSAT_CDMA_RAT;
      mru_data->band = (sys_band_class_e_type) mru_entry->cdma.band;
      mru_data->channel= (sys_channel_num_type) mru_entry->cdma.channel;
      break;

    case SYS_SYS_MODE_HDR:
      mru_data->rat = DSAT_HDR_RAT;
      mru_data->band = (sys_band_class_e_type) mru_entry->cdma.band;
      mru_data->channel= (sys_channel_num_type) mru_entry->cdma.channel;
      break;

    case SYS_SYS_MODE_GSM:
#ifdef FEATURE_GSM
      mru_data->rat = DSAT_GSM_RAT;
      mru_band_pref  = (uint32) mru_entry->gsm.band_pref.bit_31_16;
      mru_band_pref  = (uint32)((uint32)mru_band_pref << 16);
      mru_band_pref  = DSAT_BAND_ADD32( mru_band_pref, mru_entry->gsm.band_pref.bit_15_0);

      size = vend_convert_band_pref_to_sys_band32(mru_band_pref,mru_data->band_pref,sizeof(mru_data->band_pref));

      if(size == 0)
      {
        invalid_data = TRUE;
      }
     (void) dsatutil_memscpy((void*)mru_data->plmn.identity, 3,
                             (void*)&mru_entry->gsm.plmn_id.identity[0],3);    
#endif /* FEATURE_GSM */
      break;

    case SYS_SYS_MODE_WCDMA:
#ifdef FEATURE_WCDMA
      mru_data->rat = DSAT_WCDMA_RAT;
      mru_band_pref  = (uint32) mru_entry->wcdma.bc.band_pref.bit_31_16;
      mru_band_pref  = (uint32)((uint32)mru_band_pref << 16);
      mru_band_pref  = DSAT_BAND_ADD32( mru_band_pref, mru_entry->wcdma.bc.band_pref.bit_15_0);
      if( mru_entry->wcdma.is_chan )
      {
        mru_data->band = (sys_band_class_e_type) mru_entry->wcdma.bc.band_chan.band;
        mru_data->channel = (sys_channel_num_type) mru_entry->wcdma.bc.band_chan.chan;
      }
      else
      {
       size = vend_convert_band_pref_to_sys_band32(mru_band_pref,mru_data->band_pref,sizeof(mru_data->band_pref));

        if(size == 0)
        {
          invalid_data = TRUE;
        }
      }
     (void) dsatutil_memscpy((void*)mru_data->plmn.identity, 3,
                        (void*)&mru_entry->wcdma.plmn_id.identity[0],3);     
#endif /* FEATURE_WCDMA */
      break;

    case SYS_SYS_MODE_GW:
#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
      mru_band_pref  = (uint32) mru_entry->gw.band_pref.bit_31_16;
      mru_band_pref  = (uint32)((uint32)mru_band_pref << 16);
      mru_band_pref  = DSAT_BAND_ADD32( mru_band_pref, mru_entry->gw.band_pref.bit_15_0);

      size = vend_convert_band_pref_to_sys_band32(mru_band_pref,mru_data->band_pref,sizeof(mru_data->band_pref));
      if(size == 0)
      {
        invalid_data = TRUE;
      }
      mru_data->rat = (mru_entry->gw.acq_mode == SYS_SYS_MODE_GSM) ? DSAT_GSM_RAT : DSAT_WCDMA_RAT;
      (void) dsatutil_memscpy((void*)mru_data->plmn.identity, 3,
                              (void*)&mru_entry->gw.plmn_id.identity[0],3);      
#endif /* FEATURE_GSM || FEATURE_WCDMA */
      break;

    case SYS_SYS_MODE_GWL:
#ifdef FEATURE_DSAT_LTE
      DS_AT_MSG1_HIGH("Found acq mode - %d ",mru_entry->gwl.acq_mode);

      mru_band_pref  = (uint32) mru_entry->gwl.band_pref.bit_31_16;
      mru_band_pref  = (uint32)((uint32)mru_band_pref << 16);
      mru_band_pref  = DSAT_BAND_ADD32( mru_band_pref, mru_entry->gwl.band_pref.bit_15_0);
      
      size = vend_convert_band_pref_to_sys_band32(mru_band_pref,mru_data->band_pref,sizeof(mru_data->band_pref));
      if(size == 0)
      {
        invalid_data = TRUE;
      }
      if(mru_entry->gwl.acq_mode == SYS_SYS_MODE_GSM )
      {
        mru_data->rat = DSAT_GSM_RAT;
      }
      else if( mru_entry->gwl.acq_mode == SYS_SYS_MODE_WCDMA )
      {
        mru_data->rat = DSAT_WCDMA_RAT;
      }
      else if ( mru_entry->gwl.acq_mode == SYS_SYS_MODE_LTE )
      {
        mru_data->rat = DSAT_LTE_RAT;
      }
      else
      {
        invalid_data = TRUE;
      }

      if(mru_data->rat == DSAT_LTE_RAT)
      {
        lte_mru_band_pref = (uint64)mru_entry->gwl.lte_band_pref.bit_15_0;
        lte_mru_band_pref |= (uint64)((uint64)mru_entry->gwl.lte_band_pref.bit_31_16<<16);
        lte_mru_band_pref |= (uint64)((uint64)mru_entry->gwl.lte_band_pref.bit_47_32<<32);
        lte_mru_band_pref |= (uint64)((uint64)mru_entry->gwl.lte_band_pref.bit_63_48<<48);
  
        size = vend_convert_lte_band_pref_to_sys_band( lte_mru_band_pref ,mru_data->band_pref,sizeof(mru_data->band_pref));
        if(size == 0)
        {
          invalid_data = TRUE;
        }
      }
     (void) dsatutil_memscpy((void*)mru_data->plmn.identity, 3,
                        (void*)&mru_entry->gwl.plmn_id.identity[0],3);      
#endif/* FEATURE_DSAT_LTE */
    break;

    case SYS_SYS_MODE_NO_SRV:
    case SYS_SYS_MODE_GPS:
    case SYS_SYS_MODE_MAX:
    case SYS_SYS_MODE_WLAN:
    case SYS_SYS_MODE_LTE:
    default:
     invalid_data = TRUE;
     break;
  }

  if( invalid_data )
  {
    DS_AT_MSG1_HIGH("Invalid Data in MRU ,%d mode ",mru_entry->mode);
    return FALSE;
  }
  return TRUE;
}/* vend_convert_mru */

/*===========================================================================

FUNCTION  VEND_CONVERT_BAND_PREF_TO_SYS_BAND32

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

uint32 vend_convert_band_pref_to_sys_band32
(
  uint32 dsat_band_pref,
  char *band_ptr,
  uint32 src_size
)
{
  dsat_num_item_type  size = 0;
  uint8 index = 0;
  boolean comma_required = FALSE;
  
  ASSERT(band_ptr != NULL);

  memset((void *)band_ptr,0x0,src_size);
  size = strlcat( band_ptr, "\"",src_size);

  DS_AT_MSG1_MED("vend_convert_band_pref_to_sys_band32 dsat_band_pref = %lx", dsat_band_pref);
  
  for(index = 0 ; index < ARR_SIZE(mru_band_pref_map_gw); index++)
  {
    if( DSAT_BAND_CHECK32( dsat_band_pref, mru_band_pref_map_gw[index].mask) )
    {
      if(comma_required)
      {
        size = strlcat( band_ptr, ",", src_size);
      }
      size = strlcat( band_ptr, mru_band_pref_map_gw[index].str, src_size);
      comma_required = TRUE;  
    }
  }

  size = strlcat( band_ptr, "\"",src_size);

  return size;
}/* vend_convert_band_pref_to_sys_band32 */

/*===========================================================================

FUNCTION  VEND_CONVERT_LTE_BAND_PREF_TO_SYS_BAND

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
uint32 vend_convert_lte_band_pref_to_sys_band
(
  uint64 dsat_band_pref,
  char *band_ptr,
  uint32 src_size
)
{
  dsat_num_item_type  size = 0;
  uint8 index = 0;
  boolean comma_required = FALSE;
  ASSERT(band_ptr != NULL);

  memset((void *)band_ptr,0x0,src_size);
  size = strlcat( band_ptr, "\"",src_size);
  
  DS_AT_MSG1_HIGH("Converting LTE %lx - dsat_band_pref ",dsat_band_pref);
  
  for( index = 0; index < ARR_SIZE(mru_band_pref_map_lte); index++)
  {
    if( DSAT_BAND_CHECK64( dsat_band_pref, mru_band_pref_map_lte[index].mask) )
    {
       if(comma_required)
       {
         size = strlcat( band_ptr, ",", src_size);
       }
       size = strlcat( band_ptr, mru_band_pref_map_lte[index].str ,src_size);
       comma_required = TRUE;
    }
  }
  
  size = strlcat( band_ptr, "\"",src_size);

  return size;

}/* vend_convert_lte_band_pref_to_sys_band */

/*===========================================================================

FUNCTION  VEND_CONVERT_MRU_BAND_INPUT_TO_GSM_BAND_PREF

DESCRIPTION
  Convert MRU band input value to corresponding GSM band preference value.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

LOCAL uint32 vend_convert_mru_band_input_to_gsm_band_pref
(
  dsat_mru_band_input_gsm_e_type band
)
{
  uint32 band_pref = DSAT_MRU_BAND_PREF_NONE;

  switch (band)
  {
    case DSAT_MRU_BAND_INPUT_GSM_450:
      band_pref = DSAT_MRU_BAND_PREF_GSM_450;
      break;

    case DSAT_MRU_BAND_INPUT_GSM_480:
      band_pref = DSAT_MRU_BAND_PREF_GSM_480;
      break;

    case DSAT_MRU_BAND_INPUT_GSM_750:
      band_pref = DSAT_MRU_BAND_PREF_GSM_750;
      break;

    case DSAT_MRU_BAND_INPUT_GSM_850:
      band_pref = DSAT_MRU_BAND_PREF_GSM_850;
      break;

    case DSAT_MRU_BAND_INPUT_GSM_EGSM_900:
      band_pref = DSAT_MRU_BAND_PREF_GSM_EGSM_900;
      break;

    case DSAT_MRU_BAND_INPUT_GSM_PGSM_900:
      band_pref = DSAT_MRU_BAND_PREF_GSM_PGSM_900;
      break;

    case DSAT_MRU_BAND_INPUT_GSM_RGSM_900:
      band_pref = DSAT_MRU_BAND_PREF_GSM_RGSM_900;
      break;

    case DSAT_MRU_BAND_INPUT_GSM_DCS_1800:
      band_pref = DSAT_MRU_BAND_PREF_GSM_DCS_1800;
      break;

    case DSAT_MRU_BAND_INPUT_GSM_PCS_1900:
      band_pref = DSAT_MRU_BAND_PREF_GSM_PCS_1900;
      break;

    default:
      break;
  }

  return band_pref;
}/* vend_convert_mru_band_input_to_gsm_band_pref */

/*===========================================================================

FUNCTION  VEND_CONVERT_MRU_BAND_INPUT_TO_WCDMA_BAND_PREF

DESCRIPTION
  Convert MRU band input value to corresponding WCDMA band preference value.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

LOCAL uint32 vend_convert_mru_band_input_to_wcdma_band_pref
(
  dsat_mru_band_input_wcdma_e_type band
)
{
  uint32 band_pref = DSAT_MRU_BAND_PREF_NONE;

  switch (band)
  {
    case DSAT_MRU_BAND_INPUT_WCDMA_I_IMT_2000:
      band_pref = DSAT_MRU_BAND_PREF_WCDMA_I_IMT_2000;
      break;

    case DSAT_MRU_BAND_INPUT_WCDMA_II_PCS_1900:
      band_pref = DSAT_MRU_BAND_PREF_WCDMA_II_PCS_1900;
      break;

    case DSAT_MRU_BAND_INPUT_WCDMA_III_1700:
      band_pref = DSAT_MRU_BAND_PREF_WCDMA_III_1700;
      break;

    case DSAT_MRU_BAND_INPUT_WCDMA_IV_1700:
      band_pref = DSAT_MRU_BAND_PREF_WCDMA_IV_1700;
      break;

    case DSAT_MRU_BAND_INPUT_WCDMA_V_850:
      band_pref = DSAT_MRU_BAND_PREF_WCDMA_V_850;
      break;

    case DSAT_MRU_BAND_INPUT_WCDMA_VI_800:
      band_pref = DSAT_MRU_BAND_PREF_WCDMA_VI_800;
      break;

    case DSAT_MRU_BAND_INPUT_WCDMA_VII_2600:
      band_pref = DSAT_MRU_BAND_PREF_WCDMA_VII_2600;
      break;

    case DSAT_MRU_BAND_INPUT_WCDMA_VIII_900:
      band_pref = DSAT_MRU_BAND_PREF_WCDMA_VIII_900;
      break;

    case DSAT_MRU_BAND_INPUT_WCDMA_IX_1700:
      band_pref = DSAT_MRU_BAND_PREF_WCDMA_IX_1700;
      break;

    case DSAT_MRU_BAND_INPUT_WCDMA_XI_1500:
      band_pref = DSAT_MRU_BAND_PREF_WCDMA_XI_1500;
      break;

    default:
      break;
  }

  return band_pref;
}/* vend_convert_mru_band_input_to_wcdma_band_pref */

/*===========================================================================

FUNCTION  VEND_CONVERT_MRU_BAND_INPUT_TO_LTE_BAND_PREF

DESCRIPTION
  Convert MRU band input value to corresponding LTE band preference value.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

LOCAL uint64 vend_convert_mru_band_input_to_lte_band_pref
(
  dsat_mru_band_input_lte_e_type band
)
{
  uint64 band_pref = DSAT_MRU_BAND_PREF_LTE_EMPTY;

  switch (band)
  {
    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND1:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_1;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND2:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_2;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND3:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_3;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND4:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_4;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND5:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_5;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND6:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_6;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND7:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_7;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND8:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_8;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND9:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_9;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND10:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_10;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND11:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_11;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND12:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_12;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND13:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_13;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND14:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_14;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND17:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_17;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND18:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_18;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND19:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_19;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND20:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_20;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND21:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_21;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND33:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_33;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND34:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_34;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND35:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_35;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND36:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_36;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND37:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_37;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND38:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_38;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND39:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_39;
      break;

    case DSAT_MRU_BAND_INPUT_LTE_EUTRAN_BAND40:
      band_pref = DSAT_MRU_BAND_PREF_LTE_EUTRAN_40;
      break;

    default:
      break;
  }

  return band_pref;
}/* vend_convert_mru_band_input_to_lte_band_pref */

/*===========================================================================

FUNCTION  VEND_VALIDATE_MRU_INPUTS

DESCRIPTION
  Validate input values in MRU write command.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

LOCAL boolean vend_validate_mru_inputs
(
  dsat_num_item_type index,
  dsat_num_item_type rat,
  dsat_num_item_type band
)
{
  boolean is_valid = FALSE;

  if ( index >= CM_MRU_TABLE_SIZE )
  {
    return FALSE;
  }

  switch ( rat )
  {
    case DSAT_CDMA_RAT:
    case DSAT_HDR_RAT:
      if ( ( band < DSAT_MRU_BAND_INPUT_3GPP2_MAX ) &&
           ( band != DSAT_MRU_BAND_INPUT_3GPP2_BC2 ) )
      {
        is_valid = TRUE;
      }
      break;

    case DSAT_GSM_RAT:
      if ( band < DSAT_MRU_BAND_INPUT_GSM_MAX )
      {
        is_valid = TRUE;
      }
      break;

    case DSAT_WCDMA_RAT:
      if ( band < DSAT_MRU_BAND_INPUT_WCDMA_MAX )
      {
        is_valid = TRUE;
      }
      break;

    case DSAT_LTE_RAT:
      if ( band < DSAT_MRU_BAND_INPUT_LTE_MAX )
      {
        is_valid = TRUE;
      }
      break;

    default:
      break;
  }

  return is_valid;
}/* vend_validate_mru_inputs */

/*===========================================================================

FUNCTION  VEND_MRU_CHANGE_SYS_OPRT_MODE

DESCRIPTION
  Change system operating mode in MRU commands.

DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/

LOCAL boolean vend_mru_change_sys_oprt_mode
(
  sys_oprt_mode_e_type new_mode
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD;

  if ( FALSE == cm_ph_cmd_oprt_mode ( dsatcmif_ph_cmd_cb_func,
                                      NULL,
                                      dsatcm_client_id,
                                      new_mode ) )
  {
    DS_AT_MSG1_ERROR("Failed to change sys oprt mode to %d", new_mode);

    if ( CHECK_NOT_PENDING( DSAT_VENDOR_QCMRUE_IDX, 0, DSAT_PENDING_MRU_NONE ) )
    {
      SET_PENDING( DSAT_VENDOR_QCMRUE_IDX, 0, DSAT_PENDING_MRU_NONE );
    }

    if ( CHECK_NOT_PENDING( DSAT_VENDOR_QCMRUC_IDX, 0, DSAT_PENDING_MRU_NONE ) )
    {
      SET_PENDING( DSAT_VENDOR_QCMRUC_IDX, 0, DSAT_PENDING_MRU_NONE );
    }

    result = DSAT_ERROR;
  }

  return result;
} /* vend_mru_change_sys_oprt_mode */

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCMRUE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$QCMRUE command.
  at$QCMRUE is used to edit MRU database. 

  As per the spec 3GPP2 C.S0057 and SD implementation only the
  following 3GPP2 bands and channels are supported: -
  Band 0, Channel 1-799, 991-1023
  Band 1, Channel 0-1199
  Band 3, Channel 1-799, 801-1039, 1041-1199, 1201-1600
  Band 4, Channel 0-599
  Band 5, Channel 1-400,  472-871, 1039-1473, 1536-1715, 1792-2016
  Band 6, Channel 0-1199
  Band 10, Channel 0-919
  Band 11, Channel 1-400, 472-871, 1536-1715
  Band 12, Channel 0-239
  Band 14, Channel 0-1299
  Band 15, Channel 0-899
  Band 16, Channel 140-459, 470-2719
  Band 17, Channel 140-1459 (Only in HDR)
  Band 18, Channel 0-240
  Band 19, Channel 0-360

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcmrue_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
#define DSAT_MIN_MRU_PLMN_LEN 5
#define DSAT_MAX_MRU_PLMN_LEN 6
  dsat_result_enum_type result = DSAT_OK;
  byte oper[DSAT_MAX_MRU_PLMN_LEN+1];
  byte buffer[8];
  dsat_num_item_type mcc = 0;
  dsat_num_item_type mnc = 0;
  boolean pcs_flag  = FALSE;
  dsat_error_info_s_type   err_info;
  dsat_num_item_type      size = 0;
  dsat_string_item_type   *efs_data = NULL;
  char                    *local_buffer = NULL;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if(tok_ptr->op == (NA|QU)||(tok_ptr->op == (NA|EQ|AR)))
  {
    if ((dsat_num_item_type)dsatutil_get_val(
             DSAT_EXT_CFUN_IDX,0,0,MIX_NUM_TYPE) != DSAT_ME_FUNC_FULL)
    {
      DS_AT_MSG0_HIGH(" MRU can be operated only in Online mode ");
      dsatme_set_cme_error(DSAT_CME_OP_NOT_SUPPORTED, res_buff_ptr);
      return  DSAT_CMD_ERR_RSP;
    }
    else
    {
      if(tok_ptr->op == (NA|EQ|AR))
      {
        if( tok_ptr->args_found != 4 )
        {
          err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
          goto send_error;
        }
        memset((void*)&dsat_mru_input,0x0,sizeof(dsat_mru_input));

        if ( ATOI_OK != dsatutil_atoi( &dsat_mru_input.index, tok_ptr->arg[0], 10 ) )
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.errval = 0;
          goto send_error;
        }

        if ( ATOI_OK != dsatutil_atoi( &dsat_mru_input.rat, tok_ptr->arg[1], 10 ) )
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.errval = 1;
          goto send_error;
        }

        if ( ATOI_OK != dsatutil_atoi( &dsat_mru_input.band, tok_ptr->arg[2], 10 ) )
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.errval = 2;
          goto send_error;
        }
        /* Channel input is valid only for CDMA and HDR */
        err_info.errval = 3;
        if((dsat_mru_input.rat == DSAT_CDMA_RAT) || (dsat_mru_input.rat == DSAT_HDR_RAT))
        {
          if ( ATOI_OK != dsatutil_atoi( &dsat_mru_input.channel, tok_ptr->arg[3], 10 ) )
          {
            err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
            goto send_error;;
          }
        }
        else /* For GSM,WCDMA,LTE treat the arg[3] as PLMN ID */
        {
            memset ((void*)oper, 0x0, sizeof(oper));

          /* Strip quotes */
           if ('"' == *tok_ptr->arg[3])
           {
             if (!dsatutil_strip_quotes_out(
                    tok_ptr->arg[3], &oper[0],
                    DSAT_MAX_MRU_PLMN_LEN+1))
             {
               err_info.errval = DSAT_ERR_QUOTE_STRIP_FAILURE;
               goto send_error;
             }
           }
           else
           {
             err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
             goto send_error;
           }

           if(strlen((char *)oper) == 0)
           {
             DS_AT_MSG0_HIGH("NULL PLMN ID found setting the acq_mode to Auto ");
             dsat_mru_input.is_plmn_valid = FALSE;
           }
           else if (strlen((char *)oper) < DSAT_MIN_MRU_PLMN_LEN)
           {
             err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
             goto send_error;
           }
           else
           {
             memset ((void*)buffer, 0x0, sizeof(buffer));
            (void)strlcat ((char*)buffer,
                               (char*)oper,
                               PLMN_STR_MCC_LEN+1);
             if ( ATOI_OK !=
                  dsatutil_atoi (&mcc, buffer, 10) )
             {
               DS_AT_MSG0_ERROR("Cannot convert MCC value");
               return DSAT_ERROR;
             }
            
             /* MNC is next 2 or 3 characters */
             memset ((void*)buffer, 0x0, sizeof(buffer));
             (void)strlcat ((char*)buffer,
                                (char*)(oper+PLMN_STR_MCC_LEN),
                                PLMN_STR_MNC_LEN+1);
             if ( ATOI_OK !=
                  dsatutil_atoi (&mnc, buffer, 10) )
             {
               DS_AT_MSG0_ERROR("Cannot convert MNC value");
               return DSAT_ERROR;
             }
            
             /* Convert MCC/MNC to PLMN, set 3rd MNC digit if MNC string
                was of 3 characters */
  
             if((PLMN_STR_MNC_LEN == strlen((char*)buffer)))
             {
               pcs_flag = TRUE;
             }
  
             if ( FALSE == 
                  sys_plmn_set_mcc_mnc( (mnc > 99 || pcs_flag)? TRUE : FALSE,
                                        mcc, mnc, &dsat_mru_input.plmn ) ) 
             { 
               DS_AT_MSG2_ERROR("Invalid mcc %d mnc %d found. Assuming Manual selection",mcc,mnc);
               dsat_mru_input.is_plmn_valid = FALSE;
             }
             else
             {
               dsat_mru_input.is_plmn_valid = TRUE;
             }
          }
        }

         /* Validate input */
         if ( FALSE == vend_validate_mru_inputs( dsat_mru_input.index,
                                                 dsat_mru_input.rat,
                                                 dsat_mru_input.band ) )
         {
           err_info.errval = DSAT_ERR_INVALID_TOKENS;
           goto send_error;
         }

         SET_PENDING( DSAT_VENDOR_QCMRUE_IDX, 0, DSAT_PENDING_MRU_WRITE );
         result = vend_mru_change_sys_oprt_mode( SYS_OPRT_MODE_LPM );
      }/* NA|EQ|AR */
      else
      {
        SET_PENDING( DSAT_VENDOR_QCMRUE_IDX, 0, DSAT_PENDING_MRU_READ );
        result = dsatvend_mru_action(DSAT_PENDING_MRU_READ);
      }
    }
  }
  else if ( tok_ptr->op == (NA|EQ|QU))
  {
    res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                 res_buff_ptr->size,
#ifdef FEATURE_DSAT_LTE
                   "$QCMRUE: (0-11),\n(0-CDMA,1-HDR,2-GSM,3-WCDMA,4-LTE),\n");
#else
                   "$QCMRUE: (0-11),\n(0-CDMA,1-HDR,2-GSM,3-WCDMA),\n");
#endif /* FEATURE_DSAT_LTE */

    result = DSAT_ERROR;
    if((size = dsat_read_efs_data("mru", DSAT_FILE_STR_TYPE, (void **)&efs_data)))
    {
      result = DSAT_OK;
      if(!dsat_process_efs_data(efs_data,size,res_buff_ptr,DSAT_EFS_MRU_TEST,NULL))
      {
        result = DSAT_ERROR;
      }
      /*Free the previous allocated efs_data for MRU file*/
      dsatutil_free_memory_ext((void **)&efs_data);
    }
#ifdef FEATURE_DSAT_LTE
    if (result != DSAT_ERROR) 
    {
      if((size = dsat_read_efs_data("mru_lte", DSAT_FILE_STR_TYPE, (void **)&efs_data)))
      {
        result = DSAT_OK;
        if(!dsat_process_efs_data(efs_data,size,res_buff_ptr,DSAT_EFS_MRU_TEST,NULL))
        {
          result = DSAT_ERROR;
        }
      }
      /*Free the previous allocated efs_data for MRU file*/
      dsatutil_free_memory_ext((void **)&efs_data);
    }
#endif /* FEATURE_DSAT_LTE */
    if (result != DSAT_ERROR) 
    {
      local_buffer = "\"(0 - 2047)\"";

      dsat_dsm_pushdown_tail(&res_buff_ptr,
                             local_buffer,
                             strlen(local_buffer),
                             DSM_ITEM_POOL(res_buff_ptr),
                             FALSE);
    }
  }
  return result;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_NUM_OF_ARGS ||
      err_info.errval == DSAT_ERR_INVALID_TOKENS )
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  /*Free the previous allocated efs_data for MRU file*/
  dsatutil_free_memory_ext((void **)&efs_data);
  return DSAT_ERROR;

}/* dsatvend_exec_qcmrue_cmd */


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCMRUC_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$QCMRUC command.
  at$QCMRUC is used to clear MRU database. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcmruc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  if(tok_ptr->op == (NA))
  {
    if ((dsat_num_item_type)dsatutil_get_val(
             DSAT_EXT_CFUN_IDX,0,0,MIX_NUM_TYPE) != DSAT_ME_FUNC_FULL)
    {
      DS_AT_MSG0_HIGH(" MRU can be operated only in Online mode ");
      dsatme_set_cme_error(DSAT_CME_OP_NOT_SUPPORTED, res_buff_ptr);
      return  DSAT_CMD_ERR_RSP;
    }
    else
    {
      SET_PENDING( DSAT_VENDOR_QCMRUC_IDX, 0, DSAT_PENDING_MRU_CLEAR );
      result = vend_mru_change_sys_oprt_mode( SYS_OPRT_MODE_LPM );
    }
  }
  else
  {
    DS_AT_MSG0_HIGH(" Unsupported command format ");
    result = DSAT_ERROR;
  }
  return result;
}/* dsatvend_exec_qcmrue_cmd */

/*===========================================================================

FUNCTION  DSATVEND_MRU_READ_CM_CMD

DESCRIPTION
  This function commands CM to read a MRU table index.
  The read data is returned in callback function.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_ASYNC_CMD :  if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_mru_read_cm_cmd
(
  dsat_num_item_type index
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD;
  static cm_mmode_mru_table_entry_u_type mru_table_entry;

  memset(&mru_table_entry, 0, sizeof(mru_table_entry));

  DS_AT_MSG1_HIGH("Commanding CM to read MRU table index %d", index);

  if (TRUE != cm_ph_cmd_read_mru( dsatcmif_ph_cmd_cb_func,
                                  &mru_table_entry,
                                  dsatcm_client_id,
                                  index ))
  {
    DS_AT_MSG0_ERROR("CM command to read MRU table index is failed");
    SET_PENDING( DSAT_VENDOR_QCMRUE_IDX, 0, DSAT_PENDING_MRU_NONE );
    result = DSAT_ERROR;
  }

  return result;
} /* dsatvend_mru_read_cm_cmd */

/*===========================================================================

FUNCTION DSATVEND_MRU_ACTION

DESCRIPTION
  This function carries the action on the MRU tables

DEPENDENCIES
  Phone should be in Offline.

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT: If there is a further Async calls made
    DSAT_OK: If Everything is fine.
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_mru_action
(
  dsat_cmd_pending_enum_type action
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD;
  cm_mmode_mru_table_entry_u_type mru_entry;
  uint32 temp;
#ifdef FEATURE_DSAT_LTE
  uint64 temp2;
#endif /* FEATURE_DSAT_LTE */

  DS_AT_MSG1_HIGH("In MRU %d action ",action);

  switch(action)
  {
    case DSAT_PENDING_MRU_CLEAR:
    {
      if (TRUE != cm_ph_cmd_mru_update( dsatcmif_ph_cmd_cb_func,
                                        NULL,
                                        dsatcm_client_id,
                                        &mru_entry,
                                        0,
                                        0,
                                        CM_MRU_CLEAN ))
      {
        DS_AT_MSG0_ERROR("CM command to update MRU table is failed");

        SET_PENDING( DSAT_VENDOR_QCMRUC_IDX, 0, DSAT_PENDING_MRU_RESULT );

        /* Store result to report it after operating mode changed back to ONLINE */
        dsat_mru_result = DSAT_ERROR;

        result = vend_mru_change_sys_oprt_mode( SYS_OPRT_MODE_ONLINE );
      }
    }
    break;

    case DSAT_PENDING_MRU_READ:
    {
      dsm_item_type *res_buff_ptr = NULL;

      dsat_mru_read_index = 0;

      result = dsatvend_mru_read_cm_cmd(dsat_mru_read_index);
      if (result == DSAT_ASYNC_CMD)
      {
        res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
        res_buff_ptr->used += (word)snprintf ( (char *)res_buff_ptr->data_ptr,
                                               res_buff_ptr->size,
                                               "$QCMRUE: ");

        dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
      }
    }
    break;

    case DSAT_PENDING_MRU_WRITE:
    {
      memset((void *)&mru_entry, 0x0, sizeof(mru_entry));

      switch(dsat_mru_input.rat)
      {
        case DSAT_CDMA_RAT:
        {
          mru_entry.mode = SYS_SYS_MODE_CDMA;
          mru_entry.cdma.mode = SYS_SYS_MODE_CDMA;
          mru_entry.cdma.band = (sys_band_class_e_type)dsat_mru_input.band;
          mru_entry.cdma.channel = (sys_channel_num_type)dsat_mru_input.channel;
        }
        break;
        case DSAT_HDR_RAT:
        {
          mru_entry.mode = SYS_SYS_MODE_HDR;
          mru_entry.cdma.mode = SYS_SYS_MODE_HDR;
          mru_entry.cdma.band = (sys_band_class_e_type)dsat_mru_input.band;
          mru_entry.cdma.channel = (sys_channel_num_type)dsat_mru_input.channel;
        }
        break;
        case DSAT_GSM_RAT:
        {
          mru_entry.mode = SYS_SYS_MODE_GSM;
          mru_entry.gsm.mode = SYS_SYS_MODE_GSM;
          temp = vend_convert_mru_band_input_to_gsm_band_pref(dsat_mru_input.band);
          mru_entry.gsm.band_pref.bit_15_0 = (word) ((word) temp & 0xffff );
          mru_entry.gsm.band_pref.bit_31_16 =(word) ( (uint32)temp >> 16 );
          if(dsat_mru_input.is_plmn_valid )
          {
            mru_entry.gsm.net_select_mode = 4; /* SD_NET_SELECT_MODE_MANUAL_SYS */
          (void) dsatutil_memscpy((void*)&mru_entry.gsm.plmn_id.identity[0], 3,
                             (void*)dsat_mru_input.plmn.identity,3);          
          }
          else
          {
            mru_entry.gsm.net_select_mode = 1; /* SD_NET_SELECT_MODE_AUTO */
            memset((byte *)&mru_entry.gsm.plmn_id.identity[0],0xFF,3);
          }
        }
        break;
        case DSAT_WCDMA_RAT:
        {
          mru_entry.mode = SYS_SYS_MODE_WCDMA;
          mru_entry.wcdma.mode = SYS_SYS_MODE_WCDMA;
          temp = vend_convert_mru_band_input_to_wcdma_band_pref(dsat_mru_input.band);
          mru_entry.wcdma.bc.band_pref.bit_15_0 = (word) ((word) temp & 0xffff );
          mru_entry.wcdma.bc.band_pref.bit_31_16 =(word) ( (uint32)temp >> 16 );
          if(dsat_mru_input.is_plmn_valid )
          {
            mru_entry.wcdma.net_select_mode = 4; /* SD_NET_SELECT_MODE_MANUAL_SYS */
          (void) dsatutil_memscpy((void*)&mru_entry.wcdma.plmn_id.identity[0], 3,
                             (void*)dsat_mru_input.plmn.identity,3);         
          }
          else
          {
            mru_entry.wcdma.net_select_mode = 1; /* SD_NET_SELECT_MODE_AUTO */
            memset((byte *)&mru_entry.wcdma.plmn_id.identity[0],0xFF,3);
          }
        }
        break;
#ifdef FEATURE_DSAT_LTE
        case DSAT_LTE_RAT:
        {
          mru_entry.mode = SYS_SYS_MODE_GWL;
          mru_entry.gwl.mode = SYS_SYS_MODE_GWL;
          mru_entry.gwl.acq_mode = SYS_SYS_MODE_LTE;
          temp2 = vend_convert_mru_band_input_to_lte_band_pref(dsat_mru_input.band);
          mru_entry.gwl.lte_band_pref.bit_15_0 = (word) ((word) temp2 & 0xffff );
          mru_entry.gwl.lte_band_pref.bit_31_16 =(word) ((uint64)temp2 >> 16 );
          mru_entry.gwl.lte_band_pref.bit_47_32 = (word) ((uint64) temp2 >> 32 );
          mru_entry.gwl.lte_band_pref.bit_63_48 =(word) ((uint64)temp2 >> 48 );
          mru_entry.gwl.num_rats = 1; 
          mru_entry.gwl.rat[0] = SD_MODE_LTE;
          if(dsat_mru_input.is_plmn_valid )
          {
            mru_entry.gwl.net_select_mode = 4; /* SD_NET_SELECT_MODE_MANUAL_SYS */
           (void) dsatutil_memscpy((void*)&mru_entry.gwl.plmn_id.identity[0], 3,
                             (void*)dsat_mru_input.plmn.identity,3);           
          }
          else
          {
            mru_entry.gwl.net_select_mode = 1; /* SD_NET_SELECT_MODE_AUTO */
            memset((byte *)&mru_entry.gwl.plmn_id.identity[0],0xFF,3);
          }
        }
        break;
#endif /* FEATURE_DSAT_LTE */
        default:
        break;
      }

      if (TRUE != cm_ph_cmd_mru_update( dsatcmif_ph_cmd_cb_func,
                                        NULL,
                                        dsatcm_client_id,
                                        &mru_entry,
                                        dsat_mru_input.index,
                                        mru_entry.mode,
                                        CM_MRU_UPDATE ))
      {
        DS_AT_MSG0_ERROR("CM command to update MRU table is failed");

        SET_PENDING( DSAT_VENDOR_QCMRUE_IDX, 0, DSAT_PENDING_MRU_RESULT );

        /* Store result to report it after operating mode changed back to ONLINE */
        dsat_mru_result = DSAT_ERROR;

        result = vend_mru_change_sys_oprt_mode( SYS_OPRT_MODE_ONLINE );
      }
    }/* case - DSAT_MRU_WRITE */
    break;

    case DSAT_PENDING_MRU_RESULT:
    {
      result = dsat_mru_result;
    }
    break;

    default:
      result = DSAT_OK;
  }

  return result;
}/* dsatvend_mru_action */

#ifdef FEATURE_DSAT_LTE 
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCACQDB_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCACQDBC command.
  at$QCACQDBC is used to clear ACQ database. 

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcacqdbc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  if(tok_ptr->op == (NA))
  {
    if(0 != lte_rrc_delete_acq_db())
    {
      result = DSAT_ERROR;
    }
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}/* dsatvend_exec_qcacqdb_cmd */
#endif /* FEATURE_DSAT_LTE */

/*===========================================================================
FUNCTION DSATVEND_PROCESS_MRU_UPDATE_RESULT

DESCRIPTION
  This function processes the MRU update result.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT: If there is a further Async calls made
    DSAT_OK: If Everything is fine.
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_mru_update_result
(
  cm_ph_cmd_err_e_type err
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD;

  DS_AT_MSG0_MED("Process MRU update result");

  if ( CHECK_PENDING( DSAT_VENDOR_QCMRUE_IDX, 0, DSAT_PENDING_MRU_WRITE ) )
  {
    SET_PENDING( DSAT_VENDOR_QCMRUE_IDX, 0, DSAT_PENDING_MRU_RESULT );
  }
  else if ( CHECK_PENDING( DSAT_VENDOR_QCMRUC_IDX, 0, DSAT_PENDING_MRU_CLEAR ) )
  {
    SET_PENDING( DSAT_VENDOR_QCMRUC_IDX, 0, DSAT_PENDING_MRU_RESULT );
  }
  else
  {
    if (CM_PH_CMD_ERR_NOERR != err)
    {
      dsat_mru_result = DSAT_ERROR;
    }
  
    DS_AT_MSG0_HIGH("Ignoring if there is no pending MRU_WRITE or MRU_CLEAR");
    return DSAT_ASYNC_EVENT;
  }

  /* Store result to report it after operating mode changed back to ONLINE */
  if (CM_PH_CMD_ERR_NOERR == err)
  {
    dsat_mru_result = DSAT_OK;
  }
  else
  {
    dsat_mru_result = DSAT_ERROR;
  }

  result = vend_mru_change_sys_oprt_mode( SYS_OPRT_MODE_ONLINE );

  return result;
}/* dsatvend_process_mru_update_result */

/*===========================================================================

FUNCTION DSATVEND_PROCESS_MRU_READ_RESULT

DESCRIPTION
  This function processes the MRU read result.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT: If there is a further Async calls made
    DSAT_OK: If Everything is fine.
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_mru_read_result
(
  cm_ph_cmd_err_e_type             err,
  cm_mmode_mru_table_entry_u_type *mru_entry
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD;
  dsm_item_type *res_buff_ptr = NULL;
  dsat_mru_input_s_type mru_data;

  DS_AT_MSG0_MED("Process MRU read result");

  if ( CHECK_NOT_PENDING( DSAT_VENDOR_QCMRUE_IDX, 0, DSAT_PENDING_MRU_READ ) )
  {
    DS_AT_MSG0_ERROR("Unexpected MRU read result");
    return DSAT_ASYNC_CMD;
  }

  if ((CM_PH_CMD_ERR_NOERR != err) || (NULL == mru_entry))
  {
    DS_AT_MSG2_ERROR("Process MRU read result: err = %d, mru_entry = %p",
                     err, mru_entry);

    SET_PENDING( DSAT_VENDOR_QCMRUE_IDX, 0, DSAT_PENDING_MRU_NONE );
    return DSAT_ERROR;
  }

  if( TRUE == vend_convert_mru(mru_entry, &mru_data) )
  {
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
    if((mru_data.rat == DSAT_CDMA_RAT)||(mru_data.rat == DSAT_HDR_RAT))
    {
      res_buff_ptr->used += (word)snprintf((char *)res_buff_ptr->data_ptr,
                                           res_buff_ptr->size,
                                           "%d,%d,%d,%d",
                                           dsat_mru_read_index,
                                           mru_data.rat,
                                           mru_data.band,
                                           mru_data.channel);
    }
    else
    {  
      sys_mcc_type  mcc;
      sys_mnc_type  mnc;
      boolean undef = TRUE;
      boolean pcs_flag = FALSE;
      sys_plmn_get_mcc_mnc( mru_data.plmn, &undef, &pcs_flag, &mcc, &mnc );
      if (TRUE == undef)
      {
        mcc = mnc = 0;
        pcs_flag = FALSE;
      }
      if(pcs_flag)
      {
        res_buff_ptr->used += (word)snprintf((char *)res_buff_ptr->data_ptr,
                                             res_buff_ptr->size,
                                             "%d,%d,%s,\"%03lu%03lu\"",
                                             dsat_mru_read_index,
                                             mru_data.rat,
                                             mru_data.band_pref,
                                             mcc,mnc);
      }
      else
      {
        res_buff_ptr->used += (word)snprintf((char *)res_buff_ptr->data_ptr,
                                             res_buff_ptr->size,
                                             "%d,%d,%s,\"%03lu%02lu\"",
                                             dsat_mru_read_index,
                                             mru_data.rat,
                                             mru_data.band_pref,
                                             mcc,mnc);
      }
    }

    dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_END_OF_RSP);
  }

  dsat_mru_read_index++;

  if (dsat_mru_read_index >= CM_MRU_TABLE_SIZE)
  {
    SET_PENDING( DSAT_VENDOR_QCMRUE_IDX, 0, DSAT_PENDING_MRU_NONE );
    result = DSAT_OK;
  }
  else
  {
    result = dsatvend_mru_read_cm_cmd(dsat_mru_read_index);
  }

  return result;
}/* dsatvend_process_mru_read_result */

#ifdef FEATURE_DATA_UCSD_SCUDIF_TEST
void ds_ucsd_trigger_incall_modification( uint8 dir, uint8 test_id );



/*===========================================================================

FUNCTION DSATVEND_EXEC_QCSCFTEST_CMD

DESCRIPTION
  This function sets the SCUIDF test mode.

DEPENDENCIES
  The values exported may or may not be appropriate for the mode-specific
  handler.  Validation in mode-specific handler is assumed. 
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcscftest_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ERROR;
  #define MO_INDEX 0
  #define MT_INDEX 1
  #define INCALL_SC_MODE_TEST 6
  
  /* Processing for WRITE syntax */
  if ( (tok_ptr->op == (NA|EQ|AR)) )
  {
    dsat_num_item_type orig_val[2];

    (void) dsatutil_memscpy((void*)orig_val, sizeof(orig_val),
             (void*)dsatutil_get_val(DSAT_VENDOR_QCSCFTEST_IDX,0,0,STR_TYPE),sizeof(orig_val));
    
    /* Perform default paramater processing */
    if (DSAT_OK == dsatparm_exec_param_cmd( mode,
                                            parse_table,
                                            tok_ptr,
                                            res_buff_ptr ))
    {
      /* Test for action conditions */
      if( INCALL_SC_MODE_TEST == (dsat_num_item_type)dsatutil_get_val(
                               DSAT_VENDOR_QCSCFTEST_IDX,0,1,NUM_TYPE))
      {
        if( MO_INDEX == (dsat_num_item_type)dsatutil_get_val(
                              DSAT_VENDOR_QCSCFTEST_IDX,0,0,NUM_TYPE) )
        {
          ds_ucsd_trigger_incall_modification( MO_INDEX, INCALL_SC_MODE_TEST );
          (void) dsatutil_memscpy((void*)dsatutil_get_val(DSAT_VENDOR_QCSCFTEST_IDX,0,0,STR_TYPE), sizeof(orig_val),
                   (void*)orig_val,sizeof(orig_val));
        }
        else if( MT_INDEX == (dsat_num_item_type)dsatutil_get_val(
                                DSAT_VENDOR_QCSCFTEST_IDX,0,0,NUM_TYPE) )
        {
          ds_ucsd_trigger_incall_modification( MT_INDEX, INCALL_SC_MODE_TEST );

          (void) dsatutil_memscpy((void*)dsatutil_get_val(DSAT_VENDOR_QCSCFTEST_IDX,0,0,STR_TYPE), sizeof(orig_val),
                   (void*)orig_val,sizeof(orig_val));
        }
      }
      /* Otherwise populate the exported values */
      else
      {
        dsat_set_qcscftest_exp_val(((dsat_num_item_type)dsatutil_get_val(
              DSAT_VENDOR_QCSCFTEST_IDX,0,0,NUM_TYPE)), 
           (uint8)dsatutil_get_val(DSAT_VENDOR_QCSCFTEST_IDX,0,1,NUM_TYPE));
      }
      result = DSAT_OK;
    }
  }
  /* Processing for TEST and READ syntax */
  else if ( tok_ptr->op == (NA|QU) )
  {
    char buffer[80];  /* scratch buffer */
    word size;
    
    size = (word)snprintf( buffer, sizeof(buffer),
                           "%s: %s, %d\n"
                           "%s: %s, %d",
                           parse_table->name,
                           dsat_qcscftest_list[0].list_v[DSAT_QCSCFTEST_MO_INDEX],
                           dsat_get_qcscftest_exp_val(DSAT_QCSCFTEST_MO_INDEX),
                           parse_table->name,
                           dsat_qcscftest_list[0].list_v[DSAT_QCSCFTEST_MT_INDEX],
                           dsat_get_qcscftest_exp_val(DSAT_QCSCFTEST_MT_INDEX) );
    
    dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                           DSM_ITEM_POOL(res_buff_ptr), FALSE);
    
    result = DSAT_OK;
  }
  else if ( (tok_ptr->op == (NA|EQ|QU)) ||
            (tok_ptr->op == (NA)) )
  {
    result = dsatparm_exec_param_cmd( mode,
                                      parse_table,
                                      tok_ptr,
                                      res_buff_ptr );

    /* For default case, export default parameters */
    if (tok_ptr->op == (NA))
    {
      dsat_set_qcscftest_exp_val( DSAT_QCSCFTEST_MO_INDEX, 
                                  (uint8)dsat_qcscftest_list[1].default_v);
      dsat_set_qcscftest_exp_val( DSAT_QCSCFTEST_MT_INDEX,
                                  (uint8)dsat_qcscftest_list[1].default_v);
    }
  }

  return result;
} /* dsatvend_exec_qcscftest_cmd */

#endif /* FEATURE_DATA_UCSD_SCUDIF_TEST */
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCSYSMODE_CMD

DESCRIPTION
  This function returns the current system mode.

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcsysmode_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD;
  
  if ((tok_ptr->op == NA) || (tok_ptr->op == (NA|QU)) )
  {
    SET_PENDING(DSAT_VENDOR_QCSYSMODE_IDX, 0, DSAT_PENDING_TRUE)

    if (dsatcmif_get_cm_ss_info() == DSAT_ERROR)
    {
      result = DSAT_ERROR;

      SET_PENDING(DSAT_VENDOR_QCSYSMODE_IDX, 0, DSAT_PENDING_FALSE)
    }
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}/* dsatvend_exec_qcsysmode_cmd */

/*===========================================================================

FUNCTION DSAVEND_PROCESS_QCSYSMODE_CMD

DESCRIPTION
  This function process the response for the $QCSYSMODE command
  on return from asynchronous processing.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_qcsysmode_cmd
(
  dsat_cm_msg_s_type *msg_ptr
)
{
  dsat_result_enum_type result = DSAT_ERROR;
  dsat_qcsysmode_id_e_type qcsysmode_id = DSAT_QCSYSMODE_ID_MAX;
  dsat_qcsysmode_hs_ind_e_type qcsysmode_hs_ind = DSAT_QCSYSMODE_HS_IND_NONE;
#ifdef FEATURE_DATA_IS707
  ps_iface_bearer_technology_type bearer_tech;
#endif /* FEATURE_DATA_IS707 */
  dsm_item_type *res_buff_ptr = NULL;

  DS_AT_MSG3_HIGH("hdr_hybrid = %d, hdr_srv_status = %d, hs_ind = %d",
                  msg_ptr->msg.ss_event.event_info[DSAT_STACK_HYBR].hdr_hybrid,
                  msg_ptr->msg.ss_event.event_info[DSAT_STACK_HYBR].hdr_srv_status,
                  msg_ptr->msg.ss_event.event_info[DSAT_STACK_MAIN].cell_srv_ind.hs_ind);

  switch (msg_ptr->msg.ss_event.event_info[DSAT_STACK_MAIN].sys_mode)
  {
    case SYS_SYS_MODE_NO_SRV:
      qcsysmode_id = DSAT_QCSYSMODE_ID_NO_SRV;
      break;

    case SYS_SYS_MODE_CDMA:
      if ((msg_ptr->msg.ss_event.event_info[DSAT_STACK_HYBR].hdr_hybrid == TRUE) &&
         (msg_ptr->msg.ss_event.event_info[DSAT_STACK_HYBR].hdr_srv_status == SYS_SRV_STATUS_SRV))
      {
        qcsysmode_id = DSAT_QCSYSMODE_ID_CDMA_HDR;
      }
      else if ((msg_ptr->msg.ss_event.event_info[DSAT_STACK_HYBR].is_operational == TRUE) &&
              (msg_ptr->msg.ss_event.event_info[DSAT_STACK_HYBR].sys_mode == SYS_SYS_MODE_LTE))
      {
        qcsysmode_id = DSAT_QCSYSMODE_ID_CDMA_LTE;
      }
      else
      {
        qcsysmode_id = DSAT_QCSYSMODE_ID_CDMA;
      }
     break;

    case SYS_SYS_MODE_GSM:
      qcsysmode_id = DSAT_QCSYSMODE_ID_GSM;
      break;

    case SYS_SYS_MODE_HDR:
      qcsysmode_id = DSAT_QCSYSMODE_ID_HDR;

#ifdef FEATURE_DATA_IS707
      if (ds707_pkt_mgr_get_bearer_tech_info(&bearer_tech) == TRUE)
      {
        switch (bearer_tech.data.cdma_type.rat_mask)
        {
          case PS_IFACE_CDMA_EVDO_REV0:
            qcsysmode_id = DSAT_QCSYSMODE_ID_HDR_REV0;
            break;

          case PS_IFACE_CDMA_EVDO_REVA:
            if (bearer_tech.data.cdma_type.so_mask == PS_IFACE_CDMA_EVDO_EMPA_EHRPD)
            {
              qcsysmode_id = DSAT_QCSYSMODE_ID_HDR_EMPA_EHRPD;
            }
            else
            {
              qcsysmode_id = DSAT_QCSYSMODE_ID_HDR_REVA;
            }
            break;

          case PS_IFACE_CDMA_EVDO_REVB:
            if (bearer_tech.data.cdma_type.so_mask == PS_IFACE_CDMA_EVDO_MMPA_EHRPD)
            {
              qcsysmode_id = DSAT_QCSYSMODE_ID_HDR_MMPA_EHRPD;
            }
            else
            {
              qcsysmode_id = DSAT_QCSYSMODE_ID_HDR_REVB;
            }
            break;

          default:
            DS_AT_MSG0_ERROR("Invalid HDR Protocol");

        }
      }
      else
      {
        DS_AT_MSG0_ERROR("Couldn't get HDR Protocol");
      }
#endif /* FEATURE_DATA_IS707 */
      break;

    case SYS_SYS_MODE_WCDMA:
      qcsysmode_id = DSAT_QCSYSMODE_ID_WCDMA;
      break;

    case SYS_SYS_MODE_LTE:
      qcsysmode_id = DSAT_QCSYSMODE_ID_LTE;
      break;

    case SYS_SYS_MODE_TDS:
      qcsysmode_id = DSAT_QCSYSMODE_ID_TDS;
      break;

    default:
      DS_AT_MSG0_ERROR("Invalid Sys Mode");
      break;
  }

  if (DSAT_QCSYSMODE_ID_WCDMA == qcsysmode_id || DSAT_QCSYSMODE_ID_TDS == qcsysmode_id )
  {
      switch(msg_ptr->msg.ss_event.event_info[DSAT_STACK_MAIN].cell_srv_ind.hs_ind)
      {
        case SYS_HS_IND_HSDPA_HSUPA_UNSUPP_CELL:
          break;

        case SYS_HS_IND_HSDPA_SUPP_CELL: 
        qcsysmode_hs_ind = DSAT_QCSYSMODE_HS_IND_HSDPA;
          break;

        case SYS_HS_IND_HSUPA_SUPP_CELL:
        qcsysmode_hs_ind = DSAT_QCSYSMODE_HS_IND_HSUPA;
          break;

        case SYS_HS_IND_HSDPA_HSUPA_SUPP_CELL: 
        qcsysmode_hs_ind = DSAT_QCSYSMODE_HS_IND_HSDPA_HSUPA;
          break;

        case SYS_HS_IND_HSDPAPLUS_SUPP_CELL: 
        qcsysmode_hs_ind = DSAT_QCSYSMODE_HS_IND_HSDPA_PLUS;
          break;

        case SYS_HS_IND_HSDPAPLUS_HSUPA_SUPP_CELL: 
        qcsysmode_hs_ind = DSAT_QCSYSMODE_HS_IND_HSDPA_PLUS_HSUPA;
          break;

        case SYS_HS_IND_DC_HSDPAPLUS_SUPP_CELL: 
        qcsysmode_hs_ind = DSAT_QCSYSMODE_HS_IND_DC_HSDPA_PLUS;
          break;

        case SYS_HS_IND_DC_HSDPAPLUS_HSUPA_SUPP_CELL: 
        qcsysmode_hs_ind = DSAT_QCSYSMODE_HS_IND_DC_HSDPA_PLUS_HSUPA;
          break;

        case SYS_HS_IND_HSDPAPLUS_64QAM_SUPP_CELL: 
        qcsysmode_hs_ind = DSAT_QCSYSMODE_HS_IND_HSDPA_PLUS_64QAM;
          break;

        case SYS_HS_IND_HSDPAPLUS_64QAM_HSUPA_SUPP_CELL: 
        qcsysmode_hs_ind = DSAT_QCSYSMODE_HS_IND_HSDPA_PLUS_HSUPA_64QAM;
          break;

      case SYS_HS_IND_DC_HSDPAPLUS_DC_HSUPA_SUPP_CELL:
        qcsysmode_hs_ind = DSAT_QCSYSMODE_HS_IND_DC_HSDPA_PLUS_DC_HSUPA_SUPP_CELL;
      break;

    default:
        DS_AT_MSG0_ERROR("Invalid HS Indication");
      break;
  }
  }

  if ((qcsysmode_id < DSAT_QCSYSMODE_ID_MAX) && (qcsysmode_hs_ind < DSAT_QCSYSMODE_HS_IND_MAX))
  {
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
    res_buff_ptr->used = snprintf( (char*)res_buff_ptr->data_ptr,
                                    res_buff_ptr->size,
                                    "%s%s",
                                    (const char*)dsat_qcsysmode_text[qcsysmode_id],
                                    (const char*)dsat_qcsysmode_hs_ind[qcsysmode_hs_ind]);

    dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);

    result = DSAT_OK;
  }
  else
  {
    result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
  }

  return result;
}/* dsatvend_process_qcsysmode_cmd */
#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCATMOD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcatmod command.
  AT$QCATMOD is used indicate the SIOLIB AT Command Processing State.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcatmod_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{

   ds3g_siolib_port_e_type     port_count = DS3G_SIOLIB_DATA_PORT;
   boolean                     dtr_status = FALSE;
   ds3g_siolib_state_info_type *state = NULL;
   uint8                       flag = FALSE;

  if (tok_ptr->op == (NA|EQ|AR) ) 
  {
    DS_AT_MSG0_ERROR("Write command is not supported ");
    return DSAT_ERROR;
  } 
  else if(tok_ptr->op == (NA|EQ|QU))
  {
    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                              res_buff_ptr->size,
                                              "$QCATMOD:(%d - %d),(%d - %d)",
                                              DS3G_SIOLIB_DATA_PORT,
                                              DS3G_SIOLIB_PORT_MAX-1,
                                              DS3G_SIOLIB_NULL_AT_STATE,
                                              DS3G_SIOLIB_ONLINE_CMD_AT_STATE);
  }
  else if(tok_ptr->op == (NA|QU) || (tok_ptr->op == NA))
  {
    while( port_count < DS3G_SIOLIB_PORT_MAX ) 
    { 
      (void)ds3g_siolib_ex_is_dtr_asserted(&dtr_status, port_count); 
      state = ds3g_siolib_get_ds3g_siolib_state(port_count); 
      if( state->serial_info.serial_state == DS3G_SIOLIB_AUTODETECT_SERIAL_STATE && dtr_status == TRUE) 
      {
        res_buff_ptr->used += (word) snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                       res_buff_ptr->size - res_buff_ptr->used,
                                                      "$QCATMOD: %d, %d \n",port_count,state->at_state);
        flag = TRUE;
     
      }
      port_count++;
    }
    if(flag == FALSE)
    {
      state = ds3g_siolib_get_ds3g_siolib_state(DS3G_SIOLIB_CLIENT_VSP_PORT);
     res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                            res_buff_ptr->size,
                                                         "$QCATMOD: %d, %d \n",DS3G_SIOLIB_CLIENT_VSP_PORT,
                                                          state->at_state);
    }
  }
  else
  {
    return DSAT_ERROR;
  }
  return DSAT_OK;
}/*dsatvend_exec_qcatmod_cmd*/
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT*/
/*===========================================================================

FUNCTION  VEND_CONVERT_RSSI_ANTENNA_NUMBER

DESCRIPTION
  This function converts rssi value to antenna signal level.

DEPENDENCIES
  None

RETURN VALUE
  dsat_ante_rssi_level_e_type.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_ante_rssi_level_e_type vend_convert_rssi_antenna_number
(
  void
)
{
#define SIGNAL_LEVEL_60   60
#define SIGNAL_LEVEL_75   75
#define SIGNAL_LEVEL_90   90
#define SIGNAL_LEVEL_105  105

  dsat_ante_rssi_level_e_type antenna_lvl;
  dsatcmif_servs_state_ms_info  *ph_ss_ms_val = NULL;
  
  ph_ss_ms_val = dsat_get_base_addr_current_subs(DSAT_SS_PH_MS_VALS);

  if ( ph_ss_ms_val->dsatcmif_signal_reporting.rssi < SIGNAL_LEVEL_60 )
  {
    antenna_lvl = RSSI_LEVEL_4;
  }
  else if ( ph_ss_ms_val->dsatcmif_signal_reporting.rssi < SIGNAL_LEVEL_75 )
  {
    antenna_lvl = RSSI_LEVEL_3;
  }
  else if ( ph_ss_ms_val->dsatcmif_signal_reporting.rssi < SIGNAL_LEVEL_90 )
  {
    antenna_lvl = RSSI_LEVEL_2;
  }
  else if ( ph_ss_ms_val->dsatcmif_signal_reporting.rssi < SIGNAL_LEVEL_105 )
  {
    antenna_lvl = RSSI_LEVEL_1;
  }
  else
  {
    antenna_lvl = RSSI_LEVEL_0;
  }
  if( ph_ss_ms_val->dsatcmif_signal_reporting.rssi == RSSI_NO_SIGNAL )
  {
    /* out of service */
    antenna_lvl = RSSI_LEVEL_0;
  }
  return antenna_lvl;
} /* vend_convert_rssi_antenna_number */


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCANTE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $QCANTE command.
  The result is in res_buff_ptr, reporting number of antenna bars
  in a scale of 0-4.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :         if it is a success and execution completed.

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcante_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /* process command */
  if ( tok_ptr->op == NA )
  {
    dsat_ante_rssi_level_e_type antenna_level;
    
    antenna_level = vend_convert_rssi_antenna_number();
    if(antenna_level == RSSI_LEVEL_INVALID)
    {
      return DSAT_ERROR;
    }
    /* Generate response */
    res_buff_ptr->used = 
      (word) snprintf ((char*)res_buff_ptr->data_ptr, 
                              res_buff_ptr->size,
                              "%s: %d", 
                              "$QCANTE", 
                              antenna_level );
  }
  /* process the TEST command */
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    /* Generate supported values response */
    res_buff_ptr->used =
      (word)snprintf((char*)res_buff_ptr->data_ptr, 
                           res_buff_ptr->size ,
                           "%s",
                           "$QCANTE: (0-4)" );
  }
  /* other commands are not supported */
  else
  {
    result = DSAT_ERROR;
  }
  
  return result;
} /* dsatvend_exec_qcante_cmd */


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCRPW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $QCRPW command.
  The result is in res_buff_ptr, reporting recieved radio signal 
  power in a scale of 0-75.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :         if it is a success and execution completed.

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcrpw_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
#define DSAT_QCRPW_MAX_SIGNAL 75
#define DSAT_QCRPW_MIN_SIGNAL 0
  dsat_result_enum_type result = DSAT_OK;
  dsatcmif_servs_state_ms_info  *ph_ss_ms_val = NULL;


   ph_ss_ms_val = dsat_get_base_addr_current_subs(DSAT_SS_PH_MS_VALS);

  /* process command */
  if ( tok_ptr->op == NA )
  {
   /* Convert RSSI value to reporting range of 0-75.
    * The RSSI value reported by CM is actually, RSCP value (l1 layer)
    * for WCDMA.
    */
    uint8 rssi_code = dsatetsime_convert_rssi(
                         ph_ss_ms_val->dsatcmif_signal_reporting.rssi,
                         DSAT_QCRPW_MAX_SIGNAL );
    if ( RSSI_TOOHI_CODE == rssi_code )
    {
       /* Capped maximum for QCRPW*/
      rssi_code = DSAT_QCRPW_MAX_SIGNAL;
    }
    else if ( RSSI_UNKNOWN_CODE == rssi_code )
    {
      rssi_code = DSAT_QCRPW_MIN_SIGNAL;
    }

    /* Generate response */
    res_buff_ptr->used = 
      (word) snprintf ((char*)res_buff_ptr->data_ptr, 
                              res_buff_ptr->size,
                              "%s: %d", 
                              "$QCRPW", 
                              rssi_code);
  }
  /* process the TEST command */
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    /* Generate supported values response */
    res_buff_ptr->used =
      (word)snprintf((char*)res_buff_ptr->data_ptr, 
                           res_buff_ptr->size,
                           "%s",
                           "$QCRPW: (0-75)" );
  }
  /* other commands are not supported */
  else
  {
    result = DSAT_ERROR;
  }
  
  return result;
} /* dsatvend_exec_qcrpw_cmd */


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCSQ_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcsq command.
  This function returns RSCP, ECIO,SIR,PATHLOSS and RSSI if present
  in the following format $QCSQ: <rscp>,<ecio>,<sir>,<pathloss>,<rssi>.

DEPENDENCIES
  None
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success and execution completed.
  
SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcsq_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsatcmif_servs_state_ms_info  *ph_ss_ms_val = NULL;
  
  /* Read Command */
  if ( tok_ptr->op == (NA) )
  {
    uint16 rssi=0;
    
    /* RSSI reported by CM is RSCP in actual in case of WCDMA.
     * The values reported by CM are absolute values in case of 
     * RSCP and ECIO.
     * In case of GSM , first value in $QCSQ denotes RSSI.
     * rssi(dbm)= rscp(dbm) - ecio(dbm).
     * QCSQ: <rscp>,<ecio>,<sir>,<pathloss>,<rssi>.
     */
     ph_ss_ms_val = dsat_get_base_addr_current_subs(DSAT_SS_PH_MS_VALS);

    DS_AT_MSG3_HIGH("Reported Values of RSSI %d, RSCP %d and RSSI2 %d",
              ph_ss_ms_val->dsatcmif_signal_reporting.rssi,
              ph_ss_ms_val->dsatcmif_signal_reporting.rscp,
              ph_ss_ms_val->dsatcmif_signal_reporting.rssi2);

    if( RSSI_NO_SIGNAL <= ph_ss_ms_val->dsatcmif_signal_reporting.rssi)
    {
      res_buff_ptr->used = (word)snprintf(
                               (char*)res_buff_ptr->data_ptr,
                                res_buff_ptr->size,
                                "$QCSQ :");
      return DSAT_OK;
    }
    #ifdef FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
    #error code not present
#else
    res_buff_ptr->used = (word)snprintf(
                               (char*)res_buff_ptr->data_ptr,
                                res_buff_ptr->size,
                                "$QCSQ :%d,",
                                (-1)*(ph_ss_ms_val->dsatcmif_signal_reporting.rssi));
    #endif /* FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY */
    /* ECIO value will be reported by higher layer only 
     * when FEATURE_UMTS_REPORT_ECIO is enabled. ECIO is not valid
     * for GSM and CM will report a MAX value equivalent to 0xFF.
     */
    if( ph_ss_ms_val->dsatcmif_signal_reporting.ecio < DSAT_ECIO_NO_SIGNAL )
    {
      /* The ecio recieved from CM is 2*<actual_ecio> */
      res_buff_ptr->used = (word)snprintf(
                               (char*)res_buff_ptr->data_ptr,
                                res_buff_ptr->size,
                                "%s%d,",
                                res_buff_ptr->data_ptr,
                                (-1)*(ph_ss_ms_val->dsatcmif_signal_reporting.ecio/2));

      rssi = ph_ss_ms_val->dsatcmif_signal_reporting.rssi + 
             (uint16)(ph_ss_ms_val->dsatcmif_signal_reporting.ecio/2);
    }
    else
    {
      res_buff_ptr->used = (word)snprintf(
                              (char*)res_buff_ptr->data_ptr,
                                res_buff_ptr->size,
                                "%s,",
                                res_buff_ptr->data_ptr);
    }
    /* SIR and PATHLOSS values will be reported by higher layer only 
     * when FEATURE_UMTS_SIR_PATHLOSS is enabled. Both SIR and PATHLOSS 
     * are valid for WCDMA when in connected mode. ie, when a data/voice
     * call is present. These values are not valid in GSM mode.
     */
    if( ph_ss_ms_val->dsatcmif_signal_reporting.sir < DSAT_SIR_NO_SIGNAL )
    {
      res_buff_ptr->used = (word)snprintf(
                               (char*)res_buff_ptr->data_ptr,
                                res_buff_ptr->size,
                                "%s%d,",
                                res_buff_ptr->data_ptr,
                                ph_ss_ms_val->dsatcmif_signal_reporting.sir);
    }
    else
    {
      res_buff_ptr->used = (word)snprintf(
                               (char*)res_buff_ptr->data_ptr,
                                res_buff_ptr->size,
                                "%s,",
                                res_buff_ptr->data_ptr);
    }
    if( ph_ss_ms_val->dsatcmif_signal_reporting.pathloss < DSAT_PATHLOSS_NO_SIGNAL )
    {
      res_buff_ptr->used = (word)snprintf(
                               (char*)res_buff_ptr->data_ptr,
                                res_buff_ptr->size,
                                "%s%d,",
                                res_buff_ptr->data_ptr,
                                ph_ss_ms_val->dsatcmif_signal_reporting.pathloss);
    }
    else
    {
      res_buff_ptr->used = (word)snprintf(
                               (char*)res_buff_ptr->data_ptr,
                                res_buff_ptr->size,
                                "%s,",
                                res_buff_ptr->data_ptr);
    }
    /* RSSI value will be reported for WCDMA only 
     * when FEATURE_UMTS_REPORT_ECIO is enabled.
     */
    #ifdef FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY
      #error code not present
#else
    if( 0 != rssi)
    {
      res_buff_ptr->used = (word)snprintf(
                               (char*)res_buff_ptr->data_ptr,
                                res_buff_ptr->size,
                                "%s%d",
                                res_buff_ptr->data_ptr,
                                (-1)*rssi);
    }
    #endif /* FEATURE_WCDMA_DATA_FOR_UI_BAR_DISPLAY */
  }
  /* Test Command */
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    res_buff_ptr->used = 
      (word) snprintf ((char*)res_buff_ptr->data_ptr, 
                              res_buff_ptr->size,
                              "$QCSQ: (-121 - -25),"
                              "(-31 - 0),(-10 - 20),"
                              "(46 - 148),(-121 - -25)" );
  }
  else
  {
    /* other commands are not supported */
    return DSAT_ERROR;
  }
  return DSAT_OK;
} /* dsatvend_exec_qcsq_cmd */


#ifdef FEATURE_DATA_PS_SLIP
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCSLIP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCSLIP command.
  AT$QCSLIP command is used to switch to SLIP mode.
  It brings up the SLIP iface and applications can use the SLIP iface for
  data transfer.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR   : if there was any problem in executing the command
    DSAT_CONNECT : if SLIP is ready

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcslip_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  ds3g_siolib_port_alloc_info_s_type  alloc_info;

  /* Allocate the command buffer and queue the command to DCC/DS Task */
  /*-------------------------------------------------------------------------
    DCC task is available, post the command to DCC.
  -------------------------------------------------------------------------*/
  slip_iface_post_cmd(SLIP_IFACE_CALLP_UP_CMD);
  /*-------------------------------------------------------------------------
  Try to allocate SIO port for external data call.  Without successfully
  downing the semaphore, the call cannot proceed.
  -------------------------------------------------------------------------*/
  memset((void*)&alloc_info, 0x0, sizeof(ds3g_siolib_port_alloc_info_s_type));
  alloc_info.mode = DS3G_SIOLIB_PORT_ALLOC_MODE_AUTOMATIC;
  alloc_info.call_dir = DS3G_SIOLIB_PORT_CALL_DIR_ORIGINATED;

  slip_ser_port = ds3g_siolib_allocate_port(&alloc_info);
  if (slip_ser_port == DS3G_SIOLIB_PORTID_NONE) 
  {
    DS_AT_MSG0_ERROR("Cannot allocate serial port for SLIP connection");
    return DSAT_ERROR;
  }

  return DSAT_CONNECT;
}/* dsatvend_exec_qcslip_cmd () */
#endif /* FEATURE_DATA_PS_SLIP */

#ifdef FEATURE_DSAT_DEV_CMDS
#ifdef FEATURE_DSAT_ETSI_MODE
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCCNTI_CMD

DESCRIPTION
  This function takes the result from the command line parser and
  executes it. It executes *CNTI command, which deals as follows:
  *CNTI=<n>
  *CNTI: <n>,<tech>[,<tech>[...]]
  <n>: parameter controls the results displayed by the response
  
   0- Technology currently in use to access the network  
   1- The available technologies on the current network  
   2- All technologies supported by the device
  
  <tech>: alphanumeric string used to identify technology
   GSM
   GPRS
   EDGE
   UMTS
   HSDPA
   HSUPA

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qccnti_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
#define DSAT_CNTI_TECH_CURRENT 0
#define DSAT_CNTI_TECH_AVAILABLE 1
#define DSAT_CNTI_TECH_SUPPORTED 2
#define DSAT_CNTI_BUFFER_LEN    60
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type temp1;
  char qccnti_tech[8][12]={"NONE","GSM","GPRS","EDGE","UMTS","HSDPA","HSUPA",
                          "HSDPA+HSUPA"};
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  if ( (tok_ptr->op == (NA|EQ|AR) )|| (tok_ptr->op == (NA|EQ|QU)) )
  {
    result = dsatparm_exec_param_cmd( mode,
                                      parse_table,
                                      tok_ptr,
                                      res_buff_ptr );
  }
  else if ( (tok_ptr->op != (NA|QU) ) )
  {
    result = dsat_send_cme_error( DSAT_CME_OP_NOT_SUPPORTED );
  }

  if ( result == DSAT_OK )
  {
    if ( ( tok_ptr->op == (NA|EQ|AR))||
         ( tok_ptr->op == (NA|QU)))
    {
      temp1 = (dsat_num_item_type)dsatutil_get_val(
                    DSAT_VENDOR_CNTI_IDX,subs_id,0,NUM_TYPE);
      res_buff_ptr->used = (word)snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                 res_buff_ptr->size,
                                                 "*CNTI: %d",
                                                 temp1);
      temp1  = (dsat_num_item_type)dsatutil_get_val(
                    DSAT_VENDOR_CNTI_IDX,subs_id,0,NUM_TYPE);
      switch ( temp1 )
      {
      case DSAT_CNTI_TECH_CURRENT:
        res_buff_ptr->used += (word)snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                     (res_buff_ptr->size - res_buff_ptr->used),
                                                     ",%s",
                                                     qccnti_tech[dsat_qccnti_tech]);
        break;
      case DSAT_CNTI_TECH_AVAILABLE:
        /* List out all technologies starting with lowest till higher 
           Display NONE when No technology is selected */
        if( (dsat_qccnti_tech == DSAT_CNTI_GSM) || 
            (dsat_qccnti_tech == DSAT_CNTI_UMTS) ||
            (dsat_qccnti_tech == DSAT_CNTI_NONE) )
        {
          res_buff_ptr->used += (word)snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                     (res_buff_ptr->size - res_buff_ptr->used),
                                                     ",%s",
                                                     qccnti_tech[dsat_qccnti_tech]);
        }
        else if( (dsat_qccnti_tech == DSAT_CNTI_GPRS) || 
                 (dsat_qccnti_tech == DSAT_CNTI_EGPRS) )
        {
          res_buff_ptr->used += (word)snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                      (res_buff_ptr->size - res_buff_ptr->used),
                                                       ",%s,%s",qccnti_tech[DSAT_CNTI_GSM],
                                                       qccnti_tech[dsat_qccnti_tech]);
        }
        else if ( dsat_qccnti_tech == DSAT_CNTI_HSDPA_HSUPA )
        {
          res_buff_ptr->used += (word)snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                      (res_buff_ptr->size - res_buff_ptr->used),
                                                      ",%s,%s,%s",qccnti_tech[DSAT_CNTI_UMTS],
                                                      qccnti_tech[DSAT_CNTI_HSDPA],
                                                      qccnti_tech[DSAT_CNTI_HSUPA]);
        }
        else
        {
          res_buff_ptr->used += (word)snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                     (res_buff_ptr->size - res_buff_ptr->used),
                                                     ",%s,%s",qccnti_tech[DSAT_CNTI_UMTS],
                                                     qccnti_tech[dsat_qccnti_tech]);
        }
        break;
      case DSAT_CNTI_TECH_SUPPORTED:
      /* List out supported technologies based on code feauturization*/
      {
        uint16 len = 0; /* Technology NONE */
        char fmt_string[DSAT_CNTI_BUFFER_LEN];
        
        memset(fmt_string,0x0,DSAT_CNTI_BUFFER_LEN);
#ifdef FEATURE_GSM
         /* Technology GSM */
         len = (word)snprintf(fmt_string,sizeof(fmt_string),", %s",qccnti_tech[1]);
#endif /* FEATURE_GSM */
#ifdef FEATURE_GSM_GPRS
        /* Technology GPRS */
          if( len < DSAT_CNTI_BUFFER_LEN )
          {
            len += (word)snprintf(fmt_string+len,sizeof(fmt_string) - len,", %s",qccnti_tech[2]);
          }
#endif /* FEATURE_GSM_GPRS */
#ifdef FEATURE_GSM_EGPRS
        /* Technology EGPRS */
          if( len < DSAT_CNTI_BUFFER_LEN )
          {
            len += (word)snprintf(fmt_string+len,sizeof(fmt_string) - len,", %s",qccnti_tech[3]);
          }
#endif /* FEATURE_GSM_EGPRS */
#ifdef FEATURE_WCDMA
        /* Technology UMTS */
          if( len < DSAT_CNTI_BUFFER_LEN )
          {
            len += (word)snprintf(fmt_string+len,sizeof(fmt_string) - len,", %s",qccnti_tech[4]);
          }
#endif /* FEATURE_WCDMA */
#ifdef FEATURE_HSDPA
        /* Technology HSDPA */
          if( len < DSAT_CNTI_BUFFER_LEN )
          {
            len += (word)snprintf(fmt_string+len,sizeof(fmt_string) - len,", %s",qccnti_tech[5]);
          }
#endif /* FEATURE_HSDPA */
#ifdef FEATURE_WCDMA_HSUPA
        /* Technology HSUPA */
          if( len < DSAT_CNTI_BUFFER_LEN )
          {
            len += (word)snprintf(fmt_string+len,sizeof(fmt_string) - len,", %s",qccnti_tech[6]);
          }
#endif /* FEATURE_WCDMA_HSUPA */
          res_buff_ptr->used += (word)snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                      (res_buff_ptr->size - res_buff_ptr->used),
                                                      "%s", fmt_string);
      }
        break;
      default:
        result = DSAT_ERROR;
        break;
      }
    }
  }
  if(result == DSAT_ERROR)
  {
    result = dsat_send_cme_error( DSAT_CME_OP_NOT_ALLOWED );
  }
  return result;
} /* dsatvend_exec_qccnti_cmd */
#endif /* FEATURE_DSAT_ETSI_MODE */
#endif /* FEATURE_DSAT_DEV_CMDS */
/*===========================================================================

FUNCTION VEND_PROCESS_INBAND_VALUES

DESCRIPTION
  This function process the inband values and returns the values in CM
  data type. This function is used for ^BANDPREF write command to convert
  the receievd input value to CM band type. 
 
DEPENDENCIES
  None

RETURN VALUE

  CM_BAND_PREF_E_TYPE value. 

SIDE EFFECTS
  None

===========================================================================*/
cm_band_pref_e_type vend_process_inband_values
(
  dsat_bandpref_e_type in_band
)
{
  cm_band_pref_e_type out_band;
  switch( in_band )
  {
    case DSAT_BAND_PREF_BC0_A :
      out_band = CM_BAND_PREF_BC0_A;
      break;
    case DSAT_BAND_PREF_BC0_B :
      out_band = CM_BAND_PREF_BC0_B;
      break;
    case DSAT_BAND_PREF_BC0 :
      out_band = CM_BAND_PREF_BC0;
      break;
    case DSAT_BAND_PREF_BC1 :
      out_band = CM_BAND_PREF_BC1;
      break;
    case DSAT_BAND_PREF_BC3 :
      out_band = CM_BAND_PREF_BC3;
      break;
    case DSAT_BAND_PREF_BC4 :
      out_band = CM_BAND_PREF_BC4;
      break;
    case DSAT_BAND_PREF_BC5 :
      out_band = CM_BAND_PREF_BC5;
      break;
    case DSAT_BAND_PREF_GSM_DCS_1800 :
      out_band = CM_BAND_PREF_GSM_DCS_1800;
      break;
    case DSAT_BAND_PREF_GSM_EGSM_900 :
      out_band = CM_BAND_PREF_GSM_EGSM_900;
      break;
    case DSAT_BAND_PREF_GSM_PGSM_900 :
      out_band = CM_BAND_PREF_GSM_PGSM_900;
      break;
    case DSAT_BAND_PREF_BC6 :
      out_band = CM_BAND_PREF_BC6;
      break;
    case DSAT_BAND_PREF_BC7 :
      out_band = CM_BAND_PREF_BC7;
      break;
    case DSAT_BAND_PREF_BC8 :
      out_band =  CM_BAND_PREF_BC8;
      break;
    case DSAT_BAND_PREF_BC9 :
      out_band = CM_BAND_PREF_BC9;
      break;
    case DSAT_BAND_PREF_BC10 :
      out_band = CM_BAND_PREF_BC10;
      break;
    case DSAT_BAND_PREF_BC11 :
      out_band = CM_BAND_PREF_BC11;
      break;
    case DSAT_BAND_PREF_BC12 :
      out_band = CM_BAND_PREF_BC12;
      break;
    case DSAT_BAND_PREF_BC14 :
      out_band = CM_BAND_PREF_BC14;
      break;
    case DSAT_BAND_PREF_BC15 :
      out_band = CM_BAND_PREF_BC15;
      break;
    case DSAT_BAND_PREF_BC16 :
      out_band = CM_BAND_PREF_BC16;
      break;
    case DSAT_BAND_PREF_GSM_450 :
      out_band = CM_BAND_PREF_GSM_450;
      break;
    case DSAT_BAND_PREF_GSM_480 :
      out_band = CM_BAND_PREF_GSM_480;
      break;
    case DSAT_BAND_PREF_GSM_750 :
      out_band = CM_BAND_PREF_GSM_750;
      break;
    case DSAT_BAND_PREF_GSM_850 :
      out_band = CM_BAND_PREF_GSM_850;
      break;
    case DSAT_BAND_PREF_GSM_RGSM_900 :
      out_band = CM_BAND_PREF_GSM_RGSM_900;
      break;
    case DSAT_BAND_PREF_GSM_PCS_1900 :
      out_band = CM_BAND_PREF_GSM_PCS_1900;
      break;
    case DSAT_BAND_PREF_WCDMA_I_IMT_2000 :
      out_band = CM_BAND_PREF_WCDMA_I_IMT_2000;
      break;
    case DSAT_BAND_PREF_WCDMA_II_PCS_1900 :
      out_band = CM_BAND_PREF_WCDMA_II_PCS_1900;
      break;
    case DSAT_BAND_PREF_WCDMA_III_1700 :
      out_band = CM_BAND_PREF_WCDMA_III_1700;
      break;
    case DSAT_BAND_PREF_WCDMA_IV_1700 :
      out_band = CM_BAND_PREF_WCDMA_IV_1700;
      break;
    case DSAT_BAND_PREF_WCDMA_V_850 :
      out_band = CM_BAND_PREF_WCDMA_V_850;
      break;
    case DSAT_BAND_PREF_WCDMA_VI_800 :
      out_band = CM_BAND_PREF_WCDMA_VI_800;
      break;
    case DSAT_BAND_PREF_WCDMA_VII_2600 :
      out_band = CM_BAND_PREF_WCDMA_VII_2600;
      break;
    case DSAT_BAND_PREF_WCDMA_VIII_900 :
      out_band = CM_BAND_PREF_WCDMA_VIII_900;
      break;
    case DSAT_BAND_PREF_WCDMA_IX_1700 :
      out_band = CM_BAND_PREF_WCDMA_IX_1700;
      break;
    case DSAT_BAND_PREF_WLAN_2400_US :
      out_band = CM_BAND_PREF_WLAN_2400_US;
      break;
    case DSAT_BAND_PREF_WLAN_2400_JP :
      out_band = CM_BAND_PREF_WLAN_2400_JP;
      break;
    case DSAT_BAND_PREF_WLAN_2400_ETSI :
      out_band = CM_BAND_PREF_WLAN_2400_ETSI;
      break;
    case DSAT_BAND_PREF_WLAN_2400_SP :
      out_band = CM_BAND_PREF_WLAN_2400_SP;
      break;
    case DSAT_BAND_PREF_WLAN_2400_FR :
      out_band = CM_BAND_PREF_WLAN_2400_FR;
      break;
    case DSAT_BAND_PREF_WLAN_5000_US :
      out_band = CM_BAND_PREF_WLAN_5000_US;
      break;
    case DSAT_BAND_PREF_WLAN_5000_JP :
      out_band = CM_BAND_PREF_WLAN_5000_JP;
      break;
    case DSAT_BAND_PREF_WLAN_5000_ETSI :
      out_band = CM_BAND_PREF_WLAN_5000_ETSI;
      break;
    case DSAT_BAND_PREF_WLAN_5000_SP :
      out_band = CM_BAND_PREF_WLAN_5000_SP;
      break;
    case DSAT_BAND_PREF_WLAN_5000_FR :
      out_band = CM_BAND_PREF_WLAN_5000_FR;
      break;
    case DSAT_BAND_PREF_ANY :
      out_band = CM_BAND_PREF_ANY;
      break;
    default:
      out_band = CM_BAND_PREF_NONE;
  }

  return out_band;
}
/*===========================================================================

FUNCTION VEND_PROCESS_OUTBAND_VALUES

DESCRIPTION
  This function process is used for ^BANDPREF read command to convert
  the CM band pref type to DSAT band pref type and prepares a list to 
  display the data in strings. 
 
DEPENDENCIES
  None

RETURN VALUE
  TRUE: Success in conversion.
  FAIL: Failed in conversion. 

SIDE EFFECTS
  None

===========================================================================*/
boolean vend_process_outband_values
(
  cm_band_pref_e_type  out_band,
  dsat_bandpref_e_type result_list[],
  uint32               result_list_size
)
{
  uint32 out_index = 0;
  dsat_bandpref_e_type local_dsat_band = DSAT_BAND_PREF_NONE;
  cm_band_pref_e_type  local_cm_band   = CM_BAND_PREF_NONE;

  /* Reset the values to 0 so that the last data after valid band would be 
     DSAT_BAND_PREF_NONE */
  memset((void *)result_list, 0X00, result_list_size );
  while ( DSAT_BAND_PREF_MAX > local_dsat_band )
  {
    if ( result_list_size < out_index )
    {
      DS_AT_MSG2_ERROR("Insufficient space for BAND PREF list size %d, out_index %d",
                                                        result_list_size,out_index);
      return FALSE;
    }
    /* Point to the next DSAT_BAND_PREF */
    ++local_dsat_band;
    /* get the corresponding CM_BAND_PREF */
    local_cm_band = vend_process_inband_values ( local_dsat_band );
    /* Verify if this is set in CM */
    if ( local_cm_band == (out_band & local_cm_band ) )
    {
      result_list[out_index++] = local_dsat_band;
    }
  }
  DS_AT_MSG0_MED("Processing of out bands done; List is updated");
  return TRUE;
}/* vend_process_outband_values  */
/*===========================================================================
FUNCTION PROCESS_QCBANDPREF_PARAMS

DESCRIPTION
  This function process the input band values received in a byte array
  and updates the global variable dsat_bandpref_val list after parsing the data.
 
DEPENDENCIES
  None

RETURN VALUE
  returns a boolean that describes the result of parsing of data. 
  possible values:
  FALSE :    if there was any problem in parsing the data
  TRUE :     if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
boolean process_qcbandpref_params
(
  byte *band_pref  
)
{
  #define MAX_DIG_NUM 6
  
  byte noquotes_band[DSAT_MAX_BANDPREF_STRING_LENGTH] = {0};
  uint32 index = 0;
  uint32 loop = 0;
  byte temp[MAX_DIG_NUM]={0};  /* Maxnumber of digits of a max integer */
  dsat_num_item_type value = 0;
  dsat_bandpref_e_type index_band = DSAT_BAND_PREF_NONE;

  /* Extract data without quoutes */
  if ( FALSE == dsatutil_strip_quotes_out( band_pref, 
                                           noquotes_band, 
                                           DSAT_MAX_BANDPREF_STRING_LENGTH ) )
  {
    DS_AT_MSG0_ERROR("Error in stripping quotes");
    return FALSE;
  }

  /* loop through the data till null */
  while( loop < DSAT_MAX_BANDPREF_STRING_LENGTH && noquotes_band[loop] != '\0')
  {
    if ( index_band >= DSAT_MAX_BANDPREF )
    {
      DS_AT_MSG1_ERROR(" invalid band received %d",index_band);
      return FALSE;
    }
    index = 0;
    /* parse the byte data till the delimiter of , or end of string */
    while (loop < DSAT_MAX_BANDPREF_STRING_LENGTH && noquotes_band[loop] !=',' && noquotes_band [loop]!= '\0'   )
    {
      /* ERROR if more number of digits than 6 */
      if ( index >= (MAX_DIG_NUM -1))
      {
        return FALSE;
      }
      temp[index++] = noquotes_band[loop++];
    }
    temp[index] = '\0';

    /* convert the parsed ascii format of bytes of data into integer number */
    if ( ATOI_OK != dsatutil_atoi( &value, temp, 10 ) )
    {
      return FALSE;
    }

    dsat_bandpref_val[index_band++]=(dsat_bandpref_e_type)value;
    loop++;
  }
  dsat_bandpref_val[index_band] = DSAT_BAND_PREF_NONE;

  return TRUE;

}/* process_qcbandpref_params */

/*===========================================================================
FUNCTION DSATVEND_EXEC_QCBANDPREF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcbandpref command.
  This command provides the ability to set/get the band preferences

  
<band_pref>: Is a string containing indexes separated by comma. 
for eg: "1,2,3,4,5,6" which means set all the bandprefs which has indexes from
1,2,3,4,5,6 in the test command.

<persitence>:0/1

currently, the band preference change is not persistent. 

DEPENDENCIES
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_ASYNC_CMD : if success.
SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcbandpref_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

#ifdef FEATURE_DUAL_SIM  
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
#endif /* FEATURE_DUAL_SIM */
  
  dsat_error_info_s_type  err_info;
  dsat_num_item_type      size = 0;
  dsat_string_item_type   *buffer = NULL;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if ( (NA|EQ|AR) == tok_ptr->op )
  {
    if ( ( tok_ptr->args_found > 2 ) || ( !VALID_TOKEN(1) ))
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }

  /* Prepare a list of band pref values out of the given string and 
       update it in dsat_bandpref_val*/
    if ( FALSE == process_qcbandpref_params(tok_ptr->arg[1]))
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 1;
      goto send_error;
    }

    /* Extract persistence details; default is NOT PERSISTANT  */
    err_info.arg_num = 0;
    if ( VALID_TOKEN(0) )
    {
       if ( ( ATOI_OK != dsatutil_atoi( &dsat_bandpref_pers_val,
                                     tok_ptr->arg[0], 
                                     10 ) ) )
       {
         err_info.errval =  DSAT_ERR_INVALID_ARGUMENT;
         goto send_error;
       }
       if ( ( dsat_bandpref_pers_val != DSAT_BANDPREF_NON_PERS ) &&
           ( dsat_bandpref_pers_val != DSAT_BANDPREF_PERS ) )
       {
         err_info.errval =  DSAT_ERR_PARAMETER_OUT_OF_RANGE;
         goto send_error;
       }
    }
    else
    {
       /* Set default persistance value to 0 when second parameter is not given */
       dsat_bandpref_pers_val = DSAT_BANDPREF_NON_PERS;
    }
    /* Setting bandpref would happen in two stages. 
       1. Get the mode pref to validate the band pref values 
       2. Set the band pref */
    SET_PENDING(DSAT_VENDOR_QCBANDPREF_IDX ,0, DSAT_PENDING_PREFMODE_BANDPREF_VERIFY)
    /* The return value is ignored as both success and failure cases
      * are handled by the call back function */
#ifdef FEATURE_DUAL_SIM
     cm_ph_cmd_get_ph_info_per_subs(dsatcmif_ph_cmd_cb_func,
                                           NULL,
                                           dsatcm_client_id,
                                           subs_id
                                           );
#else
    (void)cm_ph_cmd_get_ph_info (dsatcmif_ph_cmd_cb_func,
                                 NULL,
                                 dsatcm_client_id
                                 );
#endif /* FEATURE_DUAL_SIM */
    result = DSAT_ASYNC_CMD;
  }
  if ( (NA|QU) == tok_ptr->op )
  {
    SET_PENDING(DSAT_VENDOR_QCBANDPREF_IDX ,0, DSAT_PENDING_PREFMODE_BANDPREF_READ)
    /* The return value is ignored as both success and failure cases
     * are handled by the call back function */
#ifdef FEATURE_DUAL_SIM
    cm_ph_cmd_get_ph_info_per_subs(dsatcmif_ph_cmd_cb_func,
                                          NULL,
                                          dsatcm_client_id,
                                          subs_id
                                          );
#else
    (void)cm_ph_cmd_get_ph_info (dsatcmif_ph_cmd_cb_func,
                                 NULL,
                                 dsatcm_client_id
                                 );
#endif /* FEATURE_DUAL_SIM */

    result = DSAT_ASYNC_CMD;

  }
  if ( (NA|EQ|QU) == tok_ptr->op )
  {

    res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                 res_buff_ptr->size,
                                 "$QCBANDPREF: (0-1),\n");

    if((size = dsat_read_efs_data("bandpref", DSAT_FILE_STR_TYPE, (void **)&buffer)))
    {
      result = DSAT_OK;
      if(!dsat_process_efs_data(buffer,size,res_buff_ptr,DSAT_EFS_PREFMODE_TEST,NULL))
      {
        result = DSAT_ERROR;
      }
    }
    else
    {
      result = DSAT_ERROR;
    }
  }

  /*free the buffer here*/
  dsatutil_free_memory_ext((void **) &buffer);
  return result;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_NUM_OF_ARGS )
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;
}/* dsatvend_exec_qcbandpref_cmd */
/*===========================================================================
FUNCTION DSATVEND_EXEC_PREFMODE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at^prefmode command.
  This command provides the ability to set/get the network mode preferences. 

<pref_mode> network mode, values as follows:
0 Automatic
2 CDMA mode
4 HDR mode
8 CDMA/HDR HYBRID mode

 The following note is ignored:
For the data card which only support CDMA 1X, the command isn't realized.

DEPENDENCIES
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK/DSAT_ASYNC_CMD : if success.
    
SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_prefmode_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  cm_mode_pref_e_type  pref_mode;
  cm_hybr_pref_e_type  hybr_pref = CM_HYBR_PREF_NO_CHANGE;
  dsat_num_item_type in_pref_val;
  dsat_error_info_s_type  err_info;
  
  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if ( !IS_CDMA_MODE(dsatcmdp_get_current_mode() ))
  {
    DS_AT_MSG0_ERROR("Not applicable in UMTS mode ");
    return DSAT_ERROR;
  }
  if ( (NA|EQ|AR) == tok_ptr->op )
  {
    /* Verify the input parameter range  */

    if ( ( tok_ptr->args_found > 1 ) || ( !VALID_TOKEN(0) ) )
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }
    if ( dsatutil_atoi( &in_pref_val, 
                       tok_ptr->arg[0], 10) != ATOI_OK )
    {
      err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
      err_info.errval = 0;
      goto send_error;
    }

    switch( in_pref_val )
    {
      
#ifndef FEATURE_DSAT_EXTENDED_CMD
        case DSAT_PREFMODE_SYSMODE_AUTOMATIC:
        pref_mode = CM_MODE_PREF_AUTOMATIC;
        break;
#endif /* FEATURE_DSAT_EXTENDED_CMD */
      case DSAT_PREFMODE_SYSMODE_CDMA:
        pref_mode = CM_MODE_PREF_CDMA_ONLY;
        break;
      case DSAT_PREFMODE_SYSMODE_HDR:
        pref_mode = CM_MODE_PREF_HDR_ONLY;
        break;
      case DSAT_PREFMODE_SYSMODE_HYBRID_CDMA_HDR:
        pref_mode = CM_MODE_PREF_CDMA_HDR_ONLY;
        /* 8960 onwards HYBD_PREF can be set only for HDR_LTE_CDMA  */
        hybr_pref = CM_HYBR_PREF_NO_CHANGE;
        break;
      default:
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        err_info.errval = 0;
        goto send_error;
    }

    /* Verify if the pref values are correct */
    if ( FALSE == cm_is_valid_mode_pref(pref_mode))
    {
      DS_AT_MSG0_ERROR("Invalid CM MODE PREF setting");
      (void)dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
      return DSAT_CMD_ERR_RSP;
    }
    SET_PENDING(DSAT_VENDOR_PREFMODE_IDX ,0, DSAT_PENDING_PREFMODE_SYSMODE_WRITE)
    (void) cm_ph_cmd_sys_sel_pref_new ( dsatcmif_ph_cmd_cb_func,
                                    NULL,
                                    dsatcm_client_id,
                                    pref_mode,
                                    CM_PREF_TERM_PERMANENT,
                                    0,
                                    CM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                    CM_BAND_PREF_NO_CHANGE,
                                    CM_PRL_PREF_NO_CHANGE,
                                    CM_ROAM_PREF_NO_CHANGE,
                                    hybr_pref,
                                    CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                    CM_NETWORK_SEL_MODE_PREF_AUTOMATIC,
                                    NULL 
                                  ); 
    result = DSAT_ASYNC_CMD;
  }
  else if ( (NA|EQ|QU) == tok_ptr->op )
  {
    res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                        res_buff_ptr->size,
    #ifdef FEATURE_DSAT_EXTENDED_CMD
                                              "^PREFMODE:(%d,%d,%d)",
#else 
                                              "^PREFMODE:(%d,%d,%d,%d)",
                                        DSAT_PREFMODE_SYSMODE_AUTOMATIC,
#endif /* FEATURE_DSAT_EXTENDED_CMD */
                                        DSAT_PREFMODE_SYSMODE_CDMA,
                                        DSAT_PREFMODE_SYSMODE_HDR,
                                        DSAT_PREFMODE_SYSMODE_HYBRID_CDMA_HDR);
    result = DSAT_OK;
  }
  else if ( (NA|QU) == tok_ptr->op )
  {
    SET_PENDING(DSAT_VENDOR_PREFMODE_IDX ,0, DSAT_PENDING_PREFMODE_SYSMODE_READ)
    /* The return value is ignored as both success and failure cases
     * are handled by the call back function */
    (void)cm_ph_cmd_get_ph_info (dsatcmif_ph_cmd_cb_func,
                                 NULL,
                                 dsatcm_client_id
                                 );
    
    result = DSAT_ASYNC_CMD;

  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_NUM_OF_ARGS )
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;

}/*  dsatvend_exec_prefmode_cmd */
/*===========================================================================

FUNCTION DSAVEND_PROCESS_PREFMODE_CMD

DESCRIPTION
  This function process the response for the ^PREFMODE command on return from
  asynchronous processing. 
 
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_CMD_ERR_RESP:  If mode value is not valid. 
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/

dsat_result_enum_type dsatvend_process_prefmode_cmd 
(
  dsat_cmd_pending_enum_type cmd_type,
  ds_at_cm_ph_pref_type  *pref_mode    /* Network preference mode */
)
{
  dsat_result_enum_type result = DSAT_ERROR;

#ifdef FEATURE_DUAL_SIM
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
#endif /* FEATURE_DUAL_SIM */

  ASSERT( pref_mode != NULL );
  DS_AT_MSG2_HIGH("Processing PREFMODE cmd %d, mode pref %d", cmd_type,
                                           pref_mode->network_rat_mode_pref);
  switch ( cmd_type )
  {
    case DSAT_PENDING_PREFMODE_SYSMODE_READ:
    {
      dsm_item_type *res_buff_ptr = NULL;
      switch ( pref_mode->network_rat_mode_pref )
      {
        case CM_MODE_PREF_CDMA_ONLY:
          DSATUTIL_SET_VAL(DSAT_VENDOR_PREFMODE_IDX,0,0,0,DSAT_PREFMODE_SYSMODE_CDMA,NUM_TYPE)
          break;
        case CM_MODE_PREF_HDR_ONLY:
          DSATUTIL_SET_VAL(DSAT_VENDOR_PREFMODE_IDX,0,0,0,DSAT_PREFMODE_SYSMODE_HDR,NUM_TYPE)
          break;
        case CM_MODE_PREF_CDMA_HDR_ONLY:
          DSATUTIL_SET_VAL(DSAT_VENDOR_PREFMODE_IDX,0,0,0,DSAT_PREFMODE_SYSMODE_HYBRID_CDMA_HDR,NUM_TYPE)
          break;
        case CM_MODE_PREF_NO_CHANGE:
          /* Nothing changed report the old prefmode value */
          break;
        case CM_MODE_PREF_AUTOMATIC:
        default:
          DSATUTIL_SET_VAL(DSAT_VENDOR_PREFMODE_IDX,0,0,0,DSAT_PREFMODE_SYSMODE_AUTOMATIC,NUM_TYPE)
          DS_AT_MSG1_ERROR("Invalid pref mode %d received, seeting it to Automatic",
                                          pref_mode->network_rat_mode_pref);
      }
      res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
      res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                              res_buff_ptr->size,
                                             "^PREFMODE:%d", 
                                             (dsat_num_item_type)dsatutil_get_val(
                                              DSAT_VENDOR_PREFMODE_IDX,0,0,NUM_TYPE));
      dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
      result = DSAT_OK;
    }
      break;
    case DSAT_PENDING_PREFMODE_SYSMODE_WRITE:
    {
      /* Nothing to do  */
      result = DSAT_OK;
    }
      break;
    case DSAT_PENDING_PREFMODE_BANDPREF_READ:
    {

      dsat_bandpref_e_type  result_list[DSAT_MAX_BANDPREF];
      dsm_item_type *res_buff_ptr = NULL;
      dsat_string_item_type  *buffer = NULL;
      dsat_num_item_type      size   = 0;

      if ( FALSE == vend_process_outband_values( pref_mode->network_band_pref,
                                   &result_list[0],
                                   (uint32)DSAT_MAX_BANDPREF ) )
      {
        DS_AT_MSG0_ERROR("Band preferene query failed");
        result = DSAT_ERROR;
        break;
      }
      res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
      res_buff_ptr->used = (word)snprintf ( (char *)res_buff_ptr->data_ptr,
                                             res_buff_ptr->size,
                                             "$QCBANDPREF: \n");
      if((size = dsat_read_efs_data("bandpref", DSAT_FILE_STR_TYPE, (void **)&buffer)))
      {
        result = DSAT_OK;
        if(!dsat_process_efs_data(buffer,size,res_buff_ptr,DSAT_EFS_PREFMODE_QUERY,&result_list[0]))
        {
          result = DSAT_ERROR;
        }
      }
        /*free the buffer here*/
      dsatutil_free_memory_ext((void **) &buffer);
      dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
    }
      break;
    case DSAT_PENDING_PREFMODE_BANDPREF_VERIFY:
      {
        uint32 index = 0;
        cm_band_pref_e_type temp_band_pref = CM_BAND_PREF_NONE;
        cm_band_pref_e_type req_band_pref  = CM_BAND_PREF_NONE;
        cm_pref_term_e_type pref_term = CM_PREF_TERM_PWR_CYCLE;

        /* Prepare the required CM band pref type from the received input
           which is stored as DSAT band pref type. */
        while ( dsat_bandpref_val[index] != DSAT_BAND_PREF_NONE )
        {
          if ( CM_BAND_PREF_NONE == 
                ( temp_band_pref = vend_process_inband_values(
                                               dsat_bandpref_val[index])))
          {
            DS_AT_MSG1_ERROR("Invalid band preference %d",dsat_bandpref_val[index ]);
            memset((void *)dsat_bandpref_val, 0x0, sizeof(dsat_bandpref_val));
            SET_PENDING(DSAT_VENDOR_QCBANDPREF_IDX ,0, DSAT_PENDING_PREFMODE_NONE)
            return DSAT_ERROR;
          }
          req_band_pref |= temp_band_pref;
          ++index;
        }
        /* Verify if the requested bandpref is valid in current mode */
        DS_AT_MSG2_MED("network_rat_mode_pref = %d, req_band_pref = %d",
                       pref_mode->network_rat_mode_pref, req_band_pref);
        if ( FALSE == cm_is_valid_mode_band_pref( 
                                     pref_mode->network_rat_mode_pref, 
                                     req_band_pref ))
        {
          DS_AT_MSG0_ERROR("Invalid Mode+Band Preference");
          SET_PENDING(DSAT_VENDOR_QCBANDPREF_IDX ,0, DSAT_PENDING_PREFMODE_NONE)
          (void)dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
          return DSAT_CMD_ERR_RSP;
        }

        if ( DSAT_BANDPREF_PERS == dsat_bandpref_pers_val )
        {
          pref_term = CM_PREF_TERM_PERMANENT;
        }
#ifdef FEATURE_DUAL_SIM
        (void) cm_ph_cmd_sys_sel_pref_per_subs( dsatcmif_ph_cmd_cb_func,
                                                NULL,
                                                dsatcm_client_id,
                                                subs_id,
                                                CM_MODE_PREF_NO_CHANGE,
                                                pref_term,
                                                0,
                                                CM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                                req_band_pref,
                                                CM_PRL_PREF_NO_CHANGE,
                                                CM_ROAM_PREF_NO_CHANGE,
                                                CM_HYBR_PREF_NO_CHANGE,
                                                CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                                CM_NETWORK_SEL_MODE_PREF_AUTOMATIC,
                                                NULL
                                               );
#else
        (void) cm_ph_cmd_sys_sel_pref_new ( dsatcmif_ph_cmd_cb_func,
                                      NULL,
                                      dsatcm_client_id,
                                      CM_MODE_PREF_NO_CHANGE,
                                      pref_term,
                                      0,
                                      CM_GW_ACQ_ORDER_PREF_NO_CHANGE,
                                      req_band_pref,
                                      CM_PRL_PREF_NO_CHANGE,
                                      CM_ROAM_PREF_NO_CHANGE,
                                      CM_HYBR_PREF_NO_CHANGE,
                                      CM_SRV_DOMAIN_PREF_NO_CHANGE,
                                      CM_NETWORK_SEL_MODE_PREF_AUTOMATIC,
                                      NULL 
                                            ); 
#endif /* FEATURE_DUAL_SIM */

        DS_AT_MSG0_HIGH(" Band pref phase 1 succesful, posted commadn for phase 2");
        return DSAT_ASYNC_CMD;

      }
    case DSAT_PENDING_PREFMODE_BANDPREF_WRITE:
    {
      /* Setting of band pref is done, Nothing to do */
      return DSAT_OK;
    }
    default:
      DS_AT_MSG1_ERROR("Preference mode mismatch %d",pref_mode->network_rat_mode_pref);
      ASSERT(0);
  }

  return result;
}/* dsatvend_process_prefmode_cmd */
/*===========================================================================
  FUNCTION DSATVEND_EXEC_SYSINFO_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^SYSINFO command.

  The command inquires the current system message. Such as 
  system service status, domain, roam, system mode, UIM card status, etc.

  <srv_status> system service status, values as follows:
  0 no service
  1 limited service
  2 service available
  3 limited area service
  4 power saving and dormancy status.

  <srv_domain> system service, values as follows:
  0 no service
  1 only CS service
  2 only PS service
  3 PS+CS service
  4 CS and PS don't register and are in the status of serching.
  255 CDMA doesn't support.

  <roam_status> roaming status, values as follows:
  0 non-roaming status.
  1 roaming status.

  <sys_mode> system mode, values as follows:
  0 no service
  1 AMPS mode (not use provisionally)
  2 CDMA mode
  3 GSM/GPRS mode
  4 HDR mode
  5 WCDMA mode
  6 GPS mode
  7 GSM/WCDMA
  8 CDMA/HDR HYBRID

  <sim_stat> UIM card status, values as follows:
  1 UIM card status available
  240 ROMSIM version
  255 UIM card doesn't exist

   Currently following are not supported
  <lock_state> CDMA production doesn't use the parameter.
  <sys_submode> CDMA production doesn't use the parameter.


  DEPENDENCIES
  None

  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_ASYNC_CMD : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsatvend_exec_sysinfo_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ASYNC_CMD;

  if (( dsat_sysinfo_cmd.dsat_rtre_config == DSAT_RTRE_CONTROL_NONE ) && 
      (dsatcmdp_get_operating_cmd_mode() == CDMA_CMD_MODE))
  {
    DS_AT_MSG0_ERROR("RTRE not configured yet");
    return DSAT_ERROR;
  }
  /* Only NA is supported */
  if ( tok_ptr->op == NA )
  {
    dsat_sysinfo_cmd.cmd_pending = TRUE;
    if ( DSAT_ERROR == dsatcmif_get_cm_ss_info() )
    {
      result = DSAT_ERROR;
    }
  }
  else
  {
    return DSAT_ERROR;
  }

  return result;
}/* dsatvend_exec_sysinfo_cmd */
/*===========================================================================

FUNCTION  VEND_CONVERT_PH_PREF_TYPE

DESCRIPTION
  This Function converts various SYS parameters to ATCoP values and vice-versa 
DEPENDENCIES
  None

RETURN VALUE
    DSAT_ERROR : if there was any problem in execution.
    DSAT_OK  : if it is a success. 

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type vend_convert_ph_pref_type
(
  boolean to_at_state,
  dsat_sysconfig_type *at_pref_ptr,
  ds_at_cm_ph_pref_type *cm_pref_ptr
)
{

  if( to_at_state == TRUE )
  {
    switch( cm_pref_ptr->network_rat_mode_pref )
    {
      case CM_MODE_PREF_AUTOMATIC:
        at_pref_ptr->mode_val = DSAT_SYSCONFIG_MODE_AUTOMATIC;
        break;
      case CM_MODE_PREF_GSM_ONLY:
        at_pref_ptr->mode_val = DSAT_SYSCONFIG_MODE_GSM ;
        break;
      case CM_MODE_PREF_WCDMA_ONLY:
        at_pref_ptr->mode_val = DSAT_SYSCONFIG_MODE_WCDMA;
        break;
#ifdef FEATURE_TDSCDMA		
      case CM_MODE_PREF_TDS_ONLY:
        at_pref_ptr->mode_val = DSAT_SYSCONFIG_MODE_TDSCDMA;
        break;
#endif /* FEATURE_TDSCDMA */		
      case CM_MODE_PREF_NO_CHANGE:
        at_pref_ptr->mode_val = DSAT_SYSCONFIG_MODE_NO_CHANGE;
        break;
      default:
        DS_AT_MSG1_ERROR("Unsupported pref mode %d ",cm_pref_ptr->network_rat_mode_pref);
        at_pref_ptr->mode_val = DSAT_SYSCONFIG_MODE_UNKNOWN;
    }


    switch( cm_pref_ptr->acq_order_pref )
    {
      case CM_GW_ACQ_ORDER_PREF_AUTOMATIC:
        at_pref_ptr->order_val =DSAT_SYSCONFIG_ORDER_AUTOMATIC ;
        break;
      case CM_GW_ACQ_ORDER_PREF_GSM_WCDMA:
        at_pref_ptr->order_val =DSAT_SYSCONFIG_ORDER_GSM_WCDMA ;
        break;
      case CM_GW_ACQ_ORDER_PREF_WCDMA_GSM:
        at_pref_ptr->order_val =DSAT_SYSCONFIG_ORDER_WCDMA_GSM ;
        break;
      case CM_GW_ACQ_ORDER_PREF_NO_CHANGE:
        at_pref_ptr->order_val =DSAT_SYSCONFIG_ORDER_NO_CHANGE ;
        break;
      default:
        DS_AT_MSG1_ERROR("Invalid acq_order %d ",cm_pref_ptr->acq_order_pref);
        at_pref_ptr->order_val =DSAT_SYSCONFIG_ORDER_UNKNOWN ;

    }

    switch( cm_pref_ptr->network_roam_pref )
    {
      case CM_ROAM_PREF_HOME:
        at_pref_ptr->roam_val =  DSAT_SYSCONFIG_ROAM_NONE;
        break;
      case CM_ROAM_PREF_ANY:
        at_pref_ptr->roam_val = DSAT_SYSCONFIG_ROAM_ANY;
        break;
      case CM_ROAM_PREF_NO_CHANGE:
        at_pref_ptr->roam_val = DSAT_SYSCONFIG_ROAM_NO_CHANGE ;
        break;
      default:
        DS_AT_MSG1_ERROR("Invalid roam mode %d ",cm_pref_ptr->network_roam_pref);
        at_pref_ptr->roam_val = DSAT_SYSCONFIG_ROAM_UNKNOWN ;

    }
	
    switch( cm_pref_ptr->service_domain_pref )
    {
      case CM_SRV_DOMAIN_PREF_CS_ONLY:
        at_pref_ptr->domain_val =  DSAT_SYSCONFIG_DOMAIN_CS;
        break;
      case CM_SRV_DOMAIN_PREF_PS_ONLY:
        at_pref_ptr->domain_val =  DSAT_SYSCONFIG_DOMAIN_PS;
        break;
      case CM_SRV_DOMAIN_PREF_CS_PS:
        at_pref_ptr->domain_val =  DSAT_SYSCONFIG_DOMAIN_CS_PS;
        break;
      case CM_SRV_DOMAIN_PREF_ANY:
        at_pref_ptr->domain_val =  DSAT_SYSCONFIG_DOMAIN_ANY;
        break;
      case CM_SRV_DOMAIN_PREF_NO_CHANGE:
        at_pref_ptr->domain_val =  DSAT_SYSCONFIG_DOMAIN_NO_CHANGE;
        break;
      default:
        DS_AT_MSG1_ERROR("Invalid roam mode %d ",cm_pref_ptr->service_domain_pref);
        at_pref_ptr->domain_val =  DSAT_SYSCONFIG_DOMAIN_UNKNOWN;

    }
  }
  else
  {
    switch( at_pref_ptr->mode_val )
    {
      case DSAT_SYSCONFIG_MODE_AUTOMATIC:
        cm_pref_ptr->network_rat_mode_pref = CM_MODE_PREF_AUTOMATIC;
        break;
      case DSAT_SYSCONFIG_MODE_GSM:
        cm_pref_ptr->network_rat_mode_pref = CM_MODE_PREF_GSM_ONLY;
        break;
      case DSAT_SYSCONFIG_MODE_WCDMA:
        cm_pref_ptr->network_rat_mode_pref = CM_MODE_PREF_WCDMA_ONLY;
        break;
#ifdef FEATURE_TDSCDMA		
      case DSAT_SYSCONFIG_MODE_TDSCDMA:
        cm_pref_ptr->network_rat_mode_pref = CM_MODE_PREF_TDS_ONLY;
        break;
#endif /* FEATURE_TDSCDMA */
      case DSAT_SYSCONFIG_MODE_NO_CHANGE:
        cm_pref_ptr->network_rat_mode_pref = CM_MODE_PREF_NO_CHANGE;
        break;
      default:
        DS_AT_MSG1_ERROR("Invalid pref mode %d ",at_pref_ptr->mode_val);
        return DSAT_ERROR;
    }

    switch( at_pref_ptr->order_val )
    {
      case DSAT_SYSCONFIG_ORDER_AUTOMATIC:
        cm_pref_ptr->acq_order_pref = CM_GW_ACQ_ORDER_PREF_AUTOMATIC;
        break;
      case DSAT_SYSCONFIG_ORDER_GSM_WCDMA:
        cm_pref_ptr->acq_order_pref = CM_GW_ACQ_ORDER_PREF_GSM_WCDMA;
        break;
      case DSAT_SYSCONFIG_ORDER_WCDMA_GSM:
        cm_pref_ptr->acq_order_pref = CM_GW_ACQ_ORDER_PREF_WCDMA_GSM;
        break;
      case DSAT_SYSCONFIG_ORDER_NO_CHANGE:
        cm_pref_ptr->acq_order_pref = CM_GW_ACQ_ORDER_PREF_NO_CHANGE;
        break;
      default:
        DS_AT_MSG1_ERROR("Invalid acq_order %d ",at_pref_ptr->order_val);
        return DSAT_ERROR;
    }
	
    switch( at_pref_ptr->roam_val )
    {
      case DSAT_SYSCONFIG_ROAM_NONE:
        cm_pref_ptr->network_roam_pref = CM_ROAM_PREF_HOME;
        break;
      case DSAT_SYSCONFIG_ROAM_ANY:
        cm_pref_ptr->network_roam_pref = CM_ROAM_PREF_ANY;
        break;
      case DSAT_SYSCONFIG_ROAM_NO_CHANGE:
        cm_pref_ptr->network_roam_pref = CM_ROAM_PREF_NO_CHANGE;
        break;

      default:
        DS_AT_MSG1_ERROR("Invalid roam mode %d ",at_pref_ptr->roam_val);
        return DSAT_ERROR;
    }
	
    switch( at_pref_ptr->domain_val )
    {
      case DSAT_SYSCONFIG_DOMAIN_CS:
        cm_pref_ptr->service_domain_pref = CM_SRV_DOMAIN_PREF_CS_ONLY;
        break;
      case DSAT_SYSCONFIG_DOMAIN_PS:
        cm_pref_ptr->service_domain_pref = CM_SRV_DOMAIN_PREF_PS_ONLY;
        break;
      case DSAT_SYSCONFIG_DOMAIN_CS_PS:
        cm_pref_ptr->service_domain_pref = CM_SRV_DOMAIN_PREF_CS_PS;
        break;
      case DSAT_SYSCONFIG_DOMAIN_ANY:
        cm_pref_ptr->service_domain_pref = CM_SRV_DOMAIN_PREF_ANY;
        break;
      case DSAT_SYSCONFIG_DOMAIN_NO_CHANGE:
        cm_pref_ptr->service_domain_pref = CM_SRV_DOMAIN_PREF_NO_CHANGE;
        break;
      default:
        DS_AT_MSG1_ERROR("Invalid roam mode %d ",at_pref_ptr->domain_val);
        return DSAT_ERROR;
    }

  }
  return DSAT_OK;
}/* vend_convert_ph_pref_type */

/*===========================================================================

FUNCTION DSATVEND_PROCESS_SYSCONFIG_CMD

DESCRIPTION
  This function process the response for the ^SYSCONFIG command on return from
  asynchronous processing. 
 
DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_sysconfig_cmd 
(
  dsat_cmd_pending_enum_type cmd_type,
  ds_at_cm_ph_pref_type  *pref_mode    /* Network preference mode */
)
{
  dsat_result_enum_type result = DSAT_ERROR;

  switch ( cmd_type )
  {
    case DSAT_PENDING_SYSCONFIG_READ:
    {
      dsm_item_type *res_buff_ptr = NULL;
      dsat_sysconfig_type in_pref_val;

      result = vend_convert_ph_pref_type( TRUE,&in_pref_val,pref_mode );
      res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
      res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                              res_buff_ptr->size,
                                             "^SYSCONFIG: %d,%d,%d,%d\n",
                                             in_pref_val.mode_val,
                                             in_pref_val.order_val,
                                             in_pref_val.roam_val,
                                             in_pref_val.domain_val);
      dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
      result = DSAT_OK;
    }
      break;
    case DSAT_PENDING_SYSCONFIG_WRITE:
    {
      /* Nothing to do  */
      result = DSAT_OK;
    }
    break;
      default:
      ASSERT(0);
  }

  return result;
}/* dsatvend_process_sysconfig_cmd */

/*===========================================================================
  FUNCTION DSATVEND_EXEC_SYSCONFIG_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ^SYSCONFIG command

^ SYSCONFIG =  <mode>,<acqorder>,<roam>,<srvdomain>	<CR><LF>OK<CR><LF>
^ SYSCONFIG?
    ^SYSCONFIG:<mode>,<acqorder>,<roam>,<srvdomain><CR><LF><CR><LF>OK<CR><LF>

  <mode> system mode reference:
  2    Automatically select
  13   GSM ONLY
  14   WCDMA ONLY
  15   TDSCDMA ONLY
  16   no change
  <acqorder>: network accessing order reference:
  0    Automatically
  1    GSM first, UTRAN second
  2    UTRAN first, GSM second
  3    No change
  <roam>: roaming support:
  0	not support
  1	can roam
  2   No change
  <srvdomain>: domain configuration:
  0	CS_ONLY
  1	PS_ONLY
  2   CS_PS
  3   ANY
  4   No change

  DEPENDENCIES
  None

  RETURN VALUE
  
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_ASYNC_CMD : if it is a success.

  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsatvend_exec_sysconfig_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_sysconfig_type in_pref_val;
  ds_at_cm_ph_pref_type ph_pref_type;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
  dsat_error_info_s_type err_info;

  err_info.errval= DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if ( !IS_ETSI_MODE(dsatcmdp_get_current_mode() ))
  {
    DS_AT_MSG0_ERROR("Not applicable in CDMA mode ");
    return DSAT_ERROR;
  }
  if ( (NA|EQ|AR) == tok_ptr->op )
  {
    /* Verify the input parameter range  */

    if ( tok_ptr->args_found != 4 )
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }
    /*         MODE            */
    if ( dsatutil_atoi( &in_pref_val.mode_val, 
                       tok_ptr->arg[0], 10) != ATOI_OK )
    {
      err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
      err_info.arg_num = 0;
      goto send_error;
    }
	
    /*        ACQ_ORDER            */	
    if ( dsatutil_atoi( &in_pref_val.order_val, 
                       tok_ptr->arg[1], 10) != ATOI_OK )
    {
      err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
      err_info.arg_num = 1;
      goto send_error;
    }
	
    /*        ROAM_PREF            */	
    if ( dsatutil_atoi( &in_pref_val.roam_val, 
                       tok_ptr->arg[2], 10) != ATOI_OK )
    {
      err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
      err_info.arg_num = 2;
      goto send_error;
    }
	
    /*        DOMAIN_PREF            */	
    if ( dsatutil_atoi( &in_pref_val.domain_val, 
                       tok_ptr->arg[3], 10) != ATOI_OK )
    {
      err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
      err_info.arg_num = 3;
      goto send_error;
    }
    /* Convert to CM enum types */
    result = vend_convert_ph_pref_type( FALSE,&in_pref_val,&ph_pref_type );
    if(DSAT_OK != result)
    {
      err_info.errval = DSAT_ERR_INVALID_TOKENS;
      goto send_error;
    }
	
    /* Verify if the pref values are correct */
    if ( FALSE == cm_is_valid_mode_pref(ph_pref_type.network_rat_mode_pref))
    {
      DS_AT_MSG0_ERROR("Invalid CM MODE PREF setting");
      (void)dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
      return DSAT_CMD_ERR_RSP;
    }


    SET_PENDING(DSAT_VENDOR_SYSCONFIG_IDX ,0, DSAT_PENDING_SYSCONFIG_WRITE)
#ifdef FEATURE_DUAL_SIM
    (void) cm_ph_cmd_sys_sel_pref_per_subs(
                                            dsatcmif_ph_cmd_cb_func,
                                            NULL,
                                            dsatcm_client_id,
                                            subs_id,
                                            ph_pref_type.network_rat_mode_pref,
                                            CM_PREF_TERM_PERMANENT,
                                            0,
                                            ph_pref_type.acq_order_pref,
                                            CM_BAND_PREF_NO_CHANGE,
                                            CM_PRL_PREF_NO_CHANGE,
                                            ph_pref_type.network_roam_pref,
                                            CM_HYBR_PREF_NO_CHANGE,
                                            ph_pref_type.service_domain_pref,
                                            CM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                                            NULL 
                                           );
#else
    (void) cm_ph_cmd_sys_sel_pref_new ( dsatcmif_ph_cmd_cb_func,
                                    NULL,
                                    dsatcm_client_id,
                                    ph_pref_type.network_rat_mode_pref,
                                    CM_PREF_TERM_PERMANENT,
                                    0,
                                    ph_pref_type.acq_order_pref,
                                    CM_BAND_PREF_NO_CHANGE,
                                    CM_PRL_PREF_NO_CHANGE,
                                    ph_pref_type.network_roam_pref,
                                    CM_HYBR_PREF_NO_CHANGE,
                                    ph_pref_type.service_domain_pref,
                                    CM_NETWORK_SEL_MODE_PREF_NO_CHANGE,
                                    NULL 
                                  ); 
#endif /* FEATURE_DUAL_SIM */
    result = DSAT_ASYNC_CMD;
  }
  else if ( (NA|QU) == tok_ptr->op )
  {
    SET_PENDING(DSAT_VENDOR_SYSCONFIG_IDX ,0, DSAT_PENDING_SYSCONFIG_READ)
    /* The return value is ignored as both success and failure cases
     * are handled by the call back function */
    (void)cm_ph_cmd_get_ph_info_per_subs (dsatcmif_ph_cmd_cb_func,
                                 NULL,
                                 dsatcm_client_id,
                                 subs_id
                                 );
    
    result = DSAT_ASYNC_CMD;

  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_NUM_OF_ARGS ||
      err_info.errval == DSAT_ERR_INVALID_TOKENS)
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;

}/* dsatvend_exec_sysconfig_md */
/*===========================================================================

FUNCTION DSATVEND_EXEC_PACSP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT+PACSP command.
  This function returns the current PLMN mode bit setting of EF CSP file.

DEPENDENCIES
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : If the data is read at initalization, result is returned
              immediately. This is another success case. 
SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_pacsp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ERROR;
  dsatme_mmgsdi_state_ms_info  *me_ms_val = NULL;
  
  me_ms_val = dsat_get_base_addr_current_session();

  /* Only read command is supported */
  if ( tok_ptr->op == (NA|QU) )
  {
    if(me_ms_val->dsatme_plmn_mode_sel_bit != CSP_PLMN_MODE_BIT_UNSET)
    {
      res_buff_ptr->used =
        (word) snprintf((char*)res_buff_ptr->data_ptr,
                              res_buff_ptr->size,
                              "+PACSP%d",
                              me_ms_val->dsatme_plmn_mode_sel_bit);
      result = DSAT_OK;
    }
    else
    {
      DS_AT_MSG0_ERROR("EONS files not read at Init");
    }
  }
  return result;
}/* dsatvend_exec_pacsp_cmd */

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCVOLT_CMD

DESCRIPTION
  This function returns the current main chip voltage in milli-volts.

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcvolt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /*-------------------------------------------------------
  command with no arguments will return the info string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
    int32 adc_ret = 0;

#ifdef FEATURE_DSAT_EXTENDED_CMD
    adc_ret = vbatt_read_ichg_out_mv();                      
    if( !dsatutil_strcmp_ig_sp_case((const byte *)parse_table->name, (const byte *)"^VOLT") )
      {
        res_buff_ptr->used =
         (word)snprintf((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                               "%s: %d", parse_table->name, (int)adc_ret);
      }
      else
#endif/* FEATURE_DSAT_EXTENDED_CMD */
      {
        res_buff_ptr->used =
        (word)snprintf((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                               "%d", (int)adc_ret);
      }
  }
  else
  {
     result = DSAT_ERROR;
  }
  return result;
} /* dsatvend_exec_qcvolt_cmd */

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCHWREV_CMD

DESCRIPTION
  This function returns the MSM chip HW part number and revision info.

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qchwrev_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /*-------------------------------------------------------
  command with no arguments will return the info string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
    DalDeviceHandle *phChipInfo = NULL;
    DalChipInfoIdType nChipId = DALCHIPINFO_ID_UNKNOWN;
    DalChipInfoVersionType pnVersion = 0;
    
    if (DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &phChipInfo) != DAL_SUCCESS)
    {
      DS_AT_MSG0_ERROR("Could not get DAL Device Handle, bailing out");
      return DSAT_ERROR;
    }
    DalChipInfo_GetChipId( phChipInfo, &nChipId );
    DalChipInfo_GetChipVersion(phChipInfo,  &pnVersion);

#ifdef FEATURE_DSAT_EXTENDED_CMD
    if( !dsatutil_strcmp_ig_sp_case((const byte *)parse_table->name, (const byte *)"^HWVER") )
    {
      res_buff_ptr->used =
        (uint16)snprintf((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                          "%s: Part=%d  version=%d.%d",
                            parse_table->name,
                            nChipId,
                           (int)(pnVersion >> 16),(int)(pnVersion & 0xFF));
    }
    else
#endif/* FEATURE_DSAT_EXTENDED_CMD */
    {
      res_buff_ptr->used =
        (uint16)snprintf((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                      "Part=%d  version=%d.%d",
                      nChipId,
                      (int)(pnVersion >> 16),(int)(pnVersion & 0xFF));
    }
  }
  else
  {
     result = DSAT_ERROR;
  }
  return result;
} /* dsatvend_exec_qchwrev_cmd */

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCBOOTVER_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $QCQCBOOTVER command.
  The result is in res_buff_ptr, reporting boot code build info.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :         if it is a success and execution completed.

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcbootver_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ERROR;

#ifdef FEATURE_DSAT_GOBI_MAINLINE 
  /* process command */
  if ( tok_ptr->op == NA )
  {
    /* Request boot code build info  */
    char       boot_ver[GOBI_IM_BUILD_ID_LEN];

    gobi_nv_status_e_type nv_status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    (void) memset(boot_ver, 0, sizeof(boot_ver));
    nv_status = gobi_nv_read(GOBI_NV_ITEM_BOOT_SW_VER, 0, boot_ver, 
                             sizeof(boot_ver));
    
    /* Request boot code build info  */
    status = dsatutil_get_nv_item( (nv_items_enum_type)5252/*NV_BOOT_SW_VER_I*/, &ds_nv_item );
    if ( status == NV_DONE_S )
    {
      /*-----------------------------------------------------------------------
        Store value retrieved from NV.
      -----------------------------------------------------------------------*/
          /* Generate response */
          /* boot_sw_ver need to be added, when NV item is provisioned */
          res_buff_ptr->used = 
            (word)snprintf((char*)res_buff_ptr->data_ptr, 
                                 res_buff_ptr->size,
                                 "%s",
                                 boot_ver);
          result = DSAT_OK;
      }
    }
#endif /* FEATURE_DSAT_GOBI_MAINLINE */

  return result;
} /* dsatvend_exec_qcbootver_cmd */

#ifdef FEATURE_DSAT_GOBI_MAINLINE
/*============================================================================

FUNCTION DSATVEND_FTM_DIAGPKT_FWD 

DESCRIPTION
   Forward (via ONRPC) FTM diagpkt to FTM

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
void dsatvend_ftm_diagpkt_fwd
(
   ftm_pkt_type *ftm_pkt, 
   dsatvend_ftm_diagpkt_cb_f_type rsp_cb,
   void         *rsp_arg
)
{
   dsatvend_ftm_diagpkt_rsp_params.rsp_cb  = rsp_cb;
   dsatvend_ftm_diagpkt_rsp_params.rsp_arg = rsp_arg;
   ftm_diagpkt_fwd((char *)ftm_pkt, dsatvend_ftm_diagpkt_fwd_cb);
} /* dsatvend_ftm_diagpkt_fwd */
  
/*============================================================================

FUNCTION DSATVEND_FTM_DIAGPKT_FWD_CB

DESCRIPTION
   Called in ONRPC context for FTM diagpkt response call back.

   This callback schedules DS_AT_FTM_DIAGPKT_RSP_CMD event to DS task
   in order to process the pktdiag message in DS context.

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
void dsatvend_ftm_diagpkt_fwd_cb(char *rsp_ftm_pkt)
{
  ftm_pkt_type *ftm_pkt;
  ds_cmd_type  *cmd_buf;

  /* Schedule callback under DS to process the response from FTM  */
  DS_AT_MSG0_HIGH("Schedule callback under DS to process the response from FTM");

  /* Save FTM packet response - must be processed in callback under DS
  to avoid deadlock if the response callback sends FTM diagpkt */
  cmd_buf = dsat_get_cmd_buf(sizeof(ftm_pkt_type), FALSE);
  ftm_pkt = cmd_buf->cmd_payload_ptr;

  cmd_buf->hdr.cmd_id = (ds_cmd_enum_type)DS_CMD_ATCOP_FTM_DIAGPKT_RSP_CMD;
  (void) dsatutil_memscpy((void*)ftm_pkt,
                          sizeof(ftm_pkt_type),
                          (void*)rsp_ftm_pkt,
                          sizeof(ftm_pkt_type));       
  ds_put_cmd(cmd_buf);
} /* dsatvend_ftm_diagpkt_fwd_cb */

/*============================================================================

FUNCTION DSATVEND_FTM_DIAGPKT_RSP_HANDLER

DESCRIPTION
   Called in DS context for DS_AT_FTM_DIAGPKT_RSP_CMD event.
   This event is used to execute callbacks to process FTM diag packets.

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_ASYNC_EVENT : if it a ASYNC event.
SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_ftm_diagpkt_rsp_handler
(
  ds_cmd_type         *cmd_buf             /* DS Command pointer         */
)
{

   dsat_result_enum_type result    = DSAT_ASYNC_EVENT;
   ftm_pkt_type *ftm_pkt_ptr = dsat_get_cmd_payload_ptr(cmd_buf);

   /* If callback registered, execute callback with response diagpkt */
   if ( dsatvend_ftm_diagpkt_rsp_params.rsp_cb )
   {
     dsatvend_ftm_diagpkt_cb_f_type *rsp_cb;
     (void) dsatutil_memscpy((void*)&dsatvend_ftm_diagpkt_rsp_params.ftm_diagpkt,
             sizeof(ftm_pkt_type),(void*)ftm_pkt_ptr,sizeof(ftm_pkt_type));    
     rsp_cb = dsatvend_ftm_diagpkt_rsp_params.rsp_cb;
     dsatvend_ftm_diagpkt_rsp_params.rsp_cb = NULL;
     return rsp_cb(&dsatvend_ftm_diagpkt_rsp_params.ftm_diagpkt,
                              dsatvend_ftm_diagpkt_rsp_params.rsp_arg);
   }
   
   return result;
} /* dsatvend_ftm_diagpkt_rsp_handler */

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCTEMP_CMD

DESCRIPTION
  This function returns the current temperature reported by MDM thermistor

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qctemp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsatdvend_proc_qcagc_type proc_qcagc;

  if (tok_ptr->op == NA)
  {
    int32 adc_ret;

    adc_ret = vbatt_read_pmic_temperature();
        res_buff_ptr->used = 
          (uint16)snprintf((char*)res_buff_ptr->data_ptr,
                                 res_buff_ptr->size,
                                 "%d", 
                                   adc_ret);
  }
  else
  {
     result = DSAT_ERROR;
  }

  return result; 
} /* dsatvend_exec_qctemp_cmd */

/*============================================================================

FUNCTION DSATVEND_EXEC_QCAGC_GET_AGC_CB

DESCRIPTION
   FTM response AGC value is converted and displayed.  
   AT command is completed.

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcagc_get_agc_cb
(
   ftm_pkt_type *ftm_pkt,
   void *rsp_arg
)
{
   dsatdvend_proc_qcagc_type *proc_qcagc = 
                      (dsatdvend_proc_qcagc_type *)rsp_arg;
   dsm_item_type *res_buff_ptr;
   short agc_val;
   short rssi_val;

   dsati_mode_e_type current_mode = dsatcmdp_get_current_mode();
   
   agc_val = ftm_pkt->rf_params.ftm_rf_factory_data.gen_w;

   /* Convert 10bit 2's complement */
   if (agc_val > 511) 
   {
      agc_val -= 1024;
   }

   if ( IS_ETSI_MODE(current_mode) )
   {
     /* Convert mdsp db value to mdb using equation:
      *     RSSI  = (RXAGC + 512)/AgcUnitsPerDB + MinRSSIofInterest
      *     where AgcUnitsPerDB=12 and MinRSSIofInterest= -106dBm
      */
   rssi_val = (agc_val + 512)/12  - 106;
     /* Use Cal NV items to adjust the RSSI -
      * NV_WCDMA_VGA_GAIN_OFFSET, NV_WCDMA_VGA_GAIN_OFFSET_VS_FREQ 
      * 
      * Cal Adjustment Val = -(_OFFSET + _VS_FREQ) / 12
      * 
      * Adjusted RSSI = measured RSSI + Cal Adjustment
      */ 
     rssi_val -= ((proc_qcagc->sys_mode.etsi.vga_gain_offset + proc_qcagc->sys_mode.etsi.vga_vs_chan_freq)/12);
   }
   else if ( IS_CDMA_MODE(current_mode) )
   {
   /* Convert mdsp db value to mDb using equation:
      RSSI  = (RXAGC + 512)/AgcUnitsPerDB + MinRSSIofInterest */
     rssi_val = (agc_val + 512)/proc_qcagc->sys_mode.cdma.rx_agc_range + 
                  proc_qcagc->sys_mode.cdma.rx_min_rssi;
   }
   else
   {
     return DSAT_ERROR;
   }

  res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
  res_buff_ptr->used = snprintf((char*)res_buff_ptr->data_ptr,
                                     res_buff_ptr->size, "RSSI: %d",
                                     rssi_val );

  dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);

  SET_PENDING(DSAT_VENDOR_QCAGC_IDX ,0, DSAT_PENDING_QCAGC_NONE)
  return DSAT_OK;
} /* dsatvend_exec_qcagc_get_agc_cb */

/*============================================================================

FUNCTION DSATVEND_EXEC_QCAGC_SET_LNA_CB

DESCRIPTION
   Received FTM command FTM_SET_LNA_RANGE response.
   Send FTM FTM_GET_CAGC_RX_AGC command on specified path

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_ASYNC_EVENT : if its a ASYNC EVENT
SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcagc_set_lna_cb
(
   ftm_pkt_type *ftm_pkt,
   void *rsp_arg
)
{
   dsatdvend_proc_qcagc_type *proc_qcagc = 
                      (dsatdvend_proc_qcagc_type *)rsp_arg;
   
   ftm_diagpkt_subsys_header_type *ftm_diag_hdr;

   /* Get AGC */
   ftm_diag_hdr = (ftm_diagpkt_subsys_header_type *)&ftm_pkt->header;
   ftm_diag_hdr->subsys_cmd_code = (diagpkt_subsys_cmd_code_type)
            (proc_qcagc->enable_rx_diversity ?
          proc_qcagc->ftm_dispatch_div_id : proc_qcagc->ftm_dispatch_id);

   ftm_pkt->rf_params.ftm_rf_cmd  = (word)FTM_GET_CAGC_RX_AGC;

   dsatvend_ftm_diagpkt_fwd(ftm_pkt, 
                            dsatvend_exec_qcagc_get_agc_cb,
                            (void *)proc_qcagc);
    
   SET_PENDING(DSAT_VENDOR_QCAGC_IDX ,0, DSAT_PENDING_QCAGC_GET_AGC)
   return DSAT_ASYNC_EVENT;
} /* dsatvend_exec_qcagc_set_lna_cb */

/*============================================================================

FUNCTION DSATVEND_EXEC_QCAGC_SET_SECONDARY_CHAIN_CB

DESCRIPTION
   Process received FTM_SET_SECONDARY_CHAIN response.
   Send FTM_SET_LNA_RANGE command on diversity path

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_ASYNC_EVENT : if it is a ASYNC EVENT
SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcagc_set_secondary_chain_cb
(
   ftm_pkt_type *ftm_pkt,
   void *rsp_arg
)
{
   dsatdvend_proc_qcagc_type *proc_qcagc = 
                      (dsatdvend_proc_qcagc_type *)rsp_arg;
   
   ftm_diagpkt_subsys_header_type *ftm_diag_hdr;

   /* Send LNA selection on diversity path */
   ftm_diag_hdr = (ftm_diagpkt_subsys_header_type *)&ftm_pkt->header;
   ftm_diag_hdr->subsys_cmd_code = (diagpkt_subsys_cmd_code_type)
                                    proc_qcagc->ftm_dispatch_div_id;

   ftm_pkt->rf_params.ftm_rf_cmd  = (word)FTM_SET_LNA_RANGE;
   ftm_pkt->rf_params.ftm_rf_factory_data.range = 
                             (ftm_pa_range_type)proc_qcagc->lna;

   dsatvend_ftm_diagpkt_fwd(ftm_pkt, 
                             dsatvend_exec_qcagc_set_lna_cb,
                            (void *)proc_qcagc);
  
   SET_PENDING(DSAT_VENDOR_QCAGC_IDX ,0, DSAT_PENDING_QCAGC_SET_LNA)
   return DSAT_ASYNC_EVENT;
} /* dsatvend_exec_qcagc_set_secondary_chain_cb */


/*============================================================================

FUNCTION DSATVEND_EXEC_QCAGC_GET_SYNTH_CB

DESCRIPTION
   Process received FTM_GET_SYNTH_STATE response.
   If RX diversity path
      Send FTM_SET_SECONDARY_CHAIN command.
   Else
      Send FTM_SET_LNA_RANGE command.

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_ASYNC_EVENT : if it is a ASYNC EVENT
    DSAT_CMD_ERR_RSP : if sending out error_response

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcagc_get_synth_cb
(
   ftm_pkt_type *ftm_pkt,
   void *rsp_arg
)
{
   dsatdvend_proc_qcagc_type *proc_qcagc = 
                      (dsatdvend_proc_qcagc_type *)rsp_arg;
   
   ftm_diagpkt_subsys_header_type *ftm_diag_hdr;

   /* Verify Lock state in synth */
   if (!ftm_pkt->rf_params.ftm_rf_factory_data.on_off) 
   {
      dsm_item_type *res_buff_ptr;
      res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
      res_buff_ptr->used = (word)snprintf((char*)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,
                                                "AGC:Synth not locked");
      dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);

      SET_PENDING(DSAT_VENDOR_QCAGC_IDX ,0, DSAT_PENDING_QCAGC_NONE)
      return DSAT_CMD_ERR_RSP;
   }

   /* If RX diversity path is used, enable path via FTM_SET_SECONDARY_CHAIN */
   if (proc_qcagc->enable_rx_diversity)
   {
      ftm_diag_hdr = (ftm_diagpkt_subsys_header_type *)&ftm_pkt->header;
      ftm_diag_hdr->subsys_cmd_code = (diagpkt_subsys_cmd_code_type)
                                       proc_qcagc->ftm_dispatch_div_id;

      ftm_pkt->rf_params.ftm_rf_cmd  = (word)FTM_SET_SECONDARY_CHAIN;
      ftm_pkt->rf_params.ftm_rf_factory_data.on_off = 1;

      dsatvend_ftm_diagpkt_fwd(ftm_pkt, 
                               dsatvend_exec_qcagc_set_secondary_chain_cb,
                               (void *)proc_qcagc);

      SET_PENDING(DSAT_VENDOR_QCAGC_IDX ,0, DSAT_PENDING_QCAGC_SET_SEC_CHAIN)
      return DSAT_ASYNC_EVENT;
   }

   /* Else send LNA selection */
   ftm_diag_hdr = (ftm_diagpkt_subsys_header_type *)&ftm_pkt->header;
   ftm_diag_hdr->subsys_cmd_code = (diagpkt_subsys_cmd_code_type)
                                    proc_qcagc->ftm_dispatch_id;

   ftm_pkt->rf_params.ftm_rf_cmd  = (word)FTM_SET_LNA_RANGE;
   ftm_pkt->rf_params.ftm_rf_factory_data.range = 
                             (ftm_pa_range_type)proc_qcagc->lna;

   dsatvend_ftm_diagpkt_fwd(ftm_pkt, 
                             dsatvend_exec_qcagc_set_lna_cb,
                             (void *)proc_qcagc);
  
   SET_PENDING(DSAT_VENDOR_QCAGC_IDX ,0, DSAT_PENDING_QCAGC_SET_LNA)
   return DSAT_ASYNC_EVENT;
} /* dsatvend_exec_qcagc_get_synth_cb */

/*============================================================================

FUNCTION DSATVEND_EXEC_QCAGC_SET_CHAN_CB

DESCRIPTION
   Process received FTM_SET_CHAN response.
   Send FTM_GET_SYNTH_STATE command.

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_ASYNC_EVENT : if it is a ASYNC EVENT

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcagc_set_chan_cb
(
   ftm_pkt_type *ftm_pkt,
   void *rsp_arg
)
{
   dsatdvend_proc_qcagc_type *proc_qcagc = 
                      (dsatdvend_proc_qcagc_type *)rsp_arg;
   
   ftm_diagpkt_subsys_header_type *ftm_diag_hdr;

   /* Send Get SYNTH state */
   ftm_diag_hdr = (ftm_diagpkt_subsys_header_type *)&ftm_pkt->header;
   ftm_diag_hdr->subsys_cmd_code = (diagpkt_subsys_cmd_code_type)
                                    proc_qcagc->ftm_dispatch_id;
 
   ftm_pkt->rf_params.ftm_rf_cmd  = (word)FTM_GET_SYNTH_STATE;
 
   dsatvend_ftm_diagpkt_fwd(ftm_pkt, 
                             dsatvend_exec_qcagc_get_synth_cb,
                             (void *)proc_qcagc);
  
   SET_PENDING(DSAT_VENDOR_QCAGC_IDX ,0, DSAT_PENDING_QCAGC_GET_SYNTH)
   return DSAT_ASYNC_EVENT; 
} /* dsatvend_exec_qcagc_set_chan_cb */


/*============================================================================

FUNCTION DSATVEND_EXEC_QCAGC_SET_MODE_CB

DESCRIPTION
   Process received FTM_SET_MODE response.
   Send FTM_SET_CHAN command.

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_ASYNC_EVENT : if it is a ASYNC EVENT

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcagc_set_mode_cb
(
   ftm_pkt_type *ftm_pkt,
   void *rsp_arg
)
{
   dsatdvend_proc_qcagc_type *proc_qcagc = 
                      (dsatdvend_proc_qcagc_type *)rsp_arg;
   ftm_diagpkt_subsys_header_type *ftm_diag_hdr;

   /* Send channel selection */
   ftm_diag_hdr = (ftm_diagpkt_subsys_header_type *)&ftm_pkt->header;
   ftm_diag_hdr->subsys_cmd_code = (diagpkt_subsys_cmd_code_type)
                                    proc_qcagc->ftm_dispatch_id;

   ftm_pkt->rf_params.ftm_rf_cmd  = (word)FTM_SET_CHAN;
   ftm_pkt->rf_params.ftm_rf_factory_data.chan = proc_qcagc->sys_mode.cdma.chan;

   dsatvend_ftm_diagpkt_fwd(ftm_pkt, 
                            dsatvend_exec_qcagc_set_chan_cb,
                            (void *)proc_qcagc);
   
   
   SET_PENDING(DSAT_VENDOR_QCAGC_IDX ,0, DSAT_PENDING_QCAGC_SET_CHAN)
   
   return DSAT_ASYNC_EVENT;
} /* dsatvend_exec_qcagc_set_mode_cb */

/*============================================================================

FUNCTION DSATVEND_EXEC_QCAGC_CMD

DESCRIPTION
  Get current AGC value from specified band/channel
   
  AT$QCAGC <band>,<rx_chan>[,<path>]
     band
     chan
     path  ["main"] "aux"

   band -
      CDMA_800    (Cell)
      CDMA_1900   (PCS)
   
  Note: Restricted to FTM
        WCDMA version implemented (below)
  
DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_ASYNC_EVENT : if it is a ASYNC EVENT

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcagc_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsati_mode_e_type oper_mode = dsatcmdp_get_current_mode();

#define  CHAN_LIST_RANGE_VAL -1 /* denotes inclusive range pair */
#define  CHAN_LIST_EOL_VAL   -2 /* end of list */
#define  CHAN_LIST_ENTRIES_MAX 20  /* maximum entries in the channel-list */

  if ( IS_CDMA_MODE (oper_mode) )
  {     
     static dsatdvend_proc_qcagc_type proc_qcagc;
     
     typedef struct band_label_s {
        int              ftm_dispatch_main_id;
        int              ftm_dispatch_div_id;
        char            *name;
        ftm_mode_id_type mode_band;
        int              chan_list[CHAN_LIST_ENTRIES_MAX];
     } band_label_type;
  
     band_label_type *band_label;
     
     /* Supported Mode/Band labels with associated FTM and NV defs */
     band_label_type agc_band_labels[] = {
        { FTM_1X_C, FTM_1X_RX_2_C,           "\"CDMA_800\"",  
                       FTM_PHONE_MODE_CDMA_800,
                       {   1, CHAN_LIST_RANGE_VAL,  799,
                         991, CHAN_LIST_RANGE_VAL, 1023,
                         CHAN_LIST_EOL_VAL} },
        
        { FTM_1X_C, FTM_1X_RX_2_C,           "\"CDMA_1900\"",  
                        FTM_PHONE_MODE_CDMA_1900,
                       { 0, CHAN_LIST_RANGE_VAL , 1199,
                         CHAN_LIST_EOL_VAL} },
       };
       
     /* If arguments passed... */ 
     if (tok_ptr->op == (NA|EQ|AR) ) 
     {
        int i;
        int chan;        
        
        if ((tok_ptr->args_found < 1) || (tok_ptr->args_found > 3)) 
        {
           return DSAT_ERROR;
        }
        
        if (ftm_get_mode() != FTM_MODE) 
        {
          res_buff_ptr->used =
            (uint16)snprintf((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                                "ERROR - restricted to FTM");
           return DSAT_CMD_ERR_RSP;  
        }
  
        /* Get and validate mode/band arg */
        band_label = NULL;
        for (i = 0; (!band_label) && (i < (int)ARR_SIZE(agc_band_labels)); i++)
        {
           if (!dsatutil_strcmp_ig_sp_case((const byte *)agc_band_labels[i].name, 
                                           tok_ptr->arg[0])) 
           {
              band_label = &agc_band_labels[i];
           }
        }
        if (!band_label) 
        {
          res_buff_ptr->used = 
            (uint16)snprintf((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                                            "ERROR: Invalid <band>\n");
          return DSAT_CMD_ERR_RSP;
        }
        proc_qcagc.mode_band  = band_label->mode_band;
  
        /* Get chan and validate */
        if (dsatutil_atoi((dsat_num_item_type *)&chan, tok_ptr->arg[1], 10)) 
        {
           return DSAT_ERROR;
        }
  
        /* Verify chan value is in list */
        for (i = 0; (CHAN_LIST_EOL_VAL != band_label->chan_list[i]) 
              && (i < (CHAN_LIST_ENTRIES_MAX-2)); i++)
        {
           if ((int)chan >= band_label->chan_list[i])
           {
              if (chan == band_label->chan_list[i])
              {
                 break;
              }
              if ((CHAN_LIST_EOL_VAL   != band_label->chan_list[i+2]) &&
                  (CHAN_LIST_RANGE_VAL == band_label->chan_list[i+1]) &&
                  ((int)chan <= band_label->chan_list[i+2])) 
              {
                 break;
              }
           }
        }
        if (CHAN_LIST_EOL_VAL == band_label->chan_list[i])
        {
          res_buff_ptr->used = 
            (uint16)snprintf((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                                                "ERROR: Invalid <chan>\n");
          return DSAT_CMD_ERR_RSP;
        }

        proc_qcagc.sys_mode.cdma.chan = chan;

        /* If path specified, get and validate */
        proc_qcagc.ftm_dispatch_id     = band_label->ftm_dispatch_main_id;
        proc_qcagc.ftm_dispatch_div_id = band_label->ftm_dispatch_div_id;
        proc_qcagc.enable_rx_diversity = 0;
        
        if (tok_ptr->args_found == 3)
        {
           if (!dsatutil_strcmp_ig_sp_case("\"aux\"", tok_ptr->arg[2]))
           {
              proc_qcagc.enable_rx_diversity = 1;
           } 
           else if (dsatutil_strcmp_ig_sp_case("\"main\"", tok_ptr->arg[2]))
           {
             res_buff_ptr->used = 
               (uint16)snprintf((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                                                   "ERROR: Invalid <path>\n");
             return DSAT_CMD_ERR_RSP;
           }
        }
        
        proc_qcagc.lna = 0;
  
        /* Start FTM command sequence to get AGC via commands -
         *    Get NV Cal values for specific band
         *    FTM_SET_MODE
         *    FTM_SET_CHAN
         *    FTM_GET_SYNTH_STATE
         *    FTM_SET_LNA_RANGE
         *    FTM_GET_CAGC_RX_AGC
         */
        {
           ftm_pkt_type ftm_pkt;
           ftm_diagpkt_subsys_header_type *ftm_diag_hdr;
           nv_stat_enum_type nv_status;
           
           /* Default RSSI measurement variables */
           proc_qcagc.sys_mode.cdma.rx_agc_range   = 10;
           proc_qcagc.sys_mode.cdma.rx_min_rssi    = -115;
           
           /* Adjust RSSI measurement per NV Cal values if present */
           nv_status = dsatutil_get_nv_item(NV_CDMA_DYNAMIC_RANGE_I, 
                                            &ds_nv_item);
           if (nv_status == NV_DONE_S) 
           {
              proc_qcagc.sys_mode.cdma.rx_agc_range = ds_nv_item.cdma_dynamic_range;
           }
           
           nv_status = dsatutil_get_nv_item(NV_CDMA_MIN_RX_RSSI_I,
                                            &ds_nv_item);
           if (nv_status == NV_DONE_S) 
           {
              proc_qcagc.sys_mode.cdma.rx_min_rssi = ds_nv_item.cdma_min_rx_rssi;
           }
  
           /* Send FTM Mode / band selection */
           ftm_diag_hdr = (ftm_diagpkt_subsys_header_type *)&ftm_pkt.header;
           ftm_diag_hdr->subsys_cmd_code = (diagpkt_subsys_cmd_code_type)
                                            proc_qcagc.ftm_dispatch_id;
           ftm_pkt.rf_params.ftm_rf_cmd  = (word)FTM_SET_MODE;
           ftm_pkt.rf_params.ftm_rf_factory_data.mode = 
                                             proc_qcagc.mode_band;
           
           dsatvend_ftm_diagpkt_fwd(&ftm_pkt, 
                                    dsatvend_exec_qcagc_set_mode_cb,
                                    (void *)&proc_qcagc);

           SET_PENDING(DSAT_VENDOR_QCAGC_IDX ,0, DSAT_PENDING_QCAGC_SET_MODE)
           result = DSAT_ASYNC_CMD;
        }
     }
     else if (tok_ptr->op == (NA|EQ|QU) )
     {
        uint16 i, j, buf_size;
        buf_size =  res_buff_ptr->size;
        res_buff_ptr->used = 
          (uint16)snprintf((char*)res_buff_ptr->data_ptr, buf_size,
                                              "AGC: <band>,<rx_chan>[,<path>]\n");
  
        for (i = 0; i < (int)ARR_SIZE(agc_band_labels); i++)
        {
           band_label =  &agc_band_labels[i];
           res_buff_ptr->used += 
             (uint16)snprintf((char*)res_buff_ptr->data_ptr + res_buff_ptr->used, 
                                              buf_size - res_buff_ptr->used,
                                              "  <band>,<rx_chan> %s,(", band_label->name);
           
           for (j = 0; (CHAN_LIST_EOL_VAL != band_label->chan_list[j]) 
                 && (j < CHAN_LIST_ENTRIES_MAX); j++)
           {
              char str_buf[24];
              if (CHAN_LIST_RANGE_VAL == band_label->chan_list[j])
              {
                 (void)snprintf(str_buf, sizeof(str_buf), "-");
              }
              else 
              {
                 if (!j || (CHAN_LIST_RANGE_VAL == band_label->chan_list[j-1]))
                 {
                    (void)snprintf(str_buf, sizeof(str_buf), "%d",
                                   band_label->chan_list[j]);
                 }
                 else
                 {
                    (void)snprintf(str_buf, sizeof(str_buf), ",%d",
                                   band_label->chan_list[j]);
                 }
              }
              res_buff_ptr->used += 
                (uint16)snprintf((char*)res_buff_ptr->data_ptr + res_buff_ptr->used,
                                   buf_size - res_buff_ptr->used,
                                   "%s", str_buf);
              
           } /* End for (j = 0; CHAN_LIST_EOL_VAL */
           
           res_buff_ptr->used += 
             (uint16)snprintf((char*)res_buff_ptr->data_ptr + res_buff_ptr->used,
                               buf_size - res_buff_ptr->used,
                               ")\n");
        } /* End for (i = 0; i < ARR_SIZE(agc_band_labels) */
        
        res_buff_ptr->used += 
          (uint16)snprintf((char*)res_buff_ptr->data_ptr + res_buff_ptr->used, 
                            buf_size - res_buff_ptr->used,
                            "  <path> (\"MAIN\",\"AUX\")\n");
        return DSAT_OK;
        
     } /* End else if (tok_ptr->op == (NA|EQ|QU) ) */
     else
     {
       result = DSAT_ERROR;
     }
   }
   else if ( IS_ETSI_MODE(oper_mode))
   {        
     static dsatdvend_proc_qcagc_type proc_qcagc;
        
     typedef struct band_nv_cal_s {
       nv_items_enum_type  cal_rx_chan;        
       nv_items_enum_type  vga_gain_offset;
       nv_items_enum_type  vga_vs_freq;
     } band_nv_cal_type;
        
     typedef struct band_label_s {
       int              ftm_dispatch_main_id;
       int              ftm_dispatch_div_id;
       char            *name;
       ftm_mode_id_type mode_band;
       int              rx_tx_chan_offset; /* Offset from Rx to Tx */
       band_nv_cal_type cal_id_main;       /* Cal NV items for main path */
       band_nv_cal_type cal_id_div;        /* Cal NV items for diversity path */
       int     chan_list[CHAN_LIST_ENTRIES_MAX];
     } band_label_type;
     
     band_label_type *band_label;
        
     /* Supported Mode/Band labels with associated FTM and NV defs */
     band_label_type agc_band_labels[] = {
       { FTM_WCDMA_C, FTM_WCDMA_RX_2_C,           "\"WCDMA_IMT\"",  
         FTM_PHONE_MODE_WCDMA_IMT,
         BSP_RF_WCDMA_BC1_RX_TX_CHAN_NUMBER_OFFSET,
         { NV_WCDMA_RX_CAL_CHAN_I ,
           NV_WCDMA_VGA_GAIN_OFFSET_I, 
           NV_WCDMA_VGA_GAIN_OFFSET_VS_FREQ_I },
         { NV_C1_WCDMA_2100_RX_CAL_CHAN_I ,
           NV_C1_WCDMA_2100_VGA_GAIN_OFFSET_I, 
           NV_C1_WCDMA_2100_VGA_GAIN_OFFSET_VS_FREQ_I },
         { BSP_RF_RX_CHAN_WCDMA_BC1_MIN, CHAN_LIST_RANGE_VAL, 
           BSP_RF_RX_CHAN_WCDMA_BC1_MAX, CHAN_LIST_EOL_VAL} },
       
       { FTM_WCDMA_C, FTM_WCDMA_RX_2_C,           "\"WCDMA_800\"",  
         FTM_PHONE_MODE_WCDMA_800 ,
         BSP_RF_WCDMA_BC5_RX_TX_CHAN_NUMBER_OFFSET,
         { NV_WCDMA_800_RX_CAL_CHAN_I ,
           NV_WCDMA_800_VGA_GAIN_OFFSET_I, 
           NV_WCDMA_800_VGA_GAIN_OFFSET_VS_FREQ_I },
         { NV_C1_WCDMA_800_RX_CAL_CHAN_I ,
           NV_C1_WCDMA_800_VGA_GAIN_OFFSET_I, 
           NV_C1_WCDMA_800_VGA_GAIN_OFFSET_VS_FREQ_I },
         { BSP_RF_RX_CHAN_WCDMA_BC5_MIN, CHAN_LIST_RANGE_VAL , 
           BSP_RF_RX_CHAN_WCDMA_BC5_MAX, CHAN_LIST_EOL_VAL} },
       
      { FTM_WCDMA_C, FTM_WCDMA_RX_2_C,           "\"WCDMA_900\"",  
                     FTM_PHONE_MODE_WCDMA_BC8 ,
                     BSP_RF_WCDMA_BC8_RX_TX_CHAN_NUMBER_OFFSET,
                     { NV_WCDMA_900_RX_CAL_CHAN_I, 
                       NV_WCDMA_900_VGA_GAIN_OFFSET_I, 
                       NV_WCDMA_900_VGA_GAIN_OFFSET_VS_FREQ_I },
                     { NV_C1_WCDMA_900_RX_CAL_CHAN_I ,
                       NV_C1_WCDMA_900_VGA_GAIN_OFFSET_I, 
                       NV_C1_WCDMA_900_VGA_GAIN_OFFSET_VS_FREQ_I },
                     { BSP_RF_RX_CHAN_WCDMA_BC8_MIN, CHAN_LIST_RANGE_VAL , 
                       BSP_RF_RX_CHAN_WCDMA_BC8_MAX, CHAN_LIST_EOL_VAL} },
      
       { FTM_WCDMA_C, FTM_WCDMA_RX_2_C,           "\"WCDMA_1900\"", 
         FTM_PHONE_MODE_WCDMA_1900A, 
         BSP_RF_WCDMA_BC2_RX_TX_CHAN_NUMBER_OFFSET,
         { NV_WCDMA_1900_RX_CAL_CHAN_I ,
           NV_WCDMA_1900_VGA_GAIN_OFFSET_I, 
           NV_WCDMA_1900_VGA_GAIN_OFFSET_VS_FREQ_I },
         { NV_C1_WCDMA_1900_RX_CAL_CHAN_I ,
           NV_C1_WCDMA_1900_VGA_GAIN_OFFSET_I, 
           NV_C1_WCDMA_1900_VGA_GAIN_OFFSET_VS_FREQ_I },
         { BSP_RF_RX_CHAN_WCDMA_BC2_MIN, CHAN_LIST_RANGE_VAL, 
           BSP_RF_RX_CHAN_WCDMA_BC2_MAX, CHAN_LIST_EOL_VAL } }
     };
          
      /* If arguments passed */ 
     if (tok_ptr->op == (NA|EQ|AR) )
     {
       int i;
       int chan;        
       
       if ((tok_ptr->args_found < 1) || (tok_ptr->args_found > 3))
       {
         return DSAT_ERROR;
       }
           
       if (ftm_get_mode() != FTM_MODE)
       {
         res_buff_ptr->used =
           (uint16)snprintf ((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                               "ERROR - restricted to FTM");
         return DSAT_CMD_ERR_RSP;  
       }
     
       /* Get and validate mode/band arg */
       band_label = NULL;
       for (i = 0; (!band_label) && (i < (int)ARR_SIZE(agc_band_labels)); i++)
       {
         if (!dsatutil_strcmp_ig_sp_case((const byte *)agc_band_labels[i].name, 
                                         tok_ptr->arg[0])) 
         {
           band_label = (band_label_type *)&agc_band_labels[i];
         }
       }
       if (!band_label)
       {
         res_buff_ptr->used = 
           (uint16)snprintf((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                                               "ERROR: Invalid <band>\n");
         return DSAT_CMD_ERR_RSP;
       }
       proc_qcagc.mode_band  = band_label->mode_band;
     
       /* Get chan and validate */
       if (dsatutil_atoi((dsat_num_item_type *)&chan, tok_ptr->arg[1], 10))
       {
         return DSAT_ERROR;
       }
     
       /* Verify chan value is in list */
       for (i = 0; (CHAN_LIST_EOL_VAL != band_label->chan_list[i])
             && (i < (CHAN_LIST_ENTRIES_MAX-2)); i++)
       {
         if ((int)chan >= band_label->chan_list[i])
         {
           if (chan == band_label->chan_list[i])
           {
             break;
           }
           if ((CHAN_LIST_EOL_VAL   != band_label->chan_list[i+2]) &&
               (CHAN_LIST_RANGE_VAL == band_label->chan_list[i+1]) &&
               ((int)chan <= band_label->chan_list[i+2]))
           {
             break;
           }
         }
       }
       if (CHAN_LIST_EOL_VAL == band_label->chan_list[i])
       {
         res_buff_ptr->used = 
           (uint16)snprintf((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                                               "ERROR: Invalid <chan>\n");
         return DSAT_CMD_ERR_RSP;
       }

       proc_qcagc.sys_mode.etsi.rx_chan = chan;
       proc_qcagc.sys_mode.etsi.tx_chan = proc_qcagc.sys_mode.etsi.rx_chan -
         band_label->rx_tx_chan_offset;
       
       /* If path specified, get and validate */
       proc_qcagc.ftm_dispatch_id     = band_label->ftm_dispatch_main_id;
       proc_qcagc.ftm_dispatch_div_id = band_label->ftm_dispatch_div_id;
       proc_qcagc.enable_rx_diversity = 0;
       
       if (tok_ptr->args_found == 3)
       {
         if (!dsatutil_strcmp_ig_sp_case("\"aux\"", tok_ptr->arg[2]))
         {
           proc_qcagc.enable_rx_diversity = 1;
         } 
         else if (dsatutil_strcmp_ig_sp_case("\"main\"", tok_ptr->arg[2]))
         {
           res_buff_ptr->used = 
             (uint16)snprintf((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                                                 "ERROR: Invalid <path>\n");
           return DSAT_CMD_ERR_RSP;
         }
       }
           
       proc_qcagc.lna = 0;
       
       /* Start FTM command sequence to get AGC via commands -
        *    Get NV Cal values for specific band
        *    FTM_SET_MODE
        *    FTM_SET_CHAN
        *    FTM_GET_SYNTH_STATE
        *    FTM_SET_LNA_RANGE
        *    FTM_GET_CAGC_RX_AGC
        */
       {
         ftm_pkt_type ftm_pkt;
         ftm_diagpkt_subsys_header_type *ftm_diag_hdr;
         nv_stat_enum_type nv_status;
         int missing_calibration = 0;
         int16 rx_cal_chan[NV_FREQ_TABLE_SIZ];
         
     
         /* Get NV Cal values for specified band / path */
         proc_qcagc.sys_mode.etsi.vga_gain_offset = 0;
         proc_qcagc.sys_mode.etsi.vga_vs_chan_freq = 0;
         missing_calibration = FALSE;
              
         nv_status = dsatutil_get_nv_item(
           proc_qcagc.enable_rx_diversity ?
           band_label->cal_id_div.cal_rx_chan :
           band_label->cal_id_main.cal_rx_chan, &ds_nv_item);
         if (nv_status == NV_DONE_S)
         {
         (void) dsatutil_memscpy((void*)rx_cal_chan,
           NV_FREQ_TABLE_SIZ,(void*)ds_nv_item.wcdma_rx_cal_chan,
                  sizeof(rx_cal_chan));
         }
         else
         {
           missing_calibration = TRUE;
         }
         
         nv_status = dsatutil_get_nv_item(
           proc_qcagc.enable_rx_diversity ?
           band_label->cal_id_div.vga_gain_offset :
           band_label->cal_id_main.vga_gain_offset, &ds_nv_item);
         if (nv_status == NV_DONE_S)
         {
           proc_qcagc.sys_mode.etsi.vga_gain_offset = ds_nv_item.wcdma_vga_gain_offset;
         }
         else
         {
           missing_calibration = TRUE;
         }
         
         /* Get the VGA offset based on channel frequency */
         nv_status = dsatutil_get_nv_item(
           proc_qcagc.enable_rx_diversity ?
           band_label->cal_id_div.vga_vs_freq :
           band_label->cal_id_main.vga_vs_freq, &ds_nv_item);
         if ((nv_status == NV_DONE_S) && !missing_calibration)
         {
           int16 *nv_chan_freq = &rx_cal_chan[0];
           for (i = 0; i < NV_FREQ_TABLE_SIZ; i++)
           {
             if (proc_qcagc.sys_mode.etsi.rx_chan <= nv_chan_freq[i])
             { 
               break;
             }
           }
           if (i == NV_FREQ_TABLE_SIZ)
           {
             i = NV_FREQ_TABLE_SIZ - 1;
           }
           proc_qcagc.sys_mode.etsi.vga_vs_chan_freq = 
             ds_nv_item.wcdma_vga_gain_offset_vs_freq[i];
         }
         else
         {
           missing_calibration = TRUE;
         }
         
         if (missing_calibration)
         {
           res_buff_ptr->used =
             (uint16)snprintf((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                                 "ERROR - Missing NV calibration values");
           return DSAT_CMD_ERR_RSP;  
         }
         
         /* Send FTM Mode / band selection */
         ftm_diag_hdr = (ftm_diagpkt_subsys_header_type *)&ftm_pkt.header;
         ftm_diag_hdr->subsys_cmd_code = (diagpkt_subsys_cmd_code_type)
         proc_qcagc.ftm_dispatch_id;
         ftm_pkt.rf_params.ftm_rf_cmd  = (word)FTM_SET_MODE;
         ftm_pkt.rf_params.ftm_rf_factory_data.mode = 
           proc_qcagc.mode_band;
         
         dsatvend_ftm_diagpkt_fwd(&ftm_pkt, 
                                  dsatvend_exec_qcagc_set_mode_cb,
                                  (void *)&proc_qcagc);

         SET_PENDING(DSAT_VENDOR_QCAGC_IDX ,0, DSAT_PENDING_QCAGC_SET_MODE)
         result = DSAT_ASYNC_CMD;
       }
     }
     else if (tok_ptr->op == (NA|EQ|QU) )
     {
       int i, j, buf_size;
       buf_size =  res_buff_ptr->size;
       res_buff_ptr->used = 
         (uint16)snprintf((char*)res_buff_ptr->data_ptr, buf_size,
                                             "AGC: <band>,<rx_chan>[,<path>]\n");
       
       for (i = 0; i < (int)ARR_SIZE(agc_band_labels); i++)
       {
         band_label =  &agc_band_labels[i];
         res_buff_ptr->used += 
           (uint16)snprintf((char*)res_buff_ptr->data_ptr + res_buff_ptr->used, 
           buf_size - res_buff_ptr->used,
           "  <band>,<rx_chan> %s,(", band_label->name);
         
         for (j = 0; (CHAN_LIST_EOL_VAL != band_label->chan_list[j]) 
               && (j < CHAN_LIST_ENTRIES_MAX); j++)
         {
           char str_buf[24];
           if (CHAN_LIST_RANGE_VAL == band_label->chan_list[j])
           {
             (void)snprintf(str_buf, sizeof(str_buf), "-");
           }
           else
           {
             if (!j || (CHAN_LIST_RANGE_VAL == band_label->chan_list[j-1]))
             {
               (void)snprintf(str_buf, sizeof(str_buf), "%d",
                                  band_label->chan_list[j]);
             }
             else
             {
               (void)snprintf(str_buf, sizeof(str_buf), ",%d",
                                  band_label->chan_list[j]);
             }
           }
           res_buff_ptr->used += 
             (uint16)snprintf((char*)res_buff_ptr->data_ptr + res_buff_ptr->used,
             buf_size - res_buff_ptr->used,
             "%s", str_buf);
           
         } /* End for (j = 0; CHAN_LIST_EOL_VAL  */
         
         res_buff_ptr->used += 
           (uint16)snprintf((char*)res_buff_ptr->data_ptr + res_buff_ptr->used,
           buf_size - res_buff_ptr->used,
           ")\n");
       } /* End for (i = 0; i < ARR_SIZE(agc_band_labels) */
       
       res_buff_ptr->used += 
         (uint16)snprintf((char*)res_buff_ptr->data_ptr + res_buff_ptr->used, 
         buf_size - res_buff_ptr->used,
         "  <path> (\"MAIN\",\"AUX\")\n");
       return DSAT_OK;
       
     } /* End else if (tok_ptr->op == (NA|EQ|QU) ) */
     else
     {
       result = DSAT_ERROR;
     }
   }
   return result;   
} /* dsatvend_exec_qcagc_cmd */

#ifdef FEATURE_WCDMA
/*============================================================================

FUNCTION DSATVEND_EXEC_QCALLUP_SET_PA_RANGE_CB

DESCRIPTION
   Process received FTM_SET_PA_RANGE response

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcallup_set_pa_range_cb
(
   ftm_pkt_type *ftm_pkt,
   void *rsp_arg
)
{
   dsatdvend_proc_qcallup_type *proc_qcallup = 
                      (dsatdvend_proc_qcallup_type *)rsp_arg;
   dsm_item_type *res_buff_ptr;
   
   res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
   res_buff_ptr->used = (word)snprintf((char*)res_buff_ptr->data_ptr,
                                             res_buff_ptr->size,
                                             "ALLUP: %s",
                                       proc_qcallup->tx_enable ?
                                          "ON" : "OFF");
   dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
   SET_PENDING(DSAT_VENDOR_QCALLUP_IDX ,0, DSAT_PENDING_QCALLUP_NONE)
   return DSAT_OK;
} /* dsatvend_exec_qcallup_set_pa_range_cb */

/*============================================================================

FUNCTION DSATVEND_EXEC_QCALLUP_SET_TX_CB

DESCRIPTION
   Process received FTM_SET_TX_ON or FTM_SET_TX_OFF response
   Send FTM_SET_PA_RANGE command.

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_ASYNC_EVENT : if it is a ASYNC EVENT

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcallup_set_tx_cb
(
   ftm_pkt_type *ftm_pkt,
   void *rsp_arg
)
{
   dsatdvend_proc_qcallup_type *proc_qcallup = 
                      (dsatdvend_proc_qcallup_type *)rsp_arg;
   
   ftm_diagpkt_subsys_header_type *ftm_diag_hdr;

   /* Send Get SYNTH state */
   ftm_diag_hdr = (ftm_diagpkt_subsys_header_type *)&ftm_pkt->header;
   ftm_diag_hdr->subsys_cmd_code = (diagpkt_subsys_cmd_code_type)
                                    proc_qcallup->ftm_dispatch_id;
 
   ftm_pkt->rf_params.ftm_rf_cmd  = (word)FTM_SET_PA_RANGE;
   ftm_pkt->rf_params.ftm_rf_factory_data.range = FTM_PA_R3;
 
   dsatvend_ftm_diagpkt_fwd(ftm_pkt, 
                             dsatvend_exec_qcallup_set_pa_range_cb,
                             (void *)proc_qcallup);
   SET_PENDING(DSAT_VENDOR_QCALLUP_IDX ,0, DSAT_PENDING_QCALLUP_SET_PA_RANGE)
   return DSAT_ASYNC_EVENT;
} /* dsatvend_exec_qcallup_set_tx_cb */

/*============================================================================

FUNCTION DSATVEND_EXEC_QCALLUP_SET_PDM_CB

DESCRIPTION
   Process received FTM_SET_PDM response
   Send FTM_SET_TX_ON or FTM_SET_TX_OFF

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_ASYNC_EVENT : if it is a ASYNC EVENT

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcallup_set_pdm_cb
(
   ftm_pkt_type *ftm_pkt,
   void *rsp_arg
)
{
   dsatdvend_proc_qcallup_type *proc_qcallup = 
                      (dsatdvend_proc_qcallup_type *)rsp_arg;
   
   ftm_diagpkt_subsys_header_type *ftm_diag_hdr;

   /* Send Get SYNTH state */
   ftm_diag_hdr = (ftm_diagpkt_subsys_header_type *)&ftm_pkt->header;
   ftm_diag_hdr->subsys_cmd_code = (diagpkt_subsys_cmd_code_type)
                                    proc_qcallup->ftm_dispatch_id;
 
   if (proc_qcallup->tx_enable)
   {
      ftm_pkt->rf_params.ftm_rf_cmd  = (word)FTM_SET_TX_ON;
   }
   else
   {
      ftm_pkt->rf_params.ftm_rf_cmd  = (word)FTM_SET_TX_OFF;
   }
 
   dsatvend_ftm_diagpkt_fwd(ftm_pkt, 
                             dsatvend_exec_qcallup_set_tx_cb,
                             (void *)proc_qcallup);
   SET_PENDING(DSAT_VENDOR_QCALLUP_IDX ,0, DSAT_PENDING_QCALLUP_SET_TX)
   return DSAT_ASYNC_EVENT;
} /* dsatvend_exec_qcallup_set_pdm_cb */

/*============================================================================

FUNCTION DSATVEND_EXEC_QCALLUP_SET_CW_WAVEFORM_CB

DESCRIPTION
   Process received FTM_SET_CDMA_CW_WAVEFORM response.
   Send FTM_SET_PDM command.

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_ASYNC_EVENT : if it is a ASYNC EVENT

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcallup_set_cw_waveform_cb
(
   ftm_pkt_type *ftm_pkt,
   void *rsp_arg
)
{
   dsatdvend_proc_qcallup_type *proc_qcallup = 
                      (dsatdvend_proc_qcallup_type *)rsp_arg;
   
   ftm_diagpkt_subsys_header_type *ftm_diag_hdr;

   /* Send Get SYNTH state */
   ftm_diag_hdr = (ftm_diagpkt_subsys_header_type *)&ftm_pkt->header;
   ftm_diag_hdr->subsys_cmd_code = (diagpkt_subsys_cmd_code_type)
                                    proc_qcallup->ftm_dispatch_id;
 
   ftm_pkt->rf_params.ftm_rf_cmd  = (word)FTM_SET_PDM;
   ftm_pkt->rf_params.ftm_rf_factory_data.id_val.id  = (word)FTM_PDM_TX_AGC_ADJ;
   ftm_pkt->rf_params.ftm_rf_factory_data.id_val.val = proc_qcallup->pdm;
   dsatvend_ftm_diagpkt_fwd(ftm_pkt, 
                             dsatvend_exec_qcallup_set_pdm_cb,
                             (void *)proc_qcallup);
   SET_PENDING(DSAT_VENDOR_QCALLUP_IDX ,0, DSAT_PENDING_QCALLUP_SET_PDM)
   return DSAT_ASYNC_EVENT;
  
} /* dsatvend_exec_qcallup_set_cw_waveform_cb */

/*============================================================================

FUNCTION DSATVEND_EXEC_QCALLUP_GET_SYNTH_CB

DESCRIPTION
   Process received FTM_GET_SYNTH_STATE response.
   Send FTM_SET_CDMA_CW_WAVEFORM command.

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_ASYNC_EVENT : if it is a ASYNC EVENT
    DSAT_CMD_ERR_RSP : if sending out error_response

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcallup_get_synth_cb
(
   ftm_pkt_type *ftm_pkt,
   void *rsp_arg
)
{
   dsatdvend_proc_qcallup_type *proc_qcallup = 
                      (dsatdvend_proc_qcallup_type *)rsp_arg;
   
   ftm_diagpkt_subsys_header_type *ftm_diag_hdr;

   /* Verify Lock state in synth */
   if (!ftm_pkt->rf_params.ftm_rf_factory_data.on_off)
   {
      dsm_item_type *res_buff_ptr;
      res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);
      res_buff_ptr->used = (word)snprintf((char*)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,
                                                "AGC:Synth not locked");
      dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
      SET_PENDING(DSAT_VENDOR_QCALLUP_IDX ,0, DSAT_PENDING_QCALLUP_NONE)
      return DSAT_CMD_ERR_RSP;
   }


   /* Send FTM_SET_CDMA_CW_WAVEFORM (0) */
   ftm_diag_hdr = (ftm_diagpkt_subsys_header_type *)&ftm_pkt->header;
   ftm_diag_hdr->subsys_cmd_code = (diagpkt_subsys_cmd_code_type)
                                    proc_qcallup->ftm_dispatch_id;

   ftm_pkt->rf_params.ftm_rf_cmd  = (word)FTM_SET_CDMA_CW_WAVEFORM;
   ftm_pkt->rf_params.ftm_rf_factory_data.on_off = 0;

   dsatvend_ftm_diagpkt_fwd(ftm_pkt, 
                             dsatvend_exec_qcallup_set_cw_waveform_cb,
                             (void *)proc_qcallup);
  
  SET_PENDING(DSAT_VENDOR_QCALLUP_IDX ,0, DSAT_PENDING_QCALLUP_SET_CW_WAVE)
   return DSAT_ASYNC_EVENT;
} /* dsatvend_exec_qcallup_get_synth_cb */

/*============================================================================

FUNCTION DSATVEND_EXEC_QCALLUP_SET_CHAN_CB

DESCRIPTION
   Process received FTM_SET_CHAN response.
   Send FTM_GET_SYNTH_STATE command.

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_ASYNC_EVENT : if it is a ASYNC EVENT

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcallup_set_chan_cb
(
   ftm_pkt_type *ftm_pkt,
   void *rsp_arg
)
{
   dsatdvend_proc_qcallup_type *proc_qcallup = 
                      (dsatdvend_proc_qcallup_type *)rsp_arg;
   
   ftm_diagpkt_subsys_header_type *ftm_diag_hdr;

   /* Send Get SYNTH state */
   ftm_diag_hdr = (ftm_diagpkt_subsys_header_type *)&ftm_pkt->header;
   ftm_diag_hdr->subsys_cmd_code = (diagpkt_subsys_cmd_code_type)
                                    proc_qcallup->ftm_dispatch_id;
 
   ftm_pkt->rf_params.ftm_rf_cmd  = (word)FTM_GET_SYNTH_STATE;
 
   dsatvend_ftm_diagpkt_fwd(ftm_pkt, 
                             dsatvend_exec_qcallup_get_synth_cb,
                             (void *)proc_qcallup);
  
   SET_PENDING(DSAT_VENDOR_QCALLUP_IDX ,0, DSAT_PENDING_QCAGC_GET_SYNTH)
   return DSAT_ASYNC_EVENT;
} /* dsatvend_exec_qcallup_set_chan_cb */

/*============================================================================

FUNCTION DSATVEND_EXEC_QCALLUP_SET_MODE_CB

DESCRIPTION
   Process received FTM_SET_MODE response.
   Send FTM_SET_CHAN command.

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_ASYNC_EVENT : if it is a ASYNC EVENT

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcallup_set_mode_cb
(
   ftm_pkt_type *ftm_pkt,
   void *rsp_arg
)
{
   dsatdvend_proc_qcallup_type *proc_qcallup = 
                      (dsatdvend_proc_qcallup_type *)rsp_arg;
   ftm_diagpkt_subsys_header_type *ftm_diag_hdr;

   /* Send channel selection */
   ftm_diag_hdr = (ftm_diagpkt_subsys_header_type *)&ftm_pkt->header;
   ftm_diag_hdr->subsys_cmd_code = (diagpkt_subsys_cmd_code_type)
                                    proc_qcallup->ftm_dispatch_id;

   ftm_pkt->rf_params.ftm_rf_cmd  = (word)FTM_SET_CHAN;
   ftm_pkt->rf_params.ftm_rf_factory_data.chan = proc_qcallup->tx_chan;

  
   dsatvend_ftm_diagpkt_fwd(ftm_pkt, 
                            dsatvend_exec_qcallup_set_chan_cb,
                            (void *)proc_qcallup);
   
   SET_PENDING(DSAT_VENDOR_QCALLUP_IDX ,0, DSAT_PENDING_QCALLUP_SET_CHAN)
   return DSAT_ASYNC_EVENT;
} /* dsatvend_exec_qcallup_set_mode_cb */

/*============================================================================

FUNCTION DSATVEND_EXEC_QCALLUP_CMD

DESCRIPTION
  Turn RF transmitter on / off for specified band tuned to the first channel
   
  AT$QCALLUP <band>,<tx_chan>[,on|off]
  
   band -
      WCDMA_IMT     
      WCDMA_800    (Cell)
      WCDMA_1900   (PCS)
   
  Note: Restricted to FTM

DEPENDENCIES
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
                        
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution
    DSAT_ASYNC_EVENT : if it is a ASYNC EVENT
    DSAT_CMD_ERR_RSP : if sending out error_response

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_qcallup_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
   dsat_result_enum_type result = DSAT_OK;
   
   static dsatdvend_proc_qcallup_type proc_qcallup;
   
   typedef struct tx_band_label_s {
      int              ftm_dispatch_id;
      char            *name;
      ftm_mode_id_type mode_band;
      int              chan_list[20];
   #define  CHAN_LIST_RANGE_VAL -1 /* denotes inclusive range pair */
   #define  CHAN_LIST_EOL_VAL   -2 /* end of list */
   } tx_band_label_type;

   tx_band_label_type *band_label;
   
   /* Supported Mode/Band labels with associated FTM and NV defs */
   tx_band_label_type allup_band_labels[] = {
      { FTM_WCDMA_C,                             "\"WCDMA_IMT\"",  
                     FTM_PHONE_MODE_WCDMA_IMT,
                     { BSP_RF_TX_CHAN_WCDMA_BC1_MIN, CHAN_LIST_RANGE_VAL, 
                       BSP_RF_TX_CHAN_WCDMA_BC1_MAX, CHAN_LIST_EOL_VAL} },
      
      { FTM_WCDMA_C,                             "\"WCDMA_800\"",  
                     FTM_PHONE_MODE_WCDMA_800 ,
                     { BSP_RF_TX_CHAN_WCDMA_BC5_MIN, CHAN_LIST_RANGE_VAL , 
                       BSP_RF_TX_CHAN_WCDMA_BC5_MAX, CHAN_LIST_EOL_VAL} },
      
      { FTM_WCDMA_C,                             "\"WCDMA_1900\"", 
                     FTM_PHONE_MODE_WCDMA_1900A, 
                     { BSP_RF_TX_CHAN_WCDMA_BC2_MIN, CHAN_LIST_RANGE_VAL, 
                       BSP_RF_TX_CHAN_WCDMA_BC2_MAX, CHAN_LIST_EOL_VAL } }
     };
     
   /* If arguments passed */
   if (tok_ptr->op == (NA|EQ|AR))
   {
      int i;
      int chan;        
      
      if ((tok_ptr->args_found < 1) || (tok_ptr->args_found > 4))
      {
         return DSAT_ERROR;
      }
      
      if (ftm_get_mode() != FTM_MODE)
      {
         res_buff_ptr->used =
            (uint16)snprintf ((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                           "ERROR - restricted to FTM");
         return DSAT_CMD_ERR_RSP;  
      }

      /* Get and validate mode/band arg */
      band_label = NULL;
      for (i = 0; (!band_label) && (i < (int)ARR_SIZE(allup_band_labels)); i++)
      {
         if (!dsatutil_strcmp_ig_sp_case((const byte *)allup_band_labels[i].name, 
                                         tok_ptr->arg[0]))
         {
            band_label = &allup_band_labels[i];
         }
      }
      if (!band_label)
      {
         res_buff_ptr->used = 
           (uint16)snprintf((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                            "ERROR: Invalid <band>\n");
         return DSAT_CMD_ERR_RSP;
      }
      proc_qcallup.mode_band       = band_label->mode_band;
      proc_qcallup.ftm_dispatch_id = band_label->ftm_dispatch_id;
      proc_qcallup.tx_enable       = 1;

      /* Get chan and validate */
      if (dsatutil_atoi((dsat_num_item_type *)&chan, tok_ptr->arg[1], 10))
      {
         return DSAT_ERROR;
      }

      /* Verify chan value is in list */
      for (i = 0; CHAN_LIST_EOL_VAL != band_label->chan_list[i]; i++)
      {
         if (chan >= band_label->chan_list[i])
         {
            if (chan == band_label->chan_list[i])
            {
               break;
            }
            if ((CHAN_LIST_EOL_VAL   != band_label->chan_list[i+2]) &&
                (CHAN_LIST_RANGE_VAL == band_label->chan_list[i+1]) &&
               (chan <= band_label->chan_list[i+2]))
            {
               break;
            }
         }
      }
      if (CHAN_LIST_EOL_VAL == band_label->chan_list[i])
      {
         res_buff_ptr->used = 
           (uint16)snprintf((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                            "ERROR: Invalid <chan>\n");
         return DSAT_CMD_ERR_RSP;
      }
      proc_qcallup.tx_chan = chan;
      
      /* Check for optional arg "on" or "off" */
      if (tok_ptr->args_found > 2)
      {
         if (!dsatutil_strcmp_ig_sp_case("\"off\"", tok_ptr->arg[2]))
         {
            proc_qcallup.tx_enable = 0;
         } 
         else if (dsatutil_strcmp_ig_sp_case("\"on\"", tok_ptr->arg[2]))
         {
            res_buff_ptr->used =
              (uint16)snprintf((char*)res_buff_ptr->data_ptr, res_buff_ptr->size,
                            "ERROR: Invalid arg for \"on\"|\"off\"\n");
            return DSAT_CMD_ERR_RSP;
         }
      }
      
      /* Check for optional pdm value */
      proc_qcallup.pdm = 200;
      if (tok_ptr->args_found > 3)
      {
         if (dsatutil_atoi((dsat_num_item_type *)&proc_qcallup.pdm, tok_ptr->arg[3], 10))
         {
            return DSAT_ERROR;
         }
      }
      
      /* Start FTM command sequence to get AGC via commands -
       *    FTM_SET_MODE
       *    FTM_SET_CHAN
       *    FTM_GET_SYNTH_STATE
       *    FTM_SET_CDMA_CW_WAVEFORM (0)
       *    FTM_SET_PDM      (FTM_PDM_TX_AGC_ADJ, val)
       *    FTM_SET_TX_ON or FTM_SET_TX_OFF
       *    FTM_SET_PA_RANGE (3)
       */
      {
         ftm_pkt_type ftm_pkt;
         ftm_diagpkt_subsys_header_type *ftm_diag_hdr;

         /* Send FTM Mode / band selection */
         ftm_diag_hdr = (ftm_diagpkt_subsys_header_type *)&ftm_pkt.header;
         ftm_diag_hdr->subsys_cmd_code = (diagpkt_subsys_cmd_code_type)
                                          proc_qcallup.ftm_dispatch_id;
         ftm_pkt.rf_params.ftm_rf_cmd  = (word)FTM_SET_MODE;
         ftm_pkt.rf_params.ftm_rf_factory_data.mode = 
                                           proc_qcallup.mode_band;
         
         dsatvend_ftm_diagpkt_fwd(&ftm_pkt, 
                                  dsatvend_exec_qcallup_set_mode_cb,
                                   (void *)&proc_qcallup);
         SET_PENDING(DSAT_VENDOR_QCALLUP_IDX ,0, DSAT_PENDING_QCALLUP_SET_MODE)
         result = DSAT_ASYNC_CMD;
      }
   }
   else if (tok_ptr->op == (NA|EQ|QU) )
   {
      int i, j, buf_size;
      buf_size =  res_buff_ptr->size;
      res_buff_ptr->used = 
        (uint16)snprintf((char*)res_buff_ptr->data_ptr, buf_size,
                               "ALLUP: <band>,<tx_chan>[,\"on\"|\"off\"[,<pdm>]]\n");
      

      for (i = 0; i < (int)ARR_SIZE(allup_band_labels); i++)
      {
         band_label =  &allup_band_labels[i];
         res_buff_ptr->used += 
           (uint16)snprintf((char*)res_buff_ptr->data_ptr + res_buff_ptr->used, 
                             buf_size - res_buff_ptr->used,
                             "  <band>,<tx_chan> %s,(", band_label->name);
         
         for (j = 0; CHAN_LIST_EOL_VAL != band_label->chan_list[j]; j++)
         {
            char str_buf[24];
            if (CHAN_LIST_RANGE_VAL == band_label->chan_list[j])
            {
               (void)snprintf(str_buf, sizeof(str_buf), "-");
            }
            else
            {
               if (!j || (CHAN_LIST_RANGE_VAL == band_label->chan_list[j-1]))
               {
                  (void)snprintf(str_buf, sizeof(str_buf), "%d",
                                 band_label->chan_list[j]);
               }
               else 
               {
                  (void)snprintf(str_buf, sizeof(str_buf), ",%d",
                                 band_label->chan_list[j]);
               }
            }
            res_buff_ptr->used += 
              (uint16)snprintf((char*)res_buff_ptr->data_ptr + res_buff_ptr->used,
                                 buf_size - res_buff_ptr->used,
                                 "%s", str_buf);
            
         } /* End for (j = 0; CHAN_LIST_EOL_VAL */
         
         res_buff_ptr->used += 
           (uint16)snprintf((char*)res_buff_ptr->data_ptr + res_buff_ptr->used,
                             buf_size - res_buff_ptr->used,
                             ")\n");
      } /* End for (i = 0; i < ARR_SIZE(allup_band_labels) */
      
      res_buff_ptr->used += 
        (uint16)snprintf((char*)res_buff_ptr->data_ptr + res_buff_ptr->used,
                          buf_size - res_buff_ptr->used,
                          "  \"on\"|\"off\" Turns Tx on or off [\"on\" if not specified]\n");
      res_buff_ptr->used += 
        (uint16)snprintf((char*)res_buff_ptr->data_ptr + res_buff_ptr->used,
                          buf_size - res_buff_ptr->used,
                          "   <pdm> optional TX PDM AGC value [200 if not specified]");
      
      return DSAT_OK;
   } /* End else if (tok_ptr->op == (NA|EQ|QU) ) */
   else
   {
     result = DSAT_ERROR;
   }
   return result;
} /* dsatvend_exec_qcallup_cmd */
#endif /* FEATURE_WCDMA */

/*===========================================================================

FUNCTION dsatvend_exec_qcsku_cmd

DESCRIPTION
  This function returns the SKU info

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcsku_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /*-------------------------------------------------------
  command with no arguments will return the info string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
     nv_stat_enum_type nv_status;

     memset(&ds_nv_item, 0, sizeof(ds_nv_item));
     nv_status = dsatutil_get_nv_item( NV_FACTORY_DATA_4_I , &ds_nv_item );

     if ( nv_status == NV_DONE_S )
     {
       res_buff_ptr->used = strlcpy((char*)res_buff_ptr->data_ptr, 
                          (const char*)ds_nv_item.factory_data_4,
                          res_buff_ptr->size);
     }
     else
     {
       res_buff_ptr->used =
         (word)snprintf ((char*)res_buff_ptr->data_ptr, 12,
                               "SKU Not Set");
     }
  }
  else
  {
     result = DSAT_ERROR;
  }
  return result;
} /* dsatvend_exec_qcsku_cmd */
#endif /* FEATURE_DSAT_GOBI_MAINLINE */
#ifdef FEATURE_ECALL_APP
/*===========================================================================
FUNCTION  DSAT_ECALL_CB_FUNC

DESCRIPTION
  ECALL status command callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Adds command in DS command buffer

===========================================================================*/
/* ARGSUSED */
LOCAL void dsat_ecall_cb_func 
(
  ecall_session_status_type  session_status 
)
{
  ds_cmd_type            *cmd_buf  = NULL;
  ds_at_ecall_cmd_s_type *ecall_cmd_ptr = NULL;
  
  cmd_buf = dsat_get_cmd_buf(sizeof(ds_at_ecall_cmd_s_type), FALSE);
  ecall_cmd_ptr = cmd_buf->cmd_payload_ptr;

  /* send the message to ATCOP */
  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_ECALL_CMD;
  ecall_cmd_ptr->session_status = session_status;
  ds_put_cmd(cmd_buf);
}/*dsat_ecall_cb_func*/

/*===========================================================================
FUNCTION DSATVEND_ECALL_AT_CMD_HANDLER

DESCRIPTION
  This function is handler function for reporting ecall session response.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ASYNC_EVENT: Always Returns ASYNC_EVENT.
 
SIDE EFFECTS
  None
  
======================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_ecall_at_cmd_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type      result       = DSAT_ERROR;
  dsat_num_item_type         cecall_type  = DSAT_CECALL_MAX;
  dsm_item_type            * res_buff_ptr = NULL;
  ds_at_ecall_cmd_s_type   * ecall_cmd_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);
  
  if(!dsatcmdp_is_etsi_cmd())
  {
    return DSAT_ASYNC_EVENT;
  }

  DS_AT_MSG3_HIGH("Response for Ecall  %d call type %d Active type %d",
                  ecall_cmd_ptr->session_status,
                  ecall_cmd_ptr->type_of_call,
                  ecall_cmd_ptr->activation_type);

  if(CHECK_PENDING(DSATETSI_EXT_CECALL_IDX,0,DSAT_PENDING_CECALL_WRITE))
  {
    if(ecall_cmd_ptr->session_status == ECALL_FAILURE)
    {
      result = DSAT_NO_CARRIER;
    }
    else
    {
      result = DSAT_OK;
    }
    
    SET_PENDING(DSATETSI_EXT_CECALL_IDX ,0, DSAT_PENDING_CECALL_NONE)
    
  }
  else if (CHECK_PENDING(DSATETSI_EXT_CECALL_IDX,0,DSAT_PENDING_CECALL_READ))
  {
    SET_PENDING(DSATETSI_EXT_CECALL_IDX ,0, DSAT_PENDING_CECALL_NONE)
    if(ecall_cmd_ptr->type_of_call == ECALL_NO_ACTIVE_CALL)
    {
      result = DSAT_OK;
    }
    else
    {
       if((ecall_cmd_ptr->type_of_call == ECALL_TEST) &&  
             (ecall_cmd_ptr->activation_type == ECALL_MANUAL_INITIATED))
       {
         cecall_type = DSAT_CECALL_TEST;
         result = DSAT_OK;
       }
       else if((ecall_cmd_ptr->type_of_call == ECALL_RECONFIG) &&  
                  (ecall_cmd_ptr->activation_type == ECALL_MANUAL_INITIATED))
       {
         cecall_type = DSAT_CECALL_RECONF;
         result = DSAT_OK;
       }
       else if((ecall_cmd_ptr->type_of_call == ECALL_EMERGENCY )&&  
                 (ecall_cmd_ptr->activation_type == ECALL_MANUAL_INITIATED))
       {  
      
         cecall_type = DSAT_CECALL_MANUAL;
         result = DSAT_OK;
       }
       else if((ecall_cmd_ptr->type_of_call ==  ECALL_EMERGENCY) &&  
                (ecall_cmd_ptr->activation_type == ECALL_AUTOMATIC_INITIATED))
       {
         cecall_type = DSAT_CECALL_AUTO;
         result = DSAT_OK;
       }
       else
       {
         result = DSAT_ERROR;
       }

      if(result == DSAT_OK)
      {
        res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
        res_buff_ptr->used = (word)snprintf ( (char*)res_buff_ptr->data_ptr,
                                                     res_buff_ptr->size,
                                                     "+CECALL: %d",
                                                     cecall_type);
        dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
      
      }
    }
  }
  else
  {
    if(ecall_cmd_ptr->session_status == ECALL_FAILURE)
    {
      dsatrsp_send_cell_result(DSAT_NO_CARRIER);
    }
    result = DSAT_ASYNC_EVENT;
  }
  return result;
}/*dsatvend_ecall_at_cmd_handler*/

/*===========================================================================

FUNCTION DSAVEND_EXEC_ECALL_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$ECALL command.
  This command provides the ability to make a eCall to the network. 

dsat_ecall_val[0]  -> Start/Stop the eCall 0 - Stop ; 1 - Start
dsat_ecall_val[1]  -> Type of the eCall    0 - Test ; 1 - Emergency
dsat_ecall_val[2]  -> Activation type      0 - Manual; 1- Automatic

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_exec_ecall_cmd 
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
#define START_ECALL 1
  dsat_result_enum_type result = DSAT_ERROR;
  dsat_num_item_type dsat_prev_ecall_val[3];
  dsati_operating_cmd_mode_type oprt_mode = dsatcmdp_get_operating_cmd_mode();

  if(oprt_mode != ETSI_CMD_MODE)
  {
    DS_AT_MSG0_ERROR("Invalid Operating Mode");
    return result;
  }

  if( (tok_ptr->op == (NA|EQ|AR)) ||
      (tok_ptr->op == (NA|EQ|QU)) ||
      (tok_ptr->op == (NA|QU)))
  {
    dsat_prev_ecall_val[0] = (dsat_num_item_type)dsatutil_get_val(
        DSAT_VENDOR_ECALL_IDX,0,0,NUM_TYPE);
    dsat_prev_ecall_val[1] = (dsat_num_item_type)dsatutil_get_val(
        DSAT_VENDOR_ECALL_IDX,0,1,NUM_TYPE);
    dsat_prev_ecall_val[2] =(dsat_num_item_type)dsatutil_get_val(
        DSAT_VENDOR_ECALL_IDX,0,2,NUM_TYPE);

    /* First argument is mandatory for an Action command.
       If omitted throw an Error */
    if(((tok_ptr->op == (NA|EQ|AR))) && 
       ( !VALID_TOKEN(0) && (tok_ptr->args_found > 1) ))
    {
      DS_AT_MSG0_ERROR("Invalid params");
      return result;
    }
    result = dsatparm_exec_param_cmd ( mode,
                                        parse_table,
                                        tok_ptr,
                                        res_buff_ptr
                                      );
    if((result == DSAT_OK)&&
       (tok_ptr->op == (NA|EQ|AR)))
    {
      if((dsat_num_item_type)dsatutil_get_val(
                   DSAT_VENDOR_ECALL_IDX,0,0,NUM_TYPE) == START_ECALL)
      {
        if( FALSE == ecall_session_start((ecall_type_of_call)(dsat_num_item_type)dsatutil_get_val(
                                         DSAT_VENDOR_ECALL_IDX,0,1,NUM_TYPE),
                                         (ecall_activation_type)(dsat_num_item_type)dsatutil_get_val(
                                         DSAT_VENDOR_ECALL_IDX,0,2,NUM_TYPE),
                                         dsat_ecall_cb_func))
        {
          DS_AT_MSG0_ERROR("Unable to start the Ecall");

          DSATUTIL_SET_VAL(DSAT_VENDOR_ECALL_IDX,0,0,0,dsat_prev_ecall_val[0],NUM_TYPE)
          DSATUTIL_SET_VAL(DSAT_VENDOR_ECALL_IDX,0,1,0,dsat_prev_ecall_val[1],NUM_TYPE)
          DSATUTIL_SET_VAL(DSAT_VENDOR_ECALL_IDX,0,2,0,dsat_prev_ecall_val[2],NUM_TYPE)
  
          result = DSAT_ERROR;
        }
      }
      else
      {
        if( FALSE == ecall_session_stop( ))
        {
          DS_AT_MSG0_ERROR("Unable to stop the Ecall");

          DSATUTIL_SET_VAL(DSAT_VENDOR_ECALL_IDX,0,0,0,dsat_prev_ecall_val[0],NUM_TYPE)
          DSATUTIL_SET_VAL(DSAT_VENDOR_ECALL_IDX,0,1,0,dsat_prev_ecall_val[1],NUM_TYPE)
          DSATUTIL_SET_VAL(DSAT_VENDOR_ECALL_IDX,0,2,0,dsat_prev_ecall_val[2],NUM_TYPE)

  
          result = DSAT_ERROR;
        }
      }

    }
  }
  return result;
}/*dsatvend_exec_ecall_cmd*/
#endif /* FEATURE_ECALL_APP */
/*===========================================================================

FUNCTION dsatvend_exec_qcsku_cmd

DESCRIPTION
  This function returns the SKU info

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcsku_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /*-------------------------------------------------------
  command with no arguments will return the info string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
     nv_stat_enum_type nv_status;

     memset(&ds_nv_item, 0, sizeof(ds_nv_item));
     nv_status = dsatutil_get_nv_item( NV_FACTORY_DATA_4_I , &ds_nv_item );

     if ( nv_status == NV_DONE_S )
     {
       res_buff_ptr->used = strlcpy((char*)res_buff_ptr->data_ptr, 
                          (const char*)ds_nv_item.factory_data_4,
                          res_buff_ptr->size);
     }
     else
     {
       res_buff_ptr->used =
         (word)snprintf ((char*)res_buff_ptr->data_ptr, 12,
                               "SKU Not Set");
     }
  }
  else
  {
     result = DSAT_ERROR;
  }
  return result;
} /* dsatvend_exec_qcsku_cmd */

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCGSN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +GSN command.
  It reads the required NV items (ESN/MEID) and prints them out.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_OK          : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcgsn_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  /*-------------------------------------------------------
  command with no arguments will return the info string
  ---------------------------------------------------------*/
  if (tok_ptr->op == NA)
  {
     nv_stat_enum_type nv_status;

     nv_status = dsatutil_get_nv_item( NV_MEID_I , &ds_nv_item );

     /* If MEID is not set, then report ESN. */
     if (( nv_status != NV_DONE_S ) || 
         ((0 == qw_hi(ds_nv_item.meid)) && (0 == qw_lo(ds_nv_item.meid)))) 
     {
        
        /* ESN Reporting */
        nv_status = dsatutil_get_nv_item (NV_ESN_I, &ds_nv_item);
        if ((nv_status == NV_DONE_S) && 
            (0 != ds_nv_item.esn.esn)) 
        {
          res_buff_ptr->used = (word)snprintf(
                                              (char*)res_buff_ptr->data_ptr,
                                              res_buff_ptr->size,
                                              "0x%08X",(unsigned int) ds_nv_item.esn.esn);
        }
        else 
        {
           res_buff_ptr->used = (word)snprintf(
                                              (char*)res_buff_ptr->data_ptr,
                                               res_buff_ptr->size,
                                               "0x0");
        }
     }

     /* Else report MEID
     */
     else
     {
#if defined(FEATURE_DSAT_EXTENDED_CMD)
       if ( (!strncmp((const char *)parse_table->name,(const char *)"^MEID",strlen((const char *)"^MEID"))) || 
            (!strncmp((const char *)parse_table->name,(const char *)"^GSN",strlen((const char *)"^GSN"))) )
       {
         res_buff_ptr->used = (word)snprintf(
                                              (char*)res_buff_ptr->data_ptr,
                                              res_buff_ptr->size,
                                              "0x%06X%08X", 
                                              (unsigned int)qw_hi(ds_nv_item.meid),
                                              (unsigned int)qw_lo(ds_nv_item.meid));
       }
       else
#endif /* defined(FEATURE_DSAT_EXTENDED_CMD) */
       {
       
          res_buff_ptr->used = (word)snprintf(
                                              (char*)res_buff_ptr->data_ptr,
                                              res_buff_ptr->size,
                                              "0x%08X%08X", 
                                              (unsigned int)qw_hi(ds_nv_item.meid),
                                              (unsigned int)qw_lo(ds_nv_item.meid));
       }
     }
  }
  /*-------------------------------------------------------
  General commands with a query should produce no response
  ---------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /* this command is valid, but do nothing */
    result = DSAT_OK;
  }  
  else
  {
     result = DSAT_ERROR;
  }
  return result;
}/* dsatvend_exec_qcgsn_cmd */

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCCLAC_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $QCCLAC command.
  It reads all the supported AT commands and prints them.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :         if it is a success and execution completed.
SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qcclac_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
)
{
  /* process command */
  if ((tok_ptr->op == NA) | (tok_ptr->op == (NA|QU)))
  {
    unsigned int i=0;
    char  buffer[MAX_CMD_SIZE];
    word size = 0;

    for (i=0;i<dsat_basic_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsat_basic_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }

    for (i=0;i<dsat_basic_action_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsat_basic_action_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }

    for (i=0;i<dsat_sreg_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsat_sreg_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }

    for (i=0;i<dsat_ext_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsat_ext_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }

#ifdef FEATURE_DATA_IS707
    for (i=0;i<dsat707_ext_para_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsat707_ext_para_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }

    for (i=0;i<dsat707_ext_action_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsat707_ext_action_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }

#ifdef FEATURE_DATA_IS707_ASYNC
    for (i=0;i<dsat707_fax_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsat707_fax_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }
#endif /* FEATURE_DATA_IS707_ASYNC */    
#endif /* FEATURE_DATA_IS707 */

    for (i=0;i<dsat_vendor_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsat_vendor_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }

#ifdef FEATURE_DATA_IS707

    for (i=0;i<dsat707_vend_para_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsat707_vend_para_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }

    for (i=0;i<dsat707_vend_action_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsat707_vend_action_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }

#ifdef FEATURE_HDR_AT_TEST_ONLY
    for (i=0;i<dsat707_hdr_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsat707_hdr_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }
#endif /* FEATURE_HDR_AT_TEST_ONLY */

#ifdef FEATURE_DS_MOBILE_IP

    for (i=0;i<dsat707_mip_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsat707_mip_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DS_PSTATS
    for (i=0;i<dsat707_pstats_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsat707_pstats_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }
#endif /* FEATURE_DS_PSTATS */

#ifdef FEATURE_CDMA_SMS
    for (i=0;i<dsat707_sms_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsat707_sms_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }
#endif /* FEATURE_CDMA_SMS */
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_DSAT_ETSI_MODE
    for (i=0;i<dsatetsi_ext_action_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsatetsi_ext_action_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }

    for (i=0;i<dsatetsi_ext_table_size;i++)
    {
      size = (word)snprintf(&(buffer[0]),
                             MAX_CMD_SIZE,
                             "%s\n",
                             dsatetsi_ext_table[i].name);

      dsat_dsm_pushdown_tail(&res_buff_ptr, buffer, size,
                             DSM_ITEM_POOL(res_buff_ptr), FALSE);
    }
#endif /* FEATURE_DSAT_ETSI_MODE */

    return DSAT_OK;
  }
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    return DSAT_OK;
  }
  else
  {
    return DSAT_ERROR;
  }
} /* dsatvend_exec_qcclac_cmd */

/*===========================================================================
FUNCTION   DSATVEND_IFACE_EV_CB()

DESCRIPTION
  This callback function is called when one of the registered ps_iface events
  occur on external interface.  COMING UP, DOWN and CONFIG iface events are
  registered, and affect the DUN call info message and indication behaviour.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void dsatvend_rmnet_ev_cb
(
   rmnet_qcrmcall_action_e_type  action,
   rmnet_qcrmcall_result_type  * result
)
{
  ds_cmd_type            *cmd_buf     = NULL;
  dsat_rmnet_info_s_type *rmnet_info_ptr = NULL;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  DS_AT_MSG1_HIGH("Received RmNet Action Event [%d]", action);

  switch( action )
  {
    case QCRMCALL_ACTION_START:
    case QCRMCALL_ACTION_STOP:
    case QCRMCALL_ACTION_QUERY:
    {
      cmd_buf = dsat_get_cmd_buf(sizeof(dsat_rmnet_info_s_type), FALSE);

      rmnet_info_ptr = cmd_buf->cmd_payload_ptr;
      rmnet_info_ptr->action = (uint8) action;
      dsatutil_memscpy(&rmnet_info_ptr->result,
                       sizeof(rmnet_qcrmcall_result_type),
                       result,
                       sizeof(rmnet_qcrmcall_result_type));

      /* send the message to ATCOP */
      cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_RMNET_EV_CMD;
        
      ds_put_cmd(cmd_buf);
      break;
    }
    case QCRMCALL_ACTION_NONE:
    default:
    {
      DS_AT_MSG1_ERROR("Unregistered event %d recvd, ignoring", action);
      break;
    }
  } /* switch( event ) */
} /* dsatvend_rmnet_ev_cb() */

/*===========================================================================
FUNCTION   DSATVEND_IFACE_EV_HANDLER()

DESCRIPTION
 This is a handler function for all the IFACE releated events. ATCoP is intrested in IFACE COMING UP, DOWN,
 CONFIGURE events. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatvend_rmnet_ev_handler
(
  ds_cmd_type * cmd_ptr     /* DS Command pointer */
)
{

  dsat_result_enum_type result = DSAT_OK;
  rmnet_qcrmcall_result_type  * result_rmnet = NULL;
  rmnet_qcrmcall_action_e_type  action;
  dsm_item_type *res_buff_ptr = NULL;           /*  Place to put response       */
  dsat_rmnet_info_s_type *rmnet_info_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);
  boolean status        = FALSE;
  uint8 qcrm_inst       = 0;
  uint8 ip_type         = 0;
  dsat_num_item_type  i = 0;

  result_rmnet = (rmnet_qcrmcall_result_type  *)  &rmnet_info_ptr->result;
  action       = (rmnet_qcrmcall_action_e_type)   rmnet_info_ptr->action;

  status    = result_rmnet->sni.status;
  qcrm_inst = result_rmnet->sni.qcrmcall_inst;
  ip_type   = result_rmnet->sni.ip_family;

  DS_AT_MSG1_MED("Recvd action cmd :  %d", action);

  DS_AT_MSG3_MED("QCRMCALL status:%d , inst: %d, ip_type: %d", status,qcrm_inst, ip_type);
  
  switch (action)
  {
     case QCRMCALL_ACTION_QUERY: 

       dsat_qcrmcall_info.state        =  DSAT_PENDING_QCRMCALL_NONE; /*Reset Pending call state*/
       
       if (result_rmnet->list.num_qcrmcall == 0)
       {
          result = DSAT_OK;
       }
       else if(result_rmnet->list.num_qcrmcall > RMNET_QCRMCALL_MAX)
       {         
         result = DSAT_ERROR;
       }
       else
       {
         res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, FALSE);

         for (i = 0; i < result_rmnet->list.num_qcrmcall; i++)
         {
           qcrm_inst = result_rmnet->list.qcrmcall[i].qcrmcall_inst;
           ip_type = result_rmnet->list.qcrmcall[i].ip_family;
           
           res_buff_ptr->used += (word)snprintf(
                              (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                              res_buff_ptr->size -  res_buff_ptr->used,
                              "$QCRMCALL: %d",(int)(qcrm_inst+1));

           if (ip_type == 4)
           {
             res_buff_ptr->used += (word)snprintf(
                                (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                                res_buff_ptr->size -  res_buff_ptr->used,
                                ", V4\n");
           }
           else if (ip_type == 6)
           {
             res_buff_ptr->used += (word)snprintf(
                                   (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                                   res_buff_ptr->size -  res_buff_ptr->used,
                                   ", V6\n");
           }
           else
           {
             DS_AT_MSG1_ERROR("Invalid Data with IP_Family: %d", ip_type);
           }
         }
         (void)dsatcmdp_send_respose_to_te(res_buff_ptr, DSAT_COMPLETE_RSP);
         
       }
       break;
     case   QCRMCALL_ACTION_STOP:

       switch(ip_type)
       {
         case QMI_AF_INET:
          {
            if (status)
            {
              /*Increment instance count as we output Rmnet_inst + 1*/
              dsat_qcrmcall_info.v4_instance =  qcrm_inst+1;
            }
            else
            {
              /*Reset V4 instance in case of failure*/
              dsat_qcrmcall_info.v4_instance = 0;
            }
            /*Reset pending stop state for IPV4*/
            dsat_qcrmcall_info.state &= ~DSAT_PENDING_QCRMCALL_STOP_V4;
          }
          break;
          case QMI_AF_INET6:
            {
              if (status)
              {
                dsat_qcrmcall_info.v6_instance =  qcrm_inst+1;
              }
              else
              {
                dsat_qcrmcall_info.v6_instance = 0;
              }
              /*Reset pending stop state for IPV6*/
              dsat_qcrmcall_info.state       &= ~DSAT_PENDING_QCRMCALL_STOP_V6;
           }
          break;
          default:
            {
              memset(&dsat_qcrmcall_info,0x0,sizeof(dsat_qcrmcall_info));
            }
            break;
       }

       /*Return error as we recieved invalid status (call failures) from RmNet*/
       if(dsat_qcrmcall_info.state ==  DSAT_PENDING_QCRMCALL_NONE)
       {
         if(dsat_qcrmcall_info.v4_instance == 0 &&
            dsat_qcrmcall_info.v6_instance == 0)
         {
           result = DSAT_ERROR;
         }
         else
         {
           memset(&dsat_qcrmcall_info,0x0,sizeof(dsat_qcrmcall_info));
           result = DSAT_OK;
         }
       }
       else
       {
         result = DSAT_ASYNC_CMD;
       }
       break;

     case QCRMCALL_ACTION_START:
       switch(ip_type)
       {
         case QMI_AF_INET:
          {
            if (status)
            {
              dsat_qcrmcall_info.v4_instance =  qcrm_inst + 1;
            }
            else
            {
              dsat_qcrmcall_info.v4_instance = 0;
            }
            /*Reset pending start state for IPV4*/
            dsat_qcrmcall_info.state       &= ~DSAT_PENDING_QCRMCALL_START_V4;
            break;
          }
          case QMI_AF_INET6:
            {
              if (status)
              {
                dsat_qcrmcall_info.v6_instance =  qcrm_inst + 1;
              }
              else
              {
                dsat_qcrmcall_info.v6_instance = 0;
              }

              /*Reset pending start state for IPV6*/
              dsat_qcrmcall_info.state       &= ~DSAT_PENDING_QCRMCALL_START_V6;
              break;
            }

          default:
            memset(&dsat_qcrmcall_info,0x0,sizeof(dsat_qcrmcall_info));
            DS_AT_MSG1_ERROR("Invalid Data with IP_Family: %d", ip_type);
          break;
       }
       if(dsat_qcrmcall_info.state ==  DSAT_PENDING_QCRMCALL_NONE)
       {
         res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);

         if (dsat_qcrmcall_info.v4_instance != 0)
         {
           res_buff_ptr->used = snprintf((char *)res_buff_ptr->data_ptr,
                                        res_buff_ptr->size,
                                        "$QCRMCALL: %d, V4", (qcrm_inst)+1);
           DS_AT_MSG0_HIGH(" QCRMCALL V4 Connected");
         }
         if (dsat_qcrmcall_info.v6_instance != 0)
         {
           if(dsat_qcrmcall_info.v4_instance != 0)
           {
             res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                        res_buff_ptr->size,
                                        "\n");
           }
           res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                        res_buff_ptr->size,
                                        "$QCRMCALL: %d, V6", (qcrm_inst)+1);             
           DS_AT_MSG0_HIGH(" QCRMCALL V6 Connected");
         }

         /*Reset No_Carrier as no call was brought up by RmNet*/
         if(dsat_qcrmcall_info.v4_instance == 0 &&
            dsat_qcrmcall_info.v6_instance == 0)
         {
           result = DSAT_NO_CARRIER;
         }
         else
         {
           memset(&dsat_qcrmcall_info,0x0,sizeof(dsat_qcrmcall_info));
           (void)dsatcmdp_send_respose_to_te(res_buff_ptr, DSAT_COMPLETE_RSP);
           result = DSAT_OK;
         }
       }
       else
       {
         result = DSAT_ASYNC_CMD;
       }
       break;

     case QCRMCALL_ACTION_NONE:
     default:
       DS_AT_MSG0_MED("Igorning other events");
       break;
  }

  return result;
}
/*===========================================================================
FUNCTION   DSAT_GET_DEFAULT_PROFILE_NUMBER

DESCRIPTION
  This function is used to retreive default profile number. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dsat_get_default_profile_number
(
  uint8                        profile_type,
  uint8                        profile_family,
  uint16                      *default_profile_num
)
{
  ds_profile_status_etype           profile_status;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    
  profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  *default_profile_num = 0;

  /*-------------------------------------------------------------------------
                       Call the API to get the default profile number.
    -------------------------------------------------------------------------*/
  profile_status = 
    ds_profile_get_default_profile_num((ds_profile_tech_etype)profile_type, 
                                        (uint32)profile_family,
                                        default_profile_num);
  if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS && 
      profile_status != DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_TYPE)
  {
    DS_AT_MSG2_ERROR ("Get Default Profile for type %d failed. Error %d", 
               profile_type, profile_status);
  }

}

/*===========================================================================

FUNCTION QCRMCALL_VALIDATE_PARAMS

DESCRIPTION
  This functions validates the parameters received. 

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_ASYNC_CMD          : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
boolean qcrmcall_validate_input_params
(
  const tokens_struct_type *tok_ptr, 
  rmnet_meta_sm_network_info_type *sni_params
)
{
  #define QCRMCALL_ACT_3GPP2  1
  #define QCRMCALL_ACT_3GPP   2
  #define QCRMCALL_IP_V4      1
  #define QCRMCALL_IP_V6      2
  #define QCRMCALL_IP_V4V6    3
    
  dsat_num_item_type  temp_data = 0;
  uint16  def_profile_num = 0;
  uint32  len = 0;
/*------------------------------------------------------------------------*/

  ASSERT(NULL != sni_params );

  if ( (NA|EQ|AR) == tok_ptr->op )
  {
    /* Set Defaults */
     dsat_get_default_profile_number(DS_PROFILE_TECH_3GPP,
                                     DS_PROFILE_3GPP_RMNET_PROFILE_FAMILY,
                                     &def_profile_num);
    
     sni_params->umts_profile_num = def_profile_num;

     dsat_get_default_profile_number(DS_PROFILE_TECH_3GPP2,
                                     DS_PROFILE_3GPP_RMNET_PROFILE_FAMILY,
                                     &def_profile_num);

    sni_params->cdma_profile_num = (uint8) def_profile_num;

    sni_params->tech_pref = WWAN_GROUP;
    sni_params->ip_family_pref = QMI_AF_INET;

    /* Retreive optional parameter IP type */
    if ( VALID_TOKEN(2))
    {
      if ( dsatutil_atoi( &temp_data, 
                       tok_ptr->arg[2], 10) != ATOI_OK )
      {
        DS_AT_MSG0_ERROR("Ascii to integer conversion failed");
        return FALSE;
      }
      if ( (QCRMCALL_IP_V4V6 < temp_data) || (temp_data < QCRMCALL_IP_V4) )
      {
        DS_AT_MSG1_ERROR("Invalid IP Type received [%d]",temp_data);
        return FALSE; 
      }
      switch (temp_data)
      {
        case QCRMCALL_IP_V4:         /* IPv4        */
          sni_params->ip_family_pref = QMI_AF_INET;
          break;          
        case QCRMCALL_IP_V6:        /* IPv6        */
          sni_params->ip_family_pref = QMI_AF_INET6;
          break;
        case QCRMCALL_IP_V4V6:       /* IPV4V6 */
          sni_params->ip_family_pref = QMI_AF_INET_4_6;
          break;
        default:
          sni_params->ip_family_pref = QMI_AF_INET;
      }
    }

    /* Retreive Tech Preference */
    if ( VALID_TOKEN(3))
    {
      if ( dsatutil_atoi( &temp_data, 
                       tok_ptr->arg[3], 10) != ATOI_OK )
      {
        DS_AT_MSG0_ERROR("Ascii to integer conversion failed");
        return FALSE;
      }
      if ( QCRMCALL_ACT_3GPP2 == temp_data )
      {
        sni_params->tech_pref = CDMA_SN_IFACE;
      }
      else if ( QCRMCALL_ACT_3GPP == temp_data )
      {
        sni_params->tech_pref = UMTS_IFACE;
      }
      else
      {
        DS_AT_MSG1_ERROR("Invalid Tech Pref [%d]", temp_data);
        return FALSE;
      }
    }
    /* Retreive optional parameter UMTS Profile number */
    if ( VALID_TOKEN(4))
    {
      if ( dsatutil_atoi( &temp_data, 
                       tok_ptr->arg[4], 10) != ATOI_OK )
      {
        DS_AT_MSG0_ERROR("Ascii to integer conversion failed");
        return FALSE;
      }
      if ( (temp_data == 0 ) || 
           (!DSAT_IS_PROFILE_ID_WITHIN_RANGE(temp_data)) )
      {
        DS_AT_MSG1_ERROR("Invalid UMTS Profile [%d]", temp_data);
        return FALSE;
      }
      sni_params->umts_profile_num = temp_data;
    }
    /* Retreive optional parameter CDMA Profile number */
    if ( VALID_TOKEN(5))
    {
      if ( dsatutil_atoi( &temp_data, 
                       tok_ptr->arg[5], 10) != ATOI_OK )
      {
        DS_AT_MSG0_ERROR("Ascii to integer conversion failed");
        return FALSE;
      }
      sni_params->cdma_profile_num = temp_data;
    }
    /* Retreive optional parameterAPN Name */
    if ( VALID_TOKEN(6))
    {
      len = strlen((char *)tok_ptr->arg[6]);
      if ( len > ( 2 + DS_PROFILE_3GPP_MAX_APN_STRING_LEN ))
      {
        DS_AT_MSG1_ERROR("APN is too long [%d]",len);
        return FALSE;
      }
      if ( '"' == *tok_ptr->arg[6]) /* Strip Quotes */
      {
        if(!dsatutil_strip_quotes_out((const byte*)tok_ptr->arg[6],
                                      (byte*)tok_ptr->arg[6],(word) len))
        {
          return FALSE;        
        }
        (void) dsatutil_memscpy((void*)sni_params->apn.name,255,
                                (void*)tok_ptr->arg[6],(len-2));
        
        sni_params->apn.length = (uint8) len - 2;
      }
      else
      {
        return FALSE;
      }

    }
    return TRUE;
    
  }

  return FALSE;
}/* qcrmcall_validate_input_params */

/*===========================================================================

FUNCTION DSATVEND_QCRMCALL_ABORT_CMD_HANDLER

DESCRIPTION
  This function is an abort command handler, which stops the current originate 
  request which if it is still in progress. 

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:        syntax error.
    DSAT_ASYNC_CMD          : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatvend_qcrmcall_abort_cmd_handler
(
  const dsati_cmd_type *cmd_table_entry_ptr  /* Ptr to cmd entry in table. */
)
{
  rmnet_qcrmcall_request_type      action_request;
  dsat_num_item_type               temp_instance_v4;
  dsat_num_item_type               temp_instance_v6;

  memset(&action_request, 0x0 ,sizeof(action_request));

  temp_instance_v4 = dsat_qcrmcall_info.v4_instance - 1;
  temp_instance_v6 = dsat_qcrmcall_info.v6_instance - 1;

  /*Should not abort Stop command*/
  if ((dsat_qcrmcall_info.state & DSAT_PENDING_QCRMCALL_STOP_V4) ||
      (dsat_qcrmcall_info.state & DSAT_PENDING_QCRMCALL_STOP_V6))
  {
    return FALSE;
  }

  /* Check if a call is still in pending state, then tear it down*/
  else if ( dsat_qcrmcall_info.state != DSAT_PENDING_QCRMCALL_NONE )
  {
    if (dsat_qcrmcall_info.v4_instance != 0)
    {
      action_request.stop.qcrmcall_inst = temp_instance_v4;
      action_request.stop.ip_family      = QMI_AF_INET;
      dsat_qcrmcall_info.state          |= DSAT_PENDING_QCRMCALL_STOP_V4; /*Set pending stop state for IPV4*/

      if( !(rmnet_send_qcrmcall_request
          (QCRMCALL_ACTION_STOP, &action_request, dsatvend_rmnet_ev_cb)))
      {
        dsat_qcrmcall_info.state         &= ~DSAT_PENDING_QCRMCALL_STOP_V4;
        DS_AT_MSG1_HIGH(" ABORT QCRMCall on instance %d failed on IPV4 ",temp_instance_v4);
        return FALSE;
      }
    }
    if (dsat_qcrmcall_info.v6_instance != 0 )
    {
      action_request.stop.qcrmcall_inst = temp_instance_v6;
      action_request.stop.ip_family      = QMI_AF_INET6;  /*Set pending stop state for IPV6*/
      dsat_qcrmcall_info.state          |= DSAT_PENDING_QCRMCALL_STOP_V6;

      if( !(rmnet_send_qcrmcall_request
          (QCRMCALL_ACTION_STOP, &action_request, dsatvend_rmnet_ev_cb)))
      {
        dsat_qcrmcall_info.state         &= ~DSAT_PENDING_QCRMCALL_STOP_V6;
        DS_AT_MSG1_HIGH(" ABORT QCRMCall on instance %d failed on IPV6 ",temp_instance_v6);
        return FALSE;
      }
    }
  }

  return TRUE;
  
}/* dsatvend_qcrmcall_abort_cmd_handler */


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCRMCALL_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes $QCRMCALL command.
  $QCRMCALL command is used to trigger a RmNet call via AT command. 
  It just passes on the Start Network Interface parameters into RmNet to
  bring up the call. 

  $QCRMCALL =<action>, <instance>, [<IP type>, <[Act],<umts_profile>,<cdma_profile>,<APN>]

   Response:
   $QCRMCALL:<Instance>, <V4>
   $QCRMCALL:<Instance>, <V6> 
   OK

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR:    syntax error.
    DSAT_ASYNC_CMD : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcrmcall_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{  
#define DSAT_QCRMCALL_ACTION_START 1
#define DSAT_QCRMCALL_ACTION_STOP  0

  rmnet_meta_sm_network_info_type  *sni_params_ptr = NULL;
  dsat_num_item_type               temp_instance = 0;
  rmnet_qcrmcall_request_type      action_request;
  dsat_num_item_type               action = 0;
  dsat_error_info_s_type   err_info;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;
/* -----------------------------------------------------------------------------*/


  /*========================Few Basic Concepts ======================================
     * State Machine and Code Flow
     * We support bringing up IPv4/IPV6 calls on multi-instances of RmNet
     * We also support DUAL-IP over single RmNet (means two calls on same RmNet instance) 
     * We mantain our own state machine for bringing up RmNet call's 
     *  
     * State Machine 
     *  
     * at$qcrmcall=1,1,1 (Means Start IPV4 call on RmNet instance 0) 
     *  
     *  Example of State Transitions for QCRMCALL on IPv4-
     *  
     *  DSAT_PENDING_QCRMCALL_NONE ------------> DSAT_PENDING_QCRMCALL_START_V4
     *  Wait for Callback from RmNet
     *  DSAT_PENDING_QCRMCALL_START_V4 ---------> DSAT_PENDING_QCRMCALL_NONE
     *  
     *  IPV6 Call
     *  
     *  DSAT_PENDING_QCRMCALL_NONE ------------> DSAT_PENDING_QCRMCALL_START_V6
     *  Wait for Callback from RmNet
     *  DSAT_PENDING_QCRMCALL_START_V6 ---------> DSAT_PENDING_QCRMCALL_NONE
     *  
     *  IPV4-IPV6 - Here we bring two calls up on same RmNet instance
     *  
     *  DSAT_PENDING_QCRMCALL_NONE ------------> DSAT_PENDING_QCRMCALL_START_V4
     *  DSAT_PENDING_QCRMCALL_START_V4---------> DSAT_PENDING_QCRMCALL_START_V6
     *                               |---------->DSAT_PENDING_QCRMCALL_START_V4
     *  Wait for Callback from RmNet
     *  DSAT_PENDING_QCRMCALL_START_V4 ---------> DSAT_PENDING_QCRMCALL_NONE
     *  DSAT_PENDING_QCRMCALL_START_V6---------------------------|
     *  
    =============================================================================  */
  /* Validate Input Params for Write command */
   memset(&dsat_qcrmcall_info,0x0,sizeof(dsat_qcrmcall_info));

  if ( (NA|EQ|AR) == tok_ptr->op )
  {
    
    /* Action Param; Mandatory */ 
    err_info.arg_num = 0;
    if ( VALID_TOKEN(0))
    {
      if ( dsatutil_atoi( &action, 
                       tok_ptr->arg[0], 10) != ATOI_OK )
      {
        err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
        goto send_error;
      }
  
      if( (DSAT_QCRMCALL_ACTION_START != action ) &&
          (DSAT_QCRMCALL_ACTION_STOP != action  ))
      {
        err_info.errval = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
        goto send_error;
      }
    }
    else
    { /* Action is mandatory */
      err_info.errval = DSAT_ERR_PARAMETER_MANDATORY;
      goto send_error;
    }

    err_info.arg_num = 1;
    if ( VALID_TOKEN(1))
    {
      if ( ( dsatutil_atoi( &temp_instance, 
                       tok_ptr->arg[1], 10) != ATOI_OK ) || ( temp_instance == 0))
      {
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        goto send_error;
      }
      sni_params_ptr = (rmnet_meta_sm_network_info_type *)dsat_alloc_memory(sizeof(rmnet_meta_sm_network_info_type),FALSE);
      if ( FALSE == qcrmcall_validate_input_params( tok_ptr, sni_params_ptr) )
      {
        DS_AT_MSG0_ERROR("Invalid Params received; cannot proceed further");
        dsatutil_free_memory(sni_params_ptr);
        return DSAT_ERROR;
      }


      /* Here we are making the RmNet instance to be 1 less as Rmnet instance start from 0*/
     temp_instance = temp_instance - 1;

    if ( DSAT_QCRMCALL_ACTION_START == action )
    {
      sni_params_ptr->call_type = RMNET_CALL_TYPE_LAPTOP; 
      action_request.start.sni_params = sni_params_ptr;
      action_request.start.qcrmcall_inst = RMNET_INSTANCE_MAX;

      DS_AT_MSG2_HIGH(" QCRMCall START inst:%d IPtype: %d ",temp_instance,sni_params_ptr->ip_family_pref);
      
      switch ( sni_params_ptr->ip_family_pref )
      {
        case QMI_AF_INET_4_6:
          {
            /*Setting Parameters recieved as Start_NWK_Interface Params for RmNet call*/

            action_request.start.qcrmcall_inst              = temp_instance; 
            action_request.start.sni_params->ip_family_pref = (uint8) QMI_AF_INET;
            dsat_qcrmcall_info.state                        = DSAT_PENDING_QCRMCALL_START_V4; /*Setting V4_STATE_START*/
            dsat_qcrmcall_info.v4_instance                  = action_request.start.qcrmcall_inst + 1;
            
            if( !(rmnet_send_qcrmcall_request
                (QCRMCALL_ACTION_START, &action_request, dsatvend_rmnet_ev_cb)))
            {
              DS_AT_MSG1_HIGH("V4 QCRMCall on instance %d failed",temp_instance);
              dsat_qcrmcall_info.state        =  DSAT_PENDING_QCRMCALL_NONE;
              dsat_qcrmcall_info.v4_instance  = 0;
            }

            action_request.start.qcrmcall_inst               = temp_instance;
            action_request.start.sni_params->ip_family_pref  = (uint8) QMI_AF_INET6;
            dsat_qcrmcall_info.state                        |= DSAT_PENDING_QCRMCALL_START_V6;
            dsat_qcrmcall_info.v6_instance                   =  action_request.start.qcrmcall_inst + 1;
            
            if( !(rmnet_send_qcrmcall_request
                (QCRMCALL_ACTION_START, &action_request, dsatvend_rmnet_ev_cb)))
            {
              DS_AT_MSG1_HIGH("V6 QCRMCall on instance %d failed",dsat_qcrmcall_info.v6_instance );
              dsat_qcrmcall_info.state       &= ~DSAT_PENDING_QCRMCALL_START_V6;
              dsat_qcrmcall_info.v6_instance  = 0;
            }
            
            DS_AT_MSG3_HIGH("QCRMCALL V4/V6 QCRMCall on instance: %d family : %d state %d",
                                                      temp_instance, 
                                                      sni_params_ptr->ip_family_pref ,
                                                      dsat_qcrmcall_info.state);

            if(dsat_qcrmcall_info.state == DSAT_PENDING_QCRMCALL_NONE)
            {
              DS_AT_MSG0_ERROR("QCRMCALL V4/V6 Call failed");
              dsatutil_free_memory(sni_params_ptr);
              return DSAT_ERROR;
            }   
            DS_AT_MSG3_HIGH("QCRMCALL V4/V6 QCRMCall on V4: %d V6 : %d state %d",
                                                      dsat_qcrmcall_info.v4_instance, 
                                                      dsat_qcrmcall_info.v6_instance ,
                                                      dsat_qcrmcall_info.state);
            break;
          }
        case QMI_AF_INET6:
        case QMI_AF_INET:
        default:
        {
            if(sni_params_ptr->ip_family_pref == QMI_AF_INET6)
            {
              dsat_qcrmcall_info.v6_instance   =  temp_instance + 1;
              dsat_qcrmcall_info.state        |= DSAT_PENDING_QCRMCALL_START_V6;
            }else
            {
              dsat_qcrmcall_info.state         = DSAT_PENDING_QCRMCALL_START_V4;
              dsat_qcrmcall_info.v4_instance   = temp_instance + 1;
            }
            
            action_request.start.qcrmcall_inst = temp_instance;
            if( !(rmnet_send_qcrmcall_request
                (QCRMCALL_ACTION_START, &action_request, dsatvend_rmnet_ev_cb)))
            {
              DS_AT_MSG2_HIGH(" QCRMCall on instance %d failed on IPV%d ",temp_instance,sni_params_ptr->ip_family_pref);
              dsat_qcrmcall_info.state       =  DSAT_PENDING_QCRMCALL_NONE;
              dsat_qcrmcall_info.v4_instance = 0;
              dsat_qcrmcall_info.v6_instance  = 0;
              dsatutil_free_memory(sni_params_ptr);
              return DSAT_ERROR;
            }
            break;
          }   
        }
      dsatutil_free_memory(sni_params_ptr);
      return DSAT_ASYNC_CMD;
      }
    else
    {

      dsat_qcrmcall_info.state        =  DSAT_PENDING_QCRMCALL_NONE;
      
        switch ( sni_params_ptr->ip_family_pref )
        {  
           case QMI_AF_INET_4_6:
           {
              action_request.stop.qcrmcall_inst              = temp_instance;
              action_request.stop.ip_family                  = QMI_AF_INET;
              dsat_qcrmcall_info.state                       = DSAT_PENDING_QCRMCALL_STOP_V4;
              dsat_qcrmcall_info.v4_instance                 = action_request.stop.qcrmcall_inst + 1;
              
              if( !(rmnet_send_qcrmcall_request
                  (QCRMCALL_ACTION_STOP, &action_request, dsatvend_rmnet_ev_cb)))
              {
                DS_AT_MSG1_HIGH("V4 QCRMCall on instance %d failed",temp_instance);
                dsat_qcrmcall_info.state        =  DSAT_PENDING_QCRMCALL_NONE;
                dsat_qcrmcall_info.v4_instance  = 0;
              }
  
              action_request.stop.qcrmcall_inst               = temp_instance;
              action_request.stop.ip_family                   = QMI_AF_INET6;
              dsat_qcrmcall_info.state                       |= DSAT_PENDING_QCRMCALL_STOP_V6;
              dsat_qcrmcall_info.v6_instance                  =  action_request.stop.qcrmcall_inst + 1;
              
              if( !(rmnet_send_qcrmcall_request
                  (QCRMCALL_ACTION_STOP, &action_request, dsatvend_rmnet_ev_cb)))
              {
                DS_AT_MSG1_HIGH("V6 QCRMCall on instance %d failed",temp_instance+1);
                dsat_qcrmcall_info.state       &= ~DSAT_PENDING_QCRMCALL_STOP_V6;
                dsat_qcrmcall_info.v6_instance  = 0;
              }
              
              DS_AT_MSG3_HIGH("QCRMCALL V4/V6 on instance: %d family : %d state %d",
                                                        temp_instance, 
                                                        sni_params_ptr->ip_family_pref ,
                                                        dsat_qcrmcall_info.state);
  
              if(dsat_qcrmcall_info.state == DSAT_PENDING_QCRMCALL_NONE)
              {
                DS_AT_MSG0_ERROR("QCRMCALL V4/V6 Call failed");
                dsatutil_free_memory(sni_params_ptr);
                return DSAT_ERROR;
              }
              DS_AT_MSG3_HIGH("QCRMCALL V4/V6 QCRMCall on V4: %d V6 : %d state %d",
                                                      dsat_qcrmcall_info.v4_instance, 
                                                      dsat_qcrmcall_info.v6_instance ,
                                                      dsat_qcrmcall_info.state);
               break;
            }
              
            case QMI_AF_INET:
            case QMI_AF_INET6:
            default:
            {
              action_request.stop.qcrmcall_inst = temp_instance;

              if(sni_params_ptr->ip_family_pref == QMI_AF_INET6)
              {
                action_request.stop.ip_family    = QMI_AF_INET6;
                dsat_qcrmcall_info.v6_instance   =  temp_instance + 1;
                dsat_qcrmcall_info.state         = DSAT_PENDING_QCRMCALL_STOP_V6;
              }
              else
              {
                action_request.stop.ip_family    = QMI_AF_INET;
                dsat_qcrmcall_info.state         = DSAT_PENDING_QCRMCALL_STOP_V4;
                dsat_qcrmcall_info.v4_instance   = temp_instance + 1;
              }
              
              if( !(rmnet_send_qcrmcall_request
                  (QCRMCALL_ACTION_STOP, &action_request, dsatvend_rmnet_ev_cb)))
              {
                DS_AT_MSG2_HIGH(" QCRMCall on instance %d failed on IPV%d ",temp_instance,sni_params_ptr->ip_family_pref);
                dsat_qcrmcall_info.state       =  DSAT_PENDING_QCRMCALL_NONE;
                dsat_qcrmcall_info.v4_instance = 0;
                dsat_qcrmcall_info.v6_instance  = 0;
                dsatutil_free_memory(sni_params_ptr);
                return DSAT_ERROR;
              }
              break;
            }   
          }
          dsatutil_free_memory(sni_params_ptr);
          return DSAT_ASYNC_CMD; /*Means No error found*/
        }
        
    }
    else
    {
      DS_AT_MSG0_ERROR("Invalid ARG 1 received; cannot proceed further");
      return DSAT_ERROR;
    }
  }
  else if ( (NA|EQ|QU) == tok_ptr->op )
  {
    rmnet_instance_e_type rmnet_instance = RMNET_INSTANCE_MAX;

    /* Report Action Data */
    res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                        res_buff_ptr->size,
                                        "$QCRMCALL: (0-1),(");

   /* Report available instances  */
   for ( rmnet_instance = 0;rmnet_instance < RMNET_QCRMCALL_MAX; 
         rmnet_instance++)
   {
       res_buff_ptr->used += (word)snprintf(
                        (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                        res_buff_ptr->size -  res_buff_ptr->used,
                                      "%d,",(int)1+(rmnet_instance));
   }
   /* To Removed , after Instance */
   res_buff_ptr->used--;
   
   res_buff_ptr->used += (word)snprintf(
                        (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                        res_buff_ptr->size -  res_buff_ptr->used,
                                        ")");

   /*Report PDP Type,  AcT, UMTS, CDMA profile numbers*/

   res_buff_ptr->used += (word)snprintf(
                         (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                          res_buff_ptr->size -  res_buff_ptr->used,
                          ",(1-3),(1-2),(%d-%d,%d-%d),,",
                          DSAT_LEGACY_PROFILE_NUM_MIN,
                          DSAT_LEGACY_PROFILE_NUM_MAX,
                          DSAT_EXT_PROFILE_NUM_MIN,
                          DSAT_EXT_PROFILE_NUM_MAX); 
    return DSAT_OK;
  }
  else if ( tok_ptr->op == (NA|QU))
  {
    dsat_qcrmcall_info.state        =  DSAT_PENDING_QCRMCALL_QUERY;
    if( !(rmnet_send_qcrmcall_request
        (QCRMCALL_ACTION_QUERY, &action_request, dsatvend_rmnet_ev_cb)))
    {
      DS_AT_MSG1_HIGH("Query QCRMCall on instance %d failed",temp_instance);
      dsat_qcrmcall_info.state        =  DSAT_PENDING_QCRMCALL_NONE;
      return DSAT_ERROR;
    }

    DS_AT_MSG1_HIGH("Query V4 QCRMCall on instance %d",temp_instance);
    return DSAT_ASYNC_CMD;
  }

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_TOKENS)
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }

  return DSAT_ERROR;
}/*  dsatvend_exec_qcrmcall_cmd */

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCSIMT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcsimt command.
  This function returns SIM TYPE
    * RUIM
    * USIM
    * SIM

DEPENDENCIES
  None
  
RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success and execution completed.
  
SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */

dsat_result_enum_type dsatvend_exec_qcsimt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;

  if ( tok_ptr->op == (NA) ||
       tok_ptr->op == (NA|QU))
  {
    switch (dsat_get_current_app_type(TRUE))
    {
      case MMGSDI_APP_RUIM:
      {
        res_buff_ptr->used = (word) snprintf ( (char *) res_buff_ptr->data_ptr,
                                                        res_buff_ptr->size,
                                                       "$QCSIMT: RUIM");
        break;
      }

      case MMGSDI_APP_USIM:
      {
        res_buff_ptr->used = (word) snprintf ( (char *) res_buff_ptr->data_ptr,
                                                        res_buff_ptr->size,
                                                       "$QCSIMT: USIM");
        break;
      }

      case MMGSDI_APP_SIM:
      {
        res_buff_ptr->used = (word) snprintf ( (char *) res_buff_ptr->data_ptr,
                                                        res_buff_ptr->size,
                                                       "$QCSIMT: SIM");
        break;
      }

      default:
        result = DSAT_ERROR;
    }
  }
  else
  {
    /* other operations are not supported */
    result = DSAT_ERROR;
  }
  return result;
}

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCNSP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCNSP command.
  AT$QCSNT command is used to configure the 
  network selection mode.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR   : if there was any problem in executing the command
    DSAT_OK        :  if it is a success and execution completed.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */

dsat_result_enum_type dsatvend_exec_qcnsp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{ 

  dsat_result_enum_type  result          = DSAT_ASYNC_CMD;
  dsat_num_item_type     mode_val        = DSAT_QCNSP_RAT_MODE_MAX;
  dsat_num_item_type     net_sel_mode    = DSAT_QCNSP_NET_SEL_MODE_MAX;
  dsat_num_item_type     acq_pri_order_pref  = DSAT_QCNSP_ACQ_ORDER_PREF_MAX;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  if (tok_ptr->op == (NA|EQ|AR))
  {		
    if(VALID_TOKEN(0) && (ATOI_OK == dsatutil_atoi( &mode_val, tok_ptr->arg[0], 10 )))
    {
       switch(mode_val)
       {
         case DSAT_QCNSP_RAT_MODE_AUTOMATIC:
           DSATUTIL_SET_VAL(DSAT_VENDOR_QCNSP_IDX,0,0,0,CM_MODE_PREF_AUTOMATIC,NUM_TYPE)
            break;
         case DSAT_QCNSP_RAT_MODE_GSM:
           DSATUTIL_SET_VAL(DSAT_VENDOR_QCNSP_IDX,0,0,0,CM_MODE_PREF_GSM_ONLY,NUM_TYPE)
            break;
         case DSAT_QCNSP_RAT_MODE_WCDMA:
           DSATUTIL_SET_VAL(DSAT_VENDOR_QCNSP_IDX,0,0,0,CM_MODE_PREF_WCDMA_ONLY,NUM_TYPE)
            break;
         case DSAT_QCNSP_RAT_MODE_LTE:
           DSATUTIL_SET_VAL(DSAT_VENDOR_QCNSP_IDX,0,0,0,CM_MODE_PREF_LTE_ONLY,NUM_TYPE)
           break;
         default:
            return DSAT_ERROR;
       }
    }
    else
    {
    	 DSATUTIL_SET_VAL(DSAT_VENDOR_QCNSP_IDX,0,0,0,CM_MODE_PREF_NO_CHANGE,NUM_TYPE)
    }
    if(VALID_TOKEN(1) && (ATOI_OK == dsatutil_atoi ( &net_sel_mode, tok_ptr->arg[1], 10 ) ))
    {
      if( (net_sel_mode == CM_NETWORK_SEL_MODE_PREF_AUTOMATIC) ||
          (net_sel_mode == CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV) )
      {
         DSATUTIL_SET_VAL(DSAT_VENDOR_QCNSP_IDX,0,1,0,net_sel_mode,NUM_TYPE)
      }
      else
      {
        return DSAT_ERROR;
      }
    }
    else
    {
       DSATUTIL_SET_VAL(DSAT_VENDOR_QCNSP_IDX,0,1,0,CM_NETWORK_SEL_MODE_PREF_NO_CHANGE,NUM_TYPE)
    }
    if(VALID_TOKEN(2) && (ATOI_OK == dsatutil_atoi ( &acq_pri_order_pref, tok_ptr->arg[2], 10 ) ))
    {
      switch(acq_pri_order_pref)
      {
        case DSAT_QCNSP_ACQ_ORDER_PREF_AUTOMATIC:
          DSATUTIL_SET_VAL(DSAT_VENDOR_QCNSP_IDX,0,2,0,SYS_SYS_MODE_NONE,NUM_TYPE)
         break;
        case DSAT_QCNSP_ACQ_ORDER_PREF_GSM:
          DSATUTIL_SET_VAL(DSAT_VENDOR_QCNSP_IDX,0,2,0,SYS_SYS_MODE_GSM,NUM_TYPE)
         break;
        case DSAT_QCNSP_ACQ_ORDER_PREF_WCDMA:
          DSATUTIL_SET_VAL(DSAT_VENDOR_QCNSP_IDX,0,2,0,SYS_SYS_MODE_WCDMA,NUM_TYPE)
         break;
        case DSAT_QCNSP_ACQ_ORDER_PREF_LTE:
          DSATUTIL_SET_VAL(DSAT_VENDOR_QCNSP_IDX,0,2,0,SYS_SYS_MODE_LTE,NUM_TYPE)
         break;
        default:
	        return DSAT_ERROR;
       }
    }
    else
    {
      DSATUTIL_SET_VAL(DSAT_VENDOR_QCNSP_IDX,0,2,0,SYS_SYS_MODE_NONE,NUM_TYPE)
    }

    if(  mode_val != DSAT_QCNSP_RAT_MODE_MAX ||
         net_sel_mode  != DSAT_QCNSP_NET_SEL_MODE_MAX ||
         acq_pri_order_pref  != DSAT_QCNSP_ACQ_ORDER_PREF_MAX
      )
    {
      SET_PENDING(DSAT_VENDOR_QCNSP_IDX,0, DSAT_PENDING_QCNSP_GET_PRI_RAT)
      if ( FALSE == cm_ph_cmd_get_ph_info_per_subs(dsatcmif_ph_cmd_cb_func,
                                    NULL,
                                    dsatcm_client_id,
                                    subs_id
                                    ) )
      {
        SET_PENDING(DSAT_VENDOR_QCNSP_IDX ,0, DSAT_PENDING_QCNSP_NONE)
        result = DSAT_ERROR;
      }
    }
    else
    {
      /*CM will not send events if ther is NO CHANGE*/
       result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|QU))
  {
    SET_PENDING(DSAT_VENDOR_QCNSP_IDX,0, DSAT_PENDING_QCNSP_READ)
    if ( FALSE == cm_ph_cmd_get_ph_info_per_subs(dsatcmif_ph_cmd_cb_func,
                                    NULL,
                                    dsatcm_client_id,
                                    subs_id
                                    ) )
    {
      SET_PENDING(DSAT_VENDOR_QCNSP_IDX ,0, DSAT_PENDING_QCNSP_NONE)
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    res_buff_ptr->used = 
      (word) snprintf ((char*)res_buff_ptr->data_ptr, 
                              res_buff_ptr->size,
                              "$QCNSP: (0,1,2,6),"
                              "(0-2),"
                              "(0-3)" );
    result = DSAT_OK;
  }
  else
  {
    result = DSAT_ERROR;
  }
  
  return result;
}

/*===========================================================================

FUNCTION DSAVEND_PROCESS_QCNSP_CMD

DESCRIPTION
  This function process the response for the $QCNSP command on return from
  asynchronous processing. 
 
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
.

SIDE EFFECTS
  None

===========================================================================*/

dsat_result_enum_type dsatvend_process_qcnsp_cmd 
(
  dsat_cmd_pending_enum_type cmd_type,
  ds_at_cm_ph_pref_type  *pref_mode    /* Network preference mode */
)
{
  dsat_result_enum_type     result                    = DSAT_ASYNC_CMD;
  dsm_item_type            *res_buff_ptr              = NULL;
  dsat_num_item_type        mode_pref                 = DSAT_QCNSP_RAT_MODE_MAX;
  dsat_num_item_type        network_sel_mode_pref     = DSAT_QCNSP_NET_SEL_MODE_MAX;
  dsat_num_item_type        acq_pri_order_pref        = DSAT_QCNSP_ACQ_ORDER_PREF_MAX;
  sys_modem_as_id_e_type    subs_id = dsat_get_current_subs_id(FALSE);

  if( NULL == pref_mode)
  {
    return DSAT_ERROR;
  }
  switch( cmd_type )
  {
    case DSAT_PENDING_QCNSP_READ:
    {
      DS_AT_MSG3_HIGH("mode_pref=%d, network_sel_mode_pref=%d, acq_order_pref=%d",
      	                                         pref_mode->network_rat_mode_pref,
      	                                         pref_mode ->network_sel_mode_pref,
      	                                         pref_mode ->acq_pri_order_pref.acq_order[0]);
      res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);

      switch( pref_mode->network_rat_mode_pref )
      {
        case CM_MODE_PREF_AUTOMATIC:
          mode_pref = DSAT_QCNSP_RAT_MODE_AUTOMATIC;
          break;
        case CM_MODE_PREF_GSM_ONLY:
          mode_pref = DSAT_QCNSP_RAT_MODE_GSM;
          break;
        case CM_MODE_PREF_WCDMA_ONLY:
          mode_pref = DSAT_QCNSP_RAT_MODE_WCDMA;
          break;
        case CM_MODE_PREF_LTE_ONLY:
          mode_pref = DSAT_QCNSP_RAT_MODE_LTE;
          break;
        default:
          mode_pref = DSAT_QCNSP_RAT_MODE_MAX;
      }
      switch (pref_mode ->network_sel_mode_pref)
      {
        case CM_NETWORK_SEL_MODE_PREF_AUTOMATIC:
          network_sel_mode_pref = DSAT_QCNSP_NET_SEL_MODE_AUTOMATIC;
          break;
        case CM_NETWORK_SEL_MODE_PREF_MANUAL:
          network_sel_mode_pref = DSAT_QCNSP_NET_SEL_MODE_MANUAL;
          break;
        case CM_NETWORK_SEL_MODE_PREF_LIMITED_SRV:
          network_sel_mode_pref = DSAT_QCNSP_NET_SEL_MODE_LIMITED_SRV;
          break;
        default:
          network_sel_mode_pref = DSAT_QCNSP_NET_SEL_MODE_MAX;
      }
      switch(pref_mode ->acq_pri_order_pref.acq_order[0])
      {
        case SYS_SYS_MODE_NONE:
          acq_pri_order_pref = DSAT_QCNSP_ACQ_ORDER_PREF_AUTOMATIC;
          break;
        case SYS_SYS_MODE_GSM:
          acq_pri_order_pref = DSAT_QCNSP_ACQ_ORDER_PREF_GSM;
          break;
        case SYS_SYS_MODE_WCDMA:
          acq_pri_order_pref = DSAT_QCNSP_ACQ_ORDER_PREF_WCDMA;
          break;
        case SYS_SYS_MODE_LTE:
          acq_pri_order_pref = DSAT_QCNSP_ACQ_ORDER_PREF_LTE;
          break;
        default:
          acq_pri_order_pref = DSAT_QCNSP_ACQ_ORDER_PREF_MAX;;
      }
      res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                                  res_buff_ptr->size,	
                                                  "$QCNSP: %d,%d,%d",
                                                   mode_pref,
                                                   network_sel_mode_pref,
                                                   acq_pri_order_pref);
      dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
      result = DSAT_OK;
    
  	}
   break;
   case DSAT_PENDING_QCNSP_GET_PRI_RAT:
   {
     uint8                        rat_index;
     boolean                      is_rat_exists = FALSE;
     cm_cmd_user_pref_update_type user_pref;
     cm_rat_acq_order_pref_s_type rat_acq_order_pref;

     memset(&user_pref,0x0,sizeof(user_pref));
     if ( !cm_user_pref_init( &user_pref ) )
     {
       return DSAT_ERROR;
     }

     /*Mode Pref*/
     user_pref.mode_pref = 
                 (dsat_num_item_type)dsatutil_get_val(DSAT_VENDOR_QCNSP_IDX,0,0,NUM_TYPE);
     /*Network Selection Pref*/
     user_pref.network_sel_mode_pref = 
     	            (dsat_num_item_type)dsatutil_get_val(DSAT_VENDOR_QCNSP_IDX,0,1,NUM_TYPE);
     user_pref.asubs_id = subs_id;
     /* RAT order*/
     acq_pri_order_pref = (dsat_num_item_type)dsatutil_get_val(DSAT_VENDOR_QCNSP_IDX,0,2,NUM_TYPE);
     user_pref.client_id = dsatcm_client_id;
     user_pref.pref_term = CM_PREF_TERM_PERMANENT;

     if(SYS_SYS_MODE_NONE == acq_pri_order_pref)
     {
       user_pref.rat_acq_order_pref_ptr = NULL;
     }
     else
     {
       memscpy(&rat_acq_order_pref.acq_order.rat_acq_pri_order, 
       	       sizeof(cm_acq_pri_order_pref_s_type),
               &pref_mode->acq_pri_order_pref,
               sizeof(cm_acq_pri_order_pref_s_type));
       rat_acq_order_pref.type = CM_ACQ_ORDER_TYPE_RAT_PRI;
       for(rat_index = 0; (rat_index < pref_mode->acq_pri_order_pref.num_rat && rat_index < SYS_MAX_ACQ_PREF_RAT_LIST_NUM) ; rat_index++ )
       {
         if( acq_pri_order_pref == pref_mode->acq_pri_order_pref.acq_order[rat_index])
         {
           is_rat_exists = TRUE;
           break;
         }
       }
       
       if( FALSE == is_rat_exists)
       {
         return DSAT_ERROR;
       }
       else
       {
         for( ; 0 != rat_index; rat_index--)
         {
           rat_acq_order_pref.acq_order.rat_acq_pri_order.acq_order[rat_index] = 
                                             rat_acq_order_pref.acq_order.rat_acq_pri_order.acq_order[rat_index-1];
         }
         rat_acq_order_pref.acq_order.rat_acq_pri_order.acq_order[0] = acq_pri_order_pref;
       }
        user_pref.rat_acq_order_pref_ptr = &rat_acq_order_pref;
     }
      
     SET_PENDING(DSAT_VENDOR_QCNSP_IDX,0, DSAT_PENDING_QCNSP_WRITE)
     if(FALSE == cm_user_pref_update_req(&user_pref,dsatcmif_ph_cmd_cb_func,NULL))
     {
       SET_PENDING(DSAT_VENDOR_QCNSP_IDX ,0, DSAT_PENDING_QCNSP_NONE)
       result = DSAT_ERROR;
     }
     break;
   }
   case DSAT_PENDING_QCNSP_WRITE:
    result = DSAT_OK;
    break;
   default:
     DS_AT_MSG1_ERROR("Invalid cmd type %d",cmd_type);
     result = DSAT_ERROR;
  }
  return result;
}/* dsatvend_process_qcnsp_cmd */


#ifdef FEATURE_DSAT_LTE
#if defined(FEATURE_VOIP) && defined(FEATURE_IMS)
/*===========================================================================
FUNCTION   DSAT_CONVERT_AMRNUM_TO_AMRSTR

DESCRIPTION
 Function will convert AMR number value int AMR string. It will check the SET bit in given number
 and generate corresponding string.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void dsat_convert_amrnum_to_amrstr
(
  char                   *rsp_buf,
  dsat_num_item_type      buf_len,
  dsat_num_item_type      amr_num
)
{
  dsat_num_item_type bit_num = 0;
  dsat_num_item_type cur_buf_len = 0;
  
  if(NULL == rsp_buf)
  {
    return;
  }
  memset(rsp_buf,0,buf_len);
  
  while((amr_num != 0) && cur_buf_len < buf_len)
  {
    if(0 != (amr_num & 1))
    {
      cur_buf_len += snprintf(&rsp_buf[cur_buf_len],buf_len - cur_buf_len,"%d,",bit_num);
    }
    amr_num = amr_num>>1;
    bit_num++;
  }
  /*Replace , with Null terminator*/
  if(cur_buf_len != 0)
  {
    rsp_buf[cur_buf_len-1] = '\0';
  }
  return;
}/*dsat_convert_amrnum_to_amrstr*/

/*===========================================================================
FUNCTION DSATVEND_VOIPM_CONFIG_CB

DESCRIPTION
  This function handles set/get response from IMS.  

DEPENDENCIES
  None


RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsatvend_voipm_config_cb
(
  ims_settings_at_cmd_rsp_info at_cmd_rsp_info
)
{
  ds_cmd_type                  *cmd_buf  = NULL;
  ims_settings_at_cmd_rsp_info *rsp_info_ptr = NULL;

  DS_AT_MSG3_HIGH("QCVOIPM Response cmd=%d, type=%d, user data =%d",
                             at_cmd_rsp_info.eCmd,
                             at_cmd_rsp_info.rspType,
                            (dsat_num_item_type)at_cmd_rsp_info.pRspCbUserData);
  
  cmd_buf = dsat_get_cmd_buf(sizeof(ims_settings_at_cmd_rsp_info), FALSE);
  /* Send the message to ATCOP */
  cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_VOIPM_AT_CMD;
  rsp_info_ptr = cmd_buf->cmd_payload_ptr;

  /*
       * Copy complete reponse from IMS
       */
  if(NULL != rsp_info_ptr)
  {
    dsatutil_memscpy(rsp_info_ptr,
                     sizeof(ims_settings_at_cmd_rsp_info),
                     &at_cmd_rsp_info,
                     sizeof(ims_settings_at_cmd_rsp_info));
  }
  ds_put_cmd(cmd_buf);
}/*dsatvend_voipm_config_cb*/

/*===========================================================================
FUNCTION DSATVEND_VOIPM_CONFIG_HANDLER

DESCRIPTION
  Handler function for IMS  response. 

DEPENDENCIES
  None
  
RETURN VALUE
  
DSAT_ERROR : if there was any problem in executing the command.
DSAT_OK : if it is a success and execution completed.

SIDE EFFECTS
  None
  
======================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatvend_voipm_config_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  dsat_result_enum_type              result            = DSAT_OK;
  dsm_item_type                     *res_buff_ptr      = NULL;
  ims_settings_at_cmd_rsp_info      *rsp_info_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);

  if( (rsp_info_ptr->rspType != QC_IMS_SETTINGS_AT_CMD_RSP_SUCCESS) )
  {
    DS_AT_MSG1_ERROR("Failure response received %d",rsp_info_ptr->rspType);
    SET_PENDING(DSAT_VENDOR_QCVOIPM_IDX ,0, DSAT_PENDING_VOIP_NONE)
    result = DSAT_ERROR;
  }
  else if(CHECK_PENDING( DSAT_VENDOR_QCVOIPM_IDX, 0, DSAT_PENDING_VOIP_WRITE ))
  {
    if(rsp_info_ptr->eCmd == QC_IMS_SETTINGS_AT_CMD_SET_VOIP_CONFIG_RSP)
    {
      result = DSAT_OK;
    }
    else
    {
      result = DSAT_ERROR;
    }
  }
  else if(CHECK_PENDING( DSAT_VENDOR_QCVOIPM_IDX, 0, DSAT_PENDING_VOIP_READ ))
  {
    if(rsp_info_ptr->eCmd == QC_IMS_SETTINGS_AT_CMD_GET_VOIP_CONFIG_RSP)
    {
      ims_settings_at_voip_configuration *rsp_data = NULL;
      char              temp_str[QC_SETTINGS_AT_CMD_VOIP_AMR_WB_MODE_STR_LEN+1];
      
      rsp_data = &rsp_info_ptr->rspData.voipConfig;

      res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);

      res_buff_ptr->used = (word)snprintf((char *)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,
                                                "$QCVOIPM: ");
      if(rsp_data->iIncludeSessionExpires)
      {
        res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
              res_buff_ptr->size - res_buff_ptr->used,"%d",rsp_data->iSessionExpires);
      
      }
      res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
              res_buff_ptr->size - res_buff_ptr->used,",");
    
      if(rsp_data->iIncludeMinSessionExpiry)
      {
        res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,"%d",rsp_data->iMinSessionExpiry);
    
      }
      res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,",");
      
      if(rsp_data->iIncludeRtpRtcpInactivityTimer)
      {
        res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,"%d",(int)rsp_data->iRtpRtcpInactivityTimer);
    
      }
      res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,",");

      if(rsp_data->iIncludeRingingTimerValid)
      {
        res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,"%d",rsp_data->iRingingTimer/1000);
      
      }
      res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,",");
  
      if(rsp_data->iIncludeRingbackTimerValid)
      {
        res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,"%d",rsp_data->iRingbackTimer/1000);
      
      }
      res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,",");
  
      if(rsp_data->iIncludeAmrWbEnable)
      {
        res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,"%d",rsp_data->iAmrWbEnable);
      
      }
      res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,",");
  
  
      if(rsp_data->iIncludeScrAmrEnable)
      {
        res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,"%d",rsp_data->iScrAmrEnable);
      
      }
      res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,",");
  
      if(rsp_data->iIncludeScrAmrWbEnable)
      {
        res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,"%d",rsp_data->iScrAmrWbEnable);
      
      }
      res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,",");
  
      if(rsp_data->iIncludeAmrMode)
      {
        dsat_convert_amrnum_to_amrstr(temp_str,sizeof(temp_str),rsp_data->iAmrMode);
        res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,"\"%s\"",temp_str);
      
      }
      res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,",");
  
      if(rsp_data->iIncludeAmrWbMode)
      {
        dsat_convert_amrnum_to_amrstr(temp_str,sizeof(temp_str),rsp_data->iAmrWbMode);
        res_buff_ptr->used += snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
            res_buff_ptr->size - res_buff_ptr->used,"\"%s\"",temp_str);
      
      }
      dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
      result = DSAT_OK;
    }
    else
    {
      result = DSAT_ERROR;
    }
  }
  SET_PENDING(DSAT_VENDOR_QCVOIPM_IDX ,0, DSAT_PENDING_VOIP_NONE)
  
  return result;
  
}/*dsatvend_voipm_config_handler*/

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCVOIPM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCVOIPM command.
  This command provides the ability to set/get the drx coefficient.


DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command.
    DSAT_ASYNC_CMD : if success.
    DSAT_OK : if it is a success and execution completed.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcvoipm_cmd
(
  dsat_mode_enum_type       mode,             /*  AT command mode:            */
  const dsati_cmd_type     *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,          /*  Command tokens from parser  */
  dsm_item_type            *res_buff_ptr      /*  Place to put response       */
)
{
#define DSAT_UINT16_MAX_NUM           65535
#define DSAT_MIN_SESSION_MINSE_TIMER  90
#define DSAT_MAX_RTCP_TIMER           20
#define DSAT_MIN_RINGER_TIMER         30
#define DSAT_MAX_RINGER_TIMER         45

#define DSAT_MIN_RINGER_BACK_TIMER    35
#define DSAT_MAX_RINGER_BACK_TIMER    50


  dsat_result_enum_type               result     = DSAT_OK;
  ims_settings_at_voip_configuration *ims_config = NULL;
  ims_settings_at_cmd_req_info        voip_params;
  dsat_num_item_type                  arg_val    = 0;

  memset(&voip_params, 0x0 , sizeof(voip_params));
  /*----------------------------------------------------------------
 Standard processing for a command with an argument (WRITE command):
 -----------------------------------------------------------------*/
   if (tok_ptr->op == (NA|EQ|AR))
  {
    
     ims_config = &voip_params.reqData.voipConfig;
/* Check the number of arguments.*/
    if ( tok_ptr->args_found != parse_table->compound)
    {
      DS_AT_MSG1_ERROR("$QCVOIPM: Invalid Number of arguements  %d", tok_ptr->args_found);
      result = DSAT_ERROR;
    }
/*Arg 0  Indicates the session_timer_ims parameter ; Unit is Sec*/
    if( (result == DSAT_OK) && (VALID_TOKEN(0))&& 
        (ATOI_OK == dsatutil_atoi(&arg_val,tok_ptr->arg[0],10))&&
        (arg_val >= DSAT_MIN_SESSION_MINSE_TIMER && arg_val <= DSAT_UINT16_MAX_NUM))
    {
      ims_config->iIncludeSessionExpires = 1;
      ims_config->iSessionExpires        = arg_val;
       
    }else
    {
      DS_AT_MSG0_ERROR("$QCVOIPM: SessionExpires is mandatory ");
      result = DSAT_ERROR;
    }
/*Arg 1  Indicates the value of min_se parameter in seconds ;*/
    if( (result == DSAT_OK) && (VALID_TOKEN(1))&& 
        (ATOI_OK == dsatutil_atoi(&arg_val,tok_ptr->arg[1],10))&&
        (arg_val >= DSAT_MIN_SESSION_MINSE_TIMER && arg_val <= DSAT_UINT16_MAX_NUM))
    {
      ims_config->iIncludeMinSessionExpiry = 1;
      ims_config->iMinSessionExpiry        = arg_val;
        
    }else
    {
      DS_AT_MSG0_ERROR("$QCVOIPM: MinSessionExpiry is mandatory  ");
      result = DSAT_ERROR;
    }
/*Arg 2  Indicates the value of  rtp_rtcp_inactivity_timer_ims parameter in seconds;*/
    if( (result == DSAT_OK) && (VALID_TOKEN(2))&& 
        (ATOI_OK == dsatutil_atoi(&arg_val,tok_ptr->arg[2],10))&&
        (arg_val <= DSAT_MAX_RTCP_TIMER))
    {
      ims_config->iIncludeRtpRtcpInactivityTimer = 1;
      ims_config->iRtpRtcpInactivityTimer        = arg_val;
              
    }else
    {
      DS_AT_MSG0_ERROR("$QCVOIPM: RtpRtcpInactivityTimer is mandatory  ");
      result = DSAT_ERROR;
    }
/*Arg 3  Indicates the value of ringing_timer_ims parameter in seconds*/
    if( (result == DSAT_OK) && (VALID_TOKEN(3))&& 
        (ATOI_OK == dsatutil_atoi(&arg_val,tok_ptr->arg[3],10))&&
        (arg_val >= DSAT_MIN_RINGER_TIMER && arg_val <= DSAT_MAX_RINGER_TIMER))
    {
/* IMS expects value in Ms, convert Sec into Ms*/
      ims_config->iIncludeRingingTimerValid = 1;
      ims_config->iRingingTimer             = arg_val * 1000;
     
    }else
    {
      DS_AT_MSG0_ERROR("$QCVOIPM: RingingTimer is mandatory  ");
      result = DSAT_ERROR;
    }
/*Arg 4 Indicates the  ringback_timer_ims parameter in seconds*/
    if( (result == DSAT_OK) && (VALID_TOKEN(4))&& 
        (ATOI_OK == dsatutil_atoi(&arg_val,tok_ptr->arg[4],10))&&
        (arg_val >= DSAT_MIN_RINGER_BACK_TIMER && arg_val <= DSAT_MAX_RINGER_BACK_TIMER))
    {
/* IMS expect value in Ms, convert Sec into Ms*/
      ims_config->iIncludeRingbackTimerValid = 1;
      ims_config->iRingbackTimer             = arg_val * 1000;
    }else
    {
      DS_AT_MSG0_ERROR("$QCVOIPM: RingbackTimer is mandatory  ");
      result = DSAT_ERROR;
    }
/*Arg 5  Indicates the value of amrwb parameter*/
    if( (result == DSAT_OK) && (VALID_TOKEN(5))&& 
        (ATOI_OK == dsatutil_atoi(&arg_val,tok_ptr->arg[5],10))&&
        (arg_val == 0 || arg_val == 1))
    {
      ims_config->iIncludeAmrWbEnable = 1;
      ims_config->iAmrWbEnable        = arg_val;
       
    }else
    {
      DS_AT_MSG0_ERROR("$QCVOIPM: AmrWbEnable is mandatory  ");
      result = DSAT_ERROR;
    }
/*Arg 6  Indicates the value of scr_amr parameter*/
    if( (result == DSAT_OK) && (VALID_TOKEN(6))&& 
        (ATOI_OK == dsatutil_atoi(&arg_val,tok_ptr->arg[6],10))&&
        (arg_val == 0 || arg_val == 1))
    {
      ims_config->iIncludeScrAmrEnable = 1;
      ims_config->iScrAmrEnable        = arg_val;
       
    }else
    {
      DS_AT_MSG0_ERROR("$QCVOIPM: ScrAmrEnable is mandatory  ");
      result = DSAT_ERROR;
    }
/*Arg 7 Indicates the value of scr_amrwb parameter*/
    if( (result == DSAT_OK) && (VALID_TOKEN(7))&& 
        (ATOI_OK == dsatutil_atoi(&arg_val,tok_ptr->arg[7],10))&&
        (arg_val == 0 || arg_val == 1))
    {
      ims_config->iIncludeScrAmrWbEnable = 1;
      ims_config->iScrAmrWbEnable        = arg_val;
       
    }else
    {
      DS_AT_MSG0_ERROR("$QCVOIPM: ScrAmrWbEnable  is mandatory  ");
      result = DSAT_ERROR;
    }
/*Arg 8 Indicates the value of amrmodeset parameter*/
    if( (result == DSAT_OK) && (VALID_TOKEN(8))&& 
        (TRUE == dsatutil_strip_quotes_out((const byte*)tok_ptr->arg[8],
        (byte *)&ims_config->cAmrModeStr[0],QC_SETTINGS_AT_CMD_VOIP_AMR_MODE_STR_LEN + 1)))
    {
      ims_config->iIncludeAmrModeStr = 1;
       
    }else
    {
      DS_AT_MSG0_ERROR("$QCVOIPM: AmrModeStr is mandatory  ");
      result = DSAT_ERROR;
    }
/*Arg 9 Indicates the value of amrwbmodset parameter*/
    if( (result == DSAT_OK) && (VALID_TOKEN(9))&& 
        (TRUE == dsatutil_strip_quotes_out((const byte*)tok_ptr->arg[9],
        (byte *)&ims_config->cAmrWbModeStr[0],QC_SETTINGS_AT_CMD_VOIP_AMR_WB_MODE_STR_LEN + 1)))
    {
      ims_config->iIncludeAmrWbModeStr = 1;
     
    }else
    {
      DS_AT_MSG0_ERROR("$QCVOIPM: Amr WbModeStr is mandatory  ");
      result = DSAT_ERROR;
    }

    if(result == DSAT_OK)
    {
      voip_params.eCmd           = QC_IMS_SETTINGS_AT_CMD_SET_VOIP_CONFIG_REQ;
      voip_params.pRspCbUserData = (QPVOID *)DSAT_PENDING_VOIP_WRITE;
      voip_params.pRspCallback   = dsatvend_voipm_config_cb;
      
      SET_PENDING(DSAT_VENDOR_QCVOIPM_IDX ,0, DSAT_PENDING_VOIP_WRITE)
      result = DSAT_ASYNC_CMD;
      
      if(AEE_IMS_SUCCESS != ims_settings_at_command_req(voip_params))
      {
        SET_PENDING(DSAT_VENDOR_QCVOIPM_IDX ,0, DSAT_PENDING_VOIP_NONE)
        result = DSAT_ERROR;
      }
    }
    
  }
  /*--------------------------------------  
 Standard processing for a READ command:
 ----------------------------------------*/
        
  else if ( tok_ptr->op == (NA|QU) )
  {
    voip_params.eCmd           = QC_IMS_SETTINGS_AT_CMD_GET_VOIP_CONFIG_REQ;
    voip_params.pRspCbUserData = (QPVOID *)DSAT_PENDING_VOIP_READ;
    voip_params.pRspCallback   = dsatvend_voipm_config_cb;
    
    SET_PENDING(DSAT_VENDOR_QCVOIPM_IDX ,0, DSAT_PENDING_VOIP_READ)
    result = DSAT_ASYNC_CMD;
    
    if(AEE_IMS_SUCCESS != ims_settings_at_command_req(voip_params))
    {
      SET_PENDING(DSAT_VENDOR_QCVOIPM_IDX ,0, DSAT_PENDING_VOIP_NONE)
      result = DSAT_ERROR;
    }
  }
  /*--------------------------------------
Standard processing for a TEST command:
----------------------------------------*/

  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
   /*Command will display current supported range*/
   res_buff_ptr->used = (word) 
    snprintf((char *)res_buff_ptr->data_ptr,
                     res_buff_ptr->size,
                    "$QCVOIPM: (%d - %d),(%d - %d ),(0 - %d),(%d - %d),(%d - %d),(0 - 1),(0 - 1),(0 - 1),(%s),(%s)\n",
                     DSAT_MIN_SESSION_MINSE_TIMER,
                     DSAT_UINT16_MAX_NUM,
                     DSAT_MIN_SESSION_MINSE_TIMER,
                     DSAT_UINT16_MAX_NUM,
                     DSAT_MAX_RTCP_TIMER,
                     DSAT_MIN_RINGER_TIMER,
                     DSAT_MAX_RINGER_TIMER,
                     DSAT_MIN_RINGER_BACK_TIMER,
                     DSAT_MAX_RINGER_BACK_TIMER,
                     "\"0,1,2,3,4,5,6,7,8\"",
                     "\"0,1,2,3,4,5,6,7,8,9\"");
  }
  /*--------------------------------------------------
Command error: Operator(s) not allowed.
----------------------------------------------------*/
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}/*dsatvend_exec_qcvoipm_cmd*/
#endif /* defined(FEATURE_VOIP) && defined(FEATURE_IMS) */
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDRX_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT^QCDRX command.
  This command provides the ability to set/get the drx coefficient.

  <drx_coefficient> drx coefficient, values as follows:
    0 Not specified by MS
    6 CN = 6, T = 32
    7 CN = 7, T = 64
    8 CN = 8, T = 128
    9 CN = 9, T = 256

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command.
    DSAT_ASYNC_CMD : if success.
    DSAT_OK : if it is a success and execution completed.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdrx_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ERROR;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    if ( dsatparm_exec_param_cmd(mode, parse_table, tok_ptr, res_buff_ptr) != DSAT_ERROR )
    {
      cm_set_drx_info_s_type set_drx_info;

      SET_PENDING(DSAT_VENDOR_QCDRX_IDX, 0, DSAT_PENDING_QCDRX_SET);

      result = DSAT_ASYNC_CMD;

      set_drx_info.asubs_id = subs_id;
      set_drx_info.drx_coefficient = (sys_drx_cn_coefficient_s1_e_type)
        dsatutil_get_val ( DSAT_VENDOR_QCDRX_IDX, 0, 0, NUM_TYPE );
      if ( cm_ph_cmd_set_drx_req_per_subs ( dsatcmif_ph_cmd_cb_func,
                                            NULL,
                                            dsatcm_client_id,
                                            &set_drx_info ) == FALSE )
      {
        DS_AT_MSG0_ERROR("No buffer to send request");

        SET_PENDING(DSAT_VENDOR_QCDRX_IDX, 0, DSAT_PENDING_QCDRX_NONE);

        result = DSAT_ERROR;
      }
    }
  }
  else if ( tok_ptr->op == (NA|QU) )
  {
    cm_get_drx_info_s_type get_drx_info;

    SET_PENDING(DSAT_VENDOR_QCDRX_IDX, 0, DSAT_PENDING_QCDRX_GET);

    result = DSAT_ASYNC_CMD;

    get_drx_info.asubs_id = subs_id;
    if ( cm_ph_cmd_get_drx_req_per_subs ( dsatcmif_ph_cmd_cb_func,
                                          NULL,
                                          dsatcm_client_id,
                                          &get_drx_info ) == FALSE )
    {
      DS_AT_MSG0_ERROR("No buffer to send request");

      SET_PENDING(DSAT_VENDOR_QCDRX_IDX, 0, DSAT_PENDING_QCDRX_NONE);

      result = DSAT_ERROR;
    }
  }
  else if ( tok_ptr->op == (NA|EQ|QU) )
  {
    result = dsatparm_exec_param_cmd(mode, parse_table, tok_ptr, res_buff_ptr);
  }

  return result;
}/* dsatvend_exec_qcdrx_cmd */

/*===========================================================================
FUNCTION DSATVEND_PROCESS_QCDRX_CMD

DESCRIPTION
  This function process the response for the $QCDRX command on return from
  asynchronous processing.
 
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command.
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_process_qcdrx_cmd
(
  dsat_cmd_pending_enum_type cmd_type,  /* cmd type */
  ds_at_ph_info_u_type  *ph_info        /* PH info */
)
{
  dsat_result_enum_type result = DSAT_ERROR;

  SET_PENDING(DSAT_VENDOR_QCDRX_IDX, 0, DSAT_PENDING_QCDRX_NONE);

  switch ( cmd_type )
  {
    case DSAT_PENDING_QCDRX_SET:
      {
        if ( ph_info->set_drx_result == TRUE )
        {
          result = DSAT_OK;
        }
      }
      break;

    case DSAT_PENDING_QCDRX_GET:
      {
        dsm_item_type *res_buff_ptr;
  
        res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL, FALSE);
        res_buff_ptr->used = (word) snprintf ( (char *) res_buff_ptr->data_ptr,
                                               res_buff_ptr->size,
                                               "$QCDRX: %d",
                                               ph_info->drx_coefficient );
  
        dsatcmdp_handle_async_cmd_rsp(res_buff_ptr, DSAT_COMPLETE_RSP);
  
        result = DSAT_OK;
      }
      break;

    default:
      DS_AT_MSG1_ERROR("Invalid cmd type %d", cmd_type);
  }

  return result;
}/* dsatvend_process_qcdrx_cmd */

/*===========================================================================
FUNCTION DSATVEND_EXEC_QCRSRP_CMD

DESCRIPTION
  This function executes $QCRSRP and $QCRSRQ commands and provides response 
  Calls CM API to receive neighbour cell info like Cell id,EARFCN,RSRP,RSRQ.
  RSRP - Reference Signal Received Power
  RSRQ - Reference Signal Received Quality
 
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command.
    DSAT_OK :    if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcrsrp_cmd
(
    dsat_mode_enum_type mode,             /*  AT command mode:            */
    const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
    const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
    dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  #define CMD_RSRP 0
  #define CMD_RSRQ 1
  #define MAX_RSR_COUNT CMAPI_LTE_NGBR_IND_NUM_CELLS
  
  dsat_result_enum_type result = DSAT_ERROR;
  cmapi_err_e_type err_wcdma_mode = CMAPI_SUCCESS;
  cmapi_err_e_type err_lte_mode = CMAPI_SUCCESS;
  cmapi_err_e_type err_cmapi = CMAPI_SUCCESS;
  cmapi_rat_meas_info_s_type *meas_info_ptr = NULL;
  boolean cmd = 0;
  int i = 0,j = 0;
  int rsr_count = 0;
  int16 value = 0;

  /* Supported only NA|QU */
  if( tok_ptr->op != (NA|QU) )
  {
    return result;
  }
  
  res_buff_ptr->data_ptr[0] = '\0';
  if (0 == strncmp((const char *)parse_table->name,(const char *)"$QCRSRP",strlen((const char *)"$QCRSRP")))
  {
    res_buff_ptr->used = (word)snprintf((char*)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,
                                                "$QCRSRP: ");
    cmd = CMD_RSRP;
  }
  else if(0 == strncmp((const char *)parse_table->name,(const char *)"$QCRSRQ",strlen((const char *)"$QCRSRQ")))
  {
    res_buff_ptr->used = (word)snprintf((char*)res_buff_ptr->data_ptr,
                                                res_buff_ptr->size,
                                                "$QCRSRQ: ");
    cmd = CMD_RSRQ;
  }
  else
  {
    return result;
  }

  meas_info_ptr = (cmapi_rat_meas_info_s_type *)dsat_alloc_memory(sizeof(cmapi_rat_meas_info_s_type),FALSE);

  //get LTE Cell info
  err_lte_mode = cmapi_get_rat_meas_info( CMAPI_SYS_MODE_LTE, meas_info_ptr );
  if( err_lte_mode == CMAPI_SUCCESS )
  {
    if( meas_info_ptr->is_service_available && (meas_info_ptr->sys_mode == CMAPI_SYS_MODE_LTE) &&
         meas_info_ptr->meas_info_u.lte_nbr_info.valid )
    {
      cmapi_lte_ngbr_ind_s* nbr_info = &meas_info_ptr->meas_info_u.lte_nbr_info;
     /*lte intra start: in intra cell nbr info each cell different rsrp and rsrq values get the info for all cells */
      DS_AT_MSG1_HIGH("LTE: num intra num cells: %d", nbr_info->lte_intra.num_lte_cells);
      for( i = 0; i<nbr_info->lte_intra.num_lte_cells && i < CMAPI_LTE_NGBR_IND_NUM_CELLS; i++ )
      {
        value = (cmd == CMD_RSRP) ? nbr_info->lte_intra.cell[i].rsrp : nbr_info->lte_intra.cell[i].rsrq;
        if( cmd == CMD_RSRP )
        {
          res_buff_ptr->used += (word)snprintf(
                                (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                res_buff_ptr->size - res_buff_ptr->used,
                                "%3.3d,%lu,\"%3.3d.%d0\",",
                                nbr_info->lte_intra.cell[i].pci,
                                (uint32)nbr_info->lte_intra.earfcn,
                                value/10,
                                DSAT_ABSOLUTE_VAL(value)%10);
        }
        else
        {
          res_buff_ptr->used += (word)snprintf(
                                   (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                    res_buff_ptr->size - res_buff_ptr->used,
                                   "%3.3d,%lu,\"%2.2d.%d0\"," ,
                                    nbr_info->lte_intra.cell[i].pci,
                                    (uint32)nbr_info->lte_intra.earfcn,
                                    value/10,
                                    DSAT_ABSOLUTE_VAL(value)%10);
        }
          rsr_count++;
      }
      /*lte intra end*/

      /*lte inter start :in the inter  cell nbr info multriple ferequencies are available hence collect all cells info for each frequency*/
      DS_AT_MSG2_HIGH("LTE: num inter num freqs: %d current rsr_count = %d", nbr_info->lte_inter.num_freqs,rsr_count);
      for( i=0; ( i<nbr_info->lte_inter.num_freqs && i < CMAPI_LTE_NGBR_IND_NUM_FREQS) &&
                ( rsr_count < MAX_RSR_COUNT); i++ )
      {
        DS_AT_MSG1_HIGH("LTE: num inter num cells: %d", nbr_info->lte_inter.freqs[i].num_lte_cells);
        
        for( j=0; ( j<nbr_info->lte_inter.freqs[i].num_lte_cells && j < CMAPI_LTE_NGBR_IND_NUM_CELLS) && 
                  ( rsr_count < MAX_RSR_COUNT); j++ )
        {
            value = (cmd == CMD_RSRP) ? nbr_info->lte_inter.freqs[i].cells[j].rsrp : nbr_info->lte_inter.freqs[i].cells[j].rsrq;
          if( cmd == CMD_RSRP )
          {
            res_buff_ptr->used += (word)snprintf(
                                   (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                   res_buff_ptr->size - res_buff_ptr->used,
                                   "%3.3d,%lu,\"%3.3d.%d0\"," ,
                                   nbr_info->lte_inter.freqs[i].cells[j].pci,
                                   (uint32)nbr_info->lte_inter.freqs[i].earfcn,
                                   value/10,
                                   DSAT_ABSOLUTE_VAL(value)%10);
          }
          else
          {
            res_buff_ptr->used += (word)snprintf(
                                   (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                   res_buff_ptr->size - res_buff_ptr->used,
                                   "%3.3d,%lu,\"%2.2d.%d0\",",
                                   nbr_info->lte_inter.freqs[i].cells[j].pci,
                                   (uint32)nbr_info->lte_inter.freqs[i].earfcn,
                                   value/10,
                                   DSAT_ABSOLUTE_VAL(value)%10);
          }
            rsr_count++;
        }
      }
      /*lte inter enter*/
    }
    else
    {
      DS_AT_MSG0_MED("LTE info is not valid");
    }
  }
  else
  {
    DS_AT_MSG0_ERROR("cmapi for meas info failed in CMAPI_SYS_MODE_LTE ");
  }

  /*Get WCDMA cell Info: here we are collecting LTE nbr cells of current cell(wcdma) 
     .their might be multiple earfcn nbr info hence collect cell info for each earfcn*/
  err_wcdma_mode = cmapi_get_rat_meas_info( CMAPI_SYS_MODE_WCDMA, meas_info_ptr);
  if( (err_wcdma_mode == CMAPI_SUCCESS) && ( rsr_count < MAX_RSR_COUNT))
  {
    if( meas_info_ptr->is_service_available && (meas_info_ptr->sys_mode == CMAPI_SYS_MODE_WCDMA) &&
      meas_info_ptr->meas_info_u.wcdma_nbr_info.is_data_valid )
    {
     cmapi_wcdma_signal_info_struct_type* nbr_info = &meas_info_ptr->meas_info_u.wcdma_nbr_info;
     for( i=0; (i< nbr_info->cmapi_wcdma_lte_cell_info.num_earfcn && i < CMAPI_WCDMA_SRCH_LTE_EARFCN_MAX) &&
                ( rsr_count < MAX_RSR_COUNT) ; i++ )
     {
       DS_AT_MSG1_HIGH("WCDMA info : num cells = %d",nbr_info->cmapi_wcdma_lte_cell_info.lte_earfcn[i].num_cells);
       for( j=0; ( j< nbr_info->cmapi_wcdma_lte_cell_info.lte_earfcn[i].num_cells && i < CMAPI_WCDMA_MAX_CELLS_REPORTED) &&
                  ( rsr_count < MAX_RSR_COUNT); j++ )
       {
            value = (cmd == CMD_RSRP) ? 
            	        nbr_info->cmapi_wcdma_lte_cell_info.lte_earfcn[i].cell[j].rsrp :
            	        nbr_info->cmapi_wcdma_lte_cell_info.lte_earfcn[i].cell[j].rsrq ;
         if(cmd == CMD_RSRP)
         {
            res_buff_ptr->used += (word)snprintf(
                                   (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                   res_buff_ptr->size - res_buff_ptr->used,
                                   "%3.3d,%lu,\"%3.3d.%d0\"," ,
                                   nbr_info->cmapi_wcdma_lte_cell_info.lte_earfcn[i].cell[j].cell_id,
                                   (uint32)nbr_info->cmapi_wcdma_lte_cell_info.lte_earfcn[i].earfcn,
                                   value/10,
                                   DSAT_ABSOLUTE_VAL(value)%10);
         }
         else
         {
            res_buff_ptr->used += (word)snprintf(
                                   (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                   res_buff_ptr->size - res_buff_ptr->used,
                                   "%3.3d,%lu,\"%2.2d.%d0\",",
                                   nbr_info->cmapi_wcdma_lte_cell_info.lte_earfcn[i].cell[j].cell_id,
                                   (uint32)nbr_info->cmapi_wcdma_lte_cell_info.lte_earfcn[i].earfcn,
                                   value/10,
                                   DSAT_ABSOLUTE_VAL(value)%10);
         }
            rsr_count++;
        }
      }
    }
    else
    {
      DS_AT_MSG0_MED("WCDMA info is not valid");
    }
  }
  else
  {
    DS_AT_MSG0_ERROR("cmapi for meas info failed in CMAPI_SYS_MODE_WCDMA ");
  }
  if( rsr_count )
  {
    res_buff_ptr->used--;
  }
  /* if api returns failure for both systems then we need to disaply error message*/
  if( err_wcdma_mode != CMAPI_SUCCESS && err_lte_mode != CMAPI_SUCCESS)
  {
    res_buff_ptr->used = 0;
    res_buff_ptr->data_ptr[0] = '\0';
    if(dsatcmdp_get_current_mode() == DSAT_MODE_WCDMA)
    {
      err_cmapi = err_wcdma_mode;
    }
    else
    {
      err_cmapi = err_lte_mode;
    }
    switch( err_cmapi )
    {
      case CMAPI_NO_SERVICE:
        result = dsat_send_cme_error(DSAT_CME_NO_NETWORK_SERVICE);
        break;
      default:
        DS_AT_MSG1_MED("CMAPI Error : %d",err_cmapi);
        result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
        break;
    }
  }
  /* if api call is successful and data is not valid display error*/
  else if( meas_info_ptr->meas_info_u.wcdma_nbr_info.is_data_valid == 0 && meas_info_ptr->meas_info_u.lte_nbr_info.valid == 0)
  {
    res_buff_ptr->used = 0;
    res_buff_ptr->data_ptr[0] = '\0';
    result = DSAT_ERROR;
  }
  /* if both or any one of the api calls are successful then return success.*/
  else
  {
    result = DSAT_OK;
  }
  dsatutil_free_memory((void *)meas_info_ptr);
  return result;
}
#endif /* FEATURE_DSAT_LTE */

#ifdef FEATURE_SGLTE
/*===========================================================================

FUNCTION DSATVEND_EXEC_QCHCOPS_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCHCOPS command, a read only command and a version of
  +COPS read command for hybrid stack.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP: if there was a problem in lower layers
    DSAT_ASYNC_CMD : if aynchronous command continues successfully
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qchcops_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ERROR;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  if ((tok_ptr->op == (NA|QU)) && (TRUE == dsat_subs_info[subs_id].is_sglte_sub))
  {
    dsatcmif_servs_state_ms_info *ph_ss_ms_val = NULL;
    dsat_net_reg_state_s_type    *net_reg_state_ptr = NULL;
    net_id_info_s_type           *net_id_info_ptr = NULL;
    dsat_num_item_type            mode_val;
    dsat_cops_fmt_e_type          fmt;
    sys_modem_as_id_e_type        hybr_subs_id;

    hybr_subs_id = dsatcmif_get_sglte_hybr_subs_id();
    ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS, hybr_subs_id, FALSE);

    net_reg_state_ptr = &ph_ss_ms_val->dsat_net_reg_state;
    net_id_info_ptr = &net_reg_state_ptr->net_id_info;

    mode_val = (dsat_num_item_type) dsatutil_get_val( DSATETSI_EXT_ACT_COPS_ETSI_IDX,
                                      subs_id, 0, MIX_NUM_TYPE);

    fmt = (dsat_num_item_type) dsatutil_get_val( DSATETSI_EXT_ACT_COPS_ETSI_IDX,
                                 subs_id, 1, MIX_NUM_TYPE);

    if ( FALSE == net_id_info_ptr->present )
    {
      res_buff_ptr->used = (word) snprintf( (char *) res_buff_ptr->data_ptr,
                                            res_buff_ptr->size,
                                            "%s: %d",
                                            parse_table->name,
                                            mode_val );

      result = DSAT_OK;
    }
    else if ( fmt == DSAT_COPS_FMT_PLMN )
    {
      boolean plmn_undefined = FALSE;
      sys_mcc_type mcc = 0;
      sys_mnc_type mnc = 0;
      boolean pcs_flag = FALSE;

      sys_plmn_get_mcc_mnc( net_id_info_ptr->plmn,
                            &plmn_undefined,
                            &pcs_flag,
                            &mcc,
                            &mnc );

      /* PLMN may be undefined */
      if (TRUE == plmn_undefined)
      {
        res_buff_ptr->used = (word) snprintf ( (char *) res_buff_ptr->data_ptr,
                                               res_buff_ptr->size,
                                               "%s: %d,%d,\"\",%d",
                                               (const char *) parse_table->name,
                                               mode_val,
                                               fmt,
                                               net_reg_state_ptr->act );
      }
      else
      {
        if ( pcs_flag )
        {
          /* Print MNC as 3 digits */
          res_buff_ptr->used = (word) snprintf ( (char *) res_buff_ptr->data_ptr,
                                                 res_buff_ptr->size,
                                                 "%s: %d,%d,\"%03lu%03lu\",%d",
                                                 (const char *) parse_table->name,
                                                 mode_val,
                                                 fmt,
                                                 mcc, mnc,
                                                 net_reg_state_ptr->act );
        }
        else
        {
          res_buff_ptr->used = (word) snprintf ( (char *) res_buff_ptr->data_ptr,
                                                 res_buff_ptr->size,
                                                 "%s: %d,%d,\"%03lu%02lu\",%d",
                                                 (const char *) parse_table->name,
                                                 mode_val,
                                                 fmt,
                                                 mcc, mnc,
                                                 net_reg_state_ptr->act );
        }
      }

      result = DSAT_OK;
    }
    else
    {
#ifdef FEATURE_MMGSDI
      mmgsdi_plmn_id_list_type plmn_id_list;
      mmgsdi_plmn_id_type plmn_id;

      plmn_id_list.num_of_plmn_ids = 1;

      (void) dsatutil_memscpy( (void *) &plmn_id.plmn_id_val[0], MMGSDI_PLMN_ID_SIZE,
                               (void *) &net_id_info_ptr->plmn.identity[0],
                               MMGSDI_PLMN_ID_SIZE );

      plmn_id.csg_id = 0xFFFFFFFF;

      switch (net_reg_state_ptr->sys_mode)
      {
        case SYS_SYS_MODE_WCDMA:
          plmn_id.rat = MMGSDI_RAT_UMTS;
          break;
#ifdef FEATURE_TDSCDMA
        case SYS_SYS_MODE_TDS:
#endif /* FEATURE_TDSCDMA */
          plmn_id.rat = MMGSDI_RAT_TDS;
          break;

        case SYS_SYS_MODE_LTE:
          plmn_id.rat = MMGSDI_RAT_LTE;
          break;

        case SYS_SYS_MODE_GSM:
          plmn_id.rat = MMGSDI_RAT_GSM;
          break;

        default:
          plmn_id.rat = MMGSDI_RAT_NONE;
          break;
      }

      plmn_id_list.plmn_list_ptr = &plmn_id;

      ph_ss_ms_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_GET_NAME;

      if( MMGSDI_SUCCESS == mmgsdi_session_get_operator_name (
                dsat_get_current_gw_session_id(FALSE),
                plmn_id_list,
                dsatme_mmgsdi_get_operator_name_cb,
                DSAT_MMGSDI_GET_CURR_HYBR_OPER_NAME) )
      {
        result = DSAT_ASYNC_CMD;
      }
      else
      {
        ph_ss_ms_val->dsatetsicall_network_list.cmd_state = DSAT_COPS_ASTATE_NULL;
        ph_ss_ms_val->dsatetsicall_network_list.cmd_idx = CMD_IDX_NONE;
        DS_AT_MSG0_ERROR("mmgsdi_session_get_operator_name API call failed");
        dsatme_set_cme_error(DSAT_CME_PHONE_FAILURE, res_buff_ptr);
        result = DSAT_CMD_ERR_RSP;
      }
#endif /* FEATURE_MMGSDI */
    }
  }

  return result;
} /* dsatvend_exec_qchcops_cmd */

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCHCREG_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes
  it. It executes $QCHCREG command, a read only command and a version of
  +CREG read command for hybrid stack.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success for local execution

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_qchcreg_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
#define CREG_DISP_LAC_CELL_ID (2)
  dsat_result_enum_type result = DSAT_ERROR;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  if ((tok_ptr->op == (NA|QU)) && (TRUE == dsat_subs_info[subs_id].is_sglte_sub) )
  {
    dsatcmif_servs_state_ms_info *ph_ss_ms_val = NULL;
    dsat_net_reg_state_s_type    *net_reg_state_ptr = NULL;
    dsat_creg_result_s_type      *creg_reported_ptr = NULL;
    dsat_num_item_type            creg_n;
    char                          fmt_string[70];
    sys_modem_as_id_e_type        hybr_subs_id;

    hybr_subs_id = dsatcmif_get_sglte_hybr_subs_id();
    ph_ss_ms_val = dsat_get_base_addr_per_subs(DSAT_SS_PH_MS_VALS, hybr_subs_id, FALSE);

    net_reg_state_ptr = &ph_ss_ms_val->dsat_net_reg_state;
    creg_reported_ptr = &net_reg_state_ptr->dsat_creg_reported;

    creg_n = (dsat_num_item_type) dsatutil_get_val( DSATETSI_EXT_CREG_IDX,
                                    subs_id, 0, NUM_TYPE );

    res_buff_ptr->used =
      (word)snprintf((char*)res_buff_ptr->data_ptr,res_buff_ptr->size,
                            "%s: %d,%d",
                            parse_table->name,
                            creg_n,
                            creg_reported_ptr->net_domain[DSAT_NET_REG_DOMAIN_CS]);

    /* location id, cell id is required only when CREG = 2*/
    /* Cell ID should be in  hex format */
    /* Display Cell id and location id only when camped */
    if ( creg_n == CREG_DISP_LAC_CELL_ID )
    {
      /* $QCHCREG requires only lac and cell_id */
      if ( ( ( creg_reported_ptr->net_domain[DSAT_NET_REG_DOMAIN_CS] == DSAT_NET_REG_HOME ) ||
             ( creg_reported_ptr->net_domain[DSAT_NET_REG_DOMAIN_CS] == DSAT_NET_REG_ROAMING ) ) &&
           ( ( creg_reported_ptr->cell_id != 0xFFFFFFFF ) &&
             ( creg_reported_ptr->plmn_lac != 0XFFFF ) ) )
      {
        (void) snprintf( fmt_string,
                         sizeof(fmt_string),
                         ",\"%X\",\"%X\",%X",
                         creg_reported_ptr->plmn_lac,
                         (unsigned int) creg_reported_ptr->cell_id,
                         net_reg_state_ptr->act );

        res_buff_ptr->used +=
          (word) snprintf( (char*) &res_buff_ptr->data_ptr[res_buff_ptr->used],
                           res_buff_ptr->size - res_buff_ptr->used,
                           "%s", fmt_string );
      }
    }

    result = DSAT_OK;
  }

  return result;
} /* dsatvend_exec_qchcreg_cmd */
#endif /* FEATURE_SGLTE */

/*===========================================================================

FUNCTION DSATVEND_REMOVE_BLANK_SPACES

DESCRIPTION
This function takes char string as an argument and returns pointer to
the string without blank spaces

DEPENDENCIES
  None

RETURN VALUE
  Pointer to resultant string.

SIDE EFFECTS
  None

===========================================================================*/

void dsatvend_remove_blank_spaces
(
  char*          input
)
{
  char *output=input;
  int i;
  uint8 j;
  boolean initial_spaces = TRUE;
 
  i = strlen(input) - 2;
  
  while( i >= 0 && input[i] == ' ')
  {
    input[i--] = 0;           /*Removing spaces from the end*/
  }
  
  for ( i = 0, j = 0; i < (int) strlen(input); i++, j++)
  {
    if( initial_spaces == TRUE && input[i] == ' ')
    {
      j--;
      continue;
    }
  
    initial_spaces = FALSE;
    output[j] = input[i];
  }
  output[j] = 0;
}/*dsatvend_remove_blank_spaces*/
/*===========================================================================

FUNCTION DSATVEND_GET_PARAM_TYPE_VALUE

DESCRIPTION
This function processes the argument passed and tokenizes it to get parameter
type and value.

DEPENDENCIES
  None

RETURN VALUE
    DSAT_SUCCESS : if the tokens are successfuly created
    DSAT_FAILURE : if an invalid argument is passed

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatvend_get_param_type_value
(
  const char         *arg,                /* Argument passed in the form A:B */
  char               *param_type,         /* Parameter Type (A) */
  uint8               type_size,          /* Maximum size of Parameter Type */
  char               *param_value,        /* Parameter Value (B) */ 
  uint8               value_size          /* Maximum size of Parameter Value */
)
{
  char *type   =   param_type;
  char *value  =   param_value;
  /*Save KWs*/
  if(NULL == arg || NULL == param_type || NULL == param_value)
  {
    return DSAT_FAILURE;
  }
  while( type_size != 0  && *arg != ':' && *arg != '\0' )
  {
    *param_type++ = *arg++ ;
     type_size = type_size - 1;
  }
/*Valid terminator found ':' ; Terminate Param type*/
  if( *arg == ':' )
  {
    *param_type++ = '\0';
     arg++; /* Move to Next char in the String */
  }
  else
  {
    DS_AT_MSG1_ERROR("$QCPRFCRT: Invalid Tokens Name,Remain Size  %d", type_size);
    return DSAT_FAILURE;
  }

  while(value_size != 0 && *arg != '\0' )
  {
    *param_value++ = *arg++ ;
     value_size = value_size - 1;
  }
/* Valid  terminator found'\0' ; Terminate Param value*/
  if( *arg == '\0' )
  {
    *param_value = '\0';
  }
  else
  {
    DS_AT_MSG1_ERROR("$QCPRFCRT: Invalid Tokens value,Remain Size  %d", value_size);
    return DSAT_FAILURE;
  }

  dsatvend_remove_blank_spaces(type);
  dsatvend_remove_blank_spaces(value);
  
  /* Validating Param type and Value after removing white space*/
  if(strlen(type) == 0 || strlen(value) == 0)
  {
    DS_AT_MSG2_ERROR("$QCPRFCRT: Invalid Tokens String,Name Size %d Value Size", 
                      strlen(type),strlen(value));
    return DSAT_FAILURE;
  }
  return DSAT_SUCCESS;
}/*dsatvend_get_param_type_value*/

/*===========================================================================
  FUNCTION DSATVEND_DISPLAY_QCPRFMOD_TEST_RESULT
 
  DESCRIPTION
  It displays $QCPRFMOD test command results
 
  PARAMETERS

  RETURN VALUE
  
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
===========================================================================*/
LOCAL dsat_result_enum_type dsatvend_display_qcprfmod_test_result
(
  dsm_item_type *res_buff_ptr
)
{
  uint8       idx;
  ds_profile_tech_etype     tech_idx = DS_PROFILE_TECH_3GPP;
  dsat_profile_parameter_details_s_type *param_list;
  uint8                                  param_list_size;
  uint8                                  i;

  while( tech_idx != DS_PROFILE_TECH_INVALID)
  {
    if( tech_idx == DS_PROFILE_TECH_3GPP )
    {
      param_list = &profile_3gpp_parameter_details_list[0];
      param_list_size = ARR_SIZE(profile_3gpp_parameter_details_list);
      tech_idx = DS_PROFILE_TECH_3GPP2;
    }
    else if( tech_idx == DS_PROFILE_TECH_3GPP2 )
    {
      param_list = &profile_3gpp2_parameter_details_list[0];
      param_list_size = ARR_SIZE(profile_3gpp2_parameter_details_list);
      tech_idx = DS_PROFILE_TECH_EPC;
    }
    else if (tech_idx == DS_PROFILE_TECH_EPC)
    {
      param_list = &profile_epc_parameter_details_list[0];
      param_list_size = ARR_SIZE(profile_epc_parameter_details_list);
      tech_idx = DS_PROFILE_TECH_EPC+1;
    }
    else
    {    /*Print general parameters*/
      param_list = &profile_general_parameter_details_list[0];
      param_list_size = ARR_SIZE(profile_general_parameter_details_list);
      tech_idx = DS_PROFILE_TECH_INVALID ; 
    }

    for(idx = 0; idx < param_list_size; idx++)
    {
      CHECK_APPEND_RESPONSE_BUFFER(res_buff_ptr->size - 30)
      switch(param_list[idx].type)
      {
        case NUM_TYPE:
        case LNGR_TMR_TYPE:
        {
          res_buff_ptr->used += (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                   res_buff_ptr->size - res_buff_ptr->used,
                                               "\n%s:(",param_list[idx].abbrv);
          
          for(i = 0;!( param_list[idx].range[i].lower == RANGE_END &&
                       param_list[idx].range[i].upper == RANGE_END ); i++)
          {
            if ( param_list[idx].range[i].lower == param_list[idx].range[i].upper)
            {
              res_buff_ptr->used += (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                    res_buff_ptr->size - res_buff_ptr->used,
                                    "%lu,",param_list[idx].range[i].lower);
            }
            else
            {
              res_buff_ptr->used += (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                     res_buff_ptr->size - res_buff_ptr->used,
                                                     "%lu-%lu,",
                                                     param_list[idx].range[i].lower,
                                                     param_list[idx].range[i].upper );
            }
          }
          res_buff_ptr->used--;
          res_buff_ptr->used += (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                 res_buff_ptr->size - res_buff_ptr->used,
                                                 ")");
          break;
        }
        case STR_TYPE:
        case ADDR_V4_TYPE:
        case ADDR_C_V6_TYPE:
        case ADDR_P_V6_TYPE:
        case ADDR_P2_V6_TYPE:
        {
          /* Both 3GPP APN name and 3GPP2 APN name will  point to same value.(General Param)
                    Skipping P2 APN name*/
          if( 0 == dsatutil_strcmp_ig_sp_case((byte*)"P2APNNAME",(byte*)param_list[idx].abbrv) )
          {
            continue;
          }
          if( param_list[idx].identifier == DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN )
          {
            res_buff_ptr->used += (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                   res_buff_ptr->size - res_buff_ptr->used,
                                                  "\nAPNNAME:\"\"");
          }
          else
          {
             res_buff_ptr->used += (word) snprintf((char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                    res_buff_ptr->size - res_buff_ptr->used,
                                                   "\n%s:\"\"",param_list[idx].abbrv);
          }
          break;
        }
        default:
         break;
      }
    }
  }
  return 0;
}/* dsatvend_display_qcprfmod_test_result */

/*===========================================================================

FUNCTION DSATVEND_3GPP2_PROFILE_CREATE_GET_PARAM_INFO

DESCRIPTION
This function takes an argument and calculates param_identifier and 
corressponding param_value.
DEPENDENCIES
  None

RETURN VALUE
    DSAT_SUCCESS : if param_identifier and param_value is found correponding to 
                    param_type
                   else
    DSAT_FAILURE : 

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatvend_3gpp2_profile_create_get_param_info
(
  const tokens_struct_type                      *tok_ptr,                    /* Parameter Type (A) */
  dsat_3gpp2_profile_create_parameter_s_type    *param
)
{
  uint8                index = 0;
  uint8                count = 0;
  char                 param_type[DSAT_3GPP2_PARAM_TYPE_MAX_LENGTH];
  char                 param_value[DSAT_3GPP2_PARAM_VALUE_MAX_LENGTH];
  boolean              result = DSAT_SUCCESS;
  dsat_3gpp2_profile_create_param_e_type    param_identifier;
  dsat_num_item_type                        value; 
  
  for(count = 0 ; count< tok_ptr->args_found; count++ )
  {
    memset(param_type, 0x0, DSAT_3GPP2_PARAM_TYPE_MAX_LENGTH);
    memset(param_value, 0x0, DSAT_3GPP2_PARAM_VALUE_MAX_LENGTH);
    
    if( !VALID_TOKEN(count) || 
        (DSAT_FAILURE == dsatvend_get_param_type_value( (char*) tok_ptr->arg[count],
                                                       (char*)&param_type[0],
                                                        DSAT_3GPP2_PARAM_TYPE_MAX_LENGTH,
                                                       (char*)&param_value[0],
                                                        DSAT_3GPP2_PARAM_VALUE_MAX_LENGTH)) )
    {
      DS_AT_MSG1_ERROR("Parsing failed for Argument %d",count);
      return DSAT_FAILURE;
    }
    
    for( index = 0; index < ARR_SIZE(dsat_3gpp2_profile_param_abbrv_type); index++ )
    {
      /*find index*/
      if (0 == dsatutil_strcmp_ig_sp_case( (byte*) param_type,
                  (byte*) dsat_3gpp2_profile_param_abbrv_type[index].param_abbrv ) )
      {
        break;
      }
    }
    if( index == ARR_SIZE(dsat_3gpp2_profile_param_abbrv_type) )
    {
      DS_AT_MSG1_ERROR("Invalid param_type for arg num %d",count);
      return DSAT_FAILURE;
    }
      
    param_identifier = dsat_3gpp2_profile_param_abbrv_type[index].param_type;

    if (DSAT_3GPP2_PARAM_APN_NAME != dsat_3gpp2_profile_param_abbrv_type[index].param_type &&
        DSAT_3GPP2_PARAM_PDP_TYPE != dsat_3gpp2_profile_param_abbrv_type[index].param_type)
    {
      if(ATOI_OK != dsatutil_atoi((dsat_num_item_type*)&value,(byte*)&param_value[0], 10 ))
      {
        DS_AT_MSG1_ERROR("ARG convertion failed %d",count);
        return DSAT_FAILURE;
      }
      switch( param_identifier)
      {
        case DSAT_3GPP2_PARAM_OPRT_TYPE:
        {
          if( value == DSAT_3GPP2_PROFILE_CREATE || value == DSAT_3GPP2_PROFILE_DELETE )
          {
            param->param_present =  param->param_present | DSAT_PROFILE_CREATE_MASK_OPRT;  
            param->oprt_val= (uint8)value;
          }
          else
          {
            DS_AT_MSG1_ERROR("ARG out of range %d",value);
            return DSAT_FAILURE;
          }
          break;
        }
      
        case DSAT_3GPP2_PARAM_TECH_TYPE:
        {
          if( (ds_profile_tech_etype) value == DS_PROFILE_TECH_3GPP2 || 
              (ds_profile_tech_etype) value == DS_PROFILE_TECH_EPC )
          {
            param->param_present = param->param_present | DSAT_PROFILE_CREATE_MASK_TECH;
            param->tech_val= (uint8) value;
          }
          else
          {
            DS_AT_MSG1_ERROR("ARG out of range %d",value);
            return DSAT_FAILURE;
          }
          break;
        }
        
        case DSAT_3GPP2_PARAM_PERSISTENT:
        {
          if( value == DSAT_PERSISTENT || value == DSAT_NON_PERSISTENT )
          {
            param->param_present = param->param_present | DSAT_PROFILE_CREATE_MASK_PERSIST;
            param->persistent_val= (uint8) value;
          }
          else
          {
            DS_AT_MSG1_ERROR("ARG out of range %d",value);
            return DSAT_FAILURE;
          }
          break;
        }
        case DSAT_3GPP2_PARAM_PROFILE_ID:
        {
          param->param_present = param->param_present | DSAT_PROFILE_CREATE_MASK_PROFILE_ID;
          param->profile_id_val= (uint8) value;        
          break;
        }
        default: 
          return DSAT_FAILURE;
        }
      }
      else
      {
        switch( param_identifier )
        {
         case DSAT_3GPP2_PARAM_APN_NAME:
         {
           if (strlen(param_value) < DS_PROFILE_3GPP2_APN_MAX_VAL_LEN+1)
           {
             param->param_present = param->param_present | DSAT_PROFILE_CREATE_MASK_APN_NAME;
             memset(param->apn_name,0x0, DS_PROFILE_3GPP2_APN_MAX_VAL_LEN);
             dsatutil_memscpy((void *)param->apn_name, DS_PROFILE_3GPP2_APN_MAX_VAL_LEN, 
                              (void *)param_value, strlen(param_value));
           }
           else
           {
             DS_AT_MSG1_ERROR("ARG out of range %d",strlen(param_value));
             return DSAT_FAILURE;
           }
           break;
         }
  
         case DSAT_3GPP2_PARAM_PDP_TYPE:
         {
           if( strlen(param_value) < DSAT_MAX_PDP_TYPE_LEN+2 )
           {
             /*Copy the the buffer, as it will be converted to int later*/
             param->param_present = param->param_present | DSAT_PROFILE_CREATE_MASK_PDP_TYPE;
             memset(param->pdp_type,0x0, DSAT_MAX_PDP_TYPE_LEN);
             dsatutil_memscpy((void *) param->pdp_type, DSAT_MAX_PDP_TYPE_LEN, 
              (void *) param_value, strlen(param_value));
           }
           else
           {
             DS_AT_MSG1_ERROR("ARG out of range %d",strlen(param_value));
             return DSAT_FAILURE;
           }
           break;
         }
        default: 
          return DSAT_FAILURE;
      }
    }
  }
  return result;
}/* dsatvend_3gpp2_profile_create_get_param_info */

/*===========================================================================
  FUNCTION DSATVEND_PROFILE_SET_PARAM_DETAILS
 
  DESCRIPTION
    Gets the param_identifier, param_len, param_value_validation using
    param_type and tech_type.
 
  PARAMETERS
    param_id :         Parameter identifier.
    param_type:        parameter type (string)
 
  RETURN VALUE
    if param_id, param_len, param_val is successfully found : DSAT_SUCCESS.
    else  :DSAT_FAILURE
  
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
===========================================================================*/
LOCAL boolean dsatvend_profile_set_param_details
(
  ds_profile_tech_etype                  tech_type,
  ds_profile_hndl_type                   profile_hndl,
  char                                  *param_type,
  char                                  *param_value
)
{
  uint8                                    idx;
  uint8                                    i;
  dsat_num_item_type                       value = 0;
  dsat_profile_parameter_details_s_type    *param_list;
  uint8                                    param_list_size;
  ds_profile_info_type                     param_info;
  ds_profile_status_etype                  profile_status;
  ds_profile_linger_params_type            linger_data;
  uint32                                   param_id;
  dsat_v4_addr_type                        v4_addr;
  dsat_v6_addr_u_type                      v6_addr;
  uint32                                   addr[4];
  boolean                                  valid_value = FALSE;
  boolean                                  param_found = FALSE;
  uint32                                   dsat_failure_timers[DS_PROFILE_3GPP_FAILURE_TIMER_MAX];
  uint8                                    failure_index = 0;

  DS_AT_MSG_SPRINTF_2_HIGH(" param_type:%s, param_value=%s",param_type,param_value);
/*
  * APNNAME mapped to 3GPP2 APN name or 3GPP APN name based on profile tech type
  * APN NAME is general Param and tied to profile.
 */
  if( 0 == dsatutil_strcmp_ig_sp_case((byte*)"APNNAME", (byte*)param_type ))
  {
    if( tech_type == DS_PROFILE_TECH_EPC || tech_type == DS_PROFILE_TECH_3GPP)
    {
      param_type = "PAPNNAME";
    }
    else if (tech_type == DS_PROFILE_TECH_3GPP2)
    {
      param_type = "P2APNNAME";
    }
  }

  if( tech_type == DS_PROFILE_TECH_EPC )
  {
    /* Search in 3GPP2 list*/
    param_list = &profile_3gpp2_parameter_details_list[0];
    param_list_size = ARR_SIZE(profile_3gpp2_parameter_details_list);

    for(idx = 0; idx < param_list_size; idx++)
    {
      if( 0 == dsatutil_strcmp_ig_sp_case((byte*)param_type,(byte*)param_list[idx].abbrv) )
      {
        param_found = TRUE;
        break;
      }
    }

    /*Search in common EPC list*/
    if (FALSE == param_found)
    {
      param_list = &profile_epc_parameter_details_list[0];
      param_list_size = ARR_SIZE(profile_epc_parameter_details_list);

      for(idx = 0; idx < param_list_size; idx++)
      {
        if( 0 == dsatutil_strcmp_ig_sp_case((byte*)param_type,(byte*)param_list[idx].abbrv) )
        {
          param_found = TRUE;
          break;
        }
      }
    }

    /* Search in 3GPP list*/
    if (FALSE == param_found)
    {
      param_list = &profile_3gpp_parameter_details_list[0];
      param_list_size = ARR_SIZE(profile_3gpp_parameter_details_list);

      for(idx = 0; idx < param_list_size; idx++)
      {
        if( 0 == dsatutil_strcmp_ig_sp_case((byte*)param_type,(byte*)param_list[idx].abbrv) )
        {
          param_found = TRUE;
          break;
        }
      }
    }
  }
  else if (tech_type == DS_PROFILE_TECH_3GPP2)
  {
    param_list = &profile_3gpp2_parameter_details_list[0];
    param_list_size = ARR_SIZE(profile_3gpp2_parameter_details_list);

    for(idx = 0; idx < param_list_size; idx++)
    {
      if( 0 == dsatutil_strcmp_ig_sp_case((byte*)param_type,(byte*)param_list[idx].abbrv) )
      {
        param_found = TRUE;
        break;
      }
    }
  }
  else if (tech_type == DS_PROFILE_TECH_3GPP)
  {
    param_list = &profile_3gpp_parameter_details_list[0];
    param_list_size = ARR_SIZE(profile_3gpp_parameter_details_list);

    for(idx = 0; idx < param_list_size; idx++)
    {
      if( 0 == dsatutil_strcmp_ig_sp_case((byte*)param_type,(byte*)param_list[idx].abbrv) )
      {
        param_found = TRUE;
        break;
      }
    }
  }
  /*Search in general param list which is independent of 
    tech (general param will always exist in a profile)*/
  if (FALSE == param_found)
  {
    param_list = &profile_general_parameter_details_list[0];
    param_list_size = ARR_SIZE(profile_general_parameter_details_list);

    for(idx = 0; idx < param_list_size; idx++)
    {
      if( 0 == dsatutil_strcmp_ig_sp_case((byte*)param_type,(byte*)param_list[idx].abbrv) )
      {
        param_found = TRUE;
        break;
      }
    }
  }

  if( param_found )
  {
    param_id       =  param_list[idx].identifier;
    param_info.len =  param_list[idx].len;

    switch( param_list[idx].type )
    {
      case STR_TYPE:
      {
        if( FALSE == dsatutil_strip_quotes_out( (byte*) param_value,
                                                (byte*) param_value,
                                                (word)  param_list[idx].range[0].lower ))
        {
          return DSAT_FAILURE;
        }
        DS_AT_MSG_SPRINTF_2_HIGH("Setting param_id:%d, param_value=%s",param_id,param_value);

        /*Special case:: PDPTYPE- 3GPP and 3GPP2/Common have different values for legacy. 
          New PDPTYPE getting added will have this value ex - ROAMING_PDP_TYPE below*/

        if( param_id == DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_TYPE ||
            param_id == DS_PROFILE_TECH_COMMON_PROFILE_PARAM_PDP_TYPE ||
            param_id == DS_PROFILE_3GPP_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE ||
            param_id == DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE ||
            param_id == DS_PROFILE_3GPP_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE)
        {
          for (i = 0; i < DSAT_MAX_PDP_TYPE_NUM - 1; i++)
          {
            if( 0 == dsatutil_strcmp_ig_sp_case((byte*)param_value, (byte*)dsat_p2_pdptype[i] ))
            {
              param_info.buf = (void*)&i;
              param_info.len =  sizeof(uint8);
              break;
            }
          }
          if((DSAT_MAX_PDP_TYPE_NUM - 1) == i)
          {
            DS_AT_MSG1_ERROR("Invalid PDP TYPE =%d",param_id );
            return DSAT_FAILURE;
          }
        }
        else if( param_id == DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE)
        {
          for (i = 0; i < DSAT_MAX_PDP_TYPE_NUM; i++)
          {
            if( 0 == dsatutil_strcmp_ig_sp_case((byte*)param_value, (byte*)dsat_pp_pdptype[i] ))
            {
              param_info.buf = (void*)&i;
              param_info.len =  sizeof(uint8);
              break;
            }
          }
          if(DSAT_MAX_PDP_TYPE_NUM  == i)
          {
            DS_AT_MSG1_ERROR("Invalid PDP TYPE =%d",param_id );
            return DSAT_FAILURE;
          }
        }
        else
        {
          param_info.buf = (void*) param_value;
          param_info.len = strlen(param_value);
        }
        break;
      }
      case NUM_TYPE:
      {
        if( ATOI_OK != dsatutil_atoi( &value, (byte*) param_value, 10 ))
        {
          DS_AT_MSG1_ERROR("ATOI failed for param_id =%d",param_id );
          return DSAT_FAILURE;
        }

        /* Range Validation */
        for( i = 0; !(param_list[idx].range[i].lower == RANGE_END &&
                       param_list[idx].range[i].upper == RANGE_END ); i++ )
        {
          if( value >= param_list[idx].range[i].lower &&
              value <= param_list[idx].range[i].upper	)
          {
            valid_value = TRUE;
            break;
          }
        }
        if(!valid_value)
        {
          DS_AT_MSG2_ERROR("Range Validation failed for param_id:%d, value:%d",param_id,value);
          return DSAT_FAILURE;
        }

        DS_AT_MSG2_HIGH("Setting val=%d for param_id %d",value,param_id);
        /*Special case for 3GPP failure timers since they are all tagged to one identifier 
          First:   Get all the 10 timers and 
          Second : Then set back the modified timer(s)*/

        if( param_id == DS_PROFILE_3GPP_PROFILE_PARAM_FAILURE_TIMERS)
        {
           memset(&dsat_failure_timers[0], 0, sizeof(dsat_failure_timers));
           param_info.buf = (void*)&dsat_failure_timers[0];
           param_info.len =  sizeof(dsat_failure_timers);

           /*Find which failure timer needs to be modified*/
           for (failure_index = 0;
                failure_index < DS_PROFILE_3GPP_FAILURE_TIMER_MAX; 
                failure_index++)
           {
             if( 0 == dsatutil_strcmp_ig_sp_case((byte*)param_list[idx].abbrv, 
                                                 (byte*)dsat_pp_failure_timers[failure_index] ))
             {
               break;
             }
           }

           /*getting the value for all the timers set (its an array) */
           profile_status =  ds_profile_get_param( profile_hndl, 
                                                   param_list[idx].identifier, 
                                                   &param_info);

           DS_AT_MSG2_HIGH("failure_timer: %d, status is: %d", 
                           param_list[idx].identifier, 
                           profile_status);

           if(profile_status == DS_PROFILE_REG_RESULT_SUCCESS)
           {
              if (failure_index < DS_PROFILE_3GPP_FAILURE_TIMER_MAX) 
              {
                dsat_failure_timers[failure_index] = value;
              }
              else
              {
                 DS_AT_MSG1_ERROR("ERROR: failure_timer invalid param index: %d" , i);
              }
           }
           break;
        }

        param_info.buf = (void*)&value;
        break;
      }
      case ADDR_V4_TYPE:
      {
  /*-----------------------------------------------------------------------
    Handling IPV4 address. Converting them to Big Endian format before
    calling DS Profile API's
  -----------------------------------------------------------------------*/

        if( FALSE == dsatutil_strip_quotes_out( (byte*) param_value,
                                                (byte*) param_value,
                                                (word)  param_list[idx].range[0].lower ))
        {
          DS_AT_MSG1_ERROR("Strip quote failure for param_id %d",param_id);
          return DSAT_FAILURE;
        }

        DS_AT_MSG_SPRINTF_2_HIGH("V4 address to be set param_id:%d, param_value=%s",param_id,param_value);
        if(DSAT_OK != dsatutil_convert_tuple( STRING_TO_INT,
                                              TUPLE_TYPE_IPv4,
                                              &v4_addr,
                                              (byte*)param_value,
                                              MAX_V4_ADDR_LEN ,
                                              DSAT_CONVERT_IP) )
        {
          DS_AT_MSG1_ERROR("V4 addr conversion failure for param_id %d",param_id);
          return DSAT_FAILURE;
        }
        param_info.buf = (void*) &v4_addr;
        break;
      }
      case ADDR_C_V6_TYPE:
      case ADDR_P_V6_TYPE:
      case ADDR_P2_V6_TYPE:
      {
        /* Conditions to verify V6 address */
        if( FALSE == dsatutil_strip_quotes_out( (byte*) param_value,
                                                (byte*) param_value,
                                                (word)  param_list[idx].range[0].lower ))
        {
          DS_AT_MSG1_ERROR("Strip quote failure for param_id %d",param_id);
          return DSAT_FAILURE;
        }
        DS_AT_MSG_SPRINTF_2_HIGH("V6 address to be set param_id:%d, param_value=%s",param_id,param_value);
        if(DSAT_OK != dsatutil_convert_tuple( STRING_TO_INT,
                                              TUPLE_TYPE_IPv6_OCTETS,
                                              &addr[0],
                                              (byte*)param_value,
                                              MAX_IPADDR_STR_LEN,
                                              DSAT_CONVERT_IP) )
        {
          DS_AT_MSG1_ERROR("V6 addr conversion failure for param_id %d",param_id);
          return DSAT_FAILURE;
        }
        if(param_list[idx].type == ADDR_C_V6_TYPE )
        {
          v6_addr.v6_common.ds_profile_s6_addr32[0] = addr[0];
          v6_addr.v6_common.ds_profile_s6_addr32[1] = addr[1];
          v6_addr.v6_common.ds_profile_s6_addr32[2] = addr[2];
          v6_addr.v6_common.ds_profile_s6_addr32[3] = addr[3];
          param_info.buf = (void*) &(v6_addr.v6_common);
        }
        else if (param_list[idx].type == ADDR_P_V6_TYPE )
        {
          v6_addr.v6_3gpp.in6_u.u6_addr32[0] = addr[0];
          v6_addr.v6_3gpp.in6_u.u6_addr32[1] = addr[1];
          v6_addr.v6_3gpp.in6_u.u6_addr32[2] = addr[2];
          v6_addr.v6_3gpp.in6_u.u6_addr32[3] = addr[3];
          param_info.buf = (void*) &(v6_addr.v6_3gpp);
        }
        else
        {
          v6_addr.v6_3gpp2.ds_profile_3gpp2_s6_addr32[0] = addr[0];
          v6_addr.v6_3gpp2.ds_profile_3gpp2_s6_addr32[1] = addr[1];
          v6_addr.v6_3gpp2.ds_profile_3gpp2_s6_addr32[2] = addr[2];
          v6_addr.v6_3gpp2.ds_profile_3gpp2_s6_addr32[3] = addr[3];
          param_info.buf = (void*) &(v6_addr.v6_3gpp2);
        }
        break;
      }
      case LNGR_TMR_TYPE:
      {
        if( ATOI_OK != dsatutil_atoi( &value, (byte*) param_value, 10 ))
        {
          DS_AT_MSG1_ERROR("ATOI failed for param_id =%d",param_list[idx].identifier);
          return DSAT_FAILURE;
        }      
        /* Range Validation */
        for( i = 0; !(param_list[idx].range[i].lower == RANGE_END &&
                       param_list[idx].range[i].upper == RANGE_END ); i++ )
        {
          if( value >= param_list[idx].range[i].lower &&
              value <= param_list[idx].range[i].upper	)
          {
            valid_value = TRUE;
            break;
          }
        }
        if(!valid_value)
        {
          DS_AT_MSG2_ERROR("Range Validation failed for param_id:%d, value:%d",param_id,value);
          return DSAT_FAILURE;
        }
        memset(&param_info, 0, sizeof(ds_profile_info_type));
        memset(&linger_data, 0, sizeof(ds_profile_linger_params_type));

        linger_data.linger_timeout_val = (uint16) value;
        param_info.buf = (void*) &linger_data;
        param_info.len = sizeof(ds_profile_linger_params_type);
        break;
      }
      default:
        return DSAT_FAILURE;
    }

    /*-----------------------------------------------------------------------
      Set the Profile with the value of profile identifier.
    -----------------------------------------------------------------------*/
    profile_status = ds_profile_set_param(profile_hndl, (uint32)param_id, 
                                          &param_info);

    if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      DS_AT_MSG2_ERROR("Set Profile returns error. Profile_identifier %d, Error %d",
                       param_id , profile_status);
      return DSAT_FAILURE;
    }
    return DSAT_SUCCESS;
  }

  DS_AT_MSG0_ERROR("ARG not found!");
  return DSAT_FAILURE;
}/*dsatvend_profile_set_param_details*/

/*===========================================================================
  FUNCTION DSATVEND_PROFILE_UPDATE_PARAM_DETAILS()
 
  DESCRIPTION
    Gets the param_identifier, param_len, param_value_validation using
    param_type and tech_type.
 
  PARAMETERS
    param_id :         Parameter identifier.
    param_type:        parameter type (string)
 
  RETURN VALUE
    if param_id, param_len, param_val is successfully found : DSAT_SUCCESS.
    else  :DSAT_FAILURE
  
  DEPENDENCIES
    None
 
  SIDE EFFECTS
    None
===========================================================================*/
LOCAL dsat_result_enum_type dsatvend_profile_update_param_details
(
  dsat_num_item_type                profile_id,/*  Profile ID       */
  ds_profile_tech_etype             tech_type,/*  Tech Type       */
  dsm_item_type                    *res_buff_ptr /*  Place to put response       */
)
{
  #define BUFFER_OFFSET_VAL 30
   dsat_profile_parameter_details_s_type    *param_list = NULL;
   uint8                                     param_list_size;
   ds_profile_status_etype                   profile_status;
   dsat_result_enum_type                     result = DSAT_OK;
   ds_profile_hndl_type                      profile_hndl = NULL;
   char                                      param_value[DSAT_MAX_PROFILE_PARAM_VAL_LEN];
   ds_profile_info_type                      param_info;
   dsat_num_item_type                        current_index = 0;
   uint16                                    index = 0;
   dsat_num_item_type                        num_val;
   byte                                      value[MAX_IPADDR_STR_LEN];
   ds_profile_in6_addr_type                  v6_addr;
   ds_profile_linger_params_type            linger_data;
   uint8                                    array_size_pp = 0;
   uint8                                    array_size_pp2 = 0;
   uint8                                    array_size_common = 0;
   uint8                                    array_size_general = 0;   
   dsat_num_item_type                       dsat_failure_timers[DS_PROFILE_3GPP_FAILURE_TIMER_MAX];
   uint8                                    failure_index = 0;
   
  DSAT_BEGIN_PROFILE_TRANSACTION(profile_id,tech_type,profile_hndl);
   
  if( profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
     return DSAT_ERROR;
  }

  if(tech_type == DS_PROFILE_TECH_3GPP || 
     tech_type == DS_PROFILE_TECH_EPC)
  {
      array_size_pp   = ARR_SIZE(profile_3gpp_parameter_details_list);
  }

  if(tech_type == DS_PROFILE_TECH_3GPP2 || 
     tech_type == DS_PROFILE_TECH_EPC)
  {
      array_size_pp2  = ARR_SIZE(profile_3gpp2_parameter_details_list);
  }

  if(tech_type == DS_PROFILE_TECH_EPC)
  {
    array_size_common = ARR_SIZE(profile_epc_parameter_details_list);
  }
  array_size_general = ARR_SIZE(profile_general_parameter_details_list);

  param_list_size   = array_size_pp + array_size_pp2 + array_size_common + array_size_general;

  /* Check to see if a new DSM item is required */
  CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)

  res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                              res_buff_ptr->size - res_buff_ptr->used,
                                              "$QCPRFMOD: PID:%d TECH:%d\n",
                                               profile_id,
                                               tech_type);

  while (index < param_list_size)
  {
    /* Need to complete logic for general params*/

    if (tech_type == DS_PROFILE_TECH_3GPP2)
    {
      if (index < array_size_pp2)
      {
        param_list =  &profile_3gpp2_parameter_details_list[0];
        current_index = index;
      }
      else
      {
        param_list =  &profile_general_parameter_details_list[0];
        current_index = index-array_size_pp2;
      }
    }
    else if (tech_type == DS_PROFILE_TECH_3GPP)
    {
      if (index < array_size_pp)
      {
        param_list =  &profile_3gpp_parameter_details_list[0];
        current_index = index;
      }
      else
      {
        param_list =  &profile_general_parameter_details_list[0];
        current_index = index-array_size_pp;
      }
    }
    else if (tech_type == DS_PROFILE_TECH_EPC)
    {
      if(index < array_size_pp)
      {
        param_list =  &profile_3gpp_parameter_details_list[0];
        current_index = index;
      }
      else if((index >= array_size_pp) &&
              (index < (array_size_pp + array_size_common)))
      {
        param_list =  &profile_epc_parameter_details_list[0];
        current_index = index-array_size_pp;
      }
      else if((index >= array_size_pp + array_size_common) && 
              (index < (array_size_pp + array_size_common + array_size_pp2)))
      {
        param_list =  &profile_3gpp2_parameter_details_list[0];
        current_index = index-array_size_pp-array_size_common;
      }
      else
      {
        param_list =  &profile_general_parameter_details_list[0];
        current_index = index-array_size_pp-array_size_common-array_size_pp2;
      }
    }
    else
    {
      DS_AT_MSG1_ERROR("Invalid tech_type %d", tech_type);
      return DSAT_ERROR;
    }

    memset(&param_info, 0, sizeof(ds_profile_info_type));

    /*APNNAME needs to be printed only once, as its a special general param*/
    if( 0 == dsatutil_strcmp_ig_sp_case((byte*)"P2APNNAME",(byte*)param_list[current_index].abbrv) )
    {
      index++;
      continue;
    }

    param_info.len = param_list[current_index].len;
    param_info.buf = &param_value[0];

    switch(param_list[current_index].type)
    {
       case NUM_TYPE:
       {
         /*Special case for 3GPP failure timers since they are all tagged to one identifier 
          First:   Get all the 10 timers and 
          Second : print all the ten timers and increment the index by max-1 */

        if( param_list[current_index].identifier == DS_PROFILE_3GPP_PROFILE_PARAM_FAILURE_TIMERS)
        {
          memset(&dsat_failure_timers[0], 0, sizeof(dsat_failure_timers));
          param_info.buf = (void*)&dsat_failure_timers[0];
          param_info.len =  sizeof(dsat_failure_timers);

          /*getting the value for all the timers set (its an array) */
          profile_status =  ds_profile_get_param( profile_hndl, 
                                                  param_list[current_index].identifier, 
                                                  &param_info);

          DS_AT_MSG2_HIGH("failure_timer: %d, status is: %d", 
                          param_list[current_index].identifier, 
                          profile_status);

          if(profile_status == DS_PROFILE_REG_RESULT_SUCCESS)
          {
             for(failure_index=0; 
                  failure_index < DS_PROFILE_3GPP_FAILURE_TIMER_MAX; 
                  failure_index++) 
             {
               /*print all the failure timer values 1-10*/
               res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                          res_buff_ptr->size - res_buff_ptr->used,
                                                          "%s:%u\n",
                                                          param_list[current_index+failure_index].abbrv,
                                                          dsat_failure_timers[failure_index]);
             }
             /*increment the index for each iteration of failure timer (max -1)*/
             index += DS_PROFILE_3GPP_FAILURE_TIMER_MAX - 1;
           }
           break;
        }

         memset(&num_val,0x0,sizeof(num_val));
         param_info.buf = (void*)&num_val;
         
         profile_status =  ds_profile_get_param( profile_hndl, 
                                                 param_list[current_index].identifier, 
                                                 &param_info);
         DS_AT_MSG2_HIGH("status for param %d is %d", param_list[current_index].identifier,profile_status);
         if(profile_status == DS_PROFILE_REG_RESULT_SUCCESS)
         {
           /* Check to see if a new DSM item is required */
           CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)

           res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                      res_buff_ptr->size - res_buff_ptr->used,
                                                      "%s:%u\n",
                                                      param_list[current_index].abbrv,
                                                      num_val);
         }
       }
       break;

       case STR_TYPE:
       {

        if( param_list[current_index].identifier == DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_TYPE ||
            param_list[current_index].identifier == DS_PROFILE_TECH_COMMON_PROFILE_PARAM_PDP_TYPE ||
            param_list[current_index].identifier == DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE ||
            param_list[current_index].identifier == DS_PROFILE_3GPP_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE ||
            param_list[current_index].identifier == DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE ||
            param_list[current_index].identifier == DS_PROFILE_3GPP_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE)
        {
          memset(&num_val,0x0,sizeof(num_val));
          param_info.buf = (void*)&num_val;
          param_info.len = sizeof(num_val);
        }
        else
        {
          memset(&param_value[0], 0 , sizeof(param_value));
          param_info.buf = (void*)&param_value[0];
          param_info.len = sizeof(param_value);
        }
         profile_status =  ds_profile_get_param( profile_hndl, 
                                                 param_list[current_index].identifier, 
                                                 &param_info);

         DS_AT_MSG2_HIGH("status for param %d is %d", param_list[current_index].identifier,profile_status);

         if(profile_status == DS_PROFILE_REG_RESULT_SUCCESS)
         {
           /* Check to see if a new DSM item is required */
           CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - (DS_PROFILE_3GPP_MAX_APN_STRING_LEN + BUFFER_OFFSET_VAL))

           if( param_list[current_index].identifier == DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN )
           {
             res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                    res_buff_ptr->size - res_buff_ptr->used,
                                                    "APNNAME:\"%s\"\n",
                                                    (char*)param_info.buf);
           }
           else if( param_list[current_index].identifier == DS_PROFILE_3GPP2_PROFILE_PARAM_AUTH_PASSWORD || 
                    param_list[current_index].identifier == DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_PASSWORD  || 
                    param_list[current_index].identifier == DS_PROFILE_TECH_COMMON_PROFILE_PARAM_AUTH_PASSWORD)
           {

             res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                     res_buff_ptr->size - res_buff_ptr->used,
                                                     "%s:\"\"\n",
                                                      param_list[current_index].abbrv);
           }

           /*Special case for PDPTYPE in 3GPP, 3GPP2, & common params*/
           else if( param_list[current_index].identifier == DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_TYPE ||
                    param_list[current_index].identifier == DS_PROFILE_TECH_COMMON_PROFILE_PARAM_PDP_TYPE ||
                    param_list[current_index].identifier == DS_PROFILE_3GPP_PROFILE_PARAM_LTE_ROAMING_PDP_TYPE ||
                    param_list[current_index].identifier == DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_ROAMING_PDP_TYPE)
           {
             res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                    res_buff_ptr->size - res_buff_ptr->used,
                                                    "%s:\"%s\"\n",
                                                    param_list[current_index].abbrv,
                                                    (char*)dsat_p2_pdptype[num_val]);
           }
           else if( param_list[current_index].identifier == DS_PROFILE_3GPP_PROFILE_PARAM_OVERRIDE_HOME_PDP_TYPE )
           {
             /* Checking if Override Home pdp type is configured. If not, then don't display any PDP type. */
             if (DS_PROFILE_3GPP_OVERRIDE_HOME_PDP_MAX == num_val)
             {
               num_val = 3;
             }
             res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                    res_buff_ptr->size - res_buff_ptr->used,
                                                    "%s:\"%s\"\n",
                                                    param_list[current_index].abbrv,
                                                    (char*)dsat_p2_pdptype[num_val]);
           }
           else if(param_list[current_index].identifier == DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE)
           {
             res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                    res_buff_ptr->size - res_buff_ptr->used,
                                                    "%s:\"%s\"\n",
                                                    param_list[current_index].abbrv,
                                                    (char*)dsat_pp_pdptype[num_val]);
           }
           else
           {
             res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                    res_buff_ptr->size - res_buff_ptr->used,
                                                    "%s:\"%s\"\n",
                                                    param_list[current_index].abbrv,
                                                    (char*)param_info.buf);
           }
         }
       }
       break;

      case LNGR_TMR_TYPE:
      {
        memset(&linger_data,0x0,sizeof(linger_data));
        param_info.buf = (void*)&linger_data;
        param_info.len = sizeof(linger_data);

        profile_status =  ds_profile_get_param( profile_hndl, 
                                                param_list[current_index].identifier, 
                                                &param_info);
        DS_AT_MSG2_HIGH("status for param %d is %d", param_list[current_index].identifier,profile_status);
        if(profile_status == DS_PROFILE_REG_RESULT_SUCCESS)
        {
          /* Check to see if a new DSM item is required */
          CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - BUFFER_OFFSET_VAL)

          res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                  res_buff_ptr->size - res_buff_ptr->used,
                                                  "%s:%d\n",
                                                  param_list[current_index].abbrv,
                                                  linger_data.linger_timeout_val);
        }
      }
      break;

      case ADDR_V4_TYPE:
      {
        memset(&num_val,0x0,sizeof(num_val));
        memset(value, 0, sizeof(value));
        param_info.buf = (void*)&num_val;
        param_info.len =  param_list[current_index].len;

        profile_status =  ds_profile_get_param( profile_hndl, 
                                                param_list[current_index].identifier, 
                                                &param_info);

        DS_AT_MSG2_HIGH("status for param %d is %d", param_list[current_index].identifier,profile_status);
        if(profile_status == DS_PROFILE_REG_RESULT_SUCCESS)
        {
          if( DSAT_OK != dsatutil_convert_tuple( INT_TO_STRING,
                                                 TUPLE_TYPE_IPv4 ,
                                                 (uint32*)&num_val,
                                                 (byte*)value ,
                                                 MAX_V4_ADDR_LEN,
                                                 DSAT_CONVERT_IP))
          {
            DS_AT_MSG1_ERROR("IPV4 address conversion failed for param_id %d",param_list[current_index].identifier);
            DSAT_END_PROFILE_TRANSACTION(profile_hndl);
            return DSAT_ERROR;
          }
          /* Check to see if a new DSM item is required */
          CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - (MAX_V4_ADDR_LEN + BUFFER_OFFSET_VAL))

          res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                 res_buff_ptr->size - res_buff_ptr->used,
                                                 "%s:\"%s\"\n",
                                                 param_list[current_index].abbrv,
                                                 (char*)value);
        }
      }
      break;

      case ADDR_C_V6_TYPE:
      case ADDR_P_V6_TYPE:
      case ADDR_P2_V6_TYPE:
      {
        memset(value, 0, sizeof(value));
        memset(&v6_addr, 0, sizeof(v6_addr));
        param_info.buf = (void*)&v6_addr;
        param_info.len =  sizeof(ds_profile_in6_addr_type);

        profile_status =  ds_profile_get_param( profile_hndl, 
                                                param_list[current_index].identifier, 
                                                &param_info);

        DS_AT_MSG2_HIGH("status for param %d is %d", param_list[current_index].identifier,profile_status);
        if(profile_status == DS_PROFILE_REG_RESULT_SUCCESS)
        {

          if( DSAT_OK != dsatutil_convert_tuple( INT_TO_STRING,
                                                 TUPLE_TYPE_IPv6_OCTETS,
                                                 (uint32*) &v6_addr.ds_profile_in6_u,
                                                 (byte*)value ,
                                                  MAX_IPADDR_STR_LEN,
                                                  DSAT_CONVERT_IP))
          {
            DS_AT_MSG1_ERROR("IPV6 address conversion failed for param_id %d",param_list[current_index].identifier);
            DSAT_END_PROFILE_TRANSACTION(profile_hndl);
            return DSAT_ERROR;
          }
          CHECK_APPEND_RESPONSE_BUFFER (res_buff_ptr->size - (MAX_IPADDR_STR_LEN + BUFFER_OFFSET_VAL))
          
          res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                                 res_buff_ptr->size - res_buff_ptr->used,
                                                 "%s:\"%s\"\n",
                                                 param_list[current_index].abbrv,
                                                 (char*)value);
        }
      }
      break;        

     default:
       break;
    }

    index++; 
  }
  DSAT_END_PROFILE_TRANSACTION(profile_hndl);
  
  return result;

}/*dsatvend_profile_set_param_details*/

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPRFMOD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This command is used to configure parameters 
  for 3GPP2/EPC profile.
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcprfmod_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{

  dsat_result_enum_type             result = DSAT_OK;
  char                              param_type[DSAT_MAX_PROFILE_PARAM_TYPE_LEN];
  char                              param_value[DSAT_MAX_PROFILE_PARAM_VAL_LEN];
  dsat_num_item_type                profile_id;
  ds_profile_hndl_type              profile_hndl = NULL;
  ds_profile_status_etype           profile_status;
  ds_profile_tech_etype             tech_type;
  uint8                             index;
  
  profile_status = DS_PROFILE_REG_RESULT_SUCCESS;

  if( tok_ptr->op == (NA|EQ|AR) )
  {
    /*---------------------------------------------------------------
      Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/
    
    /*---------------------------------------------------------------
      Checking no. of arguments.
      Min Args = 2 (Profile ID and minimum one parameter to configure)
      Max Args = 16 (Profile ID and max 15 paramters to configure)
    ---------------------------------------------------------------*/
    if( tok_ptr->args_found < 2 || tok_ptr->args_found > 16 )
    {
      DS_AT_MSG1_ERROR("Invalid Number of arguements  %d", tok_ptr->args_found);
      return(DSAT_ERROR);
    }
    /*---------------------------------------------------------------
      Function call to get Profile ID.
    ---------------------------------------------------------------*/
    if(!VALID_TOKEN(0) || DSAT_FAILURE == dsatvend_get_param_type_value( (char*)tok_ptr->arg[0],
                                                      (char*) param_type,
                                                       DSAT_MAX_PROFILE_PARAM_TYPE_LEN,
                                                      (char*)param_value,
                                                       DSAT_MAX_PROFILE_PARAM_VAL_LEN ) )
    {
      DS_AT_MSG0_ERROR("Failed to get Profile ID");
      return DSAT_ERROR;
    }

    if( !( ( 0 == dsatutil_strcmp_ig_sp_case((byte*)param_type,(byte*) "PID")) &&
          ( ATOI_OK == dsatutil_atoi(&profile_id,(byte*)param_value, 10 ))  ) )
    {
      DS_AT_MSG0_ERROR("Invalid first argument. Should be valid profile ID");
      return(DSAT_ERROR);
    }

    tech_type = ds_profile_get_tech_type_from_profile_num_per_sub( profile_id, dsat_get_current_ds_profile_subs_id());
    
    if (!( tech_type == DS_PROFILE_TECH_3GPP || 
    	   tech_type == DS_PROFILE_TECH_3GPP2 ||
    	   tech_type == DS_PROFILE_TECH_EPC ) )
    {
      DS_AT_MSG2_ERROR("Cannot get tech_type for profile id %d or invalid tech_type %d",
                         profile_id, tech_type);
      return DSAT_ERROR;
    }
    DS_AT_MSG1_HIGH("$QCPRFMOD: tech_type = %d", tech_type);

  /*-------------------------------------------------------------------------
    Begin transaction
  -------------------------------------------------------------------------*/
    DSAT_BEGIN_PROFILE_TRANSACTION(profile_id,tech_type,profile_hndl);
    for(index = 1; index < tok_ptr->args_found; index++)
    {
      memset(param_type, 0, sizeof(param_type));
      memset(param_value, 0, sizeof(param_value));

     /*---------------------------------------------------------------
       Function call to get each param_type and param_value.
      ---------------------------------------------------------------*/
      if( !VALID_TOKEN(index) || DSAT_FAILURE == dsatvend_get_param_type_value( (char*) tok_ptr->arg[index],
                                                        (char*)param_type,
                                                         DSAT_MAX_PROFILE_PARAM_TYPE_LEN,
                                                         (char*)param_value,
                                                         DSAT_MAX_PROFILE_PARAM_VAL_LEN ))
      {
        DS_AT_MSG1_ERROR("Failed to tokenize argument no. %d", index);
        DSAT_END_PROFILE_TRANSACTION(profile_hndl);
        return DSAT_ERROR;
      } 
      if( DSAT_FAILURE == dsatvend_profile_set_param_details( tech_type,profile_hndl,
                                                             param_type, param_value ) )
      {
        DS_AT_MSG1_ERROR("Failed to set value for param for agr no. %d", index);
        DSAT_END_PROFILE_TRANSACTION(profile_hndl);
        return DSAT_ERROR;
      }
     
    } /* end FOR */
    
    /*-------------------------------------------------------------------------
      End transaction
    -------------------------------------------------------------------------*/
    DSAT_END_PROFILE_TRANSACTION(profile_hndl);
  }

  else if( tok_ptr->op == (NA|EQ|QU) )
  {
     res_buff_ptr->used = (word) snprintf((char*)&res_buff_ptr->data_ptr[0],
                                                   res_buff_ptr->size ,
                                               "$QCPRFMOD: ");
     (void) dsatvend_display_qcprfmod_test_result( res_buff_ptr );
  }

  else if( tok_ptr->op == (NA|QU) )
  {
    /*---------------------------------------------------------------
      Processing for read command
    ---------------------------------------------------------------*/
    ds_profile_tech_etype           tech_array[]={ DS_PROFILE_TECH_3GPP,
                                                   DS_PROFILE_TECH_3GPP2,
                                                   DS_PROFILE_TECH_EPC,
                                                   DS_PROFILE_TECH_MAX};
    ds_profile_itr_type             itr;
    ds_profile_list_info_type       list_info;
    ds_profile_list_type            search_list_info;
    ds_profile_info_type            profile_info;
    byte                            value[150];

    /*-------------------------------------------------------------------------
      Get the iterator. If there is no entry then the DS Profile will return
      LIST_END error code. Fill number of instances as 0 and return success.
    -------------------------------------------------------------------------*/
    for(index = 0; tech_array[index] != DS_PROFILE_TECH_MAX; index++)
    {
      memset(&search_list_info, 0, sizeof(search_list_info));
      memset(&itr, 0, sizeof(ds_profile_itr_type));
      
      /* Default operation - Get all profiles */
      search_list_info.dfn = DS_PROFILE_LIST_ALL_PROFILES;
      
      profile_status = ds_profile_get_list_itr_per_sub( tech_array[index], 
                                                &search_list_info, 
                                                &itr,
                                                dsat_get_current_ds_profile_subs_id());
      
      if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
      {
        DS_AT_MSG1_ERROR("$QCPRFMOD: Getting profile list iterator failed. Error %d ",
                           profile_status);
        continue;    
      }
    
      while(profile_status != DS_PROFILE_REG_RESULT_LIST_END)
      {
        memset(&list_info, 0, sizeof (ds_profile_list_info_type));
        memset(&profile_info, 0, sizeof (ds_profile_info_type));
        memset(value, 0, sizeof(value));

        list_info.name = &profile_info;
        list_info.name->len  = sizeof(value);
        list_info.name->buf  = (void*)value;

      /*-----------------------------------------------------------------------
        Call the profile registry API to get the profile info
      -----------------------------------------------------------------------*/
        profile_status = ds_profile_get_info_by_itr(itr, &list_info);
        if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
        {
          DS_AT_MSG1_ERROR ("$QCPRFMOD: Getting profile info by iterator failed. Error %d",
                             profile_status);
          (void) ds_profile_itr_destroy(itr);
          return DSAT_ERROR;
        }
      
        result = dsatvend_profile_update_param_details( list_info.num,
                                                       tech_array[index],
                                                       res_buff_ptr);
        
        if(result == DSAT_ERROR)
        {
          DSAT_PROFILE_DESTROY_ITERATOR(itr);
          return result;
        }

        profile_status = ds_profile_itr_next(itr);
        if ( (profile_status != DS_PROFILE_REG_RESULT_SUCCESS) &&
             (profile_status != DS_PROFILE_REG_RESULT_LIST_END) )
        {
          DS_AT_MSG1_ERROR("Getting Next iterator failed. Error %d",
                           profile_status);
          DSAT_PROFILE_DESTROY_ITERATOR(itr);
          return DSAT_ERROR;
        }
      } /* while (profile_status != DS_PROFILE_REG_RESULT_LIST_END) */
      /*-------------------------------------------------------------------------
        Destroy the iterator
      -------------------------------------------------------------------------*/
      DSAT_PROFILE_DESTROY_ITERATOR(itr);
   	}
    if( res_buff_ptr->used == 0 )
    {
       res_buff_ptr->used = (word) snprintf((char*)&res_buff_ptr->data_ptr[0],
                                res_buff_ptr->size,
                                "$QCPRFMOD: ");
    }
  }
  else
  {
    /* Not Supported */
    return DSAT_ERROR;
  }
 return result;
}/*dsatvend_exec_qcprfmod_cmd*/

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCPRFCRT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This command is used to create or delete 
  any 3GPP2/EPC profile.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcprfcrt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type             result = DSAT_OK;
  ds_profile_status_etype           profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  dsat_num_item_type                profile_id = 0;
  ds_profile_config_type            profile_config;
  ds_profile_tech_etype             tech_type = DS_PROFILE_TECH_INVALID;
  uint8                             index = 0;
  ds_profile_hndl_type              profile_hndl = NULL;
  char                              param_info_type[DSAT_3GPP2_PARAM_TYPE_MAX_LENGTH];
  dsat_3gpp2_profile_create_parameter_s_type    param;

  memset(&param, 0x00, sizeof(dsat_3gpp2_profile_create_parameter_s_type));
  memset(&profile_config, 0x00, sizeof(ds_profile_config_type));
  
  if (tok_ptr->op == (NA|EQ|AR))
  {
    /*---------------------------------------------------------------
      Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/
    
    /*---------------------------------------------------------------
      Checking no. of arguments.
      For Profile Deletion - Exact 2 arguments (OPRT, Profile ID)
      For Profile Creation - Either 3 or 4 arguments (OPRT, TECH, PERSIST,Profile ID(Optional))
      
    ---------------------------------------------------------------*/
    
    if( tok_ptr->args_found < 2 || tok_ptr->args_found > 6 )
    {
      DS_AT_MSG1_ERROR("$QCPRFCRT: Invalid Number of arguements  %d", tok_ptr->args_found);
      return(DSAT_ERROR);
    }

    /*---------------------------------------------------------------
      Function call to convert tok_ptr->arg into parameter
      type and parameter value.
    ---------------------------------------------------------------*/
    if( DSAT_FAILURE == dsatvend_3gpp2_profile_create_get_param_info( tok_ptr, &param ) )
    {
      DS_AT_MSG0_ERROR("Invalid parameter type or value.");
      return DSAT_ERROR;
    }
    /* Operation type is a mandatory parameter */
    if(0 == (param.param_present & DSAT_PROFILE_CREATE_MASK_OPRT) )
    {
      DS_AT_MSG0_ERROR("OPRT type is not present.");
      return DSAT_ERROR;
    }

    switch(param.oprt_val)
    {
      case DSAT_3GPP2_PROFILE_DELETE:
      {
        if( !( (tok_ptr->args_found == 2) && ( param.param_present & DSAT_PROFILE_CREATE_MASK_PROFILE_ID) ) )
        {
          DS_AT_MSG1_ERROR(" Profile Deletion Failed. num_of_Arg = %d ",tok_ptr->args_found);
          return DSAT_ERROR;
        }

        if ( dsatetsipkt_is_profile_context_valid( param.profile_id_val,
                                                   DS_PROFILE_TECH_3GPP2,
                                                   dsat_get_current_ds_profile_subs_id()))

        {
          tech_type = DS_PROFILE_TECH_3GPP2;
        }
        else if ( dsatetsipkt_is_profile_context_valid( param.profile_id_val,
                                                        DS_PROFILE_TECH_EPC,
                                                        dsat_get_current_ds_profile_subs_id()))
        {
          tech_type = DS_PROFILE_TECH_EPC;
        }
        else
        {
          DS_AT_MSG1_ERROR("Invalid tech_type for profile id %d ",param.profile_id_val);
          return DSAT_ERROR;
        }
        
        profile_status = ds_profile_delete_per_sub((ds_profile_tech_etype)tech_type, 
                                           (ds_profile_num_type)param.profile_id_val,dsat_get_current_ds_profile_subs_id());
        if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
        {
          DS_AT_MSG3_ERROR("$QCPRFCRT: Cannot delete profile (%d) Tech (%d) Error (%d)",
                            param.profile_id_val, tech_type, profile_status);
          return DSAT_ERROR;
        }
        break;
      }

      case DSAT_3GPP2_PROFILE_CREATE:
      {
        if((param.param_present & DSAT_PROFILE_CREATE_MASK_PERSIST ) && 
            (param.param_present &  DSAT_PROFILE_CREATE_MASK_TECH))
        {
          profile_config.config_mask =  DS_PROFILE_CONFIG_MASK_PERSISTENCE | 
                                        DS_PROFILE_CONFIG_MASK_SUBS_ID ;
        }
        else
        {
          DS_AT_MSG1_ERROR("Mandatory fields are missing", profile_id);
          return DSAT_ERROR;
        }
           
          /*Check if a profile is present if PID is given by user*/
          if (param.param_present & DSAT_PROFILE_CREATE_MASK_PROFILE_ID)
        {
          profile_id = param.profile_id_val;
        /*-------------------------------------------------------------------------
             Check if the profile ID already exists by checking if a valid tech_type
             exists for the user given profile_id (PID).
          -------------------------------------------------------------------------*/
          if ( dsatetsipkt_is_profile_context_valid( profile_id,
                                                     DS_PROFILE_TECH_3GPP,
                                                     dsat_get_current_ds_profile_subs_id()))
          {
            tech_type = DS_PROFILE_TECH_3GPP;
          }
          else if ( dsatetsipkt_is_profile_context_valid( profile_id,
                                                          DS_PROFILE_TECH_3GPP2,
                                                          dsat_get_current_ds_profile_subs_id()))
          {
            tech_type = DS_PROFILE_TECH_3GPP2;
          }
          else if ( dsatetsipkt_is_profile_context_valid( profile_id,
                                                          DS_PROFILE_TECH_EPC,
                                                          dsat_get_current_ds_profile_subs_id()))
          {
            tech_type = DS_PROFILE_TECH_EPC;
          }

          if (tech_type != DS_PROFILE_TECH_INVALID)
          {
            DS_AT_MSG1_ERROR("Profile ID %d already exists", profile_id);
             return DSAT_ERROR;
          }
          else
          {
            /* Could not acquire handle. Profile ID does not exist already */
            /* No need to end transaction if Begin Transaction was not successful */
            profile_config.num = param.profile_id_val;
              profile_config.config_mask |=  DS_PROFILE_CONFIG_MASK_PROFILE_ID;
            }
          }
        }
        profile_config.is_persistent = param.persistent_val;
        profile_config.subs_id = dsat_get_current_ds_profile_subs_id();
        profile_status = ds_profile_create_ex( param.tech_val, 
                                               &profile_config,
                                              (ds_profile_num_type*)&profile_id );
        
        if (profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
        {
          DS_AT_MSG2_ERROR("$QCPRFCRT: Cannot create profile for tech type %d. Error %d",
                            param.tech_val, profile_status);
          return DSAT_ERROR;
        }

        DSAT_BEGIN_PROFILE_TRANSACTION(profile_id,param.tech_val,profile_hndl);

        /* Set the APN_NAME and PDP_TYPE params if present*/
        if(param.param_present & DSAT_PROFILE_CREATE_MASK_APN_NAME)
        {
          /*Set the general param APNNAME after profile creation*/
          memset(param_info_type,0,DSAT_3GPP2_PARAM_TYPE_MAX_LENGTH);
          strlcpy(param_info_type, "APNNAME",sizeof(param_info_type));
          if( DSAT_FAILURE == dsatvend_profile_set_param_details( param.tech_val, profile_hndl,
                                                                 param_info_type, param.apn_name ) )
          {
            DSAT_END_PROFILE_TRANSACTION(profile_hndl);
            return DSAT_ERROR;
          }
        }
        /*Set the common or tech specific param PDP Type after profile creation*/
        if(param.param_present & DSAT_PROFILE_CREATE_MASK_PDP_TYPE)
        {
          memset(param_info_type,0,DSAT_3GPP2_PARAM_TYPE_MAX_LENGTH);
          
          if (param.tech_val == DS_PROFILE_TECH_3GPP)
          {
            strlcpy(param_info_type, "PDPTYPE",sizeof(param_info_type));
          }
          else if (param.tech_val == DS_PROFILE_TECH_3GPP2)
          {
            strlcpy(param_info_type, "PDNTYPE",sizeof(param_info_type));
          }
          else if (param.tech_val == DS_PROFILE_TECH_EPC)
          {
            strlcpy(param_info_type, "CPDPTYPE",sizeof(param_info_type));
          }
          if( DSAT_FAILURE == dsatvend_profile_set_param_details( param.tech_val, profile_hndl,
                                                                 param_info_type, param.pdp_type ) )
          {
            DSAT_END_PROFILE_TRANSACTION(profile_hndl);
            return DSAT_ERROR;
          } 
        }

        /*-------------------------------------------------------------------------
          End transaction
        -------------------------------------------------------------------------*/
        DSAT_END_PROFILE_TRANSACTION(profile_hndl);

        res_buff_ptr->used = (word) snprintf((char*)&res_buff_ptr->data_ptr[0],
                                               res_buff_ptr->size,
                                               "$QCPRFCRT: %d",profile_id);
        break;
      default:
       break;
      }
    }
   
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /*---------------------------------------------------------------
      Processing for test command
    ---------------------------------------------------------------*/
    res_buff_ptr->used = (word) snprintf((char *)res_buff_ptr->data_ptr,
                                res_buff_ptr->size,
                                "$QCPRFCRT: OPRT:(%d,%d),TECH:(%d,%d),PERSIST:(%d,%d),PID:(%d,%d),"
                                "APNNAME:\"\",PDPTYPE:\"\"",
                                DSAT_3GPP2_PROFILE_CREATE, DSAT_3GPP2_PROFILE_DELETE,
                                DS_PROFILE_TECH_3GPP2 ,DS_PROFILE_TECH_EPC,
                                DSAT_NON_PERSISTENT, DSAT_PERSISTENT,
                                100 ,179); /*range 100 to 179 */
  }

  else if (tok_ptr->op == (NA|QU))
  {
    /*---------------------------------------------------------------
      Processing for read command
    ---------------------------------------------------------------*/
/*  BUFFER_LIMIT defines the maximum number of character that are going to
    to be written into DSM item to display details of each profile. Here, 156 includes
    the total bytes needed to write all the param types and their values and some extra buffer.*/
#define BUFFER_LIMIT (DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 156)

    ds_profile_tech_etype           tech_array[]={DS_PROFILE_TECH_3GPP2,
                                                  DS_PROFILE_TECH_EPC,
                                                  DS_PROFILE_TECH_MAX};
    ds_profile_itr_type             itr;
    ds_profile_list_info_type       list_info;
    ds_profile_list_type            search_list_info;
    ds_profile_info_type            profile_info;
    byte                            value[150];
    char                            apn_value[DS_PROFILE_3GPP_MAX_APN_STRING_LEN+1];
    dsat_num_item_type              is_persistent = 0;
    ds_profile_identifier_type      identifier_apn = DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN;
    ds_profile_identifier_type      identifier_pdp_type = DS_PROFILE_TECH_COMMON_PROFILE_PARAM_PDP_TYPE;
    dsat_num_item_type              num_val;

    /*-------------------------------------------------------------------------
      Get the iterator. If there is no entry then the DS Profile will return
      LIST_END error code. Fill number of instances as 0 and return success.
    -------------------------------------------------------------------------*/
    for(index = 0; tech_array[index] != DS_PROFILE_TECH_MAX; index++)
    {
      memset(&search_list_info, 0, sizeof(search_list_info));
      memset(&itr, 0, sizeof(ds_profile_itr_type));
      
      /* Default operation - Get all profiles */
      search_list_info.dfn = DS_PROFILE_LIST_ALL_PROFILES;
      
      profile_status = ds_profile_get_list_itr_per_sub( tech_array[index], 
                                                &search_list_info, 
                                                &itr,
                                                dsat_get_current_ds_profile_subs_id());
      
      if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
      {
        DS_AT_MSG1_ERROR("$QCPRFCRT: Getting profile list iterator failed. Error %d ",
                           profile_status);
        continue;    
      }
    
      while(profile_status != DS_PROFILE_REG_RESULT_LIST_END)
      {
        memset(&list_info, 0, sizeof (ds_profile_list_info_type));
        memset(&profile_info, 0, sizeof (ds_profile_info_type));
        memset(value, 0, sizeof(value));

        list_info.name = &profile_info;
        list_info.name->len  = sizeof(value);
        list_info.name->buf  = (void*)value;

      /*-----------------------------------------------------------------------
        Call the profile registry API to get the profile info
      -----------------------------------------------------------------------*/
        profile_status = ds_profile_get_info_by_itr(itr, &list_info);
        if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
        {
          DS_AT_MSG1_ERROR ("$QCPRFCRT: Getting profile info by iterator failed. Error %d",
                             profile_status);
          (void) ds_profile_itr_destroy(itr);
          return DSAT_ERROR;
        }
        (void)ds_profile_get_persistence_from_profile_num_per_sub (list_info.num,(boolean *)&is_persistent, dsat_get_current_ds_profile_subs_id());

        DSAT_BEGIN_PROFILE_TRANSACTION(list_info.num,tech_array[index],profile_hndl);

        /*APN NAME is special case of general params*/
        if (tech_array[index] == DS_PROFILE_TECH_EPC)
        {
          identifier_apn = DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN;
          identifier_pdp_type = DS_PROFILE_TECH_COMMON_PROFILE_PARAM_PDP_TYPE;
        }
        else if (tech_array[index] == DS_PROFILE_TECH_3GPP2 )
        {
          identifier_apn      = DS_PROFILE_3GPP2_PROFILE_PARAM_APN_STRING;
          identifier_pdp_type = DS_PROFILE_3GPP2_PROFILE_PARAM_PDN_TYPE;
        }

        memset(apn_value,0,sizeof(apn_value));
        profile_info.buf = (void*)apn_value;
        profile_info.len = sizeof(apn_value);
        profile_status =  ds_profile_get_param( profile_hndl, 
                                                identifier_apn, 
                                                &profile_info);

        memset(&num_val,0x0,sizeof(num_val));
        profile_info.buf = (void*)&num_val;
        profile_info.len = sizeof(ds_profile_3gpp2_pdn_type_enum_type); /*Always one byte*/
        profile_status =  ds_profile_get_param( profile_hndl, 
                                                identifier_pdp_type, 
                                                &profile_info);

        /* PDPType (0-2) exists for 3GPP2 and Common*/
        if (num_val > DS_PROFILE_3GPP2_PDN_TYPE_UNSPEC)
        {
          DS_AT_MSG1_ERROR ("Invalid PDP_TYPE: %d", num_val);
          DSAT_PROFILE_DESTROY_ITERATOR(itr);
          return DSAT_ERROR;
        }
        
        /* Check to see if a new DSM item is required */    
        CHECK_APPEND_RESPONSE_BUFFER(res_buff_ptr->size - BUFFER_LIMIT)
        
        res_buff_ptr->used += (word) snprintf( (char*)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                              res_buff_ptr->size - res_buff_ptr->used,
                                              "$QCPRFCRT: PID:%d,TECH:%d,PERSIST:%d,APNNAME:\"%s\",PDPTYPE:\"%s\"\n",
                                               list_info.num,
                                               tech_array[index],
                                               is_persistent,
                                               apn_value,
                                               dsat_p2_pdptype[num_val]);

        DSAT_END_PROFILE_TRANSACTION(profile_hndl);

        profile_status = ds_profile_itr_next(itr);
        if ( (profile_status != DS_PROFILE_REG_RESULT_SUCCESS) &&
             (profile_status != DS_PROFILE_REG_RESULT_LIST_END) )
        {
          DS_AT_MSG1_ERROR("Getting Next iterator failed. Error %d",
                           profile_status);
          DSAT_PROFILE_DESTROY_ITERATOR(itr);
          return DSAT_ERROR;
        }
      } /* while (profile_status != DS_PROFILE_REG_RESULT_LIST_END) */
      /*-------------------------------------------------------------------------
        Destroy the iterator
      -------------------------------------------------------------------------*/
      DSAT_PROFILE_DESTROY_ITERATOR(itr);
    }
    if( res_buff_ptr->used == 0 )
    {
       res_buff_ptr->used = (word) snprintf((char*)&res_buff_ptr->data_ptr[0],
                                res_buff_ptr->size,
                                "$QCPRFCRT: ");
    }
  }
  return result;
}/*dsatvend_exec_qcprfcrt_cmd*/


