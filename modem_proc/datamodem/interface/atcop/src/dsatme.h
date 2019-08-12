#ifndef DSATME_H
#define DSATME_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                A T   C O M M A N D   
                
                ( M o b i l e  E q u i p m e n t  C O M M A N D S )
                
                P R O C E S S I N G

                E X T E R N A L   H E A D E R   F I L E


GENERAL DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  executing the mobile equipment commands.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

   Copyright (c) 2003 - 2015 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsatme.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   tk/pg   Added support for Dynamic ATCoP.
06/12/14   pg      Depricated mmgsdi_session_perso APIs.
06/06/14   sc      Fixed AT+CPBR=? issue. 
10/28/13   pg      Fixed CPOL issue on DSDS.
07/02/13   pg      Migrated to MMGSDI API to find PLMN info in SE13 table
06/05/13   tk      ATCoP changes for SGLTE support on Dime Plus.
07/31/12   sk      C-EONS Feature Support.
05/18/12   tk      Migrated to MSG 2.0 macros
02/06/12   nc      Added support for ^SPN and URC Queue.
01/19/12   sk      Feature cleanup.
10/25/11   ua      Enhanced EONS to read EFHPLMN to check if a PLMN is registered
                   in HPLMN. 
10/20/11   nc      Added support for CUAD/CEAP/CERP commands.
07/05/11   nc      Added support for ^MODE,^CARDMODE,^SYSCONFIG,^SYSINFO,^DSCI.
06/24/11   bs      Added support for EF CFIS/CFF.
04/13/11   ua      Extneding EONS for DSDS. 
03/17/11   bs      Fixed DSM corruption issue.
01/11/11   ad      Removed pending variable.
09/30/10   ad      Including pbmlib.h for CMI compliance.  
07/14/10   ua      Fixed race condition in processing REFRESH for 
                   SIM files against regular SIM file operations. 
06/04/10   bs      MMGSDI SESSION Migration.
06/01/10   ad      Adding new fields in +CPBW Commands .
05/10/10   kk      Adding support for +ICCID.
01/20/10   kk      Extended +CFUN to CDMA/MM targets.
01/15/10   ua      PBM AT Commands support for DSDS and CSIM. 
01/15/10   sa      Added support for +CPIN,^CPIN and +CLCK for RUIM cards.
11/16/09   ca      Added support for MD5 message entry mode. 
12/15/09   nc      Featurisation changes for LTE.
09/09/09   ua      Added support for EONS (spec 22.101).
                   Removed COPS_GSDI_SIM_READ as it is not valid anymore.
12/12/08   ua      Fixed Off target lint errors.
10/23/08   bs      Added support for 1X AT Phonebook commands.
07/21/08   bs      Added support for CDMA NAARefresh.
06/13/08   ua      Correcting reading of data from RUIM/NV items
                   for OMH 2.
01/31/08   bs      Fixing $QCSIMSTAT featurization issue.
01/09/08   bs      Added new command $QCSIMSTAT for sim status.
06/27/07   ua      Removing FEATURE wraps across mmgsdi callback functions. 
03/07/07   sa      Fixed feature wrap issues
05/09/06   ua      Added function dsatme_mmgsdi_get_file_attr_cb 
02/09/06   ua      The FEATURE_DSAT_CFUN_CARD_POWER_CTL is removed from this file 
                   to extend for MMGSDI support for +CPLS and +CPOL commands. 
08/29/05   snb     Add MMGSDI client ID variable.
01/06/03   snb     Move registration for pin event with GSDI and handling 
                   callback for pin events to common file and add protocol
                   stack re-initialization under feature define.

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"


#include "dsati.h"

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
#include "pbmlib.h"
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

#ifdef FEATURE_MMGSDI_PERSONALIZATION
#define PIN_MAX_LEN MMGSDI_PERSO_MAX_KEY
#elif defined(FEATURE_MMGSDI) /* FEATURE_MMGSDI_PERSONALIZATION */
#define PIN_MAX_LEN MMGSDI_PIN_MAX_LEN
#else /* FEATURE_MMGSDI */
#error code not present
#endif

#ifdef FEATURE_DSAT_ETSI_MODE
#include "dsatetsicall.h"
#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_MMGSDI
#include "mmgsdilib.h"
#include "mmgsdisessionlib.h"
#include "mmgsdisessionlib_v.h"
#include "simlock_modem_lib.h"
#include "simlock_common.h"
#define DSAT_MAX_QCSIM_STAT 2
#define DSAT_MAX_QCSIMSTAT_CHARS 40
#endif /* FEATURE_MMGSDI */

#define DSAT_PIN_LENGTH 8

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)

#define DSAT_CPBW_MAX_FIELDS 3

#if defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD)

#define DSAT_CPBW_MAX_FIELDS_TOTAL 8
#else
#define DSAT_CPBW_MAX_FIELDS_TOTAL  DSAT_CPBW_MAX_FIELDS
#endif /* defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD) */

#define CPB_FIELD_TEXT 0
#define CPB_FIELD_NUMBER 1
#define CPB_FIELD_NUMBER_TYPE 2

#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

/* Note which callback function submitted async cmd status cmd.
   This determine the action to take when the status handler function 
   is called.
   
   The variable of this type identifies the ME cmd processing status before 
   calling the async commands of a subsystem.  It determines 
   the next action to take after the async cmd cb is called.
   
   Note which callback function is expected to be called for debug purpose.
   Make sure the right callback is called at the right time.
   Usually you should not care about it unless you want the callback safty
   check too.  Then you need make sure the cb_expected is set correctly.
   Hint, looking for ASSERT(cb_expected == xxx)
*/

/* turn off complaint about CB_MAX not used. It is needed to handle the feature
   combinations */
