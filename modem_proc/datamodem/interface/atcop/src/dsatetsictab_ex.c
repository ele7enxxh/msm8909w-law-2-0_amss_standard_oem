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

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatetsictab_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/11/14   tk      Fixed compilation errors in LTE compile out flavor.
06/27/14   sc      Initial revision (created file for Dynamic ATCoP).

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
#include "mn_cm_exp.h"

#ifdef FEATURE_DSAT_ETSI_DATA
#include "dsatetsipkt.h"
#include "dsumtspsqos.h"
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

/*---------------------------------------------------------------------------
                           Defaults and limits
---------------------------------------------------------------------------*/

/* GPRS Quality of Service Profile limits */
/*   see 3GPP TS 03.60 section 15.2       */
#define MAX_QOS_GPRS_REQ_PRECEDENCE_CLASS    3
#define MAX_QOS_GPRS_REQ_DELAY_CLASS         4
#define MAX_QOS_GPRS_REQ_RELIABILITY_CLASS   5
#define MAX_QOS_GPRS_REQ_PEAK_BITRATE_CLASS  9
#define MAX_QOS_GPRS_REQ_MEAN_BITRATE_CLASS  18

#define MAX_QOS_GPRS_MIN_PRECEDENCE_CLASS    3
#define MAX_QOS_GPRS_MIN_DELAY_CLASS         4
#define MAX_QOS_GPRS_MIN_RELIABILITY_CLASS   5
#define MAX_QOS_GPRS_MIN_PEAK_BITRATE_CLASS  9
#define MAX_QOS_GPRS_MIN_MEAN_BITRATE_CLASS  18

/* UMTS Quality of Service Profile limits */
/*   see 3GPP TS 23.107 section 6.5.1     */
/*       3GPP TS 24.008 section 10.5.6.5  */
#define MAX_QOS_UTMS_GTD_UL_BITRATE  QOS_MAX_UL_KBIT_RATE
#define MAX_QOS_UTMS_REQ_UL_BITRATE  QOS_MAX_UL_KBIT_RATE

#define MAX_QOS_UTMS_REQ_DL_BITRATE  QOS_MAX_DL_KBIT_RATE
#define MAX_QOS_UTMS_GTD_DL_BITRATE  QOS_MAX_DL_KBIT_RATE

#define MAX_QOS_UTMS_SDU_SIZE        1520
#define MAX_QOS_UTMS_TRANSFER_DELAY  4000
#define MIN_QOS_UTMS_TRANSFER_DELAY  100

#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)
/* +CBST command */
/* Command values and Test sting initialization*/
const dsat_string_item_type dsat_cbst_speed_valstr [][8] =
{ 
  "0", "7", "12", "14", "16", "17", "39", "43", "48", "51", 
       "71", "75", "80", "81", "83", "84", "116", "134", ""
};

LOCAL const dsat_string_item_type dsat_cbst_speed_tststr [] =
{ 
  "(0,7,12,14,16,17,39,43,48,51,71,75,80,81,83,84,116,134)" 
};

const dsat_string_item_type dsat_cbst_name_valstr [][8] =
{ "0", "1", "4", "" };

LOCAL const dsat_string_item_type dsat_cbst_name_tststr [] =
{ "(0,1,4)" };

const dsat_string_item_type dsat_cbst_ce_valstr [][8] =
{ "0", "1", "" };

LOCAL const dsat_string_item_type dsat_cbst_ce_tststr [] =
{ "(0,1)" };

#if defined(FEATURE_GSM)
/* GSM mode defaults */
const def_list_type dsat_cbst_list[] =
{
  { 0, dsat_cbst_speed_valstr, dsat_cbst_speed_tststr },
  { 0, dsat_cbst_name_valstr, dsat_cbst_name_tststr },
  { 1, dsat_cbst_ce_valstr, dsat_cbst_ce_tststr }
};
#elif defined(FEATURE_WCDMA)
/* WCDMA only mode defaults */
const def_list_type dsat_cbst_list[] =
{
  { 0, dsat_cbst_speed_valstr, dsat_cbst_speed_tststr },
  { 1, dsat_cbst_name_valstr, dsat_cbst_name_tststr },
  { 0, dsat_cbst_ce_valstr, dsat_cbst_ce_tststr }
};
#endif /* defined(FEATURE_GSM) */

dsat_num_item_type dsat_cbst_export_val[3] = {0};

#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_WCDMA_L1_HS_CQI_STAT
LOCAL const dflm_type dsat_cqi_dfl[1] =
{ 
  { 0, 0, 1 },     /* 0 - disable, 1 - enable */
};
#endif /* FEATURE_WCDMA_L1_HS_CQI_STAT  */
#endif /* FEATURE_DSAT_ETSI_MODE */

/* +CRLP Command */
const dflm_type dsat_crlp_dflm[][MAX_CRLP_PARAMS] =
{
  {                      /* Versions 0 & 1 */
    { 61  , 0  , 61  },  /* iws */
    { 61  , 0  , 61  },  /* mws */
    { 48  , 38 , 255 },  /* T1  */
    { 6   , 1  , 255 }   /* N2  */
  },
  {                      /* Version 2 */
    { 240 , 0  , 488 },  /* iws */
    { 240 , 0  , 488 },  /* mws */
    { 52  , 42 , 255 },  /* T1  */
    { 6   , 1  , 255 }   /* N2  */
  }
};

/* +CV120 Command */
LOCAL const dflm_type dsat_cv120_dflm[DSAT_CV120_MAX_POS] =
{
  { 1 , 1 , 1 },  /* rah */
  { 1 , 0 , 1 },  /* mfm */
  { 1 , 1 , 1 },  /* mode */
  { 0 , 0 , 0 },  /* llineg */
  { 0 , 0 , 0 },  /* assign */
  { 0 , 0 , 0 }   /* negtype */
};

dsat_num_item_type dsat_cv120_export_val = 0;

/* +CHSN */
LOCAL const dflm_type dsat_chsn_dflm [] =
{
  { 0, 0, 2 },                 /* wAiur   */
#ifdef FEATURE_DSAT_ETSI_RLP2
  { 0, 4, 4 },
  { 0, 7, 7 },
#endif /*  FEATURE_DSAT_ETSI_RLP2 */
  { MAX_BROKEN_RANGE, 0, 0 },
  { 0, 0, 0 },                 /* wRx     */
  { MAX_BROKEN_RANGE, 0, 0 },
  { 0, 0, 0 },                 /* topRx   */
  { MAX_BROKEN_RANGE, 0, 0 },
  { 0, 0, 0 },                 /* codings */
  { MAX_BROKEN_RANGE, 0, 0 },
};

#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */

/* +CSTA Command */
LOCAL const dflm_type dsat_csta_dflm [] =
{
  { DSAT_TA_UNKNOWN, DSAT_TA_UNKNOWN,       DSAT_TA_UNKNOWN },
  { DSAT_TA_UNKNOWN, DSAT_TA_INTERNATIONAL, DSAT_TA_INTERNATIONAL},
  { MAX_BROKEN_RANGE, 0, 0 },
};


/* +CSCS Command */
/* Values indices in array should match enum dsati_chset_type in dsati.h 
   ALPHA_HEX added to dsati_chset_type but not here so that conversion
   could be used in SMS functions but this character set wasn't requested 
   for +CSCS */
LOCAL const dsat_string_item_type dsat_cscs_valstr [][8] =
{ "IRA", "GSM", "UCS2", "" };

LOCAL const dsat_string_item_type dsat_cscs_tststr [] =
{ "(\"IRA\",\"GSM\",\"UCS2\")" };

LOCAL const def_list_type dsat_cscs_list =
{ 0, dsat_cscs_valstr, dsat_cscs_tststr };


/* +CR Command */
LOCAL const dsat_string_item_type dsat_cr_valstr [][8] =
{ "0", "1", ""};

LOCAL const dsat_string_item_type dsat_cr_tststr [] =
{ "(0,1)" };

const def_list_type dsat_cr_list =
{ 0, dsat_cr_valstr, dsat_cr_tststr };

/* +CRC Command */
LOCAL const dsat_string_item_type dsat_crc_valstr [][8] =
{ "0", "1", ""};

LOCAL const dsat_string_item_type dsat_crc_tststr [] =
{ "(0,1)" };

const def_list_type dsat_crc_list =
{ 0, dsat_crc_valstr, dsat_crc_tststr };

#ifdef FEATURE_WCDMA

/* +ES command */
LOCAL const word dsat_es_orig_rqst_len = ES_ORGI_RQST_MAX_LEN;

/*lint -e708 */
/*lint -e826 */
LOCAL const mixed_def_s_type dsat_es_orig_rqst_def_val =
{
  CONFIG | STRING | NO_QUOTE,
  (mixed_def_u_type *) &dsat_es_orig_rqst_len
};

/* user input should be inside this range else no input */ 
const dflm_type dsat_es_orig_dflm = 
{6, 6, 6};
LOCAL const word dsat_es_orig_fbk_len = ES_ORIG_FBK_MAX_LEN;

LOCAL const mixed_def_s_type dsat_es_orig_fbk_def_val =
{
  CONFIG | STRING | NO_QUOTE,
  (mixed_def_u_type *) &dsat_es_orig_fbk_len
};
LOCAL const word dsat_es_ans_fbk_len = ES_ANS_FBK_MAX_LEN;

LOCAL const mixed_def_s_type dsat_es_ans_fbk_def_val =
{
  CONFIG | STRING | NO_QUOTE,
  (mixed_def_u_type *) &dsat_es_ans_fbk_len
};

/* user input should be inside this range else no input */ 
const dflm_type dsat_es_ans_fbk_dflm = 
{8, 8, 8};

LOCAL const mixed_def_s_type * dsat_es_mixed_dfl[3] =
{
  &dsat_es_orig_rqst_def_val,
  &dsat_es_orig_fbk_def_val,
  &dsat_es_ans_fbk_def_val
};

dsat_num_item_type dsat_es_export_val[3] = {0};

/* +ESA command */

LOCAL const dflm_type dsat_esa_trans_idle_dflm = 
{ 0, 0, 0};

LOCAL const mixed_def_s_type dsat_esa_trans_idle_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &dsat_esa_trans_idle_dflm
};

LOCAL const word dsat_esa_framed_idle_len = ESA_FRAMED_IDLE_MAX_LEN;

LOCAL const mixed_def_s_type dsat_esa_framed_idle_def_val =
{
  CONFIG | STRING | NO_QUOTE,
  (mixed_def_u_type *) &dsat_esa_framed_idle_len
};
LOCAL const word dsat_esa_framed_un_ov_len = ESA_FRAMED_UN_OV_MAX_LEN;

LOCAL const mixed_def_s_type dsat_esa_framed_un_ov_def_val =
{
  CONFIG | STRING | NO_QUOTE,
  (mixed_def_u_type *) &dsat_esa_framed_un_ov_len
};
LOCAL const word dsat_esa_hd_auto_len = ESA_HD_AUTO_MAX_LEN;

LOCAL const mixed_def_s_type dsat_esa_hd_auto_def_val =
{
  CONFIG | STRING | NO_QUOTE,
  (mixed_def_u_type *) &dsat_esa_hd_auto_len
};

LOCAL const dflm_type dsat_esa_crc_type_dflm = 
{ 0, 0, 0};

LOCAL const mixed_def_s_type dsat_esa_crc_type_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &dsat_esa_crc_type_dflm
};

LOCAL const dflm_type dsat_esa_nrzi_en_dflm = 
{ 0, 0, 0};

LOCAL const mixed_def_s_type dsat_esa_nrzi_en_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &dsat_esa_nrzi_en_dflm
};

LOCAL const dflm_type dsat_esa_syn1_dflm = 
{ 255, 0, 255};

LOCAL const mixed_def_s_type dsat_esa_syn1_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &dsat_esa_syn1_dflm
};
LOCAL const word dsat_esa_syn2_len = ESA_SYN2_MAX_LEN;

LOCAL const mixed_def_s_type dsat_esa_syn2_def_val =
{
  CONFIG | STRING | NO_QUOTE,
  (mixed_def_u_type *) &dsat_esa_syn2_len
};

LOCAL const mixed_def_s_type * dsat_esa_mixed_dfl[8] =
{
  &dsat_esa_trans_idle_def_val,
  &dsat_esa_framed_idle_def_val,
  &dsat_esa_framed_un_ov_def_val,
  &dsat_esa_hd_auto_def_val,
  &dsat_esa_crc_type_def_val,
  &dsat_esa_nrzi_en_def_val,
  &dsat_esa_syn1_def_val,
  &dsat_esa_syn2_def_val
};
dsat_num_item_type dsat_esa_export_val[8] = {0};

#endif /* FEATURE_WCDMA */

/* cpms values */
LOCAL const dsat_string_item_type dsat_cpms_mem1_valstr[][8] =
{ "ME", "MT", "SM", "SR", "" };

LOCAL const dsat_string_item_type dsat_cpms_mem1_tststr[] =
{ "(\"ME\",\"MT\",\"SM\",\"SR\")" };

LOCAL const dsat_string_item_type dsat_cpms_mem2_valstr[][8] =
{ "ME", "MT", "SM", "SR" };

LOCAL const dsat_string_item_type dsat_cpms_mem2_tststr[] =
{ "(\"ME\",\"MT\",\"SM\",\"SR\")" };

LOCAL const dsat_string_item_type dsat_cpms_mem3_valstr[][8] =
{ "ME", "MT", "SM", "SR", "" };

LOCAL const dsat_string_item_type dsat_cpms_mem3_tststr[] =
{ "(\"ME\",\"MT\",\"SM\",\"SR\")" };

const def_list_type dsat_cpms_list[] =
{
  { 0, dsat_cpms_mem1_valstr, dsat_cpms_mem1_tststr },
  { 0, dsat_cpms_mem2_valstr, dsat_cpms_mem2_tststr },
  { 0, dsat_cpms_mem3_valstr, dsat_cpms_mem3_tststr }
};

/*  Numeric Config items  */

