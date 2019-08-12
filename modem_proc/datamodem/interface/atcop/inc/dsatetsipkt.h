#ifndef DSATETSIPKT_H
#define DSATETSIPKT_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                D A T A   S E R V I C E S

                       U M T S

                P A C K E T   D O M A I N

                A T   C O M M A N D   P R O C E S S I N G

DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( UMTS packet domain commands ) processor.

INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2001 - 2014 by Qualcomm Technologies Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath:  L:/src/asw/MSM5200/data/vcs/dsatetsipkt.h_v   1.4   12 Jul 2002 10:05:54   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/inc/dsatetsipkt.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/24/14   tk      ATCoP changes for dynamic memory optimizations.
08/12/14   pg      Modified DS Profile APIs to per subs.
07/29/14   sc      Added support for NV Refresh.
06/27/14   tk      Added support for Dynamic ATCoP.
09/25/13   sc      Added support for +CNMPSD command.
02/17/12   sk      Migrating Data related AT commands interface to unified MH.
01/19/12   sk      Feature cleanup.
02/16/11   ttv     Added DSDS changes for consolidated profile family.
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
07/09/10   bs      Fixed +CGACT for LTE.
05/15/10   nc      Added support fot +CGCONTRDP,+CGSCONTRDP,
                   +CGEQOSRDP and +CGTFTRDP for LTE.
04/20/10   bs      Added support for +CGCMOD for LTE.
03/18/10   nc      Added Support for +CGEQOS for LTE.
01/06/10   bs      Added +CGACT command support for LTE.
12/15/09   nc      Featurisation changes for LTE.
10/09/09   sa      Adding support for +CEMODE command.
05/19/09   vk      Defeaturization of PDP registry for CMI
01/30/09   ua      Added support of CGEV for DUN calls.
12/12/08   ua      Fixed Off target lint errors.
10/23/08   sa      Added support for $QCPDPFAM command.
11/04/07   pp      Lint low - fixes.
10/09/07   ss      Added support for $QCGANSM, $QCGARL, $QCGAPL commands.
08/22/07   sa      Added support for +CGANS and +CGAUTO commands.
08/06/07   sa      Removed +CGEQREQ and +CGEQMIN support in GSM only builds.
04/23/07   pp      Lint Medium fixes.
03/29/07   ua      Reverting back CGCLASS modifications which supported CLASS B.
                   Now it supports only CLASS A. 
02/13/07   pkp     dsatetsipkt_cgcmod_test_response_fmt prototype changed.
08/02/06   snb     Dual processor changes.
06/08/06   ua      Modification of +CGCLASS command to support only CLASSB.
12/08/05   snb     Add IPV6 support
11/30/05   snb     Add support for +CGCMOD command.
07/25/05   snb     Change PDP connect info struct to support WCDMA-mode 
                   AT$QCDGEN.
06/27/05   snb     Add fields to dsat_pdp_connect_info_s_type to support 
                   $QCDGEN sending data simultaneously on multiple contexts.
05/03/05   tkk     Added prototype for context ID initialization.
04/04/05   snb     Made external a function returning number of contexts 
                   without an assigned TFT.
03/14/05   tkk     Added prototype for GPRS event (+CGEREP) queues.
03/02/05   snb     Add fields to dsat_pdp_connect_info_s_type and 
                   dsat_pdp_cm_info_s_type to support $QCDGEN command.
01/31/05   tkk     Added prototypes for exec'ing +CGDATA, packet call 
                   initiation & dsat_cgdata_info_s_type structure for 
                   +CGDATA command.
