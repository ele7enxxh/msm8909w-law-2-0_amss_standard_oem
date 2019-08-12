/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               C O M M O N   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the command tables and data definitions required
  to define the AT modem commands that are common across all modes of
  operation (ETSI, CDMA2000, IS-95, etc.).

EXTERNALIZED FUNCTIONS
  dsatctab_data_init
    This function initializes the AT command defaults, limits, and lists
    that depend on a run-time determination of the hardware.

    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 2001 - 2015 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatctab_ex.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
04/07/15   tk      Fixed issue in getting QCSIAMPP value.
01/29/15   tk      Feature wrapped $QCVOIPM command with FEATURE_IMS.
11/24/14   pg      Fixed ws46 featurization issue.
08/12/14   pg      Modified DS Profile APIs to per subs.
06/27/14   sc      Initial revision (created file for Dynamic ATCoP).

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "target.h"
#include "dsati.h"
#include "msg.h"
#include "dsatparm.h"
#include "dsatact.h"
#include "dsatctab.h"
#include "dsatme.h"
#include "dsatvend.h"
#include "dstaski.h"
#include "nv.h"

#ifdef FEATURE_DSAT_ETSI_DATA
#include "dsatetsipkt.h"
#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsictab.h"
#ifdef FEATURE_DSAT_DEV_CMDS
#include "dsatetsicall.h"
#endif /* FEATURE_DSAT_DEV_CMDS */
#include "dsatme.h"
#include "dsatcmif.h"
#ifdef FEATURE_ETSI_SMS
#include "dsatetsisms.h"
#endif /* FEATURE_ETSI_SMS */
#ifdef FEATURE_DATA_ETSI_PIN
#include "dsatetsime.h"
#endif /* FEATURE_DATA_ETSI_PIN */
#endif /* FEATURE_DSAT_ETSI_MODE */
#include "ds_rmnet_defs.h"

/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  NV interface data 
---------------------------------------------------------------------------*/
nv_item_type         ds_nv_item;                  /* actual data item     */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                           Defaults and limits
---------------------------------------------------------------------------*/

dsat_qcsimapp_table_s_type dsat_qcsimapp_table[DSAT_SLOT_IDX_MAX];
dsat_qcsimapp_info_s_type  dsat_qcsimapp_info[DSAT_APPS_ID_MAX];
dsat_subs_info_s_type      dsat_subs_info[DSAT_SYS_MODEM_AS_ID_MAX];

boolean dsatme_pending_mmgsdi_command[DSAT_APPS_ID_MAX - 1] = {FALSE,FALSE,FALSE};   // -1 because the variable is not used for 1X_Primary

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
/* +CPBS Command */
/* the sequence of the storage type is important, it has to be the same as
   that of dsatetsi_pb_pbm_table 
   DEFAULT_CPB_STORAGE is SM, we use macro here to synchronize it with 
   me_cpbs_default */
#ifdef FEATURE_PBM_USE_EFS_PB
const dsat_string_item_type dsat_cpbs_storage_valstr [][8] =
{ DEFAULT_CPB_STORAGE, "DC", "FD", "LD","MC",DEFAULT_NV_CPB_STORAGE, "RC", "EN","ON",""};

LOCAL const dsat_string_item_type dsat_cpbs_storage_tststr [] =
{ "(\"SM\",\"DC\",\"FD\",\"LD\",\"MC\",\"ME\",\"RC\",\"EN\",\"ON\")" };

#else
const dsat_string_item_type dsat_cpbs_storage_valstr [][8] =
{ DEFAULT_CPB_STORAGE, "DC", "FD", "LD","MC", "RC", "EN","ON",""};

LOCAL const dsat_string_item_type dsat_cpbs_storage_tststr [] =
{ "(\"SM\",\"DC\",\"FD\",\"LD\",\"MC\",\"RC\",\"EN\",\"ON\")" };

#endif/*FEATURE_PBM_USE_EFS_PB*/

const def_list_type dsat_cpbs_list =
{ 0, dsat_cpbs_storage_valstr, dsat_cpbs_storage_tststr };

#ifdef FEATURE_TRIPLE_SIM
LOCAL const dflm_type dsat_qcpbmpref_dflm[]  =
{ 
  { 0, 0, 5 }, /* 0 -- Slot 1 Global Phone Book (default)*/
               /* 1 -- Apps 1 Local Phone Book */
               /* 2 -- Slot 2 Global Phone Book */
               /* 3 -- Apps 2 Local Phone Book */
               /* 4 -- Slot 3 Global Phone Book */
               /* 5 -- Apps 3 Local Phone Book */
};
#else
LOCAL const dflm_type dsat_qcpbmpref_dflm[]  =
{ 
  { 0, 0, 3 }, /* 0 -- Slot 1 Global Phone Book (default)*/
               /* 1 -- Apps 1 Local Phone Book */
               /* 2 -- Slot 2 Global Phone Book */
               /* 3 -- Apps 2 Local Phone Book */
};
#endif /* FEATURE_TRIPLE_SIM */
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

LOCAL const dflm_type dsat_amp_c_dflm [] =
{
  { 2  , 0  , 2    },
} ;

LOCAL const dflm_type dsat_amp_d_dflm [] =
{
  { 2  , 0  , 2    },
} ;

LOCAL const dflm_type dsat_amp_f_dflm [] =
{
  { 0  , 0  , 0    },
} ;

LOCAL const dflm_type dsat_amp_e_dflm [] =
{
  { 0 , 0 , 1 },
} ;

LOCAL const dflm_type dsat_amp_s_dflm [] =
{
  { 0 , 0 , 1 },
} ;

LOCAL const dflm_type dsat_amp_w_dflm [] =
{
  { 0 , 0 , 0 },
} ;

LOCAL const dflm_type dsat_fclass_dfl =
{ 0, 0, 2 };

LOCAL const dflm_type dsat_icf_dflm [] =
{
  { 3  , 3  , 3    },
  { 3  , 0  , 3    },
} ;
LOCAL const dflm_type dsat_ifc_dflm [] =
{
  { 2  , 0  , 3    },
  { 2  , 0  , 2    },
} ;

/*  The bit rates in the "valstr" table must match exactly the  */
/*  value and order of the "sio_baudrate_type" enum in sio.h    */
/*  Unused values must have space strings as placeholders       */
/*  The "valstr" table must be terminated with a null string    */

/*  Bit rates for MSM 2.3 and later                             */
/* Applies to ETSI & IS707 modes */  
 const dsat_string_item_type dsat_ipr_valstr [][8] =
  { " ", " ", " ", " ", " ", " ", "300", "600", "1200","2400",
    "4800", "9600", " ", "19200", "38400", "57600", " ", "115200",
#ifdef FEATURE_UART_TCXO_CLK_FREQ
    "230400", " ", "921600", "2000000", "2900000", "3000000", "3200000"
    , "3686400", "4000000",
#endif /* FEATURE_UART_TCXO_CLK_FREQ */
    "" };

LOCAL const dsat_string_item_type dsat_ipr_tststr [] =
  { "(),"
#ifdef FEATURE_UART_TCXO_CLK_FREQ
    "(300,600,1200,2400,4800,9600,19200,38400,57600,115200,230400,921600,2000000,2900000,3000000,3200000,3686400,4000000)"
#else
    "(300,600,1200,2400,4800,9600,19200,38400,57600,115200)"
#endif /* FEATURE_UART_TCXO_CLK_FREQ */
  };

/* Initialized at powerup by dsatctab_data_init().This initialization to 
MSM 2.2 is "just in case". */
LOCAL def_list_type dsat_ipr_list =
{
  17,  /*  115200 */
  dsat_ipr_valstr,  
  dsat_ipr_tststr
} ;

LOCAL const dsat_string_item_type dsat_qcdmr_valstr [][8] =
  { " ", " ", " ", " ", " ", " ", " ", " ", " ",
    " ", " ", " ", " ", " ", "38400", "57600", " ", "115200", " ", "" };

LOCAL const dsat_string_item_type dsat_qcdmr_tststr [] =
  { "(38400,57600,115200)" };

/* $QCTER baud rates should match those for +IPR */
/* Applies to ETSI & IS707 modes */
LOCAL const dsat_string_item_type dsat_qcter_valstr [][8] =
  { " ", " ", " ", " ", " ", " ", "300", "600", "1200","2400",
    "4800", "9600", " ", "19200", "38400", "57600", " ", "115200",
#ifdef FEATURE_UART_TCXO_CLK_FREQ
    "230400", " ", "921600", "2000000", "2900000", "3000000", "3200000"
    , "3686400", "4000000",
#endif /* FEATURE_UART_TCXO_CLK_FREQ */
    "" };

