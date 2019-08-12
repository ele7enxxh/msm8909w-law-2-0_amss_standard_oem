/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the vendor specific AT modem commands for the IS-707 mode.

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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707vendctab_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/27/14   sc      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "ds707.h"
#include "dsatvend.h"
#include "dsat707ext.h"
#include "dsat707vend.h"
#include "dsat707vendctab.h"
#include "dsat707extctab.h"
#include "ds_1x_profile.h"
#include "dsatctab.h"
#include "dsatme.h"
#include "dsatparm.h"
#include "dstaski.h"
#include "err.h"
#include "msg.h"
#include "nv.h"

#if defined(FEATURE_DSAT_SP_CMDS)
#include "hw.h"
#include "prl_v.h"
#endif /* defined(FEATURE_DSAT_SP_CMDS) */

#if defined(FEATURE_DSAT_GPS_CMDS)
#include "pdapi.h"
#endif /* defined(FEATURE_DSAT_GPS_CMDS) */

/*=========================================================================*/
/* Defaults & Limits for HDR commands */
/*=========================================================================*/

LOCAL const dflm_type dsat707_qcvad_dflm [] =
{
  { 0  , 0  , 4    },
} ;

#if defined(FEATURE_DSAT_SP_CMDS)
/* For MIP commands */
dflm_type dsat707_spmiperror_dflm [] =
{
  { 0, 0, 5 },
} ;
LOCAL dsat_num_item_type dsat707_spmiperror_val;
#endif /* defined(FEATURE_DSAT_SP_CMDS) */

#if defined(FEATURE_DSAT_GPS_CMDS)
/* For GPS commands */
dflm_type dsat707_gps_mode_dflm [] =
{
  { 0, 0, 5 },
};

dflm_type dsat707_pde_trs_dflm [] =
{
  { 0, 0, 2 },
};

dflm_type dsat707_gps_pdeaddr_dflm [] =
{
  { 0, 0, 255 },
  { 0, 0, 255 },
  { 0, 0, 255 },
  { 0, 0, 255 },
  { 0, 0, 65535 },
};

dflm_type dsat707_gps_port_dflm [] =
{
  { 0, 0, 65535 },
};
dflm_type dsat707_gps_init_mask_dflm [] =
{
  { 0, 0, 65535 },
};

dflm_type dsat707_gps_nmea_config_dflm [] =
{
  { 1, 0, 1 },
  { 2, 0, 3 },
};

dflm_type dsat707_sp_lcs_state_dflm [] = 
{
    { 0, 0, 1 },
};
dflm_type dsat707_roam_pref_dflm [] = 
{
    { 0, 0, 2 },
};

/* SPC */
const word dsat_spc_string_len = NV_SEC_CODE_SIZE;

LOCAL const mixed_def_s_type dsat_spc_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_spc_string_len
};

LOCAL const mixed_def_s_type * dsat_spc_mixed_dfl = 
{
  &dsat_spc_def_val,
};
const word dsat_unlock_string_len = UNLOCK_STRING_LEN;

LOCAL const mixed_def_s_type dsat_unlock_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_unlock_string_len
};

LOCAL const mixed_def_s_type * dsat_unlock_mixed_dfl = 
{
  &dsat_unlock_def_val,
};

/* LOCK */
const word dsat_lock_string_len = NV_LOCK_CODE_SIZE;

LOCAL const mixed_def_s_type dsat_lock_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_lock_string_len
};

LOCAL const mixed_def_s_type * dsat_lock_mixed_dfl = 
{
  &dsat_lock_def_val,
};
const word dsat_old_lock_string_len = LOCK_OLD_STRING_LEN;
const word dsat_new_lock_string_len = NV_LOCK_CODE_SIZE;

LOCAL const mixed_def_s_type dsat_old_lock_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_old_lock_string_len
};

LOCAL const mixed_def_s_type dsat_new_lock_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_new_lock_string_len
};

LOCAL const mixed_def_s_type * dsat_lock_chg_mixed_dfl [2] = 
{
  &dsat_old_lock_def_val,
  &dsat_new_lock_def_val
};

#endif /* defined(FEATURE_DSAT_GPS_CMDS) */

