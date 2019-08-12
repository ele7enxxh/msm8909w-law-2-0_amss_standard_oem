/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

                 E T S I   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the AT modem commands that are specific to ETSI mode of
  operation.

EXTERNALIZED FUNCTIONS
  None
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatetsictab.c_v   1.12   12 Jul 2002 10:06:12   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatetsictab.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/11/14   tk      Fixed compilation errors in LTE compile out flavor.
06/27/14   sc      Added support for Dynamic ATCoP.
11/13/13   sc      Added support for $QCCSGCOPS command.
09/25/13   sc      Added support for +CNMPSD command.
11/30/12   tk      ATCoP changes for Triton DSDA.
09/25/12   ss      3GPP MH LTE-Off compilation fixes
08/10/12   tk      Deprecated ATCoP - DIAG interface.
06/30/12   sk      Added UMTS_CMD check.
03/07/12   sk      Made CLCC common for all modes.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
02/06/12   ad      Added CSTF/CEN/CGPIAF/CVMOD command support.
01/19/12   sk      Feature cleanup.
10/20/11   nc      Added support for CUAD/CEAP/CERP commands.
10/17/11   ad      Added +CECALL support.
07/27/11   ttv     Added changes required for source statistics descriptor
                   addition.
05/23/11   ad      Allowed to set Packet Filter ID from 1 to 16.
05/03/11   ad      Allowed +CHUP command in RESTRICTED mode.
04/05/11   ad      Fixed +CPOL issue. 
03/29/11   nc      Fixed +VTS command.
03/01/11   nc      Modified +CEMODE to be inline with CM Design.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
06/04/10   ua      Removing support of CKPD and HS services for all 
                   THIN UI targets.
31/01/10   ad      Adding new fields in +CPBW Commands .
05/15/10   nc      Added support fot +CGCONTRDP,+CGSCONTRDP,
                   +CGEQOSRDP and +CGTFTRDP for LTE.
05/17/10   ad      Added Support for +CEREG for LTE.
05/10/10   kk      Mainlining pending GOBI changes.
03/15/10   bs      Added +CQI support.
03/18/10   nc      Added Support for +CGEQOS for LTE.
02/10/10   kk      Moved +CKPD out of featurization.
02/08/10   ua      Removing support for CLVL and CMUT. 
01/20/10   kk      Moved +CFUN to CommonATCoP.
11/18/09   sa      Moved +CMEE command to dsatctab.
12/15/09   nc      Featurisation changes for LTE.
12/11/09   bs      Added +COPS support for LTE.
12/07/09   nc      Added Support for new parameters in +CPOL, +CGDCONT for LTE.
04/20/09   sa      Feature wrapped +CKPD,+CLVL and +CMUT for Winmobile and Linux.
08/06/09   sa      Added +CSIM and +CRSM to restricted category.
04/29/09   ua      Fixed compiler warnings. 
04/16/09   sa      Fixed time range for +CCFC command as per 27.007 Spec.
12/29/08   nc      Added support for +VTS Command
07/17/08   sa      Removed the support of +CKPD for thin-UI 8K builds.
04/14/08   ua      Removing support for +CLVL, +CMUT commands in 8K targets due
                   to non availabiltiy of SND support to ATCOP.
12/12/08   ua      Fixed Off target lint errors.
11/28/08   cs      Off-Target build Compiler and Lint warning fixes.
10/23/08   bs      Added support for 1X AT Phonebook commands.
10/13/08   bs      Fixing 6800 compilation issues.
07/21/08   bs      Added support for +CREG=2 command.
06/27/08   sa      Replaced LOCAL macro for DL/UL rate with UMTS PS macro.
03/03/08   bs      Fixing CSCA regression issues.
10/31/07   ua      Added support for +CSAS and +CRES commands. 
10/08/07   sa      Added feature wrapping FEATURE_GPRS_COMP_ENUM_TYPES
                   for h_comp and d_comp parameters.
08/24/07   ar      Fix feature wrapping for non-GCSD builds.
08/22/07   sa      Added support for +CGANS and +CGAUTO commands.
08/06/07   pp      Moved +CLVL, +CMUT commands to mainline.
08/02/07   sa      Feature wrap changes on header and data compression for +CGDCONT
                   and +CGDSCONT.
07/20/07   ss      Fixed a bug in atd>"<text>" command.
06/21/07   ua      Enabling +CKPD command for single processor THIN UI builds.
05/15/07   sa      +CMEE should be allowed in restricted mode as per spec.
05/07/07   sa      Correcting the index of FD in dsat_clck_fac_valstr and 
                   dsat_clck_fac_tststr.