LOCAL const dsat_string_item_type dsat_qcter_tststr [] =
  { "(),"
#ifdef FEATURE_UART_TCXO_CLK_FREQ
    "(300,600,1200,2400,4800,9600,19200,38400,57600,115200,230400,921600,2000000,2900000,3000000,3200000,3686400,4000000)"
#else
    "(300,600,1200,2400,4800,9600,19200,38400,57600,115200)"
#endif /* FEATURE_UART_TCXO_CLK_FREQ */
  };

LOCAL const def_list_type dsat_qcdmr_list =
{
  17,  /*  115200 */ 
  dsat_qcdmr_valstr,
  dsat_qcdmr_tststr
} ;

LOCAL def_list_type dsat_qcter_list =
{
  17,  /*  115200 */
  dsat_qcter_valstr,    
  dsat_qcter_tststr
} ;

LOCAL const dflm_type dsat_e_dflm [] =
{
  { 1  , 0  , 1    },
} ;

LOCAL const dflm_type dsat_l_dflm [] =
{
  { 0  , 0  , 3    },
} ;

LOCAL const dflm_type dsat_m_dflm [] =
{
  { 0  , 0  , 2    },
} ;

LOCAL const dflm_type dsat_q_dflm [] =
{
  { 0  , 0  , 1    },
} ;

LOCAL const dflm_type dsat_s0_dflm [] =
{
  { 0  , 0  , 255  },
} ;

/* V.25 defines a range of 1 - 154            */
/* IS707 has a range of 1 - 255, IS-707 Wins! */
LOCAL const dflm_type dsat_s10_dflm [] =
{
  { 14 , 1  , 255  },
} ;

LOCAL const dflm_type dsat_s11_dflm [] =
{
  { 95 , 50 , 255  },
} ;

LOCAL const dflm_type dsat_s2_dflm [] =
{
  { 43 , 0 , 127 },
} ;

LOCAL const dflm_type dsat_o_dflm[]=
{
  { 0, 0, 2 },
};

LOCAL const dflm_type dsat_s3_dflm [] =
{
  { 13 , 0  , 127  },
} ;
LOCAL const dflm_type dsat_s4_dflm [] =
{
  { 10 , 0  , 127  },
} ;

LOCAL const dflm_type dsat_s5_dflm [] =
{
  { 8  , 0  , 127  },
} ;

LOCAL const dflm_type dsat_x_dflm [] =
{
  { 1  , 0  , 4    },
} ;

LOCAL const dflm_type dsat_s6_dflm [] =
{
  { 2  , 2  , 10   },
} ;

LOCAL const dflm_type dsat_s7_dflm [] =
{
  { 0 , 0 , 255  },/* 0 - Default - Will not Start the timer  */
} ;

LOCAL const dflm_type dsat_s8_dflm [] =
{
  { 2  , 0  , 255  },
} ;

LOCAL const dflm_type dsat_s9_dflm [] =
{
  { 6  , 0  , 255  },
} ;

LOCAL const dflm_type dsat_s30_dflm [] =
{
  { 0 , 0 , 255 },
} ;

LOCAL const dflm_type dsat_s103_dflm [] =
{
  { 1 , 0 , 2 },
} ;

LOCAL const dflm_type dsat_s104_dflm [] =
{
  { 1 , 0 , 2 },
} ;

LOCAL const dflm_type dsat_slash_s_dflm [] =
{
  { 0 , 0 , 0 },
} ;

LOCAL const dflm_type dsat_slash_v_dflm [] =
{
  { 0 , 0 , 1 },
} ;

LOCAL const dflm_type dsat_v_dflm [] =
{
  { 1  , 0  , 1    },
} ;

LOCAL const dflm_type dsat_z_dflm [] =
{
  { 0  , 0  , 0    },
} ;

/* AT\Q command */
LOCAL const dflm_type dsat_slash_q_dflm [] =
{
  { 3, 0, 1 },                 
  { 3, 3, 3 },
  { MAX_BROKEN_RANGE, 0, 0 },
};

/* +CMEE Command */
LOCAL const dsat_string_item_type dsat_cmee_valstr [][8] =
{ "0", "1", "2", ""};

LOCAL const dsat_string_item_type dsat_cmee_tststr [] =
{ "(0,1,2)" };

const def_list_type dsat_cmee_list =
{ 2, dsat_cmee_valstr, dsat_cmee_tststr };

#ifdef FEATURE_DSAT_DEV_CMDS
#ifdef FEATURE_DSAT_ETSI_MODE
/* $CREG Command */
LOCAL const dflm_type dsat_qccreg_dfl[] =
{ 
#ifdef FEATURE_GSM_RR_CELL_CALLBACK
  { 0, 0, 2 },    /* 0 - disable, 1 - enable, 
     2 - enable with location, cell id and PSC if present */
#else
  { 0, 0, 1 },     /* 0 - disable, 1 - enable */
#endif /* FEATURE_GSM_RR_CELL_CALLBACK */
};
#endif /* ( FEATURE_DSAT_ETSI_MODE) */
#endif /* FEATURE_DSAT_DEV_CMDS */


/* +DR Command */
LOCAL const dflm_type dsat_dr_dflm [] =
{
  { 0  ,  0  , 1    }
};
#ifdef FEATURE_TTY
LOCAL const dflm_type dsat_qcctm_dflm [] =
{
  { 0  ,  0  , 1    }
};
#endif/* FEATURE_TTY */
/* +DS Command */
const dflm_type dsat_ds_dflm [] =
{
  { 0  ,  0  , 3    },  /* Direction       */
  { 0  ,  0  , 0    },  /* Negotiation     */
  { 2048, 512, 2048 },  /* Max Dictionary  */
  { 6  ,  6  , 6   }    /* Max String      */
} ;

#ifdef FEATURE_DSAT_ETSI_MODE
/* +WS46 Command */
#ifdef FEATURE_DSAT_LTE
/* G+L or W+L combination is not supported by lower layers */
LOCAL const dsat_string_item_type dsat_ws46_valstr [][8] =
  { "12", "22", "25", "28", "29", "" };
LOCAL const dsat_string_item_type dsat_ws46_tststr [] =
  { "(12,22,25,28,29)" };
#elif defined( FEATURE_WCDMA )
LOCAL const dsat_string_item_type dsat_ws46_valstr [][8] =
  { "12", "22", "25", "" };
LOCAL const dsat_string_item_type dsat_ws46_tststr [] =
  { "(12,22,25)" };
#else
LOCAL const dsat_string_item_type dsat_ws46_valstr [][8] =
  { "12", "" };
LOCAL const dsat_string_item_type dsat_ws46_tststr [] =
  { "(12)" };
#endif /* defined ( FEATURE_DSAT_LTE ) || defined( FEATURE_WCDMA ) */
const def_list_type dsat_ws46_list[] =
{
  { 0, dsat_ws46_valstr, dsat_ws46_tststr },
};
#endif /* FEATURE_DSAT_ETSI_MODE */
/* $QCSIMAPP */
LOCAL const dflm_type dsat_qcsimapp_dflm [] =
{
  { 0, 0, 2 }
} ;
#ifdef FEATURE_DSAT_ETSI_MODE
/* $QCSIMISTAT Command */
LOCAL const dflm_type dsat_qcsimstat_dflm [] =
{
  { 0, 0, 2 }
};
#endif /* FEATURE_DSAT_ETSI_MODE*/

#ifdef FEATURE_DSAT_ETSI_DATA
/* $QCPDPP Command */
LOCAL const dflm_type dsat_qcpdpp_cid_dflm [] =
{
  { DSAT_LEGACY_PROFILE_NUM_MIN, DSAT_LEGACY_PROFILE_NUM_MIN, DSAT_LEGACY_PROFILE_NUM_MAX },
  { DSAT_LEGACY_PROFILE_NUM_MIN, DSAT_EXT_PROFILE_NUM_MIN, DSAT_EXT_PROFILE_NUM_MAX},
  { MAX_BROKEN_RANGE, 0, 0}
} ;

LOCAL const dflm_type dsat_qcpdpp_auth_dflm [] =
{
  { 0  , 0  , 3  },
} ;

/*  Use over-long strings to validate max permitted length */
/* Accomodate quotes in addition to DS_MAX_QCPDP_STRING_LEN characters */
LOCAL const mixed_def_u_type dsat_qcpdpp_uname_string_len = 
                                            {{DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+5}};
LOCAL const mixed_def_u_type dsat_qcpdpp_passw_string_len = 
                                            {{DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+5}};



LOCAL const mixed_def_s_type dsat_qcpdpp_cid_def_val =
{
  CONFIG | LOCAL_TEST | BROKEN_RANGE, (mixed_def_u_type *) dsat_qcpdpp_cid_dflm
};

