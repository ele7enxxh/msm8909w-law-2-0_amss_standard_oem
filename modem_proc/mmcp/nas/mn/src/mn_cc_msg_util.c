/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated and its licensors.  All Rights 
Reserved.  QUALCOMM Proprietary.  Export of this technology or software 
is regulated by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/mn/vcs/mn_cc_msg_util.c_v   1.16   19 Jul 2002 10:55:34   trebman  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/mn/src/mn_cc_msg_util.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/01/04    AR     Add setting other ITC support.

3/20/01     CD     Replaced old header with QCOM header
                   Removed #ifdef ASW_SIMBA

5/3/01      CD     Corrected several compilation warnings C4244

5/3/01      CD     corrected several compile warning C4244 in MN_put_llc_data, 
                   MN_put_hlc_data

5/4/01      CD     Initialized ie_length=0 in MN_get_bearer_capability_data()
                   to avoid compile warning C4701
                   Fixed several compile warnings C4244 conversion from int to byte

5/9/01      CD     Removed unnecesary header file includes

5/14/01     CD     Fixed indentation in MN_get_cuase_data()

5/21/01     CD     Removed declaration of 'return_value' in 
                   MN_get_bearer_capability_data  and MN_get_cause_data 
                   since it is not used.  

                   Removed #include ms.h and couse.h as they are not needed

26-Jul-2001  VT   CS Data: Added support for the following parameters in functions, 
                    MN_put_bearer_capability & MN_get_bearer_capability_data:
                         compression, access_id, layer_1_id, & layer_2_id.
                    Added the left-shifting by 3 bits while filling 
                      acceptable_channel_codings in MN_put_bearer_capability().
                    Added the right-shifting by 3 bits while filling 
                      acceptable_channel_codings in MN_get_bearer_capability_data().
                    Corrected the bit-mask from 0x80 to 0x08 while filling 
                      transfer_mode in MN_get_bearer_capability_data().

27-Jul-2001  VT   CS Data: Added a lot of error checking in 
                    MN_get_bearer_capability_data().

8/10/01      CD   Removed unnecessary queue.h

08/16/01     AB   Renamed MMI references to CM.
08/31/01     TS   Updated to reflect change from TRANSPARENT to TRANSPARENT_DATA

01/25/02     CD   Replaced IEI by boolean flag
                  Updated Copyright
                  Replaced ERR() by MSG_ERROR()

03/12/02     AB   Added MN_put_invoke_cd_data() to encodes the Call Deflection
                    invoke components to be sent in DISCONNECT message.

                  Fixed the encoding of coding standard field in
                    MN_put_cause_data().

19-Feb-2002  VT   GSM EFR: In MN_put_bearer_capability(), added code for 
                    filling octets 3a, etc., in the OTA Bearer Capability IE.

04/02/02     AB   Corrected setting of the parameters length in 
                    MN_put_invoke_cd_data().

05/06/02     AB   Added SS_VERSION_PHASE1,2 to replace the
                    constant 0x00 used for SS VERSION.

05/15/02    TMR   Fixed bug in MN_put_bearer_capability() of not saving
                   Octet #3 in the non-voice case

05/31/02     CD   Corrected initialization of bearer capability in
                  MN_get_bearer_capability_data().
                  Added connection_id to arguments passed in

06/25/02     CD   In MN_get_cause_data() added logging of cause value, and
                  removed byte shifting of the coding standard

07/01/02     CD   In MN_put_invoke_cd_data() fixed loss of precision, loss
                  of sign lint errors

07/19/02    TMR   Re-adding initialization of Phase2+ bearer capability
                  fields in MN_get_bearer_capability_data()

08/02/02     AB   In MN_put_invoke_cd_data(), added check for subaddress 
                  with zero length, and adjusted for CD invoke parameters 
                  Context sepcific tag + len.

09/16/02     AB   In MN_put_bearer_capability(), removed the 
                  FEATURE_GSM_EFR, code exclusion.

11/26/02     AB   Added support for the multimedia, other rate adaption 
                  octet 5a, 5b in MN_get/put_bearer_capability_data().

02/12/03     AB   Removed Lint errors.

03/19/03     AB   Fixed the processing of the gsm_speech_ver_pref_3, which 
                  is used to indicate half rate speech version.

04/10/03     AB   Fixed the processing of octet 5n in 
                      MN_get/put_bearer_capability_data().

05/12/03     AB   Added supports for GSM AMR Full rate and Half Rate
                      in the speech prefer list, 3d, 3e.

02/11/04     AB   Removed the inclusion of 5b on non V120 rate adaption.

07/02/04     AB   Added supports for ITC_AUX_SPEECH.

03/07/05     HS   Added supports for FEATURE_TTY (CTM bit).

04/02/05     AB   Added checking for diagnostic length in MN_get_cause_data() 
                    to prevent cm_cc_cause buffer over-run, which can corrupt 
                    stack space. 

05/17/05     HS   Added support for FEATURE_CCBS

02/28/06     NR   Correcting code for scenario where octet 3 of BC indicates the 
                  value speech and no speech version indication is present in octet 3a

10/23/06     NR   Adding check in MN_compare_bearer_capability() for comparing BCIE related to voice 
===========================================================================*/

