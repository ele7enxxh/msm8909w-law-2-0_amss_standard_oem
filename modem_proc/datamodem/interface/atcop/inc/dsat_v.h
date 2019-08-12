#ifndef DSAT_V_H
#define DSAT_V_H
/*===========================================================================

                        D A T A   S E R V I C E S

                A T   C O M M A N D   P R O C E S S O R

                E X T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor.
  
Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsat.h_v   1.10   08 Nov 2002 12:46:24   wxie  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/inc/dsat_v.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/14   tk      Added support for Dynamic ATCoP.
09/13/13   tk      ATCoP changes for LTE and DSDS concurrency solution.
02/20/13   tk      Fixed PDP related commands issue with MAX_ARG define.
08/22/12   sk      Mainlining Nikel Changes.
08/10/12   tk      Deprecated ATCoP - DIAG interface.
07/31/12   sk      C-EONS Feature Support.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
01/19/12   sk      Feature cleanup.
10/20/11   nc      Added support for CUAD/CEAP/CERP commands.
07/27/11   ttv     Added changes required for source statistics descriptor
                   addition.
04/20/11   bs      NIKEL Phase I support.
04/13/11   ua      Extneding EONS for DSDS. 
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
10/25/10   ad      Init Changes for DSDS. 
07/28/10   bs      Fixed C+G build compiler issue.
07/23/10   nc      Added 1x SMS Support for FUSION Architecture.
05/17/10   ua      Removing dependency of ATCoP Macro MAX_ARG at QMI.
06/04/10   bs      MMGSDI SESSION Migration.
05/14/10   sa      Added support for LTE packet event reporting.
05/10/10   kk      Added support for ^HC SMS commands. 
01/29/10   sa      Added unified MH interface handler.
11/16/09   ca      Added support for the MMGSDI authentication
                   interface commands.
12/15/09   nc      Featurisation changes for LTE.
12/12/09   sa      Added prototype for dsat_cgact_done_handler().
10/14/09   ss      CMI AU level cleanup.
09/09/09   ua      Added support for EONS (spec 22.101).
09/07/09   ps      CMI SU level modifications.
05/06/09   ua      Fixed compiler warnings. 
04/20/09   bs/sa   Added support for Modem Bridge Architecture.
03/04/09   sa      AU level CMI modifications.
10/23/08   bs      Added support for 1X AT Phonebook commands.
09/30/08   ua      Extending the "connect rate" for 1x data calls. Supported rates
                   for 1x RTT, HDR REV0, HDR REVA.
07/16/08   nc      Increased the MAX_LINE_SIZE to include the 
                   <path_id> parameter for +CRSM.
07/16/08   ua      Added support for external client support. 
02/18/08   sa      Added modification for Passport Feature support.
12/20/07   sa      Fixed feature wrap issues.
11/24/07   ua      Fixed Lint-lows. 
10/09/07   ss      Added support for $QCGANSM, $QCGARL, $QCGAPL commands.
09/28/07   ua      Added struct for receiving call connection rate.
09/27/07   sa      Added new function dsat_deregister_pdp_handlers().
09/13/07   sa      Feature wrapped S2 register in FEATURE_DATA_S2_REG.
08/22/07   sa      Added new register function dsat_register_pdp_handlers() and  
                   modified existing dsat_register_handlers() for MT PDP support. 
08/16/07   sa      Replaced dsat_get_escape_char_val with a genaral
                   query function for S register dsat_get_sreg_val().
08/06/07   pp      New command: AT\Q.
07/27/07   ar      Added dsat_change_rlp_params() and dsat_get_rlp_params().
07/26/07   sa      Added dsat_get_escape_char_val.
05/09/07   az      Changed FEATURE_JCDMA_2 to FEATURE_JCDMA_1X
05/02/07   ua      Fixed lint medium errors. 
04/03/07   pkp     New commands: &E, \V, &S, \S, &W, S2, S30, S103, S104, %V.
12/01/06   rsj     Added new definition for DSAT_DELAYED enum value in 
                   dsat_result_type for JCDMA JATE requirement.
03/31/06   snb     Moved dsat_rsp_call_mode outside of ETSI wrapper.
01/07/06   TMR     Added dsat_get_baud
09/12/05   snb     Make get_imei() external function dsat_get_imei().
09/08/05   ar      Add support for $QCSDFTEST vendor command.
09/07/05   ar      Add support for call response mode.
08/14/05   sl      Added new API in serializer (dsat_change_sio_params).
04/27/05   ar      Add support for +CV120 command.
06/16/04   snb     Add function for sending intermediate result code to 
                   correct problem with fax calls.
05/24/04   snb     Correct low and high water callbacks.
11/19/03   ar      Add TFT related +CMEE errors.
11/14/03   snb     Add support for CSD call data rate reporting on CONNECT.
11/07/03   snb     Fix out of order responses due to local formatted response
                   buffers.
08/24/03   sb      Cell broadcast editions (first cut)
08/15/03   ar      Added support for dual UIM slot CME error codes.
08/13/03   snb     Added support for $QCPDPLT command
07/30/03   ar      Add multiple versions supprot to dsat_crlp_val.
05/28/03   sb      Fix for WCDMA CS Calls
05/01/03   sb      Added new result code (DSAT_ATH_CMD) to indicate that we 
                   are processing ATH command.
04/28/03   sb      AT command flow control. Removed FEATURE_DSAT_FLOW_CONTROL
03/28/03   ar      Added dsat_flush_me_msg_queue() prototype.
03/17/03   ar      Add +CHSN command support.
03/07/03   ar      Remove FEATURE_DATA_ETSI_SUPSERV wrappers
01/16/03   wx      I,G modifier support
01/09/03   wx      Added +ES, +ESA support
01/08/03   ar      Removed obsolete dsat_get_cur_umts_cgdcont() prototype
01/07/03   rsl     Removed dsat_qcpma_val, not being used anymore.
12/03/02   ar      Added vendor specific codes to dsat_cme_error_e_type
11/08/02   wx      Export dsat_qcdns_val for ps.
10/30/02   rsl     Made DSAT_CRC_CODE_VOICE common.
09/05/02   sb      ATCOP changes based on new Multi mode Data services design
07/13/02   atp     Added dsat_reg_rsp_fmter() for adding ability to register
                   response formating function.
06/27/02   ar      Added support for Quality of Service profile
04/17/02   rc      Removed FEATURE wrap FEATURE_DS_SOCKETS around qcdns 
                   variables. 
02/21/02   sjd     Changed init service mode parameter to CM system mode type.
08/28/01   sjd     Modifications to support new ATCOP VU structure.
08/10/01   sjd     Initial release to MSM5200 archives.

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"



#include "cm.h"
#include "dsm.h"
#include "sys.h"
#include "dsatapi.h"
#include "dsat_ext_api.h"
#include "rex.h"
#if ( defined( FEATURE_DATA_GCSD ) || defined( FEATURE_DATA_WCDMA_CS ) )
#include "dsucsdappif.h"
#endif /* FEATURE_DATA_GCSD || FEATURE_DATA_WCDMA_CS */

#ifdef FEATURE_DSAT_ETSI_DATA
#include "ps_iface_defs.h"
#include "ds_profile_3gpp.h"
#endif /* FEATURE_DSAT_ETSI_DATA */

#include "mmgsdilib_common.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*-------------------------------------------------------------------------
                Constants:
-------------------------------------------------------------------------*/
    
    /* ATCOP subtask signals registered with DS task */
#define DSAT_SUBTASK_SIGNALS \
            (DS_AT_MT_MSG_SIG | DS_AT_SMS_SIG | DS_AT_CM_MSG_SIG)

#ifdef FEATURE_MMGSDI_ATCSIM 
/* This command line size required for +CRSM command:
   AT+CRSM=<cmd>,<file_id>,<P1>,<P2>,<P3>,<data> 
   where <cmd> could be up to 3 digits, <file_id> up to 5, <P1..3> up to 3,
   <data> a quoted string up to 512,,<path_id> a quoted string up to 15
   w/ quotes and terminating NULL * w/ quotes and terminating NULL */
  #define DSAT_MAX_LINE_SIZE 560     /*  Maximum length of AT command line    */
                                /*  (including terminating null char)         */
#else /* FEATURE_MMGSDI_ATCSIM */
  #define DSAT_MAX_LINE_SIZE 260     /*  Maximum length of AT command line    */
                                /*  (including terminating null char)         */
#endif /* FEATURE_MMGSDI_ATCSIM  */

#define DSAT_MAX_CMD_SIZE   (20)  /* max length of the command name */

#ifdef FEATURE_DSAT_LARGE_ARGS
#define DSAT_MAX_ARG 44
#else
/* PDP related commands require maximum of 17 arguments. */
#define DSAT_MAX_ARG 17    /* Maximum number of args for compound AT command */
#endif /* FEATURE_DSAT_LARGE_ARGS */

