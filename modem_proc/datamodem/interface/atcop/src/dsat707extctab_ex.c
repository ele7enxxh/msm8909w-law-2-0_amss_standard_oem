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
  
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707extctab_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

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
/* Data declarations for extended commands */
/*=========================================================================*/

#if defined(FEATURE_CDMA_SMS)
extern const def_list_type dsat707_qcnmi_list[];
extern const dflm_type dsat707sms_qcmgf_dflm [];
#endif /* defined(FEATURE_CDMA_SMS) */
#ifdef FEATURE_DS_IS707A_CMUX
/* Look-up table for the numeric value of the fwd mux option. This table  */
/* must contain the same number of items as the "cmux_fwd_valstr" table,  */
/* and the value and order of the items must match exactly.               */
const word dsat707_cmux_fwd_table[15] =
{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 0 };

/* Look-up table for the numeric value of the rev mux option. This table  */
/* must contain the same number of items as the "cmux_rev_valstr" table,  */
/* and the value and order of the items must match exactly.               */
const word dsat707_cmux_rev_table[2] =
{ 1, 2 };

LOCAL const dsat_string_item_type dsat707_cmux_fwd_valstr [][8] =
{ "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "F", "0" };

LOCAL const dsat_string_item_type dsat707_cmux_fwd_tststr [] =
{ "(1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, F, 0)" };

LOCAL const dsat_string_item_type dsat707_cmux_rev_valstr [][8] =
{ "1", "2" };

LOCAL const dsat_string_item_type dsat707_cmux_rev_tststr [] =
{ "(1, 2)" };

/* The default for at+qcmux is set to C,2 here (C,2 corresponds to fwd &    */
/* rev valstr indexes of 11 & 1 respectively.                              */
LOCAL const def_list_type dsat707_cmux_list[] =
{
  { 11, dsat707_cmux_fwd_valstr, dsat707_cmux_fwd_tststr },
  { 1, dsat707_cmux_rev_valstr, dsat707_cmux_rev_tststr }
};

/* The values stored in the "cmux_val" array are the indices to the fwd &  */
/* rev mux options in the "valstr" tables (which contain strings). Use the */
/* "cmux_fwd_table" & "cmux_rev_table" to lookup the actual numeric value. */

#else  /* FEATURE_DS_IS707A_CMUX */

LOCAL const dflm_type dsat707_cmux_dflm [] =
{
  { 2  , 1  , 2    },
} ;
#endif  /* FEATURE_DS_IS707A_CMUX */

/*=========================================================================*/
/* Defaults & Limits for extended commands */
/*=========================================================================*/

#ifdef FEATURE_DSAT_CDMA_PIN 
const word dsat707_cpin_pin_string_len = MAX_PIN_STRING_LEN;
const word dsat707_cpin_newpin_string_len = MAX_PIN_STRING_LEN;

LOCAL const mixed_def_s_type dsat707_cpin_pin_def_val =
{
  CONFIG | STRING | NO_QUOTE, 
  (mixed_def_u_type *) &dsat707_cpin_pin_string_len
};

LOCAL const mixed_def_s_type dsat707_cpin_newpin_def_val =
{
  CONFIG | STRING | NO_QUOTE, 
  (mixed_def_u_type *) &dsat707_cpin_newpin_string_len
};

const mixed_def_s_type * dsat707_cpin_mixed_dfl[2] = 
{
  &dsat707_cpin_pin_def_val,
  &dsat707_cpin_newpin_def_val
};
/* +CLCK Command */
#define CDMA_CLCK_FAC_STR_MAX_LEN 4
#define CDMA_CLCK_MODE_STR_MAX_LEN 1

/* Allow for quotes on next string */
#define CDMA_CLCK_PASSWD_STR_MAX_LEN (MAX_PIN_STRING_LEN +2)

word dsat707_clck_fac_string_len = CDMA_CLCK_FAC_STR_MAX_LEN;
LOCAL dsat_string_item_type cdma_clck_number_val[CDMA_CLCK_FAC_STR_MAX_LEN+1];

const mixed_def_s_type dsat707_clck_fac_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat707_clck_fac_string_len
};

const dsat_string_item_type dsat707_clck_fac_valstr[][8] =
{ "\"SC\"", "\"FD\"","" };

const dsat_string_item_type dsat707_clck_fac_tststr[] =
{ "(\"SC\",\"FD\")" };

word dsat707_clck_mode_string_len = CDMA_CLCK_MODE_STR_MAX_LEN;
LOCAL dsat_string_item_type cdma_clck_mode_val[CDMA_CLCK_MODE_STR_MAX_LEN+1];

LOCAL const mixed_def_s_type dsat707_clck_mode_def_val =
{
  CONFIG | STRING | NO_QUOTE, (mixed_def_u_type *) &dsat707_clck_mode_string_len
};