01/27/05   pdv     Merge from Sirius-ATCoP development branch.
01/27/05   pdv     Rebase from Mainline.
01/17/05   tkk     Added support for +CGEREP command.
01/10/05   tkk     Added support for +CGPADDR command.
09/10/04   sb      Secondary PDP changes
11/19/03   ar      Added support for +CGDSCONT and +CGTFT commands.
08/13/03   snb     Added support for $QCPDPLT command
03/31/03   ar      Added command write_mode to context activation tracking.
02/14/04   ar      Add support for PDP context profile activation.
01/30/03   ar      Add support for +CGEQMIN command
01/13/03   ar      Removed obsolete dsatetsipkt_init_pdp_context() ptotype
08/26/02   ar      Code review action items
06/27/02   ar      Add support for +CGEQREQ; split query operations into 
                   separate functions for extended commands, used by &v logic
04/11/02   ar      Add support for $QCPDPP command
02/04/02   ar      Add support for +CGDCONT command
08/27/01   SB      created file

===========================================================================*/

#include "datamodem_variation.h"
#ifdef FEATURE_DSAT_ETSI_DATA

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds3gmgr.h"
#include "dsumtsps.h"
#include "ds_3gpp_rmsm_atif.h"
#include "dsatctab.h"
#include "dsatetsictab.h"
#include "dsati.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Macro to copy to uint32 fm byte array with byte reversal */
#define ADDR2INT(x,a) (x = (((a[0] << 24 ) & 0xFF000000) | \
                            ((a[1] << 16 ) & 0x00FF0000) | \
                            ((a[2] << 8  ) & 0x0000FF00) | \
                            (a[3] & 0x000000FF)))

/* Macro for copying between PDP addresses per 24.008 section 10.5.6.4 and 
   DSUMTS IP addresses: performs byte reversal during copy, assumes d & s 
   are byte pointers */
#define BYTE_REV_CPY(d,s,l) {uint8 idx; for (idx=0; idx<l; idx++) *(d + idx) = *(s + ((l - idx) - 1));} 

typedef enum
{
  READ_COMMAND,
  WRITE_COMMAND,
  RESET_COMMAND
}dsat_pdp_command_type;

/*---------------------------------------------------------------------------
    Typedefs for PDP context profile activation support
---------------------------------------------------------------------------*/
/* Parameter table strucure  used in get and set param API's */
typedef struct atcop_profile_3gpp_param_list
{
  ds_profile_3gpp_param_enum_type   param_type;
  uint16                            param_len;  
}atcop_profile_3gpp_param_list_type;

/* Param length for Strings is 0*/
static const atcop_profile_3gpp_param_list_type context_param_tbl[] = 
{
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_NUMBER,
    sizeof (ds_profile_3gpp_pdp_context_number_type) },

  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_TYPE,
    sizeof (ds_profile_3gpp_pdp_type_enum_type) },
  
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_H_COMP,
    sizeof (ds_profile_3gpp_pdp_header_comp_e_type) },
  
  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_D_COMP,
    sizeof(ds_profile_3gpp_pdp_data_comp_e_type) },

  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V4,
    sizeof(ds_profile_3gpp_pdp_addr_type_ipv4) },

  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PDP_ADDR_V6,
    sizeof(ds_profile_3gpp_pdp_addr_type_ipv6) },

  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_APN,
    DS_PROFILE_3GPP_MAX_APN_STRING_LEN+1 },

  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_SECONDARY_FLAG,
    sizeof(ds_profile_3gpp_pdp_context_secondary_flag_type) },

  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_PRIMARY_ID,
    sizeof(ds_profile_3gpp_pdp_context_primary_id_type) },

  { DS_PROFILE_3GPP_PROFILE_PARAM_PDP_CONTEXT_TE_MT_ACCESS_CTRL_FLAG,
    sizeof(ds_profile_3gpp_pdp_access_control_e_type) },

  { DS_PROFILE_3GPP_PROFILE_PARAM_IPV4_ADDR_ALLOC,
    sizeof(ds_profile_3gpp_pdp_ipv4_addr_alloc_e_type) }
};

static const atcop_profile_3gpp_param_list_type dsat_cgdcont_ext_table[] = 
{
  { DS_PROFILE_3GPP_PROFILE_PARAM_EMERGENCY_CALLS_SUPPORTED,
    sizeof(ds_profile_3gpp_emergency_calls_are_supported) }
};