/*lint -e749 */
typedef enum
{
  CB_NONE = 0,            /* no callback function expected to be 
                             called                                     */
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
  CPB_PBM_CB_READY = 10,       /* inform atcop that pbm is ready             */
  CPB_PBM_CB_READY_FILE_INFO = 11,
                          /* atcop calls pbm_file_info to get the info 
                             of the default ph book storage type.  It
                             is not for an AT cmd                       */
  CPB_PBM_CB_FILE_INFO = 12,   /* cb return's the request pbm device 
                             information for cpbs                       */
  CPB_PBM_CB_WRITE = 13,       /* cb return's status if the write action 
                             succeeds                                   */
  /* the pbm cmd group for direct dialing                               */
  DD_PBM_FILE_INFO = 14,     /* direct dialing processes ATD><mem><n>.  
                           It calls pbm_file_info to get the valid 
                           index range of ph book type <mem>            */
  CPBS_MMGSDI_PIN2_STATUS = 15,   /* cpbs command requested pin status          */
  CPBS_MMGSDI_PIN2_VERIFY = 16,   /* cpbs command requested pin verification    */
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

  CLCK_MMGSDI_FDN_ENABLE = 20,   /*  clck command requested to lock FD */
  CLCK_MMGSDI_FDN_DISABLE = 21,  /*  clck command requested to unlock FD */
  CLCK_MMGSDI_FDN_QUERY = 22,    /*  clck command requested to query FD */

  CLCK_MMGSDI_PIN1_STATUS = 23,     /* clck command requested pin 1 status */

  CLCK_MMGSDI_PIN1_VERIFY = 24,     /* clck command requested pin verification */

  CPIN_MMGSDI_ALL_PIN_STATUS = 25,  /* requested PIN1 and PIN2 status for CPIN init */
  CPIN_MMGSDI_PIN1_STATUS = 26,     /* cpin command requested pin1 status */
  CPIN_MMGSDI_PIN2_STATUS = 27,     /* cpin command requested pin2 status */

  CACM_MMGSDI_PIN2_VERIFY = 28,     /* cacm command requested pin verification */
  CAMM_MMGSDI_PIN2_VERIFY = 29,     /* camm command requested pin verification */
  CPUC_MMGSDI_PIN2_VERIFY = 30,     /* cpuc command requested pin verification */


  CPIN_MMGSDI_PIN1_VERIFY = 31,     /* cpin command requested pin1 verification */
  CPIN_MMGSDI_PIN2_VERIFY = 32,     /* cpin command requested pin2 verification */

  CPWD_MMGSDI_PIN1_CHANGE = 33,     /* cpwd command requested pin change */
  CPWD_MMGSDI_PIN2_CHANGE = 34,     /* cpwd command requested pin change */

#ifdef FEATURE_MMGSDI_PERSONALIZATION
  CPIN_MMGSDI_PERSO_REG = 35,        /* requested registration for GSDI personalization
                                 PIN events */
  CPIN_MMGSDI_PERSO_DEACT = 36,      /* cpin command requested deactivation of a 
                                 personalization PIN */
  CPIN_MMGSDI_PERSO_ACT = 37,        /* clck cmd to a activate fac thr' clck */
  CPIN_MMGSDI_PERSO_PERM_DIS = 38,   /* clck cmd to disable fac thr' clck */
  CPIN_MMGSDI_PERSO_GET_IND_RSP = 39,/* clck cmd to query fac thr' clck */
#endif /* FEATURE_MMGSDI_PERSONALIZATION */
  CLCK_MMGSDI_FDN_LOCK = 40,
  CLCK_MMGSDI_FDN_UNLOCK = 41,

  ICCID_MMGSDI_SIM_READ = 51,       /* iccid command requested SCID */
  CPUC_MMGSDI_SIM_READ = 52,        /* cpuc command requested PUC */
  CPUC_MMGSDI_SIM_WRITE = 53,       /* cpuc command updating PUC */
  COPS_MMGSDI_SIM_READ = 54,        /* cops command requested IMSI */
  COPS_CM_CMD_ABORT = 55,         /* cops command aborted */
  CLCC_CM_CMD_ABORT = 56,         /* clcc command aborted */
  COPN_CM_CMD_ABORT = 57,         /* copn command aborted */
  COPN_UI_NETWK_BY_INDEX = 58,    /* to get plmn list one at a time for copn command */

  CUAD_MMGSDI_SIM_FILE_ATTR = 61,
  CUAD_MMGSDI_SIM_READ      = 62,
  CUAD_MMGSDI_SIM_ERROR     = 63,
#ifdef FEATURE_MMGSDI_ATCSIM
  CSIM_MMGSDI_OPEN_CHAN        = 70,       /* +CSIM open channel */
  CSIM_MMGSDI_SEND_APDU        = 71,
  CSIM_MMGSDI_CLOSE_CHAN       = 72,      /* +CSIM close channel */
  CSIM_MMGSDI_USIM_ADF_SELECT  = 73,
#endif /* FEATURE_MMGSDI_ATCSIM */

  CPOL_MMGSDI_SIM_READ = 80,      /* +CPOL read command */
  CPOL_MMGSDI_SIM_WRITE = 81,     /* +CPOL write command */
  CPOL_MMGSDI_SIM_FILE_ATTR = 82, /* +CPOL Test Command */
  CPOL_MMGSDI_SIM_ERROR = 83,     /* +CPOL read/write error */
  CPOL_MMGSDI_SIM_ACCESS_DENIED = 84, /* +CPOL read/write access denied */  
  COPS_MMGSDI_PLMN_INFO = 85,  /*+COPS PLMN ID read*/
  CPOL_MMGSDI_PLMN_INFO = 86,  /*CPOL PLMN ID read*/
  SS_CM_CMD_ABORT = 90,          /* Supplementary service command aborted */
#ifdef FEATURE_DSAT_CFUN_CARD_POWER_CTL
  CFUN_MMGSDI_CARD_PWR_CTL = 100,   /* +CFUN SIM card power control */
#endif /* FEATURE_DSAT_CFUN_CARD_POWER_CTL */

  DSAT_MMGSDI_CSP_READ  = 120,    /* +PACSP CSP read */
  DSAT_MMGSDI_SPN_READ  = 121,    /*  EONS SPN read */
  DSAT_MMGSDI_IMSI_READ = 122,    /*  IMSI  read */

#ifdef FEATURE_DSAT_CDMA_PIN
  DSAT_CDMA_CPIN_MMGSDI_ALL_PIN_STATUS = 140, /* requested PIN1 and PIN2 status 
                                            for CDMA ^CPIN                    */
  DSAT_CDMA_CPIN_MMGSDI_PIN1_VERIFY = 141,    /* cpin command req pin1 verification */
  DSAT_CDMA_CPIN_MMGSDI_PIN2_VERIFY = 142,    /* cpin command req pin2 verification */
  DSAT_CDMA_CLCK_MMGSDI_PIN1_VERIFY = 143,    /* CLCK SIM PIN1 verfication          */
  DSAT_CDMA_CLCK_MMGSDI_FDN_ENABLE_DISABLE = 144,/* CLCK FDN enabling/disabling        */
  DSAT_CDMA_CLCK_MMGSDI_FDN_QUERY = 145,
  DSAT_CDMA_CLCK_MMGSDI_PIN1_STATUS = 146,
  DSAT_CDMA_CPWD_MMGSDI_PIN1_CHANGE = 147,     /* cpwd CDMA  pin1 change            */
  DSAT_CDMA_CPWD_MMGSDI_PIN2_CHANGE = 148,     /* cpwd CDMA pin2 change             */
#endif /* FEATURE_DSAT_CDMA_PIN */
  CLCK_GSDI_FDN_PIN2_VERIFY = 149,   /*  clck command requested PIN2 verification */
#ifdef FEATURE_DSAT_MMGSDI_AUTH  
  SSDUDPCFM_MMGSDI_CFM_SSD = 150, /*confirm SSD command */
  SSDUDP_MMGSDI_RAND_SSD   = 151, /*update SSD command*/
  SSD_MMGSDI_BSCHAL_SSD    = 152, /*Base Station Challenge Order  command*/
  CAVE_MMGSDI_AUTH_RANDU   = 153, /*CAVE authentication command*/
  VPM_MMGSDI_KEY_GEN       = 154, /*VPM key generation command*/
  UIMAUTH_MMGSDI_CAVE_MD5  = 155, /*UIM checks the authentication algo it supports*/
  GSN_MMGSDI_MEID_ESN      = 156, /*Client checks datacard/module's ESN and UIMID*/
  MD5_MMGSDI_IP_AUTH       = 157, /* To support MD5 authentication  */
#endif	/*FEATURE_DSAT_MMGSDI_AUTH*/

  DSAT_MMGSDI_GET_CURR_OPER_NAME = 160, /* C-EONS Camped PLMN name */
  DSAT_MMGSDI_GET_OPER_NAME_LIST = 161, /* C-EONS PLMN search list */
  DSAT_MMGSDI_GET_PREF_NETS_NAME = 162, /* C-EONS CPOL PLMN list */
  DSAT_MMGSDI_GET_CURR_HYBR_OPER_NAME = 163, /* C-EONS Camped Hybrid PLMN name */

  DSAT_MMGSDI_REFRESH_MASK_CB = 0xFF00,  /* To differentiate regular MMGSDI file 
                                            operations with REFRESH file operations.  */

  CB_MAX = 0xFFFF
} cb_cmd_e_type;

typedef struct
{
   mmgsdi_session_id_type session_id;
   cb_cmd_e_type perso_cmd;
   uint8 slot;
}dsat_perso_user_type;

typedef dsat_result_enum_type (*status_handler_type) (ds_at_cmd_status_type *);

#if defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION)
/*This enum will trace MMGSDI Session changed event + Active flag */
typedef enum
{
  DSAT_MMGSDI_INACTIVE      = 0, /* This will set in bootup or when CFUN call card PUP or card down */
  DSAT_MMGSDI_ACTIVE        = 1, /* This will set when SUBSCRIPTION CHANGED recevied with slot info */
  DSAT_MMGSDI_DOWN          = 2, /*+CFUN 0 is in progress */
  DSAT_MMGSDI_UP            = 4  /*+CFUN 1 is in progress */
} dsat_mmgsdi_ready_state;

#endif /* defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION) */

typedef enum
{
  DSAT_QCSIMSTAT_SIM_INIT_COMPLETED,
  DSAT_QCSIMSTAT_SIM_ERROR,
  DSAT_QCSIMSTAT_UNKNOWN
} dsat_qcsimstat_e_type;

typedef struct 
{
  const cb_cmd_e_type cmd_id;
  const status_handler_type status_handler;
} status_handler_table_entry_type;

typedef struct 
{
  mmgsdi_slot_id_enum_type slot;
  mmgsdi_aid_type aid_info;
} dsat_app_info_s_type;

/* Removed FEATURE_DSAT_CFUN_CARD_POWER_CTL for MMGSDI support for +CPLS and 
   +CPOL commands */
/*---------------------------------------------------------------------------
  ATCOP's MMGSDI client ID: 
  Only needed for SIM card power control functionality.
---------------------------------------------------------------------------*/

extern boolean dsatme_pending_mmgsdi_command[];

  #define VERIFY_PIN( session_id, pin_id, pin_data, client_ref ) \
    mmgsdi_status = mmgsdi_session_verify_pin\
                                     (session_id,\
                                      pin_id, \
                                      pin_data, \
                                      dsatme_mmgsdi_req_cb, \
                                      (mmgsdi_client_data_type)client_ref);

