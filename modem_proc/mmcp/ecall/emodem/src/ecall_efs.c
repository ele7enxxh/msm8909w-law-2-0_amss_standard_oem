/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          ECALL PERSISTENT STORAGE FILE

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/ecall/emodem/src/ecall_efs.c#1 $
$DateTime: 2016/12/13 08:00:05 $
$Author: mplcsds1 $ 
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                         I N C L U D E   F I L E S

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_ECALL_PSAP
#include "ecall_app.h"
#endif

#if (defined(FEATURE_ECALL_APP) || defined(FEATURE_ECALL_IVS) ||defined(FEATURE_ECALL_PSAP))

#include "msg.h"
#include "err.h"

#include "ecall_efs.h"
#include "ecall_msd_defines.h"
#include "ecall_modem_apis.h"

#include "fs_sys_types.h"
#include "fs_fcntl.h"
#include "fs_public.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stringl.h"

#define ECALL_EFS_CR 13            /* Decimal code of Carriage Return char */
#define ECALL_EFS_LF 10            /* Decimal code of Line Feed char */
#define ECALL_EFS_EOF_MARKER 26    /* Decimal code of DOS end-of-file marker */
#define ECALL_EFS_MAX_REC_LEN 1024 /* Maximum size of input buffer */
#define ECALL_EFS_SPACE 32

#define ECALL_EFS_ARRAY_SIZE  50
#define ECALL_EFS_STRING_SIZE 32

#define ECALL_EFS_U8_MAX     255
#define ECALL_EFS_U32_MAX    4294967295UL
#define ECALL_EFS_I16_MIN   -32768
#define ECALL_EFS_I16_MAX    32767

char  *efs_msd_file_name = "ecall//ecall_msd_efs.txt"; 
char ecall_file_contents[ECALL_EFS_MAX_REC_LEN];

/* Each line has 2 fields, so efs file can have max 25 lines */
char ecall_efs_data[ECALL_EFS_ARRAY_SIZE][ECALL_EFS_STRING_SIZE];  

static uint8 ecall_efs_tokens;


/*===========================================================================

                          INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION      STR_COMP

DESCRIPTION   compares the two strings passed

PARAMETERS    *str1  - Pointer to string 
              *str2  - Pointer to string
               
DEPENDENCIES  None

RETURN VALUE  Return 0 if both the strings are equal

SIDE EFFECTS  None
===========================================================================*/
int8 str_comp(const char *str1, const char *str2)
{
    int8 temp=0;

    while(*str1 && *str2)
    {
      if (*str1 == *str2)
      {
        str1++;
        str2++;
      }
      else
      {
        temp =1;
        break;
      }
     }
 
 return temp;
}


/*===========================================================================
FUNCTION      ecall_strrev

DESCRIPTION   

PARAMETERS    
               
DEPENDENCIES  None

RETURN VALUE  string in reverse

SIDE EFFECTS  None
===========================================================================*/
char *ecall_strrev
(
  char *str
)
{
  /*lint --e{525}*/
  char *p1, *p2;

  if(str == NULL)
  {
    return str;
  }

  for(p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
  {
    *p1 ^= *p2;
    *p2 ^= *p1;
    *p1 ^= *p2;
  }

  return str;
}

/*===========================================================================
FUNCTION      ecall_itoa

DESCRIPTION   
      converts integer value to ascii       

PARAMETERS    
               
DEPENDENCIES  None

RETURN VALUE  converts to ascii 

SIDE EFFECTS  None
===========================================================================*/
char *ecall_itoa
(
  int32 n,
  char *str,
  int8 b
)
{
  /*lint --e{525}*/
  static char digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
  int8 i=0;
  int32 sign =0, temp_n =0;
    
  temp_n = n;
    
  if ((sign = temp_n) < 0)
    temp_n = -temp_n;

  do
  {
    str[i++] = digits[temp_n % b];
  }while ((temp_n /= b) > 0);

  if(sign < 0)
  {
    str[i++] = '-';
  }

  str[i] = '\0';

  return ecall_strrev(str);

}

/*==========================================================================
  FUNCTION ECALL_MEMSCPY

  DESCRIPTION
    This function will use memscpy/memcpy accordingly by eCall feature flag

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_memscpy
(
  void *dst,
  size_t dst_size,
  const void *src,
  size_t src_size
)
{
  #ifdef FEATURE_ECALL_HAS_V1_HEADER
  memcpy  ( dst, src, MIN(dst_size,src_size) );
  #else
  memscpy ( dst, dst_size, src, src_size );
  #endif /* FEATURE_ECALL_HAS_V1_HEADER */

} /* ECALL_MEMSCPY */

/*==========================================================================
  FUNCTION ECALL_MEMSMOVE

  DESCRIPTION
    This function will use memsmove/memmove accordingly by eCall feature flag

  DEPENDENCIES
    None.

  RETURN VALUE
    None.

  SIDE EFFECTS
    None.
 
===========================================================================*/
void ecall_memsmove
(
  void *dst,
  size_t dst_size,
  const void *src,
  size_t src_size
)
{
  #ifdef FEATURE_ECALL_HAS_V1_HEADER
  memmove  ( dst, src, MIN(dst_size,src_size) );
  #else
  memsmove ( dst, dst_size, src, src_size );
  #endif /* FEATURE_ECALL_HAS_V1_HEADER */

} /* ECALL_MEMSCPY */

