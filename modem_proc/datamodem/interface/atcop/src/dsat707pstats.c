/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This module executes AT commands. It executes IS-707 specific Protocol
  Statistics AT commands.

EXTERNALIZED FUNCTIONS
dsat707_exec_qcrlpd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPD command.
  Dump RLP protocol statistics.

dsat707_exec_qcrlpr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPR command.
  Reset RLP protocol statistics.
  
dsat707_exec_qcrlpd33_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPD33 command.
  Dump RLP 3 protocol statistics.
  
dsat707_exec_qcrlpr33_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPR33 command.
  Reset RLP 3 protocol statistics.
  
dsat707_exec_qcpppd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPPPD command.
  Dump PPP protocol statistics.
  
dsat707_exec_qcpppr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPPPR command.
  Reset PPP protocol statistics.
  
dsat707_exec_qcipd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCIPD command.
  Dump IP protocol statistics.
  
dsat707_exec_qcipr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCIPR command.
  Reset IP protocol statistics.
  
dsat707_exec_qcudpd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCUDPD command.
  Dump UDP protocol statistics.
  
dsat707_exec_qcudpr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCUDPR command.
  Reset UDP protocol statistics.
  
dsat707_exec_qctcpd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCTCPD command.
  Dump TCP protocol statistics.
  
dsat707_exec_qctcpr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCTCPR command.
  Reset TCP protocol statistics.
  
  
Copyright (c) 1995,1996,1997 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1998 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 1999, 2000, 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
Copyright (c) 2002 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/atcop/src/dsat707pstats.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
		      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/11/11   ad      Remove extern usage.used get/set API for command associated  
                   value pointers.
04/03/02   rsl     Initial version of file.

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_DS_PSTATS
#include "dsati.h"
#include "dsat707pstats.h"
#include "dsatparm.h"
#include "msg.h"


/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/
LOCAL byte *dsat707_build_ascii_stat_array
(
  word                     *stat_array,/* array of stats                   */
  const byte *const *const name_array, /* array of pointers to stats names */
  const word               stat_count, /* count of stats in stat_array     */
  byte                     *rb_ptr,    /* pointer to response buffer       */
  dsm_item_type  **res_buff_ptr_ptr,   /* pointer to response item         */
  boolean ppp_stats_and_online         /* ignore netmodel stats if true    */
) ;

LOCAL byte *dsat707_build_ascii_stat_array_uint32
(
  uint32                   *stat_array,/* array of stats                   */
  const byte *const *const name_array, /* array of pointers to stats names */
  const word               stat_count, /* count of stats in stat_array     */
  byte                     *rb_ptr,    /* pointer to response buffer       */
  dsm_item_type  **res_buff_ptr_ptr,   /* pointer to response item         */
  boolean ppp_stats_and_online         /* ignore netmodel stats if true    */
) ;


LOCAL uint16 dsat707_qcrlpd
(
  byte           *rb_ptr,           /* pointer to response buffer */
  uint16                     size,       /* Buffer Size*/
  dsm_item_type  **res_buff_ptr_ptr /* pointer to response item   */
) ;

#ifdef FEATURE_DS_RLP3
LOCAL uint16 dsat707_qcrlpd33
(
  byte           *rb_ptr,           /* pointer to response buffer */
  uint16                     size,       /* Buffer Size*/
  dsm_item_type  **res_buff_ptr_ptr /* pointer to response item   */
) ;
#endif /* FEATURE_DS_RLP3 */

LOCAL uint16 dsat707_qcpppd
(
  byte                      *rb_ptr,     /* pointer to response buffer */
  uint16                     size,       /* Buffer Size*/
  dsm_item_type  **res_buff_ptr_ptr,     /* pointer to response item   */
  dsat_mode_enum_type       mode     /* what mode are we in?       */
) ;

LOCAL uint16 dsat707_qcipd
(
  byte           *rb_ptr,           /* pointer to response buffer */
  uint16          size,             /* Buffer Size*/
  dsm_item_type  **res_buff_ptr_ptr /* pointer to response item   */
) ;

LOCAL uint16 dsat707_qcudpd
(
  byte           *rb_ptr,           /* pointer to response buffer */
  dsm_item_type  **res_buff_ptr_ptr /* pointer to response item   */
) ;