#define GET_PIN_STATUS( session_id, client_ref ) \
   mmgsdi_status = mmgsdi_session_get_all_pin_status \
                           (session_id, \
                            dsatme_mmgsdi_req_cb,\
                            (mmgsdi_client_data_type)client_ref); 


/* Length of storage type string */
#define CPBS_VAL_LEN 2

#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
#ifdef FEATURE_DSAT_EXTENDED_CMD
#define DSATME_ENCODING_RAW   1
#define DSATME_ENCODING_ASCII 2
#endif /* FEATURE_DSAT_EXTENDED_CMD */

/* the structure contains infos about a ph book storage type */
typedef struct {
  /* pbm enum corresponding to the atcop storage type */
  pbm_device_type device; 

  /* ATCOP ph book storage type corresponding to the pbm enum.
     This field seems redundant with device.  It is here to save
     a lot of conversion between ATCOP storage type string and 
     pbm device enum. */
  byte storage[CPBS_VAL_LEN + 1]; 

  /* total locations in current ph book, -1 means pbm does
     not reveal the information about the storage */
  int16 total;

  /* maximum text length allowed for the current 
     storage type */
  int16 text_len;

  pbm_return_type pbm_status;
} me_pb_entry_type;

typedef struct 
{
  int     index;                /* Logical index across multiple phonebook sets */
  char    number[PBM_NUM_SIZE+1]; /* + and null for string */
  uint8   type;                 /* type of address octet in integer format */
  char    text[PBM_TEXT_SIZE_BYTES ];  /* null terminated string */
  uint32  time;                        /* Date and time in clock seconds */
  uint32  duration;                    /* in seconds, USIM supports up to 0x00FFFFFF */
  boolean is_answered;                 /* TRUE = answered, FALSE = not answered */ 
#if defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD)
  uint8   adtype;                            /* type of address octet in integer format */
  char    group[PBM_TEXT_SIZE_BYTES];        /* group number */
  char    adnumber[PBM_NUM_SIZE];            /* additional number */
  char    secondtext[PBM_TEXT_SIZE_BYTES ];  /* null terminated string */
  char    email[PBM_TEXT_SIZE_BYTES ];       /* null terminated string */
#endif  /* defined(FEATURE_PBM_USIM_SUPPORT) && !defined(FEATURE_DSAT_EXTENDED_CMD) */
} dsat_pbm_record_s_type;

typedef pbm_record_id_type dsat_pbm_rec_id_type;

#define DSATME_ENCODING_RAW   1
#define DSATME_ENCODING_ASCII 2
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

typedef enum
{
  CSP_PLMN_MODE_BIT_OFF =0,
  CSP_PLMN_MODE_BIT_ON,
  CSP_PLMN_MODE_BIT_UNSET = 0xFF
} me_plmn_mode_bit_e_type;

#define INVALID_PLMN_DIGIT  0xFF
#define MAX_2_DIGIT_DEC_NUM 99
#define PLMN_LEN            3
#define PLMN_W_ACT_LEN      5
#define ACT_NONE            0
#define ACT_GSM             0x80
#define ACT_GSM_COMPACT     0x40
#define ACT_UMTS            0x80

/* Macros required in EONS feature */
/* Length values of the various fields in Corresponding SIM files */
#define DSAT_NETWORK_PLMN_LEN            0x03
#define DSAT_MAX_SPN_NAME_LEN            18
#define DSAT_MAX_OPL_LIST_LEN            50
#define DSAT_MAX_NETWORK_LONGNAME_LEN    40
#define DSAT_MAX_NETWORK_SHORTNAME_LEN   20
#define DSAT_MAX_PNN_ARRAY_SIZE          50
#define DSAT_INVALID_NUM_RECS            -1
#define DSAT_MAX_CPHS_OPNAME_LEN         20
#define DAT_MAX_SPDI_PLMN_LIST           50
#define DSAT_MAX_SST_SIZE                50
#define DSAT_MAX_OPL_PLMN_LIST           50
#define DAT_MAX_EHPLMN_PLMN_LIST         50

/* Structure to hold the SPN details */
typedef struct
{
  boolean     spn_info_valid;  /* True, if SPN info is valid  */
  uint8       display_cond;    /* Display condition */
  char        operator_name[DSAT_MAX_SPN_NAME_LEN]; /* Operator name */
  uint8       operator_name_len;
  dsati_chset_type encoding_type;
}dsat_mmgsdi_spn_info_s_type;

/* Structure to hold the ONS details */
typedef struct
{
  boolean  ons_info_valid;
  uint8    name_len;
  char     operator_name[DSAT_MAX_CPHS_OPNAME_LEN];
}dsat_mmgsdi_ons_info_s_type;

/* Structure to hold the SPDI details */
typedef struct
{
  boolean            spdi_info_valid;  /* True, if SPDI info is valid */
  uint16             num_plmn;         /* Number of valid PLMN's*/
  sys_plmn_id_s_type plmn_id[DAT_MAX_SPDI_PLMN_LIST]; /* PLMN id*/
}dsat_mmgsdi_spdi_info_s_type;

/* Structure to hold the SST details */
typedef struct
{
  boolean        sst_info_valid;           /* True, if SST info is valid */
  byte           data[DSAT_MAX_SST_SIZE];  /* Data */
  uint16         len;                      /* Length*/
}dsat_mmgsdi_sst_info_s_type;

/* Structure to hold the AD details */
typedef struct
{
  byte           data[4];   /* Data */
  uint16         len;                      /* Length*/
}dsat_mmgsdi_ad_info_s_type;

/* Structure to hold the IMSI details */
typedef struct
{
  byte           data[MMGSDI_IMSI_LEN];   /* Data */
  uint16         len;                      /* Length*/
}dsat_mmgsdi_imsi_info_s_type;


/* Structure to hold the PNN details */
typedef struct
{
  sys_plmn_id_s_type    plmn_id;
  word                  lac1;
  word                  lac2;
  byte                  pnn_rec_num;
}dsat_pnn_plmn_info;

typedef struct
{

  byte    long_name[DSAT_MAX_FULLNAME_STR_LEN + 1];
  uint16  long_name_len;
  byte    short_name[DSAT_MAX_SHORTNAME_STR_LEN + 1];
  uint8   short_name_len;
  boolean country_initial; /*Country Initial */
  uint16  coding_scheme; /* GSM 7 bit Default or UCS2 */
  uint8   spare_bits_long_name;
  uint8   spare_bits_short_name;
}dsat_pnn_record_s_type;

typedef struct
{
  uint16                         num_recs_read;
  boolean                        pnn_info_available;
  dsat_pnn_record_s_type         pnn_list[DSAT_MAX_PNN_ARRAY_SIZE ];
  mmgsdi_file_attributes_type    file_attrib; 
}dsat_mmgsdi_pnn_info_s_type;

/* Structure to hold the OPL details */
typedef struct
{
  uint16                       num_plmn; /* Number of valid PLMN's in a record*/
  uint16                       num_recs_read;/* Number of valid records*/
  boolean                      opl_info_available; /* If OPL info available */
  dsat_pnn_plmn_info           plmn_info[DSAT_MAX_OPL_PLMN_LIST];/*PLMN info */
  mmgsdi_file_attributes_type  file_attrib; 
}dsat_mmgsdi_opl_info_s_type;

/* Levels of phone functionality for +CFUN command */
typedef enum me_func
{
  /* Minimimum functionality & SIM card power down if 
     if FEATURE_DSAT_CFUN_CARD_POWER_CTL defined */
  DSAT_ME_FUNC_MIN     = 0,    
  DSAT_ME_FUNC_FULL    = 1,    /* Normal functionality */
  DSAT_ME_FUNC_RFOFF   = 4,    /* TX & Rx RF off */
  DSAT_ME_FUNC_FTM     = 5,    /* Factory Test Mode */
  DSAT_ME_FUNC_RESET   = 6,    /* Reset UE */
  DSAT_ME_FUNC_OFFLINE = 7,    /* Offline mode */
  DSAT_ME_FUNC_MAX             /* Internal use only! */
} dsat_me_func_e_type;

/* Reset values for +CFUN command */
typedef enum me_reset
{
  DSAT_ME_DONT_RESET   = 0,    
  DSAT_ME_DO_RESET     = 1
} dsat_me_reset_e_type;