/*===========================================================================
FUNCTION      ecall_tokenizer_for_persistent_data

DESCRIPTION   This function iterates through a character array containing all 
              the tokens that are going to make up the EFS structure.  In 
              parsing it will populate the EFS structure one token at a time.

PARAMETERS    *ecall_file_contents - character array containing all the tokens that 
                               are to make up the EFS structure.
 
              file_size - the size of the character array
               
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ecall_tokenizer_for_persistent_data
(
  const char *file_contents
)
{
  int8 isNewline = 0;              /* Boolean indicating we've read a ECALL_EFS_CR or ECALL_EFS_LF */
  int8 flag = 0, skipline = 0, index2 = 0;

  memset(ecall_efs_data, 0, ECALL_EFS_ARRAY_SIZE * ECALL_EFS_STRING_SIZE );
  ecall_efs_tokens = 0;
 
  while(*file_contents)                 /* Read until reaching null char for each line*/
  {
    isNewline = 0;

    while (*file_contents)               /* Read until reaching null char */
    {
      if (!isNewline)               /* Haven't read a ECALL_EFS_CR or ECALL_EFS_LF yet */
      {
        if (*file_contents == ECALL_EFS_CR || *file_contents == ECALL_EFS_LF ) /* This char IS a ECALL_EFS_CR or ECALL_EFS_LF */
          isNewline = 1;                        /* Set flag */
      }

      else if (*file_contents != ECALL_EFS_CR && *file_contents != ECALL_EFS_LF ) /* Already found ECALL_EFS_CR or ECALL_EFS_LF */
      {
         break;                                     /* Done with line */
      }

      if(*file_contents == '#')
      {
         skipline=1;
      }

      if(*file_contents == ECALL_EFS_CR || *file_contents == ECALL_EFS_LF || *file_contents == ECALL_EFS_EOF_MARKER || *file_contents == ECALL_EFS_SPACE)
      {
         if(flag ==0 && skipline==0 && (str_comp(ecall_efs_data[ecall_efs_tokens]," ") !=0))
         {
           flag=1;
         }
         if(skipline && *file_contents == ECALL_EFS_LF)
         {
           skipline=0;
         }
      }
      else
      {
         if(flag ==1 && skipline==0)
         {
            flag= 0 ;
            ecall_efs_data[ecall_efs_tokens][index2] = '\0';
            ecall_efs_tokens++;
            index2 = 0;
         }
         if(skipline == 0)
         {
            ecall_efs_data[ecall_efs_tokens][index2] = *file_contents;
            index2++;
         }
      }

      file_contents++;

    } /* end while (*file_contents) */

  } /* end while (file_contents <= cEndPtr) */
  
} /* ecall_tokenizer_for_persistent_data() */


/*===========================================================================
FUNCTION      ECALL_READ_PERSISTENT_DATA_FROM_EFS

DESCRIPTION   This function will read from the efs file all the necessary 
              data that will populate the EFS structure.

PARAMETERS    efs_ps_file_name - the name of the EFS file to read from 



DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
boolean ecall_read_persistent_data_from_efs
( 
  const char *efs_ps_file_name
)
{

  fs_off_t file_size;
  fs_ssize_t bytes_read;
  int32 file_descriptor =0, result =0;

  /* using efs_open to get file size */
  file_descriptor = efs_open( efs_ps_file_name , O_RDONLY );
  if (file_descriptor < 0)
  {
    ECALL_MSG_0( ECALL_ERROR, "Err opening EFS file" );
    return FALSE;
  }

  /* obtain file size */
  file_size = efs_lseek (file_descriptor, 0, SEEK_END);

  /* if empty file was received */
  if ( file_size == 0 )
  {
    ECALL_MSG_0( ECALL_ERROR, "ERROR: Empty file ecall_psap_efs.txt." );
    return FALSE;
  }

  /* close the efs file */
  result = efs_close(file_descriptor);
  if ( result != 0 )
  {
    ECALL_MSG_0( ECALL_ERROR, "ERROR: Failed to close ecall_psap_efs file. Proceed anyway. Using Defualt values" );
    return FALSE;
  }


 /****************************************************************************
   The reason opening, closing and re-opening the same file is because there 
   is no efs_rewind function which will allow us to iterate through the file 
   from the begining till the end.  For this matter the file is closed then 
   re-openend when the file is re-opened the file pointer will be at the 
   beginning and thus all the bytes of data within the file can be read.
 ****************************************************************************/

  /* using efs_open to get data */
  file_descriptor = efs_open( efs_ps_file_name , O_RDONLY );

  /* efs_open failed */
  if (file_descriptor == (-1))
  {
    ECALL_MSG_0( ECALL_ERROR, "Err opening EFS file ecall_psap_efs. Using Default values" );
    return FALSE;
  }

  /* Clear the buffer before you read */
  memset(ecall_file_contents, 0x0, ECALL_EFS_MAX_REC_LEN);

  /* efs_read to populate file_contents */
  /*lint -e732*/
  bytes_read = efs_read(file_descriptor, ecall_file_contents, file_size);
  if ( bytes_read != file_size )
  {
    ECALL_MSG_0( ECALL_ERROR, "ERROR: Failed to read entire file. Proceed anyway." );
  }
  /*lint +e732*/

  if ( bytes_read > ECALL_EFS_MAX_REC_LEN ) 
  {
     ECALL_MSG_0( ECALL_ERROR, "ERROR: ecall_psap_efs File Size Limit: ECALL_EFS_MAX_REC_LEN. Using Defualt values" );
     return FALSE;
  }
  /* create the tokens */
  ecall_tokenizer_for_persistent_data ( ecall_file_contents );

  /* close efs file */
  result = efs_close(file_descriptor);
  if (result != 0)
  {
    ECALL_MSG_0( ECALL_ERROR, "ERROR: Failed to close file. Proceed anyway." );
  }

  return TRUE;
} /* ecall_read_persistent_data_from_efs() */

#ifdef FEATURE_ECALL_PSAP

/*===========================================================================
FUNCTION      ECALL_PSAP_DEFUALT_EFS_VALUES

DESCRIPTION  On the error conditions like if file size is big or if the file
              does not exist the defualt values will be set.

PARAMETERS   ecall_psap_efs_config - pointer to efs config structure

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ecall_psap_default_efs_values
(
  ecall_psap_efs_struct_type *ecall_psap_efs_config
)
{
    /* If Read fails set to the default values*/
    ECALL_MSG_0( ECALL_HIGH, "Setting Defualt PSAP EFS values" );

    ecall_psap_efs_config->canned_psap        = FALSE;
    ecall_psap_efs_config->enable_slip        = FALSE;
    ecall_psap_efs_config->invert_sig         = FALSE;
    ecall_psap_efs_config->force_pull_mode    = FALSE;
    ecall_psap_efs_config->hlack_data         = 0;
    ecall_psap_efs_config->num_times_to_loop  = 1;
    ecall_psap_efs_config->restart_timer      = 0;

    ecall_psap_efs_config->canned_msg_count   = 0;

    ECALL_MSG_3( ECALL_HIGH, "Canned_psap %d, num_times_to_loop %d,  canned_msg_count %d",
                              ecall_psap_efs_config->canned_psap,
                              ecall_psap_efs_config->num_times_to_loop,
                              ecall_psap_efs_config->canned_msg_count );
    
}