LOCAL uint16 dsat707_qctcpd
(
  byte           *rb_ptr,           /* pointer to response buffer */
  uint16                 size,       /* Buffer Size*/
  dsm_item_type  **res_buff_ptr_ptr /* pointer to response item   */
) ;

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCRLPD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPD command.
  Dump RLP protocol statistics.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcrlpd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*--------------------------------------------------------------------------*/
  res_buff_ptr->used = dsat707_qcrlpd(res_buff_ptr->data_ptr,res_buff_ptr->size
                                          &res_buff_ptr);
  return result;
} /* dsat707_exec_qcrlpd_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCRLPR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPR command.
  Reset RLP protocol statistics.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcrlpr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*--------------------------------------------------------------------------*/
  rlp_reset_stats();
  return result;
} /* dsat707_exec_qcrlpr_cmd */


#ifdef FEATURE_DS_RLP3
/*===========================================================================
  FUNCTION DSAT707_EXEC_QCRLPD33_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPD33 command.
  Dump RLP 3 protocol statistics.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcrlpd33_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*--------------------------------------------------------------------------*/
  res_buff_ptr->used = dsat707_qcrlpd33(res_buff_ptr->data_ptr,res_buff_ptr->size
                                            &res_buff_ptr);
  return result;
} /* dsat707_exec_qcrlpd33_cmd */
#endif /* FEATURE_DS_RLP3 */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCRLPR33_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPR33 command.
  Reset RLP 3 protocol statistics.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcrlpr33_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*--------------------------------------------------------------------------*/
  dsrlp_clear_stats(DSRLP_FIXED_SR_ID_INDEX);
  return result;
} /* dsat707_exec_qcrlpr33_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCPPPD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPPPD command.
  Dump PPP protocol statistics.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcpppd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  byte *rb_ptr = res_buff_ptr->data_ptr;
/*--------------------------------------------------------------------------*/
  res_buff_ptr->used = dsat707_qcpppd(res_buff_ptr->data_ptr,
                                          res_buff_ptr->size,
                                          &res_buff_ptr,
                                          mode);
  return result;
} /* dsat707_exec_qcpppd_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCPPPR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPPPR command.
  Reset PPP protocol statistics.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcpppr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*--------------------------------------------------------------------------*/
  ppp_reset_stats();
  return result;
} /* dsat707_exec_qcpppr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCIPD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCIPD command.
  Dump IP protocol statistics.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcipd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  byte *rb_ptr = res_buff_ptr->data_ptr;
/*--------------------------------------------------------------------------*/
  res_buff_ptr->used = dsat707_qcipd(res_buff_ptr->data_ptr,res_buff_ptr->size,
                                         &res_buff_ptr);
  return result;
} /* dsat707_exec_qcipd_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCIPR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCIPR command.
  Reset IP protocol statistics.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcipr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*--------------------------------------------------------------------------*/
  ip_reset_stats();
  icmp_reset_stats();
  return result;
} /* dsat707_exec_qcipr_cmd */


/*===========================================================================
  FUNCTION DSAT707_EXEC_QCUDPD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCUDPD command.
  Dump UDP protocol statistics.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcudpd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  byte *rb_ptr = res_buff_ptr->data_ptr;
/*--------------------------------------------------------------------------*/
  res_buff_ptr->used = dsat707_qcudpd(res_buff_ptr->data_ptr,
                                          &res_buff_ptr);
  return result;
} /* dsat707_exec_qcudpd_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCUDPR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCUDPR command.
  Reset UDP protocol statistics.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qcudpr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*--------------------------------------------------------------------------*/
  udp_reset_stats();
  return result;
} /* dsat707_exec_qcudpr_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCTCPD_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCTCPD command.
  Dump TCP protocol statistics.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qctcpd_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
  byte *rb_ptr = res_buff_ptr->data_ptr;
/*--------------------------------------------------------------------------*/
  res_buff_ptr->used = dsat707_qctcpd(res_buff_ptr->data_ptr,res_buff_ptr->size
                                          &res_buff_ptr);
  return result;
} /* dsat707_exec_qctcpd_cmd */