/* include header files */

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "environ.h"
#include "memory.h"
#include "timers_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "msg.h"
#include "err.h"
#include "ds_cap_v.h"
#include "bearer_capability_utils.h"
#include "nasutils.h"
#include <stringl/stringl.h>


/* Prototypes */


/* Globals */

/*
 * Function to generate the ASN.1 string to encode an unsigned integer
 * 
 * Parameters: buf_ptr - pointer to buffer where encoded integer is to be 
 *                       placed
 *             data - integer value to encode
 *
 * Output:     length of encoded string, excluding the length field
 *
 * eg, to encode 128, < 02 00 80 > is writted to buf_ptr, and the function
 * returns 2
 *
 */

byte EncodeAsnInt ( byte *buf_ptr, unsigned long int data)
{
 
    int                 len;
    int                 i;
    dword               mask;
 
    /*
     * calculate encoded length of the integer (content)
     */  
 
   mask = (0x7f80UL << (dword)((sizeof(dword) - 2) * 8UL));
 
   for (len = sizeof(dword) ; len > 1 ; --len)
   {
      if ( (data & mask) == 0)
      {  
         mask >>= 8;
      }  
      else
      {   
         break;
      }  
   }   

   /*
    * write the BER integer
    */

   *buf_ptr = (byte) len;

   for (i = 0; i < len; i++)
   {
      *(buf_ptr+len-i) = (unsigned char) (data & 0xff);
      data >>= 8;
   }
 
   return (byte)len;
}


/*
 *   Function name : MN_put_bearer_capability
 *   ---------------
 *
 *   Description:
 *   ------------
 *  Encodes the bearer capability IE for an OTA message 
 *  
 *  
 * 
 * 
 *  
 *
 *  Parameters:
 *  -----------
 *  IN data_pointer
 *     cm_bearer_capability
 *
 *  Returns:
 *     index
 *
 */


int
MN_put_bearer_capability(byte *data_ptr, 
cm_bearer_capability_T *cm_bearer_capability,
byte                   *last_octet_data_ptr)
{
   /* Locals */
   int index = 0;

   /*  OCTET 1  */
   *data_ptr = IEI_BEARER_CAPABILITY;
   data_ptr++;
 
  (void)pack_bearer_capability(data_ptr, 
                                cm_bearer_capability, 
                                &index);   
   /* add IEI length*/
   return (index +  1);
}



/*
 *   Function name : MN_put_cause_data
 *   ---------------
 *
 *   Description:
 *   ------------
 *   Encodes the cause IE for an OTA message 
 *  
 *  
 * 
 * 
 *  
 *
 *   Parameters:
 *   -----------
 *   IN data_pointer
 *     cm_cc_cause
 *
 *   Returns:
 *     index
 *
 */

int MN_put_cause_data(byte *data_ptr, ie_cm_cc_cause_T *cm_cc_cause, byte *last_octet_data_ptr)
{

   /* Locals */

   int     index,
   count;
   byte    max_count;
   byte    *size_ptr;
   byte    octet;

   /*  SET UP THE CAUSE DATA  */

   index = 0;
   max_count = MIN(cm_cc_cause->diagnostic_length, sizeof(cm_cc_cause->diagnostics));

   if((last_octet_data_ptr - data_ptr + 1) < (4 + max_count)) /* Check that there is space for atleast 4 mandatory bytes + diagnostic elements in data_ptr buffer*/
   {
     MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR !! No Space in buffer pointed to by data_ptr. ");
     return index;
   }

   /*  OCTET 1  */

   *data_ptr = IEI_CAUSE;  
   index++;
   data_ptr++;

   /*  OCTET 2  */
   size_ptr = data_ptr;  /* get length */

   index++;
   data_ptr++;

   /*  OCTET 3  */
   if(cm_cc_cause->recommendation_present == TRUE)
   {

      /*  OCTET 3a  */

      /* corrected compilation warning C4244 conversion form int to byte */
      
      *data_ptr = (byte)((cm_cc_cause->recommendation) | 0x80);

      data_ptr++;
      index++;
   }
   else 
   {  
       octet = (byte)((cm_cc_cause->coding_standard) & 0x60); /* valued already shifted */
       octet |= ((cm_cc_cause->location) & 0x0f);
       octet |= 0x80;
       *data_ptr = octet;

       data_ptr++;
       index++;
   }

   /*  OCTET 4 - EXTENSION BIT ALWAYS SET  */
   
   *data_ptr = (byte)((cm_cc_cause->cause_value) | 0x80); /* corrected compilation warning C4244 conversion form int to byte */

   data_ptr++;
   index++;

   /* OCTET - the rest */

   for(count = 0; count < max_count; count++) /* Note that max_count is already sanitized in the beginning of function to ensure no read/write overflow */
   {
      *data_ptr = (cm_cc_cause->diagnostics[count]);

      data_ptr++;
      index++;
   }

   /* Set the Length */
   *size_ptr = (byte)(index - 2); /* adjust iei tag and length field */

   return index;
}