/*==========================================================================

  FUNCTION    ECALL_PSAP_EFS_READ_CANNED_MSG

  DESCRIPTION
    Read from the PSAP configuration from the efs and store it in a structure
    
  DEPENDENCIES
    FILE FORMAT:
       CANNED_PSAP  TRUE/FALSE  (To choose between Canned Messages or real PSAP)
       LOOP   4 2000    (Loops the PSAP behaviour 4 times with 2000 ms delay )  
       START  3         (Sends 3 START messages if CANNED_PSAP is TRUE)
       NACK   2  
       ......

  RETURN VALUE
    None
  
  SIDE EFFECTS
    None

===========================================================================*/
void ecall_psap_efs_read_canned_msg
(
  ecall_psap_efs_struct_type *ecall_psap_efs_config
)
{
  boolean ecall_efs_read_status = FALSE;
  uint8 i, canned_index=0;
  char  *efs_psap_file_name = "ecall//ecall_psap_efs.txt"; 

  ecall_efs_read_status = ecall_read_persistent_data_from_efs(efs_psap_file_name);
  ECALL_MSG_2( ECALL_HIGH, "efs read status %d  tokens %d",
                            ecall_efs_read_status,
                            ecall_efs_tokens );

  /* psap efs file is not expected to have more than the below 5 tokens and messages 
    of not more than ECALL_PSAP_CANNED_MSG * ECALL_PSAP_CANNED_MSG_CNT */
  /*---------------------------------------
       CANNED_PSAP  TRUE/FALSE     (2 Tokens)
       ENABLE_SLIP  TRUE/FALSE     (2 Tokens)
       INVERT_SIG   TRUE/FALSE     (2 Tokens)
       FORCE_PULL   TRUE/FALSE     (2 Tokens)
       HLACK_DATA   0              (2 Tokens)
       LOOP         4 2000         (3 Tokens)
  -----------------------------------------*/
  if ((ecall_efs_tokens < ECALL_PSAP_CANNED_MSG * ECALL_PSAP_CANNED_MSG_CNT + 13) &&
      (ecall_efs_read_status == TRUE))
  {
    ecall_psap_efs_config->canned_psap = (str_comp(ecall_efs_data[1], "TRUE") == 0) ? TRUE : FALSE;
    ecall_psap_efs_config->enable_slip = (str_comp(ecall_efs_data[3], "TRUE") == 0) ? TRUE : FALSE;
    ecall_psap_efs_config->invert_sig  = (str_comp(ecall_efs_data[5], "TRUE") == 0) ? TRUE : FALSE;
    ecall_psap_efs_config->force_pull_mode = (str_comp(ecall_efs_data[7], "TRUE") == 0) ? TRUE : FALSE;

    ECALL_MSG_2( ECALL_HIGH, "canned_psap %d, enable_slip %d", 
                              ecall_psap_efs_config->canned_psap, 
                              ecall_psap_efs_config->enable_slip ); 

    ECALL_MSG_2( ECALL_HIGH, "invert_sig %d, force_pull %d",
                              ecall_psap_efs_config->invert_sig,
                              ecall_psap_efs_config->force_pull_mode );
      
    ecall_psap_efs_config->hlack_data         = (uint8  )atoi(ecall_efs_data[9]);
    ecall_psap_efs_config->num_times_to_loop  = (uint16 )atoi(ecall_efs_data[11]);
    ecall_psap_efs_config->restart_timer      = (uint32 )atoi(ecall_efs_data[12]);

    ECALL_MSG_3( ECALL_HIGH, "hlack_data %d, num_times_to_loop %d, restart_timer %d",
                              ecall_psap_efs_config->hlack_data,
                              ecall_psap_efs_config->num_times_to_loop,
                              ecall_psap_efs_config->restart_timer );

    for(i = 13; i < ecall_efs_tokens; i++)
    {
      if(str_comp(ecall_efs_data[i], "START") == 0)
      {
        ecall_psap_efs_config->canned_msg_array[canned_index][0] = DlMsgStart;
      }
      else if(str_comp(ecall_efs_data[i], "NACK") == 0)
      {
        ecall_psap_efs_config->canned_msg_array[canned_index][0] = DlMsgNack;
      }
      else if(str_comp(ecall_efs_data[i], "ACK") == 0)
      {
        ecall_psap_efs_config->canned_msg_array[canned_index][0] = DlMsgAck;
      }
      else if(str_comp(ecall_efs_data[i], "SEND") == 0)
      {
        ecall_psap_efs_config->canned_msg_array[canned_index][0] = DlMsgSend;
      }
      else if(str_comp(ecall_efs_data[i], "HLACK") == 0)
      {
        ecall_psap_efs_config->canned_msg_array[canned_index][0] = DlMsgHlack;
      }
      else
      {
        ECALL_MSG_0( ECALL_ERROR, "could not recognise the string consider start" );
        ecall_psap_efs_config->canned_msg_array[canned_index][0] = DlMsgStart;
      }

      ecall_psap_efs_config->canned_msg_array[canned_index][1] = atoi(ecall_efs_data[++i]);

      ECALL_MSG_2( ECALL_HIGH, "%d rep_tokens: %d",
                                ecall_psap_efs_config->canned_msg_array[canned_index][0],
                                ecall_psap_efs_config->canned_msg_array[canned_index][1] );
      canned_index++;
    }

    ecall_psap_efs_config->canned_msg_count = canned_index;
  }
  else
  { 
    ECALL_MSG_0( ECALL_ERROR, "more than 27 lines or data inconsistent in efs" );
    ecall_psap_default_efs_values(ecall_psap_efs_config);
  } 
}

