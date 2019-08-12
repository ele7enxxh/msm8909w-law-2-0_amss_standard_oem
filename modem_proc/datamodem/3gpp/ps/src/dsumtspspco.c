/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  U M T S ( W C D M A / G P R S )  P A C K E T - S W I T C H E D   D A T A

               P R O T O C O L  C O N F I G  O P T I O N S 

GENERAL DESCRIPTION
  This file contains miscenallenous functions pertaining to 
  protocol config options encoding and decoding WCDMA/GPRS 
  mode specific handler   

Copyright (c) 2002-2014 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp/ps/src/dsumtspspco.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/22/09   vrk	   Merged LTE changes.
05/11/09   ms      Updated CommonData variable names.
03/07/08   vrk     PCSCF addr decoding is independent of FEATURE_DATA_PS_IPV6
10/16/07   vk      Detect zero option length in ipcp packet and return error
06/01/07   aw      Featurized the IMS related changes under FEATURE_DATA_IMS.
03/07/07   aw      Fixed copying of pcscf address. Changed get64 to memcpy().
01/24/06   rr      Fixed Klocwork warnings.
11/06/06   vk      Wrapped decoding of PCSCF addresses under IPv6 feature, to
                   fix compilation error when IPv6 feature is not defined.
10/04/06   rr      Fixed copyying of IPV6 dns address. Get64 could nt be used 
                   here.
09/09/06   rr      Added NV Item support for NBNS.
08/01/06   rr      Fixed Lint errors/warnings.
07/17/06   aw      Changed cscf to pcscf.
07/18/06   aw      Fixed errors form IMS testing and changed function name
06/13/06   aw      Added support to request and store cscf address.
11/04/05   rc      In dsumtsps_gen_ipcp_req_pkt(), removed pointer returned
                   from put32 since it is not used. 
05/12/05   rr      Added IPV6 code.
01/27/05   ssh     Changed include file name dsbyte.h to ps_byte.h
11/01/04   rr      Added WINS Support
11/14/03   vsk     gen chap pkts even when secret / name fields are NULL
04/18/03   vsk     Added dsumtsps_ppp_fill_chap_response_pkt function
                   added dns values to be encoded to dsumtsps_gen_ipcp_req_pkt()
03/21/03   vsk     Handle IPCP config Ack or Nak to retrieve DNS addresses
                   to handle GGSN's which send ACK instead of NAK    
02/26/03   rc      Removed check for identifer for a CONFIG_REQ packet in
                   dsumtsps_decode_ipcp_pkt().
11/13/02   rc      Added multimode support for GPRS.
10/03/02   rc      Added support for Multimode.
07/27/02   vsk     Created module/initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include <stringl/stringl.h>

#include "amssassert.h"
#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS) || defined (FEATURE_DATA_LTE)

#include "msg.h"
#include "err.h"
#include "rex.h"
#include "dsumtsps.h"
#include "dsumtspspco.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_ipcp.h"
#include "md5.h"
#include "ps_byte.h"
#include "ps_in.h"
#include "ds_3gppi_utils.h"

#include "ds3gmgr.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*===========================================================================
                   INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/


/*===========================================================================
FUNCTION DSUMTSPS_PPP_GEN_PAP_AUTH_REQ_PKT

DESCRIPTION
  This function generates a PAP authentication request packet.
  
PARAMETERS  
    pkt_buff_ptr  - Pointer to the buffer to store the packet
    pkt_buff_len  - length of buffer to store the packet
    identifier    - ID of the PAP authentication pkt 
    user_id_ptr   - pointer to buffer which has the user_id
    user_id_len   - length of the user_id
    passwd_ptr    - pointer to the buffer which has the password
    passwd_len    - length of the password
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  > 0 - packet generated succesfully & indicates the length of the packet
  -1  - pkt_buffer too short for packet       
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
int dsumtsps_ppp_gen_pap_auth_req_pkt
(
  byte  *pkt_buff_ptr, 
  int   pkt_buff_len,
  byte  identifier,
  const byte *user_id_ptr,
  byte  user_id_len,
  const byte *passwd_ptr,
  byte  passwd_len
)
{
  int       pap_pkt_len;
  register  uint8 *cp;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(pkt_buff_ptr == NULL || user_id_ptr == NULL || passwd_ptr == NULL)
  {
    DS_3GPP_MSG3_ERROR("Null ptr: pkt_buff_ptr %x, user_id_ptr %x, passwd_ptr %x",
                                  pkt_buff_ptr,user_id_ptr,passwd_ptr);
    ASSERT(0);
    return -1; /* If ASSERT is disabled, -1 will fail call origination*/
  } 

  /*-------------------------------------------------------------------------
    Find the length of the packet to be generated     
    Pkt format: This is a PAP config. request : 
    |Pkt type|Pkt ID |Len|user id len|user id|passwd len|passwd
    | 1 byte |1 byte |2  | 1 byte    |< 255  | 1        | <255   
    Len is the length of the whole pkt starting at pkt-type
    CONFIG_HDR_LEN includes pkt_type, pkt_id and length fields.

    Since user_id-len and passwd-len do not include their own size, we add 2
    ( 1 each for user_id len & passwd_len )
  -------------------------------------------------------------------------*/
  pap_pkt_len = CONFIG_HDR_LEN 
                + user_id_len 
                + passwd_len 
                + 2 ;
  
  if( pap_pkt_len > pkt_buff_len )
  {
    ERR("Buff too short reqd:%d len:%d",pap_pkt_len,pkt_buff_len,0);
    return -1;
  }
         
  /*-------------------------------------------------------------------------
    Construct the packet
  -------------------------------------------------------------------------*/
  cp  = pkt_buff_ptr;
  
  /*-------------------------------------------------------------------------
    Fill Pkt type  
  -------------------------------------------------------------------------*/
  *cp++ = CONFIG_REQ;  

  /*-------------------------------------------------------------------------
    Fill Identifier
  -------------------------------------------------------------------------*/
  *cp++ = identifier;           

  /*-------------------------------------------------------------------------
    Fill Pkt length    , put16 increments the ptr by two
  -------------------------------------------------------------------------*/
  cp  = put16( cp, (word)pap_pkt_len );

  if(cp == NULL)
  {
    DATA_ERR_FATAL("put16 returned NULL pointer");
    return -1; /* If ASSERT is disabled, -1 will fail call origination*/
  }    

  /*-------------------------------------------------------------------------
    Make the user-id field in the PAP config-req.
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Put in the id-length field  
  -------------------------------------------------------------------------*/
  *cp++ = user_id_len; 
  
  /*-------------------------------------------------------------------------
    Put the user id in the item      
  -------------------------------------------------------------------------*/
  memscpy( cp, user_id_len, user_id_ptr,user_id_len);   
    
  /*-------------------------------------------------------------------------
    Increment the pointer to store the password 
  -------------------------------------------------------------------------*/
  cp  += user_id_len;  
    
  /*-------------------------------------------------------------------------
    Make the password field in the PAP config-req.
  -------------------------------------------------------------------------*/
     
  /*-------------------------------------------------------------------------
    Put in the passwd-length field    
  -------------------------------------------------------------------------*/
  *cp++ = passwd_len;     
     
  /*-------------------------------------------------------------------------
   Put the passsword in the packet    
  -------------------------------------------------------------------------*/
  memscpy( cp, passwd_len, passwd_ptr, passwd_len);   
   
  DS_3GPP_MSG0_LOW("PAP authentication packet generated");

  return pap_pkt_len;

} /* dswpsd_ppp_gen_pap_auth_req_pkt() */


/*===========================================================================
FUNCTION DSUMTSPS_PPP_GEN_CHAP_CHALLENGE_PKT

DESCRIPTION
  This function generates a CHAP Challange packet.
  
PARAMETERS   
    pkt_buff_ptr  - Pointer to the buffer to store the packet
    pkt_buff_len  - length of buffer to store the packet
    identifier    - ID of the CHAP challenge pkt 
    challenge_ptr - pointer to buffer which has CHAP challenge
    challenge_len - length of challenge
    sys_name_ptr  - pointer to buffer which has system name 
    sys_name_len  - length of system name
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  0   - packet generated succesfully 
  -1  - pkt_buffer too short for packet       
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
int dsumtsps_ppp_gen_chap_challenge_pkt
(
  byte *pkt_buff_ptr, 
  int   pkt_buff_len,
  byte  identifier,
  const byte *challenge_ptr,
  byte  challenge_len,
  const byte *sys_name_ptr,
  byte  sys_name_len
)
{
  int       chap_pkt_len;
  register  uint8 *cp;
  int       dest_buff_len = pkt_buff_len;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(pkt_buff_ptr == NULL || challenge_ptr == NULL || sys_name_ptr == NULL)
  {
    DS_3GPP_MSG3_ERROR("Null ptr: pkt_buff_ptr %x, challenge_ptr %x, sys_name_ptr %x",
                                  pkt_buff_ptr,challenge_ptr,sys_name_ptr);
    ASSERT(0);
    return -1; /* If ASSERT is disabled, -1 will fail call origination*/
  } 

  DS_3GPP_MSG0_HIGH("Generating CHAP Chall pkt");

  /*-------------------------------------------------------------------------
    Find the length of the packet to be generated     
    pkt format : Chap Challenge Pkt :
    Pkt type|Pkt ID |Len|value size |value  | name
    |1 byte |1 byte |2  | 1 byte    |< 255  | 
    Len is the length of the whole pkt starting at pkt-type
    CONFIG_HDR_LEN includes pkt_type, pkt_id and length fields.

    We add 1 since value_size does not include its own size
    ( 1 byte for value_size )
  -------------------------------------------------------------------------*/
  chap_pkt_len = CONFIG_HDR_LEN 
                + challenge_len 
                + sys_name_len 
                + 1 ;
  
  if( chap_pkt_len > pkt_buff_len )
  {
    DATA_MSG2_ERROR("Buff too short reqd:%d len:%d",chap_pkt_len,pkt_buff_len);
    return -1;
  }

  DS_3GPP_MSG1_HIGH("chap chal pkt len :%d",chap_pkt_len);         
  /*-------------------------------------------------------------------------
    Creating the packet
  -------------------------------------------------------------------------*/
  cp  = pkt_buff_ptr;
  
  /*-------------------------------------------------------------------------
    Fill Pkt type  
  -------------------------------------------------------------------------*/
  *cp++ = CODE_CHAP_CHALLENGE;  
  dest_buff_len --;

  /*-------------------------------------------------------------------------
    Fill Identifier
  -------------------------------------------------------------------------*/
  *cp++ = identifier;           
  dest_buff_len --;
  /*-------------------------------------------------------------------------
    Fill Pkt length : put16 increments the ptr by two
  -------------------------------------------------------------------------*/
  cp  = put16( cp, (word)chap_pkt_len );
  dest_buff_len = dest_buff_len - 2;
    
  if(cp == NULL)
  {
    DATA_ERR_FATAL("put16 returned NULL pointer");
    return -1; /* If ASSERT is disabled, -1 will fail call origination*/
  }  

  /*-------------------------------------------------------------------------
    Make the challenge ( value) field of chap packet
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Put in the value size field
  -------------------------------------------------------------------------*/
  *cp++ = challenge_len; 
  dest_buff_len--;
  DS_3GPP_MSG1_HIGH("chap challenge len :%d",challenge_len);         
  
  /*-------------------------------------------------------------------------
    Put in the challenge
  -------------------------------------------------------------------------*/
  memscpy( cp, dest_buff_len, challenge_ptr,challenge_len);   
    
  /*-------------------------------------------------------------------------
    Increment the pointer to store the name
  -------------------------------------------------------------------------*/
  cp  += challenge_len;  
  dest_buff_len = dest_buff_len - challenge_len;
  /*-------------------------------------------------------------------------
    Make the name field. The length is not coded separately. Rather 
    inferred from the packet length
  -------------------------------------------------------------------------*/
  memscpy( cp, dest_buff_len,sys_name_ptr, sys_name_len);   
  DS_3GPP_MSG1_HIGH("chap sys name len :%d",sys_name_len);          

  DS_3GPP_MSG0_LOW("CHAP challenge packet generated");

  return chap_pkt_len;

} /* dswpsd_ppp_gen_chap_challenge_pkt() */


