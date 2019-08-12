/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

              F T M  H W T C  D V T  S T R I N G  M O D U L E

GENERAL DESCRIPTION
  This is the FTM HWTC DVT string module, contains command string 
  related utilities and tests prototypes.

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2002 by Qualcomm Technologies INCORPORATED.
  Copyright (c) 2003-2009 by Qualcomm Technologies INCORPORATED.
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
 
                           EDIT HISTORY FOR MODULE
                             
  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/rfa/rf/common/hwtc/inc/ftm_hwtc_dvt.h#1 $ 
  $DateTime: 2016/12/13 07:59:52 $ 
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/26/11   sar     Removed AEE library prototypes.
01/06/10   sar     Renamed ftm_task.h to rf_test_task_v.h.
12/09/09   ntn     Added external prototype for std_strlcpy() to supress warnings 
09/25/09   aro     Fixed "PACKED" usage to be compatible with QDSP6
07/29/04   lcl     Sync up to 1x changes
03/16/03   wd      Removed prototype for ftm_hwtc_dvt_str_printf function.
03/04/03   wd      Initial release
===========================================================================*/
#ifndef FTM_HWTC_DVT_H
#define FTM_HWTC_DVT_H


/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "ftm_hwtc_cmd.h"
#include "ftm_hwtc_dispatch.h"
#include "diagpkt.h"


#include "ftmdiag.h"
#include "ftm.h"
#include "rf_test_task_v.h"

/*===========================================================================
 
                            DATA DECLARATIONS

===========================================================================*/

/* HWTC DVT string request packet type */
typedef PACK(struct) {
  diagpkt_subsys_header_type header;
  ftm_cmd_header_type ftm_cmd_header;
  char dvt_string[1];
} ftm_hwtc_dvt_pkt_type;


/* HWTC DVT String command prototype */
typedef char* (ftm_hwtc_dvt_func_ptr_type)(char*);

/* command string user table entry type */
typedef struct
{
  char *cmd_name;
  ftm_hwtc_dvt_func_ptr_type *func_ptr;
}
ftm_hwtc_dvt_user_table_entry_type;

/* this type is intended to be used by the FTM_HWTC_DVT_DISPATCH_TABLE_REGISTER 
   macro. Do not used by itself */
typedef struct
{
  word  user_table_entry_count;
  const ftm_hwtc_dvt_user_table_entry_type *user_table;
} ftm_hwtc_dvt_master_table_type;


/* basic command's switches */
#define DVT_HELP_DESC     "-help_desc"
#define DVT_HELP          "-h"

/*===========================================================================

                                  MACROS

===========================================================================*/
#define FTM_HWTC_DVT_DISPATCH_TABLE_REGISTER(xx_entry) \
    do { \
      static const ftm_hwtc_dvt_master_table_type xx_entry##_table = { \
        sizeof (xx_entry) / sizeof (xx_entry[0]), xx_entry \
      }; \
      ftm_hwtc_dvt_tbl_reg (&xx_entry##_table); \
    } while (0)

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION       FTM_HWTC_DVT_INIT

DESCRIPTION
  This function initializes the master DVT string table. 
               
DEPENDENCIES
  none

===========================================================================*/
void ftm_hwtc_dvt_init (void);

/*===========================================================================

FUNCTION       FTM_HWTC_DVT_TBL_REG

DESCRIPTION
  This function registers a user DVT string command table with the master
  DVT string table. 
               
DEPENDENCIES
  none

===========================================================================*/
void ftm_hwtc_dvt_tbl_reg (const ftm_hwtc_dvt_master_table_type *tbl_ptr);

/*===========================================================================

FUNCTION       FTM_HWTC_DVT_FIND_CMD

DESCRIPTION
  This function searches the master DVT string table for a given command.
               
DEPENDENCIES
  none

===========================================================================*/
void ftm_hwtc_dvt_find_cmd (ftm_hwtc_dvt_user_table_entry_type *cmd_table_entry);

/*===========================================================================

FUNCTION FTM_HWTC_DVT_DISPATCH

DESCRIPTION
   This function dispatches the appropiate a DVT String function for the 
   requested operation.

DEPENDENCIES
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_hwtc_dvt_dispatch(ftm_hwtc_dvt_pkt_type* cmd_ptr);

/*===========================================================================

FUNCTION       FTM_HWTC_DVT_STRLWR

DESCRIPTION
  This function converts a given string from upper case to lower case. 
               
DEPENDENCIES
  none.

===========================================================================*/
char *ftm_hwtc_dvt_strlwr(char *string);

/*===========================================================================

FUNCTION       FTM_HWTC_DVT_STRLWR

DESCRIPTION
  This function converts a given string from lower case to upper case. 
               
DEPENDENCIES
  none.

===========================================================================*/
char *ftm_hwtc_dvt_strupr(char *string);

