/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the FAX related AT modem commands for the IS-707 mode.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2001 - 2014 Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707faxctab.c_v   1.5   23 Oct 2002 10:10:04   sramacha  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707faxctab.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc      Added support for Dynamic ATCoP.
01/19/12   sk      Feature cleanup.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
11/28/08   cs      Off-Target build Compiler and Lint warning fixes.
04/11/07   ua      Modifications as per KDDI requirements. 
08/13/03   rsl     Moved at+fclass to dsatctab.c
10/15/02   ak      Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
4/3/01     rsl     Initial release. 

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707_ASYNC
#include "dsati.h"
#include "dsat707fax.h"
#include "dsat707faxctab.h"
#include "dsatparm.h"
#include "msg.h"

#include "ds707_roaming.h"

/*=========================================================================*/
/* IS-707 Fax related AT command table */
/*=========================================================================*/
const dsati_cmd_ex_type dsat707_fax_table_ex [] =
{
  /*lint -save -e785 */
  { DSAT707_FAX_FAA_IDX,       dsat707_exec_faa_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FAP_IDX,      dsat707_exec_fap_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FBO_IDX,      dsat707_exec_fbo_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FBS_IDX,      dsat707_exec_fbs_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FBU_IDX,      dsat707_exec_fbu_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FCQ_IDX,      dsat707_exec_fcq_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FCC_IDX,      dsat707_exec_fcc_cmd},
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FCR_IDX,      dsat707_exec_fcr_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FCT_IDX,      dsat707_exec_fct_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FDR_IDX,      dsat707_exec_fdr_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FDT_IDX,      dsat707_exec_fdt_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FEA_IDX ,     dsat707_exec_fea_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FFC_IDX,      dsat707_exec_ffc_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FHS_IDX,      dsat707_exec_fhs_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FIE_IDX,      dsat707_exec_fie_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FIP_IDX,      dsat707_exec_fip_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FIS_IDX,      dsat707_exec_fis_cmd},
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FKS_IDX,      dsat707_exec_fks_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FLI_IDX,      dsat707_exec_fli_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FLO_IDX,      dsat707_exec_flo_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FLP_IDX,      dsat707_exec_flp_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FMS_IDX,      dsat707_exec_fms_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FNR_IDX,      dsat707_exec_fnr_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FNS_IDX,      dsat707_exec_fns_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FPA_IDX,      dsat707_exec_fpa_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FPI_IDX,      dsat707_exec_fpi_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FPP_IDX,      dsat707_exec_fpp_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FPR_IDX,      dsat707_exec_fpr_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FPS_IDX,      dsat707_exec_fps_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FPW_IDX,      dsat707_exec_fpw_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FRQ_IDX,      dsat707_exec_frq_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FRY_IDX,      dsat707_exec_fry_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FSA_IDX,      dsat707_exec_fsa_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_FAX_FSP_IDX,      dsat707_exec_fsp_cmd },
/*-------------------------------------------------------------------------*/
  /*lint -restore */
};

#endif /* FEATURE_DATA_IS707_ASYNC */