LOCAL const mixed_def_s_type dsat_qcpdpp_auth_def_val =
{
  CONFIG | LOCAL_TEST, (mixed_def_u_type *) dsat_qcpdpp_auth_dflm
};

LOCAL const mixed_def_s_type dsat_qcpdpp_passw_def_val =
{
  CONFIG | STRING | NO_DISPLAY | NO_QUOTE,
  (mixed_def_u_type *) &(dsat_qcpdpp_passw_string_len)
};

LOCAL const mixed_def_s_type dsat_qcpdpp_uname_def_val =
{
  CONFIG | STRING | YES_QUOTE,
  (mixed_def_u_type *) &(dsat_qcpdpp_uname_string_len)
};

/*lint -save -e708 */
LOCAL const mixed_def_s_type * dsat_qcpdpp_mixed_dfl[4] = 
{
  &dsat_qcpdpp_cid_def_val,
  &dsat_qcpdpp_auth_def_val,
  &dsat_qcpdpp_passw_def_val,
  &dsat_qcpdpp_uname_def_val
};
/*lint -restore */
/*lint -restore */


LOCAL const dflm_type dsat_qcpdplt_dflm [] =
{
  { QCPDPLT_DEF_VAL, 0, 1 },
} ;

#endif /* FEATURE_DSAT_ETSI_DATA */


#ifdef FEATURE_DATA_UCSD_SCUDIF_TEST
/* SCUDIF Test Modes */
LOCAL const dsat_string_item_type dsat_qcscftest_dir_valstr [][8] =
  { "MO", "MT", "" };

LOCAL const dsat_string_item_type dsat_qcscftest_dir_tststr [] =
  { "(MO,MT)" };

/* List of test configurations defined in CSData code. */
const dsat_string_item_type dsat_qcscftest_mode_valstr [][8] =
{ "0", "1", "2", "3", "4", "5", "6", "7", "8", "" };

LOCAL const dsat_string_item_type dsat_qcscftest_mode_tststr [] =
{ "(0,1,2,3,4,5,6,7,8)" };

const def_list_type dsat_qcscftest_list[] =
{
  { 0, dsat_qcscftest_dir_valstr, dsat_qcscftest_dir_tststr },
  { 0, dsat_qcscftest_mode_valstr, dsat_qcscftest_mode_tststr }
};


uint8 dsat_qcscftest_exp_val[2] = {0};
#endif /* FEATURE_DATA_UCSD_SCUDIF_TEST */
#ifdef FEATURE_ECALL_APP
/*
dsat_ecall_val[0]  -> Start/Stop the eCall 0 - Stop ; 1 - Start
dsat_ecall_val[1]  -> Type of the eCall    0 - Test ; 1 - Emergency
dsat_ecall_val[2]  -> Activation type      0 - Manual; 1- Automatic
*/
LOCAL const dsat_string_item_type dsat_ecall_session_valstr [][8] =
{
  "0", "1", ""
};

LOCAL const dsat_string_item_type dsat_ecall_session_tststr [] =
{ 
  "(0,1)" 
};

LOCAL const dsat_string_item_type dsat_ecall_act_type_valstr [][8] =
{
  "0", "1", ""
};

LOCAL const dsat_string_item_type dsat_ecall_act_type_tststr [] =
{ 
  "(0,1)" 
};

LOCAL const dsat_string_item_type dsat_ecall_type_of_call_valstr [][8] =
{
  "0", "1", "2",""
};

LOCAL const dsat_string_item_type dsat_ecall_type_of_call_tststr [] =
{ 
  "(0,2)" 
};

const def_list_type dsat_ecall_list[] =
{
  { 0, dsat_ecall_session_valstr, dsat_ecall_session_tststr },
  { 0, dsat_ecall_type_of_call_valstr, dsat_ecall_type_of_call_tststr },
  { 0, dsat_ecall_act_type_valstr, dsat_ecall_act_type_tststr }

} ;
#endif /* FEATURE_ECALL_APP */

#ifdef FEATURE_DSAT_TEST_32ARG 

/*---------------------------------------------------------------------------
  The following def and val's are for test purpose only.
  AT+TST32, AT+TTT32, and AT$TVT32 are used to test 32 bit argument support
  Since we don't currently have a AT command with 32 bit argument,
  we have to make up commands to test the functionality.
---------------------------------------------------------------------------*/


LOCAL const dflm_type dsat_tst32_dflm[] = 
{
  { 0x075BCD15,  /* 123456789 default*/
    0x00000fff,  /* 4095  */
    ~0         },/* 0xffffffff, 4294967295 */

  { 0x075BCD15,  /* 123456789 */
    0x0000ffff,  /* 65535  */
    ~0         } /* 4294967295 */
};

#define DSAT_TUT32_FIRST_STR_LEN 21

LOCAL dsat_string_item_type 
dsat_tut32_first_str_val[DSAT_TUT32_FIRST_STR_LEN + 1];

LOCAL dsat_mixed_param_val_type dsat_tut32_val[2] = {
  dsat_tut32_first_str_val, 0
};

LOCAL uint16 dsat_tut32_first_str_len = DSAT_TUT32_FIRST_STR_LEN;

LOCAL mixed_def_s_type dsat_tut32_first_def =
{ CONFIG | STRING, (mixed_def_u_type *) & dsat_tut32_first_str_len};

LOCAL dflm_type dsat_tut32_second_dflm =
{ 0x075BCD15,   /* 123456789  */
  0x0000ffff,   /* 65535      */
  ~0         }; /* 4294967295 */

LOCAL mixed_def_s_type dsat_tut32_second_def =
{ CONFIG | LOCAL_TEST, (mixed_def_u_type *) &dsat_tut32_second_dflm };

LOCAL mixed_def_s_type * dsat_tut32_mixed_dfl[] =
{
  &dsat_tut32_first_def, &dsat_tut32_second_def
};

#endif /* FEATURE_DSAT_TEST_32ARG */

#ifdef FEATURE_DATA_TE_MT_PDP
/* $QCGANSM command */
const dsat_string_item_type dsat_qcgansm_state_valstr [][8] =
{ "0", "1", "2","" } ;

LOCAL const dsat_string_item_type dsat_qcgansm_state_tststr [] =
{ "(0-2)" };

LOCAL def_list_type dsat_qcgansm_list =
{
  0,
  dsat_qcgansm_state_valstr,
  dsat_qcgansm_state_tststr
};


#endif /* FEATURE_DATA_TE_MT_PDP */

#ifdef FEATURE_DSAT_DEV_CMDS
#ifdef FEATURE_DSAT_ETSI_MODE
/* *CNTI command*/
LOCAL const dflm_type dsat_qccnti_dflm [] =
{
  { 0 , 0 , 2 } /* 0 -- default indicates Current Technology */
};
dsati_cnti_tech_enum_type dsat_qccnti_tech;
#endif /* FEATURE_DSAT_ETSI_MODE*/
#endif /* FEATURE_DSAT_DEV_CMDS */
/* ^PREFMODE variables  */
LOCAL const dflm_type dsat_prefmode_dfl[] =
{
  {0,0,8}  /* Defaulting ^PREFMODE to Automatic 
              But valid values are 0 - Automatic, 2-CDMA, 4-HDR, 8-HYBRID */
};

LOCAL const dflm_type dsat_dsci_dflm [] =
{
  { 0  , 0  , 1  },
} ;

/* +CFUN Command */
/* Functionality */
LOCAL const dflm_type dsat_cfun_fun_dfl[] =
{ 
  { 1, 0, 1 },
#if defined(FEATURE_DSAT_EXTENDED_CMD)
  { 1, 4, 6 },
#else
  { 1, 4, 7 },
#endif /* defined(FEATURE_DSAT_EXTENDED_CMD) */
  { MAX_BROKEN_RANGE, 0, 0 }
};

LOCAL const mixed_def_s_type dsat_cfun_fun_def_val =
{
  CONFIG | NO_RESET | NO_DISPLAY | LOCAL_TEST | BROKEN_RANGE,
  (mixed_def_u_type *) dsat_cfun_fun_dfl
};

/* Note: reset parameter value of 1 is only supported for full
   functionality */
LOCAL const dflm_type dsat_cfun_rst_dfl[] =
{
  { 0, 0, 1 }
};

LOCAL const mixed_def_s_type dsat_cfun_rst_def_val =
{
  CONFIG | NO_RESET | NO_DISPLAY | LOCAL_TEST,
  (mixed_def_u_type *) dsat_cfun_rst_dfl
};

LOCAL const mixed_def_s_type * dsat_cfun_mixed_dfl[2] = 
{
  &dsat_cfun_fun_def_val,
  &dsat_cfun_rst_def_val
};