/*=========================================================================*/
/* IS-707 Vendor Specific AT parameter command table */
/*=========================================================================*/
const dsati_cmd_type dsat707_vend_para_table [] =
{
/*lint -save -e785*/
/*-------------------------------------------------------------------------*/
 { "$QCPREV",
    EXTENDED  | READ_ONLY | DO_PREF_CMD,
    SPECIAL_QCPREV,
    0,
    DSAT707_VEND_PARA_QCPREV_IDX,
    NULL}
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DS_AT_TEST_ONLY

 ,{ "$QCMDR",
    EXTENDED  | LOCAL_TEST | COMMON_CMD,
    SPECIAL_QCMDR,
    1,
    DSAT707_VEND_PARA_QCMDR_IDX,
    NULL}

#endif /* FEATURE_DS_AT_TEST_ONLY */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_IS2000_R_SCH
#ifdef FEATURE_DS_AT_TEST_ONLY

 ,{ "$QCSCRM",
    EXTENDED  | LOCAL_TEST | COMMON_CMD,
    SPECIAL_QCSCRM,
    1,
    DSAT707_VEND_PARA_QCSCRM_IDX,
    NULL }
/*-------------------------------------------------------------------------*/
 ,{ "$QCTRTL",
    EXTENDED  | LOCAL_TEST | COMMON_CMD,
    SPECIAL_QCTRTL,
    1,
    DSAT707_VEND_PARA_QCTRTL_IDX,
    NULL}
#endif /* FEATURE_DS_AT_TEST_ONLY */
#endif /* FEATURE_IS2000_R_SCH */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DS_QNC

 ,{ "$QCQNC",
    EXTENDED  | LOCAL_TEST | COMMON_CMD,
    SPECIAL_QCQNC,
    1,
    DSAT707_VEND_PARA_QCQNC_IDX,
    NULL}

#endif /* FEATURE_DS_QNC */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DS_CHOOSE_SO
#ifdef FEATURE_DS_AT_TEST_ONLY
 ,{ "$QCSO",
    EXTENDED  | LOCAL_TEST | COMMON_CMD,
    SPECIAL_QCSO,
    1,
    DSAT707_VEND_PARA_QCSO_IDX,
    NULL}
#endif /* FEATURE_DS_AT_TEST_ONLY */
#endif /* FEATURE_DS_CHOOSE_SO */
/*-------------------------------------------------------------------------*/
 ,{ "$QCVAD",
    EXTENDED | CONFIG | LOCAL_TEST | CDMA_CMD,
    SPECIAL_QCVAD,
    1,
    DSAT707_VEND_PARA_QCVAD_IDX,
    &dsat707_qcvad_dflm[0] }
/*-------------------------------------------------------------------------*/
 ,{ "$QCCAV",
    EXTENDED | CDMA_CMD,
    SPECIAL_QCCAV ,
    0,
    DSAT707_VEND_PARA_QCCAV_IDX,
    NULL}
/*-------------------------------------------------------------------------*/
 ,{ "$QCCHV",
    EXTENDED | CDMA_CMD,
    SPECIAL_QCCHV ,
    0,
    DSAT707_VEND_PARA_QCCHV_IDX,
    NULL }
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_IS2000_REL_A
 ,{ "$QCQOSPRI",
    EXTENDED  | LOCAL_TEST | COMMON_CMD,
    SPECIAL_QCQOSPRI,
    1,
    DSAT707_VEND_PARA_QCQOSPRI_IDX,
    NULL}
#endif /* FEATURE_IS2000_REL_A */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_IS2000_CHS

 ,{ "$QCCHS",
    EXTENDED  | LOCAL_TEST | COMMON_CMD,
    SPECIAL_QCCHS,
    1,
    DSAT707_VEND_PARA_QCCHS_IDX,
    NULL}

#endif /* FEATURE_IS2000_CHS */
/*-------------------------------------------------------------------------*/
,{ "^HDRCSQ",
    READ_ONLY | RESTRICTED | DO_PREF_CMD,
    SPECIAL_NONE, 
    0,
    DSAT707_VEND_PARA_HDRCSQ_IDX,
    NULL }
 /*-------------------------------------------------------------------------*/
 ,{ "^GSN",
     EXTENDED | READ_ONLY | RESTRICTED | DO_PREF_CMD,
     SPECIAL_NONE, 
     0,
     DSAT707_VEND_PARA_GSN_IDX,
     NULL }
/*-------------------------------------------------------------------------*/
 ,{ "^CGSN",
     EXTENDED | READ_ONLY | RESTRICTED | DO_PREF_CMD,
     SPECIAL_NONE, 
     0,
     DSAT707_VEND_PARA_CGSN_IDX,
     NULL }
/*-------------------------------------------------------------------------*/
 ,{ "^MEID",
     EXTENDED | READ_ONLY | RESTRICTED | DO_PREF_CMD,
     SPECIAL_NONE, 
     0,
     DSAT707_VEND_PARA_MEID_IDX,
     NULL }
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/
/*lint -restore */
};