/*===========================================================================
  FUNCTION DSAT707_EXEC_QCTCPR_CMD

  DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes AT$QCTCPR command.
  Reset TCP protocol statistics.
  
  DEPENDENCIES
  None
  
  RETURN VALUE
  Returns an enum that describes the result of the command execution.
  possible values:
  DSAT_ERROR : if there was any problem in executing the command
  DSAT_OK : if it is a success.
  
  SIDE EFFECTS
  None.
===========================================================================*/
dsat_result_enum_type dsat707_exec_qctcpr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *tab_entry,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *rb_ptr           /*  Place to put response       */
)
{
  dsat_result_enum_type result = DSAT_OK;
/*--------------------------------------------------------------------------*/
  tcp_reset_stats();
  return result;
} /* dsat707_exec_qctcpr_cmd */


/*===========================================================================

FUNCTION DSAT707_BUILD_ASCII_STAT_ARRAY

DESCRIPTION
  Builds an ASCII representation of the given array of statistics names
  and values.  This is copied to the response buffer.
  The array is formatted two per line as follows, and the output is
  terminated with a NULL character:

  <CR><LF>NAME_0:002a NAME_1:3ef1
  <CR><LF>NAME_2:a490 NAME_3:000e
  <CR><LF>NAME_4:0e00 NAME_5:00b0
  '\0'

DEPENDENCIES
   rb_ptr must point to enough space to hold all the values.
   For n values, and names of length m,
     (6+m)n + (n+1)/2 + 1
   bytes are required (includes room for the null terminator).

RETURN VALUE
   rb_ptr incremented by the size of the ASCII representation of
   the array, minus one, so that it points to the final NULL character of
   the string.

SIDE EFFECTS
   None.

===========================================================================*/
byte * dsat707_build_ascii_stat_array
(
  word                     *stat_array,/* array of stats                   */
  const byte *const *const name_array, /* array of pointers to stats names */
  const word               stat_count, /* count of stats in stat_array     */
  byte                     *rb_ptr,    /* pointer to response buffer       */
  dsm_item_type  **res_buff_ptr_ptr,   /* pointer to response item         */
  boolean ppp_stats_and_online         /* ignore netmodel stats if true    */
)
{
  word which_stat; /* Index into the stat array   */
  const byte *cp;  /* Pointer for copying character strings */
  
  /*-----------------------------------------------------------------------*/
  int size = (6 + strlen((const char *)name_array[0])*stat_count
              + (stat_count+1)/2 + 1);
  if ( rb_ptr + size  >
       (*res_buff_ptr_ptr)->data_ptr + (*res_buff_ptr_ptr)->size
)
  {
    /* A more sophisticated implementation could queue the DSM item
       and get a new one, but we won't do that until one of the responses
       grows large enough to require it.
       */
    ERR("Response of %d bytes too big for DSM item",size,0,0);
    *rb_ptr = '\0'; /* don't post-increment the ptr! */
    return rb_ptr;
  }

  /* Iterate through all the elements in stat_array.
     For each statistic, copy its label, then its value, to the
     response buffer. */
  for (which_stat = 0; which_stat < stat_count; which_stat++)
  {
#ifdef FEATURE_DS_NET_MODEL
      /* If we are in online command mode, and we are doing this for PPP then
         we want to ignore the Network Model PPP stats: these fall between
         the range of Um Framed and Rm Total.  This code will break if these
         values are changed and are not the bounds of the Netmodel stats!!!!
      */
    if(ppp_stats_and_online == TRUE &&
       which_stat >= PPP_STATS_UM_FRAMED_PACKETS &&
       which_stat <= PPP_STATS_RM_TOTAL_PACKETS)
    {
      continue;
    }
#endif /* FEATURE_DS_NET_MODEL */
    
    if (which_stat == (which_stat | 1))
    {
      /* Odd-numbered stats are prefixed by one space to separate them
         from the previous stat value on the same line. */
      *rb_ptr++ = ' ';
    }
    else
    {
      /* Even-numbered stats are prefixed by end-of-line to separate them
         from the previous line. */
      
      /* Copy contents of S3 and S4 registers (typically CR and LF)
         to the response buffer. */
      *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
      *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
    }
    
    /* Copy the name of the statistic to the response buffer. */
    cp = name_array[which_stat];
    while ((*rb_ptr++ = *cp++) != '\0') ;
    /* String was generated by compiler and is null-terminated. */
    --rb_ptr; /* Back up over the string's null terminator. */
    
    /* Separate the name and value with a colon. */
    *rb_ptr++ = ':';
    
    /* Copy the ASCII hex representation of the stat value, to the
       response buffer. */
    rb_ptr = dsatutil_xwtoa (stat_array[which_stat], rb_ptr);
  }
  *rb_ptr = '\0'; /* don't post-increment the ptr! */
  
  return rb_ptr;
} /* dsat707_build_ascii_stat_array */


