/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               M I P   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the Mobile IP AT modem commands for the IS-707 mode.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 1995-2014 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707mipctab.c_v   1.5   07 Feb 2003 10:24:10   sramacha  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707mipctab.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc      Added support for Dynamic ATCoP.
01/19/12   sk      Feature cleanup.
07/22/11   dvk     Global Variable Cleanup
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
05/10/10   kk      Featurization fix for $QCMIPNAI.
09/04/09   ss      CMI SU level modifications.
04/28/04   jd      Include dsmip.h for length constants, remove unused vars
09/18/02   jay     Modified comments and formatted code.
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
#include "dsat707mip.h"
#include "dsat707mipctab.h"
#include "dsatparm.h"
#include "ds_1x_profile.h"
#include "msg.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "dsmip_v.h"
#endif /* FEATURE_DS_MOBILE_IP */

#ifdef FEATURE_DS_MOBILE_IP

/*=========================================================================*/
/*  Mobile IP related AT command table */
/*=========================================================================*/

const dsati_cmd_ex_type dsat707_mip_table_ex [] =
{
#ifdef FEATURE_DS_AT_TEST_ONLY
  { DSAT707_MIP_QCMIP_IDX,      dsat707_exec_qcmip_cmd },
#endif /* FEATURE_DS_AT_TEST_ONLY */
/*-------------------------------------------------------------------------*/
  { DSAT707_MIP_QCMIPP_IDX,     dsat707_exec_qcmipp_cmd },
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DS_AT_TEST_ONLY
  { DSAT707_MIP_QCMIPT_IDX,     dsat707_exec_qcmipt_cmd },
#endif /* FEATURE_DS_AT_TEST_ONLY */
/*-------------------------------------------------------------------------*/
  { DSAT707_MIP_QCMIPEP_IDX,    dsat707_exec_qcmipep_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_MIP_QCMIPMASS_IDX,  dsat707_exec_qcmipmass_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_MIP_QCMIPMHSS_IDX,  dsat707_exec_qcmipmhss_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_MIP_QCMIPMASPI_IDX, dsat707_exec_qcmipmaspi_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_MIP_QCMIPMHSPI_IDX, dsat707_exec_qcmipmhspi_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_MIP_QCMIPRT_IDX,    dsat707_exec_qcmiprt_cmd },
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_MDN_BASED_NAI
  { DSAT707_MIP_QCMIPNAI_IDX,   dsat707_exec_qcmipnai_cmd },
#else /* FEATURE_MDN_BASED_NAI */
  { DSAT707_MIP_QCMIPNAI_IDX,   dsat707_exec_qcmipnai_cmd },
#endif /* FEATURE_MDN_BASED_NAI */
/*-------------------------------------------------------------------------*/
  { DSAT707_MIP_QCMIPHA_IDX,    dsat707_exec_qcmipha_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_MIP_QCMIPPHA_IDX,   dsat707_exec_qcmippha_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_MIP_QCMIPSHA_IDX,   dsat707_exec_qcmipsha_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_MIP_QCMIPGETP_IDX,  dsat707_exec_qcmipgetp_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_MIP_QCMIPMASSX_IDX, dsat707_exec_qcmipmassx_cmd },
/*-------------------------------------------------------------------------*/
  { DSAT707_MIP_QCMIPMHSSX_IDX, dsat707_exec_qcmipmhssx_cmd }
/*-------------------------------------------------------------------------*/
};

#endif /* FEATURE_DS_MOBILE_IP */
#endif /* FEATURE_DATA_IS707     */

