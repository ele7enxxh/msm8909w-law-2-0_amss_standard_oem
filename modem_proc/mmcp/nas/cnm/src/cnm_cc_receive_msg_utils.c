/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001, 2002 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cnm_cc_receive_msg_utils.c_v   1.10   22 Apr 2002 10:12:08   cdealy  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/src/cnm_cc_receive_msg_utils.c#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
20-Jun-2001  VT    Removed unwanted reference(s) to m20000_debug_string

08/06/01   CD      Clean-up of include header files 

08/14/01   AB      Renamed connection manager references to CNM from CM.

13-Sep-2001  VT    CS Data: In the function, CNM_]future_extensions,
                     For the Bearer Capability IE, for octet 6, changed the 
                     number of expected_octets from 4 to 6 as octets 6d & 6e
                     are supported now.

09/14/01   CD      Modifed call to MSG_LOW to avoid using %s on target

11-Mar-2002   VT   Removed Lint error.

04/19/02   CD      Modified argument in CNM_check_for_protocol_errors()
                   Updated copyright
                   
09/04/02   AB      In CNM_check_future_extensions(), added checking
                   for IEI_REDIR_PARTY_BCD_NO, and IEI_REDIR_PARTY_SUBADDR.
                     
12/05/02   AB      In CNM_check_future_extensions(), expanded the expected
                   octet octet5a, b, and 6g to support Multimedia 
                   extension in BCIE.

02/14/03   AB      Removed lint errors.

05/25/05   HS      Added fix_facility_ie_len() to revise possible X.209 encoding
                   in facility IE length field

04/04/06   NR      Correcting the number of octets for BCIE in CNM_check_future_extensions()

09/21/11   PM      Fix for CR 262599.
09/12/11   HC     Replacing the #include "ms_timer.h" with #include "ms_timer_v.h" 
                  as a part of CMI activity
===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"
#include "rex.h"
#include "err.h"

#include "environ.h"
#include "com_iei_v.h"
#include "mn.h"
#include "mnglobal.h"
#include "cc_cc.h"
#include "timers_v.h"
#include "ms_cc_mm_v.h"
#include "ms_timer_v.h"
#include "cause.h"

#include "nasutils.h"
#include "cmlib.h"
#include "cnm.h"
#include "ms.h"

/*
 * local type definition
 */

typedef struct stored_allowed_ie_tag
{
   byte     allowed_IEI;
   byte     type;
   byte     IE_length;
   byte     presence;
   byte     format;
   boolean  found;

} stored_allowed_ie_T;

/*
 * Global data reference
 */

extern   struct MessageIETag  *next_ref;

/*
 * definition of data storage for IE reference storage. This is
 * set up in the function, then used in all the message processing
 * functions. Entry 0 will contain the PD/TI field, and entry 1 will
 * contain the message type field. At the end of this module,
 * no_of_IEs will contain the number of information elements in the
 * received message
 */

#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)

byte        no_of_IEs_sim[MAX_AS_IDS];        /* set to zero initially in this function */
IE_info_T   cc_IE_info_sim[MAX_AS_IDS][MAXNO_IES_IN_MESSAGE];
#define no_of_IEs (no_of_IEs_sim[mn_dsda_as_id])
#define cc_IE_info (cc_IE_info_sim[mn_dsda_as_id])
#else

byte        no_of_IEs;        /* set to zero initially in this function */
IE_info_T   cc_IE_info[MAXNO_IES_IN_MESSAGE];

#endif


static VOID_FUNC   CNM_check_future_extensions( byte , byte *, byte *, byte *);

/**************** Begin of   CNM_get_IE_length ************************************
 *
 *  Function name: CNM_get_IE_length()
 *  ---------------------------------
 *
 *  Description:
 *  ------------
 *  Function to get length of the IE after verifying the length received in
 *  the message buffer.
 *
 *  Parameters:
 *  -----------
 *  length  - IN : length of the IE received in the message buffer
 *
 *  Returns:
 *  --------
 *  byte - length of the IE.
 *
 *  Note:
 *  --------
 *  Applicable only to IE of TLV format. 
 *  This returns length of IE including the bytes for T,L.
 *
 ******************************************************************************/
byte CNM_get_IE_length( byte length )
{
  if((length + 2) > MAX_OCTETS_L3_MESSAGE)
  {
    /*
    ** Since we received illegal length, return value greater than 
    ** max layer3 message size so as to ignore checking rest of 
    ** the message.
    */
    MSG_ERROR_DS_1(MN_SUB,"=MNCNM= CNM: Illegal L3 IE length(%d) ", (byte)(length));
    return ((unsigned char)(MAX_OCTETS_L3_MESSAGE+1));
  }
  else
  {
    return ((unsigned char)(length + 2));
  }
}