/*===========================================================================
FUNCTION DSUMTSPS_PPP_GEN_CHAP_RESPONSE_PKT

DESCRIPTION
  This function generates a CHAP response  packet. It uses the parameters
  passed in to digest the challenge & generate the reponse & then construct
  the ppp chap response packet
  
PARAMETERS   
    pkt_buff_ptr  - Pointer to the buffer to store the packet
    pkt_buff_len  - length of buffer to store the packet
    identifier    - ID of the CHAP challenge pkt 
    challenge_ptr - pointer to buffer which has CHAP challenge
    challenge_len - length of challenge
    secret_ptr    - pointer to buffer which has the secret / password
    secret_len    - length of password
    sys_name_ptr  - pointer to buffer which has system name 
    sys_name_len  - length of system name
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  0   - packet generated succesfully 
  -1  - pkt_buffer too short for packet       
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
int dsumtsps_ppp_gen_chap_response_pkt
(
  byte  *pkt_buff_ptr, 
  int   pkt_buff_len,
  byte  identifier,
  const byte  *challenge_ptr,
  byte  challenge_len,
  const byte  *secret_ptr,
  byte  secret_len,
  const byte  *sys_name_ptr,
  byte  sys_name_len
)
{
  register  uint8 *cp;
  MD5_CTX   context;  /*contains the seed to be used for making MD5 output */
  unsigned char digest[16];/* array for the 128 bit result of MD5 digest   */
                           /* algorithm                                    */
  uint16    chap_response_length; /*length of the Chap Response to be sent */
  dsm_item_type *item_ptr;    /*Pointer to the Message for MD5 Digest algo */
  byte      *bp;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(pkt_buff_ptr == NULL || challenge_ptr == NULL || secret_ptr == NULL || 
                                                        sys_name_ptr == NULL)
  {
    MSG_4(MSG_SSID_DS,
          MSG_LEGACY_ERROR,
          "Null ptr: pkt_buff_ptr %x, challenge_ptr %x, secret_ptr %x, sys_name_ptr %x",
           pkt_buff_ptr, challenge_ptr , secret_ptr, sys_name_ptr);
    ASSERT(0);
    return -1; /* If ASSERT is disabled, -1 will fail call origination*/
  } 

  DS_3GPP_MSG0_HIGH("Generating CHAP Res pkt");
  /*-------------------------------------------------------------------------
    Check if the challenge lengths is 0. 
    If yes, then return without generating any packet
  -------------------------------------------------------------------------*/
  DS_3GPP_MSG3_HIGH("CHAP Chal :%d Sec :%d Nam:%d",
           challenge_len,secret_len,sys_name_len);

  if( challenge_len == 0 ) 
  {
    DS_3GPP_MSG0_HIGH("NULL challenge");
    return -1;
  }


  /*-------------------------------------------------------------------------
    Find the length of the packet to be generated     
    pkt format : Chap Response Pkt :
    |Pkt type|Pkt ID |Len|value size |value  | name
    | 1 byte |1 byte |2  | 1 byte    |< 255  | 
    Len is the length of the whole pkt starting at pkt-type
    CONFIG_HDR_LEN includes pkt_type, pkt_id and length fields.

    We add 1 since value_size does not include its own size
    ( 1 byte for value_size )
    
    Note : Value is 16 bytes if MD5 is used
  -------------------------------------------------------------------------*/
  chap_response_length =  CONFIG_HDR_LEN 
                          + MD5_DIGEST_SIZE 
                          + sys_name_len 
                          + 1 ;

  if( chap_response_length > pkt_buff_len )
  {
    DATA_MSG2_ERROR("Buff too short reqd:%d len:%d",chap_response_length,pkt_buff_len);
    return -1;
  }

  DS_3GPP_MSG1_HIGH("Chap Chal res pkt len :%d",chap_response_length);
  /*-------------------------------------------------------------------------
    Build message for preparing the 16 octet message-digest output. The 
    message is built by appending the ID, Secret and the Challenge. 
    Obtain the challenge from the chap_challenge_ptr. 
    Obtain the secret from secret_ptr
  
    Obtain a new dsm_item_type to build the message. This message will be 
    given to MD5 algorithm for generating the 128bit Message digest.
    The message will have a length of :
    1  ( size of idenfifier  )
    + secret_len           
    + challenge_len
            
    item_ptr will point to the new item. This item will be filled with
    chap challenge ID , Secret and Challenge.
  -------------------------------------------------------------------------*/
  if((item_ptr  = dsm_new_buffer(DSM_DS_POOL_SIZE (1
                                  + secret_len
                                  + challenge_len))) == NULL)
    {
       DATA_MSG0_ERROR("Message pkt allocation failed ");
       return -1;
    }

  /*-------------------------------------------------------------------------
    Obtain the pointer to the data in the item_ptr. This will be used to 
    fill the message for the MD5 input.
  -------------------------------------------------------------------------*/
  bp = item_ptr->data_ptr;

  /*-------------------------------------------------------------------------
    Fill the incoming challenge's ID 
    Fill the secret of the Mobile Station. 
    Fill the challenge from the challenge_ptr. 
  -------------------------------------------------------------------------*/
  *bp++ = identifier;

  memscpy( bp, secret_len,secret_ptr, secret_len);
  bp += secret_len;

  memscpy( bp,challenge_len, challenge_ptr,challenge_len);
            
  /*-------------------------------------------------------------------------
    By now, the input for the MD5 has been prepared. This should now be given
    to the MD5 algorithm to prepare the 128bit output. bp should again point
    to the beginning of the data i.e. points to the chap ID. bp, will be 
    passed to the MD5 algorithm as an input.
  -------------------------------------------------------------------------*/
  bp = item_ptr->data_ptr;

  DS_3GPP_MSG0_HIGH("Starting the response generation");

  /*-------------------------------------------------------------------------
    Initialize MD5 with a new context.  This function loads magic 
    initialization constants.
  -------------------------------------------------------------------------*/
  MD5Init(&context);

  /*-------------------------------------------------------------------------
    Update the context with the new message block.
    The inputs that are expected are:
    MD5_CTX *context for context 
    unsigned char *input for input block
    unsigned int inputLen for length of input block 
  -------------------------------------------------------------------------*/
  MD5Update(&context, 
          bp, 
          1+challenge_len+secret_len);
  
  /*-------------------------------------------------------------------------
    MD5 finalization. Ends an MD5 message-digest operation, writing the
    the message digest and zeroizing the context. The following parameters
    should be passed:
    unsigned char digest[16] for message digest 
    MD5_CTX *context for context
  -------------------------------------------------------------------------*/
  MD5Final(digest, &context);

  /*-----------------------------------------------------------------------
    Free the dsm_item which had the message to be digested
  -------------------------------------------------------------------------*/
  dsm_free_packet(&item_ptr);   
        
  /*-------------------------------------------------------------------------
    By now, all the information needed to be put in the Chap Response is
    available. So, start building the response pointed by, Chap_response_ptr 
    Make the follwing for the Response:
    Code field should be CHAP_RESPONSE
    Identifier field 
    Length field should be Chap_response_length 
    Value_size field should be MD5_Size. This will be 16 for  MD5.
    Value field should be the contents of the digest array.
    Name field should be the sys_name_ptr
        
    Use cp to point to the data_ptr in the chap_response_ptr and update the
    fields sequentially.
  -------------------------------------------------------------------------*/
  cp      = pkt_buff_ptr;                     /* cp points to byte array   */
  *cp++   = CODE_CHAP_RESPONSE;               /* Fill in response code     */
  *cp++   = identifier;                       /* Fill in the identifier    */
  cp      = put16( cp,chap_response_length ); /*Fill in length field       */

  if(cp == NULL)
  {
    DATA_ERR_FATAL("put16 returned NULL pointer");
    return -1; /* If ASSERT is disabled, -1 will fail call origination*/
  }  
  *cp++   = MD5_DIGEST_SIZE;                  /* Fill in value size field  */
                                              
  memscpy(cp, MD5_DIGEST_SIZE, digest, MD5_DIGEST_SIZE);        /* copy 128bit response      */
  cp += MD5_DIGEST_SIZE;                      /* Increment cp by 16bytes   */

  /*-------------------------------------------------------------------------
    Copy the sys_name into the response packet.
  -------------------------------------------------------------------------*/
  memscpy( cp, sys_name_len, sys_name_ptr,sys_name_len);
          
  return chap_response_length;

} /* dsumtsps_ppp_gen_chap_response_pkt() */