/* Add one to string length to test overflow */
word dsat707_clck_passwd_string_len = CDMA_CLCK_PASSWD_STR_MAX_LEN+1;
LOCAL dsat_string_item_type cdma_clck_passwd_val[CDMA_CLCK_PASSWD_STR_MAX_LEN+1];

LOCAL const mixed_def_s_type dsat707_clck_passwd_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat707_clck_passwd_string_len
};

LOCAL const mixed_def_s_type * dsat707_clck_mixed_dfl[3] = 
{
  &dsat707_clck_fac_def_val,
  &dsat707_clck_mode_def_val,
  &dsat707_clck_passwd_def_val,
};

/* +CPWD Command */
#define CDMA_CPWD_FAC_STR_MAX_LEN CDMA_CLCK_FAC_STR_MAX_LEN

/* Allow for quotes on password strings */
/* String length is MMGSDI_PIN_MAX_LEN */ 
#define CDMA_CPWD_PASSWD_STR_MAX_LEN  MMGSDI_PIN_MAX_LEN

word dsat707_cpwd_fac_string_len = CDMA_CPWD_FAC_STR_MAX_LEN;

const mixed_def_s_type dsat707_cpwd_fac_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat707_cpwd_fac_string_len
};

const dsat_string_item_type dsat707_cpwd_fac_valstr[][8] =
{ "\"SC\"", "\"P2\"","" };

/* Add one to string length to test overflow */
word dsat707_cpwd_passwd_string_len = CDMA_CPWD_PASSWD_STR_MAX_LEN+1;

LOCAL const mixed_def_s_type dsat707_cpwd_old_passwd_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat707_cpwd_passwd_string_len
};

LOCAL const mixed_def_s_type dsat707_cpwd_new_passwd_def_val =
{
  CONFIG | STRING | YES_QUOTE, (mixed_def_u_type *) &dsat707_cpwd_passwd_string_len
};

const mixed_def_s_type * dsat707_cpwd_mixed_dfl[3] = 
{
  &dsat707_cpwd_fac_def_val,
  &dsat707_cpwd_old_passwd_def_val,
  &dsat707_cpwd_new_passwd_def_val,
};

#endif /* FEATURE_DSAT_CDMA_PIN */


LOCAL const dflm_type dsat707_cdr_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_cds_dflm [] =
{
  { 0  , 0  , 0    },
  { 1  , 1  , 1    },
  { 2048, 512, 65535 },
  { 6  , 6  , 250  },
} ;

LOCAL const dflm_type dsat707_cfc_dflm [] =
{
  { 0  , 0  , 0    },
} ;

LOCAL const dflm_type dsat707_cqd_dflm [] =
{
  { 10 , 0  , 255  },
} ;

LOCAL const dflm_type dsat707_crc_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL dflm_type dsat707_crm_dflm [] =
{
#ifdef FEATURE_DS_NET_MODEL
  #ifdef FEATURE_WINMOBILE
  { 2  , 0  , 2    },
  #else
  { 0  , 0  , 2    },
  #endif /* FEATURE_WINMOBILE */
#else
  { 0  , 0  , 1    },
#endif /* FEATURE_DS_NET_MODEL */
} ;
LOCAL const dflm_type dsat707_cxt_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_eb_dflm [] =
{
  { 1  , 0  , 3    },
  { 0  , 0  , 1    },
  { 30 , 0  , 254  },
} ;

LOCAL const dflm_type dsat707_efcs_dflm [] =
{
  { 1  , 0  , 2    },
} ;

LOCAL const dflm_type dsat707_er_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_esr_dflm [] =
{
  { 1  , 0  , 2    },
} ;


LOCAL const dflm_type dsat707_etbm_dflm [] =
{
  { 1  , 0  , 2    },
  { 1  , 0  , 2    },
  { 20 , 0  , 30   },
} ;

LOCAL const dflm_type dsat707_ilrr_dflm [] =
{
  { 0  , 0  , 0    },
} ;

LOCAL const dflm_type dsat707_mr_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_mv18r_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat707_mv18s_dflm [] =
{
  { 0  , 0  , 7    },
  { 0  , 0  , 4    },
  { 0  , 0  , 1    },
} ;
/*=========================================================================*/
/* General IS-707 extended format AT parameter command table */
/*=========================================================================*/

