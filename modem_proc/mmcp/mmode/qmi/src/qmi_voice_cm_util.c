/*===========================================================================

                         Q M I _ V O I C E _ C M _ U T I L . C

DESCRIPTION

 The Data Services Qualcomm Voice utilities file.

EXTERNALIZED FUNCTIONS


Copyright (c) 2004-2014 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/
/*===========================================================================
                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/mmode/qmi/src/qmi_voice_cm_util.c#1 $ $DateTime:$ $Author:$

when        who    what, where, why
--------    ---    ----------------------------------------------------------
04/26/09    sk     Created Module.
===========================================================================*/

/*===========================================================================
 
  INCLUDE FILES FOR MODULE
 
===========================================================================*/

#include "mmcp_variation.h"
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_QMI_SERVICE_SPLIT

//#include <math.h> // for floor() --> replaced it with myfloor()
#include <memory.h>
#include <string.h>
#include "nv.h"
#include "amssassert.h"
#include "msg.h"
#include "qm_comdef.h"
#include "qmi_voice_cm_if.h"
#include "qmi_voice_cm_util.h"
#include "qmi_voice_call_list.h"
#include "qmi_voice_cm_sups.h"
#include "qmi_mmode_crit_sect.h"
#include "qmi_mmode_task_cmd.h"
#include <stringl/stringl.h>


#ifndef FEATURE_NO_QMI_VOICE



/*===========================================================================

                   INTERNAL DEFINITIONS AND TYPES

===========================================================================*/

typedef struct
{
  qmi_voice_cm_if_sys_mode_e_type     sys_mode[CM_NO_STACKS];
  sys_srv_status_e_type               srv_status[CM_NO_STACKS];
}qmi_voice_cm_ss_info; 

typedef struct
  {
  boolean is_ss_info_rcvd;

  qmi_voice_cm_ss_info ss_info[QMI_VOICE_CM_IF_AS_ID_MAX];
  qmi_mmode_crit_sect_type ss_cs;

  boolean available_conn_ids[ CM_CALL_ID_MAX ];

}qmi_voice_cm_util_info_s;

typedef struct
{
  uint16  utf16_char;
  byte    gsm7_char;
} utf16_to_gsm7_ext_set;

/*
*/
static const utf16_to_gsm7_ext_set utf16_to_gsm7_ext_map[] =
{
  {12,   10}, /*Form feed*/
  {94,   20}, /*Caret / Circumflex*/
  {123,  40}, /*Left curly bracket*/
  {125,  41}, /*Right curly bracket*/
  {92,   47}, /*Backslash*/
  {91,   60}, /*Left square bracket*/
  {126,  61}, /*Tilde*/
  {93,   62}, /*Right square bracket*/
  {124,  64}  /*Vertical bar*/
              /* Euro sign handling missing*/
};

#define QMI_MMODE_CHAR_QN 63
#define QMI_MMODE_CHAR_ESC 27


/*===========================================================================

                         LOCAL VARIABLES

===========================================================================*/

static qmi_voice_cm_util_info_s util_info;


/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/

void qmi_voice_cm_utili_init_conn_ids
(
  void
);


/*===========================================================================

                                FUNCTIONS

===========================================================================*/


double myfloor(double x)
{
  return (double)(int)x;
}


/*=========================================================================
  FUNCTION:  qmi_voice_cm_util_init

===========================================================================*/
/*!
    @brief

    Intitialization to be done here
    
    @return
    None.
*/
/*=========================================================================*/
void qmi_voice_cm_util_init
(
  void
)
{
  memset(&util_info, 0 , sizeof(util_info));
  QMI_MMODE_INIT_CRIT_SECTION(&util_info.ss_cs);

  qmi_voice_cm_utili_init_conn_ids();

}/* qmi_voice_cm_util_init */


/*=========================================================================
  FUNCTION:  qmi_voice_cm_util_copy_alpha_info_from_cm

===========================================================================*/
/*!
    @brief

    Utility to make a copy of the alpha buffer and data sent from CM
    
    @return
    None.
*/
/*=========================================================================*/
void qmi_voice_cm_util_copy_alpha_info_from_cm
(
  qmi_voice_cm_if_alpha_id_s *dest,
  cm_alpha_s_type *src
)
{

    if(!src || !dest )
    {
      QM_MSG_ERROR("Invalid input parameters");
      return;
    }
	
    //Copy length
    dest->alpha_len = src->len; 

    /* Alpha from CM is nul terminated. */
    if(dest->alpha_len == 0)
    {
      QM_MSG_MED_1("Invalid Alpha length from CM %d", src->len);
      dest->alpha_len = 0;
      return;
    }	
    else if((dest->alpha_len == 1) && (src->buf[0] == '\0'))
    {
      QM_MSG_MED("NULL alpha of length 1 received from CM; it will be passed on as it is.");
      dest->alpha_len = 1;
      dest->alpha_text[0] = '\0';
      return;
    }
	
    //Check for validity of length
    if(dest->alpha_len > QMI_VOICE_CM_IF_MAX_ALPHA_TAG_CHARS)
    {
      dest->alpha_len = QMI_VOICE_CM_IF_MAX_ALPHA_TAG_CHARS;
    }
     
    //Copy DCS info
    if(src->dcs == CM_USSD_DCS_UCS2)
    {
      dest->alpha_dcs = QMI_VOICE_CM_IF_ALPHA_DCS_UCS2;
      if(dest->alpha_len >= 2) //Prevents invalid memory access
      {
        if((src->buf[dest->alpha_len-1] == '\0') && (src->buf[dest->alpha_len-2] == '\0')) //Double NULL termination removed
        {
          dest->alpha_len = dest->alpha_len-2;
        }
        else if((src->buf[dest->alpha_len-1] == '\0') && (src->buf[dest->alpha_len-2] != '\0')) //Single NULL termination removed
        {
          dest->alpha_len = dest->alpha_len-1;
        }
      }
    }
    else if(src->dcs == CM_USSD_DCS_8_BIT)
    {
      dest->alpha_dcs = QMI_VOICE_CM_IF_ALPHA_DCS_GSM_DEFAULT;
      if(src->buf[dest->alpha_len-1] == '\0')
      {
        dest->alpha_len = dest->alpha_len-1;
      } 
    }
    else 
    {
      QM_MSG_ERROR_1("Incorrect DCS received? %d", src->dcs);
    }
    
    //Copy alpha buffer
    memscpy(dest->alpha_text, sizeof(dest->alpha_text), src->buf, dest->alpha_len);

    return;
}/* qmi_voice_cm_util_copy_alpha_info_from_cm */


/*=========================================================================
  FUNCTION:  qmi_voice_cm_util_copy_alpha_ident_from_cm

===========================================================================*/
/*!
    @brief

    Utility to make a copy of the alpha buffer and data sent from CM
    
    @return
    None.
*/
/*=========================================================================*/
void qmi_voice_cm_util_copy_alpha_ident_from_cm
(
  voice_alpha_ident_type_v02 *dest,
  cm_alpha_s_type            *src
)
{
  if(!src || !dest )
  {
    QM_MSG_ERROR("Invalid input parameters");
    return;
  }

  //Copy length
  dest->alpha_text_len = src->len; 

  /* Alpha from CM is nul terminated. */
  if(dest->alpha_text_len == 0)
  {
    QM_MSG_MED_1("Invalid Alpha length from CM %d", src->len);
    dest->alpha_text_len = 0;
    return;
  }
  else if((dest->alpha_text_len == 1) && (src->buf[0] == '\0'))
  {
    QM_MSG_MED("NULL alpha of length 1 received from CM; it will be passed on as it is.");
    dest->alpha_text_len = 1;
    dest->alpha_text[0] = '\0';
    return;
  }

  //Check for validity of length
  if(dest->alpha_text_len > QMI_VOICE_ALPHA_TEXT_MAX_V02)
  {
    dest->alpha_text_len = QMI_VOICE_ALPHA_TEXT_MAX_V02;
  }

  //Copy DCS info
  if(src->dcs == CM_USSD_DCS_UCS2)
  {
    dest->alpha_dcs = ALPHA_DCS_UCS2_V02;
    if(dest->alpha_text_len >= 2) //Prevents invalid memory access
    {
      if((src->buf[dest->alpha_text_len-1] == '\0') && (src->buf[dest->alpha_text_len-2] == '\0')) //Double NULL termination removed
      {
        dest->alpha_text_len = dest->alpha_text_len-2;
      }
      else if((src->buf[dest->alpha_text_len-1] == '\0') && (src->buf[dest->alpha_text_len-2] != '\0')) //Single NULL termination removed
      {
        dest->alpha_text_len = dest->alpha_text_len-1;
      }
    }
  }
  else if(src->dcs == CM_USSD_DCS_8_BIT)
  {
    dest->alpha_dcs = ALPHA_DCS_GSM_V02;
    if(src->buf[dest->alpha_text_len-1] == '\0')
    {
      dest->alpha_text_len = dest->alpha_text_len-1;
    } 
  }
  else 
  {
    QM_MSG_ERROR_1("Incorrect DCS received? %d", src->dcs);
  }

  //Copy alpha buffer
  memscpy(dest->alpha_text, sizeof(dest->alpha_text), src->buf, dest->alpha_text_len);

  return;
}/* qmi_voice_cm_util_copy_alpha_ident_from_cm */


/*=========================================================================
  FUNCTION:  qmi_voice_cm_util_copy_cc_alpha_info_from_cm

===========================================================================*/
/*!
    @brief

    Utility to make a copy of the call control alpha buffer and data sent from CM
    
    @return
    None.
*/
/*=========================================================================*/
void qmi_voice_cm_util_copy_cc_alpha_info_from_cm
(
  qmi_voice_cm_if_cc_alpha_id_s *dest,
  cm_alpha_s_type *src
)
{
    uint8 len;

    if(!src || !dest )
    {
      QM_MSG_ERROR("Invalid input parameters");
      return;
    }

    //Copy length
    dest->alpha_len = MIN(src->len,QMI_VOICE_CM_IF_MAX_CC_ALPHA_TAG_CHARS);
    len = dest->alpha_len;

    //Copy DCS info
    if(src->dcs == CM_USSD_DCS_UCS2)
    {
      dest->alpha_dcs = QMI_VOICE_CM_IF_ALPHA_DCS_UCS2;
      if((len >=2) && (src->buf[len-1] == '\0') && (src->buf[len-2] == '\0')) /*double null terminated UCS2 string*/
      {
        len = len-2;
      }
      else if((len >= 1) && (src->buf[len-1] == '\0')) /*single null terminated UCS2 string*/
      {
        len = len-1;
      }
    }
    else if(src->dcs == CM_USSD_DCS_8_BIT)
    {
      dest->alpha_dcs = QMI_VOICE_CM_IF_ALPHA_DCS_GSM_DEFAULT;
      if((len >= 1) && src->buf[len-1] == '\0')
      {
        len = len-1;
      } 
    }
    else 
    {
      QM_MSG_ERROR_1("Incorrect DCS received? %d", src->dcs);
    }
    
    dest->alpha_len = len;
    //Copy alpha buffer
    memscpy(dest->alpha_text, sizeof(dest->alpha_text), src->buf, dest->alpha_len);

    return;
}/* qmi_voice_cm_util_copy_cc_alpha_info_from_cm */