/*===========================================================================
FUNCTION DSUMTSPS_PPP_FILL_CHAP_RESPONSE_PKT

DESCRIPTION
  This function constructs a CHAP response  packet from the parameters passed
  in. The chap response is passed in and this function just fills in 
  all the parameters to construct the packet
  
PARAMETERS   
    pkt_buff_ptr  - Pointer to the buffer to store the packet
    pkt_buff_len  - length of buffer to store the packet
    identifier    - ID of the CHAP challenge pkt 
    hash_ptr      - pointer to buffer which has CHAP response
    hash_len      - length of challenge
    responder_name_ptr  - pointer to buffer which has responder's name 
    responder_name_len  - length of responder's name
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  0   - packet generated succesfully 
  -1  - pkt_buffer too short for packet       
  
SIDE EFFECTS 
  None.
  
===========================================================================*/
int dsumtsps_ppp_fill_chap_response_pkt
(
  byte  *pkt_buff_ptr, 
  int   pkt_buff_len,
  byte  identifier,
  const byte  *hash_ptr,
  byte  hash_len,
  const byte  *responder_name_ptr,
  byte  responder_name_len
)
{
  register  uint8 *cp;
  uint16    chap_response_length; /*length of the Chap Response to be sent */
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(pkt_buff_ptr == NULL || hash_ptr == NULL || responder_name_ptr == NULL)
  {
    DS_3GPP_MSG3_ERROR("Null ptr: pkt_buff_ptr %x, hash_ptr %x, responder_name_ptr %x",
                                  pkt_buff_ptr,hash_ptr,responder_name_ptr);
    ASSERT(0);
    return -1; /* If ASSERT is disabled, -1 will fail call origination*/
  } 

  DS_3GPP_MSG0_HIGH("constructing CHAP Res pkt from parms passed");
  /*-------------------------------------------------------------------------
    Check if the hash length is 0. 
    If yes, then return without generating any packet
  -------------------------------------------------------------------------*/
  if(  hash_len == 0  ) 
  {
    ERR("NULL hash:%d ", hash_len,0,0);
    return -1;
  }
  
  DS_3GPP_MSG2_HIGH("hash len:%d resp len:%d",hash_len,responder_name_len);

  /*-------------------------------------------------------------------------
    Find the length of the packet to be generated     
    pkt format : Chap Response Pkt :
    |Pkt type|Pkt ID |Len|value size |value  | name
    | 1 byte |1 byte |2  | 1 byte    |< 255  | 
    Len is the length of the whole pkt starting at pkt-type
    CONFIG_HDR_LEN includes pkt_type, pkt_id and length fields.

    We add 1 since value_size does not include its own size
    ( 1 byte for value_size )
    
    Note : Value is 16 bytes if MD5 is used
  -------------------------------------------------------------------------*/
  chap_response_length =  CONFIG_HDR_LEN 
                          + hash_len 
                          + responder_name_len 
                          + 1 ;

  if( chap_response_length > pkt_buff_len )
  {
    DATA_MSG2_ERROR("Buff too short reqd:%d len:%d",chap_response_length,pkt_buff_len);
    return -1;
  }

  DS_3GPP_MSG1_HIGH("Chap Chal res pkt len :%d",chap_response_length);
  
  /*-------------------------------------------------------------------------
    start building the response pointed by, Chap_response_ptr 
    Make the follwing for the Response:
    Code field should be CHAP_RESPONSE
    Identifier field 
    Length field should be Chap_response_length 
    Value_size field should be MD5_Size. This will be 16 for  MD5.
    Value field should be the contents of the digest array.
    Name field should be the sys_name_ptr
        
    Use cp to point to the data_ptr in the chap_response_ptr and update the
    fields sequentially.
  -------------------------------------------------------------------------*/
  cp      = pkt_buff_ptr;                     /* cp points to byte array   */
  *cp++   = CODE_CHAP_RESPONSE;               /* Fill in response code     */
  *cp++   = identifier;                       /* Fill in the identifier    */
  cp      = put16( cp,chap_response_length ); /*Fill in length field       */

  if(cp == NULL)
  {
    DATA_ERR_FATAL("put16 returned NULL pointer");
    return -1; /* If ASSERT is disabled, -1 will fail call origination*/
  }  

  *cp++   = hash_len;                         /* Fill in value size field  */
                                              
  memscpy(cp, hash_len, hash_ptr, hash_len);             /* copy hash response        */
  cp += MD5_DIGEST_SIZE;                      /* Increment cp by 16bytes   */

  /*-------------------------------------------------------------------------
    Copy the sys_name into the response packet.
  -------------------------------------------------------------------------*/
  memscpy( cp, responder_name_len, responder_name_ptr,responder_name_len);
          
  return chap_response_length;

} /* dsumtsps_ppp_fill_chap_response_pkt() */


/*===========================================================================
FUNCTION DSUMTSPS_GEN_IPCP_REQ_PKT

DESCRIPTION
  This function generates a IPCP request packet. The IPCP options that
  are requested are Primary and Secondary DNS addresses.
  
PARAMETERS   
  protocol_pkt_ptr  - Pointer to the buffer to store the IPCP packet
  protocol_pkt_len  - length of buffer to store the IPCP packet
  identifier - ID of the IPCP packet
  primary_dns   - primary_dns address
  secondary_dns - secondary_dns address
    
DEPENDENCIES 
  None.

RETURN VALUE  
  > 0 - packet generated succesfully & indicates the length of the packet
  -1  - pkt_buffer too short for packet       
  
SIDE EFFECTS 
  None.

===========================================================================*/
int dsumtsps_gen_ipcp_req_pkt
(
  byte  *protocol_pkt_ptr,/* Pointer to the buffer to store the IPCP packet*/
  int   protocol_pkt_len,          /* Length of buffer to store the packet */
  byte  identifier,                               /* ID of the IPCP packet */
  uint32 primary_dns,                             /* primary DNS address   */
  uint32 secondary_dns,                           /* secondary DNS address */
  uint32 primary_nbns,                            /* primary NBNS address  */
  uint32 secondary_nbns,                          /* secondary NBNS address*/
  net_cfg_params_mask_e_type valid_fields         /* IPCP fileds to request */
)
{
  int       dns_pkt_len;                          /* Length of IPCP packet */  
  register  byte *buf_ptr;                       /* Buffer to build packet */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(protocol_pkt_ptr == NULL)
  {
    DS_3GPP_MSG1_ERROR("Null ptr: protocol_pkt_ptr %x", protocol_pkt_ptr);
    ASSERT(0);
    return -1; /* If ASSERT is disabled, -1 will fail call origination*/
  } 

  /*-------------------------------------------------------------------------
    Pkt format: This is a Primary and Secondary DNS config. request packet:
    
     |Code|Identifier|Length|Type|Length|Primary DNS|Type|Len|Secondary DNS|         
       1       1        2     1     1        4        1    1      4
   
    Find the length of the packet to be generated.
    CONFIG_HDR_LEN = length of (Code + identifer + length)
    IPCP_PRIMARY_DNS_SIZ = Length of IPCP Primary DNS option
                           (Type + Len + Primary DNS)
    IPCP_SECONDARY_DNS_SIZ = Length of IPCP Secondary DNS option
                             (Type + Len + Secondary DNS)
  -------------------------------------------------------------------------*/
  dns_pkt_len = CONFIG_HDR_LEN +
                IPCP_PRIMARY_DNS_SIZ;
  /*lint -e655*/
  if (valid_fields & NET_CFG_PRM_SEC_DNS_MASK) {  
    dns_pkt_len += IPCP_SECONDARY_DNS_SIZ;
  }
  /*lint +e655*/

  
  /*-------------------------------------------------------------------------
    Request NBNS addresses only if requested in ppp_info.
  -------------------------------------------------------------------------*/

  /*lint -e655*/
  if(valid_fields & NET_CFG_PRM_PRI_NBNS_MASK)
  {
    dns_pkt_len += IPCP_PRIMARY_NBNS_SIZ;
  }

  if(valid_fields & NET_CFG_PRM_SEC_NBNS_MASK)
  {
    dns_pkt_len += IPCP_SECONDARY_NBNS_SIZ;
  }
  /*lint +e655*/

  if( dns_pkt_len > protocol_pkt_len )
  {
    DATA_MSG2_ERROR("Buff too short reqd:%d len:%d",dns_pkt_len,protocol_pkt_len);
    return -1;
  }
         
  /*-------------------------------------------------------------------------
    Construct the packet
  -------------------------------------------------------------------------*/
  buf_ptr  = protocol_pkt_ptr;
  
  /*-------------------------------------------------------------------------
    Fill Code - 0 : CONFIG_REQ  
  -------------------------------------------------------------------------*/
  *buf_ptr++ = CONFIG_REQ;  

  /*-------------------------------------------------------------------------
    Fill Identifier
  -------------------------------------------------------------------------*/
  *buf_ptr++ = identifier;    

  /*-------------------------------------------------------------------------
    Fill Packet length
  -------------------------------------------------------------------------*/
   buf_ptr  = put16( buf_ptr, (word)dns_pkt_len );

  if(buf_ptr == NULL)
  {
    DATA_ERR_FATAL("put16 returned NULL pointer");
    return -1; /* If ASSERT is disabled, -1 will fail call origination*/
  }  

  /*-------------------------------------------------------------------------
    Fill in the Type field for Primary DNS - 129
  -------------------------------------------------------------------------*/
  *buf_ptr++ = IPCP_PRIMARY_DNS;

  /*-------------------------------------------------------------------------
    Fill in the length field for Primary DNS
  -------------------------------------------------------------------------*/
  *buf_ptr++ = IPCP_PRIMARY_DNS_SIZ;

  /*-------------------------------------------------------------------------
   Fill in the primary DNS value(4 bytes) - Set it to the value requested
  -------------------------------------------------------------------------*/
  buf_ptr = hput32(buf_ptr,primary_dns);

  if(buf_ptr == NULL)
  {
    DATA_ERR_FATAL("put32 returned NULL pointer");
    return -1; /* If ASSERT is disabled, -1 will fail call origination*/
  }    


  /*lint -e655*/
  if(valid_fields & NET_CFG_PRM_PRI_NBNS_MASK)
  /*lint +e655*/
  {
    /*-----------------------------------------------------------------------
    Fill in the Type field for Primary NBNS - 130
    -----------------------------------------------------------------------*/
    *buf_ptr++ = IPCP_PRIMARY_NBNS;

    /*-----------------------------------------------------------------------
    Fill in the length field for Primary NBNS
    -----------------------------------------------------------------------*/
    *buf_ptr++ = IPCP_PRIMARY_NBNS_SIZ;

    /*-----------------------------------------------------------------------
     Fill in the primary NBNS value(4 bytes) - Set it to the value requested
    -----------------------------------------------------------------------*/
    buf_ptr = hput32(buf_ptr,primary_nbns);

    if(buf_ptr == NULL)
    {
      DATA_ERR_FATAL("put32 returned NULL pointer");
      return -1; /* If ASSERT is disabled, -1 will fail call origination*/
    } 

  }
  if (((uint32) valid_fields) & ((uint32) NET_CFG_PRM_SEC_DNS_MASK)) {
    /*-------------------------------------------------------------------------
      Fill in the Type field for Secondary DNS - 131
    -------------------------------------------------------------------------*/
    *buf_ptr++ = IPCP_SECONDARY_DNS;
  
    /*-------------------------------------------------------------------------
      Fill in the length field for Secondary DNS
    -------------------------------------------------------------------------*/
    *buf_ptr++ = IPCP_SECONDARY_DNS_SIZ;
  
    /*-------------------------------------------------------------------------
     Fill in the Secondary DNS value(4 bytes) - Set it to the value requeste
    -------------------------------------------------------------------------*/
    buf_ptr = hput32(buf_ptr,secondary_dns);
  
    if(buf_ptr == NULL)
    {
      DATA_ERR_FATAL("put32 returned NULL pointer");
      return -1; /* If ASSERT is disabled, -1 will fail call origination*/
    } 
  }

  /*lint -e655*/
  if(valid_fields & NET_CFG_PRM_SEC_NBNS_MASK)
  /*lint +e655*/
  {
    /*-----------------------------------------------------------------------
    Fill in the Type field for Secondary NBNS - 132
    -----------------------------------------------------------------------*/
    *buf_ptr++ = IPCP_SECONDARY_NBNS;

    /*-----------------------------------------------------------------------
    Fill in the length field for Secondary NBNS
    -----------------------------------------------------------------------*/
    *buf_ptr++ = IPCP_SECONDARY_NBNS_SIZ;

    /*----------------------------------------------------------------------
     Fill in the Secondary NBNS value(4 bytes) - Set it to the value requeste
    -----------------------------------------------------------------------*/
    buf_ptr = hput32(buf_ptr,secondary_nbns);

    if(buf_ptr == NULL)
    {
      DATA_ERR_FATAL("put32 returned NULL pointer");
      return -1; /* If ASSERT is disabled, -1 will fail call origination*/
    } 

  }
  DS_3GPP_MSG1_LOW("DNS NBNS req PktLen %d",dns_pkt_len);
  return dns_pkt_len;

} /* dsumtsps_gen_ipcp_req_pkt() */
    