static const atcop_profile_3gpp_param_list_type auth_param_tbl[] = 
{

  { DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_TYPE,
    sizeof (ds_profile_3gpp_auth_pref_type) },

  { DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_PASSWORD,
    DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1 },
  
  { DS_PROFILE_3GPP_PROFILE_PARAM_AUTH_USERNAME,
    DS_PROFILE_3GPP_MAX_QCPDP_STRING_LEN+1 }
};

typedef enum pdp_connect_state
{
  DSAT_PDP_CONNECT_DEACTIVATED = 0,
  DSAT_PDP_CONNECT_ACTIVATED   = 1,
  DSAT_PDP_CONNECT_MAX            /* Internal use only ! */
} dsat_pdp_connect_state_e_type;

typedef enum call_query_mode
{
  DSAT_CQM_NONE,                  /* Internal use only ! */
  DSAT_CQM_CPAS_READ,             /* +CPAS read command */
  DSAT_CQM_CGACT_WRITE,           /* +CGACT write command */
  DSAT_CQM_CGCMOD_WRITE,          /* +CGCMOD write command */
  DSAT_CQM_CGCMOD_TEST,           /* +CGCMOD test command */
  DSAT_CQM_MAX                    /* Internal use only ! */
} dsat_call_query_mode_e_type;

#define UNUSED 255

typedef struct dsat_pdp_connect_tbl
{
  dsat_pdp_connect_state_e_type  demand_state;    /* Connection state */
  uint8                          change_index;    /* Connection changing */
  boolean                        pending;         /* Operation pending? */
  dsat_num_item_type             valid_profile_count;/*Total valid profile  */
} dsat_pdp_connect_s_type;

/* For +CGDATA */
typedef struct dsat_cgdata_info {
  boolean   valid;
  byte      cid; /* Context ID */
} dsat_cgdata_info_s_type;

#define DSAT_CGEREP_BUFFER_AT_MT           (dsat_num_item_type)0
#define DSAT_CGEREP_BUFFER_DISCARD_FWD_TE  (dsat_num_item_type)1
#define DSAT_CGEREP_BUFFER_FWD_TE          (dsat_num_item_type)2

#define DSAT_CGEREP_EMPTY_BUFFER           (dsat_num_item_type)0
#define DSAT_CGEREP_FLUSH_BUFFER           (dsat_num_item_type)1

/* PDP context profile activation state information */
extern dsat_pdp_connect_s_type dsat_pdp_state;
extern byte  dsat_pdp_cid[];  /* Context profile id */

/*---------------------------------------------------------------------------
                        Command defaults and limits
---------------------------------------------------------------------------*/
#define QCPDPLT_DEF_VAL 1

#define DSATPKT_CID_ALL (dsat_num_item_type)0x55

/* Added for +CGDATA support */
#define DSATETSIPKT_GPRS_SERVICE_CODE       99 
#define DSATETSIPKT_CGDATA_CMD              0x66
#define DSATETSIPKT_CID_PRIMARY             (boolean)0
#define DSATETSIPKT_CID_SECONDARY           (boolean)1

#define DSAT_GPRS_EVENT_MSG_COUNT   15
/* Worst case: +CGEV: NW REACT <PDP_type>,<PDP_addr>,<cid> */
#define GPRS_EVENT_DATA_SIZE        (30 + MAX_IPADDR_STR_LEN)

typedef struct {
  q_link_type    link;           /* Queue link type */
  char           event_data[GPRS_EVENT_DATA_SIZE]; /* GPRS event data */
  uint16         event_data_len;
} etsipkt_gprs_msg_type;

typedef struct  dsat_gprs_msg_ms_info
{
  boolean flush_gprs_buffer;
  q_type  etsipkt_gprs_msg_q;
}dsatetsipkt_gprs_msg_ms_info;