/*=========================================================================

  FUNCTION    ECALL_PSAP_EFS_READ_SAMPLES

  DESCRIPTION
    Should have the ENABLE_SLIP TRUE in ecall_psap_efs.txt config file.
    If the ecall_psap_efs.txt doesn't exist slipping from PSAP TX is FALSE.
    If we have the flag set to ENABLE_SLIP TRUE and ecall_psap_sample.txt
    doesn't exist uses random numbers to generate slips
        
  DEPENDENCIES
    FILE FORMAT:
          MSG          ACK
          num_samples  1
          msgIndex     5
          frameIndex   6 7

   
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
boolean ecall_psap_efs_read_samples
(
  ecall_psap_efs_sample_type *ecall_psap_efs_sample
)
{
  int16 i;
  boolean ecall_efs_read_status = FALSE;
  char  *efs_psap_sample_file = "ecall//ecall_psap_sample.txt"; 

  ecall_efs_read_status = ecall_read_persistent_data_from_efs(efs_psap_sample_file);
  ECALL_MSG_2( ECALL_HIGH, "efs read status %d  tokens %d",
                            ecall_efs_read_status,
                            ecall_efs_tokens );

  /* initialise to default values */
   ecall_psap_efs_sample->frameIndex[0]  = -1;
   ecall_psap_efs_sample->frameIndex[1]  = -1;
   ecall_psap_efs_sample->frameIndex[2]  = -1;

  /*---------------------------------------
       CANNED_PSAP  TRUE/FALSE     (2 Tokens)
  -----------------------------------------*/
  if(ecall_efs_read_status  == TRUE)
  {
    if(str_comp(ecall_efs_data[1], "START") == 0)
    {
      ecall_psap_efs_sample->sampleMsg = DlMsgStart;
    }
    else if(str_comp(ecall_efs_data[1], "NACK") == 0)
    {
      ecall_psap_efs_sample->sampleMsg = DlMsgNack;
    }
    else if(str_comp(ecall_efs_data[1], "ACK") == 0)
    {
      ecall_psap_efs_sample->sampleMsg = DlMsgAck;
    }
    else if(str_comp(ecall_efs_data[1], "HLACK") == 0)
    {
      ecall_psap_efs_sample->sampleMsg = DlMsgHlack;
    }
    else
    {
      ECALL_MSG_0( ECALL_ERROR, "could not recognise the string consider ACK" );
      ecall_psap_efs_sample->sampleMsg  = DlMsgAck;
    }

    ecall_psap_efs_sample->numSamples   = (uint16 )atoi(ecall_efs_data[3]);
    ecall_psap_efs_sample->msgIndex     = (uint16 )atoi(ecall_efs_data[5]);
    
    for(i=0; i<=(ecall_efs_tokens-7) && i<ECALL_PSAP_NUM_SLIP_POSITIONS;i++)
    {
      ecall_psap_efs_sample->frameIndex[i]  = (uint16 )atoi(ecall_efs_data[7+i]);
    }
  }
  else
  { 
    ECALL_MSG_0( ECALL_ERROR, "no efs_psap_sample_file found generating using rand" );
    return FALSE;
  }

  return TRUE; 
}

#endif  /* end FEATURE_ECALL_PSAP */