#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION dsumtsps_gen_ipv6_dns_req_pkt

DESCRIPTION
  This function generates a IPV6 DNS container packet to be sent in protocol
  config options.
  
PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to store V6 dns container. 
                        Not used in the function since spec only allows 
                        sending 0 length IPV6 DNS requests. Just kept the 
                        function so that ATCOP can use a generic API.
    
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS

===========================================================================*/
/*lint -esym(715, protocol_pkt_ptr)*/
int dsumtsps_gen_ipv6_dns_req_pkt
(
  const byte  *protocol_pkt_ptr /* Pointer to the buffer to store V6 dns container*/ 
)
{
  int       dns_pkt_len = 0;                      /* Length of IPCP packet */  

  /*-----------------------------------------------------------------------*/
  /*Since IPV6 DNS contents should be empty and Container length should be 
    0, no packet generation is needed. Just return a length of 0           */
  /*-----------------------------------------------------------------------*/ 

  return dns_pkt_len;
}
/*lint +esym(715, protocol_pkt_ptr)*/

#endif

#ifdef FEATURE_DATA_IMS
/*===========================================================================
FUNCTION DS3GPP_DECODE_V6_PCSCF_PKT

DESCRIPTION
  This function decodes the PCSCF packet received from the network. The
  pcscf addresses, if any sent by the network, are stored in ps_iface
  structure. Network sends only 1 IP address in 1 packet. 

PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract pcscf address from
    protocol_pkt_len  - Length of packet to extract pcscf address from
    pcscf_addr_array   - Array to store the pcscf addresses
    num_pcscf_addresses- Stores the total pcscf addresses processed
                        till now
     
    
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_decode_v6_pcscf_pkt
(
  const byte  *protocol_pkt_ptr, /* Pointer to the buffer to extract pcscf */
                                 /* addresses from                         */
  int   protocol_pkt_len,  /* Length of packet to extract pcscf addresse frm*/
  ps_ip_addr_type *pcscf_v4_addr_array,           /* v4 pcscf addresses received */
  uint32 *num_v4_pcscf_addresses, /* number of v4 pcscf addresses processed till now */
  ps_ip_addr_type *pcscf_v6_addr_array,           /* v6 pcscf addresses received */
  uint32 *num_v6_pcscf_addresses /* number of v6 pcscf addresses processed till now */
)
{
  boolean  status = FALSE;
  uint32    num_addr;
  struct ps_in_addr  v4_addr;
  struct ps_in6_addr v6_addr;
  /*-------------------------------------------------------------------------
    Network is updating the v6 PCSCF address to zero update same thing to the client
  -------------------------------------------------------------------------*/
  if(protocol_pkt_len  == 0)
  {
   /* address is v6, store it in the v6 array */

    num_addr = *num_v6_pcscf_addresses;
    if(num_addr >= MAX_PCSCF_ADDRESSES)
    {
      DS_3GPP_MSG0_HIGH("More than max pcscf addresses not supported");
      return status;
    }
    pcscf_v6_addr_array[num_addr].type = IPV6_ADDR;
    memscpy(pcscf_v6_addr_array[num_addr].addr.v6.ps_s6_addr,
            sizeof(pcscf_v6_addr_array[num_addr].addr.v6.ps_s6_addr),
	    protocol_pkt_ptr, sizeof(struct ps_in6_addr) );
    (*num_v6_pcscf_addresses)++;
	
    return TRUE;   
  }
    /*-------------------------------------------------------------------------
    Make sure the length of protocol packet is equal to IPV6 address 
  -------------------------------------------------------------------------*/
  if(protocol_pkt_len != DSUMTS_IPV6_ADDR_LEN)
  {
    DS_3GPP_MSG0_HIGH("Wrong pcscf address, incorrect length!");
    return status;
  }


  /*-----------------------------------------------------------------------
    Process only one pcscf addresses. If more than one pcscf addresses are
    received in context accept message, they would be in seperate
    containers. Extract the lower 64 bits and upper 64 bits into the IPv6
    IP addrdess array.
  -----------------------------------------------------------------------*/
  memscpy(&v6_addr, sizeof(struct ps_in6_addr), protocol_pkt_ptr, 
          sizeof(struct ps_in6_addr) );

  v4_addr.ps_s_addr = PS_IN6_GET_V4_FROM_V4_MAPPED_V6(&v6_addr);

  if( v4_addr.ps_s_addr == 0)
  {
    /* address is v6, store it in the v6 array */
    num_addr = *num_v6_pcscf_addresses;
    if(num_addr >= MAX_PCSCF_ADDRESSES)
    {
      DS_3GPP_MSG0_HIGH("More than max pcscf addresses not supported");
      return status;
    }
    pcscf_v6_addr_array[num_addr].type = IPV6_ADDR;
    memscpy(pcscf_v6_addr_array[num_addr].addr.v6.ps_s6_addr,
            sizeof(pcscf_v6_addr_array[num_addr].addr.v6.ps_s6_addr),
	    protocol_pkt_ptr, sizeof(struct ps_in6_addr) );
    (*num_v6_pcscf_addresses)++;
  }
  else
  {
    /* address is v4, store it in the v4 array */
    num_addr = *num_v4_pcscf_addresses;
    if(num_addr >= MAX_PCSCF_ADDRESSES)
    {
      DS_3GPP_MSG0_HIGH("More than max pcscf addresses not supported");
      return status;
    }
    /* Copy the adress to v4 format */
    pcscf_v4_addr_array[num_addr].type = IPV4_ADDR;
    pcscf_v4_addr_array[num_addr].addr.v4 = v4_addr;
    (*num_v4_pcscf_addresses)++;
  }

  status = TRUE;
  return status;
}

