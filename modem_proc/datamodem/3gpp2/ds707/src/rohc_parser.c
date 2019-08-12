/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           ROHC_PARSER . C

GENERAL DESCRIPTION
  This file contains the functions required to parse empa rohc efs file.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 

 Copyright (c) 2004-2013 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/rohc_parser.c#1 $
  

--------------------------------------------------------------------------
when        who    what, where, why
--------    ---    -------------------------------------------------------
08/06/13    sd     Replaced memcpy with memscpy.
02/11/11    ack    Global Variable Cleanup
10/01/10    op     Added NULL check for arguments in some functions
04/26/10    gc     Lint fixes
04/16/10    ls     Change dshdr_efs* to ds_efs*
12/04/07    ms     Lint Critical fix
05/17/07    ac     Lint fix
09/21/06    ac     Error fix on the parser
07/24/06    hm     Initial revision.
===========================================================================*/

/*=========================================================================
                      INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "fs_public.h"
#include "err.h"
#include "amssassert.h"
#include "msg.h"
#include "data_msg.h"
#include "dsutil.h"
#include "ds3gcfgmgr.h"
#include <stringl/stringl.h>

#include "ps_rohc.h"

/*---------------------------------------------------------------------------
  max number of records
---------------------------------------------------------------------------*/
#define ROHC_MAX_DB_ENTRIES 32

/*---------------------------------------------------------------------------
  EFS file where the database information is stored 
---------------------------------------------------------------------------*/
#define ROHC_EFS_FILE_NAME "/data/data/rohcparameters.txt"
//#define ROHC_EFS_FILE_NAME "C:\\depot\\empa2_m6800_5101\\hdr\\ut\\rlp\\rlp\\Debug\\rohcparameters.txt"

/* RoHC Compressor Flow Parameters for VoIP Flow - Recommendation from VoIP FDD */
#define DS707_SEC_PKT_MGR_ROHC_FLOW_EXP_BACKOFF 3;
#define DS707_SEC_PKT_MGR_ROHC_FLOW_F_MAX_TIME 40000;
#define DS707_SEC_PKT_MGR_ROHC_FLOW_LOSS_OPTION 0;
#define DS707_SEC_PKT_MGR_ROHC_FLOW_NUM_NONUPDATES 4;
#define DS707_SEC_PKT_MGR_ROHC_FLOW_NUM_UPDATE_IR 4;
#define DS707_SEC_PKT_MGR_ROHC_FLOW_SO_FOT 255;
#define DS707_SEC_PKT_MGR_ROHC_FLOW_SO_IRT 512;
#define DS707_SEC_PKT_MGR_ROHC_FLOW_TIME_STRIDE 0;
#define DS707_SEC_PKT_MGR_ROHC_FLOW_TS_STRIDE_RECALC 0;
#define DS707_SEC_PKT_MGR_ROHC_FLOW_WW_NUM_UPDATE 6;
    
/* RoHC De-Compressor Flow Parameters for VoIP Flow - Recommendation from VoIP FDD */
#define DS707_SEC_PKT_MGR_ROHC_FLOW_OPTIONAL_ACK_ENABLED 0;
#define DS707_SEC_PKT_MGR_ROHC_FLOW_RD_DEPTH 0;
#define DS707_SEC_PKT_MGR_ROHC_FLOW_TIMER_BASED_DECOMP 1;

/* RoHC Compressor Channel Parameters */
#define DS707_SEC_PKT_MGR_ROHC_CHANNEL_MAX_JITTER_CD 150;
#define DS707_SEC_PKT_MGR_ROHC_CHANNEL_NUM_ALLOWED_PKT_SIZES 0;
#define DS707_SEC_PKT_MGR_ROHC_CHANNEL_SKIP_CID 0;

/* RoHC De-Compressor Channel Parameters */
#define DS707_SEC_PKT_MGR_ROHC_CHANNEL_K1_OUT_N1 ROHC_POLICY_1_OUT_1
#define DS707_SEC_PKT_MGR_ROHC_CHANNEL_K2_OUT_N2 ROHC_POLICY_1_OUT_1

