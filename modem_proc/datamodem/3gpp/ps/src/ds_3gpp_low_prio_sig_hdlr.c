/*!
  @file
  ds_3gpp_low_prio_sig_hdlr.c

  @brief
  Handles low priority signaling

  @detail
  OPTIONAL detailed description of this C module.
  - DELETE this section if unused.

*/

/*===========================================================================

  Copyright (c) 2015 - 2016 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/ds_3gpp_low_prio_sig_hdlr.c#1 $

when         who     what, where, why
--------     ---     --------------------------------------------------------
10/09/15     vb      Low priority signaling handler

===========================================================================*/
#include "ds_3gpp_pdn_context.h" 
#include "ds_eps_pdn_context.h"
#include "ds3gmmgsdiif.h"
#include "ds_3gpp_low_prio_sig_hdlr.h"
#include "ds_3gpp_plmn_hdlr.h"
#include "ds_3gppi_utils.h"
#include "ds_3gpp_nv_manager.h"

/*----------------------------------------------------------------------- 
  0x80 indicates low priority signaling tag
  ----------------------------------------------------------------------*/
#define DS_3GPP_LOW_PRIORITY_SIG_TAG 0x80
#define DS_3GPP_AC_CLASS_12_to_14    0x70
#define DS_3GPP_AC_CLASS_11_or_15    0x88
#define DS_3GPP_INVALID_TAG          0xFF 

/*===========================================================================
FUNCTION DS_3GPP_LOW_PRIO_GET_NAS_CONFIG_TAG_LENGTH
 
DESCRIPTION 
 The function takes NAS CONFIG read from UIM and for every TAG gives the
 tag length
 
 It also gives the offset --> Offset signifies how many bytes the length field
 occupy

PARAMETERS 
  Input File stream 
  Offset
 
DEPENDENCIES 
  The first byte must always be pointing to the stream length
  
RETURN VALUE 
  tag_length
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
uint8 ds_3gpp_low_prio_get_nas_config_tag_length
(
   uint8   *input_file_stream,
   uint8   *offset
)
{
  uint8 tag_length = 0; 
  uint8 first_byte = 0;
  uint8 i = 0;
  uint8 number_of_octets = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if((input_file_stream == NULL) || (offset == NULL))
  {
    DS_3GPP_MSG2_ERROR("Input_file_stream:0x%x, Offset:0x%x",
                       input_file_stream,
                       offset);
    return tag_length;
  }

  /*---------------------------------------------------------------------- 
    Point the first byte to stream length
   
    If firstbyte has the MSB set, IT means the length field stretches
    over multiple bytes, Multiply by 256 and add the value for each bit
    ----------------------------------------------------------------------*/
  first_byte = *input_file_stream++;

  if(first_byte & 0x80)
  { 
    /* Multi byte length */
    number_of_octets = first_byte & 0x7F;
    for(i = 0; i < number_of_octets; i++ )
    {
      tag_length *= 256;
      tag_length += *input_file_stream++;
    }
    *offset = (uint8)number_of_octets;
  } 
  else
  {
    /*Single byte length*/
    tag_length = first_byte;
    *offset = 1;
  }

  DS_3GPP_MSG1_HIGH("Tag length returned is %d",tag_length);
  return tag_length;
}/*ds_3gpp_low_prio_get_nas_config_tag_length*/