/*=========================================================================
  FUNCTION:  qmi_voice_cm_util_process_cnap_info

===========================================================================*/
/*!
    @brief
    This function is called when the incom call arrives,
    if any CNAP information is present it is processed.

    @return
    None.
*/
/*=========================================================================*/
void qmi_voice_cm_util_process_cnap_info 
(
  const cm_mm_call_info_s_type * call_info_ptr,
  boolean report_change
)
{
  char name[CM_MAX_ALPHA_TAG_CHARS];
  IxErrnoType  status = E_SUCCESS;
  uint8 pi ; /*presentation indicator */
  uint8 name_len;
  cm_ussd_dcs_e_type dcs=CM_USSD_DCS_UNSPECIFIED;
  byte carriage_return = 0xD;
  byte last_7bits = 0x0;
  cm_enc_alpha_s_type  temp_buf;
  uint8  num_bytes_nw = 0;
  double num_bytes_req=0;
  uint8 as_id = (uint8)QMI_VOICE_CM_IF_AS_ID_PRIMARY;

  ASSERT( call_info_ptr != NULL );

#ifdef FEATURE_DUAL_SIM
  as_id = (uint8)call_info_ptr->asubs_id;
#endif

  memset(&temp_buf, 0, sizeof( temp_buf));
  memscpy (&temp_buf, sizeof(temp_buf), &(call_info_ptr->enc_alpha), sizeof(temp_buf));

  if( ( temp_buf.num_bytes > 0 )  &&
      ( qmi_voice_call_list_is_call_present( call_info_ptr->call_id ) ) )
  {

      dcs = qmi_voice_cm_util_get_ussd_dcs( call_info_ptr->enc_alpha.coding_scheme );
      QM_MSG_MED_3("Received DCS=%d, coding_scheme=%d, len=%d ", dcs, call_info_ptr->enc_alpha.coding_scheme, call_info_ptr->enc_alpha.num_bytes);
   
      if(dcs == CM_USSD_DCS_7_BIT)
      {

        //Check whether network has sent extra NULL byte(s)
        num_bytes_nw = call_info_ptr->enc_alpha.num_bytes;   //Number of bytes from Network
        num_bytes_req = ((call_info_ptr->enc_alpha.num_characters * 7.0)/8.0); //Number of bytes required to pack num_characters
      
        QM_MSG_MED_3(" Number of bytes Req= %lf (float),%d (int),Nw sent=%d, ", num_bytes_req, (int)num_bytes_req, num_bytes_nw); 
        if((num_bytes_req - (int)num_bytes_req) != 0)
        {
          num_bytes_req = (int) num_bytes_req+1;
        QM_MSG_MED_1(" Number of bytes Req= %lf", num_bytes_req); 
        }
        
        if(num_bytes_req < num_bytes_nw)  //If network has sent more data, truncate it
        {
          temp_buf.num_bytes = num_bytes_req;
          QM_MSG_MED_1("Final number of bytes = %d", temp_buf.num_bytes);  
          if(temp_buf.num_bytes == 0)
	  {
	    return; 
	  }
        }
        else if( (num_bytes_req) > num_bytes_nw)
        {
          QM_MSG_MED_1( "%s", "Inconsistent data received from network");  
        }

        name_len = cm_util_gsm_alphabet_to_ascii_internal( (byte *)name, sizeof(name),(byte *)call_info_ptr->enc_alpha.buf,
                                     temp_buf.num_bytes );
        QM_MSG_MED_2( "CNAP name of length %d in ascii = %s", name_len, name );  
        if(name_len == 0)
        {
          return; 
        }
        else if(call_info_ptr->enc_alpha.num_characters != name_len)
        { 
           QM_MSG_MED_2("There is a padding @ in asci string Exp len = %d in Act len = %d", call_info_ptr->enc_alpha.num_characters, name_len);
           if(call_info_ptr->enc_alpha.num_bytes > 0 && call_info_ptr->enc_alpha.num_bytes <= CM_MAX_ALPHA_TAG_BYTES)
           {
             QM_MSG_MED_1("Last byte before:  padding with CR is = %x (in hex)", temp_buf.buf[temp_buf.num_bytes-1]);
             last_7bits = temp_buf.buf[temp_buf.num_bytes-1];
           }
           last_7bits = last_7bits & 0xFE ;  //(11111110 = 0xFE) get last 7 bits of the byte
           QM_MSG_MED_1("Last 7 bits are = %x (in hex)", last_7bits);

           if( last_7bits == 0x0 )
           {                            
              if(call_info_ptr->enc_alpha.num_bytes > 0 && call_info_ptr->enc_alpha.num_bytes <= CM_MAX_ALPHA_TAG_BYTES)
             {
                temp_buf.buf[temp_buf.num_bytes-1] = temp_buf.buf[temp_buf.num_bytes-1] | (carriage_return<<1);
                QM_MSG_MED_1("Last byte after : padding with CR is = %x (in hex)", temp_buf.buf[temp_buf.num_bytes-1]);                 
             }
              name_len = cm_util_gsm_alphabet_to_ascii_internal( (byte *)name, sizeof(name),(byte *)temp_buf.buf,
                                     temp_buf.num_bytes );        
              QM_MSG_MED_2( "CNAP name After <CR> padding of length %d in ascii = %s", name_len, name );  
           }
           else
           {
              if(call_info_ptr->enc_alpha.num_bytes > 0 && call_info_ptr->enc_alpha.num_bytes <= CM_MAX_ALPHA_TAG_BYTES)
             {
                QM_MSG_ERROR_1("Fatal error the last seven bits are not 0! hex-val= %x", temp_buf.buf[temp_buf.num_bytes-1]);
             }
           }

        }
        else
        {
          QM_MSG_MED_1( "No padding issue for %s", temp_buf.buf );  
        }
      }

      /* Update name in the call list */
      if ( ( status = qmi_voice_call_list_update_cnap_name( call_info_ptr->call_id, temp_buf.buf,
                                                            temp_buf.num_bytes) )!= E_SUCCESS )
      {
         QM_MSG_ERROR_1("Failed to update the name of Call List entry for call_id %d", call_info_ptr->call_id);
      }

      /* Update name presentation in the call list*/
      if ( status == E_SUCCESS )
      {
         pi = (uint8)call_info_ptr->num.pi;
         if( call_info_ptr->num.pi == 4 )
         {
            pi = 0 ; /* in case when PI over ride category is provisioned */
         }
         if( ( status = qmi_voice_call_list_update_name_presentation( call_info_ptr->call_id,
                                                                        pi ) ) != E_SUCCESS )
         {
            QM_MSG_ERROR_1("Failed to update the name presentation of Call List entry for call_id %d", call_info_ptr->call_id);
         }
      }

      /* Update coding scheme in the call list*/
      if ( status == E_SUCCESS )
      {
         if( ( status = qmi_voice_call_list_update_coding_scheme( call_info_ptr->call_id,
                                                                 call_info_ptr->enc_alpha.coding_scheme ) ) != E_SUCCESS )
         {
            QM_MSG_ERROR_1("Failed to update the coding scheme of Call list entry for call_id %d", call_info_ptr->call_id);
         }
      }

      if ( (status == E_SUCCESS)  && report_change)
      {
         qmi_voice_call_list_report_all_call_status_changed(as_id);
      }
  }
} /* end of qmi_voice_cm_util_process_cnap_info */


/*=========================================================================
  FUNCTION:  qmi_voice_cm_util_convert_2s_complement_to_int

===========================================================================*/
/*!
    @brief
    convert a byte from 2's complement 6 bit number to integer

    @return
    integer

*/
/*=========================================================================*/
int qmi_voice_cm_util_convert_2s_complement_to_int(byte i)
{
  byte j=0;
  if (i >= 0x20 )
  {
    j = ((~i + 1) << 2);
    return ( -1 * (j >> 2) );
  }
  else
  {
    return i;
  }
} /* qmi_voice_cm_util_convert_2s_complement_to_int() */


/*=========================================================================
  FUNCTION:  qmi_voice_cm_util_is_digits_dtmf

===========================================================================*/
/*!
    @brief
    check if the digits in the number passed are dtmf digits

    @return
    TRUE if all digits are 0~9 and * or #. FALSE otherwise

*/
/*=========================================================================*/
boolean qmi_voice_cm_util_is_digits_dtmf(uint8 cnt, const uint8 *digit_buf)
{

  uint8                       i;

  ASSERT(digit_buf != NULL); 

#define  DECCHK( c ) ((c) >= '0' && (c) <= '9')

  for ( i=0; i<cnt; i++ )
  {
    if (!DECCHK(digit_buf[i]) &&
         digit_buf[i] != '*'  &&
         digit_buf[i] != '#'     )
    {
      return FALSE;
    }
  }

  return TRUE;

} /* qmi_voice_cm_util_is_digits_dtmf() */


/*=========================================================================
  FUNCTION:  qmi_voice_cm_utili_init_conn_ids

===========================================================================*/
/*!
    @brief
    Initializes the connection ids array

    @return
*/
/*=========================================================================*/
void qmi_voice_cm_utili_init_conn_ids
(
  void
)
{
  uint8 i;
  
  for(i=0;i<CM_CALL_ID_MAX;i++)
  {
    util_info.available_conn_ids[i] = TRUE;
  }

}/* qmi_voice_cm_utili_init_conn_ids */


/*=========================================================================
  FUNCTION:  qmi_voice_cm_util_allocate_conn_id

===========================================================================*/
/*!
    @brief
    Allocates the least available connection index

    @return
    E_SUCCESS upon allocation of Connection index
    E_FAILURE if there is no Connection Index available
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cm_util_allocate_conn_id
(
  uint8* conn_id
)
{
  IxErrnoType status = E_FAILURE;
  uint8 i;

  ASSERT(conn_id != NULL);

  for(i=0;i<CM_CALL_ID_MAX;i++)
  {
    if(util_info.available_conn_ids[i])
    {
      *conn_id = i+1;
      /* This conn id is no longer available until deallocation */
      util_info.available_conn_ids[i] = FALSE;
      status = E_SUCCESS;
      break;
    }
  }

  return status;
}/* qmi_voice_cm_util_allocate_conn_id() */



/*=========================================================================
  FUNCTION:  qmi_voice_cm_util_deallocate_conn_id

===========================================================================*/
/*!
    @brief
    Frees the connection index

    @return
    E_SUCCESS upon de-allocation of Connection index
    E_FAILURE otherwise
*/
/*=========================================================================*/
IxErrnoType qmi_voice_cm_util_deallocate_conn_id
(
  uint8 conn_id
)
{

  if( (conn_id == QMI_VOICE_CM_UTIL_CONN_ID_INVALID) || 
      (conn_id > CM_CALL_ID_MAX) )
  {
    return E_FAILURE;
  }

  /* Free the connection index */
  util_info.available_conn_ids[conn_id - 1] = TRUE;

  return E_SUCCESS;
} /* qmi_voice_cm_util_deallocate_conn_id() */