#if (defined(FEATURE_ECALL_APP) || defined(FEATURE_ECALL_PSAP))
/*===========================================================================
FUNCTION ECALL_MSD_EFS_WRITE

DESCRIPTION
This function invokes efs_write and checks to make sure the number of bytes
written matches the requested number of bytes.

PARAMETERS
    fd - file descriptor of the file from which to write
    *buf - pointer to the bytes of data written
    nbyte - number of bytes written

DEPENDENCIES
  none

RETURN VALUE
FALSE - nbyte mismatch with efs_write return value.
TRUE - SUCCESS

SIDE EFFECTS
  none
===========================================================================*/
boolean ecall_msd_efs_write
(
  const ecall_msd_structure_s_type *ecall_msd,
  int fd
)
{
  fs_off_t file_size;
  fs_ssize_t fs_res;
  
  int16 nDestSize = 0;
  char itoa_buf[32];
  uint8 i;
  char ecall_acceptable_vin_buffer[ECALL_SIZEOF_VIN + 1]; /* Needs additional character for end of string */

  /*---------------------------------------------------------------------- 
  efs_write returns number of bytes written if successful, -1 indicates an
  error and 0 means the file system is full.
  ----------------------------------------------------------------------*/

  nDestSize = sizeof(ecall_file_contents);
  memset( ecall_file_contents, 0x0, (uint16)nDestSize );
  memset( ecall_acceptable_vin_buffer, 0x0, sizeof(ecall_acceptable_vin_buffer) );

  /* --------------------------------------------------------------------
                  Writing the data in chunk 1
  ----------------------------------------------------------------------*/
  (void)ecall_itoa(ecall_msd->ecall_formatversion, itoa_buf, 10);
  (void) strlcpy(ecall_file_contents, "\nFormatVersion   ",nDestSize);
  (void) strlcat(ecall_file_contents, itoa_buf, nDestSize);
  
  (void)ecall_itoa(ecall_msd->ecall_messageidentifier, itoa_buf, 10);
  (void) strlcat(ecall_file_contents, "\nMessageIdentifier   ",nDestSize);
  (void) strlcat(ecall_file_contents, itoa_buf, nDestSize);

  (void) strlcat(ecall_file_contents, "\n\nActivation    ",nDestSize);
  if(ecall_msd->ecall_control_info.activation == ECALL_MANUAL_INITIATED)
  {
    (void) strlcat(ecall_file_contents, "ECALL_MANUAL_INITIATED",nDestSize);
  }
  else
  {
    (void) strlcat(ecall_file_contents, "ECALL_AUTOMATIC_INITIATED",nDestSize);
  }

  (void) strlcat(ecall_file_contents, "\ncallType    ",nDestSize);
  if(ecall_msd->ecall_control_info.callType == (ecall_type_of_call)0 )
  {
    /* EN15722: callType(0): Emergency Call */
    (void) strlcat(ecall_file_contents, "ECALL_EMERGENCY",nDestSize);
  }
  else if(ecall_msd->ecall_control_info.callType == (ecall_type_of_call)1 )
  {
    /* EN15722: callType(1): Test Call */
    (void) strlcat(ecall_file_contents, "ECALL_TEST",nDestSize);
  }
  else
  {
    (void) strlcat(ecall_file_contents, "ECALL_UNKNOWN",nDestSize);
  }

  (void) ecall_itoa(ecall_msd->ecall_control_info.ecall_vehicletype, itoa_buf, 10);
  (void) strlcat(ecall_file_contents, "\nvehicleType    ",nDestSize);
  (void) strlcat(ecall_file_contents, itoa_buf, nDestSize);

  (void) strlcat(ecall_file_contents, "\npositionCanBeTrusted    ",nDestSize);
  if (ecall_msd->ecall_control_info.positionconfidence == TRUE )
  {
    (void) strlcat(ecall_file_contents, "TRUE", nDestSize);
  }
  else
  {
    (void) strlcat(ecall_file_contents, "FALSE", nDestSize);
  }

  (void) strlcat(ecall_file_contents, "\nVIN    ", nDestSize);
  for (i = 0; i < ECALL_SIZEOF_VIN; i ++)
  {
    if ( ((ecall_msd->ecall_vin[i] >= 'A') && (ecall_msd->ecall_vin[i] <= 'H')) ||
         ((ecall_msd->ecall_vin[i] >= 'J') && (ecall_msd->ecall_vin[i] <= 'N')) ||
          (ecall_msd->ecall_vin[i] == 'P') || 
         ((ecall_msd->ecall_vin[i] >= 'R') && (ecall_msd->ecall_vin[i] <= 'Z')) ||
         ((ecall_msd->ecall_vin[i] >= '0') && (ecall_msd->ecall_vin[i] <= '9')) )
    {
      /* Supported VIN characters */
      ecall_acceptable_vin_buffer[i] = ecall_msd->ecall_vin[i];
    }
    else
    {
      /* Unsupported VIN characters*/
      ecall_acceptable_vin_buffer[i] = '0';
    }
  }
  ecall_acceptable_vin_buffer[i] = '\0';
  (void) strlcat(ecall_file_contents, ecall_acceptable_vin_buffer, nDestSize);

  fs_res = efs_write(fd, ecall_file_contents, strlen(ecall_file_contents));

  if (fs_res == -1) 
  {
    ECALL_MSG_1( ECALL_ERROR, "efs_write failed for fd=%d",
                               fd );
    return FALSE;
  }

  /* --------------------------------------------------------------------
                  Writing the data in chunk 2
  ----------------------------------------------------------------------*/
  (void) strlcpy(ecall_file_contents, "\n\ngasolineTankPresent    ",nDestSize);
  if(ecall_msd->ecall_vehicle_propulsionstorage.gasolineTankPresent == TRUE)
  {
    (void) strlcat(ecall_file_contents, "TRUE",nDestSize);
  }
  else
  {
    (void) strlcat(ecall_file_contents, "FALSE",nDestSize);
  }

  (void) strlcat(ecall_file_contents, "\ndieselTankPresent    ",nDestSize);
  if(ecall_msd->ecall_vehicle_propulsionstorage.dieselTankPresent == TRUE)
  {
    (void) strlcat(ecall_file_contents, "TRUE",nDestSize);
  }
  else
  {
    (void) strlcat(ecall_file_contents, "FALSE",nDestSize);
  }

  (void) strlcat(ecall_file_contents, "\ncompressedNaturalGas    ",nDestSize);
  if(ecall_msd->ecall_vehicle_propulsionstorage.compressedNaturalGas == TRUE)
  {
    (void) strlcat(ecall_file_contents, "TRUE",nDestSize);
  }
  else
  {
    (void) strlcat(ecall_file_contents, "FALSE",nDestSize);
  }

  (void) strlcat(ecall_file_contents, "\nliquidPropaneGas    ",nDestSize);
  if(ecall_msd->ecall_vehicle_propulsionstorage.liquidPropaneGas == TRUE)
  {
    (void) strlcat(ecall_file_contents, "TRUE",nDestSize);
  }
  else
  {
    (void) strlcat(ecall_file_contents, "FALSE",nDestSize);
  }

  (void) strlcat(ecall_file_contents, "\nelectricEnergyStorage    ",nDestSize);
  if(ecall_msd->ecall_vehicle_propulsionstorage.electricEnergyStorage == TRUE)
  {
    (void) strlcat(ecall_file_contents, "TRUE",nDestSize);
  }
  else
  {
    (void) strlcat(ecall_file_contents, "FALSE",nDestSize);
  }

  (void) strlcat(ecall_file_contents, "\nhydrogenStorage    ",nDestSize);
  if(ecall_msd->ecall_vehicle_propulsionstorage.hydrogenStorage == TRUE)
  {
    (void) strlcat(ecall_file_contents, "TRUE",nDestSize);
  }
  else
  {
    (void) strlcat(ecall_file_contents, "FALSE",nDestSize);
  }

  /* Timestamp is uint32*/
  (void) ecall_itoa(ecall_msd->ecall_timestamp, itoa_buf, 10); /*lint !e713*/
  (void) strlcat(ecall_file_contents, "\nTimeStamp   ",nDestSize);
  (void) strlcat(ecall_file_contents, itoa_buf, nDestSize);

  (void) ecall_itoa(ecall_msd->ecall_vehicle_location.positionlatitude, itoa_buf, 10);
  (void) strlcat(ecall_file_contents, "\n\npositionLatitude   ",nDestSize);
  (void) strlcat(ecall_file_contents, itoa_buf, nDestSize);

  (void) ecall_itoa(ecall_msd->ecall_vehicle_location.positionlongitude, itoa_buf, 10);
  (void) strlcat(ecall_file_contents, "\npositionLongitude   ",nDestSize);
  (void) strlcat(ecall_file_contents, itoa_buf, nDestSize);
  
  (void) ecall_itoa(ecall_msd->ecall_vehicledirection, itoa_buf, 10);
  (void) strlcat(ecall_file_contents, "\nvehicledirection   ",nDestSize);
  (void) strlcat(ecall_file_contents, itoa_buf, nDestSize);

  (void) ecall_itoa(ecall_msd->ecall_numberOfPassengers, itoa_buf, 10);
  (void) strlcat(ecall_file_contents, "\n\nnumberOfPassengers   ",nDestSize);
  (void) strlcat(ecall_file_contents, itoa_buf, nDestSize);


  fs_res = efs_write(fd, ecall_file_contents, strlen(ecall_file_contents));


  if (fs_res == -1) 
  {
    ECALL_MSG_1( ECALL_ERROR, "efs_write failed for fd=%d",
                               fd );
    return FALSE;
  }
  /*--------------------------------------------------------------------------
    truncate any contents after the current position if any
  -------------------------------------------------------------------------*/

  file_size = efs_lseek(fd, 0, SEEK_CUR);

  if (file_size == -1) 
  {
    ECALL_MSG_1( ECALL_ERROR, "efs_lseek failed for %d",
                               fd );
    return FALSE;
  }

  if (efs_ftruncate(fd, file_size) == -1)
  {
    ECALL_MSG_1( ECALL_ERROR, "config :fs_truncate failed fd=%d ",
                               fd );
    efs_close (fd);
    return FALSE;
  }

  ECALL_MSG_0( ECALL_HIGH, "ecall msd saved to EFS" );
  
  return TRUE;
}

/*===========================================================================
                  EXTERNAL FUNCTIONS FOR THIS MODULE
===========================================================================*/


