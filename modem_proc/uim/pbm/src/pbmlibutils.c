/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                          PHONEBOOK MANAGER LIBRARY UTILS

GENERAL DESCRIPTION
  This file contains the common utility functions used by PBM lib (external interface).

  Copyright (c) 2002 - 2014 by QUALCOMM Technologies, Inc(QTI).
  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/uim/pbm/src/pbmlibutils.c#1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/16   sp      Logging improvements
05/10/16   sp      Logging improvements
01/31/14   NR      Use v2.0 diag macros
04/10/14   NR      Added memset to set the *ptyped_fields_buf_ptr allocated in function
                   pbm_allocate_fields ()
01/12/14   NR      TMC header deprecation changes
10/14/09  krishnac Support for feature DSDS
07/10/09  ashwanik Support for CSIM and Dual Sim Features
05/04/09   krishna changes for CR 167785
01/17/08   clm     Add FEATURE_PBM_USIM_SUPPORT to featurize USIM features.
10/10/06   cvs     Move messages to PBM's diag msg ID
10/03/06   cvs     Add support for MBN
08/02/06   cvs     Header file cleanup for WINCE
05/03/06   cvs     convert to MMGSDI APIs.  Lint cleanup.
03/24/06   cvs     %x -> 0x%x in log messages
02/28/06   cvs     Add support for MBDN
01/09/06   cvs     Lint fixes
11/16/05   cvs     Lint fixes
11/16/05   cvs     Moved pbm_record_id_to_file_id() to pbmutils.c
08/22/05   AT      Moved contents inside of a FEATURE_PBM_TASK.
08/12/05   AT      Added support for UCS2 encoding.
04/21/05   cvs     Initial Revision
===========================================================================*/
#include "uim_variation.h"
#include "customer.h"
#include "uim_msg.h"
#include "stringl/stringl.h"
#include "pbmutils.h"
#include "pbmlib.h"

/*===========================================================================
                         DEFINITIONS
===========================================================================*/
#define PBM_NUM_PROV_BITS 3
#define PBM_NUM_SLOT_BITS 2
#define PBM_NUM_PB_TYPE_BITS 1
#define PBM_NUM_PB_BITS 5
#define PBM_NUM_LOC_BITS 11
#define PBM_PB_MASK         ((1 << PBM_NUM_PB_BITS) - 1) /* 0x001F */
#define PBM_LOC_MASK     ((1 << PBM_NUM_LOC_BITS) - 1)   /* 0x07FF */
#define PBM_PB_TYPE_MASK     PBM_NUM_PB_TYPE_BITS        /* 0x0001 */
#define PBM_SLOT_MASK       ((1 << PBM_NUM_SLOT_BITS) - 1) /* 0x0003 */
#define PBM_PROV_MASK       ((1 << PBM_NUM_PROV_BITS) - 1) /* 0x0007 */

/* The phone book's record number uses the top 5 bits
 * to indicate the phonebook.  Below is the mapping.
 * Use PBM_RESERVED for new phonebooks in future */
static pbm_device_type pbm_pb_bits_to_pb_id[] = {
PBM_EFS,            /* Phonebook bits 00000 */
PBM_SDN,            /* Phonebook bits 00001 */
PBM_ADN,            /* Phonebook bits 00010 */
PBM_FDN,            /* Phonebook bits 00011 */
PBM_MSISDN,         /* Phonebook bits 00100 */
PBM_ECC,            /* Phonebook bits 00101 */
PBM_SDL,            /* Phonebook bits 00110 */
PBM_MBDN,           /* Phonebook bits 00111 */
PBM_DIALED,         /* Phonebook bits 01000 */
PBM_RCVD,           /* Phonebook bits 01001 */
PBM_MBN,            /* Phonebook bits 01010 */
PBM_MISS,           /* Phonebook bits 01011 */
PBM_LND,            /* Phonebook bits 01100 */
PBM_ECC_OTHER,      /* Phonebook bits 01101 */
PBM_ICI,            /* Phonebook bits 01110 */
PBM_GRPNAME,        /* Phonebook bits 01111 */
PBM_OCI,            /* Phonebook bits 10000 */
PBM_ADN2,           /* Phonebook bits 10001 */
PBM_FDN2,           /* Phonebook bits 10010 */
PBM_BDN2,           /* Phonebook bits 10011 */
PBM_LND2,           /* Phonebook bits 10100 */
PBM_OCI2,           /* Phonebook bits 10101 */
PBM_ICI2,            /* Phonebook bits 10110 */
PBM_SDN2,            /* Phonebook bits 10110 */
PBM_MSISDN2,         /* Phonebook bits 10110 */
PBM_MBDN2,           /* Phonebook bits 10110 */
PBM_MBN2,            /* Phonebook bits 10110 */
PBM_ECC2,            /* Phonebook bits 10110 */
PBM_AAS,             /* Phonebook bits 10111 */
PBM_RESERVED, /* For further use */
PBM_RESERVED, /* For further use */
PBM_RESERVED  /* For further use */
};