#define MAX_LINE_SIZE  DSAT_MAX_LINE_SIZE
#define MAX_CMD_SIZE   DSAT_MAX_CMD_SIZE
#define MAX_ARG        DSAT_MAX_ARG

/*--------------------------------------------------------------------------
                Syntax Flags For the AT Command Parser 
--------------------------------------------------------------------------*/
#define NA    1                 /*  Name field found      */
#define EQ    2                 /*  <=> found             */
#define QU    4                 /*  <?> found             */
#define AR    8                 /*  Argument field found  */

typedef byte dsat_string_item_type;

/*---------------------------------------------------------------------------
    The numerical values in this "enum" are important:  
    The values for OK thru NO_ANSWER must match the numeric codes in EIA-602.                         
    The values for the cellular extention codes must begin with the 
      value MIN_CELL_RES (defined in dsati.h), and increase in sequence.            
    The values for the cellular error codes must being with the value 
      MIN_CELL_ERR (defined in dsati.h), and increase in sequence.

    No specific values are required for ETSI extended format of incoming
    indications, as they are only displayed as strings.
    
    The values for the internal codes must be negative, other than those
    used to identify the range of particular result code types.
---------------------------------------------------------------------------*/

typedef enum
{
  /*-------------------------------------------------------------------------
    Numbered per ITU-T V.25 ter
  -------------------------------------------------------------------------*/
  DSAT_OK             = 0,    /*  AT: OK            */
  DSAT_CONNECT        = 1,    /*  AT: CONNECT       */
  DSAT_RING           = 2,    /*  AT: RING          */
  DSAT_NO_CARRIER     = 3,    /*  AT: NO CARRIER    */
  DSAT_ERROR          = 4,    /*  AT: ERROR         */
  DSAT_NO_DIALTONE    = 6,    /*  AT: NO DIAL TONE  */
  DSAT_BUSY           = 7,    /*  AT: BUSY          */
  DSAT_NO_ANSWER      = 8,    /*  AT: NO ANSWER     */
  DSAT_DELAYED        = 9,   /* AT: Call Throttled */
  DSAT_MAX_BASIC      = 10,   /*  End of basic result codes                */  

  DSAT_CRC_CODE_VOICE = 31,  /* normal voice                       */

#ifdef FEATURE_DSAT_ETSI_MODE
  /*-------------------------------------------------------------------------
    +CRC Cellular Result Codes from 3GPP TS 27.007 6.11.  These are
    ETSI extended format of incoming call indications.  The standard
    does not specify any numeric value for these result codes as they
    are only displayed as stings.
  -------------------------------------------------------------------------*/
  DSAT_CRC_CODE_MIN,                 /* Start of ETSI CRC codes            */
  DSAT_CRC_CODE_ASYNC = DSAT_CRC_CODE_MIN, /* asynchronous transparent     */
  DSAT_CRC_CODE_SYNC,                /* synchronous transparent            */
  DSAT_CRC_CODE_REL_ASYNC,           /* asynchronous non-transparent       */
  DSAT_CRC_CODE_REL_SYNC,            /* synchronous non-transparent        */
  DSAT_CRC_CODE_FAX,                 /* facsimile                          */
  DSAT_CRC_CODE_VOICE_ASYNC,         /* voice followed by asynchronous 
                                        transparent data                   */
  DSAT_CRC_CODE_VOICE_SYNC,          /* voice followed by synchronous 
                                        transparent data                   */
  DSAT_CRC_CODE_VOICE_REL_ASYNC,     /* voice followed by asynchronous 
                                        non-transparent data               */
  DSAT_CRC_CODE_VOICE_REL_SYNC,      /* voice followed by synchronous 
                                        non-transparent data               */
  DSAT_CRC_CODE_ALT_VOICE_ASYNC,     /* alternating voice/asynchronous 
                                        transparent data, voice first      */
  DSAT_CRC_CODE_ALT_VOICE_SYNC,      /* alternating voice/synchronous 
                                        transparent data, voice first      */
  DSAT_CRC_CODE_ALT_VOICE_REL_ASYNC, /* alternating voice/asynchronous 
                                        non-transparent data, voice first  */
  DSAT_CRC_CODE_ALT_VOICE_REL_SYNC,  /* alternating voice/synchronous 
                                        non-transparent data, voice first  */
  DSAT_CRC_CODE_ALT_ASYNC_VOICE,     /* alternating voice/asynchronous 
                                        transparent data, data first       */
  DSAT_CRC_CODE_ALT_SYNC_VOICE,      /* alternating voice/synchronous 
                                        transparent data, data first       */
  DSAT_CRC_CODE_ALT_REL_ASYNC_VOICE, /* alternating voice/asynchronous 
                                        non-transparent data, data first   */
  DSAT_CRC_CODE_ALT_REL_SYNC_VOICE,  /* alternating voice/synchronous 
                                        non-transparent data, data first   */
  DSAT_CRC_CODE_ALT_VOICE_FAX,       /* alternating voice/fax, voice first */
  DSAT_CRC_CODE_ALT_FAX_VOICE,       /* alternating voice/fax, fax first   */
  DSAT_CRC_CODE_GPRS,                /* GPRS network request for PDP 
                                        context activation                 */
  DSAT_CRC_CODE_MAX,                 /* ATCOP internal use only - end of
                                        CRC result codes                   */
#endif /* FEATURE_DSAT_ETSI_MODE */

  /*-------------------------------------------------------------------------
     Not reported to TE
     Note that DSAT_CXT_ORIG is defined as -13 and not as -12  
     because some builds used -12 for other definition.
  -------------------------------------------------------------------------*/
  DSAT_NO_RESULT_CODE = -21,
  DSAT_SMS_PROMPT     = -20,   /* internal: Proxy SMS Prompt            */
  DSAT_SMS_EOM        = -19,   /* internal: Proxy SMS EOM               */
  DSAT_ATH_CMD        = -18,  /*  internal: hangup                      */
  DSAT_ASYNC_EVENT    = -17,  /*  internal: async event, not associated
                                            with a command              */
  DSAT_ASYNC_CMD      = -16,  /*  internal: async command in progress   */
  DSAT_CMD_ERR_RSP    = -15,  /*  internal: send cmd specific error
                                            rsp only and no result code */

  DSAT_NO_RSP         = -14,  /*  internal: send no response to cmd  */
  DSAT_CXT_ORIG       = -13,  /*  internal: originate w/ unknown cmd */

  DSAT_DO_QCOTC       = -11,  /*  internal: open traffic channel     */
  DSAT_DO_DM          = -10,  /*  internal: change to DM mode        */
  DSAT_DO_QCDAM       = -9,   /*  internal: analog dial for PCMCIA   */
  DSAT_DO_HARD_PARSE  = -8,   /*  internal: parse cmd w/o delimiter  */
  DSAT_DO_CHV         = -7,   /*  internal: cellular hang up voice   */
  DSAT_DO_CDV         = -6,   /*  internal: cellular dial voice      */
  DSAT_TEST           = -5,   /*  internal: remote test command      */
  DSAT_PARAM          = -4,   /*  internal: parameter out of range   */
  DSAT_UNREC          = -3,   /*  internal: unrecognized command     */
  DSAT_DO_ANS         = -2,   /*  internal: answer  (auto or ATA)    */
  DSAT_DO_ORIG        = -1    /*  internal: originate data call      */

} dsat_result_enum_type;

/* Internal Current Operating Mode Variable */
typedef enum
{
  DSAT_MODE_GSM,  /* GSM mode   */
  DSAT_MODE_WCDMA,/* WCDMA mode */
  DSAT_MODE_LTE,  /* LTE mode */
  DSAT_MODE_TDS,  /* TDS mode */
  DSAT_MODE_CDMA, /* CDMA mode  */
  DSAT_MODE_1XDO,
  DSAT_MODE_1XLTE,
  DSAT_MODE_MAX   /* Mode None  */
} dsati_mode_e_type;

/*Enum to identify which efs file type it is*/
typedef enum
{
  DSAT_FILE_STR_TYPE, /*EFS file type to identify if its a string*/
  DSAT_FILE_CMD_TYPE /*EFS file type to identify if its a command name*/
}dsat_efs_file_e_type;

/*Enum to identify special cases for reporting data to TE*/
typedef enum
{
 DSAT_EFS_PREFMODE_QUERY,
 DSAT_EFS_PREFMODE_TEST,
 DSAT_EFS_MRU_TEST
}dsat_efs_data_e_type;

/*===========================================================================
                 AT Command Processor Data Structures
===========================================================================*/