/*===========================================================================
FUNCTION DS3GPP_DECODE_V4_PCSCF_PKT

DESCRIPTION
  This function decodes the PCSCF packet received from the network. The
  pcscf addresses, if any sent by the network, are stored in ps_iface
  structure. Network sends only 1 IP address in 1 packet. 

PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract pcscf address from
    protocol_pkt_len  - Length of packet to extract pcscf address from
    pcscf_addr_array   - Array to store the pcscf addresses
    num_pcscf_addresses- Stores the total pcscf addresses processed
                        till now
     
    
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean ds3gpp_decode_v4_pcscf_pkt
(
  const byte  *protocol_pkt_ptr, /* Pointer to the buffer to extract pcscf */
                                 /* addresses from                         */
  int   protocol_pkt_len,  /* Length of packet to extract pcscf addresse frm*/
  ps_ip_addr_type *pcscf_v4_addr_array,           /* v4 pcscf addresses received */
  uint32 *num_v4_pcscf_addresses /* number of v4 pcscf addresses processed till now */
)
{
  boolean  status = FALSE;
  uint32    num_addr;
  struct ps_in_addr  v4_addr;
  /*-------------------------------------------------------------------------
    Network is updating the v4 P-CSCF address to zero update same thing to the client
  -------------------------------------------------------------------------*/

  if(protocol_pkt_len  == 0)
  {
   /* address is v4, store it in the v4 array */
    num_addr = *num_v4_pcscf_addresses;
    if(num_addr >= MAX_PCSCF_ADDRESSES)
    {
      DS_3GPP_MSG0_HIGH("More than max pcscf addresses not supported");
      return status;
    }
   memscpy(&v4_addr, sizeof(struct ps_in_addr), protocol_pkt_ptr, 
          sizeof(struct ps_in_addr) );
    pcscf_v4_addr_array[num_addr].type = IPV4_ADDR;
    pcscf_v4_addr_array[num_addr].addr.v4 = v4_addr;
    (*num_v4_pcscf_addresses)++;

    return TRUE;  
  }
  /*-------------------------------------------------------------------------
    Make sure the length of protocol packet is equal to IPV6 address 
  -------------------------------------------------------------------------*/
  if(protocol_pkt_len != DSUMTS_IPV4_ADDR_LEN)
  {
    DS_3GPP_MSG0_HIGH("Wrong pcscf address, incorrect v4 length!");
    return status;
  }


  /*-----------------------------------------------------------------------
    Process only one pcscf addresses. If more than one pcscf addresses are
    received in context accept message, they would be in seperate
    containers. Extract the lower 64 bits and upper 64 bits into the IPv6
    IP addrdess array.
  -----------------------------------------------------------------------*/
  memscpy(&v4_addr, sizeof(struct ps_in_addr), protocol_pkt_ptr, 
          sizeof(struct ps_in_addr) );

  if( v4_addr.ps_s_addr != 0)
  {
    num_addr = *num_v4_pcscf_addresses;
    if(num_addr >= MAX_PCSCF_ADDRESSES)
    {
      DS_3GPP_MSG0_HIGH("More than max pcscf addresses not supported");
      return status;
    }
    pcscf_v4_addr_array[num_addr].type = IPV4_ADDR;
    pcscf_v4_addr_array[num_addr].addr.v4 = v4_addr;
    (*num_v4_pcscf_addresses)++;
  }
  else
  {
    DS_3GPP_MSG0_HIGH("NULL v4 address in PCO");
    return status;
  }

  status = TRUE;
  return status;
}

/*===========================================================================
FUNCTION DSUMTSPS_DECODE_PCSCF_PKT

DESCRIPTION
  This function decodes the PCSCF packet received from the network. The
  pcscf addresses, if any sent by the network, are stored in ps_iface
  structure. Network sends only 1 IP address in 1 packet. 

PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract pcscf address from
    protocol_pkt_len  - Length of packet to extract pcscf address from
    pcscf_addr_array   - Array to store the pcscf addresses
    num_pcscf_addresses- Stores the total pcscf addresses processed
                        till now
     
    
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  None.

===========================================================================*/
boolean dsumtsps_decode_pcscf_pkt
(
  const byte  *protocol_pkt_ptr, /* Pointer to the buffer to extract pcscf        */
                           /* addresses from                               */
  int   protocol_pkt_len,  /* Length of packet to extract pcscf addresse frm*/
  ps_ip_addr_type *pcscf_addr_array,           /* pcscf addresses received */
  uint32 *num_pcscf_addresses/* number of pcscf addresses processed till now */
)
{
  boolean  status = FALSE;
  uint32    num_addr;
  struct ps_in_addr v4_addr;
  /*-------------------------------------------------------------------------
    Make sure the length of protocol packet is equal to IPV6 address 
  -------------------------------------------------------------------------*/
  /*lint -save -e572 suppressing error 572*/
  if(protocol_pkt_len != DSUMTS_IPV6_ADDR_LEN)
  {
    DS_3GPP_MSG0_HIGH("Wrong pcscf address");
    return status;
  }

  num_addr = *num_pcscf_addresses;

  if(num_addr >= MAX_PCSCF_ADDRESSES)
  {
    DS_3GPP_MSG0_HIGH("More than max pcscf addresses not supported");
    return status;
  }

  /*-----------------------------------------------------------------------
    Process only one pcscf addresses. If more than one pcscf addresses are
    received in context accept message, they would be in seperate
    containers. Extract the lower 64 bits and upper 64 bits into the IPv6
    IP addrdess array.
  -----------------------------------------------------------------------*/
  memscpy(pcscf_addr_array[num_addr].addr.v6.ps_s6_addr, 
          sizeof(pcscf_addr_array[num_addr].addr.v6.ps_s6_addr),
          protocol_pkt_ptr, sizeof(struct ps_in6_addr) );

  v4_addr.ps_s_addr = PS_IN6_GET_V4_FROM_V4_MAPPED_V6(
    pcscf_addr_array[num_addr].addr.v6.ps_s6_addr);

  if( v4_addr.ps_s_addr == 0)
  {
    pcscf_addr_array[num_addr].type = IPV6_ADDR;
  }
  else
  {
    /* Copy the adress to v4 format */
    pcscf_addr_array[num_addr].type = IPV4_ADDR;
    pcscf_addr_array[num_addr].addr.v4 = v4_addr;
  }

  (*num_pcscf_addresses)++;

  status = TRUE;
  return status;
  /*lint -restore Restore lint error 572*/
}
#endif // FEATURE_DATA_IMS


/*===========================================================================
FUNCTION DS3GPP_DECODE_IPCP_PKT

DESCRIPTION
  This function decodes the IPCP packet received from the network. The only
  Codes that are processed are:
  CONFIG_NAK, CONFIG_REJ and CONFIG_ACK. All other options are discarded.
  The IPCP options that are decoded are:
    - Primary DNS Address
    - Secondary DNS address
  All other options are ignored.
  The identifer that was received must match the identifier that was sent in
  the PDP context activation request.
  
PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract ipcp options from
    protocol_pkt_len  - Length of packet to extract ipcp options from
    ipcp_options      - IPCP options received
     
    
DEPENDENCIES 
  None.

RETURN VALUE
  DS3G_SUCCESS : if successful
  DS3G_FAILURE : on error
  
SIDE EFFECTS
  ipcp_options->primary_dns and ipcp_options->secondary_dns might be
  modified by this function, if valid values are received.

===========================================================================*/
boolean ds3gpp_decode_ipcp_pkt
(
  const byte *protocol_pkt_ptr,         /* Pointer to the buffer to        */
                                        /* extract options from            */
  int   protocol_pkt_len,               /* Length of packet to extract     */
                                        /* ipcp options from               */
  ds3gpp_ipcpv4_options_s *ipcp_options /* IPCP options received           */  

)
{
  return dsumtsps_decode_ipcp_pkt(
                                  protocol_pkt_ptr,
                                  protocol_pkt_len,
                                  ipcp_options
                                  );
} /* ds3gpp_decode_ipcp_pkt() */