#ifdef FEATURE_ETSI_SMS
/* SMS Commands */
/* +CSMS Command */
LOCAL const dflm_type dsat_csms_dflm[] =
{
  { 0  , 0  , 1  }
} ;

/* +CMGF Command */
LOCAL const dflm_type dsat_cmgf_dflm [] =
{
  { 0  , 0  , 1  },
} ;
#endif /* FEATURE_ETSI_SMS */

/* +CSDH Command */
LOCAL const dflm_type dsat_csdh_dflm [] =
{
  { 0  , 0  , 1  },
} ;

LOCAL const dsat_string_item_type dsat_csdh_tststr [] =
  { "(0, 1)"};

/* +CNMI Command */
LOCAL const dsat_string_item_type dsat_cnmi_mode_valstr [][8] =
{
  "0", "1", "2", ""
};

LOCAL const dsat_string_item_type dsat_cnmi_mode_tststr [] =
{ 
  "(0,1,2)" 
};

/* NOTE: Enable this only when WMS releases all the needed fixes  */
LOCAL const dsat_string_item_type dsat_cnmi_mt_valstr [][8] =
{ "0", "1", "2", "3", "" };

LOCAL const dsat_string_item_type dsat_cnmi_mt_tststr [] =
{ "(0,1,2,3)" };

#ifdef FEATURE_ETSI_SMS_CB
LOCAL const dsat_string_item_type dsat_cnmi_bm_valstr [][8] =
{ "0", "1", "2", "" }; /* Currently <bm>=1 is not supported */

LOCAL const dsat_string_item_type dsat_cnmi_bm_tststr [] =
{ "(0,2)" };
#else
LOCAL const dsat_string_item_type dsat_cnmi_bm_valstr [][8] =
{ "0", "" };

LOCAL const dsat_string_item_type dsat_cnmi_bm_tststr [] =
{ "(0)" };
#endif 

LOCAL const dsat_string_item_type dsat_cnmi_ds_valstr [][8] =
{ "0", "1", "2", "" };

LOCAL const dsat_string_item_type dsat_cnmi_ds_tststr [] =
{ "(0,1,2)" };

LOCAL const dsat_string_item_type dsat_cnmi_bfr_valstr [][8] =
{ "0", "1", ""};

LOCAL const dsat_string_item_type dsat_cnmi_bfr_tststr [] =
{ "(0,1)" };

const def_list_type dsat_cnmi_list[] =
{
  { 0, dsat_cnmi_mode_valstr, dsat_cnmi_mode_tststr },
  { 0, dsat_cnmi_mt_valstr, dsat_cnmi_mt_tststr },
  { 0, dsat_cnmi_bm_valstr, dsat_cnmi_bm_tststr },
  { 0, dsat_cnmi_ds_valstr, dsat_cnmi_ds_tststr },
  { 0, dsat_cnmi_bfr_valstr, dsat_cnmi_bfr_tststr }
} ;
/* CSAS comand */
LOCAL const dflm_type dsat_csas_dflm [] =
{
  { 0  , 0  , 0  },  /* Currently only profile 0 is supported */
} ;

/* CRES comand */
LOCAL const dflm_type dsat_cres_dflm [] =
{
  { 0  , 0  , 0  },  /* Currently only profile 0 is supported */
} ;

/* CSCA command */
LOCAL word dsat_csca_sca_string_len = CSCA_SCA_STR_MAX_LEN;

LOCAL const mixed_def_s_type dsat_csca_sca_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_csca_sca_string_len
};

LOCAL word dsat_csca_tosca_string_len = CSCA_TOSCA_STR_MAX_LEN;

LOCAL const mixed_def_s_type dsat_csca_tosca_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_csca_tosca_string_len
};

LOCAL const mixed_def_s_type * dsat_csca_mixed_dfl[2] = 
{
  &dsat_csca_sca_def_val, &dsat_csca_tosca_def_val
};

/* CSMP Command */
LOCAL word dsat_csmp_fo_string_len = CSMP_FO_STR_MAX_LEN;

LOCAL const mixed_def_s_type dsat_csmp_fo_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_csmp_fo_string_len
};

LOCAL word dsat_csmp_vp_string_len = CSMP_VP_STR_MAX_LEN;

LOCAL const mixed_def_s_type dsat_csmp_vp_def_val =
{
  CONFIG | STRING, (mixed_def_u_type *) &dsat_csmp_vp_string_len
};

LOCAL const dflm_type dsat_csmp_pid_dflm =
{
  0, 0, 255
};

LOCAL const mixed_def_s_type dsat_csmp_pid_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_csmp_pid_dflm
};

LOCAL const dflm_type dsat_csmp_dcs_dflm =
{
  0, 0, 255
};

LOCAL const mixed_def_s_type dsat_csmp_dcs_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_csmp_dcs_dflm
};

LOCAL const mixed_def_s_type * dsat_csmp_mixed_dfl[4] = 
{
  &dsat_csmp_fo_def_val , &dsat_csmp_vp_def_val, 
  &dsat_csmp_pid_def_val, &dsat_csmp_dcs_def_val
};

#ifdef FEATURE_ETSI_SMS_CB
/* CSCB Command */
LOCAL const dflm_type dsat_cscb_mode_dflm[] =
{
  { 0, 0, 1 },
};

LOCAL const mixed_def_s_type dsat_cscb_mode_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cscb_mode_dflm
};

LOCAL word dsat_cscb_mids_string_len = CSCB_MIDS_STR_MAX_LEN;

LOCAL const mixed_def_s_type dsat_cscb_mids_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cscb_mids_string_len
};

LOCAL word dsat_cscb_dcss_string_len = CSCB_DCSS_STR_MAX_LEN;

LOCAL const mixed_def_s_type dsat_cscb_dcss_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cscb_dcss_string_len
};

LOCAL const mixed_def_s_type * dsat_cscb_mixed_dfl[3] = 
{
  &dsat_cscb_mode_def_val, 
  &dsat_cscb_mids_def_val, 
  &dsat_cscb_dcss_def_val
};
#endif /* FEATURE_ETSI_SMS_CB */

 const dflm_type dsat_cmms_dflm =
{ 0, 0, 2 };


#ifdef FEATURE_DATA_GCSD_FAX
/* define the ranges for fax parameters */

LOCAL const dflm_type dsat_fdd_dfl =
{ 0, 0, 1 };

LOCAL const dflm_type dsat_far_dfl =
{ 0, 0, 0 };

LOCAL const dflm_type dsat_fcl_dfl =
{ 0, 0, 255};

LOCAL const dflm_type dsat_fit_dfl[2] =
{ 
  { 0, 0, 255 },
  { 0, 0, 1 }
};

LOCAL const dflm_type dsat_fts_dflm =
{ 0, 0, 255};

LOCAL const dflm_type dsat_frs_dflm =
{ 0, 0, 255};

/* fth constants */
const dsat_string_item_type dsat_fth_valstr [][8] =
{"3", ""};

LOCAL const dsat_string_item_type dsat_fth_tststr [] =
{"(3)"};

LOCAL const def_list_type dsat_fth_list =
{
  0,  /*  300 */
  dsat_fth_valstr,
  dsat_fth_tststr
};

/* frh constants */
const dsat_string_item_type dsat_frh_valstr [][8] =
{"3", ""};

LOCAL const dsat_string_item_type dsat_frh_tststr [] =
{"(3)"};

LOCAL const def_list_type dsat_frh_list =
{
  0,  /* 300 */
  dsat_frh_valstr,
  dsat_frh_tststr
};

/* ftm constants */
const dsat_string_item_type dsat_ftm_valstr [][8] =
{"72", "73", "74", "96", "97", "98", "" };

LOCAL const dsat_string_item_type dsat_ftm_tststr [] =
{"(72,73,74,96,97,98)"};

LOCAL const def_list_type dsat_ftm_list =
{
  3,  /* 9600 */
  dsat_ftm_valstr,
  dsat_ftm_tststr
};

/* frm constants */
const dsat_string_item_type dsat_frm_valstr [][8] =
{"72", "73", "74", "96", "97", "98", ""};

LOCAL const dsat_string_item_type dsat_frm_tststr [] =
{"(72,73,74,96,97,98)"};

LOCAL def_list_type dsat_frm_list =
{
  3,  /* 9600 */
  dsat_frm_valstr,
  dsat_frm_tststr
};

#endif  /* FEATURE_DATA_GCSD_FAX */

#ifdef FEATURE_ETSI_SMS

#ifdef FEATURE_ETSI_SMS_PS
/* there is no default for +CGSMS,
   put 0 here to please compiler, it is not used anywhere.
*/
const dflm_type          dsat_cgsms_dflm = {0, 0, 3};

#endif /* FEATURE_ETSI_SMS_PS */

dsat_string_item_type dsat_csca_prev_sca_val[CSCA_SCA_STR_MAX_LEN+1];
dsat_string_item_type dsat_csca_prev_tosca_val[CSCA_TOSCA_STR_MAX_LEN+1];

dsat_mixed_param_val_type dsat_csca_prev_val[2] = {
  {dsat_csca_prev_sca_val}, 
  {dsat_csca_prev_tosca_val}
};
#endif /* FEATURE_ETSI_SMS */

dsat_num_item_type dsat_cmgw_val[3][25];

#ifdef FEATURE_CMGC
dsat_num_item_type dsat_cmgc_val[6][25];
#endif /* FEATURE_CMGC */


LOCAL const dflm_type dsat_cmod_dflm [] =
{
  { 0  , 0  , 0 },
} ;

/* +CEN Command */

LOCAL const dflm_type dsat_cen_dflm [] =
{
  { 0  , 0  , 1 },
} ;

#ifdef FEATURE_DSAT_LTE
  
LOCAL const dflm_type dsat_cemode_dflm [] =
{
  { 0  , 0  , 3 },
} ;
 
#endif /* FEATURE_DSAT_LTE */

/* +CVHU Command */
/* Default value of command is 1 which violates 27.007 recommendation but 
   was required so that ATH wouldn't end voice calls during concurrent calls */
LOCAL const dflm_type dsat_cvhu_dflm [] =
{
  { 1  , 0  , 1 },
} ;

#ifdef FEATURE_DSAT_ETSI_DATA

const dflm_type dsat_pdp_cid_dflm[] =
{
  { DSAT_LEGACY_PROFILE_NUM_MIN, DSAT_LEGACY_PROFILE_NUM_MIN, DSAT_LEGACY_PROFILE_NUM_MAX},
  { DSAT_LEGACY_PROFILE_NUM_MIN, DSAT_EXT_PROFILE_NUM_MIN, DSAT_EXT_PROFILE_NUM_MAX },
  { MAX_BROKEN_RANGE, 0, 0}
} ;

const dflm_type dsat_emergency_dflm[] =
{
    { 0  , 0  , 1 },
} ;

/* Parameter that controls how the MT/TA requests to get the IPv4 address 
   information */
const dflm_type dsat_cgdcont_ipv4_addr_alloc_dflm[] =
{
  { 0 , 0 , 1}, /* Defualt - 0 - Address Allocation through NAS Signaling  */
} ;


#ifdef FEATURE_DATA_PS_IPV6
  const dsat_string_item_type dsat_pdptype_valstr [][8] =
  { "IP", "PPP", "IPV6","IPV4V6","" };

  LOCAL const dsat_string_item_type dsat_pdptype_tststr [] =
  { "(\"IP\",\"PPP\",\"IPV6\",\"IPV4V6\")" };
#else  /* FEATURE_DATA_PS_IPV6 */
  const dsat_string_item_type dsat_pdptype_valstr [][8] =
  { "IP", "PPP", "IPV4V6","" };
  
  LOCAL const dsat_string_item_type dsat_pdptype_tststr [] =
  { "(\"IP\",\"PPP\",\"IPV4V6\")" };
#endif /* FEATURE_DATA_PS_IPV6 */

/* CGDCONT Command */
#ifdef FEATURE_GPRS_COMP_ENUM_TYPES
const dsat_string_item_type dsat_cgdcont_hcomp_valstr [][8] =
{ "0", "1", "2",
  "3",
  "4",
  "" };

/* Both IPHC and ROHC are ON */
const dsat_string_item_type dsat_cgdcont_hcomp_tststr[]
  = { "(0-4)" };

const dsat_string_item_type dsat_cgdcont_dcomp_valstr [][8] =
{ "0", "1","2", "" };

const dsat_string_item_type dsat_cgdcont_dcomp_tststr [] =
{ "(0-2)" };

#else /* FEATURE_GPRS_COMP_ENUM_TYPES */

const dsat_string_item_type dsat_cgdcont_hcomp_valstr [][8] =
{ "0", "1", "" };
const dsat_string_item_type dsat_cgdcont_dcomp_valstr [][8] =
{ "0", "1","" };
const dsat_string_item_type dsat_cgdcont_hcomp_tststr [] =
{ "(0-1)" };
const dsat_string_item_type dsat_cgdcont_dcomp_tststr [] =
{ "(0-1)" };
#endif /* FEATURE_GPRS_COMP_ENUM_TYPES */

LOCAL const def_list_type dsat_cgdcont_pdptype_list =
{ 0, dsat_pdptype_valstr, dsat_pdptype_tststr };

LOCAL const def_list_type dsat_cgdcont_hcomp_list =
{ 0, dsat_cgdcont_hcomp_valstr, dsat_cgdcont_hcomp_tststr };

LOCAL const def_list_type dsat_cgdcont_dcomp_list =
{ 0, dsat_cgdcont_dcomp_valstr, dsat_cgdcont_dcomp_tststr };

/* Allow quotes on following strings */
LOCAL const word dsat_cgdcont_apn_string_len = DS_PROFILE_3GPP_MAX_APN_STRING_LEN+2;
LOCAL const word dsat_cgdcont_pdpaddr_string_len = MAX_IPADDR_STR_LEN+2;


LOCAL const mixed_def_s_type dsat_pdp_cid_def_val =
{
  CONFIG | LOCAL_TEST | BROKEN_RANGE, (mixed_def_u_type *) dsat_pdp_cid_dflm
};

LOCAL const mixed_def_s_type dsat_cgdcont_ipv4_addr_alloc_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgdcont_ipv4_addr_alloc_dflm
};

