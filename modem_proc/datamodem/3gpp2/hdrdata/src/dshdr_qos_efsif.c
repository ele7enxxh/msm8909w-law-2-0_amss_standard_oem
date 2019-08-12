/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                      D S H D R _ Q O S _ E F S I F

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2004-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/hdrdata/src/dshdr_qos_efsif.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who        what, where, why
--------   ---      -------------------------------------------------------
06/26/13   sc       Replace all the memcpy and memmove with memscpy and 
                    memsmove as its safer.
05/12/11   op       Migrated to MSG 2.0 macros
04/16/10   ls       Change dshdr_efs* to ds_efs*
07/04/09   pp       hton/ntoh* macros replaced with ps_hton/ntoh*.
02/10/05   sy       Fixed issue where the port is 0 and port range exist.
01/31/05   sy       Fixed the issues in EFS file for specifying the
                    port and port range.
01/26/05   msr      Converting destination port to network order.
10/18/04   sy       Fixed a problem where the number of records are not read
                    correctly.
09/15/04   sy       Created module.


===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "data_msg.h"
#include <stringl/stringl.h>

#ifdef FEATURE_HDR_QOS
#include "dshdr_qos_efsif.h"
#include "ds3gcfgmgr.h"
#include "msg.h"
#include "ps_in.h"


/*-----------------------------------------------------------------------------
  Enum to specify the order in which the data need to read from the parsed file.
  The various fields of the record are seperated by semicolon in the code.

    Note : All these fields should be specified in one row. If a new line is hit
  then we should consider the end of record.

-----------------------------------------------------------------------------*/
typedef enum
{
 DSHDR_QOS_DEST_IP_ADDRESS  = 0,
 DSHDR_QOS_DEST_SUBNET_MASK = 1,
 DSHDR_QOS_RLP_PRTCL        = 2,
 DSHDR_QOS_DEST_PORT        = 3,
 DSHDR_QOS_DEST_PORT_RANGE  = 4,
 DSHDR_QOS_RLP_REV_FLOW_ID  = 5
}dshdr_qos_field_id_enum_type;




#define DSHDR_QOS_PROTOCOL_LEN     4
#define DSHDR_QOS_DOT_DEC_IP_LEN   15

/*===========================================================================

MACRO       DSHDR_QOS_EFSIF_ATOI

DESCRIPTION    Converts the character input to integer type

PARAMETERS     x : Begin pointer
               y : End pointer
               z : ouput pinter

DEPENDENCIES   NONE

RETURN VALUE   NONE

SIDE EFFECTS   NONE
===========================================================================*/
/*--------------------------------------------------------------------------
 We need to make sure we don't consider carriage return as some editors have
 both \r\n and some have only \n. Make sure it gets skipped. 
 --------------------------------------------------------------------------*/
#define DSHDR_QOS_EFSIF_ATOI(x, y, z)\
         while((x) < (y) && (*x != '\r') )           \
        {                         \
          z = ((z)*10 + ((*x)- '0'));  \
          (x)++;                    \
        }


/*===========================================================================
        Debug Messages
===========================================================================*/
#define MSG_DEBUG_ERROR(x, a, b, c)  MSG_ERROR(x, a, b, c)
#define MSG_DEBUG_HIGH(x, a, b, c)   MSG_HIGH(x, a, b, c)
//#define MSG_DEBUG_MED(x, a, b, c)    MSG_MED(x, a, b, c)
//#define MSG_DEBUG_LOW(x, a, b, c)    MSG_LOW(x, a, b, c)


/*===========================================================================
        Local functions
===========================================================================*/
static dshdr_qos_ret_val_enum_type dshdr_qosi_read_file
(
  ds3gcfgmgr_efs_token_type *sm,
  dshdr_qos_file_record_type *record
);

/*===========================================================================
        Local parameters
===========================================================================*/
STATIC  dshdr_qos_file_record_type dshdr_qos_frecord[DSHDR_QOS_MAX_QOS_RECORDS];
                       /* EFS file records has the format of this stucture  */
STATIC uint32 dshdr_qos_max_rec_read = 0;
                      /* Maximum records read from the EFS file successfully*/


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DSHDR_QOS_GET_MRLP_MAX_RECORDS

DESCRIPTION   The function gets the maximum number of records presents in the
              EFS file.

DEPENDENCIES  None.

RETURN VALUE
              0 : No records.
              n : Number of records successfully read from the EFS file.

SIDE EFFECTS  None.
===========================================================================*/

uint32 dshdr_qos_get_mrlp_total_records(void)
{
 return dshdr_qos_max_rec_read;
}