/*===========================================================================

FUNCTION       FTM_HWTC_DVT_IS_WHITESPACE

DESCRIPTION
  This function returns true or false depending if the input var is
  whitespace or not. 
               
DEPENDENCIES
  None.

===========================================================================*/
boolean ftm_hwtc_dvt_is_whitespace(char t);

/*===========================================================================

FUNCTION       FTM_HWTC_DVT_GET_STR_PARM_CS

DESCRIPTION
  This function looks for a given parameter in a string and returns the
  value assigned to it. This function is case sensitive. The parameter 
  should be in the following form (ignoring white-space).
  
  parm = value

  For Example:
  string = "cmd x=34 yy =test zzzz = 423x"
  ftm_hwtc_dvt_get_int_parm_cs(string,"x",i, sizeof(i)) then i=34, function returns TRUE.
  ftm_hwtc_dvt_get_int_parm_cs(string,"yy",i, sizeof(i)) then i=test, function returns TRUE.
  ftm_hwtc_dvt_get_int_parm_cs(string,"zzzz",i, sizeof(i)) then i=423x, function returns TRUE.
  ftm_hwtc_dvt_get_int_parm_cs(string,"xyz",i, sizeof(i)) then i=?, function returns FALSE.

DEPENDENCIES
  String must be null terminated.

===========================================================================*/
boolean ftm_hwtc_dvt_get_str_parm_cs(const char *string, const char *parm_name, char *value, int value_size);

/*===========================================================================

FUNCTION       FTM_HWTC_DVT_GET_STR_PARM

DESCRIPTION
  This function looks for a given parameter in a string and returns the
  value assigned to it. This function is NOT case sensitive. The parameter 
  should be in the following form (ignoring white-space).
  
  parm = value

  For Example:
  string = "cmd x=34 yy =test zzzz = 423x"
  ftm_hwtc_dvt_get_int_parm(string,"x",i, sizeof(i)) then i=34, function returns TRUE.
  ftm_hwtc_dvt_get_int_parm(string,"yy",i, sizeof(i)) then i=test, function returns TRUE.
  ftm_hwtc_dvt_get_int_parm(string,"zzzz",i, sizeof(i)) then i=423x, function returns TRUE.
  ftm_hwtc_dvt_get_int_parm(string,"xyz",i, sizeof(i)) then i=?, function returns FALSE.

DEPENDENCIES
  String must be null terminated.

===========================================================================*/
boolean ftm_hwtc_dvt_get_str_parm(const char *string, const char *parm_name, char *value, int value_size);


/*===========================================================================

FUNCTION       FTM_HWTC_DVT_GET_DOUBLE_PARM_CS

DESCRIPTION
  This function looks for a given parameter in a string and returns the
  value assigned to it. This function is case sensitive. The parameter 
  should be in the following form (ignoring white-space).
  
  parm = value

  For Example:
  string = "cmd x=34 yy =12.34 zzZZ = 423.34x"
  ftm_hwtc_dvt_get_double_parm_cs(string,"x",i) then i=34, function returns TRUE.
  ftm_hwtc_dvt_get_double_parm_cs(string,"yy",i) then i=12.34, function returns TRUE.
  ftm_hwtc_dvt_get_double_parm_cs(string,"zzZZ",i) then i=423.34, function returns TRUE.
  ftm_hwtc_dvt_get_double_parm_cs(string,"zzzz",i) then i=?, function returns FALSE.
  ftm_hwtc_dvt_get_double_parm_cs(string,"xyz",i) then i=?, function returns FALSE.

               
DEPENDENCIES
  String must be null terminated.

===========================================================================*/
boolean ftm_hwtc_dvt_get_double_parm_cs(const char *string, const char *parm_name, double *value);

/*===========================================================================

FUNCTION       FTM_HWTC_DVT_GET_DOUBLE_PARM

DESCRIPTION
  This function looks for a given parameter in a string and returns the
  value assigned to it. This function is NOT case sensitive. The parameter 
  should be in the following form (ignoring white-space).
  
  parm = value

  For Example:
  string = "cmd x=34 yy =12.34 zzZZ = 423.34x"
  ftm_hwtc_dvt_get_double_parm_cs(string,"x",i) then i=34, function returns TRUE.
  ftm_hwtc_dvt_get_double_parm_cs(string,"yy",i) then i=12.34, function returns TRUE.
  ftm_hwtc_dvt_get_double_parm_cs(string,"zzZZ",i) then i=423.34, function returns TRUE.
  ftm_hwtc_dvt_get_double_parm_cs(string,"zzzz",i) then i=423.34, function returns TRUE.
  ftm_hwtc_dvt_get_double_parm_cs(string,"xyz",i) then i=?, function returns FALSE.

               
DEPENDENCIES
  String must be null terminated.

===========================================================================*/
boolean ftm_hwtc_dvt_get_double_parm(const char *string, const char *parm_name, double *value);