/*---------------------------------------------------------------------------
     The following enumerated type represents the states of ATCOP: command, 
     online data, and online command. The ME is in command state when the 
     transport layer is not in the ESTABLISHED state. When in command state
     or online command state, all AT commands received on Rm are processed.
     When the ME is in the online data state, the ME passes all data received
     on the Rm directly to the IWF.  
---------------------------------------------------------------------------*/

typedef enum
{
  BASIC_AT_CMD = 0,
  SREG_AT_CMD,
  EXT_AT_CMD,
  VENDOR_AT_CMD,
  NUM_AT_CMD_CATEGORIES
} dsat_cmd_category_enum_type;

typedef enum
{
  DSAT_RMSM_CGACT_CMD = 0 ,   /* +CGACT command response */
  DSAT_RMSM_QCDGEN_CMD = 1,   /* $QCDGEN command response */
  DSAT_RMSM_CGCMOD_CMD = 2,
  DSAT_RMSM_CMD_MAX =0xFF
}dsat_rmsm_cmd_type;

typedef enum
{
  DSAT_RMSM_INVALID_REASON = -1, /* Invalid reason */
  DSAT_RMSM_IFACE_NET_DOWN_REASON = 0,  /* ps_iface_net_down_reason_type valid */
  DSAT_RMSM_EXT_INFO_CODE = 1,
  DSAT_RMSM_REASON_MAX =0xFF
}dsat_rmsm_reason_type;

typedef struct dsat_rmsm_info_type
{
  dsat_result_enum_type result;
  dsat_rmsm_reason_type reason_type;
  union
  {
    ps_iface_net_down_reason_type  iface_net_down;/* Primary PS call down reason */
    ps_extended_info_code_enum_type ext_info_code;/* Secondary PS call down reason */
  }reason;
  void* user_info_ptr;
}dsat_rmsm_info_s_type;

/* CGEREP - Packet event report types */
typedef enum dsat_cgerep {
  DSAT_MS_INITIATED_PDP_REJECT         = 0,
  DSAT_NW_INITIATED_PDP_REACTIVATE     = 1,
  DSAT_NW_INITIATED_PDP_DEACTIVATE     = 2,
  DSAT_MS_INITIATED_PDP_DEACTIVATE     = 3,
  DSAT_NW_INITIATED_DETACH             = 4,
  DSAT_MS_INITIATED_DETACH             = 5,
  DSAT_NW_INITIATED_CHANGE_CLASS_C_CC  = 6,
  DSAT_NW_INITIATED_CHANGE_CLASS_C_CG  = 7,
  DSAT_MS_INITIATED_CHANGE_CLASS_B     = 8,
  DSAT_MS_INITIATED_CHANGE_CLASS_C_CC  = 9,
  DSAT_MS_INITIATED_CHANGE_CLASS_C_CG  = 10,
  DSAT_PDN_ACTIVATE                    = 11,
  DSAT_PDN_DEACTIVATE                  = 12,
  DSAT_DED_NW_DED_ACTIVATE             = 13,
  DSAT_MS_ACTIVATE                     = 14,
  DSAT_DED_NW_DEACTIVATE               = 15,
  DSAT_MS_DEACTIVATE                   = 16,
  DSAT_NW_MODIFY                       = 17,
  DSAT_MS_MODIFY                       = 18
} dsat_cgerep_e_type;

typedef struct
{
  uint32    cid;
  uint32    p_cid;
  uint8     change_reason;
  uint8     evt_type;
  sys_modem_as_id_e_type cm_subs_id;
}dsat_cgerep_evt_info_s_type;

/*---------------------------------------------------------------------------
  The following structure is the data type for tokens in the parsing of   
  the AT commands.

  working_at_line - Pointer to command line to be processed. 
  name            - The name of the AT command.
  op              - Syntax flags. Can be one of four valid values (NA, 
                    EQ, QU, AR) or an invalid value. 
  arg             - AT command arguments.
  args_found      - Keeps track of the number of AT command arguments 
                    found.
  end_of_line     - Pointer to end of data (last NULL character) placed in
                    working_at_line buffer.
  cmd_category    - Category of command (Basic, Extended, S-register,
                    Vendor Specific)
---------------------------------------------------------------------------*/
typedef struct
{
  byte *arg[MAX_ARG];
  byte *working_at_line;
  byte *name;
  byte *end_of_line;
  uint8 op;
  uint16 args_found;
  dsat_cmd_category_enum_type cmd_category;
} tokens_struct_type;

/*===========================================================================
                AT Command Response Generator Data Structures
===========================================================================*/

/*---------------------------------------------------------------------------
  The enum type for specifying the type of response data passed into the
  format response function.  Used to determine if header and/or trailer
  should be added to response.
---------------------------------------------------------------------------*/
typedef enum
{
  DSAT_START_OF_RSP,        /* Start of response - add header only        */
  DSAT_END_OF_RSP,          /* End of response - add trailer only         */
  DSAT_INTERMEDIATE_RSP,    /* Intermediate response - add no header or
                               trailer                                    */
  DSAT_COMPLETE_RSP,         /* Complete response - add header and trailer */
  DSAT_PROXY_RSP
} dsat_rsp_enum_type;

#ifdef FEATURE_DSAT_ETSI_MODE
/*---------------------------------------------------------------------------
  The enum type for Service reporting servs from 3GPP TS27.007 6.9
---------------------------------------------------------------------------*/
typedef enum
{
  DSAT_CR_SERVICE_ASYNC,     /* asynchronous transparent */
  DSAT_CR_SERVICE_SYNC,      /* synchronous transparent */
  DSAT_CR_SERVICE_REL_ASYNC, /* asynchronous non-transparent */
  DSAT_CR_SERVICE_REL_SYNC,  /* synchronous non-transparent */
  DSAT_CR_SERVICE_GPRS,      /* GPRS */
  DSAT_CR_SERVICE_MAX        /* ATCOP internal use only */
} dsat_cr_service_e_type;

#endif /* FEATURE_DSAT_ETSI_MODE */

/*---------------------------------------------------------------------------
  The enum type for V.42bis result code reporting from V.25ter 6.6.2
---------------------------------------------------------------------------*/
typedef enum
{
  DSAT_DR_MODE_NONE,         /* data compression not used */
  DSAT_DR_MODE_BOTH,         /* data compression both directions */
  DSAT_DR_MODE_RD,           /* data compression receive direction only */
  DSAT_DR_MODE_TD,           /* data compression transmit direction only */
  DSAT_DR_MODE_MAX           /* ATCOP internal use only */
} dsat_dr_mode_e_type;