/*===========================================================================
FUNCTION      DSHDR_QOS_GET_MRLP_REC

DESCRIPTION   The function gets the nth record present.

DEPENDENCIES  None.

RETURN VALUE

SIDE EFFECTS  None.
===========================================================================*/
int dshdr_qos_get_mrlp_rec(dshdr_qos_file_record_type* record, uint32 i)
{
  /* copy the contents of the record i.*/
  if(i > dshdr_qos_max_rec_read)
  {
    DATA_HDR_MSG0(MSG_LEGACY_ERROR, "Record is out of range");
    return -1;
  }
  memscpy(record, sizeof(dshdr_qos_file_record_type), 
          &(dshdr_qos_frecord[i]), sizeof(dshdr_qos_file_record_type));
  return 0;

}

/*===========================================================================
FUNCTION      DSHDR_QOS_POWERUP_INIT

DESCRIPTION   The function intializes the state machine and
              also opens the file

DEPENDENCIES  None.

RETURN VALUE
              0  : DSHDR_QOS_SUCCESS: The file is good, readable,
                            State Machine Initialized.
              -1 : DSHDR_QOS_FAILURE: The file cannot be opened/ readable.

SIDE EFFECTS  None.
===========================================================================*/

uint32 dshdr_qos_powerup_init( void )
{
  int i;
  ds3gcfgmgr_efs_token_type fparser;
  dshdr_qos_ret_val_enum_type ret_val = DSHDR_QOS_SUCCESS;
  uint32 dshdr_qos_max_rec_read_before_fetch = 0;
  boolean dumped_record = FALSE;
  /*-------------------------------------------------------------------------*/

  if( ds3gcfgmgr_efs_file_init ("/hdr/fltr_spec.txt", &fparser) == -1 )
  {
    DATA_HDR_MSG0(MSG_LEGACY_ERROR, "Exiting OUT");
    return 0;
  }

  for( i = 0;
      (i < DSHDR_QOS_MAX_QOS_RECORDS) && (DSHDR_QOS_EOF != ret_val);
      i++ )
  {
    dshdr_qos_max_rec_read_before_fetch = dshdr_qos_max_rec_read;
    if(dumped_record && i > 0) 
    {
      i -= 1;
      dumped_record = FALSE;
    }
    ret_val = dshdr_qosi_read_file(&fparser, &dshdr_qos_frecord[i]);
    if(dshdr_qos_max_rec_read_before_fetch == dshdr_qos_max_rec_read)
    {
      /* Dumped the record for whatever reason. */
      dumped_record = TRUE;
    }
  }

  ds3gcfgmgr_efs_file_close(&fparser);

  DATA_HDR_MSG1(MSG_LEGACY_HIGH, " Number of records : %d",i);

  return dshdr_qos_max_rec_read;
}


/*===========================================================================
FUNCTION      DSHDR_QOSI_EFSIF_IPATOI

DESCRIPTION   This function converts a ip address from string to integer.

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS
===========================================================================*/

static int dshdr_qosi_efsif_ipatoi( const char *ip, uint32 *addr )
{
  int i;
  byte ip_octet;
  *addr = 0;                       /* Initialize the parameter; */
  for( i = 0, ip_octet = 0; i < 4; ip++ )
  {
    if( *ip == '\0' )  /* Have we reached the end of the string */
    {
      *addr = *addr << 8 | ip_octet;

      /* Need to store the field in the correct format */
      *addr = ps_ntohl(*addr);
      return -1;
    }
    else if( *ip == '.' )  /* pick one octet at a time */
    {
      *addr = *addr << 8 | ip_octet;
      i++;
      ip_octet = 0;
    }
    else
    {
      /*--------------------------------------------------------------------
       Need to add a check for non integers in the ip_address??
      ---------------------------------------------------------------------*/
      /*lint -save -e734*/
      ip_octet = ip_octet * 10 + *ip - '0';
      /*lint -restore*/
    }
  }

  /* Need to store the field in the correct format */
  *addr = ps_ntohl(*addr); 
  return 0;
}