/*---------------------------------------------------------------------------
  Enum to specify the order in which the data need to read from the file.
  The various fields of the record are seperated by semicolon in the code. 

  Note : All these fields should be specified in one row. If a new line is hit
  then we should consider the end of record.

---------------------------------------------------------------------------*/
typedef enum 
{
 ROHC_COMPCHANNEL,
 ROHC_COMPFLOW,
 ROHC_DECOMPCHANNEL,
 ROHC_DECOMPFLOW
}rohc_id_enum_type;

typedef enum 
{
 COMP_CHANNEL,
 COMPCHANNEL_MAX_CID, 
 COMPCHANNEL_USE_LARGE_CIDS,
 COMPCHANNEL_MRRU,
 COMPCHANNEL_PROFILES,
 COMPCHANNEL_MAX_JITTER_CD,
 COMPCHANNEL_NUM_ALLOWED_PKT_SIZES,
 COMPCHANNEL_SKIP_CID
}compchannel_id_enum_type;

typedef enum 
{
 COMP_FLOW,
 COMPFLOW_WW_NUM_UPDATE, 
 COMPFLOW_NUM_UPDATE_IR,
 COMPFLOW_NUM_NONUPDATES,
 COMPFLOW_SO_IRT,
 COMPFLOW_SO_FOT,
 COMPFLOW_TIMER_BASED_COMP,
 COMPFLOW_PROFILE_HINT,
 COMPFLOW_F_MAX_TIME,
 COMPFLOW_EXP_BACKOFF,
 COMPFLOW_TIME_STRIDE,
 COMPFLOW_TS_STRIDE_RECALC,
 COMPFLOW_LOSS_OPTION,
 COMPFLOW_P_SN,
 COMPFLOW_CONST_IP_ID_HANDLING
}compflow_id_enum_type;

typedef enum 
{
 DECOMP_CHANNEL,
 DECOMPCHANNEL_MAX_CID, 
 DECOMPCHANNEL_USE_LARGE_CIDS,
 DECOMPCHANNEL_MRRU,
 DECOMPCHANNEL_PROFILES,
 DECOMPCHANNEL_K1_OUT_N1,
 DECOMPCHANNEL_K2_OUT_N2,
 DECOMPCHANNEL_SKIP_CID,
 DECOMPCHANNEL_FEEDBACK_FOR,
 DECOMPCHANNEL_FB_FUNC,
 DECOMPCHANNEL_FB_COOKIE
}decompchannel_id_enum_type;

typedef enum 
{
 DECOMP_FLOW,
 DECOMPFLOW_PREFERRED_MODE,
 DECOMPFLOW_OPTIONAL_ACK_ENABLED,
 DECOMPFLOW_REVERSE_DECOMPRESSION_DEPTH,
 DECOMPFLOW_TIMER_BASED_DECOMP,
 DECOMPFLOW_LOSS_OPTION_THRES,
 DECOMPFLOW_P_SN,
}decompflow_id_enum_type;
/*----------------------------------------------------------------------
  Populate efs file values to a structure as values will be stored in a 
  structure and read only once.
------------------------------------------------------------------------*/
static rohc_params_comp_channel_s_type default_comp_channel;
static rohc_params_comp_flow_s_type default_comp_flow;
static rohc_params_decomp_channel_s_type default_decomp_channel;
static rohc_params_decomp_flow_s_type default_decomp_flow;

/*===========================================================================
                       EFS FILE PARSING MACROS
===========================================================================*/
/*===========================================================================
MACRO         SKIP_SUBSTR_OR_RETURN

DESCRIPTION   Checks whether the parameter str starts with the specified
              substr of n bytes and if it does, it will move str to point 
              to the character after the substring. If str does not start
              with the specified substring, this will set *err_num to 
              E_BAD_DATA and cause the calling function to return. str is
              undefined if *err_num is set to E_BAD_DATA.

DEPENDENCIES  The parameters str and substr must be a pointers to NULL 
              terminated strings. str cannot itself be NULL. n must be the
              length of substr without the '\0'. err_num must be a valid
              pointer.

              This macro does not perform and checks for input parameters. 
              It is the responsibility of the calling entity to make sure 
              that the input parameters are valid.

RETURN VALUE  None.

SIDE EFFECTS  Modifies the string pointer str and *err_num that is passed.
              It could cause the code to return back from the calling
              function.
===========================================================================*/
#define SKIP_SPACE(from, to)							\
while((*from == ' ' || *from == '\t')&& (from <= to))   \
{														\
	from++;												\
}

