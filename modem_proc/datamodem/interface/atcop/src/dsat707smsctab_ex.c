/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the SMS related AT modem commands for the IS-707 mode.

  The AT commands here are not specified in 707 spec.  They are vendor
  specific commands for automating sms testing.
  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2002 - 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707smsctab_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   sc      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_CDMA_SMS
#include "dsati.h"
#include "dsat707sms.h"
#include "dsat707smsctab.h"
#include "dsatparm.h"
#include "msg.h"


/*=========================================================================*/
/* Defaults & Limits for SMS commands */
/*=========================================================================*/

/* Teleservice Id */
LOCAL const dsat_string_item_type       qcsmp_tid_valstr [][8] =
{ "4095", "4096", "4097", "4098", "4099", "4100", "4101", "4102", ""};

LOCAL const dsat_string_item_type       qcsmp_tid_teststr[] =
{ "(\"4095\",\"4096\",\"4097\",\"4098\",\"4099\",\"4100\",\"4101\",\"4102\")" };

/* Teleservice Id uses 4099 (VMN 95) as default */
LOCAL const def_list_type               qcsmp_tid_list =
{ 3, qcsmp_tid_valstr, qcsmp_tid_teststr };

LOCAL const mixed_def_s_type            qcsmp_tid_def_val = 
{
  CONFIG | LIST | LOCAL_TEST,
  (mixed_def_u_type *) &qcsmp_tid_list
};

/* Valid Period Format, 0 Absolute, 1 Relative 
 * See EIA 637 Table 4.5.5 for Absolute Time Format
 * See EIA 637 Table 4.5.6-1 for Relative Time Format
 */
LOCAL const dflm_type                   qcsmp_vpf_dflm =
{ 0, 0, 1 };

LOCAL const mixed_def_s_type            qcsmp_vpf_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &qcsmp_vpf_dflm
};
LOCAL const word                        qcsmp_vp_len =
                                          QCSMP_VP_STR_MAX_LEN;
LOCAL const mixed_def_s_type            qcsmp_vp_def_val =
{
  CONFIG | STRING,
  (mixed_def_u_type *) &qcsmp_vp_len
};

/* Deferred Delivery Time Format, 0 Absolute, 1 Relative */
LOCAL const dflm_type                   qcsmp_ddtf_dflm =
{ 0, 0, 1 };

LOCAL const mixed_def_s_type            qcsmp_ddtf_def_val =
{
  CONFIG | LOCAL_TEST,
  (mixed_def_u_type *) &qcsmp_ddtf_dflm
};
LOCAL const uint16                      qcsmp_ddt_len = QCSMP_DDT_STR_MAX_LEN;
LOCAL const mixed_def_s_type            qcsmp_ddt_def_val =
{
  CONFIG | STRING,
  (mixed_def_u_type *) &qcsmp_ddt_len
};

LOCAL const mixed_def_s_type * qcsmp_list[] =
{
  &qcsmp_tid_def_val, &qcsmp_vpf_def_val, &qcsmp_vp_def_val, 
  &qcsmp_ddtf_def_val, &qcsmp_ddt_def_val
};

LOCAL const dsat_string_item_type qcpms_mem1_valstr[][8] =
/* Don't include SM device option if RUIM is not present */
#ifdef FEATURE_RUIM
{ "ME", "MT", "SM", "" };
#else
{ "ME", "MT", "" };
#endif /* FEATURE_RUIM */


LOCAL const dsat_string_item_type qcpms_mem1_tststr[] =
/* Don't include SM device option if RUIM is not present */
#ifdef FEATURE_RUIM
{ "(\"ME\",\"MT\",\"SM\")" };
#else
{ "(\"ME\",\"MT\")" };
#endif /* FEATURE_RUIM */


LOCAL const dsat_string_item_type qcpms_mem2_valstr[][8] =
/* Don't include SM device option if RUIM is not present */
#ifdef FEATURE_RUIM
{ "ME", "MT", "SM", "" };
#else
{ "ME", "MT", "" };
#endif /* FEATURE_RUIM */

LOCAL const dsat_string_item_type qcpms_mem2_tststr[] =
/* Don't include SM device option if RUIM is not present */
#ifdef FEATURE_RUIM
{ "(\"ME\",\"MT\",\"SM\")" };
#else
{ "(\"ME\",\"MT\")" };
#endif /* FEATURE_RUIM */