/*===========================================================================
FUNCTION      DSHDR_QOSI_READ_FILE

DESCRIPTION   The function reads the file and assign the token to the
              appropriate variables.

DEPENDENCIES  The file should have opened already.

RETURN VALUE

SIDE EFFECTS  The order of parameters in the enum is the order in
              which data is read. This order should match the order
              in which the data is placed int he efs file.
===========================================================================*/
static dshdr_qos_ret_val_enum_type dshdr_qosi_read_file
(
  ds3gcfgmgr_efs_token_type *sm,
  dshdr_qos_file_record_type *record
)
{
  int copy_len, i;
  char *from, *to;
  ds3gcfgmgr_efs_token_parse_status_type  ret_val;
  dshdr_qos_field_id_enum_type field_id;
  char ipadd[DSHDR_QOS_DOT_DEC_IP_LEN+1];
  boolean bad_record = FALSE;
  boolean skipped_port = FALSE;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  field_id = DSHDR_QOS_DEST_IP_ADDRESS;
  ret_val = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
  /*-------------------------------------------------------------------------
    Parse each token at a time and assign it to the appropriate variable
   ------------------------------------------------------------------------*/
  while ( DS3GCFGMGR_EFS_TOKEN_PARSE_EOF != (ret_val = ds3gcfgmgr_efs_tokenizer(sm, &from, &to )))
  {
    /*------------------------------------------------------------------------
      Token being read. from points to the beginning of the token and
      to point to the end of the token.
     ------------------------------------------------------------------------*/
    if(DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS == ret_val)
    {
      copy_len = 0;
      i = 0;
      switch(field_id)
      {
      case DSHDR_QOS_DEST_IP_ADDRESS:
        ipadd[0] = '\0';
        copy_len = MIN(DSHDR_QOS_DOT_DEC_IP_LEN, to-from);

        for(i = 0; i < copy_len; i++)
        {
          ipadd[i] = from[i];
        }
        ipadd[copy_len] = '\0';
        (void)dshdr_qosi_efsif_ipatoi(ipadd, &record->ip_address.addr.v4);
        field_id = DSHDR_QOS_DEST_SUBNET_MASK;
        break;

      case DSHDR_QOS_DEST_SUBNET_MASK:
        ipadd[0]='\0';
        copy_len = MIN(DSHDR_QOS_DOT_DEC_IP_LEN, to-from);
        for(i = 0; i < copy_len; i++)
        {
          ipadd[i] = from[i];
        }
        ipadd[copy_len] = '\0';
        if(copy_len == 0)
        { 
          ipadd[i] = 0xFF;
        }
        (void)dshdr_qosi_efsif_ipatoi(ipadd, &record->subnet_mask.addr.v4);
        field_id = DSHDR_QOS_RLP_PRTCL;
        break;

/*lint -save -e641 Converting enum to int*/
      case DSHDR_QOS_RLP_PRTCL:
        copy_len = MIN(DSHDR_QOS_PROTOCOL_LEN, to-from);
        if(from[0] == 'T' || from[0] == 't')
          record->protocol =PS_IPPROTO_TCP;
        else if(from[0] == 'U' || from[0] == 'u')
          record->protocol =PS_IPPROTO_UDP;
        else if(from[0] == 'I' || from[0] == 'i')
          record->protocol = PS_IPPROTO_ICMP;
        else
          record->protocol =59 /*PS_NO_NEXT_HDR = 59*/;
        field_id = DSHDR_QOS_DEST_PORT;
        break;
/*lint -restore Restore lint error 641*/

      case DSHDR_QOS_DEST_PORT:
        record->start_port = 0;
        skipped_port = FALSE;
        if(from == to)
        {
          skipped_port = TRUE;
        }
        else
        {
          /*lint -save -e734*/
          DSHDR_QOS_EFSIF_ATOI( from, to, record->start_port);
          record->start_port = ps_htons(record->start_port);
          /*lint -restore*/
        }
        field_id = DSHDR_QOS_DEST_PORT_RANGE;
        break;

      case DSHDR_QOS_DEST_PORT_RANGE:
        record->port_range = 0;
        if((skipped_port == TRUE) && (from != to))
        { 
          /*dump this record*/
          bad_record = TRUE;
        }
        if(from == to)
        {
          record->port_range = 65535;
        }
        else
        {
          /*lint -save -e734*/
          DSHDR_QOS_EFSIF_ATOI( from, to, record->port_range);
          /*lint -restore*/
        }
        field_id = DSHDR_QOS_RLP_REV_FLOW_ID;
        break;

      case DSHDR_QOS_RLP_REV_FLOW_ID:
        record->rlp_rev_flw_id = 0;
        /*lint -save -e734*/
        DSHDR_QOS_EFSIF_ATOI( from, to, record->rlp_rev_flw_id);
        /*lint -restore*/
        field_id = DSHDR_QOS_DEST_IP_ADDRESS;
        if(bad_record == TRUE)
        {
          bad_record = FALSE;
          /* Trash this record, so don't increment this record*/
        }
        else
        {
          dshdr_qos_max_rec_read++;
        }
        break;
      }
    }
    else if(DS3GCFGMGR_EFS_TOKEN_PARSE_EOL == ret_val)
    {
      /*--------------------------------------------------------------------
        Reached the end of the line so reset the field_id to the begining.
      --------------------------------------------------------------------*/
      if(field_id != DSHDR_QOS_DEST_IP_ADDRESS)
      {
        DATA_HDR_MSG0(MSG_LEGACY_ERROR, "Invalid Record acquired ");
        field_id = DSHDR_QOS_DEST_IP_ADDRESS;
        continue;
      }
      else
      {
        return DSHDR_QOS_END_OF_RECORD;
      }
    }
    
  } /* Parsed the file*/
  DATA_HDR_MSG0(MSG_LEGACY_HIGH, "Done Parsing the file");
  return DSHDR_QOS_EOF;
}
#endif /*FEATURE_HDR_QOS*/