/**************** Begin of   CNM_store_IE_info ************************************
 *
 *  Function name: CNM_store_IE_info()
 *  ---------------------------------
 *
 *  Description:
 *  ------------
 *  Function to store the start address of the IE, the id, and the length
 *  in a global data array for use by other functions and resets syntax_ok
 *  parameter for this IE to 1. This parameter will be set after checking of syntax
 *  in function CNM_perform_check_on_IEs()
 *
 *  Parameters:
 *  -----------
 *  start  - IN : address of start of an IE
 *  IEI    - IN : identification of the IE
 *  length - IN : length of the IE;
 *  presence - 
 *  last_octet_data_pointer - pointer to last octet in that buffer where start is pointing to.
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

VOID_FUNC CNM_store_IE_info( byte *start,
                            byte IEI,
                            byte  length,
                            byte  presence,
                            byte *last_octet_data_pointer)
{

   if ( no_of_IEs < MAXNO_IES_IN_MESSAGE )
   {

      if ( no_of_IEs >= 2 )
      {
#ifdef FEATURE_CNM_UNIT_TEST
         MSG_LOW_DS_1(MN_SUB,"=MNCNM= IE> %s\n", DEBUG_identify_IE(IEI));
#else
         MSG_LOW_DS_1(MN_SUB,"=MNCNM= IE> %#X\n",IEI);
#endif
      }

      cc_IE_info[no_of_IEs].start = start;
      cc_IE_info[no_of_IEs].IEI = IEI;
      cc_IE_info[no_of_IEs].length = length;
      cc_IE_info[no_of_IEs].presence = presence;
      cc_IE_info[no_of_IEs].syntax_ok = 1;  
      /* 
       * it is reset to one here. Will be set in cnm_perform_check_on_IEs() 
       * the default value used is 1.
       */


      /*
       * only process real IEs, entries 0 and 1 are not real so ignore
       */

      if ( no_of_IEs >= 2 )
      {
         CNM_check_future_extensions( IEI,
                                     start,
                                     &(cc_IE_info[no_of_IEs].length),
                                     last_octet_data_pointer);
      }

      no_of_IEs++;
   }
   else
   {
      ERR("Too many IEs in message - ignored\n", 0, 0, 0);
   }
}
/**************** End of   CNM_store_IE_info **************************************/

/**************** Begin of   fix_facility_ie_len ************************
 *
 *  Function name: fix_facility_ie_len
 *  ---------------------------------------------
 *
 *  Description:
 *  ------------
 *  Re-calculate facility IE length just in case it is in X.209 fashion.
 *  This function is static and only used in CNM_check_for_protocol_errors().
 *
 *  Parameters:
 *  -----------
 *  pLen: pointer to the length field of facility IE
 *  last_octet_data_pointer : pointer to the last octet which can be read to calculate facility length.
 *
 *  Returns:
 *  --------
 *  void
 *
 ******************************************************************************/

static void fix_facility_ie_len (byte *pLen, byte *last_octet_data_pointer)
{
   return_size_T  size;
   byte           fac_size=0, comp_size;
   byte          *pComponent=(pLen+1); // point to the first component

   while ( (pComponent <= last_octet_data_pointer) && (INVOKE<=pComponent[0] && pComponent[0]<=REJECT))
   {
      size = MN_get_element_length(pComponent+1, SS_MAX_FACILITY_LENGTH, (last_octet_data_pointer - (pComponent + 1) + 1)); /* max size of buffer allowed to be read*/

      /* Check to ensure that char overflow doesn't happen while assigning value to comp_size */
      if((size.increment + size.length) >= MAX_UNSIGNED_CHAR_LIMIT)
      {
        MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Error!! Wrong msg input for fix_facility_ie_len, inc = %d, len = %d",size.increment,size.length);
        comp_size = MAX_UNSIGNED_CHAR_LIMIT; /* maximum a char can hold - 255 */
      }
      else
      {
        comp_size = (byte)(1/*component id*/ + size.increment + size.length);
      }

      if (pComponent[1] == 0x80) // indefinite component length
      {
         if(comp_size < (MAX_UNSIGNED_CHAR_LIMIT -1))
         {
           comp_size += 2;  // for 0x00 0x00
         }
         else
         {
           comp_size = MAX_UNSIGNED_CHAR_LIMIT;
         }
         
      }

      pComponent += comp_size; // point to next component

      if(fac_size > (MAX_UNSIGNED_CHAR_LIMIT - comp_size))
      {
        MSG_ERROR_DS_2(MN_SUB,"=MNCNM= Error!! fac_size = %d, comp_size = %d",fac_size,comp_size);
        fac_size   = MAX_UNSIGNED_CHAR_LIMIT;
      }
      else
      {
        fac_size   += comp_size; // facility IE length
      }
   }

/********************************************************************************** 
* if the re-calculated length of the Facility IE is > 220, truncate it to 220. Facility IE is a Type 4 IE
* whose theoritical max value 220 is derived to from Spec TS 24.080 , X.209, TS 24.008.
   *********************************************************************************/
   if( fac_size > (sizeof(ie_facility_T) - 2))
   {
     fac_size = (sizeof(ie_facility_T) - 2);
   }

   MSG_HIGH_DS_2(MN_SUB,"=MNCNM= Re-calculated facility IE len:%d->%d", *pLen, fac_size);
   *pLen = fac_size;
}