/*---------------------------------------------------------------------------
  The enum type for +CME Error Codes from 3GPP TS27.007 9.2
---------------------------------------------------------------------------*/
typedef enum
{
  DSAT_CME_NO_ERROR              =  -1,  /* RESERVED */
  DSAT_CME_PHONE_FAILURE         =   0,  /* phone failure */
  DSAT_CME_NO_CONNECTION         =   1,  /* no connection to phone */
  DSAT_CME_ADAPTOR_LINK_RESERVED =   2,  /* phone adaptor link reserved */
  DSAT_CME_OP_NOT_ALLOWED        =   3,  /* operation not allowed */
  DSAT_CME_OP_NOT_SUPPORTED      =   4,  /* operation not supported */
  DSAT_CME_PH_SIM_PIN_REQUIRED   =   5,  /* PH-SIM PIN required */
  DSAT_CME_PH_FSIM_PIN_REQUIRED  =   6,  /* PH-FSIM PIN required */
  DSAT_CME_PH_FSIM_PUK_REQUIRED  =   7,  /* PH-FSIM PUK required */
  DSAT_CME_SIM_NOT_INSERTED      =  10,  /* SIM not inserted */
  DSAT_CME_SIM_PIN_REQUIRED      =  11,  /* SIM PIN required */
  DSAT_CME_SIM_PUK_REQUIRED      =  12,  /* SIM PUK required */
  DSAT_CME_SIM_FAILURE           =  13,  /* SIM failure */
  DSAT_CME_SIM_BUSY              =  14,  /* SIM busy */
  DSAT_CME_SIM_WRONG             =  15,  /* SIM wrong */
  DSAT_CME_INCORRECT_PASSWORD    =  16,  /* incorrect password */
  DSAT_CME_SIM_PIN2_REQUIRED     =  17,  /* SIM PIN2 required */
  DSAT_CME_SIM_PUK2_REQUIRED     =  18,  /* SIM PUK2 required */
  DSAT_CME_MEMORY_FULL           =  20,  /* memory full */
  DSAT_CME_INVALID_INDEX         =  21,  /* invalid index */
  DSAT_CME_NOT_FOUND             =  22,  /* not found */
  DSAT_CME_MEMORY_FAILURE        =  23,  /* memory failure */
  DSAT_CME_TEXT_STRING_TOO_LONG  =  24,  /* text string too long */
  DSAT_CME_INVALID_CHAR_IN_TEXT  =  25,  /* invalid characters in text string */
  DSAT_CME_DIAL_STRING_TOO_LONG  =  26,  /* dial string too long */
  DSAT_CME_INVALID_CHAR_IN_DIAL_STRING
    =  27,  /* invalid characters in dial string */
  DSAT_CME_NO_NETWORK_SERVICE    =  30,  /* no network service */
  DSAT_CME_NETWORK_TIMEOUT       =  31,  /* network timeout */
  DSAT_CME_NETWORK_NOT_ALLOWED   =  32,  /* network not allowed - emergency 
                                             calls only */
  DSAT_CME_NET_PERSON_PIN_REQUIRED= 40,  /* network personalization PIN 
                                             required */
  DSAT_CME_NET_PERSON_PUK_REQUIRED= 41,  /* network personalization PUK 
                                             required */
  DSAT_CME_NET_SUBSET_PERSON_PIN_REQUIRED
                                  =  42,  /* network subset personalization 
                                             PIN required */
  DSAT_CME_NET_SUBSET_PERSON_PUK_REQUIRED
                                  =  43,  /* network subset personalization PUK
                                             required */
  DSAT_CME_SP_PERSON_PIN_REQUIRED=  44,  /* service provider personalization 
                                             PIN required */
  DSAT_CME_SP_PERSON_PUK_REQUIRED=  45,  /* service provider personalization 
                                             PUK required */
  DSAT_CME_CORP_PERSON_PIN_REQUIRED
                                  =  46,  /* corporate personalization PIN 
                                             required */
  DSAT_CME_CORP_PERSON_PUK_REQUIRED
                                  =  47,  /* corporate personalization PUK
                                             required */
  DSAT_CME_EAP_NOT_SUPPORTED      = 49,    /*  EAP method not supported  */
  DSAT_CME_EAP_INVALID_PARAM      = 50,
  DSAT_CME_AUTH_PARAM_LENGTH_ERROR = 51,  /* Parameter length error for 
                                                 all Auth commands */
  DSAT_CME_MMGSDI_AUTH_ERROR       = 52,    /* Temporary error for all auth cmds */

  DSAT_CME_UNKNOWN               = 100,  /* unknown */
  DSAT_CME_ILLEGAL_MS            = 103,  /* Illegal Mem_Store */
  DSAT_CME_ILLEGAL_ME            = 106,  /* Illegal ME */
  DSAT_CME_GPRS_serv_not_allowed = 107,  /* GPRS services not allowed */
  DSAT_CME_PLMN_NOT_ALLOWED      = 111,  /* PLMN not allowed */
  DSAT_CME_LOCATION_NOT_ALLOWED  = 112,  /* Location area not allowed */
  DSAT_CME_ROAMING_NOT_ALLOWED   = 113,  /* Roaming not allowed in this 
                                             location area */
  DSAT_CME_SERV_OP_NOT_SUPPORTED = 132,  /* service option not supported */
  DSAT_CME_REQ_SERV_OP_NOT_SUBSCRIBED
                                  = 133,  /* requested service option not 
                                             subscribed */
  DSAT_CME_SERV_OP_TEMP_OUT_OF_ORDER
                                  = 134,  /* service option temporarily 
                                             out of order */
  DSAT_CME_unspecified_GPRS_error= 148,  /* unspecified GPRS error */
  DSAT_CME_PDP_AUTH_FAILURE      = 149,  /* PDP authentication failure */
  DSAT_CME_INVALID_MOBILE_CLASS  = 150,  /* invalid mobile class */
  
  /* Qualcomm specific error codes */
  DSAT_CME_SUPS_NETWORK_REJECTED = 257,  /* network rejected supserv request */
  DSAT_CME_SUPS_RETRY_OPERATION  = 258,  /* retry operation */
  DSAT_CME_SUPS_DFL_INVALID_NUMBER  = 259,  /* invalid deflected to number */
  DSAT_CME_SUPS_DFL_SELF_NUMBER  = 260,  /* deflected to own number */
  DSAT_CME_SUPS_UNKNOWN_NUMBER   = 261,  /* unknown subscriber */
  DSAT_CME_SUPS_SERVICE_UNAVAIL  = 262,  /* service not available */
  DSAT_CME_SUPS_UNKNOWN_CLASS    = 263,  /* unknown class */
  DSAT_CME_SUPS_UNKNOWN_NETMSG   = 264,  /* unknown network message */
  
#ifdef FEATURE_SECONDARY_PDP
  DSAT_CME_MIN_TFT_PER_ADDR      = 273,  /* Minimum TFT per PDP address error */
  DSAT_CME_TFT_EPI_DUPLICATE     = 274,  /* Duplicate TFT eval prec index */
  DSAT_CME_INVALID_PARAM_COMB    = 275,  /* Invalid TFT param combination */
#endif /* FEATURE_SECONDARY_PDP */

  DSAT_CME_CALL_INDEX_ERROR      = 320,
  DSAT_CME_CALL_STATE_ERROR      = 321,
  DSAT_CME_SYS_STATE_ERROR       = 322,
  DSAT_CME_PARAMETER_ERROR       = 323,

  DSAT_CME_MAX                           /* atcop internal only */
} dsat_cme_error_e_type;

#ifdef FEATURE_DATA_GCSD_FAX
/* Callback prototype for fax protocol stack */
typedef void (*dsat_fps_callback_type) (dsat_result_enum_type result);

#endif  /* FEATURE_DATA_GCSD_FAX */
/*---------------------------------------------------------------------------
  The following function pointer type provides mode specific AT commmand
  response routing functionality.  It defines a function type that routes
  responses based on operating mode and AT command state.

  rsp_ptr               - Pointer to response buffer to send
  at_state              - AT command state: command, online data, or online
                          command
  rearm_autodetect      - Flag to indicate if SIO autodetect mode should
                          rearmed when response is sent by SIO
---------------------------------------------------------------------------*/
typedef void (*dsat_routing_func_type)
(
  dsm_item_type *rsp_ptr,               
  const dsat_mode_enum_type at_state,
  const boolean rearm_autodetect         
);

typedef void (*dsat_fmting_func_type)
(
  dsm_item_type **in_ptr_ptr,         /* Raw response  in chained
                                         buffers                           */
  dsat_rsp_enum_type type_of_rsp,     /* Type of response data in
                                         in_ptr_ptr                        */
  boolean is_result_code,             /* Flag to indicate if response is a
                                         result code or information
                                         response                          */
  dsm_item_type **out_ptr_ptr,        /* Location to store formatted
                                         response, output will NOT be NULL
                                         terminated                        */
  dsat_result_enum_type result        /* Result code                       */
);

/* Multi Mode Support */

/* Enum for type of call */
typedef enum
{
  DSAT_DEFAULT_DIAL_TYPE = 0, /* Circuit Call */
  DSAT_PKT_DIAL_TYPE = 1,     /* Packet Call  */
  DSAT_MAX_DIAL_TYPE
}dsat_dial_str_enum_type;

/* Call abort callback */
typedef void (*dsat_call_abort_cb_type) ( void );

/* AT Proxy Call abort callback */
typedef void (*dsat_proxy_call_abort_cb_type) ( dsat_result_enum_type result_code);

#ifdef FEATURE_DATA_TE_MT_PDP
typedef struct
{
  uint8     number_of_profiles; /* number of profile numbers entered */
  uint16    profile_number[DSAT_MAX_ALLOWED_PROFILES]; 
                                /* profile numbers */
} dsat_pdp_answer_data_type;

/* Incoming MT PDP call callback */
typedef dsat_call_abort_cb_type (*dsat_pdp_incom_answer_cb_type) 
  ( dsat_pdp_answer_data_type * answer_cb_data );

#endif /* FEATURE_DATA_TE_MT_PDP */

/* Incoming call callback */
typedef dsat_call_abort_cb_type (*dsat_incom_answer_cb_type) ( void );

/* Call hangup callback */
typedef void (*dsat_call_hangup_cb_type) ( void );

/* ATZ callback */
typedef void (*dsat_atz_cb_type) ( void );

/* enum that describes what type of dial modifier is present 
   in the dial string
*/
typedef enum
{
  DSAT_CID_DIAL_MODIFIER    = 0x0001,
  DSAT_CLI_DIAL_MODIFIER    = 0x0002,
  DSAT_CUG_DIAL_MODIFIER    = 0x0004,
  DSAT_CLI_AND_CUG_MODIFIER = 0x0008
} dsat_dial_modifier_e_type;

/* CLI Dial modifier */
typedef struct
{
  boolean                       cli_enabled;
} dsat_cli_modifier_type;