LOCAL const mixed_def_s_type dsat_cgdcont_pdptype_def_val =
{
  CONFIG | LIST | LOCAL_TEST | YES_QUOTE,
  (mixed_def_u_type *) &dsat_cgdcont_pdptype_list
};

LOCAL const mixed_def_s_type dsat_cgdcont_apn_def_val =
{
  CONFIG | STRING | YES_QUOTE,
  (mixed_def_u_type *) &dsat_cgdcont_apn_string_len
};

LOCAL const mixed_def_s_type dsat_cgdcont_pdpaddr_def_val =
{
  CONFIG | STRING | NO_QUOTE,
  (mixed_def_u_type *) &dsat_cgdcont_pdpaddr_string_len
};

LOCAL const mixed_def_s_type dsat_cgdcont_hcomp_def_val =
{
  CONFIG |  LIST | LOCAL_TEST, (mixed_def_u_type *) &dsat_cgdcont_hcomp_list
};

LOCAL const mixed_def_s_type dsat_cgdcont_dcomp_def_val =
{
  CONFIG | LIST | LOCAL_TEST, (mixed_def_u_type *) &dsat_cgdcont_dcomp_list
};

LOCAL const mixed_def_s_type dsat_cgdcont_emergency_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_emergency_dflm
};

LOCAL const mixed_def_s_type * dsat_cgdcont_mixed_dfl[8] = 
{
  &dsat_pdp_cid_def_val,
  &dsat_cgdcont_pdptype_def_val,
  &dsat_cgdcont_apn_def_val,
  &dsat_cgdcont_pdpaddr_def_val,
  &dsat_cgdcont_dcomp_def_val,
  &dsat_cgdcont_hcomp_def_val,
  &dsat_cgdcont_ipv4_addr_alloc_val,
  &dsat_cgdcont_emergency_def_val
};

#ifdef FEATURE_SECONDARY_PDP
/* CGDSCONT Command */
LOCAL const mixed_def_s_type dsat_pdp_p_cid_def_val =
{
  CONFIG | LOCAL_TEST | BROKEN_RANGE, (mixed_def_u_type *) dsat_pdp_cid_dflm
};

LOCAL const mixed_def_s_type * dsat_cgdscont_mixed_dfl[4] = 
{
  &dsat_pdp_cid_def_val,
  &dsat_pdp_p_cid_def_val,
  &dsat_cgdcont_dcomp_def_val,
  &dsat_cgdcont_hcomp_def_val
};

/* CGTFT Command */
#define DSAT_MAX_FILTER_ID  16

const dflm_type dsat_cgtft_pfid_dflm [] =
{
  { 1, 1, DSAT_MAX_FILTER_ID }, /* Allowed Filter ID from 1 to 255*/
};

const dflm_type dsat_cgtft_pn_nh_dflm [] =
{
  { 0, 0, 255 },
};

const dflm_type dsat_cgtft_spi_dflm [] =
{
  { 0, 0, (dsat_num_item_type) 0xFFFFFFFF },
};

const dflm_type dsat_cgtft_prec_dflm [] =
{
  { 0, 0, 255 },
};

const dflm_type dsat_cgtft_flow_dflm [] =
{
  { 0, 0, 0xFFFFF },
};

LOCAL const mixed_def_s_type dsat_cgtft_pfid_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgtft_pfid_dflm
};

LOCAL const mixed_def_s_type dsat_cgtft_pn_nh_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgtft_pn_nh_dflm
};

LOCAL const mixed_def_s_type dsat_cgtft_spi_def_val =
{
  CONFIG | LOCAL_TEST | HEX, (mixed_def_u_type *) dsat_cgtft_spi_dflm
};

LOCAL const mixed_def_s_type dsat_cgtft_prec_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgtft_prec_dflm
};

LOCAL const mixed_def_s_type dsat_cgtft_flow_def_val =
{
  CONFIG | LOCAL_TEST | HEX, (mixed_def_u_type *) dsat_cgtft_flow_dflm
};
LOCAL const word dsat_cgtft_addr_subnet_string_len = (MAX_ADDR_SUBNET_STRING_LEN+2);
LOCAL const word dsat_cgtft_port_string_len = (MAX_PORT_STRING_LEN+2);
LOCAL const word dsat_cgtft_tos_tclass_string_len = (MAX_TOS_TCLASS_STRING_LEN+2);

LOCAL const mixed_def_s_type dsat_cgtft_addr_subnet_def_val =
{
  CONFIG | STRING | NO_QUOTE,
  (mixed_def_u_type *) &dsat_cgtft_addr_subnet_string_len
};

LOCAL const mixed_def_s_type dsat_cgtft_port_def_val =
{
  CONFIG | STRING | NO_QUOTE,
  (mixed_def_u_type *) &dsat_cgtft_port_string_len
};

LOCAL const mixed_def_s_type dsat_cgtft_tos_tclass_def_val =
{
  CONFIG | STRING | NO_QUOTE,
  (mixed_def_u_type *) &dsat_cgtft_tos_tclass_string_len
};

LOCAL const mixed_def_s_type * dsat_cgtft_mixed_dfl[10] = 
{
  &dsat_pdp_cid_def_val,
  &dsat_cgtft_pfid_def_val,
  &dsat_cgtft_prec_def_val,
  &dsat_cgtft_addr_subnet_def_val,
  &dsat_cgtft_pn_nh_def_val,    
  &dsat_cgtft_port_def_val,     /* destination */
  &dsat_cgtft_port_def_val,     /* source */
  &dsat_cgtft_spi_def_val,    
  &dsat_cgtft_tos_tclass_def_val,    
  &dsat_cgtft_flow_def_val
};
#endif /* FEATURE_SECONDARY_PDP */

#ifdef FEATURE_DATA_WCDMA_PS

/* CGEQREQ Command */
const dflm_type dsat_cgeqreq_tclass_dflm [] =
{
  { 4, 0, 4 },
};

const dflm_type dsat_cgeqreq_req_ul_bitrate_dflm [] =
{
  { 0, 0, MAX_QOS_UTMS_REQ_UL_BITRATE },
};

const dflm_type dsat_cgeqreq_req_dl_bitrate_dflm [] =
{
  { 0, 0, MAX_QOS_UTMS_REQ_DL_BITRATE },
};

const dflm_type dsat_cgeqreq_gtd_ul_bitrate_dflm [] =
{
  { 0, 0, MAX_QOS_UTMS_GTD_UL_BITRATE },
};

const dflm_type dsat_cgeqreq_gtd_dl_bitrate_dflm [] =
{
  { 0, 0, MAX_QOS_UTMS_GTD_DL_BITRATE },
};

const dflm_type dsat_cgeqreq_dlvry_order_dflm [] =
{
  { 2, 0, 2 },
};

const dflm_type dsat_cgeqreq_max_sdu_size_dflm [] =
{
  { 0, 0, MAX_QOS_UTMS_SDU_SIZE },
};

const dflm_type dsat_cgeqreq_src_stat_desc_dflm [] =
{
  { 0, 0, 1 },
};

const dflm_type dsat_cgeqreq_sig_ind_dflm [] =
{
  { 0, 0, 1 },
};

const dsat_string_item_type dsat_cgeqreq_sdu_err_valstr [][8] =
{ "0E0", "1E1", "1E2", "7E3", "1E3", "1E4", "1E5", "1E6", "" };

const dsat_string_item_type dsat_cgeqreq_sdu_err_tststr [] =
{ "(\"0E0\",\"1E1\",\"1E2\",\"7E3\",\"1E3\",\"1E4\",\"1E5\",\"1E6\")" };

const dsat_string_item_type dsat_cgeqreq_res_biterr_valstr [][8] =
{ "0E0", "5E2", "1E2", "5E3", "4E3", "1E3", "1E4", "1E5", "1E6", "6E8", "" };

const dsat_string_item_type dsat_cgeqreq_res_biterr_tststr [] =
{ "(\"0E0\",\"5E2\",\"1E2\",\"5E3\",\"4E3\",\"1E3\",\"1E4\",\"1E5\",\"1E6\",\"6E8\")" };

const dflm_type dsat_cgeqreq_dlvry_err_dflm [] =
{
  { 3, 0, 3 },
};

const dflm_type dsat_cgeqreq_trans_delay_dflm [] =
{
  { 0, 0, 0 },
  { 0, MIN_QOS_UTMS_TRANSFER_DELAY, MAX_QOS_UTMS_TRANSFER_DELAY },
  { MAX_BROKEN_RANGE, 0, 0 },
};

const dflm_type dsat_cgeqreq_thandle_prio_dflm [] =
{
  { 0, 0, 3 },
};

LOCAL const def_list_type dsat_cgeqreq_sdu_err_list =
{ 0, dsat_cgeqreq_sdu_err_valstr, dsat_cgeqreq_sdu_err_tststr };

LOCAL const def_list_type dsat_cgeqreq_res_biterr_list =
{ 0, dsat_cgeqreq_res_biterr_valstr, dsat_cgeqreq_res_biterr_tststr };


LOCAL const mixed_def_s_type dsat_cgeqreq_tclass_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqreq_tclass_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqreq_req_ul_bitrate_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqreq_req_ul_bitrate_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqreq_req_dl_bitrate_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqreq_req_dl_bitrate_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqreq_gtd_ul_bitrate_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqreq_gtd_ul_bitrate_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqreq_gtd_dl_bitrate_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqreq_gtd_dl_bitrate_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqreq_max_sdu_size_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqreq_max_sdu_size_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqreq_dlvry_order_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqreq_dlvry_order_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqreq_dlvry_err_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqreq_dlvry_err_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqreq_trans_delay_def_val =
{
  CONFIG | LOCAL_TEST | BROKEN_RANGE,
  (mixed_def_u_type *) dsat_cgeqreq_trans_delay_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqreq_thandle_prio_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqreq_thandle_prio_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqreq_src_stat_desc_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqreq_src_stat_desc_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqreq_sig_ind_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqreq_sig_ind_dflm
};
const word dsat_cgeqreq_sdu_err_string_len = MAX_QOSERR_STRING_LEN;
const word dsat_cgeqreq_res_biterr_string_len = MAX_QOSERR_STRING_LEN;

LOCAL const mixed_def_s_type dsat_cgeqreq_sdu_err_def_val =
{
  CONFIG | LIST | LOCAL_TEST | YES_QUOTE,
  (mixed_def_u_type *) &dsat_cgeqreq_sdu_err_list
};

LOCAL const mixed_def_s_type dsat_cgeqreq_res_biterr_def_val =
{
  CONFIG | LIST | LOCAL_TEST | YES_QUOTE,
  (mixed_def_u_type *) &dsat_cgeqreq_res_biterr_list
};

LOCAL const mixed_def_s_type * dsat_cgeqreq_mixed_dfl[15] = 
{
  &dsat_pdp_cid_def_val,
  &dsat_cgeqreq_tclass_def_val,
  &dsat_cgeqreq_req_ul_bitrate_def_val,          /* Requested UL */
  &dsat_cgeqreq_req_dl_bitrate_def_val,          /* Requested DL */
  &dsat_cgeqreq_gtd_ul_bitrate_def_val,          /* Guaranteed UL */
  &dsat_cgeqreq_gtd_dl_bitrate_def_val,          /* Guaranteed DL */
  &dsat_cgeqreq_dlvry_order_def_val,
  &dsat_cgeqreq_max_sdu_size_def_val,
  &dsat_cgeqreq_sdu_err_def_val,
  &dsat_cgeqreq_res_biterr_def_val,
  &dsat_cgeqreq_dlvry_err_def_val,
  &dsat_cgeqreq_trans_delay_def_val,
  &dsat_cgeqreq_thandle_prio_def_val,
  &dsat_cgeqreq_src_stat_desc_def_val,
  &dsat_cgeqreq_sig_ind_def_val
};
/* CGEQMIN Command */
const dflm_type dsat_cgeqmin_tclass_dflm [] =
{
  { 4, 0, 4 },
};

const dflm_type dsat_cgeqmin_req_ul_bitrate_dflm [] =
{
  { 0, 0, MAX_QOS_UTMS_REQ_UL_BITRATE },
};

const dflm_type dsat_cgeqmin_req_dl_bitrate_dflm [] =
{
  { 0, 0, MAX_QOS_UTMS_REQ_DL_BITRATE },
};

const dflm_type dsat_cgeqmin_gtd_ul_bitrate_dflm [] =
{
  { 0, 0, MAX_QOS_UTMS_GTD_UL_BITRATE },
};

const dflm_type dsat_cgeqmin_gtd_dl_bitrate_dflm [] =
{
  { 0, 0, MAX_QOS_UTMS_GTD_DL_BITRATE },
};

const dflm_type dsat_cgeqmin_dlvry_order_dflm [] =
{
  { 2, 0, 2 },
};

const dflm_type dsat_cgeqmin_max_sdu_size_dflm [] =
{
  { 0, 0, MAX_QOS_UTMS_SDU_SIZE },
};

const dsat_string_item_type dsat_cgeqmin_sdu_err_valstr [][8] =
{ "0E0", "1E1", "1E2", "7E3", "1E3", "1E4", "1E5", "1E6", "" };

const dsat_string_item_type dsat_cgeqmin_sdu_err_tststr [] =
{ "(\"0E0\",\"1E1\",\"1E2\",\"7E3\",\"1E3\",\"1E4\",\"1E5\",\"1E6\")" };

const dsat_string_item_type dsat_cgeqmin_res_biterr_valstr [][8] =
{ "0E0", "5E2", "1E2", "5E3", "4E3", "1E3", "1E4", "1E5", "1E6", "6E8", "" };

const dsat_string_item_type dsat_cgeqmin_res_biterr_tststr [] =
{ "(\"0E0\",\"5E2\",\"1E2\",\"5E3\",\"4E3\",\"1E3\",\"1E4\",\"1E5\",\"1E6\",\"6E8\")" };

const dflm_type dsat_cgeqmin_dlvry_err_dflm [] =
{
  { 3, 0, 3 },
};