#define SKIP_SUBSTR_OR_RETURN(from, to, substr, n, err_num)                   \
{                                                                           \
  SKIP_SPACE(from, to);														\
  CONVERT_TO_UPPERCASE(from, to);											\
  if((n > (to - from)) || (0 != strncmp(from,substr,n)))                                                       \
  {                                                                         \
    DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Parse failed");				\
    *err_num = E_BAD_DATA;                                                     \
    return -1;                                                              \
  }                                                                         \
                                                                            \
  from += n;                                                                \
}


void CONVERT_TO_UPPERCASE(char *from, char *to)                                              
{                                                                           
  while (from < to)
  {                                                            
    *from = toupper(*from);                                                 
    from++;                                                                 
  }                                                                                                                                        
}                                                                           

/*===========================================================================
FUNCTION      ROHC_PARAM_INIT

DESCRIPTION   Assigns default values to all ROHC parameters

DEPENDENCIES  None

RETURN VALUE  None
              
SIDE EFFECTS  None
===========================================================================*/

void rohc_param_init(void)
{
  default_comp_flow.exp_backoff				= DS707_SEC_PKT_MGR_ROHC_FLOW_EXP_BACKOFF;
  default_comp_flow.f_max_time				= DS707_SEC_PKT_MGR_ROHC_FLOW_F_MAX_TIME;
  default_comp_flow.loss_option				= DS707_SEC_PKT_MGR_ROHC_FLOW_LOSS_OPTION;
  default_comp_flow.p_sn						= 6;
  default_comp_flow. const_ip_id_handling		= TRUE;
  default_comp_flow.num_nonupdates			= DS707_SEC_PKT_MGR_ROHC_FLOW_NUM_NONUPDATES;
  default_comp_flow.num_update_IR				= DS707_SEC_PKT_MGR_ROHC_FLOW_NUM_UPDATE_IR;
  default_comp_flow.profile_hint				= ROHC_PROFILE_NONE_MASK;
  default_comp_flow.SO_FOt					= DS707_SEC_PKT_MGR_ROHC_FLOW_SO_FOT;
  default_comp_flow.SO_IRt 					= DS707_SEC_PKT_MGR_ROHC_FLOW_SO_IRT;
  default_comp_flow.time_stride				= DS707_SEC_PKT_MGR_ROHC_FLOW_TIME_STRIDE;    
  default_comp_flow.ts_stride_recalc			= DS707_SEC_PKT_MGR_ROHC_FLOW_TS_STRIDE_RECALC;
  default_comp_flow.ww_num_update				= DS707_SEC_PKT_MGR_ROHC_FLOW_WW_NUM_UPDATE;


  default_comp_channel.max_jitter_cd			= DS707_SEC_PKT_MGR_ROHC_CHANNEL_MAX_JITTER_CD;
  default_comp_channel.num_allowed_pkt_sizes	= DS707_SEC_PKT_MGR_ROHC_CHANNEL_NUM_ALLOWED_PKT_SIZES;
  default_comp_channel.skip_cid				= DS707_SEC_PKT_MGR_ROHC_CHANNEL_SKIP_CID;


  default_decomp_flow.optional_ack_enabled	= DS707_SEC_PKT_MGR_ROHC_FLOW_OPTIONAL_ACK_ENABLED;
  default_decomp_flow.preferred_mode			= ROHC_O_MODE;
  default_decomp_flow.rd_depth				= DS707_SEC_PKT_MGR_ROHC_FLOW_RD_DEPTH;
  default_decomp_flow.timer_based_decomp		= DS707_SEC_PKT_MGR_ROHC_FLOW_TIMER_BASED_DECOMP;
  default_decomp_flow.loss_option_thres		= 0;
  default_decomp_flow.p_sn					= 6;

  default_decomp_channel.k1_out_n1			= DS707_SEC_PKT_MGR_ROHC_CHANNEL_K1_OUT_N1;
  default_decomp_channel.k2_out_n2			= DS707_SEC_PKT_MGR_ROHC_CHANNEL_K2_OUT_N2;
  default_decomp_channel.skip_cid				= DS707_SEC_PKT_MGR_ROHC_CHANNEL_SKIP_CID;
}
/*lint -save -e641 Converting enum to int as error code 641*/
/*===========================================================================
FUNCTION      ROHC_READ_RECORD_FOM_EFS

DESCRIPTION   Reads and parses a record from the passed EFS file.

DEPENDENCIES  file_handle must point to a valid file in the EFS that has 
              been opened for READ access by the calling function.

RETURN VALUE  If successfully able to read the record from EFS and populate
              the database structure, returns 0 and *err_num is set to 
              E_SUCCESS.

              If an error is encountered while reading from the EFS or
              while parsing, returns -1 and *err_num is set to one of

              E_EOF           End of file reached
              E_INVALID_ARG   I/O parameter is invalid
              E_BAD_DATA      Parse error in the EFS text record
              E_OVERFLOW      Parsing exceed the max record size


SIDE EFFECTS  If successfully able to read and parse the text record from
              the EFS file, the file position in the EFS is set to the
              beginning of the next record. If a parse error is encountered,
              the file position is undefined.
===========================================================================*/
LOCAL sint15 rohc_read_record_from_efs
(              /*parse file to find comp_params;
  default_comp.max_cid=15;*/
  ds3gcfgmgr_efs_token_type  *efs_db_ptr,
  sint15 *err_num
)
{
  char *from, *to;
  ds3gcfgmgr_efs_token_parse_status_type  ret_val 
                                              = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
  rohc_id_enum_type id=ROHC_COMPCHANNEL;
  compchannel_id_enum_type cch = COMP_CHANNEL;
  compflow_id_enum_type cflow = COMP_FLOW;
  decompchannel_id_enum_type dch = DECOMP_CHANNEL;
  decompflow_id_enum_type dflow = DECOMP_FLOW;
  uint32 temp;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Sanity checks on input parameters
  -------------------------------------------------------------------------*/
  if(err_num == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "err_num is NULL!");
    return -1;
  }

  if(efs_db_ptr == NULL)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "database/record is NULL!");
    *err_num = E_INVALID_ARG;
    return -1;
  }

  *err_num = E_FAILURE;

  /*-------------------------------------------------------------------------
    Parse the record tokens from the EFS file and assign them to the 
    appropriate record field. If there is a parse error, this will set the
    err_num and return. If the record is parsed successfully, set the database
    entry valid flag to true. The tokens in a record may be separated either
    by '\n' or by ';'
  -------------------------------------------------------------------------*/  
  while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF 
          != (ret_val = ds3gcfgmgr_efs_tokenizer(efs_db_ptr, &from, &to )))
  {
    /*------------------------------------------------------------------------
      Token being read. from points to the beginning of the token and 
      to point to the end of the token.

      The tokenizer automatically skips blank lines and comments (lines 
      beginning with #, so no need to check for them here).
    ------------------------------------------------------------------------*/
    if((from == to) || (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == ret_val))
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      continue;
    }     
    else if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == ret_val)
    {
      DATA_IS707_MSG1(MSG_LEGACY_MED, "Reading DB record %d", id);
      SKIP_SUBSTR_OR_RETURN(from, to, "EMPA",4, err_num);

      switch(id)
      {
        case ROHC_COMPCHANNEL:
          while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF 
                 != (ret_val = ds3gcfgmgr_efs_tokenizer(efs_db_ptr, &from, &to )))
          {
             /*------------------------------------------------------------------------
               Token being read. from points to the beginning of the token and 
               to point to the end of the token.
 
               The tokenizer automatically skips blank lines and comments (lines 
               beginning with #, so no need to check for them here).
              ------------------------------------------------------------------------*/

            if (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == ret_val)
            {
              break; /* go to next record */
            }
            else if(from == to)
            {
               /*----------------------------------------------------------------------
                 Skip empty tokens.
               ----------------------------------------------------------------------*/
              continue;
            }     
            else if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == ret_val)
            {
              switch(cch)
              {
                case COMP_CHANNEL:
                  SKIP_SUBSTR_OR_RETURN(from, to, "COMP_CHANNEL",12, err_num);
                  break;

                case COMPCHANNEL_MAX_CID:
                  SKIP_SUBSTR_OR_RETURN(from, to, "MAX_CID",7, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_channel.max_cid));
                  break;

                case COMPCHANNEL_USE_LARGE_CIDS:
                  SKIP_SUBSTR_OR_RETURN(from, to, "USE_LARGE_CIDS",14, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_channel.use_large_cids));
                  break;

                case COMPCHANNEL_MRRU:
                  SKIP_SUBSTR_OR_RETURN(from, to, "MRRU",4, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_channel.mrru));
                  break;

                case COMPCHANNEL_PROFILES:
                  SKIP_SUBSTR_OR_RETURN(from, to, "PROFILES",8, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (temp));
                  default_comp_channel.profiles = (rohc_profiles_mask_e_type)temp;
                  break;

                case COMPCHANNEL_MAX_JITTER_CD:
                  SKIP_SUBSTR_OR_RETURN(from, to, "MAX_JITTER_CD",13, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_channel.max_jitter_cd));
                  break;

               case COMPCHANNEL_NUM_ALLOWED_PKT_SIZES:
                  SKIP_SUBSTR_OR_RETURN(from, to, "NUM_ALLOWED_PKT_SIZES",21, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_channel.num_allowed_pkt_sizes));
                  break;

               case COMPCHANNEL_SKIP_CID:
                 SKIP_SUBSTR_OR_RETURN(from, to, "SKIP_CID",8, err_num);
                 SKIP_SPACE(from, to);
                 DSUTIL_ATOI(from, to, (default_comp_channel.skip_cid));
                 break;
              }
              cch++;
            }
          } /* while ROHC_COMPCHANNEL */
          break;

        case ROHC_COMPFLOW:
          while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF 
                 != (ret_val = ds3gcfgmgr_efs_tokenizer(efs_db_ptr, &from, &to )))
          {
             /*------------------------------------------------------------------------
               Token being read. from points to the beginning of the token and 
               to point to the end of the token.
 
               The tokenizer automatically skips blank lines and comments (lines 
               beginning with #, so no need to check for them here).
              ------------------------------------------------------------------------*/

            if (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == ret_val)
            {
              break; /* go to next record */
            }
            else if (from == to)
            {
            /*----------------------------------------------------------------------
                Skip empty tokens.
              ----------------------------------------------------------------------*/
            continue;
            }     
            else if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == ret_val)
            {
              switch(cflow)
              {
                case COMP_FLOW:	
                  SKIP_SUBSTR_OR_RETURN(from, to, "COMP_FLOW", 9, err_num);
                  break;

                case COMPFLOW_WW_NUM_UPDATE:
                  SKIP_SUBSTR_OR_RETURN(from, to, "WW_NUM_UPDATE",13, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_flow.ww_num_update));
                  break;

                case COMPFLOW_NUM_UPDATE_IR:
                  SKIP_SUBSTR_OR_RETURN(from, to, "NUM_UPDATE_IR",13, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_flow.num_update_IR));
                  break;

                case COMPFLOW_NUM_NONUPDATES:
                  SKIP_SUBSTR_OR_RETURN(from, to, "NUM_NONUPDATES",14, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_flow.num_nonupdates));
                  break;

                case COMPFLOW_SO_IRT:
                  SKIP_SUBSTR_OR_RETURN(from, to, "SO_IRT",6, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_flow.SO_IRt));
                  break;

                case COMPFLOW_SO_FOT:
                  SKIP_SUBSTR_OR_RETURN(from, to, "SO_FOT",6, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_flow.SO_FOt));
                  break;

                case COMPFLOW_TIMER_BASED_COMP:
                  SKIP_SUBSTR_OR_RETURN(from, to, "TIMER_BASED_COMP",16, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_flow.timer_based_comp));
                  break;

                case COMPFLOW_PROFILE_HINT:
                  SKIP_SUBSTR_OR_RETURN(from, to, "PROFILE_HINT",12, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (temp));
                  default_comp_flow.profile_hint = (rohc_profiles_mask_e_type)temp;
                  break;

                case COMPFLOW_F_MAX_TIME:
                  SKIP_SUBSTR_OR_RETURN(from, to, "F_MAX_TIME",10, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_flow.f_max_time));
                  break;

                case COMPFLOW_EXP_BACKOFF:
                  SKIP_SUBSTR_OR_RETURN(from, to, "EXP_BACKOFF",11, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_flow.exp_backoff));
                  break;

                case COMPFLOW_TIME_STRIDE:
                  SKIP_SUBSTR_OR_RETURN(from, to, "TIME_STRIDE",11, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_flow.time_stride));
                  break;

                case COMPFLOW_TS_STRIDE_RECALC:
                  SKIP_SUBSTR_OR_RETURN(from, to, "TS_STRIDE_RECALC",16, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_flow.ts_stride_recalc));
                  break;

                case COMPFLOW_LOSS_OPTION:
                  SKIP_SUBSTR_OR_RETURN(from, to, "LOSS_OPTION",11, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_flow.loss_option));
                  break;

                case COMPFLOW_P_SN:
                  SKIP_SUBSTR_OR_RETURN(from, to, "P_SN",4, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_flow.p_sn));
                  break;

                case COMPFLOW_CONST_IP_ID_HANDLING:
                  SKIP_SUBSTR_OR_RETURN(from, to, "CONST_IP_ID_HANDLING",20, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_comp_flow.const_ip_id_handling));
                  break;
               }
               cflow++;
             }
          } /* while ROHC_COMPFLOW */
          break;

      case ROHC_DECOMPCHANNEL: 
        while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF 
              != (ret_val = ds3gcfgmgr_efs_tokenizer(efs_db_ptr, &from, &to )))
        {
           /*------------------------------------------------------------------------
             Token being read. from points to the beginning of the token and 
             to point to the end of the token.
             The tokenizer automatically skips blank lines and comments (lines 
             beginning with #, so no need to check for them here).
             ------------------------------------------------------------------------*/
             if (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == ret_val)
             {
               break; /* go to next record */
             }
             else if (from == to)
             {
             /*----------------------------------------------------------------------
                  Skip empty tokens.
              ----------------------------------------------------------------------*/
               continue;
             }
             else if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == ret_val)
             {
               switch(dch)
               {
                 case DECOMP_CHANNEL:	
                   SKIP_SUBSTR_OR_RETURN(from, to, "DECOMP_CHANNEL",14, err_num);
                   break;

                 case DECOMPCHANNEL_MAX_CID:
                   SKIP_SUBSTR_OR_RETURN(from, to, "MAX_CID",7, err_num);
                   SKIP_SPACE(from, to);
                   DSUTIL_ATOI(from, to, (default_decomp_channel.max_cid));
                   break;

                 case DECOMPCHANNEL_USE_LARGE_CIDS:
                   SKIP_SUBSTR_OR_RETURN(from, to, "USE_LARGE_CIDS",14, err_num);
                   SKIP_SPACE(from, to);
                   DSUTIL_ATOI(from, to, (default_decomp_channel.use_large_cids));
                   break;

                 case DECOMPCHANNEL_MRRU:
                   SKIP_SUBSTR_OR_RETURN(from, to, "MRRU",4, err_num);
                   SKIP_SPACE(from, to);
                   DSUTIL_ATOI(from, to, (default_decomp_channel.mrru));
                   break;

                 case DECOMPCHANNEL_PROFILES:
                   SKIP_SUBSTR_OR_RETURN(from, to, "PROFILES",8, err_num);
                   SKIP_SPACE(from, to);
                   DSUTIL_ATOI(from, to, (temp));
                   default_decomp_channel.profiles = (rohc_profiles_mask_e_type)temp;
                   break;

                case DECOMPCHANNEL_K1_OUT_N1:
                   SKIP_SUBSTR_OR_RETURN(from, to, "K1_OUT_N1",9, err_num);
                   SKIP_SPACE(from, to);
                   DSUTIL_ATOI(from, to, (temp));
                   default_decomp_channel.k1_out_n1 = (rohc_k1n1_k2n2_policy_e_type)temp;
                   break;

                case DECOMPCHANNEL_K2_OUT_N2:
                   SKIP_SUBSTR_OR_RETURN(from, to, "K2_OUT_N2",9, err_num);
                   SKIP_SPACE(from, to);
                   DSUTIL_ATOI(from, to, (temp));
                   default_decomp_channel.k2_out_n2 = (rohc_k1n1_k2n2_policy_e_type)temp;
                   break;

                 case DECOMPCHANNEL_SKIP_CID:
                   SKIP_SUBSTR_OR_RETURN(from, to, "SKIP_CID",8, err_num);
                   SKIP_SPACE(from, to);
                   DSUTIL_ATOI(from, to, (default_decomp_channel.skip_cid));
                   break;

                 case DECOMPCHANNEL_FEEDBACK_FOR:
                   /* Field feedback_for ignored for EMPA */
                   default_decomp_channel.feedback_for = NULL;
                   break;

                 case DECOMPCHANNEL_FB_FUNC:             
                   /* Field fb_func ignored for EMPA */
                   default_decomp_channel.fb_func = NULL;
                   break;

                 case DECOMPCHANNEL_FB_COOKIE:
                   /* Field fb_cookie ignored for EMPA */ 
                   default_decomp_channel.fb_cookie = NULL;
                   break; 
                }
                dch++;
             }
        }
        break;

       case ROHC_DECOMPFLOW:      
         while (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF 
               != (ret_val = ds3gcfgmgr_efs_tokenizer(efs_db_ptr, &from, &to )))
         {
          /*------------------------------------------------------------------------
             Token being read. from points to the beginning of the token and 
             to point to the end of the token.
 
             The tokenizer automatically skips blank lines and comments (lines 
             beginning with #, so no need to check for them here).
            ------------------------------------------------------------------------*/
            if (DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == ret_val)
            {
              break; /* go to next record */
            }
            else if (from == to)
            {
             /*----------------------------------------------------------------------
               Skip empty tokens.
             ----------------------------------------------------------------------*/
              continue;
            }     
            else if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == ret_val)
            {
              switch(dflow)
              {
                case DECOMP_FLOW:
                  SKIP_SUBSTR_OR_RETURN(from, to, "DECOMP_FLOW",11, err_num);
                  break;

                case DECOMPFLOW_PREFERRED_MODE:
                  SKIP_SUBSTR_OR_RETURN(from, to, "PREFERRED_MODE",14, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (temp));
                  default_decomp_flow.preferred_mode = (rohc_mode_e_type)temp;
                  break;

                case DECOMPFLOW_OPTIONAL_ACK_ENABLED:
                  SKIP_SUBSTR_OR_RETURN(from, to, "OPTIONAL_ACK_ENABLED",20, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_decomp_flow.optional_ack_enabled));
                  break;

                case DECOMPFLOW_REVERSE_DECOMPRESSION_DEPTH:
                  SKIP_SUBSTR_OR_RETURN(from, to, "REVERSE_DECOMPRESSION_DEPTH",27, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_decomp_flow.rd_depth));
                  break;

                case DECOMPFLOW_TIMER_BASED_DECOMP:
                  SKIP_SUBSTR_OR_RETURN(from, to, "TIMER_BASED_DECOMP",18, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_decomp_flow.timer_based_decomp));
                  break;

                case DECOMPFLOW_LOSS_OPTION_THRES:
                  SKIP_SUBSTR_OR_RETURN(from, to, "LOSS_OPTION_THRES",17, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_decomp_flow.loss_option_thres));
                  break;

                case DECOMPFLOW_P_SN:
                  SKIP_SUBSTR_OR_RETURN(from, to, "P_SN",4, err_num);
                  SKIP_SPACE(from, to);
                  DSUTIL_ATOI(from, to, (default_decomp_flow.p_sn));
                  break;
              }
              dflow++;
            }
         }
         break;

       default:
         DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Parse error reading database");
         *err_num = E_FAILURE;
         return -1;
      } /* switch on id */
    id++;
    }
    else
    {
      ASSERT(DS3GCFGMGR_EFS_TOKEN_PARSE_FAILURE == ret_val);
      *err_num = E_FAILURE;
      return -1;
    }
  } /* Big while Parsed the file*/

  DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Done Parsing record");
  /* Since we exited the while loop, ret_val 
     must be DS_EFS_TOKEN_PARSE_EOF */
    *err_num = E_EOF;

  return -1;

} /* rohc_read_record_from_efs*/