/* $QCDEFPROF Command */
LOCAL const dflm_type dsat_qcdefprof_dfl[] =
{ 
  { 0, 0, 1 },    /* family */
#ifdef FEATURE_DUAL_SIM
  { 1, 1, DS_SUBSCRIPTION_MAX },    /* subscription id */
#else  /* FEATURE_DUAL_SIM */
  { 1, 1, 1 },    /* subscription id */
#endif /* FEATURE_DUAL_SIM */
  { 1, 1, 16 },   /* profile num */
};

#ifdef FEATURE_DSAT_LTE
LOCAL const dflm_type dsat_qcdrx_dflm[] =
{
  { 6, 0, 0 },
  { 6, 6, 9 },
  { MAX_BROKEN_RANGE, 0, 0 },
};
#endif /* FEATURE_DSAT_LTE*/
/*^MODE Command*/
LOCAL const dflm_type dsat_mode_dflm[] =
{
  { 0, 0, 1},
};
/*$QCRCIND Command*/
LOCAL const dflm_type dsat_qcrcind_dflm[] =
{
  { 0, 0, 1},
};

/* $QCCOPS Command*/
extern const mixed_def_s_type * dsat_cops_mixed_dfl[];

/*--------------------------------------------------------------------------
                          Common Command Tables 
--------------------------------------------------------------------------*/

/* Basic common AT command table. */
const dsati_cmd_type dsat_basic_table [] =
{
  { "&C",       CONFIG | SIMPLE_DEC | COMMON_CMD,
    SPECIAL_NONE, 1,DSAT_BASIC_N_C_IDX,   &dsat_amp_c_dflm[0]},

  { "&D",       CONFIG | SIMPLE_DEC | COMMON_CMD,
    SPECIAL_NONE,1,DSAT_BASIC_N_D_IDX,    &dsat_amp_d_dflm[0]},

  { "&E",       CONFIG | SIMPLE_DEC | COMMON_CMD,
    SPECIAL_NONE,  1,DSAT_BASIC_N_E_IDX,   &dsat_amp_e_dflm[0]},

  { "&F",       CONFIG | SIMPLE_DEC | RESTRICTED | COMMON_CMD,
    SPECIAL_AMP_F, 1,DSAT_BASIC_N_F_IDX,   &dsat_amp_f_dflm[0]},

  { "&S",       CONFIG | COMMON_CMD,
    SPECIAL_NONE, 1,DSAT_BASIC_N_S_IDX,   &dsat_amp_s_dflm[0]},

  { "&V",       ATTRIB_NONE | COMMON_CMD,
    SPECIAL_AMP_V, 0 ,DSAT_BASIC_N_V_IDX,   NULL},

  { "&W",       CONFIG | SIMPLE_DEC | COMMON_CMD,
    SPECIAL_NONE,  1,DSAT_BASIC_N_W_IDX, &dsat_amp_w_dflm[0]},

#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
  { "E",        CONFIG | SIMPLE_DEC | DOWN_LOAD | RESTRICTED|NOT_PROVISIONED | COMMON_CMD,
    SPECIAL_NONE,1,DSAT_BASIC_E_IDX,&dsat_e_dflm[0]},
#else
  { "E",        CONFIG | SIMPLE_DEC | DOWN_LOAD | RESTRICTED|NOT_PROVISIONED | COMMON_CMD,
    SPECIAL_NONE,  1 , DSAT_BASIC_E_IDX ,   &dsat_e_dflm[0]},
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/

  { "I",        READ_ONLY | RESTRICTED|NOT_PROVISIONED | COMMON_CMD | MULTI_SLOT,
    SPECIAL_NONE,    0,   DSAT_BASIC_I_IDX,  NULL},

{ "L",        CONFIG | SIMPLE_DEC | RESTRICTED|NOT_PROVISIONED | COMMON_CMD,
  SPECIAL_NONE,    1,DSAT_BASIC_L_IDX,    &dsat_l_dflm[0]},

{ "M",        CONFIG | SIMPLE_DEC | RESTRICTED|NOT_PROVISIONED | COMMON_CMD,
  SPECIAL_NONE,    1,DSAT_BASIC_M_IDX,    &dsat_m_dflm[0]},

  { "Q",        CONFIG | SIMPLE_DEC | DOWN_LOAD | RESTRICTED | COMMON_CMD|NOT_PROVISIONED,
    SPECIAL_NONE,   1, DSAT_BASIC_Q_IDX ,       &dsat_q_dflm[0]},

  { "V",        CONFIG | SIMPLE_DEC | DOWN_LOAD | RESTRICTED|NOT_PROVISIONED | COMMON_CMD,
    SPECIAL_NONE, 1, DSAT_BASIC_V_IDX,       &dsat_v_dflm[0]},

  { "X",        CONFIG | SIMPLE_DEC | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE, 1, DSAT_BASIC_X_IDX,     &dsat_x_dflm[0]},

  { "Z",        CONFIG | SIMPLE_DEC | RESTRICTED | COMMON_CMD,
    SPECIAL_Z,   1, DSAT_BASIC_Z_IDX,       &dsat_z_dflm[0]},

  { "T",       ATTRIB_NONE | COMMON_CMD,
    SPECIAL_NONE,   0, DSAT_BASIC_T_IDX, NULL},

  { "P",       ATTRIB_NONE | COMMON_CMD,
    SPECIAL_NONE,   0, DSAT_BASIC_P_IDX,NULL},

  { "\\Q", SIMPLE_DEC | CONFIG | BROKEN_RANGE,
    SPECIAL_NONE,1,DSAT_BASIC_DS_Q_IDX, &dsat_slash_q_dflm[0]},

  { "\\S",    CONFIG | SIMPLE_DEC | COMMON_CMD,
    SPECIAL_NONE, 1,DSAT_BASIC_DS_S_IDX,&dsat_slash_s_dflm[0]},

  { "\\V",    CONFIG | SIMPLE_DEC | COMMON_CMD,
    SPECIAL_NONE, 1,DSAT_BASIC_DS_V_IDX,&dsat_slash_v_dflm[0]},

  { "%V",     READ_ONLY | COMMON_CMD, /* output similar to +GMR */
    SPECIAL_NONE, 0, DSAT_BASIC_P_V_IDX,NULL},
} ;

/* Basic common action command table. */
const dsati_cmd_type dsat_basic_action_table [] =
{
  { "D",       ABORT_CMD | RESTRICTED | MULTI_SUBS | NOT_PROVISIONED | COMMON_CMD,
    SPECIAL_ATD,    0,DSAT_BASIC_ACT_D_IDX,  NULL },

  { "A",       ABORT_CMD | ATTRIB_NONE | COMMON_CMD | MULTI_SUBS,
    SPECIAL_NONE,    0,DSAT_BASIC_ACT_A_IDX,NULL },

  { "H",       ATTRIB_NONE | COMMON_CMD,
    SPECIAL_NONE,    0,DSAT_BASIC_ACT_H_IDX,NULL },

  { "O",       CONFIG | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_BASIC_ACT_O_IDX,   &dsat_o_dflm[0] }
};

/* Common S-register command table. */
const dsati_cmd_type dsat_sreg_table [] =
{

  { "S0",       CONFIG | COMMON_CMD | RESTRICTED|NOT_PROVISIONED,
  SPECIAL_NONE,    1,DSAT_SREG_S0_IDX,  &dsat_s0_dflm[0] },


  { "S2",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S2_IDX, &dsat_s2_dflm[0] },

  { "S3",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S3_IDX,  &dsat_s3_dflm[0] },

  { "S4",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S4_IDX,  &dsat_s4_dflm[0] },

  { "S5",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S5_IDX,  &dsat_s5_dflm[0] },

  { "S6",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S6_IDX, &dsat_s6_dflm[0] },

  { "S7",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S7_IDX,  &dsat_s7_dflm[0] },

  { "S8",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S8_IDX, &dsat_s8_dflm[0] },

  { "S9",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S9_IDX, &dsat_s9_dflm[0] },

  { "S10",      CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S10_IDX, &dsat_s10_dflm[0] },

  { "S11",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S11_IDX,  &dsat_s11_dflm[0] },

  { "S30",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S30_IDX, &dsat_s30_dflm[0] },

  { "S103",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S103_IDX, &dsat_s103_dflm[0] },

  { "S104",       CONFIG | DOWN_LOAD | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_SREG_S104_IDX,&dsat_s104_dflm[0] }
} ;