typedef struct dsat_gprs_msg_info
{
  dsatetsipkt_gprs_msg_ms_info ms[DSAT_MS_MAX_SUBS];
}dsatetsipkt_gprs_msg_info;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

 FUNCTION  ETSIPKT_CHECK_RESPONSE_BUFFER
 
 DESCRIPTION
   This function checks the lenth of the response buffer to see if
   another must be chained to it.  A buffer limit value of zero means
   use normal DSM buffer default.
 
 DEPENDENCIES
   None
 
 RETURN VALUE
   returns pointer to current DSM item buffer on success; NULL otherwise
 
 SIDE EFFECTS
   DSM item may be chained to passed in buffer, moving input pointer
   to new item.
   
 ===========================================================================*/
 dsm_item_type* etsipkt_check_response_buffer
(
  dsm_item_type  *res_buff_ptr,   /* Response buffer */
  uint16          buf_limit       /* Buffer contents limit */
);

/*===========================================================================

FUNCTION ETSIPKT_GET_PROFILE_LIST

DESCRIPTION
  This function generates the list of valid profiles either persistent
  or non-persistent per technology type. 
DEPENDENCIES
  None

RETURN VALUE 
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dsat_result_enum_type etsipkt_get_profile_list
(
  ds_profile_tech_etype  tech_type,           /* Technology Type  */
  uint8                  *profile_list,       /* Profile_ID list index */
  uint8                  profile_list_len,    /* Max length of profile_list_possible */
  uint8                  *total_profiles,      /* Total Number of Profiles */
  ds_profile_subs_etype    ps_subs
);

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGPADDR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGPADDR command.
  This command returns a list of PDP addresses for the specified
  contect identifiers

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgpaddr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#ifdef FEATURE_DSAT_LTE

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGCONTRDP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGCONTRDP command.
  This command returns a set of dynamic parameters associated with the specified
  context identifier.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgcontrdp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGSCONTRDP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGSCONTRDP command.
  This command returns a set of dynamic parameters associated with the specified
  context identifier.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgscontrdp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGEQOSRDP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGEQOSRDP command.
  This command returns a set of dynamin parameters associated with the specified
  context identifier.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgeqosrdp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGTFTRDP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGTFTRDP command.
  This command returns a set of dynamin parameters associated with the specified
  context identifier.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgtftrdp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#endif /* FEATURE_DSAT_LTE */


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGDCONT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGDCONT command.
  This command sets PDP context parameters.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgdcont_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#ifdef FEATURE_DATA_TE_MT_PDP

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_QCGARL_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the $QCGARL command.
  This command sets PDP context reject list.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_qcgarl_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_QCGAPL_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the $QCGAPL command.
  This command sets PDP context permission list.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_qcgapl_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* FEATURE_DATA_TE_MT_PDP */
/*===========================================================================

FUNCTION DSATETSIPKT_CGDCONT_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGDCONT read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsipkt_cgdcont_response_fmt
(
  const dsati_cmd_type  *parse_table,     /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,   /*  Place to put response       */
  multi_index_fmt_e_type format         /*  Flag for format type        */
);

#ifdef FEATURE_SECONDARY_PDP
/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGDSCONT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGDSCONT command.
  This command sets PDP secondary context parameters.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgdscont_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATETSIPKT_CGDSCONT_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGDSCONT read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsipkt_cgdscont_response_fmt
(
  const dsati_cmd_type  *parse_table,    /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format        /*  Flag for format type        */
);



/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGTFT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGTFT command.
  This command sets PDP Traffic Flow Template parameters.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgtft_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATETSIPKT_CGTFT_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGTFT read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsipkt_cgtft_response_fmt
(
  const dsati_cmd_type  *parse_table,    /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format        /*  Flag for format type        */
);
#endif /* FEATURE_SECONDARY_PDP */