/*
 *   Function name : MN_put_invoke_cd_data
 *   ---------------
 *
 *   Description:
 *   ------------
 *   Encodes the invoke components for call deflection request 
 *     which piggy bag the IE in the OTA DISCONNECT message. 
 *  
 *   Parameters:
 *   -----------
 *   IN data_pointer cm_cc_invoke
 *
 *   Returns:
 *     index
 *
 */

int MN_put_invoke_cd_data(byte *data_ptr, ie_cm_cc_invoke_T *cm_cc_invoke, byte *last_octet_data_ptr)
{
   int     c_index = 0, max_octets_to_write; 
   byte    component[sizeof(facility_invoke_component_data_T)];
   byte    *facility_length, *component_length, *parameters_length;
   boolean success = TRUE; /* Variable to determine whether read buffers overflow or not*/

   max_octets_to_write = last_octet_data_ptr - data_ptr + 1;

   /*  Sanity check the arguments  */   
   if((cm_cc_invoke->defelectedToSubAddress.present) && 
      (cm_cc_invoke->defelectedToSubAddress.length))
   {
       if(cm_cc_invoke->deflectedToNumber.length > sizeof(cm_cc_invoke->deflectedToNumber.data))
       {
         success = FALSE;
         MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Error!!! deflectedToNumber length too long, length = %d, invoke = %d", cm_cc_invoke->deflectedToNumber.length, cm_cc_invoke->invoke_id);
       }
       else if(cm_cc_invoke->defelectedToSubAddress.length > sizeof(cm_cc_invoke->defelectedToSubAddress.data))
       {
         success = FALSE;
         MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Error!!! defelectedToSubAddress length too long, length = %d, invoke = %d", cm_cc_invoke->defelectedToSubAddress.length, cm_cc_invoke->invoke_id);  
       }

   }
   else 
   {
       if(cm_cc_invoke->deflectedToNumber.length > sizeof(cm_cc_invoke->deflectedToNumber.data))
       {
         success = FALSE;
         MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Error!!! deflectedToNumber length too long, length = %d, invoke = %d", cm_cc_invoke->deflectedToNumber.length, cm_cc_invoke->invoke_id); 
       }
   }

   /* SANITY check the arguments */
   
   if(success == TRUE) 
   {  
       /*  FACILITY IE with  SET UP THE INVOKE COMPONENT in */

       /*  OCTET 1  */

       *data_ptr = IEI_FACILITY;
       data_ptr++;

       /*  OCTET 2 , facility length */

       facility_length = data_ptr++;  
 
       /* INVOKE Component */

       component[0] = INVOKE; /* component type */
       component_length = &component[1]; /* component length = component structure + parameters */

       component[2] = INVOKE_ID_TAG;  /* invoke id tag */
       component[3] = 1 ; 
       component[4] = cm_cc_invoke->invoke_id;

       component[5] = OP_CODE_TAG;  /* operation code tag */
       component[6] = 1;
       component[7] = callDeflection;

       component[8] = SEQUENCE_TAG;  /* parameters tag */
       parameters_length = &component[9]; /* sequence length */
  
       /* Required Parameters, Directory number */
       component[10] =  CONTEXT_SPECIFY_PRIMITIVE | 0x00; /* context specific - BCD Number */
       component[11] = cm_cc_invoke->deflectedToNumber.length;
       memscpy(&component[12], 
              sizeof(component) -12,
              cm_cc_invoke->deflectedToNumber.data, 
              cm_cc_invoke->deflectedToNumber.length); /* bcd number */

       c_index += (12 + cm_cc_invoke->deflectedToNumber.length);

       if(c_index >= (int)(sizeof(facility_invoke_component_data_T)-2))/*for the two icrements in the block*/
       {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and bufferover flow");
       }
       /* Optional CD subaddress */
       else if(cm_cc_invoke->defelectedToSubAddress.present == TRUE) 
       {
           component[c_index++] = CONTEXT_SPECIFY_PRIMITIVE | 0x01;  /* octect string + local value subadress; IEI */
           component[c_index++] = cm_cc_invoke->defelectedToSubAddress.length;

           memscpy( &component[c_index], 
                   sizeof(component)- c_index,
                   cm_cc_invoke->defelectedToSubAddress.data,  
                   cm_cc_invoke->defelectedToSubAddress.length);

           c_index += cm_cc_invoke->defelectedToSubAddress.length; 
       } 

       /* Set up the length */
       *parameters_length = (byte)((c_index - 12) + 2); /* 12 = component octet required field octet, 2 = IE tag + IE Len */

       *component_length = (byte)(c_index - 2); /* component tag + length octet */

       *facility_length = *component_length + 2; /* tag + length octet */

       /* Optional SS Version */
       if(c_index >= (int)(sizeof(facility_invoke_component_data_T)-3))/*for the three icrements in the block*/
       {
         MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and bufferover flow");
       }
       else
       {
         component[c_index++] =IEI_SS_VERSION_INDICATOR;
         component[c_index++] = 1; /* length */
         component[c_index++] = SS_VERSION_PHASE1; /* version */
       }

       if(max_octets_to_write >= c_index)
       {
         /* Transfer data to DISC REQ data space */
         memscpy(data_ptr,(last_octet_data_ptr - data_ptr + 1), component, (unsigned int)c_index);
       }
       else
       {
         success = FALSE;
         c_index = 0; /* This is required to let caller know that no byte has been written in data_ptr */
         MSG_ERROR_DS_3(MN_SUB,"=MNCNM= Error!!! Not enough space in write buffer to copy CD arguments, invoke = %d, space_available = %d, space required = %d", cm_cc_invoke->invoke_id, max_octets_to_write, c_index);
       }
   }
   else 
   {
       MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Some length error in CD arguments or not enough space in write buffer, invoke = %d", 0, cm_cc_invoke->invoke_id);
   }

   if(success == TRUE)
   {
     return (c_index + 1); /* + facility tag */
   }
   else
   {
     return 0; /* No byte is copied in data_ptr */
   }
}