/**************** Begin of   CNM_check_for_protocol_errors ************************
 *
 *  Function name: CNM_check_for_protocol_errors()
 *  ---------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to check the IEs in a message, removing IEs which are coded using
 *  non-GSM code-sets. If there is an IE which is not valid for
 *  the message, then a release complete is generated and sent
 *  to the network. Special processing applies for certain errors and certain
 *  messages. No check is made of the IE syntax. It is left up to the individual
 *  message processors to do this.
 *
 *  Parameters:
 *  -----------
 *  message : IN
 *
 *  Returns:
 *  --------
 *  boolean - TRUE if message content is OK
 *
 ******************************************************************************/

boolean CNM_check_for_protocol_errors( mmcnm_data_ind_T * message )
{

   boolean                 return_value = TRUE;
   byte                    *p_message_read;
   byte                    actual_IE_length;
   int                     length;
   connection_id_T         connection_id;
   byte                    message_type;
   byte                    protocol_descriminator;
   boolean                 valid;
   release_msg_T           *p_release;
   boolean                 valid_IE = FALSE;
   boolean                 allowed_IE = FALSE;
   byte                    message_IEI = 0xff;
   stored_allowed_ie_T     stored_allowed_ie[MAXNO_IES_IN_MESSAGE];
   int                     ie_counter;
   int                     allowed_ie_pointer;
   int                     this_ie = 0;
   byte                    no_of_bytes;
   byte                   *last_octet_data_pointer; /* last octet of the buffer where p_message_read points */


   no_of_IEs = 0;       /* initialise IE counter global */

   GET_IMH_LEN(length, &message->msg_head.IMH); /* message header */
#ifdef FEATURE_DUAL_SIM
   length = length - sizeof(byte);
#endif

   connection_id = CNM_get_connection_id(message );

   protocol_descriminator = (message->msg_head.ti_pd & 0x0f);

   message_type =  message->msg_head.type;

   p_message_read  = (byte *)message->data;

   last_octet_data_pointer = MIN( (p_message_read + sizeof(message->data) - 1) , ((byte *)message + sizeof(IMH_T) + length - 1) );


   p_release = (release_msg_T*) CNM_alloc( sizeof(release_msg_T));

   if ((protocol_descriminator == PD_CC) ||   /* check protocol desc. for CC */
       (protocol_descriminator == PD_SS))     /* check protocol desc. for SS */
   {

      /*
       * store the location of the PD/TI IE
       */

      CNM_store_IE_info( p_message_read, 0, 1, MANDATORY_IE, last_octet_data_pointer);

      /*
       * move pointers back to message type
       */
      p_message_read--;

      message_type = ((*(byte *)p_message_read) & 0x3f);

      /*
       * store the message type
       */

      CNM_store_IE_info(p_message_read, message_type, 1, MANDATORY_IE, last_octet_data_pointer);

      p_message_read++;

      /*
       * Now pointers are pointing at first IE
       *
       * nextref obtained in CNM_process_message_type function
       */

      valid = TRUE;
      ie_counter = 0;

      /*
       * load stored_allowed_ie array with all the allowed information
       * elements for the received message
       */

      while (valid != FALSE)
      {
         valid = CNM_get_next_IE_allowed(
                                & stored_allowed_ie[ie_counter].allowed_IEI,
                                & stored_allowed_ie[ie_counter].type,
                                & stored_allowed_ie[ie_counter].IE_length,
                                & stored_allowed_ie[ie_counter].presence,
                                & stored_allowed_ie[ie_counter].format,
                                & next_ref);

         stored_allowed_ie[ie_counter].found = FALSE;

         if (valid != FALSE)
         {
            ie_counter++;        /* count only good ones */
         }

      }

      allowed_ie_pointer = 0;

      /*
       * while more IEs in message
       */

      while ((p_message_read < ((byte *) message + sizeof(IMH_T) + length)) &&
             (return_value != FALSE ))
      {

         /*
          * check for no more mandatory IEs
          */

         if ( ( allowed_ie_pointer >= ie_counter ) ||
              ( stored_allowed_ie[allowed_ie_pointer].presence != MANDATORY_IE ))
         {
            break;
         }

         /*
          * exclude odd IEs from check which dont actually have an IEI
          */

         switch (stored_allowed_ie[allowed_ie_pointer].format)
         {
            case FORMAT_V:       /* single byte, no IE */

               /*
                * in this case, the pointer points to the location 1 byte
                * before the actual start of the IE, so that when the IE
                * is copied into messages for MN, space is left for the
                * IEI which must be explicitly entered. The length value
                * is also modified to allow for the extra IEI byte
                */
               if((p_message_read + sizeof(byte)) > ((byte *) message + sizeof(IMH_T) + length))
               {
                 return_value = FALSE;
                 MSG_HIGH_DS_0(MN_SUB,"=MNCNM= 1 Done with checking protocol for mandatory IEs");
                 break;
               }

               CNM_store_IE_info
               (
                  p_message_read - 1,  /* leave space for phantom IEI */
                  stored_allowed_ie[allowed_ie_pointer].allowed_IEI,
                  (unsigned char)(stored_allowed_ie[allowed_ie_pointer].IE_length + 1),
                  MANDATORY_IE,
                  last_octet_data_pointer
               );


               stored_allowed_ie[allowed_ie_pointer].found = TRUE;

               p_message_read++;
               allowed_ie_pointer++;
               break;

            case FORMAT_LV:         /* variable number of  bytes, no IE */

               /*
                * type obtained from allowed value, length is in first
                * byte of IE, but this does not include the length field, so
                * add 1, plus 1 for the phantom byte at the beginning of the
                * IE where the IEI should be
                */

               /* *********************************************
                * Fix facility IE length if necessary
                * ********************************************* */
               if ( stored_allowed_ie[allowed_ie_pointer].allowed_IEI == IEI_FACILITY &&
                     (p_message_read[0] & 0x80) /* possible X.209 encoding*/                )
               {
                  fix_facility_ie_len (&p_message_read[0], last_octet_data_pointer);
               }
               if((p_message_read + p_message_read[0] + 1) > ((byte *) message + sizeof(IMH_T) + length))
               {
                 return_value = FALSE;
                 MSG_HIGH_DS_0(MN_SUB,"=MNCNM= 2 Done with checking protocol for mandatory IEs");
                 break;
               }


               actual_IE_length = CNM_get_IE_length(p_message_read[0]);
               CNM_store_IE_info
               (
                  p_message_read - 1,  /* leave space for phantom IEI */
                  stored_allowed_ie[allowed_ie_pointer].allowed_IEI,
                  actual_IE_length, 
                  MANDATORY_IE,
                  last_octet_data_pointer
               );


               stored_allowed_ie[allowed_ie_pointer].found = TRUE;

               /*
                * increment pointer by length of IE, determined from first
                * byte in IE
                */

               p_message_read += actual_IE_length -1;
               allowed_ie_pointer++;
               break;

            default:       /* all other formats - we can check the IEI */

               if ( *p_message_read != stored_allowed_ie
                        [allowed_ie_pointer].allowed_IEI )
               {

                  /*
                   *  we do not have a mandatory match
                   */

                  ERR("Missing mandatory IE: expected mandatory IE = %d\n",
                        (dword) (stored_allowed_ie[allowed_ie_pointer].allowed_IEI), 0, 0);

                  message_IEI =
                      stored_allowed_ie[allowed_ie_pointer].allowed_IEI;

                  return_value = FALSE;
               }
               else
               {

                  /*
                   * work out the IE length
                   */

                  if (stored_allowed_ie[allowed_ie_pointer].format
                        == FORMAT_TLV)
                  {

                     /*
                      * length is second byte of IE, but this doesnt
                      * include the IEI and the length field, so add 2
                      */
                     if((p_message_read +1) >= ((byte *) message + sizeof(IMH_T) + length))
                     {
                      return_value = FALSE;
                      MSG_HIGH_DS_0(MN_SUB,"=MNCNM= Done with checking protocol for mandatory IEs");
                      break;
                     }
                     /* *********************************************
                      * Fix facility IE length if necessary
                      * ********************************************* */
                     if ( stored_allowed_ie[allowed_ie_pointer].allowed_IEI == IEI_FACILITY &&
                           (p_message_read[1] & 0x80) /* possible X.209 encoding*/                )
                     {
                        fix_facility_ie_len (&p_message_read[1], last_octet_data_pointer);
                     }
                     actual_IE_length = CNM_get_IE_length(p_message_read[1]);
                  }
                  else   /* must be TV or T */
                  {

                     /*
                      * length is pre-defined, so comes from the IE
                      * database
                      */

                     actual_IE_length = stored_allowed_ie
                        [allowed_ie_pointer].IE_length;
                  }
                  if((p_message_read + actual_IE_length) > ((byte *) message + sizeof(IMH_T) + length))
                  {
                    return_value = FALSE;
                    MSG_HIGH_DS_0(MN_SUB,"=MNCNM= 3 Done with checking protocol for mandatory IEs");
                    break;
                  }

                  CNM_store_IE_info(
                     p_message_read,
                     stored_allowed_ie[allowed_ie_pointer].allowed_IEI,
                     actual_IE_length,
                     MANDATORY_IE,
                     last_octet_data_pointer);

                  stored_allowed_ie[allowed_ie_pointer].found = TRUE;

                  p_message_read += actual_IE_length;
                  allowed_ie_pointer++;

               }
               break;
         }

      }  /* end while more data in message and imperative part */

      if ( return_value != FALSE )   /* if ok so far */
      {
         while ( p_message_read <
                  (byte *) message + sizeof(IMH_T) + length )
         {
            if ( IS_IEI_SHIFT( *p_message_read ) )
            {
               if (((*p_message_read) & 0x08) == 0 ) /* locking shift */
               {
                  break;
               }
               else     /* non-locking shift */
               {

                  /*
                   * Add length of this IE to pMessageRead
                   */

                  p_message_read += 1;

                  if ( p_message_read <
                        (byte *) message + sizeof(IMH_T) + length )
                  {
                     break;      /* check silly message ending */
                  }

                  /*
                   * check for silly case of locking shift following
                   * non-locking shift (see 04.08 10.5.4.3)
                   */

                  if ( (IS_IEI_SHIFT( *p_message_read ) ) &&
                       (((*p_message_read) & 0x08) == 0 ) )
                  {
                     break;
                  }

                  /*
                   * otherwise skip next IE
                   */

                  if (((*p_message_read) & 0x80) != 0 )  /* type 1 or 2 */
                  {
                     p_message_read += 1;

                     if ( p_message_read <
                           (byte *) message + sizeof(IMH_T) + length )
                     {
                        break;      /* check silly message ending */
                     }

                  }
                  else
                  {
                     if((p_message_read +1) >= ((byte *) message + sizeof(IMH_T) + length))
                     {
                      break;
                     }
                     p_message_read = p_message_read +
                                       p_message_read[1] +
                                       2;

                     if ( p_message_read <
                           (byte *) message + sizeof(IMH_T) + length )
                     {
                        break;      /* check silly message ending */
                     }

                  }

               }  /* end if locking shift */
            }  /* end if shift */
            else
            {

               if (IS_IEI_REPEAT_INDICATOR(*p_message_read) )
               {
                  message_IEI = IEI_REPEAT_INDICATOR;
               }
               else
               {
                  message_IEI = * p_message_read;
               }


               valid_IE = FALSE;
               allowed_IE = FALSE;

               for ( this_ie=0;
                     this_ie< ie_counter;
                     this_ie++)
               {
                  if ( stored_allowed_ie[this_ie].allowed_IEI ==
                           message_IEI )
                  {

                     /*
                      * flag that this ie is expected
                      */

                     allowed_IE = TRUE;

                     /*
                      * Check that this has not been found before
                      */

                     if ( stored_allowed_ie[this_ie].found == FALSE)
                     {
                        valid_IE = TRUE;
                        stored_allowed_ie[this_ie].found = TRUE;
                        break;
                     }
                  }
               }

            }
            if ( valid_IE != FALSE )     /* we have a match */
            {

               /*
                * get actual length of this IE. 1 byte if its
                * a type 1 or 2, otherwise the length is in the
                * byte after the IEI
                */

               if ((message_IEI & 0x80) != 0)      /* type 1 or 2 */
               {
                  actual_IE_length = 1;
               }
               else
               {
                  if ( stored_allowed_ie[this_ie].format == FORMAT_TV )
                  {
                     actual_IE_length = stored_allowed_ie[this_ie].IE_length;
                  }
                  else
                  {
                     /* *********************************************
                      * Fix facility IE length if necessary
                      * ********************************************* */

                     if((p_message_read +1) >= ((byte *) message + sizeof(IMH_T) + length))
                     {
                      break;
                     }
                     if ( stored_allowed_ie[this_ie].allowed_IEI == IEI_FACILITY &&
                          p_message_read[1] & 0x80 /* possible X.209 encoding*/                )
                     {
                        fix_facility_ie_len (&p_message_read[1], last_octet_data_pointer);
                     }
                     actual_IE_length = CNM_get_IE_length(p_message_read[1]);
                  }
               }

               /*
                * save details of this IE found - note the presence indication
                * may be mandatory as in some messages there are mandatory
                * elements after some optional or conditional ones, so get
                * value from allowed ie data
                */

               if((p_message_read + actual_IE_length) > ((byte *) message + sizeof(IMH_T) + length))
               {
                 if(stored_allowed_ie[this_ie].presence == MANDATORY_IE)
                 {
                    return_value = FALSE; 
                 }
                 MSG_HIGH_DS_0(MN_SUB,"=MNCNM= 4 Done with checking protocol for optional IEs");
                 break;
               }


               CNM_store_IE_info(
                  p_message_read,
                  message_IEI,
                  actual_IE_length,
                  stored_allowed_ie[this_ie].presence,
                  last_octet_data_pointer);

               p_message_read += actual_IE_length;
            }
            else if (allowed_IE == TRUE )   /* check for unexpected repeat */
            {

               /*
                * skip IE to ignore it
                */

               if ((message_IEI & 0x80) != 0)      /* type 1 or 2 */
               {
                  actual_IE_length = 1;
               }
               else
               {
                 if((p_message_read +1) >= ((byte *) message + sizeof(IMH_T) + length))
                 {
                   break;
                 }
                 actual_IE_length = CNM_get_IE_length(p_message_read[1]);
               }
               p_message_read += actual_IE_length;
            }
            else   /* not a valid IE */
            {

               /*
                * check if comprehension is required
                */

               if ( ( message_IEI & 0xf0 ) == 0 )
               {
                  return_value = FALSE;
                  break;
               }
               else
               {
                  if ((message_IEI & 0x80) != 0)      /* type 1 or 2 */
                  {
                     actual_IE_length = 1;
                  }
                  else
                  {
                   if((p_message_read +1) >= ((byte *) message + sizeof(IMH_T) + length))
                   {
                    break;
                   }
                    actual_IE_length = CNM_get_IE_length(p_message_read[1]);
                  }

               }
               p_message_read += actual_IE_length;

            }  /* end else not valie IE */

         }     /* end while more data in message */
      }        /* end if return_value is TRUE */

      if ( return_value != FALSE )
      {
         valid_IE = TRUE;

         for ( this_ie=0;
               this_ie< ie_counter;
               this_ie++)
         {
            if ( ( stored_allowed_ie[this_ie].found == FALSE)
              && ( stored_allowed_ie[this_ie].presence == MANDATORY_IE ) )
            {
               message_IEI = stored_allowed_ie[this_ie].allowed_IEI;

               valid_IE = FALSE;
               break;
            }
         }

         if ( valid_IE == FALSE )
         {

            /*
             * missing mandatory IE
             */

            return_value = FALSE;
         }
      }


      if ( return_value == FALSE )
      {

         if ( ( message_type == SETUP ) ||
              ( message_type == EMERGENCY_SETUP ) ||
              ( message_type == RELEASE ) )
         {

            /* If protocol error is found in the RELEASE message
             * for mandatory IEs and a RELEASE_COMP message is sent
             * to NW by CNM, then CNM must send MNCC_REL_IND
             * to MN so that upper layers can be informed about call
             * clearing.
             * This scenario doesn't exist right now because only TI-PD
             * and message_type are mandatory informations in RELEASE message
             * as of now. This processing will be needed if in future, RELEASE
             * message includes any other mandatory information.
             * */

            if((message_type == RELEASE) &&
               (connection_id < MAXNO_CC_TRANSACTIONS) &&
               (mn_call_information_p[connection_id] != NULL))
            {
               MNCC_REL_IND_T             *p_mncc_rel_ind;
               p_mncc_rel_ind = (MNCC_REL_IND_T*) CNM_alloc( sizeof( MNCC_REL_IND_T ));
               if(p_mncc_rel_ind != NULL)
               {
                   memset(p_mncc_rel_ind, NULL, sizeof(MNCC_REL_IND_T));
                   p_mncc_rel_ind->message_header.message_set = MS_MN_CC;
                   p_mncc_rel_ind->message_header.message_id  = MNCC_REL_IND;
                   p_mncc_rel_ind->connection_id = connection_id;
#if defined(FEATURE_DUAL_SIM) && defined(FEATURE_DUAL_ACTIVE)
                   {
                      p_mncc_rel_ind->as_id = (sys_modem_as_id_e_type)mn_dsda_as_id;
                   }
#endif
                  
                   p_mncc_rel_ind->cause = 0;          
   
                   PUT_IMH_LEN( (int)( (byte *)p_mncc_rel_ind->data -
                                    (byte *)&p_mncc_rel_ind->as_id),
                                     &p_mncc_rel_ind->message_header);
                  mn_call_information_p[connection_id]->offline = TRUE;
       
                   CNM_send_message( (IMH_T *)p_mncc_rel_ind,
                                 GS_QUEUE_MN_CM );
                   CNM_free( p_mncc_rel_ind );
               }
            }
            else
            {
              MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid Connection Id %d",connection_id);
            }

            CNM_send_release_complete( connection_id,
                                      PD_CC,
                                      INVALID_MANDATORY_INFORMATION,
                                      (ie_facility_T *) 0 );

            /*
             * set this connection call state back to CC_STATE_U0
             */
            if(connection_id < MAXNO_CC_TRANSACTIONS)
            {
              CNM_set_state_to_U0( connection_id );

              if( message_type == SETUP )
              {
                /* If protocol error is found in SETUP
                 * release the call objects.
                 * */
                MN_clear_call_data(connection_id);
              }

            }
            else
            {
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bounds and buffer overflow");
            }
         }
         else if ( message_type == DISCONNECT )
         {

            p_release->msg_head.type = RELEASE;

            no_of_bytes = CNM_write_cause_ie( (byte *)p_release->data,
                                          INVALID_MANDATORY_INFORMATION);

            /*
             * Fill cm_cc_cause in mn_call_information_p global variable, if it is not already present.
             * This will be used to fill cause in 0x713D log packet logged to DIAG, when UE receives
             * RELEASE_COMPLETE in respnse to this message from NW.
             */
            if( (connection_id < MAXNO_CC_TRANSACTIONS) && 
              (mn_call_information_p[connection_id] != NULL) )             
            {
              mn_call_information_p[connection_id]->mo_release_cause_diag_packet.present= TRUE;
              mn_call_information_p[connection_id]->mo_release_cause_diag_packet.cause_value = INVALID_MANDATORY_INFORMATION;
            }
            else
            {
              MSG_ERROR_DS_1(MN_SUB,"=MNCNM= Invalid connection id %d\n",connection_id);
            }
            PUT_IMH_LEN( sizeof(msg_head_T) +
                        no_of_bytes -
                        sizeof(IMH_T),
                        &p_release->msg_head.IMH);

            CNM_send_primitive_to_mm(connection_id,
                                    PD_CC,
                                    (IMH_T *)p_release);

            /*
             * set this connection call state to CC_STATE_U19
             */
            if(connection_id < MAXNO_CC_TRANSACTIONS)
            {
              if(cc_transaction_information_p[connection_id] != NULL)
              {
                cc_transaction_information_p[connection_id]->cc_state = CC_STATE_U19;
              }
              else
              {
                MSG_HIGH_DS_1(MN_SUB,"=MNCNM= Cannot set cc_state to CC_STATE_U19 as cc_transaction_information_p[%d] is NULL",
                              connection_id);
              }
            }
            else
            {
              MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Index out of bound and buffer overflow");
            }

         }
         else if ( ( message_type == RELEASE_COMPLETE ) ||
              ( message_type == HOLD_REJECT ) ||
              ( message_type == RETRIEVE_REJ ) )
         {
            return_value = TRUE;              /* process it anyway */
         }
         else     /* check if comprehension required */
         if ( (message_IEI & 0xf0) == 0 )
         {

            CNM_send_status( connection_id,
                            protocol_descriminator,
                            INVALID_MANDATORY_INFORMATION);
         }
      }
   }
   else
   if (protocol_descriminator == PD_SMS)    /* check protocol desc. for SMS */
   {

      /*
       * all we can check here is that the CP-DATA message has some RP-DATA,
       * and the CP-ERROR message has a cause value
       */

      switch ( message_type )
      {
         case CP_DATA:
            GET_IMH_LEN (length, &message->msg_head.IMH);
#ifdef FEATURE_DUAL_SIM
            length = length - sizeof(byte);
#endif
            if ( length < 5 )    /* no data */
            {
               CNM_send_cp_error( connection_id,
                        CP_CAUSE_INVALID_MANDATORY_INFORMATION );
               return_value = FALSE;
            }
            break;

         case CP_ERROR:
            GET_IMH_LEN (length, &message->msg_head.IMH);
#ifdef FEATURE_DUAL_SIM
            length = length - sizeof(byte);
#endif
            if ( length < 3 )    /* no data */
            {
               return_value = FALSE;
            }
            break;

         case CP_ACK:            /* no checking possible */
            break;

         default:
            CNM_send_cp_error( connection_id,
                        CP_CAUSE_MSG_TYPE_NON_EXIST_OR_NOT_IMPLEMENTED );
            return_value = FALSE;    /* ignore unknown message */
            break;
      }
   }
   else
   {
      ERR("Unexpected PD\n", 0, 0, 0);

      return_value = FALSE;
   }

   CNM_free( p_release );

   return return_value;
}
/**************** End of   CNM_check_for_protocol_errors **************************************/