/*===========================================================================
                         FUNCTIONS
===========================================================================*/


/*===========================================================================
FUNCTION PBM_ALLOCATE_FIELDS

DESCRIPTION
  Allocate the complete memory needed for the field array in the cache
  structure.  Allocate memory from the global heap.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
boolean pbm_allocate_fields(uint8 **fields_buf_ptr,
                            uint32 *fields_buf_size,
                            uint32 num_fields,
                            const uint32 *sizes)
{
   uint32 i;
   uint32 total_size = 0;
   uint32 mem_offset = 0;
   const uint32 size_one_field = ALIGNED_SIZE(sizeof(pbm_field_s_type));
   /*lint -e{740}*/ /* Definately, an unusual pointer cast. */
   pbm_field_s_type **ptyped_fields_buf_ptr = (pbm_field_s_type **)
                                              fields_buf_ptr;

   PBM_CHECK_PTR3_RET(ptyped_fields_buf_ptr, sizes, fields_buf_size, FALSE);

   *ptyped_fields_buf_ptr = NULL;
   total_size = num_fields * size_one_field;
   for (i = 0; i < num_fields; i++)
   {
      total_size += ALIGNED_SIZE (sizes[i]);
   }

   *fields_buf_size = total_size;

   if (total_size > 0)
   {
      //here intentionaly mem_malloc being used instead PBM_MEM_ALLOC since this
      //call be used from the clients directly.
      *ptyped_fields_buf_ptr = (pbm_field_s_type *)malloc(total_size); //lint !e826 area too small
      if (*ptyped_fields_buf_ptr == NULL)
      {
         return FALSE;
      }

      memset(*ptyped_fields_buf_ptr,0,total_size);
      mem_offset += num_fields * size_one_field;
      for (i = 0; i < num_fields; i++)
      {
         (*ptyped_fields_buf_ptr)[i].buffer_offset = mem_offset;
         mem_offset += ALIGNED_SIZE(sizes[i]);
         mem_offset -= size_one_field; /* take #bytes from next field to its data */
      }
   }
   UIM_MSG_LOW_2("PBM allocated from libc %d bytes 0x%x",
           total_size, fields_buf_ptr ? *fields_buf_ptr : 0);
   return TRUE;
}


/*===========================================================================
FUNCTION PBM_FREE_FIELDS

DESCRIPTION
  Frees the memory allocated by pbm_allocate_fields.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_free_fields( uint8 **fields, int num_fields)
{
   PBM_CHECK_PTR_RET(fields, VOID);

   if ((num_fields > 0) && (*fields != NULL))
   {
      UIM_MSG_LOW_1("PBM freed from libc heap 0x%x", *fields);
   
      //here intentionaly mem_free being used instead PBM_MEM_FREEIF since this
      //call be used from the clients directly. The memory allocated also done using mem_malloc
      free(*fields);
   } else
   {
      UIM_MSG_ERR_2("PBM not freeing memory, num fields %d, ptr 0x%x", num_fields, *fields);
   }
   *fields = NULL;

}


/*===========================================================================
FUNCTION PBM_GET_FIELD_DATA_PTR

DESCRIPTION
  This function is just a shortcut return the pointer to the data buffer for
  a given field number.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void * pbm_get_field_data_ptr(const void *field_buf,
                              int field_num)
{
   uint8 *ptr;
   uint32 offset;
   pbm_field_s_type *ptyped_field_buf = (pbm_field_s_type*)field_buf;

   PBM_CHECK_PTR_RET(ptyped_field_buf, NULL);

   offset = ptyped_field_buf[field_num].buffer_offset;

   ptr = (uint8*)&(ptyped_field_buf[field_num]) + offset;

   return(void*)ptr;
}

/*===========================================================================
FUNCTION PBM_GET_FIELDS_FROM_COMPLEX

DESCRIPTION
  This function converts a complex field into its constituent simple fields and
  adds it to the enumeration list.

DEPENDENCIES
  Not reentrant.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_get_fields_from_complex(pbm_complex_field_s_type *complex_field,
                                              pbm_field_s_type *fields_buf,
                                              int *num_fields)
{
  pbm_complex_result_s_type result = {0};
  *num_fields = pbm_complex_num_sub_fields(complex_field);
  result.num_fields = *num_fields;
  result.fields = (pbm_field_s_type*)fields_buf;
  return pbm_complex_unwrap(complex_field,&result);
}

/*===========================================================================
FUNCTION PBM_COMPLEX_NUM_SUB_FIELDS

DESCRIPTION
  This function returns the total number of sub fields within a complex
  field. If there are complex fields within complex fields, only the sub
  fields (or the leaves) are counted.

DEPENDENCIES
  Not reentrant.

SIDE EFFECTS
===========================================================================*/
int pbm_complex_num_sub_fields(pbm_complex_field_s_type *complex_field)
{
   int num = 0;
   int i;
   pbm_sub_field_s_type *sub_field;

   PBM_CHECK_PTR_RET(complex_field, 0);

   sub_field = PBM_FIELD_ALIGN(complex_field->buf);

   for (i = 0; i < complex_field->num_subfields; i++)
   {
      if (sub_field->field_id == PBM_FT_COMPLEX)
         num += pbm_complex_num_sub_fields(PBM_FIELD_ALIGN(sub_field->buf));
      else
         num++;
      sub_field = PBM_NEXT_SUB_FIELD(sub_field);
   }

   return num;
}