/*===========================================================================
FUNCTION      ROHC_READ_DB_FROM_EFS

DESCRIPTION   Read the database from EFS

DEPENDENCIES  None

RETURN VALUE  Returns E_SUCCESS if the database is read successfully, 
              E_FAILURE otherwise. 

              We can add other return error codes in the future if needed.

SIDE EFFECTS  Updates the database in memory with contents of the EFS if
              EFS read is successful. If EFS read is unsuccessful, the 
              contents of the database in memory are unspecified.
===========================================================================*/
sint15 rohc_read_db_from_efs(void)
{
 ds3gcfgmgr_efs_token_type  efs_db;
 sint15  err_num;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(ds3gcfgmgr_efs_file_init(ROHC_EFS_FILE_NAME, &efs_db) == -1)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Error opening file Rohcparameters from EFS! Using def. values");
    rohc_param_init();
    return E_FAILURE;
  }

/*-------------------------------------------------------------------------
    Read each record from EFS. 
    If the EFS file is too small, this will result in returning E_SUCCESS
    but only some of the entries in the database will be valid.
    If there is a parse error this will return E_FAILURE.
  -------------------------------------------------------------------------*/
  if(rohc_read_record_from_efs(&efs_db, &err_num) == -1)
  {
    ds3gcfgmgr_efs_file_close(&efs_db);
    if(err_num != E_EOF)
    {
      DATA_IS707_MSG1(MSG_LEGACY_HIGH, "Error %d parsing record from EFS.Aborting and using def. values!", err_num);
      rohc_param_init();
      return E_FAILURE;
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "EOF reached reading record from EFS.");
      return E_SUCCESS;
    }
  }
 
  ds3gcfgmgr_efs_file_close(&efs_db);

  DATA_IS707_MSG0(MSG_LEGACY_MED, "EFS file read successfully");
  return E_SUCCESS;
}/*rohc_read_db_from_efs*/
/*lint -restore Restore lint error 641*/