/*=========================================================================
  FUNCTION:  qmi_voice_cm_util_get_ussd_dcs

===========================================================================*/
/*!
    @brief
    Retrieve the Data coding scheme from the network sent values.

    @return
    Length of UTF8 string.

*/
/*=========================================================================*/
cm_ussd_dcs_e_type qmi_voice_cm_util_get_ussd_dcs
(
  byte uss_data_coding_scheme
)
{
  byte hi_dcs, lo_dcs;

  hi_dcs = ( ( uss_data_coding_scheme & 0xF0 ) >> 4 );
  lo_dcs = ( uss_data_coding_scheme & 0x0F );

  /* Refer to 3GPP TS 23.038 V6.1.0 , Section 5 : CBS Data Coding Scheme */
  /* GSM 7-bit alphabet */
  QM_MSG_MED_1("USSD DCS recieved = %d", uss_data_coding_scheme);
  
  if ( ( hi_dcs == 0x00 ) || 
       ( ( hi_dcs == 0x01 ) && ( lo_dcs == QMI_VOICE_CM_SUPS_USSD_DCS_7_BIT_MASK ) ) ||
       ( hi_dcs == 0x02 ) || 
       ( hi_dcs == 0x03 ) || 
       ( ( hi_dcs & 0x04 ) && ( ( lo_dcs & 0x0C ) == QMI_VOICE_CM_SUPS_USSD_DCS_7_BIT_MASK ) ) ||
       ( ( hi_dcs == 0x09 ) && ( ( lo_dcs & 0x0C ) == QMI_VOICE_CM_SUPS_USSD_DCS_7_BIT_MASK ) ) ||
       ( ( hi_dcs == 0x0F ) && ( ( lo_dcs & 0x04 ) == QMI_VOICE_CM_SUPS_USSD_DCS_7_BIT_MASK ) ) )
  { 
    /* Convert from GSM 7-bit alphabet to UTF8 */
     QM_MSG_MED_2("USSD DCS 7-bit str, hi=%d lo=%d", hi_dcs, lo_dcs);
     return (CM_USSD_DCS_7_BIT);
  }
  /* 8-bit */
  else if ( ( ( hi_dcs & 0x04 ) && ( ( lo_dcs & 0x0C ) == QMI_VOICE_CM_SUPS_USSD_DCS_8_BIT_MASK ) ) ||
            ( ( hi_dcs == 0x09 ) && ( ( lo_dcs & 0x0C ) == QMI_VOICE_CM_SUPS_USSD_DCS_8_BIT_MASK ) ) ||
            ( ( hi_dcs == 0x0F ) && ( ( lo_dcs & 0x04 ) == QMI_VOICE_CM_SUPS_USSD_DCS_8_BIT_MASK ) ) )
  {
      /* Convert from GSM 8-bit alphabet to UTF8 */
    QM_MSG_MED_2("USSD DCS 8-bit str, hi=%d lo=%d", hi_dcs, lo_dcs);
    return (CM_USSD_DCS_8_BIT);

  }
  /* UCS2 preceded by two GSM 7-bit alphabet */
  else if ( ( hi_dcs == 0x01 ) && ( lo_dcs == QMI_VOICE_CM_SUPS_USSD_DCS_UCS2_LANG_IND_MASK ) )  
  {
    QM_MSG_MED_2("USSD DCS DCS2 str, hi=%d lo=%d", hi_dcs, lo_dcs);
    return (CM_USSD_DCS_UCS2);

  }
  /* UCS2 */
  else if ( ( ( hi_dcs & 0x04 ) && ( ( lo_dcs & 0x0C ) == QMI_VOICE_CM_SUPS_USSD_DCS_UCS2_MASK ) ) || 
            ( ( hi_dcs == 0x09 ) && ( ( lo_dcs & 0x0C ) == QMI_VOICE_CM_SUPS_USSD_DCS_UCS2_MASK ) ) )
  {
    QM_MSG_MED_2("USSD DCS DCS2 str, hi=%d lo=%d", hi_dcs, lo_dcs);
    return (CM_USSD_DCS_UCS2); 
  }
  else
  {
    QM_MSG_MED_2("USSD DCS Unknown str format, hi=%d lo=%d", hi_dcs, lo_dcs);
    return (CM_USSD_DCS_UNSPECIFIED); 
  }
} /* qmi_voice_cm_util_get_ussd_dcs */

/*=========================================================================
  FUNCTION:  qmi_voice_cm_util_get_num_info_from_bcd_num

===========================================================================*/
/*!
    @brief
    Convert the BCD Number into (cm_num_s_type) number.

    @return
    none

*/
/*=========================================================================*/

void qmi_voice_cm_util_get_num_info_from_bcd_num
(
  cm_num_s_type *num,
  cm_called_party_bcd_no_s_type *bcd_num
)
{
  byte                    ton_npi;
  byte                    calling_party_num[CM_CALLED_PARTY_BCD_WITH_BCD_LEN];
  byte                    ascii_num [CM_MAX_NUMBER_CHARS + 1];
  boolean                 presentation_ind_absent = TRUE;
  byte                    pres_scren_ind = 0;
  byte                    presentation_indicator = 0; /* presentation allowed */
  byte                    screening_indicator = 0;    /* user provided, not screened */

  ASSERT(num != NULL); 
  ASSERT(bcd_num != NULL); 

  memset( calling_party_num, 0, CM_CALLED_PARTY_BCD_WITH_BCD_LEN );
  memset( ascii_num, 0, sizeof(ascii_num) );

  QM_MSG_MED_2("Recieved BCD number present = %d, length = %d", bcd_num->present, bcd_num->length);

  cm_num_init(num);

  if(bcd_num->present)
  {
    ton_npi = bcd_num->data[0];
  
    num->number_plan = (byte)(ton_npi & QMI_VOICE_CM_UTIL_NUM_PLAN_MASK);
    num->number_type = (byte)((ton_npi & QMI_VOICE_CM_UTIL_TON_MASK) >> 4);
  
  
    /* look for the presentation indicator according to 24.008 Calling party BCD number definition*/
    presentation_ind_absent = (boolean)((ton_npi & QMI_VOICE_CM_UTIL_CM_EXTENSION_BIT) >> 7);
  
    if ( !presentation_ind_absent )
    {
      /* check extra byte octet 3a */
      pres_scren_ind = bcd_num->data[1];
      presentation_indicator = (byte)((pres_scren_ind & QMI_VOICE_CM_UTIL_PRESENTATION_IND_MASK) >> 5);
      screening_indicator = (byte) (pres_scren_ind & QMI_VOICE_CM_UTIL_SCREENING_IND_MASK);
    }
  
    calling_party_num[0] = bcd_num->length;
  
    if ( calling_party_num[0] > CM_CALLED_PARTY_BCD_NO_LENGTH)
    {
      calling_party_num[0] = CM_CALLED_PARTY_BCD_NO_LENGTH ;
    }
  
    memscpy(&calling_party_num[1], sizeof(calling_party_num) - 1 , 
            bcd_num->data, calling_party_num[0]);
  
    /* ascii_num is nul-terminated by CM */
    num->len = cm_util_bcd_num_to_ascii_num(calling_party_num, ascii_num, sizeof(ascii_num));
  
    if ( num->len > 0 )
    {
      /* Gaurd check to see if ascii chars returned by CM is within the range */
      num->len = MIN(num->len, (sizeof(ascii_num)-1));
      memscpy(num->buf, sizeof(num->buf), ascii_num, num->len);
    }
    num->pi = presentation_indicator;
    num->si = screening_indicator;  
    QM_MSG_MED_7("converted pres_scren_ind=%d, pi=%d, si=%d, num len=%d, num=%s, type=%d, num plan=%d",
                 pres_scren_ind, num->pi, num->si, num->len,
                 (char *)num->buf, num->number_type, num->number_plan);
  }
  else
  {
    QM_MSG_ERROR_1("Unable to convert BCD number with present = %d", bcd_num->present);
  }
}/*qmi_voice_cm_util_get_num_info_from_bcd_num*/

/*=========================================================================
  FUNCTION:  qmi_voice_cm_util_get_ss_info

===========================================================================*/
/*!
    @brief
    Fetch the Srv sys info.

    @return
    TRUE if valid ss info present, FALSE otherwise

*/
/*=========================================================================*/
boolean qmi_voice_cm_util_get_ss_info
(
  qmi_voice_cm_util_ss_info_s  *ss_info,
  uint8 asubs_id
)
{
  boolean ret = FALSE;
  ASSERT(ss_info);
  
  if(asubs_id < QMI_VOICE_CM_IF_AS_ID_PRIMARY || asubs_id >= QMI_VOICE_CM_IF_AS_ID_MAX)
  {
    QM_MSG_MED_1("Invalid asubs_id %d", asubs_id);
    return ret;
  }

  QMI_MMODE_ENTER_CRIT_SECTION(&util_info.ss_cs);

  if(util_info.is_ss_info_rcvd)
  {
    //return info from main stack
    ss_info->sys_mode = qmi_voice_cm_util_map_qmi_to_cm_sys_mode(util_info.ss_info[asubs_id].sys_mode[0]) ;
    ss_info->srv_status = util_info.ss_info[asubs_id].srv_status[0];

    ret = TRUE;
  }

  QMI_MMODE_LEAVE_CRIT_SECTION(&util_info.ss_cs);
  QM_MSG_MED_3("Get SS Info: subs_id %d, sys_mode %d, srv_status %d",
               asubs_id, ss_info->sys_mode, ss_info->srv_status);
  return ret;

}/* qmi_voice_cm_util_get_ss_info */


/*===========================================================================

FUNCTION qmi_voice_util_map_efs_id_to_filename

DESCRIPTION
 Maps efs id to filename

DEPENDENCIES
  None

RETURN VALUE
  File path name

SIDE EFFECTS
  None.

===========================================================================*/

const char *   qmi_voice_util_map_efs_id_to_filename(
        qmi_voice_efs_id_e_type          efs_id
            /* passing the efs Id to map  */
)
{
   static const struct {
       qmi_voice_efs_id_e_type    efs_id;
       const char            *file_name;
           /* EFS file name  */
  } qmi_voice_util_efs_id_to_path_mapping_tbl[] = {

     { QMI_VOICE_DOMAIN_PREF_F_NAME, "/nv/item_files/modem/mmode/voice_domain_pref" },
     { QMI_VOICE_UI_TTY_SETTING_PRI_F_NAME, "/nv/item_files/modem/mmode/qmi/ui_tty_setting_pri" },
  };

  if(!((efs_id > QMI_VOICE_EFS_ID_NONE) && (efs_id < QMI_VOICE_EFS_ID_MAX)))
  {
    QM_MSG_ERROR_1("write:Bad efs_id = %d in qmi_voice_util_map_efs_id_to_filename", efs_id);
    return NULL;
  }

  return qmi_voice_util_efs_id_to_path_mapping_tbl[efs_id].file_name;
}


/*===========================================================================

FUNCTION qmi_voice_cm_util_efs_write

DESCRIPTION
  Write contents of data_ptr to EFS item denoted by efs_id

DEPENDENCIES
  None

RETURN VALUE
 boolean : 1 = Read successful
           0 = Error in Read

SIDE EFFECTS
  None.

===========================================================================*/

boolean qmi_voice_cm_util_efs_write(

qmi_voice_efs_id_e_type          efs_id,
            /* passing the efs Id to write.       */
const byte                       *data_ptr,
            /* Pointer to a data buffer to write to EFS.      */
int32                            size
            /* Size of the data_ptr buffer.            */
)
{
  return qmi_voice_cm_util_efs_write_per_subs(efs_id, data_ptr, size, QMI_VOICE_CM_IF_AS_ID_PRIMARY);
}

/*===========================================================================

FUNCTION qmi_voice_cm_util_efs_write_per_subs

DESCRIPTION
  Write contents of data_ptr to EFS item denoted by efs_id for given as_id

DEPENDENCIES
  None

RETURN VALUE
 boolean : 1 = Read successful
           0 = Error in Read

SIDE EFFECTS
  None.

===========================================================================*/