/*===========================================================================
FUNCTION PBM_COMPLEX_UNWRAP

DESCRIPTION
  This function converts a complex field into its constituent simple fields and
  adds it to the enumeration list.

DEPENDENCIES
  Not reentrant.

SIDE EFFECTS
===========================================================================*/
pbm_return_type pbm_complex_unwrap(pbm_complex_field_s_type *complex_field,
                                         pbm_complex_result_s_type *result)
{
   pbm_return_type ret;
   pbm_field_s_type *field;
   pbm_sub_field_s_type *sub_field;
   int i;
   void *field_ptr_to_be_used = NULL;

  PBM_CHECK_PTR2_RET(complex_field, result, PBM_ERROR);

   sub_field = PBM_FIELD_ALIGN(complex_field->buf);

   for ( i = 0; i < complex_field->num_subfields; i++)
   {
      if (sub_field->field_type == PBM_FT_COMPLEX)
      {
         ret = pbm_complex_unwrap(PBM_FIELD_ALIGN(sub_field->buf), result);
         if (ret != PBM_SUCCESS)
         {
            UIM_MSG_ERR_2("Error %d while unwrapping 0x%x", ret, PBM_FIELD_ALIGN(sub_field->buf));
            return ret;
         }
      } else
      {
         if (result->enumerator >= result->num_fields)
            return PBM_ERROR;

         field = &result->fields[result->enumerator++];

         field->field_id = sub_field->field_id;
         field->field_type = sub_field->field_type;
         field->data_len = sub_field->data_len;
         field_ptr_to_be_used = pbm_get_field_data_ptr(field,0);
         if ( field_ptr_to_be_used != NULL)
         {
            memscpy(field_ptr_to_be_used, sub_field->data_len,
                    (uint8*)sub_field->buf, sub_field->data_len);
         }
         else
         {
            return PBM_ERROR;
         }
      }

      sub_field = PBM_NEXT_SUB_FIELD(sub_field);
   }

   return PBM_SUCCESS;
}