/*
 *   Function name : MN_put_llc_data
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function encodes the low layer compatibility IE 
 *   into an OTA message (MN -> CM)
 *  
 *  
 *
 *  Parameters:
 *  -----------
 *  IN cm_low_layer_compatibility
 *     output message pointer
 *
 *  Returns:
 *      index  ( = no of bytes of data generated)
 *
 */

int
MN_put_llc_data(byte *data_ptr, 
cm_low_layer_compatibility_T *cm_low_layer_compatibility,
byte                         *last_octet_data_ptr
)
{
   int index;
   byte *size_ptr;
   int counter;
   byte max_bytes_to_copy = MIN(cm_low_layer_compatibility->length, sizeof(cm_low_layer_compatibility->data)) + 2;
   index = 0;

   if(data_ptr + max_bytes_to_copy -1 <= last_octet_data_ptr)
   {

      *data_ptr = IEI_LOW_LAYER_COMPATIB;
   
      data_ptr++;
      index++;
   
      /*  OCTET 2  */
   
      size_ptr = data_ptr;
   
      data_ptr++;
      index++;
   
      /*  OCTET 3  */
       if(cm_low_layer_compatibility->length > sizeof(cm_low_layer_compatibility->data))
      {
         cm_low_layer_compatibility->length =  sizeof(cm_low_layer_compatibility->data);
         MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Cause IE: invalid cm_low_layer_compatibility length = %d , truncating ", cm_low_layer_compatibility->length);
      }
   
      for(counter = 0; counter < cm_low_layer_compatibility->length; counter++)
   
      {
         *data_ptr = cm_low_layer_compatibility->data[counter];
         data_ptr++;
         index++;
      }
   
      /* corrected compile warning C4244 conversion from int to byte */
      
      *size_ptr = (byte)(index - 2);
   }
   else
   {
      MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Function: MN_put_llc_data() - Not enough space in write buffer, Space = %d, # of bytes to copy = %d ",
                                            (last_octet_data_ptr - data_ptr + 1),max_bytes_to_copy);
   }

   return index;

}



/*
 *   Function name : MN_put_hlc_data
 *   ---------------
 *
 *   Description:
 *   ------------
 *  This function encodes the high layer compatibility IE 
 *   into an OTA message (MN -> CM)
 *  
 *  
 *
 *  Parameters:
 *  -----------
 *  IN cm_high_layer_compatibility
 *     output message pointer
 *
 *  Returns:
 *      index  ( = no of bytes of data generated)
 *
 *
 */

int
MN_put_hlc_data(byte *data_ptr, 
cm_high_layer_compatibility_T *cm_high_layer_compatibility,
byte                          *last_octet_data_ptr)
{

   /* Locals */

   int index;
   byte octet;
   byte *size_ptr;
   byte max_bytes_to_copy = 5;/* maximum bytes copied in this function in data_ptr buffer*/

   index = 0;

   if(data_ptr + max_bytes_to_copy -1 <= last_octet_data_ptr) /* ensure that write buffer has ennough space to copy max_bytes_to_copy # of bytes*/
   {
      *data_ptr = IEI_HIGH_LAYER_COMPATIB;
      index++;
      data_ptr++;
   
      /*  OCTET 2  */
   
      size_ptr = data_ptr;
      index++;
      data_ptr++;
   
      if(cm_high_layer_compatibility->not_applicable == FALSE)
      {
         /*  OCTET 3  */
   
         /* corrected compile warning C4244 conversion from int to byte */
   
         octet = (byte)((cm_high_layer_compatibility->coding_standard & 0x03) << 5);
   
         octet |= (cm_high_layer_compatibility->interpretation & 0x07) << 2;
         octet |= (cm_high_layer_compatibility->presentation_method & 0x03);
         octet |= 0x80;
   
         *data_ptr = octet;
   
         index++;
         data_ptr++;
   
         /*  OCTET 4  */
   
         /* corrected compilation warning C4244 conversion from int to byte */
         octet = (byte)(cm_high_layer_compatibility->high_layer_characteristic_id & 0x7f);
   
         if(cm_high_layer_compatibility->ehlcid_present != TRUE)
         {
            octet |= 0x80;
         }
   
         *data_ptr = octet;
         index++;
         data_ptr++;
   
         if(cm_high_layer_compatibility->ehlcid_present == TRUE)
         {
            /*  OCTET 4a  */
            /* corrected compile warning C4244 conversion from int to byte */
             *data_ptr = (byte)(cm_high_layer_compatibility->
                extended_high_layer_characteristic_id | 0x80);
            index++;
            data_ptr++;
         }
      }
      /* corrected compile warning C4244 conversion from int to byte */
      *size_ptr = (byte)(index - 2);
   }
   else
   {
      MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Function: MN_put_hlc_data() - Not enough space in write buffer, Space = %d, # of bytes to copy = %d ",
                                            (last_octet_data_ptr - data_ptr + 1),max_bytes_to_copy);
   }
   return index;

}