/*=========================================================================*/
/* IS-707 Vendor Specific AT parameter command table */
/*=========================================================================*/
const dsati_cmd_type dsat707_vend_action_table [] =
{
#ifdef FEATURE_DSAT_EXTENDED_CMD
/*-------------------------------------------------------------------------*/
  { "^CPBR",
    ATTRIB_NONE  ,
    SPECIAL_NONE,
    2,
    DSAT707_VEND_ACT_CPBR_IDX,
    NULL },
/*-------------------------------------------------------------------------*/
  { "^CPBF",
    ATTRIB_NONE ,
    SPECIAL_NONE,
    1,
    DSAT707_VEND_ACT_CPBF_IDX,
    NULL},
/*-------------------------------------------------------------------------*/
  { "^CPBW",
    ATTRIB_NONE ,
    SPECIAL_NONE,
    10,
    DSAT707_VEND_ACT_CPBW_IDX,
    NULL },
#endif /* FEATURE_DSAT_EXTENDED_CMD */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707_ASYNC
  { "$QCOTC",
    EXTENDED | CONFIG | STRING | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_QCOTC,
    240,
    DSAT707_VEND_ACT_QCOTC_IDX,
    NULL},
#endif /* FEATURE_DATA_IS707_ASYNC */
/*-------------------------------------------------------------------------*/
  { "$QCCAV",
    EXTENDED | CDMA_CMD,
    SPECIAL_QCCAV,
    0,
    DSAT707_VEND_ACT_QCCAV_IDX,
    NULL }
/*-------------------------------------------------------------------------*/
 ,{ "$QCCHV",
    EXTENDED | CDMA_CMD,
    SPECIAL_QCCHV ,
    0,
    DSAT707_VEND_ACT_QCCHV_IDX,
    NULL }
/*-------------------------------------------------------------------------*/
  ,{ "^HWVER",
    READ_ONLY | RESTRICTED,
    SPECIAL_NONE, 
    0,
    DSAT707_VEND_ACT_HWVER_IDX,
    NULL }
/*-------------------------------------------------------------------------*/
  ,{"^RESET",
    READ_ONLY | RESTRICTED,
    SPECIAL_NONE, 
    0,
    DSAT707_VEND_ACT_RESET_IDX,
    NULL }
/*-------------------------------------------------------------------------*/
  ,{ "^VOLT",
    READ_ONLY | RESTRICTED,
    SPECIAL_NONE, 
    0,
    DSAT707_VEND_ACT_VOLT_IDX,
    NULL }
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DSAT_CDMA_PIN 
  ,{ "^CPIN",
     EXTENDED | CONFIG | MIXED_PARAM | LIST | LOCAL_TEST | RESTRICTED | DO_PREF_CMD,
     SPECIAL_NONE,
     2,
     DSAT707_VEND_ACT_CPIN_IDX,
     &dsat707_cpin_mixed_dfl[0] }
#endif /* FEATURE_DSAT_CDMA_PIN*/
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DSAT_MMGSDI_AUTH
  ,{ "^CAVE",
    EXTENDED | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    1,
    DSAT707_VEND_ACT_CAVE_IDX,
    NULL }
  /*-------------------------------------------------------------------------*/
  ,{ "^SSDUPD",
    EXTENDED | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    1,
    DSAT707_VEND_ACT_SSDUPD_IDX,
    NULL }
  /*-------------------------------------------------------------------------*/
  ,{ "^SSDUPDCFM",
    EXTENDED | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    1,
    DSAT707_VEND_ACT_SSDUPDCFM_IDX,
    NULL }
  /*-------------------------------------------------------------------------*/
  ,{ "^VPM",
    EXTENDED | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    0,
    DSAT707_VEND_ACT_VPM_IDX,
    NULL }
  /*-------------------------------------------------------------------------*/
  ,{ "^UIMAUTH",
    EXTENDED | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    0,
    DSAT707_VEND_ACT_UIMAUTH_IDX,
    NULL }
  /*-------------------------------------------------------------------------*/
  ,{ "^GSN",
    EXTENDED | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    0,
    DSAT707_VEND_ACT_GSN_IDX,
    NULL }
  /*-------------------------------------------------------------------------*/
  ,{ "^MD5",
    EXTENDED | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    2,
    DSAT707_VEND_ACT_MD5_IDX,
    NULL }
#endif   /*FEATURE_DSAT_MMGSDI_AUTH */

#if defined(FEATURE_DSAT_SP_CMDS)
/*-------------------------------------------------------------------------*/
 ,{ "$QCMSID",
    EXTENDED  | LOCAL_TEST | READ_ONLY | DO_PREF_CMD,
    SPECIAL_NONE,
    0,
    DSAT707_VEND_ACT_QCMSID_IDX,
    NULL }
/*-------------------------------------------------------------------------*/
 ,{ "$SPSERVICE",
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_NONE,
      0,
      DSAT707_VEND_ACT_SPSERVICE_IDX,
      NULL }
/*-------------------------------------------------------------------------*/
 ,{ "$SPSPC",
     EXTENDED | CONFIG | MIXED_PARAM | LOCAL_TEST | LIST | DO_PREF_CMD,
     SPECIAL_NONE,
     1,
     DSAT707_VEND_ACT_SPSPC_IDX,
     &dsat_spc_mixed_dfl }
 /*-------------------------------------------------------------------------*/
 ,{ "$SPRESET",
     EXTENDED | DO_PREF_CMD,
     SPECIAL_NONE,
     0,
     DSAT707_VEND_ACT_SPRESET_IDX,
     NULL }
  /*-------------------------------------------------------------------------*/
 ,{ "$SPFWREV",
     EXTENDED | READ_ONLY  | DO_PREF_CMD,
     SPECIAL_NONE,
     0,
     DSAT707_VEND_ACT_SPFWREV_IDX,
     NULL }
 /*-------------------------------------------------------------------------*/
 ,{ "$SPPRL",
     EXTENDED,
     SPECIAL_NONE | DO_PREF_CMD,
     0,
     DSAT707_VEND_ACT_SPPRL_IDX,
     NULL }
 /*-------------------------------------------------------------------------*/
 ,{ "$SPROAM",
     EXTENDED | CONFIG | LOCAL_TEST | DO_PREF_CMD,
     SPECIAL_NONE,
     1,
     DSAT707_VEND_ACT_SPROAM_IDX,
     &dsat707_roam_pref_dflm[0] }
 /*-------------------------------------------------------------------------*/ 
 ,{ "$SPERI",
     EXTENDED | READ_ONLY | DO_PREF_CMD,
     SPECIAL_NONE,
     0,
     DSAT707_VEND_ACT_SPERI_IDX,
     NULL }
 /*-------------------------------------------------------------------------*/
 ,{ "$SPSIGDBM",
     EXTENDED | READ_ONLY | DO_PREF_CMD,
     SPECIAL_NONE,
     0,
     DSAT707_VEND_ACT_SPSIGDBM_IDX,
     NULL }
 /*-------------------------------------------------------------------------*/
 ,{ "$SPLOCKED",
     EXTENDED | READ_ONLY | DO_PREF_CMD,
     SPECIAL_NONE,
     0,
     DSAT707_VEND_ACT_SPLOCKED_IDX,
     NULL }
 /*-------------------------------------------------------------------------*/
 ,{ "$SPUNLOCK",
     EXTENDED | CONFIG | MIXED_PARAM | LOCAL_TEST | LIST | DO_PREF_CMD,
     SPECIAL_NONE,
     1,
     DSAT707_VEND_ACT_SPUNLOCK_IDX,
     &dsat_unlock_mixed_dfl }
 /*-------------------------------------------------------------------------*/
 ,{ "$SPLOCK",
     EXTENDED | CONFIG | MIXED_PARAM | LOCAL_TEST | LIST | DO_PREF_CMD,
     SPECIAL_NONE,
     1,
     DSAT707_VEND_ACT_SPLOCK_IDX,
     &dsat_lock_mixed_dfl }
 /*-------------------------------------------------------------------------*/
 ,{ "$SPLOCKCHG",
     EXTENDED | CONFIG | MIXED_PARAM | LOCAL_TEST | LIST | DO_PREF_CMD,
     SPECIAL_NONE,
     2,
     DSAT707_VEND_ACT_SPLOCKCHG_IDX,
     &dsat_lock_chg_mixed_dfl[0] }
 /*-------------------------------------------------------------------------*/
 ,{ "$SPCURRENTLOCK",
     EXTENDED | READ_ONLY | DO_PREF_CMD,
     SPECIAL_NONE,
     0,
     DSAT707_VEND_ACT_SPCURRENTLOCK_IDX,
     NULL }
 /*-------------------------------------------------------------------------*/
  ,{ "$SPNAI",
     EXTENDED | READ_ONLY | DO_PREF_CMD,
     SPECIAL_NONE,
     0,
     DSAT707_VEND_ACT_SPNAI_IDX,
     NULL }
 /*-------------------------------------------------------------------------*/
#endif /* defined(FEATURE_DSAT_SP_CMDS) */
#if defined(FEATURE_DSAT_GPS_CMDS)
 /*-------------------------------------------------------------------------*/
 ,{ "$$A_TRACKING",
    EXTENDED | DO_PREF_CMD ,
    SPECIAL_NONE,
    0,
    DSAT707_VEND_ACT_ACT_A_TRACKING_IDX,
    NULL }
 /*-------------------------------------------------------------------------*/
 ,{ "$SPGETLOCATION",
     EXTENDED | DO_PREF_CMD,
     SPECIAL_NONE,
     0,
     DSAT707_VEND_ACT_SPGETLOCATION_IDX,
     NULL }
 /*-------------------------------------------------------------------------*/
 ,{ "$SPNMEA",
    EXTENDED | CONFIG | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    2,
    DSAT707_VEND_ACT_SPNMEA_IDX,
    &dsat707_gps_nmea_config_dflm[0]}
 /*-------------------------------------------------------------------------*/
 ,{ "$SPLOCATION",
     EXTENDED | DO_PREF_CMD,
     SPECIAL_NONE,
     1,
     DSAT707_VEND_ACT_SPLOCATION_IDX,
     &dsat707_sp_lcs_state_dflm[0] }
/*-------------------------------------------------------------------------*/
 ,{ "$$GPS_MODE",
    EXTENDED | CONFIG | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    1,
    DSAT707_VEND_ACT_ACT_GPS_MODE_IDX,
    &dsat707_gps_mode_dflm[0] }
/*-------------------------------------------------------------------------*/
 ,{ "$$GPS_PORT",
    EXTENDED | CONFIG | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    1,
    DSAT707_VEND_ACT_ACT_GPS_PORT_IDX,
    &dsat707_gps_port_dflm[0]}
/*-------------------------------------------------------------------------*/
 ,{ "$$PDE_TRS",
    EXTENDED | CONFIG | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    1,
    DSAT707_VEND_ACT_ACT_PDE_TRS_IDX,
    &dsat707_pde_trs_dflm[0] }
/*-------------------------------------------------------------------------*/
 ,{ "$$GPS_PDEADDR",
    EXTENDED | CONFIG | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    5,
    DSAT707_VEND_ACT_ACT_GPS_PDEADDR_IDX,
    &dsat707_gps_pdeaddr_dflm[0] }
/*-------------------------------------------------------------------------*/
 ,{ "$$INIT_MASK",
    EXTENDED | CONFIG | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    1,
    DSAT707_VEND_ACT_ACT_INIT_MASK_IDX,
    &dsat707_gps_init_mask_dflm[0]  }
 /*-------------------------------------------------------------------------*/
 ,{ "$$GET_POS",
    EXTENDED | DO_PREF_CMD,
    SPECIAL_NONE,
    0,
    DSAT707_VEND_ACT_ACT_GET_POS_IDX,
    NULL }
/*-------------------------------------------------------------------------*/
 ,{ "$$GPS_INFO",
    EXTENDED | DO_PREF_CMD,
    SPECIAL_NONE,
    0,
    DSAT707_VEND_ACT_ACT_GPS_INFO_IDX,
    NULL }
/*-------------------------------------------------------------------------*/
 ,{ "$$GPS_STOP",
    EXTENDED | DO_PREF_CMD,
    SPECIAL_NONE,
    0,
    DSAT707_VEND_ACT_ACT_GPS_STOP_IDX,
    NULL }
/*-------------------------------------------------------------------------*/
#endif /* defined(FEATURE_DSAT_GPS_CMDS) */
/*lint -restore */
};

  /* Size of vendor specific command table */
const unsigned int dsat707_vend_para_table_size = ARR_SIZE( dsat707_vend_para_table );
const unsigned int dsat707_vend_action_table_size = ARR_SIZE( dsat707_vend_action_table );

#endif /* FEATURE_DATA_IS707 */