/* Extended common AT command table */
const dsati_cmd_type dsat_ext_table [] =
{
  { "+FCLASS",  EXTENDED | CONFIG | DOWN_LOAD | RESTRICTED|NOT_PROVISIONED | COMMON_CMD,
    SPECIAL_FCLASS,  1,DSAT_EXT_FCLASS_IDX,&dsat_fclass_dfl },

  { "+ICF",     EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE, 2,DSAT_EXT_ICF_IDX,  &dsat_icf_dflm[0] },

  { "+IFC",     EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE, 2,DSAT_EXT_IFC_IDX,   &dsat_ifc_dflm[0] },

  { "+IPR",     EXTENDED | CONFIG | LIST | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE, 1, DSAT_EXT_IPR_IDX,      &dsat_ipr_list },

  { "+GMI",     READ_ONLY | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE, 0,DSAT_EXT_GMI_IDX,   NULL },

  { "+GMM",     READ_ONLY | RESTRICTED|NOT_PROVISIONED | COMMON_CMD,
    SPECIAL_NONE,   0, DSAT_EXT_GMM_IDX,   NULL },

  { "+GMR",     READ_ONLY | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   0, DSAT_EXT_GMR_IDX,   NULL },

  { "+GCAP",    READ_ONLY | COMMON_CMD,
    SPECIAL_NONE,   0, DSAT_EXT_GCAP_IDX,    NULL },

  { "+GSN",     READ_ONLY | RESTRICTED | MULTI_SLOT | COMMON_CMD,
    SPECIAL_NONE,   0,  DSAT_EXT_GSN_IDX,    NULL },

  { "+DR",      EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,   1,  DSAT_EXT_DR_IDX,    &dsat_dr_dflm[0] },

  { "+DS",      EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,   4,DSAT_EXT_DS_IDX,   &dsat_ds_dflm[0] },

  { "+CMEE",     EXTENDED | CONFIG | LIST | LOCAL_TEST | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   1,DSAT_EXT_CMEE_IDX,     &dsat_cmee_list },

#if defined(FEATURE_DSAT_ETSI_MODE)
/* Exclude CDMA (for now, may be required later) */
  { "+WS46",      EXTENDED | CONFIG |  LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   1,DSAT_EXT_WS46_IDX,      &dsat_ws46_list[0] },
#endif /* FEATURE_DSAT_ETSI_MODE */

#if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM))
  { "+PACSP",       READ_ONLY | UMTS_CMD,
    SPECIAL_NONE,    0,DSAT_EXT_PACSP_IDX,   NULL },
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM)*/

#ifdef FEATURE_DSAT_TEST_32ARG 
  { "+TST32",   EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,   2,DSAT_EXT_TST32_IDX,   &dsat_tst32_dflm[0] },
#endif /* FEATURE_DSAT_TEST_32ARG */

  { "+CFUN",    EXTENDED | CONFIG | LOCAL_TEST | LIST | MIXED_PARAM | 
                NO_RESET | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE, 2,DSAT_EXT_CFUN_IDX,   dsat_cfun_mixed_dfl },

#ifndef FEATURE_DSAT_CUST
  { "+CLCC",   ABORT_CMD | COMMON_CMD ,
    SPECIAL_NONE,    0,DSAT_EXT_CLCC_IDX,  NULL },
#endif /* FEATURE_DSAT_CUST  */

} ;

