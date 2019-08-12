/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                U M T S

                P A C K E T   D O M A I N
                                
                A T   C O M M A N D   P R O C E S S I N G

GENERAL DESCRIPTION
  This module executes AT commands. It executes UMTS & GPRS specific packet
  domain AT commands.

  
EXTERNALIZED FUNCTIONS

  dsatetsipkt_exec_cgclass_cmd
    This function takes the result from the command line parser 
    and executes it. It handles the +CGCLASS command. It supports
    only CLASSB. 

  dsatetsipkt_exec_cgpaddr_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGPADDR command.
    This command returns a list of PDP addresses for the specified
    context identifiers

  dsatetsipkt_cgpaddr_response_fmt
    This function generates the response for the +CGPADDR read & test
    commands.  Data is extracted from data structure and formatted according
    to parameter flag.
  
  dsatetsipkt_exec_cgdcont_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGDCONT command.
    This command sets basic PDP context parameters.

  dsatetsipkt_exec_cgdscont_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGDSCONT command.
    This command sets PDP secondary context parameters.

  dsatetsipkt_exec_cgtft_cmd  
    This function takes the result from the command line parser
    and executes it. It handles the +CGTFT command.
    This command sets PDP Traffic Flow Template parameters.

  dsatetsipkt_cgdcont_response_fmt
    This function generates the response for the +CGDCONT read & test
    commands.  Data is extracted from data structure and formatted according
    to parameter flag.

  dsatetsipkt_exec_cgeqreq_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGEQREQ command.
    This command sets PDP context parameters for UMTS QoS Profile
    (Requested).

  dsatetsipkt_exec_cgeqmin_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGEQMIN command.
    This command sets PDP context parameters for UMTS QoS Profile
    (Minimum).

  dsatetsipkt_cgeqreq_response_fmt
    This function generates the response for the +CGEQREQ read & test
    commands.  Data is extracted from data structure and formatted according
    to parameter flag.

  dsatetsipkt_exec_cgqreq_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGQREQ command.
    This command sets PDP context parameters for GPRS QoS Profile
    (Requested).

  dsatetsipkt_cgqreq_response_fmt
    This function generates the response for the +CGQREQ read & test
    commands.  Data is extracted from data structure and formatted according
    to parameter flag.

  dsatetsipkt_exec_cgqmin_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGQMIN command.
    This command sets PDP context parameters for GPRS QoS Profile
    (Minimum Acceptable).

  dsatetsipkt_cgqmin_response_fmt
    This function generates the response for the +CGQMIN read & test
    commands.  Data is extracted from data structure and formatted according
    to parameter flag.

  dsatetsipkt_exec_qcpdpp_cmd
    This function takes the result from the command line parser
    and executes it. It handles the $QCDPDP command.
    This command sets PDP authentication parameters.

  dsatetsipkt_exec_cgatt_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGATT command.
    This command reports the connection status for network packet
    domain service.

  dsatetsipkt_exec_cgact_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGACT command.
    This command activates or deactivates PDP context profiles.

  dsatetsipkt_reset_pdp_context_profiles
    This function resets all persistent PDP context profiles to their
    default values.

  dsatetsipkt_exec_cgcmod_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGCMOD command.
    This command modifies the Qos, TFT and APN of active PDP contexts.

  dsatetsipkt_exec_cgauto_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGAUTO command. This command disables or 
	enables an automatic response for a MT PDP Context Activation message. 
	
  dsatetsipkt_exec_cgans_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CGANS command.
	This command is used to answer a incoming MT PDP call.

  dsatetsipkt_exec_cemode_cmd
    This function takes the result from the command line parser
    and executes it. It handles the +CEMODE command.

  dsatetsipkt_exec_cnmpsd_cmd
    This function executes +CNMPSD command which releases PS signaling.
    This command indicates that no application on the MT is expected to
    exchange data.

    
INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2001 - 2015 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatetsipkt.c_v   1.7   12 Jul 2002 10:05:36   randrew  $  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatetsipkt.c#1 $   $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/13/15   sc      Fixed +CGDCONT AT read command parameter display issue.
02/12/15   tk      Fixed compilation errors when feature flag
                   FEATURE_SECONDARY_PDP is undefiend.
01/26/15   sc      Fixed +CGTFT\+QCGTFT AT command issue.
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
08/12/14   pg      Modified DS Profile APIs to per subs.
06/27/14   tk      Added support for Dynamic ATCoP.
05/22/14   sc      Fixed QCPDPP issue.
04/17/14   sc      Fixed ATCoP KW errors.
03/14/14   tk/sc   Optimized debug macros usage in ATCoP.
01/24/14   sc      Fixed ATCoP KW errors.
01/07/14   sc      Converted high usage ATCoP macros to functions.
11/14/13   tk      Fixed issues in online command state.
09/25/13   sc      Added support for +CNMPSD command.
09/13/13   sc      Fixed KW errors.
05/22/13   sk      Fixed Issues with AT+CGCONTRDP and CGTFTRDP.
03/27/13   tk      Fixed issues in dual stack commands.
03/21/13   sk      Fixed CGPADDR dual IP separator issue.
02/01/13   sk      Fixed CGEREP URC Issue with PDP reject.
08/22/12   sk      Mainlining Nikel Changes.
07/26/12   sb      Fixed KW Errors.
07/25/12   tk      Fixed CGPADDR issue in display of static IP addresses.
07/19/12   sk      CGACT issue fixed.
05/18/12   tk      Migrated to MSG 2.0 macros
03/19/12   sk      traffic handling priority check removed for CGEQREQ.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
02/06/12   ad      Added CSTF/CEN/CGPIAF/CVMOD command support.
01/19/12   sk      Feature cleanup.
01/04/12   sb      Fixed KW Warnings.
12/06/11   sb      Avoid QOS mismatch between GPRS and Umts profile data.
11/03/11   sb      Fixed KW Warnings.
10/05/11   ht      Fixed Klocwork Issue.
09/13/11   ttv     Changes to add src_stat_desc as profile parameter.
07/27/11   ttv     Added changes required for source statistics descriptor
                   addition.
05/23/11   ad      Allowed to set Packet Filter ID from 1 to 16.
04/01/11   ttv     Added changes to use get/set functions.
03/01/11   nc      Modified +CEMODE to be inline with CM Design.
02/16/11   ttv     Added DSDS changes for consolidated profile family.
02/16/11   bs      Fixed memory alignment issue.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
10/06/10   ad      Added Error msg before Assert(0).
09/16/10   nc      Fixed CGCONTRDP.
09/16/10   nc      Fixed the loop for forwarding a secondary PDP context
                   request to MH.
08/31/10   sa      Added support for default pdp type as IPv4v6.
07/09/10   bs      Fixed +CGACT for LTE.
05/31/10   ad      Fixed Klocwork errors. 
05/15/10   nc      Added support fot +CGCONTRDP,+CGSCONTRDP,
                   +CGEQOSRDP and +CGTFTRDP for LTE.
05/14/10   sa      Added support for LTE packet event reporting.
04/20/10   bs      Added support for +CGCMOD for LTE.
03/18/10   nc      Added Support for +CGEQOS and +CGPADDR for LTE.
01/15/10   ua      Using new send_response_to_te API. 
01/06/10   bs      Added +CGACT command support for LTE.
01/04/10   vk      Removing certain macros from PDP reg API header file
                   that cause problems with autogenerated RPC stub code
12/15/09   nc      Featurisation changes for LTE.
12/07/09   nc      Added support for <IPv4AddrAlloc> parameter for +CGDCONT.
10/12/09   vk      PDP profile changes for IPv4v6 support
10/09/09   sa      Adding support for +CEMODE command.
09/10/09   nc      Fixed Klocwork errors. 
09/21/09   sch     Accepting DS_UMTS_PDP_AUTH_CHAP_OR_PAP as a valid auth type
08/20/09   nc      Fixed the conversion of Uplink Max Bit rate for Rel7.
08/10/09   ars     CMI featurization related changes. Modified NUM_PROTOCOL_ID
06/29/09   vg	   Replacing MSG_XXX with MSG_SPRINTF_X.
04/20/09   ua      Fixed issues related with call commands followed by +CIND Query.
06/25/09   vk      Removing use of certain deprecated macros in pdp reg api
06/18/09   sa      Integrated the support for higher maximum bitrate and
                   guaranteed bit rate in the QoS IE for Rel7.
05/19/09   vk      Defeaturization of PDP registry for CMI
04/16/09   sa      Added DS_UMTS_QOS_RES_BER_1E5 for Conversational class.
01/30/09   ua      Added support of CGEV for DUN calls.
12/12/08   ua      Fixed Off target lint errors.
12/02/08   ss      Off target Lint fix.
10/23/08   bs      Added support for 1X AT Phonebook commands.
09/03/08   bs      Fixed Alignment issues with MDM8200.
07/31/08   nc      Verification of transfer delay is 
                   ignored for interactive and background class 
03/24/08   bs      Fixed Klocwork criticals.
11/04/07   pp      Lint low - fixes.
10/09/07   ss      Added support for $QCGANSM, $QCGARL, $QCGAPL commands.
08/22/07   sa      Added support for +CGANS and +CGAUTO commands.
08/06/07   sa      Removed +CGEQREQ and +CGEQMIN support in GSM only builds.
08/02/07   sa      Feature wrap changes on header and data compression for +CGDCONT
                   and +CGDSCONT.
07/12/07   sa      Fixing side effects of Banned APIs changes. 
04/26/07   sa      Fixed lint medium errors.
05/02/07   ss      Modified dsatetsipkt_setup_cm_orig_params to fix compilation
                   error when FEATURE_SECONDARY_PDP is disabled
04/05/07   pkp     +CGATT syntax corrections.
03/30/07   sa      Fixing buffer size for APN string in +CGDCONT.
03/29/07   ua      Reverting back CGCLASS modifications which supported CLASS B.
                   Now it supports only CLASS A. 
02/15/07   ss      Fixed lint high errors
02/15/07   ph      Lint CRITICAL fixes
02/13/07   pkp     Fixed RVCT compiler warnings.
01/30/07   ss      Replace banned string API calls.
12/07/07   ua      Fixing buffer overflow and also correcting the display 
                   for cgdscont test command.
11/03/06   snb     Dual processor changes.
10/30/06   ua      Correcting +CGACT state value verification.
10/27/06   sa      Fix for cid values not getting undefined for CGPADDR.
09/14/06   snb     Comply with IPCP API change.
08/03/06   sa      Add header and data compression support for +CGDCONT and +CGDSCONT.
08/01/06   snb     Clear query_mode on failure.
06/08/06   ua      Modification of +CGCLASS command to support only CLASSB.
02/21/06   sa      Merged changs from Sirius Branch
                   -Fixed additional buffer allocation for 
                   etsipkt_initiate_packet_call.
12/08/05   snb     Add IPV6 support
11/30/05   snb     Add support for +CGCMOD command.
10/02/05   snb     On +CGATT detach command when already detached still send 
                   user preference to lower layers.
07/29/05   snb     Use correct mode type in call to dial command for +CGDATA
                   processing.
05/09/05   tkk     Modification to dsat_addr_info for CID retrieval.
05/03/05   tkk     Added execution syntax to +CGPADDR command.
04/12/05   snb     Removed mutual exclusive setting of +CGQREQ/+CGEQREQ and
                   +CGQMIN/+CGEQMIN parameters.
04/05/05   tkk     Corrected test syntax for +CGPADDR command.
04/04/05   snb     Made external a function returning number of contexts 
                   without an assigned TFT and removed verification of 
                   "1 context without TFT" rule.
03/15/05   tkk     Fixed bugs for +CGPADDR and +CGEREP commands.
01/31/05   tkk     Added +CGDATA command.
01/27/05   pdv     Merge from Sirius-ATCoP development.
01/27/05   pdv     Rebase from Mainline.
01/17/05   tkk     Added support for +CGEREP command.
01/11/05   snb     Output +CGDCONT's PDP address as quoted string.
01/10/05   tkk     Added support for +CGPADDR command.
11/22/04    rr     Set higher order byte of profile number to DS_UMTS_PDP_
                   ATCOP_PROFILE_FAMILY to access profiles from 
                   "pdp_profile/atcop_profiles" directory
09/13/04   snb     Fix buffer overflow in $QCPDPP read command when 16 profiles
                   defined and auth parms set for each with maximum length user
                   names.
06/21/04   snb     Add support for 16 PDP profiles.
06/01/04   snb     Corrected +CGDCONT portion of &V response.
05/12/04   snb     Corrected +CGACT reponse when no contexts defined.
01/05/04   ar      Added mapping for profile QoS delivery err SDUs param.
11/19/03   ar      Replace dsatutil_format_ipaddr with dsatutil_convert_tuple
                   Added support for +CGDSCONT and +CGTFT commands.
                   Removed unused etsipkt_reset_multi_param().
09/29/03   ar      Remove default value handling on +CGACT parameters.
09/25/03   sb      Fix the order in which h_comp and d_comp parameters of 
                   +CGDCONT are copied into the profile (EFS).
08/13/03   snb     Added support for $QCPDPLT command
06/24/03   ar      Remove LOCAL on staticly declared structures.
03/20/03   ar      Generate +CGACT response only for defined contexts.
                   Gracefully handle PDP context API failure in &V response.
03/17/03   ar      Move VALID_TOKEN macro to common header file.
02/26/03   ar      Reset profile section to default before update on
                   +CGxxxx write syntax
02/11/03   ar      Use persistent context profile for +CGEQMIN. Remove order
                   dependency on context profile commands.
02/03/03   ar      Suppress action in +CGATT if demand same as current state.
01/30/03   ar      Add support for +CGEQMIN command
01/10/03   sb      Added WRITE operation for +CGATT command
01/06/03   ar      Migrated context cmds to persistent PDP profile support 
08/26/02   ar      Move special paramater validation before saving input
                   parameters. Code review action items.
07/11/02   ar      Add support for +CGQMIN command
07/10/02   ar      Add validation of +CGDCONT APN parameter
06/28/02   ar      Add support for +CGEQREQ and +CGQREQ commands
04/11/02   ar      Add support for $QCPDPP command
02/04/02   ar      Add support for +CGDCONT command
08/27/01   sb      Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#include <stringl/stringl.h>

#ifdef FEATURE_DSAT_ETSI_DATA
#include "stdio.h"

#include <stringl/stringl.h>
#include "msg.h"
#include "nv.h"
#include "dstaski.h"

#include "dsati.h"
#include "dsatparm.h"
#include "dsatctab.h"
#include "dsatetsictab.h"
#include "dsatetsime.h"
#include "dsatetsipkt.h"
#include "dsatetsicmif.h"
#include "ds_profile_3gppi.h"
#include "dstaski.h"
#include "nv.h"
#include "sm.h"
#include "sm_common.h"
#include "cm.h"
#include "dsatact.h"
#include "ps_ppp_defs.h"
#include "ps_utils.h"
#include "ps_in.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
  These macros used to be in dsumtspdpreg.h but had to removed from there 
  due to compilation issues seen with pdp reg rpc stub code.
-------------------------------------------------------------------------*/
#define pdp_addr_ipv4 pdp_addr.pdp_addr_ipv4
#define pdp_addr_ipv6 pdp_addr.pdp_addr_ipv6

/* Constant Macros */
#define MAX_INDEX 255   /* Table terminator */
#define MAX_TMP_BUFF_SIZE 200     /* Temporary buffer limit */

/* Macro to perform assigment only if user parameter specified */
#define CONDITIONAL_ASSIGN(i,dest,src)\
          if (VALID_TOKEN(i))\
          {\
            dest = src;\
          }

        /*Begin and end transactions are not needed for reset param
          After Reset to default read command return default values*/
#define ETSIPKT_RESET_DEFAULT_PROFILE_PARAM(profile_num,param_type)\
            profile_status = dsat_profile_reset_param_to_default(tech_type,\
                                                         profile_num,\
                                                         param_type);

        /*Begin and end transactions are not needed for reset param
          After Reset to Invalid read command will return ERROR */
#define ETSIPKT_RESET_INVALID_PROFILE_PARAM(profile_num,param_type,tech_type)\
            profile_status = dsat_profile_reset_param_to_invalid(tech_type,\
                                                         profile_num,\
                                                         param_type);

/* Macro to reset the context base paramaters  */
#define RESET_BASE_CONTEXT_PARAMS(cid)\
         if(etsipkt_delete_profile_context (cid) != TRUE)\
        { \
           result = DSAT_ERROR;\
         }\

/* Macro to test pbase profile validity and set valid flag  */
#define IS_BASE_CONTEXT_VALID(cid,result)\
        result = \
            dsatetsipkt_is_profile_context_valid( cid, DS_PROFILE_TECH_3GPP, \
                                                  dsat_get_current_ds_profile_subs_id())

#ifdef FEATURE_DATA_PS_IPV6
  /* IP(0)/IPv4(0),,PPP(1), IPv6(2) and IPv4v6(3)are valid */
  #define DSAT_MAX_PDP_TYPE DS_PROFILE_3GPP_PDP_IPV4V6 
#else  /* FEATURE_DATA_PS_IPV6 */
  /* IP,PPP and IPv4v6 are valid */
  #define DSAT_MAX_PDP_TYPE  2  
#endif /* FEATURE_DATA_PS_IPV6 */

#ifdef FEATURE_SECONDARY_PDP
/* Enum for TFT parameter references */
enum cgtft_params
{
  CGTFT_CID,
  CGTFT_PFI,
  CGTFT_EPI,
  CGTFT_SA_SM,
  CGTFT_PN_NH,
  CGTFT_DSTP,
  CGTFT_SRCP,
  CGTFT_SPI,
  CGTFT_TOS,
  CGTFT_FLOW,
  CGTFT_MAX      /* Internal use only! */
};
#endif /* FEATURE_SECONDARY_PDP */

typedef  enum
{
  TFT_FILDER_ID_1 = DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID1,
  TFT_FILTER_ID_2 = DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID2,
  TFT_FILTER_ID_MAX,
}filter_id_e_type;

/* Defaults from dsatetsictab.c */
extern const dflm_type dsat_pdp_cid_dflm[];
extern const dflm_type dsat_cgdcont_ipv4_addr_alloc_dflm[];
extern const dflm_type dsat_emergency_dflm[];

extern const dsat_string_item_type dsat_pdptype_valstr[][8];
extern const dsat_string_item_type dsat_cgdcont_hcomp_valstr [][8];
extern const dsat_string_item_type dsat_cgdcont_hcomp_tststr[];
extern const dsat_string_item_type dsat_cgdcont_dcomp_valstr [][8];
extern const dsat_string_item_type dsat_cgdcont_dcomp_tststr[];

#ifdef FEATURE_SECONDARY_PDP
extern const dflm_type dsat_cgtft_pfid_dflm[];
extern const dflm_type dsat_cgtft_prec_dflm[];
extern const dflm_type dsat_cgtft_pn_nh_dflm[];
extern const dflm_type dsat_cgtft_spi_dflm[];
extern const dflm_type dsat_cgtft_flow_dflm[];
#endif /* FEATURE_SECONDARY_PDP */

#ifdef FEATURE_DATA_WCDMA_PS
extern const dflm_type dsat_cgeqreq_tclass_dflm[];
extern const dflm_type dsat_cgeqreq_req_ul_bitrate_dflm[];
extern const dflm_type dsat_cgeqreq_req_dl_bitrate_dflm[];
extern const dflm_type dsat_cgeqreq_gtd_ul_bitrate_dflm[];
extern const dflm_type dsat_cgeqreq_gtd_dl_bitrate_dflm[];
extern const dflm_type dsat_cgeqreq_dlvry_order_dflm[];
extern const dflm_type dsat_cgeqreq_max_sdu_size_dflm[];
extern const dsat_string_item_type dsat_cgeqreq_sdu_err_tststr[];
extern const dsat_string_item_type dsat_cgeqreq_res_biterr_tststr[];
extern const dflm_type dsat_cgeqreq_dlvry_err_dflm[];
extern const dflm_type dsat_cgeqreq_trans_delay_dflm[];
extern const dflm_type dsat_cgeqreq_thandle_prio_dflm[];
extern const dflm_type dsat_cgeqreq_src_stat_desc_dflm[];
extern const dflm_type dsat_cgeqreq_sig_ind_dflm[];
extern const dsat_string_item_type dsat_cgeqreq_sdu_err_valstr[][8];
extern const dsat_string_item_type dsat_cgeqreq_res_biterr_valstr [][8];

extern const dflm_type dsat_cgeqmin_tclass_dflm[];
extern const dflm_type dsat_cgeqmin_req_ul_bitrate_dflm[];
extern const dflm_type dsat_cgeqmin_req_dl_bitrate_dflm[];
extern const dflm_type dsat_cgeqmin_gtd_ul_bitrate_dflm[];
extern const dflm_type dsat_cgeqmin_gtd_dl_bitrate_dflm[];
extern const dflm_type dsat_cgeqmin_dlvry_order_dflm[];
extern const dflm_type dsat_cgeqmin_max_sdu_size_dflm[];
extern const dsat_string_item_type dsat_cgeqmin_sdu_err_tststr[];
extern const dsat_string_item_type dsat_cgeqmin_res_biterr_tststr[];
extern const dflm_type dsat_cgeqmin_dlvry_err_dflm[];
extern const dflm_type dsat_cgeqmin_trans_delay_dflm[];
extern const dflm_type dsat_cgeqmin_thandle_prio_dflm[];
extern const dflm_type dsat_cgeqmin_src_stat_desc_dflm[];
extern const dflm_type dsat_cgeqmin_sig_ind_dflm[];
extern const dsat_string_item_type dsat_cgeqmin_sdu_err_valstr[][8];
extern const dsat_string_item_type dsat_cgeqmin_res_biterr_valstr [][8];
#endif /* FEATURE_DATA_WCDMA_PS */

extern const dflm_type dsat_cgqreq_prec_dflm[];
extern const dflm_type dsat_cgqreq_delay_dflm[];
extern const dflm_type dsat_cgqreq_rel_dflm[];
extern const dflm_type dsat_cgqreq_peak_dflm[];
extern const dflm_type dsat_cgqreq_mean_dflm[];

extern const dflm_type dsat_cgqmin_prec_dflm[];
extern const dflm_type dsat_cgqmin_delay_dflm[];
extern const dflm_type dsat_cgqmin_rel_dflm[];
extern const dflm_type dsat_cgqmin_peak_dflm[];
extern const dflm_type dsat_cgqmin_mean_dflm[];

/* Flag for detach writes intended for local stack only:
   This change for GCF test cases allows user preference to be given to 
   local stack only and lets async event processing terminate once CMPH
   event is received as no serving system event is expected.  */
extern boolean dsatcmif_ss_evt_not_expected;

#ifdef FEATURE_DSAT_LTE
extern const dflm_type dsat_qci_dflm[];
extern const dflm_type dsat_cgeqos_req_dl_gbr_dflm[];
extern const dflm_type dsat_cgeqos_req_ul_gbr_dflm[];
extern const dflm_type dsat_cgeqos_req_dl_mbr_dflm[];
extern const dflm_type dsat_cgeqos_req_ul_mbr_dflm[];
#endif /* FEATURE_DSAT_LTE */

/* Invalid PDP profile id */
#define INVALID_CID (0x0)

/* PDP context state information*/
byte  dsat_pdp_cid[DSAT_MAX_ALLOWED_PROFILES] = {0};  /* Context profile id */

LOCAL ds_3gpp_atif_dynamic_params_info_type  *dynamic_param_ptr;


/* Scratch buffer */
byte etsipkt_tmp_buf[MAX_TMP_BUFF_SIZE];


/* Added for +CGDATA support */
LOCAL dsat_cgdata_info_s_type    dsat_cgdata_context;

typedef PACKED struct PACKED_POST
{
  boolean     emergency_flag;
}dsat_cgdcont_ext_info;

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/

LOCAL dsat_result_enum_type etsipkt_validate_apn
(
  byte *s_ptr                           /*  Points to string  */
);

LOCAL dsat_result_enum_type etsipkt_validate_tclass
(
  const dsati_cmd_type *parse_table,                /* Ptr to cmd in parse table   */
  ds_profile_3gpp_3gpp_qos_params_type * pdata_ptr  /* Ptr to Profile data */
);

LOCAL dsat_result_enum_type etsipkt_process_param
(
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  const uint32 pindex,                  /*  Parameter index             */
  void * result_ptr ,                    /*  Pointer to result           */  
  uint16 result_buff_len           /*  Length of result buffer */
);

LOCAL boolean etsipkt_map_sdu_err
(
  const boolean  to_enum,                    /* Conversion flag  */
  dsat_num_item_type *index,                 /* Value list index */
  ds_profile_3gpp_sdu_err_ratio_type *eval   /* Enum value       */
);

LOCAL boolean etsipkt_map_dlvr_err_sdu
(
  const boolean  to_enum,                       /* Conversion flag  */
  dsat_num_item_type *index,                    /* Value list index */
  ds_profile_3gpp_deliver_err_sdu_type *eval    /* Enum value       */
);

LOCAL boolean etsipkt_map_res_ber
(
  const boolean  to_enum,                           /* Conversion flag  */
  dsat_num_item_type *index,                        /* Value list index */
  ds_profile_3gpp_residual_ber_ratio_type *eval     /* Enum value       */
);

LOCAL boolean etsipkt_map_dorder
(
  const boolean  to_enum,                            /* Conversion flag  */
  dsat_num_item_type *index,                         /* Value list index */
  ds_profile_3gpp_qos_delivery_order_type *eval      /* Enum value       */
);

LOCAL boolean etsipkt_map_tclass
(
  const boolean  to_enum,                       /* Conversion flag  */
  dsat_num_item_type *index,                    /* Value list index */
  ds_profile_3gpp_traffic_class_type *eval      /* Enum value       */
);

LOCAL boolean etsipkt_build_context_processing_list
(
  const tokens_struct_type *tok_ptr,     /* Command tokens from parser   */
  dsat_call_query_mode_e_type type_list, /* For which command */
  boolean * state_changed_ptr            /* Pointer to state change flag */
);

LOCAL dsm_item_type* etsipkt_check_response_buffer
(
  dsm_item_type  *res_buff_ptr,   /* Response buffer */
  uint16          buf_limit       /* Buffer contents limit */
);

LOCAL boolean etsipkt_print_ipv4_addr
(
  dsat_ip_addr_type       *pdp_adr,
  uint32                  src_adr,
  boolean                 is_static_addr,
  dsm_item_type           *res_buff_ptr,
  char                    *buffer
);

LOCAL boolean etsipkt_print_ipv6_addr
(
  dsat_ip_addr_type                       *pdp_adr,
  ds_profile_3gpp_pdp_addr_type_ipv6      *src_adr,
  boolean                                 is_static_addr,
  dsm_item_type                           *res_buff_ptr,
  char                                    *buffer
);

LOCAL dsat_num_item_type dsatetsipkt_qcdefprof_response_fmt
(
  const dsati_cmd_type  *parse_table,   /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr   /*  Place to put response       */
);

#ifdef FEATURE_SECONDARY_PDP
LOCAL boolean etsipkt_validate_cgtft_param_combination
(
  const tokens_struct_type *tok_ptr /* Parameter Values */
);

LOCAL boolean etsipkt_get_primary_profile_list
(
  byte * buf_ptr,    /* Pointer to buffer */
  byte   buf_size    /* Size of buffer */ 
);

LOCAL boolean etsipkt_validate_port_param
(
  byte   * buf_ptr,    /* Pointer to string buffer */
  byte     buf_size,   /* Size of string buffer */ 
  uint16 * from_ptr,   /* Pointer to from port numeric equivalent */
  uint16 * to_ptr      /* Pointer to to port numeric equivalent */
);

LOCAL boolean etsipkt_validate_address_mask_param
(
  byte   * buf_ptr,      /* Pointer to string buffer */
  uint16   buf_size,     /* Size of string buffer */ 
  uint32 * address_ptr,  /* Pointer to address numeric equivalent */
  uint32 * snmask_ptr    /* Pointer to mask numeric equivalent */
);

LOCAL boolean etsipkt_find_linked_profiles
(
  const byte   primary_id, /* Primary profile identifier */
  byte * size_ptr,         /* Size of output list */
  byte * list_ptr          /* List of secondary profile identifiers */
);

LOCAL boolean etsipkt_find_profile_group
(
  const byte  cid,         /* Profile identifier */
  byte * size_ptr,         /* Size of output list */
  byte * list_ptr          /* List of secondary profile identifiers */
);

LOCAL boolean etsipkt_reset_linked_context
(
  const byte   primary_id  /* Primary profile identifier */
);

LOCAL boolean etsipkt_validate_tft_epi_param
(
  const byte   cid,        /* Context profile identifier */
  const byte   pfi,        /* Packet filter index        */
  const byte   epi         /* Evaluation precedence index */
);

LOCAL byte etsipkt_cal_index_frm_pfi 
(
  const byte   cid,        /* Context profile identifier */
  const byte   pfi        /* Packet filter identifier */
);

#endif /* FEATURE_SECONDARY_PDP */

LOCAL boolean etsipkt_delete_profile_context
(
  const byte   primary_id  /* Primary profile identifier */
);

ds_profile_status_etype dsatetsipkt_get_pdp_profile_secondary_flag
(
  uint16 profile_number,
  ds_profile_3gpp_pdp_context_secondary_flag_type* secon_flag
);

LOCAL dsat_result_enum_type etsipkt_cgpaddr_build_cid_list
(
  const tokens_struct_type *tok_ptr,  /* Command tokens from parser  */
  uint8 *cid_list,                    /* Parse results list */
  uint8 *index                        /* Length of list */
);

LOCAL boolean etsipkt_cgpaddr_format_helper_response
(
  uint32                            cid,
  dsm_item_type                     *res_buff_ptr
);

LOCAL dsat_result_enum_type etsipkt_cgpaddr_format_response
(
  const byte               *cid_list_ptr,
  const byte                cid_list_len,
  const tokens_struct_type *tok_ptr,
  dsm_item_type            *format_buffer
);

LOCAL boolean etsipkt_cgcontrdp_format_helper_response
(
   ds_3gpp_atif_dynamic_params_info_type  *dynamic_param_ptr,
   ds_3gpp_atif_primary_dynamic_info_type *prim_dynamic_ptr,
   dsm_item_type                          *format_buffer
);

LOCAL dsat_result_enum_type etsipkt_cgcontrdp_format_response
(
  const byte               *cid_list_ptr,
  const byte                cid_list_len,
  const tokens_struct_type *tok_ptr,
  dsm_item_type            *format_buffer
);

LOCAL dsat_result_enum_type etsipkt_cgscontrdp_format_response
(
  const byte               *cid_list_ptr,
  const byte                cid_list_len,
  const tokens_struct_type *tok_ptr,
  dsm_item_type            *format_buffer
);

LOCAL dsat_result_enum_type etsipkt_cgeqosrdp_format_response
(
  const byte               *cid_list_ptr,
  const byte                cid_list_len,
  const tokens_struct_type *tok_ptr,
  dsm_item_type            *format_buffer
);

LOCAL boolean etsipkt_cgtftrdp_format_helper_response
(
  uint32                                   cid,
  ds_3gpp_atif_dynamic_params_info_type  * dynamic_param_ptr,
  dsm_item_type                          * res_buff_ptr
);

LOCAL dsat_result_enum_type etsipkt_cgtftrdp_format_response
(
  const byte               *cid_list_ptr,
  const byte                cid_list_len,
  const tokens_struct_type *tok_ptr,
  dsm_item_type            *res_buff_ptr
);

LOCAL dsat_cme_error_e_type etsipkt_cgdata_get_cid_info(byte *cid);

LOCAL dsat_result_enum_type etsipkt_initiate_packet_call(
                                      dsm_item_type *res_buff_ptr);

LOCAL void etsipkt_clear_gprs_events_buffer
(
 sys_modem_as_id_e_type subs_id
);

#ifdef FEATURE_DSAT_UMTS_DATA
#ifdef FEATURE_DATA_TE_MT_PDP
LOCAL dsat_result_enum_type etsipkt_process_access_control_write
(
  dsat_pdp_access_control_e_type operation,  /*  Acess control type  */
  const tokens_struct_type *tok_ptr          /*  Command tokens from parser  */
);

LOCAL dsat_result_enum_type etsipkt_process_access_control_read
(
  dsat_pdp_access_control_e_type operation, /*  Acess control type  */
  const tokens_struct_type *tok_ptr,        /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr               /*  Place to put response       */
);
#endif /* FEATURE_DATA_TE_MT_PDP */

#endif /* FEATURE_DSAT_UMTS_DATA */

/*-------------------------------------------------------------------------
    Function Definitions:
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION ETSIPKT_GET_PROFILE_LIST

DESCRIPTION
  This function generates the list of valid profiles either persistent
  or non-persistent per technology type. 

DEPENDENCIES
  None

RETURN VALUE 
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_get_profile_list
(
  ds_profile_tech_etype    tech_type,           /* Technology Type  */
  uint8                    *profile_list,       /* Profile_ID list index */
  uint8                    profile_list_len,    /* Max length of profile_list_possible */
  uint8                    *total_profiles,      /* Total Number of Profiles */
  ds_profile_subs_etype    ps_subs
)
{
  byte                            value[150];
  ds_profile_itr_type             itr = NULL;
  ds_profile_list_info_type       list_info;
  ds_profile_list_type            search_list_info;
  ds_profile_info_type            profile_info;
  ds_profile_status_etype         profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  dsat_result_enum_type           result = DSAT_OK;

  //check for pointers
  if ( profile_list == NULL || total_profiles == NULL)
  {
    return DSAT_ERROR;
  }
  
  /*Currently supports max 24 profiles */
  memset (profile_list, 0, sizeof(uint8)*profile_list_len);
  *total_profiles = 0;

  memset(&search_list_info, 0, sizeof(search_list_info));
  memset(&itr, 0, sizeof(ds_profile_itr_type));

  /* Default operation - Get all profiles */
  search_list_info.dfn = DS_PROFILE_LIST_ALL_PROFILES;
  profile_status = ds_profile_get_list_itr_per_sub( tech_type, 
                                            &search_list_info, 
                                            &itr,
                                            ps_subs);

  if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    DS_AT_MSG0_HIGH("No valid profile exists");
    result = DSAT_OK;
    goto return_result;  
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
      DS_AT_MSG1_ERROR ("Getting profile info by iterator failed. Error %d",
                         profile_status);
      result = DSAT_ERROR;
      goto return_result;
    }
    profile_list[*total_profiles] = (uint8)list_info.num;
    *total_profiles = *total_profiles + 1;
    profile_status = ds_profile_itr_next(itr);
    if (((profile_status != DS_PROFILE_REG_RESULT_SUCCESS) &&
         (profile_status != DS_PROFILE_REG_RESULT_LIST_END)) ||
        *total_profiles == profile_list_len )
    {
       DS_AT_MSG2_ERROR("profile_status=%d, total_profiles=%d", profile_status,
                                                             *total_profiles );
       result = DSAT_ERROR;
       goto return_result;
    }
      
  } /* while (profile_status != DS_PROFILE_REG_RESULT_LIST_END) */
return_result:

  /*-------------------------------------------------------------------------
    Destroy the iterator
  -------------------------------------------------------------------------*/
  if (itr != NULL)
  {
    profile_status = ds_profile_itr_destroy(itr); 
    if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS) 
    { 
      DS_AT_MSG1_ERROR ("Destroying iterator failed. Error %d", profile_status); 
    }
  }
  return result;
}/*etsipkt_get_profile_list*/

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGPADDR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGPADDR command.
  This command returns a list of PDP addresses for the specified
  contect identifiers

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
/* ARGSUSED */
dsat_result_enum_type dsatetsipkt_exec_cgpaddr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_OK;

  /*---------------------------------------------------------
   Processing for a command with an argument (WRITE command)
  -----------------------------------------------------------*/
  if(tok_ptr->op == (NA|EQ|AR))
  {
    if( !VALID_TOKEN(0) )
    {
      /* No CID list specified */
      result = etsipkt_cgpaddr_format_response( NULL, 0,
						tok_ptr,
						res_buff_ptr);
    }
    else
    {
      /* CID list specified and must be parsed */
      uint8 cid_list[DSAT_MAX_ALLOWED_PROFILES];
      uint8 list_len = 0;

      /* Build the CID list */
      result = etsipkt_cgpaddr_build_cid_list( tok_ptr,
                                               cid_list,
                                               &list_len );

      /* Generate response */
      if( DSAT_OK == result)
      {
        result = etsipkt_cgpaddr_format_response( cid_list, list_len,
                                                  tok_ptr,
                                                  res_buff_ptr);
      }
    }
  }
  /*-----------------------------------------------------------
   Processing for a command without an argument (WRITE command)
   and TEST syntax
  ------------------------------------------------------------*/
  else if( (tok_ptr->op == (NA)) ||
           (tok_ptr->op == (NA|EQ|QU)) )
  {
    result = etsipkt_cgpaddr_format_response( NULL, 0,
                                              tok_ptr,
                                              res_buff_ptr );
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatetsipkt_exec_cgpaddr_cmd */



/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGDCONT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGDCONT command.
  This command sets basic PDP context parameters.

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
dsat_result_enum_type dsatetsipkt_exec_cgdcont_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_ip_addr_type     ipaddr;
  dsat_num_item_type    val         = 0;
  ds_profile_hndl_type      profile_hndl = NULL;
  ds_profile_status_etype   profile_status = DS_PROFILE_REG_RESULT_SUCCESS;

#ifdef FEATURE_SECONDARY_PDP
  boolean reset_linked_profiles = FALSE;
#endif /* FEATURE_SECONDARY_PDP */
  dsat_mixed_param_val_type * val_ptr = 
           (dsat_mixed_param_val_type *)dsatutil_get_val(parse_table->cmd_id,0,0,STR_TYPE);
  dsat_error_info_s_type    err_info;
  dsat_cgdcont_ext_info     ext_info;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  memset( (void *)&ipaddr, 0, sizeof(ipaddr) );

  if (tok_ptr->op == (NA|EQ|AR))
  {
    /*---------------------------------------------------------------
      Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/
    ds_profile_3gpp_pdp_context_type pdata;
    dsat_num_item_type num_val = 0;

    /* Process connection ID */
    if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 0, &val,1))
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 0;
      goto send_error;
    }
#ifdef FEATURE_SECONDARY_PDP
    /* Query previously saved profile (if any) */
    memset((void*)&pdata, 0, sizeof(pdata));
    /* Query context profile data */
    if(dsatetsipkt_is_profile_context_valid( val, DS_PROFILE_TECH_3GPP,
                                             dsat_get_current_ds_profile_subs_id()))

    {
      if (DS_PROFILE_REG_RESULT_SUCCESS !=
            dsatetsipkt_get_pdp_profile_secondary_flag( (uint16)val , &pdata.secondary_flag) )
      {
        DS_AT_MSG1_ERROR("Error CGDCONT PDP profile read: %d",val);
        pdata.secondary_flag = FALSE;
      }      
    }
      /* Set flag to reset linked profiles on type change */
      reset_linked_profiles = (FALSE == pdata.secondary_flag)? TRUE : FALSE;
    /* Test for only CID specified */
    if (1 == tok_ptr->args_found)
    {
        /* Ensure cid to reset is primary context */
        if (TRUE == pdata.secondary_flag)
        {
          DS_AT_MSG1_ERROR("Cannot reset SECONDARY context with CGDCONT: %d",val);
          return DSAT_ERROR;
        }
        else
        {
          /* Reset any linked secondary profiles to given cid on 
           * primary undefine.  We do this to ensure secondary
           * does not loose link to its primary, becoming orphan.  */
          if (FALSE == etsipkt_reset_linked_context((byte)val))
          {
            DS_AT_MSG1_ERROR("Error resetting linked context: %d",val);
            return DSAT_ERROR;
          }
          /* Reset specified context profile */
          RESET_BASE_CONTEXT_PARAMS(val);
      }
    }
#else
    /* Test for only CID specified */
    if (DSAT_IS_PROFILE_ID_WITHIN_RANGE(val) && (1 == tok_ptr->args_found ))
    {
      /* Clear the user specified +CGDCONT context parameters */
       RESET_BASE_CONTEXT_PARAMS(val);
    }
#endif /* FEATURE_SECONDARY_PDP */
    else
    {
      byte temp_buf[MAX_IPADDR_STR_LEN+1];
      byte apn_buff[DS_PROFILE_3GPP_MAX_APN_STRING_LEN +1];
      ds_profile_config_type    config_ptr;
      ds_profile_num_type       num = 0;
      byte                      cid = (byte)val;

      
      memset( temp_buf, 0, sizeof(temp_buf) );
      memset( apn_buff, 0, sizeof(apn_buff) );
      memset( &config_ptr, 0, sizeof(ds_profile_config_type) );
      
      /* Verify IP address is correct; OK if null */
      if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 3,
                                 temp_buf,sizeof(temp_buf)))
      {
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        err_info.arg_num = 3;
        goto send_error;
      }

      if ( !( ('\0' == temp_buf[0]) ||
              (DSAT_OK == dsatutil_convert_ipaddr(STRING_TO_INT,
                                                  &ipaddr,
                                                  temp_buf)) ) )
      {
        /* There is a MSG_MACRO inside function. */
        return DSAT_ERROR;
      }
      
      /* Verify APN is correct; OK if null */
      if( (VALID_TOKEN(2) ) && ('"' != *tok_ptr->arg[2]) )
      {
        /* APN should be entered in quotes ("") */
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        err_info.arg_num = 2;
        goto send_error;
      }
      if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 2, 
                                apn_buff,sizeof(apn_buff)))
      {
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        err_info.arg_num = 2;
        goto send_error;
      }
      if (!(('\0' == apn_buff[0]) ||
            (DSAT_OK == etsipkt_validate_apn((byte*)apn_buff))))
      {
        /* There is a MSG_MACRO inside function. */
        return DSAT_ERROR;
      }

      /* Perform normal parameter processing */
      if (DSAT_OK == dsatparm_exec_param_cmd(
                                             mode,
                                             parse_table,
                                             tok_ptr,
                                             res_buff_ptr
                                            ))
      {
        /* Transfer user data to internal storage */
        if( ((val_ptr[1].num_item == DS_PROFILE_3GPP_PDP_IP && ipaddr.ip_vsn == DS_PROFILE_3GPP_IP_V6)) ||
             ((val_ptr[1].num_item == DS_PROFILE_3GPP_PDP_IPV6 && ipaddr.ip_vsn == DS_PROFILE_3GPP_IP_V4)) )
        {
          return DSAT_ERROR;
        }

#ifdef FEATURE_SECONDARY_PDP
        /* Check for linked profiles to reset */
        if (TRUE == reset_linked_profiles)
        {
          /* Reset any linked secondary profiles to given cid if profile
           * type changes from primary to secondary.  We do this to ensure
           * a secondary does not link to another secondary.  */
          if (FALSE == etsipkt_reset_linked_context(cid))
          {
            DS_AT_MSG1_HIGH("Error resetting linked context: %d",cid);
            return DSAT_ERROR;
          }
        }
#endif /* FEATURE_SECONDARY_PDP */

        /* Set specified context profile to IPV4 defaults */
        DSAT_BEGIN_PROFILE_TRANSACTION(cid,DS_PROFILE_TECH_3GPP,profile_hndl);
        if( profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
        {
          /*profile is not present create it.*/
          config_ptr.config_mask = DS_PROFILE_CONFIG_MASK_PROFILE_ID | 
                                   DS_PROFILE_CONFIG_MASK_SUBS_ID |
                                   DS_PROFILE_CONFIG_MASK_PERSISTENCE;
          config_ptr.num = cid;
          config_ptr.is_persistent = TRUE;
          config_ptr.subs_id = dsat_get_current_ds_profile_subs_id();
          profile_status = ds_profile_create_ex(DS_PROFILE_TECH_3GPP,&config_ptr,(uint16 *) &num );
          if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
          {
            DS_AT_MSG1_HIGH("Profile creation failed : %d",profile_status);
            return DSAT_ERROR;
          }
          else
          {
            cid =(byte)num;
          }
        }
        else
        {
          DSAT_END_PROFILE_TRANSACTION(profile_hndl);
          
          if(ds_profile_reset_pdp_context_per_sub(DS_PROFILE_TECH_3GPP,cid,dsat_get_current_ds_profile_subs_id()) != DS_PROFILE_REG_RESULT_SUCCESS)
          {
            DS_AT_MSG1_ERROR("Error CGDCONT PDP profile reset: %d",cid);
            return DSAT_ERROR;
          }
        }

        /* Query default profile from EFS */
        memset((void*)&pdata, 0, sizeof(pdata));
        memset((void*)&ext_info, 0, sizeof(ext_info));

        /* Make conditional edits to profile */
        pdata.pdp_context_number = cid;
        CONDITIONAL_ASSIGN(1, pdata.pdp_type, (ds_profile_3gpp_pdp_type_enum_type)val_ptr[1].num_item);
        CONDITIONAL_ASSIGN(3, pdata.pdp_addr,  ipaddr.addr);
        if( VALID_TOKEN(4) )
        {
          if( ATOI_OK == dsatutil_atoi(&num_val,
              &dsat_cgdcont_dcomp_valstr[val_ptr[4].num_item][0] ,10))
          {
            pdata.d_comp = (ds_profile_3gpp_pdp_data_comp_e_type)num_val;
          }
          else
          {
            return DSAT_ERROR;
          }
        }

        if( VALID_TOKEN(5) )
        {
          if( ATOI_OK == dsatutil_atoi(&num_val,
              &dsat_cgdcont_hcomp_valstr[val_ptr[5].num_item][0], 10) )
          {
            pdata.h_comp = (ds_profile_3gpp_pdp_header_comp_e_type)num_val;
          }
          else
          {
            return DSAT_ERROR;
          }
        }
        
        if( VALID_TOKEN(6) )
        {
          if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 6, &val,1))
          {
            DS_AT_MSG0_ERROR("Cannot convert ipv4 addr alloc type parameter");
            return DSAT_ERROR;
          }
          else
          {
            pdata.ipv4_addr_alloc = (ds_profile_3gpp_pdp_ipv4_addr_alloc_e_type)val;
          }
        }
        
        if (VALID_TOKEN(2))
        {
          (void)strlcpy((char *)pdata.apn, 
                            (const char *)val_ptr[2].string_item,
                            DS_PROFILE_3GPP_MAX_APN_STRING_LEN + 1);
        }

        if( VALID_TOKEN(7) )
        {
          if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 7, &val,1))
          {
            DS_AT_MSG0_ERROR("Cannot convert emergency flag");
            return DSAT_ERROR;
          }
          ext_info.emergency_flag = (boolean)val;
        }

        /* Write to EFS */
        if(dsatetsipkt_read_write_param_table(context_param_tbl,
                                              sizeof(context_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
                                              cid,
                                              WRITE_COMMAND,
                                              (byte*)&pdata,
                                              sizeof(pdata),
                                              dsat_get_current_ds_profile_subs_id()) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error CGDCONT PDP profile write: %d",cid);
          return DSAT_ERROR;
        }

        /* Write extended table to EFS */
        if(dsatetsipkt_read_write_param_table(dsat_cgdcont_ext_table,
                                              sizeof(dsat_cgdcont_ext_table)/sizeof(atcop_profile_3gpp_param_list_type),
                                              cid,
                                              WRITE_COMMAND,
                                              (byte*)&ext_info,
                                              sizeof(ext_info),
                                              dsat_get_current_ds_profile_subs_id()) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error CGDCONT EXT Table: %d",cid);
          return DSAT_ERROR;
        }
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /*---------------------------------------------------------------
      Processing for a command to query valid parameters
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgdcont_response_fmt (parse_table,
                                               &res_buff_ptr,
                                               MULTI_INDEX_TEST))
    {
      DS_AT_MSG0_ERROR("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|QU))
  {
    /*---------------------------------------------------------------
       Processing for a command to query current parameters
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgdcont_response_fmt (parse_table,
                                               &res_buff_ptr,
                                               MULTI_INDEX_READ))
    {
      DS_AT_MSG0_ERROR("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA))
  {
    /* Do nothing */
    result = DSAT_OK;
  }
  else
  {
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }
  return result;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_SYNTAX )
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;
} /* dsatetsipkt_exec_cgdcont_cmd */


#ifdef FEATURE_DATA_TE_MT_PDP

#define DSAT_ACCESS_CTRL_ADD  0
#define DSAT_ACCESS_CTRL_DELETE  1


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_QCGARL_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes it.
  It handles the $QCGARL command. This command sets PDP context reject list.

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
dsat_result_enum_type dsatetsipkt_exec_qcgarl_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_pdp_access_control_e_type operation = DSAT_PDP_ACCESS_CONTROL_REJECT;
  
  if (tok_ptr->op == (NA|EQ|AR))
  {
    /*  Processing WRITE command  */
    result = etsipkt_process_access_control_write( operation, tok_ptr);
  }
  else if (tok_ptr->op == (NA|QU))
  {
    /*  Processing READ command  */
    result = etsipkt_process_access_control_read( operation, tok_ptr, res_buff_ptr);
  }
  else  if (tok_ptr->op == (NA|EQ|QU))
  {
    /*  Processing TEST command  */
    res_buff_ptr->used += (word) snprintf(
                           (char *)res_buff_ptr->data_ptr, 
                           res_buff_ptr->size, 
                           "%s: (0,1)",
                           (char *) parse_table->name);
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsatetsipkt_exec_qcgarl_cmd */


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_QCGAPL_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes it.
  It handles the $QCGAPL command. This command sets PDP context permission list.

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
dsat_result_enum_type dsatetsipkt_exec_qcgapl_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_pdp_access_control_e_type operation = DSAT_PDP_ACCESS_CONTROL_PERMISSION;
  if (tok_ptr->op == (NA|EQ|AR))
  {
    /*  Processing WRITE command  */
    result = etsipkt_process_access_control_write( operation, tok_ptr);
  }
  else if (tok_ptr->op == (NA|QU))
  {
    /*  Processing READ command  */
    result = etsipkt_process_access_control_read( operation, tok_ptr, res_buff_ptr);
  }
  else  if (tok_ptr->op == (NA|EQ|QU))
  {
    /*  Processing TEST command  */
    res_buff_ptr->used += (word) snprintf(
                           (char *)res_buff_ptr->data_ptr, 
                           res_buff_ptr->size, 
                           "%s: (0,1)",
                           (char *) parse_table->name);
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsatetsipkt_exec_qcgapl_cmd */

/*===========================================================================
FUNCTION ETSIPKT_PRCOESS_ACCESS_CONTROL_WRITE

DESCRIPTION
  This function peforms the write operation to the access control list.

DEPENDENCIES
  None.
  
RETURN VALUE
  DSAT_OK
  DSAT_ERROR
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_process_access_control_write
(
  dsat_pdp_access_control_e_type operation,  /*  Access control type  */
  const tokens_struct_type *tok_ptr          /*  Command tokens from parser  */
)
{
  dsat_num_item_type cid = 0;
  dsat_num_item_type num_val = 0;
  byte i;
  char* command;
  ds_profile_3gpp_pdp_access_control_e_type access_ctrl_flag;
  dsat_error_info_s_type    err_info;
  uint8                     profile_list[DSAT_MAX_ALLOWED_PROFILES] = {0};
  uint8                     total_profiles = 0;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  /* Set the command name */
  if (operation == DSAT_PDP_ACCESS_CONTROL_REJECT)
  {
    command="$QCGARL";
  }
  else
  {
    command="$QCGAPL";
  }
  
  if (tok_ptr->args_found > 2) 
  {
    err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
    goto send_error;
  }
  else if (tok_ptr->args_found == 2) 
  {
    if(!VALID_TOKEN(1) || dsatutil_atoi(&cid,tok_ptr->arg[1],10) != ATOI_OK)
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 1;
      goto send_error;
    }
    if(!dsatetsipkt_is_profile_context_valid( cid, DS_PROFILE_TECH_3GPP,
                                              dsat_get_current_ds_profile_subs_id()))

    {
      DS_AT_MSG1_ERROR("Invalid profile %d",cid);
      return DSAT_ERROR;
    }
    /* Make conditional edits to profile */
    if(dsatutil_atoi(&num_val,tok_ptr->arg[0],10) != ATOI_OK)
    {
      err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
      err_info.arg_num = 0;
      goto send_error;
    }
    if(num_val == DSAT_ACCESS_CTRL_ADD) /* Adding*/
    {
      access_ctrl_flag = operation;
    }
    else if(num_val == DSAT_ACCESS_CTRL_DELETE)   /* Deleting */
    {
      access_ctrl_flag = DSAT_PDP_ACCESS_CONTROL_NONE;
    }
    else
    {
      err_info.errval = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
      err_info.arg_num = 0;
      goto send_error;
    }
    /* Write to EFS */
    if (DSAT_ERROR ==
          dsatetsipkt_read_write_profile_param( cid ,WRITE_COMMAND,
              DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG,
              &access_ctrl_flag,sizeof(ds_profile_3gpp_pdp_access_control_e_type)))
    {
      DS_AT_MSG_SPRINTF_2_ERROR( "Error %s PDP profile write: %d",command,cid);
       return DSAT_ERROR;
    }

  }
  else if ( tok_ptr->args_found == 1) 
  {
    ds_profile_3gpp_pdp_context_secondary_flag_type secon_flag;
    if(!VALID_TOKEN(0) || dsatutil_atoi(&num_val,tok_ptr->arg[0],10) != ATOI_OK)
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 0;
      goto send_error;
    }

    if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                               sizeof(profile_list), &total_profiles,
                                               dsat_get_current_ds_profile_subs_id()) )
    {
      DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
    }
    /* Loop over each PDP context and modify the access control*/
    for (i = 0; i < total_profiles; i++) 
    {
#ifdef FEATURE_SECONDARY_PDP
      /* Skip further processing if secondary profile QCGAPL, QCGARL are
         not applicable for secondary contexts*/
      if (dsatetsipkt_get_pdp_profile_secondary_flag(profile_list[i],&secon_flag)!= 
                                             DS_PROFILE_REG_RESULT_SUCCESS)
      {
        DS_AT_MSG1_ERROR("Error PDP profile read: %d",profile_list[i]);
        return DSAT_ERROR;
      }
      if(secon_flag == TRUE)
      {
        continue;
      }
      
#endif /* FEATURE_SECONDARY_PDP */
        if(num_val == DSAT_ACCESS_CTRL_ADD) /* Adding*/
        {
        access_ctrl_flag = operation;
        }
        else if(num_val == DSAT_ACCESS_CTRL_DELETE) /* Deleting */
        {
          /* Delete only those entries which were earlier marked 
             as the same operation */
        if (access_ctrl_flag == operation )
          {
          access_ctrl_flag = DSAT_PDP_ACCESS_CONTROL_NONE;
          }
        }
        else
        {
          err_info.errval = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
          err_info.arg_num = 0;
          goto send_error;
        }

        /* Write to EFS */
      if (DSAT_ERROR ==
            dsatetsipkt_read_write_profile_param( profile_list[i] ,WRITE_COMMAND,
                DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG,
                &access_ctrl_flag,sizeof(ds_profile_3gpp_pdp_access_control_e_type)))
        {
          DS_AT_MSG_SPRINTF_2_ERROR("Error %s PDP profile write: %d",command,profile_list[i]);
          return DSAT_ERROR;
        }
    }
  }
  else 
  {
    err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
    goto send_error;
  }
  return DSAT_OK;

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
} /* etsipkt_process_access_control_write */

/*===========================================================================
FUNCTION ETSIPKT_PRCOESS_ACCESS_CONTROL_READ

DESCRIPTION
  This function peforms the read operation to the access control list.

DEPENDENCIES
  None.
  
RETURN VALUE
  DSAT_OK
  DSAT_ERROR
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_process_access_control_read
(
  dsat_pdp_access_control_e_type operation,  /*  Access control type  */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  byte i;
  char* command;
  ds_profile_3gpp_pdp_access_control_e_type access_ctrl_flag;
  ds_profile_3gpp_pdp_context_secondary_flag_type secon_flag;
  uint8       profile_list[DSAT_MAX_ALLOWED_PROFILES] = {0};
  uint8       total_profiles = 0;
  
  /* Set the command name */
  if (operation == DSAT_PDP_ACCESS_CONTROL_REJECT)
  {
    command="$QCGARL";
  }
  else
  {
    command="$QCGAPL";
  }

  if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                             sizeof(profile_list), &total_profiles
                                             dsat_get_current_ds_profile_subs_id()) )
  {
    DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
  }
  /* Report each profile on seperate line */
  for (i = 0; i < total_profiles; i++) 
  {
#ifdef FEATURE_SECONDARY_PDP
          /* Skip further processing if secondary profile QCGAPL, QCGARL are
             not applicable for secondary contexts*/
    if (dsatetsipkt_get_pdp_profile_secondary_flag(profile_list[i],&secon_flag) != 
                                                  DS_PROFILE_REG_RESULT_SUCCESS)
    {
      DS_AT_MSG1_ERROR("Error PDP profile read: %d",profile_list[i]);
      return DSAT_ERROR;
    }
    if(secon_flag == TRUE)
    {
      continue;
    }
#endif /* FEATURE_SECONDARY_PDP */
    if (DSAT_ERROR ==
          dsatetsipkt_read_write_profile_param( profile_list[i] ,READ_COMMAND,
              DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG,
              &access_ctrl_flag,sizeof(ds_profile_3gpp_pdp_access_control_e_type)))
    {
      DS_AT_MSG_SPRINTF_2_ERROR("Error %s PDP profile write: %d",command,profile_list[i]);
      return DSAT_ERROR;
    }
    if ( access_ctrl_flag == operation )
      {
        res_buff_ptr->used = (word) snprintf(
                              (char *)res_buff_ptr->data_ptr, 
                              res_buff_ptr->size,
                              "%s\n%s:%d",
                              res_buff_ptr->data_ptr,
                              command,
                              profile_list[i]);
    }
  }
  return DSAT_OK;
} /* etsipkt_process_access_control_read */
#endif /* FEATURE_DATA_TE_MT_PDP*/

/*===========================================================================

FUNCTION DSATETSIPKT_CGDCONT_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGDCONT read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsipkt_cgdcont_response_fmt
(
  const dsati_cmd_type  *parse_table,   /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format         /*  Flag for format type        */
)
{
  byte name_buff[16];
  byte ipaddr_str[MAX_IPADDR_STR_LEN+1];
  byte i, cntr = 0;
  int size = 0;
  int result = 0;
  dsm_item_type * res_buff_ptr = *out_buff_ptr;
  ds_profile_3gpp_pdp_context_type pdata;
  dsat_cgdcont_ext_info            ext_info;
  uint8             profile_list[DSAT_MAX_ALLOWED_PROFILES] = {0};
  uint8             total_profiles = 0;

  /* Format the command name */
  (void)snprintf((char*)name_buff,
                       sizeof(name_buff),
                       "%s: ",
                       parse_table->name);
  res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
  ipaddr_str[0] = '\0';

  switch (format)
  {
    case MULTI_INDEX_TEST:
      /* Report each PDP type on seperate line */

      /*Range we support for 3GPP only profiles is: 1-16 
        Range we support for EPC profiles is:       100-179 */
      for (i=0; i <= (byte)DSAT_MAX_PDP_TYPE; i++) 
      {
        size = snprintf ((char *)res_buff_ptr->data_ptr,
                        res_buff_ptr->size,
                        "%s%s%s(%d-%d,%d-%d),\"%s\",%s,%s,%s,%s,(%d-%d),(%d-%d)",
                        res_buff_ptr->data_ptr,
                        (i > 0) ? "\n" : "",
                        name_buff,
                        DSAT_LEGACY_PROFILE_NUM_MIN,
                        DSAT_LEGACY_PROFILE_NUM_MAX,
                        DSAT_EXT_PROFILE_NUM_MIN,
                        DSAT_EXT_PROFILE_NUM_MAX,
                        dsat_pdptype_valstr[i],
                        "",
                        "",
                        dsat_cgdcont_dcomp_tststr,
                        dsat_cgdcont_hcomp_tststr,
                        ((dflm_type *)dsat_cgdcont_ipv4_addr_alloc_dflm)->lower,
                        ((dflm_type *)dsat_cgdcont_ipv4_addr_alloc_dflm)->upper,
                        ((dflm_type *)dsat_emergency_dflm)->lower,
                        ((dflm_type *)dsat_emergency_dflm)->upper
                       );
      }
      res_buff_ptr->used = (uint16)size;
      result = size;
      break;

    case MULTI_INDEX_READ:
      /* Report each profile on seperate line */

      if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                                 sizeof(profile_list), &total_profiles,
                                                 dsat_get_current_ds_profile_subs_id()) )
      {
        DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
      }
      for (i = 0; i < total_profiles; i++) 
      {
        dsat_ip_addr_type addr;

#ifdef FEATURE_SECONDARY_PDP
        /* Skip further processing if secondary profile */
        if ((dsatetsipkt_get_pdp_profile_secondary_flag(profile_list[i],&pdata.secondary_flag) 
                 == DS_PROFILE_REG_RESULT_SUCCESS) && (TRUE == pdata.secondary_flag))
        {
          continue;
        }
#endif /* FEATURE_SECONDARY_PDP */
        memset((void*)&pdata, 0, sizeof(pdata));
        memset((void*)&ext_info, 0, sizeof(ext_info));
        memset((void*)ipaddr_str, 0, sizeof(ipaddr_str));
        if(dsatetsipkt_read_write_param_table(context_param_tbl,sizeof(context_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
                                 profile_list[i],READ_COMMAND,(byte*)&pdata,sizeof(pdata),dsat_get_current_ds_profile_subs_id()) == DSAT_ERROR)
        {
          DS_AT_MSG0_ERROR("CGDCONT Read Failed");
          return 0;
        }

        if(dsatetsipkt_read_write_param_table(dsat_cgdcont_ext_table,
                                              sizeof(dsat_cgdcont_ext_table)/sizeof(atcop_profile_3gpp_param_list_type),
                                              profile_list[i],
                                              READ_COMMAND,
                                              (byte*)&ext_info,
                                              sizeof(ext_info),
                                              dsat_get_current_ds_profile_subs_id()) == DSAT_ERROR)
        {
          DS_AT_MSG0_ERROR("CGDCONT Ext Read Failed");
          return 0;
        }

        DS_AT_MSG2_HIGH("cid = %d type = %d",pdata.pdp_context_number,pdata.pdp_type);
        if(pdata.pdp_type == DS_PROFILE_3GPP_PDP_IP)
        {
         addr.ip_vsn = DS_PROFILE_3GPP_IP_V4;
        }
        else if(pdata.pdp_type == DS_PROFILE_3GPP_PDP_IPV6)
        {
          addr.ip_vsn = DS_PROFILE_3GPP_IP_V6;
        }
        else if(pdata.pdp_type == DS_PROFILE_3GPP_PDP_IPV4V6 ||
                pdata.pdp_type == DS_PROFILE_3GPP_PDP_PPP )
        {
          if(pdata.pdp_addr.ds_profile_3gpp_pdp_addr_ipv4 != 0)
          {
            addr.ip_vsn = DS_PROFILE_3GPP_IP_V4;
          }
          else
          {
            addr.ip_vsn = DS_PROFILE_3GPP_IP_V6;
          }
        }
        else
        {
          DS_AT_MSG0_ERROR("PDP Type is invalid for ATCoP");
          return 0;
        }
        addr.addr = pdata.pdp_addr;
          /* Decode PDP IP address if set */
        if ( DSAT_OK != dsatutil_convert_ipaddr(INT_TO_STRING,
                                              &addr,
                                                ipaddr_str) )
        {
          return 0;
        }

        /* Check to see if a new DSM item is required */
        CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - \
                                     DS_PROFILE_3GPP_MAX_APN_STRING_LEN) );
                                      
        res_buff_ptr->used = (uint16)snprintf(
                  (char *)res_buff_ptr->data_ptr, 
                  res_buff_ptr->size,
                  "%s%s%s%d,\"%s\",\"%s\",\"%s\",%d,%d,%d,%d",
                  res_buff_ptr->data_ptr,
                  (cntr > 0) ? "\n" : "",
                  name_buff,
                  pdata.pdp_context_number,
                  dsat_pdptype_valstr[pdata.pdp_type],
                  pdata.apn,
                  ipaddr_str,
                  pdata.d_comp,
                  pdata.h_comp,
                  pdata.ipv4_addr_alloc,
                  ext_info.emergency_flag
                 );

        cntr++;
      }
      
      /* No contexts defined, just print command name */
      if (0 == cntr)
      {
        res_buff_ptr->used = (uint16)snprintf(
                                         (char *)res_buff_ptr->data_ptr,
                                         res_buff_ptr->size, "%s", name_buff);
      }
      result = res_buff_ptr->used;
      break;

    case MULTI_INDEX_AMPV:
      size = res_buff_ptr->used;
      res_buff_ptr->used = (word)snprintf(
                                   (char *)res_buff_ptr->data_ptr,
                                   res_buff_ptr->size,
                                   "%s%s",
                                   res_buff_ptr->data_ptr,
                                   name_buff);

      if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                                 sizeof(profile_list), &total_profiles,
                                                 dsat_get_current_ds_profile_subs_id()) )
      {
        DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
      }
        
      for (i = 0; i < total_profiles; i++) 
        {
        dsat_ip_addr_type addr;
#ifdef FEATURE_SECONDARY_PDP
        /* Skip further processing if secondary profile */
        if ((dsatetsipkt_get_pdp_profile_secondary_flag(profile_list[i],&pdata.secondary_flag) 
                 == DS_PROFILE_REG_RESULT_SUCCESS) && (TRUE == pdata.secondary_flag))
        {
          continue;
        }
#endif /* FEATURE_SECONDARY_PDP */

        memset((void*)&pdata, 0, sizeof(pdata));
        memset((void*)&ext_info, 0, sizeof(ext_info));
        memset((void*)ipaddr_str, 0, sizeof(ipaddr_str));
        if(dsatetsipkt_read_write_param_table(context_param_tbl,sizeof(context_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
                                 profile_list[i],READ_COMMAND,(byte*)&pdata,sizeof(pdata),dsat_get_current_ds_profile_subs_id()) == DSAT_ERROR)
        {
          DS_AT_MSG0_ERROR("CGDCONT Read Failed");
          result = (res_buff_ptr->used - size);
          res_buff_ptr->used -= (word)result;  /* caller increments used */
          return (dsat_num_item_type)result;
        }

        if(dsatetsipkt_read_write_param_table(dsat_cgdcont_ext_table,sizeof(dsat_cgdcont_ext_table)/sizeof(atcop_profile_3gpp_param_list_type),
                                 profile_list[i],READ_COMMAND,(byte*)&ext_info,sizeof(ext_info),dsat_get_current_ds_profile_subs_id()) == DSAT_ERROR)
        {
          DS_AT_MSG0_ERROR("CGDCONT Ext Read Failed");
          result = (res_buff_ptr->used - size);
          res_buff_ptr->used -= (word)result;  /* caller increments used */
          return (dsat_num_item_type)result;
        }
          
        if(pdata.pdp_type == DS_PROFILE_3GPP_PDP_IP)
        {
          addr.ip_vsn = DS_PROFILE_3GPP_IP_V4;
        }
        else if(pdata.pdp_type == DS_PROFILE_3GPP_PDP_IPV6)
        {
          addr.ip_vsn = DS_PROFILE_3GPP_IP_V6;
        }
        else if(pdata.pdp_type == DS_PROFILE_3GPP_PDP_IPV4V6)
        {
          if(pdata.pdp_addr.ds_profile_3gpp_pdp_addr_ipv4 != 0)
          {
            addr.ip_vsn = DS_PROFILE_3GPP_IP_V4;
          }
          else
          {
            addr.ip_vsn = DS_PROFILE_3GPP_IP_V6;
          }
        }
        else
        {
          DS_AT_MSG0_ERROR("PDP Type is invalid for ATCoP");
          return DSAT_ERROR;
        }
        addr.addr = pdata.pdp_addr;
          /* Decode PDP IP address if set */
        if ( DSAT_OK != dsatutil_convert_ipaddr(INT_TO_STRING,
                                                  &addr,
                                        ipaddr_str) )
        {
            result = (res_buff_ptr->used - size);
            res_buff_ptr->used -= (word)result;  /* caller increments used */
            return (dsat_num_item_type)result;
        }
          /* Check to see if a new DSM item is required */
        CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - \
                                          DS_PROFILE_3GPP_MAX_APN_STRING_LEN) );
                                        
          /* Generate response from internal storage */
          res_buff_ptr->used = (word)snprintf (
                    (char *)res_buff_ptr->data_ptr, 
                    res_buff_ptr->size,
                    "%s%s(%d,\"%s\",\"%s\",\"%s\",%d,%d,%d,%d)",
                    res_buff_ptr->data_ptr,
                    (cntr > 0) ? "," : "",
                    pdata.pdp_context_number,
                    dsat_pdptype_valstr[pdata.pdp_type],
                    pdata.apn,
                    ipaddr_str,
                    pdata.d_comp,
                    pdata.h_comp,
                    pdata.ipv4_addr_alloc,
                    ext_info.emergency_flag
                   );
          cntr++;
      }

      /* Return zero if DSM item changed */
      result = (res_buff_ptr == *out_buff_ptr)?
               (res_buff_ptr->used - size) : 0;
      (*out_buff_ptr)->used -= (word)result;  /* caller increments used */
      break;
      
    default:
      DS_AT_MSG0_HIGH("Unsupported multi-index format type");
  }
  return (dsat_num_item_type)result;
}  /* dsatetsipkt_cgdcont_response_fmt */

#ifdef FEATURE_SECONDARY_PDP

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGDSCONT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGDSCONT command.
  This command sets PDP secondary context parameters.

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
dsat_result_enum_type dsatetsipkt_exec_cgdscont_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type val = 0;
  dsat_mixed_param_val_type * val_ptr = 
    (dsat_mixed_param_val_type *) dsatutil_get_val(parse_table->cmd_id,0,0,STR_TYPE);
  dsat_num_item_type context_id = 0;
  byte cid = 0;
  ds_profile_status_etype profile_status;
  ds_profile_config_type    config_ptr;
  dsat_num_item_type num_val = 0;

  dsat_error_info_s_type    err_info;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if (tok_ptr->op == (NA|EQ|AR))
  {
    /*---------------------------------------------------------------
      Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/
    ds_profile_3gpp_pdp_context_type pdata;
    boolean reset_linked_profiles = FALSE;

        /* Query context profile data for cid */
    memset((void*)&pdata, 0, sizeof(pdata));
    memset( &config_ptr, 0, sizeof(ds_profile_config_type) );
    
    /* Process connection ID */
    if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 0, &val,1))
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 0;
      goto send_error;
    }
    
    /* Test for only CID specified */
    if (1 == tok_ptr->args_found)
    {
      if(dsatetsipkt_is_profile_context_valid( val, DS_PROFILE_TECH_3GPP,
                                               dsat_get_current_ds_profile_subs_id() ) != TRUE)

      {
        DS_AT_MSG1_HIGH("Profile doesn't exist %d",val);
        return DSAT_OK;
      }else
      {
        /* Check profile is SEC profile */
        if(dsatetsipkt_get_pdp_profile_secondary_flag(val,&pdata.secondary_flag)
                != DS_PROFILE_REG_RESULT_SUCCESS)
        {
          DS_AT_MSG1_HIGH("Error CGDSCONT PDP profile read: %d",val);
          pdata.secondary_flag = FALSE;
        }
        /* Ensure cid to reset is secondary context */
        if (FALSE == pdata.secondary_flag)
        {
          DS_AT_MSG1_HIGH("Cannot reset PRIMARY context with CGDSCONT: %d",val);
          return DSAT_ERROR;
        }
        /* Clear the user specified +CGDSCONT context parameters */
        RESET_BASE_CONTEXT_PARAMS(val);
        return result;
      }
    }
    else
    {
      cid = (byte)val;
      num_val = 0;

      if(dsatetsipkt_is_profile_context_valid( cid, DS_PROFILE_TECH_3GPP,
                                               dsat_get_current_ds_profile_subs_id() ) != TRUE)

      {
          /* profile is not present create it.*/
        config_ptr.config_mask = DS_PROFILE_CONFIG_MASK_PROFILE_ID |
                                 DS_PROFILE_CONFIG_MASK_SUBS_ID |
                                 DS_PROFILE_CONFIG_MASK_PERSISTENCE;
        config_ptr.num = cid;
        config_ptr.is_persistent = TRUE;
        config_ptr.subs_id = dsat_get_current_ds_profile_subs_id();
        profile_status = ds_profile_create_ex(DS_PROFILE_TECH_3GPP,&config_ptr,(uint16 *)&context_id );
        if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
        {
          DS_AT_MSG1_HIGH("Profile creation failed : %d",profile_status);
          return DSAT_ERROR;
        }
        else
        {
          cid = context_id;
        }
        /*Created a New profile; No need to reset linked profile*/
        pdata.secondary_flag = TRUE;
      }
      else
      {
        /*Check Present profile is secondary profile */
        if(dsatetsipkt_get_pdp_profile_secondary_flag(cid,&pdata.secondary_flag)
                != DS_PROFILE_REG_RESULT_SUCCESS)
        {
          DS_AT_MSG1_HIGH("Error CGDSCONT PDP profile read: %d",cid);
          pdata.secondary_flag = TRUE;
        }
      }

      /* Check for overwriting a primary profile. */
      /* We will reset any linked secondaries later. */
      
      reset_linked_profiles = (FALSE == pdata.secondary_flag)? TRUE : FALSE;

      /* Verify primary context is valid */
      if (VALID_TOKEN(1))
      {
        /* Process primary connection ID */
        if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 1, &val,1))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 1;
          goto send_error;
        }
        
        /* Query PRIMARY context profile data */
        memset((void*)&pdata, 0, sizeof(pdata));
        if(dsatetsipkt_is_profile_context_valid( val, DS_PROFILE_TECH_3GPP,
                                                 dsat_get_current_ds_profile_subs_id() ) != TRUE)

        {
          return DSAT_ERROR;
        }
        if(dsatetsipkt_get_pdp_profile_secondary_flag(val,&pdata.secondary_flag)
               != DS_PROFILE_REG_RESULT_SUCCESS)
        {
            DS_AT_MSG1_HIGH("Error CGDSCONT PDP profile read: %d",val);
            return DSAT_ERROR;
        }
        /* Ensure primary context defined */
        if (TRUE == pdata.secondary_flag)
        {
          DS_AT_MSG1_HIGH("Primary context ID is undefined: %d",val);
          return DSAT_ERROR;
        }
        /* Prohibit circular references */
        if (cid == val)
        {
          DS_AT_MSG1_HIGH("Primary and Secondary CID same: %d",cid);
          return DSAT_ERROR;
        }
      }
      else
      {
        return DSAT_ERROR;
      }

      /* Perform normal parameter processing */
      if (DSAT_OK == dsatparm_exec_param_cmd(
                                             mode,
                                             parse_table,
                                             tok_ptr,
                                             res_buff_ptr
                                            ))
      {
        /* Reset linked profiles if detectd earlier */
        if (TRUE == reset_linked_profiles)
        {
          /* Reset any linked secondary profiles to given cid if profile
           * type changes from primary to secondary.  We do this to ensure
           * a secondary does not link to another secondary.  */
          if (FALSE == etsipkt_reset_linked_context(cid))
          {
            DS_AT_MSG1_HIGH("Error resetting linked context: %d",cid);
            return DSAT_ERROR;
          }
        }
        /*Reset whole profile before write*/
        if(ds_profile_reset_pdp_context_per_sub(DS_PROFILE_TECH_3GPP,cid,dsat_get_current_ds_profile_subs_id()) != DS_PROFILE_REG_RESULT_SUCCESS)
        {
            DS_AT_MSG1_ERROR("Error CGDSCONT PDP profile reset: %d",cid);
            return DSAT_ERROR;
        }
        /* Set specified profile to IPV4 defaults */
        /* Note: we are intentionally permitting overwrite of either a */
        /*       primary or secondary profile here.                   */
        
        pdata.pdp_context_number = cid;
        pdata.secondary_flag     = TRUE;
        CONDITIONAL_ASSIGN(1, pdata.primary_id,
                              (byte)val_ptr[1].num_item);
        if( VALID_TOKEN(2) )
        {
          if( ATOI_OK == dsatutil_atoi(&num_val, 
            &dsat_cgdcont_dcomp_valstr[val_ptr[2].num_item][0], 10) )
          {
            pdata.d_comp = (ds_profile_3gpp_pdp_data_comp_e_type)num_val;
          }
          else
          {
            return DSAT_ERROR;
          }
        }
        if( VALID_TOKEN(3) )
        {
          if( ATOI_OK == dsatutil_atoi(&num_val, 
              &dsat_cgdcont_hcomp_valstr[val_ptr[3].num_item][0], 10))
          { 
            pdata.h_comp =(ds_profile_3gpp_pdp_header_comp_e_type)num_val;
          }
          else
          {
            return DSAT_ERROR;
          }
        }
        /* Write to EFS */
        if(dsatetsipkt_read_write_param_table(context_param_tbl,sizeof(context_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
                                 cid,WRITE_COMMAND,(byte*)&pdata,sizeof(pdata),dsat_get_current_ds_profile_subs_id()) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error CGDSCONT PDP profile write: %d",cid);
          return DSAT_ERROR;
        }
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /*---------------------------------------------------------------
      Processing for a command to query valid parameters
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgdscont_response_fmt (parse_table,
                                                &res_buff_ptr,
                                                MULTI_INDEX_TEST))
    {
      DS_AT_MSG0_HIGH("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|QU))
  {
    /*---------------------------------------------------------------
       Processing for a command to query current parameters
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgdscont_response_fmt (parse_table,
                                                &res_buff_ptr,
                                                MULTI_INDEX_READ))
    {
      DS_AT_MSG0_HIGH("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA))
  {
    /* Do nothing */
    result = DSAT_OK;
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;

send_error:
  DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  return DSAT_ERROR;

} /* dsatetsipkt_exec_cgdscont_cmd */



/*===========================================================================

FUNCTION DSATETSIPKT_CGDSCONT_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGDSCONT read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsipkt_cgdscont_response_fmt
(
  const dsati_cmd_type  *parse_table,   /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format         /*  Flag for format type        */
)
{
  #define MAX_BUFFER_SIZE (4*DSAT_MAX_ALLOWED_PROFILES) 
  /* MAX size should be thrice the size of DS_UMTS_MAX_PDP_PROFILE_NUM 
   to store profile number (in ascii) followed by a coma. It requires 
   4 bytes for profile numbers greater than or equal to 100.*/
  byte temp_buf[MAX_BUFFER_SIZE];
  byte name_buff[MAX_BUFFER_SIZE];
  byte i, cntr = 0;
  int size = 0;
  int result = 0;
  dsm_item_type * res_buff_ptr = *out_buff_ptr;
  ds_profile_3gpp_pdp_context_type pdata;

  uint8             profile_list[DSAT_MAX_ALLOWED_PROFILES] = {0};
  uint8             total_profiles = 0;

  /* Format the command name */
  (void)snprintf((char*)name_buff, 
                        sizeof(name_buff),
                        "%s: ", parse_table->name);
  res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';

  switch (format)
  {
    case MULTI_INDEX_TEST:
      /* Get list of defines primary profiles */
      if (FALSE == etsipkt_get_primary_profile_list (temp_buf, sizeof(temp_buf)))
      {
        DS_AT_MSG0_HIGH("Error on get PDP primary profile list");
        return 0;
      }
      
      /* Report each PDP type on seperate line */

      /*Range we support for 3GPP only profiles is: 1-16 
        Range we support for EPC profiles is:       100-179 */
      for (i=0; i <= (byte)DSAT_MAX_PDP_TYPE; i++) 
      {
        size = snprintf ((char *)res_buff_ptr->data_ptr,
                        res_buff_ptr->size,
                        "%s%s%s(%d-%d,%d-%d),(%s),\"%s\",%s,%s",
                        res_buff_ptr->data_ptr,
                        (i > 0) ? "\n" : "",
                        name_buff,
                        DSAT_LEGACY_PROFILE_NUM_MIN,
                        DSAT_LEGACY_PROFILE_NUM_MAX,
                        DSAT_EXT_PROFILE_NUM_MIN,
                        DSAT_EXT_PROFILE_NUM_MAX,
                        temp_buf,
                        dsat_pdptype_valstr[i],
                        dsat_cgdcont_dcomp_tststr,
                        dsat_cgdcont_hcomp_tststr
                       );
      }
      res_buff_ptr->used = (uint16)size;
      result = size;
      break;

    case MULTI_INDEX_READ:
      /* Report each profile on seperate line */
      if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                                 sizeof(profile_list), &total_profiles,
                                                 dsat_get_current_ds_profile_subs_id()) )
      {
        DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
      }
      for ( i = 0; i < total_profiles; i++) 
      {
        if(dsatetsipkt_get_pdp_profile_secondary_flag(profile_list[i],&pdata.secondary_flag)
               != DS_PROFILE_REG_RESULT_SUCCESS)
          {
          DS_AT_MSG1_HIGH("Error CGDSCONT PDP profile read: %d",profile_list[i]);
          return DSAT_ERROR;
          }
          /* Skip further processing if primary profile */
          if (FALSE == pdata.secondary_flag)
          {
            continue;
          }
          
        memset((void*)&pdata, 0, sizeof(pdata));
        /* Query profile data */
        if(dsatetsipkt_read_write_param_table(context_param_tbl,sizeof(context_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
                                 profile_list[i],READ_COMMAND,(byte*)&pdata,sizeof(pdata),dsat_get_current_ds_profile_subs_id()) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error CGDSCONT PDP profile read: %d",profile_list[i]);
          return DSAT_ERROR;
        }
          size = snprintf ((char *)res_buff_ptr->data_ptr, 
                    res_buff_ptr->size,
                    "%s%s%s%d,%d,%d,%d",
                    res_buff_ptr->data_ptr,
                    (cntr > 0) ? "\n" : "",
                    name_buff,
                    pdata.pdp_context_number,
                    pdata.primary_id,
                    pdata.d_comp,
                    pdata.h_comp
                   );
          cntr++;
        }
      
      /* No contexts defined, just print command name */
      if (0 == cntr)
      {
        size = snprintf((char *)res_buff_ptr->data_ptr,
                              res_buff_ptr->size, "%s", name_buff);
      }
      res_buff_ptr->used = (uint16)size;
      result = size;
      break;

    case MULTI_INDEX_AMPV:
      size = res_buff_ptr->used;
      res_buff_ptr->used = (word)snprintf(
                                   (char *)res_buff_ptr->data_ptr,
                                   res_buff_ptr->size,
                                   "%s%s",
                                   res_buff_ptr->data_ptr,
                                   name_buff);

      if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                                 sizeof(profile_list), &total_profiles,
                                                 dsat_get_current_ds_profile_subs_id()) )
        {
          DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
        }
      for ( i = 0; i < total_profiles; i++) 
      {
        if(dsatetsipkt_get_pdp_profile_secondary_flag(profile_list[i],&pdata.secondary_flag)
               != DS_PROFILE_REG_RESULT_SUCCESS)
          {
            DS_AT_MSG1_HIGH("Error CGDSCONT PDP profile read: %d",profile_list[i]);
            result = (res_buff_ptr->used - size);
            res_buff_ptr->used -= (word)result;  /* caller increments used */
            return (dsat_num_item_type)result;
          }
          /* Skip further processing if primary profile */
          if (FALSE == pdata.secondary_flag)
          {
            continue;
          }
        memset((void*)&pdata, 0, sizeof(pdata));
        if(dsatetsipkt_read_write_param_table(context_param_tbl,sizeof(context_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
                                 profile_list[i],READ_COMMAND,(byte*)&pdata,sizeof(pdata),dsat_get_current_ds_profile_subs_id()) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error CGDSCONT PDP profile read: %d",profile_list[i]);
                    result = (res_buff_ptr->used - size);
          res_buff_ptr->used -= (word)result;  /* caller increments used */
          return (dsat_num_item_type)result;
        }

          /* Check to see if a new DSM item is required */
          CHECK_APPEND_RESPONSE_BUFFER (0);
          
          /* Generate response from internal storage */
          res_buff_ptr->used = (word)snprintf (
                    (char *)res_buff_ptr->data_ptr, 
                    res_buff_ptr->size,
                    "%s%s(%d,%d,%d,%d)",
                    res_buff_ptr->data_ptr,
                    (cntr > 0) ? "," : "",
                    pdata.pdp_context_number,
                    pdata.primary_id,
                    pdata.d_comp,
                    pdata.h_comp
                   );
          cntr++;
        }

      /* Return zero if DSM item changed */
      result = (res_buff_ptr == *out_buff_ptr)?
               (res_buff_ptr->used - size) : 0;
      (*out_buff_ptr)->used -= (word)result;  /* caller increments used */
      break;
      
    default:
      DS_AT_MSG0_HIGH("Unsupported multi-index format type");
  }
  return (dsat_num_item_type)result;
}  /* dsatetsipkt_cgdcont_response_fmt */



/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGTFT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGTFT and +QCGTFT command.
  This command sets PDP Traffic Flow Template parameters.

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
/* ARGSUSED */
dsat_result_enum_type dsatetsipkt_exec_cgtft_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type val = 0;
  ds_profile_status_etype  profile_status;
  dsat_error_info_s_type    err_info;
  ds_profile_tech_etype             tech_type = DS_PROFILE_TECH_INVALID;
  byte cid = 0;
  byte temp_buf[MAX_ADDR_SUBNET_STRING_LEN+1];
  uint32 temp_val=0;
  ds_profile_3gpp_tft_params_type pdata;
  byte filter_index = 0;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if (tok_ptr->op == (NA|EQ|AR))
  {
    /*---------------------------------------------------------------
      Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/

    /* Process connection ID */
    if (DSAT_OK !=
        etsipkt_process_param(parse_table, tok_ptr, (uint32)CGTFT_CID, &val,1))
    {
      
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = (uint8) CGTFT_CID;
      goto send_error;
    }
    cid       = (byte)val;

    if(0 == (dsatutil_strncmp_ig_sp_case((const byte *)parse_table->name,
                                         (const byte *)"+QCGTFT" ,strlen((const char *)"+QCGTFT"))))
    {
      //EPC check range over here
      tech_type = DS_PROFILE_TECH_EPC;
      if ((cid < 100) ||
          (TRUE != dsatetsipkt_is_profile_context_valid(cid,
                                                        tech_type,
                                                        dsat_get_current_ds_profile_subs_id())))
      {
        DS_AT_MSG2_ERROR("Error Incorrect cid:%d, tech_type:%d pair",cid, tech_type);
        return DSAT_ERROR;
      }
    }
    else 
    {
      // 3GPP profiles check for +CGTFT
      tech_type = DS_PROFILE_TECH_3GPP;
      if (TRUE != dsatetsipkt_is_profile_context_valid(cid,
                                                       tech_type,
                                                       dsat_get_current_ds_profile_subs_id()))
      {
        DS_AT_MSG2_ERROR("Error Incorrect cid:%d, tech_type:%d pair",cid, tech_type);
        return DSAT_ERROR;
      }
    }

    /* Test for only CID specified */
    if (1 == tok_ptr->args_found)
    {
      /* Clear the user specified +CGTFT context parameters */
      ETSIPKT_RESET_INVALID_PROFILE_PARAM(val,DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID1,tech_type);
      if ( DS_PROFILE_REG_RESULT_SUCCESS != profile_status )
      {
        return DSAT_ERROR;
      }
      ETSIPKT_RESET_INVALID_PROFILE_PARAM(val,DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID2,tech_type);
      if ( DS_PROFILE_REG_RESULT_SUCCESS != profile_status )
      {
        return DSAT_ERROR;
      }
    }
    else
    {
      #define PFI_INVALID_INDEX 3
      cid = (byte)val;
      filter_index = PFI_INVALID_INDEX;
      memset ((void*)&pdata, 0, sizeof(ds_profile_3gpp_tft_params_type));

      /* Given the command has no default paramater values, all validation */
      /* must be done explictly to avoid previous value handling in normal */
      /* parameter processing.  Need to detect skipped parameters to       */
      /* determine valid combinations.                                     */
      
      /* Validate packet filter identifier.                                */
      /* Note the PFI will be unique as we use it for storage position,    */
      /* overwriting any of same identifier.                               */
      if ( VALID_TOKEN((uint32)CGTFT_PFI) )
      {
        if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, (uint32)CGTFT_PFI,
                                             (byte*)&temp_val,sizeof(temp_val)))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = (uint8) CGTFT_PFI;
          goto send_error;
        }
        /*
         * Check if this packet Filter ID exist then overwrite it
         * If both filter ID filled and New Filter ID doesn't match to set 
         * Filter ids then return ERROR.
        */
        filter_index =  etsipkt_cal_index_frm_pfi (cid,(byte)temp_val);
        DS_AT_MSG1_HIGH(" Filter index %d ",filter_index);
        if (PFI_INVALID_INDEX == filter_index)
        {
          DS_AT_MSG0_ERROR(" MAXIMUM two filter can set ");
          return DSAT_ERROR;
        }
        pdata.filter_id = (byte)temp_val;
      }

      /* Validate evaluation precedence index (EPI) */
      if ( VALID_TOKEN((uint32)CGTFT_EPI) )
      {
        if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, (uint32)CGTFT_EPI,
                                             (byte*)&temp_val,sizeof(temp_val)))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = (uint8) CGTFT_EPI;
          goto send_error;
        }
        else
        {
          /* Search for duplicate EPI in related TFTs */
          if (TRUE == etsipkt_validate_tft_epi_param (cid, filter_index,
                                                      (byte)temp_val))
          {
            pdata.eval_prec_id = (byte)temp_val; 
          }
          else
          {
            DS_AT_MSG1_HIGH("EPI is not unique for all profiles: %d",temp_val);
            dsatme_set_cme_error (DSAT_CME_TFT_EPI_DUPLICATE, res_buff_ptr);
            return DSAT_CMD_ERR_RSP;
          }
        }
      }

      /* Ensure both PFI and EPI are provided */
      if ( !(VALID_TOKEN((uint32)CGTFT_PFI) && VALID_TOKEN((uint32)CGTFT_EPI)) )
      {
        DS_AT_MSG2_HIGH("Need both PFI and EPI parameters: PFI=%d, EPI=%d",
                  pdata.filter_id,pdata.eval_prec_id);
        return DSAT_ERROR;
      }
      
      /* Validate source address and subnet mask */
      if ( VALID_TOKEN((uint32)CGTFT_SA_SM) )
      {
        temp_buf[0] = '\0';
        if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, (uint32)CGTFT_SA_SM,
                                             temp_buf,sizeof(temp_buf)))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = (uint8) CGTFT_SA_SM;
          goto send_error;
        }
        else
        {
          if ( DSAT_OK != dsatutil_convert_addr_mask( STRING_TO_INT,
                                                      &pdata.src_addr,
                                                      &pdata.ip_version,
                                                      temp_buf ) )
          {
            DS_AT_MSG0_HIGH("Error validating Address Mask parameter");
            return DSAT_ERROR;
          }
        }
      }
      /* Validate protocol number / next header */
      if ( VALID_TOKEN((uint32)CGTFT_PN_NH) )
      {
        if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, (uint32)CGTFT_PN_NH,
                                             (byte*)&temp_val,sizeof(temp_val)))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = (uint8) CGTFT_PN_NH;
          goto send_error;
        }
        pdata.prot_num = (byte)temp_val;
      }
      
      /* Validate destination port range */
      if ( VALID_TOKEN((uint32)CGTFT_DSTP) )
      {
        temp_buf[0] = '\0';
        if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, (uint32)CGTFT_DSTP,
                                             temp_buf,sizeof(temp_buf)))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = (uint8) CGTFT_DSTP;
          goto send_error;
        }
        else
        {
          uint16 from,to;

          /* Convert & validate string tuple */
          if (FALSE ==
              etsipkt_validate_port_param(temp_buf,
                                          sizeof(temp_buf),
                                          (uint16*)&from,
                                          (uint16*)&to))
          {
            err_info.errval = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
            err_info.arg_num = (uint8) CGTFT_DSTP;
            goto send_error;
          }
          pdata.dest_port_range.from = from;
          pdata.dest_port_range.to = to;
        }
      }
      
      /* Validate source port range */
      if ( VALID_TOKEN((uint32)CGTFT_SRCP) )
      {
        temp_buf[0] = '\0';
        if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, (uint32)CGTFT_SRCP,
                                             temp_buf,sizeof(temp_buf))) 
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = (uint8) CGTFT_SRCP;
          goto send_error;
        }
        else
        {
          uint16 from,to;

          /* Convert & validate string tuple */
          if (FALSE ==
              etsipkt_validate_port_param(temp_buf,
                                          sizeof(temp_buf),
                                          (uint16*)&from,
                                          (uint16*)&to))
          {
            err_info.errval = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
            err_info.arg_num = (uint8) CGTFT_SRCP;
            goto send_error;
          }
          pdata.src_port_range.from = from;
          pdata.src_port_range.to = to;
        }
      }

      /* Validate security parameter index*/
      if ( VALID_TOKEN((uint32)CGTFT_SPI) )
      {
        if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, (uint32)CGTFT_SPI,
                                             (uint32*)&temp_val,sizeof(temp_val)))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = (uint8) CGTFT_SPI;
          goto send_error;
        }
        pdata.ipsec_spi = temp_val;
      }

      /* Validate TOS / traffic class and mask */
      if ( VALID_TOKEN((uint32)CGTFT_TOS) )
      {
        temp_buf[0] = '\0';
        if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, (uint32)CGTFT_TOS,
                                             temp_buf,sizeof(temp_buf)))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = (uint8) CGTFT_TOS;
          goto send_error;
        }
        else
        {
          /* Convert & validate string tuple */
          uint32 tmask;
          if (DSAT_OK != dsatutil_convert_tuple (STRING_TO_INT,
                                                 TUPLE_TYPE_TMASK,
                                                 &tmask,
                                                 temp_buf,
                                                 sizeof(temp_buf),
                                                 DSAT_CONVERT_IP) )
          {
            err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
            err_info.arg_num = (uint8) CGTFT_TOS;
            goto send_error;
          }
          pdata.tos_mask = (uint16)tmask;
        }
      }

      /* Validate flow label */
      if ( VALID_TOKEN((uint32)CGTFT_FLOW) )
      {
        if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, (uint32)CGTFT_FLOW,
                                             (uint32*)&temp_val,sizeof(temp_val)))
        {
          DS_AT_MSG0_HIGH("Cannot process FLOW parameter");
          return DSAT_ERROR;
        }
        pdata.flow_label = temp_val;
      }
      
      /* Validate combination of parameters */
      if (!etsipkt_validate_cgtft_param_combination(tok_ptr) )
      {
        DS_AT_MSG0_HIGH("Paramater combination invalid");
        dsatme_set_cme_error (DSAT_CME_INVALID_PARAM_COMB, res_buff_ptr);
        return DSAT_CMD_ERR_RSP;
      }
      else
      {
        /* Write to EFS */
        filter_id_e_type filter_id_param =
                (filter_id_e_type) DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID1;
        if(dsatetsipkt_read_write_profile_param(cid,WRITE_COMMAND,
                   (filter_id_param+filter_index-1),
                   &pdata,sizeof(ds_profile_3gpp_tft_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_HIGH("Error CGTFT PDP profile write: %d",cid);
          return DSAT_ERROR;
        }
      }
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /*---------------------------------------------------------------
      Processing for a command to query valid parameters
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgtft_response_fmt (parse_table,
                                             &res_buff_ptr,
                                             MULTI_INDEX_TEST))
    {
      DS_AT_MSG0_HIGH("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|QU))
  {
    /*---------------------------------------------------------------
       Processing for a command to query current parameters
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgtft_response_fmt (parse_table,
                                             &res_buff_ptr,
                                             MULTI_INDEX_READ))
    {
      DS_AT_MSG0_HIGH("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA))
  {
    /* Do nothing */
    result = DSAT_OK;
  }
  else
  {
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }
  return result;

  
send_error:
  if( err_info.errval == DSAT_ERR_INVALID_NUM_OF_ARGS  || 
      err_info.errval == DSAT_ERR_INVALID_SYNTAX )
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;
} /* dsatetsipkt_exec_cgtft_cmd */



/*===========================================================================

FUNCTION DSATETSIPKT_CGTFT_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGTFT read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsipkt_cgtft_response_fmt
(
  const dsati_cmd_type  *parse_table,   /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format         /*  Flag for format type        */
)
{
  #define NET_PORT_RANGE "(0-65535.0-65535)"
  #define TMASK_RANGE "(0-255.0-255)"
  byte name_buff[16];
  byte i,j,cntr = 0;
  int size = 0;
  int result = 0;
  dsm_item_type * res_buff_ptr = *out_buff_ptr;
  ds_profile_3gpp_tft_params_type pdata;
  uint8             profile_list[DSAT_MAX_ALLOWED_PROFILES];
  uint8             total_profiles = 0;

  /* Format the command name */
  (void)snprintf((char*)name_buff, 
                        sizeof(name_buff),
                        "%s: ", parse_table->name);
  res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';

  switch (format)
  {
    case MULTI_INDEX_TEST:
      /* Report each PDP type on seperate line */
      for (i=0; i <= (byte)DSAT_MAX_PDP_TYPE; i++) 
      {
        size = snprintf ((char *)res_buff_ptr->data_ptr, 
                        res_buff_ptr->size,
                        "%s%s%s\"%s\",(%d-%d),(%d-%d),%s,(%d-%d),"
                        "%s,%s,(%X-%X),%s,(%X-%X)",
                        res_buff_ptr->data_ptr,
                        (i > 0) ? "\n" : "",
                        name_buff,
                        dsat_pdptype_valstr[i],
                        ((dflm_type *)dsat_cgtft_pfid_dflm)->lower,
                        ((dflm_type *)dsat_cgtft_pfid_dflm)->upper,
                        ((dflm_type *)dsat_cgtft_prec_dflm)->lower,
                        ((dflm_type *)dsat_cgtft_prec_dflm)->upper,
                        "",
                        ((dflm_type *)dsat_cgtft_pn_nh_dflm)->lower,
                        ((dflm_type *)dsat_cgtft_pn_nh_dflm)->upper,
                        NET_PORT_RANGE,
                        NET_PORT_RANGE,
                        ((dflm_type *)dsat_cgtft_spi_dflm)->lower,
                        ((dflm_type *)dsat_cgtft_spi_dflm)->upper,
                        TMASK_RANGE,
                        ((dflm_type *)dsat_cgtft_flow_dflm)->lower,
                        ((dflm_type *)dsat_cgtft_flow_dflm)->upper
                       );
      }
      res_buff_ptr->used = (uint16)size;
      result = size;
      break;

    case MULTI_INDEX_READ:

      if(0 == (dsatutil_strncmp_ig_sp_case((const byte *)parse_table->name,
                                           (const byte *)"+QCGTFT" ,strlen((const char *)"+QCGTFT"))))
      {
        if (DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_EPC, &profile_list[0],
                                                    sizeof(profile_list), &total_profiles,
                                                    dsat_get_current_ds_profile_subs_id()) )
        {
          DS_AT_MSG0_ERROR("Error getting EPC profile list!");
        }
      }
      else
      {
        // Means +CGTFT command
        if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                                   sizeof(profile_list), &total_profiles,
                                                   dsat_get_current_ds_profile_subs_id()) )
        {
          DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
        }
      }

      /* Report each profile on seperate line */
      for (i=0; i < total_profiles; i++) 
      {
        /* Loop over all templates */
        for (j=0; j < DS_PROFILE_3GPP_MAX_TFT_PARAM_SETS; j++)
        {
          memset((void*)&pdata, 0, sizeof(pdata));
          if(dsatetsipkt_read_write_profile_param(profile_list[i],READ_COMMAND,
                     DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID1 + j,
                     &pdata,sizeof(ds_profile_3gpp_tft_params_type)) == DSAT_ERROR)
          {
            DS_AT_MSG1_ERROR("Error get param CID : %d ",profile_list[i]);
            continue;
          }
          else
          {
            uint8 tmask = (pdata.tos_mask & 0xFF);
            byte addr_mask_buf[MAX_ADDR_SUBNET_STRING_LEN+1];

            memset(addr_mask_buf, 0, sizeof(addr_mask_buf));

            if ( DSAT_OK != dsatutil_convert_addr_mask( INT_TO_STRING,
                                                        &pdata.src_addr,
                                                        &pdata.ip_version,
                                                        addr_mask_buf ) )
            {
              DS_AT_MSG2_HIGH("Error converting CGTFT address: %d,%d",profile_list[i],j);
              return 0;
            }
              
            /* Check to see if a new DSM item is required */
            CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - 32));
            
            size = res_buff_ptr->used = (word) 
                   snprintf ((char *)res_buff_ptr->data_ptr, 
                            res_buff_ptr->size,
                            "%s%s%s%d,%d,%d,\"%s\",%d,%d.%d,%d.%d,%X,%d.%d,%X",
                            res_buff_ptr->data_ptr,
                            (cntr > 0) ? "\n" : "",
                            name_buff,
                            profile_list[i],
                            pdata.filter_id,
                            pdata.eval_prec_id,
                            addr_mask_buf,
                            pdata.prot_num,
                            pdata.dest_port_range.from,
                            pdata.dest_port_range.to,
                            pdata.src_port_range.from,
                            pdata.src_port_range.to,
                            (int)pdata.ipsec_spi,
                            (pdata.tos_mask >> 8),
                            tmask,
                            (int)pdata.flow_label
                           );
            cntr++;
          }
        } /* for */
      } /* for */
      
      /* No contexts defined, just print command name */
      if (0 == cntr)
      {
        size = snprintf((char *)res_buff_ptr->data_ptr,
                              res_buff_ptr->size, "%s", name_buff);
      }
      res_buff_ptr->used = (uint16)size;
      result = size;
      break;

    case MULTI_INDEX_AMPV:
      size = res_buff_ptr->used;
      res_buff_ptr->used = (word)snprintf(
                                   (char *)res_buff_ptr->data_ptr,
                                   res_buff_ptr->size,
                                   "%s%s",
                                   res_buff_ptr->data_ptr,
                                   name_buff);
      if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                                 sizeof(profile_list), &total_profiles,
                                                 dsat_get_current_ds_profile_subs_id()) )
      {
        DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
      }

      /* Report each profile on seperate line */
      for ( i = 0; i < total_profiles; i++) 
      {
        /* Loop over all templates */
        for (j=0; j<DS_PROFILE_3GPP_MAX_TFT_PARAM_SETS; j++)
        {
          memset((void*)&pdata, 0, sizeof(pdata));
          if(dsatetsipkt_read_write_profile_param(profile_list[i],READ_COMMAND,
                     DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID1 + j,
                     &pdata,sizeof(ds_profile_3gpp_tft_params_type)) == DSAT_ERROR)
          {
            DS_AT_MSG1_ERROR("Error get param CID : %d ",profile_list[i]);
            continue;
          }
          else
          {
            uint8 tmask = (pdata.tos_mask & 0xFF);
            byte addr_mask_buf[MAX_ADDR_SUBNET_STRING_LEN+1];

            memset(addr_mask_buf, 0, sizeof(addr_mask_buf));

            if ( DSAT_OK != dsatutil_convert_addr_mask( INT_TO_STRING,
                                                        &pdata.src_addr,
                                                        &pdata.ip_version,
                                                        addr_mask_buf ) )
            {
              DS_AT_MSG2_HIGH("Error converting CGTFT address: %d,%d",profile_list[i],j);
              return 0;
            }
                            
            /* Check to see if a new DSM item is required */
            CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - 32));
            
            /* Generate response from internal storage */
            res_buff_ptr->used =
              (word)snprintf (
                             (char *)res_buff_ptr->data_ptr, 
                             res_buff_ptr->size,
                             "%s%s(%d,%d,%d,\"%s\",%d,%d.%d,%d.%d,%X,%d.%d,%X)",
                             res_buff_ptr->data_ptr,
                             (cntr > 0) ? "," : "",
                             profile_list[i],
                             pdata.filter_id,
                             pdata.eval_prec_id,
                             addr_mask_buf,
                             pdata.prot_num,
                             pdata.dest_port_range.from,
                             pdata.dest_port_range.to,
                             pdata.src_port_range.from,
                             pdata.src_port_range.to,
                             (int)pdata.ipsec_spi,
                             (pdata.tos_mask >> 8),
                             tmask,
                             (int)pdata.flow_label
                             );
            cntr++;
          }
        } /* for */
      } /* for */

      /* Return zero if DSM item changed */
      result = (res_buff_ptr == *out_buff_ptr)?
               (res_buff_ptr->used - size) : 0;
      (*out_buff_ptr)->used -= (word)result;  /* caller increments used */
      break;
      
    default:
      DS_AT_MSG0_HIGH("Unsupported multi-index format type");
  }
  return (dsat_num_item_type)result;
}  /* dsatetsipkt_cgtft_response_fmt */
#endif /* FEATURE_SECONDARY_PDP */

#ifdef FEATURE_DATA_WCDMA_PS

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGEQREQ_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGEQREQ and +QCGEQREQ command.
  This command sets PDP context parameters for UMTS QoS Profile
  (Requested).

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  Defining +CGEQREQ parameters will undefine +CGQREQ/+CGQMIN parameters;
  these commands are mutually exclusive.

===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgeqreq_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type val = 0;
  dsat_mixed_param_val_type * val_ptr = 
    (dsat_mixed_param_val_type *) dsatutil_get_val(parse_table->cmd_id,0,0,STR_TYPE);
  uint32                           src_stat_desc_req = 0;
  byte cid = 0;
  ds_profile_status_etype  profile_status;
  dsat_error_info_s_type    err_info;
  ds_profile_tech_etype             tech_type = DS_PROFILE_TECH_INVALID;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if (tok_ptr->op == (NA|EQ|AR))
  {
    /*---------------------------------------------------------------
      Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/

    /* Validate connection ID */
    if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 0, &val,1))
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 0;
      goto send_error;
    }
    
    cid       = (byte)val;

    if(0 == (dsatutil_strncmp_ig_sp_case((const byte *)parse_table->name,
                                         (const byte *)"+QCGEQREQ" ,strlen((const char *)"+QCGEQREQ"))))
    {
      //EPC check range over here
      tech_type = DS_PROFILE_TECH_EPC;
      if ((cid < 100) ||
          (TRUE != dsatetsipkt_is_profile_context_valid(cid,
                                                        tech_type,
                                                        dsat_get_current_ds_profile_subs_id())))
      {
        DS_AT_MSG2_ERROR("Error Incorrect cid:%d, tech_type:%d pair",cid, tech_type);
        return DSAT_ERROR;
      }
    }
    else 
    {
      // 3GPP profiles check for +CGTFT
      tech_type = DS_PROFILE_TECH_3GPP;
      if (TRUE != dsatetsipkt_is_profile_context_valid(cid,
                                                       tech_type,
                                                       dsat_get_current_ds_profile_subs_id()))
      {
        DS_AT_MSG2_ERROR("Error Incorrect cid:%d, tech_type:%d pair",cid, tech_type);
        return DSAT_ERROR;
      }
    }
    
    /* Test for only CID specified */
    if (1 == tok_ptr->args_found)
    {
      /* Clear the user specified context parameters */
      ETSIPKT_RESET_INVALID_PROFILE_PARAM(val,DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_REQ_QOS,tech_type);
      if ( DS_PROFILE_REG_RESULT_SUCCESS != profile_status )
      {
        return DSAT_ERROR;
      }
      ETSIPKT_RESET_INVALID_PROFILE_PARAM(val,DS_PROFILE_3GPP_PROFILE_PARAM_SRC_STAT_DESC_REQ,tech_type);
      if ( DS_PROFILE_REG_RESULT_SUCCESS != profile_status )
      {
        return DSAT_ERROR;
      }
    }
    else
    {
      /* Perform normal parameter processing */
      if (DSAT_OK == dsatparm_exec_param_cmd(
                                             mode,
                                             parse_table,
                                             tok_ptr,
                                             res_buff_ptr
                                            ))
      {
        /* Transfer user data to internal storage */
        ds_profile_3gpp_3gpp_qos_params_type  pdata;
        /* Local variables for PACKED structure */
        ds_profile_3gpp_traffic_class_type      traffic_class;
        ds_profile_3gpp_qos_delivery_order_type dlvry_order;
        ds_profile_3gpp_sdu_err_ratio_type      sdu_err;
        ds_profile_3gpp_deliver_err_sdu_type    sdu_dlvr;
        ds_profile_3gpp_residual_ber_ratio_type res_biterr;

         /* Set specified profile to defaults in UMTS */
         ETSIPKT_RESET_DEFAULT_PROFILE_PARAM(cid,DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_REQ_QOS);
         if ( DS_PROFILE_REG_RESULT_SUCCESS != profile_status )
        {
          return DSAT_ERROR;
        }
         ETSIPKT_RESET_DEFAULT_PROFILE_PARAM(cid,DS_PROFILE_3GPP_PROFILE_PARAM_SRC_STAT_DESC_REQ);
         if ( DS_PROFILE_REG_RESULT_SUCCESS != profile_status )
        {
          return DSAT_ERROR;
        }
        
        /* Query default profile from EFS */
        memset((void*)&pdata, 0, sizeof(pdata));
        if(dsatetsipkt_read_write_profile_param(cid,READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_REQ_QOS,
               &pdata,sizeof(ds_profile_3gpp_3gpp_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param : %d",cid);
          return DSAT_ERROR;
        }
        if(dsatetsipkt_read_write_profile_param(cid,READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_SRC_STAT_DESC_REQ ,
                     &src_stat_desc_req,sizeof(uint32)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param : %d",cid);
          return DSAT_ERROR;
        }
        /* Make conditional edits to profile */
        CONDITIONAL_ASSIGN(2,  pdata.max_ul_bitrate, val_ptr[2].num_item);
        CONDITIONAL_ASSIGN(3,  pdata.max_dl_bitrate, val_ptr[3].num_item);
        CONDITIONAL_ASSIGN(4,  pdata.gtd_ul_bitrate, val_ptr[4].num_item);
        CONDITIONAL_ASSIGN(5,  pdata.gtd_dl_bitrate, val_ptr[5].num_item);
        CONDITIONAL_ASSIGN(7,  pdata.max_sdu_size,   val_ptr[7].num_item);
        CONDITIONAL_ASSIGN(11, pdata.trans_delay,    val_ptr[11].num_item);
        CONDITIONAL_ASSIGN(12, pdata.thandle_prio,   val_ptr[12].num_item);
        CONDITIONAL_ASSIGN(13, src_stat_desc_req,    val_ptr[13].num_item);
        CONDITIONAL_ASSIGN(14, pdata.sig_ind,        val_ptr[14].num_item);

        /* Map Traffic Class parameter to enum type */
        if (FALSE == etsipkt_map_tclass (TRUE, &val_ptr[1].num_item,
                                         &traffic_class))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 1;
          goto send_error;
        }
        else
        {
          CONDITIONAL_ASSIGN(1, pdata.traffic_class, traffic_class);
        }

        /* Map Delivery Order parameter to enum type */
        if (FALSE == etsipkt_map_dorder (TRUE, &val_ptr[6].num_item,
                                         &dlvry_order))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 6;
          goto send_error;
        }
        else
        {
          CONDITIONAL_ASSIGN(6, pdata.dlvry_order, dlvry_order);
        }
          
        /* Map SDU error parameter to enum type */
        if (FALSE == etsipkt_map_sdu_err(TRUE, &val_ptr[8].num_item,
                                         &sdu_err))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 8;
          goto send_error;
        }
        else
        {
          CONDITIONAL_ASSIGN(8, pdata.sdu_err, sdu_err);
        }
          
        /* Map Erroneous SDU Delivery parameter to enum type */
        if (FALSE == etsipkt_map_dlvr_err_sdu(TRUE, &val_ptr[10].num_item,
                                              &sdu_dlvr))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 10;
          goto send_error;
        }
        else
        {
          CONDITIONAL_ASSIGN(10, pdata.dlvr_err_sdu, sdu_dlvr);
        }

        /* Map Residual BER parameter to enum type */
        if (FALSE == etsipkt_map_res_ber(TRUE, &val_ptr[9].num_item,
                                         &res_biterr))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 9;
          goto send_error;
        }
        else
        {
          CONDITIONAL_ASSIGN(9, pdata.res_biterr, res_biterr);
        }


        /* Validate the traffic class against dependent parameters.    */
        /* This must be done after user edits applied but before write */
        if (DSAT_ERROR == etsipkt_validate_tclass(parse_table, &pdata))
        {
          return DSAT_ERROR;
        }
        else
        {
          DS_AT_MSG1_HIGH("TClass validated: %d",pdata.traffic_class);
        }

        /* Write to EFS */
        if(dsatetsipkt_read_write_profile_param(cid,WRITE_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_REQ_QOS,
               &pdata,sizeof(ds_profile_3gpp_3gpp_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error set param : %d",cid);
          return DSAT_ERROR;
        }
        if( src_stat_desc_req == 1) 
        {
          if( !((pdata.traffic_class == DS_PROFILE_3GPP_TC_CONVERSATIONAL) ||
                (pdata.traffic_class == DS_PROFILE_3GPP_TC_STREAMING)) )
          {
            DS_AT_MSG0_HIGH("Error Src Stat Desc shouldn't set with traffic class"
                      "other than conversational or streaming");
            return DSAT_ERROR;
          }
        }
        if(dsatetsipkt_read_write_profile_param(cid,WRITE_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_SRC_STAT_DESC_REQ ,
               &src_stat_desc_req,sizeof(uint32)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error set param : %d",cid);
          return DSAT_ERROR;
        }
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /*---------------------------------------------------------------
      Processing for a command to query valid parameters
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgeqreq_response_fmt (parse_table,
                                               &res_buff_ptr,
                                               MULTI_INDEX_TEST))
    {
      DS_AT_MSG0_HIGH("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|QU))
  {
    /*---------------------------------------------------------------
       Processing for a command to query current parameters
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgeqreq_response_fmt (parse_table,
                                               &res_buff_ptr,
                                               MULTI_INDEX_READ))
    {
      DS_AT_MSG0_HIGH("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA))
  {
    /* Do nothing */
    result = DSAT_OK;
  }
  else
  {
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }
  return result;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_SYNTAX)
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;
} /* dsatetsipkt_exec_cgeqreq_cmd */

/*===========================================================================

FUNCTION DSATETSIPKT_CGEQREQ_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGEQREQ read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

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
dsat_num_item_type dsatetsipkt_cgeqreq_response_fmt
(
  const dsati_cmd_type  *parse_table,   /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format         /*  Flag for format type        */
)
{
  byte name_buff[16];
  byte i, cntr = 0;
  int size = 0;
  int result = 0;
  dsm_item_type * res_buff_ptr = *out_buff_ptr;
  ds_profile_3gpp_3gpp_qos_params_type pdata;
  uint32                           src_stat_desc_req = 0;
    
  uint8             profile_list[DSAT_MAX_ALLOWED_PROFILES];
  uint8             total_profiles = 0;
    
  ds_profile_tech_etype tech_type = DS_PROFILE_TECH_INVALID;

  /* Format the command name */
  (void)snprintf((char*)name_buff, 
                       sizeof(name_buff),
                       "%s: ", parse_table->name);
  res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
  
  switch (format)
  {
    case MULTI_INDEX_TEST:
      /* Report each PDP type on seperate line */
      for (i=0; i <= (byte)DSAT_MAX_PDP_TYPE; i++) 
      {
        size = snprintf ((char *)res_buff_ptr->data_ptr, 
                 res_buff_ptr->size,
                 "%s%s%s\"%s\",(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d)",
                 res_buff_ptr->data_ptr,
                 (i > 0) ? "\n" : "",
                 name_buff,
                 dsat_pdptype_valstr[i],       
                 ((dflm_type *)dsat_cgeqreq_tclass_dflm)->lower,
                 ((dflm_type *)dsat_cgeqreq_tclass_dflm)->upper,
                 ((dflm_type *)dsat_cgeqreq_req_ul_bitrate_dflm)->lower,
                 ((dflm_type *)dsat_cgeqreq_req_ul_bitrate_dflm)->upper,
                 ((dflm_type *)dsat_cgeqreq_req_dl_bitrate_dflm)->lower,
                 ((dflm_type *)dsat_cgeqreq_req_dl_bitrate_dflm)->upper,
                 ((dflm_type *)dsat_cgeqreq_gtd_ul_bitrate_dflm)->lower,
                 ((dflm_type *)dsat_cgeqreq_gtd_ul_bitrate_dflm)->upper,
                 ((dflm_type *)dsat_cgeqreq_gtd_dl_bitrate_dflm)->lower,
                 ((dflm_type *)dsat_cgeqreq_gtd_dl_bitrate_dflm)->upper,
                 ((dflm_type *)dsat_cgeqreq_dlvry_order_dflm)->lower,
                 ((dflm_type *)dsat_cgeqreq_dlvry_order_dflm)->upper,
                 ((dflm_type *)dsat_cgeqreq_max_sdu_size_dflm)->lower,
                 ((dflm_type *)dsat_cgeqreq_max_sdu_size_dflm)->upper
                );
        
        size = snprintf ((char *)res_buff_ptr->data_ptr, 
                 res_buff_ptr->size,	
                 "%s,%s,%s,(%d-%d),(%d,%d-%d),(%d-%d),(%d-%d),(%d-%d)",
                 res_buff_ptr->data_ptr,
                 dsat_cgeqreq_sdu_err_tststr,
                 dsat_cgeqreq_res_biterr_tststr,
                 ((dflm_type *)dsat_cgeqreq_dlvry_err_dflm)->lower,
                 ((dflm_type *)dsat_cgeqreq_dlvry_err_dflm)->upper,
                 ((dflm_type *)dsat_cgeqreq_trans_delay_dflm)->lower,
                 ((dflm_type *)dsat_cgeqreq_trans_delay_dflm+1)->lower,
                 ((dflm_type *)dsat_cgeqreq_trans_delay_dflm+1)->upper,
                 ((dflm_type *)dsat_cgeqreq_thandle_prio_dflm)->lower,
                 ((dflm_type *)dsat_cgeqreq_thandle_prio_dflm)->upper,
                 ((dflm_type *)dsat_cgeqreq_src_stat_desc_dflm)->lower,
                 ((dflm_type *)dsat_cgeqreq_src_stat_desc_dflm)->upper,
                 ((dflm_type *)dsat_cgeqreq_sig_ind_dflm)->lower,
                 ((dflm_type *)dsat_cgeqreq_sig_ind_dflm)->upper
                );
      }
      res_buff_ptr->used = (uint16)size;
      result = size;
      break;

    case MULTI_INDEX_READ:

      if(0 == (dsatutil_strncmp_ig_sp_case((const byte *)parse_table->name,
                                           (const byte *)"+QCGEQREQ" ,strlen((const char *)"+QCGEQREQ"))))
      {
        tech_type = DS_PROFILE_TECH_EPC;
        if (DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_EPC, &profile_list[0],
                                                    sizeof(profile_list), &total_profiles,
                                                    dsat_get_current_ds_profile_subs_id()) )
        {
          DS_AT_MSG0_ERROR("Error getting EPC profile list!");
        }
      }
      else
      {
        // Means +CGEQREQ command
        tech_type = DS_PROFILE_TECH_3GPP;
        if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                                   sizeof(profile_list), &total_profiles,
                                                   dsat_get_current_ds_profile_subs_id()) )
        {
          DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
        }
      }

      for (i=0; i < total_profiles; i++) 
      {
        ds_profile_3gpp_traffic_class_type      traffic_class;
        ds_profile_3gpp_qos_delivery_order_type dlvry_order;
        ds_profile_3gpp_sdu_err_ratio_type      sdu_err;
        ds_profile_3gpp_deliver_err_sdu_type    sdu_dlvr;
        ds_profile_3gpp_residual_ber_ratio_type res_biterr;
        dsat_num_item_type tclass = 0;
        dsat_num_item_type dorder = 0;
        dsat_num_item_type sdu_err_index = 0;
        dsat_num_item_type dlvr_err_sdu = 0;
        dsat_num_item_type res_ber_index = 0;
        
        /* Query profile from EFS */
        memset((void*)&pdata, 0, sizeof(pdata));
        if (!dsatetsipkt_is_profile_context_valid(profile_list[i], tech_type,
                                                  dsat_get_current_ds_profile_subs_id()))
        {
          continue;
        }
        if(dsatetsipkt_read_write_profile_param(profile_list[i],READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_REQ_QOS,
               &pdata,sizeof(ds_profile_3gpp_3gpp_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param : %d",profile_list[i]);
          continue;
        }
        if(dsatetsipkt_read_write_profile_param(profile_list[i],READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_SRC_STAT_DESC_REQ ,
                     &src_stat_desc_req,sizeof(uint32)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param : %d",profile_list[i]);
          continue;
        }
        
        /* Perform table lookups */
        traffic_class = pdata.traffic_class;
        if (FALSE ==
            etsipkt_map_tclass(FALSE, &tclass, &traffic_class))
        {
          DS_AT_MSG1_HIGH("Error mapping TClass: %d",traffic_class);
          return 0;
        }
        
        dlvry_order = pdata.dlvry_order;
        if (FALSE ==
            etsipkt_map_dorder(FALSE, &dorder, &dlvry_order))
        {
          DS_AT_MSG1_HIGH("Error mapping DOrder: %d",dlvry_order);
          return 0;
        }
        
        sdu_err = pdata.sdu_err;
        if (FALSE ==
            etsipkt_map_sdu_err(FALSE, &sdu_err_index, &sdu_err))
        {
          DS_AT_MSG1_HIGH("Error mapping SDU error: %d",sdu_err);
          return 0;
        }
        
        sdu_dlvr = pdata.dlvr_err_sdu;
        if (FALSE ==
            etsipkt_map_dlvr_err_sdu(FALSE, &dlvr_err_sdu, &sdu_dlvr))
        {
          DS_AT_MSG1_HIGH("Error mapping Dlvr Err SDU: %d",sdu_dlvr);
          return 0;
        }

        res_biterr = pdata.res_biterr;
        if (FALSE ==
            etsipkt_map_res_ber(FALSE, &res_ber_index, &res_biterr))
        {
          DS_AT_MSG1_HIGH("Error mapping RES BER: %d",pdata.res_biterr);
          return 0;
        }
        
        /* Check to see if a new DSM item is required */
        CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - 30));
          
        /* Generate response from internal storage */
        res_buff_ptr->used = (uint16)snprintf((char *)res_buff_ptr->data_ptr, 
                 res_buff_ptr->size,
                 "%s%s%s%d,%d,%lu,%lu,%lu,%lu,%d,%lu,\"%s\",\"%s\",%d,%lu,%lu,%u,%d",
                 res_buff_ptr->data_ptr,
                 (cntr > 0) ? "\n" : "",
                 name_buff,
                 profile_list[i],
                 tclass,
                 pdata.max_ul_bitrate,       
                 pdata.max_dl_bitrate,       
                 pdata.gtd_ul_bitrate,       
                 pdata.gtd_dl_bitrate,       
                 dorder,       
                 pdata.max_sdu_size,       
                 dsat_cgeqreq_sdu_err_valstr[sdu_err_index],
                 dsat_cgeqreq_res_biterr_valstr[res_ber_index],
                 dlvr_err_sdu,
                 pdata.trans_delay,
                 pdata.thandle_prio,
                 (unsigned int)src_stat_desc_req,
                 pdata.sig_ind
                );
        cntr++;
      }
      
      /* No contexts defined, just print command name */
      if (0 == cntr)
      {
        res_buff_ptr->used = (uint16)snprintf((char *)res_buff_ptr->data_ptr,
                       res_buff_ptr->size,
                       "%s%s",
                       res_buff_ptr->data_ptr,
                       name_buff);
      }
      result = res_buff_ptr->used;
      break;

    case MULTI_INDEX_AMPV:
      size = res_buff_ptr->used;
      res_buff_ptr->used = (word)snprintf(
                                   (char *)res_buff_ptr->data_ptr,
                                   res_buff_ptr->size,
                                   "%s%s",
                                   res_buff_ptr->data_ptr,
                                   name_buff);
      if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                                 sizeof(profile_list), &total_profiles,
                                                 dsat_get_current_ds_profile_subs_id()) )
      {
        DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
      }


      for ( i = 0; i < total_profiles; i++) 
      {
        /* Local variables for PACKED structure */
        ds_profile_3gpp_traffic_class_type      traffic_class;
        ds_profile_3gpp_qos_delivery_order_type dlvry_order;
        ds_profile_3gpp_sdu_err_ratio_type      sdu_err;
        ds_profile_3gpp_deliver_err_sdu_type    sdu_dlvr;
        ds_profile_3gpp_residual_ber_ratio_type res_biterr;
        
        dsat_num_item_type tclass = 0;
        dsat_num_item_type dorder = 0;
        dsat_num_item_type sdu_err_index = 0;
        dsat_num_item_type dlvr_err_sdu = 0;
        dsat_num_item_type res_ber_index = 0;
        
        /* Query profile from EFS */
        memset((void*)&pdata, 0, sizeof(pdata));
        if(dsatetsipkt_read_write_profile_param(profile_list[i],READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_REQ_QOS,
               &pdata,sizeof(ds_profile_3gpp_3gpp_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param : %d",profile_list[i]);
          continue;
        }
        if(dsatetsipkt_read_write_profile_param(profile_list[i],READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_SRC_STAT_DESC_REQ ,
                     &src_stat_desc_req,sizeof(uint32)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param : %d",profile_list[i]);
          continue;
        }
        
        /* Perform table lookups */
        traffic_class = pdata.traffic_class;
        if (FALSE ==
            etsipkt_map_tclass(FALSE, &tclass, &traffic_class))
        {
          DS_AT_MSG1_HIGH("Error mapping TClass: %d",traffic_class);
          return 0;
        }
        
        dlvry_order = pdata.dlvry_order;
        if (FALSE ==
            etsipkt_map_dorder(FALSE, &dorder, &dlvry_order))
        {
          DS_AT_MSG1_HIGH("Error mapping DOrder: %d",dlvry_order);
          return 0;
        }
        
        sdu_err = pdata.sdu_err;
        if (FALSE ==
            etsipkt_map_sdu_err(FALSE, &sdu_err_index, &sdu_err))
        {
          DS_AT_MSG1_HIGH("Error mapping SDU error: %d",sdu_err);
          return 0;
        }
        
        sdu_dlvr = pdata.dlvr_err_sdu;
        if (FALSE ==
            etsipkt_map_dlvr_err_sdu(FALSE, &dlvr_err_sdu, &sdu_dlvr))
        {
          DS_AT_MSG1_HIGH("Error mapping Dlvr Err SDU: %d",sdu_dlvr);
          return 0;
        }

        res_biterr = pdata.res_biterr;
        if (FALSE ==
            etsipkt_map_res_ber(FALSE, &res_ber_index, &res_biterr))
        {
          DS_AT_MSG1_HIGH("Error mapping RES BER: %d",pdata.res_biterr);
          return 0;
        }
        
        /* Check to see if a new DSM item is required */
        CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - 30));
          
        /* Generate response from internal storage */
        res_buff_ptr->used = (word)snprintf (
                 (char *)res_buff_ptr->data_ptr, 
                 res_buff_ptr->size,
                 "%s%s(%d,%d,%lu,%lu,%lu,%lu,%d,%lu,\"%s\",\"%s\",%d,%lu,%lu,%u,%d)",
                 res_buff_ptr->data_ptr,
                 (cntr > 0) ? "," : "",
                 profile_list[i],
                 tclass,
                 pdata.max_ul_bitrate,       
                 pdata.max_dl_bitrate,       
                 pdata.gtd_ul_bitrate,       
                 pdata.gtd_dl_bitrate,       
                 dorder,       
                 pdata.max_sdu_size,       
                 dsat_cgeqreq_sdu_err_valstr[sdu_err_index],
                 dsat_cgeqreq_res_biterr_valstr[res_ber_index],
                 dlvr_err_sdu,
                 pdata.trans_delay,
                 pdata.thandle_prio,
                 (unsigned int)src_stat_desc_req,
                 pdata.sig_ind
                );
        cntr++;
      }

      /* Return zero if DSM item changed */
      result = (res_buff_ptr == *out_buff_ptr)?
               (res_buff_ptr->used - size) : 0;
      (*out_buff_ptr)->used -= (word)result;  /* caller increments used */
      break;
      
    default:
      DS_AT_MSG0_HIGH("Unsupported multi-index format type");
  }

  return (dsat_num_item_type)result;
}  /* dsatetsipkt_cgeqreq_response_fmt */



/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGEQMIN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGEQMIN & + QCGEQMIN command.
  This command sets PDP context parameters for UMTS QoS Profile
  (Minimum).

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  Defining +CGEQMIN parameters will undefine +CGQREQ/+CGQMIN parameters;
  these commands are mutually exclusive.

===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgeqmin_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type val = 0;
  dsat_mixed_param_val_type * val_ptr = 
    (dsat_mixed_param_val_type *) dsatutil_get_val(parse_table->cmd_id,0,0,STR_TYPE);
  uint32                           src_stat_desc_min = 0;
  byte cid = 0;

  ds_profile_status_etype  profile_status;
  dsat_error_info_s_type    err_info;

  ds_profile_tech_etype             tech_type = DS_PROFILE_TECH_INVALID;
  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if (tok_ptr->op == (NA|EQ|AR))
  {
    /*---------------------------------------------------------------
      Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/

    /* Validate connection ID */
    if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 0, &val,1))
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 0;
      goto send_error;
    }

    cid       = (byte)val;

    if(0 == (dsatutil_strncmp_ig_sp_case((const byte *)parse_table->name,
                                         (const byte *)"+QCGEQMIN" ,strlen((const char *)"+QCGEQMIN"))))
    {
      //EPC check range over here
      tech_type = DS_PROFILE_TECH_EPC;
      if ((cid < 100) ||
          (TRUE != dsatetsipkt_is_profile_context_valid(cid,
                                                        tech_type,
                                                        dsat_get_current_ds_profile_subs_id())))
      {
        DS_AT_MSG2_ERROR("Error Incorrect cid:%d, tech_type:%d pair",cid, tech_type);
        return DSAT_ERROR;
      }
    }
    else 
    {
      // 3GPP profiles check for +CGTFT
      tech_type = DS_PROFILE_TECH_3GPP;
      if (TRUE != dsatetsipkt_is_profile_context_valid(cid,
                                                       tech_type,
                                                       dsat_get_current_ds_profile_subs_id()))
      {
        DS_AT_MSG2_ERROR("Error Incorrect cid:%d, tech_type:%d pair",cid, tech_type);
        return DSAT_ERROR;
      }
    }

    /* Test for only CID specified */
    if (1 == tok_ptr->args_found)
    {
      /* Clear the user specified context parameters */
      ETSIPKT_RESET_INVALID_PROFILE_PARAM(val,DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_MIN_QOS,tech_type);
      if ( DS_PROFILE_REG_RESULT_SUCCESS != profile_status )
      {
        return DSAT_ERROR;
      }
      ETSIPKT_RESET_INVALID_PROFILE_PARAM(val,DS_PROFILE_3GPP_PROFILE_PARAM_SRC_STAT_DESC_MIN,tech_type);
      if ( DS_PROFILE_REG_RESULT_SUCCESS != profile_status )
      {
        return DSAT_ERROR;
      }
    }
    else
    {
      /* Perform normal parameter processing */
      if (DSAT_OK == dsatparm_exec_param_cmd(
                                             mode,
                                             parse_table,
                                             tok_ptr,
                                             res_buff_ptr
                                            ))
      {
        /* Transfer user data to internal storage */
        ds_profile_3gpp_3gpp_qos_params_type  pdata;
        /* Local variables for PACKED structure */
        ds_profile_3gpp_traffic_class_type      traffic_class;
        ds_profile_3gpp_qos_delivery_order_type dlvry_order;
        ds_profile_3gpp_sdu_err_ratio_type      sdu_err;
        ds_profile_3gpp_deliver_err_sdu_type    sdu_dlvr;
        ds_profile_3gpp_residual_ber_ratio_type res_biterr;

        /* Set specified profile to defaults in both GPRS and UMTS */
        ETSIPKT_RESET_DEFAULT_PROFILE_PARAM(cid,DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_MIN_QOS);
        if ( DS_PROFILE_REG_RESULT_SUCCESS != profile_status )
        {
          DS_AT_MSG1_ERROR("Unable to Reset for <cid> : %d",cid);
          return DSAT_ERROR;
        }
        ETSIPKT_RESET_DEFAULT_PROFILE_PARAM(cid,DS_PROFILE_3GPP_PROFILE_PARAM_SRC_STAT_DESC_MIN);
        if ( DS_PROFILE_REG_RESULT_SUCCESS != profile_status )
        {
          DS_AT_MSG1_ERROR("Unable to Reset for <cid> : %d",cid);
          return DSAT_ERROR;
        }
        /* Query default profile from EFS */
        memset((void*)&pdata, 0, sizeof(pdata));

        if(dsatetsipkt_read_write_profile_param(cid,READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_MIN_QOS,
               &pdata,sizeof(ds_profile_3gpp_3gpp_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param : %d",cid);
          return DSAT_ERROR;
        }
        if(dsatetsipkt_read_write_profile_param(cid,READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_SRC_STAT_DESC_MIN ,
                     &src_stat_desc_min,sizeof(uint32)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param : %d",cid);
          return DSAT_ERROR;
        }
        
        /* Make conditional edits to profile */
        CONDITIONAL_ASSIGN(2,  pdata.max_ul_bitrate, val_ptr[2].num_item);
        CONDITIONAL_ASSIGN(3,  pdata.max_dl_bitrate, val_ptr[3].num_item);
        CONDITIONAL_ASSIGN(4,  pdata.gtd_ul_bitrate, val_ptr[4].num_item);
        CONDITIONAL_ASSIGN(5,  pdata.gtd_dl_bitrate, val_ptr[5].num_item);
        CONDITIONAL_ASSIGN(7,  pdata.max_sdu_size,   val_ptr[7].num_item);
        CONDITIONAL_ASSIGN(11, pdata.trans_delay,    val_ptr[11].num_item);
        CONDITIONAL_ASSIGN(12, pdata.thandle_prio,   val_ptr[12].num_item);
        CONDITIONAL_ASSIGN(13, src_stat_desc_min,    val_ptr[13].num_item);
        CONDITIONAL_ASSIGN(14, pdata.sig_ind,        val_ptr[14].num_item);

        /* Map Traffic Class parameter to enum type */
        if (FALSE == etsipkt_map_tclass (TRUE, &val_ptr[1].num_item,
                                         &traffic_class))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 1;
          goto send_error;
        }
        else
        {
          CONDITIONAL_ASSIGN(1, pdata.traffic_class, traffic_class);
        }

        /* Map Delivery Order parameter to enum type */
        if (FALSE == etsipkt_map_dorder (TRUE, &val_ptr[6].num_item,
                                         &dlvry_order))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 6;
          goto send_error;
        }
        else
        {
          CONDITIONAL_ASSIGN(6, pdata.dlvry_order, dlvry_order);
        }
          
        /* Map SDU error parameter to enum type */
        if (FALSE == etsipkt_map_sdu_err(TRUE, &val_ptr[8].num_item,
                                         &sdu_err))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 8;
          goto send_error;
        }
        else
        {
          CONDITIONAL_ASSIGN(8, pdata.sdu_err, sdu_err);
        }
          
        /* Map Erroneous SDU Delivery parameter to enum type */
        if (FALSE == etsipkt_map_dlvr_err_sdu(TRUE, &val_ptr[10].num_item,
                                              &sdu_dlvr))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 10;
          goto send_error;
        }
        else
        {
          CONDITIONAL_ASSIGN(10, pdata.dlvr_err_sdu, sdu_dlvr);
        }

        /* Map Residual BER parameter to enum type */
        if (FALSE == etsipkt_map_res_ber(TRUE, &val_ptr[9].num_item,
                                         &res_biterr))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 9;
          goto send_error;
        }
        else
        {
          CONDITIONAL_ASSIGN(9, pdata.res_biterr, res_biterr);
        }


        /* Validate the traffic class against dependent parameters.    */
        /* This must be done after user edits applied but before write */
        if (DSAT_ERROR == etsipkt_validate_tclass(parse_table, &pdata))
        {
          return DSAT_ERROR;
        }
        else
        {
          DS_AT_MSG1_HIGH("TClass validated: %d",pdata.traffic_class);
        }

        /* Write to EFS */
        if(dsatetsipkt_read_write_profile_param(cid,WRITE_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_MIN_QOS,
               &pdata,sizeof(ds_profile_3gpp_3gpp_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error set param : %d",cid);
          return DSAT_ERROR;
        }
        if( src_stat_desc_min == 1) 
        {
          if( !((pdata.traffic_class == DS_PROFILE_3GPP_TC_CONVERSATIONAL) ||
                (pdata.traffic_class == DS_PROFILE_3GPP_TC_STREAMING)) )
          {
            DS_AT_MSG0_HIGH("Error Src Stat Desc shouldn't set with traffic class"
                      "other than conversational or streaming");
            return DSAT_ERROR;
          }
        }
        if(dsatetsipkt_read_write_profile_param(cid,WRITE_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_SRC_STAT_DESC_MIN ,
               &src_stat_desc_min,sizeof(uint32)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error set param : %d",cid);
          return DSAT_ERROR;
        }

      }
      else
      {
        result = DSAT_ERROR;
      }
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /*---------------------------------------------------------------
      Processing for a command to query valid parameters
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgeqmin_response_fmt (parse_table,
                                               &res_buff_ptr,
                                               MULTI_INDEX_TEST))
    {
      DS_AT_MSG0_HIGH("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|QU))
  {
    /*---------------------------------------------------------------
       Processing for a command to query current parameters
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgeqmin_response_fmt (parse_table,
                                               &res_buff_ptr,
                                               MULTI_INDEX_READ))
    {
      DS_AT_MSG0_HIGH("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA))
  {
    /* Do nothing */
    result = DSAT_OK;
  }
  else
  {
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }
  return result;
  
send_error:
  if( err_info.errval == DSAT_ERR_INVALID_SYNTAX)
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;

} /* dsatetsipkt_exec_cgeqmin_cmd */



/*===========================================================================

FUNCTION DSATETSIPKT_CGEQMIN_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGEQMIN read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

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
dsat_num_item_type dsatetsipkt_cgeqmin_response_fmt
(
  const dsati_cmd_type  *parse_table,   /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format         /*  Flag for format type        */
)
{
  byte name_buff[16];
  byte i, cntr = 0;
  int size = 0;
  int result = 0;
  dsm_item_type * res_buff_ptr = *out_buff_ptr;
  ds_profile_3gpp_3gpp_qos_params_type pdata;
  uint32                           src_stat_desc_min = 0;
    
  uint8             profile_list[DSAT_MAX_ALLOWED_PROFILES];
  uint8             total_profiles = 0;
  ds_profile_tech_etype tech_type = DS_PROFILE_TECH_INVALID;

    
  /* Format the command name */
  (void)snprintf((char*)name_buff,sizeof(name_buff),
                       "%s: ", parse_table->name);
  res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
  
  switch (format)
  {
    case MULTI_INDEX_TEST:
      /* Report each PDP type on seperate line */
      for (i=0; i <= (byte)DSAT_MAX_PDP_TYPE; i++) 
      {
        size = snprintf ((char *)res_buff_ptr->data_ptr, 
                 res_buff_ptr->size,
                 "%s%s%s\"%s\",(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d)",
                 res_buff_ptr->data_ptr,
                 (i > 0) ? "\n" : "",
                 name_buff,
                 dsat_pdptype_valstr[i],       
                 ((dflm_type *)dsat_cgeqmin_tclass_dflm)->lower,
                 ((dflm_type *)dsat_cgeqmin_tclass_dflm)->upper,
                 ((dflm_type *)dsat_cgeqmin_req_ul_bitrate_dflm)->lower,
                 ((dflm_type *)dsat_cgeqmin_req_ul_bitrate_dflm)->upper,
                 ((dflm_type *)dsat_cgeqmin_req_dl_bitrate_dflm)->lower,
                 ((dflm_type *)dsat_cgeqmin_req_dl_bitrate_dflm)->upper,
                 ((dflm_type *)dsat_cgeqmin_gtd_ul_bitrate_dflm)->lower,
                 ((dflm_type *)dsat_cgeqmin_gtd_ul_bitrate_dflm)->upper,
                 ((dflm_type *)dsat_cgeqmin_gtd_dl_bitrate_dflm)->lower,
                 ((dflm_type *)dsat_cgeqmin_gtd_dl_bitrate_dflm)->upper,
                 ((dflm_type *)dsat_cgeqmin_dlvry_order_dflm)->lower,
                 ((dflm_type *)dsat_cgeqmin_dlvry_order_dflm)->upper,
                 ((dflm_type *)dsat_cgeqmin_max_sdu_size_dflm)->lower,
                 ((dflm_type *)dsat_cgeqmin_max_sdu_size_dflm)->upper
                );
        
        size = snprintf ((char *)res_buff_ptr->data_ptr, 
                 res_buff_ptr->size,
                 "%s,%s,%s,(%d-%d),(%d,%d-%d),(%d-%d),(%d-%d),(%d-%d)",
                 res_buff_ptr->data_ptr,
                 dsat_cgeqmin_sdu_err_tststr,
                 dsat_cgeqmin_res_biterr_tststr,
                 ((dflm_type *)dsat_cgeqmin_dlvry_err_dflm)->lower,
                 ((dflm_type *)dsat_cgeqmin_dlvry_err_dflm)->upper,
                 ((dflm_type *)dsat_cgeqmin_trans_delay_dflm)->lower,
                 ((dflm_type *)dsat_cgeqmin_trans_delay_dflm+1)->lower,
                 ((dflm_type *)dsat_cgeqmin_trans_delay_dflm+1)->upper,
                 ((dflm_type *)dsat_cgeqmin_thandle_prio_dflm)->lower,
                 ((dflm_type *)dsat_cgeqmin_thandle_prio_dflm)->upper,
                 ((dflm_type *)dsat_cgeqmin_src_stat_desc_dflm)->lower,
                 ((dflm_type *)dsat_cgeqmin_src_stat_desc_dflm)->upper,
                 ((dflm_type *)dsat_cgeqmin_sig_ind_dflm)->lower,
                 ((dflm_type *)dsat_cgeqmin_sig_ind_dflm)->upper
                );
      }
      res_buff_ptr->used = (uint16)size;
      result = size;
      break;

    case MULTI_INDEX_READ:
      if(0 == (dsatutil_strncmp_ig_sp_case((const byte *)parse_table->name,
                                           (const byte *)"+QCGEQMIN" ,strlen((const char *)"+QCGEQMIN"))))
      {
        tech_type = DS_PROFILE_TECH_EPC;
        if (DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_EPC, &profile_list[0],
                                                    sizeof(profile_list), &total_profiles,
                                                    dsat_get_current_ds_profile_subs_id()) )
        {
          DS_AT_MSG0_ERROR("Error getting EPC profile list!");
        }
      }
      else
      {
        // Means +CGEQMIN command
        tech_type = DS_PROFILE_TECH_3GPP;
        if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                                   sizeof(profile_list), &total_profiles,
                                                   dsat_get_current_ds_profile_subs_id()) )
        {
          DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
        }
      }

      /* Report each profile on seperate line */
      for (i=0; i < total_profiles; i++) 
      {
        ds_profile_3gpp_traffic_class_type      traffic_class;
        ds_profile_3gpp_qos_delivery_order_type dlvry_order;
        ds_profile_3gpp_sdu_err_ratio_type      sdu_err;
        ds_profile_3gpp_deliver_err_sdu_type    sdu_dlvr;
        ds_profile_3gpp_residual_ber_ratio_type res_biterr;

        dsat_num_item_type tclass = 0;
        dsat_num_item_type dorder = 0;
        dsat_num_item_type sdu_err_index = 0;
        dsat_num_item_type dlvr_err_sdu = 0;
        dsat_num_item_type res_ber_index = 0;
        
        /* Query profile from EFS */
        memset((void*)&pdata, 0, sizeof(pdata));
        if (FALSE == dsatetsipkt_is_profile_context_valid(profile_list[i], tech_type,
                                                          dsat_get_current_ds_profile_subs_id()))

        {
          continue;
        }
        if(dsatetsipkt_read_write_profile_param(profile_list[i],READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_MIN_QOS,
               &pdata,sizeof(ds_profile_3gpp_3gpp_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param : %d",profile_list[i]);
          continue;
        }
        if(dsatetsipkt_read_write_profile_param(profile_list[i],READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_SRC_STAT_DESC_MIN ,
                     &src_stat_desc_min,sizeof(uint32)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param : %d",profile_list[i]);
          continue;
        }
        
        /* Perform table lookups */
        traffic_class = pdata.traffic_class;
        if (FALSE ==
            etsipkt_map_tclass(FALSE, &tclass, &traffic_class))
        {
          DS_AT_MSG1_HIGH("Error mapping TClass: %d",traffic_class);
          return 0;
        }
        
        dlvry_order = pdata.dlvry_order;
        if (FALSE ==
            etsipkt_map_dorder(FALSE, &dorder, &dlvry_order))
        {
          DS_AT_MSG1_HIGH("Error mapping DOrder: %d",dlvry_order);
          return 0;
        }
        
        sdu_err = pdata.sdu_err;
        if (FALSE ==
            etsipkt_map_sdu_err(FALSE, &sdu_err_index, &sdu_err))
        {
          DS_AT_MSG1_HIGH("Error mapping SDU error: %d",sdu_err);
          return 0;
        }
        
        sdu_dlvr = pdata.dlvr_err_sdu;
        if (FALSE ==
            etsipkt_map_dlvr_err_sdu(FALSE, &dlvr_err_sdu, &sdu_dlvr))
        {
          DS_AT_MSG1_HIGH("Error mapping Dlvr Err SDU: %d",sdu_dlvr);
          return 0;
        }

        res_biterr = pdata.res_biterr;
        if (FALSE ==
            etsipkt_map_res_ber(FALSE, &res_ber_index, &res_biterr))
        {
          DS_AT_MSG1_HIGH("Error mapping RES BER: %d",pdata.res_biterr);
          return 0;
        }
        
        /* Check to see if a new DSM item is required */
        CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - 30));
        
        /* Generate response from internal storage */
        res_buff_ptr->used = (uint16)snprintf(
                 (char *)res_buff_ptr->data_ptr, 
                 res_buff_ptr->size,
                 "%s%s%s%d,%d,%lu,%lu,%lu,%lu,%d,%lu,\"%s\",\"%s\",%d,%lu,%lu,%u,%d",
                 res_buff_ptr->data_ptr,
                 (cntr > 0) ? "\n" : "",
                 name_buff,
                 profile_list[i],
                 tclass,
                 pdata.max_ul_bitrate,       
                 pdata.max_dl_bitrate,       
                 pdata.gtd_ul_bitrate,       
                 pdata.gtd_dl_bitrate,       
                 dorder,       
                 pdata.max_sdu_size,       
                 dsat_cgeqmin_sdu_err_valstr[sdu_err_index],
                 dsat_cgeqmin_res_biterr_valstr[res_ber_index],
                 dlvr_err_sdu,
                 pdata.trans_delay,
                 pdata.thandle_prio,
                 (unsigned int)src_stat_desc_min,
                 pdata.sig_ind
                );
        cntr++;
      }
      
      /* No contexts defined, just print command name */
      if (0 == cntr)
      {
        res_buff_ptr->used = (uint16)snprintf(
                       (char *)res_buff_ptr->data_ptr,
                       res_buff_ptr->size,
                       "%s%s",
                       res_buff_ptr->data_ptr,
                       name_buff);
      }
      result = res_buff_ptr->used;
      break;

    case MULTI_INDEX_AMPV:
      size = res_buff_ptr->used;
      res_buff_ptr->used = (word)snprintf(
                                   (char *)res_buff_ptr->data_ptr,
                                   res_buff_ptr->size,
                                   "%s%s",
                                   res_buff_ptr->data_ptr,
                                   name_buff);

      if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                                 sizeof(profile_list), &total_profiles,
                                                 dsat_get_current_ds_profile_subs_id()) )
      {
        DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
      }

      /* Report each profile on seperate line */
      for ( i = 0; i < total_profiles; i++) 
      {
        /* Local variables for PACKED structure */
        ds_profile_3gpp_traffic_class_type      traffic_class;
        ds_profile_3gpp_qos_delivery_order_type dlvry_order;
        ds_profile_3gpp_sdu_err_ratio_type      sdu_err;
        ds_profile_3gpp_deliver_err_sdu_type    sdu_dlvr;
        ds_profile_3gpp_residual_ber_ratio_type res_biterr;

        
        dsat_num_item_type tclass = 0;
        dsat_num_item_type dorder = 0;
        dsat_num_item_type sdu_err_index = 0;
        dsat_num_item_type dlvr_err_sdu = 0;
        dsat_num_item_type res_ber_index = 0;
        
        /* Query profile from EFS */
        memset((void*)&pdata, 0, sizeof(pdata));

        if(dsatetsipkt_read_write_profile_param(profile_list[i],READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_UMTS_MIN_QOS,
               &pdata,sizeof(ds_profile_3gpp_3gpp_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param : %d",profile_list[i]);
          continue;
        }
        if(dsatetsipkt_read_write_profile_param( profile_list[i],READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_SRC_STAT_DESC_MIN ,
                     &src_stat_desc_min,sizeof(uint32)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param : %d",profile_list[i]);
          continue;
        }
        
        /* Perform table lookups */
        traffic_class = pdata.traffic_class;
        if (FALSE ==
            etsipkt_map_tclass(FALSE, &tclass, &traffic_class))
        {
          DS_AT_MSG1_HIGH("Error mapping TClass: %d",traffic_class);
          return 0;
        }
        
        dlvry_order = pdata.dlvry_order;
        if (FALSE ==
            etsipkt_map_dorder(FALSE, &dorder, &dlvry_order))
        {
          DS_AT_MSG1_HIGH("Error mapping DOrder: %d",dlvry_order);
          return 0;
        }
        
        sdu_err = pdata.sdu_err;
        if (FALSE ==
            etsipkt_map_sdu_err(FALSE, &sdu_err_index, &sdu_err))
        {
          DS_AT_MSG1_HIGH("Error mapping SDU error: %d",sdu_err);
          return 0;
        }
        
        sdu_dlvr = pdata.dlvr_err_sdu;
        if (FALSE ==
            etsipkt_map_dlvr_err_sdu(FALSE, &dlvr_err_sdu, &sdu_dlvr))
        {
          DS_AT_MSG1_HIGH("Error mapping Dlvr Err SDU: %d",sdu_dlvr);
          return 0;
        }

        res_biterr = pdata.res_biterr;
        if (FALSE ==
            etsipkt_map_res_ber(FALSE, &res_ber_index, &res_biterr))
        {
          DS_AT_MSG1_HIGH("Error mapping RES BER: %d",pdata.res_biterr);
          return 0;
        }
        
        /* Check to see if a new DSM item is required */
        CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - 30));
        
        /* Generate response from internal storage */
        res_buff_ptr->used = (word)snprintf (
                 (char *)res_buff_ptr->data_ptr, 
                 res_buff_ptr->size,
                 "%s%s(%d,%d,%lu,%lu,%lu,%lu,%d,%lu,\"%s\",\"%s\",%d,%lu,%lu,%u,%d)",
                 res_buff_ptr->data_ptr,
                 (cntr > 0) ? "," : "",
                 profile_list[i],
                 tclass,
                 pdata.max_ul_bitrate,       
                 pdata.max_dl_bitrate,       
                 pdata.gtd_ul_bitrate,       
                 pdata.gtd_dl_bitrate,       
                 dorder,       
                 pdata.max_sdu_size,       
                 dsat_cgeqmin_sdu_err_valstr[sdu_err_index],
                 dsat_cgeqmin_res_biterr_valstr[res_ber_index],
                 dlvr_err_sdu,
                 pdata.trans_delay,
                 pdata.thandle_prio,
                 (unsigned int)src_stat_desc_min,
                 pdata.sig_ind
                );
        cntr++;
      }

      /* Return zero if DSM item changed */
      result = (res_buff_ptr == *out_buff_ptr)?
               (res_buff_ptr->used - size) : 0;
      (*out_buff_ptr)->used -= (word)result;  /* caller increments used */
      break;
      
    default:
      DS_AT_MSG0_HIGH("Unsupported multi-index format type");
  }

  return (dsat_num_item_type)result;
}  /* dsatetsipkt_cgeqmin_response_fmt */
#endif /* FEATURE_DATA_WCDMA_PS */
#ifdef FEATURE_DSAT_LTE
/*===========================================================================
FUNCTION DSATETSIPKT_EXEC_CGEQOS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGEQOS and +QCGEQOS command.
  This command sets EPS QOS parameters for LTE/UMTS/GPRS QoS Profile
  (Requested) depending upon the mode.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS

===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatetsipkt_exec_cgeqos_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type val = 0;
  ds_profile_status_etype  profile_status;
  dsat_mixed_param_val_type * val_ptr = 
    (dsat_mixed_param_val_type *) dsatutil_get_val(parse_table->cmd_id,0,0,STR_TYPE);
  dsat_error_info_s_type    err_info;

  byte			            cid;
  ds_profile_tech_etype             tech_type = DS_PROFILE_TECH_INVALID;


  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  if (tok_ptr->op == (NA|EQ|AR))
  {
    /*---------------------------------------------------------------
      Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/

    if(( tok_ptr->args_found == 3)||( tok_ptr->args_found == 5))
    {
      err_info.errval = DSAT_ERR_INVALID_NUM_OF_ARGS;
      goto send_error;
    }

    /* Validate connection ID */
    if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 0, &val,1))
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 0;
      goto send_error;
    }
    
    cid       = (byte)val;

    if(0 == (dsatutil_strncmp_ig_sp_case((const byte *)parse_table->name,
                                         (const byte *)"+QCGEQOS" ,strlen((const char *)"+QCGEQOS"))))
    {
      //EPC check range over here
      tech_type = DS_PROFILE_TECH_EPC;
      if ((cid < 100) ||
          (TRUE != dsatetsipkt_is_profile_context_valid(cid,
                                                        tech_type,
                                                        dsat_get_current_ds_profile_subs_id())))
      {
        DS_AT_MSG2_ERROR("Error Incorrect cid:%d, tech_type:%d pair",cid, tech_type);
        return DSAT_ERROR;
      }
    }
    else 
    {
      tech_type = DS_PROFILE_TECH_3GPP;
      if (TRUE != dsatetsipkt_is_profile_context_valid(cid,
                                                       tech_type,
                                                       dsat_get_current_ds_profile_subs_id()))
      {
        DS_AT_MSG2_ERROR("Error Incorrect cid:%d, tech_type:%d pair",cid, tech_type);
        return DSAT_ERROR;
      }
    }
    
    /* Test for only CID specified */
    if (1 == tok_ptr->args_found)
    {
      /* Clear the user specified context parameters */
      ETSIPKT_RESET_INVALID_PROFILE_PARAM(val,DS_PROFILE_3GPP_PROFILE_PARAM_LTE_REQ_QOS,tech_type);
      if ( DS_PROFILE_REG_RESULT_SUCCESS != profile_status )
      {
        return DSAT_ERROR;
      }
    }
    else
    {
      /* Perform normal parameter processing */
      if (DSAT_OK == dsatparm_exec_param_cmd(
                                             mode,
                                             parse_table,
                                             tok_ptr,
                                             res_buff_ptr
                                            ))
      {
        /* Transfer user data to internal storage */
        ds_profile_3gpp_lte_qos_params_type pdata;
        ETSIPKT_RESET_DEFAULT_PROFILE_PARAM(cid,DS_PROFILE_3GPP_PROFILE_PARAM_LTE_REQ_QOS);
        /* Set specified profile to defaults */
        if ( DS_PROFILE_REG_RESULT_SUCCESS != profile_status )
        {
          DS_AT_MSG1_ERROR("Unable to Reset for <cid> : %d",cid);
          return DSAT_ERROR;
        }
        
        /* Read the profile values from EFS */
        memset((void*)&pdata, 0, sizeof(pdata));
        /* Read using DS Profile API*/
        if(dsatetsipkt_read_write_profile_param(cid,READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_LTE_REQ_QOS,
               &pdata,sizeof(ds_profile_3gpp_lte_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param CID : %d ",cid);
          return DSAT_ERROR;
        }
        /* Make conditional edits to profile */
        CONDITIONAL_ASSIGN(1,  pdata.qci, (ds_profile_3gpp_lte_qci_e_type)val_ptr[1].num_item);
        CONDITIONAL_ASSIGN(2,  pdata.g_dl_bit_rate,   val_ptr[2].num_item);
        CONDITIONAL_ASSIGN(3,  pdata.g_ul_bit_rate,   val_ptr[3].num_item);
        CONDITIONAL_ASSIGN(4,  pdata.max_dl_bit_rate, val_ptr[4].num_item);
        CONDITIONAL_ASSIGN(5,  pdata.max_ul_bit_rate, val_ptr[5].num_item);

        /* Write to EFS using DS Profile API*/
        if(dsatetsipkt_read_write_profile_param(cid,WRITE_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_LTE_REQ_QOS,
               &pdata,sizeof(ds_profile_3gpp_lte_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error set param : %d",cid);
          return DSAT_ERROR;
        }
      }
      else
      {
      err_info.errval = DSAT_ERR_INVALID_TOKENS;
      goto send_error;
      }
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /*---------------------------------------------------------------
      Processing Test command  
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgeqos_response_fmt (parse_table,
                                               &res_buff_ptr,
                                               MULTI_INDEX_TEST))
    {
      DS_AT_MSG0_HIGH("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|QU))
  {
    /*---------------------------------------------------------------
       Processing Read command  
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgeqos_response_fmt (parse_table,
                                               &res_buff_ptr,
                                               MULTI_INDEX_READ))
    {
      DS_AT_MSG0_HIGH("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA))
  {
    /* Do nothing */
    result = DSAT_OK;
  }
  else
  {
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }
  return result;
  
send_error:
  if( err_info.errval == DSAT_ERR_INVALID_NUM_OF_ARGS || 
      err_info.errval == DSAT_ERR_INVALID_SYNTAX ||
      err_info.errval == DSAT_ERR_INVALID_TOKENS )
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;
}/* dsatetsipkt_exec_cgeqos_cmd */

/*===========================================================================

FUNCTION DSATETSIPKT_CGEQREQ_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGEQREQ read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

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
dsat_num_item_type dsatetsipkt_cgeqos_response_fmt
(
  const dsati_cmd_type  *parse_table,   /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format         /*  Flag for format type        */
)
{
  byte name_buff[16];
  byte i, cntr = 0;
  int size = 0;
  int result = 0;
  dsm_item_type * res_buff_ptr = *out_buff_ptr;
  ds_profile_3gpp_lte_qos_params_type  pdata;
    
  uint8             profile_list[DSAT_MAX_ALLOWED_PROFILES];
  uint8             total_profiles = 0;
    
  ds_profile_tech_etype tech_type = DS_PROFILE_TECH_INVALID;
    
  /* Format the command name */
  (void)snprintf((char*)name_buff, 
                       sizeof(name_buff),
                       "%s: ", parse_table->name);
  res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
  
  switch (format)
  {
    case MULTI_INDEX_TEST:

      /*Range we support for 3GPP only profiles is: 1-16 
        Range we support for EPC profiles is:       100-179 */
      if(0 == (dsatutil_strncmp_ig_sp_case((const byte *)parse_table->name,
                                           (const byte *)"+QCGEQOS" ,strlen((const char *)"+QCGEQOS"))))
      {
        size = snprintf ((char *)res_buff_ptr->data_ptr, 
                 res_buff_ptr->size,
                 "%s%s(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d)",
                 res_buff_ptr->data_ptr,
                 name_buff,
                 DSAT_EXT_PROFILE_NUM_MIN,
                 DSAT_EXT_PROFILE_NUM_MAX,
                 ((dflm_type *)dsat_qci_dflm )->lower,
                 ((dflm_type *)dsat_qci_dflm)->upper,
                 ((dflm_type *)dsat_cgeqos_req_dl_gbr_dflm)->lower,
                 ((dflm_type *)dsat_cgeqos_req_dl_gbr_dflm)->upper,
                 ((dflm_type *)dsat_cgeqos_req_ul_gbr_dflm)->lower,
                 ((dflm_type *)dsat_cgeqos_req_ul_gbr_dflm)->upper,
                 ((dflm_type *)dsat_cgeqos_req_dl_mbr_dflm)->lower,
                 ((dflm_type *)dsat_cgeqos_req_dl_mbr_dflm)->upper,
                 ((dflm_type *)dsat_cgeqos_req_ul_mbr_dflm)->lower,
                 ((dflm_type *)dsat_cgeqos_req_ul_mbr_dflm)->upper
                );
      }
      else
      {
        // Means +CGEQOS command
      size = snprintf ((char *)res_buff_ptr->data_ptr, 
               res_buff_ptr->size,
               "%s%s(%d-%d,%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d),(%d-%d)",
               res_buff_ptr->data_ptr,
               name_buff,
               DSAT_LEGACY_PROFILE_NUM_MIN,
               DSAT_LEGACY_PROFILE_NUM_MAX,
               DSAT_EXT_PROFILE_NUM_MIN,
               DSAT_EXT_PROFILE_NUM_MAX,
               ((dflm_type *)dsat_qci_dflm )->lower,
               ((dflm_type *)dsat_qci_dflm)->upper,
               ((dflm_type *)dsat_cgeqos_req_dl_gbr_dflm)->lower,
               ((dflm_type *)dsat_cgeqos_req_dl_gbr_dflm)->upper,
               ((dflm_type *)dsat_cgeqos_req_ul_gbr_dflm)->lower,
               ((dflm_type *)dsat_cgeqos_req_ul_gbr_dflm)->upper,
               ((dflm_type *)dsat_cgeqos_req_dl_mbr_dflm)->lower,
               ((dflm_type *)dsat_cgeqos_req_dl_mbr_dflm)->upper,
               ((dflm_type *)dsat_cgeqos_req_ul_mbr_dflm)->lower,
               ((dflm_type *)dsat_cgeqos_req_ul_mbr_dflm)->upper
              );
      }
      res_buff_ptr->used = (uint16)size;
      result = size;
      break;

    case MULTI_INDEX_READ:
      if(0 == (dsatutil_strncmp_ig_sp_case((const byte *)parse_table->name,
                                           (const byte *)"+QCGEQOS" ,strlen((const char *)"+QCGEQOS"))))
      {
        tech_type = DS_PROFILE_TECH_EPC;
        if (DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_EPC, &profile_list[0],
                                                    sizeof(profile_list), &total_profiles,
                                                    dsat_get_current_ds_profile_subs_id()))
        {
          DS_AT_MSG0_ERROR("Error getting EPC profile list!");
        }
      }
      else
      {
        // Means +CGEQOS command
        tech_type = DS_PROFILE_TECH_3GPP;
        if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP, &profile_list[0],
                                                   sizeof(profile_list), &total_profiles,
                                                   dsat_get_current_ds_profile_subs_id()))
        {
          DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
        }
      }

      for (i=0; i < total_profiles; i++) 
      {
        memset((void*)&pdata, 0, sizeof(pdata));

        /* Read using DS Profile API*/
        if(dsatetsipkt_read_write_profile_param(profile_list[i],READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_LTE_REQ_QOS,
               &pdata,sizeof(ds_profile_3gpp_lte_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param CID : %d ",profile_list[i]);
          continue;
        }
        
        
        /* Check to see if a new DSM item is required */
        CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - 30));
          
        /* Generate response from internal storage */
        res_buff_ptr->used = (word)snprintf (
                        (char *)res_buff_ptr->data_ptr, 
                        res_buff_ptr->size,
                        "%s%s%s%d,%d,%lu,%lu,%lu,%lu",
                        res_buff_ptr->data_ptr,
                        (cntr > 0) ? "\n" : "",
                        name_buff,
                        profile_list[i],
                        pdata.qci,
                        pdata.g_dl_bit_rate,       
                        pdata.g_ul_bit_rate,       
                        pdata.max_dl_bit_rate,       
                        pdata.max_ul_bit_rate       
                       );
        cntr++;
      }
      
      /* No contexts defined, just print command name */
      if (0 == cntr)
      {
        res_buff_ptr->used = (uint16)snprintf((char *)res_buff_ptr->data_ptr,
                       res_buff_ptr->size,
                       "%s%s",
                       res_buff_ptr->data_ptr,
                       name_buff);
      }
      result = res_buff_ptr->used;
      break;

    case MULTI_INDEX_AMPV:
      size = res_buff_ptr->used;
      res_buff_ptr->used = (word)snprintf(
                                   (char *)res_buff_ptr->data_ptr,
                                   res_buff_ptr->size,
                                   "%s%s",
                                   res_buff_ptr->data_ptr,
                                   name_buff);
      if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                                 sizeof(profile_list), &total_profiles,
                                                 dsat_get_current_ds_profile_subs_id()))
      {
        DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
      }
        
      for (i = 0; i < total_profiles; i++) 
        {
        memset((void*)&pdata, 0, sizeof(pdata));
        
        /* Read using DS Profile API*/
        if(dsatetsipkt_read_write_profile_param(profile_list[i],READ_COMMAND,
                       DS_PROFILE_3GPP_PROFILE_PARAM_LTE_REQ_QOS,
                       &pdata,sizeof(ds_profile_3gpp_lte_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param CID : %d ",profile_list[i]);
          continue;
        }
        
        /* Check to see if a new DSM item is required */
        CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - 30));
          
        /* Generate response from internal storage */
        res_buff_ptr->used = (word)snprintf (
                        (char *)res_buff_ptr->data_ptr, 
                        res_buff_ptr->size,
                        "%s%s(%d,%d,%lu,%lu,%lu,%lu)",
                        res_buff_ptr->data_ptr,
                        (cntr > 0) ? "," : "",
                        profile_list[i],
                        pdata.qci,
                        pdata.g_dl_bit_rate,       
                        pdata.g_ul_bit_rate,       
                        pdata.max_dl_bit_rate,       
                        pdata.max_ul_bit_rate       
                       );
        cntr++;
      }
      
      /* Return zero if DSM item changed */
      result = (res_buff_ptr == *out_buff_ptr)?
               (res_buff_ptr->used - size) : 0;
      (*out_buff_ptr)->used -= (word)result;  /* caller increments used */
      break;

      
    default:
      DS_AT_MSG0_HIGH("Unsupported multi-index format type");
  }

  return (dsat_num_item_type)result;
}  /* dsatetsipkt_cgeqos_response_fmt */
#endif /* FEATURE_DSAT_LTE */
/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGCONTRDP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGCONTRDP command.
  This command returns a set of dynamic parameters associated with the specified
  context identifier.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatetsipkt_exec_cgcontrdp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_OK;

  /*---------------------------------------------------------
   Processing for a command with an argument (WRITE command)
  -----------------------------------------------------------*/
  if(tok_ptr->op == (NA|EQ|AR))
  {
    if(tok_ptr->args_found > 1)
    {
      DS_AT_MSG0_ERROR("Invalid number of Arguments in CGCONTRDP");
      result = DSAT_ERROR;
    }
    else
    {
      if( !VALID_TOKEN(0) )
      {
        /* No CID specified */
        result = etsipkt_cgcontrdp_format_response( NULL, 0,
                          tok_ptr,
                          res_buff_ptr);
      }
      else
      {
        dsat_num_item_type p_cid = 0 ;
    
        if( ATOI_OK == dsatutil_atoi (&p_cid, tok_ptr->arg[0], 10) )
        {
          /* Validate range */
          if( !DSAT_IS_PROFILE_ID_WITHIN_RANGE(p_cid) )
          {
            DS_AT_MSG1_ERROR("CID out of range value=%d",p_cid);
            result = DSAT_ERROR;
          }
        }
    
        /* Generate response */
        if( DSAT_OK == result)
        {
          result = etsipkt_cgcontrdp_format_response( (uint8 *)&p_cid, 1,
                                                    tok_ptr,
                                                    res_buff_ptr);
        }
      }
    }
  }
    /*-----------------------------------------------------------
     Processing for a command without an argument (WRITE command)
     and TEST syntax
    ------------------------------------------------------------*/
  else if( (tok_ptr->op == (NA)) ||
           (tok_ptr->op == (NA|EQ|QU)) )
  {
    result = etsipkt_cgcontrdp_format_response( NULL, 0,
                                              tok_ptr,
                                              res_buff_ptr );
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}/* dsatetsipkt_exec_cgcontrdp_cmd */

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGSCONTRDP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGSCONTRDP command.
  This command returns a set of dynamic parameters associated with the specified
  context identifier.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatetsipkt_exec_cgscontrdp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_OK;
  
  /*---------------------------------------------------------
   Processing for a command with an argument (WRITE command)
  -----------------------------------------------------------*/
  if(tok_ptr->op == (NA|EQ|AR))
  {
    if(tok_ptr->args_found > 1)
    {
      DS_AT_MSG0_ERROR("Invalid number of Arguments in CGSCONTRDP");
      result = DSAT_ERROR;
    }
    else
    {
      if( !VALID_TOKEN(0) )
      {
        /* No CID specified */
        result = etsipkt_cgscontrdp_format_response( NULL, 0,
                          tok_ptr,
                          res_buff_ptr);
      }
      else
      {
        dsat_num_item_type cid = 0 ;
    
        if( ATOI_OK != dsatutil_atoi (&cid, tok_ptr->arg[0], 10) )
        {
          result = DSAT_ERROR;
        }
    
        /* Generate response */
        if( DSAT_OK == result)
        {
          result = etsipkt_cgscontrdp_format_response( (uint8 *)&cid, 1,
                                                    tok_ptr,
                                                    res_buff_ptr);
        }
      }
    }
  }
    /*-----------------------------------------------------------
     Processing for a command without an argument (WRITE command)
     and TEST syntax
    ------------------------------------------------------------*/
  else if( (tok_ptr->op == (NA)) ||
            (tok_ptr->op == (NA|EQ|QU)) )
  {
    result = etsipkt_cgscontrdp_format_response( NULL, 0,
                                              tok_ptr,
                                              res_buff_ptr );
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
}/* dsatetsipkt_exec_cgscontrdp_cmd */

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGEQOSRDP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGEQOSRDP command.
  This command returns a set of dynamin parameters associated with the specified
  context identifier.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatetsipkt_exec_cgeqosrdp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_OK;
  
  /*---------------------------------------------------------
   Processing for a command with an argument (WRITE command)
  -----------------------------------------------------------*/
  if(tok_ptr->op == (NA|EQ|AR))
  {
    if(tok_ptr->args_found > 1)
    {
      DS_AT_MSG0_ERROR("Invalid number of Arguments in CGEQOSRDP");
      result = DSAT_ERROR;
    }
    else
    {
      if( !VALID_TOKEN(0) )
      {
        /* No CID specified */
        result = etsipkt_cgeqosrdp_format_response( NULL, 0,
                                                    tok_ptr,
                                                    res_buff_ptr);
      }
      else
      {
        dsat_num_item_type cid = 0 ;
    
        if( ATOI_OK  != dsatutil_atoi (&cid, tok_ptr->arg[0], 10) )
        {
          DS_AT_MSG1_ERROR("CID out of range value=%d",cid);
          result = DSAT_ERROR;
        }
    
        /* Generate response */
        if( DSAT_OK == result)
        {
          result = etsipkt_cgeqosrdp_format_response( (uint8 *)&cid, 1,
                                                       tok_ptr,
                                                       res_buff_ptr);
        }
      }
    }
  }
    /*-----------------------------------------------------------
     Processing for a command without an argument (WRITE command)
     and TEST syntax
    ------------------------------------------------------------*/
  else if( (tok_ptr->op == (NA)) ||
           (tok_ptr->op == (NA|EQ|QU)) )
  {
    result = etsipkt_cgeqosrdp_format_response( NULL, 0,
                                                tok_ptr,
                                                res_buff_ptr );
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;

}/* dsatetsipkt_exec_cgqosrdp_cmd */

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGTFTRDP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGTFTRDP command.
  This command returns a set of dynamin parameters associated with the specified
  context identifier.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatetsipkt_exec_cgtftrdp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_OK;
  
  /*---------------------------------------------------------
   Processing for a command with an argument (WRITE command)
  -----------------------------------------------------------*/
  if(tok_ptr->op == (NA|EQ|AR))
  {
    if(tok_ptr->args_found > 1)
    {
      DS_AT_MSG0_ERROR("Invalid number of Arguments in CGTFTRDP");
      result = DSAT_ERROR;
    }
    else
    {
      if( !VALID_TOKEN(0) )
      {
        /* No CID specified */
        result = etsipkt_cgtftrdp_format_response( NULL, 0,
                                                   tok_ptr,
                                                   res_buff_ptr);
      }
      else
      {
        dsat_num_item_type cid = 0 ;
    
        if( ATOI_OK  != dsatutil_atoi (&cid, tok_ptr->arg[0], 10) )
        {
          DS_AT_MSG1_ERROR("CID out of range value=%d",cid);
          result = DSAT_ERROR;
        }
    
        /* Generate response */
        if( DSAT_OK == result)
        {
          result = etsipkt_cgtftrdp_format_response( (uint8 *)&cid, 1,
                                                      tok_ptr,
                                                      res_buff_ptr);
        }
      }
    }
  }
    /*-----------------------------------------------------------
     Processing for a command without an argument (WRITE command)
     and TEST syntax
    ------------------------------------------------------------*/
  else if( (tok_ptr->op == (NA)) ||
           (tok_ptr->op == (NA|EQ|QU)) )
  {
    result = etsipkt_cgtftrdp_format_response( NULL, 0,
                                              tok_ptr,
                                              res_buff_ptr );
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;

}/* dsatetsipkt_exec_cgtftrdp_cmd */


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGQREQ_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGQREQ and +QCGQREQ command.
  This command sets PDP context parameters for GPRS QoS Profile
  (Requested).

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  Defining +CGQREQ parameters will undefine +CGEQREQ parameters;
  these commands are mutually exclusive.

===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgqreq_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type val = 0;
  ds_profile_status_etype  profile_status;
  dsat_mixed_param_val_type * val_ptr = 
    (dsat_mixed_param_val_type *) dsatutil_get_val(parse_table->cmd_id,0,0,STR_TYPE);

  ds_profile_tech_etype             tech_type = DS_PROFILE_TECH_INVALID;
  byte                              cid       = 0;

  if (tok_ptr->op == (NA|EQ|AR))
  {
    /*---------------------------------------------------------------
      Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/

    /* Validate connection ID */
    if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 0, &val,1))
    {
      DS_AT_MSG0_HIGH("Cannot convert connection ID parameter");
      return DSAT_ERROR;
    }
    cid       = (byte)val;

    if(0 == (dsatutil_strncmp_ig_sp_case((const byte *)parse_table->name,
                                         (const byte *)"+QCGQREQ" ,strlen((const char *)"+QCGQREQ"))))
    {
      //EPC check range over here
      tech_type = DS_PROFILE_TECH_EPC;
      if ((cid < 100) ||
          (TRUE != dsatetsipkt_is_profile_context_valid(cid,
                                                        tech_type,
                                                        dsat_get_current_ds_profile_subs_id())))
      {
        DS_AT_MSG2_ERROR("Error Incorrect cid:%d, tech_type:%d pair",cid, tech_type);
        return DSAT_ERROR;
      }
    }
    else 
    {
      // 3GPP profiles check for +CGTFT
      tech_type = DS_PROFILE_TECH_3GPP;
      if (TRUE != dsatetsipkt_is_profile_context_valid(cid,
                                                       tech_type,
                                                       dsat_get_current_ds_profile_subs_id()))
      {
        DS_AT_MSG2_ERROR("Error Incorrect cid:%d, tech_type:%d pair",cid, tech_type);
        return DSAT_ERROR;
      }
    }
    
    /* Test for only CID specified */
    if (1 == tok_ptr->args_found)
    {
      /* Clear the user specified context parameters */
      ETSIPKT_RESET_INVALID_PROFILE_PARAM( val ,DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_REQ_QOS,tech_type);
      if ( DS_PROFILE_REG_RESULT_SUCCESS != profile_status )
      {
        return DSAT_ERROR;
      }
    }
    else
    {
      /* Perform normal parameter processing */
      if (DSAT_OK == dsatparm_exec_param_cmd(
                                             mode,
                                             parse_table,
                                             tok_ptr,
                                             res_buff_ptr
                                            ))
      {
        /* Transfer user data to internal storage */
        ds_profile_3gpp_gprs_qos_params_type pdata;
        /* Set specified profile to defaults in GPRS */
        ETSIPKT_RESET_DEFAULT_PROFILE_PARAM( cid ,DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_REQ_QOS);
        if ( DS_PROFILE_REG_RESULT_SUCCESS != profile_status )
        {
          return DSAT_ERROR;
        }

        memset((void*)&pdata, 0, sizeof(pdata));

       /* Read using DS Profile API*/
        if(dsatetsipkt_read_write_profile_param(cid,READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_REQ_QOS,
               &pdata,sizeof(ds_profile_3gpp_gprs_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param CID : %d ",cid);
          return DSAT_ERROR;
        }
        
        /* Make conditional edits to profile */
        CONDITIONAL_ASSIGN(1, pdata.precedence,  val_ptr[1].num_item);
        CONDITIONAL_ASSIGN(2, pdata.delay,       val_ptr[2].num_item);
        CONDITIONAL_ASSIGN(3, pdata.reliability, val_ptr[3].num_item);
        CONDITIONAL_ASSIGN(4, pdata.peak,        val_ptr[4].num_item);
        CONDITIONAL_ASSIGN(5, pdata.mean,        val_ptr[5].num_item);

        /* Write to EFS */
        if(dsatetsipkt_read_write_profile_param(cid,WRITE_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_REQ_QOS,
               &pdata,sizeof(ds_profile_3gpp_gprs_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error set param : %d",cid);
          result = DSAT_ERROR;
        }
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /*---------------------------------------------------------------
      Processing for a command to query valid parameters
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgqreq_response_fmt (parse_table,
                                              &res_buff_ptr,
                                              MULTI_INDEX_TEST))
    {
      DS_AT_MSG0_HIGH("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|QU))
  {
    /*---------------------------------------------------------------
       Processing for a command to query current parameters
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgqreq_response_fmt (parse_table,
                                              &res_buff_ptr,
                                              MULTI_INDEX_READ))
    {
      DS_AT_MSG0_HIGH("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA))
  {
    /* Do nothing */
    result = DSAT_OK;
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsatetsipkt_exec_cgqreq_cmd */

/*===========================================================================

FUNCTION DSATETSIPKT_CGQREQ_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGQREQ read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsipkt_cgqreq_response_fmt
(
  const dsati_cmd_type  *parse_table,   /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format         /*  Flag for format type        */
)
{
  byte name_buff[16];
  byte i, j, cntr = 0;
  int size = 0;
  int result = 0;
  dsm_item_type * res_buff_ptr = *out_buff_ptr;
  ds_profile_3gpp_gprs_qos_params_type pdata;

  uint8             profile_list[DSAT_MAX_ALLOWED_PROFILES];
  uint8             total_profiles = 0;

  ds_profile_tech_etype tech_type = DS_PROFILE_TECH_INVALID;

  /* Format the command name */
  (void)snprintf((char*)name_buff,sizeof(name_buff),
                       "%s: ", parse_table->name);
  res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
  
  switch (format)
  {
    case MULTI_INDEX_TEST:
      /* Report each PDP type on seperate line */
      for (i=0; i <= (byte)DSAT_MAX_PDP_TYPE; i++) 
      {
        size = snprintf ((char *)res_buff_ptr->data_ptr, 
                 res_buff_ptr->size,
                 "%s%s%s\"%s\",(%d-%d),(%d-%d),(%d-%d),(%d-%d),(",
                 res_buff_ptr->data_ptr,
                 (i > 0) ? "\n" : "",
                 name_buff,
                 dsat_pdptype_valstr[i],       
                 ((dflm_type *)dsat_cgqreq_prec_dflm)->lower,
                 ((dflm_type *)dsat_cgqreq_prec_dflm)->upper,
                 ((dflm_type *)dsat_cgqreq_delay_dflm)->lower,
                 ((dflm_type *)dsat_cgqreq_delay_dflm)->upper,
                 ((dflm_type *)dsat_cgqreq_rel_dflm)->lower,
                 ((dflm_type *)dsat_cgqreq_rel_dflm)->upper,
                 ((dflm_type *)dsat_cgqreq_peak_dflm)->lower,
                 ((dflm_type *)dsat_cgqreq_peak_dflm)->upper
                );

        /* Handle disjoint mean range */
        j = 0;
        while (MAX_BROKEN_RANGE !=
                ((dflm_type *)dsat_cgqreq_mean_dflm)[j].default_v)
        {
          /* Check for single value */
          if ( ((dflm_type *)dsat_cgqreq_mean_dflm)[j].lower ==
               ((dflm_type *)dsat_cgqreq_mean_dflm)[j].upper )
          {
            size = snprintf ((char *)res_buff_ptr->data_ptr,
                            res_buff_ptr->size,
                            "%s%d,",
                            res_buff_ptr->data_ptr,
                            ((dflm_type *)dsat_cgqreq_mean_dflm)[j].lower
                           );
          }
          else
          {
            size = snprintf ((char *)res_buff_ptr->data_ptr,
                            res_buff_ptr->size,
                            "%s%d-%d,",
                            res_buff_ptr->data_ptr,
                            ((dflm_type *)dsat_cgqreq_mean_dflm)[j].lower,
                            ((dflm_type *)dsat_cgqreq_mean_dflm)[j].upper
                           );
          }
          j++;
        }
        res_buff_ptr->data_ptr[size-1] = ')';
      }
      res_buff_ptr->used = (uint16)size;
      result = size;
      break;

    case MULTI_INDEX_READ:
      if(0 == (dsatutil_strncmp_ig_sp_case((const byte *)parse_table->name,
                                           (const byte *)"+QCGQREQ" ,strlen((const char *)"+QCGQREQ"))))
      {
        tech_type = DS_PROFILE_TECH_EPC;
        if (DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_EPC, &profile_list[0],
                                                    sizeof(profile_list), &total_profiles,
                                                    dsat_get_current_ds_profile_subs_id()) )
        {
          DS_AT_MSG0_ERROR("Error getting EPC profile list!");
        }
      }
      else
      {
        // Means +CGQREQ command
        tech_type = DS_PROFILE_TECH_3GPP;
        if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                                   sizeof(profile_list), &total_profiles,
                                                   dsat_get_current_ds_profile_subs_id()) )
        {
          DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
        }
      }
      for (i=0; i < total_profiles; i++) 
      {
        memset((void*)&pdata, 0, sizeof(pdata));

        /* Read using DS Profile API*/
        if(dsatetsipkt_read_write_profile_param(profile_list[i],READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_REQ_QOS,
               &pdata,sizeof(ds_profile_3gpp_gprs_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param CID : %d ",profile_list[i]);
          continue;
        }
        
        /* Generate response from internal storage */
        size = snprintf ((char *)res_buff_ptr->data_ptr,
                 res_buff_ptr->size,
                 "%s%s%s%d,%lu,%lu,%lu,%lu,%lu",
                 res_buff_ptr->data_ptr,
                 (cntr > 0) ? "\n" : "",
                 name_buff,
                 profile_list[i],
                 pdata.precedence,       
                 pdata.delay,       
                 pdata.reliability,
                 pdata.peak,       
                 pdata.mean
                );
        cntr++;
      }
      
      /* No contexts defined, just print command name */
      if (0 == cntr)
      {
        size = snprintf((char *)res_buff_ptr->data_ptr,
                       res_buff_ptr->size,
                       "%s%s",
                       res_buff_ptr->data_ptr,
                       name_buff);
      }
      res_buff_ptr->used = (uint16)size;
      result = size;
      break;

    case MULTI_INDEX_AMPV:
      size = res_buff_ptr->used;
      res_buff_ptr->used = (word)snprintf(
                                   (char *)res_buff_ptr->data_ptr,
                                   res_buff_ptr->size,
                                   "%s%s",
                                   res_buff_ptr->data_ptr,
                                   name_buff);

      if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                                 sizeof(profile_list), &total_profiles,
                                                 dsat_get_current_ds_profile_subs_id()) )
      {
        DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
      }

      for ( i = 0; i < total_profiles; i++) 
      {
        /* Query profile from EFS */
        memset((void*)&pdata, 0, sizeof(pdata));
        
        /* Read using DS Profile API*/
        if(dsatetsipkt_read_write_profile_param(profile_list[i],READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_REQ_QOS,
               &pdata,sizeof(ds_profile_3gpp_gprs_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param CID : %d ",profile_list[i]);
          continue;
        }
        
        /* Check to see if a new DSM item is required */
        CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - 30));
         
        /* Generate response from internal storage */
        res_buff_ptr->used = (word)snprintf (
                 (char *)res_buff_ptr->data_ptr, 
                 res_buff_ptr->size,
                 "%s%s(%d,%lu,%lu,%lu,%lu,%lu)",
                 res_buff_ptr->data_ptr,
                 (cntr > 0) ? "," : "",
                 profile_list[i],
                 pdata.precedence,       
                 pdata.delay,       
                 pdata.reliability,
                 pdata.peak,       
                 pdata.mean
                );
        cntr++;
      }

      /* Return zero if DSM item changed */
      result = (res_buff_ptr == *out_buff_ptr)?
               (res_buff_ptr->used - size) : 0;
      (*out_buff_ptr)->used -= (word)result;  /* caller increments used */
      break;
      
    default:
      DS_AT_MSG0_HIGH("Unsupported multi-index format type");
  }

  return (dsat_num_item_type)result;
} /* dsatetsipkt_cgqreq_response_fmt */



/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGQMIN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGQMIN and +QCGQMIN command.
  This command sets PDP context parameters for GPRS QoS Profile
  (Minimum Acceptable).

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  Defining +CGQMIN parameters will undefine +CGEQREQ parameters;
  these commands are mutually exclusive.
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgqmin_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  dsat_num_item_type val = 0;
  dsat_mixed_param_val_type * val_ptr = 
    (dsat_mixed_param_val_type *) dsatutil_get_val(parse_table->cmd_id,0,0,STR_TYPE);
  ds_profile_status_etype  profile_status;
  ds_profile_tech_etype             tech_type = DS_PROFILE_TECH_INVALID;
  byte cid = 0;


  if (tok_ptr->op == (NA|EQ|AR))
  {
    /*---------------------------------------------------------------
      Processing for a command with an argument (WRITE command)
    ---------------------------------------------------------------*/

    /* Validate connection ID */
    if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 0, &val,1))
    {
      DS_AT_MSG0_HIGH("Cannot convert connection ID parameter");
      return DSAT_ERROR;
    }
    
    cid       = (byte)val;
    if(0 == (dsatutil_strncmp_ig_sp_case((const byte *)parse_table->name,
                                         (const byte *)"+QCGQMIN" ,strlen((const char *)"+QCGQMIN"))))
    {
      //EPC check range over here
      tech_type = DS_PROFILE_TECH_EPC;
      if ((cid < 100) ||
          (TRUE != dsatetsipkt_is_profile_context_valid(cid,
                                                        tech_type,
                                                        dsat_get_current_ds_profile_subs_id())))
      {
        DS_AT_MSG2_ERROR("Error Incorrect cid:%d, tech_type:%d pair",cid, tech_type);
        return DSAT_ERROR;
      }
    }
    else 
    {
      // 3GPP profiles check for +CGTFT
      tech_type = DS_PROFILE_TECH_3GPP;
      if (TRUE != dsatetsipkt_is_profile_context_valid(cid,
                                                       tech_type,
                                                       dsat_get_current_ds_profile_subs_id()))
      {
        DS_AT_MSG2_ERROR("Error Incorrect cid:%d, tech_type:%d pair",cid, tech_type);
        return DSAT_ERROR;
      }
    }
    
    /* Test for only CID specified */
    if (1 == tok_ptr->args_found)
    {
      /* Clear the user specified context parameters */
      ETSIPKT_RESET_INVALID_PROFILE_PARAM(val,DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_MIN_QOS,tech_type);
      if ( DS_PROFILE_REG_RESULT_SUCCESS != profile_status )
      {
        return DSAT_ERROR;
      }
    }
    else
    {
      /* Perform normal parameter processing */
      if (DSAT_OK == dsatparm_exec_param_cmd(
                                             mode,
                                             parse_table,
                                             tok_ptr,
                                             res_buff_ptr
                                            ))
      {
        /* Transfer user data to internal storage */
        ds_profile_3gpp_gprs_qos_params_type pdata;
          
        /* Set specified profile to defaults */
        ETSIPKT_RESET_DEFAULT_PROFILE_PARAM(val,DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_MIN_QOS);
        if ( DS_PROFILE_REG_RESULT_SUCCESS != profile_status )
        {
          return DSAT_ERROR;
        }

        /* Query default profile from EFS */
        memset((void*)&pdata, 0, sizeof(pdata));

        if(dsatetsipkt_read_write_profile_param(cid,READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_MIN_QOS,
               &pdata,sizeof(ds_profile_3gpp_gprs_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param : %d",cid);
          return DSAT_ERROR;
        }
        
        /* Make conditional edits to profile */
        CONDITIONAL_ASSIGN(1, pdata.precedence,  val_ptr[1].num_item);
        CONDITIONAL_ASSIGN(2, pdata.delay,       val_ptr[2].num_item);
        CONDITIONAL_ASSIGN(3, pdata.reliability, val_ptr[3].num_item);
        CONDITIONAL_ASSIGN(4, pdata.peak,        val_ptr[4].num_item);
        CONDITIONAL_ASSIGN(5, pdata.mean,        val_ptr[5].num_item);

        /* Write to EFS */
        if(dsatetsipkt_read_write_profile_param(cid,WRITE_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_MIN_QOS,
               &pdata,sizeof(ds_profile_3gpp_gprs_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param : %d",cid);
          return DSAT_ERROR;
        }

      }
      else
      {
        result = DSAT_ERROR;
      }
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /*---------------------------------------------------------------
      Processing for a command to query valid parameters
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgqmin_response_fmt (parse_table,
                                              &res_buff_ptr,
                                              MULTI_INDEX_TEST))
    {
      DS_AT_MSG0_HIGH("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|QU))
  {
    /*---------------------------------------------------------------
       Processing for a command to query current parameters
    ---------------------------------------------------------------*/
    if (0 == dsatetsipkt_cgqmin_response_fmt (parse_table,
                                              &res_buff_ptr,
                                              MULTI_INDEX_READ))
    {
      DS_AT_MSG0_HIGH("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA))
  {
    /* Do nothing */
  }
  else
  {
    result = DSAT_ERROR;
  }
  return result;
} /* dsatetsipkt_exec_cgqmin_cmd */



/*===========================================================================

FUNCTION DSATETSIPKT_CGQMIN_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGQMIN read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsipkt_cgqmin_response_fmt
(
  const dsati_cmd_type  *parse_table,   /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format         /*  Flag for format type        */
)
{
  byte name_buff[16];
  byte i, j, cntr = 0;
  int size = 0;
  int result = 0;
  dsm_item_type * res_buff_ptr = *out_buff_ptr;
  ds_profile_3gpp_gprs_qos_params_type pdata;

  uint8             profile_list[DSAT_MAX_ALLOWED_PROFILES];
  uint8             total_profiles = 0;
  ds_profile_tech_etype tech_type = DS_PROFILE_TECH_INVALID;

  /* Format the command name */
  (void)snprintf((char*)name_buff,
                                  sizeof(name_buff),
                                  "%s: ",
                                  parse_table->name
                                  );
  res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
  
  switch (format)
  {
    case MULTI_INDEX_TEST:
      /* Report each PDP type on seperate line */
      for (i=0; i <= (byte)DSAT_MAX_PDP_TYPE; i++) 
      {
        size = snprintf ((char *)res_buff_ptr->data_ptr, 
                 res_buff_ptr->size,
                 "%s%s%s\"%s\",(%d-%d),(%d-%d),(%d-%d),(%d-%d),(",
                 res_buff_ptr->data_ptr,
                 (i > 0) ? "\n" : "",
                 name_buff,
                 dsat_pdptype_valstr[i],       
                 ((dflm_type *)dsat_cgqmin_prec_dflm)->lower,
                 ((dflm_type *)dsat_cgqmin_prec_dflm)->upper,
                 ((dflm_type *)dsat_cgqmin_delay_dflm)->lower,
                 ((dflm_type *)dsat_cgqmin_delay_dflm)->upper,
                 ((dflm_type *)dsat_cgqmin_rel_dflm)->lower,
                 ((dflm_type *)dsat_cgqmin_rel_dflm)->upper,
                 ((dflm_type *)dsat_cgqmin_peak_dflm)->lower,
                 ((dflm_type *)dsat_cgqmin_peak_dflm)->upper
                );

        /* Handle disjoint mean range */
        j = 0;
        while (MAX_BROKEN_RANGE !=
                ((dflm_type *)dsat_cgqmin_mean_dflm)[j].default_v)
        {
          /* Check for single value */
          if ( ((dflm_type *)dsat_cgqmin_mean_dflm)[j].lower ==
               ((dflm_type *)dsat_cgqmin_mean_dflm)[j].upper )
          {
            size = snprintf ((char *)res_buff_ptr->data_ptr, 
                            res_buff_ptr->size,
                            "%s%d,",
                            res_buff_ptr->data_ptr,
                            ((dflm_type *)dsat_cgqmin_mean_dflm)[j].lower
                           );
          }
          else
          {
            size = snprintf ((char *)res_buff_ptr->data_ptr, 
                            res_buff_ptr->size,
                            "%s%d-%d,",
                            res_buff_ptr->data_ptr,
                            ((dflm_type *)dsat_cgqmin_mean_dflm)[j].lower,
                            ((dflm_type *)dsat_cgqmin_mean_dflm)[j].upper
                           );
          }
          j++;
        }
        res_buff_ptr->data_ptr[size-1] = ')';
      }
      res_buff_ptr->used = (uint16)size;
      result = size;
      break;

    case MULTI_INDEX_READ:
      /* Report each context on seperate line */
      if(0 == (dsatutil_strncmp_ig_sp_case((const byte *)parse_table->name,
                                           (const byte *)"+QCGQMIN" ,strlen((const char *)"+QCGQMIN"))))
      {
        tech_type = DS_PROFILE_TECH_EPC;
        if (DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_EPC, &profile_list[0],
                                                    sizeof(profile_list), &total_profiles,
                                                    dsat_get_current_ds_profile_subs_id()) )
        {
          DS_AT_MSG0_ERROR("Error getting EPC profile list!");
        }
      }
      else
      {
        // Means +CGQMIN command
        tech_type = DS_PROFILE_TECH_3GPP;
        if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                                   sizeof(profile_list), &total_profiles,
                                                   dsat_get_current_ds_profile_subs_id()) )
        {
          DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
        }
      }

      for (i=0; i < total_profiles; i++) 
      {
        /* Query profile from EFS */
        memset((void*)&pdata, 0, sizeof(pdata));

        if(dsatetsipkt_read_write_profile_param(profile_list[i],READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_MIN_QOS,
               &pdata,sizeof(ds_profile_3gpp_gprs_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param : %d",profile_list[i]);
          continue;
        }
        
        /* Generate response from internal storage */
        size = snprintf ((char *)res_buff_ptr->data_ptr, 
                 res_buff_ptr->size,
                 "%s%s%s%d,%lu,%lu,%lu,%lu,%lu",
                 res_buff_ptr->data_ptr,
                 (cntr > 0) ? "\n" : "",
                 name_buff,
                 profile_list[i],
                 pdata.precedence,       
                 pdata.delay,       
                 pdata.reliability,
                 pdata.peak,       
                 pdata.mean
                );
        cntr++;
      }
      
      /* No contexts defined, just print command name */
      if (0 == cntr)
      {
        size = snprintf((char *)res_buff_ptr->data_ptr,
                       res_buff_ptr->size,
                       "%s%s",
                       res_buff_ptr->data_ptr,
                       name_buff);
      }
      res_buff_ptr->used = (uint16)size;
      result = size;
      break;

    case MULTI_INDEX_AMPV:
      size = res_buff_ptr->used;
      res_buff_ptr->used = (word)snprintf(
                                   (char *)res_buff_ptr->data_ptr,
                                   res_buff_ptr->size,
                                   "%s%s",
                                   res_buff_ptr->data_ptr,
                                   name_buff);

      if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                                 sizeof(profile_list), &total_profiles,
                                                 dsat_get_current_ds_profile_subs_id()) )
      {
        DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
      }

      for ( i = 0; i < total_profiles; i++) 
      {
        /* Query profile from EFS */
        memset((void*)&pdata, 0, sizeof(pdata));
        
        if(dsatetsipkt_read_write_profile_param(profile_list[i],READ_COMMAND,
               DS_PROFILE_3GPP_PROFILE_PARAM_GPRS_MIN_QOS,
               &pdata,sizeof(ds_profile_3gpp_gprs_qos_params_type)) == DSAT_ERROR)
        {
          DS_AT_MSG1_ERROR("Error get param : %d",profile_list[i]);
          continue;
        }
        /* Check to see if a new DSM item is required */
        CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - 30));
         
        /* Generate response from internal storage */
        res_buff_ptr->used = (word)snprintf (
                 (char *)res_buff_ptr->data_ptr,
                 res_buff_ptr->size,
                 "%s%s(%d,%lu,%lu,%lu,%lu,%lu)",
                 res_buff_ptr->data_ptr,
                 (cntr > 0) ? "," : "",
                 profile_list[i],
                 pdata.precedence,       
                 pdata.delay,       
                 pdata.reliability,
                 pdata.peak,       
                 pdata.mean
                );
        cntr++;
      }

      /* Return zero if DSM item changed */
      result = (res_buff_ptr == *out_buff_ptr)?
               (res_buff_ptr->used - size) : 0;
      (*out_buff_ptr)->used -= (word)result;  /* caller increments used */
      break;
      
    default:
      DS_AT_MSG0_HIGH("Unsupported multi-index format type");
  }

  return (dsat_num_item_type)result;
} /* dsatetsipkt_cgqmin_response_fmt */



/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_QCPDPP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the $QCDPDP command.
  This command sets PDP authentication parameters.

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
dsat_result_enum_type dsatetsipkt_exec_qcpdpp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  #define MAX_QCPDPP_ENTRY_LEN (DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN + 20)
  dsat_result_enum_type result = DSAT_OK;
  dsm_item_type *buff_ptr = res_buff_ptr;
  byte name_buff[16];
  dsat_num_item_type val = 0;
  byte i, cntr = 0;
  byte cid = 0;
  dsat_mixed_param_val_type * val_ptr = 
    (dsat_mixed_param_val_type *)dsatutil_get_val(parse_table->cmd_id,0,0,STR_TYPE);
  ds_profile_3gpp_pdp_auth_type pdata;
  dsat_error_info_s_type    err_info;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;

  memset(&pdata, 0x00, sizeof(ds_profile_3gpp_pdp_auth_type));
  if (tok_ptr->op == (NA|EQ|AR))
  {
    /*---------------------------------------------------------------
      Processing for a WRITE command 
      ---------------------------------------------------------------*/

    /* Validate connection ID */
    if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 0, &val,1))
    {
      err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
      err_info.arg_num = 0;
      goto send_error;
    }
    
    /* Test for only CID specified */
    if (1 == tok_ptr->args_found)
    {
      /* Clear the user specified context parameters */
      if(DSAT_ERROR == dsatetsipkt_read_write_param_table(auth_param_tbl,sizeof(auth_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
              val,RESET_COMMAND,NULL,0,dsat_get_current_ds_profile_subs_id()))
      {
        return DSAT_ERROR;
      }
    }
    else
    {
      byte user_buf[DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1];
      byte pass_buf[DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1];
      cid = (byte)val;
      
      /* Validate auth type and parameters */
      if (DSAT_OK != etsipkt_process_param(parse_table, tok_ptr, 1, &val,1))
      {
        err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
        err_info.arg_num = 1;
        goto send_error;
      }

      memset((void*)user_buf,0,sizeof(user_buf));
      memset((void*)pass_buf,0,sizeof(pass_buf));

      /* Validate authentication strings */
      /* For a valid AUTH, need username and password */
      if ( ((byte)DS_PROFILE_3GPP_AUTH_PREF_PAP_CHAP_NOT_ALLOWED != val) &&
           ((byte)DS_PROFILE_3GPP_AUTH_PREF_MAX != val) )
      {
        if (VALID_TOKEN(2) && (DSAT_OK !=
            etsipkt_process_param(parse_table, tok_ptr, 2,
                             pass_buf,sizeof(pass_buf))))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 2;
          goto send_error;
        }

        if (VALID_TOKEN(3) && (DSAT_OK !=
            etsipkt_process_param(parse_table, tok_ptr, 3, 
                            user_buf,sizeof(user_buf))))
        {
          err_info.errval = DSAT_ERR_INVALID_ARGUMENT;
          err_info.arg_num = 3;
          goto send_error;
        }
        if (('\0' == pass_buf[0]) || ('\0' == user_buf[0]))
        {
          DS_AT_MSG0_HIGH("PAP auth requires username and password");
          return DSAT_ERROR;
        }
      }
      /* For NONE, no parameters allowed */
      else if ((byte)DS_PROFILE_3GPP_AUTH_PREF_PAP_CHAP_NOT_ALLOWED == val)
      {
        if (2 < tok_ptr->args_found)
        {
          DS_AT_MSG0_HIGH("NO auth rejects username/password");
          return DSAT_ERROR;
        }
      }
      
      /* Validate username/password string length */
      if ((DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN < strlen((const char *)pass_buf)) ||
          (DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN < strlen((const char *)user_buf)))
      {
        DS_AT_MSG1_HIGH("Username or password exceeds %d characters",
                 DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN);
        return DSAT_ERROR;
      }

      /* Perform normal parameter processing */
      if (DSAT_OK == dsatparm_exec_param_cmd(
                                             mode,
                                             parse_table,
                                             tok_ptr,
                                             res_buff_ptr
                                            ))
      {
        /* Transfer user data to internal storage */

        /* Set specified profile to defaults */
        if(FALSE == dsatetsipkt_is_profile_context_valid(cid, DS_PROFILE_TECH_3GPP,
                                                         dsat_get_current_ds_profile_subs_id()))
        {
          DS_AT_MSG1_ERROR("Profile %d is not valid",cid);
          return DSAT_ERROR;
        }
      
        CONDITIONAL_ASSIGN(1, pdata.auth_type,(ds_profile_3gpp_auth_pref_type)val_ptr[1].num_item);
        
        /* Preserve password for valid authentication */
        if (VALID_TOKEN(2))
        {
          if ( (DS_PROFILE_3GPP_AUTH_PREF_PAP_CHAP_NOT_ALLOWED != pdata.auth_type) &&
               (DS_PROFILE_3GPP_AUTH_PREF_MAX != pdata.auth_type) )
          {
            (void)strlcpy((char *)pdata.password,
                              (const char *)pass_buf,
                              DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1);
         
          
            (void)strlcpy((char *)val_ptr[2].string_item,
                              (const char *)pass_buf,
                              DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1);
           }
        }
        
        /* Preserve username for valid authentication */
        if (VALID_TOKEN(3))
        {
          if ( (DS_PROFILE_3GPP_AUTH_PREF_PAP_CHAP_NOT_ALLOWED != pdata.auth_type) &&
               (DS_PROFILE_3GPP_AUTH_PREF_MAX != pdata.auth_type) )
          {
            (void)strlcpy((char *)pdata.username, 
                          (const char *)user_buf,
                          DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1);
                     
            (void)strlcpy((char *)val_ptr[3].string_item,
                          (const char *)user_buf,
                          DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1);
           }
        }

        /* Write to EFS */
        if(DSAT_ERROR == dsatetsipkt_read_write_param_table(auth_param_tbl,sizeof(auth_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
               cid,WRITE_COMMAND,(byte*)&pdata,sizeof(pdata),dsat_get_current_ds_profile_subs_id()))
        {
          DS_AT_MSG1_HIGH("Error QCPDPP PDP profile write: %d",cid);
          return DSAT_ERROR;
        }
      }
      else
      {
        result = DSAT_ERROR;
      }
    }
  }
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    /*---------------------------------------------------------------
      Processing for a command to TEST command
      ---------------------------------------------------------------*/
    if (DSAT_OK != dsatparm_exec_param_cmd(
                                           mode,
                                           parse_table,
                                           tok_ptr,
                                           res_buff_ptr
                                          ))
    {
      result = DSAT_ERROR;
    }
  }
  else if (tok_ptr->op == (NA|QU))
  {
    uint8     profile_list[DSAT_MAX_ALLOWED_PROFILES] = {0};
    uint8     total_profiles = 0;
    /*---------------------------------------------------------------
      Processing for a READ command 
      ---------------------------------------------------------------*/
    
    /* Format the command name */
    (void)snprintf((char*)name_buff,
                         sizeof(name_buff),
                         "%s: ",
                         parse_table->name
                         );
    buff_ptr->data_ptr[buff_ptr->used] = '\0';
    
    /* Report each profile on seperate line */
    if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                               sizeof(profile_list), &total_profiles,
                                               dsat_get_current_ds_profile_subs_id()) )
    {
      DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
    }
    if (total_profiles == 0)
    {
      buff_ptr->used =
        (word)snprintf ((char *)buff_ptr->data_ptr, 
                       buff_ptr->size,
                       "%s%s",
                       buff_ptr->data_ptr,
                       name_buff
                       );
    }
    else
    {
      for ( i = 0; i < total_profiles; i++) 
      {
        memset((void*)&pdata, 0, sizeof(pdata));
        if(DSAT_ERROR == dsatetsipkt_read_write_param_table(auth_param_tbl,sizeof(auth_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
               profile_list[i],READ_COMMAND,(byte*)&pdata,sizeof(pdata),dsat_get_current_ds_profile_subs_id()))
        {
          DS_AT_MSG1_HIGH("Error QCPDPP PDP profile write: %d",profile_list[i]);
          continue;
        }
        if (NULL == (buff_ptr = etsipkt_check_response_buffer(
                                  buff_ptr,
                                  DSM_BUFFER_THRESHOLD - MAX_QCPDPP_ENTRY_LEN)))
        { 
          return DSAT_ERROR; 
        }
  
        /* Generate response from internal storage (excluding password) */
        buff_ptr->used =
          (word)snprintf ((char *)buff_ptr->data_ptr, 
                         buff_ptr->size,
                         "%s%s%s%d,%d",
                         buff_ptr->data_ptr,
                         (cntr > 0) ? "\n" : "",
                         name_buff,
                         profile_list[i],
                         pdata.auth_type
                         );
        
        /* Display username for valid auth type */
        if ( (DS_PROFILE_3GPP_AUTH_PREF_PAP_CHAP_NOT_ALLOWED != pdata.auth_type) &&
             (DS_PROFILE_3GPP_AUTH_PREF_MAX != pdata.auth_type) )
        {
          buff_ptr->used =
            (word)snprintf ((char *)buff_ptr->data_ptr, 
                           buff_ptr->size,
                           "%s,\"%s\"",
                           buff_ptr->data_ptr,
                           pdata.username
                           );
        }
        cntr++;
      }
    }
  }
  else if (tok_ptr->op == (NA))
  {
    /* Do nothing */
    result = DSAT_OK;
  }
  else
  {
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }

  return result;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_SYNTAX)
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;
} /* dsatetsipkt_exec_qcpdpp_cmd */


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_QCPDPLT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcpdplt command storing write value to NV.
  $qcpdplt value is used to enable/disable tolerance to long delays in PDP 
  call setup.

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
dsat_result_enum_type dsatetsipkt_exec_qcpdplt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result;
  dsat_num_item_type    temp_val; 

  char rbuffer[32];
  int ctr = 0;

  result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );
  
  if (DSAT_OK == result) 
  {
    /*--------------------------------------
    Processing for a WRITE command:
    ----------------------------------------*/
    if (tok_ptr->op == (NA|EQ|AR) || tok_ptr->op == (NA))
    {
      /*---------------------------------------------------------------
      Write item into the NV 
      ---------------------------------------------------------------*/
      temp_val = (dsat_num_item_type)dsatutil_get_val(DSAT_VENDOR_QCPDPLT_IDX,0,0,NUM_TYPE) ;
      ds_nv_item.long_pdpact_tolerance_for_te  = (boolean)temp_val;
      (void)dsatutil_put_nv_item( NV_LONG_PDPACT_TOLERANCE_FOR_TE_I,
                                  &ds_nv_item );
      result = DSAT_OK;
    }
    /*--------------------------------------
    Processing for a READ command:
    ----------------------------------------*/
    else if (tok_ptr->op == (NA|QU))
    {
      /* Use extended format on value query */
      ctr = snprintf ((char*)rbuffer,sizeof(rbuffer),"%s: %d", 
                            (char*)parse_table->name, 
                            (int)dsatutil_get_val(
                            DSAT_VENDOR_QCPDPLT_IDX,0,0,NUM_TYPE));

      (void) strlcpy((char*)res_buff_ptr->data_ptr,
                         (const char*)rbuffer,
                         res_buff_ptr->size);
      res_buff_ptr->used += (unsigned short)ctr;
          
      result = DSAT_OK;
    }
  }

  return result;
}  /* dsatvend_exec_qcpdplt_cmd() */


/*===========================================================================
FUNCTION DSATETSIPKT_RESET_TOLERANCE

DESCRIPTION
  This function resets pdp call setup delay tolerance to a default value of 
  QCPDPLT_DEF_VAL and stores the same in NV.
    
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsipkt_reset_tolerance
(
  void
)
{
  /*---------------------------------------------------------------
  Set item value and write into the NV 
  ---------------------------------------------------------------*/
  DSATUTIL_SET_VAL(DSAT_VENDOR_QCPDPLT_IDX,0,0,0,QCPDPLT_DEF_VAL,NUM_TYPE)
  ds_nv_item.long_pdpact_tolerance_for_te = (boolean)QCPDPLT_DEF_VAL;
  (void)dsatutil_put_nv_item( NV_LONG_PDPACT_TOLERANCE_FOR_TE_I,
                              &ds_nv_item );
} /* dsatetsipkt_reset_tolerance( ) */

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CNMPSD_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CNMPSD command which releases PS signaling. This command
  indicates that no application on the MT is expected to exchange data.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if the command is read or write OR when the default command
    execution fails.
    DSAT_OK : if it is a query command 
    DSAT_ASYNC_CMD :  successful execution of CM API cm_mm_call_cmd_ps_sig_release.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsipkt_exec_cnmpsd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  if( (DSAT_MODE_WCDMA != dsatcmdp_get_current_mode()) && 
      (DSAT_MODE_TDS != dsatcmdp_get_current_mode()) )
  {
    result = dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
  }
  else
  {
     if (tok_ptr->op == (NA))
     {
       SET_PENDING(DSATETSI_EXT_ACT_CNMPSD_ETSI_IDX ,0, DSAT_PENDING_TRUE);
       if ( FALSE == cm_mm_call_cmd_ps_sig_release ( dsatcmif_call_cmd_cb_func,
                                                     NULL,
                                                     dsatcm_client_id
                                                   ) )
       {
         SET_PENDING(DSATETSI_EXT_ACT_CNMPSD_ETSI_IDX ,0, DSAT_PENDING_FALSE);
         result = DSAT_ERROR; 
       }
       else
       {
         result = DSAT_ASYNC_CMD;
       }
     }
     else if (tok_ptr->op == (NA|EQ|QU))
     {
       result = DSAT_OK;
     }
     else
     {
       result = DSAT_ERROR;
     }
  }

  return result;
} /* dsatetsipkt_exec_cnmpsd_cmd */

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGATT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGATT command.
  This command reports the connection status for network packet
  domain service.
  
DEPENDENCIES
  This function only supports read & test commands at this time.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsipkt_exec_cgatt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  /*---------------------------------------------------------
    Processing for a READ & TEST command
  -----------------------------------------------------------*/
  if ((tok_ptr->op == (NA|QU)) ||
      (tok_ptr->op == (NA|EQ|QU)))
  {
    if (DSAT_OK != dsatparm_exec_param_cmd(
                                           mode,
                                           parse_table,
                                           tok_ptr,
                                           res_buff_ptr
                                          ))
    {
      result = DSAT_ERROR;
    }
  }
#ifdef FEATURE_ETSI_ATTACH
  /*---------------------------------------------------------
    Processing for a WRITE command
  -----------------------------------------------------------*/
  else if ( tok_ptr->op == (NA|EQ|AR) )
  {
    dsat_num_item_type state;
	
    /* syntax check: number of arguments should be one! */
    if (tok_ptr->args_found > 1)
    {
      return DSAT_ERROR;
    }
	
    if (ATOI_OK != dsatutil_atoi(&state, tok_ptr->arg[0], 10))
    {
      DS_AT_MSG0_HIGH("+CGATT: cannot convert state parameter");
      return DSAT_ERROR;
    }
	
    /* Attach requested */
    if ( state == 1 )
    {
      /* Ensure detached currently */ 
      if ( 0 == (dsat_num_item_type)dsatutil_get_val(
                                DSATETSI_EXT_ACT_CGATT_ETSI_IDX,subs_id,0,NUM_TYPE) )
      {
        return dsatetsicmif_attach_ps ();
      }
      else
      {
        DS_AT_MSG0_HIGH("Already packet domain attached");
      }
    }
    /* Detach requested */
    else if ( state == 0 )
    {
      /* If detached currently... */ 
      if ( 1 == (dsat_num_item_type)dsatutil_get_val(
                                  DSATETSI_EXT_ACT_CGATT_ETSI_IDX,subs_id,0,NUM_TYPE))
      {
        return dsatetsicmif_detach_ps ();
      }
      else
      {
        /* Even when already detached call CMPH function indicating user 
           preference CM_SRV_DOMAIN_PREF_PS_DETACH to local stack but 
           terminate processing on CMPH event. */
        DS_AT_MSG0_HIGH("Already packet domain detached");
        dsatcmif_ss_evt_not_expected = TRUE;

        if ( (result = dsatetsicmif_detach_ps ()) == DSAT_ERROR )
        {
          dsatcmif_ss_evt_not_expected = FALSE;
        }

        return result;
      }
    }
    else
    {
      /* Invalid value */
      return DSAT_ERROR;
    }
  }
#endif /* FEATURE_ETSI_ATTACH */
  else 
  {
    /* unsupported operation */
    result = DSAT_ERROR;
  }
  return result;
} /* dsatetsipkt_exec_cgatt_cmd */

/*===========================================================================

FUNCTION DSATETSIPKT_CGACT_PROC_CMD

DESCRIPTION
  This function does the CGACT processing for all the requested profile id's.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.
    DSAT_ASYNC_CMD : If the response is asynchronous.
    
SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_cgact_proc_cmd( void )
{
  dsat_result_enum_type result = DSAT_OK;
  
  /* 
     Loop over the 'dsat_pdp_cid' to activate all the entries with VALID CID.
     Primary profile entries exist from the begin of this array to the middle while 
     secondary profiles exist from end to the middle of this array !!
  */
  for( ;(dsat_pdp_state.change_index < dsat_pdp_state.valid_profile_count); )
  {
    if(dsat_pdp_cid[dsat_pdp_state.change_index] == INVALID_CID)
    {
      dsat_pdp_state.change_index++;
      continue;
    }

    dsat_pdp_state.pending = TRUE;
    result = ds_3gpp_rmsm_atif_cgact_handler(
           dsat_pdp_cid[dsat_pdp_state.change_index],
           (ds_3gpp_atif_cgact_act_mode_e_type)dsat_pdp_state.demand_state,
            NULL,
            dsat_get_current_subs_id(FALSE));
    
    if(result == (dsat_result_enum_type)DSAT_INVALID_PROFILE)
    {
      dsat_pdp_state.change_index++;
      continue;
    }
    if(result == DSAT_OK)
    {
      dsat_pdp_state.pending = FALSE;
      dsat_pdp_state.change_index++;
    }
    else if(result == DSAT_ERROR)
    {
      DS_AT_MSG1_ERROR("PDP request failed for CID = %d",
        dsat_pdp_cid[dsat_pdp_state.change_index]);
      break;
    }
    else if(result == DSAT_ASYNC_CMD)
    {
      DS_AT_MSG1_HIGH("PDP request for CID %d sent to Mode handler:",
        dsat_pdp_cid[dsat_pdp_state.change_index]);
      return DSAT_ASYNC_CMD;
    }
    else
    {
      DS_AT_MSG1_ERROR("PDP request failed result code is %d ",result);
      ASSERT(0);
    }
  }
  /*CGATT=0, No Profile present*/
  if(result == (dsat_result_enum_type)DSAT_INVALID_PROFILE)
  {
    result = DSAT_OK;
  }
  memset(&dsat_pdp_cid[0],0,sizeof(dsat_pdp_cid));
  memset((void *)&dsat_pdp_state,0,sizeof(dsat_pdp_state));
  return result;
} /* dsatetsipkt_cgact_proc_cmd */

/*===========================================================================
FUNCTION DSATETSIPKT_AT_CMD_HANDLER

DESCRIPTION
  This function is handler function for reporting +CGACT,+CGCMOD command response.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
======================================================================*/
/*ARGSUSED*/
dsat_result_enum_type dsatetsipkt_at_cmd_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
)
{
  
  dsat_result_enum_type result   = DSAT_ERROR;
  ds_at_pdp_cmd_type *pdp_info_ptr = dsat_get_cmd_payload_ptr(cmd_ptr);

  if(!dsatcmdp_is_etsi_cmd())
  {
    return DSAT_ASYNC_EVENT;
  }

  DS_AT_MSG2_HIGH("Response for %d = %d",pdp_info_ptr->dsat_rmsm_cmd,
                  pdp_info_ptr->response);
  
  if(pdp_info_ptr->response == DSAT_OK)
  {
    result = pdp_info_ptr->response;
  }
  else
  {
    DS_AT_MSG3_HIGH("Reason_type %d, %d net_down, %d ext_info",pdp_info_ptr->reason_type,
                       pdp_info_ptr->iface_net_down,pdp_info_ptr->ext_info_code);

    if(pdp_info_ptr->reason_type == DSAT_RMSM_IFACE_NET_DOWN_REASON)
    {
      
      switch(pdp_info_ptr->iface_net_down)
      {
        case PS_NET_DOWN_REASON_OPTION_NOT_SUPPORTED:
          result =  dsat_send_cme_error(DSAT_CME_SERV_OP_NOT_SUPPORTED);
          break;
        case PS_NET_DOWN_REASON_OPTION_TEMP_OOO:
          result = dsat_send_cme_error(DSAT_CME_SERV_OP_TEMP_OUT_OF_ORDER);
          break;
        case PS_NET_DOWN_REASON_OPTION_UNSUBSCRIBED:
          result = dsat_send_cme_error(DSAT_CME_REQ_SERV_OP_NOT_SUBSCRIBED);
          break;
        case PS_NET_DOWN_REASON_AUTH_FAILED:
          result = dsat_send_cme_error(DSAT_CME_PDP_AUTH_FAILURE);
          break;
        case PS_NET_DOWN_REASON_NO_SRV:
        case PS_NET_DOWN_REASON_NO_GW_SRV:
           result = dsat_send_cme_error(DSAT_CME_NO_NETWORK_SERVICE);
           break;
        default:
          result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
        
      }
    }
    else
    {
      result = dsat_send_cme_error(DSAT_CME_UNKNOWN);
    }
  }

  return result;
}/* dsatetsipkt_at_cmd_handler */

#ifdef FEATURE_DSAT_ETSI_DATA
/*===========================================================================
FUNCTION DSATETSIPKT_CGACT_DONE_HANDLER

DESCRIPTION
  This function is called when +CGACT processing for a profile 
  is complete.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
======================================================================*/
void dsatetsipkt_cgact_done_handler
(
  dsat_rmsm_info_s_type  dsat_rmsm_info,
  void* user_info_ptr
)
{
  ds_cmd_type           *cmd_buf;
  ds_at_pdp_cmd_type    *pdp_info_ptr = NULL;
  dsat_result_enum_type  response  = dsat_rmsm_info.result;

  DS_AT_MSG1_HIGH("CGACT done handler, result = %d",response);
  if(dsat_pdp_state.pending == TRUE)
  {
    dsat_pdp_state.pending = FALSE;
    if(response == DSAT_ERROR)
    {
      memset(&dsat_pdp_cid[0],0,sizeof(dsat_pdp_cid));
      memset((void*)&dsat_pdp_state,0,sizeof(dsat_pdp_state));
    }
    else if(response == DSAT_OK)
    {
      dsat_pdp_state.change_index++;
      response = dsatetsipkt_cgact_proc_cmd();
      if(response == DSAT_ASYNC_CMD)
      {
        return;
      }
      dsat_reg_atz_cb (dsatetsicmif_context_activation_atz_cb);
    }
    else
    {
      DS_AT_MSG0_ERROR("No pending command, ignore");
      return;
    }

    cmd_buf = dsat_get_cmd_buf(sizeof(ds_at_pdp_cmd_type), FALSE);
    pdp_info_ptr = cmd_buf->cmd_payload_ptr;

    /* send the message to ATCOP */
    cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_PDP_CMD;

    pdp_info_ptr->response         = response;
    pdp_info_ptr->reason_type      = dsat_rmsm_info.reason_type;
    pdp_info_ptr->iface_net_down   = dsat_rmsm_info.reason.iface_net_down;
    pdp_info_ptr->ext_info_code    = dsat_rmsm_info.reason.ext_info_code;
    pdp_info_ptr->dsat_rmsm_cmd    = DSAT_RMSM_CGACT_CMD;
    ds_put_cmd(cmd_buf);
  }
  else
  {
    DS_AT_MSG0_ERROR("No pending +CGACT command");
  }
} /* dsatetsipkt_cgact_done_handler */

/*===========================================================================
FUNCTION DSATETSIPKT_CGCMOD_DONE_HANDLER

DESCRIPTION
  This function is called when +CGCMOD processing for a profile 
  is complete.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
======================================================================*/
/*ARGSUSED*/
void dsatetsipkt_cgcmod_done_handler
(
  dsat_result_enum_type result,
  void* user_info_ptr
)
{
  ds_cmd_type * cmd_buf;
  dsat_result_enum_type response = result;
  ds_at_pdp_cmd_type    *pdp_info_ptr = NULL;

  DS_AT_MSG1_HIGH("CGCMOD done handler, result = %d",response);
  if(dsat_pdp_state.pending == TRUE)
  {
    dsat_pdp_state.pending = FALSE;
    if(response == DSAT_ERROR)
    {
      memset(&dsat_pdp_cid[0],0,sizeof(dsat_pdp_cid));
      memset((void*)&dsat_pdp_state,0,sizeof(dsat_pdp_state));
    }
    else if(response == DSAT_OK)
    {
      dsat_pdp_state.change_index++;
      if((dsat_pdp_state.change_index < dsat_pdp_state.valid_profile_count)&&
         (dsat_pdp_cid[dsat_pdp_state.change_index] != INVALID_CID))
      {
        response = ds_3gpp_rmsm_atif_cgcmod_handler(
               dsat_pdp_cid[dsat_pdp_state.change_index],
               dsat_get_current_subs_id(FALSE),
                NULL);
        if(response == DSAT_ASYNC_CMD)
        {
          dsat_pdp_state.pending = TRUE;
          return;
        }
      }
    }
    else
    {
      DS_AT_MSG0_ERROR("No pending command, ignore");
      return;
    }

    cmd_buf = dsat_get_cmd_buf(sizeof(ds_at_pdp_cmd_type), FALSE);
    pdp_info_ptr = cmd_buf->cmd_payload_ptr;
    /* send the message to ATCOP */
    cmd_buf->hdr.cmd_id = DS_CMD_ATCOP_PDP_CMD;
    pdp_info_ptr->response = response;
    pdp_info_ptr->dsat_rmsm_cmd = DSAT_RMSM_CGCMOD_CMD;
    ds_put_cmd(cmd_buf);
  }
  else
  {
    DS_AT_MSG0_ERROR("Invalid Response");
  }
} /* dsatetsipkt_cgcmod_done_handler */
#endif /* FEATURE_DSAT_ETSI_DATA */

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGACT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGACT command.
  This command activates or deactivates PDP context profiles.
  
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
dsat_result_enum_type dsatetsipkt_exec_cgact_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  uint8 loop;
  
  /*---------------------------------------------------------
    Processing for a READ command
  -----------------------------------------------------------*/
  if ( tok_ptr->op == (NA|QU) )
  {
      ds_3gpp_atif_profile_status_info_type profile_status;
      uint16 cntr = 0;
      uint32 length = 0;

      memset(&profile_status,0,sizeof(profile_status));

      if(DSAT_OK != ds_3gpp_rmsm_atif_cgact_query(&profile_status,
                 dsat_get_current_subs_id(FALSE)))
      {
        DS_AT_MSG0_ERROR("CGACT query failed");
        return DSAT_ERROR;
      }
      else if (profile_status.profile_count > DSAT_MAX_ALLOWED_PROFILES)
      {
        DS_AT_MSG0_ERROR("Invalid Profiles provided by 3GPP ATIF");
        return DSAT_ERROR;
      }

      res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
  
      DS_AT_MSG1_HIGH("CGACT query Total_Profile: %d ",profile_status.profile_count);
  
      /*Here we support max 24 profiles but there can be only max 8 PDN up at a time*/
      for (loop=0; loop < profile_status.profile_count; loop++) 
        {
        DS_AT_MSG1_HIGH("CGACT query executing, profile_num: %d ",profile_status.list[loop].profile_num);
        
        /* Generate response from data structure */
        length = snprintf ( (char *)res_buff_ptr->data_ptr,
           res_buff_ptr->size,
           "%s%s%s: %lu,%d",
           res_buff_ptr->data_ptr,
           (cntr++ > 0) ? "\n" : "",
           "+CGACT",
           profile_status.list[loop].profile_num,
           (profile_status.list[loop].act_status == DS_3GPP_ATIF_PDP_ACTIVATED)?
           DS_3GPP_ATIF_ACTIVATION : DS_3GPP_ATIF_DEACTIVATION );
      }
           /* No contexts defined, just print command name */
      if (0 == cntr)
      {
        length = snprintf((char *)res_buff_ptr->data_ptr, 
                                res_buff_ptr->size,"%s: ", "+CGACT");
      }
      res_buff_ptr->used = (uint16)length;
      return DSAT_OK;
    }
  /*---------------------------------------------------------
    Processing for a WRITE command
  -----------------------------------------------------------*/
  else if ( tok_ptr->op == (NA|EQ|AR) )
  {
    boolean state_changed = FALSE;
    /* Build the processing order for CID list */
    if ( !etsipkt_build_context_processing_list(tok_ptr, 
                                                DSAT_CQM_CGACT_WRITE, 
                                                &state_changed) )
    {
      DS_AT_MSG0_ERROR("Error building CGACT processing order list");
      return DSAT_ERROR;
    }
    
    if(dsat_pdp_state.valid_profile_count == 0)
    {
      memset ((void*)&dsat_pdp_state, 0x0,sizeof(dsat_pdp_state));
      DS_AT_MSG0_ERROR("Empty CGACT processing order list");
      return DSAT_OK;
    }
    
    return dsatetsipkt_cgact_proc_cmd();
  }
  /*---------------------------------------------------------
    Processing for a TEST command
  -----------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    if (DSAT_OK != dsatparm_exec_param_cmd(
                                           mode,
                                           parse_table,
                                           tok_ptr,
                                           res_buff_ptr
                                          ))
    {
      result = DSAT_ERROR;
    }
  }
  else 
  {
    /* unsupported operation */
    result = DSAT_ERROR;
  }
  return result;
} /* dsatetsipkt_exec_cgact_cmd */

/*===========================================================================

FUNCTION DSATETSIPKT_CGACT_RESPONSE_FMT

DESCRIPTION
  This function generates +CGACT response for AMPV command  
  commands.  Data is queried from ModeHandlers and formatted according
  to parameter flag.

DEPENDENCIES
  Query to Mode Handlers.

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
/*ARGSUSED*/
dsat_num_item_type dsatetsipkt_cgact_response_fmt
(
  const dsati_cmd_type  *parse_table,   /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format         /*  Flag for format type        */
)
{
  byte name_buff[16] = "+CGACT";
  int size = 0;
  ds_3gpp_atif_profile_status_info_type profile_status;
  int result = 0;
  dsm_item_type * res_buff_ptr = *out_buff_ptr;
  uint8 loop;
  byte cntr = 0;

  res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
  
  if( format != MULTI_INDEX_AMPV )
  {
    DS_AT_MSG0_HIGH("UnSupported format");
    return DSAT_ERROR;
  }

  size = res_buff_ptr->used;
  result = (word)snprintf((char *)res_buff_ptr->data_ptr,
                             res_buff_ptr->size,
                             "%s%s",
                             res_buff_ptr->data_ptr,
                             name_buff);
  memset(&profile_status,0,sizeof(profile_status));
		
  if(DSAT_OK != ds_3gpp_rmsm_atif_cgact_query(&profile_status,dsat_get_current_subs_id(FALSE)))
  {
    DS_AT_MSG0_ERROR("CGACT query failed for AMPV Command");
    return DSAT_ERROR;
  }
  DS_AT_MSG1_HIGH("CGACT query Total_Profile: %d ",profile_status.profile_count);

  /* Got profile_status and add CID of each 3GPP profile to response list... */
  for (loop = 0; loop < profile_status.profile_count; loop++) 
    {
      /* Check to see if a new DSM item is required */
      CHECK_APPEND_RESPONSE_BUFFER ((DSM_BUFFER_THRESHOLD - 30));
      result = (word)snprintf ((char *)res_buff_ptr->data_ptr, 
                     res_buff_ptr->size,
                   "%s%s(%lu,%d)",
                     res_buff_ptr->data_ptr,
                     (cntr++ > 0) ? "," : "",
                   profile_status.list[loop].profile_num,
                   (profile_status.list[loop].act_status == DS_3GPP_ATIF_PDP_ACTIVATED)?
                    DS_3GPP_ATIF_ACTIVATION : DS_3GPP_ATIF_DEACTIVATION );
  }
  /* caller increments res_buff_ptr->used */
  result -= size;
  DS_AT_MSG0_HIGH("CGACT - AMPV Query response");
  return (dsat_num_item_type)result;
} /* dsatetsipkt_cgact_response_fmt */

/*===========================================================================

FUNCTION DSATETSIPKT_RESET_PDP_CONTEXT_PROFILES

DESCRIPTION
  This function resets all persistent PDP context profiles to their
  default values.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in execution
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_reset_pdp_context_profiles ( void )
{
  byte i;
  dsat_result_enum_type result = DSAT_OK;
  ds_profile_num_type num;
  uint8     profile_list[DSAT_MAX_ALLOWED_PROFILES] = {0};
  uint8     total_profiles = 0;

  if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                             sizeof(profile_list), &total_profiles,
                                             dsat_get_current_ds_profile_subs_id()) )
  {
    DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
  }
  for (i = 0; i < total_profiles; i++) 
  {
    /* Perform reset for specific profile */
    RESET_BASE_CONTEXT_PARAMS(profile_list[i]);
    if(result == DSAT_ERROR)
    {
      return DSAT_ERROR;
    }
  }
  if(DS_PROFILE_REG_RESULT_SUCCESS != ds_profile_get_default_profile_num(DS_PROFILE_TECH_3GPP,
                 DS_PROFILE_EMBEDDED_PROFILE_FAMILY,&num))
  {
    DS_AT_MSG0_HIGH("Error getting default embedded profile: %d");
    return DSAT_ERROR;
  }
  /* RESET default socket profile to default values */
  if(DS_PROFILE_REG_RESULT_SUCCESS == ds_profile_create(DS_PROFILE_TECH_3GPP,&num))
  {
    if(ds_profile_reset_profile_to_default_per_sub(DS_PROFILE_TECH_3GPP,num,dsat_get_current_ds_profile_subs_id()) != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      DS_AT_MSG1_HIGH("Error defaulting profile: %d",DS_UMTS_DEF_SOCKET_PROFILE);
      return DSAT_ERROR;
    }
  }
  else
  {
    DS_AT_MSG1_HIGH("Error defaulting profile: %d",DS_UMTS_DEF_SOCKET_PROFILE);
    return DSAT_ERROR;
  }
  if(DS_PROFILE_REG_RESULT_SUCCESS != ds_profile_set_default_profile_num(DS_PROFILE_TECH_3GPP,
               DS_PROFILE_EMBEDDED_PROFILE_FAMILY,num))
  {
    DS_AT_MSG0_HIGH("Error setting default embedded profile: %d");
    return DSAT_ERROR;
  }
  return DSAT_OK;
} /* dsatetsipkt_reset_pdp_context_profiles */

/*===========================================================================

FUNCTION ETSIPKT_VALIDATE_APN

DESCRIPTION
  This function validates the format of an access point name (APN)
  per specification TS23.003 section 9.1

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
LOCAL dsat_result_enum_type etsipkt_validate_apn
(
  byte *s_ptr                             /*  Points to string  */
)
{
  byte temp_buf[DS_PROFILE_3GPP_MAX_APN_STRING_LEN+1];
  byte c;
  byte * w_ptr = s_ptr;
  int i, cnt = 0;

  /* Do nothing with empty string */
  if ((0 == s_ptr) || ('\0' == *s_ptr))
    return DSAT_OK;

  /* Strip quotes if required */
  if ('"' == *s_ptr)
  {
    (void)dsatutil_strip_quotes_out(s_ptr, temp_buf,
                                    (word)strlen((const char*)s_ptr));
    w_ptr = temp_buf;
  }
 
  /* Process until end of string */
  while ('\0' != *(w_ptr+cnt))
  {
    c = UPCASE(*(w_ptr+cnt)); 

    /* Check for valid character */
    if ( ! ((('A' <= c ) && ('Z' >= c)) ||
            (('0' <= c ) && ('9' >= c)) ||
            ('-' == c) ||
            ('.' == c)) )
    {
      DS_AT_MSG1_HIGH("Invalid character in APN: %c", c);
      return DSAT_ERROR;
    }
    cnt++;
  }

  if ( cnt == 1 )
  {
    /* We received a single character for APN */
    /* No need for further error checks       */
    return DSAT_OK;
  }

  /* Check first and last characters */
  for (i = 0; i < cnt; i += cnt-1)
  {
    c = UPCASE(*(w_ptr+i));
    if ( ! ((('A' <= c ) && ('Z' >= c)) ||
            (('0' <= c ) && ('9' >= c))) )
    {
      DS_AT_MSG1_HIGH("Invalid begin/end character in APN: %c", c);
      return DSAT_ERROR;
    }
  }
  
  /* Check APN length < maximum (100) */
  if (DS_PROFILE_3GPP_MAX_APN_STRING_LEN < cnt)
  {
      DS_AT_MSG1_HIGH("APN exceeds maximum length: %d", cnt);
      return DSAT_ERROR;
  }
  
  return DSAT_OK;
} /* etsipkt_validate_apn */



/*===========================================================================

FUNCTION ETSIPKT_VALIDATE_TCLASS

DESCRIPTION
  This function validates the UMTS QoS traffic class against dependent
  parameters.  The mapping is specified in 3GPP TS 23.107 section 6.5.1

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of validation.
  possible values:
    DSAT_ERROR : if there was any problem
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
LOCAL dsat_result_enum_type etsipkt_validate_tclass
(
  const dsati_cmd_type *parse_table,        /* Ptr to cmd in parse table   */
  ds_profile_3gpp_3gpp_qos_params_type * pdata_ptr  /* Ptr to Profile data */
)
{
  uint32 index;
  uint32 j = 0;
  boolean hit = FALSE;

  /* Map traffic class to dependent parameters */
  #define MAX_TCLASS_ENTRIES 4
  typedef struct 
  {
    ds_profile_3gpp_traffic_class_type      traffic_class;
    ds_profile_3gpp_sdu_err_ratio_type      sdu_errors[7];
    ds_profile_3gpp_residual_ber_ratio_type res_ber[8];
    dsat_num_item_type              min_trans_delay;
    boolean                         use_gtd_bitrate;
    boolean                         use_thandle_prio;
  } tclass_dep_s_type;
  
  tclass_dep_s_type * tclass_dep_ptr;
  static const tclass_dep_s_type traffic_class_dependency_table[] =
  {
    {
      DS_PROFILE_3GPP_TC_CONVERSATIONAL,
      {
        DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG2, DS_PROFILE_3GPP_SDU_ERR_RATIO_7ENEG3,
        DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG3,
        DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG4, DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG5,
        DS_PROFILE_3GPP_SDU_ERR_RESERVED, DS_PROFILE_3GPP_SDU_ERR_RESERVED
      },
      {
        DS_PROFILE_3GPP_RESIDUAL_BER_5ENEG2, DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG2, 
        DS_PROFILE_3GPP_RESIDUAL_BER_5ENEG3,
        DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG3, DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG4,
        DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG5,
        DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG6, DS_PROFILE_3GPP_RESIDUAL_BER_RESERVED
      },
      100,
      TRUE,
      FALSE
    },
    {
      DS_PROFILE_3GPP_TC_STREAMING,
      {
        DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG1, DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG2,
        DS_PROFILE_3GPP_SDU_ERR_RATIO_7ENEG3,
        DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG3, DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG4,
        DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG5,
        DS_PROFILE_3GPP_SDU_ERR_RESERVED
      },
      {
        DS_PROFILE_3GPP_RESIDUAL_BER_5ENEG2, DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG2,
        DS_PROFILE_3GPP_RESIDUAL_BER_5ENEG3,
        DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG3, DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG4, 
        DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG5,
        DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG6, DS_PROFILE_3GPP_RESIDUAL_BER_RESERVED
      },
      250,
      TRUE,
      FALSE
    },
    {
      DS_PROFILE_3GPP_TC_INTERACTIVE,
      {
        DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG3, DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG4, 
        DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG6,
        DS_PROFILE_3GPP_SDU_ERR_RESERVED, DS_PROFILE_3GPP_SDU_ERR_RESERVED,
        DS_PROFILE_3GPP_SDU_ERR_RESERVED, DS_PROFILE_3GPP_SDU_ERR_RESERVED
      },
      {
        DS_PROFILE_3GPP_RESIDUAL_BER_4ENEG3, DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG5, 
        DS_PROFILE_3GPP_RESIDUAL_BER_6ENEG8,
        DS_PROFILE_3GPP_RESIDUAL_BER_RESERVED, DS_PROFILE_3GPP_RESIDUAL_BER_RESERVED,
        DS_PROFILE_3GPP_RESIDUAL_BER_RESERVED, DS_PROFILE_3GPP_RESIDUAL_BER_RESERVED,
        DS_PROFILE_3GPP_RESIDUAL_BER_RESERVED,
      },
      (dsat_num_item_type)0xFFFFFFFF,
      FALSE,
      TRUE
    },
    {
      DS_PROFILE_3GPP_TC_BACKGROUND,
      {
        DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG3, DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG4,
        DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG6,
        DS_PROFILE_3GPP_SDU_ERR_RESERVED, DS_PROFILE_3GPP_SDU_ERR_RESERVED,
        DS_PROFILE_3GPP_SDU_ERR_RESERVED, DS_PROFILE_3GPP_SDU_ERR_RESERVED
      },
      {
        DS_PROFILE_3GPP_RESIDUAL_BER_4ENEG3, DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG5,
        DS_PROFILE_3GPP_RESIDUAL_BER_6ENEG8,
        DS_PROFILE_3GPP_RESIDUAL_BER_RESERVED, DS_PROFILE_3GPP_RESIDUAL_BER_RESERVED,
        DS_PROFILE_3GPP_RESIDUAL_BER_RESERVED, DS_PROFILE_3GPP_RESIDUAL_BER_RESERVED,
        DS_PROFILE_3GPP_RESIDUAL_BER_RESERVED
      },
      (dsat_num_item_type)0xFFFFFFFF,
      FALSE,
      FALSE
    }
  };
  if( (parse_table == NULL)||(pdata_ptr == NULL) )
  {
     DS_AT_MSG0_HIGH("Invalid params in etsipkt_validate_tclass()");
     return DSAT_ERROR;
  }
  /* Validate dependent parameters based on traffic class */
  for (index = 0; index < MAX_TCLASS_ENTRIES; index++)
  {
    /* Find lookup table entry */
    if (pdata_ptr->traffic_class ==
        traffic_class_dependency_table[index].traffic_class)
    {
      tclass_dep_ptr = (tclass_dep_s_type*) &traffic_class_dependency_table[index];
      
      /* Verify SDU error ratio applicable to traffic class */
      j = 0;
      hit = FALSE;
      while (DS_PROFILE_3GPP_SDU_ERR_RESERVED != tclass_dep_ptr->sdu_errors[j])
      {
        if ((pdata_ptr->sdu_err == tclass_dep_ptr->sdu_errors[j]) ||
            (pdata_ptr->sdu_err == DS_PROFILE_3GPP_SDU_ERR_RATIO_SUBSCRIBE))
        {
          hit = TRUE;
          break;
        }
        j++;
      }
      if (FALSE == hit)
      {
          DS_AT_MSG0_ERROR("SDU error ratio not valid for TClass");
          return DSAT_ERROR;
      }
      
      /* Verify residual BER applicable to traffic class */
      j = 0;
      hit = FALSE;
      while (DS_PROFILE_3GPP_RESIDUAL_BER_RESERVED != tclass_dep_ptr->res_ber[j])
      {
        if ((pdata_ptr->res_biterr == tclass_dep_ptr->res_ber[j]) ||
            (pdata_ptr->res_biterr == DS_PROFILE_3GPP_RESIDUAL_BER_SUBSCRIBE))
        {
          hit = TRUE;
          break;
        }
        j++;
      }
      if (FALSE == hit)
      {
        DS_AT_MSG0_ERROR("Residual BER not valid for TClass");
        return DSAT_ERROR;
      }
      
      /* Verify transfer delay > minimum for traffic class */
      /* As per Per 3gpp 24.008, Sec 10.5.6.5, the Transfer delay */
      /* value is ignored if the Traffic Class is Interactive class */
      /* or Background class */
      if( ( pdata_ptr->traffic_class != DS_PROFILE_3GPP_TC_INTERACTIVE ) && 
          ( pdata_ptr->traffic_class != DS_PROFILE_3GPP_TC_BACKGROUND ) )
      {
        if ((pdata_ptr->trans_delay < tclass_dep_ptr->min_trans_delay) &&
            (0 < pdata_ptr->trans_delay))
        {
          DS_AT_MSG1_ERROR("Transfer delay not valid for %d TClass ",
                        pdata_ptr->traffic_class);
          return DSAT_ERROR;
        }
      }
      /* Verify guaranteed bitrate specified only when allowed */
      if ((FALSE == tclass_dep_ptr->use_gtd_bitrate) &&
          ((0 < pdata_ptr->gtd_ul_bitrate) ||
           (0 < pdata_ptr->gtd_dl_bitrate)))
      {
        DS_AT_MSG0_ERROR("Grntd bitrate not valid for TClass");
        return DSAT_ERROR;
      }
      break;
    }
  }

#ifdef FEATURE_DATA_TCLASS_SUBSCRITION_OVERRIDE
  /* For Traffic Class CONVERSATIONAL and STREAMING, ensure   */
  /* subscribed maximum & guaranteed UL/DL bit rates are set  */
  /* explicitly (TS 27.103 CR 111)                            */
  if ((DS_PROFILE_3GPP_TC_CONVERSATIONAL == pdata_ptr->traffic_class) ||
      (DS_PROFILE_3GPP_TC_STREAMING == pdata_ptr->traffic_class))
  {
    mixed_def_s_type ** def_lim_pptr = 
      ( mixed_def_s_type **) parse_table->def_lim_ptr;

#define SET_MAX_IF_DEFAULT(i,param) \
    if (def_lim_pptr[i]->def->dflm.default_v == param) \
    { \
      param = def_lim_pptr[i]->def->dflm.upper; \
    } 

    /* If subscribed, set to parameter upper limit */
    SET_MAX_IF_DEFAULT(2, pdata_ptr->max_ul_bitrate);
    SET_MAX_IF_DEFAULT(3, pdata_ptr->max_dl_bitrate);
    SET_MAX_IF_DEFAULT(4, pdata_ptr->gtd_ul_bitrate);
    SET_MAX_IF_DEFAULT(5, pdata_ptr->gtd_dl_bitrate);
  }
#endif /* FEATURE_DATA_TCLASS_SUBSCRITION_OVERRIDE */

  return DSAT_OK;
} /* etsipkt_validate_tclass */



/*===========================================================================

FUNCTION ETSIPKT_PROCESS_PARAM

DESCRIPTION
  This function performs multitype parameter set processing/conversion.
  The target parameter to process is passed as an index.  The result is
  passed back to the caller by reference.
  If no string value was specified, the previous value is returned.

DEPENDENCIES
  The caller must allocate sufficient storage for string result buffer.

RETURN VALUE
  returns an enum that describes the result of validation.
  possible values:
    DSAT_ERROR : if there was any problem
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_process_param
(
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  const uint32 pindex,                  /*  Parameter index             */
  void * result_ptr,                     /*  Pointer to result           */  
  uint16 result_buff_len           /*  Length of result buffer */
)
{
  word radix;
  dsat_mixed_param_val_type * val_ptr = 
    (dsat_mixed_param_val_type *) dsatutil_get_val(parse_table->cmd_id,0,0,STR_TYPE);
  mixed_def_s_type ** def_lim_pptr = 
    ( mixed_def_s_type **) parse_table->def_lim_ptr;
  boolean is_range_valid = FALSE;
  
  /* Check for user specified value */
  if ( VALID_TOKEN(pindex) &&
      ((pindex+1) <= tok_ptr->args_found))
  {
    /* Check for list attribute */
    if (0 != (def_lim_pptr[pindex]->attrib & LIST))
    {
      /* Lookup position in list */
      const def_list_type * def_list_ptr = &def_lim_pptr[pindex]->def->def_list;
	  param_srch_enum_type srchrc = 
      dsatutil_mixed_param_set_num_arg(result_ptr, def_list_ptr,
                                         tok_ptr->arg[pindex], 
                                         def_lim_pptr[pindex]->attrib);
      
      /* Continue if parameter found */
      if (PARAM_NULL != srchrc)
      {
        if(PARAM_MISS == srchrc)
        {
          DS_AT_MSG1_HIGH("Cannot find in list parameter %d",pindex);
          return DSAT_ERROR;
        }
      }
    }
    /* Check for STRING attribute */
    else if (0 != (def_lim_pptr[pindex]->attrib & STRING))
    {
      word len = (word) strlen((char*)tok_ptr->arg[pindex]);
      if( len > def_lim_pptr[pindex]->def->string_len )
      {
        DS_AT_MSG0_HIGH("The size of the parameter is greater ");
        return DSAT_ERROR;
      }
      /* if the argument is quoted, strip out the quotes */
      if ((((def_lim_pptr[pindex]->attrib & YES_QUOTE)!= 0) ||
           ((def_lim_pptr[pindex]->attrib & NO_QUOTE) != 0)) &&
          ('"' == *tok_ptr->arg[pindex]))
      {
        if(!dsatutil_strip_quotes_out((const byte*)tok_ptr->arg[pindex],
                                      (byte*)result_ptr, len))
        {
          return DSAT_ERROR;        
        }
      }
      else
      {
        (void)strlcpy ((char*)result_ptr, (const char*)tok_ptr->arg[pindex],
                           result_buff_len);
       }
    }
    /* Assume numeric otherwise */
    else 
    {
      /* Convert to number */
      radix = ( (def_lim_pptr[pindex]->attrib & HEX) ? 16 : 10);
      if (ATOI_OK != dsatutil_atoi(result_ptr, tok_ptr->arg[pindex], radix))
      {
        DS_AT_MSG1_HIGH("Cannot convert parameter %d",pindex);
        return DSAT_ERROR;
      }

      /* Check against limits */
      if ( def_lim_pptr[pindex]->attrib & BROKEN_RANGE )
      {
        const mixed_def_u_type *def = def_lim_pptr[pindex]->def;

        while( def->dflm.default_v != MAX_BROKEN_RANGE &&
               def->dflm.lower != def->dflm.upper )
        {
          if ( (def->dflm.lower <= *(uint32 *)result_ptr) &&
               (def->dflm.upper >= *(uint32 *)result_ptr) )
          {
            is_range_valid = TRUE;
            break;
          }
          def++;
        }
      }
      else
      {
        if ( (def_lim_pptr[pindex]->def->dflm.lower <= *(uint32 *)result_ptr) &&
             (def_lim_pptr[pindex]->def->dflm.upper >= *(uint32 *)result_ptr) )
        {
          is_range_valid = TRUE;
        }
      }
      if( !is_range_valid )
      {
        DS_AT_MSG1_HIGH("Value outside range: %d",*(int32 *)(result_ptr));
        return DSAT_ERROR;
      }
    }
  }
  else
  {
    /* Load previous value */
    if (0 != (def_lim_pptr[pindex]->attrib & STRING))
    {
      /* String values (quotes already stripped) */
      (void)strlcpy ((char*)result_ptr, (const char*)val_ptr[pindex].string_item,
                         result_buff_len);
     }
    else
    {
      /* Numeric & LIST values */
      *(dsat_num_item_type *)(result_ptr) = val_ptr[pindex].num_item;
    }
  }
  return DSAT_OK; 
} /* etsipkt_process_param */


/*===========================================================================

FUNCTION ETSIPKT_MAP_SDU_ERR

DESCRIPTION
  This function performs table lookup to map the ATCOP display index to
  the PDP context enum value for SDU errors.  

DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if lookup successful
    FALSE : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean etsipkt_map_sdu_err
(
  const boolean  to_enum,                 /* Conversion flag  */
  dsat_num_item_type *index,              /* Value list index */
  ds_profile_3gpp_sdu_err_ratio_type *eval   /* Enum value       */
)
{
  byte cntr = 0;

  /* Table to map between the dsat_cgeqreq_sdu_err_valstr[] array  */
  /* values for display and ds_profile_3gpp_sdu_err_ratio_type values */
  static const struct sdu_error_conv_s
  {
    byte                             index;
    ds_profile_3gpp_sdu_err_ratio_type  sdu_error;
  } sdu_error_conv_table[] =
  {
    {0,  DS_PROFILE_3GPP_SDU_ERR_RATIO_SUBSCRIBE},
    {1,  DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG1},
    {2,  DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG2},
    {3,  DS_PROFILE_3GPP_SDU_ERR_RATIO_7ENEG3},
    {4,  DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG3},
    {5,  DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG4},
    {6,  DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG5},
    {7,  DS_PROFILE_3GPP_SDU_ERR_RATIO_1ENEG6},
    {MAX_INDEX,  DS_PROFILE_3GPP_SDU_ERR_RESERVED}
  };

  while (MAX_INDEX != sdu_error_conv_table[cntr].index )
  {
    /* Convert from list index to enum value */
    if (to_enum)
    {
      if (*index == sdu_error_conv_table[cntr].index)
      {
        *eval = sdu_error_conv_table[cntr].sdu_error;
        return TRUE;
      }
    }
    /* Convert to list index from enum value */
    else
    {
      if (*eval == sdu_error_conv_table[cntr].sdu_error)
      {
        *index = sdu_error_conv_table[cntr].index;
        return TRUE;
      }
    }
    cntr++;
    if(cntr >= ARR_SIZE(sdu_error_conv_table))
    {
      DS_AT_MSG0_ERROR("Invalid Access into sdu_error_conv_table");
      return FALSE;
    }
  }
  return FALSE;
} /* etsipkt_map_sdu_err */
  

/*===========================================================================

FUNCTION ETSIPKT_MAP_RES_BER

DESCRIPTION
  This function performs table lookup to map the ATCOP display index to
  the PDP context enum value for Residual BER.  

DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if lookup successful
    FALSE : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean etsipkt_map_res_ber
(
  const boolean  to_enum,                 /* Conversion flag  */
  dsat_num_item_type *index,              /* Value list index */
  ds_profile_3gpp_residual_ber_ratio_type *eval     /* Enum value       */
)
{
  byte cntr = 0;

  /* Table to map between the dsat_cgeqreq_res_biterr_valstr[] array  */
  /* values for display and ds_profile_qos_res_ber_enum_type values      */
  static const struct res_ber_conv_s
  {
    byte                             index;
    ds_profile_3gpp_residual_ber_ratio_type    res_ber;
  } res_ber_conv_table[] =
  {
    {0,  DS_PROFILE_3GPP_RESIDUAL_BER_SUBSCRIBE},
    {1,  DS_PROFILE_3GPP_RESIDUAL_BER_5ENEG2},
    {2,  DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG2},
    {3,  DS_PROFILE_3GPP_RESIDUAL_BER_5ENEG3},
    {4,  DS_PROFILE_3GPP_RESIDUAL_BER_4ENEG3},
    {5,  DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG3},
    {6,  DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG4},
    {7,  DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG5},
    {8,  DS_PROFILE_3GPP_RESIDUAL_BER_1ENEG6},
    {9,  DS_PROFILE_3GPP_RESIDUAL_BER_6ENEG8},
    {MAX_INDEX,  DS_PROFILE_3GPP_RESIDUAL_BER_RESERVED}
  };

  while (MAX_INDEX != res_ber_conv_table[cntr].index )
  {
    /* Convert from list index to enum value */
    if (to_enum)
    {
      if (*index == res_ber_conv_table[cntr].index)
      {
        *eval = res_ber_conv_table[cntr].res_ber;
        return TRUE;
      }
    }
    /* Convert to list index from enum value */
    else
    {
      if (*eval == res_ber_conv_table[cntr].res_ber)
      {
        *index = res_ber_conv_table[cntr].index;
        return TRUE;
      }
    }
    cntr++;
    if(cntr >= ARR_SIZE(res_ber_conv_table))
    {
      DS_AT_MSG0_ERROR("Invalid Access into res_ber_conv_table");
      return FALSE;
    }

  }
  return FALSE;
} /* etsipkt_map_res_ber */


/*===========================================================================

FUNCTION ETSIPKT_MAP_DORDER

DESCRIPTION
  This function performs table lookup to map the ATCOP display index to
  the PDP context enum value for Delivery Order.  

DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if lookup successful
    FALSE : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean etsipkt_map_dorder
(
  const boolean  to_enum,                 /* Conversion flag  */
  dsat_num_item_type *index,              /* Value list index */
  ds_profile_3gpp_qos_delivery_order_type *eval      /* Enum value       */
)
{
  byte cntr = 0;

  /* Table to map between the dsat_cgeqreq_dlvry_order_dflm[] range  */
  /* values for display and ds_profile_qos_dorder_enum_type values      */
  static const struct dorder_conv_s
  {
    byte                            index;
    ds_profile_3gpp_qos_delivery_order_type    dorder;
  } dorder_conv_table[] =
  {
    {0,  DS_PROFILE_3GPP_DO_OFF},
    {1,  DS_PROFILE_3GPP_DO_ON},
    {2,  DS_PROFILE_3GPP_DO_SUBSCRIBED},
    {MAX_INDEX,  DS_PROFILE_3GPP_DO_RESERVED}
  };

  while (MAX_INDEX != dorder_conv_table[cntr].index )
  {
    /* Convert from list index to enum value */
    if (to_enum)
    {
      if (*index == dorder_conv_table[cntr].index)
      {
        *eval = dorder_conv_table[cntr].dorder;
        return TRUE;
      }
    }
    /* Convert to list index from enum value */
    else
    {
      if (*eval == dorder_conv_table[cntr].dorder)
      {
        *index = dorder_conv_table[cntr].index;
        return TRUE;
      }
    }
    cntr++;
    if(cntr >= ARR_SIZE(dorder_conv_table))
    {
      DS_AT_MSG0_ERROR("Invalid Access into dorder_conv_table");
      return FALSE;
    }
  }
  return FALSE;
} /* etsipkt_map_dorder */


/*===========================================================================

FUNCTION ETSIPKT_MAP_DLVR_ERR_SDU

DESCRIPTION
  This function performs table lookup to map the ATCOP display index to
  the PDP context enum value for Delivery of Erroneous SDUs.  

DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if lookup successful
    FALSE : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean etsipkt_map_dlvr_err_sdu
(
  const boolean  to_enum,                 /* Conversion flag  */
  dsat_num_item_type *index,              /* Value list index */
  ds_profile_3gpp_deliver_err_sdu_type *eval    /* Enum value       */
)
{
  byte cntr = 0;

  /* Table to map between the dsat_cgeqreq_dlvry_order_dflm[] range  */
  /* values for display and ds_profile_qos_sdu_dlvr_enum_type values    */
  static const struct sdu_dlvr_conv_s
  {
    byte                            index;
    ds_profile_3gpp_deliver_err_sdu_type  sdu_dlvr;
  } sdu_dlvr_conv_table[] =
  {
    {0,  DS_PROFILE_3GPP_DELIVER_ERR_SDU_NO_DELIVER},
    {1,  DS_PROFILE_3GPP_DELIVER_ERR_SDU_DELIVER},
    {2,  DS_PROFILE_3GPP_DELIVER_ERR_SDU_NO_DETECT},
    {3,  DS_PROFILE_3GPP_DELIVER_ERR_SDU_SUBSCRIBE},
    {MAX_INDEX,  DS_PROFILE_3GPP_DELIVER_ERR_SDU_RESERVED}
  };

  while (MAX_INDEX != sdu_dlvr_conv_table[cntr].index )
  {
    /* Convert from list index to enum value */
    if (to_enum)
    {
      if (*index == sdu_dlvr_conv_table[cntr].index)
      {
        *eval = sdu_dlvr_conv_table[cntr].sdu_dlvr;
        return TRUE;
      }
    }
    /* Convert to list index from enum value */
    else
    {
      if (*eval == sdu_dlvr_conv_table[cntr].sdu_dlvr)
      {
        *index = sdu_dlvr_conv_table[cntr].index;
        return TRUE;
      }
    }
    cntr++;
    if(cntr >= ARR_SIZE(sdu_dlvr_conv_table))
    {
      DS_AT_MSG0_ERROR("Invalid Access into sdu_dlvr_conv_table");
      return FALSE;
    }

  }
  return FALSE;
} /* etsipkt_map_dlvr_err_sdu */



/*===========================================================================

FUNCTION ETSIPKT_MAP_TCLASS

DESCRIPTION
  This function performs table lookup to map the ATCOP display index to
  the PDP context enum value for Traffic Class.  

DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if lookup successful
    FALSE : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean etsipkt_map_tclass
(
  const boolean  to_enum,                 /* Conversion flag  */
  dsat_num_item_type *index,              /* Value list index */
  ds_profile_3gpp_traffic_class_type *eval      /* Enum value       */
)
{
  byte cntr = 0;

  /* Table to map between the dsat_cgeqreq_tclass_dflm[] range   */
  /* values for display and ds_profile_qos_tclass_enum_type values  */
  static const struct tclass_conv_s
  {
    byte                            index;
    ds_profile_3gpp_traffic_class_type    tclass;
  } tclass_conv_table[] =
  {
    {0,  DS_PROFILE_3GPP_TC_CONVERSATIONAL},
    {1,  DS_PROFILE_3GPP_TC_STREAMING},
    {2,  DS_PROFILE_3GPP_TC_INTERACTIVE},
    {3,  DS_PROFILE_3GPP_TC_BACKGROUND},
    {4,  DS_PROFILE_3GPP_TC_SUBSCRIBED},
    {MAX_INDEX,  DS_PROFILE_3GPP_TC_RESERVED}
  };

  while (MAX_INDEX != tclass_conv_table[cntr].index )
  {
    /* Convert from list index to enum value */
    if (to_enum)
    {
      if (*index == tclass_conv_table[cntr].index)
      {
        *eval = tclass_conv_table[cntr].tclass;
        return TRUE;
      }
    }
    /* Convert to list index from enum value */
    else
    {
      if (*eval == tclass_conv_table[cntr].tclass)
      {
        *index = tclass_conv_table[cntr].index;
        return TRUE;
      }
    }
    cntr++;
    if(cntr >= ARR_SIZE(tclass_conv_table))
    {
      DS_AT_MSG0_ERROR("Invalid Access into tclass_conv_table");
      return FALSE;
    }

  }
  return FALSE;
} /* etsipkt_map_tclass */

#ifdef FEATURE_SECONDARY_PDP

/*===========================================================================

FUNCTION ETSIPKT_VALIDATE_CGTFT_PARAM_COMBINATION

DESCRIPTION
  This function validates the combination of parameters given in CGTFT
  command parameters.  Returns error if illegal combination detected.
  The list of combinations is documented in 3GPP TS 23.060 section 15.3.2.

DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if validation successful
    FALSE : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean etsipkt_validate_cgtft_param_combination
(
  const tokens_struct_type *tok_ptr /* Parameter Values */
)
{
  #define MAX_LIST_SIZE 5
  #define MAX_LISTS 2
  #define MAX_COMBINATIONS 3
  #define ALLOWED_LIST 0
  #define EXCLUDE_LIST 1

  /* Define valid conbinations based on allowed and exclusion list. */
  /* Combination is invalid if any parameter on the exclude list is */
  /* specified.  At least one paramater on the allowed list must be */
  /* specified for the parameter set to be a valid combination.     */
  struct valid_combi
  {
    byte size;
    byte list[MAX_LIST_SIZE];
  } valid_combi_table[MAX_COMBINATIONS][MAX_LISTS] =
  {
    {{ 5, {(byte)CGTFT_SA_SM, (byte)CGTFT_TOS,  (byte)CGTFT_PN_NH, 
           (byte)CGTFT_DSTP,  (byte)CGTFT_SRCP} },
     { 2, {(byte)CGTFT_SPI,   (byte)CGTFT_FLOW, (byte)CGTFT_MAX,   
           (byte)CGTFT_MAX,   (byte)CGTFT_MAX } }},
    {{ 4, {(byte)CGTFT_SA_SM, (byte)CGTFT_TOS,  (byte)CGTFT_PN_NH, 
           (byte)CGTFT_SPI,   (byte)CGTFT_MAX } },
     { 3, {(byte)CGTFT_DSTP,  (byte)CGTFT_SRCP, (byte)CGTFT_FLOW,  
           (byte)CGTFT_MAX,   (byte)CGTFT_MAX } }},
    {{ 3, {(byte)CGTFT_SA_SM, (byte)CGTFT_TOS,  (byte)CGTFT_FLOW,  
           (byte)CGTFT_MAX,   (byte)CGTFT_MAX } },
     { 4, {(byte)CGTFT_PN_NH, (byte)CGTFT_DSTP, (byte)CGTFT_SRCP,  
           (byte)CGTFT_SPI,   (byte)CGTFT_MAX } }}
  };
  boolean exclude_hit = FALSE;
  boolean allowed_hit = FALSE;
  byte i,j;
  
  ASSERT (NULL != tok_ptr);

  /* Loop over posssible combinations until an allowable one is found */
  for (i=0; ((i < MAX_COMBINATIONS) && !(allowed_hit && !exclude_hit)); i++)
  {
    /* Test against exclude list; skip if param set */
    exclude_hit = FALSE;
    for (j=0; j < valid_combi_table[i][EXCLUDE_LIST].size; j++)
    {
      if ( VALID_TOKEN( valid_combi_table[i][EXCLUDE_LIST].list[j] ) )
      {
        exclude_hit = TRUE;
        DS_AT_MSG3_MED("Param combination excluded: [%d,%d]=%d",
                 i,j,valid_combi_table[i][EXCLUDE_LIST].list[j]);
        break;
      }
    }
    
    /* Test against allowed list; one param must be set */
    allowed_hit = FALSE;
    for (j=0; !exclude_hit &&
              (j < valid_combi_table[i][ALLOWED_LIST].size); j++)
    {
      if ( VALID_TOKEN( valid_combi_table[i][ALLOWED_LIST].list[j] ) )
      {
        allowed_hit = TRUE;
        DS_AT_MSG3_MED("Param combination allowed: [%d,%d]=%d",
                 i,j,valid_combi_table[i][ALLOWED_LIST].list[j]);
        break;
      }
    }
  }

  if (allowed_hit == FALSE || exclude_hit == TRUE)
  {
    DS_AT_MSG0_HIGH("Missing mandatory TFT parameter");
  }

  return (allowed_hit && !exclude_hit);
} /* etsipkt_validate_cgtft_param_combination */



/*===========================================================================

FUNCTION ETSIPKT_GET_PRIMARY_PROFILE_LIST

DESCRIPTION
  This function generates a list string of defined primary context profile
  connection identifiers.  If no primary profiles are defined, the string
  is returned empty.  The caller must pass in the string buffer size.

DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if generation successful
    FALSE : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean etsipkt_get_primary_profile_list
(
  byte * buf_ptr,    /* Pointer to buffer */
  byte   buf_size    /* Size of buffer */ 
)
{
  ds_profile_3gpp_pdp_context_type pdata;
  byte * c = buf_ptr;
  byte i;
  dsat_result_enum_type  result = DSAT_OK;
  uint8             profile_list[DSAT_MAX_ALLOWED_PROFILES] = {0};
  uint8             total_profiles = 0;

  ASSERT((NULL != buf_ptr) && (0 < buf_size));
  
  memset ((void*)buf_ptr,0,buf_size);
  memset((void*)&pdata, 0, sizeof(pdata));
  /* Loop over all contexts */

  if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                             sizeof(profile_list), &total_profiles,
                                             dsat_get_current_ds_profile_subs_id()) )
  {
    DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
  }
  
  for (i=0; i < total_profiles; i++) 
  {
    /* See if context profile is valid */
    IS_BASE_CONTEXT_VALID(i,result);
    if( result == FALSE )
    {
      continue;
    }
    if(dsatetsipkt_get_pdp_profile_secondary_flag(profile_list[i],&pdata.secondary_flag)
              != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      DS_AT_MSG1_HIGH("Error CGDSCONT PDP profile read: %d",i);
      pdata.secondary_flag = FALSE;
    }
      /* Skip further processing if secondary profile */
    if (TRUE == pdata.secondary_flag)
    {
      continue;
    }
    /* Continue processing if profile data is valid */      
    /* Query profile data */
    memset((void*)&pdata, 0, sizeof(pdata));
    if(dsatetsipkt_read_write_param_table(context_param_tbl,sizeof(context_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
                             profile_list[i],READ_COMMAND,(byte*)&pdata,sizeof(pdata),dsat_get_current_ds_profile_subs_id()) == DSAT_ERROR)
    {
      DS_AT_MSG1_ERROR("Error CGDSCONT PDP profile write: %d",profile_list[i]);
      return DSAT_ERROR;
    }
      /* Check for adequate buffer space (char + terminator) 
      assuming a max of two char + terminator so check the availability 
      of 3 bytes */
      if ( buf_size >= ((c - buf_ptr) + 3) )
      {
        /* Add primary cid to buffer */
        if ( pdata.pdp_context_number < 10 )
        {
        /* for single digit, directly convert */
          *c++ = pdata.pdp_context_number + '0';
        }
        else
        {
        /* for two digit, use the utility function */
          c = dsatutil_itoa (pdata.pdp_context_number, c, 10); 
        }
        *c++ = ',';
      }
      else
      {
        /* Indicate error in processing */
        return FALSE;
      }
    }

  /* Remove last comma */
  if ( c != buf_ptr )
  {
    *(--c) = '\0';
  }

  return TRUE;
} /* etsipkt_get_primary_profile_list */




/*===========================================================================

FUNCTION ETSIPKT_VALIDATE_PORT_PARAM

DESCRIPTION
  This function converts the network port range tupple into a numeric and
  validates the range.  If conversion files or range is invalid, FALSE is
  returned to the caller.

DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if validation successful
    FALSE : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean etsipkt_validate_port_param
(
  byte   * buf_ptr,    /* Pointer to string buffer */
  byte     buf_size,   /* Size of string buffer */ 
  uint16 * from_ptr,   /* Pointer to from port numeric equivalent */
  uint16 * to_ptr      /* Pointer to to port numeric equivalent */
)
{
  uint32 port_range;
  
  ASSERT((NULL != from_ptr) && (NULL != to_ptr) &&
         (NULL != buf_ptr) && (0 < buf_size));

  if (DSAT_OK == dsatutil_convert_tuple (STRING_TO_INT,
                                         TUPLE_TYPE_PORT,
                                         &port_range,
                                         buf_ptr,
                                         buf_size,
                                         DSAT_CONVERT_IP) )
  {
    uint16 to   = (uint16)(port_range & 0xFFFF);
    uint16 from = (uint16)(port_range >> 16);
    
    /* Ensure range is not descending */
    if (from > to )
    {
      DS_AT_MSG2_HIGH("Source from/to port range invalid: %d > %d",
                from, to);
    }
    else
    {
      *from_ptr = from;
      *to_ptr   = to;
      return TRUE;
    }
  }
  return FALSE;
} /* etsipkt_validate_port_param */


/*===========================================================================

FUNCTION ETSIPKT_FIND_LINKED_PROFILES

DESCRIPTION
  This function finds all PDP secondary context profiles linked to a
  given primary profile. The list is returned as an array and size.
  The input profile is NOT in the returned list.  The caller is
  responsible for allocating array of sufficient size.
  
DEPENDENCIES
  Input array must be of size DSAT_MAX_ALLOWED_PROFILES.

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if processing successful
    FALSE : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean etsipkt_find_linked_profiles
(
  const byte   primary_id, /* Primary profile identifier */
  byte * size_ptr,         /* Size of output list */
  byte * list_ptr          /* List of secondary profile identifiers */
)
{
  uint16 i;
  ds_profile_3gpp_pdp_context_type pdata;
  uint8             profile_list[DSAT_MAX_ALLOWED_PROFILES];
  uint8             total_profiles = 0;

  
  ASSERT( (primary_id > 0) &&
          (NULL != size_ptr) && (NULL != list_ptr) ); 

  *size_ptr = 0;

  if ( dsatetsipkt_is_profile_context_valid( primary_id,
                                             DS_PROFILE_TECH_EPC,
                                             dsat_get_current_ds_profile_subs_id() ))

  {
    if (DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_EPC, &profile_list[0],
                                                sizeof(profile_list), &total_profiles,
                                                dsat_get_current_ds_profile_subs_id()) )
    {
      DS_AT_MSG0_ERROR("Error getting EPC profile list!");
      return TRUE;
    }
  }
  else if ( dsatetsipkt_is_profile_context_valid( primary_id,
                                                  DS_PROFILE_TECH_3GPP,
                                                  dsat_get_current_ds_profile_subs_id() ))
  {
    if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                               sizeof(profile_list), &total_profiles,
                                               dsat_get_current_ds_profile_subs_id()) )
    {
      DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
      return TRUE;
    }
  }
  else
  {
    return TRUE;
  }
    
  /* Loop over all profiles */
  for (i=0; i<total_profiles; i++)
    {
      /* Query context profile data */
      memset((void*)&pdata, 0, sizeof(pdata));
      if(dsatetsipkt_read_write_param_table(context_param_tbl,sizeof(context_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
                               profile_list[i],READ_COMMAND,(byte*)&pdata,sizeof(pdata),dsat_get_current_ds_profile_subs_id()) == DSAT_ERROR)
      {
        DS_AT_MSG1_ERROR("Error CGDSCONT PDP profile read: %d",profile_list[i]);
        return DSAT_ERROR;
      }
      /* See if secondary is linked to given primary */
      if (pdata.primary_id == primary_id)
      {
        if (TRUE == pdata.secondary_flag)
        {
          /* Add profile to list */
          list_ptr[(*size_ptr)++] = profile_list[i];
        }
        else
        {
          DS_AT_MSG2_ERROR("Primary profiles cannot be linked together: %d->%d",
                     pdata.primary_id, primary_id);
          return FALSE;
        }
      }
    }
  return TRUE;
} /* etsipkt_find_linked_profiles */



/*===========================================================================

FUNCTION ETSIPKT_FIND_PROFILE_GROUP

DESCRIPTION
  This function generates list of all profiles in a profile group.
  The group is defined as one primary profile and all secondary
  profiles linked to the the primary.  The input profile is in the
  returned list.  The caller is responsible for allocating array of
  sufficient size.
  
DEPENDENCIES
  Input array must be of size DSAT_LEGACY_PROFILE_NUM_MAX.

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if processing successful
    FALSE : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean etsipkt_find_profile_group
(
  const byte  cid,         /* Profile identifier */
  byte * size_ptr,         /* Size of output list */
  byte * list_ptr          /* List of profile identifiers */
)
{
  ds_profile_3gpp_pdp_context_type pdata;
  byte primary_id;
  
  memset(&pdata, 0x00, sizeof(ds_profile_3gpp_pdp_context_type));
  ASSERT( (cid > 0) &&
          (NULL != size_ptr) && (NULL != list_ptr) );

  *size_ptr = 0;
  
  /* Query profile data */
  if(dsatetsipkt_read_write_param_table(context_param_tbl,sizeof(context_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
                           cid,READ_COMMAND,(byte*)&pdata,sizeof(pdata),dsat_get_current_ds_profile_subs_id()) == DSAT_ERROR)
  {
    DS_AT_MSG1_ERROR("Error PDP profile read: %d",cid);
    return FALSE;
  }
  /* Determine primary context profile */
  primary_id = (TRUE == pdata.secondary_flag)? pdata.primary_id : cid;
    
  /* Get list of all profiles mapped to same primary profile. */
  if (FALSE ==
      etsipkt_find_linked_profiles (primary_id, size_ptr, list_ptr))
  {
    DS_AT_MSG1_HIGH("Error finding linked profiles: %d",primary_id);
    return FALSE;
  }

  /* Add primary profile to group list */
  list_ptr[(*size_ptr)++] = primary_id;
  
  return TRUE;
} /* etsipkt_find_profile_group */


/*===========================================================================

FUNCTION ETSIPKT_RESET_LINKED_CONTEXT

DESCRIPTION
  This function finds secondary profiles linked to the
  passed primary context ID and resets them.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if processing successful
    FALSE : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean etsipkt_reset_linked_context
(
  const byte   primary_id        /* Primary profile identifier */
)
{
  byte profile_list[DSAT_MAX_ALLOWED_PROFILES];
  byte size_list;

  if (TRUE ==
      etsipkt_find_linked_profiles (primary_id, &size_list, profile_list))
  {
    byte i;
    dsat_result_enum_type result = DSAT_OK;
    for ( i = 0; (i < size_list) && (i < DSAT_MAX_ALLOWED_PROFILES); i++)
    {
      /* Clear the specified context parameters */
      RESET_BASE_CONTEXT_PARAMS(profile_list[i]);
      if( result == DSAT_ERROR)
      {
        return FALSE;
      }
    }
  }
  else
  {
    DS_AT_MSG1_HIGH("Error finding linked profiles: %d",primary_id);
    return FALSE;
  }
  
  return TRUE;
} /* etsipkt_reset_linked_context */



/*===========================================================================

FUNCTION ETSIPKT_VALIDATE_TFT_EPI_PARAM

DESCRIPTION
  This function validates the Evaluation Precedence Index (EPI) for a
  Traffic Flow Template.  The validation rule is given in 3GPP TS 27.007
  section 10.1.3.

  Must ensure the EPI is unique within all TFTs for all contexts
  having the same PDP address.  Given the address is assigned on
  primary context activation yet the address field in the primary
  profile may not be the same address assigned by the network, we
  assume each primary will have a unique address for a given APN. Then
  it is sufficuent that the EPI is unique for all TFTs mapped to given
  primary profile.

  From a secondary CID, the mapped primary is found then all profiles
  mapped to it.  From this list, all associated TFTs are read,
  checking EPI for a duplicate.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if validation successful
    FALSE : otherwise

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL boolean etsipkt_validate_tft_epi_param
(
  const byte   cid,        /* Context profile identifier */
  const byte   pfi,        /* Packet filter index        */
  const byte   epi         /* Evaluation precedence index */
)
{
  ds_profile_3gpp_tft_params_type tdata;
  byte profile_list[DSAT_MAX_ALLOWED_PROFILES];
  byte size_list;
  byte i,j;
  boolean hit = FALSE;
  filter_id_e_type filter_id_param =
             (filter_id_e_type) DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID1;

  /* Get profile group for input profile. */
  if (FALSE ==
      etsipkt_find_profile_group (cid, &size_list, profile_list))
  {
    DS_AT_MSG1_HIGH("Error finding profile group: %d",cid);
    return FALSE;
  }

  /* Seach for same EPI value */
  for (i=0; ((i<size_list && i < DSAT_MAX_ALLOWED_PROFILES) && !hit); i++)
  {
    /* Scan TFTs for each profile */
    for (j=1; j<=DS_PROFILE_3GPP_MAX_TFT_PARAM_SETS; j++)
    {
      /* Ignore TFT PFI belonging to input profile (overwrite case) */
      if ((cid == profile_list[i]) && (j == pfi))
      {
        continue;
      }
      memset((void*)&tdata, 0, sizeof(tdata));
      if(dsatetsipkt_read_write_profile_param(profile_list[i],READ_COMMAND,
                 (filter_id_param+j-1),
                 &tdata,sizeof(ds_profile_3gpp_tft_params_type)) == DSAT_ERROR)
      {
        DS_AT_MSG1_ERROR("Error get param CID : %d ",cid);
        continue;
      }
            
      /* Test EPI for match */
      if ( epi  == tdata.eval_prec_id )
      {
        hit = TRUE;
        break;
      }
    } /* for */
  } /* for */

  return !hit;
  
} /* etsipkt_validate_tft_epi_param */

/*===========================================================================

FUNCTION ETSIPKT_CAL_INDEX_FRM_PFI

DESCRIPTION
   If both filter ID filled and New Filter ID doesn't match to set 
   Filter ids then return INVALID INDEX else return VALID INDEX .
  
DEPENDENCIES
  None

RETURN VALUE
  returns an INDEX
    VALID INDEX (1 or 2) : if validation successful
    INVALID INDEX (3)    : otherwise

SIDE EFFECTS
  None
  
===========================================================================*/

LOCAL byte etsipkt_cal_index_frm_pfi 
(
  const byte   cid,        /* Context profile identifier */
  const byte   pfi        /* Packet filter identifier */
)
{
  #define PFI_INVALID_INDEX  3
  ds_profile_3gpp_tft_params_type temp_data;
  byte filter_index = 0;
  byte filter_idx[DS_PROFILE_3GPP_MAX_TFT_PARAM_SETS] = {PFI_INVALID_INDEX ,
                                                 PFI_INVALID_INDEX};
  filter_id_e_type filter_id_param = 
                (filter_id_e_type) DS_PROFILE_3GPP_PROFILE_PARAM_TFT_FILTER_ID1;
  
  
  memset((void*)&temp_data, 0, sizeof(temp_data));
  
  for(filter_index = 1; (filter_index <= DS_PROFILE_3GPP_MAX_TFT_PARAM_SETS) && 
                                   (filter_id_param < TFT_FILTER_ID_MAX); filter_index++,filter_id_param++)
  {
    memset((void*)&temp_data, 0, sizeof(temp_data));
    if(dsatetsipkt_read_write_profile_param(cid,READ_COMMAND,
               filter_id_param,
               &temp_data,sizeof(ds_profile_3gpp_tft_params_type)) == DSAT_ERROR)
    {
      DS_AT_MSG1_ERROR("Error get param CID : %d ",cid);
      continue;
    }
    if(temp_data.filter_id == pfi)
    {
      DS_AT_MSG1_HIGH("fi idx = %d",filter_index);
      return filter_index;
    }
    else
    {
      filter_idx[filter_index -1]=  filter_index ;
    }
  }
  DS_AT_MSG2_HIGH("fi idx = %d idx2 = %d",filter_idx[0],filter_idx[1]);
  if(filter_idx[0] ==  PFI_INVALID_INDEX)
  {
    return DS_PROFILE_3GPP_TFT_FILTER_ID1+1;
  }
  if(filter_idx[1] ==  PFI_INVALID_INDEX)
  {
    return DS_PROFILE_3GPP_TFT_FILTER_ID2+1;
  }
  return PFI_INVALID_INDEX;
}/* etsipkt_cal_index_frm_pfi */
#endif /* FEATURE_SECONDARY_PDP */

/*===========================================================================

FUNCTION ETSIPKT_DELTE_PROFILE_CONTEXT

DESCRIPTION
  This function finds profile delete it.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if processing successful
    FALSE : if error occurred

SIDE EFFECTS
  None
  
===========================================================================*/


LOCAL boolean etsipkt_delete_profile_context
(
  const byte   primary_id  /* Primary profile identifier */
)
{
  ds_profile_hndl_type      profile_hndl = NULL;
  ds_profile_status_etype   profile_status = DS_PROFILE_REG_RESULT_SUCCESS;

  DSAT_BEGIN_PROFILE_TRANSACTION(primary_id,DS_PROFILE_TECH_3GPP,profile_hndl);
  if( profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
  {
    if(profile_status == DS_PROFILE_REG_RESULT_ERR_INVAL_PROFILE_NUM)
    {
      return TRUE;
    }else
    {
      DS_AT_MSG1_HIGH("Error on profile context reset: %d",primary_id);
      return FALSE;
    }
  }
  else
  {
    DSAT_END_PROFILE_TRANSACTION(profile_hndl);
  }
  
   if(ds_profile_delete_ex (DS_PROFILE_TECH_3GPP,primary_id,TRUE) != DS_PROFILE_REG_RESULT_SUCCESS)
  { 
     DS_AT_MSG1_HIGH("Error on profile context reset: %d",primary_id);
    return FALSE;
  }
  return TRUE;

}/*etsipkt_delete_profile_context*/

/*===========================================================================

FUNCTION  ETSIPKT_CHECK_RESPONSE_BUFFER

DESCRIPTION
  This function checks the lenth of the response buffer to see if
  another must be chained to it.  A buffer limit value of zero means
  use normal DSM buffer default.

DEPENDENCIES
  None

RETURN VALUE
  returns pointer to current DSM item buffer on success; NULL otherwise

SIDE EFFECTS
  DSM item may be chained to passed in buffer, moving input pointer
  to new item.
  
===========================================================================*/
LOCAL dsm_item_type* etsipkt_check_response_buffer
(
  dsm_item_type  *res_buff_ptr,   /* Response buffer */
  uint16          buf_limit       /* Buffer contents limit */
)
{
  dsm_item_type  *cur_buff_ptr = res_buff_ptr;

  ASSERT (NULL != res_buff_ptr);

  /* Use default DSM packet limit if necessary */
  if (0 == buf_limit)
  {
    buf_limit = DSM_BUFFER_THRESHOLD;
  }
  
  /* Append new buffer if required. */
  cur_buff_ptr = dsatutil_append_dsm_item( cur_buff_ptr,
                                           buf_limit );
  /* If buffer appended, ensure null terminated for string functions. */
  if ( cur_buff_ptr != res_buff_ptr)
  {
    cur_buff_ptr->data_ptr[cur_buff_ptr->used]   = '\0';
  }

  return cur_buff_ptr;
} /* etsipkt_check_response_buffer */


/*===========================================================================

FUNCTION  ETSIPKT_BUILD_CONTEXT_PROCESSING_LIST

DESCRIPTION
  This function builds the processing order for +CGACT and +CGCMOD list of 
  context profiles.  The order of activation is all primary contexts before
  any secondary.  This avoids unnecessary failures should a dependent
  secondary context attempt activation before the primary yet both are
  in user specified list. 
  *state_changed_ptr only set when type_list is DSAT_CQM_CGACT_WRITE.
   
DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    TRUE : if processing successful 
    FALSE : otherwise

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean etsipkt_build_context_processing_list
(
  const tokens_struct_type *tok_ptr,     /* Command tokens from parser   */
  dsat_call_query_mode_e_type type_list, /* For which command */
  boolean * state_changed_ptr            /* Pointer to state change flag */
)
{
  byte                    prim_ctxlist[DSAT_MAX_ALLOWED_PROFILES];
  byte                    sec_ctxlist[DSAT_MAX_ALLOWED_PROFILES];
  byte *pri_ptr = prim_ctxlist;
  boolean no_cid_list;
  uint8                   i = 0;
  byte cid_limit = 0;
  dsat_num_item_type demand_state = (dsat_num_item_type)DSAT_PDP_CONNECT_MAX;
  dsat_num_item_type prim_profile_count = 0;
  dsat_num_item_type sec_profile_count  = 0;
  uint8                   profile_list[DSAT_MAX_ALLOWED_PROFILES] = {0};
  uint8                   total_profiles     = 0;

#ifdef FEATURE_SECONDARY_PDP
  byte *sec_ptr = sec_ctxlist;
  ds_profile_3gpp_pdp_context_secondary_flag_type secon_flag = FALSE;
#endif /* FEATURE_SECONDARY_PDP */

  ASSERT (NULL != tok_ptr);

  switch ( type_list )
  {
    case DSAT_CQM_CGACT_WRITE:
      *state_changed_ptr = FALSE;
      /* For +CGACT write command the first parm is state */
      no_cid_list = (1 == tok_ptr->args_found);
    break;

    case DSAT_CQM_CGCMOD_WRITE:
    /* For +CGCMOD write command there is only CID list */
      no_cid_list = (0 == tok_ptr->args_found);
    break;

    default:
      return FALSE;
  }
  
  memset((void*)prim_ctxlist, 0x0, sizeof(prim_ctxlist));
  memset((void*)sec_ctxlist, 0x0, sizeof(sec_ctxlist));

  if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                             sizeof(profile_list), &total_profiles,
                                             dsat_get_current_ds_profile_subs_id()) )
  {
    DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
  }

  if ( type_list == DSAT_CQM_CGACT_WRITE )
  {
    cid_limit = (byte)((no_cid_list)? total_profiles: (tok_ptr->args_found-1));

    /* Convert state parameter string to integer */
    if ( ATOI_OK != dsatutil_atoi(&demand_state, tok_ptr->arg[0], 10))
    {
      DS_AT_MSG1_ERROR("Cannot convert to integer parameter: %d",0);
      return FALSE;
    }

    if ((dsat_num_item_type)DSAT_PDP_CONNECT_MAX <= demand_state)
    {
      DS_AT_MSG1_ERROR("Demand state invalid: %d",demand_state);
      return FALSE;
    }
  }
  else if ( type_list == DSAT_CQM_CGCMOD_WRITE )
  {
    cid_limit = (byte)( no_cid_list ? total_profiles:tok_ptr->args_found);
  }
    
  if ( no_cid_list == FALSE )
  {
    memset( &profile_list[0], 0x00, sizeof(profile_list) );
    total_profiles = 0;
    for ( i = 1; i <= cid_limit; i++)
    {
      dsat_num_item_type cid = 0;
      
      /* First parameter is <state> for +CGACT, 
         start of <cid> list for +CGCMOD */
      byte *parm_str = (type_list == DSAT_CQM_CGACT_WRITE) ? 
      tok_ptr->arg[i] : tok_ptr->arg[i-1];
      if (ATOI_OK != dsatutil_atoi(&cid, parm_str, 10))
      {
        DS_AT_MSG1_ERROR("Cannot convert to integer parameter: %d",i);
        return FALSE;
      }
      /* Test for CID within valid range */
      if (!DSAT_IS_PROFILE_ID_WITHIN_RANGE(cid))
      {
        DS_AT_MSG1_ERROR("Connection ID invalid: %d",cid);
        return FALSE;
      }

      if ( !dsatetsipkt_is_profile_context_valid((uint16) cid,
                                                 DS_PROFILE_TECH_3GPP,
                                                 dsat_get_current_ds_profile_subs_id()))
        {
        DS_AT_MSG1_ERROR("Profile id %d is not a 3gpp profile",cid);
          return FALSE;
        }
      profile_list[i-1] = (uint8) cid;
      total_profiles++;
    }
      }


  /* Loop over each cid to build list of profiles to work on */
  /* Limit loop to maximum number of supported context profiles */
  for ( i = 0; i < total_profiles ; i++)
  {
#ifdef FEATURE_SECONDARY_PDP
          if (DS_PROFILE_REG_RESULT_SUCCESS !=
         dsatetsipkt_get_pdp_profile_secondary_flag(profile_list[i], &secon_flag))
          {
      DS_AT_MSG1_HIGH("Error PDP profile read: %d",profile_list[i]);
            return FALSE;
          }
      /* Build list of primary & secondary contexts.    */
    if(FALSE == secon_flag) 
      {
        /* Primaries at front of list */
      *pri_ptr++ = (byte)profile_list[i];
        prim_profile_count++;
      }
      else
      {
        /* Secondaries at end of list */
      *sec_ptr++ = (byte)profile_list[i];
        sec_profile_count++;
      }
#else
   *pri_ptr++ = profile_list[i];
   prim_profile_count++;
#endif /* FEATURE_SECONDARY_PDP */
  } /* for */

    memset(&dsat_pdp_cid[0],0,sizeof(dsat_pdp_cid));

    if ( total_profiles != (prim_profile_count + sec_profile_count) )
    {
      memset ((void*)&dsat_pdp_state, 0x0,sizeof(dsat_pdp_state));
      DS_AT_MSG3_ERROR("Incorrect number of pri=%d, sec=%d, total=%d",prim_profile_count,
      	                                                               sec_profile_count,
                                                                      total_profiles);
      return FALSE;
    }

  (void) dsatutil_memscpy((void*)&dsat_pdp_cid[0],
                            DSAT_MAX_ALLOWED_PROFILES,
                            (void*)&prim_ctxlist[0],
                            prim_profile_count); 

    (void) dsatutil_memscpy((void*)&dsat_pdp_cid[prim_profile_count],
                            (DSAT_MAX_ALLOWED_PROFILES-prim_profile_count),
                            (void*)&sec_ctxlist[0],
                            sec_profile_count); 
  
  dsat_pdp_state.change_index = 0;
    dsat_pdp_state.valid_profile_count                = total_profiles;

    /* Copy to structure */
    MSG_HIGH ( "Valid PDP profile count:%d, prim_count:%d, sec_count:%d", 
                total_profiles, prim_profile_count,sec_profile_count);
  
  if ( type_list == DSAT_CQM_CGACT_WRITE )
  {
    dsat_pdp_state.demand_state = (dsat_pdp_connect_state_e_type)demand_state;
  }
  return TRUE;
} /* etsipkt_build_context_processing_list */

/*===========================================================================

FUNCTION  ETSIPKT_PRINT_IPV4_ADDR

DESCRIPTION
  This function prints IPV4 address in the required format.
   
DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    DSAT_SUCCESS : if processing successful 
    DSAT_FAILURE : otherwise

SIDE EFFECTS
  None

===========================================================================*/

LOCAL boolean etsipkt_print_ipv4_addr
(
  dsat_ip_addr_type      *pdp_adr_ptr,
  uint32                  src_adr,
  boolean                 is_static_addr,
  dsm_item_type           *res_buff_ptr,
  char                    *buffer
)
{
  pdp_adr_ptr->ip_vsn = DS_PROFILE_3GPP_IP_V4; 
  if( FALSE == is_static_addr ) 
  {
    pdp_adr_ptr->addr.ds_profile_3gpp_pdp_addr_ipv4 = ps_ntohl(src_adr);
  }
  else 
  { 
    pdp_adr_ptr->addr.ds_profile_3gpp_pdp_addr_ipv4 = src_adr;
  }
  if( dsatutil_convert_ipaddr(INT_TO_STRING, pdp_adr_ptr,(byte *)buffer) != DSAT_OK )
  {
    return DSAT_FAILURE;
  } 
  else
  {
    res_buff_ptr->used += (word) snprintf((char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                          res_buff_ptr->size - res_buff_ptr->used,
                                          "%s",buffer);
  }
  return DSAT_SUCCESS;
} /* etsipkt_print_ipv4_addr */

/*===========================================================================

FUNCTION  ETSIPKT_PRINT_IPV6_ADDR

DESCRIPTION
  This function prints IPV6 address in the required format.
   
DEPENDENCIES
  None

RETURN VALUE
  returns an flag that describes the result of execution.
  possible values:
    DSAT_SUCCESS : if processing successful 
    DSAT_FAILURE : otherwise

SIDE EFFECTS
  None

===========================================================================*/

LOCAL boolean etsipkt_print_ipv6_addr
(
  dsat_ip_addr_type                       *pdp_adr_ptr,
  ds_profile_3gpp_pdp_addr_type_ipv6      *src_adr_ptr,
  boolean                                 is_static_addr,
  dsm_item_type                           *res_buff_ptr,
  char                                    *buffer
)
{
  pdp_adr_ptr->ip_vsn = DS_PROFILE_3GPP_IP_V6;
  if( FALSE == is_static_addr )
  {
    pdp_adr_ptr->addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u.u6_addr64[0] = ps_ntohll(src_adr_ptr->in6_u.u6_addr64[1]);
    pdp_adr_ptr->addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u.u6_addr64[1] = ps_ntohll(src_adr_ptr->in6_u.u6_addr64[0]);
  }
  else
  {
    pdp_adr_ptr->addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u.u6_addr64[0] = src_adr_ptr->in6_u.u6_addr64[0];
    pdp_adr_ptr->addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u.u6_addr64[1] = src_adr_ptr->in6_u.u6_addr64[1];
  }
  if( dsatutil_convert_ipaddr(INT_TO_STRING,pdp_adr_ptr,(byte *)buffer) != DSAT_OK )
  {
    return DSAT_FAILURE;
  }
  else
  {
    res_buff_ptr->used += (word) snprintf( (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                                           res_buff_ptr->size - res_buff_ptr->used,
                                           "%s",buffer);
  }
  return DSAT_SUCCESS;
} /* etsipkt_print_ipv6_addr */


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGEREP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGEREP command.
  This command reports (buffers) certain GPRS events when occured viz.,
  detach, PDP deactivate, class change.

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
dsat_result_enum_type dsatetsipkt_exec_cgerep_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_OK;
  dsat_num_item_type cgerep_val ;
  dsatetsipkt_gprs_msg_ms_info * gprs_dd_val = NULL;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  gprs_dd_val = dsat_get_base_addr_per_subs(DSAT_MS_MD_VALS, subs_id, FALSE);
 
  result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );

  if(result == DSAT_OK)
  {
    if(VALID_TOKEN(1))
    {
      cgerep_val =(dsat_num_item_type)dsatutil_get_val(
                                         DSATETSI_EXT_CGEREP_IDX,subs_id,0,NUM_TYPE);
      if(cgerep_val == DSAT_CGEREP_BUFFER_DISCARD_FWD_TE ||
         cgerep_val == DSAT_CGEREP_BUFFER_FWD_TE )
      {
        cgerep_val =(dsat_num_item_type)dsatutil_get_val(
                                         DSATETSI_EXT_CGEREP_IDX,subs_id,1,NUM_TYPE);
        if(cgerep_val == DSAT_CGEREP_EMPTY_BUFFER)
        {
          /* Clearing the event buffer */
          etsipkt_clear_gprs_events_buffer(subs_id);
        }
        else
        {
          gprs_dd_val->flush_gprs_buffer = TRUE;
        }
      }
    }
  }

  return result;
} /* dsatetsipkt_exec_cgerep_cmd */


/*===========================================================================
FUNCTION  ETSIPKT_CGPADDR_BUILD_CID_LIST

DESCRIPTION
  This function builds the CID list from the passed tokens.
   
DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK: if a valid CID was issued
  DSAT_ERROR: otherwise

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_cgpaddr_build_cid_list
(
  const tokens_struct_type *tok_ptr,  /* Command tokens from parser  */
  uint8 *cid_list,                    /* Parse results list */
  uint8 *index                        /* Length of list */
)
{
  dsat_result_enum_type  result = DSAT_OK;

  ASSERT( ( NULL != cid_list ) && ( NULL != index ) );

  if(tok_ptr->args_found > MAX_ARG )
  {
    DS_AT_MSG2_ERROR("Args found %d > %d max args supported",tok_ptr->args_found,
    	                                                        MAX_ARG);
    return DSAT_ERROR;
  }
    
  while( ( *index < tok_ptr->args_found) &&
         ( *index < DSAT_MAX_ALLOWED_PROFILES ) )
  {
    dsat_num_item_type cid;

    /* Convert token to numeric */
    if( ATOI_OK == dsatutil_atoi (&cid, tok_ptr->arg[*index], 10) )
    {
      /* Validate range */
      if( DSAT_IS_PROFILE_ID_WITHIN_RANGE(cid))
      {
        /* Store in list to report on */
        cid_list[(*index)++] = (uint8)cid;
      }
      else
      {
        DS_AT_MSG2_ERROR("CID out of range: index=%d value=%d",*index,cid);
        result = DSAT_ERROR;
        break;
      }
    }
    else
    {
      DS_AT_MSG1_ERROR("Cannot convert CID value: index=%d",*index);
      result = DSAT_ERROR;
      break;
    }
  } /* while */

  return result;
} /* etsipkt_cgpaddr_build_cid_list */


/*===========================================================================
FUNCTION  ETSIPKT_CGPADDR_FORMAT_HELPER_RESPONSE

DESCRIPTION
  This function builds the CID, PDP address formatting before sending to TE.
   
DEPENDENCIES
  None

RETURN VALUE
  TRUE:  If Successful, reutrn TRUE
  FALSE: Otherwise FALSE

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean etsipkt_cgpaddr_format_helper_response
(
  uint32                            cid,
  dsm_item_type                     *res_buff_ptr
)
{
  boolean                          is_static_addr = FALSE;
  boolean                          is_context_valid = FALSE;
  ds_profile_3gpp_pdp_context_type pdata;
  ds_umts_pdp_addr_type            pdp_addr;
  dsat_ip_addr_type                ip_addr;
  dsat_ip_addr_type                temp_pdp_addr;
  uint16                           cid_word = 0;
  char                             buf[MAX_IPADDR_STR_LEN+1];

  if (res_buff_ptr == NULL)
  {
    DS_AT_MSG0_ERROR("Buffer NULL");
    return FALSE;
  }

  memset(&pdata, 0x00, sizeof(ds_profile_3gpp_pdp_context_type));

  /*Storing cid as uint16 which will be used below for a couple internal functions*/
  cid_word = (uint16)cid;

  if(dsatetsipkt_is_profile_context_valid(cid_word,
                                          DS_PROFILE_TECH_3GPP,
                                          dsat_get_current_ds_profile_subs_id()))
  {
     if(dsatetsipkt_read_write_param_table(context_param_tbl,
                                           sizeof(context_param_tbl)/sizeof(atcop_profile_3gpp_param_list_type),
                                           cid_word,
                                           READ_COMMAND,
                                           (byte*)&pdata,
                                           sizeof(pdata),
                                           dsat_get_current_ds_profile_subs_id()) != DSAT_ERROR)
    {
      is_context_valid = TRUE;
      is_static_addr = FALSE;
      if(DSAT_OK != ds_3gpp_rmsm_atif_get_pdp_addr(cid,dsat_get_current_subs_id(FALSE),&pdp_addr))
      {
        is_static_addr = TRUE;
        DS_AT_MSG1_MED("PDP context - %d has a static address.",cid);

        if(pdata.pdp_type == DS_PROFILE_3GPP_PDP_IPV6)
        {
          ip_addr.ip_vsn = DS_PROFILE_3GPP_IP_V6;
        }
        else if(pdata.pdp_type == DS_PROFILE_3GPP_PDP_IPV4V6)
        {
          ip_addr.ip_vsn = DS_PROFILE_3GPP_IP_V4V6;
        }
        else
        {
          ip_addr.ip_vsn = DS_PROFILE_3GPP_IP_V4;
        }

        dsatutil_memscpy((void*)&ip_addr.addr,
                         sizeof(ds_profile_3gpp_pdp_addr_type),
                         (void*)&pdata.pdp_addr,
                         sizeof(ds_profile_3gpp_pdp_addr_type));
      }
      else
      {
        DS_AT_MSG2_MED("PDP context - %d has a dynamic address: version %d",
                       cid_word,
                       pdp_addr.ip_vsn);

        ip_addr.ip_vsn = (ds_profile_3gpp_ip_version_enum_type)pdp_addr.ip_vsn;
        ip_addr.addr.ds_profile_3gpp_pdp_addr_ipv4 = pdp_addr.pdp_addr_ipv4;

        ip_addr.addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u.u6_addr64[0] = 
                                            pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[0];
        ip_addr.addr.ds_profile_3gpp_pdp_addr_ipv6.in6_u.u6_addr64[1] = 
                                           pdp_addr.pdp_addr_ipv6.in6_u.u6_addr64[1];
      }
    }
    else
    {
      is_context_valid = FALSE;
    }
  }
  else
  {
    is_context_valid = FALSE;
  }
  if( TRUE == is_context_valid )
  {
    buf[0] = '\0';
    res_buff_ptr->used += (word) snprintf(
                          (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
                          res_buff_ptr->size - res_buff_ptr->used,
                          "%s %lu,", "+CGPADDR:",cid);

    if( ip_addr.ip_vsn == DS_PROFILE_3GPP_IP_V4)
    {
      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr , 
                                   ip_addr.addr.ds_profile_3gpp_pdp_addr_ipv4,
                                   is_static_addr, 
                                   res_buff_ptr, 
                                   buf ))
      {
        return FALSE;
      }
    }
    else if( ip_addr.ip_vsn == DS_PROFILE_3GPP_IP_V6 )
    {
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr , 
                                   &ip_addr.addr.ds_profile_3gpp_pdp_addr_ipv6,
                                   is_static_addr, 
                                   res_buff_ptr, 
                                   buf ))
      {
         return FALSE;
      }
    }
    else if( ip_addr.ip_vsn == DS_PROFILE_3GPP_IP_V4V6 )
    {
      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr,
                                   ip_addr.addr.ds_profile_3gpp_pdp_addr_ipv4,
                                   is_static_addr,
                                   res_buff_ptr,
                                   buf ))
      {
         return FALSE;
      }
      buf[0] = '\0';
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   &ip_addr.addr.ds_profile_3gpp_pdp_addr_ipv6,
                                   is_static_addr, 
                                   res_buff_ptr, 
                                   buf ))
      {
        return FALSE;
      }
    }

    res_buff_ptr->data_ptr[res_buff_ptr->used++] = '\n';
  }
  return TRUE;
}/*etsipkt_cgpaddr_format_helper_response*/

/*===========================================================================
FUNCTION  ETSIPKT_CGPADDR_FORMAT_RESPONSE

DESCRIPTION
  This function builds the CID, PDP address formatting before sending to TE.
   
DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK: if a valid CID was issued
  DSAT_ERROR: otherwise

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_cgpaddr_format_response
(
  const byte               *cid_list_ptr,
  const byte                cid_list_len,
  const tokens_struct_type *tok_ptr,
  dsm_item_type            *res_buff_ptr
)
{
  dsat_result_enum_type  result = DSAT_OK;
  uint32                 i;
  uint16                 used;
  uint8  profile_list[DSAT_MAX_ALLOWED_PROFILES] = {0};
  uint8 total_profiles = 0;

  if(tok_ptr->op == (NA|EQ|QU))
  {
    /* TEST syntax */
    
    /* Loop through the list to check if atleast one CID is defined 
       Else just return OK */

    res_buff_ptr->used = (word)snprintf(
             (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
             res_buff_ptr->size - res_buff_ptr->used,
             "%s", "+CGPADDR: (");
    used = res_buff_ptr->used;

    if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                               sizeof(profile_list), 
                                               &total_profiles,
                                               dsat_get_current_ds_profile_subs_id()) )
    {
      DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
    }
    for(i = 0; i < total_profiles; ++i)
    {
      res_buff_ptr->used += (word)snprintf(
          (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
          res_buff_ptr->size - res_buff_ptr->used,
                         "%d,", profile_list[i]);
    }
    if( used == res_buff_ptr->used )
    {
      /* If atleast one Context is not valid Reset data_ptr */
      res_buff_ptr->used = 0;
      res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
    }
    else
    {
      res_buff_ptr->used--; 
      res_buff_ptr->used += (word)snprintf(
               (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
               res_buff_ptr->size - res_buff_ptr->used,
               ")");
    }
  }
  else
  {
    /* WRITE syntax */
    res_buff_ptr->used = 0;
    
    /* Check for all contexts processing case */
    if( NULL == cid_list_ptr )
    {
      if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                                 sizeof(profile_list), 
                                                 &total_profiles,
                                                 dsat_get_current_ds_profile_subs_id()) )
      {
        DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
      }
      for(i = 0; i < total_profiles; ++i)
      {
         if(!etsipkt_cgpaddr_format_helper_response(profile_list[i],
                                                    res_buff_ptr))
         {
            return dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
         }
      }
    }
    else
    {
      /* Process specific contexts */
      for(i = 0; (i < cid_list_len) && (i < DSAT_MAX_ALLOWED_PROFILES); ++i)
      {
        byte index = cid_list_ptr[i];

        if(dsatetsipkt_is_profile_context_valid(index, DS_PROFILE_TECH_3GPP,
                                                dsat_get_current_ds_profile_subs_id()))
        if(!etsipkt_cgpaddr_format_helper_response(cid_list_ptr[i],
                                                   res_buff_ptr))
        {
           return dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
        }
      }
    }
    if(res_buff_ptr->used != 0)
    {
      res_buff_ptr->used--;
      res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
    }
  }
  return result;
} /* etsipkt_cgpaddr_format_response */

/*===========================================================================
FUNCTION  ETSIPKT_CGCONTRDP_FORMAT_HELPER_RESPONSE

DESCRIPTION
This function builds the P-CID, and related Dynamic parameters info
into a formatted form as mentioned in the 27.007.
   
DEPENDENCIES
  None

RETURN VALUE
  TRUE:  If Successful, reutrn TRUE
  FALSE: Otherwise FALSE

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean etsipkt_cgcontrdp_format_helper_response
(
  ds_3gpp_atif_dynamic_params_info_type  *dynamic_param_ptr,
  ds_3gpp_atif_primary_dynamic_info_type *prim_dynamic_ptr,
  dsm_item_type                          *res_buff_ptr
)
{
  dsat_ip_addr_type      temp_pdp_addr;
  char                   buf[MAX_IPADDR_STR_LEN+1];
  
  if (prim_dynamic_ptr == NULL ||
      prim_dynamic_ptr == NULL ||
      res_buff_ptr == NULL)
  {
    DS_AT_MSG0_ERROR("Buffer NULL");
    return FALSE;
  }

  if( DS_3GPP_ATIF_PRIM_INFO == dynamic_param_ptr->info_type)
  {
    if( prim_dynamic_ptr->ip_addr.ip_vsn ==(ds_ip_version_enum_type) DS_PROFILE_3GPP_IP_V4V6)
    {
      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr , 
                                   prim_dynamic_ptr->ip_addr.pdp_addr_ipv4,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }

      buf[0] = '\0';

      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   (ds_profile_3gpp_pdp_addr_type_ipv6 *) &prim_dynamic_ptr->ip_addr.pdp_addr_ipv6.in6_u,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    else if(prim_dynamic_ptr->ip_addr.ip_vsn == (ds_ip_version_enum_type) DS_PROFILE_3GPP_IP_V4)
    {
      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr ,
                                   prim_dynamic_ptr->ip_addr.pdp_addr_ipv4,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    else if(prim_dynamic_ptr->ip_addr.ip_vsn == (ds_ip_version_enum_type) DS_PROFILE_3GPP_IP_V6)
    {
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   (ds_profile_3gpp_pdp_addr_type_ipv6 *) &prim_dynamic_ptr->ip_addr.pdp_addr_ipv6.in6_u,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    buf[0] = '\0';

    res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';

    /* Print the <gw_addr> - First the IPv4 followed by IPv6 */
    if(prim_dynamic_ptr->v4_ipcp.ipcp4_options.gateway_addr != 0)
    {
      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr ,
                                   prim_dynamic_ptr->v4_ipcp.ipcp4_options.gateway_addr,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }

    }
    buf[0] = '\0';

    if(prim_dynamic_ptr->v6_ipcp.ipv6cp_options.gateway_addr.ps_s6_addr64[1] != 0)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ' ';
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   (ds_profile_3gpp_pdp_addr_type_ipv6*) &prim_dynamic_ptr->v6_ipcp.ipv6cp_options.gateway_addr,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    buf[0] = '\0';

    res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';

    /* Print the <DNS_prim_addr> - First the IPv4 followed by IPv6 */
    if(prim_dynamic_ptr->v4_ipcp.ipcp4_options.primary_dns!= 0)
    {
      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr ,
                                   prim_dynamic_ptr->v4_ipcp.ipcp4_options.primary_dns,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    buf[0] = '\0';

    if(prim_dynamic_ptr->v6_ipcp.ipv6cp_options.primary_dns.ps_s6_addr64[1] != 0)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ' ';
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   (ds_profile_3gpp_pdp_addr_type_ipv6*)&prim_dynamic_ptr->v6_ipcp.ipv6cp_options.primary_dns,
                                    FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    buf[0] = '\0';
    res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';

    /* Print the <DNS_sec_addr> - First the IPv4 followed by IPv6 */
    if(prim_dynamic_ptr->v4_ipcp.ipcp4_options.secondary_dns!= 0)
    {
      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr ,
                                   prim_dynamic_ptr->v4_ipcp.ipcp4_options.secondary_dns,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    buf[0] = '\0';

    if(prim_dynamic_ptr->v6_ipcp.ipv6cp_options.secondary_dns.ps_s6_addr64[1] != 0)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ' ';
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   (ds_profile_3gpp_pdp_addr_type_ipv6*)&prim_dynamic_ptr->v6_ipcp.ipv6cp_options.secondary_dns,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    buf[0] = '\0';

    /* Print the <P_CSCF_prim_addr>  */
    if( prim_dynamic_ptr->prim_pcscf_addr.ip_vsn == (ds_ip_version_enum_type) DS_PROFILE_3GPP_IP_V4V6)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';

      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr ,
                                   prim_dynamic_ptr->prim_pcscf_addr.pdp_addr_ipv4,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }

      buf[0] = '\0';

      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   (ds_profile_3gpp_pdp_addr_type_ipv6*)&prim_dynamic_ptr->prim_pcscf_addr.pdp_addr_ipv6.in6_u,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    else if(prim_dynamic_ptr->prim_pcscf_addr.ip_vsn == (ds_ip_version_enum_type) DS_PROFILE_3GPP_IP_V4)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';

      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr ,
                                   prim_dynamic_ptr->prim_pcscf_addr.pdp_addr_ipv4,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    else if(prim_dynamic_ptr->prim_pcscf_addr.ip_vsn == (ds_ip_version_enum_type) DS_PROFILE_3GPP_IP_V6)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   (ds_profile_3gpp_pdp_addr_type_ipv6 *) &prim_dynamic_ptr->prim_pcscf_addr.pdp_addr_ipv6.in6_u,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    buf[0] = '\0';

    /* Print the <P_CSCF_sec_addr>  */
    if( prim_dynamic_ptr->sec_pcscf_addr.ip_vsn == (ds_ip_version_enum_type) DS_PROFILE_3GPP_IP_V4V6)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';

      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr ,
                                   prim_dynamic_ptr->sec_pcscf_addr.pdp_addr_ipv4,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }

      buf[0] = '\0';

      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   (ds_profile_3gpp_pdp_addr_type_ipv6 *) &prim_dynamic_ptr->sec_pcscf_addr.pdp_addr_ipv6.in6_u,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    else if(prim_dynamic_ptr->sec_pcscf_addr.ip_vsn == (ds_ip_version_enum_type) DS_PROFILE_3GPP_IP_V4)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';

      if(!etsipkt_print_ipv4_addr( &temp_pdp_addr ,
                                   prim_dynamic_ptr->sec_pcscf_addr.pdp_addr_ipv4,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    else if(prim_dynamic_ptr->sec_pcscf_addr.ip_vsn == (ds_ip_version_enum_type) DS_PROFILE_3GPP_IP_V6)
    {
      res_buff_ptr->data_ptr[res_buff_ptr->used++] = ',';
      if(!etsipkt_print_ipv6_addr( &temp_pdp_addr ,
                                   (ds_profile_3gpp_pdp_addr_type_ipv6 *) &prim_dynamic_ptr->sec_pcscf_addr.pdp_addr_ipv6.in6_u,
                                   FALSE, res_buff_ptr, &buf[0] ))
      {
        return DSAT_FAILURE;
      }
    }
    buf[0] = '\0';

    res_buff_ptr->data_ptr[res_buff_ptr->used++] = '\n';
  }
  else
  {
    DS_AT_MSG1_ERROR(" Info type mismatch with DS_3GPP_ATIF_PRIM_INFO %d ",dynamic_param_ptr->info_type);
    ASSERT(0);
    return FALSE;
  }

  return TRUE;
} /*etsipkt_cgcontrdp_format_helper_response*/

/*===========================================================================
FUNCTION  ETSIPKT_CGCONTRDP_FORMAT_RESPONSE

DESCRIPTION
This function builds the P-CID, and related Dynamic parameters info
into a formatted form as mentioned in the 27.007.
   
DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK: if a valid CID was issued
  DSAT_ERROR: otherwise

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_cgcontrdp_format_response
(
  const byte               *cid_list_ptr,
  const byte                cid_list_len,
  const tokens_struct_type *tok_ptr,
  dsm_item_type            *res_buff_ptr
)
{
  dsat_result_enum_type  result = DSAT_OK;
  uint16                 used;
  uint32                 i;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
  ds_3gpp_atif_prim_profile_list_type prim_profile_list;

  memset((void*)&prim_profile_list, 0x0, sizeof(prim_profile_list));
  res_buff_ptr->used = 0;

  /* Get the Active Primary Contexts list */

  ds_3gpp_rmsm_atif_get_all_active_prim_profiles(subs_id,&prim_profile_list);

  if(tok_ptr->op == (NA|EQ|QU))
  {
    /* TEST syntax */

    res_buff_ptr->used = (word)snprintf(
             (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
             res_buff_ptr->size - res_buff_ptr->used,
             "%s ", "+CGCONTRDP: (");
    used = res_buff_ptr->used;

    for(i = 0;(i< DS_3GPP_MAX_PDN_CONTEXT)&&(i< prim_profile_list.data_len); ++i)
    {
      res_buff_ptr->used += (word)snprintf(
           (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
           res_buff_ptr->size - res_buff_ptr->used,
           "%lu,", prim_profile_list.data_ptr[i]);
    }
    if( used == res_buff_ptr->used )
    {
      /* If atleast one Context is not valid Reset data_ptr */
      
      res_buff_ptr->used = 0;
      res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
    }
    else
    {
      res_buff_ptr->used--; 
      res_buff_ptr->used += (word)snprintf(
               (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
               res_buff_ptr->size - res_buff_ptr->used,
               " )");
    }
  }
  else
  {
    /* WRITE syntax */
    dynamic_param_ptr = 
      (ds_3gpp_atif_dynamic_params_info_type *) dsat_alloc_memory(
                               sizeof(ds_3gpp_atif_dynamic_params_info_type), FALSE);

    memset((void*)dynamic_param_ptr,0x0,sizeof(ds_3gpp_atif_dynamic_params_info_type));

    /* Check for all contexts processing case */
    if( NULL == cid_list_ptr )
    {
      /* Process specified context */
      for(i = 0;(i< DS_3GPP_MAX_PDN_CONTEXT)&&(i< prim_profile_list.data_len); ++i)
      {
        memset((void*)dynamic_param_ptr,0x0,sizeof(ds_3gpp_atif_dynamic_params_info_type));

        if(TRUE == ds_3gpp_rmsm_atif_read_dynamic_params(prim_profile_list.data_ptr[i],
                                  subs_id,DS_3GPP_ATIF_PRIM_INFO,dynamic_param_ptr))
        {
          ds_3gpp_atif_primary_dynamic_info_type *prim_dynamic_ptr = 
                                            &dynamic_param_ptr->u.prim_dynamic_info;
          uint16 check_size = 20+9*MAX_IPADDR_STR_LEN+ \
                            strlen((const char *)prim_dynamic_ptr->apn);
          /*One loop below would need 40 max for IPV6, 20 for other params 
            +length of APN */

          CHECK_APPEND_RESPONSE_BUFFER_NEW (((uint16)DSM_BUFFER_THRESHOLD - check_size));

          res_buff_ptr->used += (word)snprintf(
                   (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                   res_buff_ptr->size - res_buff_ptr->used,
                  "%s %lu,%u,%s,", "+CGCONTRDP:",
                    prim_profile_list.data_ptr[i],
                    prim_dynamic_ptr->bearer_id,
                    prim_dynamic_ptr->apn);
          if (!etsipkt_cgcontrdp_format_helper_response(dynamic_param_ptr,
                                                        prim_dynamic_ptr,
                                                        res_buff_ptr))
          {
            dsatutil_free_memory((void*)dynamic_param_ptr);
            return dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
          }
        }
      }
    }
    else if(cid_list_len == 1)
    {
      /* Process specified context */

      if(TRUE == ds_3gpp_rmsm_atif_read_dynamic_params(cid_list_ptr[0],
                                                       subs_id,
                                                       DS_3GPP_ATIF_PRIM_INFO,
                                                       dynamic_param_ptr))
      {
        if( DS_3GPP_ATIF_PRIM_INFO == dynamic_param_ptr->info_type)
        {
          ds_3gpp_atif_primary_dynamic_info_type *prim_dynamic_ptr = 
                                            &dynamic_param_ptr->u.prim_dynamic_info;

          res_buff_ptr->used = (word)snprintf(
                   (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
                   res_buff_ptr->size - res_buff_ptr->used,
                  "%s %d,%u,%s,", "+CGCONTRDP:",
                    cid_list_ptr[0],
                    prim_dynamic_ptr->bearer_id,
                    prim_dynamic_ptr->apn);

          DS_AT_MSG2_HIGH(" IP addr version for the cid %d is %d ",cid_list_ptr[0],prim_dynamic_ptr->ip_addr.ip_vsn);

          if (!etsipkt_cgcontrdp_format_helper_response(dynamic_param_ptr,
                                                        prim_dynamic_ptr,
                                                        res_buff_ptr))
          {
            dsatutil_free_memory((void*)dynamic_param_ptr);
            return dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
          }
        }
      }
    }

    if(res_buff_ptr->used != 0)
    {
      res_buff_ptr->used--;
      res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
    }
    dsatutil_free_memory((void*)dynamic_param_ptr);
  }

  return result;
} /* etsipkt_cgcontrdp_format_response */

/*===========================================================================
FUNCTION  ETSIPKT_CGSCONTRDP_FORMAT_RESPONSE

DESCRIPTION
This function builds the CID, and related Dynamic parameters of all 
Non Primary Contexts info into a formatted form as mentioned in the 27.007.

   
DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK: if a valid CID was issued
  DSAT_ERROR: otherwise

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_cgscontrdp_format_response
(
  const byte               *cid_list_ptr,
  const byte                cid_list_len,
  const tokens_struct_type *tok_ptr,
  dsm_item_type            *format_buffer
)
{
  dsat_result_enum_type  result = DSAT_OK;
  uint32                 i;
  uint16                 used;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
  ds_3gpp_atif_flow_cid_list_type flow_profile_list;

  memset((void*)&flow_profile_list, 0x0, sizeof(flow_profile_list));
  format_buffer->used = 0;

  /* Get the Active Non-Primary Contexts list */
   ds_3gpp_rmsm_atif_get_all_active_flow_profiles(subs_id,&flow_profile_list);


  if(tok_ptr->op == (NA|EQ|QU))
  {
    /* TEST syntax */
    
    format_buffer->used = (word)snprintf(
             (char *)&format_buffer->data_ptr[format_buffer->used], 
             format_buffer->size - format_buffer->used,
             "%s ", "+CGSCONTRDP: (");
    used = format_buffer->used;

    for(i = 0;(i< DS_3GPP_MAX_FLOW_CONTEXT)&&(i< flow_profile_list.data_len); ++i)
    {
      format_buffer->used += (word)snprintf(
           (char *)&format_buffer->data_ptr[format_buffer->used], 
           format_buffer->size - format_buffer->used,
           "%ld,", (long int) flow_profile_list.data_ptr[i]);
    }
    if( used == format_buffer->used )
    {
      /* If atleast one Context is not valid Reset data_ptr */
      format_buffer->used = 0;
      format_buffer->data_ptr[format_buffer->used] = '\0';
    }
    else
    {
      format_buffer->used--; 
      format_buffer->used += (word)snprintf(
               (char *)&format_buffer->data_ptr[format_buffer->used],
               format_buffer->size - format_buffer->used,
               " )");
    }
  }
  else
  {
    /* WRITE syntax */
    dynamic_param_ptr = 
      (ds_3gpp_atif_dynamic_params_info_type *) dsat_alloc_memory(
                               sizeof(ds_3gpp_atif_dynamic_params_info_type), FALSE);

    /* Check for all contexts processing case */
    if( NULL == cid_list_ptr )
    {
      /* Process All Non - Primary contexts */

      for(i = 0;(i< DS_3GPP_MAX_FLOW_CONTEXT)&&(i< flow_profile_list.data_len); ++i)
      {

        if(TRUE == ds_3gpp_rmsm_atif_read_dynamic_params(flow_profile_list.data_ptr[i],
                                  subs_id,DS_3GPP_ATIF_SEC_INFO,dynamic_param_ptr))
        {
          if( DS_3GPP_ATIF_SEC_INFO == dynamic_param_ptr->info_type)
          {
            ds_3gpp_atif_secondary_dynamic_info_type *sec_dynamic_ptr = 
                                              &dynamic_param_ptr->u.sec_dynamic_info;
  
  
            format_buffer->used += (word)snprintf(
                     (char *)&format_buffer->data_ptr[format_buffer->used], 
                     format_buffer->size - format_buffer->used,
                    "%s %d,%d,%d \n", "+CGSCONTRDP:",
                      (int)sec_dynamic_ptr->cid,
                      (int)sec_dynamic_ptr->p_cid,
                      (int)sec_dynamic_ptr->bearer_id);
          }
          else
          {
            DS_AT_MSG1_ERROR(" Info type mismatch with DS_3GPP_ATIF_SEC_INFO %d ",dynamic_param_ptr->info_type);
            ASSERT(0);
          }
        }
      }/* for Loop */
    }
    else if(cid_list_len == 1)
    {
      /* Process specified context */

      if(TRUE == ds_3gpp_rmsm_atif_read_dynamic_params(cid_list_ptr[0],
                                subs_id,DS_3GPP_ATIF_SEC_INFO,dynamic_param_ptr))
      {
        if( DS_3GPP_ATIF_SEC_INFO == dynamic_param_ptr->info_type)
        {
          ds_3gpp_atif_secondary_dynamic_info_type *sec_dynamic_ptr = 
                                            &dynamic_param_ptr->u.sec_dynamic_info;
      
      
          format_buffer->used = (word)snprintf(
                   (char *)&format_buffer->data_ptr[format_buffer->used], 
                   format_buffer->size - format_buffer->used,
                  "%s %d,%d,%d ", "+CGSCONTRDP:",
                   (int)sec_dynamic_ptr->cid,
                   (int)sec_dynamic_ptr->p_cid,
                   (int)sec_dynamic_ptr->bearer_id);
      
        }
        else
        {
          DS_AT_MSG1_ERROR(" Info type mismatch with DS_3GPP_ATIF_SEC_INFO %d ",dynamic_param_ptr->info_type);
          ASSERT(0);
        }
      }
    }

    if(format_buffer->used != 0)
    {
      format_buffer->used--;
      format_buffer->data_ptr[format_buffer->used] = '\0';
    }
    dsatutil_free_memory((void*)dynamic_param_ptr);
  }

  return result;
}/* etsipkt_cgscontrdp_format_response */

/*===========================================================================
FUNCTION  ETSIPKT_CGEQOSRDP_FORMAT_RESPONSE

DESCRIPTION
This function builds the CID, and related QOS Dynamic parameters of all 
Non Primary Contexts info into a formatted form as mentioned in the 27.007.

   
DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK: if a valid CID was issued
  DSAT_ERROR: otherwise

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_cgeqosrdp_format_response
(
  const byte               *cid_list_ptr,
  const byte                cid_list_len,
  const tokens_struct_type *tok_ptr,
  dsm_item_type            *format_buffer
)
{
  dsat_result_enum_type  result = DSAT_OK;
  uint32                 i;
  uint16                 used;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
  ds_3gpp_atif_flow_cid_list_type flow_profile_list;

  memset((void*)&flow_profile_list, 0x0, sizeof(flow_profile_list));
  format_buffer->used = 0;

  /* Get the Active Non-Primary Contexts list */
  ds_3gpp_rmsm_atif_get_all_active_flow_profiles(subs_id,&flow_profile_list);

  if(tok_ptr->op == (NA|EQ|QU))
  {
    /* TEST syntax */
    
    format_buffer->used = (word)snprintf(
             (char *)&format_buffer->data_ptr[format_buffer->used], 
             format_buffer->size - format_buffer->used,
             "%s ", "+CGEQOSRDP: (");
    used = format_buffer->used;

    for(i = 0;(i< DS_3GPP_MAX_FLOW_CONTEXT)&&(i< flow_profile_list.data_len); ++i)
    {
      format_buffer->used += (word)snprintf(
           (char *)&format_buffer->data_ptr[format_buffer->used], 
           format_buffer->size - format_buffer->used,
           "%lu,", flow_profile_list.data_ptr[i]);
    }
    if( used == format_buffer->used )
    {
      /* If atleast one Context is not valid Reset data_ptr */
      format_buffer->used = 0;
      format_buffer->data_ptr[format_buffer->used] = '\0';
    }
    else
    {
      format_buffer->used--; 
      format_buffer->used += (word)snprintf(
               (char *)&format_buffer->data_ptr[format_buffer->used],
               format_buffer->size - format_buffer->used,
               " )");
    }
  }
  else
  {
    /* WRITE syntax */

    dynamic_param_ptr = 
      (ds_3gpp_atif_dynamic_params_info_type *) dsat_alloc_memory(
                               sizeof(ds_3gpp_atif_dynamic_params_info_type), FALSE);
    /* Check for all contexts processing case */
    if( NULL == cid_list_ptr )
    {
      /* Process specified context */

      for(i = 0;(i< DS_3GPP_MAX_FLOW_CONTEXT)&&(i< flow_profile_list.data_len); ++i)
      {

        if(TRUE == ds_3gpp_rmsm_atif_read_dynamic_params(flow_profile_list.data_ptr[i],
                                  subs_id,DS_3GPP_ATIF_QOS_INFO,dynamic_param_ptr))
        {
          if( DS_3GPP_ATIF_QOS_INFO == dynamic_param_ptr->info_type)
          {
            ds_3gpp_lte_qos_params_type *qos_dynamic_ptr = 
                                              &dynamic_param_ptr->u.dynamic_qos;

            format_buffer->used += (word)snprintf(
                     (char *)&format_buffer->data_ptr[format_buffer->used], 
                     format_buffer->size - format_buffer->used,
                    "%s %lu,%d,%u,%u,%u,%u \n", "+CGEQOSRDP:",
                     flow_profile_list.data_ptr[i],
                     qos_dynamic_ptr->qci,
                     (unsigned int)qos_dynamic_ptr->g_dl_bit_rate,
                     (unsigned int)qos_dynamic_ptr->g_ul_bit_rate,
                     (unsigned int)qos_dynamic_ptr->max_dl_bit_rate,
                     (unsigned int)qos_dynamic_ptr->max_ul_bit_rate);
          }
          else
          {
            DS_AT_MSG1_ERROR(" Info type mismatch with DS_3GPP_ATIF_QOS_INFO %d ",dynamic_param_ptr->info_type);
            ASSERT(0);
          }
        }
      }/* for */
    }
    else if(cid_list_len == 1)
    {
      /* Process specified context */

      if(TRUE == ds_3gpp_rmsm_atif_read_dynamic_params(cid_list_ptr[0],
                                subs_id,DS_3GPP_ATIF_QOS_INFO,dynamic_param_ptr))
      {
        if( DS_3GPP_ATIF_QOS_INFO == dynamic_param_ptr->info_type)
        {
          ds_3gpp_lte_qos_params_type *qos_dynamic_ptr = 
                                            &dynamic_param_ptr->u.dynamic_qos;
      
          format_buffer->used = (word)snprintf(
                   (char *)&format_buffer->data_ptr[format_buffer->used], 
                   format_buffer->size - format_buffer->used,
                  "%s %d,%d,%u,%u,%u,%u ", "+CGEQOSRDP:",
                   cid_list_ptr[0],
                   qos_dynamic_ptr->qci,
                   (unsigned int)qos_dynamic_ptr->g_dl_bit_rate,
                   (unsigned int)qos_dynamic_ptr->g_ul_bit_rate,
                   (unsigned int)qos_dynamic_ptr->max_dl_bit_rate,
                   (unsigned int)qos_dynamic_ptr->max_ul_bit_rate);
        }
        else
        {
          DS_AT_MSG1_ERROR(" Info type mismatch with DS_3GPP_ATIF_QOS_INFO %d ",dynamic_param_ptr->info_type);
          ASSERT(0);
        }
      }
    }

    if(format_buffer->used != 0)
    {
      format_buffer->used--;
      format_buffer->data_ptr[format_buffer->used] = '\0';
    }
    dsatutil_free_memory((void*)dynamic_param_ptr);
  }

  return result;
}/* etsipkt_cgeqosrdp_format_response */

/*===========================================================================
FUNCTION  ETSIPKT_CGTFTRDP_FORMAT_HELPER_RESPONSE

DESCRIPTION
This function builds the CID, and TFT related Dynamic parameters of all 
Non Primary Contexts info into a formatted form as mentioned in the 27.007.

   
DEPENDENCIES
  None

RETURN VALUE
  TRUE:  If Successful, reutrn TRUE
  FALSE: Otherwise FALSE

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean etsipkt_cgtftrdp_format_helper_response
(
  uint32                                   cid,
  ds_3gpp_atif_dynamic_params_info_type  * dynamic_param_ptr,
  dsm_item_type                          * res_buff_ptr
)
{
  uint32                      j = 0;
  ds_umts_tft_params_type    *tft_p = NULL;
  byte addr_mask_buf[MAX_ADDR_SUBNET_STRING_LEN+1];
  
  ds_3gpp_atif_tft_info_type *flow_dynamic_ptr = NULL;
  
  if (dynamic_param_ptr == NULL ||
      res_buff_ptr == NULL)
  {
    DS_AT_MSG0_ERROR("Buffer NULL");
    return FALSE;
  }

  flow_dynamic_ptr = &dynamic_param_ptr->u.dynamic_tft[0];

  if( DS_3GPP_ATIF_TFT_INFO == dynamic_param_ptr->info_type)
  {
    for(j = 0;j < DS_3GPP_MAX_FILTERS ; j++)
    {
      if(TRUE == flow_dynamic_ptr[j].tft_params.valid_flg)
      {
        uint16 check_size = 0;
        ds_profile_3gpp_address_mask_type    src_addr;
        ds_profile_3gpp_ip_version_enum_type ip_vsn =
           (ds_profile_3gpp_ip_version_enum_type)dynamic_param_ptr->u.dynamic_tft[j].tft_params.src_addr_mask.ip_vsn;

        tft_p = &dynamic_param_ptr->u.dynamic_tft[j].tft_params;

        if(ip_vsn == DS_PROFILE_3GPP_IP_V6 || ip_vsn == DS_PROFILE_3GPP_IP_V4V6)
        {
          src_addr.address.ds_profile_3gpp_tft_addr_ipv6.in6_u.u6_addr32[0] = 
            tft_p->src_addr_mask.address.pdp_addr_ipv6.in6_u.u6_addr32[0];
          src_addr.address.ds_profile_3gpp_tft_addr_ipv6.in6_u.u6_addr32[1] = 
            tft_p->src_addr_mask.address.pdp_addr_ipv6.in6_u.u6_addr32[1];
          src_addr.address.ds_profile_3gpp_tft_addr_ipv6.in6_u.u6_addr32[2] = 
            tft_p->src_addr_mask.address.pdp_addr_ipv6.in6_u.u6_addr32[2];
          src_addr.address.ds_profile_3gpp_tft_addr_ipv6.in6_u.u6_addr32[3] = 
            tft_p->src_addr_mask.address.pdp_addr_ipv6.in6_u.u6_addr32[3];
          src_addr.mask.prefix_len = tft_p->src_addr_mask.mask.prefix_len;
        }
        else
        {
          src_addr.address.ds_profile_3gpp_tft_addr_ipv4 = 
            tft_p->src_addr_mask.address.pdp_addr_ipv4;
          src_addr.mask.mask = (uint8)tft_p->src_addr_mask.mask.mask;
        }
        if ( DSAT_OK != dsatutil_convert_addr_mask( INT_TO_STRING,
                                                    &src_addr,
                                                    &ip_vsn,
                                                    &addr_mask_buf[0] ) )
        {
          DS_AT_MSG0_HIGH("Error validating Address Mask parameter");
          return FALSE;
        }
         /*One loop below  50 for other params +length of mask_buf */
        check_size = 60 + strlen((const char *)addr_mask_buf);

        CHECK_APPEND_RESPONSE_BUFFER_NEW (((uint16)DSM_BUFFER_THRESHOLD - check_size));

        res_buff_ptr->used += (word)snprintf(
              (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
               res_buff_ptr->size - res_buff_ptr->used,
               "%s %lu,%d,%d,\"%s\",%d,%d.%d,%d.%d,%X,%d.%d,%X,%d,%d\n",
               "+CGTFTRDP:",
               cid,
               flow_dynamic_ptr[j].tft_params.filter_id,
               flow_dynamic_ptr[j].tft_params.eval_prec_id,
               addr_mask_buf,
               flow_dynamic_ptr[j].tft_params.prot_num,
               flow_dynamic_ptr[j].tft_params.dest_port_range.from,
               flow_dynamic_ptr[j].tft_params.dest_port_range.to,
               flow_dynamic_ptr[j].tft_params.src_port_range.from,
               flow_dynamic_ptr[j].tft_params.src_port_range.to,
               (int)flow_dynamic_ptr[j].tft_params.ipsec_spi,
               (flow_dynamic_ptr[j].tft_params.tos_mask >> 8),
               (uint8)(flow_dynamic_ptr[j].tft_params.tos_mask & 0xFF ),
               (unsigned int)flow_dynamic_ptr[j].tft_params.flow_label,
               (uint8)flow_dynamic_ptr[j].direction,
                flow_dynamic_ptr[j].tft_params.filter_id);
      }
    }/* Inner for Loop */
  }
  else
  {
     DS_AT_MSG1_ERROR(" Info type mismatch with DS_3GPP_ATIF_TFT_INFO %d ",dynamic_param_ptr->info_type);
     ASSERT(0);
     return FALSE;
  }
   return TRUE;
} /*etsipkt_cgtftrdp_format_helper_response*/

/*===========================================================================
FUNCTION  ETSIPKT_CGTFTRDP_FORMAT_RESPONSE

DESCRIPTION
This function builds the CID, and TFT related Dynamic parameters of all 
Non Primary Contexts info into a formatted form as mentioned in the 27.007.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK: if a valid CID was issued
  DSAT_ERROR: otherwise

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_cgtftrdp_format_response
(
  const byte               *cid_list_ptr,
  const byte                cid_list_len,
  const tokens_struct_type *tok_ptr,
  dsm_item_type            *res_buff_ptr
)
{
  uint32                 i;
  dsat_result_enum_type  result = DSAT_OK;
  uint16                 used;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);
  ds_3gpp_atif_flow_cid_list_type flow_profile_list;

  memset((void*)&flow_profile_list, 0x0, sizeof(flow_profile_list));
  res_buff_ptr->used = 0;

  /* Get the Active Non-Primary Contexts list */
  ds_3gpp_rmsm_atif_get_all_active_flow_profiles(subs_id,&flow_profile_list);

  if(tok_ptr->op == (NA|EQ|QU))
  {
    /* TEST syntax */
    
    res_buff_ptr->used = (word)snprintf(
             (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
             res_buff_ptr->size - res_buff_ptr->used,
             "%s ", "+CGTFTRDP: (");
    used = res_buff_ptr->used;

    for(i = 0;(i< DS_3GPP_MAX_FLOW_CONTEXT)&&(i< flow_profile_list.data_len); ++i)
    {
      res_buff_ptr->used += (word)snprintf(
           (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used], 
           res_buff_ptr->size - res_buff_ptr->used,
           "%ld,",(long int) flow_profile_list.data_ptr[i]);
    }
    if( used == res_buff_ptr->used )
    {
      /* If atleast one Context is not valid Reset data_ptr */
      res_buff_ptr->used = 0;
      res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
    }
    else
    {
      res_buff_ptr->used--; 
      res_buff_ptr->used += (word)snprintf(
               (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
               res_buff_ptr->size - res_buff_ptr->used,
               " )");
    }
  }
  else
  {
    /* WRITE syntax */
    dynamic_param_ptr = 
      (ds_3gpp_atif_dynamic_params_info_type *) dsat_alloc_memory(
                               sizeof(ds_3gpp_atif_dynamic_params_info_type), FALSE);
    /* Check for all contexts processing case */
    if( NULL == cid_list_ptr )
    {
        uint32                 i;
      /* Process specified context */

      for(i = 0;(i< DS_3GPP_MAX_FLOW_CONTEXT)&&(i< flow_profile_list.data_len); ++i)
      {

        if(TRUE == ds_3gpp_rmsm_atif_read_dynamic_params(flow_profile_list.data_ptr[i],
                                                         subs_id,
                                                         DS_3GPP_ATIF_TFT_INFO,
                                                         dynamic_param_ptr))
        {
           if(!etsipkt_cgtftrdp_format_helper_response(flow_profile_list.data_ptr[i],
                                                       dynamic_param_ptr,
                                                       res_buff_ptr))
           {
              dsatutil_free_memory((void*)dynamic_param_ptr);
              return dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
           }
        }
      }/* Outer for Loop */
    }
    else if(cid_list_len == 1)
    {
      /* Process specified context */
      if(TRUE == ds_3gpp_rmsm_atif_read_dynamic_params(cid_list_ptr[0],
                                                       subs_id,
                                                       DS_3GPP_ATIF_TFT_INFO,
                                                       dynamic_param_ptr))
      {
         if(!etsipkt_cgtftrdp_format_helper_response(cid_list_ptr[0],
                                                     dynamic_param_ptr,
                                                     res_buff_ptr))
         {
            dsatutil_free_memory((void*)dynamic_param_ptr);
            return dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
         }
      }
    }

    if(res_buff_ptr->used != 0)
    {
      res_buff_ptr->used--;
      res_buff_ptr->data_ptr[res_buff_ptr->used] = '\0';
    }
    dsatutil_free_memory((void*)dynamic_param_ptr);
  }

  return result;
}/* etsipkt_cgtftrdp_format_response */


/*===========================================================================
FUNCTION  ETSIPKT_CGDATA_GET_CID_INFO

DESCRIPTION
  Gets the defined CID from the PDP table.

DEPENDENCIES
  None

RETURNS
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_cme_error_e_type etsipkt_cgdata_get_cid_info(byte *cid)
{
  uint8 i;
  uint8 no_of_cids_defined = 0;
  ds_profile_3gpp_pdp_context_secondary_flag_type secondary_flag = FALSE;
  uint8     profile_list[DSAT_MAX_ALLOWED_PROFILES] = {0};
  uint8     total_profiles = 0;
  
  if(DSAT_ERROR == etsipkt_get_profile_list( DS_PROFILE_TECH_3GPP,&profile_list[0],
                                           sizeof(profile_list), &total_profiles,
                                           dsat_get_current_ds_profile_subs_id()) )
  {
    DS_AT_MSG0_ERROR("Error getting 3GPP profile list!");
  }

  for(i = 0; i < total_profiles; ++i)
  {
    if( dsatetsipkt_get_pdp_profile_secondary_flag(profile_list[i],&secondary_flag)!= DS_PROFILE_REG_RESULT_SUCCESS )
      {
        DS_AT_MSG1_HIGH("Error PDP profile read: %d",i+1);
        secondary_flag = FALSE;
      }
      no_of_cids_defined++;
      if(no_of_cids_defined > 1 || secondary_flag)
      {
        return DSAT_CME_OP_NOT_SUPPORTED;
      }
    *cid = profile_list[i];
  }

  if(no_of_cids_defined == 0)
  {
    return DSAT_CME_INVALID_INDEX;
  }

  return DSAT_CME_NO_ERROR;
} /* etsipkt_cgdata_get_cid_info */


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGDATA_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGDATA command.
  This command initiates a packet data call.

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
dsat_result_enum_type dsatetsipkt_exec_cgdata_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type  result = DSAT_OK;

  if(tok_ptr->op == (NA|EQ|QU))   /* Test command */ 
  {
    result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );
  }
  else if((tok_ptr->op == (NA|EQ))   /* Default arg */
     || (tok_ptr->op == (NA|EQ|AR)))  /* Write command */
  {
    dsat_cme_error_e_type  cid_info_result;

    memset((void*)&dsat_cgdata_context, 0, sizeof(dsat_cgdata_info_s_type));

    if(tok_ptr->op == (NA|EQ))
    {
      /* We would take the default values by going through the PDP table for 
         which the context IDs have been defined. */
      if((cid_info_result = 
                      etsipkt_cgdata_get_cid_info(&dsat_cgdata_context.cid))
                          != DSAT_CME_NO_ERROR)
      {
        return dsat_send_cme_error(cid_info_result);
      }
    }
    else if(tok_ptr->op == (NA|EQ|AR))
    {
      dsat_num_item_type       cid_result = FALSE;
      byte out_string[4];
      word str_len = sizeof(out_string);

      /* Verifying for lower and upper case */

      if(!(VALID_TOKEN(0)) || dsatutil_strip_quotes_out((const byte *)tok_ptr->arg[0], 
                                   out_string, str_len) != TRUE)
      {
        return DSAT_ERROR;
      }

      if((dsatutil_strcmp_ig_sp_case((const byte *)out_string, 
                                     (const byte *)"PPP") != 0)     || 
         (tok_ptr->args_found > 2))
      {
        /* Potentially more than one PDP context activation is requested 
         or a different L2P is requested. We currently do not support it */
        return dsat_send_cme_error(DSAT_CME_OP_NOT_SUPPORTED);
      }

      if(VALID_TOKEN(1))
      {
        if(dsatutil_atoi(&cid_result, tok_ptr->arg[1], 10) != ATOI_OK)
        {
          return dsat_send_cme_error(DSAT_CME_PHONE_FAILURE);
        }
        else
        {
          ds_profile_3gpp_pdp_context_secondary_flag_type secon_flag = FALSE;

          /* Check if the CID is within range of supported set or not */
          if(!DSAT_IS_PROFILE_ID_WITHIN_RANGE(cid_result))
          {
            return DSAT_ERROR;
          }
          /* Verify against PDP table whether this context ID 
           is defined */
          if( FALSE == dsatetsipkt_is_profile_context_valid((uint16)cid_result, DS_PROFILE_TECH_3GPP,
                                                            dsat_get_current_ds_profile_subs_id()) )
          {
            DS_AT_MSG1_ERROR("Context ID %d is not valid",cid_result);
            return dsat_send_cme_error(DSAT_CME_INVALID_INDEX);;
          }
          if(DS_PROFILE_REG_RESULT_SUCCESS == 
                             dsatetsipkt_get_pdp_profile_secondary_flag(
                  (uint16)cid_result , &secon_flag))
          {
            if(secon_flag == FALSE)
            {
              dsat_cgdata_context.cid =(byte)cid_result;
              return etsipkt_initiate_packet_call(res_buff_ptr);
            }
            else
            {
              /* We currently do not support secondary PDP activations 
                 through TE */
               DS_AT_MSG1_ERROR("Context ID %d is secondary",cid_result);
               return dsat_send_cme_error(DSAT_CME_OP_NOT_SUPPORTED);
            }
          }
          else
          {
            /* If we are here, there was no match for the context ID 
               specified */
            return dsat_send_cme_error(DSAT_CME_INVALID_INDEX);
          }
        }
      }
      else
      {
        if((cid_info_result = 
                 etsipkt_cgdata_get_cid_info(&dsat_cgdata_context.cid))
                          != DSAT_CME_NO_ERROR)
        {
          return dsat_send_cme_error(cid_info_result);
        }
      }
    }
  }
  else
  {
    result = DSAT_ERROR;
  }

  return result;
} /* dsatetsipkt_exec_cgdata_cmd */


/*===========================================================================
FUNCTION  ETSIPKT_INITIATE_PACKET_CALL

DESCRIPTION
  Initiate packet data call by looking at the context ID for which this call 
  makes sense

DEPENDENCIES
  None

RETURNS
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_initiate_packet_call
(
  dsm_item_type            *res_buff_ptr
)
{
  tokens_struct_type        cgdata_tok_ptr;
  char                      pkt_dial_string[15];
  uint8                     used_buffer = 0;

  /* Dial the packet call - 
     Reusing AT command ATD call functionality */

  /* Suppressing the L2P part as we support PPP alone for now 
     and atd doesn't expect this */
  used_buffer = (uint8) snprintf(pkt_dial_string, 
                                       sizeof(pkt_dial_string), "*%d***" ,
                                       DSATETSIPKT_GPRS_SERVICE_CODE);

  /* Activate PDP context as defined in +CGDATA command earlier */
  if(used_buffer < 15)
  {
    used_buffer += (uint8)snprintf(&pkt_dial_string[used_buffer],
                                       sizeof(pkt_dial_string)-used_buffer,
                                       "%d#",
                                       dsat_cgdata_context.cid);
  }

  memset((void*)&cgdata_tok_ptr, 0, sizeof(tokens_struct_type));

  /* Only tok_ptr's arg[0] and end_of_line fields are of interest for 
     dial atd function  */

  cgdata_tok_ptr.arg[0] = (byte *)pkt_dial_string;
  cgdata_tok_ptr.end_of_line = (byte *)(pkt_dial_string + 
                                        strlen(pkt_dial_string));

  return dsatact_exec_atd_cmd(dsatcmdp_at_state, 
           NULL, &cgdata_tok_ptr, res_buff_ptr);
} /* etsipkt_initiate_packet_call */

/*===========================================================================
FUNCTION  ETSIPKT_CLEAR_GPRS_EVENTS_BUFFER

DESCRIPTION
  Clears the GPRS event buffer queue

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void etsipkt_clear_gprs_events_buffer
(
 sys_modem_as_id_e_type subs_id
)
{
  etsipkt_gprs_msg_type  *msg_ptr;
  dsatetsipkt_gprs_msg_ms_info     *gprs_dd_ms_val = NULL;

  gprs_dd_ms_val = dsat_get_base_addr_per_subs(DSAT_MS_MD_VALS, subs_id, FALSE);

  /* Get messages from the queue until the queue is empty */
  while( (msg_ptr = (etsipkt_gprs_msg_type *)
                    q_get( &gprs_dd_ms_val->etsipkt_gprs_msg_q )) != NULL )
  {
    /* Free the message buffer */
    dsatutil_free_memory(msg_ptr);
  }
} /* etsipkt_clear_gprs_events_buffer */

/*===========================================================================
FUNCTION  DSATETSIPKT_FLUSH_GPRS_EVENT_INDICATIONS

DESCRIPTION
  Flushes the buffered GPRS event reports to TE

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsipkt_flush_gprs_event_indications
(
  sys_modem_as_id_e_type subs_id
)
{
  dsm_item_type          *res_buff_ptr;
  etsipkt_gprs_msg_type  *msg_ptr;
  dsatetsipkt_gprs_msg_ms_info     *gprs_dd_ms_val = NULL;

  gprs_dd_ms_val = dsat_get_base_addr_per_subs(DSAT_MS_MD_VALS, subs_id, FALSE);

  /* Get messages from the queue until the queue is empty */
  while( (msg_ptr = (etsipkt_gprs_msg_type *)
                    q_get( &gprs_dd_ms_val->etsipkt_gprs_msg_q )) != NULL )
  {
    res_buff_ptr = dsat_dsm_new_buffer(DSM_DS_LARGE_ITEM_POOL, TRUE);
    if (NULL != res_buff_ptr)
    {
      (void) dsatutil_memscpy((void*)res_buff_ptr->data_ptr,
              res_buff_ptr->size,(void*) msg_ptr->event_data,
              MIN(msg_ptr->event_data_len,GPRS_EVENT_DATA_SIZE));
    
      res_buff_ptr->used =  MIN(msg_ptr->event_data_len,GPRS_EVENT_DATA_SIZE);
      (void)dsatcmdp_send_respose_to_te (res_buff_ptr, DSAT_COMPLETE_RSP);
    }

    /* Free the message buffer */
    dsatutil_free_memory(msg_ptr);
  }
} /* dsatetsipkt_flush_gprs_event_indications */

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGCMOD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGCMOD command.
  This command modifies the Qos, TFT and APN of active PDP contexts.  

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
/* ARGSUSED */
dsat_result_enum_type dsatetsipkt_exec_cgcmod_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  /*---------------------------------------------------------
    Processing for a WRITE command
  -----------------------------------------------------------*/
  if ( tok_ptr->op == (NA|EQ|AR) )
  {
    /* Build the processing order for CID list */
    if ( !etsipkt_build_context_processing_list(tok_ptr, 
                                                DSAT_CQM_CGCMOD_WRITE, 
                                                NULL) )
    {
      DS_AT_MSG0_ERROR("+CGCMOD <cid> list contained invalid parameter");
      return DSAT_ERROR;
    }

    if(dsat_pdp_state.valid_profile_count == 0)
    {
      memset ((void*)&dsat_pdp_state, 0x0,sizeof(dsat_pdp_state));
      DS_AT_MSG0_ERROR("Empty CGCMOD processing order list");
      return DSAT_OK;
    }
    
      result = ds_3gpp_rmsm_atif_cgcmod_handler(
                 dsat_pdp_cid[dsat_pdp_state.change_index],
                 subs_id,
                   NULL);

      if(result == DSAT_ASYNC_CMD)
      {
        DS_AT_MSG1_MED("CGCMOD processing in progress, CID = %d",
                dsat_pdp_cid[dsat_pdp_state.change_index]);
      dsat_pdp_state.pending = TRUE;
    }
  }
  /*---------------------------------------------------------
    Processing for a TEST command
  -----------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    ds_3gpp_atif_profile_status_info_type profile_status;
    byte num_active;
    uint8 loop;

    memset(&profile_status,0,sizeof(profile_status));

    if(DSAT_OK != ds_3gpp_rmsm_atif_cgact_query(&profile_status, subs_id))
    {
      DS_AT_MSG0_ERROR("CGACT query failed for CGCMOD query");
      return DSAT_ERROR;
    }
    else if (profile_status.profile_count > DSAT_MAX_ALLOWED_PROFILES)
    {
      DS_AT_MSG0_ERROR("Invalid Profiles provided by 3GPP ATIF");
      return DSAT_ERROR;
    }

    res_buff_ptr->used = (uint16)snprintf( (char *)res_buff_ptr->data_ptr, 
                                       res_buff_ptr->size,
                                       "+CGCMOD: (" );

  /* Go through connect state table and add 
     CID of each active context to response list... */

    DS_AT_MSG1_HIGH("CGCMOD query Total_Profile: %d ",profile_status.profile_count);
    for (loop = 0,num_active = 0; loop < profile_status.profile_count; loop++) 
    {     
      DS_AT_MSG1_HIGH("CGCMOD query executing, profile_num: %d ",profile_status.list[loop].profile_num);

  	/* Ensure context is valid OR currently active */
      if( profile_status.list[loop].act_status != DS_3GPP_ATIF_PDP_ACTIVATED)
      {
        DS_AT_MSG1_HIGH("MHM CGCMOD lnvalid profile_num: %d ",profile_status.list[loop].profile_num);
         continue;
      }
      
      if(profile_status.list[loop].act_status == DS_3GPP_ATIF_PDP_ACTIVATED)
      {
        res_buff_ptr->used += 
          (uint16)snprintf( (char *)(&res_buff_ptr->data_ptr[res_buff_ptr->used]),
                        res_buff_ptr->size - res_buff_ptr->used,
                          "%lu,", profile_status.list[loop].profile_num );
        num_active++;
      }
    }
  /* Step back over last comma */
    if ( num_active )
    {
      res_buff_ptr->used--;
    }

    res_buff_ptr->used += (uint16)
      snprintf( (char *)(&res_buff_ptr->data_ptr[res_buff_ptr->used]),
                    res_buff_ptr->size - res_buff_ptr->used,
                    ")" );
    return DSAT_OK;
  }
  else
  {
    /* unsupported operation */
    result = DSAT_ERROR;
  }
  return result;
} /* dsatetsipkt_exec_cgcmod_cmd */


#ifdef FEATURE_DATA_TE_MT_PDP
/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGAUTO_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGAUTO command.
  This command disables or enables an automatic positive response 
  (auto-answer) to the receipt of a Request PDP Context Activation message 
  from the network.
  
DEPENDENCIES
  None.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsipkt_exec_cgauto_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  #define DSAT_CGATT_VAL_0   0
  dsat_result_enum_type result = DSAT_OK;
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);


  if (DSAT_OK != dsatparm_exec_param_cmd(
                                         mode,
                                         parse_table,
                                         tok_ptr,
                                         res_buff_ptr
                                        ))
  {
    return DSAT_ERROR;
  }

/*---------------------------------------------------------
  Processing for a WRITE command
-----------------------------------------------------------*/
  if ( (tok_ptr->op == (NA|EQ|AR)) && 
       ((dsat_num_item_type)dsatutil_get_val(DSATETSI_EXT_CGAUTO_IDX,
                               0,0,NUM_TYPE) == DSAT_CGAUTO_AUTO_ANS_ON ))
  {
#ifdef FEATURE_ETSI_ATTACH
    /* if packet domain is detached attempt PS attach */
    if ( DSAT_CGATT_VAL_0 == (dsat_num_item_type)dsatutil_get_val(
                                  DSATETSI_EXT_ACT_CGATT_ETSI_IDX,subs_id,0,NUM_TYPE))
    {
      result = dsatetsicmif_attach_ps ();
    }
    else
    {
      DS_AT_MSG0_HIGH("Already packet domain attached");
    }
#endif /* FEATURE_ETSI_ATTACH */
  }

  return result;
} /* dsatetsipkt_exec_cgauto_cmd */


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGANS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGANS command.
  This command is used to answer a incoming MT PDP call.
  
DEPENDENCIES
  None.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_NO_RSP : if answer cb is called.
    DSAT_OK : if the test command succeeds.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsipkt_exec_cgans_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  
#define RESPONSE_TYPE_MAX 2
  dsat_result_enum_type result = DSAT_OK;
  dsat_pdp_answer_data_type  answer_data;
  dsat_num_item_type response_type = 0;
  dsat_num_item_type cid;
  uint8 index =0;
  uint32 flag=0;
  dsat_error_info_s_type    err_info;

  err_info.errval = DSAT_ERR_NO_ERROR;
  err_info.arg_num = 0;
  /*---------------------------------------------------------
    Processing for a WRITE command
  -----------------------------------------------------------*/
  if ( ( mode == DSAT_CMD ) && (tok_ptr->op == (NA|EQ|AR)) )
  {
    if ( (dsat_num_item_type)dsatutil_get_val( DSATETSI_EXT_CGAUTO_IDX,
                                     0,0,NUM_TYPE) == DSAT_CGAUTO_AUTO_ANS_ON )
    {
      DS_AT_MSG0_ERROR(" +CGANS not allowed when +CGAUTO = 1");
      return  dsat_send_cme_error(DSAT_CME_OP_NOT_ALLOWED);
    }
    if (VALID_TOKEN(0))
    {
      /* Convert response parameter string to integer */
      if (ATOI_OK !=
          dsatutil_atoi(&response_type, tok_ptr->arg[0], 10))
      {
        err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
        err_info.arg_num = 0;
        goto send_error;
      }

      if ((dsat_num_item_type)RESPONSE_TYPE_MAX <= response_type)
      {
        err_info.errval = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
        err_info.arg_num = 0;
        goto send_error;
      }
      if( response_type == 0) 
      {
        /* MT PDP request is rejected when response type = 0 */
        if ( dial_string_handler.pdp_hangup_cb != NULL )
        {
          dial_string_handler.pdp_hangup_cb ();
          result = DSAT_ATH_CMD;
        }
        else
        {
          DS_AT_MSG0_HIGH("Unregistered MT PDP hangup cb handler");
          result = DSAT_NO_CARRIER;
        }
      }
      else 
      {
        /* MT PDP request is accepted if response_type = 1 */
        /* continue processing if there is a valid pdp_answer_cb */
        if ( VALID_TOKEN(1))
        {
          byte out_string[4];
          word str_len = sizeof(out_string);

           /* Verifying for lower and upper case */

           if(dsatutil_strip_quotes_out((const byte *)tok_ptr->arg[1], 
                                   out_string, str_len) != TRUE)
           {
             err_info.errval = DSAT_ERR_QUOTE_STRIP_FAILURE;
             err_info.arg_num = 1;
             goto send_error;
           }

          if (dsatutil_strcmp_ig_sp_case((const byte *)out_string, 
                                     (const byte *)"PPP") != 0)
          {
            /* A different L2P is requested. We currently do not support it */
            return dsat_send_cme_error(DSAT_CME_OP_NOT_SUPPORTED);
          }
        } /* VALID_TOKEN(1) */
        memset ((void*)&answer_data, 0, sizeof(answer_data));
        /* the first param is response_type and second L2P type */
        for(index = 2;index < tok_ptr->args_found ;index++) 
        {
          if(VALID_TOKEN(index))
          {
            byte *parm_str = tok_ptr->arg[index];
            err_info.arg_num = index;
            if (ATOI_OK !=
              dsatutil_atoi(&cid, parm_str, 10))
            {
              err_info.errval = DSAT_ERR_ATOI_CONVERSION_FAILURE;
              err_info.arg_num = index;
              goto send_error;
            }
            /* Test for CID within valid range */
            if (!DSAT_IS_PROFILE_ID_WITHIN_RANGE(cid))
            {
              err_info.errval = DSAT_ERR_PARAMETER_OUT_OF_RANGE;
              goto send_error;
            }
            if( answer_data.number_of_profiles < DSAT_MAX_ALLOWED_PROFILES)
            {
              /* check for duplicate cid entered */
              if(!(flag & (1<<cid)))
              {
                flag |= (1<<cid);
                answer_data.profile_number[answer_data.number_of_profiles]= cid;
                answer_data.number_of_profiles++;
              }
            }
          }
        } /* end of for loop */
        
        /* return NO_CARRIER if no handler is registered */
        if ( dial_string_handler.pdp_answer_cb == NULL ) 
        {
          DS_AT_MSG0_HIGH("Unregistered MT PDP answer cb handler");
          return DSAT_NO_CARRIER;
        }
        else
        {
          result = DSAT_NO_RSP;

          /* Slam the modem port to the current active port */
          ds3g_siolib_set_answer_port ( DS3G_SIOLIB_ANSWER_MODE_MANUAL );
          dsat_curr_abort_handler = 
            dial_string_handler.pdp_answer_cb (&answer_data);

          if ( dsat_curr_abort_handler == NULL )
          {
            DS_AT_MSG0_HIGH("unregistered +CGANS Abort handler");
          }
        }
      } /* response type = 1 */
    } /* VALID_TOKEN(0) */
    else
    {
      result = DSAT_ERROR;
    }
  }/* end of processing of write command */
  
  /*---------------------------------------------------------
    Processing for a TEST command
  -----------------------------------------------------------*/
  else if (tok_ptr->op == (NA|EQ|QU))
  {
    result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );
  }
  else
  {
    err_info.errval = DSAT_ERR_INVALID_SYNTAX;
    goto send_error;
  }
  return result;

send_error:
  if( err_info.errval == DSAT_ERR_INVALID_SYNTAX)
  {
    DS_AT_MSG1_ERROR("Error:%d",err_info.errval);
  }
  else
  {
    DS_AT_MSG2_ERROR("Error:%d, arg_num:%d",err_info.errval,err_info.arg_num);
  }
  return DSAT_ERROR;

} /* dsatetsipkt_exec_cgans_cmd */

#endif /* FEATURE_DATA_TE_MT_PDP */

/*===========================================================================
FUNCTION DSATETSIPKT_QCDEFPROF_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the $QCDEFPROF read command.
  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_num_item_type dsatetsipkt_qcdefprof_response_fmt
(
  const dsati_cmd_type  *parse_table,   /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr   /*  Place to put response       */
)
{
  ds_profile_tech_etype   tech   = DS_PROFILE_TECH_3GPP;
  uint32                  family;
  ds_profile_subs_etype  subs_id ;
  ds_profile_num_type     prof_num = (ds_profile_num_type)0;
  dsm_item_type *         res_buff_ptr = *out_buff_ptr;

  for( family = DS_PROFILE_EMBEDDED_PROFILE_FAMILY; 
       family <= DS_PROFILE_TETHERED_PROFILE_FAMILY; 
       family++ )
  {
    for( subs_id =DS_PROFILE_ACTIVE_SUBSCRIPTION_1; 
         subs_id < DS_PROFILE_ACTIVE_SUBSCRIPTION_MAX; 
         subs_id++ )
    {
#ifdef FEATURE_DUAL_SIM
      if( ds_profile_get_default_profile_num_per_subs (
                     tech, family, subs_id, &prof_num ) !=
          DS_PROFILE_REG_RESULT_SUCCESS )
      {
        DS_AT_MSG0_MED("Problem reading profile num");
        continue;
      }
#else
      if( subs_id != DS_PROFILE_ACTIVE_SUBSCRIPTION_1)
      {
        continue;
      }
      if( ds_profile_get_default_profile_num_per_subs (
                     tech, family, subs_id, &prof_num ) !=
          DS_PROFILE_REG_RESULT_SUCCESS )
      {
        DS_AT_MSG0_MED("Problem reading profile num");
        continue;
      }
#endif /* FEATURE_DUAL_SIM */   
      res_buff_ptr->used += snprintf
                              (
		                              (char *)&res_buff_ptr->data_ptr[res_buff_ptr->used],
	 	                             res_buff_ptr->size,
                                "%s: %d,%d,%d\n",
                                parse_table->name,
                                (int)family,
                                subs_id,
                                (int)prof_num 
                               );
      DS_AT_MSG1_HIGH("res_buff_ptr->used = %d",res_buff_ptr->used);
    }
  }
  res_buff_ptr->data_ptr[res_buff_ptr->used-1] = '\0';
  return ((dsat_num_item_type)res_buff_ptr->used);
} /* dsatetsipkt_qcdefprof_response_fmt */

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_QCDEFPROF_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes $QCDEFPROF command. This command is used to set the default 
  profile number for specific subscription and family.


DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsipkt_exec_qcdefprof_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type   result   = DSAT_OK;
  uint32                  family;
  ds_profile_subs_etype   subs_id  = DS_PROFILE_ACTIVE_SUBSCRIPTION_NONE;
  ds_profile_num_type     prof_num = (ds_profile_num_type)0;
  dsat_num_item_type      data_len = (dsat_num_item_type)0;
  dsat_num_item_type      temp;

  /*---------------------------------------------------------
    Processing for a command with an argument (WRITE command)
    Processing for TEST command 
  -----------------------------------------------------------*/
  if ((tok_ptr->op == (NA|EQ|AR)) ||
      (tok_ptr->op == (NA|EQ|QU)))
  {
    result = dsatparm_exec_param_cmd(
                                     mode,
                                     parse_table,
                                     tok_ptr,
                                     res_buff_ptr
                                    );
    
  }
  /*--------------------------------------
    Processing for a READ command
  ----------------------------------------*/
  else if (tok_ptr->op == (NA|QU) )
  {
    /* Report for read command */
    data_len = dsatetsipkt_qcdefprof_response_fmt(parse_table, &res_buff_ptr);
    if (0 == data_len)
    {
      DS_AT_MSG0_ERROR("Problem generating query response");
      result = DSAT_ERROR;
    }
  }
  else
  {
    /* wrong operation */
    result = DSAT_ERROR;
  }
  if ( result != DSAT_ERROR)
  {
    if (tok_ptr->op == (NA|EQ|AR))
    {
      temp     = (dsat_num_item_type)dsatutil_get_val(
                           DSAT_VENDOR_QCDEFPROF_IDX,0,0,NUM_TYPE);
      family   = (uint32)temp;
      temp     = (dsat_num_item_type)dsatutil_get_val(
                           DSAT_VENDOR_QCDEFPROF_IDX,0,1,NUM_TYPE);
      subs_id  = (ds_profile_subs_etype)temp;
      temp     = (dsat_num_item_type)dsatutil_get_val(
                           DSAT_VENDOR_QCDEFPROF_IDX,0,2,NUM_TYPE);
      prof_num = (ds_profile_num_type)temp;
#ifdef FEATURE_DUAL_SIM
      if ( ds_profile_set_default_profile_num_per_subs (
                      DS_PROFILE_TECH_3GPP, family, subs_id, prof_num ) !=
           DS_PROFILE_REG_RESULT_SUCCESS )
#else
      if ( subs_id != DS_PROFILE_ACTIVE_SUBSCRIPTION_1 ||
          (ds_profile_set_default_profile_num_per_subs (
                      DS_PROFILE_TECH_3GPP, family, subs_id, prof_num ) !=
           DS_PROFILE_REG_RESULT_SUCCESS) )
#endif /* FEATURE_DUAL_SIM */
      {
        DS_AT_MSG3_ERROR("Could not set profile parameters. Subs_id = %d family  = %d "
                  "profile number = %d", (int)subs_id, (int)family, (int)prof_num);
        result = DSAT_ERROR;
      }
    }
  }
  return result;
} /* dsatetsipkt_exec_qcdefprof_cmd */

#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DSAT_LTE

/*===========================================================================
FUNCTION DSATETSIPKT_EXEC_CEMODE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CEMODE command.
  The set command is used to set the MT to operate according to the specified 
  mode of operation for EPS. The read command returns the mode of operation set
  by the TE, independent of the current serving cell capability and independent
  of the current serving cell Access Technology. The test command is used for 
  requesting information on the supported MT mode of operation.
  
DEPENDENCIES
  None.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CME_OP_NOT_SUPPORTED : in case of failure
    DSAT_OK : if the command succeeds.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsipkt_exec_cemode_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_ERROR;
  if(tok_ptr->op == (NA|QU) || (tok_ptr->op == (NA|EQ|QU))) 
  {
    result = dsatparm_exec_param_cmd(
                                   mode,
                                   parse_table,
                                   tok_ptr,
                                   res_buff_ptr
                                  );
  }
  if (DSAT_OK != result)
  {
    /* In case of error return operation not supported */
    result = dsat_send_cme_error(DSAT_CME_OP_NOT_SUPPORTED);
  }
  return result;
}/* dsatetsipkt_exec_cemode_cmd */

#endif /* FEATURE_DSAT_LTE */

/*===========================================================================

FUNCTION DSATETSIPKT_GET_PDP_PROFILE_SECONDARY_FLAG

DESCRIPTION
  This function verfiies the secondary flag of passed profile and updates 
  secondary flag 

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the profile parameter read status.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */

ds_profile_status_etype dsatetsipkt_get_pdp_profile_secondary_flag
(
  uint16 profile_number,
  ds_profile_3gpp_pdp_context_secondary_flag_type* secon_flag
)
{
  ds_profile_status_etype           profile_status  = DS_PROFILE_REG_RESULT_FAIL;
  ds_profile_status_etype           read_param_status = DS_PROFILE_REG_RESULT_FAIL;
  ds_profile_hndl_type              profile_hndl = NULL;
  ds_profile_info_type              param_info;

  memset(&param_info,0,sizeof(ds_profile_info_type));
  
  DSAT_BEGIN_PROFILE_TRANSACTION(profile_number,DS_PROFILE_TECH_3GPP,profile_hndl);
  
  if(DS_PROFILE_REG_RESULT_SUCCESS != profile_status || profile_hndl == NULL)
  {
    return profile_status;
  }
  
  param_info.len     = sizeof(ds_profile_3gpp_pdp_context_secondary_flag_type);
  param_info.buf     = (void*)secon_flag;
  profile_status     = dsat_profile_get_param(profile_hndl, 
                                              DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_SECONDARY_FLAG,
                                              param_info);
  
  /*Cached reading status and will return read status; End transaction will update profile_status */
  read_param_status = profile_status;

  DSAT_END_PROFILE_TRANSACTION(profile_hndl);

  DS_AT_MSG2_HIGH("get_pdp_profile_secondary_flag status:%d flag = %d",
                      read_param_status,*secon_flag);
  
  return read_param_status;
}/* dsatetsipkt_get_pdp_profile_secondary_flag */


/*===========================================================================

FUNCTION DSATETSIPKT_IS_PROFILE_CONTEXT_VALID

DESCRIPTION
  This function verfiies whether passed profile number and Tech type pair
  is valid or not.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns true if passed profile is valid else returns false.

SIDE EFFECTS
  None
  
===========================================================================*/
 /* ARGSUSED */
boolean dsatetsipkt_is_profile_context_valid
(
  uint16 profile_number,
  ds_profile_tech_etype tech,
  ds_profile_subs_etype ps_subs
)
{
  boolean is_valid = FALSE;
  ds_profile_status_etype status = ds_profile_is_valid_profile_per_sub( profile_number,
                                                                        ps_subs,
                                                                        tech,
                                                                        &is_valid);

  if( status != DS_PROFILE_REG_RESULT_SUCCESS )
  {
    DS_AT_MSG4_HIGH("Valid profile check API fail: status:%d, PID:%d, TECH:%d, ds_subs:%d",
                                                                              status,
                                                                              profile_number,
                                                                              tech,
                                                                              ps_subs);
    return FALSE;
  }
  else
  {
    if ( FALSE == is_valid )
    {
      DS_AT_MSG2_HIGH("PID:%d, TECH:%d pair is invalid",profile_number,
                                                        tech);
      return FALSE;
    }
  }

    return TRUE;

}/* dsatetsipkt_is_profile_context_valid */

/*===========================================================================

FUNCTION DSATETSIPKT_READ_WRITE_PARAM_TABLE

DESCRIPTION
  This function Reads,writes,resets parameters from passed table.
  It calls begin and end transaction API's to process parameters.
 
DEPENDENCIES
  Argument pdata must be a packed structure to read and write appropriate values.
  
RETURN VALUE
  dsat_result_enum_type 

SIDE EFFECTS
  None
  
===========================================================================*/
 /* ARGSUSED */
dsat_result_enum_type dsatetsipkt_read_write_param_table
(
  const atcop_profile_3gpp_param_list_type* table_ptr,
  uint16                                    array_size,
  uint16                                    profile_number,
  dsat_pdp_command_type                     cmd_type,
  byte*                                     pdata,
  uint16                                    max_data_size,
  ds_profile_subs_etype                     ps_subs
)
{
  uint8 index = 0,param_offset = 0;
  ds_profile_status_etype           profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
  ds_profile_hndl_type              profile_hndl;
  ds_profile_info_type              param_info;
  ds_profile_tech_etype             tech_type      = DS_PROFILE_TECH_INVALID;

  if((cmd_type != RESET_COMMAND) && (table_ptr == NULL || pdata == NULL))
  {
    DS_AT_MSG0_ERROR("invalid table or data");
    return DSAT_ERROR;
  }
  
  if ( dsatetsipkt_is_profile_context_valid( profile_number,
                                             DS_PROFILE_TECH_EPC,
                                             dsat_get_current_ds_profile_subs_id() ))
  {
    tech_type = DS_PROFILE_TECH_EPC;
  }
  else if ( dsatetsipkt_is_profile_context_valid( profile_number,
                                                  DS_PROFILE_TECH_3GPP,
                                                  dsat_get_current_ds_profile_subs_id() ))
  {
    tech_type = DS_PROFILE_TECH_3GPP;
  }
  else if ( dsatetsipkt_is_profile_context_valid( profile_number,
                                                  DS_PROFILE_TECH_3GPP2,
                                                  dsat_get_current_ds_profile_subs_id() ))
  {
    tech_type = DS_PROFILE_TECH_3GPP2;
  }
  else
  {
    DS_AT_MSG0_ERROR("Invalid Tech Type");
    return DSAT_ERROR;
  }
  
  memset(&profile_hndl, 0, sizeof(profile_hndl));
  memset(&param_info, 0, sizeof(param_info));

  DSAT_BEGIN_PROFILE_TRANSACTION(profile_number,tech_type,profile_hndl);
  
  if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS || profile_hndl == NULL)
  {
    return DSAT_ERROR;
  }
  
  while(index < array_size)
  {
    if(cmd_type == READ_COMMAND || cmd_type == WRITE_COMMAND)
    {
      memset(&param_info, 0, sizeof(param_info));

      param_info.buf = (void*)(pdata + param_offset);
      param_info.len = table_ptr[index].param_len;
    
      if( cmd_type == READ_COMMAND)
      {
        profile_status     = dsat_profile_get_param(profile_hndl, 
                                                    table_ptr[index].param_type,
                                                    param_info);
      }
      else if(cmd_type == WRITE_COMMAND)
      {
        profile_status     = dsat_profile_set_param(profile_hndl,
                               table_ptr[index].param_type,
                               param_info);       
      }
      
      param_offset = param_offset + table_ptr[index].param_len;
      
      DS_AT_MSG1_HIGH("param_offset = %d",param_offset);
      
      if(param_offset > max_data_size)
      {
        DS_AT_MSG2_HIGH("boundary crossed param_offset = %d size = %d",param_offset,
                                           max_data_size);
        DSAT_END_PROFILE_TRANSACTION(profile_hndl);
        return DSAT_ERROR;
      }
      
    }
    else if(cmd_type == RESET_COMMAND)
    {
      profile_status = ds_profile_reset_param_to_default_per_sub(tech_type,
                                                                 profile_number,
                                                                 table_ptr[index].param_type,
                                                                 ps_subs);
    }
    
    if( profile_status != DS_PROFILE_REG_RESULT_SUCCESS && 
        table_ptr[index].param_type != DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V6  && 
        table_ptr[index].param_type != DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4  )
    {
       DS_AT_MSG3_ERROR("failed for cmd_type = %d param %d status %d",cmd_type,
                                     table_ptr[index].param_type,profile_status);
       
       DSAT_END_PROFILE_TRANSACTION(profile_hndl);
       return DSAT_ERROR;
    }
    
    index++;
  }

  DSAT_END_PROFILE_TRANSACTION(profile_hndl);
  return DSAT_OK;
}/*dsatetsipkt_read_write_param_table*/

/*===========================================================================

FUNCTION DSATETSIPKT_READ_WRITE_PROFILE_PARAM

DESCRIPTION
Based on the command type this function reads or writes passed profile 
param_type
 
DEPENDENCIES
  
RETURN VALUE
  dsat_result_enum_type 

SIDE EFFECTS
  None
  
===========================================================================*/
    /* ARGSUSED */
dsat_result_enum_type dsatetsipkt_read_write_profile_param
(
  uint16 profile_number,
  dsat_pdp_command_type cmd_type,
  ds_profile_identifier_type param_type,
  void* data,
  uint16 data_size
)
{
    dsat_result_enum_type        result = DSAT_OK;
    ds_profile_status_etype      profile_status = DS_PROFILE_REG_RESULT_SUCCESS;
    ds_profile_hndl_type         profile_hndl = NULL;
    ds_profile_info_type         param_info;


    ds_profile_tech_etype             tech_type = DS_PROFILE_TECH_INVALID;

    if(data == NULL || data_size == 0)
    {
      DS_AT_MSG0_ERROR("Invalid data or data size");
      return DSAT_ERROR;
    }
    
    /*If tech_type is invalid, return false as no profile exists for 
      that tech with input profile_number*/

   if ( dsatetsipkt_is_profile_context_valid( profile_number,
                                               DS_PROFILE_TECH_3GPP,
                                               dsat_get_current_ds_profile_subs_id()))
   {
     tech_type = DS_PROFILE_TECH_3GPP;
   }
   else if ( dsatetsipkt_is_profile_context_valid( profile_number,
                                                   DS_PROFILE_TECH_3GPP2,
                                                   dsat_get_current_ds_profile_subs_id()))
   {
     tech_type = DS_PROFILE_TECH_3GPP2;
   }
   else if ( dsatetsipkt_is_profile_context_valid( profile_number,
                                                   DS_PROFILE_TECH_EPC,
                                                   dsat_get_current_ds_profile_subs_id()))
   {
     tech_type = DS_PROFILE_TECH_EPC;
   }
   else
    {
     DS_AT_MSG1_ERROR("Cannot get tech_type for profile id %d ",
                       profile_number);
      return DSAT_ERROR;
    }
    
    DSAT_BEGIN_PROFILE_TRANSACTION(profile_number, tech_type,profile_hndl);
    
    memset(&param_info,0,sizeof(ds_profile_info_type));

    param_info.buf = data;
    param_info.len = data_size;
    
    if(profile_status == DS_PROFILE_REG_RESULT_SUCCESS && cmd_type == READ_COMMAND)
    {
      profile_status     = dsat_profile_get_param(profile_hndl, 
                                                  param_type,
                                                  param_info);
    }
    else if(profile_status == DS_PROFILE_REG_RESULT_SUCCESS && cmd_type == WRITE_COMMAND)
    {
      profile_status     = dsat_profile_set_param(profile_hndl,
                             param_type,
                             param_info);       
    }
    else if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      return DSAT_ERROR;
      
    }else
    {
      result =  DSAT_ERROR;
    }
    
    if(profile_status != DS_PROFILE_REG_RESULT_SUCCESS)
    {
      DS_AT_MSG3_ERROR("CMD %d Operation failed for param %d with status%d",
                      cmd_type,param_type,profile_status);
      result = DSAT_ERROR;
    }
    DSAT_END_PROFILE_TRANSACTION(profile_hndl);
    return result;
}/*dsatetsipkt_read_write_profile_param*/