LOCAL const dsat_string_item_type qcpms_mem3_valstr[][8] =
/* Don't include SM device option if RUIM is not present */
#ifdef FEATURE_RUIM
{ "ME", "MT", "SM", "" };
#else
{ "ME", "MT", "" };
#endif /* FEATURE_RUIM */

LOCAL const dsat_string_item_type qcpms_mem3_tststr[] =
/* Don't include SM device option if RUIM is not present */
#ifdef FEATURE_RUIM
{ "(\"ME\",\"MT\",\"SM\")" };
#else
{ "(\"ME\",\"MT\")" };
#endif /* FEATURE_RUIM */

const def_list_type dsat707sms_qcpms_list[] =
{
  { 0, qcpms_mem1_valstr, qcpms_mem1_tststr},
  { 0, qcpms_mem2_valstr, qcpms_mem2_tststr},
  { 0, qcpms_mem3_valstr, qcpms_mem3_tststr}
};

/* $QCNMI Command */
LOCAL const dsat_string_item_type dsat_qcnmi_mode_valstr [][8] =
{
  "0", "1", "2", ""
};

LOCAL const dsat_string_item_type dsat_qcnmi_mode_tststr [] =
{ 
  "(0,1,2)" 
};

/* NOTE: Enable this only when WMS releases all the needed fixes  */
LOCAL const dsat_string_item_type dsat_qcnmi_mt_valstr [][8] =
{ "0", "1", "2", "3", "" };

LOCAL const dsat_string_item_type dsat_qcnmi_mt_tststr [] =
#ifdef FEATURE_DSAT_EXTENDED_SMS
  { "(1,2,3)" };
#else
  { "(0,1,2,3)" };
#endif /* FEATURE_DSAT_EXTENDED_SMS */

LOCAL const dsat_string_item_type dsat_qcnmi_bfr_valstr [][8] =
{ "0", "1", ""};

LOCAL const dsat_string_item_type dsat_qcnmi_bfr_tststr [] =
{ "(0,1)" };

#ifdef FEATURE_DSAT_EXTENDED_SMS
LOCAL const dsat_string_item_type dsat_qcnmi_ds_valstr [][8] =
{ "0", "1", "2", ""};

LOCAL const dsat_string_item_type dsat_qcnmi_ds_tststr [] =
{ "(0,1,2)" };

LOCAL const dsat_string_item_type dsat_qcnmi_bm_valstr [][8] =
{ "0","1",""};

LOCAL const dsat_string_item_type dsat_qcnmi_bm_tststr [] =
{ "(0,1)" };
#endif /* FEATURE_DSAT_EXTENDED_SMS */

const def_list_type dsat707_qcnmi_list[] =
{
  { 0, dsat_qcnmi_mode_valstr, dsat_qcnmi_mode_tststr },
  { 0, dsat_qcnmi_mt_valstr, dsat_qcnmi_mt_tststr },
#ifdef FEATURE_DSAT_EXTENDED_SMS
  { 0, dsat_qcnmi_bm_valstr, dsat_qcnmi_bm_tststr },
  { 0, dsat_qcnmi_ds_valstr, dsat_qcnmi_ds_tststr },
#endif /* FEATURE_DSAT_EXTENDED_SMS */
  { 0, dsat_qcnmi_bfr_valstr, dsat_qcnmi_bfr_tststr }
} ;

const dflm_type dsat707sms_qcmgf_dflm [] =
{
  { 1  , 1  , 1  },
};

#ifdef FEATURE_DSAT_EXTENDED_SMS

LOCAL const dsat_string_item_type dsat_smsss_ack_valstr [][8] =
{ "0", "1", "" };

LOCAL const dsat_string_item_type dsat_smsss_ack_tststr [] =
{ "(0,1)" };

LOCAL const dsat_string_item_type dsat_smsss_prt_valstr [][8] =
{ "0", "1", "2", "3", ""};

LOCAL const dsat_string_item_type dsat_smsss_prt_tststr [] =
{ "(0,1,2,3)" };

LOCAL const dsat_string_item_type dsat_smsss_fmt_valstr [][8] =
{ "0", "1", "2", "3", "4", "5", "6", "7", ""};

LOCAL const dsat_string_item_type dsat_smsss_fmt_tststr [] =
{ "(0,1,2,3,4,5,6,7)" };

LOCAL const dsat_string_item_type dsat_smsss_prv_valstr [][8] =
{ "0","1","2","3",""};