/*===========================================================================
FUNCTION PBM_CALCULATE_SIZE_FROM_FIELDS

DESCRIPTION
  Calculates how much memory is needed for a copy of the fields structure
  passed in.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_calculate_size_from_fields( const pbm_field_s_type *fields, int num_fields)
{
   int total_size=0;
   int i;

   PBM_CHECK_PTR_RET(fields, 0);

   if (num_fields > 0)
   {
      total_size = (int) ALIGNED_SIZE ((uint32) num_fields * sizeof(pbm_field_s_type));
      for (i = 0; i < num_fields; i++)
      {
         total_size += ALIGNED_SIZE ((fields)[i].data_len);
      }
   }

   return total_size;
}
/*===========================================================================
FUNCTION pbm_calculate_rec_size_from_fields_sizes

DESCRIPTION
  Calculates how much memory is needed for a copy of the fields structure
  passed in.
DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
int pbm_calculate_rec_size_from_fields_sizes( const uint32 *fields_sizes, int num_fields)
{
   int total_size=0;
   int i;

   PBM_CHECK_PTR_RET(fields_sizes, 0);

   if (num_fields > 0)
   {
      total_size = (int) ALIGNED_SIZE ((uint32) num_fields * sizeof(pbm_field_s_type));
      for (i = 0; i < num_fields; i++)
      {
         total_size += ALIGNED_SIZE (fields_sizes[i]);
      }
   }

   return total_size;
}

pbm_return_type pbm_fill_complex_field(int         num_fields,
                                  const uint8            *data_buf,
                                  int                     data_buf_size,
                                  pbm_field_s_type       *field)
{
  uint16 i = 0,alloc_size = 0; 
  pbm_complex_field_s_type *complex_field = NULL;    
  pbm_sub_field_s_type *sub_field;
  pbm_field_s_type       *input_fields = (pbm_field_s_type*)data_buf;
  int calculated_size;
  uint32 *sub_field_sizes = NULL; 
  void *temp = NULL; 
  calculated_size = pbm_calculate_size_from_fields((pbm_field_s_type*)data_buf,num_fields);
  if ((data_buf_size < calculated_size) || (calculated_size > PBM_MAX_RECORD_SIZE))
  {
     if (calculated_size > PBM_MAX_RECORD_SIZE)
     {
        UIM_MSG_ERR_3("Invalid Record (have %d fields, calculated size %d beyond max needed %d)",
                    num_fields, calculated_size, PBM_MAX_RECORD_SIZE );
     } else
     {
        UIM_MSG_ERR_3("Invalid Record (have %d fields, data given %d needed %d)",
                    num_fields, data_buf_size, calculated_size );
     }
     return PBM_ERROR_INVALID_REC;
  } 
  
  
  //here (ALIGNMENT - 1) need to be added to make it align 
  alloc_size = PBM_COMPLEX_FIELD_HEADER_SIZE + ALIGNMENT - 1;
  //alloc_size = PBM_COMPLEX_FIELD_HEADER_SIZE;  
               
  for(i=0;i<num_fields;i++)
  {
    alloc_size += PBM_SUB_FIELD_HEADER_SIZE + input_fields[i].data_len + ALIGNMENT - 1;      
    //alloc_size += PBM_SUB_FIELD_HEADER_SIZE + input_fields[i].data_len;    
  }
  complex_field =  malloc(alloc_size);
  PBM_CHECK_PTR_RET(complex_field,PBM_ERROR);
  complex_field->num_subfields = num_fields;
  sub_field = PBM_FIELD_ALIGN(complex_field->buf);
   for (i = 0; i < complex_field->num_subfields; i++)
   {
      sub_field->field_id = input_fields[i].field_id;
      sub_field->field_type = input_fields[i].field_type;
      sub_field->data_len = input_fields[i].data_len;
      temp=pbm_get_field_data_ptr(input_fields,i);
      if ( temp == NULL)
      {                                          
        UIM_MSG_ERR_0("Unexpected NULL pointer"); 
        free(complex_field);
        return PBM_ERROR;           
      }
      memscpy((uint8*)sub_field + OFFSET(pbm_sub_field_s_type, buf),input_fields[i].data_len,
	  	     temp ,input_fields[i].data_len);
      sub_field = PBM_NEXT_SUB_FIELD(sub_field);
   }
   sub_field_sizes = malloc(sizeof(uint32) * num_fields);
    if ( sub_field_sizes == NULL)
    {                                          
      UIM_MSG_ERR_0("Unexpected NULL pointer");
      free(complex_field);
      return PBM_ERROR_MEM_FULL;           
    }
     
   (void)pbm_complex_fill_sizes(sub_field_sizes,
                                    num_fields,
                                    complex_field);
   field->data_len = pbm_cal_complex_field_size(num_fields,sub_field_sizes);   
   free(sub_field_sizes);
   pbm_fill_field(field,PBM_FIELD_COMPLEX,PBM_FT_COMPLEX,(uint8*)complex_field,field->data_len);
   free(complex_field);
   return PBM_SUCCESS;
  
}

uint16 pbm_cal_complex_field_size(int         num_fields,
                                  const uint32            *anr_data_sizes)
{
  uint16 i = 0,alloc_size = 0; 
  
  
  //here (ALIGNMENT - 1) need to be added to make it align 
  alloc_size = PBM_COMPLEX_FIELD_HEADER_SIZE + ALIGNMENT - 1;
  //alloc_size = PBM_COMPLEX_FIELD_HEADER_SIZE;  
               
  for(i=0;i<num_fields;i++)
  {
    alloc_size += PBM_SUB_FIELD_HEADER_SIZE + anr_data_sizes[i] + ALIGNMENT - 1;      
    //alloc_size += PBM_SUB_FIELD_HEADER_SIZE + anr_data_sizes[i];    
  }
  return alloc_size;
  
}


/*===========================================================================
FUNCTION PBM_COMPLEX_FILL_SIZES

DESCRIPTION
  This function fills a sizes array, so that a normal fields structure can
  be created with pbm_allocate_fields().

RETURN VALUE
  number of filled fields.

DEPENDENCIES
  Not reentrant.

SIDE EFFECTS
===========================================================================*/
int pbm_complex_fill_sizes(uint32 *sizes,
                                  int max_num_sub_fields,
                                  pbm_complex_field_s_type *complex_field)
{
   int i;
   pbm_sub_field_s_type *sub_field;
   int filled;
   int total_filled = 0;

   PBM_CHECK_PTR2_RET(sizes, complex_field, 0);

   sub_field = PBM_FIELD_ALIGN(complex_field->buf);

   for (i = 0; (i < complex_field->num_subfields) && (max_num_sub_fields > 0); i++)
   {
      if (sub_field->field_type == PBM_FT_COMPLEX)
      {
         filled = pbm_complex_fill_sizes(sizes,
                                         max_num_sub_fields,
                                         (pbm_complex_field_s_type *)sub_field->buf); //lint !e826 suspicious ptr conversion
         sizes += filled;
         max_num_sub_fields -= filled;
         total_filled += filled;
      } else
      {
         *sizes = sub_field->data_len;
         sizes++;
         max_num_sub_fields--;
         total_filled++;
      }

      sub_field = PBM_NEXT_SUB_FIELD(sub_field);
   }

   return total_filled;
}