/* Vendor specific common AT command table. */
const dsati_cmd_type dsat_vendor_table [] =
{
#ifdef FEATURE_DSAT_ETSI_MODE
  { "$QCSIMSTAT",    EXTENDED | CONFIG | RESTRICTED | MULTI_SLOT | UMTS_CMD,
    SPECIAL_NONE,1,DSAT_VENDOR_QCSIMSTAT_IDX, &dsat_qcsimstat_dflm[0] },

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
  { "$QCPBMPREF",    EXTENDED | CONFIG | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE, 1,DSAT_VENDOR_QCPBMPREF_IDX, &dsat_qcpbmpref_dflm[0] },
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

#ifdef FEATURE_DSAT_DEV_CMDS
  { "$CREG",    EXTENDED | CONFIG | LOCAL_TEST| MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE, 1,DSAT_VENDOR_CREG_IDX,dsat_qccreg_dfl },

  { "$CCLK",    EXTENDED | COMMON_CMD,
    SPECIAL_NONE, 0, DSAT_VENDOR_CCLK_IDX,NULL},
#endif /* FEATURE_DSAT_DEV_CMDS */

#ifdef FEATURE_ETSI_SMS
  { "$QCCNMI",    EXTENDED | CONFIG | LIST | LOCAL_TEST| MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,    5, DSAT_VENDOR_QCCNMI_IDX,&dsat_cnmi_list[0] },
#endif /* FEATURE_ETSI_SMS */
  
#ifdef FEATURE_DSAT_DEV_CMDS
  { "*CNTI",   EXTENDED | CONFIG  | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   1, DSAT_VENDOR_CNTI_IDX,&dsat_qccnti_dflm[0] },
#endif /* FEATURE_DSAT_DEV_CMDS */
#endif /* FEATURE_DSAT_ETSI_MODE*/

  { "$QCCLR",   EXTENDED | READ_ONLY | COMMON_CMD,
    SPECIAL_NONE,   0,DSAT_VENDOR_QCCLR_IDX,   NULL },

  { "$QCDMR",   EXTENDED | CONFIG | LIST | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,   1, DSAT_VENDOR_QCDMR_IDX, &dsat_qcdmr_list },


  { "$QCDNSP",  EXTENDED | CONFIG | STRING | NO_QUOTE | COMMON_CMD,
    SPECIAL_NONE,  MAX_DNS_SIZE,DSAT_VENDOR_QCDNSP_IDX,NULL },

  { "$QCDNSS",  EXTENDED | CONFIG | STRING | NO_QUOTE | COMMON_CMD,
    SPECIAL_NONE,  MAX_DNS_SIZE, DSAT_VENDOR_QCDNSS_IDX,NULL },

  { "$QCTER",  EXTENDED | CONFIG | LIST | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_VENDOR_QCTER_IDX, &dsat_qcter_list },
    
  { "$QCSIMAPP",  EXTENDED | CONFIG | LOCAL_TEST | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,1, DSAT_VENDOR_QCSIMAPP_IDX,  &dsat_qcsimapp_dflm[0] },
    
#ifdef FEATURE_DATA_PS_SLIP
  { "$QCSLIP",   ATTRIB_NONE | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE, 0, DSAT_VENDOR_QCSLIP_IDX,    NULL },
#endif /* FEATURE_DATA_PS_SLIP */

#ifdef FEATURE_DATA_ETSI_PIN
  { "$QCPINSTAT",  READ_ONLY | RESTRICTED | MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,    0, DSAT_VENDOR_QCPINSTAT_IDX,   NULL },
#endif /* FEATURE_DATA_ETSI_PIN */
  
#ifdef FEATURE_DSAT_ETSI_DATA
  { "$QCPDPP",  EXTENDED | CONFIG | MIXED_PARAM | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,   4, DSAT_VENDOR_QCPDPP_IDX,   &dsat_qcpdpp_mixed_dfl[0] },

  { "$QCPDPLT",   EXTENDED | CONFIG | NO_RESET | UMTS_CMD,
    SPECIAL_NONE,   1, DSAT_VENDOR_QCPDPLT_IDX,    &dsat_qcpdplt_dflm[0] },


#endif /* FEATURE_DSAT_ETSI_DATA */

#ifdef FEATURE_DATA_TE_MT_PDP
  { "$QCGANSM",    EXTENDED | CONFIG | LIST | LOCAL_TEST | NO_RESET | COMMON_CMD,
    SPECIAL_NONE,   1, DSAT_VENDOR_QCGANSM_IDX,   &dsat_qcgansm_list },

  { "$QCGARL",    EXTENDED | UMTS_CMD,
    SPECIAL_NONE,   2,DSAT_VENDOR_QCGARL_IDX,   NULL },
    
  { "$QCGAPL",    EXTENDED | UMTS_CMD,
    SPECIAL_NONE,   2,DSAT_VENDOR_QCGAPL_IDX,   NULL },
#endif /* FEATURE_DATA_TE_MT_PDP */

  { "$QCPWRDN",  EXTENDED | COMMON_CMD| RESTRICTED,
    SPECIAL_NONE,    0, DSAT_VENDOR_QCPWRDN_IDX,   NULL },

#ifdef FEATURE_DSAT_ETSI_DATA
  { "$QCDGEN",  EXTENDED | UMTS_CMD,
    SPECIAL_NONE,    2,DSAT_VENDOR_QCDGEN_IDX,   NULL },  

  { "$QCPDPCFGE",  EXTENDED | COMMON_CMD,
     SPECIAL_NONE,   12,DSAT_VENDOR_QCPDPCFGE_IDX,   NULL },
#endif /* FEATURE_DSAT_ETSI_DATA */
  
#ifdef FEATURE_DSAT_BREW_SUPPORT
  { "$BREW",   EXTENDED | COMMON_CMD,
    SPECIAL_NONE,   0, DSAT_VENDOR_BREW_IDX,     NULL },
#endif /* FEATURE_DSAT_BREW_SUPPORT */

#ifdef FEATURE_DATA_UCSD_SCUDIF_TEST
  { "$QCSCFTEST",   EXTENDED | CONFIG | LIST | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE, 2,DSAT_VENDOR_QCSCFTEST_IDX,  &dsat_qcscftest_list[0] },
#endif /*  FEATURE_DATA_UCSD_SCUDIF_TEST */

#ifdef FEATURE_DSAT_GOBI_MAINLINE
  { "$QCGSN", READ_ONLY | RESTRICTED | COMMON_CMD,
	SPECIAL_NONE,   0,DSAT_VENDOR_QCGSN_IDX,     NULL },

  { "$QCSKU",   READ_ONLY | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   0,DSAT_VENDOR_QCSKU_IDX,  NULL},
#endif /* FEATURE_DSAT_GOBI_MAINLINE */

  { "$QCANTE",   READ_ONLY | UMTS_CMD,
    SPECIAL_NONE,   0,DSAT_VENDOR_QCANTE_IDX,  NULL },
    
  { "$QCRPW",  READ_ONLY | UMTS_CMD,
    SPECIAL_NONE,    0,DSAT_VENDOR_QCRPW_IDX,   NULL },

  { "$QCSQ",  READ_ONLY | UMTS_CMD,
    SPECIAL_NONE,    0,DSAT_VENDOR_QCSQ_IDX,   NULL },
    
#ifdef FEATURE_DSAT_DEV_CMDS
  { "$CSQ",  READ_ONLY | UMTS_CMD,
    SPECIAL_NONE,    0,DSAT_VENDOR_CSQ_IDX,   NULL },
#endif /* FEATURE_DSAT_DEV_CMDS */

  { "$QCSYSMODE",    READ_ONLY | COMMON_CMD | MULTI_SUBS,
    SPECIAL_NONE,   0,DSAT_VENDOR_QCSYSMODE_IDX,  NULL }
#ifdef FEATURE_TTY
  ,{ "$QCCTM",    EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,   1,DSAT_VENDOR_QCCTM_IDX, &dsat_qcctm_dflm[0] }
#endif/* FEATURE_TTY */
 ,{ "$QCBANDPREF",    EXTENDED | LOCAL_TEST | COMMON_CMD,
      SPECIAL_NONE,    1,DSAT_VENDOR_QCBANDPREF_IDX, NULL }
#ifdef FEATURE_HDR
  ,{ "^PREFMODE",    EXTENDED | CONFIG | LOCAL_TEST | COMMON_CMD,
    SPECIAL_NONE,    1,DSAT_VENDOR_PREFMODE_IDX,  dsat_prefmode_dfl }
#endif /* FEATURE_HDR */

 ,{ "^SYSINFO",    READ_ONLY | DO_PREF_CMD | RESTRICTED | MULTI_SUBS,
    SPECIAL_NONE ,     0, DSAT_VENDOR_SYSINFO_IDX,      NULL }
 
 ,{ "^SYSCONFIG",    READ_ONLY | UMTS_CMD,
    SPECIAL_NONE ,     0, DSAT_VENDOR_SYSCONFIG_IDX,     NULL }

 ,{ "^CARDMODE",    READ_ONLY | COMMON_CMD | RESTRICTED | MULTI_SUBS,
    SPECIAL_NONE ,     0, DSAT_VENDOR_CARDMODE_IDX,     NULL }
#ifdef FEATURE_DSAT_ETSI_MODE 
  ,{ "^DSCI",    EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    1, DSAT_VENDOR_DSCI_IDX,   &dsat_dsci_dflm[0] }
#endif /* FEATURE_DSAT_ETSI_MODE */
  ,{ "$QCVOLT", READ_ONLY | RESTRICTED | COMMON_CMD,
  SPECIAL_NONE,   0,DSAT_VENDOR_QCVOLT_IDX,  NULL }

  ,{ "$QCHWREV", READ_ONLY | RESTRICTED | COMMON_CMD,
  SPECIAL_NONE,   0,DSAT_VENDOR_QCHWREV_IDX,   NULL }

  ,{ "$QCBOOTVER",   READ_ONLY | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   0, DSAT_VENDOR_QCBOOTVER_IDX,  NULL }

#ifdef FEATURE_DSAT_GOBI_MAINLINE 
  ,{ "$QCTEMP", READ_ONLY | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   0, DSAT_VENDOR_QCTEMP_IDX,  NULL }

  ,{ "$QCAGC", READ_ONLY | RESTRICTED | COMMON_CMD,
    SPECIAL_NONE,   0, DSAT_VENDOR_QCAGC_IDX,   NULL }

#ifdef FEATURE_WCDMA
  ,{ "$QCALLUP", READ_ONLY | RESTRICTED | UMTS_CMD,
    SPECIAL_NONE,   0, DSAT_VENDOR_QCALLUP_IDX,  NULL }
#endif /* FEATURE_WCDMA */
#endif /* FEATURE_DSAT_GOBI_MAINLINE */
#ifdef FEATURE_ECALL_APP 
  ,{ "$ECALL",    EXTENDED | CONFIG | LIST| LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    3,DSAT_VENDOR_ECALL_IDX,   &dsat_ecall_list[0] }
#endif /* FEATURE_ECALL_APP */

#ifdef FEATURE_DSAT_ETSI_DATA
  ,{ "$QCDEFPROF",   EXTENDED | CONFIG | LOCAL_TEST | UMTS_CMD,
  SPECIAL_NONE,    3, DSAT_VENDOR_QCDEFPROF_IDX,    &dsat_qcdefprof_dfl[0] }
#endif /* FEATURE_DSAT_ETSI_DATA */

  ,{ "$QCMRUE",  EXTENDED | COMMON_CMD,
     SPECIAL_NONE,    4,  DSAT_VENDOR_QCMRUE_IDX,  NULL }
  
  ,{ "$QCMRUC",  EXTENDED | COMMON_CMD,
     SPECIAL_NONE,    0,  DSAT_VENDOR_QCMRUC_IDX,  NULL }
#ifdef FEATURE_DSAT_ETSI_DATA  
  ,{ "$QCAPNE",  EXTENDED | UMTS_CMD,
     SPECIAL_NONE,    7,DSAT_VENDOR_QCAPNE_IDX,  NULL }

  ,{ "$QCPDPIMSCFGE",  EXTENDED | UMTS_CMD,
       SPECIAL_NONE,    4,DSAT_VENDOR_QCPDPIMSCFGE_IDX,  NULL }
      
#endif /* FEATURE_DSAT_ETSI_DATA */
  ,{ "$QCCLAC",  READ_ONLY|RESTRICTED | COMMON_CMD,
       SPECIAL_NONE,    0,DSAT_VENDOR_QCCLAC_IDX,  NULL }

  ,{ "^SPN",    READ_ONLY | UMTS_CMD,
    SPECIAL_NONE ,     0, DSAT_VENDOR_SPN_IDX,  NULL }
  
  ,{ "$QCRMCALL",  ABORT_CMD | EXTENDED | COMMON_CMD, 
       SPECIAL_NONE, 6, DSAT_VENDOR_QCRMCALL_IDX,  NULL }
#ifdef FEATURE_DSAT_LTE
  ,{ "$QCDRX", CONFIG | LOCAL_TEST | EXTENDED | BROKEN_RANGE | UMTS_CMD | MULTI_SUBS,
     SPECIAL_NONE, 1, DSAT_VENDOR_QCDRX_IDX,  dsat_qcdrx_dflm }

  ,{ "$QCRSRP", READ_ONLY| EXTENDED | UMTS_CMD,
     SPECIAL_NONE, 0 , DSAT_VENDOR_QCRSRP_IDX,  NULL }
  
  ,{ "$QCRSRQ", READ_ONLY| EXTENDED | UMTS_CMD,
     SPECIAL_NONE, 0 , DSAT_VENDOR_QCRSRQ_IDX,   NULL }

  ,{ "$QCACQDBC",  EXTENDED ,
     SPECIAL_NONE,    0,  DSAT_VENDOR_QCACQDBC_IDX,   NULL }
#endif /* FEATURE_DSAT_LTE */

#ifdef FEATURE_DSAT_EXT_CLIENT_SUPPORT
  ,{ "$QCATMOD",    EXTENDED | LOCAL_TEST| COMMON_CMD,
     SPECIAL_NONE,   1,DSAT_VENDOR_QCATMOD_IDX, NULL }
#endif /* FEATURE_DSAT_EXT_CLIENT_SUPPORT*/
#ifdef FEATURE_DSAT_HIGH_TIER
  ,{ "$QCCOPS",     ABORT_CMD | EXTENDED | LOCAL_TEST | CONFIG | LIST | MIXED_PARAM | MULTI_SUBS | UMTS_CMD,
     SPECIAL_NONE,   4,DSATETSI_EXT_ACT_COPS_ETSI_IDX, &dsat_cops_mixed_dfl[0] }
#endif /* FEATURE_DSAT_HIGH_TIER */
#ifdef FEATURE_SGLTE
  ,{ "$QCHCOPS", READ_ONLY | EXTENDED | UMTS_CMD,
     SPECIAL_NONE, 0, DSAT_VENDOR_QCHCOPS_IDX, NULL }

  ,{ "$QCHCREG", READ_ONLY | EXTENDED | UMTS_CMD,
     SPECIAL_NONE, 0, DSAT_VENDOR_QCHCREG_IDX, NULL }
#endif /* FEATURE_SGLTE */
  ,{ "^MODE",    EXTENDED | CONFIG | UMTS_CMD,
    SPECIAL_NONE ,     1, DSAT_VENDOR_MODE_IDX,  dsat_mode_dflm }

  ,{ "$QCSIMT", READ_ONLY ,
     SPECIAL_NONE,    0,	DSAT_VENDOR_QCSIMT_IDX,	  NULL }
	
  ,{ "$QCNSP",    EXTENDED |LOCAL_TEST | UMTS_CMD,
    SPECIAL_NONE,    3,   DSAT_VENDOR_QCNSP_IDX,      NULL }

  ,{ "$QCRCIND",    EXTENDED | CONFIG | UMTS_CMD,
    SPECIAL_NONE ,     1, DSAT_VENDOR_QCRCIND_IDX,  dsat_qcrcind_dflm }

  ,{ "$QCCSGCOPS",     ABORT_CMD | EXTENDED | LOCAL_TEST | CONFIG | LIST | MIXED_PARAM | MULTI_SUBS | UMTS_CMD,
    SPECIAL_NONE,   6, DSATETSI_EXT_ACT_COPS_ETSI_IDX, &dsat_cops_mixed_dfl[0] }

#ifdef FEATURE_DSAT_ETSI_DATA
  ,{ "$QCPDPCFGEXT",  EXTENDED | COMMON_CMD,
     SPECIAL_NONE,   4,DSAT_VENDOR_QCPDPCFGEXT_IDX,   NULL }
#endif /* FEATURE_DSAT_ETSI_DATA */
#ifdef FEATURE_DSAT_LTE
#if defined(FEATURE_VOIP) && defined(FEATURE_IMS)
  ,{ "$QCVOIPM",  EXTENDED | UMTS_CMD,
     SPECIAL_NONE, 10, DSAT_VENDOR_QCVOIPM_IDX,  NULL }
#endif /* defined(FEATURE_VOIP) && defined(FEATURE_IMS) */
#endif /* FEATURE_DSAT_LTE */

 ,{ "$QCPRFCRT",  EXTENDED  | LOCAL_TEST | COMMON_CMD,
     SPECIAL_NONE,   4,   DSAT_VENDOR_QCPRFCRT_IDX,   NULL }

 ,{ "$QCPRFMOD",  EXTENDED  | LOCAL_TEST | COMMON_CMD,
     SPECIAL_NONE,  15,  DSAT_VENDOR_QCPRFMOD_IDX,    NULL }

};