boolean qmi_voice_cm_util_efs_write_per_subs(

qmi_voice_efs_id_e_type          efs_id,
            /* passing the efs Id to write.       */
const byte                       *data_ptr,
            /* Pointer to a data buffer to write to EFS.      */
int32                            size,
            /* Size of the data_ptr buffer.            */
qmi_voice_cm_if_as_id_e_type     as_id
            /* subscription to write EFS file to */
)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_OK;
  const char     *file_name = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( data_ptr != NULL );
  ASSERT( size     != 0 );

  if(!((efs_id > QMI_VOICE_EFS_ID_NONE) && (efs_id < QMI_VOICE_EFS_ID_MAX)))
  {
    QM_MSG_ERROR_1("write:Bad efs_id = %d", efs_id);
    return FALSE;
  }

  /* Map efs_id to filename  */
  file_name = qmi_voice_util_map_efs_id_to_filename(efs_id);

  if(file_name == NULL)
  {
    QM_MSG_ERROR("efs write fail in qmi_voice_cm_util_efs_write file name not found");
    return FALSE;
  }
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Open the specified resource file. If open fails, return error code  */
  QM_MSG_MED_1( "Put file=%s",file_name);

  status = mcfg_fs_write( file_name, (void*)data_ptr, (int)size, MCFG_FS_O_CREAT | MCFG_FS_O_AUTODIR, MCFG_FS_ALLPERMS, MCFG_FS_TYPE_EFS, qmi_voice_cm_util_map_as_id_to_mcfg_id(as_id) );

  if ( status != MCFG_FS_STATUS_OK )
  {
    QM_MSG_ERROR_1("efs write fail status %d in qmi_voice_cm_util_efs_write", status);
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION qmi_voice_cm_util_efs_read

DESCRIPTION
  read contents to data_ptr from EFS item denoted by efs_id

DEPENDENCIES
  None

RETURN VALUE
 boolean : 1 = Read successful
           0 = Error in Read

SIDE EFFECTS
  None.

===========================================================================*/

boolean qmi_voice_cm_util_efs_read(

qmi_voice_efs_id_e_type          efs_id,
/* passing the efs Id to write.*/
byte                             *data_ptr,
/* Pointer to a data buffer to store EFS data.*/
int32                            size
/* Size of the data_ptr buffer.*/
)
{
  return qmi_voice_cm_util_efs_read_per_subs(efs_id, data_ptr, size, QMI_VOICE_CM_IF_AS_ID_PRIMARY);
}

/*===========================================================================

FUNCTION qmi_voice_cm_util_efs_read_per_subs

DESCRIPTION
  read contents to data_ptr from EFS item denoted by efs_id for given as_id

DEPENDENCIES
  None

RETURN VALUE
 boolean : 1 = Read successful
           0 = Error in Read

SIDE EFFECTS
  None.

===========================================================================*/
boolean qmi_voice_cm_util_efs_read_per_subs(

qmi_voice_efs_id_e_type          efs_id,
/* passing the efs Id to write.*/
byte                             *data_ptr,
/* Pointer to a data buffer to store EFS data.*/
int32                            size,
/* Size of the data_ptr buffer.*/
qmi_voice_cm_if_as_id_e_type     as_id
/* subscription to read EFS file from */
)
{
  mcfg_fs_status_e_type status = MCFG_FS_STATUS_OK;
  const char           *file_name = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( data_ptr != NULL );

  if(!((efs_id > QMI_VOICE_EFS_ID_NONE) && (efs_id < QMI_VOICE_EFS_ID_MAX)))
  {
    QM_MSG_ERROR_1("write:Bad efs_id = %d", efs_id);
    return FALSE;
  }

  /* Map efs_id to filename  */
  file_name = qmi_voice_util_map_efs_id_to_filename(efs_id);

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Open the specified resource file. If open fails, return error code  */
  QM_MSG_MED_1( "Get file=%s",file_name);

  status = mcfg_fs_read( file_name, data_ptr, size, MCFG_FS_TYPE_EFS, qmi_voice_cm_util_map_as_id_to_mcfg_id(as_id) );

  if ( status != MCFG_FS_STATUS_OK )
  {
    QM_MSG_ERROR_1("efs read error: status %d in qmi_voice_cm_util_efs_get", status);
    return FALSE;
  } 

  return TRUE;
}

/*===========================================================================

FUNCTION qmi_voice_cm_util_map_as_id_to_mcfg_id

DESCRIPTION
  Map modem asubs_id to mcfg id

DEPENDENCIES
  None

RETURN VALUE
  mcfg id

SIDE EFFECTS
  None

===========================================================================*/
mcfg_fs_sub_id_e_type qmi_voice_cm_util_map_as_id_to_mcfg_id(
  qmi_voice_cm_if_as_id_e_type    as_id
)
{
  mcfg_fs_sub_id_e_type  mcfg_subs;

  switch (as_id)
  {
    case QMI_VOICE_CM_IF_AS_ID_TERTIARY:
      mcfg_subs = MCFG_FS_SUBID_2;
      break;

    case QMI_VOICE_CM_IF_AS_ID_SECONDARY:
      mcfg_subs = MCFG_FS_SUBID_1;
      break;

    case QMI_VOICE_CM_IF_AS_ID_PRIMARY:
    default:
      mcfg_subs = MCFG_FS_SUBID_0;
      break;
  }

  return mcfg_subs;
}

/*=========================================================================
  FUNCTION:  qmi_voice_cm_util_set_ss_info

===========================================================================*/
/*!
    @brief
    Store the Srv sys info.

    @return
    none

*/
/*=========================================================================*/
void qmi_voice_cm_util_set_ss_info
(
  cm_mm_msim_ss_info_s_type* ss_info_ptr
)
{
  uint8 stack=0;
  ASSERT(ss_info_ptr);

  if ( (ss_info_ptr->asubs_id < SYS_MODEM_AS_ID_1) || (ss_info_ptr->asubs_id > SYS_MODEM_AS_ID_3) )
  {
    QM_MSG_FATAL_1("Invalid asubs_id %d in qmi_voice_cm_util_set_ss_info()", ss_info_ptr->asubs_id);
    return;
  }

  QMI_MMODE_ENTER_CRIT_SECTION(&util_info.ss_cs);
  
  for(stack=0;stack < ss_info_ptr->number_of_stacks;stack++)
    {
    util_info.ss_info[ss_info_ptr->asubs_id].sys_mode[stack] = qmi_voice_cm_util_map_cm_to_qmi_sys_mode(ss_info_ptr->stack_info[stack].sys_mode);
    util_info.ss_info[ss_info_ptr->asubs_id].srv_status[stack] = ss_info_ptr->stack_info[stack].srv_status;
    }
    
  util_info.is_ss_info_rcvd = TRUE;
  QMI_MMODE_LEAVE_CRIT_SECTION(&util_info.ss_cs);

} /* qmi_voice_cm_util_set_ss_info */

/*===========================================================================
FUNCTION qmi_voice_cm_util_number_match

DESCRIPTION
  Does an exact match of a phone number.  Wild card characters match
  any digits.  All searches end at the end of either string, or a pause
  character in the SIM string.  These matches require the dialed string
  does not contain a wild card or pause character - these routines are
  meant to validate a string which was given to call manager to see if
  the call is allowed.  If fdn_comparison is TRUE, we do a starts with
  match (extra digits are allowed).

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
boolean qmi_voice_cm_util_number_match(const byte *sim_num,    byte sim_num_len,
                             const byte *dialed_num, byte dialed_num_len)
{
   int index;                    /* Control variable for loop. */
   boolean ret_val = TRUE;       /* Current return value. */

   /* Validate the parameters. */
   if (sim_num_len && (!sim_num || !dialed_num))
      return FALSE;

   /* For each character, compare the sim_num to the dialed_num. */
   for (index = 0; index < sim_num_len; index++)
   {
      if (sim_num && dialed_num)
      {
         /* TIME and HARDPAUSE characters will end the sim string */
         if ((sim_num[index] == QMI_VOICE_TIMEPAUSE_CHAR) ||
             (sim_num[index] == QMI_VOICE_HARDPAUSE_CHAR))
         {
            /* Pause characters make our searches stop there. */
            break;
         }

         /* We have more sim digits, but if we are out of dialed digits,
                 * then we return that it is not a match. */
         if (dialed_num_len <= index)
         {
            ret_val = FALSE;
            break;
         }

         /* We have sim digits and dialed digits, if the SIM is a wildcard,
                 * don't worry about the dialed character.. It matches wildcards. */
         if (sim_num[index] == QMI_VOICE_WILD_CHAR)
            continue;

         /* If we reach here, the digits must be compared. */
         if (sim_num[index] != dialed_num[index])
         {
            ret_val = FALSE;
            break;
         }
      } 
	  else
      {
         QM_MSG_ERROR_2("sim_num or dialed num NULL, sim_num 0x%x, dialed_num 0x%x", sim_num, dialed_num);
      }
   }
   
   return ret_val;
}

/*===========================================================================
FUNCTION qmi_wstrlen;

DESCRIPTION
  int qmi_wstrlen(const uint16 *ucs2_str);

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
uint32 qmi_wstrlen(const uint16 *ucs2_str)
{
   uint32 retval = 0;                  /* Value to be returned. */
   if (ucs2_str)
   {
      while (*ucs2_str++)
      {
         retval++;
      } /* Increment retval until 0x0000 */
   }
   return retval;                      /* Return what we counted. */
}

/*===========================================================================
FUNCTION qmi_voice_cm_util_copy_sups_failure_string;

DESCRIPTION
 To copy the utf16 sups string from CM.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void qmi_voice_cm_util_copy_sups_failure_string(voice_set_sups_service_resp_msg_v02 *qmi_struct, cm_ip_sups_error_s_type *cm_struct)
{

   if((cm_struct->err_text_length > 0) && 
      (cm_struct->err_text != NULL))
   {
     qmi_struct->failure_cause_description_valid = TRUE;
     qmi_struct->failure_cause_description_len = (uint16) cm_struct->err_text_length;
     memscpy(qmi_struct->failure_cause_description, sizeof(qmi_struct->failure_cause_description), cm_struct->err_text, 2*(cm_struct->err_text_length));
     QM_MSG_MED_1("Sups failure_description copied; len=%d", cm_struct->err_text_length);
   }
   else
   {
     QM_MSG_MED_1("Sups failure_description not copied; len=%d", cm_struct->err_text_length);
   }
}

/*===========================================================================
  FUNCTION QMI_VOICE_CM_UTIL_MAP_CM_TO_QMI_VS_VERSION()

  DESCRIPTION
    Map the CM VS verstion value to QMI VS version enum value

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_voice_cm_if_vs_version_e_type qmi_voice_cm_util_map_cm_to_qmi_vs_version
(
  cm_ip_vs_version_e_type version
)
{
  qmi_voice_cm_if_vs_version_e_type qmi_version = QMI_VOICE_CM_IF_VS_VERSION_NONE;

  QM_MSG_MED_1("Inside qmi_voice_cm_util_map_cm_to_qmi_vs_version for cm_vs_version=%d", version);

  switch(version)
  {
    case CM_IP_VS_RCSE:
      qmi_version = QMI_VOICE_CM_IF_VS_VERSION_RCS_E;
      break;
    case CM_IP_VS_RCSV5:
      qmi_version = QMI_VOICE_CM_IF_VS_VERSION_RCS_V5;
      break;
    default:
      break;
  }
  return qmi_version;
} /* qmi_voice_cm_util_map_cm_to_qmi_vs_version() */

/*===========================================================================
  FUNCTION QMI_VOICE_CM_UTIL_MAP_CM_TO_QMI_CALL_STATE()

  DESCRIPTION
    Map the CM call state and sub state to QMI call state value

  PARAMETERS
    Cm call state
    CM call sub state

  RETURN VALUE
    qmi call state

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_voice_call_list_state_e_type qmi_voice_cm_util_map_cm_to_qmi_call_state
(
  cm_call_state_e_type cm_state,
  cm_call_substate_type cm_substate
)
{
  qmi_voice_call_list_state_e_type qmi_state = QMI_VOICE_CALL_LIST_STATE_IDLE;

  QM_MSG_MED_3("Inside map_cm_to_qmi_call_state for cm_state=%d, Sub state Incom(%d), Conv(%d)", cm_state, cm_substate.incom, cm_substate.conv);

  switch(cm_state)
  {
    case CM_CALL_STATE_IDLE:
      qmi_state = QMI_VOICE_CALL_LIST_STATE_IDLE;
      break;
    case CM_CALL_STATE_ORIG:
      if(cm_substate.orig == CM_CALL_ORIG_SUBST_PRE_ALERTING)
      {
        qmi_state = QMI_VOICE_CALL_LIST_STATE_DIALING;
      }
      else if(cm_substate.orig == CM_CALL_ORIG_SUBST_ALERTING)
      {
        qmi_state = QMI_VOICE_CALL_LIST_STATE_ALERTING;
      }
      break;
    case CM_CALL_STATE_INCOM:
      qmi_state = QMI_VOICE_CALL_LIST_STATE_INCOMING;
      break;
    case CM_CALL_STATE_CONV:
      if(cm_substate.conv == CM_CALL_CONV_CALL_ACTIVE ||
         cm_substate.conv == CM_CALL_CONV_SUBST_REGULAR ||
         cm_substate.conv == CM_CALL_CONV_HOLD_REQ_BY_UE)
      {
        qmi_state = QMI_VOICE_CALL_LIST_STATE_ACTIVE;
      }
      else if(cm_substate.conv == CM_CALL_CONV_CALL_ON_HOLD || 
              cm_substate.conv == CM_CALL_CONV_RETRIEVE_REQ_BY_UE)
      {
        qmi_state = QMI_VOICE_CALL_LIST_STATE_HOLDING;
      }
      break;
    case CM_CALL_STATE_CC_IN_PROGRESS:
      qmi_state = QMI_VOICE_CALL_LIST_STATE_CC_IN_PROG;
      break;
    default:
      QM_MSG_ERROR("Unexpected cm call state");
      break;
  }
  return qmi_state;
} /* qmi_voice_cm_util_map_cm_to_qmi_call_state() */