/*===========================================================================
FUNCTION DSUMTSPS_DECODE_IPCP_PKT

DESCRIPTION
  This function decodes the IPCP packet received from the network. The only
  Codes that are processed are:
  CONFIG_NAK, CONFIG_REJ and CONFIG_ACK. All other options are discarded.
  The IPCP options that are decoded are:
    - Primary DNS Address
    - Secondary DNS address
  All other options are ignored.
  The identifer that was received must match the identifier that was sent in
  the PDP context activation request.
  
PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract ipcp options from
    protocol_pkt_len  - Length of packet to extract ipcp options from
    ipcp_options      - IPCP options received
     
    
DEPENDENCIES 
  None.

RETURN VALUE
  DS3G_SUCCESS : if successful
  DS3G_FAILURE : on error
  
SIDE EFFECTS
  ipcp_options->primary_dns and ipcp_options->secondary_dns might be
  modified by this function, if valid values are received.

===========================================================================*/
boolean dsumtsps_decode_ipcp_pkt
(
  const byte *protocol_pkt_ptr, /* Pointer to the buffer to extract ipcp   */
                                /* options from                            */
  int   protocol_pkt_len,  /* Length of packet to extract ipcp options from*/
  dsumtsps_ipcp4_options *ipcp_options             /* IPCP options received */
)
{
   dword dns_val = 0;
   word length;                                   /* Length of IPCP packet */
   byte config_type;                 /* Identifies the kind of IPCP packet */
   byte identifier;                               /* Identifies the packet */
   byte opt_type;                                           /* Option Type */
   byte opt_len;                                          /* Option length */
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  while (protocol_pkt_len > 0) 
  {
    /*-----------------------------------------------------------------------
      Get the Message Type.     
    -----------------------------------------------------------------------*/
    config_type = *protocol_pkt_ptr++;

    /*-----------------------------------------------------------------------
      Get the identifier value. 
    -----------------------------------------------------------------------*/
    identifier = *protocol_pkt_ptr++;
       
    /*-----------------------------------------------------------------------
      Get the length of the packet.
    -----------------------------------------------------------------------*/
    length = get16((byte *)protocol_pkt_ptr);
    protocol_pkt_ptr += 2;

    if (protocol_pkt_len < length) 
    {
      DATA_MSG0_ERROR("Invalid IPCP pkt length");
      return DS3G_FAILURE;
    }

    protocol_pkt_len -= length;

    if (length < CONFIG_HDR_LEN) 
    {
       DATA_MSG0_ERROR("Invalid IPCP pkt length");
       return DS3G_FAILURE;
    }
    /*-----------------------------------------------------------------------
      Subtract CONFIG_HDR_LEN from total length.
    -----------------------------------------------------------------------*/
    length -= CONFIG_HDR_LEN;

    /*-----------------------------------------------------------------------
      Based on the type of message, do appropriate processing.
      CONFIG_NAK/CONFIG_ACK/CONFIG_REJ are the only types processed. All 
      others are silently discarded.
    -----------------------------------------------------------------------*/
    switch (config_type) 
    {
      case CONFIG_REQ:
        
        DS_3GPP_MSG0_HIGH(" CONFIG_REQ IPCP packet recvd");
        /*-------------------------------------------------------------------
          Received a CONFIG_REQ. Only Gatewary IP Address is processed in
          the CONFIG_REQ packet received from the network.
        -------------------------------------------------------------------*/        
        while (length) 
        {
          /*-----------------------------------------------------------------
            Get the option type.
          -----------------------------------------------------------------*/
          opt_type = *protocol_pkt_ptr++;

          /*-----------------------------------------------------------------
            Get the option length.
          -----------------------------------------------------------------*/
          opt_len = *protocol_pkt_ptr++;

          /*-----------------------------------------------------------------
            If remaining IPCP pkt Length is less than the option length, 
            it is an error. Return.
          -----------------------------------------------------------------*/
          if (length < opt_len) 
          {
            DATA_MSG0_ERROR("Invalid IPCP pkt Len");
            return DS3G_FAILURE;
          }

          /*-----------------------------------------------------------------
            If option length is zero, the packet is bad. Return to avoid  
            entering an infinite loop.
          -----------------------------------------------------------------*/
          if (opt_len == 0) 
          {
            DATA_MSG0_ERROR("Zero option length in IPCP pkt");
            return DS3G_FAILURE;
          }

          switch (opt_type)
          {
            case IPCP_1ADDRESS:
              /*-------------------------------------------------------------
                If the option is IPCP_1ADDRESS, and the length does not equal
                the address size(6), return. Otherwise, get the address.
              -------------------------------------------------------------*/
              if (opt_len != IPCP_1ADDRESS_SIZ) 
              {
                DS_3GPP_MSG1_HIGH("Invalid IPCP ADDRESS Option Len %d",opt_len);
                return DS3G_FAILURE;
              }
              else
              {
                ipcp_options->gateway_addr = get32((byte *)protocol_pkt_ptr);
                protocol_pkt_ptr +=4;
                length -= opt_len;
              }
              break;

            default:
              DS_3GPP_MSG1_HIGH("Option type not supported %d",opt_type);
              protocol_pkt_ptr += (opt_len - 2);
              length -= opt_len;
          }
        } /* while length */
        break;

      case CONFIG_NAK:

         DS_3GPP_MSG0_HIGH( "CONFIG NAK IPCP packet recvd.");
        /*-------------------------------------------------------------------
          Check if the identifer matches the value stored in the call table. 
          If not, discard the packet.
        -------------------------------------------------------------------*/
        if (identifier != ipcp_options->identifier) 
        {
          DS_3GPP_MSG1_HIGH("Invalid Identifier received %d",identifier);
          protocol_pkt_ptr += length;
          continue;
        }

        /*-------------------------------------------------------------------
          Received a CONFIG_NAK. The option values are different from the 
          ones requested in the PDP context activation request message.
        -------------------------------------------------------------------*/        
        while (length) 
        {
          /*-----------------------------------------------------------------
            Get the option type.
          -----------------------------------------------------------------*/
          opt_type = *protocol_pkt_ptr++;

          /*-----------------------------------------------------------------
            Get the option length.
          -----------------------------------------------------------------*/
          opt_len = *protocol_pkt_ptr++;

          /*-----------------------------------------------------------------
            If remaining IPCP pkt Length is less than the option length, 
            it is an error. Return.
          -----------------------------------------------------------------*/
          if (length < opt_len) 
          {
            DATA_MSG0_ERROR("Invalid IPCP pkt Len");
            return DS3G_FAILURE;
          }

          /*-----------------------------------------------------------------
            If option length is zero, the packet is bad. Return to avoid  
            entering an infinite loop.
          -----------------------------------------------------------------*/
          if (opt_len == 0) 
          {
            DATA_MSG0_ERROR("Zero option length in IPCP pkt");
            return DS3G_FAILURE;
          }

          switch (opt_type)
          {
            case IPCP_PRIMARY_DNS:
              /*-------------------------------------------------------------
                If the option is PRIMARY DNS, and the length does not equal
                the PRIMARY DNS size(6), return. Otherwise, get the 
                primary DNS address.
              -------------------------------------------------------------*/
              if (opt_len != IPCP_PRIMARY_DNS_SIZ) 
              {
                DS_3GPP_MSG1_HIGH("Invalid Pri DNS Option Len %d",opt_len);
                return DS3G_FAILURE;
              }
              else
              {
                ipcp_options->primary_dns = get32((byte *)protocol_pkt_ptr);
                protocol_pkt_ptr +=4;
                length -= opt_len;
                DS_3GPP_MSG1_HIGH("CONFIG NACK: Received PDNS %lx",
                         (unsigned long)ipcp_options->primary_dns);
              }
              break;

            case IPCP_PRIMARY_NBNS:
              /*-------------------------------------------------------------
                If the option is PRIMARY NBNS, and the length does not equal
                the PRIMARY NBNS size(6), return. Otherwise, get the 
                primary NBNS address.
              -------------------------------------------------------------*/
              if (opt_len != IPCP_PRIMARY_NBNS_SIZ)
              {
                DS_3GPP_MSG1_HIGH("Invalid Pri NBNS Option Len %d",opt_len);
                return DS3G_FAILURE;
              }
              else
              {
                ipcp_options->primary_nbns = get32((byte *)protocol_pkt_ptr);
                protocol_pkt_ptr +=4;
                length -= opt_len;
                DS_3GPP_MSG1_HIGH("CONFIG NACK: Received PNBNS %lx",
                         (unsigned long)ipcp_options->primary_nbns);
              }
              break;

            case IPCP_SECONDARY_NBNS:
              /*-------------------------------------------------------------
                If the option is SECONDARY NBNS, and the length does not equal
                the SECONDARY NBNS size (6), return. Otherwise, get the 
                secondary NBNS address.
              -------------------------------------------------------------*/
              if (opt_len != IPCP_SECONDARY_NBNS_SIZ)
              {
                DS_3GPP_MSG1_HIGH("Invalid Sec NBNS Option Len %d",opt_len);
                return DS3G_FAILURE;
              }
              else
              {
                ipcp_options->secondary_nbns = get32((byte *)protocol_pkt_ptr);
                protocol_pkt_ptr +=4;
                length -= opt_len;
                DS_3GPP_MSG1_HIGH("CONFIG NACK: Received SNBNS %lx",
                         (unsigned long)ipcp_options->secondary_nbns);
              }
            break;

            case IPCP_SECONDARY_DNS:
              /*-------------------------------------------------------------
                If the option is SECONDARY DNS, and the length does not equal
                the SECONDARY DNS size (6), return. Otherwise, get the 
                secondary DNS address.
              -------------------------------------------------------------*/
              if (opt_len != IPCP_SECONDARY_DNS_SIZ) 
              {
                DS_3GPP_MSG1_HIGH("Invalid Sec DNS Option Len %d",opt_len);
                return DS3G_FAILURE;
              }
              else
              {
                ipcp_options->secondary_dns = get32((byte *)protocol_pkt_ptr);
                protocol_pkt_ptr +=4;
                length -= opt_len;
                DS_3GPP_MSG1_HIGH("CONFIG NACK: Received SDNS %lx",
                         (unsigned long)ipcp_options->secondary_dns);
              }
            break;

            default:
              DS_3GPP_MSG1_HIGH("Option type not supported %d",opt_type);
              protocol_pkt_ptr += (opt_len - 2);
              length -= opt_len;
          }
        } /* while length */
        break;

      case CONFIG_ACK:
        
        DS_3GPP_MSG0_HIGH( "CONFIG ACK IPCP packet recvd. ");
        /*-------------------------------------------------------------------
          Check if the identifer matches the value stored in the call table. 
          If not, discard the packet.
        -------------------------------------------------------------------*/
        if (identifier != ipcp_options->identifier) 
        {
          DS_3GPP_MSG1_HIGH("Invalid Identifier received %d",identifier);
          protocol_pkt_ptr += length;
          continue;
        }

        /*-------------------------------------------------------------------
          Received a CONFIG_ACk. The option values must match the ones sent 
          in the PDP context activation message.
        -------------------------------------------------------------------*/        
        while (length) 
        {
          /*-----------------------------------------------------------------
            Get the option type.
          -----------------------------------------------------------------*/
          opt_type = *protocol_pkt_ptr++;

          /*-----------------------------------------------------------------
            Get the option length.
          -----------------------------------------------------------------*/
          opt_len = *protocol_pkt_ptr++;

          /*-----------------------------------------------------------------
            If remaining IPCP pkt Length is less than the option length, 
            it is an error. Return.
          -----------------------------------------------------------------*/
          if (length < opt_len) 
          {
            DATA_MSG0_ERROR("Invalid IPCP pkt Len");
            return DS3G_FAILURE;
          }

          /*-----------------------------------------------------------------
            If option length is zero, the packet is bad. Return to avoid  
            entering an infinite loop.
          -----------------------------------------------------------------*/
          if (opt_len == 0) 
          {
            DATA_MSG0_ERROR("Zero option length in IPCP pkt");
            return DS3G_FAILURE;
          }

          switch (opt_type)
          {
            case IPCP_PRIMARY_DNS: 
              /*-------------------------------------------------------------
                If the option is PRIMARY DNS, and the length does not equal
                the PRIMARY DNS size(6), return. Otherwise, get the primary
                DNS address.
              -------------------------------------------------------------*/
              if (opt_len != IPCP_PRIMARY_DNS_SIZ) 
              {
                DS_3GPP_MSG1_HIGH("Invalid Pri DNS Option Len %d",opt_len);
                return DS3G_FAILURE;
              }
              else
              {
                if ((dns_val = get32((byte *)protocol_pkt_ptr)) != 0)
                {
                  DS_3GPP_MSG1_HIGH("CONFIG ACK: Received non-zero PDNS %lx",
                                                 (unsigned long)dns_val);
                  ipcp_options->primary_dns = dns_val;
                }
                protocol_pkt_ptr +=4;
                length -= opt_len;
              }
            break;

            case IPCP_PRIMARY_NBNS:
              /*-------------------------------------------------------------
                If the option is PRIMARY NBNS, and the length does not equal
                the PRIMARY NBNS size(6), return. Otherwise, get the primary
                NBNS address.
              -------------------------------------------------------------*/
              if (opt_len != IPCP_PRIMARY_NBNS_SIZ)
              {
                DS_3GPP_MSG1_HIGH("Invalid Pri NBNS Option Len %d",opt_len);
                return DS3G_FAILURE;
              }
              else
              {
                if ((dns_val = get32((byte *)protocol_pkt_ptr)) != 0)
                {
                  DS_3GPP_MSG1_HIGH("CONFIG ACK: Received non-zero PNBNS %lx",
                                                 (unsigned long)dns_val);
                  ipcp_options->primary_nbns = dns_val;
                }
                protocol_pkt_ptr +=4;
                length -= opt_len;
              }
            break;


            case IPCP_SECONDARY_NBNS:
              /*-------------------------------------------------------------
                If the option is SECONDARY NBNS, and the length does not equal
                the SECONDARY NBNS size (6), return. Otherwise, get the 
                secondary NBNS address.
              -------------------------------------------------------------*/
              if (opt_len != IPCP_SECONDARY_NBNS_SIZ)
              {
                DS_3GPP_MSG1_HIGH("Invalid Sec NBNS Option Len %d",opt_len);
                return DS3G_FAILURE;
              }
              else
              {
                if ((dns_val = get32((byte *)protocol_pkt_ptr)) != 0)
                {
                  DS_3GPP_MSG1_HIGH("CONFIG ACK: Received non-zero SNBNS %lx",
                                                 (unsigned long)dns_val);

                  ipcp_options->secondary_nbns = dns_val;
                }
                protocol_pkt_ptr +=4;
                length -= opt_len;
              }
            break;

            case IPCP_SECONDARY_DNS:
              /*-------------------------------------------------------------
                If the option is SECONDARY DNS, and the length does not equal
                the SECONDARY DNS size (6), return. Otherwise, get the 
                secondary DNS address.
              -------------------------------------------------------------*/
              if (opt_len != IPCP_SECONDARY_DNS_SIZ) 
              {
                DS_3GPP_MSG1_HIGH("Invalid Sec DNS Option Len %d",opt_len);
                return DS3G_FAILURE;
              }
              else
              {
                if ((dns_val = get32((byte *)protocol_pkt_ptr)) != 0)
                {
                  DS_3GPP_MSG1_HIGH("CONFIG ACK: Received non-zero SDNS %lx",
                                                 (unsigned long)dns_val);
                  ipcp_options->secondary_dns = dns_val;
                }
                protocol_pkt_ptr +=4;
                length -= opt_len;
              }
            break;

            default:
              DS_3GPP_MSG1_HIGH("Option type not supported %d",opt_type);
              protocol_pkt_ptr += (opt_len - 2);
              length -= opt_len;
          }
        } /* while (length) */
        break;

      case CONFIG_REJ:

        DS_3GPP_MSG0_HIGH( "CONFIG REJ IPCP packet recvd. -Ignore");
        /*-------------------------------------------------------------------
          Check if the identifer matches the value stored in the call table. 
          If not, discard the packet.
        -------------------------------------------------------------------*/
        if (identifier != ipcp_options->identifier) 
        {
          DS_3GPP_MSG1_HIGH("Invalid Identifier received %d",identifier);
          protocol_pkt_ptr += length;
          continue;
        }
        /*-------------------------------------------------------------------
          Received a CONFIG_REJ. Use the defaults stored in the
          call specific table.
        -------------------------------------------------------------------*/
        while (length) 
        {
          /*-----------------------------------------------------------------
            Get the option type.
          -----------------------------------------------------------------*/
          opt_type = *protocol_pkt_ptr++;
          DS_3GPP_MSG1_HIGH("CONFIG_REJ: Opt Type : %d", opt_type);

          /*-----------------------------------------------------------------
            Get the option length.
          -----------------------------------------------------------------*/
          opt_len = *protocol_pkt_ptr++;

          /*-----------------------------------------------------------------
            If remaining IPCP pkt Length is less than the option length, 
            it is an error. Return.
          -----------------------------------------------------------------*/
          if (length < opt_len) 
          {
            DATA_MSG0_ERROR("Invalid IPCP pkt Len");
            return DS3G_FAILURE;
          }

          /*-----------------------------------------------------------------
            If option length is zero, the packet is bad. Return to avoid  
            entering an infinite loop.
          -----------------------------------------------------------------*/
          if (opt_len == 0) 
          {
            DATA_MSG0_ERROR("Zero option length in IPCP pkt");
            return DS3G_FAILURE;
          }

          length -= opt_len;
          protocol_pkt_ptr += (opt_len - 2);
        }
        break;

      default:
        DS_3GPP_MSG1_HIGH("Invalid IPCP code : Ignore it ",config_type);
        protocol_pkt_ptr += length;

    } /* switch (type) */

  } /* while (protocol_pkt_len) */

  return DS3G_SUCCESS;
} /* dsumtsps_decode_ipcp_pkt() */