/* Command table sizes. */
const unsigned int dsat_basic_table_size = ARR_SIZE( dsat_basic_table );
const unsigned int dsat_basic_action_table_size =
                        ARR_SIZE( dsat_basic_action_table );
const unsigned int dsat_sreg_table_size = ARR_SIZE( dsat_sreg_table );
const unsigned int dsat_ext_table_size = ARR_SIZE( dsat_ext_table );
const unsigned int dsat_vendor_table_size = ARR_SIZE( dsat_vendor_table );

/*===========================================================================

FUNCTION DSATCTAB_DATA_INIT

DESCRIPTION
  This function initializes the AT command defaults, limits, and lists
  that depend on a run-time determination of the hardware.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatctab_data_init
(
  void
)
{
  /*---------------------------------------------------------------------------
  status from NV call. This variable will be used to obtain the status of the 
  read/write operations to the NV.
  ---------------------------------------------------------------------------*/
  nv_stat_enum_type    nv_status;          

  /*-----------------------------------------------------------------------
  Read the stored Baud rate from the NV. If NV is not written, then use 
  the default values.
  -----------------------------------------------------------------------*/
  nv_status = dsatutil_get_nv_item(NV_DS_DEFAULT_BAUDRATE_I, &ds_nv_item);
  
  /*-----------------------------------------------------------------------
  Check if the NV has valid data inside and load the data from NV to 
  local item.
  
  If NV is ACTIVE, confirm that the value inside NV is the valid value 
  by calling sio_is_baud_valid()       
  -----------------------------------------------------------------------*/
  if( nv_status == NV_DONE_S)
  {
    if( sio_is_baud_valid((sio_bitrate_type)ds_nv_item.ds_default_baudrate) 
      == TRUE)
    {
      dsat_qcter_list.default_v = ds_nv_item.ds_default_baudrate;
      dsat_ipr_list.default_v   = ds_nv_item.ds_default_baudrate;
    }
  }
  else
  {
    /*-----------------------------------------------------------------------
    If NV was never written then copy the default values into the ipr_val. 
    -----------------------------------------------------------------------*/
    dsat_qcter_list.default_v  = (dsat_num_item_type)SIO_BITRATE_115200;
    dsat_ipr_list.default_v    = (dsat_num_item_type)SIO_BITRATE_115200;  
    
    /*-----------------------------------------------------------------------
      Initialize NV from statically-initialized default.
    -----------------------------------------------------------------------*/
    if( nv_status == NV_NOTACTIVE_S )
    {
      ds_nv_item.ds_default_baudrate = (uint16)dsat_qcter_list.default_v;
      (void)dsatutil_put_nv_item(NV_DS_DEFAULT_BAUDRATE_I, &ds_nv_item);
    }
  }
}  /* dsatctab_data_init( ) */

/*===========================================================================

FUNCTION DSAT_INIT_QCSIMAPP_TABLE

DESCRIPTION
  This function initializes the QCSIMAPP table.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
======================================================================*/
void dsat_init_qcsimapp_table(void)
{
  dsat_slot_idx_e_type slot_idx;
  dsat_apps_id_e_type  apps_id;

  for (slot_idx = DSAT_SLOT_IDX_1; slot_idx < DSAT_SLOT_IDX_MAX; slot_idx++)
  {
    for (apps_id = DSAT_APPS_ID_GW_PRIMARY; apps_id < DSAT_APPS_ID_MAX; apps_id++)
    {
      dsat_qcsimapp_table[slot_idx].apps_id_mapped[apps_id] = FALSE;
    }
  }

  dsat_qcsimapp_table[DSAT_SLOT_IDX_1].apps_id_mapped[DSAT_APPS_ID_GW_PRIMARY]   = TRUE;
  dsat_qcsimapp_table[DSAT_SLOT_IDX_1].apps_id_mapped[DSAT_APPS_ID_1X_PRIMARY]   = TRUE;
  dsat_qcsimapp_table[DSAT_SLOT_IDX_2].apps_id_mapped[DSAT_APPS_ID_GW_SECONDARY] = TRUE;
  dsat_qcsimapp_table[DSAT_SLOT_IDX_3].apps_id_mapped[DSAT_APPS_ID_GW_TERTIARY]  = TRUE;

  dsat_qcsimapp_table[DSAT_SLOT_IDX_1].subs_id = SYS_MODEM_AS_ID_1;
  dsat_qcsimapp_table[DSAT_SLOT_IDX_2].subs_id = SYS_MODEM_AS_ID_2;
  dsat_qcsimapp_table[DSAT_SLOT_IDX_3].subs_id = SYS_MODEM_AS_ID_3;

  return;
} /* dsat_init_qcsimapp_table */