const dsati_cmd_type dsat707_ext_para_table [] =
{
/*lint -save -e785 */
/*-------------------------------------------------------------------------*/
  { "+EB",
      EXTENDED | CONFIG | DOWN_LOAD | COMMON_CMD,
      SPECIAL_NONE,
      3,
      DSAT707_EXT_PARA_EB_IDX,
      &dsat707_eb_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+EFCS",
      EXTENDED | CONFIG | DOWN_LOAD | COMMON_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_EXT_PARA_EFCS_IDX,
      &dsat707_efcs_dflm[0]},
/*-------------------------------------------------------------------------*/
  { "+ER",
      EXTENDED | CONFIG | DOWN_LOAD | COMMON_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_EXT_PARA_ER_IDX,
      &dsat707_er_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+ESR",
      EXTENDED | CONFIG | DOWN_LOAD | COMMON_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_EXT_PARA_ESR_IDX,
      &dsat707_esr_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+ETBM",
      EXTENDED | CONFIG | DOWN_LOAD | COMMON_CMD,
      SPECIAL_NONE,
      3,
      DSAT707_EXT_PARA_ETBM_IDX,
      &dsat707_etbm_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+MA",
      EXTENDED | CONFIG | STRING | NO_QUOTE | DOWN_LOAD,
      SPECIAL_NONE,
      248,
      DSAT707_EXT_PARA_MA_IDX,
      NULL},
/*-------------------------------------------------------------------------*/
  { "+MR",
      EXTENDED | CONFIG | DOWN_LOAD | COMMON_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_EXT_PARA_MR_IDX,
      &dsat707_mr_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+MS",
      EXTENDED | CONFIG | STRING | NO_QUOTE | DOWN_LOAD | COMMON_CMD,
      SPECIAL_NONE,
      248,
      DSAT707_EXT_PARA_MS_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
  { "+MV18R",
      EXTENDED | CONFIG | DOWN_LOAD | COMMON_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_EXT_PARA_MV18R_IDX,
      &dsat707_mv18r_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+MV18S",
      EXTENDED | CONFIG | DOWN_LOAD | COMMON_CMD,
      SPECIAL_NONE,
      3,
      DSAT707_EXT_PARA_MV18S_IDX,
      &dsat707_mv18s_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+CXT",
      EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD ,
      SPECIAL_NONE,
      1,
      DSAT707_EXT_PARA_CXT_IDX,
      &dsat707_cxt_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+CDR",
      EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_EXT_PARA_CDR_IDX,
      &dsat707_cdr_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+CDS",
      EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
      SPECIAL_NONE,
      4,
      DSAT707_EXT_PARA_CDS_IDX,
      &dsat707_cds_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+CFC",
      EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_EXT_PARA_CFC_IDX,
      &dsat707_cfc_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+CFG",
      EXTENDED | CONFIG | STRING | COMMON_CMD,
      SPECIAL_NONE,
      248,
      DSAT707_EXT_PARA_CFG_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
  { "+CQD",
      EXTENDED | CONFIG | DOWN_LOAD | COMMON_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_EXT_PARA_CQD_IDX,
      &dsat707_cqd_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+CCRC",
      EXTENDED | CONFIG | DOWN_LOAD | COMMON_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_EXT_PARA_CRC_IDX,
      &dsat707_crc_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+CCSQ",
      EXTENDED | READ_ONLY | COMMON_CMD,
      SPECIAL_CSQ,
      0,
      DSAT707_EXT_PARA_CSQ_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707_ASYNC
  { "+CMIP",
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_CMIP,
      0,
      DSAT707_EXT_PARA_CMIP_IDX,
      NULL},
#endif /* FEATURE_DATA_IS707_ASYNC */
/*-------------------------------------------------------------------------*/
  { "+CAD",
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_CAD,
      0,
      DSAT707_EXT_PARA_CAD_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_IS707_ASYNC
  { "+CBIP",
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_CBIP,
      0,
      DSAT707_EXT_PARA_CBIP_IDX,
      NULL },
#endif /* FEATURE_DATA_IS707_ASYNC */
/*-------------------------------------------------------------------------*/

#ifdef FEATURE_DS_IS707A_CMUX
  { "+QCMUX",
      EXTENDED | CONFIG | LOCAL_TEST | LIST | COMMON_CMD,
      SPECIAL_CMUX,
      2,
      DSAT707_EXT_PARA_CMUX_IDX,
      &dsat707_cmux_list[0] },
#else
  { "+QCMUX",
      EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
      SPECIAL_CMUX,
      1,
      DSAT707_EXT_PARA_CMUX_IDX,
      &dsat707_cmux_dflm[0] },
#endif  /* FEATURE_DS_IS707A_CMUX */
/*-------------------------------------------------------------------------*/

  { "+CRM",
      EXTENDED | LOCAL_TEST | COMMON_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_EXT_PARA_CRM_IDX,
      &dsat707_crm_dflm[0] },
/*-------------------------------------------------------------------------*/
  { "+QCCSS",
      EXTENDED | READ_ONLY | COMMON_CMD,
      SPECIAL_CSS,
      0,
      DSAT707_EXT_PARA_CSS_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
  { "+CTA",
      EXTENDED | LOCAL_TEST | COMMON_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_EXT_PARA_CTA_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
  { "+GOI",
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_GOI,
      0,
      DSAT707_EXT_PARA_GOI_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
  { "+ILRR",
      EXTENDED | CONFIG | LOCAL_TEST | DO_PREF_CMD,
      SPECIAL_NONE,
      1,
      DSAT707_EXT_PARA_ILRR_IDX,
      &dsat707_ilrr_dflm[0] }
/*-------------------------------------------------------------------------*/

/*lint -restore */
};

/*=========================================================================*/
/* General IS-707 extended format AT action command table */
/*=========================================================================*/
const dsati_cmd_type dsat707_ext_action_table [] =
{
  { "+CDV",
    EXTENDED | CDMA_CMD,
    SPECIAL_NONE ,
    0,
    DSAT707_EXT_ACT_CDV_IDX,
    NULL},
  /*-------------------------------------------------------------------------*/
  { "+CHV",
      EXTENDED | CDMA_CMD,
      SPECIAL_CHV,
      0,
      DSAT707_EXT_ACT_CHV_IDX,
      NULL },
/*-------------------------------------------------------------------------*/
  { "+CHV0",
      EXTENDED | CDMA_CMD,
      SPECIAL_CHV,
      0,
      DSAT707_EXT_ACT_CHV0_IDX,
      NULL},
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_DSAT_CDMA_PIN 
  { 
     "+QCPIN",
     EXTENDED | CONFIG | MIXED_PARAM | LIST | LOCAL_TEST | RESTRICTED | DO_PREF_CMD,
     SPECIAL_NONE,
     2,
     DSAT707_EXT_ACT_QCPIN_IDX,
     &dsat707_cpin_mixed_dfl[0] },
/*-------------------------------------------------------------------------*/
  { 
     "+QCLCK",
     ABORT_CMD | EXTENDED | MIXED_PARAM | LIST | LOCAL_TEST | DO_PREF_CMD,
     SPECIAL_NONE,
     3,
     DSAT707_EXT_ACT_QCLCK_IDX,
     &dsat707_clck_mixed_dfl[0] },
/*-------------------------------------------------------------------------*/
  { 
     "+QCPWD",
     EXTENDED | LOCAL_TEST | LIST | MIXED_PARAM | DO_PREF_CMD,
     SPECIAL_NONE,
     3,
     DSAT707_EXT_ACT_QCPWD_IDX,
     &dsat707_cpwd_mixed_dfl[0]},
#endif /* FEATURE_DSAT_CDMA_PIN*/
/*-------------------------------------------------------------------------*/
  { "+PZID",
      EXTENDED | READ_ONLY | DO_PREF_CMD,
      SPECIAL_NONE,
      0,
      DSAT707_EXT_ACT_PZID_IDX,
      NULL}, 
  /*-------------------------------------------------------------------------*/
  { "+QCIMI",
     EXTENDED | READ_ONLY | DO_PREF_CMD,
     SPECIAL_NONE,
     0,
     DSAT707_EXT_ACT_CIMI_IDX,
     NULL },
/*-------------------------------------------------------------------------*/
  { "+MDN",
     EXTENDED | READ_ONLY | DO_PREF_CMD,
     SPECIAL_NONE,
     0,
     DSAT707_EXT_ACT_MDN_IDX,
     NULL }
/*-------------------------------------------------------------------------*/
/*lint -restore */
};
/* Size of command table */
const unsigned int dsat707_ext_action_table_size = ARR_SIZE( dsat707_ext_action_table );
const unsigned int dsat707_ext_para_table_size = ARR_SIZE( dsat707_ext_para_table );

/*===========================================================================
FUNCTION   

DESCRIPTION :  Original structure looks like this.
 LOCAL const dflm_type dsat707_crm_dflm [] =
{
#ifdef FEATURE_DS_NET_MODEL
  if(jcdma_mode)
  { 2  , 2  , 2    }
  else
  { 0  , 0  , 2    }
#else
  { 0  , 0  , 1    }
#endif
 } ;
DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void dsat707extctab_update_crm_dflm(void)
{
#ifdef FEATURE_DS_NET_MODEL
  dsat707_crm_dflm[0].default_v = 0;
  dsat707_crm_dflm[0].lower = 0;
  dsat707_crm_dflm[0].upper =2;

  if(ds707_roaming_is_curr_mode_jcdma() == TRUE)
  {
    dsat707_crm_dflm[0].default_v = 2;
    dsat707_crm_dflm[0].lower = 2;
    dsat707_crm_dflm[0].upper =2;
  }
#else
  dsat707_crm_dflm[0].default_v = 0;
  dsat707_crm_dflm[0].lower = 0;
  dsat707_crm_dflm[0].upper =1;

#endif
}
#endif /* FEATURE_DATA_IS707 */