typedef enum clck_perso_fac_e
{
  DSAT_MMGSDI_PERSO_FAC_PN,      /* Network personalization */
  DSAT_MMGSDI_PERSO_FAC_PU,      /* Network subnet personalization */
  DSAT_MMGSDI_PERSO_FAC_PP,      /* Service provider personalization */
  DSAT_MMGSDI_PERSO_FAC_PC,      /* Corporate Personalization */
  DSAT_MMGSDI_PERSO_FAC_PF       /* Phone locked to very first SIM inserted */
} clck_perso_fac_e_type;
#define DSAT_MAX_DISPLAY_ALPHA_LEN PBM_TEXT_SIZE_CHARS

#ifdef FEATURE_MMGSDI_ATCSIM
/* turn off lint info "local macro not referenced",
   macros not currently used are defined for future convenience */
/* lint -e750 */
/* +CRSM definitions: */

/* Instuction codes */
#define CRSM_CMD_RD_BIN  176 
#define CRSM_CMD_RD_REC  178    
#define CRSM_CMD_GET_RSP 192
#define CRSM_CMD_UPD_BIN 214
#define CRSM_CMD_UPD_REC 220
#define CRSM_CMD_STATUS  242
#define FILE_ID_LEN 4
#define FILE_ID_SIZE     2

#define MAX_PATH_LEN 4
#endif/*FEATURE_MMGSDI_ATCSIM*/
typedef struct 
{
  mmgsdi_rec_num_type               num_of_rec;    /**< Number of records in the file. */
  mmgsdi_rec_num_type               curr_rec;
  mmgsdi_len_type                   rec_len;       /**< Length of each record. */
}cuad_rec_type;
#ifdef FEATURE_DATA_PS_EAP
typedef enum
{
  DSAT_EAP_STATE_NONE,
  DSAT_EAP_STATE_AUTHENTICATING,
  DSAT_EAP_STATE_AUTHENTICATED,
  DSAT_EAP_STATE_HELD
}dsat_eap_state;

typedef struct
{
  uint8                 pmk[DS_EAP_MAX_PRE_MASTER_KEY_SIZE];
  uint16                pmk_len;
  uint8                 emsk[DS_EAP_MAX_PRE_MASTER_KEY_SIZE];
  uint16                emsk_len;
}me_eap_key;

typedef struct
{
  dsat_num_item_type         eap_method;
  dsat_eap_state             eap_state;
  eap_handle_type            eap_handle;
  dsat_num_item_type         eap_session_id;
  me_eap_key                 *eap_key;
  boolean                    is_initialized;
}me_eap_info;
#endif /* FEATURE_DATA_PS_EAP */
#ifdef FEATURE_MMGSDI
typedef enum
{
  DSAT_CPOL_LONG_ALPHA_NUMERIC,
  DSAT_CPOL_SHORT_ALPHA_NUMERIC,
  DSAT_CPOL_NUMERIC
}dsat_cpol_dis_frmt_e_type;

typedef struct 
{
  int16 start_index;
  int16 end_index;
  char  display_str[20];
  dsat_cpol_dis_frmt_e_type format;
  sys_user_pref_plmn_list_s_type   user_pref_networks;
}dsat_me_cpol_read_param_s_type;

typedef struct 
{
  int16 no_of_args;
  int16 index;
  sys_mcc_type  mcc;
  sys_mnc_type  mnc;
  sys_access_tech_e_type act;
  boolean       mnc_includes_pcs_digit;
}dsat_me_cpol_write_param_s_type;

/* CPLS Variable */
typedef enum 
{
  DSAT_PLMN = 0,
  DSAT_PLMNWACT = 0,
  DSAT_OPLMNWACT,
  DSAT_HPLMNWACT,
  DSAT_MAX_SUPP_PLMNWACT
}dsat_cpls_plmn_selector;
#endif /* FEATURE_MMGSDI */

typedef struct
{
  mmgsdi_client_id_type dsat_mmgsdi_client_id;
  char                  dsat_alpha_text[DSAT_MAX_DISPLAY_ALPHA_LEN];
#if defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION)
/*Having single state variable for SGLTE/DSDA 
This is diffrent then RESTRICTED mode.*/
  dsat_mmgsdi_ready_state dsat_apps_provisioned;
#endif /* defined(FEATURE_8960_SGLTE_FUSION) || defined(FEATURE_8960_DSDA_FUSION)*/
#ifdef FEATURE_DATA_PS_EAP
  me_eap_info                      dsat_eap_info;
  eap_sim_aka_meta_info_type      *dsat_eap_meta_info_ptr;
#endif /* FEATURE_DATA_PS_EAP */
#ifdef FEATURE_MMGSDI_ATCSIM
  uint32                           csim_apdu_data_len;
  uint8                            csim_apdu_data[CSIM_MAX_CMD_APDU_SIZE + 1];
#endif /* FEATURE_MMGSDI_ATCSIM*/
  boolean                          dsat_is_card_power_down;
  uint8                            dsat_num_of_card_operations;
  uint8                            dsat_card_error_event_count;
}dsatme_mmgsdi_state_ss_info;

typedef struct dsat_mmgsdi_state_ms_info
{
  dsat_mmgsdi_spn_info_s_type  dsat_spn_info;    /* EF-SPN information */
  dsat_mmgsdi_imsi_info_s_type dsat_imsi_info;
  me_plmn_mode_bit_e_type      dsatme_plmn_mode_sel_bit;
  cuad_rec_type                dsat_cuad_recs;
  dsat_fcn_ind_type            dsat_refresh_fcn_ind;
  uint32                       dsat_pin_required;
  uint32                       dsat_pin_blocked;
  boolean                      dsat_mask_pin2_state;
  boolean                      dsat_sim_init_comp;
  dsat_cme_error_e_type        dsat_pin_error;
  /* +CPOL global variables */
  dsat_me_cpol_write_param_s_type  dsat_cpol_write_params;
  /* +CPLS global variables */
  mmgsdi_file_enum_type        dsat_sim_filename_plmn_selector;
  boolean                      dsat_plmn_wact_supported;
}dsatme_mmgsdi_state_ms_info;

typedef struct dsatme_mmgsdi_state_ms_slot_info
{
  dsat_qcsimstat_e_type      dsat_qcsimstat_curr;
  boolean                    is_qcsimstat_updated;
  dsat_cme_error_e_type      dsat_pin_error;
}dsatme_mmgsdi_state_ms_slot_info;

typedef struct
{
  dsatme_mmgsdi_state_ss_info ss;
  dsatme_mmgsdi_state_ms_info ms[DSAT_MS_MAX_SUBS];
  dsatme_mmgsdi_state_ms_slot_info ms_slot[DSAT_MAX_MMGSDI_SLOTS];
}dsatme_mmgsdi_state_info;
#define MAX_PBM_AT_NAME_LEN  8
/* me_cpbs_params hold current +CPBS argument across 
   functions */
typedef struct
{
  pbm_device_type  pbm_device;
  byte password[PIN_MAX_LEN + 1];

  /* is_read field identifies whether the current +CPBS cmd 
     is this a read cmd.
     The responses for cpbs set (for example, AT+CPBS="SM")
     and read (for example, AT+CPBS?) are different */
  boolean is_read;  

  char display_name[MAX_PBM_AT_NAME_LEN];
} cpbs_argu_type;
/* used to remember the last index from pbm_read command in
   order to send the final read results to the response generator */
typedef struct {
  /* The current +CPBR index under processing.
     There is a possibility of re-entering read command handler, if we
     hit high AT->SIO TX watermark count. In such case, we resume the
     reading with next record from current (i.e., current++). */   
  uint16 current;

  /* Maintain last index. Read continues until current index reaches 
     last index. */
  uint16 last;
  char display_name[MAX_PBM_AT_NAME_LEN];
} cpbr_index_type;