04/14/07   pkp     Added support for +CCLK (Clock).
04/05/07   ua      Added support for COPS=2 (deregistration )
15/03/07   ua      CGCLASS modified to support only CLASS A
04/03/07   ua      Modified following commands behavior in GSM only builds
                   1. CGCLASS support CLASS B in GSM only and CLASS A in WCDMA
                   2. CPOL UTRAN AcT support is present in WCDMA only.
                   3. CGEQREQ, CGEQMIN are supported only for WCDMA.
                   4. COPS AcT parameter of 2 is supported in WCDMA
03/24/07   ua      Correcting Banned API changes. 
02/15/07   ua      Added new phone book "FD" support.
02/15/07   ss      Fixed lint high errors
02/12/07   ua      Removing support for +CKPD in THIN_UI builds
02/06/07   ua      Correcting <bm> parameter range for CNMI
09/29/06   snb     Increase MAX_QOS_UTMS_REQ/GTD_UL_BITRATE from 512 to 2048.
08/21/06   snb     Use dsatgsmfax_exec_param_cmd.
08/03/06   sa      Add header and data compression support for +CGDCONT and +CGDSCONT.
06/08/06   sa      Add support for +CLAC and modified +CTZU command
05/12/06   ua      CPLS, CPOL commands wrapped under FEATURE_MMGSDI
03/13/06   ua      CPOL command updated to use +CPLS command for sim file selection.
                   Increase parameters from 3 to 6 to support Access technology. 
02/21/06   sa      Merged from Sirius Branch
                   -Removed CLIR featurization.
01/19/06   ua      Add support CTFR command
01/16/06   ua      Add support CDIP command
01/09/06   ua      Add support +CPLS command
01/04/06   snb     +CFUN and +CIND(read and test cmd only) should be allowed
                   in restricted mode per spec.
12/08/05   snb     Add IPV6 support
11/30/05   snb     Add support for +CGCMOD command.
09/13/05   snb     Add 7 for offline mode to +CFUN's supported range.
08/26/05   snb     Don't reset +CFUN values for ATZ/&F.
08/15/05   snb     Add support for reset via +CFUN command.
07/15/05   snb     Increase MAX_QOS_UTMS_REQ/GTD_UL/DL_BITRATEs to 512kbps.
07/08/05   ar      Removed +CGPADDR parameter support.
06/21/05   sb      Bug fixes from direct routing of SMS STATUS-REPORT testing
06/07/05   dvp     Removed +CPWD 'static' type.
05/13/05   sb      Enable direct routing of SMS STATUS-REPORT
05/25/05   hap     Changed range for +CALM command
05/03/05   tkk     Changed cgpaddr limit to DS_UMTS_MAX_PDP_PROFILE_NUM.
04/29/05   ar      Add support for +CEER command.
04/28/05   snb     Cleanup +CGACT table entry and variables.
04/27/05   ar      Add support for +CV120 command.
04/20/05   iv      Removed +CRMP commands from #ifdef FEATURE_DSAT_PROFILE_CMD
04/19/05   tkk     Modified default value for +CMUT command.
04/12/05   ar      Extend +COPS interface to add access technology param.
04/05/05   ar      Adjust paramaters for +CMER and extend +CIND support.
04/01/05   tkk     Added support for commands requiring UI-ATCoP 
                   interaction 
04/01/05   ar      Add +CIND command support under FEATURE_DSAT_CIND_SUPPORT. 
03/30/05   snb     Change maximum and guaranteed DL bitrate QoS parms to 16000
                   kbps when FEATURE_HSDPA defined.
03/15/05   tkk     Removed featurization for +CGPADDR, +CGEREP and +CGDATA 
                   commands and unnecessary space for test strings.
03/15/05   snb     Remove +CSIM command from &V response, handle all processing
                   in command handler.
03/11/05   snb     Add +CRSM restricted SIM access command.
03/08/05   iv      Added support for +CRMP command
03/07/05   ar      Extend +CFUN capability for testing purposes.
03/07/05   ar      Adjust keypad emulation commands.
03/02/05   snb     Change maximum and guaranteed DL bitrate QoS parms to 1800
                   kbps when FEATURE_HSDPA defined.
