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

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707mipctab_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc     Initial revision (created file for Dynamic ATCoP).

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

const dsati_cmd_type dsat707_mip_table [] =
{
#ifdef FEATURE_DS_AT_TEST_ONLY
  { "$QCMIP",
      EXTENDED | LOCAL_TEST | COMMON_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_MIP_QCMIP_IDX,
      NULL },
#endif /* FEATURE_DS_AT_TEST_ONLY */
/*-------------------------------------------------------------------------*/
  { "$QCMIPP",
      EXTENDED  | LOCAL_TEST | DO_PREF_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_MIP_QCMIPP_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DS_AT_TEST_ONLY
  { "$QCMIPT",
      EXTENDED  | LOCAL_TEST | DO_PREF_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_MIP_QCMIPT_IDX,
      NULL },
#endif /* FEATURE_DS_AT_TEST_ONLY */
/*-------------------------------------------------------------------------*/
  { "$QCMIPEP",
      EXTENDED  | LOCAL_TEST | DO_PREF_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_MIP_QCMIPEP_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
  { "$QCMIPMASS",
      EXTENDED  | LOCAL_TEST | STRING | NO_QUOTE | DO_PREF_CMD,
      SPECIAL_QCMIPMASS,
      MIP_MN_AAA_SS_MAX_LEN+1,
      DSAT707_MIP_QCMIPMASS_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
  { "$QCMIPMHSS",
      EXTENDED | LOCAL_TEST | STRING | NO_QUOTE | DO_PREF_CMD,
      SPECIAL_QCMIPMHSS,
      MIP_MN_HA_SS_MAX_LEN+1,
      DSAT707_MIP_QCMIPMHSS_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
  { "$QCMIPMASPI",
      EXTENDED  | LOCAL_TEST | DO_PREF_CMD,
      SPECIAL_QCMIPMASPI,
      2,
      DSAT707_MIP_QCMIPMASPI_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
  { "$QCMIPMHSPI",
      EXTENDED  | LOCAL_TEST | DO_PREF_CMD ,
      SPECIAL_QCMIPMHSPI,
      2,
      DSAT707_MIP_QCMIPMHSPI_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
  { "$QCMIPRT",
      EXTENDED | LOCAL_TEST | DO_PREF_CMD,
      SPECIAL_NONE,
      2,
      DSAT707_MIP_QCMIPRT_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_MDN_BASED_NAI
  { "$QCMIPNAI",
      EXTENDED | READ_ONLY | LOCAL_TEST | STRING | NO_QUOTE | DO_PREF_CMD,
      SPECIAL_NONE,
      MAX_NAI_LEN,
      DSAT707_MIP_QCMIPNAI_IDX,
      NULL },
#else /* FEATURE_MDN_BASED_NAI */
  { "$QCMIPNAI",
      EXTENDED  | LOCAL_TEST | STRING | NO_QUOTE | DO_PREF_CMD,
      SPECIAL_NONE,
      MAX_NAI_LEN,
      DSAT707_MIP_QCMIPNAI_IDX,
      NULL },
#endif /* FEATURE_MDN_BASED_NAI */
/*-------------------------------------------------------------------------*/
  { "$QCMIPHA",
      EXTENDED  | LOCAL_TEST | STRING | NO_QUOTE | DO_PREF_CMD,
      SPECIAL_QCMIPHA,
      16,
      DSAT707_MIP_QCMIPHA_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
  { "$QCMIPPHA",
      EXTENDED  | LOCAL_TEST | STRING | NO_QUOTE | DO_PREF_CMD,
      SPECIAL_QCMIPPHA,
      16,
      DSAT707_MIP_QCMIPPHA_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
  { "$QCMIPSHA",
      EXTENDED| LOCAL_TEST | STRING | NO_QUOTE | DO_PREF_CMD,
      SPECIAL_QCMIPSHA,
      16,
      DSAT707_MIP_QCMIPSHA_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
  { "$QCMIPGETP",
      EXTENDED  | LOCAL_TEST | DO_PREF_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_MIP_QCMIPGETP_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
  { "$QCMIPMASSX",
      EXTENDED | LOCAL_TEST | STRING | DO_PREF_CMD,
      SPECIAL_QCMIPMASSX,
      2*MIP_MN_AAA_SS_MAX_LEN+1,
      DSAT707_MIP_QCMIPMASSX_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
  { "$QCMIPMHSSX",
      EXTENDED  | LOCAL_TEST | STRING | DO_PREF_CMD,
      SPECIAL_QCMIPMHSSX,
      2*MIP_MN_HA_SS_MAX_LEN+1,
      DSAT707_MIP_QCMIPMHSSX_IDX,
      NULL }
/*-------------------------------------------------------------------------*/
};

/* Size of Mobile IP command table */
const unsigned int dsat707_mip_table_size = ARR_SIZE( dsat707_mip_table );

#endif /* FEATURE_DS_MOBILE_IP */
#endif /* FEATURE_DATA_IS707     */