/* CUG Dial modifier */
typedef struct
{
  boolean                       cug_enabled;
  boolean                       cug_index_enabled;
  uint8                         cug_index_val;
  boolean                       suppress_pref_cug;
  boolean                       suppress_outgoing_access;
} dsat_cug_modifier_type;

/* CID Dial modifier */
typedef struct
{
  uint8                         cid_val;
} dsat_cid_modifier_type;

/* Structure that is sent to the mode specific call handlers */
typedef struct 
{
  boolean modifier_present;
  dsat_dial_modifier_e_type modifier;
  struct
  {
    dsat_cli_modifier_type  cli;
    dsat_cug_modifier_type  cug;
    dsat_cid_modifier_type  cid;
   } modifier_info;
} dsat_dial_modifier_info_type;

/* Call Handler prototype used by ATD to make a call */
typedef dsat_call_abort_cb_type (*dsat_dial_type)
(
  const byte                    *dial_string,    /* dial string         */
  boolean                       digit_mode,      /* non-digit or digits */
  dsat_dial_modifier_info_type  *modifiers,      /* dial modifier info   */
  sys_modem_as_id_e_type        subs_id          /* subs_id */
);


typedef struct
{
  uint32 data_rate;   /* Radio zone speed in bytes*/
  uint32 ul_rate;    /* Uplink data rate in bytes*/
  uint32 dl_rate;    /* Downlink data rate in bytes*/
#ifdef FEATURE_DSAT_ETSI_DATA
  byte   apn[DS_PROFILE_3GPP_MAX_APN_STRING_LEN+1];  /* APN */
#endif /* FEATURE_DSAT_ETSI_DATA */
}dsat_ps_call_s_type;

typedef struct
{
  uint32 waiur;        /* Air interface data rate */
  uint32 fnur;         /* Fixed network data rate */
  uint8  name;         /* Name indicator          */
  uint8  conn_elem;    /* Connection element      */
}dsat_cs_call_s_type;

typedef struct
{
  uint32 data_rate;   /* Radio zone speed in bytes */
  uint32 ul_rate;    /* Uplink data rate in bytes  */
  uint32 dl_rate;    /* Downlink data rate in bytes*/
}dsat_1x_call_s_type;

typedef enum
{
  DSAT_CALL_TYPE_NONE,     /* Internal use only */
  DSAT_CALL_TYPE_PS ,      /* PS data call */
  DSAT_CALL_TYPE_CS,       /* CS data call */
  DSAT_CALL_TYPE_1X,       /* 1x data call */
  DSAT_CALL_TYPE_MAX
}dsat_call_e_type;

typedef enum
{
  DSAT_COMM_PROT_NONE,
  DSAT_COMM_PROT_PPPOVERUD,  /* Communication Protocol PPPoverUD */
  DSAT_COMM_PROT_AV32K,      /* Communication Protocol AV32K */
  DSAT_COMM_PROT_AV64K,      /* Communication Protocol AV64K */
  DSAT_COMM_PROT_PACKET     /* Communication Protocol PACKET */
}dsat_comm_prot_e_type;

typedef struct 
{
  dsat_call_e_type call_type;        /* 1x, CS or PS call */ 
  dsat_comm_prot_e_type comm_prot;   /* Communication Protocol*/
  union
  {
    dsat_ps_call_s_type  ps_call;    /* UMTS PS call details */
    dsat_cs_call_s_type  cs_call;    /* UMTS CS call details */
    dsat_1x_call_s_type  cdma_call;  /* 1x call details */
  }rate;
}dsat_connect_rate_s_type;

/* Call mode enumeration */
/* Used to flag special response handling cases */
typedef enum
{
  DSAT_CALL_MODE_NONE   = 0x0000,
  DSAT_CALL_MODE_FAX    = 0x0001
} dsat_call_mode_e_type;

#define DSAT_IS_CALL_MODE( mode )  ((uint16)dsat_rsp_call_mode & (uint16)mode)
#define DSAT_SET_CALL_MODE( mode )  (dsat_rsp_call_mode |= mode)
#define DSAT_CLEAR_CALL_MODE( mode )  (dsat_rsp_call_mode &= ~mode)

/*---------------------------------------------------------------------------
     Specifies dial character validation result
---------------------------------------------------------------------------*/  
typedef enum
{
  DSAT_DIAL_EMPTY,          /*  Empty                    */
  DSAT_DIAL_DIGIT,          /*  Digits                   */
  DSAT_DIAL_ASCII,          /*  ASCII chars              */
  DSAT_DIAL_ASCII_ABC,      /*  ASCII A,B, or C          */
  DSAT_DIAL_ASCII_D,        /*  ASCII D                  */
  DSAT_DIAL_ASCII_E,        /*  ASCII E                  */
  DSAT_DIAL_ALLOWED,        /*  OTHER ALLOWED CHARACTERS */
  DSAT_DIAL_SEMICOLON,      /*  SEMICOLON                */
  DSAT_DIAL_STAR,           /*  ASTERIX                  */
  DSAT_DIAL_POUND,          /*  POUND                    */
  DSAT_DIAL_PLUS,           /*  PLUS                     */
  DSAT_DIAL_STRIP,          /*  Stripped chars           */
  DSAT_DIAL_IGNORE,         /*  Insignificant chars      */
  DSAT_DIAL_ERROR,          /*  has illegal chars        */
  DSAT_DIAL_MAX             /*  Internal use only        */
} dsat_dial_val_e_type;

/*===========================================================================
      Definitions of the parameters set by the AT commands
===========================================================================*/

/*---------------------------------------------------------------------------
  AT parameters common to all modes of operation.
---------------------------------------------------------------------------*/
typedef union 
{
  dsat_string_item_type * string_item;
  dsat_num_item_type    num_item;
} dsat_mixed_param_val_type;
#ifdef FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS 
  /*echo control for portwise for ATE command */
  typedef enum
  {
    DSAT_MODEM_PORT_NONE = -1,
    DSAT_MODEM_PORT_1 = 0,
    DSAT_MODEM_PORT_2,
    DSAT_MODEM_PORT_MAX
  } dsat_modem_port_e_type;
#endif /*FEATURE_ATCOP_ECHO_CTL_MULTI_PORTS*/

extern dsat_call_mode_e_type dsat_rsp_call_mode;

/*---------------------------------------------------------------------------
  AT parameters specific to ETSI mode of operation.
---------------------------------------------------------------------------*/
#ifdef FEATURE_DSAT_ETSI_MODE
extern dsat_num_item_type dsat_cbst_export_val[];
#endif /* FEATURE_DSAT_ETSI_MODE */

#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)
#define MAX_CRLP_VERSIONS 3
#define MAX_CRLP_PARAMS   4

/*This macro help to find index in CRLP val.treadted as singal dimention array*/
#define CRLP_SIZE(row ,column,ix)\
{\
ix = row * MAX_CRLP_PARAMS + column ;\
}

/* Default +CV120 value from 3GPP 27.007, using parameter index as bit
 * position */
#define DSAT_CV120_DEFAULT_VALUE  0x07
typedef enum dsat_cv120_pos_e
{
  DSAT_CV120_RAH_POS       = 0,
  DSAT_CV120_MFM_POS       = 1,
  DSAT_CV120_MODE_POS      = 2,
  DSAT_CV120_LLINEG_POS    = 3,
  DSAT_CV120_ASSIGN_POS    = 4,
  DSAT_CV120_NEGTYPE_POS   = 5,
  DSAT_CV120_MAX_POS
} dsat_cv120_pos_e_type;
extern dsat_num_item_type dsat_cv120_export_val;
#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */

#ifdef FEATURE_DATA_GCSD_FAX
extern const dsat_string_item_type dsat_fth_valstr [][8];
extern const dsat_string_item_type dsat_frh_valstr [][8];
extern const dsat_string_item_type dsat_ftm_valstr [][8];

extern const dsat_string_item_type dsat_frm_valstr [][8];

#endif  /* FEATURE_DATA_GCSD_FAX */

extern dsat_num_item_type dsat_es_export_val[3];
extern dsat_num_item_type dsat_esa_export_val[8];

#ifdef FEATURE_DATA_UCSD_SCUDIF_TEST
extern uint8 dsat_qcscftest_exp_val[2];
#define DSAT_QCSCFTEST_MO_INDEX 0
#define DSAT_QCSCFTEST_MT_INDEX 1
#endif /* FEATURE_DATA_UCSD_SCUDIF_TEST */

typedef enum dsatdl_cmd_e
{
  DSAT_DL_CMD_LOAD,
  DSAT_DL_CMD_UNLOAD,
  DSAT_DL_CMD_BLOCK_UNLOAD,
  DSAT_DL_CMD_UNBLOCK_UNLOAD
} dsatdl_cmd_e_type;

typedef enum dsatdl_status_e
{
  DSAT_DL_STATUS_UNLOADED,
  DSAT_DL_STATUS_LOADED,
  DSAT_DL_STATUS_ERROR
} dsatdl_status_e_type;