/*===========================================================================
FUNCTION PBM_FILL_FIELD

DESCRIPTION
  This function is just a shortcut to fill in the four different fields
  which make up the field structure.

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
void pbm_fill_field(pbm_field_s_type *field, pbm_field_id_e_type field_id,
                    pbm_field_type_e_type field_type, const byte *buffer, uint16 data_len)
{
   void *field_data_ptr;

   PBM_CHECK_PTR2_RET(field, buffer, VOID);

   if (!data_len)
   {
      UIM_MSG_ERR_1("Invalid data len passed field_id 0x%x", field_id);
      return;
   }

   field->field_id   = field_id;
   field->field_type = field_type;

   field_data_ptr = pbm_get_field_data_ptr(field,0);
   memscpy(field_data_ptr, data_len, buffer, data_len);
   field->data_len   = data_len;
}


/*==========================================================================*
*                      PBM Record ID UTILITIES                             *
*==========================================================================*/


/*===========================================================================
FUNCTION pbm_session_record_id_to_location

DESCRIPTION
Converts a record ID to a location (extracts the location information from
the record ID.)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_session_record_id_to_location(pbm_record_id_type record_id)
{
   /* Return lower 11 bits of record ID*/
   return((uint16)(record_id & PBM_LOC_MASK));
}

/*===========================================================================
FUNCTION pbm_record_id_to_location

DESCRIPTION
Converts a record ID to a location (extracts the location information from
the record ID.)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_record_id_to_location(uint16 record_id)
{
   /* Return lower 11 bits of record ID*/
   return((uint16)(record_id & PBM_LOC_MASK));
}


/*===========================================================================
FUNCTION PBM_RECORD_ID_TO_PHONEBOOK

DESCRIPTION
Converts a record ID to a phonebook ID (extracts the phonebook ID information
from the record ID.)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_device_type pbm_record_id_to_phonebook(uint16 record_id)
{
  int phonebook;
  pbm_device_type retval;

  /* Get upper 5 bits of record ID*/
  phonebook = (record_id >> PBM_NUM_LOC_BITS) & PBM_PB_MASK;

  retval = pbm_pb_bits_to_pb_id[phonebook];
  if (retval == PBM_RESERVED)
  {
    UIM_MSG_ERR_1("Unknown phonebook code %d.", phonebook);
    retval = PBM_EFS;
  }

  return retval;
}

/*===========================================================================
FUNCTION PBM_LOCATION_TO_RECORD_ID

DESCRIPTION
Converts a a phonebook ID plus a location to a record ID

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
uint16 pbm_location_to_record_id(pbm_device_type pb_id, int location)
{
  uint16 pb_code = 0;
  uint16 i;

  for (i = 0; i < (uint16) ARR_SIZE(pbm_pb_bits_to_pb_id); i++)
  {
    if (pbm_pb_bits_to_pb_id[i] == pb_id)
    {
      pb_code = i;
      break;
    }
  }

  if (i >= (uint16) ARR_SIZE(pbm_pb_bits_to_pb_id))
  {
    UIM_MSG_ERR_1("Invalid pbm_device_type for conversion: %d.", pb_id);
  }

  /* Put pb_id in upper 5 bits and location in lower 11 bits*/
  return ( ((pb_code & PBM_PB_MASK) << PBM_NUM_LOC_BITS) |
           ((uint16) location & PBM_LOC_MASK) );
}

/*===========================================================================
FUNCTION pbm_session_location_to_record_id

DESCRIPTION
Converts a a phonebook ID plus a location to a record ID

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_record_id_type pbm_session_location_to_record_id(pbm_phonebook_type pb_id, int location)
{
   uint16 pb_code = 0;
   uint16 i;

   for (i = 0; i < (pbm_record_id_type) ARR_SIZE(pbm_pb_bits_to_pb_id); i++)
   {
      if (pbm_pb_bits_to_pb_id[i] == pb_id.device_type)
      {
         pb_code = i;
         break;
      }
   }

   if (i >= (pbm_record_id_type) ARR_SIZE(pbm_pb_bits_to_pb_id))
   {
      UIM_MSG_ERR_1("Invalid pbm_phonebook_type for conversion: %d.", pb_id.device_type);
   }

   /* Put prov_type <3 bits> + slot <2 bits> + pb_category <1 bit> + pb_code <5 bits> + location <11 bits> */
   return((((uint32)pb_id.prov_type & PBM_PROV_MASK) << (PBM_NUM_SLOT_BITS + PBM_NUM_PB_TYPE_BITS + PBM_NUM_PB_BITS + PBM_NUM_LOC_BITS)) |
          ((pb_id.slot_id & PBM_SLOT_MASK) << (PBM_NUM_PB_TYPE_BITS + PBM_NUM_PB_BITS + PBM_NUM_LOC_BITS)) |
          (((uint32)pb_id.pb_category & PBM_PB_TYPE_MASK) << (PBM_NUM_PB_BITS + PBM_NUM_LOC_BITS)) |
          ((pb_code & PBM_PB_MASK) << PBM_NUM_LOC_BITS) |
          ((uint32) location & PBM_LOC_MASK) );
}


