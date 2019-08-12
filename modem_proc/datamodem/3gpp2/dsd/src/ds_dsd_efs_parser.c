/*===========================================================================
                        DSD EFS PARSER

GENERAL DESCRIPTION
  All the declarations and definitions necessary to support parsing
  of DSD related EFS file.

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Copyright (c) 2005-2011 by Qualcomm Technologies Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================
                           EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/dsd/src/ds_dsd_efs_parser.c#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $


when        who    what, where, why
--------    ---    ----------------------------------------------------------
11/07/11    var    Checkin for first version of dsd_efs_parser.c
===========================================================================*/


/*===========================================================================
                          INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "msg.h"
#include "data_msg.h"
#include "dsutil.h"
#include "ds_dsd_efs_parser.h"
#include "ds_dsd_pref_sys.h"
#include "ds_dsd_common.h"
#include <stringl/stringl.h>

/* feature not defined */
#if 0
/*===========================================================================
                              GLOBAL VARIABLES
===========================================================================*/
#define DSD_PREF_ORDER_LIST_FILE "/data/dsd_pref_order_list.txt"


/* Data structure to store the preferred order list read from the EFS file*/


/*------------------------------------------------------------------------
Preferred system list.
------------------------------------------------------------------------*/
extern dsd_pref_sys_order_type dsd_pref_ordered_list[DS_DSD_MAX_RADIO];

/*===========================================================================
                      INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DSD_ATOI

DESCRIPTION    Converts the character input to integer type

PARAMETERS    x - Begin pointer (char *)
              y - End pointer (char *)

DEPENDENCIES  None

RETURN VALUE  ouput integer

SIDE EFFECTS  None
===========================================================================*/
int dsd_atoi
(
  char* x,
  char* y
)
{
  int z = 0;
  boolean is_valid = TRUE;

  while( x < y )
  {
    if((*x) >= '0' && (*x) <= '9')
    {
      z = z *10 + (*x- '0');
      x++;
    }
    else
    {
      is_valid = FALSE;
      break;
    }
  }

  if(!is_valid)
  {
    return -1;
  }
  return z;
} /* DSD_ATOI() */



/*===========================================================================
FUNCTION      DSD_FILL_PREF_ORDER_LIST_WITH_TOKEN_CONTENTS

DESCRIPTION   This function gets the data for each token and populates the
              structure with the appropriate data that corresponds to the
              token number.

PARAMETERS    token_id -  describes the current token which is
                              being populated into the EFS structure

              char *from, *to - start and end of the character array which
                          holds the data to populate the profile structure

              dsd_pref_order_list_type * - pointer to the pref order list
                          structure that is being populated

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
int16 dsd_fill_pref_order_list_with_token_contents
(
  uint8                   token_id,
  char                    *from,
  char                    *to,
  dsd_pref_sys_order_type *pref_order
)
{
  int                 atoi_result;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(token_id < DS_DSD_MAX_RADIO)
  {
    atoi_result = dsd_atoi(from,to);

    if(atoi_result < 0)
    {
      DATA_MSG0(MSG_LEGACY_ERROR,"Error parsing priority value in EFS file.");
      return -1;
    }

    pref_order->priority        = atoi_result;
  }
  else
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"Incorrect Radio.");
    return -1;
  }
  return 0;
} /* DSD_FILL_PREF_ORDER_LIST_WITH_TOKEN_CONTENTS() */


/*===========================================================================
FUNCTION      dsd_pref_order_list_get_token_id

DESCRIPTION   This function returns the token id associated with each
              radio technology

PARAMETERS    char *from - start of the token (param name)
              char *to   - end of the token (param name)

DEPENDENCIES  None

RETURN VALUE  0 - success
              1 - failure

SIDE EFFECTS  None
===========================================================================*/
int16 dsd_pref_order_list_get_token_id
(
  char  *from,
  char  *to,
  uint8 *token_id
)
{
  int16 ret_val = -1; /* return value */
  uint8 i;            /* counter for number of radio techs */
  uint8 length;       /* length of the token (name) */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if  ( from > to )
  {
    return ret_val;
  }

  /* Get the length of the string that is passed */
  /* Assumption that the token length will not be greater than 255 */
  length =(uint8)(to-from);

  for (i=0; i<DS_DSD_MAX_RADIO; i++)
  {
    if (length == strlen(dsd_pref_ordered_list[i].tech_name_string))
    {
      if (0 == strncasecmp(from,
                           dsd_pref_ordered_list[i].tech_name_string,
                           length))
      {
        *token_id = i;
        ret_val = 0;
        break;
      }
    }
  }
  return ret_val;
} /* DSD_PREF_ORDER_LIST_GET_TOKEN_ID() */