LOCAL const dsat_string_item_type dsat_smsss_prv_tststr [] =
{ "(0,1,2,3)" };

const def_list_type dsat707_smsss_list[] =
{
  { 0, dsat_smsss_ack_valstr, dsat_smsss_ack_tststr },
  { 0, dsat_smsss_prt_valstr, dsat_smsss_prt_tststr },
  { 0, dsat_smsss_fmt_valstr, dsat_smsss_fmt_tststr },
  { 0, dsat_smsss_prv_valstr, dsat_smsss_prv_tststr }
} ;

#endif /* FEATURE_DSAT_EXTENDED_SMS */

/*=========================================================================*/
/* IS-707 Sms related AT command table */
/*=========================================================================*/
const dsati_cmd_type dsat707_sms_table [] =
{
  { "$QCNMI",
    EXTENDED | CONFIG | LOCAL_TEST | LIST | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
#ifdef FEATURE_DSAT_EXTENDED_CMD
    5,
#else
    3,
#endif /* FEATURE_DSAT_EXTENDED_CMD */
    DSAT707_SMS_QCNMI_IDX,
    &dsat707_qcnmi_list[0]
  },
  { "$QCSMP",
    EXTENDED | CONFIG | MIXED_PARAM | LIST | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    5,
    DSAT707_SMS_QCSMP_IDX,
    &qcsmp_list[0]
  },
  { "$QCPMS",
    EXTENDED | CONFIG | LIST | YES_QUOTE | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    3,
    DSAT707_SMS_QCPMS_IDX,
    &dsat707sms_qcpms_list[0]
  },
  { "$QCMGR",
    EXTENDED | DO_PREF_CMD,
    SPECIAL_NONE,
    1,
    DSAT707_SMS_QCMGR_IDX,
    NULL
  },
  { "$QCMGS",
    EXTENDED | DO_PREF_CMD,
    SPECIAL_SMS,
    2,
    DSAT707_SMS_QCMGS_IDX,
    NULL
  },
  { "$QCMSS",
     EXTENDED | DO_PREF_CMD,
     SPECIAL_SMS,
     3,
     DSAT707_SMS_QCMSS_IDX,
     NULL 
  },
  { "$QCMGD",
    EXTENDED | DO_PREF_CMD,
    SPECIAL_NONE,
    2,
    DSAT707_SMS_QCMGD_IDX,
    NULL
  },
  { "$QCMGL",
    EXTENDED | DO_PREF_CMD,
    SPECIAL_NONE,
    1,
    DSAT707_SMS_QCMGL_IDX,
    NULL
  },
  { "$QCMGF",
    EXTENDED | CONFIG | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    1,
    DSAT707_SMS_QCMGF_IDX,
    &dsat707sms_qcmgf_dflm[0]
  },

  { "$QCMGW",
    EXTENDED  | DO_PREF_CMD,
    SPECIAL_NONE,
    3,
    DSAT707_SMS_QCMGW_IDX,
    NULL 
  },

#ifdef FEATURE_DSAT_EXTENDED_SMS
  { "^HCMGW",
    EXTENDED | DO_PREF_CMD,
    SPECIAL_NONE,
    3,
    DSAT707_SMS_HCMGW_IDX,
    NULL 
  },

  { "^HCMGL",
    EXTENDED | DO_PREF_CMD,
    SPECIAL_NONE,
    1,
    DSAT707_SMS_HCMGL_IDX,
    NULL
  },

  { "^HCMGS",
    EXTENDED | DO_PREF_CMD,
    SPECIAL_NONE,
    2,
    DSAT707_SMS_HCMGS_IDX,
    NULL
  },

  { "^HCMGR",
    EXTENDED | DO_PREF_CMD,
    SPECIAL_NONE,
    1,
    DSAT707_SMS_HCMGR_IDX,
    NULL
  },

  { "^HSMSSS",
    EXTENDED | CONFIG | LIST | LOCAL_TEST | DO_PREF_CMD,
    SPECIAL_NONE,
    4,
    DSAT707_SMS_HSMSSS_IDX,
    &dsat707_smsss_list[0]
  },
#endif /* FEATURE_DSAT_EXTENDED_SMS */

};

/* Size of SMS command table */
const unsigned int dsat707_sms_table_size = ARR_SIZE( dsat707_sms_table );


#endif /* FEATURE_DS_IS707 */