/*===========================================================================
FUNCTION DS_3GPP_READ_LOW_PRIO_BIT
 
DESCRIPTION
  This function takes a byte stream and checks to see if Low_prio_signaling
  is present in the byte stream.
 
  It returns TRUE if Low priority bit is present in the file.
  The value of the low priority bit is returned through a ptr

PARAMETERS 
  Byte stream
  Length of the byte stream
  Output ptr to write the low prio bit.
 
DEPENDENCIES 
  None. 
  
RETURN VALUE 
  Boolean 
 
SIDE EFFECTS 
  None 
  
===========================================================================*/
boolean ds_3gpp_read_low_prio_bit
(
   uint8   *input_file_stream,
   uint8    input_file_length,
   boolean *low_prio_bit_ptr
)
{
  boolean ret_val          = FALSE;
  boolean lowprio_tag_set  = FALSE;
  uint8   offset_length    = 0;
  uint8   tag_length       = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ((input_file_stream == NULL) || (low_prio_bit_ptr == NULL))
  {
    DS_3GPP_MSG2_ERROR("input_file_stream is 0x%x,low_prio_bit is 0x%x",
                       input_file_stream,low_prio_bit_ptr);
    return ret_val;
  }

  *low_prio_bit_ptr = FALSE;
  
  /*----------------------------------------------------------------------- 
    Check for tag 80 in the input file stream
    80 would mean low priority signaling
    -----------------------------------------------------------------------*/

  while ((input_file_length > 0) && (lowprio_tag_set == FALSE))
  {
    switch (*input_file_stream)
    {
      case DS_3GPP_LOW_PRIORITY_SIG_TAG:
        input_file_stream++;
        input_file_length--;
        tag_length = ds_3gpp_low_prio_get_nas_config_tag_length(input_file_stream,
                                                   &offset_length);
        input_file_length = input_file_length -(tag_length + offset_length);
        /*------------------------------------------------------------------- 
          Tag length for low priority signaling should be 1 byte
          -------------------------------------------------------------------*/
        if (tag_length == 1)
        {
          input_file_stream++;
          ret_val = TRUE;
          *low_prio_bit_ptr = *input_file_stream;
        }
        lowprio_tag_set = TRUE;
        break;

      default:
       if(*input_file_stream == DS_3GPP_INVALID_TAG)
       {
         DS_3GPP_MSG0_HIGH("Invalid tag found, return");
         return ret_val;
       }
       input_file_stream++;
       input_file_length--;
       tag_length = ds_3gpp_low_prio_get_nas_config_tag_length(input_file_stream,
                                                   &offset_length);
       input_file_length = input_file_length -(tag_length + offset_length);
       input_file_stream = input_file_stream + (tag_length + offset_length);
    }
  }

  return ret_val;

}/*ds_3gpp_read_low_prio_bit*/