/*===========================================================================
FUNCTION      DSD_PARSE_PREF_ORDER_LIST_FROM_EFS

DESCRIPTION   This function will parse the efs file and read the
              preferred order list.

              EFS File Format
              Technology:Priority;

              For example
              LTE:0;

PARAMETERS    efs_db - structure that contains info about the EFS file


DEPENDENCIES  None

RETURN VALUE  0 - success
              1 - failure

SIDE EFFECTS  None
===========================================================================*/
void dsd_parse_pref_order_list_from_efs
(
  ds_efs_token_type* efs_db
)
{
  char                                   *from, *to;
  ds_efs_token_parse_status_enum_type    ret_val  = DS_EFS_TOKEN_PARSE_SUCCESS;
  uint8                                  token_id = 0;
  boolean                                token_id_valid = FALSE;

  /* flag to keep track of the parser state
     TRUE - parser expects Token Name in the EFS
     FALSE - parser expects Token Val in the EFS*/
  boolean                                pref_radio_tech_name = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Set the seperator as : */
  efs_db->seperator = ':';

  while( (DS_EFS_TOKEN_PARSE_EOF
           != (ret_val = ds_efs_tokenizer(efs_db, &from, &to))))
  {
    /*------------------------------------------------------------------------
      Token being read. from points to the beginning of the token and
      to points to the (end of the token+1).

      e.g: string= "token;" => from points to 't' and to points to ';'

      The tokenizer automatically skips blank lines and comments (lines
      beginning with #, so no need to check for them here).
    ------------------------------------------------------------------------*/
    if((from == to) || (DS_EFS_TOKEN_PARSE_EOL == ret_val))
    {
      /*----------------------------------------------------------------------
        Skip empty tokens.
      ----------------------------------------------------------------------*/
      /* If priority value is empty, reset the flag & seperator*/
      if(!pref_radio_tech_name)
      {
        pref_radio_tech_name = TRUE;
        efs_db->seperator = ':';
      }
      continue;
    }
    else if(DS_EFS_TOKEN_PARSE_SUCCESS == ret_val)
    {
      /*---------------------------------------------------------------------
      check if we are looking for pref radio tech name or priority
      ---------------------------------------------------------------------*/
      if (pref_radio_tech_name == TRUE)
      {
        /*------------------------------------------------------------------
        get the token identifier for this radio tech
        ------------------------------------------------------------------*/
        if (dsd_pref_order_list_get_token_id(from,to,&token_id) < 0)
        {
          /* This is an error scenario, Skip till the end of the line? */
          DATA_MSG0(MSG_LEGACY_ERROR,"Incorrect Param Name");
          token_id_valid = FALSE;
        }
        else
        {
          /*-----------------------------------------------------------------
          set param_name as FALSE This means the next token is a
          param value
          -----------------------------------------------------------------*/
          pref_radio_tech_name = FALSE;
          token_id_valid = TRUE;
          /* set the seperator as ; */
          efs_db->seperator = ';';
        }

      }
      /*---------------------------------------------------------------------
      This means that the token is a priority value
      ---------------------------------------------------------------------*/
      else
      {
        /*-------------------------------------------------------------------
         pass in the identifier and value to fill the preferredorder list
         structure
         ------------------------------------------------------------------*/
        if(token_id_valid)
        {
          if(dsd_fill_pref_order_list_with_token_contents(
              token_id,
              from,to,
              &dsd_pref_ordered_list[token_id]) != 0)
          {
            /* This is an error scenario, Skip till the end of the line? */
            DATA_MSG0(MSG_LEGACY_ERROR,"EFS ERROR");
          }
        }
        /*-------------------------------------------------------------------
         set pref_radio_tech_name as TRUE This means that next token is a
         radio tech name
        -------------------------------------------------------------------*/
        pref_radio_tech_name = TRUE;
        /* Set the seperator as : */
        efs_db->seperator = ':';
      }
    }
  } /* end of while loop */

  return;
} /* DSD_PARSE_PREF_ORDER_LIST_FROM_EFS() */

/*===========================================================================
FUNCTION      DSD_READ_PREF_ORDER_LIST_FROM_EFS

DESCRIPTION   This function will read from the efs file the preferred
              order list.

PARAMETERS    None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void dsd_read_pref_order_list_from_efs(void)
{
  ds_efs_token_type efs_db;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Open the EFS file */
  if(ds_efs_file_init(DSD_PREF_ORDER_LIST_FILE, &efs_db) == 0)
  {
    /* Populate the data_sess_profile structure
    with data within efs file*/
    dsd_parse_pref_order_list_from_efs(&efs_db);

    /* Close the EFS file */
    ds_efs_file_close(&efs_db);
  }
  else
  {
    DATA_MSG0(MSG_LEGACY_ERROR,"Unable to open EFS file");
  }

  return;
} /* DSD_READ_PREF_ORDER_LIST_FROM_EFS() */


/*===========================================================================
                      EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS_DSD_EFS_PARSER_INIT

DESCRIPTION   This function initializes the common system. This is during
              power up.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  1. Updates the preferred system
              2. Notifes all clients if the preferred system changes
===========================================================================*/
void ds_dsd_efs_parser_init(void)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  DATA_MSG0(MSG_LEGACY_HIGH,"ds_dsd_efs_parser_init");
  dsd_read_pref_order_list_from_efs();
  return;
}/* ds_dsd_efs_parser_init() */
#endif