#ifdef FEATURE_DATA_WCDMA_PS
/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGEQREQ_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGEQREQ command.
  This command sets PDP context parameters for UMTS QoS Profile
  (Requested).

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  Defining +CGEQREQ paramaters will undefine +CGQREQ/CGQMIN paramaters;
  these commands are mutually exclusive.

===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgeqreq_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATETSIPKT_CGEQREQ_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGEQREQ read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsipkt_cgeqreq_response_fmt
(
  const dsati_cmd_type  *parse_table,    /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format        /*  Flag for format type        */
);


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGEQMIN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGEQMIN command.
  This command sets PDP context parameters for UMTS QoS Profile
  (Requested).

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  Defining +CGEQMIN paramaters will undefine +CGQREQ/CGQMIN paramaters;
  these commands are mutually exclusive.

===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgeqmin_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATETSIPKT_CGEQMIN_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGEQMIN read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsipkt_cgeqmin_response_fmt
(
  const dsati_cmd_type  *parse_table,    /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format        /*  Flag for format type        */
);
#endif /* FEATURE_DATA_WCDMA_PS */

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGQREQ_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGQREQ command.
  This command sets PDP context parameters for GPRS QoS Profile
  (Requested).

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  Defining +CGQREQ paramaters will undefine +CGEQREQ paramaters;
  these commands are mutually exclusive.

===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgqreq_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATETSIPKT_CGQREQ_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGQREQ read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsipkt_cgqreq_response_fmt
(
  const dsati_cmd_type  *parse_table,    /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format        /*  Flag for format type        */
);


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGQMIN_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGQMIN command.
  This command sets PDP context parameters for GPRS QoS Profile
  (Minimum Acceptable).

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  Defining +CGQMIN paramaters will undefine +CGEQREQ paramaters;
  these commands are mutually exclusive.
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgqmin_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

  
/*===========================================================================

FUNCTION DSATETSIPKT_CGQMIN_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGQMIN read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsipkt_cgqmin_response_fmt
(
  const dsati_cmd_type  *parse_table,    /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format        /*  Flag for format type        */
);


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_QCPDPP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the $QCDPDP command.
  This command sets PDP authentication parameters.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_qcpdpp_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_QCPDPLT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcpdplt command.
  at$qcpdplt is used to enable/disable tolerance to long delays in PDP call
  setup.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DS_ATCOP_OK : if the command has been successfully executed
    DS_ATCOP_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_qcpdplt_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);


/*===========================================================================
FUNCTION DSATETSIPKT_INIT_TOLERANCE_FROM_NV

DESCRIPTION
  This function reads pdp call setup delay tolerance from the NV during 
  initialization. If this item was never written in NV, the parm is set to 1. 
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsipkt_init_tolerance_from_nv
(
  void
);

/*===========================================================================
FUNCTION DSATETSIPKT_RESET_TOLERANCE

DESCRIPTION
  This function resets pdp call setup delay tolerance to a value of 1 and 
  stores the same in NV.
    
DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsipkt_reset_tolerance
(
  void
);

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGATT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGATT command.
  This command reports the connection status for network packet
  domain service.
  
DEPENDENCIES
  This function only supports read & test commands at this time.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgatt_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CNMPSD_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes +CNMPSD command which releases PS signaling.This command
  indicates that no application on the MT is expected to exchange data.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if the command is read or write OR when the default command
    execution fails.
    DSAT_OK : if it is a query command 
    DSAT_ASYNC_CMD :  successful execution of CM API cm_mm_call_cmd_ps_sig_release.


SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cnmpsd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGACT_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGACT command.
  This command activates or deactivates PDP context profiles.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgact_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSIPKT_CGACT_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGACT read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsipkt_cgact_response_fmt
(
  const dsati_cmd_type  *parse_table,    /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format        /*  Flag for format type        */
);