/*===========================================================================
  FUNCTION QMI_VOICE_CM_UTIL_MAP_QMI_TO_CM_VS_VERSION()

  DESCRIPTION
    Map the QMI VS verstion value to CM VS version enum value

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
cm_ip_vs_version_e_type qmi_voice_cm_util_map_qmi_to_cm_vs_version
(
  qmi_voice_cm_if_vs_version_e_type  version
)
{
   cm_ip_vs_version_e_type cm_version = CM_IP_VS_RCSE;

  QM_MSG_MED_1("Inside qmi_voice_cm_util_map_qmi_to_cm_vs_version for qmi_vs_version=%d", version);

  switch(version)
  {
    case QMI_VOICE_CM_IF_VS_VERSION_RCS_E:
      cm_version = CM_IP_VS_RCSE;
      break;
    case QMI_VOICE_CM_IF_VS_VERSION_RCS_V5:
      cm_version = CM_IP_VS_RCSV5;
      break;
    default:
      break;
  }
  return cm_version;
} /* qmi_voice_cm_util_map_cm_to_qmi_vs_version() */

/*===========================================================================
  FUNCTION QMI_VOICE_CM_UTIL_MAP_CALL_STATE()

  DESCRIPTION
    Get the call state to be sent to control point

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

qmi_voice_cm_if_call_state_e_type qmi_voice_cm_util_map_call_state
(
  qmi_voice_call_list_state_e_type state
)
{
  qmi_voice_cm_if_call_state_e_type call_state = QMI_VOICE_CM_IF_CALL_STATE_IDLE;

  
  QM_MSG_MED_2("%s for qmi state=%d","Inside qmi_voice_cm_util_map_call_state():",call_state);

  switch(state)
  {
    case QMI_VOICE_CALL_LIST_STATE_ACTIVE:
      call_state = QMI_VOICE_CM_IF_CALL_STATE_CONV;
      break;
    case QMI_VOICE_CALL_LIST_STATE_ALERTING:
      call_state = QMI_VOICE_CM_IF_CALL_STATE_ALERTING;
      break;
    case QMI_VOICE_CALL_LIST_STATE_SETUP:
      call_state = QMI_VOICE_CM_IF_CALL_STATE_SETUP;
      break;
    case QMI_VOICE_CALL_LIST_STATE_CC_IN_PROG:
      call_state = QMI_VOICE_CM_IF_CALL_STATE_CC_IN_PROG;
      break;
    case QMI_VOICE_CALL_LIST_STATE_DIALING:
      call_state = QMI_VOICE_CM_IF_CALL_STATE_ORIG;
      break;
    case QMI_VOICE_CALL_LIST_STATE_HOLDING:
      call_state = QMI_VOICE_CM_IF_CALL_STATE_HOLD;
      break;
    case QMI_VOICE_CALL_LIST_STATE_INCOMING:
      call_state = QMI_VOICE_CM_IF_CALL_STATE_INCOMING;
      break;
    case QMI_VOICE_CALL_LIST_STATE_WAITING:
      call_state = QMI_VOICE_CM_IF_CALL_STATE_WAITING;
      break;
    case QMI_VOICE_CALL_LIST_STATE_DISCONNECTING:
      call_state = QMI_VOICE_CM_IF_CALL_STATE_DISCONNECTING;
      break;
    case QMI_VOICE_CALL_LIST_STATE_END:
      call_state = QMI_VOICE_CM_IF_CALL_STATE_END;
      break;
    default:
      break;
  }
  return call_state;
} /* qmi_voice_cm_util_map_call_state() */

/*===========================================================================
  FUNCTION QMI_VOICE_CM_UTIL_MAP_CM_TO_QMI_CALL_TYPE()

  DESCRIPTION
    Map the CM call type to QMI call type

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_voice_cm_if_call_type_e_type qmi_voice_cm_util_map_cm_to_qmi_call_type
(
  cm_call_type_e_type type,
  cm_call_mode_info_e_type mode_info_type
)
{
  qmi_voice_cm_if_call_type_e_type call_type = QMI_VOICE_CM_IF_CALL_TYPE_VOICE;

  QM_MSG_MED_1("Inside qmi_voice_cm_util_map_cm_to_qmi_call_type for cm_call_type=%d", type);

  switch(type)
  {
    case CM_CALL_TYPE_VOICE:
      if(mode_info_type == CM_CALL_MODE_INFO_IP)
      {
        call_type = QMI_VOICE_CM_IF_CALL_TYPE_VOIP;
      }
      else
      {
        call_type = QMI_VOICE_CM_IF_CALL_TYPE_VOICE;
      }
      break;
    case CM_CALL_TYPE_CS_DATA:
      call_type = QMI_VOICE_CM_IF_CALL_TYPE_FAX;
      break;
    case CM_CALL_TYPE_VT:
      call_type = QMI_VOICE_CM_IF_CALL_TYPE_VT;
      break;
    case CM_CALL_TYPE_VS:
      call_type = QMI_VOICE_CM_IF_CALL_TYPE_VIDEOSHARE;
      break;
    case CM_CALL_TYPE_OTAPA:
      call_type = QMI_VOICE_CM_IF_CALL_TYPE_OTAPA;
      break;
    case CM_CALL_TYPE_STD_OTASP:
      call_type = QMI_VOICE_CM_IF_CALL_TYPE_STD_OTASP;
      break;
    case CM_CALL_TYPE_NON_STD_OTASP:
      call_type = QMI_VOICE_CM_IF_CALL_TYPE_NON_STD_OTASP;
      break;
    case CM_CALL_TYPE_EMERGENCY:
      if(mode_info_type == CM_CALL_MODE_INFO_IP)
      {
        call_type = QMI_VOICE_CM_IF_CALL_TYPE_EMERGENCY_IP;
      }
      else
      {
        call_type = QMI_VOICE_CM_IF_CALL_TYPE_EMERGENCY;
      }
      break;
    case CM_CALL_TYPE_SUPS:
      call_type = QMI_VOICE_CM_IF_CALL_TYPE_SUPS;
      break;
    case CM_CALL_TYPE_TEST:
      call_type = QMI_VOICE_CM_IF_CALL_TYPE_TEST;
      break;
    default:
      break;
  }
  return call_type;
} /* qmi_voice_cm_util_map_cm_to_qmi_call_type() */


/*===========================================================================
  FUNCTION QMI_VOICE_CM_UTIL_MAP_SYS_MODE_TO_CALL_MODE()

  DESCRIPTION
    Get the call mode to be sent to control point

  PARAMETERS
    mode reported by CM

  RETURN VALUE
    call_mode to be sent to control points

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

uint8 qmi_voice_cm_util_map_sys_mode_to_call_mode
(
  sys_sys_mode_e_type mode
)
{
  uint8 call_mode = QMI_VOICE_CM_IF_MODE_UNKNOWN;  

  switch(mode)
  {
    case SYS_SYS_MODE_CDMA:
      call_mode = QMI_VOICE_CM_IF_MODE_CDMA ;
      break;
    case SYS_SYS_MODE_WCDMA:
      call_mode = QMI_VOICE_CM_IF_MODE_UMTS ;
      break;
    case SYS_SYS_MODE_LTE:
      call_mode = QMI_VOICE_CM_IF_MODE_LTE;
      break;
    case SYS_SYS_MODE_GSM:
      call_mode = QMI_VOICE_CM_IF_MODE_GSM;
      break;
#ifdef FEATURE_TDSCDMA
    case SYS_SYS_MODE_TDS:
      call_mode = QMI_VOICE_CM_IF_MODE_TDS;
      break;
#endif /*FEATURE_TDSCDMA*/
    case SYS_SYS_MODE_NO_SRV:
      call_mode = QMI_VOICE_CM_IF_MODE_NO_SRV;
      break;
    case SYS_SYS_MODE_WLAN:
      call_mode = QMI_VOICE_CM_IF_MODE_WLAN;
      break;
    case SYS_SYS_MODE_GWL:
    case SYS_SYS_MODE_GW:
      call_mode = QMI_VOICE_CM_IF_MODE_UMTS;
      break;
    default:
      break;
  }
  QM_MSG_MED_3("%s, SYS Mode=%d Call_Mode=%d","Inside qmi_voice_cm_util_map_sys_mode_to_call_mode():",mode,call_mode);
  return call_mode;
} /* qmi_voice_cm_util_map_sys_mode_to_call_mode() */

/*===========================================================================
  FUNCTION QMI_VOICE_CM_UTIL_MAP_CM_TO_QMI_HO_TYPE()

  DESCRIPTION
    Map the CM Handover type to QMI Handover type

  PARAMETERS
    CM Handover type, QMI Handover type pointer

  RETURN VALUE
    TRUE: Success
    FALSE: Fail

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

boolean qmi_voice_cm_util_map_cm_to_qmi_ho_type
(
  sys_voice_handover_e_type cm_ho_type,
  voice_handover_type_enum_v02 *qmi_ho_type_ptr
)
{
  boolean ret = TRUE;

  QM_MSG_MED_1("Inside qmi_voice_cm_util_map_cm_to_qmi_ho_type for cm_ho_type=%d", cm_ho_type);

  switch(cm_ho_type)
  {
    case SYS_VOICE_HO_G_2_G:
      *qmi_ho_type_ptr = VOICE_HO_G_2_G_V02;
      break;

    case SYS_VOICE_HO_G_2_W:
      *qmi_ho_type_ptr = VOICE_HO_G_2_W_V02;
      break;

    case SYS_VOICE_HO_W_2_W:
      *qmi_ho_type_ptr = VOICE_HO_W_2_W_V02;
      break;

    case SYS_VOICE_HO_W_2_G:
      *qmi_ho_type_ptr = VOICE_HO_W_2_G_V02;
      break;

    case SYS_VOICE_HO_SRVCC_L_2_G:
      *qmi_ho_type_ptr = VOICE_HO_SRVCC_L_2_G_V02;
      break;

    case SYS_VOICE_HO_SRVCC_L_2_W:
      *qmi_ho_type_ptr = VOICE_HO_SRVCC_L_2_W_V02;
      break;

    case SYS_VOICE_HO_DRVCC_WIFI_2_C:
      *qmi_ho_type_ptr = VOICE_HO_DRVCC_WIFI_2_C_V02;
      break;

    case SYS_VOICE_HO_DRVCC_WIFI_2_GW:
      *qmi_ho_type_ptr = VOICE_HO_DRVCC_WIFI_2_GW_V02;
      break;

    default:
      QM_MSG_ERROR("Unmapped CM HO type");
      ret = FALSE;
      break;
  }

  return ret;

}

/*===========================================================================
  FUNCTION qmi_voice_util_map_qmi_to_cm_call_state_substate()

  DESCRIPTION
    Get the call mode to be sent to control point

  PARAMETERS
    mode reported by CM

  RETURN VALUE
    call_mode to be sent to control points

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/

void qmi_voice_util_map_qmi_to_cm_call_state_substate
(
  cm_call_state_e_type *cm_state,
  cm_call_substate_type *cm_sub_state,
  qmi_voice_cm_if_call_state_e_type qmi_state,
  qmi_voice_cm_if_call_substate_e_type qmi_sub_state
)
{
  *cm_state = CM_CALL_STATE_NONE;
  memset(cm_sub_state,0,sizeof(cm_call_substate_type));

  switch(qmi_state)
  {
    case QMI_VOICE_CM_IF_CALL_STATE_IDLE:
      *cm_state = CM_CALL_STATE_IDLE ;
      break;
    case QMI_VOICE_CM_IF_CALL_STATE_ORIG:
      if(qmi_sub_state == QMI_VOICE_CM_IF_CALL_SUBSTATE_PRE_ALERTING)
      {
        *cm_state = CM_CALL_STATE_ORIG ;
        cm_sub_state->orig = CM_CALL_ORIG_SUBST_PRE_ALERTING;
      }
      break;
    case QMI_VOICE_CM_IF_CALL_STATE_ALERTING:
      *cm_state = CM_CALL_STATE_ORIG ;
      cm_sub_state->orig = CM_CALL_ORIG_SUBST_ALERTING;
      break;
    case QMI_VOICE_CM_IF_CALL_STATE_INCOMING:
    case QMI_VOICE_CM_IF_CALL_STATE_SETUP:
    case QMI_VOICE_CM_IF_CALL_STATE_WAITING:
      *cm_state = CM_CALL_STATE_INCOM ;
      if(qmi_sub_state == QMI_VOICE_CM_IF_CALL_SUBSTATE_INCOM_ANSWERED)
      {
        cm_sub_state->incom = CM_CALL_INCOM_SUBST_ANSWERED;
      }
      else
      {
        cm_sub_state->incom = CM_CALL_INCOM_SUBST_ALERTING;
      }
      break;
    case QMI_VOICE_CM_IF_CALL_STATE_CONV:
      *cm_state = CM_CALL_STATE_CONV;
      if(qmi_sub_state == QMI_VOICE_CM_IF_CALL_SUBSTATE_HOLD_REQ_UE)
      {
        cm_sub_state->conv = CM_CALL_CONV_HOLD_REQ_BY_UE;
      }
      else
      {
        cm_sub_state->conv = CM_CALL_CONV_CALL_ACTIVE;
      }
      break;
    case QMI_VOICE_CM_IF_CALL_STATE_HOLD:
      *cm_state = CM_CALL_STATE_CONV;
      if(qmi_sub_state == QMI_VOICE_CM_IF_CALL_SUBSTATE_RETRIVE_REQ_UE)
      {
        cm_sub_state->conv = CM_CALL_CONV_RETRIEVE_REQ_BY_UE;
      }
      else
      {
        cm_sub_state->conv = CM_CALL_CONV_CALL_ON_HOLD;
      }
      break;
    default:
      QM_MSG_MED_2("Unexpected qmi_call_state=%d sub_state=%d received", qmi_state, qmi_sub_state);
      break;
  }
  QM_MSG_MED_5("qmi_state=%d, cm_state=%d, qmi_substate=%d, cm_substate= %d or %d",
               qmi_state, *cm_state, qmi_sub_state,
               cm_sub_state->conv, cm_sub_state->incom);
} /* qmi_voice_util_map_qmi_to_cm_call_state_substate() */