/*===========================================================================

FUNCTION DSAT707_BUILD_ASCII_STAT_ARRAY_UINT32

DESCRIPTION
  Builds an ASCII representation of the given array of statistics names
  and values.  This is copied to the response buffer.
  The array is formatted two per line as follows, and the output is
  terminated with a NULL character:

  <CR><LF>NAME_0:00002a NAME_1:003ef1
  <CR><LF>NAME_2:00a490 NAME_3:00000e
  <CR><LF>NAME_4:000e00 NAME_5:0000b0
  '\0'

DEPENDENCIES
   rb_ptr must point to enough space to hold all the values.
   For n values, and names of length m,
     (10+m)n + (n+1)/2 + 1
   bytes are required (includes room for the null terminator).

RETURN VALUE
   rb_ptr incremented by the size of the ASCII representation of
   the array, minus one, so that it points to the final NULL character of
   the string.

SIDE EFFECTS
   None.

===========================================================================*/
byte * dsat707_build_ascii_stat_array_uint32
(
  uint32                   *stat_array,/* array of stats                   */
  const byte *const *const name_array, /* array of pointers to stats names */
  const word               stat_count, /* count of stats in stat_array     */
  byte                     *rb_ptr,    /* pointer to response buffer       */
  dsm_item_type  **res_buff_ptr_ptr,   /* pointer to response item         */
  boolean ppp_stats_and_online         /* ignore netmodel stats if true    */
)
{
  word which_stat; /* Index into the stat array   */
  const byte *cp;  /* Pointer for copying character strings */

  int size = (10 + strlen((const char *)name_array[0])*stat_count
              + (stat_count+1)/2 + 1);
  /*-----------------------------------------------------------------------*/
  if ( rb_ptr + size  >
       (*res_buff_ptr_ptr)->data_ptr + (*res_buff_ptr_ptr)->size
     )
  {
    /* A more sophisticated implementation could queue the DSM item
       and get a new one, but we won't do that until one of the responses
       grows large enough to require it.
    */
    ERR("Response of %d bytes too big for DSM item",size,0,0);
    *rb_ptr = '\0'; /* don't post-increment the ptr! */
    return rb_ptr;
  }

  /* Iterate through all the elements in stat_array.
     For each statistic, copy its label, then its value, to the
     response buffer.
  */
  for (which_stat = 0; which_stat < stat_count; which_stat++)
  {
#ifdef FEATURE_DS_NET_MODEL
    /* If we are in online command mode, and we are doing this for PPP then
       we want to ignore the Network Model PPP stats: these fall between
       the range of Um Framed and Rm Total.  This code will break if these
       values are changed and are not the bounds of the Netmodel stats!!!!
    */
    if(ppp_stats_and_online == TRUE &&
       which_stat >= PPP_STATS_UM_FRAMED_PACKETS &&
       which_stat <= PPP_STATS_RM_TOTAL_PACKETS)
    {
      continue;
    }
#endif /* FEATURE_DS_NET_MODEL */
    
    if (which_stat == (which_stat | 1))
    {
      /* Odd-numbered stats are prefixed by one space to separate them
         from the previous stat value on the same line. */
      *rb_ptr++ = ' ';
    }
    else
    {
      /* Even-numbered stats are prefixed by end-of-line to separate them
         from the previous line. */
      
      /* Copy contents of S3 and S4 registers (typically CR and LF)
         to the response buffer. */
      *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
      *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
    }
    
    /* Copy the name of the statistic to the response buffer. */
    cp = name_array[which_stat];
    while ((*rb_ptr++ = *cp++) != '\0') ;
    /* String was generated by compiler and is null-terminated. */
    --rb_ptr; /* Back up over the string's null terminator. */
    
    /* Separate the name and value with a colon. */
    *rb_ptr++ = ':';
    
    /* Copy the ASCII hex representation of the stat value, to the
       response buffer. */
    rb_ptr = dsatutil_xitoa (stat_array[which_stat], rb_ptr);
  } /* for */
  
  *rb_ptr = '\0'; /* don't post-increment the ptr! */
  
  return rb_ptr;
} /* dsat707_build_ascii_stat_array_uint32 */