const dflm_type dsat_cgeqmin_trans_delay_dflm [] =
{
  { 0, 0, 0 },
  { 0, MIN_QOS_UTMS_TRANSFER_DELAY, MAX_QOS_UTMS_TRANSFER_DELAY },
  { MAX_BROKEN_RANGE, 0, 0 },
};

const dflm_type dsat_cgeqmin_thandle_prio_dflm [] =
{
  { 0, 0, 3 },
};

const dflm_type dsat_cgeqmin_src_stat_desc_dflm [] =
{
  { 0, 0, 1 },
};

const dflm_type dsat_cgeqmin_sig_ind_dflm [] =
{
  { 0, 0, 1 },
};

LOCAL const def_list_type dsat_cgeqmin_sdu_err_list =
{ 0, dsat_cgeqmin_sdu_err_valstr, dsat_cgeqmin_sdu_err_tststr };

LOCAL const def_list_type dsat_cgeqmin_res_biterr_list =
{ 0, dsat_cgeqmin_res_biterr_valstr, dsat_cgeqmin_res_biterr_tststr };


LOCAL const mixed_def_s_type dsat_cgeqmin_tclass_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqmin_tclass_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqmin_req_ul_bitrate_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqmin_req_ul_bitrate_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqmin_req_dl_bitrate_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqmin_req_dl_bitrate_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqmin_gtd_ul_bitrate_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqmin_gtd_ul_bitrate_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqmin_gtd_dl_bitrate_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqmin_gtd_dl_bitrate_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqmin_max_sdu_size_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqmin_max_sdu_size_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqmin_dlvry_order_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqmin_dlvry_order_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqmin_dlvry_err_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqmin_dlvry_err_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqmin_trans_delay_def_val =
{
  CONFIG | LOCAL_TEST | BROKEN_RANGE,
  (mixed_def_u_type *) dsat_cgeqmin_trans_delay_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqmin_thandle_prio_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqmin_thandle_prio_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqmin_src_stat_desc_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqmin_src_stat_desc_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqmin_sig_ind_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqmin_sig_ind_dflm
};

const word dsat_cgeqmin_sdu_err_string_len = MAX_QOSERR_STRING_LEN;
const word dsat_cgeqmin_res_biterr_string_len = MAX_QOSERR_STRING_LEN;

LOCAL const mixed_def_s_type dsat_cgeqmin_sdu_err_def_val =
{
  CONFIG | LIST | LOCAL_TEST | YES_QUOTE,
  (mixed_def_u_type *) &dsat_cgeqmin_sdu_err_list
};

LOCAL const mixed_def_s_type dsat_cgeqmin_res_biterr_def_val =
{
  CONFIG | LIST | LOCAL_TEST | YES_QUOTE,
  (mixed_def_u_type *) &dsat_cgeqmin_res_biterr_list
};


LOCAL const mixed_def_s_type * dsat_cgeqmin_mixed_dfl[15] = 
{
  &dsat_pdp_cid_def_val,
  &dsat_cgeqmin_tclass_def_val,
  &dsat_cgeqmin_req_ul_bitrate_def_val,          /* Requested UL */
  &dsat_cgeqmin_req_dl_bitrate_def_val,          /* Requested DL */
  &dsat_cgeqmin_gtd_ul_bitrate_def_val,          /* Guaranteed UL */
  &dsat_cgeqmin_gtd_dl_bitrate_def_val,          /* Guaranteed DL */
  &dsat_cgeqmin_dlvry_order_def_val,
  &dsat_cgeqmin_max_sdu_size_def_val,
  &dsat_cgeqmin_sdu_err_def_val,
  &dsat_cgeqmin_res_biterr_def_val,
  &dsat_cgeqmin_dlvry_err_def_val,
  &dsat_cgeqmin_trans_delay_def_val,
  &dsat_cgeqmin_thandle_prio_def_val,
  &dsat_cgeqmin_src_stat_desc_def_val,
  &dsat_cgeqmin_sig_ind_def_val
};
#endif /* FEATURE_DATA_WCDMA_PS */

#ifdef FEATURE_DSAT_LTE

const dflm_type dsat_cgeqos_req_ul_gbr_dflm [] =
{
  { 0, 0, QOS_3GPP_MAX_UL_KBIT_RATE },
};

const dflm_type dsat_cgeqos_req_dl_gbr_dflm [] =
{
  { 0, 0, QOS_3GPP_MAX_DL_KBIT_RATE },
};

const dflm_type dsat_cgeqos_req_ul_mbr_dflm [] =
{
  { 0, 0, QOS_3GPP_MAX_UL_KBIT_RATE },
};

const dflm_type dsat_cgeqos_req_dl_mbr_dflm [] =
{
  { 0, 0, QOS_3GPP_MAX_DL_KBIT_RATE },
};

const dflm_type dsat_qci_dflm[] =
{
  { 0, 0, 9 },
};

LOCAL const mixed_def_s_type dsat_qci_def_val = 
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_qci_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqos_req_ul_gbr_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqos_req_ul_gbr_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqos_req_dl_gbr_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqos_req_dl_gbr_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqos_req_ul_mbr_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqos_req_ul_mbr_dflm
};

LOCAL const mixed_def_s_type dsat_cgeqos_req_dl_mbr_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgeqos_req_dl_mbr_dflm
};


LOCAL const mixed_def_s_type * dsat_cgeqos_mixed_dfl[6] = 
{
  &dsat_pdp_cid_def_val,
  &dsat_qci_def_val,
  &dsat_cgeqos_req_dl_gbr_def_val,
  &dsat_cgeqos_req_ul_gbr_def_val,
  &dsat_cgeqos_req_dl_mbr_def_val,
  &dsat_cgeqos_req_ul_mbr_def_val
};

#endif /* FEATURE_DSAT_LTE */

/* CGQREQ Command */
const dflm_type dsat_cgqreq_prec_dflm [] =
{
  { 0, 0, MAX_QOS_GPRS_REQ_PRECEDENCE_CLASS },
};

const dflm_type dsat_cgqreq_delay_dflm [] =
{
  { 0, 0, MAX_QOS_GPRS_REQ_DELAY_CLASS },
};

const dflm_type dsat_cgqreq_rel_dflm [] =
{
  { 0, 0, MAX_QOS_GPRS_REQ_RELIABILITY_CLASS },
};

const dflm_type dsat_cgqreq_peak_dflm [] =
{
  { 0, 0, MAX_QOS_GPRS_REQ_PEAK_BITRATE_CLASS },
};

const dflm_type dsat_cgqreq_mean_dflm [] =
{
  { 0,  0, MAX_QOS_GPRS_REQ_MEAN_BITRATE_CLASS },
  { 0, 31, 31 },
  { MAX_BROKEN_RANGE, 0, 0 },
};

LOCAL const mixed_def_s_type dsat_cgqreq_prec_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgqreq_prec_dflm
};

LOCAL const mixed_def_s_type dsat_cgqreq_delay_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgqreq_delay_dflm
};

LOCAL const mixed_def_s_type dsat_cgqreq_rel_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgqreq_rel_dflm
};

LOCAL const mixed_def_s_type dsat_cgqreq_peak_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgqreq_peak_dflm
};

LOCAL const mixed_def_s_type dsat_cgqreq_mean_def_val =
{
  CONFIG | LOCAL_TEST | BROKEN_RANGE,
  (mixed_def_u_type *) dsat_cgqreq_mean_dflm
};

LOCAL const mixed_def_s_type * dsat_cgqreq_mixed_dfl[6] = 
{
  &dsat_pdp_cid_def_val,
  &dsat_cgqreq_prec_def_val,
  &dsat_cgqreq_delay_def_val,
  &dsat_cgqreq_rel_def_val,
  &dsat_cgqreq_peak_def_val,
  &dsat_cgqreq_mean_def_val
};

/* CGQMIN Command */
const dflm_type dsat_cgqmin_prec_dflm [] =
{
  { 0, 0, MAX_QOS_GPRS_MIN_PRECEDENCE_CLASS },
};

const dflm_type dsat_cgqmin_delay_dflm [] =
{
  { 0, 0, MAX_QOS_GPRS_MIN_DELAY_CLASS },
};

const dflm_type dsat_cgqmin_rel_dflm [] =
{
  { 0, 0, MAX_QOS_GPRS_MIN_RELIABILITY_CLASS },
};

const dflm_type dsat_cgqmin_peak_dflm [] =
{
  { 0, 0, MAX_QOS_GPRS_MIN_PEAK_BITRATE_CLASS },
};

const dflm_type dsat_cgqmin_mean_dflm [] =
{
  { 0,  0, MAX_QOS_GPRS_MIN_MEAN_BITRATE_CLASS },
  { 0, 31, 31 },
  { MAX_BROKEN_RANGE, 0, 0 },
};

LOCAL const mixed_def_s_type dsat_cgqmin_prec_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgqmin_prec_dflm
};

LOCAL const mixed_def_s_type dsat_cgqmin_delay_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgqmin_delay_dflm
};

LOCAL const mixed_def_s_type dsat_cgqmin_rel_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgqmin_rel_dflm
};

LOCAL const mixed_def_s_type dsat_cgqmin_peak_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_cgqmin_peak_dflm
};

LOCAL const mixed_def_s_type dsat_cgqmin_mean_def_val =
{
  CONFIG | LOCAL_TEST | BROKEN_RANGE,
  (mixed_def_u_type *) dsat_cgqmin_mean_dflm
};

LOCAL const mixed_def_s_type * dsat_cgqmin_mixed_dfl[6] = 
{
  &dsat_pdp_cid_def_val,
  &dsat_cgqmin_prec_def_val,
  &dsat_cgqmin_delay_def_val,
  &dsat_cgqmin_rel_def_val,
  &dsat_cgqmin_peak_def_val,
  &dsat_cgqmin_mean_def_val
};


#ifdef FEATURE_DSAT_ETSI_DATA
/* +CGEREP command */
LOCAL const dflm_type dsat_cgerep_dflm [] =
{
  { 0  , 0  , 2  },   /* mode */
  { 0  , 0  , 1  },   /* bfr  */
} ;

/* +CGPIAF command */

LOCAL const dflm_type dsat_cgpiaf_dflm[] =
{
  { 0  , 0  , 1  },   /* Address Format */
  { 0  , 0  , 1  },   /* Subnet Notation  */
  { 0  , 0  , 1  },   /* Leading Zeros  */
  { 0  , 0  , 1  },   /* Compress Zeros  */
} ;
#endif /* FEATURE_DSAT_ETSI_DATA */


/* +CGATT command */
#ifdef FEATURE_ETSI_ATTACH
LOCAL const dsat_string_item_type dsat_cgatt_valstr [][8] =
{ "0", "1", "" } ;

LOCAL const dsat_string_item_type dsat_cgatt_tststr [] =
{ "(0,1)" };
#else
const dsat_string_item_type dsat_cgatt_valstr [][8] =
{ "" } ;

LOCAL const dsat_string_item_type dsat_cgatt_tststr [] =
{ "" };
#endif /* FEATURE_ETSI_ATTACH */

LOCAL def_list_type dsat_cgatt_list =
{
  0,
  dsat_cgatt_valstr,
  dsat_cgatt_tststr
};

/* +CGACT command */
LOCAL const dsat_string_item_type dsat_cgact_state_valstr [][8] =
{ "0", "1", "" } ;

LOCAL const dsat_string_item_type dsat_cgact_state_tststr [] =
{ "(0,1)" };

LOCAL def_list_type dsat_cgact_list =
{
  0,
  dsat_cgact_state_valstr,
  dsat_cgact_state_tststr
};

#ifdef FEATURE_DATA_TE_MT_PDP
/*  Numeric Config items +CGAUTO */
LOCAL const dflm_type dsat_cgauto_dflm [] =
{
  {3, 0, 3} /* defaulting +CGAUTO to 3 */ 
}; 
#endif /* FEATURE_DATA_TE_MT_PDP */

#if defined(FEATURE_GSM)

/* +CGCLASS command */
#if defined( FEATURE_WCDMA )
LOCAL const dsat_string_item_type dsat_cgclass_valstr [][8] =
{ "A", "" };

LOCAL const dsat_string_item_type dsat_cgclass_tststr [] =
{ "(\"A\")" };

#elif defined (FEATURE_GSM_GPRS)
const dsat_string_item_type dsat_cgclass_valstr [][8] =
{ "B", "" };

LOCAL const dsat_string_item_type dsat_cgclass_tststr [] =
{ "(\"B\")" };
#endif /* defined( FEATURE_WCDMA )   */

LOCAL def_list_type dsat_cgclass_list =
{
  0,
  dsat_cgclass_valstr,
  dsat_cgclass_tststr
};
#endif /* defined(FEATURE_GSM) */

#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DATA_ETSI_PIN 
/* CPIN Command */
LOCAL const word dsat_cpin_pin_string_len = MAX_PIN_STRING_LEN;
LOCAL const word dsat_cpin_newpin_string_len = MAX_PIN_STRING_LEN;

LOCAL const mixed_def_s_type dsat_cpin_pin_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_cpin_pin_string_len
};

LOCAL const mixed_def_s_type dsat_cpin_newpin_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_cpin_newpin_string_len
};

LOCAL const mixed_def_s_type * dsat_cpin_mixed_dfl[2] = 
{
  &dsat_cpin_pin_def_val,
  &dsat_cpin_newpin_def_val
};
#endif /* FEATURE_DATA_ETSI_PIN */


/* +CREG Command */
LOCAL const dflm_type dsat_creg_dfl[1] =
{ 
#ifdef FEATURE_GSM_RR_CELL_CALLBACK
  { 0, 0, 2 },
  /* 0 - disable, 1 - enable, 2 - enable with location and cell id */
#else
  { 0, 0, 1 },     /* 0 - disable, 1 - enable */
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */
};

/* +CGREG Command */
LOCAL const dflm_type dsat_cgreg_dfl[1] =
{ 
#ifdef FEATURE_GSM_RR_CELL_CALLBACK
	{ 0, 0, 2 },    /* 0 - disable, 1 - enable, 2 - enable with location and cell id */
#else
	{ 0, 0, 1 },     /* 0 - disable, 1 - enable */
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */
};
/* +CEREG Command */
#ifdef FEATURE_DSAT_LTE