/**************** Begin of   CNM_check_future_extensions **************************
 *
 *  Function name: CNM_check_future_extensions()
 *  -------------------------------------------
 *
 *  Description:
 *  ------------
 *  Function to check the IE and remove any unexpected extensions which
 *  may be added by the 0/1 Ext mechanism. The input message is modified
 *  to strip out the extra bytes, so that subsequent processing is
 *  unaffected
 *
 *  Parameters:
 *  -----------
 *  IEI     : IN - identity of the IE
 *  start   : IN - pointer to start of IE in input message
 *  length  : IN - pointer to length of this IE
 *  last_octet_data_pointer : IN - pointer to last octet of the buffer where start is pointing to.
 *
 *  Returns:
 *  --------
 *  None
 *
 ******************************************************************************/

static VOID_FUNC   CNM_check_future_extensions(
                                    byte IEI, byte *start, byte *length, byte *last_octet_data_pointer )
{

   int      index;
   byte     octet_number;
   byte     sub_octet_number;
   byte     new_length;
   byte     *read_ptr;
   byte     *write_ptr;
   byte     count;

   /*
    * Define expected number of bytes for each octet, starting from
    * octet 3. a -1 indicates an indeterminate number of bytes, zero
    * indicates no more bytes
    */

   static int8 expected_octets[][8] =
   {
      { IEI_AUXILIARY_STATES,       1, 0, 0, 0, 0, 0, 0 },
#if defined(FEATURE_MULTIMEDIA_FALLBACK) || defined(FEATURE_REL6)
      { IEI_BEARER_CAPABILITY,      6, 1, 3, 8, 1, 0, 0 },
#else
      { IEI_BEARER_CAPABILITY,      1, 1, 3, 8, 1, 0, 0 },
#endif
      { IEI_CALLED_PARTY_BCD_NO,    1,-1, 0, 0, 0, 0, 0 },
      { IEI_CALLED_PARTY_SUBAD,     1,-1, 0, 0, 0, 0, 0 },
      { IEI_CALLING_PARTY_BCD_NO,   2,-1, 0, 0, 0, 0, 0 },
      { IEI_CALLING_PARTY_SUBAD,    1,-1, 0, 0, 0, 0, 0 },
      { IEI_REDIR_PARTY_BCD_NO,     2,-1, 0, 0, 0, 0, 0 },
      { IEI_REDIR_PARTY_SUBADDR,    1,-1, 0, 0, 0, 0, 0 },
      { IEI_CAUSE,                  2, 1,-1, 0, 0, 0, 0 },
      { IEI_CONNECTED_NUMBER,       2,-1, 0, 0, 0, 0, 0 },
      { IEI_CONNECTED_SUBADDRESS,   1,-1, 0, 0, 0, 0, 0 },
      { IEI_HIGH_LAYER_COMPATIB,    1, 2, 0, 0, 0, 0, 0 },
      { IEI_PROGRESS_INDICATOR,     1, 1, 0, 0, 0, 0, 0 },
      { 0,                          0, 0, 0, 0, 0, 0, 0 } /* END OF LIST */
   };


   if(*length <= 2)
   {
     return;

      /*
       *   This function is not supposed to do anything if length of IE (*(start +1))
       *   is less than 1 or *length < 3
       */
   }

   /*
    * scan expected octets table for matching IEI
    */

   index = 0;

   while (expected_octets[index][0] != 0 )
   {
      if ( expected_octets[index][0] == IEI )
      {
         break;
      }
      index++;
   }

   /*
    * Ignore non specific cases
    */

   if ( expected_octets[index][0] == IEI )
   {
      read_ptr = start + 2;      /* point at first byte after length */
      write_ptr = read_ptr;

      octet_number = 3;
      sub_octet_number = 0;

      new_length = *length;


      for ( count = 0; 
           ( (count < (*length) - 2) && (write_ptr <= last_octet_data_pointer) && (read_ptr <= last_octet_data_pointer) );
           count++ )
      {
         if(octet_number > 9)
         {
            MSG_ERROR_DS_0(MN_SUB,"=MNCNM= Expected index is beyond the limits");
            return;
         }
         if (expected_octets[index][octet_number-2] == -1)
         {
            *write_ptr++ = *read_ptr++;
         }
         else if (expected_octets[index][octet_number-2] == 0)
         {
            /* do nothing - ignore the rest of the IE, but adjust the length */

            new_length--;
         }
         else
         {

            /*
             * if this sub-octet is expected, then copy it
             */

            if ( (sub_octet_number + 1) <=
                              expected_octets[index][octet_number-2] )
            {
               *write_ptr = *read_ptr;
               write_ptr++;
            }
            else
            {
               /*
                * set top bit of previous byte to indicate end of octet group
                */

               *(write_ptr-1) |= 0x80;

               new_length--;  /* skipped byte, so reduce actual length */
            }

            if (( *read_ptr & 0x80 ) == 0 )
            {
               sub_octet_number++;

            }
            else
            {
               sub_octet_number = 0;
               octet_number++;
            }
            read_ptr++;
         }
      }

      *length = new_length;

      /*
       * Update length field in IE
       */

      *(start + 1) = (unsigned char)(new_length-2);
   }
}
/**************** End of   CNM_check_future_extensions **************************************/