/*===========================================================================

FUNCTION DSAT707_QCRLPD

DESCRIPTION
  Dumps RLP stats to the given location.

DEPENDENCIES
   rb_ptr must point to enough space to hold all the values.

RETURN VALUE
   rb_ptr incremented by the size of the ASCII representation of
   the stats dump, so that it points to the final NULL character of the
   string.

SIDE EFFECTS
   None.

===========================================================================*/
uint16 dsat707_qcrlpd
(
  byte                     *rb_ptr,     /* pointer to response buffer */
  uint16                     size,       /* Buffer Size*/
  dsm_item_type  **res_buff_ptr_ptr     /* pointer to response item   */
)
{
  #define BUF_SIZE() (size < (rb_ptr - base_rb_ptr) ? 0 : (size - (rb_ptr - base_rb_ptr)))

  static const byte lst_cal_sync_lbl_ptr[] = {"Last Call Synced:"};
  word rlp_stats[RLP_STATS_COUNT];
  const byte *t_ptr;
   byte    *base_rb_ptr;

   base_rb_ptr = rb_ptr;
  /*-----------------------------------------------------------------------*/
  
  /* Get a copy of the RLP stats in the local stats buffer */
  rlp_get_stats_at(rlp_stats);
  
  /* Copy an ASCII representation of the stats array, with
     labels, to the response buffer. */
  rb_ptr = build_ascii_stat_array
    (
      rlp_stats,
      rlp_stats_name_ptr,
      RLP_STATS_COUNT,
      rb_ptr,
      res_buff_ptr_ptr,
      FALSE
  );
  /* Copy contents of S3 and S4 registers (typically CR and LF)
     to the response buffer. */
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  
  /* Copy the label for rlp_last_call_synced to the response
     buffer. */
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),
                          (void*)lst_cal_sync_lbl_ptr,sizeof(lst_cal_sync_lbl_ptr));
  rb_ptr += sizeof(lst_cal_sync_lbl_ptr)
    - 1; /* back up over NULL */
  
  /* Copy the value of rlp_last_call_synced to the response
     buffer. */
  t_ptr = rlp_last_call_synced_ptr;
  /* String was generated by compiler and is null-terminated. */
  while ( (*rb_ptr++ = *t_ptr++) != '\0' )
    ;
  rb_ptr--;
  
  return (rb_ptr - base_rb_ptr);
} /* dsat707_qcrlpd */

#ifdef FEATURE_DS_RLP3
/*===========================================================================

FUNCTION DSAT707_QCRLPD33

DESCRIPTION
  Dumps RLP stats to the given location.

DEPENDENCIES
   rb_ptr must point to enough space to hold all the values.

RETURN VALUE
   rb_ptr incremented by the size of the ASCII representation of
   the stats dump, so that it points to the final NULL character of the
   string.

SIDE EFFECTS
   None.

===========================================================================*/
uint16 dsat707_qcrlpd33
(
  byte                     *rb_ptr,     /* pointer to response buffer */
  uint16                     size,       /* Buffer Size*/
  dsm_item_type  **res_buff_ptr_ptr     /* pointer to response item   */
)
{
  #define BUF_SIZE() (size < (rb_ptr - base_rb_ptr) ? 0 : (size - (rb_ptr - base_rb_ptr)))
  static const byte lst_cal_sync_lbl_ptr[] = {"Last Call Synced:"};
  uint32 dsrlp_stats[DSRLP_STATS_COUNT];
  const byte *t_ptr;
  byte    *base_rb_ptr;
  /*-----------------------------------------------------------------------*/
   base_rb_ptr = rb_ptr;
  /* Get a copy of the RLP stats in the local stats buffer */
  dsrlp_get_stats_at((uint32 *)dsrlp_stats, (byte)DSRLP_FIXED_SR_ID_INDEX);
  
  /* Copy an ASCII representation of the stats array, with
     labels, to the response buffer. */
  
  rb_ptr = build_ascii_stat_array_uint32
    (
      dsrlp_stats,
      dsrlp_stats_name_ptr,
      DSRLP_STATS_COUNT,
      rb_ptr,
      res_buff_ptr_ptr,
      FALSE
  );
  /* Copy contents of S3 and S4 registers (typically CR and LF)
     to the response buffer. */
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  
  /* Copy the label for rlp_last_call_synced to the response
     buffer. */
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),
                          (void*)lst_cal_sync_lbl_ptr,sizeof(lst_cal_sync_lbl_ptr));
  rb_ptr += sizeof(lst_cal_sync_lbl_ptr)
    - 1; /* back up over NULL */
  
  /* Copy the value of rlp_last_call_synced to the response
     buffer. */
  t_ptr = dsrlp_last_call_synced_ptr;
  /* String was generated by compiler and is null-terminated. */
  while ( (*rb_ptr++ = *t_ptr++) != '\0' )
    ;
  rb_ptr--;
  
  return (rb_ptr - base_rb_ptr);
} /* dsat707_qcrlpd33 */
#endif /* FEATURE_DS_RLP3 */