/*===========================================================================

FUNCTION       FTM_HWTC_DVT_GET_INT_PARM_CS

DESCRIPTION
  This function looks for a given parameter in a string and returns the
  value assigned to it. This function is case sensitive. The parameter 
  should be in the following form (ignoring white-space).
  
  parm = value

  For Example:
  string = "cmd x=34 yy =12.34 zzZZ = 423.34x"
  ftm_hwtc_dvt_get_double_parm_cs(string,"x",i) then i=34, function returns TRUE.
  ftm_hwtc_dvt_get_double_parm_cs(string,"yy",i) then i=12, function returns TRUE.
  ftm_hwtc_dvt_get_double_parm_cs(string,"zzZZ",i) then i=423, function returns TRUE.
  ftm_hwtc_dvt_get_double_parm_cs(string,"zzzz",i) then i=?, function returns FALSE.
  ftm_hwtc_dvt_get_double_parm_cs(string,"xyz",i) then i=?, function returns FALSE.

               
DEPENDENCIES
  String must be null terminated.

===========================================================================*/
boolean ftm_hwtc_dvt_get_int_parm_cs(const char *string, const char *parm_name, int *value);

/*===========================================================================

FUNCTION       FTM_HWTC_DVT_GET_INT_PARM

DESCRIPTION
  This function looks for a given parameter in a string and returns the
  value assigned to it. This function is NOT case sensitive. The parameter 
  should be in the following form (ignoring white-space).
  
  parm = value

  For Example:
  string = "cmd x=34 yy =12.34 zzZZ = 423.34x"
  ftm_hwtc_dvt_get_double_parm_cs(string,"x",i) then i=34, function returns TRUE.
  ftm_hwtc_dvt_get_double_parm_cs(string,"yy",i) then i=12, function returns TRUE.
  ftm_hwtc_dvt_get_double_parm_cs(string,"zzZZ",i) then i=423, function returns TRUE.
  ftm_hwtc_dvt_get_double_parm_cs(string,"zzzz",i) then i=423, function returns TRUE.
  ftm_hwtc_dvt_get_double_parm_cs(string,"xyz",i) then i=?, function returns FALSE.
  
               
DEPENDENCIES
  String must be null terminated.

===========================================================================*/
boolean ftm_hwtc_dvt_get_int_parm(const char *string, const char *parm_name, int *value);

/*===========================================================================

FUNCTION       FTM_HWTC_DVT_FIND_PARM

DESCRIPTION
  This function looks for a given parameter (more like a substring with
  whitespace before and after it). And returns the zero-base index to that 
  parameter. Function is case sensitive.
  
  parm = value

  For Example:
            0123456789012345678901234567890123 
  string = "cmd x=34 yy =12.34 zzZZ = 423.34x"
  ftm_hwtc_dvt_find_parm_cs(string,"x",i) then i=4, function returns TRUE.
  ftm_hwtc_dvt_find_parm_cs(string,"yy",i) then i=9, function returns TRUE.
  ftm_hwtc_dvt_find_parm_cs(string,"zzZZ",i) then i=19, function returns TRUE.
  ftm_hwtc_dvt_find_parm_cs(string,"zzzz",i) then i=?, function returns FALSE.
  ftm_hwtc_dvt_find_parm_cs(string,"xyz",i) then i=?, function returns FALSE.
  
               
DEPENDENCIES
  String must be null terminated.

===========================================================================*/
boolean ftm_hwtc_dvt_find_parm_cs(const char *string, const char *parm_name, int *index /* can be NULL */);

/*===========================================================================

FUNCTION       FTM_HWTC_DVT_FIND_PARM

DESCRIPTION
  This function looks for a given parameter (more like a substring with
  whitespace before and after it). And returns the zero-base index to that 
  parameter. Function is NOT case sensitive.
  
  parm = value

  For Example:
            0123456789012345678901234567890123 
  string = "cmd x=34 yy =12.34 zzZZ = 423.34x"
  ftm_hwtc_dvt_find_parm(string,"x",i) then i=4, function returns TRUE.
  ftm_hwtc_dvt_find_parm(string,"yy",i) then i=9, function returns TRUE.
  ftm_hwtc_dvt_find_parm(string,"zzZZ",i) then i=19, function returns TRUE.
  ftm_hwtc_dvt_find_parm(string,"zzzz",i) then i=19, function returns TRUE.
  ftm_hwtc_dvt_find_parm(string,"xyz",i) then i=?, function returns FALSE.
  
               
DEPENDENCIES
  String must be null terminated.

===========================================================================*/
boolean ftm_hwtc_dvt_find_parm(const char *string, const char *parm_name, int *index /* can be NULL */);

/*===========================================================================

FUNCTION       FTM_HWTC_DVT_DO_RSP_PKT

DESCRIPTION
  This fucntion is used to tell the DVT string engine not to send a rsp_pkt
  since probably the dvt_cmd handler already returned it's own.
               
DEPENDENCIES
  None.

===========================================================================*/
void ftm_hwtc_dvt_do_rsp_pkt(boolean doit);

#endif /* FTM_HWTC_DVT_H */