/*===========================================================================
FUNCTION pbm_session_record_id_to_phonebook_type

DESCRIPTION
Converts a record ID to a phonebook ID (extracts the phonebook ID information
from the record ID.)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_phonebook_type pbm_session_record_id_to_phonebook_type(pbm_record_id_type record_id)
{
   uint32 phonebook;
   pbm_phonebook_type pb_id = {PBM_GPB};

   /* Get upper 5 bits of record ID*/
   phonebook = (record_id >> PBM_NUM_LOC_BITS) & PBM_PB_MASK;

   if(phonebook < ARR_SIZE(pbm_pb_bits_to_pb_id))
   {
     pb_id.device_type = pbm_pb_bits_to_pb_id[phonebook];
   }

   if (pb_id.device_type == PBM_RESERVED)
   {
      UIM_MSG_ERR_1("Unknown phonebook code %d.", phonebook);
      pb_id.device_type = PBM_EFS;
   }

   /* retrieve provision */
   pb_id.prov_type = (pbm_provision_enum_type)((record_id >> (PBM_NUM_SLOT_BITS + PBM_NUM_PB_TYPE_BITS + PBM_NUM_PB_BITS + PBM_NUM_LOC_BITS)) & PBM_PROV_MASK);

   /* retrieve slot */
   pb_id.slot_id = (record_id >> (PBM_NUM_PB_TYPE_BITS + PBM_NUM_PB_BITS + PBM_NUM_LOC_BITS)) & PBM_SLOT_MASK;

   /* retrieve pb category */
   pb_id.pb_category = (pbm_pb_category_enum_type)((record_id >> (PBM_NUM_PB_BITS + PBM_NUM_LOC_BITS)) & PBM_PB_TYPE_MASK);

   return pb_id;
}

/*===========================================================================
FUNCTION PBM_PB_ID_TO_SIM_ID

DESCRIPTION
  This function will find the cache associated with a given pb_id.
  It returns NULL if there is none.

SIDE EFFECTS
===========================================================================*/
pbm_slot_type pbm_device_type_to_sim_id(pbm_device_type device_type)
{
  pbm_slot_type slot = PBM_NUM_SLOTS;
  switch (device_type)
  {
    case PBM_ADN:
    case PBM_FDN:
    case PBM_LND:
    case PBM_SDN:
    case PBM_MSISDN:
    case PBM_MBN:
    case PBM_GRPNAME:
    case PBM_AAS:
    case PBM_ICI:
    case PBM_OCI:
    case PBM_MBDN:
    case PBM_ECC:
      slot =  PBM_SLOT_1;
      break;
    case PBM_ADN2:
    case PBM_FDN2:
    case PBM_LND2:
    case PBM_SDN2:
    case PBM_MSISDN2:
    case PBM_MBN2:
    case PBM_ECC2:
    case PBM_ICI2:
    case PBM_OCI2:
    case PBM_MBDN2:
      slot =  PBM_SLOT_2;
      break;
    default:
      UIM_MSG_ERR_1("pbm_device_type_to_sim_id called with %d (an invalid device)", device_type);
      slot =  PBM_NUM_SLOTS;
      break;
  }
  return slot;
}



/*===========================================================================
FUNCTION pbmutils_wstrlen;

DESCRIPTION
  int pbmutils_wstrlen(const uint16 *ucs2_str);

  ucs2_str:   [in]   The UCS2 string to count the characters.

  This function counts the number of UCS2 characters in a string.  The
  string must be terminated by 0x0000.

DEPENDENCIES
  None.

RETURN VALUE
  The number of UCS2 characters in the given string.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 pbmutils_wstrlen(const uint16 *ucs2_str)
{
   uint32 retval = 0;                  /* Value to be returned. */
   if (ucs2_str)
      while (*ucs2_str++)
      {
         retval++;
      } /* Increment retval until 0x0000 */
   return retval;                      /* Return what we counted. */
}


/*===========================================================================
FUNCTION pbm_pb_type_and_slot_to_session_type

DESCRIPTION
retrieves session id from record info

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_session_enum_type pbm_pb_type_and_slot_to_session_type(
  pbm_slot_type                   slot_id,
  pbm_pb_category_enum_type       pb_type
)
{
  if ( PBM_GPB == pb_type )
  {
    if ( PBM_SLOT_1 == slot_id )
    {
      return PBM_SESSION_GPB_1;
    }
    else if ( PBM_SLOT_2 == slot_id )
    {
      return PBM_SESSION_GPB_2;
    }
#ifdef FEATURE_TRIPLE_SIM
    else if ( PBM_SLOT_3 == slot_id )
    {
      return PBM_SESSION_GPB_3;
    } 
#endif
  }
  return PBM_SESSION_DEFAULT;
}/* pbm_pb_type_and_slot_to_session_type */