/*===========================================================================

FUNCTION DSAT707_QCPPPD

DESCRIPTION
  Dumps PPP stats to the given location.

DEPENDENCIES
   rb_ptr must point to enough space to hold all the values.

RETURN VALUE
   rb_ptr incremented by the size of the ASCII representation of
   the stats dump, so that it points to the final NULL character of the
   string.

SIDE EFFECTS
   None.

===========================================================================*/
uint16 dsat707_qcpppd
(
  byte                      *rb_ptr,     /* pointer to response buffer */
  uint16                     size,       /* buffer size*/
  dsm_item_type  **res_buff_ptr_ptr,     /* pointer to response item   */
  dsat_mode_enum_type       mode     /* what mode are we in?       */
)
{

#define BUF_SIZE() (size < (rb_ptr - base_rb_ptr) ? 0 : (size - (rb_ptr - base_rb_ptr)))

  static const byte in_octets_lbl_ptr[] =      {"InOctets    : "};
  static const byte out_octets_lbl_ptr[] =     {"OutOctets   : "};
  static const byte in_good_octets_lbl_ptr[] = {"InGoodOctets: "};
  word ppp_stats[PPP_STATS_COUNT];
  boolean online_mode;
  byte    *base_rb_ptr;
  /*-----------------------------------------------------------------------*/
   base_rb_ptr = rb_ptr;

  /* Get a copy of the PPP stats in the local stats buffer */
  ppp_get_stats(ppp_stats);

  /* are we in online mode? */
  online_mode = (mode == DSAT_ONLINE_CMD);

  /* Copy an ASCII representation of the stats array, with
     labels, to the response buffer. */

  rb_ptr = build_ascii_stat_array
    (
      ppp_stats,
      ppp_stats_name_ptr,
      PPP_STATS_COUNT,
      rb_ptr,
      res_buff_ptr_ptr,
      online_mode
  );

  /* Copy contents of S3 and S4 registers (typically CR and LF)
     to the response buffer. */
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  
  /* Copy the label for ppp_in_octets to the response buffer  */
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),
                          (void*)in_octets_lbl_ptr,sizeof(in_octets_lbl_ptr));
  rb_ptr += sizeof(in_octets_lbl_ptr) - 1; /* back up over NULL */

  /* Copy the value of ppp_in_octets to the response buffer */
  rb_ptr = dsatutil_xitoa(ppp_in_octets, rb_ptr);
  
  *rb_ptr++ = ' ';
  *rb_ptr++ = ' ';
  
  /* Copy the label for ppp_out_octets to the response buffer */
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),
                          (void*)out_octets_lbl_ptr,sizeof(out_octets_lbl_ptr));
  rb_ptr += sizeof(out_octets_lbl_ptr)- 1; /* back up over NULL */
  
  /* Copy the value of ppp_out_octets to the response buffer */
  rb_ptr = dsatutil_xitoa(ppp_out_octets, rb_ptr);
  
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  
  /* Copy the label for ppp_in_good_octets to the response buffer */
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),
                          (void*)in_good_octets_lbl_ptr,sizeof(in_good_octets_lbl_ptr));
  rb_ptr += sizeof(in_good_octets_lbl_ptr)- 1; /* back up over NULL */
  
  /* Copy the value of ppp_in_good_octets to the response buffer  */
  rb_ptr = dsatutil_xitoa(ppp_in_good_octets, rb_ptr);
  
  return (rb_ptr - base_rb_ptr);
} /* dsat707_qcpppd */