/* +CEREG Command */
LOCAL const dflm_type dsat_cereg_dfl[1] =
{
    { 0, 0, 2 }    /* 0 - disable, 1 - enable, 2 - enable with location and cell id */
};
#endif /* FEATURE_DSAT_LTE */


/* +CCFC Command */
#define CCFC_REASON_STR_MAX_LEN 1
#define CCFC_MODE_STR_MAX_LEN 1
/* Allow for quotes on next two address strings */
#define CCFC_NUMBER_STR_MAX_LEN (CM_MAX_NUMBER_CHARS+2)
#define CCFC_SUBADDR_STR_MAX_LEN (CM_MAX_CALLED_INFO_CHARS+2)

LOCAL word dsat_ccfc_reason_string_len = CCFC_REASON_STR_MAX_LEN;

LOCAL dsat_string_item_type ccfc_reason_val[CCFC_REASON_STR_MAX_LEN+1];

LOCAL const mixed_def_s_type dsat_ccfc_reason_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_ccfc_reason_string_len
};

word dsat_ccfc_mode_string_len = CCFC_MODE_STR_MAX_LEN;
LOCAL dsat_string_item_type ccfc_mode_val[CCFC_MODE_STR_MAX_LEN+1];

LOCAL const mixed_def_s_type dsat_ccfc_mode_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_ccfc_mode_string_len
};

word dsat_ccfc_number_string_len = CCFC_NUMBER_STR_MAX_LEN;
LOCAL dsat_string_item_type ccfc_number_val[CCFC_NUMBER_STR_MAX_LEN+1];

LOCAL const mixed_def_s_type dsat_ccfc_number_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_ccfc_number_string_len
};

word dsat_ccfc_subaddr_string_len = CCFC_SUBADDR_STR_MAX_LEN;
dsat_string_item_type ccfc_subaddr_val[CCFC_SUBADDR_STR_MAX_LEN+1];

LOCAL const mixed_def_s_type dsat_ccfc_subaddr_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_ccfc_subaddr_string_len
};

LOCAL const dflm_type dsat_ccfc_type_dflm =
{
  129, 0, 255
};

LOCAL const mixed_def_s_type dsat_ccfc_type_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_ccfc_type_dflm
};

LOCAL const dflm_type dsat_ccfc_satype_dflm =
{
  128, 0, 255
};

LOCAL const mixed_def_s_type dsat_ccfc_satype_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_ccfc_satype_dflm
};


LOCAL const dflm_type dsat_ccfc_class_dflm =
{
  7, 1, 255
};

LOCAL const mixed_def_s_type dsat_ccfc_class_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_ccfc_class_dflm
};

LOCAL const dflm_type dsat_ccfc_time_dflm =
{
 20, 1, 30
};

LOCAL const mixed_def_s_type dsat_ccfc_time_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_ccfc_time_dflm
};

LOCAL const mixed_def_s_type * dsat_ccfc_mixed_dfl[8] = 
{
  &dsat_ccfc_reason_def_val,
  &dsat_ccfc_mode_def_val,
  &dsat_ccfc_number_def_val,
  &dsat_ccfc_type_def_val,
  &dsat_ccfc_class_def_val,
  &dsat_ccfc_subaddr_def_val,
  &dsat_ccfc_satype_def_val,
  &dsat_ccfc_time_def_val
};


/* +CTFR Command */

#define CTFR_NUMBER_STR_MAX_LEN (CM_MAX_NUMBER_CHARS+2)
#define CTFR_SUBADDR_STR_MAX_LEN (CM_MAX_CALLED_INFO_CHARS+2)


word dsat_ctfr_number_string_len = CTFR_NUMBER_STR_MAX_LEN;
LOCAL dsat_string_item_type ctfr_number_val[CTFR_NUMBER_STR_MAX_LEN+1];

const mixed_def_s_type dsat_ctfr_number_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_ctfr_number_string_len
};

word dsat_ctfr_subaddr_string_len = CTFR_SUBADDR_STR_MAX_LEN;
dsat_string_item_type ctfr_subaddr_val[CTFR_SUBADDR_STR_MAX_LEN+1];


LOCAL const mixed_def_s_type dsat_ctfr_subaddr_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_ctfr_subaddr_string_len
};

LOCAL const dflm_type dsat_ctfr_type_dflm =
{
  129, 0, 255
};

LOCAL const mixed_def_s_type dsat_ctfr_type_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_ctfr_type_dflm
};

LOCAL const dflm_type dsat_ctfr_satype_dflm =
{
  128, 0, 255
};

LOCAL const mixed_def_s_type dsat_ctfr_satype_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_ctfr_satype_dflm
};

LOCAL const mixed_def_s_type * dsat_ctfr_mixed_dfl[4] = 
{
  &dsat_ctfr_number_def_val,
  &dsat_ctfr_type_def_val,
  &dsat_ctfr_subaddr_def_val,
  &dsat_ctfr_satype_def_val,
};



/* +CCUG Command */
LOCAL const dflm_type dsat_ccug_dfl[3] =
{ 
  { 0, 0, 1 },    /* enable/disable */
  { 0, 0, 10 },   /* index */
  { 0, 0, 3 },    /* info */
};

/* +COPS Command */
LOCAL const dsat_string_item_type dsat_cops_mode_valstr [][8] =
{
  "0", "1", "2", "3", "4", ""
};

LOCAL const dsat_string_item_type dsat_cops_mode_tststr [] =
{ "(0,1,2,3,4)" };

LOCAL const def_list_type dsat_cops_mode_list =
{
  0, dsat_cops_mode_valstr, dsat_cops_mode_tststr
};

LOCAL const mixed_def_s_type dsat_cops_mode_def_val =
{
  CONFIG | LIST | LOCAL_TEST, (mixed_def_u_type *) &dsat_cops_mode_list
};

LOCAL const dsat_string_item_type dsat_cops_format_valstr [][8] =
{
  "0", "1", "2", "" 
};

LOCAL const dsat_string_item_type dsat_cops_format_tststr [] =
{ "(0,1,2)" };

LOCAL const def_list_type dsat_cops_format_list =
{ 0, dsat_cops_format_valstr, dsat_cops_format_tststr };

LOCAL const mixed_def_s_type dsat_cops_format_def_val =
{
  CONFIG | LIST | LOCAL_TEST, (mixed_def_u_type *) &dsat_cops_format_list
};

LOCAL word dsat_cops_oper_string_len = COPS_OPER_STR_MAX_LEN;
LOCAL word dsat_cops_csg_id_string_len = COPS_CSG_ID_STR_MAX_LEN;
LOCAL const mixed_def_s_type dsat_cops_oper_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cops_oper_string_len
};

LOCAL const mixed_def_s_type dsat_cops_csg_id_def_val =
{
  CONFIG | STRING , (mixed_def_u_type *) &dsat_cops_csg_id_string_len
};

#ifdef FEATURE_DSAT_LTE 
LOCAL const dsat_string_item_type dsat_cops_act_valstr [][8] =
{
  "0", "2", "7", ""  /* 0-GSM   2-UTRAN   7-E-UTRAN */
};

LOCAL const dsat_string_item_type dsat_cops_act_tststr [] =
{ "(0,2,7)" };
#elif defined (FEATURE_WCDMA)
LOCAL const dsat_string_item_type dsat_cops_act_valstr [][8] =
{
  "0", "2", ""
};

LOCAL const dsat_string_item_type dsat_cops_act_tststr [] =
{ "(0,2)" };
#else
const dsat_string_item_type dsat_cops_act_valstr [][8] =
{
  "0", ""
};

LOCAL const dsat_string_item_type dsat_cops_act_tststr [] =
{ "(0)" };
#endif /* FEATURE_DSAT_LTE  */

LOCAL const dsat_string_item_type dsat_cops_csg_rat_valstr [][8] =
{
  "0", "1", "2", "3"  /* 0-GSM   1-UTRAN   2-TDS   3-LTE */
};

LOCAL const dsat_string_item_type dsat_cops_csg_rat_tststr [] =
{ "(0,1,2,3)" };

LOCAL const def_list_type dsat_cops_act_list =
{
  0, dsat_cops_act_valstr, dsat_cops_act_tststr
};

LOCAL const def_list_type dsat_cops_csg_rat_list =
{
  0, dsat_cops_csg_rat_valstr, dsat_cops_csg_rat_tststr
};


LOCAL const mixed_def_s_type dsat_cops_act_def_val =
{
  CONFIG | LIST | LOCAL_TEST | NO_DISPLAY,
  (mixed_def_u_type *) &dsat_cops_act_list
};

LOCAL const mixed_def_s_type dsat_cops_csg_rat_def_val =
{
  CONFIG | LIST | LOCAL_TEST | NO_DISPLAY,
  (mixed_def_u_type *) &dsat_cops_csg_rat_list
};

const mixed_def_s_type * dsat_cops_mixed_dfl[6] = 
{
  &dsat_cops_mode_def_val,
  &dsat_cops_format_def_val,
  &dsat_cops_oper_def_val,
  &dsat_cops_act_def_val,
  &dsat_cops_csg_id_def_val,
  &dsat_cops_csg_rat_def_val
};
/* +CLCK Command */
#define CLCK_FAC_STR_MAX_LEN 4
#define CLCK_MODE_STR_MAX_LEN 1
/* Allow for quotes on next string */
#define CLCK_PASSWD_STR_MAX_LEN (MAX_PWD_CHAR+2)

word dsat_clck_fac_string_len = CLCK_FAC_STR_MAX_LEN;
LOCAL dsat_string_item_type clck_number_val[CLCK_FAC_STR_MAX_LEN+1];

LOCAL const mixed_def_s_type dsat_clck_fac_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_clck_fac_string_len
};

/* The order of facility entries must be reflected in ss_fac_e_type */
/* For all Personalization facilities the order is in cb_cmd_e_type */
#ifdef FEATURE_DATA_ETSI_PIN
#ifdef FEATURE_MMGSDI_PERSONALIZATION
const dsat_string_item_type dsat_clck_fac_valstr[][8] =
{ "\"AB\"", "\"AC\"", "\"AG\"", "\"AI\"", "\"AO\"",
  "\"IR\"", "\"OI\"", "\"OX\"", "\"SC\"", "\"FD\"",
  "\"PN\"", "\"PU\"", "\"PP\"", "\"PC\"", "\"PF\"", "" };

const dsat_string_item_type dsat_clck_fac_tststr[] =
{ "(\"AB\",\"AC\",\"AG\",\"AI\",\"AO\",\"IR\",\"OI\",\"OX\",\"SC\",\"FD\",\"PN\",\"PU\",\"PP\",\"PC\",\"PF\")" };
#else
const dsat_string_item_type dsat_clck_fac_valstr[][8] =
{ "\"AB\"", "\"AC\"", "\"AG\"", "\"AI\"", "\"AO\"",
  "\"IR\"", "\"OI\"", "\"OX\"", "\"SC\"", "\"FD\"","" };

const dsat_string_item_type dsat_clck_fac_tststr[] =
{ "(\"AB\",\"AC\",\"AG\",\"AI\",\"AO\",\"IR\",\"OI\",\"OX\",\"SC\",\"FD\")" };
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
#else
const dsat_string_item_type dsat_clck_fac_valstr[][8] =
{ "\"AB\"", "\"AC\"", "\"AG\"", "\"AI\"", "\"AO\"",
  "\"IR\"", "\"OI\"", "\"OX\"", "" };

const dsat_string_item_type dsat_clck_fac_tststr[] =
{ "(\"AB\",\"AC\",\"AG\",\"AI\",\"AO\",\"IR\",\"OI\",\"OX\")" };
#endif /* FEATURE_DATA_ETSI_PIN */


word dsat_clck_mode_string_len = CLCK_MODE_STR_MAX_LEN;
LOCAL dsat_string_item_type clck_mode_val[CLCK_MODE_STR_MAX_LEN+1];

LOCAL const mixed_def_s_type dsat_clck_mode_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_clck_mode_string_len
};

/* Add one to string length to test overflow */
word dsat_clck_passwd_string_len = CLCK_PASSWD_STR_MAX_LEN+1;
LOCAL dsat_string_item_type clck_passwd_val[CLCK_PASSWD_STR_MAX_LEN+1];

LOCAL const mixed_def_s_type dsat_clck_passwd_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_clck_passwd_string_len
};

LOCAL const dflm_type dsat_clck_class_dflm =
{
  7, 1, 255
};

LOCAL const mixed_def_s_type dsat_clck_class_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_clck_class_dflm
};

LOCAL const mixed_def_s_type * dsat_clck_mixed_dfl[4] = 
{
  &dsat_clck_fac_def_val,
  &dsat_clck_mode_def_val,
  &dsat_clck_passwd_def_val,
  &dsat_clck_class_def_val
};

/* +CPOL Command */
LOCAL const dflm_type dsat_cpol_index_dflm = 
{ 0, 0, SYS_USER_PPLMN_LIST_MAX_LENGTH};

LOCAL const mixed_def_s_type dsat_cpol_index_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &dsat_cpol_index_dflm
};

LOCAL const dflm_type dsat_cpol_format_dflm = 
{ 2, 0, 2};

LOCAL const mixed_def_s_type dsat_cpol_format_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &dsat_cpol_format_dflm
};

#ifdef FEATURE_WCDMA 
LOCAL const dflm_type dsat_cpol_act3_dflm = 
{ 0, 0, 1};
#else
const dflm_type dsat_cpol_act3_dflm = 
{ 0, 0, 0};
#endif /*FEATURE_WCDMA */
LOCAL const dflm_type dsat_cpol_act_dflm = 
{ 0, 0, 1};
LOCAL const mixed_def_s_type dsat_cpol_act_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &dsat_cpol_act_dflm
};
LOCAL const mixed_def_s_type dsat_cpol_act3_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &dsat_cpol_act3_dflm
};
LOCAL word dsat_cpol_oper_string_len = CPOL_OPER_STR_MAX_LEN;
LOCAL const mixed_def_s_type dsat_cpol_oper_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cpol_oper_string_len
};