/*===========================================================================
  FUNCTION qmi_voice_cm_util_map_qmi_to_cm_ip_call_reject_cause()

  DESCRIPTION
    Map the QMI call reject cause to the value that is supported by network

  PARAMETERS
    QMI reject_cause

  RETURN VALUE
    CM reject_cause

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
cm_call_end_e_type qmi_voice_cm_util_map_qmi_to_cm_ip_call_reject_cause
(
  qmi_voice_cm_if_call_reject_cause_e_type  qmi_rej_cause
)
{
  cm_call_end_e_type cm_rej_cause = 0;

  QM_MSG_MED_1("Inside qmi_voice_cm_util_map_qmi_to_cm_call_reject_cause for qmi_rej_causee=%d", qmi_rej_cause);

  switch(qmi_rej_cause)
  {
    case QMI_VOICE_CM_IF_REJ_CAUSE_USER_BUSY:
      cm_rej_cause = CM_CALL_END_SIP_486_BUSY_HERE;
      break;

    case QMI_VOICE_CM_IF_REJ_CAUSE_USER_REJ:
      cm_rej_cause = CM_CALL_END_IP_USER_REJECT;
      break;

    case QMI_VOICE_CM_IF_REJ_CAUSE_LOW_BATTERY:
      cm_rej_cause = CM_CALL_END_IP_LOW_BATTERY;
      break;

    case QMI_VOICE_CM_IF_REJ_CAUSE_DEAD_BATTERY:
      cm_rej_cause = CM_CALL_END_IP_DEAD_BATTERY;
      break;

    case QMI_VOICE_CM_IF_REJ_CAUSE_BLACKLISTED_CALL_ID:
      cm_rej_cause = CM_CALL_END_BLACKLISTED_CALL_ID;
      break;

    default:
      break;
  }
  return cm_rej_cause;
} /* qmi_voice_cm_util_map_qmi_to_cm_ip_call_reject_cause() */


/*===========================================================================
  FUNCTION qmi_voice_cm_util_map_qmi_to_cm_call_reject_cause()

  DESCRIPTION
    Map the QMI cal reject cause to the value that is supported by network

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint32 qmi_voice_cm_util_map_qmi_to_cm_call_reject_cause
(
  qmi_voice_cm_if_call_reject_cause_e_type  qmi_rej_cause
)
{
  qmi_voice_reject_cause_network_e_type cm_rej_cause = REJ_CAUSE_NONE;

  QM_MSG_MED_1("Inside qmi_voice_cm_util_map_qmi_to_cm_call_reject_cause for qmi_rej_cause=%d", qmi_rej_cause);

  switch(qmi_rej_cause)
  {
    case QMI_VOICE_CM_IF_REJ_CAUSE_USER_BUSY:
      cm_rej_cause = REJ_CAUSE_USER_BUSY;
      break;

    case QMI_VOICE_CM_IF_REJ_CAUSE_USER_REJ:
      cm_rej_cause = REJ_CAUSE_USER_REJ;
      break;

    default:
      break;
  }
  return (uint32) cm_rej_cause;
} /* qmi_voice_cm_util_map_qmi_to_cm_call_reject_cause() */

/*===========================================================================
  FUNCTION QMI_VOICE_CM_UTIL_MAP_QMI_TO_CM_ALERTING_TYPE()

  DESCRIPTION
    Map the QMI alerting type tp CM alerting type

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
cm_srvcc_alert_media_type qmi_voice_cm_util_map_qmi_to_cm_alerting_type
(
   uint8 qmi_alerting_type
)
{
  cm_srvcc_alert_media_type cm_alt_type = CM_SRVCC_ALERTING_NONE;
  QM_MSG_MED_1("Inside qmi_voice_cm_util_map_qmi_to_cm_alerting_type for qmi_alt_type=%d", qmi_alerting_type);

  switch(qmi_alerting_type)
  {
    case QMI_VOICE_CM_IF_LOCAL_ALERTING:
      cm_alt_type = CM_SRVCC_ALERTING_NW;
      break;
    case QMI_VOICE_CM_IF_REMOTE_ALERTING:
      cm_alt_type = CM_SRVCC_ALERTING_UE;
      break;
    default:
      break;
  }
  return cm_alt_type;
} /* qmi_voice_cm_util_map_qmi_to_cm_call_type() */


/*===========================================================================
  FUNCTION QMI_VOICE_CM_UTIL_MAP_CM_TO_QMI_ALERTING_TYPE()

  DESCRIPTION
    Map the CM alerting type to QMI alerting type

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint8 qmi_voice_cm_util_map_cm_to_qmi_alerting_type
(
  cm_srvcc_alert_media_type cm_alt_type
)
{
  uint8  qmi_alt_type = QMI_VOICE_CM_IF_ALERTING_NONE;
  QM_MSG_MED_1("Inside qmi_voice_cm_util_map_cm_to_qmi_alerting_type for cm_alt_type=%d", cm_alt_type);

  switch(cm_alt_type)
  {
    case CM_SRVCC_ALERTING_NW:
      qmi_alt_type = QMI_VOICE_CM_IF_LOCAL_ALERTING;
      break;
    case CM_SRVCC_ALERTING_UE:
      qmi_alt_type = QMI_VOICE_CM_IF_REMOTE_ALERTING;
      break;
    default:
      break;
  }
  return qmi_alt_type;
} /* qmi_voice_cm_util_map_qmi_to_cm_call_type() */

/*===========================================================================
  FUNCTION QMI_VOICE_CM_UTIL_MAP_CM_TO_QMI_CALL_TYPE()

  DESCRIPTION
    Map the CM call type to QMI call type

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
cm_call_type_e_type qmi_voice_cm_util_map_qmi_to_cm_call_type
(
   qmi_voice_cm_if_call_type_e_type type
)
{
  cm_call_type_e_type call_type = CM_CALL_TYPE_VOICE;

  
  QM_MSG_MED_1("Inside qmi_voice_cm_util_map_qmi_to_cm_call_type for qmi_call_type=%d", type);

  switch(type)
  {
    case QMI_VOICE_CM_IF_CALL_TYPE_VOICE:
    case QMI_VOICE_CM_IF_CALL_TYPE_VOIP:
      call_type = CM_CALL_TYPE_VOICE;
      break;
    case QMI_VOICE_CM_IF_CALL_TYPE_FAX:
      call_type = CM_CALL_TYPE_CS_DATA;
      break;
    case QMI_VOICE_CM_IF_CALL_TYPE_VT:
      call_type = CM_CALL_TYPE_VT;
      break;
    case QMI_VOICE_CM_IF_CALL_TYPE_VIDEOSHARE:
      call_type = CM_CALL_TYPE_VS;
      break;
    case QMI_VOICE_CM_IF_CALL_TYPE_OTAPA:
      call_type = CM_CALL_TYPE_OTAPA;
      break;
    case QMI_VOICE_CM_IF_CALL_TYPE_STD_OTASP:
      call_type = CM_CALL_TYPE_STD_OTASP;
      break;
    case QMI_VOICE_CM_IF_CALL_TYPE_NON_STD_OTASP:
      call_type = CM_CALL_TYPE_NON_STD_OTASP;
      break;
    case QMI_VOICE_CM_IF_CALL_TYPE_EMERGENCY:
    case QMI_VOICE_CM_IF_CALL_TYPE_EMERGENCY_VT:
      call_type = CM_CALL_TYPE_EMERGENCY;
      break;
    case QMI_VOICE_CM_IF_CALL_TYPE_SUPS:
      call_type = CM_CALL_TYPE_SUPS;
      break;
    default:
      break;
  }
  return call_type;
} /* qmi_voice_cm_util_map_qmi_to_cm_call_type() */

/*===========================================================================
  FUNCTION qmi_voice_cm_util_map_cm_to_qmi_restrict_cause()

  DESCRIPTION
    Map the QMI cal reject cause to the value that is supported by network

  PARAMETERS
    None

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint32 qmi_voice_cm_util_map_cm_to_qmi_restrict_cause
(
  cm_call_restrict_cause_e_type  cm_cause
)
{
  qmi_voice_call_cap_restrict_cause_e_type qmi_cause = QMI_VOICE_CAP_RESTRICT_CAUSE_NONE;

  QM_MSG_MED_1("Inside qmi_voice_cm_util_map_cm_to_qmi_restrict_cause for cm_cause=%d", cm_cause);

  switch(cm_cause)
  {
    case CM_CALL_RESTRICT_CAUSE_NONE:
      qmi_cause = QMI_VOICE_CAP_RESTRICT_CAUSE_NONE;
      break;

    case CM_CALL_RESTRICT_CAUSE_DISABLED:
      qmi_cause = QMI_VOICE_CAP_RESTRICT_CAUSE_DISABLED;
      break;

    case CM_CALL_RESTRICT_CAUSE_RAT:
      qmi_cause = QMI_VOICE_CAP_RESTRICT_CAUSE_RAT;
      break;

    case CM_CALL_RESTRICT_CAUSE_HD:
      qmi_cause = QMI_VOICE_CAP_RESTRICT_CAUSE_HD;
      break;

    default:
      break;
  }
  return (uint32) qmi_cause;
} /* qmi_voice_cm_util_map_cm_to_qmi_restrict_cause() */