/*===========================================================================

FUNCTION DSAT707_QCIPD

DESCRIPTION
  Dumps IP stats to the given location.

DEPENDENCIES
   rb_ptr must point to enough space to hold all the values.

RETURN VALUE
   rb_ptr incremented by the size of the ASCII representation of
   the stats dump, so that it points to the final NULL character of the
   string.

SIDE EFFECTS
   None.

===========================================================================*/
uint16 dsat707_qcipd
(
  byte                     *rb_ptr,     /* pointer to response buffer */
  uint16                    size,
  dsm_item_type  **res_buff_ptr_ptr     /* pointer to response item   */
)
{

#define BUF_SIZE() (size < (rb_ptr - base_rb_ptr) ? 0 : (size - (rb_ptr - base_rb_ptr)))

  static const byte ip_lbl_ptr[] =   {"IP:"};
  static const byte icmp_lbl_ptr[] = {"ICMP:"};
  word ip_stats[IP_STATS_COUNT];
  byte    *base_rb_ptr;

  base_rb_ptr = rb_ptr;
  /*-----------------------------------------------------------------------*/
  /* Copy contents of S3 and S4 registers (typically CR and LF)
     to the response buffer. */
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  
  /* Copy the label for IP to the response buffer */
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),
                          (void*)ip_lbl_ptr,sizeof(ip_lbl_ptr));

  rb_ptr += sizeof(ip_lbl_ptr)- 1; /* back up over NULL */
  
  /* Get a copy of the RLP stats in the local stats buffer */
  ip_get_stats(ip_stats);
    
  /* Copy an ASCII representation of the stats array, with
     labels, to the response buffer. */
  rb_ptr = build_ascii_stat_array
    (
      ip_stats,
      ip_stats_name_ptr,
      IP_STATS_COUNT,
      rb_ptr,
      res_buff_ptr_ptr,
      FALSE
  );
  /* Copy contents of S3 and S4 registers (typically CR and LF)
     to the response buffer. */
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  
  /* Copy the label for ICMP to the response buffer */
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),(void*)icmp_lbl_ptr,sizeof(icmp_lbl_ptr));
  
  rb_ptr += sizeof(icmp_lbl_ptr) - 1; /* back up over NULL */
  
  word icmp_stats[ICMP_STATS_COUNT];
  
  /* Get a copy of the RLP stats in the local stats buffer */
  icmp_get_stats(icmp_stats);
  
  /* Copy an ASCII representation of the stats array, with
     labels, to the response buffer. */
  
  rb_ptr = build_ascii_stat_array
    (
      icmp_stats,
      icmp_stats_name_ptr,
      ICMP_STATS_COUNT,
      rb_ptr,
      res_buff_ptr_ptr,
      FALSE
  );
  return (rb_ptr - base_rb_ptr);
} /* dsat707_qcipd */

/*===========================================================================

FUNCTION DSAT707_QCUDPD

DESCRIPTION
  Dumps UDP stats to the given location.

DEPENDENCIES
   rb_ptr must point to enough space to hold all the values.

RETURN VALUE
   rb_ptr incremented by the size of the ASCII representation of
   the stats dump, so that it points to the final NULL character of the
   string.

SIDE EFFECTS
   None.

===========================================================================*/
uint16 dsat707_qcudpd
(
  byte                     *rb_ptr,     /* pointer to response buffer */
  dsm_item_type  **res_buff_ptr_ptr     /* pointer to response item   */
)
{
  word udp_stats[UDP_STATS_COUNT];
  byte    *base_rb_ptr;
  
  /*-----------------------------------------------------------------------*/
    base_rb_ptr = rb_ptr;
              
  /* Get a copy of the RLP stats in the local stats buffer */
  udp_get_stats(udp_stats);
  
  /* Copy an ASCII representation of the stats array, with
     labels, to the response buffer. */
  rb_ptr = build_ascii_stat_array
    (
      udp_stats,
      udp_stats_name_ptr,
      UDP_STATS_COUNT,
      rb_ptr,
      res_buff_ptr_ptr,
      FALSE
  );
  return (rb_ptr - base_rb_ptr);
} /* dsat707_qcudpd */