/*
 *   Function name : MN_compare_bearer_capability
 *   ---------------
 *
 *   Description: Compares the two bearer_capabilities entered
 *   ------------ in to the function and returns a success value
 *   
 *  
 *
 *  Parameters:
 *  -----------
 *  IN bearer_capability_1
 *     bearer_capability_2
 *
 *  Returns: equates
 *  
 *
 */

boolean  MN_compare_bearer_capability(
cm_bearer_capability_T  *bearer_capability_1,
cm_bearer_capability_T  *bearer_capability_2, int direction
)

{

   /* Locals */

   boolean   equates;
   boolean additional_check = FALSE;

   /*
   ** Compare all the elements of the two bearer capabilities,
   ** excepting radio channel requirement.
   ** Returns TRUE only if all elements are equal
   */

   if ((bearer_capability_1->information_transfer_capability == 
       bearer_capability_2->information_transfer_capability) &&
       ((bearer_capability_1->information_transfer_capability == ITC_SPEECH) ||
       (bearer_capability_1->information_transfer_capability == ITC_AUX_SPEECH)))
       {
          equates = TRUE;
       }
   else if ((bearer_capability_1->information_transfer_capability == 
       bearer_capability_2->information_transfer_capability) &&
       (bearer_capability_1->structure == bearer_capability_2->structure) &&
       (bearer_capability_1->duplex_mode == bearer_capability_2->duplex_mode) &&
       (bearer_capability_1->rate_adaption == 
       bearer_capability_2->rate_adaption) &&
       (bearer_capability_1->signalling_access_protocol == 
       bearer_capability_2->signalling_access_protocol) &&
       (bearer_capability_1->sync_async == bearer_capability_2->sync_async) &&
       (bearer_capability_1->number_of_data_bits == 
       bearer_capability_2->number_of_data_bits) &&
       (bearer_capability_1->number_of_stop_bits == 
       bearer_capability_2->number_of_stop_bits) &&
       (bearer_capability_1->user_rate == bearer_capability_2->user_rate) &&
       (bearer_capability_1->parity == bearer_capability_2->parity) &&
       (bearer_capability_1->modem_type == bearer_capability_2->modem_type) &&
       (bearer_capability_1->user_information_layer_2_protocol == 
       bearer_capability_2->user_information_layer_2_protocol) &&

       /* These values added for Phase2+ */
       (bearer_capability_1->other_modem_type == 
       bearer_capability_2->other_modem_type) &&
       (bearer_capability_1->fixed_network_user_rate == 
       bearer_capability_2->fixed_network_user_rate) &&
       ((bearer_capability_1->user_initiated_modification_indication == 
       bearer_capability_2->user_initiated_modification_indication)||
        ((bearer_capability_1->user_initiated_modification_indication >= 4)&&(bearer_capability_2->user_initiated_modification_indication >= 4))))
   {

      equates = TRUE;
      additional_check = TRUE;

   }
   else
   {

      equates = FALSE;

   }
   if ((equates == TRUE) && (additional_check == TRUE))
   {
      if (direction == MO)
      {
         if ((bearer_capability_1->connection_element == 
              bearer_capability_2->connection_element) &&
              (bearer_capability_1->acceptable_channel_codings == 
              bearer_capability_2->acceptable_channel_codings) &&
              (bearer_capability_1->maximum_number_of_trafic_channels == 
              bearer_capability_2->maximum_number_of_trafic_channels) &&
              (bearer_capability_1->wanted_air_interface_user_rate == 
              bearer_capability_2->wanted_air_interface_user_rate))
         {
            equates = TRUE;
         }
         else
         {
            equates = FALSE;
         }
      }      
   }   

   return(equates);

}

/*
 * Function name :  MN_get_bearer_capability_data
 * -------------
 *
 * Description :  Gets the data from the message array and puts
 * -----------    it in relevant structure
 *
 *
 *
 *
 *
 *
 * Parameters: *data_ptr - to message_data
 * ----------  *cm_bearer_capability
 *             connection_id
 *
 *
 *
 * Returns: message_data array index (int)
 * --------
 *
 */