/*===========================================================================
FUNCTION: DSAT_UPDATE_QCSIMAPP_TABLE

DESCRIPTION:
  This function updates QCSIMAPP table in flex mapping scenarios.

DEPENDENCIES:
  None

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/
void dsat_update_qcsimapp_table
(
  dsat_slot_idx_e_type slot_idx
)
{
  mmgsdi_slot_id_enum_type slot_id = slot_idx + MMGSDI_SLOT_1;
  dsat_apps_id_e_type      apps_id;

  /* Determine the lowest numbered session mapped to the slot.*/
  for (apps_id = DSAT_APPS_ID_GW_PRIMARY; apps_id < DSAT_APPS_ID_MAX; apps_id++)
  {
    if (slot_id == dsat_qcsimapp_info[apps_id].slot_id)
    {
      dsat_qcsimapp_table[slot_idx].subs_id = dsat_qcsimapp_info[apps_id].subs_id;
      break;
    }
  }

  if (DSAT_APPS_ID_MAX == apps_id)
  {
    /* Determine the lowest numbered session internally mapped to the slot index.*/
    for (apps_id = DSAT_APPS_ID_GW_PRIMARY; apps_id < DSAT_APPS_ID_MAX; apps_id++)
    {
      if (TRUE == dsat_qcsimapp_table[slot_idx].apps_id_mapped[apps_id])
      {
        dsat_qcsimapp_table[slot_idx].subs_id = dsat_qcsimapp_info[apps_id].subs_id;
        break;
      }
    }
 
    /* Subs ID must be invalid when none of the sessions are mapped to slot, this can happen
    only when either multiple GW sessions or multiple 1X sessions are present in same slot.*/
    if (DSAT_APPS_ID_MAX == apps_id)
    {
      dsat_qcsimapp_table[slot_idx].subs_id = DSAT_SYS_MODEM_AS_ID_MAX;
    }
  }

  return;
} /* dsat_update_qcsimapp_table */

/*===========================================================================

FUNCTION  DSAT_SET_QCSIMAPP_VAL

DESCRIPTION
  This function set dsat_qcsimapp_val or dsat_qcsimapp_val_updated

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsat_set_qcsimapp_val
(
  sys_modem_as_id_e_type subs_id
)
{
  dsat_slot_idx_e_type     slot_idx;
  ds3g_siolib_port_e_type  port_count = DS3G_SIOLIB_DATA_PORT;
  dsat_sio_info_s_type    *sio_info_ptr = NULL;

  for (slot_idx = DSAT_SLOT_IDX_1; slot_idx < DSAT_SLOT_IDX_MAX; slot_idx++)
  {
    if (subs_id == dsat_qcsimapp_table[slot_idx].subs_id)
    {
      break;
    }
  }

  /*Returning if the default data subscription is not mapped to any Slot.
    This can happen when two subscriptions are present on same slot.*/
  if (DSAT_SLOT_IDX_MAX == slot_idx)
  {
    DS_AT_MSG0_ERROR("Subs_id is not mapped to any Slot, ignoring setting it as qcsimapp_val");
    return;
  }

  DS_AT_MSG2_MED("Setting qcsimapp_val = %d, subs_id = %d", slot_idx, subs_id);

  dsatcmdp_dds_qcsimapp_val = slot_idx;

  while( port_count < DS3G_SIOLIB_PORT_MAX )
  { 
    if (TRUE == dsatcmdp_is_sio_info_inited(port_count))
    {
      sio_info_ptr = dsatcmdp_get_sio_info_ptr(port_count);
      if (FALSE == sio_info_ptr->qcsimapp_val_user_flag)
      {
        if((TRUE == dsatcmdp_processing_async_cmd) && (port_count == ds3g_siolib_get_active_port()))
        {
          DS_AT_MSG1_MED("Setting qcsimapp_val_updated = %d", slot_idx);
          sio_info_ptr->qcsimapp_val_updated = slot_idx;
        }
        else
        {
          sio_info_ptr->qcsimapp_val = slot_idx;
        }
      }
    }
    port_count++;
  }
}/*dsat_set_qcsimapp_val*/

/*===========================================================================

FUNCTION  DSAT_UPDATE_QCSIMAPP_VAL

DESCRIPTION
  This function updates dsat_qcsimapp_val

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsat_update_qcsimapp_val(void)
{
  dsat_sio_info_s_type *sio_info_ptr = NULL;

  sio_info_ptr = dsatcmdp_get_sio_info_ptr(ds3g_siolib_get_active_port());

  if(sio_info_ptr->qcsimapp_val_updated != DSAT_QCSIMAPP_VAL_INVALID)
  {
    sio_info_ptr->qcsimapp_val = sio_info_ptr->qcsimapp_val_updated;
    sio_info_ptr->qcsimapp_val_updated = DSAT_QCSIMAPP_VAL_INVALID;
  }
}/*dsat_update_qcsimapp_val*/

/*===========================================================================

FUNCTION  DSAT_GET_QCSIMAPP_VAL

DESCRIPTION
  This function returns value of dsat_qcsimapp_val 

DEPENDENCIES
  None
  
RETURN VALUE
  value of dsat_qcsimapp_val

SIDE EFFECTS
  None

===========================================================================*/

dsat_num_item_type dsat_get_qcsimapp_val(void)
{
  dsat_num_item_type       qcsimapp_val = dsatcmdp_dds_qcsimapp_val;
  ds3g_siolib_port_e_type  port_id      = ds3g_siolib_get_active_port();
  dsat_sio_info_s_type    *sio_info_ptr = NULL;

  if ((port_id >= DS3G_SIOLIB_DATA_PORT) && (port_id < DS3G_SIOLIB_PORT_MAX))
  {
    if (TRUE == dsatcmdp_is_sio_info_inited(port_id))
    {
      sio_info_ptr = dsatcmdp_get_sio_info_ptr(port_id);
      qcsimapp_val = sio_info_ptr->qcsimapp_val;
    }
  }

  return qcsimapp_val;
}/*dsat_get_qcsimapp_val*/

/*===========================================================================

FUNCTION  DSAT_GET_DS_PROFILE_SUBS_ID

DESCRIPTION
  This function returns value of dsat_get_ds_profile_subs_id 

DEPENDENCIES
  None
  
RETURN VALUE
  value of dsat_get_ds_profile_subs_id

SIDE EFFECTS
  None

===========================================================================*/

ds_profile_subs_etype dsat_get_ds_profile_subs_id
(
  sys_modem_as_id_e_type subs_id
)
{
  ds_profile_subs_etype ps_subs;

  switch ( subs_id )
  {
    case SYS_MODEM_AS_ID_1:
    {
      ps_subs = DS_PROFILE_ACTIVE_SUBSCRIPTION_1;
      break;
    }
    
    case SYS_MODEM_AS_ID_2:
    {
      ps_subs = DS_PROFILE_ACTIVE_SUBSCRIPTION_2;
      break;
    }
    
    case SYS_MODEM_AS_ID_3:
    {
      ps_subs = DS_PROFILE_ACTIVE_SUBSCRIPTION_3;
      break;
    }
    
    default:
    {
      ps_subs = DS_PROFILE_ACTIVE_SUBSCRIPTION_NONE;
      break;
    }
  }
  
  return ps_subs;
}/* dsat_get_ds_profile_subs_id */

/*===========================================================================

FUNCTION  DSAT_GET_CURRENT_DS_PROFILE_SUBS_ID

DESCRIPTION
  This function returns value of dsat_get_current_ds_profile_subs_id 

DEPENDENCIES
  None
  
RETURN VALUE
  value of dsat_get_ds_profile_subs_id

SIDE EFFECTS
  None

===========================================================================*/

ds_profile_subs_etype dsat_get_current_ds_profile_subs_id( void )
{
  sys_modem_as_id_e_type subs_id = dsat_get_current_subs_id(FALSE);

  return dsat_get_ds_profile_subs_id(subs_id);
}/* dsat_get_current_ds_profile_subs_id */

/*===========================================================================

FUNCTION  DSAT_SET_QCSIMAPP_VAL_USER_FLAG

DESCRIPTION
  This function sets dsat_qcsimapp_val_user_flag

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsat_set_qcsimapp_val_user_flag(void)
{
  dsat_sio_info_s_type *sio_info_ptr = NULL;
  
  sio_info_ptr = dsatcmdp_get_sio_info_ptr(ds3g_siolib_get_active_port());

  sio_info_ptr->qcsimapp_val_user_flag = TRUE;
} /* dsat_set_qcsimapp_val_user_flag */

#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS
/*===========================================================================

FUNCTION DSATCTAB_GET_MODEM_PORT_ID

DESCRIPTION
  This function returns the corresponding modem port id for AT terminal

DEPENDENCIES
  None
  
RETURN VALUE
  -1: if the input port is not modem port

SIDE EFFECTS
  None

===========================================================================*/
int dsatctab_get_modem_port_id(ds3g_siolib_port_e_type port)
{

  int modem_port_id = DSAT_MODEM_PORT_NONE;

  if ( port == DS3G_SIOLIB_USB_CDC_PORT )
  {
    modem_port_id = DSAT_MODEM_PORT_1;
  }
  else if ( port == DS3G_SIOLIB_USB_SER3_PORT ) 
  {
    modem_port_id = DSAT_MODEM_PORT_2;
  }
  else
  {
  }
  return modem_port_id;
} /*dsatctab_get_modem_port_id*/

#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/