/*===========================================================================
FUNCTION DSAT707_QCTCPD

DESCRIPTION
  Dumps TCP stats to the given location.

DEPENDENCIES
   rb_ptr must point to enough space to hold all the values.

RETURN VALUE
   rb_ptr incremented by the size of the ASCII representation of
   the stats dump, so that it points to the final NULL character of the
   string.

SIDE EFFECTS
   None.

===========================================================================*/
uint16 dsat707_qctcpd
(
  byte                     *rb_ptr,     /* pointer to response buffer */
  uint16          size,
  dsm_item_type  **res_buff_ptr_ptr     /* pointer to response item   */
)
{
  #define BUF_SIZE() (size < (rb_ptr - base_rb_ptr) ? 0 : (size - (rb_ptr - base_rb_ptr)))
  
  static const byte backoffs_lbl_ptr[] = {"Backoffs        : "};
  static const byte tcp_snt_lbl_ptr[] =  {"Payload Sent    : "};
  static const byte tcp_rcv_lbl_ptr[] =  {"Payload Received: "};

  word tcp_stats[TCP_STATS_COUNT];
  int i; /* loop index */
  byte    *base_rb_ptr;

   base_rb_ptr = rb_ptr;
  /*-----------------------------------------------------------------------*/

  /* Get a copy of the RLP stats in the local stats buffer */
  tcp_get_stats(tcp_stats);

  /* Copy an ASCII representation of the stats array, with
     labels, to the response buffer. */
  rb_ptr = build_ascii_stat_array
    (
      tcp_stats,
      tcp_stats_name_ptr,
      TCP_STATS_COUNT,
      rb_ptr,
      res_buff_ptr_ptr,
      FALSE     
  );
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  
  /* Copy the label for tcp_payload_sent to the response buffer */

  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),
                          (void*)tcp_snt_lbl_ptr,sizeof(tcp_snt_lbl_ptr));

  rb_ptr += sizeof(tcp_snt_lbl_ptr)
    - 1; /* back up over NULL */

  /* Copy the value of tcp_payload_sent to the response buffer */
  rb_ptr = dsatutil_xitoa(tcp_payload_sent, rb_ptr);

  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);
  
  /* Copy the label for tcp_payload_received to the response
     buffer */

  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),
                          (void*)tcp_rcv_lbl_ptr,sizeof(tcp_rcv_lbl_ptr));

  rb_ptr += sizeof(tcp_rcv_lbl_ptr)
    - 1; /* back up over NULL */
  
  /* Copy the value of tcp_payload_received to the response
     buffer */
  rb_ptr = dsatutil_xitoa(tcp_payload_received, rb_ptr);

  /* Copy contents of S3 and S4 registers (typically CR and LF)
     to the response buffer. */
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S3_IDX,0,0,NUM_TYPE);
  *rb_ptr++ = (byte) dsatutil_get_val(DSAT_SREG_S4_IDX,0,0,NUM_TYPE);

  /* Copy the label for tcp_backoffs to the response buffer  */
  (void) dsatutil_memscpy((void*)rb_ptr,BUF_SIZE(),
                          (void*)backoffs_lbl_ptr,sizeof(backoffs_lbl_ptr));

  rb_ptr += sizeof(backoffs_lbl_ptr)
    - 1; /* back up over NULL */
  
  /* Copy the values of tcp_backoffs[] to the response buffer */
  
  for (i = 0; i < TCP_BACKOFF_ARRAY_SIZE; i++)
  {
    *rb_ptr++ = "0123456789ABCDEF"[tcp_backoffs[i]];
  }
  *rb_ptr++ = '\0';
  return (rb_ptr - base_rb_ptr);
} /* dsat707_qctcpd */

#endif /* FEATURE_DS_PSTATS   */
#endif /* FEATURE_DATA_IS707  */