02/28/05   hap     Added support for +CSSN command
02/22/05   iv      Modified +CPBS parameters to support "ON" phone book
02/21/05   dvp     Added support for Personalization facility in +CLCK
02/14/05   tkk     Added +CVIB support.
02/14/05   dvp     Added commands ^SGM[LR]
02/14/05   iv      Modified +CPBS parameters to support more phone books
02/02/05   tkk     Added support for +CLIR command and generalized abort 
                   handler to include (+CLIR, +COLP and +CLIP).
01/31/05   tkk     Added support for +CGDATA command.
02/03/05   iv      Added support for +CPOL command.
01/31/05   iv      Added support for +COPN command.
01/27/05   pdv     Merge from Sirius-ATCoP development.
01/27/05   pdv     Rebase from Mainline.
01/17/05   tkk     Added support for +CGEREP command.
01/10/05   tkk     Added support for +CGPADDR command.
01/06/05   snb     Change +CVHU default to 1.
01/05/05   iv      Added support for CLCC command
01/05/05   hap     Added support for +CNUM command
12/30/04   tkk     Added support for +CLIP, +COLP commands.
12/29/04   snb     Make dsat_csca_sca_val and dsat_csca_tosca_val external.
12/21/04   tkk     Added support for CCLK command.
12/15/04   snb     Add +CVHU command.
12/07/04   ar      Add support for +COPS command.
11/20/04   tkk     Added support +CTZR & +CTZU commands.
11/19/04   tkk     Added support for +CLVL and +CMUT commands.
11/18/04   snb     Add support for GSM and UCS2 character sets to +CSCS 
                   command, +CMOD command and additional parms for 
                   +CPMS <mem3>, +CGSMS.
11/05/04   ar      Add support for +CPUC.
10/25/04   ar      Adjust +CBST command parameter lists for FTM.
10/21/04   snb     Add support for GSM and UCS2 character sets to +CSCS command
10/15/04   ar      Add Advice of Charge support.
09/03/04   snb     Add support for +CSIM, +CHLD and +CMMS command.
07/06/04   snb     Add support for personalization PINs for ATCOP.
06/21/04   snb     Add support for 16 PDP profiles.
05/12/04   snb     Change maximum and guaranteed UL QoS limits to 384k.
04/12/04   ar      Adjust parameter lists for +FRM aand +FTM commands.
03/19/04   snb     Add +CCWA <n> parm and defaults, controlling display of 
                   unsolicited result code. 
01/21/04   sb      Move the generic DCE control commands (+CGMI, +CGMM, +CGMR
                   and +CGSN) to ETSI only table. IN CDMA mode they should be
                   treated as unrecongnized commands.
12/29/03   snb     Removed parameters supporting rate 12000 and 14400 from 
                   +FTM and +FRM.
11/19/03   ar      Added support for +CGDSCONT and +CGTFT commands.
10/29/03   snb     Added support for $QCPINSTAT command and for async IMSI 
                   read from either card slot by +CIMI command.
10/27/03   ar      Add +CFUN command support.
10/23/03   ar      Add speed vales to +CBST lists, adjust default speed.
10/15/03   ar      Add FEATURE_DSAT_ETSI_RLP2 wrapper for CSD RLP2 changes.
10/07/03   ar      Add +CREG and +CGREG commands support.
08/24/03   sb      Cell broadcast editions (first cut)
08/13/03   rsl     Moved at+fclass to dsatctab.c
07/31/03   ar      Change +CSTA to BROKEN_RANGE.
07/29/03   ar      Adjust value lists for +CHSN and +CRLP
07/11/03   snb     Remove LOCAL specifier from dsat_clck_fac_tststr.
06/09/03   ar      Added +CMER cmd handler dsatetsime_exec_cmer_cmd().
06/05/03   ar      Add featurization required for CDMA only MSM6300 build
05/28/03   sb      Fix for WCDMA CS Calls
05/14/03   snb     Change +CHSN suported values.
04/28/03   sb      New modified list for supported +CBST values.
04/25/03   snb     Split etsi extended command table into parm and action tables.
04/09/03   sb      Modified the values of <ds> in +CNMI command and added "SR"
                   memory type to <mem2> read memory 