int
MN_get_bearer_capability_data(byte *data_ptr, 
cm_bearer_capability_T *cm_bearer_capability, connection_id_T conn_id, byte *last_octet_data_ptr)
{
   int index = 0;
   /* Initialise the output data array to the values stored in global data*/
   if(mn_call_information_p[conn_id] != NULL)
   {
     if (mn_call_information_p[conn_id]->active_bearer_capability == 0)
     {
       *cm_bearer_capability = mn_call_information_p[conn_id]->bearer_capability_1;
     }
     if (mn_call_information_p[conn_id]->active_bearer_capability == 1)
     {
       *cm_bearer_capability = mn_call_information_p[conn_id]->bearer_capability_2;
     }
   }
   else
   {
     MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot init cm_bearer_capability as mn_call_information_p[%d] is NULL",
                                 conn_id);
   }

   /* TMR - Initialize the Phase2+ elements */
   cm_bearer_capability->fixed_network_user_rate = FNUR_UNDEFINED;

   cm_bearer_capability->other_modem_type = OMT_UNDEFINED;

   cm_bearer_capability->acceptable_channel_codings = ACC_NONE;

   cm_bearer_capability->maximum_number_of_trafic_channels = 
                                                MAX_NUMBER_OF_TCH_UNDEFINED;

   cm_bearer_capability->user_initiated_modification_indication = 
                                                UIMI_UNDEFINED;

   cm_bearer_capability->wanted_air_interface_user_rate = WAIUR_UNDEFINED;

   if
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
   (
#endif
      (*data_ptr == IEI_BEARER_CAPABILITY) 
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
     || (*data_ptr == IEI_BACKUP_BEARER_CAPABILITY)
    )
#endif
   {
     data_ptr++;
     cm_bearer_capability->present = TRUE;

     (void)unpack_bearer_capability(data_ptr, 
                                  cm_bearer_capability, 
                                  &index,
                                  last_octet_data_ptr);
   }
   else
   {
     MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid IEI=%d\n",*data_ptr);
   }
   /* add IEI length */
   return (index + 1);
}


/****************************************************************/
/*
 * Function name :  MN_get_cause_data
 * -------------
 *
 * Description :  Gets the cause data from the message data
 * -----------    array and puts it in to the relevant structure
 *
 *
 *
 *
 *
 *
 * Parameters: *data_ptr - pointer to message data array
 * ----------  *cm_cc_cause
 *
 *
 *
 * Returns: message_array index (int) 
 * --------
 *
 */
int MN_get_cause_data(byte *data_ptr, ie_cm_cc_cause_T *cm_cc_cause, byte *last_octet_data_ptr)
{
   word index = 0, ie_length = 0, diag_length=0;
   byte *end_of_buffer;

   /* Reset the cm_cc_cause data structure */
   memset((VOID_DATA *)cm_cc_cause, NULL, sizeof(ie_cm_cc_cause_T));

   /*  OCTET 1  */
   if((data_ptr < last_octet_data_ptr) && (*data_ptr == IEI_CAUSE))   /* TAG */
   {
      data_ptr++;    
      index++;
      cm_cc_cause->present = TRUE;

      /*  OCTET 2  */
      ie_length = *data_ptr;    /* CAUSE LEN */
      data_ptr++;
      index++;
      ie_length = MIN(ie_length, GSM_CAUSE_SIZE -2); /*length of IEI excluding tag and length*/
      end_of_buffer = MIN(last_octet_data_ptr, (data_ptr + ie_length -1));

      if((data_ptr < end_of_buffer) && ((index - 2) < ie_length))   /* TAG & LEN */
      {
         /*  OCTET 3  */
         cm_cc_cause->coding_standard = (byte)(*data_ptr & 0x60);
         cm_cc_cause->location = (byte)(*data_ptr & 0x0f);

         /* if bit 8 is set,  octet 3a must be present*/
         if((*data_ptr & 0x80) == 0)
         {
            /*  OCTET 3a  */
            cm_cc_cause->recommendation_present = TRUE;
            index++;
            data_ptr++;

            if ((*data_ptr & 0x80 ) > 0)
            {
               cm_cc_cause->recommendation = (byte)(*data_ptr & 0x7f);
            }
            else
            {
               ERR("ERROR: Unexpected value for MSB in octet 3a of cause data",0,0,0);
            }
            index++;
            data_ptr++; 
         }
         else
         {
            cm_cc_cause->recommendation_present = FALSE;
            data_ptr++;
            index++;
         }

         /*  OCTET 4  */
         if ((data_ptr <= end_of_buffer) && ((*data_ptr & 0x80) >0))
         {
            cm_cc_cause->cause_value = (byte)(*data_ptr & 0x7f);

            MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cause IE, cause_value %d",cm_cc_cause->cause_value);
         }
         else
         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= ERROR: Unexpected value for MSB in octet 4 of cause data"); 
         }
         index++;
         data_ptr++;

         /*  OCTET 5 ONWARDS  */
         diag_length = (ie_length + 2) - index;  /* IE TAG + LEN */
       
         if(diag_length > 0)
         { 
             if(diag_length <= MAX_DIAGNOSTICS_LENGTH)
             {
                 cm_cc_cause->diagnostic_length = (byte)diag_length;
             }
             else
             {
                 cm_cc_cause->diagnostic_length = MAX_DIAGNOSTICS_LENGTH;

                 MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Cause IE: invalid diagnostic length = %d", diag_length);
             }
         }
         else
         {
             cm_cc_cause->diagnostic_length = 0;
         }

         if((cm_cc_cause->diagnostic_length > 0) && ((data_ptr + cm_cc_cause->diagnostic_length - 1) <= end_of_buffer))
         {
             memscpy(cm_cc_cause->diagnostics, sizeof(cm_cc_cause->diagnostics) ,data_ptr, cm_cc_cause->diagnostic_length);
         }
         else if(((data_ptr + cm_cc_cause->diagnostic_length - 1) > end_of_buffer))
         {
           MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Error!!! 3 Not enough space in read buffer or diagnostic length is wrong; diagnostic_length = %d",cm_cc_cause->diagnostic_length);
         }

      }
      else if(data_ptr >= end_of_buffer)
      {
        MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Error!!! 2 Not enough space in read buffer or ie_length is wrong; ie_length = %d",ie_length);
      }

   }
   else if(data_ptr >= last_octet_data_ptr)
   {
     MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Error!!! 1 Not enough space in read buffer or Cause IEI not present; IEI = %d",*data_ptr);
   }

   return index + cm_cc_cause->diagnostic_length; 
}