/*===========================================================================
FUNCTION      GET_DEFAULT_COMP_CHANNEL

DESCRIPTION   Read default values from efs file and writes them in a structure.

DEPENDENCIES  

RETURN VALUE  None.

SIDE EFFECTS  
===========================================================================*/
void get_default_comp_channel(rohc_params_comp_channel_s_type * mychannel)
{
  if (NULL == mychannel)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Functions argument, mychannel, is NULL");
    return;
  }
  memscpy( (rohc_params_comp_channel_s_type *)mychannel,
            sizeof(rohc_params_comp_channel_s_type), 
           (rohc_params_comp_channel_s_type *)&default_comp_channel,
            sizeof(default_comp_channel) );
}

/*===========================================================================
FUNCTION      GET_DEFAULT_COMP_FLOW

DESCRIPTION   Read default values from efs file and writes them in a structure.

DEPENDENCIES  

RETURN VALUE  None.

SIDE EFFECTS  
===========================================================================*/
void get_default_comp_flow(rohc_params_comp_flow_s_type * myflow)
{
  if (NULL == myflow)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Functions argument, myflow, is NULL");
    return;
  }
  memscpy( (rohc_params_comp_flow_s_type *)myflow,
            sizeof(rohc_params_comp_flow_s_type),
            (rohc_params_comp_flow_s_type *)&default_comp_flow,
            sizeof(default_comp_flow) );
}