04/04/03   sb      Enabled the routing of status reports in +CNMI
03/26/03   ar      Add +CMER, +CMEC, +CKPD command support.
03/14/03   ar      Add +CHSN command support. Add NO_RESET attrib to +CGATT.
03/07/03   ar      Remove FEATURE_DATA_ETSI_SUPSERV wrappers
02/25/03   wx      Use mixed_param to implement +ES's parameters
02/25/03   ar      Consolitate +CBST parameters into superset lists.
02/10/03   wx      Add support for +CGSMS command
02/03/03   ar      Add support for +CGACT command
01/30/03   ar      Add support for +CGEQMIN command
01/09/03   wx      Added support for +ES and +ESA
01/06/03   ar      Migrated context cmds to persistent PDP profile support 
12/09/02   ar      Added support for +CUSD command
12/09/02   sb      Added 134 to speed in +CBST command (for WCDMA mode)
12/02/02   ar      Added support for +CPWD command
11/21/01   ar      Added support for +CCUG and +CLCK commands
10/16/02   ar      Added support for +CCFC command
10/12/02   npr     Corrected GPRS dflt, min and max values for CGQMIN, CGQMAX
10/12/02   npr     Feature wrapped relevent code with FEATURE_GSM_GPRS
08/13/02   ar      Use defines for some QoS profile limits
07/11/02   ar      Add support for +CIMI , +CGQMIN commands
07/03/02   ar      Add support for +CGCLASS, +CGEREP, +CGATT commands
06/29/02   ar      Add support for +CGQREQ command
06/27/02   ar      Add support for +CGEQREQ command
04/08/02   sb      Add support for +CNMA command
04/01/02   ar      Add support for +CCPIN command and restricted attribute
02/04/02   ar      Add support for +CGDCONT command
08/27/01   sjd     Initial release of new VU structure to MSM5200 archives. 

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "dsati.h"
#include "dsatetsictab.h"
#include "dsatctab.h"
#include "dsatact.h"
#include "dsatparm.h"
#include "dsatetsicall.h"
#include "dsatetsicmif.h"
#include "dsatetsisms.h"
#include "dsatetsime.h"
#include "dsatetsicall.h"
#include "dsatsms.h"
#include "mn_cm_exp.h"

#ifdef FEATURE_DSAT_ETSI_DATA
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DATA_GCSD_FAX
#include "dsatgsmfax.h"
#endif  /* FEATURE_DATA_GCSD_FAX */

#ifdef FEATURE_DSAT_LTE
#include "ds_3gpp_qos.h"
#endif /* FEATURE_DSAT_LTE */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*--------------------------------------------------------------------------
                         ETSI Command Tables 
--------------------------------------------------------------------------*/