#ifdef FEATURE_DSAT_LTE
/* E-UTRAN access technology */
const dflm_type dsat_cpol_act4_dflm = 
{ 0, 0, 1 };   /* 0 - tech not selected; 1 - tech selected; */

LOCAL const mixed_def_s_type dsat_cpol_act4_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &dsat_cpol_act4_dflm
};

LOCAL const mixed_def_s_type * dsat_cpol_mixed_dfl[7] = 
{
  &dsat_cpol_index_def_val,
  &dsat_cpol_format_def_val,
  &dsat_cpol_oper_def_val,
  &dsat_cpol_act_def_val,
  &dsat_cpol_act_def_val,
  &dsat_cpol_act3_def_val,
  &dsat_cpol_act4_def_val
};
#define MAX_CPOL_ARGS 7
#else

LOCAL const mixed_def_s_type * dsat_cpol_mixed_dfl[6] = 
{
  &dsat_cpol_index_def_val,
  &dsat_cpol_format_def_val,
  &dsat_cpol_oper_def_val,
  &dsat_cpol_act_def_val,
  &dsat_cpol_act_def_val,
  &dsat_cpol_act3_def_val
};
#define MAX_CPOL_ARGS 6
#endif /* FEATURE_DSAT_LTE */

/* +CPWD Command */
#define CPWD_FAC_STR_MAX_LEN 4
/* Allow for quotes on password strings */
/* String length is longest of either GSDI or MN password */ 
#ifdef FEATURE_DSAT_ETSI_MODE
#define CPWD_PASSWD_STR_MAX_LEN ((MAX(MMGSDI_PIN_MAX_LEN, MAX_PWD_CHAR))+2)
#else
#define CPWD_PASSWD_STR_MAX_LEN ((MAX_PWD_CHAR)+2)
#endif /* FEATURE_DSAT_ETSI_MODE */

 /* Required as part of lint error fixing 
    GSDI_PIN_IS_MAX would be true when GSDI_MAX_PIN_LEN
    is greater than MAX_PWD_CHAR; Based on GSDI_PIN_IS_MAX
    use either GSDI_MAX_PIN_LEN or MAX_PWD_CHAR accordingly.
NOTE: GSDI_PIN_IS_MAX doesnt have the MAX value  */
#ifdef FEATURE_DSAT_ETSI_MODE
#define GSDI_PIN_IS_MAX (MMGSDI_PIN_MAX_LEN == MAX(MMGSDI_PIN_MAX_LEN, MAX_PWD_CHAR))
#endif /* FEATURE_DSAT_ETSI_MODE */

LOCAL word dsat_cpwd_fac_string_len = CPWD_FAC_STR_MAX_LEN;

LOCAL const mixed_def_s_type dsat_cpwd_fac_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cpwd_fac_string_len
};

/* The order of facility entries must be reflected in ss_fac_e_type */
#ifdef FEATURE_DATA_ETSI_PIN
const dsat_string_item_type dsat_cpwd_fac_valstr[][8] =
{ "\"AB\"", "\"AC\"", "\"AG\"", "\"AI\"", "\"AO\"",
  "\"IR\"", "\"OI\"", "\"OX\"", "\"SC\"", "\"P2\"","" };

#else
const dsat_string_item_type dsat_cpwd_fac_valstr[][8] =
{ "\"AB\"", "\"AC\"", "\"AG\"", "\"AI\"", "\"AO\"",
  "\"IR\"", "\"OI\"", "\"OX\"", "" };
#endif /* FEATURE_DATA_ETSI_PIN */


/* Add one to string length to test overflow */
LOCAL word dsat_cpwd_passwd_string_len = CPWD_PASSWD_STR_MAX_LEN+1;
#ifdef GSDI_PIN_IS_MAX
LOCAL dsat_string_item_type cpwd_old_passwd_val[MMGSDI_PIN_MAX_LEN + 2];
LOCAL dsat_string_item_type cpwd_new_passwd_val[MMGSDI_PIN_MAX_LEN + 2];
#else
LOCAL dsat_string_item_type cpwd_old_passwd_val[MAX_PWD_CHAR + 2];
LOCAL dsat_string_item_type cpwd_new_passwd_val[MAX_PWD_CHAR + 2];
#endif /* GSDI_PIN_IS_MAX */

LOCAL const mixed_def_s_type dsat_cpwd_old_passwd_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cpwd_passwd_string_len
};

LOCAL const mixed_def_s_type dsat_cpwd_new_passwd_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cpwd_passwd_string_len
};

LOCAL const mixed_def_s_type * dsat_cpwd_mixed_dfl[3] = 
{
  &dsat_cpwd_fac_def_val,
  &dsat_cpwd_old_passwd_def_val,
  &dsat_cpwd_new_passwd_def_val,
};

/* +CUSD Command */
LOCAL const dflm_type dsat_cusd_n_dflm =
{
  0, 0, 2
};

LOCAL const mixed_def_s_type dsat_cusd_n_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_cusd_n_dflm
};


/* Add one to string length to test overflow */
LOCAL word dsat_cusd_str_string_len = (CUSD_STR_MAX_LEN);

LOCAL const mixed_def_s_type dsat_cusd_str_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat_cusd_str_string_len
};

LOCAL word dsat_cusd_dcs_string_len = CUSD_DCS_STR_MAX_LEN;

LOCAL const mixed_def_s_type dsat_cusd_dcs_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat_cusd_dcs_string_len
};

LOCAL const mixed_def_s_type * dsat_cusd_mixed_dfl[3] = 
{
  &dsat_cusd_n_def_val,
  &dsat_cusd_str_def_val,
  &dsat_cusd_dcs_def_val,
};
/* +CCWA Command */
/* enable/disable unsolicited result code presentation */
const dflm_type dsat_ccwa_dflm = 
{ 0, 0, 1 };


/* +CSSN */
LOCAL const dflm_type dsat_cssn_dflm [2] =
{
  { 0, 0, 1 },                 /* CSSI   */
  { 0, 0, 1 }                  /* CSSU   */
};

/* end of CSSN */

/* +CMER */
const dflm_type dsat_cmer_dflm [] =
{
  { 0, 0, 3 },                 /* mode */
  { 0, 0, 0 },                 /* keyp */
  { 0, 0, 0 },                 /* disp */
  { 0, 0, 1 },                 /* ind  */
  { 0, 0, 1 }                  /* bfr  */
};

/* +CMEC */
LOCAL const dflm_type dsat_cmec_dflm [] =
{
  { 0, 0, 2 },                 /* keyp */
  { 0, 0, 0 },                 /* disp */
  { 0, 0, 0 },                 /* ind  */
  { 0, 0, 2 }                  /* touch */  
};


/* +CIND */
/* This value list is shared for multiple indicator parameters as
 * write syntax is not supported */
LOCAL const dsat_string_item_type dsat_cind_01_valstr [][8] =
{ "0", "1", "" };

LOCAL const dsat_string_item_type dsat_cind_03_valstr [][8] =
{ "0", "1", "2", "3", "" };

LOCAL const dsat_string_item_type dsat_cind_05_valstr [][8] =
{ "0", "1", "2", "3", "4", "5", "" };

LOCAL const dsat_string_item_type dsat_cind_battchg_tststr [] =
{ "(\"battchg\",(0-5))"  };

LOCAL const dsat_string_item_type dsat_cind_signal_tststr [] =
{ "(\"signal\",(0-5))"  };

LOCAL const dsat_string_item_type dsat_cind_service_tststr [] =
{ "(\"service\",(0-1))" };

LOCAL const dsat_string_item_type dsat_cind_call_tststr [] =
{ "(\"call\",(0-1))"  };

LOCAL const dsat_string_item_type dsat_cind_roam_tststr [] =
{ "(\"roam\",(0-1))"  };

LOCAL const dsat_string_item_type dsat_cind_smsfull_tststr [] =
{ "(\"smsfull\",(0-1))"  };

LOCAL const dsat_string_item_type dsat_cind_packet_tststr [] =
{ "(\"GPRS coverage\",(0-1))"  };

LOCAL const dsat_string_item_type dsat_cind_callsetup_tststr [] =
{ "(\"callsetup\",(0-3))"  };

LOCAL const def_list_type dsat_cind_list[MAX_CIND_INDICATORS] =
{
  { 0, dsat_cind_05_valstr, dsat_cind_battchg_tststr },
  { 0, dsat_cind_05_valstr, dsat_cind_signal_tststr },
  { 0, dsat_cind_01_valstr, dsat_cind_service_tststr },
  { 0, dsat_cind_01_valstr, dsat_cind_call_tststr },
  { 0, dsat_cind_01_valstr, dsat_cind_roam_tststr },
  { 0, dsat_cind_01_valstr, dsat_cind_smsfull_tststr },
  { 0, dsat_cind_01_valstr, dsat_cind_packet_tststr },
  { 0, dsat_cind_03_valstr, dsat_cind_callsetup_tststr },
};



/* +CAOC */
LOCAL const dflm_type dsat_caoc_dflm [] =
{
  { 1  , 0  , 2  },   /* mode */
} ;

/* Timezone Change reporting settings */
LOCAL const dflm_type dsat_ctzr_dflm [] =
{
  {0, 0, 1}, /* defaulting timezone change reporting to "disable" */
};

LOCAL const dsat_string_item_type dsat_ctzr_tststr [] =
{
  "(0,1)"
};


LOCAL const dflm_type dsat_ctzu_dflm [] =
{
  {0, 0, 1}, /* defaulting timezone update to "disable" */
};

LOCAL const dsat_string_item_type dsat_ctzu_tststr [] =
{
  "(0,1)"
};

LOCAL const dflm_type dsat_clip_dflm [] =
{
  {0, 0, 1}, /* defaulting CLIP to "disable" */ 
}; 
LOCAL const dsat_string_item_type dsat_clip_tststr [] =
{
  "(0,1)"
};


LOCAL const dflm_type dsat_cdip_dflm [] =
{
  {0, 0, 1}, /* defaulting CDIP to "disable" */ 
}; 


LOCAL const dflm_type dsat_colp_dflm [] =
{
  {0, 0, 1}, /* defaulting COLP to "disable" */ 
}; 
LOCAL const dsat_string_item_type dsat_colp_tststr [] =
{
  "(0,1)"
};

LOCAL const dsat_string_item_type dsat_crsl_tststr [] =
{
  "(0,1,2,3,4,5)"
};

dsat_num_item_type dsat_crsl_val;

/* Ringer alert mode settings */
LOCAL const dflm_type dsat_calm_dflm [] =
{
  {0, 0, 2},
};

LOCAL const dsat_string_item_type dsat_calm_tststr [] =
{
  "(0,1,2)"
};

dsat_num_item_type dsat_calm_val;

#ifdef FEATURE_DSAT_ETSI_DATA
/* +CGDATA command */
LOCAL const dsat_string_item_type dsat_cgdata_valstr [][8] =
{ "PPP", "" };

LOCAL const dsat_string_item_type dsat_cgdata_tststr [] =
{ "(\"PPP\")" };

LOCAL def_list_type dsat_cgdata_list =
{
  0,
  dsat_cgdata_valstr,
  dsat_cgdata_tststr
};

#ifdef FEATURE_DATA_TE_MT_PDP
/* +CGANS command */

LOCAL const dsat_string_item_type dsat_cgans_response_valstr [][8] =
{ "0", "1", "" };

LOCAL const dsat_string_item_type dsat_cgans_response_tststr [] =
{ "(0-1)"  };

const dsat_string_item_type dsat_cgans_l2p_valstr [][8] =
{ "PPP", "" };

LOCAL const dsat_string_item_type dsat_cgans_l2p_tststr [] =
{ "(\"PPP\")" };

const def_list_type dsat_cgans_l2p_type_list =
{ 0, dsat_cgans_l2p_valstr, dsat_cgans_l2p_tststr };

const def_list_type dsat_cgans_response_list =
{ 0, dsat_cgans_response_valstr, dsat_cgans_response_tststr };

LOCAL const mixed_def_s_type dsat_cgans_l2p_type_list_def_val =
{
  CONFIG | LIST | LOCAL_TEST | YES_QUOTE,
  (mixed_def_u_type *) &dsat_cgans_l2p_type_list
};

LOCAL const mixed_def_s_type dsat_cgans_response_def_val =
{
  CONFIG |  LIST | LOCAL_TEST, (mixed_def_u_type *) &dsat_cgans_response_list
};

LOCAL const mixed_def_s_type * dsat_cgans_mixed_dfl[2] = 
{
  &dsat_cgans_response_def_val,
  &dsat_cgans_l2p_type_list_def_val
};
/* +CGANS command end */
#endif /* FEATURE_DATA_TE_MT_PDP */
#endif /* FEATURE_DSAT_ETSI_DATA */
LOCAL const dflm_type dsat_clir_dflm [] =
{
  {0, 0, 2}, /* defaulting CLIR to "presentation indicator" */ 
}; 

LOCAL const dsat_string_item_type dsat_clir_tststr [] =
{
  "(0,1,2)"
};

#ifdef FEATURE_MMGSDI
/* +CPLS command */


LOCAL const dflm_type dsat_cpls_dflm [] =
{
   {0, 0, 2}, /* defaulting CPLS to "PLMN Selector" */ 
};
/* +CPLS command End */
#endif /* FEATURE_MMGSDI */
/*lint +e826 */
/*lint +e708 */

dsat_num_item_type dsat_cvib_val;


/*--------------------------------------------------------------------------
                         ETSI Command Tables 
--------------------------------------------------------------------------*/

