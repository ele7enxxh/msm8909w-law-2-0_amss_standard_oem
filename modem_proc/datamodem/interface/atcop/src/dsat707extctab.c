/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the extended AT modem commands for the IS-707 mode.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 1995 - 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707extctab.c_v   1.5   08 Jan 2003 12:27:34   sramacha  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707extctab.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc      Added support for Dynamic ATCoP.
05/08/13   tk      Renamed 3GPP2 +CMUX command to +QCMUX command.
03/07/12   sk      Made CLCC common for all modes.
01/19/12   sk      Feature cleanup.
11/12/11   ua      Renaming CDMA +CSS command to +QCCSS to avoid a conflict 
                   with 3GPP 27.007 touch screen command. 
08/09/11   dvk     Global Variable Cleanup
01/28/11   jee     JCDMA feature cleanup
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
05/10/10   kk      Added support for +CIMI, +MDN.
01/22/10   nc      Fixed Compilation issues for Multimode only builds.
01/06/10   ua      Added support for +CGMI,+CGMM and +CGMR cmds.
01/12/10   vg      Added support for +CPIN,^CPIN and +CLCK for RUIM cards.
02/25/09   bs      Setting +CRM default value to 2 in WM builds.
11/28/08   cs      Off-Target build Compiler and Lint warning fixes.
06/24/08   mga     Made additional changes to support CTA timer
06/10/08   mga     Made changes to support CTA timer NV item.
02/18/08   sa      Added modification for Passport Feature support.
04/11/07   ua      Modifications as per KDDI requirements. 
11/03/05   ar      Added +CIMI to extended command table.
10/15/02   ak      Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
10/15/02   ak      CMUX list no longer changeable under JCDMA, so make const.
10/13/02   ak      Clean up AT+CRM featurization for JCDMA.
04/03/01   rsl     Initial release. 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsat707ext.h"
#include "dsat707extctab.h"
#include "dsat707smsctab.h"
#include "dsatact.h"
#include "ds_1x_profile.h"
#include "dsatparm.h"
#include "msg.h"

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsime.h"
#endif /* FEATURE_DSAT_ETSI_MODE */  

#ifdef FEATURE_DSAT_CDMA_PIN 
#include "mmgsdilib.h"
#endif /* FEATURE_DSAT_CDMA_PIN */

#include "ds707_roaming.h"

#if defined(FEATURE_CDMA_SMS)
#include "dsat707smsctab.h"
#include "dsat707sms.h"
#endif /* defined(FEATURE_CDMA_SMS) */

/*=========================================================================*/
/* General IS-707 extended format AT parameter command table */
/*=========================================================================*/
const dsati_cmd_ex_type dsat707_ext_para_table_ex [] =
{
/*lint -save -e785 */
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_EB_IDX,      dsat707_exec_eb_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_EFCS_IDX,    dsat707_exec_efcs_cmd  },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_ER_IDX,      dsat707_exec_er_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_ESR_IDX,     dsat707_exec_esr_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_ETBM_IDX,    dsat707_exec_etbm_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_MA_IDX,      dsat707_exec_ma_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_MR_IDX,      dsat707_exec_mr_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_MS_IDX,      dsat707_exec_ms_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_MV18R_IDX,   dsat707_exec_mv18r_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_MV18S_IDX,   dsat707_exec_mv18s_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_CXT_IDX,     dsat707_exec_cxt_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_CDR_IDX,     dsat707_exec_cdr_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_CDS_IDX,     dsat707_exec_cds_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_CFC_IDX,     dsat707_exec_cfc_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_CFG_IDX,     dsat707_exec_cfg_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_CQD_IDX,     dsat707_exec_cqd_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_CRC_IDX,     dsat707_exec_crc_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_CSQ_IDX,     dsat707_exec_csq_cmd },
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707_ASYNC
  { DSAT707_EXT_PARA_CMIP_IDX,    dsat707_exec_cmip_cmd },
#endif /* FEATURE_DATA_IS707_ASYNC */
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_CAD_IDX,     dsat707_exec_cad_cmd },
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707_ASYNC
  { DSAT707_EXT_PARA_CBIP_IDX,    dsat707_exec_cbip_cmd },
#endif /* FEATURE_DATA_IS707_ASYNC */
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_DS_IS707A_CMUX
  { DSAT707_EXT_PARA_CMUX_IDX,    dsat707_exec_cmux_cmd },
#else
  { DSAT707_EXT_PARA_CMUX_IDX,    dsat707_exec_cmux_cmd },
#endif  /* FEATURE_DS_IS707A_CMUX */
/*-------------------------------------------------------------------------*/

  { DSAT707_EXT_PARA_CRM_IDX,     dsat707_exec_crm_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_CSS_IDX,     dsat707_exec_css_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_CTA_IDX,     dsat707_exec_cta_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_GOI_IDX,     dsat707_exec_goi_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_PARA_ILRR_IDX,    dsat707_exec_ilrr_cmd }
/*-------------------------------------------------------------------------*/
/*lint -restore */
};

/*=========================================================================*/
/* General IS-707 extended format AT action command table */
/*=========================================================================*/
const dsati_cmd_ex_type dsat707_ext_action_table_ex [] =
{
  { DSAT707_EXT_ACT_CDV_IDX,    dsat707_exec_cdv_cmd},
  /*-------------------------------------------------------------------------*/
  { DSAT707_EXT_ACT_CHV_IDX,    dsat707_exec_chv_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_ACT_CHV0_IDX,   dsat707_exec_chv0_cmd},
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DSAT_CDMA_PIN 
  { DSAT707_EXT_ACT_QCPIN_IDX,  dsat707_exec_cpin_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_ACT_QCLCK_IDX,  dsat707_exec_clck_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_ACT_QCPWD_IDX,  dsat707_exec_cpwd_cmd },
#endif /* FEATURE_DSAT_CDMA_PIN*/
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_ACT_PZID_IDX,   dsat707_exec_pzid_cmd}, 
  /*-------------------------------------------------------------------------*/
  { DSAT707_EXT_ACT_CIMI_IDX,   dsat707_exec_cimi_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_EXT_ACT_MDN_IDX,    dsat707_exec_mdn_cmd }
/*-------------------------------------------------------------------------*/
/*lint -restore */
};

#endif /* FEATURE_DATA_IS707 */