#ifdef FEATURE_DSAT_ETSI_MODE
/* ETSI extended AT command table */
const dsati_cmd_ex_type dsatetsi_ext_action_table_ex [] =
{
  { DSATETSI_EXT_ACT_CSQ_ETSI_IDX,   dsatetsime_exec_csq_cmd },

  { DSATETSI_EXT_ACT_CBC_ETSI_IDX,   dsatetsime_exec_cbc_cmd },

  { DSATETSI_EXT_ACT_CPAS_ETSI_IDX , dsatetsime_exec_cpas_cmd },

#ifdef FEATURE_DATA_ETSI_PIN 
  { DSATETSI_EXT_ACT_CPIN_ETSI_IDX,  dsatetsime_exec_cpin_cmd },
#endif /* FEATURE_DATA_ETSI_PIN */

  { DSATETSI_EXT_ACT_CMEC_ETSI_IDX,  dsatetsime_exec_cmec_cmd },

  { DSATETSI_EXT_ACT_CIND_ETSI_IDX,  dsatetsime_exec_cind_cmd },
  
  { DSATETSI_EXT_ACT_CMER_ETSI_IDX,  dsatetsime_exec_cmer_cmd },
  
#ifdef FEATURE_DSAT_ETSI_DATA
  { DSATETSI_EXT_ACT_CGATT_ETSI_IDX,  dsatetsipkt_exec_cgatt_cmd },

  { DSATETSI_EXT_ACT_CGACT_ETSI_IDX,  dsatetsipkt_exec_cgact_cmd },

  { DSATETSI_EXT_ACT_CGCMOD_ETSI_IDX,  dsatetsipkt_exec_cgcmod_cmd },
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_ETSI_PBM
  { DSATETSI_EXT_ACT_CPBS_ETSI_IDX,  dsatme_exec_cpbs_cmd },

  { DSATETSI_EXT_ACT_CPBR_ETSI_IDX,  dsatme_exec_cpbr_cmd },

  { DSATETSI_EXT_ACT_CPBF_ETSI_IDX,  dsatme_exec_cpbf_cmd },

  { DSATETSI_EXT_ACT_CPBW_ETSI_IDX, dsatme_exec_cpbw_cmd },
#endif  /* FEATURE_ETSI_PBM */

  /* SMS Commands */
#ifdef FEATURE_ETSI_SMS
  { DSATETSI_EXT_ACT_CPMS_ETSI_IDX,  dsatetsisms_exec_cpms_cmd },

  { DSATETSI_EXT_ACT_CNMI_ETSI_IDX,  dsatetsisms_exec_cnmi_cmd },

  { DSATETSI_EXT_ACT_CMGL_ETSI_IDX,  dsatetsisms_exec_cmgl_cmd },

  { DSATETSI_EXT_ACT_CMGR_ETSI_IDX,  dsatetsisms_exec_cmgr_cmd },

  { DSATETSI_EXT_ACT_CMGS_ETSI_IDX,  dsatsms_exec_sms_cmd },

  { DSATETSI_EXT_ACT_CMSS_ETSI_IDX,  dsatetsisms_exec_cmss_cmd },

  { DSATETSI_EXT_ACT_CMGW_ETSI_IDX,  dsatsms_exec_sms_cmd },

  { DSATETSI_EXT_ACT_CMGD_ETSI_IDX,  dsatetsisms_exec_cmgd_cmd },

  { DSATETSI_EXT_ACT_CMGC_ETSI_IDX,  dsatetsisms_exec_cmgc_cmd },

  { DSATETSI_EXT_ACT_CNMA_ETSI_IDX,  dsatetsisms_exec_cnma_cmd },

  { DSATETSI_EXT_ACT_CMMS_ETSI_IDX,  dsatetsisms_exec_cmms_cmd },
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_DATA_GCSD_FAX
  { DSATETSI_EXT_ACT_FTS_ETSI_IDX,  dsatgsmfax_exec_fts_cmd},

  { DSATETSI_EXT_ACT_FRS_ETSI_IDX,  dsatgsmfax_exec_frs_cmd },

  { DSATETSI_EXT_ACT_FTH_ETSI_IDX,  dsatgsmfax_exec_fth_cmd},

  { DSATETSI_EXT_ACT_FRH_ETSI_IDX,  dsatgsmfax_exec_frh_cmd },

  { DSATETSI_EXT_ACT_FTM_ETSI_IDX,  dsatgsmfax_exec_ftm_cmd},

  { DSATETSI_EXT_ACT_FRM_ETSI_IDX,  dsatgsmfax_exec_frm_cmd },
#endif  /* FEATURE_DATA_GCSD_FAX */

  { DSATETSI_EXT_ACT_CHUP_ETSI_IDX,  dsatetsicall_exec_chup_cmd },

  { DSATETSI_EXT_ACT_CCFC_ETSI_IDX,  dsatetsicall_exec_ccfc_cmd },

  { DSATETSI_EXT_ACT_CCUG_ETSI_IDX,  dsatetsicall_exec_ccug_cmd },

  { DSATETSI_EXT_ACT_COPS_ETSI_IDX,  dsatetsicall_exec_cops_cmd },

  { DSATETSI_EXT_ACT_CLCK_ETSI_IDX,  dsatetsicall_exec_clck_cmd },

  { DSATETSI_EXT_ACT_CPWD_ETSI_IDX,  dsatetsicall_exec_cpwd_cmd },

  { DSATETSI_EXT_ACT_CUSD_ETSI_IDX,  dsatetsicall_exec_cusd_cmd },

  { DSATETSI_EXT_ACT_CAOC_ETSI_IDX,  dsatetsicall_exec_caoc_cmd },

#ifdef FEATURE_DATA_ETSI_PIN
  { DSATETSI_EXT_ACT_CACM_ETSI_IDX,  dsatetsicall_exec_cacm_cmd },

  { DSATETSI_EXT_ACT_CAMM_ETSI_IDX,  dsatetsicall_exec_camm_cmd },

  { DSATETSI_EXT_ACT_CPUC_ETSI_IDX,  dsatetsicall_exec_cpuc_cmd },
#endif /* FEATURE_DATA_ETSI_PIN */

  { DSATETSI_EXT_ACT_CCWA_ETSI_IDX,  dsatetsicall_exec_ccwa_cmd },

  { DSATETSI_EXT_ACT_CHLD_ETSI_IDX,  dsatetsicall_exec_chld_cmd },

  { DSATETSI_EXT_ACT_CIMI_ETSI_IDX,  dsatetsime_exec_cimi_cmd },

  { DSATETSI_EXT_ACT_CGMI_ETSI_IDX,  dsatact_exec_gmi_cmd },

  { DSATETSI_EXT_ACT_CGMM_ETSI_IDX, dsatact_exec_gmm_cmd },

  { DSATETSI_EXT_ACT_CGMR_ETSI_IDX,  dsatact_exec_gmr_cmd },

  { DSATETSI_EXT_ACT_CGSN_ETSI_IDX,  dsatact_exec_gsn_cmd },

  { DSATETSI_EXT_ACT_CNUM_ETSI_IDX,  dsatetsime_exec_cnum_cmd },

#ifdef FEATURE_MMGSDI_ATCSIM
  { DSATETSI_EXT_ACT_CSIM_ETSI_IDX,  dsatetsime_exec_csim_cmd },

  { DSATETSI_EXT_ACT_CRSM_ETSI_IDX,  dsatetsime_exec_crsm_cmd },
#endif /* FEATURE_MMGSDI_ATCSIM */

  { DSATETSI_EXT_ACT_CCLK_ETSI_IDX,  dsatetsime_exec_cclk_cmd},
    
#ifndef FEATURE_DSAT_CUST
  { DSATETSI_EXT_ACT_COPN_ETSI_IDX,  dsatetsicall_exec_copn_cmd },
#endif /* FEATURE_DSAT_CUST */

#ifdef FEATURE_MMGSDI
  { DSATETSI_EXT_ACT_CPOL_ETSI_IDX,  dsatetsime_exec_cpol_cmd },

  { DSATETSI_EXT_ACT_CPLS_ETSI_IDX,  dsatetsime_exec_cpls_cmd },
#endif /* FEATURE_MMGSDI */

  { DSATETSI_EXT_ACT_CTZR_ETSI_IDX,  dsatparm_exec_param_cmd },
    
  { DSATETSI_EXT_ACT_CTZU_ETSI_IDX,  dsatetsime_exec_ctzu_cmd },

  { DSATETSI_EXT_ACT_CLAC_ETSI_IDX,  dsatetsime_exec_clac_cmd },

  { DSATETSI_EXT_ACT_CLIP_ETSI_IDX,  dsatetsicall_exec_clip_cmd },

  { DSATETSI_EXT_ACT_COLP_ETSI_IDX,  dsatetsicall_exec_colp_cmd },

  { DSATETSI_EXT_ACT_CDIP_ETSI_IDX,  dsatetsicall_exec_cdip_cmd},

  { DSATETSI_EXT_ACT_CTFR_ETSI_IDX,  dsatetsicall_exec_ctfr_cmd },

  { DSATETSI_EXT_ACT_CLIR_ETSI_IDX,  dsatetsicall_exec_clir_cmd },
#ifdef FEATURE_DATA_TE_MT_PDP
  { DSATETSI_EXT_ACT_CGANS_ETSI_IDX,  dsatetsipkt_exec_cgans_cmd },
#endif /* FEATURE_DATA_TE_MT_PDP */

#ifdef FEATURE_WCDMA_L1_HS_CQI_STAT
  { DSATETSI_EXT_ACT_CQI_ETSI_IDX,  dsatetsicall_exec_cqi_cmd },
#endif /* FEATURE_WCDMA_L1_HS_CQI_STAT  */

  { DSATETSI_EXT_ACT_VTS_ETSI_IDX,  dsatetsicall_exec_vts_cmd},

#ifdef FEATURE_MMGSDI
  { DSATETSI_EXT_ACT_ICCID_ETSI_IDX,  dsatetsime_exec_iccid_cmd },
#endif /* FEATURE_MMGSDI */

  { DSATETSI_EXT_ACT_CUAD_ETSI_IDX,  dsatetsime_exec_cuad_cmd },
#ifdef FEATURE_DATA_PS_EAP 
  { DSATETSI_EXT_ACT_CEAP_ETSI_IDX,  dsatetsime_exec_ceap_cmd },

  { DSATETSI_EXT_ACT_CERP_ETSI_IDX,  dsatetsime_exec_cerp_cmd },
#endif /* FEATURE_DATA_PS_EAP */

  { DSATETSI_EXT_ACT_CSTF_ETSI_IDX,  dsatetsime_exec_cstf_cmd},

  { DSATETSI_EXT_ACT_CSDF_ETSI_IDX,  dsatetsime_exec_csdf_cmd},

#ifdef FEATURE_DSAT_LTE
  { DSATETSI_EXT_ACT_CVMOD_ETSI_IDX,  dsatetsicall_exec_cvmod_cmd},
#endif /*FEATURE_DSAT_LTE*/

  { DSATETSI_EXT_ACT_CEN_ETSI_IDX,  dsatetsicall_exec_cen_cmd},

#ifdef FEATURE_DATA_MUX
  { DSATETSI_EXT_ACT_CMUX_ETSI_IDX,  dsatact_exec_cmux_cmd },
#endif /* FEATURE_DATA_MUX */

  { DSATETSI_EXT_ACT_CNMPSD_ETSI_IDX,  dsatetsipkt_exec_cnmpsd_cmd }

  };