/*
 * Function name :  MN_get_llc_data
 * -------------
 *
 * Description :  Gets low layer compatibility from the message
 * -----------    data array and puts it in relevant structure
 *
 *
 * Parameters: data_ptr - ptr to message data
 * ----------  *low_layer_compatibility
 *
 *
 *
 * Returns:  message_data array index
 * --------
 *
 */
int MN_get_llc_data( byte *data_ptr, 
                     cm_low_layer_compatibility_T *cm_low_layer_compatibility,
                     byte *last_octet_data_ptr)
{
   int index;
   int ie_length;
   byte max_ie_length;

   /* initialise the cm_low_layer_compatibility data area */

   memset((VOID_DATA *)cm_low_layer_compatibility, 0, sizeof(cm_low_layer_compatibility_T));

   /*NB - data_pointer should be a pointer of type byte */

   cm_low_layer_compatibility->present = FALSE;


   /* OCTET 1 */

   index = 0;

   if((data_ptr < last_octet_data_ptr) && (*data_ptr == IEI_LOW_LAYER_COMPATIB)) /*We read atleast two bytes from data_ptr buffer if we go in this block*/
   {
      data_ptr++;
      index++;

      cm_low_layer_compatibility->present = TRUE;
      /* OCTET 2 */
      ie_length = *data_ptr;
      cm_low_layer_compatibility->length = (byte)ie_length;
      data_ptr++;
      index++;

      /* Check that octet count equal to ie_length is available in data_ptr buffer at this point to ensure no read overflow*/

      max_ie_length = MIN((last_octet_data_ptr - data_ptr + 1), sizeof(cm_low_layer_compatibility->data));
      if(ie_length > max_ie_length)
        {
          MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Error!!! Incorrect IE Length leading to read or write buffer overflow, ie_length = %d, Truncated ie_length = ",ie_length,max_ie_length);
         ie_length = max_ie_length;
        }

      /* OCTET 3 ON */

      while((index - 2) < ie_length)
      {
         cm_low_layer_compatibility->data[index - 2] = *data_ptr;
         data_ptr++;
         index++;
      }
   }

   return index;

}


/*
 * Function name :  MN_get_hlc_data
 * -------------
 *
 * Description :  Gets hlc data from message data array and puts it
 * -----------    in the relevant structure
 *
 *
 * Parameters: data_ptr - ptr to message data array
 * ----------  *cm_high_layer_compatibility
 *
 *
 * Returns: message data array index
 * --------
 *
 */

int MN_get_hlc_data(
         byte *data_ptr, 
         cm_high_layer_compatibility_T *cm_high_layer_compatibility,
         byte *last_octet_data_ptr)
{
   int index;
   int ie_length;

   /* initialise the cm_high_layer_compatibility data area */

   memset((VOID_DATA *)cm_high_layer_compatibility, 0, sizeof(cm_high_layer_compatibility_T));

   /*NB - data_pointer should be a pointer of type byte */

   cm_high_layer_compatibility->present = FALSE;


   /* OCTET 1 */

   index = 0;

   if((data_ptr < last_octet_data_ptr) && (*data_ptr == IEI_HIGH_LAYER_COMPATIB))
   {
      data_ptr++;
      index++;

      cm_high_layer_compatibility->present = TRUE;

      /* OCTET 2 */

      ie_length = *data_ptr;
      data_ptr++;
      index++;

      if(ie_length == 0)
      {
         /* THERE IS NO MORE DATA */
         cm_high_layer_compatibility->not_applicable = TRUE;
      }
      else if(ie_length > (last_octet_data_ptr - data_ptr + 1))
      {
        MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Error!!! Incorrect ie_length or Buffer overflow; ie_length = %d and Space left to read = %d",ie_length,(last_octet_data_ptr - data_ptr + 1));
      }
      else 
      {

         /* OCTET 3 */
         /* fixed compile warning C4244 */
         cm_high_layer_compatibility->not_applicable = FALSE;
         cm_high_layer_compatibility->coding_standard  = 
                                       (byte)((*data_ptr & 0x60) >> 5);
         cm_high_layer_compatibility->interpretation = 
                                       (byte)((*data_ptr & 0x1c) >> 2);
         cm_high_layer_compatibility->presentation_method = 
                                       (byte)(*data_ptr & 0x03);

         data_ptr++;
         index++;

         if((index-2) < ie_length)
         {

            /* THERE IS FURTHER DATA */

            /* OCTET 4 */
            /* fixed compile warning C4244 */
            cm_high_layer_compatibility->high_layer_characteristic_id = 
                                       (byte)(*data_ptr & 0x7f);

            data_ptr++;
            index++;

            if(((index-2) < ie_length) && ((*(data_ptr-1) & 0x80) == 0))
            {
               /* OCTET 4a */

                /* fixed compile warning C4244 */
               cm_high_layer_compatibility->
                     extended_high_layer_characteristic_id = (byte)(*data_ptr & 0x7f);

               cm_high_layer_compatibility->ehlcid_present = TRUE;

               data_ptr++;
               index++;

            }
            else
            {
               cm_high_layer_compatibility->
                     extended_high_layer_characteristic_id = 0;

               cm_high_layer_compatibility->ehlcid_present = FALSE;
            }
         }
         else
         {
            cm_high_layer_compatibility->high_layer_characteristic_id = 0;
         }
      }
   }

   return index;

}