#ifdef FEATURE_DSAT_ETSI_MODE
/* ETSI extended AT command table */
const dsati_cmd_type dsatetsi_ext_action_table [] =
{
  { "+CSQ",    READ_ONLY | UMTS_CMD,
    SPECIAL_NONE,   0,DSATETSI_EXT_ACT_CSQ_ETSI_IDX,    NULL },

  { "+CBC",    READ_ONLY | COMMON_CMD,
    SPECIAL_NONE,   0,DSATETSI_EXT_ACT_CBC_ETSI_IDX },

  { "+CPAS",   READ_ONLY | RESTRICTED | MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,   0,DSATETSI_EXT_ACT_CPAS_ETSI_IDX ,  NULL },

#ifdef FEATURE_DATA_ETSI_PIN 
  { "+CPIN",   EXTENDED | CONFIG | MIXED_PARAM | LIST | 
               LOCAL_TEST | RESTRICTED | UMTS_CMD | MULTI_SLOT,
    SPECIAL_NONE,   2,DSATETSI_EXT_ACT_CPIN_ETSI_IDX, &dsat_cpin_mixed_dfl[0] },
#endif /* FEATURE_DATA_ETSI_PIN */

  { "+CMEC",    EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE, 4, DSATETSI_EXT_ACT_CMEC_ETSI_IDX,   &dsat_cmec_dflm[0] },

  { "+CIND",    EXTENDED | CONFIG | LIST | LOCAL_TEST | NO_RESET | RESTRICTED| MULTI_SUBS | COMMON_CMD,
    SPECIAL_NONE,   MAX_CIND_INDICATORS,DSATETSI_EXT_ACT_CIND_ETSI_IDX,  &dsat_cind_list[0]},
  
  { "+CMER",    EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,   5, DSATETSI_EXT_ACT_CMER_ETSI_IDX,   &dsat_cmer_dflm[0] },
  
#ifdef FEATURE_DSAT_ETSI_DATA
  { "+CGATT",
    EXTENDED | CONFIG | LIST | LOCAL_TEST | NO_RESET| MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,   1, DSATETSI_EXT_ACT_CGATT_ETSI_IDX,    &dsat_cgatt_list},

  { "+CGACT",
    EXTENDED | CONFIG | LIST | LOCAL_TEST | NO_RESET | MULTI_INDEX | UMTS_CMD,
    SPECIAL_NONE,   1, DSATETSI_EXT_ACT_CGACT_ETSI_IDX,   &dsat_cgact_list},

  { "+CGCMOD",
    EXTENDED | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_CGCMOD_ETSI_IDX,NULL},
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_ETSI_PBM
  { "+CPBS",    EXTENDED | CONFIG | LIST | YES_QUOTE | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_ACT_CPBS_ETSI_IDX, &dsat_cpbs_list},

  { "+CPBR",    ATTRIB_NONE | COMMON_CMD,
    SPECIAL_NONE,    2, DSATETSI_EXT_ACT_CPBR_ETSI_IDX,NULL },

  { "+CPBF",    ATTRIB_NONE | COMMON_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_ACT_CPBF_ETSI_IDX,NULL},

  { "+CPBW",    ATTRIB_NONE | COMMON_CMD,
    SPECIAL_NONE,    10,  DSATETSI_EXT_ACT_CPBW_ETSI_IDX,NULL },
#endif  /* FEATURE_ETSI_PBM */

  /* SMS Commands */
#ifdef FEATURE_ETSI_SMS
  { "+CPMS",    EXTENDED | CONFIG | LIST | YES_QUOTE | LOCAL_TEST| MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,    3, DSATETSI_EXT_ACT_CPMS_ETSI_IDX, &dsat_cpms_list[0] },

  { "+CNMI",    EXTENDED | CONFIG | LIST | LOCAL_TEST| MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,    5,DSATETSI_EXT_ACT_CNMI_ETSI_IDX,   &dsat_cnmi_list[0] },

  { "+CMGL",    EXTENDED | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_ACT_CMGL_ETSI_IDX, NULL },

  { "+CMGR",    EXTENDED | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_ACT_CMGR_ETSI_IDX, NULL },

  { "+CMGS",    EXTENDED | UMTS_CMD,
    SPECIAL_NONE,    0,DSATETSI_EXT_ACT_CMGS_ETSI_IDX, NULL},

  { "+CMSS",    EXTENDED | UMTS_CMD,
     SPECIAL_NONE,    0,DSATETSI_EXT_ACT_CMSS_ETSI_IDX, NULL},

  { "+CMGW",    EXTENDED | UMTS_CMD,
    SPECIAL_NONE,    3,DSATETSI_EXT_ACT_CMGW_ETSI_IDX, NULL},

  { "+CMGD",    EXTENDED | UMTS_CMD,
    SPECIAL_NONE,    0,DSATETSI_EXT_ACT_CMGD_ETSI_IDX,  NULL },

  { "+CMGC",    EXTENDED | UMTS_CMD,
    SPECIAL_NONE,    0, DSATETSI_EXT_ACT_CMGC_ETSI_IDX,  NULL },

  { "+CNMA",    EXTENDED | UMTS_CMD,
    SPECIAL_NONE,    2,DSATETSI_EXT_ACT_CNMA_ETSI_IDX,  NULL },

  { "+CMMS",    EXTENDED | CONFIG| MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_ACT_CMMS_ETSI_IDX,  &dsat_cmms_dflm },
#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_DATA_GCSD_FAX
  { "+FTS",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   1, DSATETSI_EXT_ACT_FTS_ETSI_IDX,    &dsat_fts_dflm},

  { "+FRS",    ABORT_CMD | EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   1, DSATETSI_EXT_ACT_FRS_ETSI_IDX,  &dsat_frs_dflm},

  { "+FTH",    EXTENDED | CONFIG | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   1, DSATETSI_EXT_ACT_FTH_ETSI_IDX,  &dsat_fth_list},

  { "+FRH",    ABORT_CMD | EXTENDED | CONFIG | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   1,DSATETSI_EXT_ACT_FRH_ETSI_IDX,    &dsat_frh_list },

  { "+FTM",    EXTENDED | CONFIG | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   1, DSATETSI_EXT_ACT_FTM_ETSI_IDX,  &dsat_ftm_list},

  { "+FRM",    ABORT_CMD | EXTENDED | CONFIG | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   1,DSATETSI_EXT_ACT_FRM_ETSI_IDX, &dsat_frm_list },
#endif  /* FEATURE_DATA_GCSD_FAX */

  { "+CHUP",       ATTRIB_NONE | RESTRICTED | MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,    0,DSATETSI_EXT_ACT_CHUP_ETSI_IDX,    NULL },

  { "+CCFC",    EXTENDED | LOCAL_TEST | LIST | MIXED_PARAM | UMTS_CMD,
    SPECIAL_NONE,    8,DSATETSI_EXT_ACT_CCFC_ETSI_IDX,   &dsat_ccfc_mixed_dfl[0] },

  { "+CCUG",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    3, DSATETSI_EXT_ACT_CCUG_ETSI_IDX,  &dsat_ccug_dfl[0] },

  { "+COPS",    ABORT_CMD | EXTENDED | LOCAL_TEST | CONFIG | LIST | MIXED_PARAM | MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,    4,DSATETSI_EXT_ACT_COPS_ETSI_IDX, &dsat_cops_mixed_dfl[0] },

  { "+CLCK",    ABORT_CMD | EXTENDED | LOCAL_TEST | LIST | MIXED_PARAM | UMTS_CMD,
    SPECIAL_NONE,    4,DSATETSI_EXT_ACT_CLCK_ETSI_IDX, &dsat_clck_mixed_dfl[0] },

  { "+CPWD",    EXTENDED | LOCAL_TEST | LIST | MIXED_PARAM | UMTS_CMD,
    SPECIAL_NONE,    3,DSATETSI_EXT_ACT_CPWD_ETSI_IDX, &dsat_cpwd_mixed_dfl[0]},

  { "+CUSD",
    EXTENDED | CONFIG | LOCAL_TEST | LIST | MIXED_PARAM | MULTI_INDEX | UMTS_CMD,
    SPECIAL_NONE,    3, DSATETSI_EXT_ACT_CUSD_ETSI_IDX,  &dsat_cusd_mixed_dfl[0] },

  { "+CAOC",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_ACT_CAOC_ETSI_IDX,      dsat_caoc_dflm },

#ifdef FEATURE_DATA_ETSI_PIN
  { "+CACM",    EXTENDED | ATTRIB_NONE | UMTS_CMD,
    SPECIAL_NONE,    0, DSATETSI_EXT_ACT_CACM_ETSI_IDX,   NULL },

  { "+CAMM",    EXTENDED | ATTRIB_NONE | UMTS_CMD,
    SPECIAL_NONE,    0, DSATETSI_EXT_ACT_CAMM_ETSI_IDX,   NULL },

  { "+CPUC",    EXTENDED | ATTRIB_NONE | UMTS_CMD,
    SPECIAL_NONE,    0, DSATETSI_EXT_ACT_CPUC_ETSI_IDX,  NULL },
#endif /* FEATURE_DATA_ETSI_PIN */

  { "+CCWA",
    EXTENDED | CONFIG | LOCAL_TEST| MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_ACT_CCWA_ETSI_IDX,   &dsat_ccwa_dflm },

  { "+CHLD",    ATTRIB_NONE | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_ACT_CHLD_ETSI_IDX, NULL },

  { "+CIMI",     READ_ONLY | UMTS_CMD,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_CIMI_ETSI_IDX, NULL },

  { "+CGMI",     READ_ONLY | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_CGMI_ETSI_IDX,   NULL },

  { "+CGMM",     READ_ONLY | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_CGMM_ETSI_IDX,   NULL },

  { "+CGMR",     READ_ONLY | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_CGMR_ETSI_IDX,    NULL},

  { "+CGSN",     READ_ONLY | RESTRICTED | MULTI_SLOT | COMMON_CMD,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_CGSN_ETSI_IDX,    NULL },

  { "+CNUM",     READ_ONLY | UMTS_CMD,
    SPECIAL_NONE,   0,DSATETSI_EXT_ACT_CNUM_ETSI_IDX,   NULL },

#ifdef FEATURE_MMGSDI_ATCSIM
  { "+CSIM",     EXTENDED | RESTRICTED | MULTI_SLOT | COMMON_CMD,
    SPECIAL_NONE,   2,DSATETSI_EXT_ACT_CSIM_ETSI_IDX,   NULL },

  { "+CRSM",     EXTENDED | RESTRICTED | MULTI_SLOT | COMMON_CMD,
    SPECIAL_NONE,   6,DSATETSI_EXT_ACT_CRSM_ETSI_IDX,    NULL },
#endif /* FEATURE_MMGSDI_ATCSIM */

  { "+CCLK",    EXTENDED | COMMON_CMD,
    SPECIAL_NONE, 1, DSATETSI_EXT_ACT_CCLK_ETSI_IDX,  NULL},
    
#ifndef FEATURE_DSAT_CUST
  { "+COPN",   ABORT_CMD | ATTRIB_NONE | UMTS_CMD,
    SPECIAL_NONE,    0,DSATETSI_EXT_ACT_COPN_ETSI_IDX,  NULL },
#endif /* FEATURE_DSAT_CUST */

#ifdef FEATURE_MMGSDI
  { "+CPOL",    ABORT_CMD | EXTENDED | LOCAL_TEST | CONFIG | LIST | MIXED_PARAM| MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE, MAX_CPOL_ARGS,DSATETSI_EXT_ACT_CPOL_ETSI_IDX,  &dsat_cpol_mixed_dfl[0] },

  { "+CPLS",     EXTENDED | CONFIG | LOCAL_TEST | MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_ACT_CPLS_ETSI_IDX,  &dsat_cpls_dflm[0] },
#endif /* FEATURE_MMGSDI */

  { "+CTZR",     EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_ACT_CTZR_ETSI_IDX,  &dsat_ctzr_dflm[0]},
    
  { "+CTZU",     EXTENDED | CONFIG | LOCAL_TEST | NO_RESET | COMMON_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_ACT_CTZU_ETSI_IDX, &dsat_ctzu_dflm[0]},

  { "+CLAC",      READ_ONLY | COMMON_CMD,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_CLAC_ETSI_IDX,     NULL},

  { "+CLIP",     ABORT_CMD | EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_ACT_CLIP_ETSI_IDX,      &dsat_clip_dflm[0]},

  { "+COLP",     ABORT_CMD | EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_ACT_COLP_ETSI_IDX,      &dsat_colp_dflm[0]},

  { "+CDIP",     ABORT_CMD | EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_ACT_CDIP_ETSI_IDX,    &dsat_cdip_dflm[0]},

  { "+CTFR",    EXTENDED | LOCAL_TEST | LIST | MIXED_PARAM | UMTS_CMD,
    SPECIAL_NONE,    4,  DSATETSI_EXT_ACT_CTFR_ETSI_IDX,  &dsat_ctfr_mixed_dfl[0] },

  { "+CLIR",      ABORT_CMD | EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_ACT_CLIR_ETSI_IDX,     &dsat_clir_dflm[0]},
#ifdef FEATURE_DATA_TE_MT_PDP
  { "+CGANS",  
    ABORT_CMD | EXTENDED | CONFIG | MIXED_PARAM | MULTI_INDEX | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   2, DSATETSI_EXT_ACT_CGANS_ETSI_IDX,  &dsat_cgans_mixed_dfl[0] },
#endif /* FEATURE_DATA_TE_MT_PDP */

#ifdef FEATURE_WCDMA_L1_HS_CQI_STAT
  { "+CQI",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_ACT_CQI_ETSI_IDX,   dsat_cqi_dfl},
#endif /* FEATURE_WCDMA_L1_HS_CQI_STAT  */

  { "+VTS",     EXTENDED | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_ACT_VTS_ETSI_IDX,  NULL},

#ifdef FEATURE_MMGSDI
  { "+ICCID",     READ_ONLY | RESTRICTED | MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_ICCID_ETSI_IDX, NULL },
#endif /* FEATURE_MMGSDI */

  { "+CUAD",     EXTENDED | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   0,DSATETSI_EXT_ACT_CUAD_ETSI_IDX,  NULL },
#ifdef FEATURE_DATA_PS_EAP 
  { "+CEAP",     EXTENDED | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   3,DSATETSI_EXT_ACT_CEAP_ETSI_IDX, NULL },

  { "+CERP",     EXTENDED | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   2,DSATETSI_EXT_ACT_CERP_ETSI_IDX, NULL },
#endif /* FEATURE_DATA_PS_EAP */

  { "+CSTF",    ATTRIB_NONE | COMMON_CMD,
    SPECIAL_NONE, 1, DSATETSI_EXT_ACT_CSTF_ETSI_IDX,   NULL},

  { "+CSDF",    ATTRIB_NONE,
    SPECIAL_NONE, 1, DSATETSI_EXT_ACT_CSDF_ETSI_IDX,   NULL},

#ifdef FEATURE_DSAT_LTE
  { "+CVMOD",    ATTRIB_NONE | UMTS_CMD,
     SPECIAL_NONE, 1, DSATETSI_EXT_ACT_CVMOD_ETSI_IDX,  NULL},
#endif /*FEATURE_DSAT_LTE*/

  { "+CEN",    EXTENDED | CONFIG | LOCAL_TEST | MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE, 1, DSATETSI_EXT_ACT_CEN_ETSI_IDX,  dsat_cen_dflm},

#ifdef FEATURE_DATA_MUX
  { "+CMUX",    EXTENDED | LOCAL_TEST,
    SPECIAL_NONE,   0, DSATETSI_EXT_ACT_CMUX_ETSI_IDX,   NULL },
#endif /* FEATURE_DATA_MUX */

  { "+CNMPSD",     EXTENDED | UMTS_CMD,
    SPECIAL_NONE,    0,DSATETSI_EXT_ACT_CNMPSD_ETSI_IDX, NULL }

  };