#endif /* FEATURE_DSAT_ETSI_MODE */

const dsati_cmd_ex_type dsatetsi_ext_table_ex [] =
{
#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)

  { DSATETSI_EXT_CBST_IDX,  dsatetsicall_exec_cbst_cmd },

  { DSATETSI_EXT_CRLP_IDX,  dsatetsicall_exec_crlp_cmd },
  
  { DSATETSI_EXT_CV120_IDX,  dsatetsicall_exec_cv120_cmd },

  { DSATETSI_EXT_CHSN_IDX,  dsatparm_exec_param_cmd },

#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */

  { DSATETSI_EXT_CSSN_IDX,  dsatetsicall_exec_cssn_cmd },

  { DSATETSI_EXT_CREG_IDX,  dsatetsicall_exec_creg_cmd },

  { DSATETSI_EXT_CGREG_IDX, dsatetsicall_exec_creg_cmd },
#ifdef FEATURE_DSAT_LTE

  { DSATETSI_EXT_CEREG_IDX,  dsatetsicall_exec_creg_cmd },
#endif /* FEATURE_DSAT_LTE */

  { DSATETSI_EXT_CSCS_IDX,  dsatparm_exec_param_cmd },

  { DSATETSI_EXT_CSTA_IDX,  dsatparm_exec_param_cmd },

  { DSATETSI_EXT_CR_IDX,  dsatparm_exec_param_cmd },

  { DSATETSI_EXT_CEER_IDX ,  dsatetsicall_exec_ceer_cmd },
  
  { DSATETSI_EXT_CRC_IDX,  dsatparm_exec_param_cmd },

#ifdef FEATURE_DSAT_ETSI_DATA
  { DSATETSI_EXT_CGDCONT_IDX,  dsatetsipkt_exec_cgdcont_cmd },

#ifdef FEATURE_SECONDARY_PDP
  { DSATETSI_EXT_CGDSCONT_IDX,  dsatetsipkt_exec_cgdscont_cmd },

  { DSATETSI_EXT_CGTFT_IDX,  dsatetsipkt_exec_cgtft_cmd },
#endif /* FEATURE_SECONDARY_PDP */
#ifdef FEATURE_DATA_WCDMA_PS
  { DSATETSI_EXT_CGEQREQ_IDX,  dsatetsipkt_exec_cgeqreq_cmd },

  { DSATETSI_EXT_CGEQMIN_IDX,  dsatetsipkt_exec_cgeqmin_cmd },
#endif /* FEATURE_DATA_WCDMA_PS */
#ifdef FEATURE_DSAT_LTE
  { DSATETSI_EXT_CGEQOS_IDX,  dsatetsipkt_exec_cgeqos_cmd },

  { DSATETSI_EXT_CGCONTRDP_IDX,  dsatetsipkt_exec_cgcontrdp_cmd },

  { DSATETSI_EXT_CGSCONTRDP_IDX, dsatetsipkt_exec_cgscontrdp_cmd },

  { DSATETSI_EXT_CGTFTRDP_IDX,  dsatetsipkt_exec_cgtftrdp_cmd },

  { DSATETSI_EXT_CGEQOSRDP_IDX,  dsatetsipkt_exec_cgeqosrdp_cmd },

#endif /* FEATURE_DSAT_LTE */
  { DSATETSI_EXT_CGQREQ_IDX,  dsatetsipkt_exec_cgqreq_cmd },

  { DSATETSI_EXT_CGQMIN_IDX, dsatetsipkt_exec_cgqmin_cmd },

  { DSATETSI_EXT_CGEREP_IDX,  dsatetsipkt_exec_cgerep_cmd },

  { DSATETSI_EXT_CGPADDR_IDX,  dsatetsipkt_exec_cgpaddr_cmd },

  { DSATETSI_EXT_CGDATA_IDX,  dsatetsipkt_exec_cgdata_cmd },

#if defined(FEATURE_GSM)
  { DSATETSI_EXT_CGCLASS_IDX,  dsatparm_exec_param_cmd },
#endif /* defined(FEATURE_GSM) */
#ifdef FEATURE_DATA_TE_MT_PDP
  { DSATETSI_EXT_CGAUTO_IDX,  dsatetsipkt_exec_cgauto_cmd },
#endif /* FEATURE_DATA_TE_MT_PDP */

  { DSATETSI_EXT_CGPIAF_IDX,  dsatparm_exec_param_cmd },
#endif /* FEATURE_DSAT_ETSI_DATA */
  
  /* SMS Commands */
#ifdef FEATURE_ETSI_SMS
#ifdef FEATURE_ETSI_SMS_PS
  { DSATETSI_EXT_CGSMS_IDX,   dsatetsisms_exec_cgsms_cmd },
#endif /* FEATURE_ETSI_SMS_PS */

  { DSATETSI_EXT_CSMS_IDX,  dsatetsisms_exec_csms_cmd },

  { DSATETSI_EXT_CMGF_IDX,  dsatparm_exec_param_cmd },

  { DSATETSI_EXT_CSAS_IDX,  dsatetsisms_exec_csas_cmd },

  { DSATETSI_EXT_CRES_IDX,   dsatetsisms_exec_cres_cmd },

  { DSATETSI_EXT_CSCA_IDX,  dsatsms_exec_sms_cmd },

  { DSATETSI_EXT_CSMP_IDX , dsatsms_exec_sms_cmd },

  { DSATETSI_EXT_CSDH_IDX,  dsatparm_exec_param_cmd },

#ifdef FEATURE_ETSI_SMS_CB
  { DSATETSI_EXT_CSCB_IDX,  dsatetsisms_exec_cscb_cmd },
#endif /* FEATURE_ETSI_SMS_CB */

#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_DATA_GCSD_FAX

  { DSATETSI_EXT_FDD_IDX,  dsatgsmfax_exec_param_cmd },
  
  { DSATETSI_EXT_FAR_IDX ,  dsatgsmfax_exec_param_cmd },

  { DSATETSI_EXT_FCL_IDX,  dsatgsmfax_exec_param_cmd },

  { DSATETSI_EXT_FIT_IDX,  dsatgsmfax_exec_param_cmd },

#endif  /* FEATURE_DATA_GCSD_FAX */

#ifdef FEATURE_WCDMA
  { DSATETSI_EXT_ES_IDX,  dsatetsicall_exec_es_cmd },

  { DSATETSI_EXT_ESA_IDX,  dsatetsicall_exec_esa_cmd },
#endif /* FEATURE_WCDMA */

  { DSATETSI_EXT_CMOD_IDX,  dsatparm_exec_param_cmd },

#ifdef FEATURE_DSAT_LTE
  {DSATETSI_EXT_CEMODE_IDX,   dsatetsipkt_exec_cemode_cmd },
#endif /* FEATURE_DSAT_LTE */

  { DSATETSI_EXT_CVHU_IDX,  dsatparm_exec_param_cmd }

#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_ECALL_APP 
  ,{ DSATETSI_EXT_CECALL_IDX,  dsatetsicall_exec_cecall_cmd }
#endif /* FEATURE_ECALL_APP */
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DATA_WCDMA_PS
  ,{ DSATETSI_EXT_CGEQMIN_IDX, dsatetsipkt_exec_cgeqmin_cmd }
#endif /* FEATURE_DATA_WCDMA_PS */

#ifdef FEATURE_DSAT_LTE
  ,{ DSATETSI_EXT_CGEQOS_IDX,  dsatetsipkt_exec_cgeqos_cmd }
#endif /* FEATURE_DSAT_LTE */

#ifdef FEATURE_DATA_WCDMA_PS
  ,{ DSATETSI_EXT_CGEQREQ_IDX, dsatetsipkt_exec_cgeqreq_cmd }
#endif /* FEATURE_DATA_WCDMA_PS */

  ,{ DSATETSI_EXT_CGQMIN_IDX,  dsatetsipkt_exec_cgqmin_cmd }

  ,{ DSATETSI_EXT_CGQREQ_IDX,  dsatetsipkt_exec_cgqreq_cmd }

#ifdef FEATURE_SECONDARY_PDP
  ,{ DSATETSI_EXT_CGTFT_IDX,   dsatetsipkt_exec_cgtft_cmd }
#endif /* FEATURE_SECONDARY_PDP */

} ;