/*===========================================================================
FUNCTION PBM_ADN_RECORD_ID_TO_PHONEBOOK

DESCRIPTION
Converts a record ID to a phonebook ID (extracts the phonebook ID information
from the record ID.)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_device_type pbm_adn_record_id_to_file_device_type(pbm_adn_record_id_type record_id)
{
   int phonebook;
   pbm_device_type retval;

   /* Get upper 5 bits of record ID*/
   phonebook = (record_id >> PBM_NUM_LOC_BITS) & PBM_PB_MASK;

   retval = pbm_pb_bits_to_pb_id[phonebook];
   if (retval == PBM_RESERVED)
   {
      UIM_MSG_ERR_1("Unknown phonebook code %d.", phonebook);
      retval = PBM_EFS;
   }

   return retval;
}


/*===========================================================================
FUNCTION pbm_file_location_to_adn_record_id

DESCRIPTION
Converts a file_id + location to adn rec id

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_adn_record_id_type pbm_file_location_to_adn_record_id(pbm_file_type file_id, int location)
{
   uint16 pb_code = 0;
   uint16 i;

   for (i = 0; i < (uint16) ARR_SIZE(pbm_pb_bits_to_pb_id); i++)
   {
      if (pbm_pb_bits_to_pb_id[i] == PBM_ADN)
      {
         pb_code = i;
         break;
      }
   }

   if (i >= (uint16) ARR_SIZE(pbm_pb_bits_to_pb_id))
   {
      UIM_MSG_ERR_1("Invalid pbm_phonebook_type for conversion: %d.", file_id);
   }

   /* Put pb_id in upper 5 bits and location in lower 11 bits*/
   return( ((pb_code & PBM_PB_MASK) << PBM_NUM_LOC_BITS) |
           ((uint16) location & PBM_LOC_MASK) );
}


/*===========================================================================
FUNCTION pbm_record_id_to_device_type

DESCRIPTION
Converts a record ID to a phonebook ID (extracts the phonebook ID information
from the record ID.)

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_device_type pbm_record_id_to_device_type(pbm_record_id_type record_id)
{
   uint32 phonebook;
   pbm_device_type retval = PBM_RESERVED;

   /* Get upper 5 bits of record ID*/
   phonebook = (record_id >> PBM_NUM_LOC_BITS) & PBM_PB_MASK;

   if(phonebook < ARR_SIZE(pbm_pb_bits_to_pb_id))
   {
     retval = pbm_pb_bits_to_pb_id[phonebook];
   }

   if (retval == PBM_RESERVED)
   {
      UIM_MSG_ERR_1("Unknown phonebook code %d.", phonebook);
      retval = PBM_EFS;
   }

   return retval;
}