#endif /* FEATURE_DSAT_ETSI_MODE */


const dsati_cmd_type dsatetsi_ext_table [] =
{
#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)

  { "+CBST",    EXTENDED | CONFIG | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    3,DSATETSI_EXT_CBST_IDX, &dsat_cbst_list[0] },

  { "+CRLP",    EXTENDED | CONFIG | MULTI_INDEX | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    5,DSATETSI_EXT_CRLP_IDX,  &dsat_crlp_dflm[0][0] },
  
  { "+CV120",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    6, DSATETSI_EXT_CV120_IDX,  &dsat_cv120_dflm[0] },

  { "+CHSN",    EXTENDED | CONFIG | LOCAL_TEST | BROKEN_RANGE | UMTS_CMD,
    SPECIAL_NONE,   4,DSATETSI_EXT_CHSN_IDX, &dsat_chsn_dflm[0] },

#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */

  { "+CSSN",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    2,DSATETSI_EXT_CSSN_IDX,     &dsat_cssn_dflm[0] },

  { "+CREG",    EXTENDED | CONFIG | LOCAL_TEST | MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_CREG_IDX,    dsat_creg_dfl },

  { "+CGREG",    EXTENDED | CONFIG | LOCAL_TEST| MULTI_SUBS | UMTS_CMD,
  SPECIAL_NONE,    1, DSATETSI_EXT_CGREG_IDX,     dsat_cgreg_dfl },
#ifdef FEATURE_DSAT_LTE

  { "+CEREG",    EXTENDED | CONFIG | LOCAL_TEST| MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_CEREG_IDX,    dsat_cereg_dfl },
#endif /* FEATURE_DSAT_LTE */

  { "+CSCS",    EXTENDED | CONFIG | LIST | LOCAL_TEST | YES_QUOTE | COMMON_CMD,
    SPECIAL_NONE,   1,DSATETSI_EXT_CSCS_IDX,    &dsat_cscs_list },

  { "+CSTA",    EXTENDED | CONFIG | BROKEN_RANGE | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,   1,DSATETSI_EXT_CSTA_IDX,     &dsat_csta_dflm[0] },

  { "+CR",      EXTENDED | CONFIG | LIST | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,   1,DSATETSI_EXT_CR_IDX,    &dsat_cr_list },

  { "+CEER",     READ_ONLY | UMTS_CMD,
    SPECIAL_NONE,   0,DSATETSI_EXT_CEER_IDX ,  NULL },
  
  { "+CRC",     EXTENDED | CONFIG | LIST | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,   1,DSATETSI_EXT_CRC_IDX,     &dsat_crc_list },

#ifdef FEATURE_DSAT_ETSI_DATA
  { "+CGDCONT",
    EXTENDED | CONFIG | MIXED_PARAM | MULTI_INDEX | LIST | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,   8,DSATETSI_EXT_CGDCONT_IDX,   &dsat_cgdcont_mixed_dfl[0] },

#ifdef FEATURE_SECONDARY_PDP
  { "+CGDSCONT",
    EXTENDED | CONFIG | MIXED_PARAM | MULTI_INDEX | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   4, DSATETSI_EXT_CGDSCONT_IDX,   &dsat_cgdscont_mixed_dfl[0] },

  { "+CGTFT",
    EXTENDED | CONFIG | MIXED_PARAM | MULTI_INDEX | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   10,DSATETSI_EXT_CGTFT_IDX,   &dsat_cgtft_mixed_dfl[0] },
#endif /* FEATURE_SECONDARY_PDP */
#ifdef FEATURE_DATA_WCDMA_PS
  { "+CGEQREQ",
    EXTENDED | CONFIG | MIXED_PARAM | MULTI_INDEX | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   15,DSATETSI_EXT_CGEQREQ_IDX,  &dsat_cgeqreq_mixed_dfl[0]},

  { "+CGEQMIN",
    EXTENDED | CONFIG | MIXED_PARAM | MULTI_INDEX | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   15,DSATETSI_EXT_CGEQMIN_IDX, &dsat_cgeqmin_mixed_dfl[0]},
#endif /* FEATURE_DATA_WCDMA_PS */
#ifdef FEATURE_DSAT_LTE
  { "+CGEQOS",
    EXTENDED | CONFIG | MIXED_PARAM | MULTI_INDEX | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   6,DSATETSI_EXT_CGEQOS_IDX,    &dsat_cgeqos_mixed_dfl[0]},

  { "+CGCONTRDP",   ATTRIB_NONE | UMTS_CMD,
    SPECIAL_NONE,    0,DSATETSI_EXT_CGCONTRDP_IDX, NULL},

  { "+CGSCONTRDP",   ATTRIB_NONE | UMTS_CMD,
    SPECIAL_NONE,    0, DSATETSI_EXT_CGSCONTRDP_IDX, NULL},

  { "+CGTFTRDP",   ATTRIB_NONE | UMTS_CMD,
    SPECIAL_NONE,    0, DSATETSI_EXT_CGTFTRDP_IDX,  NULL},

  { "+CGEQOSRDP",   ATTRIB_NONE | UMTS_CMD,
    SPECIAL_NONE,    0, DSATETSI_EXT_CGEQOSRDP_IDX, NULL},

#endif /* FEATURE_DSAT_LTE */
  { "+CGQREQ",
    EXTENDED | CONFIG | MIXED_PARAM | MULTI_INDEX | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   6,  DSATETSI_EXT_CGQREQ_IDX,    &dsat_cgqreq_mixed_dfl[0]},

  { "+CGQMIN",
    EXTENDED | CONFIG | MIXED_PARAM | MULTI_INDEX | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   6, DSATETSI_EXT_CGQMIN_IDX,   &dsat_cgqmin_mixed_dfl[0] },

  { "+CGEREP",
    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   2, DSATETSI_EXT_CGEREP_IDX,    &dsat_cgerep_dflm[0]},

  { "+CGPADDR",   ATTRIB_NONE | UMTS_CMD,
    SPECIAL_NONE,    0, DSATETSI_EXT_CGPADDR_IDX,  NULL },

  { "+CGDATA",    ABORT_CMD | EXTENDED | CONFIG | LIST | LOCAL_TEST | YES_QUOTE | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_CGDATA_IDX,   &dsat_cgdata_list },

#if defined(FEATURE_GSM)
  { "+CGCLASS",    EXTENDED | CONFIG | LIST | LOCAL_TEST | YES_QUOTE | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_CGCLASS_IDX,   &dsat_cgclass_list },
#endif /* defined(FEATURE_GSM) */
#ifdef FEATURE_DATA_TE_MT_PDP
  { "+CGAUTO",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_CGAUTO_IDX, &dsat_cgauto_dflm[0] },
#endif /* FEATURE_DATA_TE_MT_PDP */

  { "+CGPIAF",EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   4, DSATETSI_EXT_CGPIAF_IDX,    &dsat_cgpiaf_dflm[0] },

#endif /* FEATURE_DSAT_ETSI_DATA */
  
  /* SMS Commands */
#ifdef FEATURE_ETSI_SMS
#ifdef FEATURE_ETSI_SMS_PS
  { "+CGSMS",   EXTENDED | CONFIG | LOCAL_TEST | NO_RESET| MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_CGSMS_IDX,        &dsat_cgsms_dflm },
#endif /* FEATURE_ETSI_SMS_PS */

  { "+CSMS",    EXTENDED | CONFIG | LOCAL_TEST| MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_CSMS_IDX,        &dsat_csms_dflm[0] },

  { "+CMGF",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_CMGF_IDX,   &dsat_cmgf_dflm[0] },

  { "+CSAS",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_CSAS_IDX,        &dsat_csas_dflm[0] },

  { "+CRES",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_CRES_IDX,      &dsat_cres_dflm[0] },

  { "+CSCA",    EXTENDED | CONFIG | MIXED_PARAM | LIST | NO_RESET| MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,    2, DSATETSI_EXT_CSCA_IDX,       &dsat_csca_mixed_dfl[0] },

  { "+CSMP",    EXTENDED | CONFIG | MIXED_PARAM | LIST | MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,    4,DSATETSI_EXT_CSMP_IDX ,  &dsat_csmp_mixed_dfl[0] },

  { "+CSDH",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1, DSATETSI_EXT_CSDH_IDX,       &dsat_csdh_dflm[0] },

#ifdef FEATURE_ETSI_SMS_CB
  { "+CSCB",    EXTENDED | CONFIG | MIXED_PARAM | LIST | UMTS_CMD,
    SPECIAL_NONE,    3,DSATETSI_EXT_CSCB_IDX,   &dsat_cscb_mixed_dfl[0] },
#endif /* FEATURE_ETSI_SMS_CB */

#endif /* FEATURE_ETSI_SMS */

#ifdef FEATURE_DATA_GCSD_FAX

  { "+FDD",  EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_FDD_IDX,     &dsat_fdd_dfl},

  { "+FAR",  EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_FAR_IDX ,      &dsat_far_dfl},

  { "+FCL",  EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_FCL_IDX,     &dsat_fcl_dfl},

  { "+FIT",  EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    2,DSATETSI_EXT_FIT_IDX,     &dsat_fit_dfl[0]},

#endif  /* FEATURE_DATA_GCSD_FAX */

#ifdef FEATURE_WCDMA
  { "+ES",      EXTENDED | CONFIG | MIXED_PARAM | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    3,DSATETSI_EXT_ES_IDX, &dsat_es_mixed_dfl[0]},

  { "+ESA",     EXTENDED | CONFIG | MIXED_PARAM | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    8,DSATETSI_EXT_ESA_IDX, &dsat_esa_mixed_dfl[0]},
#endif /* FEATURE_WCDMA */

  { "+CMOD",    EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_CMOD_IDX,    &dsat_cmod_dflm[0]},

#ifdef FEATURE_DSAT_LTE
  { "+CEMODE",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_CEMODE_IDX,        &dsat_cemode_dflm[0]},
#endif /* FEATURE_DSAT_LTE */

  { "+CVHU",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1,DSATETSI_EXT_CVHU_IDX,     &dsat_cvhu_dflm[0] }

#ifdef FEATURE_DSAT_ETSI_MODE
#ifdef FEATURE_ECALL_APP 
  ,{ "+CECALL",    RESTRICTED | UMTS_CMD, 
     SPECIAL_NONE,   0,DSATETSI_EXT_CECALL_IDX,  NULL }
#endif /* FEATURE_ECALL_APP */
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DATA_WCDMA_PS
  ,{ "+QCGEQMIN",
    EXTENDED | CONFIG | MIXED_PARAM | MULTI_INDEX | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   15,DSATETSI_EXT_CGEQMIN_IDX, &dsat_cgeqmin_mixed_dfl[0]}
#endif /* FEATURE_DATA_WCDMA_PS */

#ifdef FEATURE_DSAT_LTE
  ,{ "+QCGEQOS",
    EXTENDED | CONFIG | MIXED_PARAM | MULTI_INDEX | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   6,DSATETSI_EXT_CGEQOS_IDX,    &dsat_cgeqos_mixed_dfl[0]}
#endif /* FEATURE_DSAT_LTE */

#ifdef FEATURE_DATA_WCDMA_PS
  ,{ "+QCGEQREQ",
    EXTENDED | CONFIG | MIXED_PARAM | MULTI_INDEX | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   15,DSATETSI_EXT_CGEQREQ_IDX,  &dsat_cgeqreq_mixed_dfl[0] }
#endif /* FEATURE_DATA_WCDMA_PS */

  ,{ "+QCGQMIN",
    EXTENDED | CONFIG | MIXED_PARAM | MULTI_INDEX | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   6, DSATETSI_EXT_CGQMIN_IDX,   &dsat_cgqmin_mixed_dfl[0] }

  ,{ "+QCGQREQ",
    EXTENDED | CONFIG | MIXED_PARAM | MULTI_INDEX | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   6,  DSATETSI_EXT_CGQREQ_IDX,    &dsat_cgqreq_mixed_dfl[0] }

#ifdef FEATURE_SECONDARY_PDP
  ,{ "+QCGTFT",
    EXTENDED | CONFIG | MIXED_PARAM | MULTI_INDEX | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   10,DSATETSI_EXT_CGTFT_IDX,   &dsat_cgtft_mixed_dfl[0] }
#endif /* FEATURE_SECONDARY_PDP */

} ;


/* Command table sizes. */
const unsigned int dsatetsi_ext_table_size = ARR_SIZE( dsatetsi_ext_table );
const unsigned int dsatetsi_ext_action_table_size = ARR_SIZE( dsatetsi_ext_action_table );