/*===========================================================================
  FUNCTION qmi_voice_cm_util_map_cm_to_qmi_ip_sups_failure_cause()

  DESCRIPTION
    CM returns a failure cause for sups operation. 
    This function is to map the CM error to qmi error.

  PARAMETERS
    cm failure cause

  RETURN VALUE
    qmi failure cause

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_voice_cm_if_failure_cause_e_type qmi_voice_cm_util_map_cm_to_qmi_ip_sups_failure_cause
(
  cm_call_end_e_type  cm_fail_cause,
  boolean *is_found
)
{
  qmi_voice_cm_if_failure_cause_e_type qmi_cause = QMI_FAILURE_CAUSE_REL_NORMAL;
  *is_found = TRUE;

  QM_MSG_MED_1("Inside qmi_voice_cm_util_map_cm_to_qmi_ip_sups_failure_cause for cm_fail_cause=%d", cm_fail_cause);

  switch(cm_fail_cause)
  {
    case CM_CALL_END_IP_LOW_BATTERY:
      qmi_cause = QMI_FAILURE_CAUSE_LOW_BATTERY;
      break;
    case CM_CALL_END_SIP_486_BUSY_HERE:
      qmi_cause = QMI_FAILURE_CAUSE_USER_BUSY;
      break;
    case CM_CALL_END_SIP_403_FORBIDDEN:
      qmi_cause = QMI_FAILURE_CAUSE_SIP_403_FORBIDDEN;
      break;
    case CM_CALL_END_IP_UNSUPPORTED_USER_TERMINAL:
      qmi_cause = QMI_FAILURE_CAUSE_INCOMPATIBLE_DESTINATION;
      break;
    case CM_CALL_END_NETWORK_NO_RESP_TIME_OUT:
      qmi_cause = QMI_FAILURE_CAUSE_NETWORK_NO_RESP_TIME_OUT;
      break;
    case CM_CALL_END_INVALID_REMOTE_URI:
      qmi_cause = QMI_FAILURE_CAUSE_INVALID_REMOTE_URI;
      break;
    case CM_CALL_END_IP_USER_REJECT:
      qmi_cause = QMI_FAILURE_CAUSE_CALL_REJECTED;
      break;
    /* Call modification realted*/
    case CM_CALL_END_UPGRADE_DOWNGRADE_REJ:
      qmi_cause = QMI_FAILURE_CAUSE_UPGRADE_DOWNGRADE_REJ;
      break;
    case CM_CALL_END_NO_NETWORK_RESP:
      qmi_cause = QMI_FAILURE_CAUSE_NO_NETWORK_RESP;
      break;
    case CM_CALL_END_UPGRADE_DOWNGRADE_FAILED:
      qmi_cause = QMI_FAILURE_CAUSE_UPGRADE_DOWNGRADE_FAILED;
      break;
    case CM_CALL_END_UPGRADE_DOWNGRADE_CANCELLED:
      qmi_cause = QMI_FAILURE_CAUSE_UPGRADE_DOWNGRADE_CANCELLED;
      break;
    case CM_CALL_END_IP_HOLD_RESUME_FAILED:
      qmi_cause = QMI_FAILURE_CAUSE_HOLD_RESUME_FAILED;
      break;
    case CM_CALL_END_IP_HOLD_RESUME_CANCELED:
      qmi_cause = QMI_FAILURE_CAUSE_HOLD_RESUME_CANCELED;
      break;
    case CM_CALL_END_IP_REINVITE_COLLISION:
      qmi_cause = QMI_FAILURE_CAUSE_REINVITE_COLLISION;
      break;
    default:
      QM_MSG_MED("unhandled failure cause received");
      qmi_cause = QMI_FAILURE_CAUSE_REL_NORMAL;
      *is_found = FALSE;
      break;
  }
  return qmi_cause;
} /* qmi_voice_cm_util_map_cm_to_qmi_ip_sups_failure_cause() */

/*===========================================================================
  FUNCTION qmi_voice_cm_util_map_cm_to_qmi_ip_call_modified_cause()

  DESCRIPTION
    CM returns an IP call modified cause. 
    This function is to map the CM call modified cause to QMI cause.

  PARAMETERS
    cm call modified cause

  RETURN VALUE
    qmi call modified cause

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_voice_cm_if_call_modified_cause_e_type qmi_voice_cm_util_map_cm_to_qmi_ip_call_modified_cause
(
  cm_call_modify_cause_e_type cm_call_modified_cause,
  boolean *is_found
)
{
  qmi_voice_cm_if_call_modified_cause_e_type qmi_cause = QMI_VOICE_CM_IF_CALL_MODIFIED_CAUSE_NONE;
  *is_found = TRUE;

  QM_MSG_MED_1("Inside qmi_voice_cm_util_map_cm_to_qmi_ip_call_modified_cause()=%d", cm_call_modified_cause);

  switch(cm_call_modified_cause)
  {
    case CM_CALL_MODIFY_CAUSE_NONE:
      qmi_cause = QMI_VOICE_CM_IF_CALL_MODIFIED_CAUSE_NONE;
      break;
    case CM_CALL_MODIFY_CAUSE_UPGRADE_DUE_TO_LOCAL_REQ:
      qmi_cause = QMI_VOICE_CM_IF_CALL_MODIFIED_CAUSE_UPGRADE_DUE_TO_LOCAL_REQ;
      break;
    case CM_CALL_MODIFY_CAUSE_UPGRADE_DUE_TO_REMOTE_REQ:
      qmi_cause = QMI_VOICE_CM_IF_CALL_MODIFIED_CAUSE_UPGRADE_DUE_TO_REMOTE_REQ;
      break;
    case CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_LOCAL_REQ:
      qmi_cause = QMI_VOICE_CM_IF_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_LOCAL_REQ;
      break;
    case CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_REMOTE_REQ:
      qmi_cause = QMI_VOICE_CM_IF_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_REMOTE_REQ;
      break;
    case CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_RTP_TIMEOUT:
      qmi_cause = QMI_VOICE_CM_IF_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_RTP_TIMEOUT;
      break;
    case CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_QOS:
      qmi_cause = QMI_VOICE_CM_IF_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_QOS;
      break;
    case CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_PACKET_LOSS:
      qmi_cause = QMI_VOICE_CM_IF_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_PACKET_LOSS;
      break;
    case CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_LOW_THRPUT:
      qmi_cause = QMI_VOICE_CM_IF_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_LOW_THRPUT;
      break;
    case CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_THERM_MITIGATION:
      qmi_cause = QMI_VOICE_CM_IF_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_THERM_MITIGATION;
      break;
    case CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_GENERIC_ERROR:
      qmi_cause = QMI_VOICE_CM_IF_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_GENERIC_ERROR;
      break;
    case CM_CALL_MODIFY_CAUSE_DOWNGRADE_DUE_TO_LIPSYNC:
      qmi_cause = QMI_VOICE_CM_IF_CALL_MODIFIED_CAUSE_DOWNGRADE_DUE_TO_LIPSYNC;
      break;
    default:
      QM_MSG_MED("unhandled call modified cause received");
      qmi_cause = QMI_VOICE_CM_IF_CALL_MODIFIED_CAUSE_NONE;
      *is_found = FALSE;
      break;
  }
  return qmi_cause;
} /* qmi_voice_cm_util_map_cm_to_qmi_ip_call_modified_cause() */

/*===========================================================================
  FUNCTION qmi_voice_cm_util_map_cm_to_qmi_sys_mode()

  DESCRIPTION


  PARAMETERS


  RETURN VALUE


  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
qmi_voice_cm_if_sys_mode_e_type qmi_voice_cm_util_map_cm_to_qmi_sys_mode
(
  sys_sys_mode_e_type  cm_sys_mode
)
{
  qmi_voice_cm_if_sys_mode_e_type qmi_sys_mode = QMI_VOICE_CM_IF_SYS_MODE_NO_SRV;

  QM_MSG_MED_1("Inside qmi_voice_cm_util_map_cm_to_qmi_sys_mode for cm_sys_mode=%d", cm_sys_mode);

  switch(cm_sys_mode)
  {
    case SYS_SYS_MODE_NO_SRV:
      qmi_sys_mode = QMI_VOICE_CM_IF_SYS_MODE_NO_SRV;
      break;
    case SYS_SYS_MODE_CDMA:
      qmi_sys_mode = QMI_VOICE_CM_IF_SYS_MODE_CDMA;
      break;
    case SYS_SYS_MODE_GSM:
      qmi_sys_mode = QMI_VOICE_CM_IF_SYS_MODE_GSM;
      break;
    case SYS_SYS_MODE_HDR:
      qmi_sys_mode = QMI_VOICE_CM_IF_SYS_MODE_HDR;
      break;
    case SYS_SYS_MODE_WCDMA:
      qmi_sys_mode = QMI_VOICE_CM_IF_SYS_MODE_WCDMA;
      break;
    case SYS_SYS_MODE_WLAN:
      qmi_sys_mode = QMI_VOICE_CM_IF_SYS_MODE_WLAN;
      break;
    case SYS_SYS_MODE_LTE:
      qmi_sys_mode = QMI_VOICE_CM_IF_SYS_MODE_LTE;
      break;
    case SYS_SYS_MODE_GWL:
    case SYS_SYS_MODE_GW:
      qmi_sys_mode = QMI_VOICE_CM_IF_SYS_MODE_GWL;
      break;
    case SYS_SYS_MODE_TDS:
      qmi_sys_mode = QMI_VOICE_CM_IF_SYS_MODE_TDS;
      break;
    default:
      QM_MSG_MED("unhandled cm_sys_mode");
      qmi_sys_mode = QMI_VOICE_CM_IF_SYS_MODE_NO_SRV;
      break;
  }
  return qmi_sys_mode;
} /* qmi_voice_cm_util_map_cm_to_qmi_sys_mode() */

/*===========================================================================
  FUNCTION qmi_voice_cm_util_map_qmi_to_cm_sys_mode()

  DESCRIPTION


  PARAMETERS


  RETURN VALUE


  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
sys_sys_mode_e_type qmi_voice_cm_util_map_qmi_to_cm_sys_mode
(
  qmi_voice_cm_if_sys_mode_e_type  qmi_sys_mode
)
{
  sys_sys_mode_e_type cm_sys_mode = SYS_SYS_MODE_NO_SRV;

  QM_MSG_MED_1("Inside qmi_voice_cm_util_map_qmi_to_cm_sys_mode for qmi_sys_mode=%d", qmi_sys_mode);

  switch(qmi_sys_mode)
  {
    case QMI_VOICE_CM_IF_SYS_MODE_NO_SRV:
      cm_sys_mode =  SYS_SYS_MODE_NO_SRV;
      break;
    case QMI_VOICE_CM_IF_SYS_MODE_CDMA:
      cm_sys_mode = SYS_SYS_MODE_CDMA;
      break;
    case QMI_VOICE_CM_IF_SYS_MODE_GSM:
      cm_sys_mode = SYS_SYS_MODE_GSM;
      break;
    case QMI_VOICE_CM_IF_SYS_MODE_HDR :
      cm_sys_mode = SYS_SYS_MODE_HDR;
      break;
    case QMI_VOICE_CM_IF_SYS_MODE_WCDMA :
      cm_sys_mode = SYS_SYS_MODE_WCDMA;
      break;
    case QMI_VOICE_CM_IF_SYS_MODE_WLAN :
      cm_sys_mode = SYS_SYS_MODE_WLAN;
      break;
    case QMI_VOICE_CM_IF_SYS_MODE_LTE :
      cm_sys_mode = SYS_SYS_MODE_LTE;
      break;
    case QMI_VOICE_CM_IF_SYS_MODE_GWL :
      cm_sys_mode = SYS_SYS_MODE_GWL;
      break;
    case QMI_VOICE_CM_IF_SYS_MODE_GW :
      cm_sys_mode = SYS_SYS_MODE_GW;
      break;
    case QMI_VOICE_CM_IF_SYS_MODE_TDS :
      cm_sys_mode = SYS_SYS_MODE_TDS;
      break;
    default:
      QM_MSG_MED("unhandled cm_sys_mode");
      cm_sys_mode = SYS_SYS_MODE_NO_SRV;
      break;
  }
  return cm_sys_mode;
} /* qmi_voice_cm_util_map_qmi_to_cm_sys_mode() */