#ifdef FEATURE_DSAT_DYNAMIC_LOADING
#define DSAT_DL_CMD_HANDLER(cmd) dsatdl_cmd_handler(cmd)
#define DSAT_DL_GET_STATUS()     dsatdl_get_status()
#else
#define DSAT_DL_CMD_HANDLER(cmd)
#define DSAT_DL_GET_STATUS()     DSAT_DL_STATUS_LOADED
#endif /* FEATURE_DSAT_DYNAMIC_LOADING */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSAT_PROCESS_SIO_COMMAND

DESCRIPTION
  This function builds the AT command line from data obtained from the 
  watermark item passed in. The command line begins with the "AT"
  characters (either case or mixed case, but with nothing in between), 
  and is terminated with the end of line character stored in modem 
  register S3.  The backspace character (in modem register S5) is processed.

  When a complete line has been accumulated, ds_atcop( ) is called.
  The line will have been edited per the backspace (above), and the AT
  removed.  The end of line character will have been replaced with the null
  character, '\0'.  Any spaces will still be present. The eighth bit of all 
  characters will have been set to zero.

  If the character combination, "A/" or "a/" is encountered while looking
  for the beginning of a command line, the previous command line, if any,
  will be immediately passed to ds_atcop( ).

  If the command line buffer overflows, the data will be discarded and
  an error flag set.  The error will be reported when the end of line
  character in S3 is encountered.

  Input data will be echoed to the serial port.  Data is echoed at the end
  of each input data item, when a small data item (used for echo output) is
  filled, or when the S3 character is found, or the buffer overflows,
  whichever comes first.  The echo is controlled by the 'ATE' flag.

DEPENDENCIES
  None

RETURN VALUE
  Abort call result; TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  Buffer accumulates data until a full line is found.

  See also ds_atcop which has many side effects.

===========================================================================*/
extern boolean dsat_process_sio_command
(
  dsat_mode_enum_type at_state,     /* Command, online data, or online
                                           command state */
  dsm_watermark_type *wm_item_ptr       /* Pointer to watermark item to
                                           retrieve command line data from */
);


/*===========================================================================

FUNCTION  DSAT_INIT 

DESCRIPTION
  Initializes ATCOP queues and internal variables, as well as all AT
  parameters to their default values.

DEPENDENCIES
  This function must be called once on data services task startup.

RETURN VALUE
  Signal mask defining REX signals the AT command processor is waiting
  for.

SIDE EFFECTS
  Initializes ATCOP internal variables and queues.

===========================================================================*/
extern rex_sigs_type dsat_init( void );


/*===========================================================================

FUNCTION DSAT_INIT_SERVICE_MODE

DESCRIPTION
  Initializes the AT command processor to new operating mode. AT
  command processing functions are selected from table based on
  new operating mode. This setting is for a particular subscription.

DEPENDENCIES
  Must be called each time operating service mode changes.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsat_init_service_mode
(
  sys_modem_as_id_e_type subs_id,       /* Subscription id */
  sys_sys_mode_e_type    service_mode1, /* Service mode - GSM, WCDMA, CDMA ... */
  sys_sys_mode_e_type    service_mode2  /* Service mode - GSM, WCDMA, CDMA ... */
);

/*===========================================================================


FUNCTION DSAT_NV_SYNC

DESCRIPTION
  This function initializes from NV the ATCOP defaults for
  those parameters stored in NV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsat_nv_sync( void );

#ifdef FEATURE_DSAT_MDM_FUSION
/*===========================================================================

FUNCTION DSAT_PROCESS_PROXY_RESPONSE

DESCRIPTION

  This function process the data received from the other Processor and packs 
  the final response and sends it out. 

DEPENDENCIES
  None

RETURN VALUE
  Abort call result; TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  Buffer accumulates data until a full line is found.

  See also ds_atcop which has many side effects.

===========================================================================*/
extern void dsat_process_proxy_response
(
  dsm_watermark_type *wm_item_ptr       /* Pointer to watermark item to
                                           retrieve command line data from */
);

/*===========================================================================

FUNCTION DSAT_PROCESS_PROXY_URC

DESCRIPTION
  This functions receives the URC from Proxy interface

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_process_proxy_urc
(
  dsm_watermark_type *wm_item_ptr       /* Pointer to watermark item to
                                           retrieve command line data from */
);
#endif /* FEATURE_DSAT_MDM_FUSION */

/*===========================================================================

FUNCTION DSAT_REG_RSP_ROUTER

DESCRIPTION
  This function registers a function that provides AT response routing
  based on operating service mode and AT command state: command mode,
  online data, or online command mode.  Registered function routes AT
  command response to either SIO or PS protocol stack.  

  If service type is IS-707 related and AT operating mode is online data
  mode, responses are routed to PS protocol stack, otherwise they are
  routed to SIO.  This provides support for IS-707 command reflection in
  future releases and makes it independent of ATCOP operation.  If no
  routing function is registered, response is sent directly to SIO.

  The response routing function may be deregistered by calling this
  function with the function pointer equal to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsat_reg_rsp_router
(
  dsat_routing_func_type func_ptr   /* Function providing mode specific
                                       AT command response routing or
                                       NULL to deregister routing
                                       function. */
);


/*===========================================================================

FUNCTION DSAT_REG_RSP_FMTER

DESCRIPTION
  This function registers a function that provides AT response formating
  based on operating service mode and AT command state: command mode,
  online data, or online command mode.

  If service type is IS-707 related and AT operating mode is online data
  mode, responses to reflected AT commands have to be formated with
  EIA-617 codes. So the default fmt_response function cannot be used and
  the 707 mode-specific handler should use this function to register a
  function that does appropriate formatting.

  The response formating function may be deregistered by calling this
  function with the function pointer equal to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsat_reg_rsp_fmter
(
  dsat_fmting_func_type func_ptr    /* Function providing mode specific
                                       AT command response formating or
                                       NULL to deregister function         */
);


/*===========================================================================

FUNCTION DSAT_FMT_RESPONSE

DESCRIPTION
  This function formats the response according whether or not
  the mode is verbose and whether or not the response is a result
  code or informational text.  

  The dsm item passed in as in_ptr_ptr can contain chained dsm items with
  the dsm "used" value defining the length of each item.  These chained
  items are assumed to contain a NULL terminated string, which will be
  formatted up until the dsm "used" value is reached for each buffer
  or a NULL character is encountered, whichever occurs first.  *in_ptr_ptr
  will be returned as NULL to indicate the dsm input items have been freed.

  The type of data passed in the dsm item pointed to by in_ptr_ptr is
  specified by the type_of_rsp parameter.  This controls response
  formatting and identifies if header, trailer, both header and trailer,
  or no header and trailer should be added to the response.  This allows
  formatting of partial responses, as oppossed to only supporting
  formatting of complete responses, to reduce DSM buffer usage. See enum
  type definition for details.

  The dsm item passed in as out_ptr_ptr is filled with the formatted
  response.  If there is not enough room in this dsm item, the full item
  is queued to send as a command line response and additional dsm
  items are allocated, with out_ptr_ptr pointing to the last item
  allocated.  out_ptr_ptr may be returned as NULL, if no more dsm
  buffers are available. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsat_fmt_response
(
  dsm_item_type **in_ptr_ptr,         /* Response to format in chained
                                         buffers                           */
  dsat_rsp_enum_type type_of_rsp,     /* Type of response data in 
                                         in_ptr_ptr                        */
  boolean result_code,                /* Flag to indicate if response is a
                                         result code or information
                                         response                          */
  dsm_item_type **out_ptr_ptr         /* Location to store formatted
                                         response, output will NOT be NULL
                                         terminated                        */
);


/*===========================================================================
FUNCTION  DSAT_SEND_INTERMEDIATE_RESULT_CODE
DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0. Also, modifies ATCOP internal state variables.  This function
  should be used when the result code to be sent is an intermediate code.
  That is, one that occurs before the final AT response is sent when 
  the call terminates such as with a T.30 FAX call which sends multiple
  AT result codes throughout the duration of the call.
  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.
  Extended cellular result codes are returned if AT+CRC is 1.
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  May modify ATCOP internal state variables and SIO preprocessor mode.
===========================================================================*/
extern void dsat_send_intermediate_result_code
(
  dsat_result_enum_type result_code         /* Command result code */
);


/*===========================================================================

FUNCTION  DSAT_SEND_RESULT_CODE

DESCRIPTION
  Sends ITU-T V.25 ter basic code or mode specific extended result code
  to TE or PS protocol stack based on response routing provided if ATQ
  is 0. Also, modifies ATCOP internal state variables.

  If AT+CRC is 0, only the basic result codes are returned.
  The extended RING codes are translated to the basic code.
  Basic result codes are returned if ATQ is 0.

  Extended cellular result codes are returned if AT+CRC is 1.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  May modify ATCOP internal state variables and SIO preprocessor mode.
===========================================================================*/
extern void dsat_send_result_code
(
  dsat_result_enum_type result_code         /* Command result code */
);