/*===========================================================================
FUNCTION ds3gpp_decode_ipv4_pkt

DESCRIPTION
  This function decodes the V4 dns container received from the network. 
  Only one dns address is decoded. If more than one dns address is sent
  by the network, they are ignored.
  
PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract V4 dns addresses 
                        from
    protocol_pkt_len  - Length of packet to extract V4 dns addresses from 
    ipv4cp_options      - IPCP options received
    num_ipv4_dns      - Number of V4 dns addresses already processed
     
    
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  ipcp_options4->primary_dns and ipcp_options4->secondary_dns might be
  modified by this function, if valid values are received.

===========================================================================*/
void ds3gpp_decode_ipv4_pkt
(
  const byte *protocol_pkt_ptr, /* Pointer to the buffer to extract dns    */
                                /* addresses from                          */
  int   protocol_pkt_len,  /* Length of packet to extract dsn addresses from*/
  ds3gpp_ipcpv4_options_s *ipv4cp_options,  /* ipv4cp options received */
  uint32 *num_ipv4_dns       /*Number of V4 dns addresses already processed   */
)
{
  dsumtsps_decode_ipv4_pkt(
     protocol_pkt_ptr,
     protocol_pkt_len,
     ipv4cp_options,
     num_ipv4_dns);  
}

/*===========================================================================
FUNCTION dsumtsps_decode_ipv4_pkt

DESCRIPTION
  This function decodes the V4 dns container received from the network. 
  Only one dns address is decoded. If more than one dns address is sent
  by the network, they are ignored.
  
PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract V4 dns addresses 
                        from
    protocol_pkt_len  - Length of packet to extract V4 dns addresses from 
    ipv4cp_options      - IPCP options received
    num_ipv4_dns      - Number of V4 dns addresses already processed
     
    
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  ipcp_options4->primary_dns and ipcp_options4->secondary_dns might be
  modified by this function, if valid values are received.

===========================================================================*/
void dsumtsps_decode_ipv4_pkt
(
  const byte *protocol_pkt_ptr, /* Pointer to the buffer to extract dns    */
                                /* addresses from                          */
  int   protocol_pkt_len,  /* Length of packet to extract dsn addresses from*/
  dsumtsps_ipcp4_options *ipv4cp_options,  /* ipv4cp options received */
  uint32 *num_ipv4_dns       /*Number of V4 dns addresses already processed   */
)
{
  uint32 dns_addr = 0;
  
  if(protocol_pkt_ptr == NULL || num_ipv4_dns == NULL || 
                                      ipv4cp_options == NULL)
  {
    DS_3GPP_MSG0_HIGH("Invalid arguments while decoding ipcp options");
    return;
  }
  if(protocol_pkt_len != DSUMTS_IPV4_ADDR_LEN)
  {
    DS_3GPP_MSG0_HIGH("Wrong IPV4 DNS Address");
    return;
  }

  DS_3GPP_MSG1_MED("Number of IPv4 DNS address in container: %d", *num_ipv4_dns);
  /*-----------------------------------------------------------------------
    Process only two DNS addressess for IPV4. If more than two DNS 
    addresses are received in context accept message, store two addresses
    and ignore rest. 
  -----------------------------------------------------------------------*/
    
  if(*num_ipv4_dns == 0)
  {
    if( (dns_addr = get32((byte *)protocol_pkt_ptr)) != 0 )
    {
      ipv4cp_options->primary_dns = dns_addr;
      DS_3GPP_MSG1_HIGH("Decoded primary v4 DNS address %x",dns_addr);
    }
    (*num_ipv4_dns)++;
  }
  else if(*num_ipv4_dns == 1)
  {
    if( (dns_addr = get32((byte *)protocol_pkt_ptr)) != 0 )
    {
      ipv4cp_options->secondary_dns = dns_addr;
      DS_3GPP_MSG1_HIGH("Decoded secondary v4 DNS address %x",dns_addr);
    }
    (*num_ipv4_dns)++;
  }    
  else 
  {
    DS_3GPP_MSG0_HIGH("More Than Two IPv4 DNS addresses present, Ignoring");
  }
}