/*===========================================================================
FUNCTION  DS_3GPP_GET_LOW_PRIORITY_SIGNALING_BIT

DESCRIPTION
  The function decides the low priority signaling bit's value.
  ie
  Low priority signaling bit will be set if UE is configured for
  Low priority signaling with the below exceptions
 
  Low priority signaling will not be set for
  1. Emergency calls
  2. If emergency PDN is already UP
  3. Access Class from the SIM is 11-15
  
 
PARAMETERS
  pdn_cntxt_p - PDN context pointer
 
DEPENDENCIES
  None.

RETURN VALUE 
  Boolean
 
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds_3gpp_get_low_priority_signaling_bit
(
  ds_pdn_context_s       *pdn_cntxt_p
)
{
  sys_modem_as_id_e_type subs_id          = SYS_MODEM_AS_ID_NONE;
  boolean                low_prio_bit     = FALSE;
  ds_eps_attach_sm_s    *attach_sm_ptr    = NULL;
  byte                  *efnasconfig      = NULL;
  uint32                 efacc_class      = 0;
  sys_plmn_id_s_type     current_plmn_id;
  sys_plmn_id_s_type     hplmn_id;
  uint16                 mcc_hplmn        = 0;
  uint16                 mnc_hplmn        = 0;
  uint16                 mcc_current_plmn = 0;
  uint16                 mnc_current_plmn = 0;
 /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  if(!ds_3gpp_pdn_cntxt_get_subs_id_from_pdn(pdn_cntxt_p,&subs_id))
  {
    DS_3GPP_MSG1_ERROR("PDN context could not fetch valid Subs Id(CM) %d"
                        ,subs_id);
    return FALSE;
  }

  /*---------------------------------------------------------------------- 
    Low priority signaling is supported for only REL 10 and upwards
    ----------------------------------------------------------------------*/
  if(!ds_3gpp_nv_manager_get_nv_lte_rel_10(subs_id))
  {
    DS_3GPP_MSG0_ERROR("PRE REL 10, Low priority signaling not supported");
    return FALSE;
  }

  do
  {

    /*------------------------------------------------------------------- 
      Get the low priority signaling bit from MMGSDI.
      If it reurns FALSE
        Try to read the low priority signaling bit from NAS EFS
      IF it returns TRUE
        Check the low priority signaling bit(low_prio_bit)
     
      If the low_prio bit is TRUE only then make other checks else exit out
      of the while loop
      -------------------------------------------------------------------*/
    if(!ds3g_mmgsdi_get_low_prio_signaling(subs_id,&low_prio_bit))
    {
      DS_3GPP_MSG0_HIGH("Try to fetch low prio bit from NAS EFS");
      efnasconfig = ds_3gpp_nv_manager_get_efnas_config(subs_id);

      if (efnasconfig != NULL)
      {
        if(!ds_3gpp_read_low_prio_bit((uint8 *)efnasconfig,
                               DS_3GPP_EFNAS_CONFIG_SIZE,
                               &low_prio_bit))
        {
          DS_3GPP_MSG0_ERROR("EFNAS CONFIG is not configured, Exit");
          low_prio_bit = FALSE;
          break;
        }
      }
    }

    /*------------------------------------------------------------------- 
      At this point every effort to read the low priority signaling is
      completed, IF low prio bit is FALSE, exit from the while loop
      Only if low priority signaling bit is set then proceed to the
      below conditions to see if the low priority signaling bit can be
      overridden
    -------------------------------------------------------------------*/
    if (!low_prio_bit)
    {
      DS_3GPP_MSG0_HIGH("Low prio bit is not SET, RETURN");
      break;
    }

    /*-------------------------------------------------------------------- 
      No low priority signaling if
       1. Emergency call is attempted
       2. If this is an attach request and NAS is responding to
          a page request
       3. AC is between 11-15 (Spl rules apply)
    --------------------------------------------------------------------*/
    if(DS_3GPP_PDN_SUPPORTS_EMERGENCY_SERVICES(pdn_cntxt_p))
    {
      low_prio_bit = FALSE;
      DS_3GPP_MSG0_MED("Emergency call is being brought up");
      break;
    }

   /*--------------------------------------------------------------------- 
    Get the attach sm ptr. This will be used to know if the current request
    attach request and if responding to paging is TRUE
    ---------------------------------------------------------------------*/
    attach_sm_ptr = ds_eps_get_attach_sm_per_sub_id(subs_id);

    if ((attach_sm_ptr != NULL) && (attach_sm_ptr->responding_to_page == TRUE))
    {
      low_prio_bit = FALSE;
      DS_3GPP_MSG0_MED("Attach request and paging response is set");
      break;
    }

    
     
    /*------------------------------------------------------------------- 
      Get the Access class
      If the access class is 12-13-14
          Check the MCC of the Home PLMN and current Serving PLMN
          If match occurs, low_prio_bit cannot be set
      If the access class is 11 or 15
          Check the current PLMN with EHPLMN(if configured),
          If that does not match, check with current HPLMN.
          If either matches no low priority signaling
      -------------------------------------------------------------------*/

    if(ds3g_mmgsdi_get_efacc_class(subs_id,&efacc_class))
    {
      DS_3GPP_MSG0_HIGH("EFACC set");

      ds_3gpp_pdn_cntx_get_current_plmn_ex(&current_plmn_id, subs_id);

      ds_3gpp_get_hplmn_id_per_sub(subs_id,&hplmn_id);

      if ((efacc_class & DS_3GPP_AC_CLASS_12_to_14) != 0)
      {
        DS_3GPP_MSG0_HIGH("AC class is 12 to 14");
        /*-------------------------------------------------------------- 
          Get the MCC of the Home PLMN country and compare it with
          current PLMN's MCC, If a match occurs no low priority signaling
          -------------------------------------------------------------*/
        ds_3gpp_extract_mcc_mnc_from_plmn_id(&current_plmn_id,
                                             &mcc_current_plmn,
                                             &mnc_current_plmn);

        ds_3gpp_extract_mcc_mnc_from_plmn_id(&hplmn_id,
                                             &mcc_hplmn,
                                             &mnc_hplmn);

        if((mcc_hplmn == mcc_current_plmn) && (mcc_hplmn != 0))
        {
          DS_3GPP_MSG0_HIGH("AC 12-14 and MCC match");
          low_prio_bit = FALSE;
        }

        break;
      }

      if((efacc_class & DS_3GPP_AC_CLASS_11_or_15)!= 0)
      {
        DS_3GPP_MSG0_HIGH("AC class is 11 or 15");
        
        /*--------------------------------------------------------- 
          See if current PLMN is a part of EHPLMN list
          If it is, low_prio bit is set to FALSE
          ---------------------------------------------------------*/
        if(ds_3gpp_is_plmn_in_ehplmn_list(current_plmn_id,subs_id))
        {
          DS_3GPP_MSG0_HIGH("EHPLMN match, No low priority signaling");
          low_prio_bit = FALSE;
          break;
        }
        /*---------------------------------------------------------- 
          Now check if the PLMN is a part of Home PLMN
          ----------------------------------------------------------*/
        if(sys_plmn_match(hplmn_id, current_plmn_id))
        {
          DS_3GPP_MSG0_HIGH("Home PLMN match, No Low priority signalling");
          low_prio_bit = FALSE;
          break;
        }
      }
    }

  }while(0);

  DS_3GPP_MSG1_HIGH("low_prio_bit is %d",low_prio_bit);
  return low_prio_bit;

}/* ds_3gpp_get_low_priority_signaling_bit */