/*===========================================================================

FUNCTION DSATETSIPKT_CGACT_PROC_CMD

DESCRIPTION
  This function does the CGACT processing for all the requested profile id's.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.
    DSAT_ASYNC_CMD : If the response is asynchronous.
    
SIDE EFFECTS
  None
  
===========================================================================*/

dsat_result_enum_type dsatetsipkt_cgact_proc_cmd( void );


/*===========================================================================

FUNCTION DSATETSIPKT_INIT_PDP_CONNECT_STATE

DESCRIPTION
  This function intializes the PDP context profile connection state
  information.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatetsipkt_init_pdp_connect_state ( void );


/*===========================================================================

FUNCTION DSATETSIPKT_RESET_PDP_CONTEXT_PROFILES

DESCRIPTION
  This function resets all persistent PDP context profiles to their
  default values.
  
DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in execution
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_reset_pdp_context_profiles ( void );


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGDATA_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGDATA command.
  This command initiates a packet data call.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgdata_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGEREP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGEREP command.
  This command reports (buffers) certain GPRS events when occured viz.,
  detach, PDP deactivate, class change.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgerep_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
FUNCTION  DSATETSIPKT_FLUSH_GPRS_EVENT_INDICATIONS

DESCRIPTION
  Flushes the buffered GPRS event reports to TE

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsipkt_flush_gprs_event_indications
(
  sys_modem_as_id_e_type subs_id
);


/*===========================================================================
FUNCTION  DSATETSIPKT_REPORT_GPRS_EVENT_TO_TE

DESCRIPTION
  Reports the GPRS event report buffer in MT.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsipkt_report_gprs_event_to_te
(
  dsat_cgerep_e_type     event_type, 
  void                  *event_data,
  sys_modem_as_id_e_type subs_id
);


/*===========================================================================
FUNCTION  DSATETSIPKT_INIT_SERVICE_PREFERENCE_FROM_NV

DESCRIPTION
  Retrieves MS's service preference option from NV.

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None

===========================================================================*/
void dsatetsipkt_init_service_preference_from_nv
(
  sys_modem_as_id_e_type subs_id
);

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGCMOD_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGCMOD command.
  This command modifies the Qos, TFT and APN of active PDP contexts.  

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgcmod_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================
FUNCTION DSATETSIPKT_CGACT_DONE_HANDLER

DESCRIPTION
  This function is called when +CGACT processing for a profile 
  is complete.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
======================================================================*/
void dsatetsipkt_cgact_done_handler
(
  dsat_rmsm_info_s_type         dsat_rmsm_info,
  void* user_info_ptr
);

/*===========================================================================
FUNCTION DSATETSIPKT_CGCMOD_DONE_HANDLER

DESCRIPTION
  This function is called when +CGCMOD processing for a profile 
  is complete.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
======================================================================*/
void dsatetsipkt_cgcmod_done_handler
(
  dsat_result_enum_type result,
  void* user_info_ptr
);

#ifdef FEATURE_DATA_TE_MT_PDP

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGAUTO_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGAUTO command.
  This command disables or enables an automatic positive response 
  (auto-answer) to the receipt of a Request PDP Context Activation message 
  from the network.
  
DEPENDENCIES
  None.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgauto_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGANS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGANS command.
  This command is used to answer a incoming MT PDP call.
  
DEPENDENCIES
  None.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_NO_RSP : if answer cb is called.
    DSAT_OK : if the test command succeeds.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgans_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#endif /* FEATURE_DATA_TE_MT_PDP */

#endif /* FEATURE_DSAT_ETSI_DATA */
#ifdef FEATURE_DSAT_LTE
/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CGEQOS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CGEQOS command.
  This command sets EPS QOS parameters for LTE/UMTS/GPRS QoS Profile
  (Requested) depending upon the mode.

DEPENDENCIES
  None

RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK : if it is a success.

SIDE EFFECTS

===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cgeqos_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSIPKT_CGEQOS_RESPONSE_FMT

DESCRIPTION
  This function generates the response for the +CGEQOS read & test
  commands.  Data is extracted from data structure and formatted according
  to parameter flag.