#if defined(FEATURE_GSM)

/*===========================================================================

FUNCTION  DSAT_SEND_DR_RESULT

DESCRIPTION
  Sends intermediate data compression reporting result code to TE or
  PS protocol stack based on response routing provided by response
  router.

  Should be called from mode specific protocol stack when compression
  negotiation completed and before connect result code sent.
 
DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
===========================================================================*/
extern void dsat_send_dr_result
(
  dsat_dr_mode_e_type compression
);
#endif /* defined(FEATURE_GSM) */


/*===========================================================================

FUNCTION  DSAT_SEND_CME_ERROR 

DESCRIPTION
  Sends Mobile Equipment error to TE or PS protocol
  stack based on response routing.  The Mobile Equipment error 
  code and error text are defined in Section 9.2 of 3GPP TS 27.007

  If AT+CMEE is 0, +CME ERROR : <err> result code is disabled
                   ERROR is used
  If AT+CMEE is 1, +CME ERROR : <err> result code is enabled
                   to used numeric err value
  If AT+CMEE is 2, +CME ERROR : <err> result code is enabled
                   to used verbose text err value

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern dsat_result_enum_type dsat_send_cme_error
(
  dsat_cme_error_e_type error_code
);

/*===========================================================================

FUNCTION DSAT_IS_STATUS

DESCRIPTION
  This function returns TRUE if the argument string begins with
  the numeric character (1-8)  or the word corresponding to the
  result specified by the second arg.  Trailing characters are ignored.
  Other wise if returns FALSE.

DEPENDENCIES
  None

RETURN VALUE
  See description.

SIDE EFFECTS
  None

===========================================================================*/
extern boolean dsat_is_status
(
  byte *a_ptr,
  dsat_result_enum_type result
);

/*===========================================================================

FUNCTION DSAT_REPORT_RATE_ON_CONNECT

DESCRIPTION
  This function is called by CS/PS modules to write the value to the current 
  rate variable dsat_report_rate_val. If dsat_x_val, set by the ATX command,
  is greater than 0.this;rate will be reported with the next CONNECT result.
  The dsat_report_rate_val variable will be reset to 0 on the
  next call to dsat_sio_is_free() and also once the CONNECT is reported to TE.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Modifies the value of the dsat_report_rate_val.
===========================================================================*/
extern void dsat_report_rate_on_connect
(
       dsat_connect_rate_s_type connect_rate   /* Rate to report with next CONNECT */
);

#if defined (FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
/*===========================================================================

FUNCTION DSAT_FLUSH_SMS_INDICATIONS

DESCRIPTION
  Flushes buffer of SMS incoming message indications and sends indications
  to TE. Indications are buffered and sent based on AT parameter setting

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  
===========================================================================*/
void dsat_flush_sms_indications
(
 void
);
#endif /* defined (FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */


/*===========================================================================

FUNCTION  DSAT_TO_SIO_LOWATER_CB

DESCRIPTION
  This is the call back function when ds_to_sio watermark hits low.

  It sets dsati_tx_wm_high flag to be FALSE and inform ds task with
  DS_AT_TO_SIO_LOW_CMD cmd.

DEPENDENCIES
  None

RETURN VALUE
  none.

SIDE EFFECTS
  None
===========================================================================*/
void dsat_to_sio_lowater_cb (
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
);


/*===========================================================================

FUNCTION  DSAT_TO_SIO_HIWATER_CB

DESCRIPTION
  This is the call back function when ds_to_sio watermark hits high.

  It sets dsati_tx_wm_high flag to be TRUE.

DEPENDENCIES
  None

RETURN VALUE
  none.

SIDE EFFECTS
  None
===========================================================================*/
void dsat_to_sio_hiwater_cb (
  struct dsm_watermark_type_s *wm_ptr, 
  void *user_data_ptr
);


/*===========================================================================

FUNCTION DSAT_REG_ATZ_CB

DESCRIPTION
  This function is used to register a call back function for ATZ.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_reg_atz_cb
(
  dsat_atz_cb_type atz_cb                                 /* ATZ call back */
);

/*===========================================================================

FUNCTION DSAT_REGISTER_HANDLERS

DESCRIPTION
  This function is used to register call back functions.
  It is used to register call back funtions for ATA and ATH.

  The call back handler can be NULL. If it is not an incoming call, 
  then ATA handler will be NULL. 

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK :  if successfully registered.
  DSAT_ERROR: if trying to register when +CGAUTO=2(only in ETSI mode)
  

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsat_register_handlers
(
  dsat_incom_answer_cb_type  call_answer_cb, /* Answer call back */
  dsat_call_hangup_cb_type call_hangup_cb    /* Hangup call back */
);

#ifdef FEATURE_DATA_TE_MT_PDP
/*===========================================================================

FUNCTION DSAT_REGISTER_PDP_HANDLERS

DESCRIPTION
  This function is used to register call back functions.
  It is used to register call back funtions for ATA ,+CGANS and ATH
  for MT PDP calls.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_OK :  if successfully registered.
  DSAT_ERROR: if trying to register when +CGAUTO=1

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type  dsat_register_pdp_handlers
(
  dsat_pdp_incom_answer_cb_type pdp_call_answer_cb, /* Answer call back */
  dsat_call_hangup_cb_type call_hangup_cb           /* Hangup call back */
);

/*===========================================================================

FUNCTION DSAT_DEREGISTER_PDP_HANDLERS

DESCRIPTION
  This function is used to deregister the MT_PDP call back functions.
  It resets all the MT PDP registered handlers to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_deregister_pdp_handlers ( void );

#endif /* FEATURE_DATA_TE_MT_PDP */
/*===========================================================================

FUNCTION DSAT_DEREGISTER_HANDLERS

DESCRIPTION
  This function is used to deregister the call back functions.
  It resets all the registered handlers to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_deregister_handlers ( void );

/*===========================================================================

FUNCTION DSAT_INIT_CB_HANDLERS

DESCRIPTION
  Initialize all the registered call handlers to NULL
  Called from dsat_init() after every reset.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_init_cb_handlers(void);


/*===========================================================================

FUNCTION DSAT_POWERUP_INIT

DESCRIPTION
  This funtion performs power up initialization of ATCoP varaibles.
  This typically would be run in RcINIT worker thread at Bootup. 
DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_powerup_init(void);

/*===========================================================================

FUNCTION DSAT_PROC_DIAL_STR

DESCRIPTION
  Parses a dial string, looking for allowed non-digits, and
  illegal non-digits.  Digits, and allowed non-digits are copied
  to the output.  Illegal characters terminate processing.  All
  other characters are ignore.  Upper and lower case are treated
  as being the same.

  Since the output will be no longer than the input, the output
  string may be the same as the input string.

DEPENDENCIES
  None

RETURN VALUE
  Returns enum value indicating dial string content or error

SIDE EFFECTS
  None

===========================================================================*/
dsat_dial_val_e_type dsat_proc_dial_str
(
  const byte * in_ptr,          /*  Input string, null terminated   */
  byte * out_ptr                /*  Output string, null terminated  */
);
#ifdef FEATURE_DATA_IS707
/*===========================================================================
FUNCTION  DSAT_INIT_DIAL_TABLE

DESCRIPTION
  This function is used by CDMA targets for initializing
  the dial string table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_init_dial_table
(
   void 
);
#endif /* FEATURE_DATA_IS707 */

/*===========================================================================

FUNCTION DSAT_CHANGE_SIO_PARAMS

DESCRIPTION
  This function is called by other modules to write the value to the current 
  baud rate variable, dsat_ipr_val, and set the flow control method for the.
  specified port. The baud rate change is also written to the NV.

  The third parameter in this function is a boolean type. It informs 
  the function if the Baud rate should be changed NOW or later (later
  implies, after sending acknowledgement). This is needed because, when 
  UI changes the baud rate, the baud rate should take effect immediately. 
  But when an AT command changes the baud rate, the baud rate should be 
  changed after an "OK" is sent to the TE2. 

DEPENDENCIES
  This code does not check for auto baud enabled or not. So, it is the 
  responsibility of the calling function to ascertain that if 
  FEATURE_AUTOBAUD is not enabled then value 0 should not be passed.

  This code can execute in non-DS task context.

RETURN VALUE
  None

SIDE EFFECTS
  Modifies the current values of IPR and QCTER and the corresponding
  value ds_default_baud in the NV.    

===========================================================================*/
void dsat_change_sio_params
(
    sio_port_id_type port_id,                      /* SIO port_id            */
    sio_flow_ctl_type tx_flow,                    /* TX flow control method */
    sio_flow_ctl_type rx_flow,                    /* RX flow control method */
    sio_bitrate_type new_baud_rate,               /* Requested baud rate    */
    boolean change_baud_now                       /* Immediate or post ack  */
);