/*===========================================================================
  FUNCTION qmi_voice_cm_util_is_sys_mode()

  DESCRIPTION


  PARAMETERS


  RETURN VALUE


  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_voice_cm_util_is_sys_mode(uint16 mode_mask, uint8 asubs_id)
{
  uint8 stack=0;
  uint16 sys_mode=0;

  for(stack=0;stack<CM_NO_STACKS;stack++)
  {
    sys_mode = (uint16) util_info.ss_info[asubs_id].sys_mode[stack];
    if(sys_mode & mode_mask)
    {
      return TRUE;
    }
  }
  return FALSE;
}

/*===========================================================================
  FUNCTION qmi_voice_util_is_gsm7_ext()

  DESCRIPTION


  PARAMETERS


  RETURN VALUE


  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_voice_util_is_gsm7_ext(uint16 utf16_char, uint8 *gsm7_ext_char)
{
  uint8 i=0;

  /*loop over to check if the character belongs to gsm 7-bit ext set*/
  for (i = 0 ; i < ARR_SIZE(utf16_to_gsm7_ext_map) ; ++i)
  {
    if (utf16_to_gsm7_ext_map[i].utf16_char == utf16_char)
    {
      QM_MSG_MED_3("Match found = %d, Idx = %d, ucs2_char = %d",
                   utf16_to_gsm7_ext_map[i].gsm7_char, i, utf16_char);
      *gsm7_ext_char = utf16_to_gsm7_ext_map[i].gsm7_char;
      return TRUE;
    }
  }
 
  QM_MSG_MED_1("Match not found for %d", utf16_char);
  return FALSE;
}

/*===========================================================================
  FUNCTION qmi_voice_util_check_if_gsm7_set()

  DESCRIPTION


  PARAMETERS


  RETURN VALUE


  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
boolean qmi_voice_util_check_if_gsm7_ext_set
(
  uint16 *utf16_data,
  uint8   utf16_data_len,
  uint8  *gsm7_data,
  uint8   gsm7_data_len,
  uint8   num_chars,
  uint8  *gsm7_ext_data,
  uint8  *gsm7_ext_str_len
)
{
  uint8 gsm7_idx=0;
  uint8 gsm7_ext_char=0;
  uint8 ext_str_idx=0;

  if( (num_chars > utf16_data_len) || (num_chars > gsm7_data_len) )
  {
    QM_MSG_ERROR_3( "num_chars %d is larger than array boundary utf16_data_len %d or gsm7_data_len %d",
                     num_chars,
                     utf16_data_len,
                     gsm7_data_len );
    return FALSE;
  }
  
  for(gsm7_idx=0; gsm7_idx<num_chars; gsm7_idx++)
  {
    /*if the conversion to gsm 7bit resulted in a "?", then check if its due to 7-bit GSM EXT char*/
    /*If the UCS2 data itself has ?, the char will remain as-is*/

    QM_MSG_MED_2("GSM7 idx = %d, GSM char=%d", gsm7_idx, gsm7_data[gsm7_idx]);

    if((gsm7_data[gsm7_idx] == QMI_MMODE_CHAR_QN) && !(utf16_data[gsm7_idx] == QMI_MMODE_CHAR_QN))
    {
       if(qmi_voice_util_is_gsm7_ext(utf16_data[gsm7_idx],&gsm7_ext_char))
       {
         gsm7_ext_data[ext_str_idx++] = QMI_MMODE_CHAR_ESC; /*GSM 7-bit ext char needs trailing ESC char*/
         gsm7_ext_data[ext_str_idx++] = gsm7_ext_char;
       }
       else
       {
         QM_MSG_MED_1("GSM 7-bit conv not possible char = %d", utf16_data[gsm7_idx]);
         return FALSE;
       }
    }
    else
    {
      gsm7_ext_data[ext_str_idx++] = gsm7_data[gsm7_idx];
    }
  }
  *gsm7_ext_str_len = ext_str_idx;
  return TRUE;
}


boolean qmi_voice_cm_util_copy_barr_num_list_info_from_cm_to_qmi(ie_call_barring_num_info_T *cm_data, qmi_voice_cm_if_icb_sc_s *qmi_data, const uint8 copy_len)
{
  uint8 i = 0;
  byte bcd_number[CM_CALLED_PARTY_BCD_NO_LENGTH];
  
  QM_MSG_MED_1("Inside qmi_voice_cm_util_copy_barr_num_list_info_from_cm_to_qmi for copy_len = %d", copy_len);
  
  if(!qmi_data)
  {
    QM_MSG_ERROR("Received NULL for qmi_data.");
    return FALSE;
  }
  
  for(i=0; i<copy_len && i<QMI_VOICE_CM_IF_MAX_BARR_LIST_LEN; i++)
  {
    memset(bcd_number, 0, CM_CALLED_PARTY_BCD_NO_LENGTH);
    bcd_number[0] = cm_data[i].call_barring_number.length;
    memscpy(&bcd_number[1], CM_CALLED_PARTY_BCD_NO_LENGTH-1, cm_data[i].call_barring_number.data, cm_data[i].call_barring_number.length);
    qmi_data[i].barr_num_len = cm_util_bcd_num_to_ascii_num(bcd_number,
                                                            qmi_data[i].barr_num,
                                                            QMI_VOICE_CM_IF_MAX_CALLING_NUM_LEN);
    if(cm_data[i].ss_status.present && cm_data[i].ss_status.cm_ss_active)
    {
      qmi_data[i].barr_num_status = QMI_VOICE_CM_SUPS_STATUS_ACTIVE;
    }//else inactive=0
    QM_MSG_MED_5("qmi_data[%d].barr_num_len = %d, ss_status.present = %d, ss_status.active = %d, barr_num_status = %d",
                 i, qmi_data[i].barr_num_len, cm_data[i].ss_status.present,
                 cm_data[i].ss_status.cm_ss_active, qmi_data[i].barr_num_status);
  }
  
  if(i==copy_len)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

boolean qmi_voice_cm_util_copy_barr_num_list_from_qmi_to_cm(qmi_voice_cm_if_icb_ind_s *qmi_data, cm_called_party_bcd_no_s_type *cm_data, const uint8 copy_len)
{
  uint8 i = 0;
  cm_num_s_type call_barring_num;
  byte bcd_number[CM_CALLED_PARTY_BCD_NO_LENGTH];
  int toa = 129;

  QM_MSG_MED_1("Inside qmi_voice_cm_util_copy_barr_num_list_from_qmi_to_cm for copy_len = %d", copy_len);

  for(i=0; i<copy_len && i<QMI_VOICE_CM_IF_MAX_BARR_LIST_LEN; i++)
  {
    memset(bcd_number, 0, CM_CALLED_PARTY_BCD_NO_LENGTH);
    memset(&call_barring_num, 0, sizeof(cm_num_s_type));
    memscpy(call_barring_num.buf, CM_MAX_NUMBER_CHARS, qmi_data[i].barr_num, qmi_data[i].barr_num_len);
    cm_num_fill(&call_barring_num, (byte *)qmi_data[i].barr_num,
                (byte)qmi_data[i].barr_num_len,
                CM_DIGIT_MODE_8BIT_ASCII);

    call_barring_num.number_type = (toa & CM_TON_MASK) >> 4;
    call_barring_num.number_plan = (toa & 0x0F);

    cm_util_number_to_bcd(&call_barring_num, bcd_number);
    cm_data[i].present = TRUE;
    /* qcril_cm_util_phone_number_to_bcd saves the length in bcd_number[0] */
    cm_data[i].length = bcd_number[0];
    memscpy(&bcd_number[0], CM_CALLED_PARTY_BCD_NO_LENGTH, &bcd_number[1], cm_data[i].length);
    bcd_number[cm_data[i].length] = '\0';
    memscpy(cm_data[i].data, CM_CALLED_PARTY_BCD_NO_LENGTH, bcd_number, cm_data[i].length);
  }
  
  if(i==copy_len)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/*===========================================================================
FUNCTION qmi_voice_cm_util_fill_number_and_length_with_plus_sign

DESCRIPTION
  This API fills the number and length with special handling on 
  international type.

DEPENDENCIES
  None.

SIDE EFFECTS
  None
===========================================================================*/
void qmi_voice_cm_util_fill_number_and_length_with_plus_sign
(
  uint8                *dest_num_buf_ptr,
  uint32               dest_num_buf_len,
  uint8                *dest_len_ptr,
  const cm_num_s_type  *src_num_s_ptr
)
{
  uint8   num_buf[CM_MAX_NUMBER_CHARS];
  memset( num_buf, 0, sizeof(num_buf) );

  if( src_num_s_ptr->len > 0 )
  {
    *dest_len_ptr = src_num_s_ptr->len;
    QM_MSG_HIGH_4("Fill Num & Len: source len = %d, source first three digits: [0]=%x, [1]=%x, [2]=%x",
                  src_num_s_ptr->len, src_num_s_ptr->buf[0],
                  src_num_s_ptr->buf[1], src_num_s_ptr->buf[2]);

    if( (src_num_s_ptr->buf[0] != '+') && (src_num_s_ptr->number_type == (uint8)CM_NUM_TYPE_INTERNATIONAL) )
    {
      if((src_num_s_ptr->buf[0] == '0') && (src_num_s_ptr->buf[1] == '0'))
      {
        /* Case when the first two digits are 00 (e.g. 0091) *
         * Remove the 00 and add a '+' in the beginning      */
        num_buf[0] = '+';
        *dest_len_ptr = *dest_len_ptr - 1;
        memscpy(&num_buf[1], sizeof(num_buf) - 1,
                &src_num_s_ptr->buf[2], src_num_s_ptr->len - 2);

        memscpy(dest_num_buf_ptr, dest_num_buf_len,
                num_buf,          sizeof(num_buf));
        QM_MSG_HIGH_4("Fill Num & Len: remove '00' with '+', new len = %d, new first three digits: [0]=%x, [1]=%x, [2]=%x",
                      *dest_len_ptr, dest_num_buf_ptr[0],
                      dest_num_buf_ptr[1], dest_num_buf_ptr[2]);
      }
      else
      {
        /* Prepend the '+' in the beginning */
        num_buf[0] = '+';
        *dest_len_ptr = *dest_len_ptr + 1;
        memscpy(&num_buf[1], sizeof(num_buf) - 1,
                src_num_s_ptr->buf, src_num_s_ptr->len);

        memscpy(dest_num_buf_ptr, dest_num_buf_len,
                num_buf,          sizeof(num_buf));
        QM_MSG_HIGH_4("Fill Num & Len: prepend with '+', new len = %d, new first three digits: [0]=%x, [1]=%x, [2]=%x",
                      *dest_len_ptr, dest_num_buf_ptr[0],
                      dest_num_buf_ptr[1], dest_num_buf_ptr[2]);
      }
    }
    else
    {
      /* Copy directly from the source buffer */
      memscpy(dest_num_buf_ptr,   dest_num_buf_len,
              src_num_s_ptr->buf, src_num_s_ptr->len);
      QM_MSG_HIGH_3("Fill Num&Len: direct copy. len=%d, [0]=%x, [1]=%x",
                     *dest_len_ptr, dest_num_buf_ptr[0], dest_num_buf_ptr[1] );
    }
  }
  else
  {
    QM_MSG_HIGH_1("Fill Num&Len: source num len < 0 (%d), set dest len to 0", src_num_s_ptr->len);
    *dest_len_ptr = 0;
  }
}

#endif /*! FEATURE_NO_QMI_VOICE*/

#endif // FEATURE_QMI_SERVICE_SPLIT