/*===========================================================================
FUNCTION pbm_uim_device_type_to_phonebook_type

DESCRIPTION
DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_phonebook_type pbm_uim_device_type_to_phonebook_type(pbm_uim_app_enum_type uim_type,
                                                         pbm_device_type device_type)
{
   pbm_phonebook_type pb_id = {PBM_GPB};
   pbm_session_enum_type session_type;

   switch (uim_type)
   {
      case PBM_UIM_APP_LPB_GW_PROV:
         session_type = PBM_SESSION_LPB_GW;
      break;

      case PBM_UIM_APP_LPB_1X_PROV:
         session_type = PBM_SESSION_LPB_1X;
      break;

#ifdef FEATURE_DUAL_SIM
      case PBM_UIM_APP_GPB_SLOT_2:
         session_type = PBM_SESSION_GPB_2;
         break;

      case PBM_UIM_APP_LPB_GW_SECONDARY:
         session_type = PBM_SESSION_LPB_GW_SECONDARY;
         break;

      case PBM_UIM_APP_LPB_1X_SECONDARY:
         session_type = PBM_SESSION_LPB_1X_SECONDARY;
         break;
#ifdef FEATURE_TRIPLE_SIM
      case PBM_UIM_APP_GPB_SLOT_3:
         session_type = PBM_SESSION_GPB_3;
         break;

      case PBM_UIM_APP_LPB_GW_TERTIARY:
         session_type = PBM_SESSION_LPB_GW_TERTIARY;
         break;

      case PBM_UIM_APP_LPB_1X_TERTIARY:
         session_type = PBM_SESSION_LPB_1X_TERTIARY;
         break;
#endif
#endif

      case PBM_UIM_APP_GPB_SLOT_1:
      default:
         session_type = PBM_SESSION_DEFAULT;
      break;
   }

   pb_id.prov_type = pbm_session_type_to_provision_type(session_type);
   if(session_type == PBM_SESSION_GPB_1)
   {
     pb_id.slot_id = (uint16) PBM_SLOT_1;
   }
   else if(session_type == PBM_SESSION_GPB_2)
   {
     pb_id.slot_id = (uint16) PBM_SLOT_2;
   }
#ifdef FEATURE_TRIPLE_SIM
   else if(session_type == PBM_SESSION_GPB_3)
   {
     pb_id.slot_id = (uint16) PBM_SLOT_3;
   }
#endif
   pb_id.pb_category = pbm_session_type_to_pb_category(session_type);
   pb_id.device_type = device_type;

   return pb_id;
}



/*===========================================================================
FUNCTION pbm_session_type_to_pb_category

DESCRIPTION
Converts a session_type to pb_id

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_pb_category_enum_type pbm_session_type_to_pb_category(pbm_session_enum_type session_type)
{
   pbm_pb_category_enum_type res = PBM_GPB;
   switch (session_type)
   {
      case PBM_SESSION_GPB_1:
#ifdef FEATURE_DUAL_SIM
      case PBM_SESSION_GPB_2:
#ifdef FEATURE_TRIPLE_SIM
      case PBM_SESSION_GPB_3:
#endif 
#endif
         res = PBM_GPB;
         break;
      case PBM_SESSION_LPB_GW:
      case PBM_SESSION_LPB_1X:
      #ifdef FEATURE_DUAL_SIM
      case PBM_SESSION_LPB_GW_SECONDARY:
      case PBM_SESSION_LPB_1X_SECONDARY:
#ifdef FEATURE_TRIPLE_SIM
      case PBM_SESSION_LPB_GW_TERTIARY:
      case PBM_SESSION_LPB_1X_TERTIARY:
#endif 
      #endif
         res = PBM_LPB;
         break;
	  default:
         break;
   }
   return res;
}


/*===========================================================================
FUNCTION PBM_SESSION_ID_TO_PROVISION_TYPE

DESCRIPTION
Converts a a session ID to provision type

DEPENDENCIES

SIDE EFFECTS
===========================================================================*/
pbm_provision_enum_type pbm_session_type_to_provision_type(pbm_session_enum_type session_type)
{
   pbm_provision_enum_type res;

   switch (session_type)
   {
      case PBM_SESSION_LPB_GW:
         res = PBM_PROVISION_GW;
         break;

      case PBM_SESSION_LPB_1X:
         res = PBM_PROVISION_1X;
         break;

#ifdef FEATURE_DUAL_SIM
     case PBM_SESSION_LPB_GW_SECONDARY:
         res = PBM_PROVISION_GW_SECONDARY;
         break;

     case PBM_SESSION_LPB_1X_SECONDARY:
         res = PBM_PROVISION_1X_SECONDARY;
         break;
#ifdef FEATURE_TRIPLE_SIM
     case PBM_SESSION_LPB_GW_TERTIARY:
         res = PBM_PROVISION_GW_TERTIARY;
         break;

     case PBM_SESSION_LPB_1X_TERTIARY:
         res = PBM_PROVISION_1X_TERTIARY;
         break;
#endif 
#endif

      case PBM_SESSION_GPB_1:
#ifdef FEATURE_DUAL_SIM
      case PBM_SESSION_GPB_2:
#ifdef FEATURE_TRIPLE_SIM
      case PBM_SESSION_GPB_3:
#endif 
#endif /*FEATURE_DUAL_SIM*/
      default:
         res = PBM_PROVISION_NONE;
         break;
   }
   return res;
}

/*===========================================================================
FUNCTION PBM_DEVICE_ID_TO_PB_ID

DESCRIPTION
  This function will find the phonebook associated with a given init file name
  and sim id.
  It returns PBM_DEFAULT if there is none.

SIDE EFFECTS
===========================================================================*/
pbm_phonebook_type pbm_device_id_to_pb_id(pbm_session_enum_type session_type,
                                          pbm_device_type device_type)
{
   pbm_phonebook_type pb_id = {PBM_GPB};


   if(session_type == PBM_SESSION_GPB_1)
   {
     pb_id.slot_id = (uint16) PBM_SLOT_1;
   }
   else if(session_type == PBM_SESSION_GPB_2)
   {
     pb_id.slot_id = (uint16) PBM_SLOT_2;
   }
#ifdef FEATURE_TRIPLE_SIM
   else if(session_type == PBM_SESSION_GPB_3)
   {
     pb_id.slot_id = (uint16) PBM_SLOT_3;
   }
#endif
   pb_id.pb_category = pbm_session_type_to_pb_category(session_type);
   pb_id.prov_type = pbm_session_type_to_provision_type(session_type);
   pb_id.device_type = device_type;

   return pb_id;
}