#ifdef FEATURE_CCBS
/*
 * Function name :  MN_fill_setup_from_mn
 * -------------
 *
 * Description :
 * -----------     
 *  Fill setup message contents with information in mn_call_information[connection_id]
 *  Doesn't fill up the header
 *
 * Parameters:
 * ----------  
 *  cm_mo_normal_call_req_T *msg           : empty setup message
 *  byte                     connection_id : connection id 
 *
 * Returns:
 * --------
 *  void
 *
 */
void MN_fill_setup_from_mn (cm_mo_normal_call_req_T *msg, byte connection_id)
{
   mn_call_information_T  *mn_info = mn_call_information_p[connection_id];

   /* fill the contents */
   msg->connection_id = connection_id;
   if(mn_info != NULL)
   {

     /* BCIE */
     if (mn_info->repeat_indicator_bc == 0) /* no BC */
     {
        /* msg->bc_repeat_indicator.present = FALSE; */
     }
     else if (mn_info->repeat_indicator_bc == 1) /* 1 BC */
     {
        /* msg->bc_repeat_indicator.present = FALSE; */
        msg->bearer_capability_1 = mn_info->bearer_capability_1;
     }
     else if (mn_info->repeat_indicator_bc >= 2) /* 2 BC */
     {
        msg->bc_repeat_indicator.present = TRUE;
        msg->bc_repeat_indicator.repeat_indication = 1;
        msg->bearer_capability_1 = mn_info->bearer_capability_1;
        msg->bearer_capability_2 = mn_info->bearer_capability_2;
     }

     /* LLC */
     if (mn_info->repeat_indication_llc == 0) /* no BC */
     {
     }
     else if (mn_info->repeat_indication_llc == 1) /* 1 BC */
     {
        msg->low_layer_compatibility_1 = mn_info->low_layer_compatibility_1;
     }
     else if (mn_info->repeat_indication_llc >= 2) /* 2 BC */
     {
        msg->llc_repeat_indicator.present = TRUE;
        msg->llc_repeat_indicator.repeat_indication = 1;
        msg->low_layer_compatibility_1 = mn_info->low_layer_compatibility_1;
        msg->low_layer_compatibility_2 = mn_info->low_layer_compatibility_2;
     }

     /* HLC */
     if (mn_info->repeat_indication_hlc == 0) /* no BC */
     {
     }
     else if (mn_info->repeat_indication_hlc == 1) /* 1 BC */
     {
        msg->high_layer_compatibility_1 = mn_info->high_layer_compatibility_1;
     }
     else if (mn_info->repeat_indication_hlc >= 2) /* 2 BC */
     {
        msg->hlc_repeat_indicator.present = TRUE;
        msg->hlc_repeat_indicator.repeat_indication = 1;
        msg->high_layer_compatibility_1 = mn_info->high_layer_compatibility_1;
        msg->high_layer_compatibility_2 = mn_info->high_layer_compatibility_2;
     }

     /* called party */
     if (mn_info->called_party_bcd_number_present)
     {
        msg->cm_called_party_bcd_number = mn_info->called_party_bcd_number;
        msg->cm_called_party_bcd_number.present = TRUE;
     }
     if (mn_info->called_party_subaddress_present)
     {
        msg->cm_called_party_subaddress = mn_info->called_party_subaddress;
        msg->cm_called_party_subaddress.present = TRUE;
     }

     /* calling party */
     if (mn_info->calling_party_subaddress_present)
     {
        msg->cm_calling_party_subaddress = mn_info->calling_party_subaddress; 
        msg->cm_calling_party_subaddress.present = TRUE;
     }

     /* CUG */
     msg->forward_cug_info = mn_info->cm_forward_cug_info;

     /* as_id */
#if defined(FEATURE_DUAL_SIM)
     if(MNCNM_IS_SINGLE_STACK)
     {
       msg->as_id = mn_info->as_id;
     }
#if defined(FEATURE_DUAL_ACTIVE)
     else
     {
       msg->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
     }
#endif
#endif


     /* CLIR (skip this for now) */
   }
   else
   {
     MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Cannot fill msg as mn_info is NULL");
   }

}
#endif /* FEATURE_CCBS */