typedef struct {
  /*Contain current issued command ^CPBF/+CPBF*/
  char display_name[MAX_PBM_AT_NAME_LEN];
} cpbf_index_type;
/* local variables for direct dialing functions */
typedef struct {
  /* contains the abbtributes of direct dial string             */
  dsati_dial_str_type   dial_str;

  /* contains the original strings after '>'
     It is used as a match str to search for a number in
     ph book after pbm cb unqualifies it as valid <mem><num>.
     It is used to hold the number we fetch from ph book.
     dial_str.num is a pointer pointing to the cmd token, which
     contains the dial number.
     The token recycles while we process direct dialing.  Thus 
     we have to save it here                                    */
  byte                  str[MAX_LINE_SIZE];

  /* contains <mem> of D><mem><n>.  This is for 27007 v370 
     Section 6.3 case 2.
     It contains '\0' if the string after > is not in the 
     pattern of <mem><n>                                        */
  byte                  mem[CPBS_VAL_LEN + 1];

  /* contains index, the <n> of D>mem<n> or D><n>.
     It is used to read the number at location n of <mem> 
     after pbm_file_info's cb function verify the index is 
     in valid range                                             
     It is -1 at init time or the command is D><str>            */
  int                   index;

  /* the best pbm number entry found so far.
     If we found a ph number with name starts with the 
     searching string, we save it here as a candidate.  We
     searching for a exact match.  If we can't found a exact
     match in all the supported ph book, we use this number 
     to dial.                                                   
     If record.number[0] is '\0', this field is not set         */
  dsat_pbm_record_s_type     record;

  /* remember which ph book we are searching, start 
     with current pbs ph book.
     Init to be -1*/
  int8                  srch_pb;

} etsime_dd_type;
/* state of the dialed number to be saved to the last dial ph book */
typedef enum {
  LD_NONE,         /* no dialing                */
  LD_DIALED,       /* number has been dialed    */
  LD_CONNECTED     /* connected                 */
} ld_state_type;
/* type of dialed number to be saved to the last dial ph book */
typedef struct {
  ld_state_type      state;      /* dialed number state             */
  uint32             conn_time;  /* time when the call is connected */
  dsat_pbm_record_s_type  rec;   /* contains the dialed record      */
} ld_rec_type;

typedef struct
{
  uint8            dsat_pbm_handler;
  boolean          dsatme_nv_only;
  cpbs_argu_type   me_cpbs_params;
  cpbr_index_type  cpbr_index;
  cpbf_index_type  cpbf_index;
} dsatme_pbm_state_ss_info;

typedef struct
{
  ld_rec_type     *ld_rec_ptr;
} dsatme_pbm_state_ms_info;

typedef struct
{
  dsatme_pbm_state_ss_info ss;
  dsatme_pbm_state_ms_info ms[DSAT_MS_MAX_SUBS];
} dsatme_pbm_state_info;

typedef dsat_result_enum_type (*pbm_cb_handler_type)(ds_at_pbm_info_type *);

typedef struct 
{
  const cb_cmd_e_type           cmd_id;
  const pbm_cb_handler_type     handler;
} pbm_cb_handler_table_type;

extern const pbm_cb_handler_table_type pbm_cb_handler_table[];
#ifdef FEATURE_DSAT_DYNAMIC_LOADING
extern const pbm_cb_handler_table_type pbm_cb_handler_table_ex[];
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */

/*===========================================================================

                        PUBLIC DATA DECLARATIONS

===========================================================================*/
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
void dsatme_cpb_pbm_read_record
(
  dsat_pbm_rec_id_type        record_id,
  dsat_pbm_record_s_type     *record_ptr,
  pbm_return_type            *pbm_return_ptr
);

#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */

void submit_cmd_status 
(
  cb_cmd_e_type           cmd_id,         /* who submitted this status */
  mmgsdi_return_enum_type status,         /* cmd status                */
  mmgsdi_session_id_type  session_id,     /* Which apps_id             */
  byte                    data            /* cmd data                  */
);

/*===========================================================================

FUNCTION ME_PBM_WRITE_CB

DESCRIPTION
  This function is called by pbm to return the results of pbm_session_record_write
  function call.

  This function is passed to pbm with pbm_session_record_write function call.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
/* ARGSUSED */
LOCAL void me_pbm_write_cb
(
  pbm_writecb_data_s_type *cb_data   /* pbm_session_record_write callback structure */
);

extern clck_perso_fac_e_type clck_perso_fac;

/*===========================================================================

FUNCTION DSATME_REQ_MMGSDI_SIM_READ

DESCRIPTION
  This function sends MMGSDI a message to read the EF from the card selected
  by $QCSLOT value. The input parameter maps to a specific EF.
                  

DEPENDENCIES
  None

RETURN VALUE
  returns boolean:
    FALSE: if dsat_qcslot_val is not GSM operational slot
    TRUE:  executing the MMGSDI async call and waiting for the callback function
           to be called.

SIDE EFFECTS
  None
===========================================================================*/
extern boolean dsatme_req_mmgsdi_sim_read
(
  cb_cmd_e_type                   cb_cmd,
  mmgsdi_file_structure_enum_type file_type,
  uint32                          rec_num,
  dsat_apps_id_e_type             apps_id
);

/*===========================================================================

FUNCTION DSATME_CMD_STATUS_HANDLER_EX

DESCRIPTION
  This function is the async event handler for async functions such as
  pbm_select and pbm_write.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatme_cmd_status_handler_ex
(
  ds_cmd_type * cmd_ptr
);

/*===========================================================================

FUNCTION DSATME_CMD_STATUS_HANDLER

DESCRIPTION
  This function is the async event handler for async functions such as
  pbm_select and pbm_write.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatme_cmd_status_handler
(
  ds_at_cmd_status_type *status_cmd_ptr /* Status command pointer */
);

/*-------------------------------------------------------------------------
            PUBLIC FUNCTION DECLARATIONS
-------------------------------------------------------------------------*/

#ifdef FEATURE_MMGSDI
/*===========================================================================

FUNCTION DSATME_MMGSDI_REQ_CB

DESCRIPTION
  This function is the callback function for the MMGSDI to return
  pin status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_mmgsdi_req_cb
(
  mmgsdi_return_enum_type status, /* MMGSDI return status */
  mmgsdi_cnf_enum_type    cnf,    /* Type of command confirmation */
  const mmgsdi_cnf_type *cnf_ptr  /* Pointer to confirmation data */
);

LOCAL dsat_result_enum_type cpin_track_pin_status
(
  mmgsdi_events_enum_type     event,
  mmgsdi_pin_status_enum_type pin_status,
  dsat_apps_id_e_type         apps_id
);

/*===========================================================================

FUNCTION  DSATME_MMGSDI_SIM_EVENT_HANDLER_EX

DESCRIPTION
  This function is the handler function for the MMGSDI SIM events

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CMD_ERR_RSP:  if there was any problem in executing the command 
    DSAT_OK :          if it is a success.
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatme_mmgsdi_sim_event_handler_ex
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION  DSATME_MMGSDI_SIM_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the MMGSDI SIM events

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CMD_ERR_RSP:  if there was any problem in executing the command 
    DSAT_OK :          if it is a success.
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatme_mmgsdi_sim_event_handler
(
  ds_at_mmgsdi_sim_info_type *sim_info /* MMGSDI SIM info pointer */
);

#ifdef FEATURE_DATA_ETSI_PIN
boolean me_is_restricting_pin_puk_set(dsat_apps_id_e_type apps_id);
#endif /* FEATURE_DATA_ETSI_PIN */
/*===========================================================================
FUNCTION DSAT_MMGSDI_INIT_HANDLER

DESCRIPTION
  This function is handler function for MMGSDI INIT event.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ERROR : if there was any problem .
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
======================================================================*/

dsat_result_enum_type  dsat_mmgsdi_init_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