/*===========================================================================
FUNCTION      ECALL_SAVE_MSD

DESCRIPTION   This function will read from the efs file all the necessary 
              data that will populate the EFS structure.

PARAMETERS      

DEPENDENCIES  None

RETURN VALUE  Returns True is if succesfully wrote to the efs

SIDE EFFECTS  None
===========================================================================*/
boolean ecall_save_msd
(
  const ecall_msd_structure_s_type *ecall_msd
)
{
  int fs_handle;
 /*-------------------------------------------------------------------------
    Open the file
  -------------------------------------------------------------------------*/
  fs_handle = efs_open(efs_msd_file_name, O_WRONLY|O_CREAT);

  if ( fs_handle == -1)
  {
    ECALL_MSG_0( ECALL_ERROR,"efs_open failed" );
    return FALSE;
  }


  /*-------------------------------------------------------------------------
    Write the contents to the file
  -------------------------------------------------------------------------*/
  if (ecall_msd_efs_write(ecall_msd, fs_handle) == FALSE)
  {
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Close the file
  -------------------------------------------------------------------------*/
  if (efs_close (fs_handle) == -1)
  {
    ECALL_MSG_0( ECALL_ERROR, "efs_close failed" );
    return FALSE;
  }

  return TRUE;
}

#endif /*end FEATURE ecall APP, PSAP*/

#ifdef FEATURE_ECALL_IVS

/*===========================================================================

  FUNCTION    ECALL_EFS_READ_ECALL_CONFIG

  DESCRIPTION
     Reads the vocoder configuration needs to be disabled  as 
     specified in the efs file
        
  DEPENDENCIES
   
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
void ecall_efs_read_ecall_config
(
  ecall_efs_config_type *ecall_enable_config
)
{
  boolean ecall_efs_read_status = FALSE;
  char  *efs_enable_config_file_name = "ecall//ecall_config.txt";

  ecall_efs_read_status = ecall_read_persistent_data_from_efs(efs_enable_config_file_name);
  ECALL_MSG_1( ECALL_HIGH, "efs read ecall_config status %d",
                            ecall_efs_read_status );

  /*Reset to default values */
  ecall_enable_config->voc_config = FALSE;
  ecall_enable_config->canned_msd = FALSE;
  ecall_enable_config->ecall_dial_num = ECALL_DIAL_NORMAL;
  ecall_enable_config->gnss_update_time_ms = ECALL_GPS_PERIODIC_FIX_TIMEOUT;
  ecall_enable_config->test_only_callback_timer_ms = ECALL_CALLBACK_TIMEOUT;
  ecall_enable_config->test_only_call_cleardown_timer_ms = ECALL_CALL_CLEARDOWN_TIMEOUT;
  ecall_enable_config->test_only_session_timer_ms = ECALL_SESSION_TIMEOUT;
  ecall_enable_config->ecall_usim_slot_id = MMGSDI_SLOT_1;
  memset( ecall_enable_config->digits, 0, sizeof(ecall_enable_config->digits));

  if(ecall_efs_read_status  == TRUE)
  {
    if(str_comp(ecall_efs_data[1],"TRUE") == 0 ) /* Voc Config */
    {
       ecall_enable_config->voc_config = TRUE;
    }    
    if(str_comp(ecall_efs_data[3],"TRUE") == 0 ) 
    {
       ecall_enable_config->canned_msd = TRUE;
    }

    if(str_comp(ecall_efs_data[5],"NORMAL") == 0 ) 
    {
       ecall_enable_config->ecall_dial_num = ECALL_DIAL_NORMAL;
       memset(ecall_enable_config->digits, 0, sizeof(ecall_enable_config->digits));
    }
    else
    {
      if(str_comp(ecall_efs_data[5],"OVERRIDE") == 0 )
      {
       ecall_enable_config->ecall_dial_num = ECALL_DIAL_OVERRIDE;
      }
      ecall_memscpy ( (void *)ecall_enable_config->digits, sizeof(ecall_enable_config->digits),
                      ecall_efs_data[7],                   sizeof(ecall_efs_data[7])  );
    }

    
    /* gnss_update_time_ms */
    ecall_enable_config->gnss_update_time_ms = (uint32)atoi(ecall_efs_data[9]);
    if (ecall_enable_config->gnss_update_time_ms < 1000)
    {
      ECALL_MSG_0( ECALL_HIGH, "gnss_update_time_ms is smaller than minimum allowable time 1000ms, set to 1000ms" );
      ecall_enable_config->gnss_update_time_ms = 1000;
    }
    ECALL_MSG_1( ECALL_HIGH, "efs set gnss_update_time_ms to %d",
                              ecall_enable_config->gnss_update_time_ms );

    /* test_only_callback_timer_ms */
    ecall_enable_config->test_only_callback_timer_ms = (uint32)atoi(ecall_efs_data[11]);
    if (ecall_enable_config->test_only_callback_timer_ms == 0)
    {
      ecall_enable_config->test_only_callback_timer_ms = ECALL_CALLBACK_TIMEOUT;
    }
    ECALL_MSG_1( ECALL_HIGH, "efs set test_only_callback_timer_ms to %d",
                              ecall_enable_config->test_only_callback_timer_ms );

    /* ecall_usim_slot_id */
    ecall_enable_config->ecall_usim_slot_id = (mmgsdi_slot_id_enum_type)atoi(ecall_efs_data[13]);
    if ((ecall_enable_config->ecall_usim_slot_id == (mmgsdi_slot_id_enum_type)0) || (ecall_enable_config->ecall_usim_slot_id > (mmgsdi_slot_id_enum_type)3) )
    {
      ECALL_MSG_2( ECALL_ERROR, "ecall_usim_slot_id not valid: %d. Set to default value %d",
                                 ecall_enable_config->ecall_usim_slot_id,
                                 MMGSDI_SLOT_1 );
      ecall_enable_config->ecall_usim_slot_id = MMGSDI_SLOT_1;
    }
    ECALL_MSG_1( ECALL_HIGH, "efs set ecall_usim_slot_id to %d",
                              ecall_enable_config->ecall_usim_slot_id );

  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "ECALL EFS CONFIG READ FAILED" );
  } 
} /* ECALL_EFS_READ_ECALL_CONFIG */

/*===========================================================================

  FUNCTION ECALL_EFS_GET_CONFIG

  DESCRIPTION
    This function return the eCall EFS config.
      
  DEPENDENCIES
    None.

  RETURN VALUE
    Boolean.

  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_efs_get_config
(
  void
)
{
  if(DalChipInfo_ChipId()==ECALL_MODEM_CONFIG)
  {
    return TRUE;
  }
  
  ECALL_MSG_0( ECALL_HIGH, "ECALL EFS INCORRECT CONFIG" );
  return FALSE;
}

/*=========================================================================

  FUNCTION    ECALL_IVS_EFS_READ_SAMPLES

  DESCRIPTION
 
        
  DEPENDENCIES
    FILE FORMAT:
          ENABLE_SLIP  TRUE/FALSE
          RV           0
          num_samples  1
          frameIndex   6 7 8

   
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
boolean ecall_ivs_efs_read_samples
(
  ecall_ivs_efs_sample_type *ecall_ivs_efs_sample
)
{
  int16 i;
  boolean ecall_efs_read_status = FALSE;
  char  *efs_ivs_sample_file = "ecall//ecall_ivs_sample.txt"; 

  ecall_efs_read_status = ecall_read_persistent_data_from_efs(efs_ivs_sample_file);
  ECALL_MSG_2( ECALL_HIGH, "efs read status %d  tokens %d",
                            ecall_efs_read_status,
                            ecall_efs_tokens );

  /* initialise to default values */
   ecall_ivs_efs_sample->enable_slip    = FALSE;
   ecall_ivs_efs_sample->rv             = -1;
   ecall_ivs_efs_sample->frameIndex[0]  = -1;
   ecall_ivs_efs_sample->frameIndex[1]  = -1;
   ecall_ivs_efs_sample->frameIndex[2]  = -1;

  /*---------------------------------------
       CANNED_PSAP  TRUE/FALSE     (2 Tokens)
  -----------------------------------------*/
  if(ecall_efs_read_status  == TRUE)
  {
   
    ecall_ivs_efs_sample->enable_slip    = (str_comp(ecall_efs_data[1], "TRUE") == 0) ? TRUE : FALSE;

    ecall_ivs_efs_sample->rv             = (int16 )atoi(ecall_efs_data[3]);
    
    ecall_ivs_efs_sample->numSamples     = (int16 )atoi(ecall_efs_data[5]);
           
    for(i=0; i<=(ecall_efs_tokens- 7) && i<3; i++)
    {
      ecall_ivs_efs_sample->frameIndex[i]  = (int16 )atoi(ecall_efs_data[7+i]);
    }           
   
  }
  else
  {
    ECALL_MSG_0( ECALL_ERROR, "no efs_ivs_sample_file found" );
    return FALSE;
  }

  return TRUE; 
}