/*===========================================================================
FUNCTION      GET_DEFAULT_DECOMP_CHANNEL

DESCRIPTION   Read default values from efs file and writes them in a structure.

DEPENDENCIES  

RETURN VALUE  None.

SIDE EFFECTS  
===========================================================================*/
void get_default_decomp_channel(rohc_params_decomp_channel_s_type * mychannel)
{
  if (NULL == mychannel)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Functions argument, mychannel, is NULL");
    return;
  }
  memscpy( (rohc_params_decomp_channel_s_type *)mychannel,
           sizeof(rohc_params_decomp_channel_s_type),
           (rohc_params_decomp_channel_s_type *)&default_decomp_channel,
           sizeof(default_decomp_channel) );
}

/*===========================================================================
FUNCTION      GET_DEFAULT_DECOMP_FLOW

DESCRIPTION   Read default values from efs file and writes them in a structure.

DEPENDENCIES  

RETURN VALUE  None.

SIDE EFFECTS  
===========================================================================*/
void get_default_decomp_flow(rohc_params_decomp_flow_s_type * myflow)
{
  if (NULL == myflow)
  {
    DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Functions argument, myflow, is NULL");
    return;
  }
  memscpy( myflow, sizeof(rohc_params_decomp_flow_s_type),
           &default_decomp_flow, sizeof(default_decomp_flow) );
}