#endif /* FEATURE_MMGSDI */
/*===========================================================================

FUNCTION  DSATETSIME_SET_CME_ERROR

DESCRIPTION
  Write Mobile Equipment error to res_buff_ptr.  The Mobile Equipment error
  code and error text are defined in Section 9.2 of 3GPP TS 27.007

  If AT+CMEE is 0, +CME ERROR : <err> result code is disabled
                   ERROR is used
  If AT+CMEE is 1, +CME ERROR : <err> result code is enabled
                   to used numeric err value
  If AT+CMEE is 2, +CME ERROR : <err> result code is enabled
                   to used verbose text err value

DEPENDENCIES
  res_buff_ptr point to a good dsm buffer with enough space left 
  (at lease 60 bytes) to write error code.
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
void dsatme_set_cme_error
(
  dsat_cme_error_e_type error_code,
  dsm_item_type         *res_buff_ptr
);
#if defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM)
/*===========================================================================
FUNCTION  DSATME_INIT_PBM

DESCRIPTION
  PBM initialization function. This function does the following:
  - Initializes ATCOP as one of the clients to PBM.
  - Registers the client call back functions.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_init_pbm ( void );
/*===========================================================================
FUNCTION DSATME_INIT_PBM_MODE_FROM_NV

DESCRIPTION

    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatme_init_pbm_mode_from_nv
(
  void
);

/*===========================================================================

FUNCTION DSATME_EXEC_CPBS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPBS command.
  The result is in res_buff_ptr.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatme_exec_cpbs_cmd
(
  dsat_mode_enum_type mode,     /*  AT command mode:            */
  const dsati_cmd_type *parse_table, /*  Ptr to cmd in parse table */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr  /*  Place to put response       */
);
/*===========================================================================

FUNCTION DSATME_EXEC_CPBR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPBR command.
  +CPBR command is used to read phonebook entries.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK        : success
    DSAT_ERROR     : if there was any problem in executing the command
    DSAT_ASYNC_CMD : Async call is successfully made to pbm to read
                     ph book number

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatme_exec_cpbr_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATME_EXEC_CPBF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPBF command.
  +CPBF command is used to search phonebook entries.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK          : success
    DSAT_ERROR       : if there was any problem in executing the command
    DSAT_ASYNC_CMD   : Async call is successfully made to pbm to find a
                       number in the ph book
    DSAT_CMD_ERR_RSP : if there is cme error.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatme_exec_cpbf_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATME_EXEC_CPBW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CPBW command.
  +CPBW command is used to write phonebook entry.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK          : success
    DSAT_ASYNC_CMD   : Async call is successfully made to pbm to write a
                       number into the ph book
    DSAT_ERROR       : if there was any problem in executing the command
    DSAT_CMD_ERR_RSP : if there is cme error.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatme_exec_cpbw_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

/* ===========================================================================
FUNCTION DSATME_EXEC_QCPBMPREF_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This command is used to choose the application and
  Local or Global phone book type. A change in the Phone book type 
  comes into effect only for the subsequent Phone book AT commands.
  Allowed range of input is (0 - 3).
     Syntax of the command:
        $QCPBMPREF=<Preference>

<Preference>:
0 -- Slot 1 Global Phone Book
1 -- App 1 Local Phone Book
2 -- Slot 2 Gloobal Phone Book
3 -- App 2 Local Phone Book

Default: 0 (Slot 1 Global Phone Book)

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK          : success
    DSAT_ERROR       : if there was any problem in executing the command

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatme_exec_qcpbmpref_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATME_RESET_PB

DESCRIPTION
  Reset ph book to default values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  me_cpbs_entry will be set to default if dsat_cpbs_val is 0
  and me_cpbs_entry.device != PBM_DS_SM

===========================================================================*/
void dsatme_reset_pb(void);

/*===========================================================================

FUNCTION DSATME_PBM_CB_CMD_HANDLER

DESCRIPTION
  Processes DS_AT_PBM_CB_CMD cmd infos.

DEPENDENCIES
  None

RETURN VALUE
  Returns the command running status.
    DSAT_ASYNC_CMD   : waiting for next cb.
    DSAT_ERROR       : if there is error.
    DSAT_CMD_ERR_RSP : if there is cme error.
    DSAT_OK          : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatme_pbm_cb_cmd_handler
(
  ds_cmd_type *         cmd_ptr
);

/*===========================================================================

FUNCTION DSATME_DIRECT_DIAL

DESCRIPTION
  Handle atd direct dialing from phonebooks command.  
  See 3GPP TS 27007 Section 6.3

  This function get the number from ph book first, and then dial 
  them number.

  This function handle all the 3 cases.
  1. D><str>[I][G][;]
  2. D>mem<n>[I][G][;]
  3. D><n>[I][G][;]

DEPENDENCIES
  None

RETURN VALUE
  DSAT_ASYNC_CMD   : Async calls were make to pbm to get the number
  DSAT_CMD_ERR_RSP : A cme error relating to ph book is returned to TE.
  DSAT_ERROR       : If the command fails.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatme_direct_dial
(
  /* containing dial string and its attribs                     */
  const dsati_dial_str_type     * dial_ptr,

  /*  Place to put response                                     */
  dsm_item_type                 * res_buff_ptr
);

/*===========================================================================

FUNCTION DSATME_LD_INIT_RECORD

DESCRIPTION
  Prepare the ld_rec for saving dialing number into last dial ph book.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatme_ld_init_record
(
  char * number, /* dialed number                         */
  int    type,   /* number type, -1 means "not provided"        */
  char * text    /* text associate with the dialed number */
);

/*===========================================================================

FUNCTION DSATME_LD_CONNECTED

DESCRIPTION
  Change ld_rec state to be connected.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatme_ld_connected
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================

FUNCTION ME_PBM_CB_SUBMIT_DS_CMD

DESCRIPTION
  Submit a ds cmd to process pbm cb result in ds_task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void me_pbm_cb_submit_ds_cmd
(
  cb_cmd_e_type   cmd,                   /* cmd id                   */
  pbm_return_type status,                /*  command status          */

                                         /* for CPB_PBM_CB_FILE_INFO */
  ds_at_pbm_file_info_type * info_ptr,   /* file info                */


  pbm_record_s_type * record_ptr         /* for CPB_PBM_CB_READ,
                                            CPB_PBM_CB_FIND          */
);

/*=======================================================================

FUNCTION DSAT_GET_PHONEBOOK_ID

DESCRIPTION
  stores the pbm_phonebook_type to a local variable.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_get_phonebook_id
(
  pbm_phonebook_type *pb_id_local
);


/*===========================================================================

FUNCTION DSATME_LD_SAVE

DESCRIPTION
  Store ld_rec to the last dial ph book.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatme_ld_save
(
  sys_modem_as_id_e_type subs_id
);
#endif /* defined(FEATURE_ETSI_PBM) || defined(FEATURE_DSAT_CDMA_PBM) */
/*===========================================================================
FUNCTION DSATME_EXEC_CARDMODE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ^CARDMODE command, which returns the 
  mode of the card amongst SIM or USIM

^CARDMODE	<CR><LF>^CARDMODE:<sim_type><CR><LF><CR><LF>OK <CR><LF>

<sim_type>:Types of SIM card. Its values are as below:
0:  Unknown mode
1:  SIM card
2:  USIM card
3:  CSIM card
4:  RUIM card

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatme_exec_cardmode_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
FUNCTION DSATME_EXEC_SPN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This command is used to display the brand information from EF-SPN

Command | Response
--------|---------------------
^SPN	|^SPN:<disp_rplmn>,<coding>,<spn_name>
        | OK

<disp_rplmn>:
 0: RPLMN will not be displayed
 1: RPLMN is required to be displayed
 99:this field is noneffective, and there’s no need for reading spn_name field again.

<coding>:coding scheme, representing the character coding of spn_name field and indicating the language:
 0:GSM 7 bit Default Alphabet
 1:USC2 mode

<spn_name>:character string, the length will not exceed 16 bytes.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatme_exec_spn_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATME_EXEC_QCSIMAPP_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes $QCSIMAPP command which selects a SIM slot.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
/*ARGSUSED */
dsat_result_enum_type dsatme_exec_qcsimapp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#ifdef FEATURE_MMGSDI

/*===========================================================================

FUNCTION  DSATME_MMGSDI_EVENT_HANDLER

DESCRIPTION
  This function is the handler function for the MMGSDI SIM events

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT : async event, continue processing  

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatme_mmgsdi_event_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

/*==============================================================================
FUNCTION  DSATME_RUIM_MMGSDI_REGISTER_FOR_REFRESH

DESCRIPTION
  This function registers the required files of RUIM NV used in ATCoP for file
  change notifications.

PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

==============================================================================*/
void dsatme_mmgsdi_register_for_refresh
(
  dsat_apps_id_e_type apps_id
);