/*===========================================================================

  FUNCTION    ECALL_EFS_READ_ERA_GLONASS_CONFIG

  DESCRIPTION
     Reads the ERA-GLONASS configuration.
        
  DEPENDENCIES
   
  RETURN VALUE
    None
  
  SIDE EFFECTS
    None
===========================================================================*/
void ecall_efs_read_era_glonass_config
(
  ecall_efs_config_type *ecall_era_glonass_config
)
{
  boolean      ecall_efs_read_status = FALSE;
  char         *efs_era_glonass_config_file_name = "ecall//ecall_era_glonass_config.txt";
  const uint8  era_glonass_default_dialing_num[] = { 0x31, 0x31, 0x32 }; /* 112 */

  ecall_efs_read_status = ecall_read_persistent_data_from_efs(efs_era_glonass_config_file_name);
  ECALL_MSG_1( ECALL_HIGH, "efs read ecall_era_glonass_config status %d",
                            ecall_efs_read_status );

  /*Reset to default values */
  ecall_era_glonass_config->voc_config = FALSE;
  ecall_era_glonass_config->canned_msd = FALSE;
  ecall_era_glonass_config->gnss_update_time_ms = ECALL_GPS_PERIODIC_FIX_TIMEOUT;
  ecall_era_glonass_config->test_only_callback_timer_ms = ECALL_CALLBACK_TIMEOUT;
  ecall_era_glonass_config->test_only_call_cleardown_timer_ms = ECALL_CALL_CLEARDOWN_TIMEOUT;
  ecall_era_glonass_config->test_only_session_timer_ms = ECALL_SESSION_TIMEOUT;
  ecall_era_glonass_config->ecall_usim_slot_id = MMGSDI_SLOT_1;
  memset( ecall_era_glonass_config->digits, 0, sizeof(ecall_era_glonass_config->digits));
  ecall_memscpy( ecall_era_glonass_config->digits, sizeof(ecall_era_glonass_config->digits),
                 era_glonass_default_dialing_num,  sizeof(era_glonass_default_dialing_num) );

  if(ecall_efs_read_status == TRUE)
  {
    /* VOC_CONFIG */
    if(str_comp(ecall_efs_data[1],"TRUE") == 0 )
    {
       ecall_era_glonass_config->voc_config = TRUE;
    }
    /* CANNED_MSD */
    if(str_comp(ecall_efs_data[3],"TRUE") == 0 )
    {
       ecall_era_glonass_config->canned_msd = TRUE;
    }
    
    /* DIGITS */
    ecall_memscpy ( (void *)ecall_era_glonass_config->digits, sizeof(ecall_era_glonass_config->digits),
                    ecall_efs_data[5],                        sizeof(ecall_efs_data[5])  );

    /* GNSS_UPDATE_TIME_MS */
    ecall_era_glonass_config->gnss_update_time_ms = (uint32)atoi(ecall_efs_data[7]);
    if (ecall_era_glonass_config->gnss_update_time_ms < 1000)
    {
      ECALL_MSG_0( ECALL_HIGH, "gnss_update_time_ms is smaller than minimum allowable time 1000ms, set to 1000ms" );
      ecall_era_glonass_config->gnss_update_time_ms = 1000;
    }
    ECALL_MSG_3( ECALL_HIGH, "efs set voc_config to %d, canned_msd to %d, gnss_update_time_ms to %d",
                              ecall_era_glonass_config->voc_config,
                              ecall_era_glonass_config->canned_msd,
                              ecall_era_glonass_config->gnss_update_time_ms );

    /* TEST_ONLY_CALLBACK_TIMER_MS */
    ecall_era_glonass_config->test_only_callback_timer_ms = (uint32)atoi(ecall_efs_data[9]);
    if (ecall_era_glonass_config->test_only_callback_timer_ms == 0)
    {
      ecall_era_glonass_config->test_only_callback_timer_ms = ECALL_CALLBACK_TIMEOUT;
    }

    /* TEST_ONLY_CALL_CLEARDOWN_TIMER_MS */
    ecall_era_glonass_config->test_only_call_cleardown_timer_ms = (uint32)atoi(ecall_efs_data[11]);
    if (ecall_era_glonass_config->test_only_call_cleardown_timer_ms == 0)
    {
      ecall_era_glonass_config->test_only_call_cleardown_timer_ms = ECALL_CALL_CLEARDOWN_TIMEOUT;
    }

    /* TEST_ONLY_SESSION_TIMER_MS */
    ecall_era_glonass_config->test_only_session_timer_ms = (uint32)atoi(ecall_efs_data[13]);
    if (ecall_era_glonass_config->test_only_session_timer_ms == 0)
    {
      ecall_era_glonass_config->test_only_session_timer_ms = ECALL_SESSION_TIMEOUT;
    }

    ECALL_MSG_3( ECALL_HIGH, "efs set test only callback_timer_ms to %d, call_cleardown_timer_ms to %d, session_timer_ms to %d",
                              ecall_era_glonass_config->test_only_callback_timer_ms,
                              ecall_era_glonass_config->test_only_call_cleardown_timer_ms,
                              ecall_era_glonass_config->test_only_session_timer_ms );

    /* ECALL_USIM_SLOT_ID */
    ecall_era_glonass_config->ecall_usim_slot_id = (mmgsdi_slot_id_enum_type)atoi(ecall_efs_data[15]);
    if ((ecall_era_glonass_config->ecall_usim_slot_id == 0) || (ecall_era_glonass_config->ecall_usim_slot_id > 3) )
    {
      ECALL_MSG_2( ECALL_ERROR, "ecall_usim_slot_id not valid: %d. Set to default value %d",
                                 ecall_era_glonass_config->ecall_usim_slot_id,
                                 MMGSDI_SLOT_1 );
      ecall_era_glonass_config->ecall_usim_slot_id = MMGSDI_SLOT_1;
    }
    ECALL_MSG_1( ECALL_HIGH, "efs set ecall_usim_slot_id to %d",
                              ecall_era_glonass_config->ecall_usim_slot_id );
  }
  else
  {
    ECALL_MSG_0( ECALL_HIGH, "eCall read ERA-GLONASS config FAILED" );
  } 
} /* ECALL_EFS_READ_ERA_GLONASS_CONFIG */