DEPENDENCIES
  None

RETURN VALUE
  Returns number of characters in response buffer.

SIDE EFFECTS
  None

===========================================================================*/
dsat_num_item_type dsatetsipkt_cgeqos_response_fmt
(
  const dsati_cmd_type  *parse_table,    /*  Ptr to cmd in parse table   */
  dsm_item_type * const *out_buff_ptr,  /*  Place to put response       */
  multi_index_fmt_e_type format        /*  Flag for format type        */
);

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_CEMODE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It handles the +CEMODE command.
  The set command is used to set the MT to operate according to the specified 
  mode of operation for EPS. The read command returns the mode of operation set
  by the TE, independent of the current serving cell capability and independent
  of the current serving cell Access Technology. The test command is used for 
  requesting information on the supported MT mode of operation.
  
DEPENDENCIES
  None.

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_CME_OP_NOT_SUPPORTED : in case of failure
    DSAT_OK : if the command succeeds.

SIDE EFFECTS
  None
  
===========================================================================*/
dsat_result_enum_type dsatetsipkt_exec_cemode_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* FEATURE_DSAT_LTE */
/*===========================================================================
FUNCTION DSATETSIPKT_AT_CMD_HANDLER

DESCRIPTION
  This function is handler function for reporting +CGACT,+CGCMOD command response.

DEPENDENCIES
  None
  
RETURN VALUE
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.

SIDE EFFECTS
  None
  
======================================================================*/
dsat_result_enum_type dsatetsipkt_at_cmd_handler
(
  ds_cmd_type         * cmd_ptr              /* DS Command pointer         */
);

/*===========================================================================

FUNCTION DSATETSIPKT_EXEC_QCDEFPROF_CMD

DESCRIPTION
  This function takes the result from the command line parser and executes 
  it. It executes $QCDEFPROF command.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns an enum that describes the result of the command execution.
  Possible values:
    DSAT_ERROR : if there was any problem in executing the command
    DSAT_OK    : if it is a success.

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsipkt_exec_qcdefprof_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATETSIPKT_IS_PROFILE_CONTEXT_VALID

DESCRIPTION
  This function verifies whether passed profile context is valid or not.

DEPENDENCIES
  None
  
RETURN VALUE
  Returns true if passed profile is valid else returns false.

SIDE EFFECTS
  None
  
===========================================================================*/
 /* ARGSUSED */
boolean dsatetsipkt_is_profile_context_valid
(
  uint16 profile_number,
  ds_profile_tech_etype tech,
  ds_profile_subs_etype ps_subs
);

/*===========================================================================

FUNCTION DSATETSIPKT_READ_WRITE_PARAM_TABLE

DESCRIPTION
  This function Reads,writes,resets parameters in the passed table.
  It calls begin and end transaction API's to process parameters.
 
DEPENDENCIES
  Argument pdata must be a packed structure to read and write appropriate values.
  
RETURN VALUE
  dsat_result_enum_type 

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsipkt_read_write_param_table
(
    const atcop_profile_3gpp_param_list_type* table_ptr,
    uint16 array_size,
    uint16 profile_number,
    dsat_pdp_command_type cmd_type,
    byte* pdata,
    uint16 max_data_size,
    ds_profile_subs_etype ps_subs
);

/*===========================================================================

FUNCTION DSATETSIPKT_READ_WRITE_PROFILE_PARAM

DESCRIPTION
  Based on the command type this function reads||writes passed profile 
  param_type
 
DEPENDENCIES
  
RETURN VALUE
  dsat_result_enum_type 

SIDE EFFECTS
  None
  
===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatetsipkt_read_write_profile_param
(
  uint16 profile_number,
  dsat_pdp_command_type cmd_type,
  ds_profile_identifier_type param_type,
  void* data,
  uint16 data_size
);



#endif /* DSATETSIPKT_H */