#ifdef FEATURE_DSAT_ETSI_MODE
/*===========================================================================

FUNCTION DSATME_EXEC_QCSIMSTAT_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes $QCSIMSTAT command, which enables/disables the URC response
  of the SIM status based on GSDI events.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatme_exec_qcsimstat_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATME_REPORT_QCSIMSTAT_RESULT

DESCRIPTION
  This function generates the unsolicited result code for SIM status
  command $QCSIMSTAT.  The command parameter <n> controls whether
  results are presented or suppressed.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatme_report_qcsimstat_result 
( 
  mmgsdi_slot_id_enum_type slot_id
);

/*===========================================================================

FUNCTION  DSATME_INIT_SIM_FILENAME

DESCRIPTION
  This function is an initialization function for the sim file which is 
  used in +CPLS and +CPOL commands.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ASYNC_EVENT : async event, contine processing  

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatme_init_sim_filename( void );

#endif /* FEATURE_DSAT_ETSI_MODE */
/*===========================================================================
FUNCTION:      DSATME_MMGSDI_READ_RECORD_CB

DESCRIPTION:
  This function provides internal callback function pointer to internal
  processing that MMGSDI does not care for the callback information returned
DEPENDENCIES:
  None

LIMITATIONS:
  Parameter check for specific commands are not performed

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/

void dsatme_mmgsdi_read_record_cb 
(
  mmgsdi_return_enum_type status, 
  mmgsdi_cnf_enum_type    cnf, 
  const mmgsdi_cnf_type   *cnf_ptr
);

/*===========================================================================
FUNCTION:      DSATME_MMGSDI_WRITE_RECORD_CB

DESCRIPTION:
  This function provides internal callback function pointer to internal
  processing that MMGSDI does not care for the callback information returned

DEPENDENCIES:
  None

LIMITATIONS:
  Parameter check for specific commands are not performed

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/

void dsatme_mmgsdi_write_record_cb 
(
  mmgsdi_return_enum_type status, 
  mmgsdi_cnf_enum_type    cnf, 
 const mmgsdi_cnf_type    *cnf_ptr
);

/*===========================================================================
FUNCTION:      DSATME_MMGSDI_READ_TRANSPARENT_CB

DESCRIPTION:
  This function provides internal callback function pointer to internal
  processing that MMGSDI does not care for the callback information returned
DEPENDENCIES:
  None

LIMITATIONS:
  Parameter check for specific commands are not performed

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/

void dsatme_mmgsdi_read_transparent_cb 
(
  mmgsdi_return_enum_type status, 
  mmgsdi_cnf_enum_type    cnf, 
  const mmgsdi_cnf_type   *cnf_ptr
);

/*===========================================================================
FUNCTION:      DSATME_MMGSDI_WRITE_TRANSPARENT_CB

DESCRIPTION:
  This function provides internal callback function pointer to internal
  processing that MMGSDI does not care for the callback information returned

DEPENDENCIES:
  None

LIMITATIONS:
  Parameter check for specific commands are not performed

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/

void dsatme_mmgsdi_write_transparent_cb 
(
  mmgsdi_return_enum_type status, 
  mmgsdi_cnf_enum_type    cnf, 
 const mmgsdi_cnf_type    *cnf_ptr
);

/*===========================================================================
FUNCTION:      DSATME_MMGSDI_GET_FILE_ATTR_CB

DESCRIPTION:
  This function provides internal callback function pointer to internal
  processing that MMGSDI does not care for the callback information returned

DEPENDENCIES:
  None

LIMITATIONS:
  Parameter check for specific commands are not performed

RETURN VALUE:
  None

SIDE EFFECTS:
  None

===========================================================================*/

void dsatme_mmgsdi_get_file_attr_cb 
(
  mmgsdi_return_enum_type status, 
  mmgsdi_cnf_enum_type    cnf, 
  const mmgsdi_cnf_type  *cnf_ptr
);

/*==============================================================================
FUNCTION  DSATME_MMGSDI_CPOL_READ_HANDLER

DESCRIPTION
  Handles the asynchronous read response for UIM_USIM_ONS or UIM_GSM_ONS.

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ERROR : if read failed
  DSAT_ASYNC_EVENT: if read is success.
  
SIDE EFFECTS
  None
==============================================================================*/
dsat_result_enum_type dsatme_mmgsdi_cpol_read_handler
(
  ds_at_mmgsdi_sim_info_type *sim_info /* MMGSDI SIM info pointer */
);

/*==============================================================================
FUNCTION  DSATME_MMGSDI_CUAD_READ_HANDLER

DESCRIPTION
  Handles the asynchronous read response for a record read on EF-DIR.

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ERROR : if read failed
  DSAT_ASYNC_EVENT: if read is success.
  
SIDE EFFECTS
  None
==============================================================================*/
dsat_result_enum_type dsatme_mmgsdi_cuad_read_handler
(
  ds_at_mmgsdi_sim_info_type *sim_info /* MMGSDI SIM info pointer */
);

/*==============================================================================
FUNCTION  DSATME_FORMAT_CMD_RSP

DESCRIPTION
  Utility function for formatting the input cmd_ptr input from integer to Hex and appending 
  it to the output res_buff_ptr[res_buff_ptr->used].
  
DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ERROR : if read failed
  DSAT_ASYNC_EVENT: if read is success.
  
SIDE EFFECTS
  None
==============================================================================*/
dsat_result_enum_type dsatme_format_cmd_rsp
(
  byte *cmd_ptr,
  uint32 num_bytes,
  dsm_item_type *res_buff_ptr
);


/*===========================================================================

FUNCTION DSATME_DISPLAY_SPN

DESCRIPTION
  This function gets the operator name from the file SPN per subscription. 

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE: Operator name present in SPN file.
  FALSE: If not present.

SIDE EFFECTS
  None
  
======================================================================*/
boolean dsatme_display_spn_per_subs
( 
  char *encoded_buffer,             /* Response, which contains the operator name */
  sys_plmn_id_s_type plmn_info,     /* PLMN info */
  boolean reg_plmn,                 /* If PLMN is a registered */
  uint8 *encoded_buffer_len,        /* actual length of the operator name */
  dsati_chset_type   *encoding_type, /* Encoding format ( UCS2 / GSM 7bit) */
  sys_modem_as_id_e_type    subs_id
);
/*===========================================================================

FUNCTION DSATME_DISPLAY_ONS

DESCRIPTION
  This function gets the operator name from the file ONS  per subscription. 

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE: Operator name present in ONS file.
  FALSE: If not present.

SIDE EFFECTS
  None
  
======================================================================*/
boolean dsatme_display_ons_per_subs 
( 
  char                     *encoded_buffer,     /* Response, 
                                            which contains the operator name */
  sys_modem_as_id_e_type    subs_id
);

/*===========================================================================

FUNCTION DSATME_DISPLAY_OPL_PNN

DESCRIPTION
  This function gets the operator name from the file OPL and PNN  per subscription. .

DEPENDENCIES
  None
  
RETURN VALUE
  TRUE: Operator name present in OPL and PNN files.
  FALSE: If not present.

SIDE EFFECTS
  None
  
======================================================================*/
boolean dsatme_display_opl_pnn_per_subs
(
  char     *encoded_buffer_long,  /* Response, with long operator name */
  char     *encoded_buffer_short, /* Response, with short operator name */
  uint16   *rec_num,              /* Record number in which the operator name is 
                                     selected */
  sys_plmn_id_s_type  plmn_info,  /* PLMN info*/
  boolean reg_plmn,                /* If the PLMN is registered */
  sys_modem_as_id_e_type    subs_id
);


/*==============================================================================
FUNCTION  DSATME_MMGSDI_SPN_READ_HANDLER

DESCRIPTION
  Handles the asynchronous read response for UIM_USIM_SPN or UIM_GSM_SPN.

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ERROR : if read failed
  DSAT_ASYNC_EVENT: if read is success.

SIDE EFFECTS

==============================================================================*/
dsat_result_enum_type dsatme_mmgsdi_spn_read_handler
(
  ds_at_mmgsdi_sim_info_type *cmd_rd_ptr /* MMGSDI SIM info pointer */
);


/*==============================================================================
FUNCTION  DSATME_MMGSDI_IMSI_READ_HANDLER

DESCRIPTION
  Handles the asynchronous read response for UIM_USIM_IMSI or UIM_GSM_IMSI.

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_ERROR : if read failed
  DSAT_ASYNC_EVENT: if read is success.
  
SIDE EFFECTS
  None
==============================================================================*/
dsat_result_enum_type dsatme_mmgsdi_imsi_read_handler
(
  ds_at_mmgsdi_sim_info_type *cmd_rd_ptr /* MMGSDI SIM info pointer */
);

/*==============================================================================
FUNCTION  DSATME_MMGSDI_CSP_READ_HANDLER

DESCRIPTION
  Handles the asynchronous read response for UIM_USIM_CSP or UIM_GSM_CSP.

DEPENDENCIES
  None.

RETURN VALUE
  DSAT_OK/DSAT_ERROR : if PACSP command is pending.
  DSAT_ASYNC_EVENT: if PACSP command is not pending
                    and reponse for FCN refresh read.

SIDE EFFECTS

==============================================================================*/
dsat_result_enum_type dsatme_mmgsdi_csp_read_handler
(
  ds_at_mmgsdi_sim_info_type *cmd_rd_ptr /* MMGSDI SIM info pointer */
);