#endif /* FEATURE_ECALL_IVS */

#ifdef FEATURE_ECALL_APP

/*==========================================================================

  FUNCTION    ECALL_APP_EFS_READ_CANNED_MSD

  DESCRIPTION
    Reads the MSD from EFS file and stores it in the msd structure
    
  DEPENDENCIES
   File format should be followed

  RETURN VALUE
    TRUE   -- If MSD data is stored in the structure
    FALSE  -- on failure cases, canned hard coded msd is sent 
  
  SIDE EFFECTS
    None

===========================================================================*/
boolean ecall_app_efs_read_canned_msd
(
  ecall_msd_structure_s_type *ecall_msd
)
{

  boolean ecall_efs_read_status = FALSE;
  char  *efs_app_input_msd_file_name = "ecall//ecall_app_efs_input_msd.txt";
  uint8 temp8;
  int32 tempi32;
  uint32 tempu32; 

  ecall_efs_read_status = ecall_read_persistent_data_from_efs(efs_app_input_msd_file_name);
  ECALL_MSG_1( ECALL_HIGH, "efs read canned msd status %d",
                            ecall_efs_read_status );

  if ((ecall_efs_read_status == TRUE) && (ecall_efs_tokens >= 23))
  {
    temp8 = (uint8 )atoi(ecall_efs_data[1]);
    if(temp8 > 0 && temp8 < ECALL_EFS_U8_MAX)
    {
      ecall_msd->ecall_formatversion     = temp8;
    }
    else
    {
      ECALL_MSG_1( ECALL_ERROR, "not a valid formatversion %d",
                                 temp8 );
    }

    temp8 = (uint8 )atoi(ecall_efs_data[3]);
    if(temp8 > 0 && temp8 < ECALL_EFS_U8_MAX)
    {
      ecall_msd->ecall_messageidentifier = temp8;
    }
    else
    {
      ECALL_MSG_1( ECALL_ERROR, "not a valid value %d",
                                 temp8 );
    }

    if(str_comp(ecall_efs_data[5], "ECALL_MANUAL_INITIATED") == 0)
    {
      ecall_msd->ecall_control_info.activation = ECALL_MANUAL_INITIATED;
    }
    else
    {
      ecall_msd->ecall_control_info.activation = ECALL_AUTOMATIC_INITIATED;
    }

    if(str_comp(ecall_efs_data[7], "ECALL_EMERGENCY") == 0)
    {
      /* EN15722: callType(0): Emergency Call */
      ecall_msd->ecall_control_info.callType = (ecall_type_of_call) 0;  
    }
    else
    {
      /* EN15722: callType(1): Test Call */
      ecall_msd->ecall_control_info.callType = (ecall_type_of_call) 1;  
    }

    ecall_msd->ecall_control_info.ecall_vehicletype = (ecall_vehicle_e_type)atoi(ecall_efs_data[9]);

    ecall_msd->ecall_control_info.positionconfidence = (str_comp(ecall_efs_data[11],"TRUE") == 0 ) ? TRUE : FALSE;

    ecall_memscpy ( (void *)ecall_msd->ecall_vin, sizeof(ecall_msd->ecall_vin),
                    ecall_efs_data[13],           sizeof(ecall_efs_data[13]) );
 
    /* Timestamp is uint32*/
    tempu32 = (ecall_timestamp_type)atoi(ecall_efs_data[15]);
    if(tempu32 < (uint32)ECALL_EFS_U32_MAX)
    {  
      ecall_msd->ecall_timestamp = tempu32; 
    }
    else
    {
      ECALL_MSG_1( ECALL_ERROR, "not a valid timestamp %d",
                                 tempu32 );
      ecall_msd->ecall_timestamp = 0;
    }

    tempi32 = atoi(ecall_efs_data[17]);
    if(tempi32 > (int32)ECALL_MIN_LAT && tempi32 <= (int32)ECALL_MAX_LAT)
    {
      ecall_msd->ecall_vehicle_location.positionlatitude = tempi32;
    }
    else
    {
      ECALL_MSG_1( ECALL_ERROR, "not a valid latitude %d",
                                 tempi32 );
      ecall_msd->ecall_vehicle_location.positionlatitude = ECALL_INVALID_GPS;
    }

    tempi32 = atoi(ecall_efs_data[19]);
    if(tempi32 > (int32)ECALL_MIN_LON && tempi32 <= (int32)ECALL_MAX_LON)
    {
      ecall_msd->ecall_vehicle_location.positionlongitude = tempi32;
    }
    else
    {
      ECALL_MSG_1( ECALL_ERROR, "not a valid longitude %d",
                                 tempi32 );
      ecall_msd->ecall_vehicle_location.positionlongitude = ECALL_INVALID_GPS;
    }

    ecall_msd->ecall_vehicledirection = (uint8 )atoi(ecall_efs_data[21]);

    temp8 = (uint8 )atoi(ecall_efs_data[23]);  
    if(temp8 > 0 && temp8 < ECALL_EFS_U8_MAX)
    {
      ecall_msd->ecall_numberOfPassengers = temp8;
    }
    else
    {
      ECALL_MSG_1( ECALL_ERROR, "not a valid value %d",
                                 temp8 );
      ecall_msd->ecall_numberOfPassengers = 0xFF;
    }
  }
  else if((ecall_efs_read_status == TRUE) && (ecall_efs_tokens < 23))
  {
    ECALL_MSG_1( ECALL_ERROR, "insufficient canned msd tokens %d",
                               ecall_efs_tokens );
    ecall_efs_read_status = FALSE;
  }
  
  return ecall_efs_read_status;
}
#endif /*end FEATURE_ECALL_APP*/

#endif

/* EOF */