#ifdef FEATURE_DSAT_ETSI_MODE
/*===========================================================================

FUNCTION DSAT_GET_IMEI

DESCRIPTION
  This function queries the IMEI value in NV and
  decodes it into an ASCII string.  The result is
  sent back to the caller in the passed buffer pointer.

DEPENDENCIES
  NV task must be available

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsat_get_imei
(
  byte * rb_ptr,                  /* Pointer to return buffer */
  dsat_num_item_type  len          /*Buffer max len*/
);
#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)

/*===========================================================================

FUNCTION DSAT_GET_RLP_PARAMS

DESCRIPTION
  This function populates the passed structure of parameters which
  control the circuit-switched non-transparent RLP.  It is used by the
  UCSD API to query the +CRLP and +DS command paramaters.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of validation.
  possible values:
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsat_get_rlp_params
(
  ds_ucsd_rlp_sets_type  *rlp_params_ptr           /* RLP parameter values */
);


/*===========================================================================

FUNCTION DSAT_CHANGE_RLP_PARAMS

DESCRIPTION
  This function updates the paramater sets which control the
  circuit-switched non-transparent RLP.  It is used by the UCSD API to
  modify the +CRLP commands paramaters.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of validation.
  possible values:
    DSAT_ERROR : if there was any problem
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
extern dsat_result_enum_type dsat_change_rlp_params
(
  const ds_ucsd_nt_info_type  *rlp_params_ptr      /* RLP parameter values */
);
#endif /* defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS) */
#ifdef FEATURE_DATA_TE_MT_PDP
/*===========================================================================

FUNCTION DSATETSIPKT_GET_ACCESS_CONTROL

DESCRIPTION
 This function gives the access control status for MT PDP.

DEPENDENCIES
  None

RETURN VALUE
  DSAT_PDP_ACCESS_CONTROL_NONE
  DSAT_PDP_ACCESS_CONTROL_REJECT
  DSAT_PDP_ACCESS_CONTROL_PERMISSION

SIDE EFFECTS
  None

===========================================================================*/
extern dsat_pdp_access_control_e_type dsatetsipkt_get_access_control(void);
#endif /* defined(FEATURE_DATA_TE_MT_PDP) */

#endif /* FEATURE_DSAT_ETSI_MODE */

#ifdef FEATURE_DATA_IS707

/*===========================================================================

FUNCTION  DSAT_INIT_FOR_JCDMA 

DESCRIPTION
  Initializes ATCOP for JCDMA mode to default value, 
  if it is not a supported value.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void dsat_init_for_jcdma
(
  void
);
#endif /* FEATURE_DATA_IS707 */

/*===========================================================================

FUNCTION DSAT_RMSM_DONE_HANDLER

DESCRIPTION
  This function is called by Mode Handler when commands processing
  is complete.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
======================================================================*/
void dsat_rmsm_done_handler
(
  dsat_rmsm_cmd_type            dsat_rmsm_cmd,
  dsat_rmsm_info_s_type         dsat_rmsm_info
);

/*===========================================================================

FUNCTION DSAT_RMSM_PKT_EVENT_RPT_HANDLER

DESCRIPTION
  This function is called by RMSM AT Handler when Packet related events
  needs to be reported.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
======================================================================*/
void dsat_rmsm_pkt_event_rpt_handler
(
  dsat_cgerep_e_type            evt_type,
  dsat_cgerep_evt_info_s_type   *evt_info_p
);


/*==============================================================================
FUNCTION  DSAT_MMGSDI_CLIENT_INIT

DESCRIPTION
  Updates ATCoP with the client and session id's.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

==============================================================================*/
void dsat_mmgsdi_client_init
(
  mmgsdi_session_open_info_type *session_info_ptr,
  mmgsdi_client_id_type          client_id
);

/*==============================================================================

FUNCTION DSAT_MMGSDI_EVENT_CB

DESCRIPTION
  This function handles the MMGSDI client event response callback.  


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsat_mmgsdi_event_cb
(
  const mmgsdi_event_data_type *event
);
#ifdef FEATURE_DATA_MUX
/*===========================================================================

FUNCTION DSAT_GET_CMUX_PORT

DESCRIPTION
 Function to retrive the previous Ds3g Siolib port on which CMUX command was issued.

DEPENDENCIES
  None

RETURN VALUE
  The previous Ds3g Siolib port on which CMUX command was issued

SIDE EFFECTS
  None

===========================================================================*/
uint8 dsat_get_cmux_port(void);

/*===========================================================================

FUNCTION DSAT_CMUX_HANDLER

DESCRIPTION
Handler function which initializes MUX module.

DEPENDENCIES
  Physical port passed on to MUX should not be in open state.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsat_cmux_handler(void);
#endif /* FEATURE_DATA_MUX */

/*==============================================================================

FUNCTION dsat_get_cbst_export_val

DESCRIPTION
  This function returns the dsat_cbst_export_val global variable .  


DEPENDENCIES
  None

RETURN VALUE
  adress of global variable

SIDE EFFECTS
  None
===========================================================================*/
dsat_num_item_type* dsat_get_cbst_export_val
(
  int index
);

#if defined(FEATURE_DATA_GCSD) || defined(FEATURE_DATA_WCDMA_CS)
/*==============================================================================

FUNCTION dsat_get_cv120_export_val

DESCRIPTION
  This function gets global variable dsat_cv120_export_val.  


DEPENDENCIES
  None

RETURN VALUE
  variable value

SIDE EFFECTS
  None
===========================================================================*/
dsat_num_item_type dsat_get_cv120_export_val(void);

#endif /* ( (FEATURE_DATA_GCSD) || (FEATURE_DATA_WCDMA_CS) )*/


/*==============================================================================

FUNCTION dsat_set_cv120_export_val

DESCRIPTION
  This function sets global variable dsat_cv120_export_val.  


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsat_set_cv120_export_val
(
  dsat_num_item_type variable_value
);


/*==============================================================================

FUNCTION dsat_get_qcscftest_exp_val

DESCRIPTION
  This function returns the dsat_qcscftest_exp_val global variable .  


DEPENDENCIES
  None

RETURN VALUE
  global variable

SIDE EFFECTS
  None
===========================================================================*/
uint8 dsat_get_qcscftest_exp_val
(
  int index
);

/*==============================================================================

FUNCTION dsat_set_qcscftest_exp_val

DESCRIPTION
  This function sets the dsat_qcscftest_exp_val global variable .  


DEPENDENCIES
  None

RETURN VALUE
  global variable

SIDE EFFECTS
  None
===========================================================================*/
void dsat_set_qcscftest_exp_val
(
  int index, uint8 value
);

/*==============================================================================

FUNCTION dsat_get_es_export_val

DESCRIPTION
  This function returns the dsat_es_export_val global variable .  


DEPENDENCIES
  None

RETURN VALUE
  global variable

SIDE EFFECTS
  None
===========================================================================*/
dsat_num_item_type dsat_get_es_export_val
(
  int index
);

/*==============================================================================

FUNCTION dsat_set_es_export_val

DESCRIPTION
  This function sets the dsat_es_export_val global variable .  


DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  None
===========================================================================*/
void dsat_set_es_export_val
(
  int index, dsat_num_item_type value
);

/*==============================================================================

FUNCTION dsat_get_esa_export_val

DESCRIPTION
  This function returns the dsat_esa_export_val global variable .  


DEPENDENCIES
  None

RETURN VALUE
  global variable

SIDE EFFECTS
  None
===========================================================================*/
dsat_num_item_type dsat_get_esa_export_val
(
  int index
);

/*==============================================================================

FUNCTION dsat_set_esa_export_val

DESCRIPTION
  This function sets the dsat_esa_export_val global variable .  


DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  None
===========================================================================*/
void dsat_set_esa_export_val
(
  int index, dsat_num_item_type value
);

#ifdef FEATURE_DSAT_DYNAMIC_LOADING

/*===========================================================================
FUNCTION  DSATDL_CMD_HANDLER

DESCRIPTION
  This function is used for handling ATCoP dynamic loading commands.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatdl_cmd_handler
(
  dsatdl_cmd_e_type cmd
);

/*===========================================================================
FUNCTION  DSATDL_GET_STATUS

DESCRIPTION
  This function returns the dynamic loading status of ATCoP DLL.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
dsatdl_status_e_type dsatdl_get_status(void);

#endif /* FEATURE_DSAT_DYNAMIC_LOADING */

#endif /* DSAT_V_H */