#ifdef FEATURE_DATA_PS_IPV6
/*===========================================================================
FUNCTION ds3gpp_decode_ipv6_pkt

DESCRIPTION
  This function decodes the V6 dns container received from the network. 
  Only two dns addresses are decoded. If more than two dns address is sent
  by the network, they are ignored.
  
PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract V6 dns addresses 
                        from
    protocol_pkt_len  - Length of packet to extract V6 dns addresses from 
    ipv6cp_options      - IPCP options received
    num_ipv6_dns      - Number of V6 dns addresses already processed
     
    
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  ipcp_options6->primary_dns and ipcp_options6->secondary_dns might be
  modified by this function, if valid values are received.

===========================================================================*/
void ds3gpp_decode_ipv6_pkt
(
  const byte *protocol_pkt_ptr, /* Pointer to the buffer to extract dns    */
                                /* addresses from                          */
  int   protocol_pkt_len,  /* Length of packet to extract dsn addresses from*/
  ds3gpp_ipv6cp_options_s *ipv6cp_options,  /* ipv6cp options received */
  int *num_ipv6_dns       /*Number of V6 dns addresses already processed   */
)
{
  dsumtsps_decode_ipv6_pkt(
     protocol_pkt_ptr,
     protocol_pkt_len,
     ipv6cp_options,
     num_ipv6_dns);   
}

/*===========================================================================
FUNCTION dsumtsps_decode_ipv6_pkt

DESCRIPTION
  This function decodes the V6 dns container received from the network. 
  Only two dns addresses are decoded. If more than two dns address is sent
  by the network, they are ignored.
  
PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract V6 dns addresses 
                        from
    protocol_pkt_len  - Length of packet to extract V6 dns addresses from 
    ipv6cp_options      - IPCP options received
    num_ipv6_dns      - Number of V6 dns addresses already processed
     
    
DEPENDENCIES 
  None.

RETURN VALUE
  None.
  
SIDE EFFECTS
  ipcp_options6->primary_dns and ipcp_options6->secondary_dns might be
  modified by this function, if valid values are received.

===========================================================================*/
void dsumtsps_decode_ipv6_pkt
(
  const byte *protocol_pkt_ptr, /* Pointer to the buffer to extract dns    */
                                /* addresses from                          */
  int   protocol_pkt_len,  /* Length of packet to extract dsn addresses from*/
  dsumtsps_ipv6cp_options *ipv6cp_options,  /* ipv6cp options received */
  int *num_ipv6_dns       /*Number of V6 dns addresses already processed   */
)
{
  if(protocol_pkt_len != DSUMTS_IPV6_ADDR_LEN)
  {
    DS_3GPP_MSG0_HIGH("Wrong IPV6 DNS Address");
    return;
  }

  /*-----------------------------------------------------------------------
    Process only two DNS addressess for IPV6. If more than two DNS 
    addresses are received in context accept message, store two addresses
    and ignore rest. 
  -----------------------------------------------------------------------*/
    
  if(*num_ipv6_dns == 0)
  {
    memscpy(ipv6cp_options->primary_dns.ps_s6_addr, sizeof(struct ps_in6_addr), 
            protocol_pkt_ptr, sizeof(struct ps_in6_addr) );
    (*num_ipv6_dns)++;
  }
  else if(*num_ipv6_dns == 1)
  {
    memscpy(ipv6cp_options->secondary_dns.ps_s6_addr, sizeof(struct ps_in6_addr),
            protocol_pkt_ptr, sizeof(struct ps_in6_addr) );
    (*num_ipv6_dns)++;
  }    
  else 
  {
    DS_3GPP_MSG0_HIGH("More Thans Two IPv6 DNS addresses present, Ignoring");
  }
}

#endif /* FEATURE_DATA_PS_IPV6 */

/*===========================================================================
FUNCTION DS3GPP_DECODE_OPERATOR_RESERVED_PCO_PKT

DESCRIPTION
  This function decodes the operator reserved PCO received from the network.
  
PARAMETERS   
    protocol_pkt_ptr  - Pointer to the buffer to extract ipcp options from
    protocol_pkt_len  - Length of packet to extract ipcp options from
  ..............
     
    
DEPENDENCIES 
  None.

RETURN VALUE
  DS3G_SUCCESS : if successful
  DS3G_FAILURE : on error
  
SIDE EFFECTS
  
===========================================================================*/
boolean ds3gpp_decode_operator_reserved_pco_pkt
(
  const byte           *protocol_pkt_ptr,/* Pointer to the buffer to   */
                             /* extract options from     */
  int                  protocol_pkt_len,/* Length of packet to extract */
                            /* operator's mcc, mnc info from               */
  ps_iface_type      *iface_v4_p,
  ps_iface_type      *iface_v6_p,
  uint16             protocol_id,
  boolean            profile_pco_id_configured
)
{
  uint16 mcc;
  uint16 mnc;
  ps_iface_op_reserved_pco_type    operator_reserved_pco_info;
  ps_iface_modem_reserved_pco_type modem_op_reserved_pco_info;

  /*----------------------------------------------------------------------- 
    Currently the num of pco_containers is always 1. There could be a
    future case where num_pco_containers can increase. We will need to make
    appropriate changes to allocate memory then.
  -------------------------------------------------------------------------*/
  
  //uint16 num_pco_containers = 1;

  if(protocol_pkt_ptr == NULL  )
  {
    DS_3GPP_MSG0_HIGH("protocol_pkt_ptr is Nulll ");
    return FALSE;
  }

  if(protocol_pkt_len < MCC_MNC_LENGTH || protocol_pkt_len > MAX_PROTOCOL_ID_CONTENT )
  {
    DS_3GPP_MSG1_HIGH("Invalid decoding operator reserved pco options- protocol pkt length %d",
                      protocol_pkt_len);
    return FALSE;
  }

/* Lower Byte Set  is 0x0F UPPER_BYTE_SET is 0xF0;
   Decode mcc mnc */
 
/*  3 Octets: mcc/mnc as part of Pco contents:"
     protocol_pkt_ptr[0]:-   mcc_digit_2  mcc_digit_1
     protocol_pkt_ptr[1]:-   mnc_digit_3  mcc_digit_3
     protocol_pkt_ptr[2]:-   mnc_digit_2  mnc_digit_1
*/
  
   
  mcc = (protocol_pkt_ptr[1] & LOWER_BYTE_SET)                  // mcc_3
      + (((protocol_pkt_ptr[0] & UPPER_BYTE_SET) >> 4) * 10)    // mcc_2
      + (protocol_pkt_ptr[0] & LOWER_BYTE_SET) * 100;           // mcc_1


  /*  
     Here mnc_digit_3 is checked for 0x0F; if so we interpret
    mnc as two digit and set mnc_includes_pcs_digi to FALSE else TRUE
  */


  if((protocol_pkt_ptr[1] & UPPER_BYTE_SET) !=UPPER_BYTE_SET)
  {

    mnc = ((protocol_pkt_ptr[1] & UPPER_BYTE_SET) >> 4)            // mnc_3
           + (((protocol_pkt_ptr[2] & UPPER_BYTE_SET) >> 4) * 10)   // mnc_2
           + ((protocol_pkt_ptr[2] & LOWER_BYTE_SET) * 100);    // mnc_1
  }
  else{
     mnc = ((protocol_pkt_ptr[2] & UPPER_BYTE_SET) >> 4)      // mnc_2
           + ((protocol_pkt_ptr[2] & LOWER_BYTE_SET) * 10);    // mnc_1

  }
   
  protocol_pkt_len -= MCC_MNC_LENGTH;


  memset(&operator_reserved_pco_info,0,sizeof(ps_iface_op_reserved_pco_type));


  operator_reserved_pco_info.mnc_includes_pcs_digit=FALSE;

  if((protocol_pkt_ptr[1] & UPPER_BYTE_SET) !=UPPER_BYTE_SET)
  {
    operator_reserved_pco_info.mnc_includes_pcs_digit=TRUE;
  }

  operator_reserved_pco_info.mcc = mcc;
  operator_reserved_pco_info.mnc = mnc;

  DS_3GPP_MSG2_MED("MCC %d, MNC %d", mcc, mnc);


  operator_reserved_pco_info.container_id = protocol_id;
  operator_reserved_pco_info.app_specific_info_len=protocol_pkt_len;

  memscpy(operator_reserved_pco_info.app_specific_info,
          sizeof(operator_reserved_pco_info.app_specific_info),
          protocol_pkt_ptr+MCC_MNC_LENGTH,protocol_pkt_len);

  /*-------------------------------------------------------------------------- 
    Currently for modem_op_pco we only have a single PCO entry, When this
    extends allocate modem_op_reserved_pco_info from heap and a logic change
    will be needed
  ----------------------------------------------------------------------------*/
  modem_op_reserved_pco_info.num_pcos  = 1;
  modem_op_reserved_pco_info.pco_info_ptr = &operator_reserved_pco_info;

  if((iface_v4_p!= NULL) && (profile_pco_id_configured))
  {
    ps_iface_set_operator_reserved_pco(iface_v4_p,&operator_reserved_pco_info);
  }
  else
  {
    ps_iface_set_modem_reserved_pco(iface_v4_p,&modem_op_reserved_pco_info);
  }


  if((iface_v6_p!= NULL)  && (profile_pco_id_configured))
  {
    ps_iface_set_operator_reserved_pco(iface_v6_p,&operator_reserved_pco_info);
  }
  else
  {
    ps_iface_set_modem_reserved_pco(iface_v6_p,&modem_op_reserved_pco_info);
  }

  return TRUE;
} /* ds3gpp_decode_operator_reserved_pco_pkt() */

#endif /* FEATURE_DATA_WCDMA_PS || FEATURE_GSM_GPRS */