/*===========================================================================

FUNCTION DSATME_REPORT_PACSP_RESULT

DESCRIPTION
  This function generates the unsolicited/normal
  response for PACSP command.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatme_report_pacsp_result 
(
  dsat_apps_id_e_type   subs_id
);

/*===========================================================================
FUNCTION DSATME_READ_UIM_FILES

DESCRIPTION
  Reads all the files required to be cached in ATCOP. 
  
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_read_uim_files ( dsat_apps_id_e_type apps_id );

/*===========================================================================
FUNCTION DSATME_GET_IMSI_INFO

DESCRIPTION
   This is an internal function, which return pointer of imsi info. 
  
DEPENDENCIES
  

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

dsat_mmgsdi_imsi_info_s_type *  dsatme_get_imsi_info(void);


/*===========================================================================

FUNCTION DSATETSIME_MMGSDI_REQ_CB

DESCRIPTION
  This function is the callback function for the MMGSDI to return
  pin status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_mmgsdi_perso_req_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
);

/*===========================================================================

FUNCTION DSATME_SIM_MMGSDI_REQ_CB

DESCRIPTION
  This function is the callback function for the MMGSDI to return for SIMLOCK feature
  pin status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_sim_mmgsdi_perso_req_cb
(
  simlock_result_enum_type status,
  const simlock_message_response_data_type  * resp_ptr,
  const void                                * user_data_ptr
);

/*===========================================================================

FUNCTION DSATETSIME_MMGSDI_FILE_ATTR_RSP_CB

DESCRIPTION
  This function is the callback function for the GSDI to return
  pin status.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void dsatetsime_mmgsdi_file_attr_rsp_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
);

/*===========================================================================

FUNCTION DSATME_MMGSDI_GET_OPERATOR_NAME_CB

DESCRIPTION
  This function is the callback function for the MMGSDI to return
  PLMN names.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_mmgsdi_get_operator_name_cb
(
  mmgsdi_return_enum_type status,
  mmgsdi_cnf_enum_type       cnf,
  const mmgsdi_cnf_type     *cnf_ptr
);

/*===========================================================================
FUNCTION DSAT_MMGSDI_GET_OPERATOR_NAME_HANDLER

DESCRIPTION
  This function is handler function for C-EONS get_operator API

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ERROR : if there was any problem .
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
======================================================================*/

dsat_result_enum_type dsat_mmgsdi_get_operator_name_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);


#endif /* FEATURE_MMGSDI */

/*===========================================================================
FUNCTION:      DSATME_UPDATE_APPSID

DESCRIPTION:
  Returns apps id based on the session id passed

DEPENDENCIES:
  None

RETURN VALUE:
  uint8 - apps id

SIDE EFFECTS:
  None

===========================================================================*/
dsat_apps_id_e_type dsatme_update_appsid
(
  mmgsdi_session_id_type  session_id
);

#ifdef FEATURE_DSAT_MMGSDI_AUTH
/*===========================================================================

FUNCTION DSATME_MD5_ABORT_CMD

DESCRIPTION
  Called by At command processor to abort an SMS command. A command can be
  aborted if user enters ESC key or if the ATCOP runs out of buffers. This 
  function will send an DS CMD to the AT Command processor indicating that
  the commaand has been aborted.

  If status is 0, the function is called when user enters ESC key.
  If status is 1, the function is called when we run out of buffer
                  or other cmd error.
  If status is other number, there is a cms error.  This function can be
                             called when there is error sending sms 
                             messages.

  Event will contain the following result code:
  DSAT_ERROR: if we are out of buffer for the message to send
  DSAT_OK: if user sent ESC character while writing the message
  A CMS error number: if there is a cms error.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_md5_abort_cmd
(
 dsat_cme_error_e_type error_code /* CME Error */
);

/*===========================================================================

FUNCTION  DSATME_MD5_ABT_HANDLER

DESCRIPTION
  Handler function for aborting a SMS command

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  DSAT_ERROR: if we are out of buffer for the message to send
  DSAT_OK: if user sent ESC character while writing the message

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatme_md5_abt_handler
(
 ds_cmd_type * cmd_ptr     /* DS Command pointer */
);

/*===========================================================================

FUNCTION DSATME_SEND_MD5_MSG

DESCRIPTION
  Central place/function call for sending sms message.
  In case of actual message transmission, it is done in 2 phases. First you 
  get the actual command with all the parameters. Parameters are stored and 
  user is sent a prompt to indicate that message has to be typed. Then the 
  message is transmited. This function is used to transmit the actual message.
  It calls the mode-specific functions to send SMS message.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_send_md5_msg
(
 byte  * msg, /* Ptr to the text message that has to be sent */
 int   len    /* Len of the text message that has to be sent */
);
#endif /* FEATURE_DSAT_MMGSDI_AUTH */

/*===========================================================================

FUNCTION DSATME_EXEC_CFUN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +CFUN command.
  This command selects the level of functionality in the phone to
  control power consumption.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.
    DSAT_ASYNC_CMD: if this is function invokes an async command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatme_exec_cfun_cmd
(
  dsat_mode_enum_type mode,                /*  AT command mode:            */
  const dsati_cmd_type *parse_table,       /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,       /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr              /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATME_PROCESS_CFUN_CMD

DESCRIPTION
  This function process the response for the +CFUN command on return from
  asynchronous processing / synchronous function call. 
 
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR :    if there was any problem in executing the command
    DSAT_OK :       if it is a success.
    DSAT_ASYNC_CMD: if this is function invokes an async command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatme_process_cfun_cmd
(
  sys_oprt_mode_e_type  oprt_mode,
  dsat_me_func_e_type   old_opmode
);

/*===========================================================================

FUNCTION DSATME_CONVERT_PHONE_STATE

DESCRIPTION
  This function converts the phone state enum values between Call
  Manager and ATCOP.  Bidirectional conversio is supported.  The ATCOP
  values are driven by 3GPP 27.007 +CFUN command description.  The
  Call Manager values are strictly internal.

DEPENDENCIES
  None

RETURN VALUE
  TRUE on successful conversion; FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean dsatme_convert_phone_state
(
  const boolean          to_at_state,  /* Conversion direction */
  sys_oprt_mode_e_type  *cm_mode_ptr,  /* Call Manager mode    */
  dsat_me_func_e_type   *at_mode_ptr   /* ATCOP mode           */
);

#ifdef FEATURE_DSAT_CFUN_CARD_POWER_CTL
/*===========================================================================

FUNCTION DSATME_PROCESS_CFUN_CARD_PDOWN_CMD

DESCRIPTION
  This function process the +CFUN card power down command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR :    If there was any problem in executing the command.
    DSAT_OK :       If it is a success.
    DSAT_ASYNC_CMD: If this is function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_process_cfun_card_pdown_cmd(void);

/*===========================================================================

FUNCTION DSATME_PROCESS_CFUN_CARD_PUP_CMD

DESCRIPTION
  This function process the +CFUN card power up command.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR :    If there was any problem in executing the command.
    DSAT_OK :       If it is a success.
    DSAT_ASYNC_CMD: If this is function invokes an async command.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatme_process_cfun_card_pup_cmd
(
  boolean opmode_change
);
#endif /* FEATURE_DSAT_CFUN_CARD_POWER_CTL */

/*===========================================================================

FUNCTION DSATME_REQ_MMGSDI_SIM_READ

DESCRIPTION
  This function sends MMGSDI a message to read the EF from the card selected
  by $QCSLOT value. The input parameter maps to a specific EF.
                  

DEPENDENCIES
  None

RETURN VALUE
  returns boolean:
    FALSE: if dsat_qcslot_val is not GSM operational slot
    TRUE:  executing the MMGSDI async call and waiting for the callback function
           to be called.

SIDE EFFECTS
  None
===========================================================================*/
boolean dsatme_req_mmgsdi_get_file_attr 
(
  cb_cmd_e_type cb_cmd , 
  dsat_apps_id_e_type apps_id 
);
/*===========================================================================

FUNCTION DSATME_INITIALIZE_TO_DEFAULT

DESCRIPTION
  .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatme_initialize_to_default(dsat_mem_type ,void *);

/*===========================================================================

FUNCTION CPB_PBM_CB_READY_HANDLER

DESCRIPTION
  .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

dsat_result_enum_type cpb_pbm_cb_ready_handler
(
  ds_at_pbm_info_type *pbm_info /* PBM info pointer */
);

/*===========================================================================

FUNCTION CPB_PBM_CB_READY_FILE_INFO_HANDLER

DESCRIPTION
  .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

dsat_result_enum_type cpb_pbm_cb_ready_file_info_handler
(
  ds_at_pbm_info_type *pbm_info /* PBM info pointer */
);

/*===========================================================================

FUNCTION ME_SESSIN_PBM_FILE_INFO_CB

DESCRIPTION
  .

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void me_session_pbm_file_info_cb
(
  pbm_return_type                status,
  pbm_phonebook_type             pbm_device,
  pbm_extended_fileinfo_s_type * pbm_extended_fileinfo
);

/*===========================================================================

FUNCTION  DSATME_INIT_CONFIG_FROM_EFS

DESCRIPTION
This Fucntion Creates EFS file  atcop_cops_auto_mode.txt if file is not present 
if file is already present it reads cops_no_mode_change value from file and
assigns it to variable cops_no_mode_change.
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void dsatme_init_config_from_efs(void);

#endif /* DSATME_H */
