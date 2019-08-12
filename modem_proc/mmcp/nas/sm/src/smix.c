/*===========================================================================
                        S M  I X  SOURCE FILE

DESCRIPTION

   Definition for packing/unpacking Session Management Layer 3 messages.
   This file contains the methods to Encode/Decode the SM-SM peer messages.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2000, 2001-2012 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

===========================================================================*/
 
/*===========================================================================

            EDIT HISTORY FOR FILE

$PVCSPath:  L:/src/asw/MSM5200/sm/vcs/smix.c_v   1.9   16 May 2002 12:23:28   kishores  $
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/sm/src/smix.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $
            
when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/01/12   am      Back off timer code
09/27/12    am      Missing handling of undefined values for MBR and GBR by the MS
07/31/12   us    NAS shall reject any PDP (primary or secondary) activation or modification request if the total length of the 
                 PDU to be sent OTA exceeds the MAX_OCTETS_L3_MESSAGE with a cause SM_MESSAGE_EXCEED_MAX_L2_LIMIT.   
06/12/12   am    Fixing compilation warnings
04/27/12   us    Adding range check for PCOs & TFT filters to avoid encoded message exceeding MAX_OCTETS_L3_MESSAGE.
02/08/12   us    Extended Uplink Data rate support added 
11/09/11   am    Fixing pack_linked_ti() from MT primary PDP.
08/18/11   am    Putting check on IEI lengths while unpacking pdp activate message 
08/04/11   am    Adding range check for nsapi to avoid buffer overflow
06/03/11   am    Discarding Protocol ID in case of message size overflow. 
02/09/10   rk    valid SAPI should be checked for G-mode only as SAPI-0 is valid in W-mode.
03/04/08   ab    Removing Klocwork defect
01/24/08   ab    Removing Klocwork defect
01/21/08   ab     Removing compiler warning
09/22/06   rk       removed lint errors raised in unpack_tft
08/18/06   rk       Added Extended TI Feature Code
08/18/06   rk       Fixed programming err in function unpack_tft, causing decode failure.
07/25/06   rk      Added Preprocessor include stmt for GPRS CC for pack_pdp_activate_mesg function.
07/06/06   ss      Fixed PDP valid setting for GPRS Call control
07/03/06   ss      Added GPRS call control fixes
13/06/06   rk      PFI value for Modify Request/Accept from Network to MS as per Release 6
05/02/06   kvk      Resetting the REL5 QoS values if network sends R99 QoS
03/06/06   ss       Corrected PDP Addr to optional element in decoding of PDP Accept
03/24/06   ss       Dynamic PFI validation added
03/06/06   kvk      Fixed packing the QoS if NV is set to REL5 in 2G
02/13/06   ss       Fixed packing of QoS for R99  and R5
01/19/06   ss       Fixed packing of IEI Length in QoS
12/06/05   kvk     Fixed packing of num_filters in TFT if FEATURE_REL5 is not enabled
11/11/05   ss      Release 5 SM PCO and TFT changes put in.
10/13/05   kvk     Now REL5 QoS IEs will be packed only in UMTS mode and
                   GCF flag is enabled
09/28/05   kvk     Removed GCF flag dependency for REL5 QoS
 09/13/05  kvk     MO PDP Modify supported. Now handling the Application initiated
                   PDP Modification for QoS and TFT.
09/12/05   kvk     Changed FEATURE_REL5_QOS flag to FEATURE_REL5.
06/29/05   ss      Not returning error when PDP address in Network PDP Modify is same
                   as for existing PDP.
05/26/05   kvk     Now TI indicator IE will not be included in case TI is not valid.
05/16/05   ss      Added qos_ie_length as global to check for R99 QoS parameters
04/15/05   ss       Returning error if Network initiated PDP modify contains PDP Address IE.
01/11/05   kvk     Fixed packing the SM status with the extended TI received in the
                   request.
01/10/05   kvk     Packing for REL5 QoS supproted. 
12/6/04    kvk     New function sm_pack_pdp_context_status is added for packing
                   pdp context status IE
10/18/04   kvk     MT PDP support added
09/02/04   kvk     Added packing activate_sec_pdp_req, modify_pdp_req. Unpacking for
                   act_pdp_sec_cnf. 
08/27/04   kvk     GCF 45_5_1 test case fix. Unknown IE processing is fixed. 
                   Fixed extended ti flag packing for packing the SM status message.
06/07/04   kvk     Now unpacking the SM cause field for SM status message. 
                   corrected unpacking for pdp number type.
12/01/03   ks      Added proper parenthesis to remove compiler warnings.
09/23/03   ks      Setting the IEI lenght correctly in unpack_proto_config()
08/05/03   mks     Added support for validating the network offered QoS against
                   the minimum QoS requirements of the MS during MO PDP activation
                   and MT PDP modification procedures. If the QoS validation fails,
                   the context deactivation is initiated.
07/25/03   ks      Removed the check for EI_PACKET_FLOW_IDENT for MT Modify Request.
05/20/03   ks      Removed the check for Packet flow identifier value from the
                   Optional IE EI_PACKET_FLOW_IDENT, as this is not used by UE.
05/09/03   ks      Setting the TI-FLAG to 1 during PDP_CONTEXT_REJ message.
02/24/03   ks      Fixed LINT errors
12/02/02   ks      Removed the FEATURE_GSM_GPRS_SM_NETWORK_REVISION_SWITCH and
                   added check for MSCR bit for processing teardown_ind IE.
09/27/02   ks      define FEATURE_GSM_GPRS_SM_NETWORK_REVISION_SWITCH so that
                   UE can pack the QOS information according the MS rev number.
09/11/02   ks      Removed setting of the cause during PDP deactivation, as 
                   it is being set by the Data Service layer.
05/15/02   ks      Setting the config_options.num_protocol_ids to 0 before
                   unpacking the IE. 
05/02/02   ks      Fixed the IE length for the Protocol Config options in 
                   the pack_pdp_activate_mesg() function.    
04/22/02   ks      Corrected setting of the TI_FLAG in pack_pd_ti_msgtype().
                   For Mobile Originated PS calls, the TI_FLAG is set to '0'
08/24/01   ks      Fixed the decoding of Transaction Id in validate_layer3_msg
08/01/01   ks      Fixed the packing of TI in pack_pd_ti_msg().
05/23/01   ks      Added functions pack_pd_ti_msg(), unpack_pdp_addr(),
                   unpack_proto_config(), process_unknown_ie().
05/17/01   ks      Added check for validating the Mandatory length of a
                   SM-SM message.
04/30/01   ks      Added function PACK_REQ_PDP_ACTIVATE_REJ to pack a SM-SM
                   peer message to reject a PDP activation request by network.
02/26/01   ks      Created file.
            
===========================================================================*/

/* -------------------------------------------------------------------------
** Include Files
** ------------------------------------------------------------------------*/
#include "mmcp_variation.h"
#include "smix_v.h"
#include "msg.h"
#include "smtask.h"
#include "smtask_v.h"
#include "memory.h"
#include "bit.h"
#include "string.h"
#include "mm_v.h"
#include "gmm_int_v.h"
#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#else
#include "gs_v.h"
#endif
#include "ds3gpp_api.h"
#include <stringl/stringl.h>


extern boolean is_ps_wcdma_mode( sys_modem_as_id_e_type as_id );
extern boolean is_ps_tdscdma_mode( sys_modem_as_id_e_type as_id );
extern boolean is_ps_gsm_mode( sys_modem_as_id_e_type as_id );

#ifdef FEATURE_DUAL_SIM
#define is_umts_mode() is_ps_umts_mode((sys_modem_as_id_e_type)sm_as_id)
#define is_gsm_mode()  is_ps_gsm_mode((sys_modem_as_id_e_type)sm_as_id)

extern boolean SM_IS_VALID_AS_ID(sys_modem_as_id_e_type as_id);
#else
#define is_umts_mode() is_ps_umts_mode(0)
#define is_gsm_mode()  is_ps_gsm_mode(0)
#endif

#ifdef FEATURE_LTE
extern lte_3gpp_release_ver_e emm_3gpp_spec_ver;
#endif

/*===========================================================================
LOCAL FUNCTION PROTOTYPES:
===========================================================================*/
#ifndef FEATURE_GPRS_CALLCONTROL
/* -----------------------------------------------------------------------
** FUNCTION     : PACK_PDP_ACTIVATE_MESG()

** DESCRIPTION  : Packs the ACTIVATE_PDP_REQUEST SM-SM peer message.
** ----------------------------------------------------------------------- */
xlate_status_T pack_pdp_activate_mesg( word* length, byte* ext_buf,
                                             pdp_context_info* pdp );
#endif


#ifdef FEATURE_MBMS
#error code not present
#endif


/* -----------------------------------------------------------------------
** FUNCTION     : PACK_REQ_PDP_ACTIVATE_REJ()

** DESCRIPTION  : Encodes the REQ_PDP_ACTIVATE_REJ message sent to Network.
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T pack_req_pdp_activate_rej( word* length, byte* ext_buf,
                                                pdp_context_info* pdp );

/* -----------------------------------------------------------------------
** FUNCTION     : PACK_PDP_DEACTIVATE_REQ()

** DESCRIPTION  : Encodes PDP_DEACTIVATE_REQ message to send to Network
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T pack_pdp_deactivate_req( word* length, byte* ext_buf,
                                              pdp_context_info* pdp );

/* -----------------------------------------------------------------------
** FUNCTION     : PACK_PDP_DEACTIVATE_ACCEPT()

** DESCRIPTION  : Encodes PACK_PDP_DEACTIVATE_ACCEPT msg to send to Network.
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T pack_pdp_deactivate_accept( word* length, byte* ext_buf,
                                                 pdp_context_info* pdp );

/* -----------------------------------------------------------------------
** FUNCTION     : PACK_PDP_MODIFY_REQ()

** DESCRIPTION  : Encodes PDP_MODIFY_REQ message to be sent to Network.
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T pack_pdp_modify_req( word* length, byte* ext_buf,
                                          pdp_context_info* pdp );

/* -----------------------------------------------------------------------
** FUNCTION     : PACK_MS_PDP_MODIFY_ACCEPT()

** DESCRIPTION  : Encodes MS_PDP_MODIFY_ACCEPT message to be sent to Network.
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T pack_ms_pdp_modify_accept( word* length, byte* ext_buf,
                                                pdp_context_info* pdp );

/* -----------------------------------------------------------------------
** FUNCTION     : PACK_PDP_SEC_ACTIVATE_REQ()

** DESCRIPTION  : encodes/packs a PDP_SEC_ACTIVATE_REQ Layer3 message.
** ----------------------------------------------------------------------- */
xlate_status_T pack_pdp_sec_activate_req( word* length, byte* ext_buf,
                                                pdp_context_info* pdp );
/* -----------------------------------------------------------------------
** FUNCTION     : PACK_PDP_SEC_ACTIVATE_REQ()

** DESCRIPTION  : encodes/packs a PDP_SEC_ACTIVATE_REQ Layer3 message.
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T pack_sm_status( word* length, byte* ext_buf,
                                                pdp_context_info* pdp );

/* -----------------------------------------------------------------------
** FUNCTION     : UNPACK_ACTIVATE_PDP_ACCEPT()

** DESCRIPTION  : Decodes the received ACTIVATE_PDP_ACCEPT msg from network.
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T unpack_activate_pdp_accept(  uint16 len, byte* ext_buf, 
                                                  pdp_context_info *pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : UNPACK_ACTIVATE_PDP_REJECT()

** DESCRIPTION  : decodes the ACTIVATE_PDP_REJECT msg sent by Network.
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T unpack_activate_pdp_reject(  uint16 len, byte* ext_buf, 
                                                  pdp_context_info *pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : UNPACK_REQUEST_PDP_ACTIVATION()

** DESCRIPTION  : decodes the REQUEST_PDP_ACTIVATION msg sent by Network
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T unpack_request_pdp_activation( uint16 len, byte* ext_buf, 
                                                    pdp_context_info *pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : UNPACK_MODIFY_PDP_ACCEPT

** DESCRIPTION  : This function (decodes) unpacks MODIFY_PDP_ACCEPT message sent by Network.
** ----------------------------------------------------------------------- */

LOCAL xlate_status_T unpack_modify_pdp_accept (  uint16 msg_length, byte* ext_buf,
                                                 pdp_context_info *pdp);



/* -----------------------------------------------------------------------
** FUNCTION     : UNPACK_REQUEST_PDP_DEACTIVATION()

** DESCRIPTION  : decodes the REQUEST_PDP_DEACTIVATION msg sent by Network.
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T unpack_request_pdp_deactivation( uint16 len, byte* ext_buf, 
                                                      pdp_context_info *pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : UNPACK_MODIFY_PDP_REJECT()

** DESCRIPTION  : decodes the MODIFY_PDP_REJECT message sent by Network.
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T unpack_modify_pdp_reject( uint16 len, byte* ext_buf, 
                                               pdp_context_info *pdp); 

/* -----------------------------------------------------------------------
** FUNCTION     : UNPACK_ACTIVATE_SEC_PDP_ACCEPT()

** DESCRIPTION  : decodes the ACTIVATE_SEC_PDP_ACCEPT msg sent by Network.
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T unpack_activate_sec_pdp_accept( uint16 len, byte* ext_buf, 
                                                     pdp_context_info *pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : UNPACK_ACTIVATE_SEC_PDP_REJECT()

** DESCRIPTION  : decodes the ACTIVATE_SEC_PDP_REJECT sent to Network.
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T unpack_activate_sec_pdp_reject(  uint16 len, byte* ext_buf, 
                                                      pdp_context_info *pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : UNPACK_NETWORK_MODIFY_PDP_REQ()

** DESCRIPTION  : decodes the UNPACK_NETWORK_MODIFY_PDP_REQ sent by network.
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T unpack_network_modify_pdp_req( uint16 len, byte* ext_buf, 
                                                    pdp_context_info *pdp);

/* -----------------------------------------------------------------------
** FUNCTION     : PACK_QOS()

** DESCRIPTION  : Helper function that packs the QOS IE.
** ----------------------------------------------------------------------- */
LOCAL void pack_qos(byte* ext_buf, qos_T* qos, uint16* pos);

/* -----------------------------------------------------------------------
** FUNCTION     : PACK_PD_TI_MSGTYPE()

** DESCRIPTION  : Helper function that packs the QOS IE.
** ----------------------------------------------------------------------- */
LOCAL void pack_pd_ti_msgtype (byte* ext_buf, word t_id, sm_layer_msg_T sm_msg);

/* -----------------------------------------------------------------------
** FUNCTION     : UNPACK_QOS()

** DESCRIPTION  : Helper function that unpacks the QOS IE.
** ----------------------------------------------------------------------- */
LOCAL byte unpack_qos(byte* ext_buf, pdp_context_info *pdp, uint16* cur_pos);

/* -----------------------------------------------------------------------
** FUNCTION     : UNPACK_PDP_ADDR()

** DESCRIPTION  : Helper function to unpack the PDP ADDRESS IE.
** ----------------------------------------------------------------------- */
xlate_status_T unpack_pdp_addr( byte* ext_buf, uint16* msg_index,
                                pdp_context_info* pdp );

/* -----------------------------------------------------------------------
** FUNCTION     : UNPACK_PROTO_CONFIG()

** DESCRIPTION  : Helper function to unpack the PROTOCOL CONFIGURATIONS IE.
** ----------------------------------------------------------------------- */
xlate_status_T unpack_proto_config( byte* ext_buf, uint16 *index,
                                    pdp_context_info* pdp );

/* -----------------------------------------------------------------------
** FUNCTION     : PROCESS_UNKNOWN_IE()

** DESCRIPTION  : Helper function that processes Unknown IE received.
** ----------------------------------------------------------------------- */
xlate_status_T process_unknown_ie ( byte* ext_buf,  uint16 *index);

/* -----------------------------------------------------------------------
**FUNCTION      :PACK_TFT
**DESCRIPTION   :This function (encodes) packs the TFT
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T pack_tft(word *msg_len, tft_type_T  *tft, byte *ext_buf);

/* -----------------------------------------------------------------------
FUNCTION    :PACK_LINKED_TI 

DESCRIPTION :This function (encodes) packs  LINKED TI with the given 
             linked TI value
** ----------------------------------------------------------------------- */
LOCAL void pack_linked_ti(word *msg_length, byte  link_ti_value, byte  *ext_buf);

/* -----------------------------------------------------------------------
** FUNCTION       : get_message_data_index 

** DESCRIPTION : This function returns the index in the message data after Message Type.
** ----------------------------------------------------------------------- */
static byte get_message_data_index (const byte* ext_buf) ;

/* -----------------------------------------------------------------------
** FUNCTION     : UNPACK_LINKED_TI()

** DESCRIPTION  : Helper function that unpacks the TFT IE.TI will be converted to semantics of UE.
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T unpack_linked_ti(byte* ext_buf, pdp_context_info *pdp, uint16* cur_pos);

/* -----------------------------------------------------------------------
** FUNCTION     : UNPACK_REQUEST_SEC_PDP_ACTIVATION()

** DESCRIPTION  : Helper function to unpack the request Secondary PDP activation
                             message sent by NW
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T unpack_request_sec_pdp_activation( uint16 len, byte* ext_buf,
                                                     pdp_context_info *pdp);


/* -----------------------------------------------------------------------
** FUNCTION     : PACK_REQ_SEC_PDP_ACTIVATION_REJ()

** DESCRIPTION  : Helper function to pack the request Secondary PDP activation
                             reject message to NW
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T pack_req_sec_pdp_activation_rej( word* length, byte* ext_buf,
                                                pdp_context_info* pdp );

/* -----------------------------------------------------------------------
** FUNCTION     : PACK_REQ_SEC_PDP_ACTIVATION_REJ()

** DESCRIPTION  : Helper function to pack the request Secondary PDP activation
                             reject message to NW
** ----------------------------------------------------------------------- */
LOCAL xlate_status_T pack_modify_pdp_reject( word* length, byte* ext_buf,
                                                pdp_context_info* pdp );

/* -----------------------------------------------------------------------
** FUNCTION     : UNPACK_REQ_QOS()

** DESCRIPTION  : Helper function that unpacks the QOS IE into req_qos
** ----------------------------------------------------------------------- */
LOCAL byte unpack_req_qos(byte* ext_buf, pdp_context_info *pdp, uint16* cur_pos);


LOCAL xlate_status_T unpack_gprs_timer_3(  byte* ext_buf, uint16 *index,  pdp_context_info* pdp);

/*===========================================================================
FUNCTION PACK_LAYER3_MESG 

DESCRIPTION
  This function packs a Layer3 message (SM-SM peer) to be sent to GMM. 
  
DEPENDENCIES
  It is assumed that all the mandatory IE fields are given by upper layers.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T pack_layer3_mesg
(
  word* length,           /* Length in bits of the Packed Layer3 message */
  byte* ext_buf,          /* Buffer contains the packed message */
  pdp_context_info* pdp,  /* PDP context information, used to pack the msg */
  sm_layer_msg_T msg_type /* Type of L3 message being packed */
)
{
  xlate_status_T status;
  status = L3_PARSE_SUCCESS;

  MSG_HIGH_DS_1( SM_SUB,"Packing L3 MSG:0x%X", msg_type );

  switch (msg_type)
  {
    case ACTIVATE_PDP_REQUEST:      
      status = pack_pdp_activate_mesg(length, ext_buf, pdp);
    break;

    case REQUEST_PDP_ACTIVATION_REJ:      
      status = pack_req_pdp_activate_rej(length, ext_buf, pdp);
    break;

    case DEACTIVATE_PDP_REQUEST:      
      status = pack_pdp_deactivate_req(length, ext_buf, pdp);
    break;

    case DEACTIVATE_PDP_ACCEPT:      
      status = pack_pdp_deactivate_accept(length, ext_buf, pdp);
    break;

    case MS_MODIFY_PDP_REQUEST:      
      status = pack_pdp_modify_req(length, ext_buf, pdp);
    break;

    case MS_MODIFY_PDP_ACCEPT:      
      status = pack_ms_pdp_modify_accept(length, ext_buf, pdp);
    break;

    case ACTIVATE_SEC_PDP_REQUEST:      
      status = pack_pdp_sec_activate_req(length, ext_buf, pdp);
    break;

    case SM_STATUS:      
      status = pack_sm_status(length, ext_buf, pdp);
    break;
#ifdef FEATURE_MBMS
#error code not present
#endif

    case REQUEST_SEC_PDP_ACTIVATION_REJ:      
      status = pack_req_sec_pdp_activation_rej(length, ext_buf, pdp);
    break;

    case MODIFY_PDP_REJECT:      
      status = pack_modify_pdp_reject(length, ext_buf,pdp);
    break;
    default:
      MSG_ERROR_DS_0( SM_SUB,"Request to pack unknown message");
    break;
  }

  return status;
}

/*===========================================================================
FUNCTION PACK_PDP_ACTIVATE_MESG 

DESCRIPTION
  This function (encodes) packs PDP_ACTIVATE_MESG sent to the Network.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T pack_pdp_activate_mesg 
(
  word* length, byte* ext_buf,
  pdp_context_info* pdp 
)
{
  uint16 msg_length,gmm_max_length;
  byte i;
  uint16 config_option_len;

  config_option_len = msg_length = 0;    

  /* ---------------------------------------------------------------
  ** Check if the Mandatory fields are set for this message. If not
  ** return a L3_PARSE_INVALID to the caller.  
  ** --------------------------------------------------------------*/
  if ((!pdp->nsapi.valid) || (!pdp->llc_sapi.valid) || (!pdp->qos.valid) ||
      (!pdp->pdp_addr.valid))
  {
    MSG_ERROR_DS_0( SM_SUB,"Mandatory Field is not Set");
    return L3_PARSE_INVALID;
  } 
  gmm_max_length = gmm_max_L3_msg_length(MM_PS_DOMAIN);
  /* ---------------------------------------------------------------
  ** Pack the Mandatory fields of the message.
  ** --------------------------------------------------------------*/

  //Pack the PD, TI and the Message type
  pack_pd_ti_msgtype (ext_buf, pdp->trans_id, ACTIVATE_PDP_REQUEST);
  msg_length += get_message_data_index (ext_buf);  // Move the message length 
 
  ext_buf[msg_length++] = pdp->nsapi.nsapi;     // Pack NSAPI  
  ext_buf[msg_length++] = pdp->llc_sapi.sapi;   // Pack LLC SAPI
  pack_qos(ext_buf, &pdp->qos, &msg_length);          // Pack QOS

  // Pack the Requested PDP address
  ext_buf[msg_length++] = (byte)(2 + pdp->pdp_addr.pdp_addr_len);
  ext_buf[msg_length++] = pdp->pdp_addr.pdp_type_org;
  ext_buf[msg_length++] = pdp->pdp_addr.pdp_type_num; 
  memscpy(&ext_buf[msg_length], (gmm_max_length - msg_length),
          pdp->pdp_addr.address, pdp->pdp_addr.pdp_addr_len);

  msg_length = (msg_length + MIN(pdp->pdp_addr.pdp_addr_len, (gmm_max_length - msg_length )));
  
  /* ---------------------------------------------------------------
  ** Pack Optional Fields if included by upper layers.
  ** --------------------------------------------------------------*/

  // Check if APN is included
  if (pdp->apn_name.valid)
  {
    ext_buf[msg_length++] = IEI_ACC_POINT_NAME;
    ext_buf[msg_length++] = (byte)pdp->apn_name.apn_addr_len;
    memscpy(&ext_buf[msg_length], (gmm_max_length - msg_length ),
            pdp->apn_name.address, pdp->apn_name.apn_addr_len );
    msg_length = (msg_length + MIN( pdp->apn_name.apn_addr_len, (gmm_max_length - msg_length )));
  } 

  // Check if Protocol Configuration options field is included
  if (pdp->config_options.valid)
  {
    // pack the IE value of Optional Protocol Configurations
    ext_buf[msg_length++] = IEI_PROTOCOL_CFG_OPTIONS;

    // Pack the IEI length : This is temporarily set to '0'
    ext_buf[msg_length++] = 0;
   
    // Pack the Configuration protocol field
    ext_buf[msg_length] = 0x80;  //set ext bit 1 and spare bits
    ext_buf[msg_length++] = ( (pdp->config_options.config_protocol & 0x07) | ext_buf[msg_length]) ;  
    
    // Pack all the individual protocol IDs
    for(i=0; i<pdp->config_options.num_protocol_ids; i++)
    {  
      /* Return parse failure if packing PCOs results in encoded message exceeding MAX_OCTETS_L3_MESSAGE */         
      if(gmm_max_length - (msg_length -1) < (3 + pdp->config_options.procol_info[i].protocol_len ))
      {
        MSG_ERROR_DS( SM_SUB,"Protocol ID onwards %d, %d is skipped, message buff exceeding. Proto ID len: %d",
                      pdp->config_options.procol_info[i].protocol_id[0],
                      pdp->config_options.procol_info[i].protocol_id[1],
                      pdp->config_options.procol_info[i].protocol_len);
        pdp->current_cause = (cc_cause_T)SM_MESSAGE_EXCEED_MAX_L2_LIMIT;
        return L3_PARSE_FAIL;
      }
      ext_buf[msg_length++] = 
        pdp->config_options.procol_info[i].protocol_id[0]; 

      ext_buf[msg_length++] = 
        pdp->config_options.procol_info[i].protocol_id[1];

      config_option_len += 2;
   
      ext_buf[msg_length++] = 
        pdp->config_options.procol_info[i].protocol_len;

      config_option_len ++;

      memscpy(&ext_buf[msg_length], (gmm_max_length - msg_length),
              pdp->config_options.procol_info[i].protocol_contents, 
              pdp->config_options.procol_info[i].protocol_len);

      msg_length = (msg_length + 
                          MIN(pdp->config_options.procol_info[i].protocol_len, (gmm_max_length - msg_length )));
      config_option_len = (config_option_len +
                           MIN(pdp->config_options.procol_info[i].protocol_len, (gmm_max_length - msg_length )));
    }
    MSG_HIGH_DS_2(SM_SUB,"=MM= config option length %d, message length %d", config_option_len,msg_length);

    // Set the Protocol configuration IEI length now.
    ext_buf[msg_length - (config_option_len +2)] = (byte)(config_option_len + 1);
  }
 
  if (pdp->request_type.valid)
  {
    /*24.008 10.5.6.17*/
    ext_buf[msg_length++] = (IEI_PDP_REQUEST_TYPE | pdp->request_type.req_type);
  }

  *length = msg_length;  // Set the Length of the packed message
  return L3_PARSE_SUCCESS;
}

/*===========================================================================
FUNCTION PACK_REQ_PDP_ACTIVATE_REJ 

DESCRIPTION
  This function (encodes) the REQ_PDP_ACTIVATE_REJ message sent to Network. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T pack_req_pdp_activate_rej 
(
  word* length, byte* ext_buf,
  pdp_context_info* pdp 
)

{
  word mesg_length;
  uint16 gmm_max_length;
#ifdef FEATURE_REL5
  uint16 config_option_len;
  byte i;
  ue_rev_level_indicator_type ue_revision;
  ue_revision = sm_get_revision_level_indicator();
  config_option_len = 0;    
#endif
  mesg_length = 0;

  //Pack the PD, TI and the Message type
  pack_pd_ti_msgtype (ext_buf, pdp->trans_id, REQUEST_PDP_ACTIVATION_REJ);
  mesg_length += get_message_data_index (ext_buf);  // Move the message length
  
  //Pack the SM cause for rejection
  ext_buf[mesg_length++] = (byte)pdp->current_cause;
  gmm_max_length = gmm_max_L3_msg_length(MM_PS_DOMAIN);
#ifdef FEATURE_REL5
  if ((ue_revision == UE_REV_REL99) && (mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL5))
  {
  // Check if Protocol Configuration options field is included
  if (pdp->config_options.valid)
  {
    // pack the IE value of Optional Protocol Configurations
    ext_buf[mesg_length++] = IEI_PROTOCOL_CFG_OPTIONS;

    // Pack the IEI length : This is temporarily set to '0'
    ext_buf[mesg_length++] = 0;
   
    // Pack the Configuration protocol field
    ext_buf[mesg_length] = 0x80;  //set ext bit 1 and spare bits
    ext_buf[mesg_length++] = ( (pdp->config_options.config_protocol & 0x07) | ext_buf[mesg_length]) ; 
    
    // Pack all the individual protocol IDs
    for(i=0; i<pdp->config_options.num_protocol_ids; i++)
    { 
      /* Return parse failure if packing PCOs results in encoded message exceeding MAX_OCTETS_L3_MESSAGE */
      if(gmm_max_length - (mesg_length -1) < (3 + pdp->config_options.procol_info[i].protocol_len ))
      {
        MSG_ERROR_DS_3( SM_SUB,"Protocol ID onwards %d, %d is skipped, message buff exceeding. Proto ID len: %d",
                      pdp->config_options.procol_info[i].protocol_id[0],
                      pdp->config_options.procol_info[i].protocol_id[1],
                      pdp->config_options.procol_info[i].protocol_len);
        pdp->current_cause = (cc_cause_T)SM_MESSAGE_EXCEED_MAX_L2_LIMIT;
        return L3_PARSE_FAIL;
      }
      
      ext_buf[mesg_length++] = 
        pdp->config_options.procol_info[i].protocol_id[0]; 

      ext_buf[mesg_length++] = 
        pdp->config_options.procol_info[i].protocol_id[1];

      config_option_len += 2;
   
      ext_buf[mesg_length++] = 
        pdp->config_options.procol_info[i].protocol_len;

      config_option_len ++;

      memscpy(&ext_buf[mesg_length], (gmm_max_length - mesg_length),
              pdp->config_options.procol_info[i].protocol_contents, 
              pdp->config_options.procol_info[i].protocol_len);

      mesg_length = (mesg_length + 
                           MIN(pdp->config_options.procol_info[i].protocol_len, (gmm_max_length - mesg_length)));
      config_option_len = (config_option_len +
                           MIN(pdp->config_options.procol_info[i].protocol_len, (gmm_max_length - mesg_length)));
    }

    // Set the Protocol configuration IEI length now.
    MSG_HIGH_DS_2(SM_SUB,"=MM= config option length %d, message length %d", config_option_len,mesg_length);
    ext_buf[(mesg_length - config_option_len) -2] = (byte)(config_option_len + 1);
  }
  }
#endif
  *length = mesg_length;
  return L3_PARSE_SUCCESS;
}

/*===========================================================================
FUNCTION PACK_PDP_DEACTIVATE_REQ 

DESCRIPTION
  This function (encodes) PDP_DEACTIVATE_REQ message to send to Network.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T pack_pdp_deactivate_req 
(
  word* length, byte* ext_buf,
  pdp_context_info* pdp 
)
{  
  word mesg_length;
  ue_rev_level_indicator_type ue_revision;
  uint16 gmm_max_length;
#ifdef FEATURE_REL5
  uint16 config_option_len;
  byte i;
  config_option_len = 0;    
#endif
  ue_revision = sm_get_revision_level_indicator();
  mesg_length = 0;
 
  gmm_max_length = gmm_max_L3_msg_length(MM_PS_DOMAIN);
  //Pack the PD, TI and the Message type
  pack_pd_ti_msgtype (ext_buf, pdp->trans_id, DEACTIVATE_PDP_REQUEST);
  mesg_length += get_message_data_index (ext_buf);  // Move the message 
  
  /* ----------------------------------------------------------------
  ** The Deactivation is set by the Data Service layer, the default
  ** value for the cause is SM_REGULAR_DEACTIVATION.
  ** ---------------------------------------------------------------*/

  ext_buf[mesg_length++] = (byte)pdp->current_cause;
   
  // Check the optional field.  TI indicator
  if (( ue_revision == UE_REV_REL99 ) && 
          pdp->teardown_ind.valid) 
  {
    if (pdp->teardown_ind.ti_flag)
    {
      // Set the TI indicator IE and value
      ext_buf[mesg_length++] = IEI_TI_INDICATOR | 0x91;
    }
    else
    {
      ext_buf[mesg_length++] = IEI_TI_INDICATOR | 0x90;
    }
  }

#ifdef FEATURE_REL5
  if ((ue_revision == UE_REV_REL99) && (mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL5))
  {
  // Check if Protocol Configuration options field is included
  if (pdp->config_options.valid)
  {
    // pack the IE value of Optional Protocol Configurations
    ext_buf[mesg_length++] = IEI_PROTOCOL_CFG_OPTIONS;

    // Pack the IEI length : This is temporarily set to '0'
    ext_buf[mesg_length++] = 0;
   
    // Pack the Configuration protocol field
    ext_buf[mesg_length] = 0x80;  //set ext bit 1 and spare bits
    ext_buf[mesg_length++] = ( (pdp->config_options.config_protocol & 0x07) | ext_buf[mesg_length]) ;  
    
    // Pack all the individual protocol IDs
    for(i=0; i<pdp->config_options.num_protocol_ids; i++)
    {  
      /* Skip the particular Prot ID if it results in the message exceeding MAX_OCTETS_L3_MESSAGE & pack the
         rest of the IDs; we are doing this, as we do not want to reject a deactivate request, & as PCOs are
         optional in a deactivate request, truncating PCOs should not hamper the deactivate request sent to network*/
      if(gmm_max_length - (mesg_length -1) < (3 + pdp->config_options.procol_info[i].protocol_len ))
      {
        MSG_ERROR_DS_3( SM_SUB,"Protocol ID %d, %d is skipped, message buff exceeding. Proto ID len: %d",
                      pdp->config_options.procol_info[i].protocol_id[0],
                      pdp->config_options.procol_info[i].protocol_id[1],
                      pdp->config_options.procol_info[i].protocol_len);
        
        continue;
      }   
      
      ext_buf[mesg_length++] = 
        pdp->config_options.procol_info[i].protocol_id[0]; 

      ext_buf[mesg_length++] = 
        pdp->config_options.procol_info[i].protocol_id[1];

      config_option_len += 2;
   
      ext_buf[mesg_length++] = 
        pdp->config_options.procol_info[i].protocol_len;

      config_option_len ++;

      memscpy(&ext_buf[mesg_length], (gmm_max_length - mesg_length),
              pdp->config_options.procol_info[i].protocol_contents, 
              pdp->config_options.procol_info[i].protocol_len);

      mesg_length = (mesg_length + 
                           MIN(pdp->config_options.procol_info[i].protocol_len, (gmm_max_length - mesg_length)));
      config_option_len = (config_option_len +
                           MIN(pdp->config_options.procol_info[i].protocol_len, (gmm_max_length - mesg_length)));
    }
    MSG_HIGH_DS_2(SM_SUB,"=MM= config option length %d, message length %d", config_option_len,mesg_length);
     // Set the Protocol configuration IEI length now.
    ext_buf[(mesg_length - config_option_len) -2] = (byte)(config_option_len + 1);
  }
  }
#endif
 
  *length = mesg_length;
  return L3_PARSE_SUCCESS;
}

/*===========================================================================
FUNCTION PACK_PDP_DEACTIVATE_ACCEPT 

DESCRIPTION
  This function (encodes) PACK_PDP_DEACTIVATE_ACCEPT message to send to Network.
  
DEPENDENCIES

  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
xlate_status_T pack_pdp_deactivate_accept 
(
  word* length, byte* ext_buf,
  pdp_context_info* pdp 
)
{  
  word mesg_length;
  uint16 gmm_max_length;
#ifdef FEATURE_REL5
  uint16 config_option_len;
  byte i;
  ue_rev_level_indicator_type ue_revision;
  ue_revision = sm_get_revision_level_indicator();
  config_option_len = 0;    
#endif
  mesg_length = 0;

  //Pack the PD, TI and the Message type
  pack_pd_ti_msgtype (ext_buf, pdp->trans_id, DEACTIVATE_PDP_ACCEPT);
  mesg_length += get_message_data_index (ext_buf);  // Move the message length
  gmm_max_length = gmm_max_L3_msg_length(MM_PS_DOMAIN);
#ifdef FEATURE_MBMS
  #error code not present
#endif


#ifdef FEATURE_REL5
  if ((ue_revision == UE_REV_REL99) && (mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL5))
  {
  // Check if Protocol Configuration options field is included
  if (pdp->config_options.valid)
  {
    // pack the IE value of Optional Protocol Configurations
    ext_buf[mesg_length++] = IEI_PROTOCOL_CFG_OPTIONS;

    // Pack the IEI length : This is temporarily set to '0'
    ext_buf[mesg_length++] = 0;
   
    // Pack the Configuration protocol field
    ext_buf[mesg_length] = 0x80;  //set ext bit 1 and spare bits
    ext_buf[mesg_length++] = ( (pdp->config_options.config_protocol & 0x07) | ext_buf[mesg_length]) ;
    
    // Pack all the individual protocol IDs
    for(i=0; i<pdp->config_options.num_protocol_ids; i++)
    {   
      /* Return parse failure if packing PCO results in encoded message exceeding MAX_OCTETS_L3_MESSAGE */
      if(gmm_max_length - (mesg_length -1) < (3 + pdp->config_options.procol_info[i].protocol_len ))
      {
        MSG_ERROR_DS_3( SM_SUB,"Protocol ID %d, %d is skipped, message buff exceeding. Proto ID len: %d",
                      pdp->config_options.procol_info[i].protocol_id[0],
                      pdp->config_options.procol_info[i].protocol_id[1],
                      pdp->config_options.procol_info[i].protocol_len);

        return L3_PARSE_FAIL;
      }        
      ext_buf[mesg_length++] = 
        pdp->config_options.procol_info[i].protocol_id[0]; 

      ext_buf[mesg_length++] = 
        pdp->config_options.procol_info[i].protocol_id[1];

      config_option_len += 2;
   
      ext_buf[mesg_length++] = 
        pdp->config_options.procol_info[i].protocol_len;

      config_option_len ++;

      memscpy(&ext_buf[mesg_length], (gmm_max_length - mesg_length),
              pdp->config_options.procol_info[i].protocol_contents, 
              pdp->config_options.procol_info[i].protocol_len);

      mesg_length = (mesg_length + 
                           MIN(pdp->config_options.procol_info[i].protocol_len, (gmm_max_length - mesg_length )));
      config_option_len = (config_option_len +
                           MIN(pdp->config_options.procol_info[i].protocol_len, (gmm_max_length - mesg_length )));
    }
    MSG_HIGH_DS_2(SM_SUB,"=MM= config option length %d, message length %d", config_option_len,mesg_length);

    // Set the Protocol configuration IEI length now.
    ext_buf[(mesg_length - config_option_len) -2] = (byte)(config_option_len + 1);
  }
  }
#endif

#ifdef FEATURE_MBMS
  #error code not present
#endif

  *length = mesg_length;
  return L3_PARSE_SUCCESS;
}

/*===========================================================================
FUNCTION PACK_PDP_MODIFY_REQ 

DESCRIPTION
  This function (encodes) PDP_MODIFY_REQ message to be sent to Network. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T pack_pdp_modify_req 
(
  word* length, byte* ext_buf,
  pdp_context_info* pdp 
)
{
  word mesg_length;
  uint16 gmm_max_length;
  xlate_status_T tft_pack_status = L3_PARSE_SUCCESS;
#ifdef FEATURE_REL5
  uint16 config_option_len;
  byte i;
  ue_rev_level_indicator_type ue_revision;
  ue_revision = sm_get_revision_level_indicator();
  config_option_len = 0;    
#endif

  mesg_length = 0;
  gmm_max_length = gmm_max_L3_msg_length(MM_PS_DOMAIN);


  //Pack the PD, TI and the Message type
  pack_pd_ti_msgtype (ext_buf, pdp->trans_id, MS_MODIFY_PDP_REQUEST);
  mesg_length += get_message_data_index (ext_buf);  // Move the message length

  // Check optional fields and pack them

  // Pack LL SAPI
  /*if (pdp->req_llc_sapi.valid)
  {
    ext_buf[mesg_length++] = IEI_REQUEST_LLC_SAPI;    
    ext_buf[mesg_length++] = pdp->req_llc_sapi.sapi;
  }*/

  // Pack QOS
  if (pdp->req_qos.valid)
  {
    ext_buf[mesg_length++] = IEI_NEW_QOS;
    pack_qos(ext_buf, &pdp->req_qos, (uint16 *)&mesg_length);
  }

  if (pdp->req_tft.valid)
  {
    word tft_iei_pos = mesg_length;  
    tft_pack_status = pack_tft(&mesg_length, &pdp->req_tft, ext_buf );
    ext_buf[tft_iei_pos] = IEI_NEW_TFT;
  }
  
  /* Return L3_PARSE_FAIL if TFT packing results in overflow of MAX_OCTETS_L3_MESSAGE */
  if (tft_pack_status == L3_PARSE_FAIL)
  {
    MSG_ERROR_DS_1(SM_SUB,"TFT cannot be packed, TFT pack status %d",tft_pack_status);
    pdp->current_cause = (cc_cause_T)SM_MESSAGE_EXCEED_MAX_L2_LIMIT;
    return L3_PARSE_FAIL;
  }
  

#ifdef FEATURE_REL5
  if ((ue_revision == UE_REV_REL99) && (mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL5))
  {
  // Check if Protocol Configuration options field is included
  if (pdp->config_options.valid)
  {
    // pack the IE value of Optional Protocol Configurations
    ext_buf[mesg_length++] = IEI_PROTOCOL_CFG_OPTIONS;

    // Pack the IEI length : This is temporarily set to '0'
    ext_buf[mesg_length++] = 0;
   
    // Pack the Configuration protocol field
    ext_buf[mesg_length] = 0x80;  //set ext bit 1 and spare bits
    ext_buf[mesg_length++] = ( (pdp->config_options.config_protocol & 0x07) | ext_buf[mesg_length]) ;   
    
    // Pack all the individual protocol IDs
    for(i=0; i<pdp->config_options.num_protocol_ids; i++)
    {  
      /* Return parse failure if PCO exceeds MAX_OCTETS_L3_MESSAGE */
      if(gmm_max_length - (mesg_length -1) < (3 + pdp->config_options.procol_info[i].protocol_len ))
      {
        MSG_ERROR_DS_3( SM_SUB,"Protocol ID onwards %d, %d is skipped, message buff exceeding. Proto ID len: %d",
                      pdp->config_options.procol_info[i].protocol_id[0],
                      pdp->config_options.procol_info[i].protocol_id[1],
                      pdp->config_options.procol_info[i].protocol_len);
        pdp->current_cause = (cc_cause_T)SM_MESSAGE_EXCEED_MAX_L2_LIMIT;
        return L3_PARSE_FAIL;
      }
      
      ext_buf[mesg_length++] = 
        pdp->config_options.procol_info[i].protocol_id[0]; 

      ext_buf[mesg_length++] = 
        pdp->config_options.procol_info[i].protocol_id[1];

      config_option_len += 2;
   
      ext_buf[mesg_length++] = 
        pdp->config_options.procol_info[i].protocol_len;

      config_option_len ++;

      memscpy(&ext_buf[mesg_length], (gmm_max_length - mesg_length),
              pdp->config_options.procol_info[i].protocol_contents, 
              pdp->config_options.procol_info[i].protocol_len);

      mesg_length = (mesg_length + 
                           MIN(pdp->config_options.procol_info[i].protocol_len, (gmm_max_length - mesg_length)));
      config_option_len = (config_option_len +
                           MIN(pdp->config_options.procol_info[i].protocol_len, (gmm_max_length - mesg_length)));
    }
    MSG_HIGH_DS_2(SM_SUB,"=MM= config option length %d, message length %d", config_option_len,mesg_length);

    // Set the Protocol configuration IEI length now.
    ext_buf[(mesg_length - config_option_len) -2] = (byte)(config_option_len + 1);
  }
  }
#endif

  *length = mesg_length;
  return L3_PARSE_SUCCESS;
}

/*===========================================================================
FUNCTION PACK_MS_PDP_MODIFY_ACCEPT 

DESCRIPTION
  This function (encodes)MS_PDP_MODIFY_ACCEPT message to be sent to Network. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T pack_ms_pdp_modify_accept 
(
  word* length, byte* ext_buf,
  pdp_context_info* pdp 
)
{
  word mesg_length;
  mesg_length = 0;

  //Pack the PD, TI and the Message type
  pack_pd_ti_msgtype (ext_buf, pdp->trans_id, MS_MODIFY_PDP_ACCEPT);
  mesg_length += get_message_data_index (ext_buf);  // Move the message length
  *length = mesg_length;
  return L3_PARSE_SUCCESS;
}

/*===========================================================================
FUNCTION PACK_PDP_SEC_ACTIVATE_REQ 

DESCRIPTION
  This function (encodes) packs a PACK_PDP_SEC_ACTIVATE_REQ Layer3 message 
  to be sent to GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T pack_pdp_sec_activate_req 
(
  word* length, 
  byte* ext_buf,
  pdp_context_info* pdp 
)
{
    word msg_length = 0;
    xlate_status_T tft_pack_status = L3_PARSE_SUCCESS;
    uint16 gmm_max_length;
#ifdef FEATURE_REL5
  uint16 config_option_len;
  byte i;
  ue_rev_level_indicator_type ue_revision;
  ue_revision = sm_get_revision_level_indicator();
  config_option_len = 0;    
#endif
  
  

  /* ---------------------------------------------------------------
  ** Check if the Mandatory fields are set for this message. If not
  ** return a L3_PARSE_INVALID to the caller.  
  ** --------------------------------------------------------------*/
  if ((!pdp->nsapi.valid) || (!pdp->llc_sapi.valid) || (!pdp->qos.valid))
  {
    MSG_ERROR_DS_0( SM_SUB,"Mandatory Field is not Set");
    return L3_PARSE_INVALID;
  }
  gmm_max_length = gmm_max_L3_msg_length(MM_PS_DOMAIN);
  /* ---------------------------------------------------------------
  ** Pack the Mandatory fields of the message.
  ** --------------------------------------------------------------*/
  //Pack the PD, TI and the Message type
  pack_pd_ti_msgtype (ext_buf, pdp->trans_id, ACTIVATE_SEC_PDP_REQUEST);
  msg_length += get_message_data_index (ext_buf);  // Move the message length
 
  ext_buf[msg_length++] = pdp->nsapi.nsapi;     // Pack NSAPI  
  ext_buf[msg_length++] = pdp->llc_sapi.sapi;   // Pack LLC SAPI
  pack_qos(ext_buf, &pdp->qos, (uint16 *)&msg_length);  // Pack QOS
  //pack the linked TI
  pack_linked_ti(&msg_length, pdp->linked_ti, ext_buf);
  //pack the TFT if it is valid
  if (pdp->tft.valid)
  {
    tft_pack_status = pack_tft(&msg_length, &pdp->tft, ext_buf);
  }

  /* Return L3_PARSE_FAIL if TFT packing results in overflow of MAX_OCTETS_L3_MESSAGE */
  if (tft_pack_status == L3_PARSE_FAIL)
  {
    MSG_ERROR_DS_1(SM_SUB,"TFT cannot be packed, TFT pack status %d",tft_pack_status);
    pdp->current_cause = (cc_cause_T)SM_MESSAGE_EXCEED_MAX_L2_LIMIT;
    return L3_PARSE_FAIL;
  }
  
#ifdef FEATURE_REL5
  if ((ue_revision == UE_REV_REL99) && (mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL5))
  {
  // Check if Protocol Configuration options field is included
  if (pdp->config_options.valid)
  {
    // pack the IE value of Optional Protocol Configurations
    ext_buf[msg_length++] = IEI_PROTOCOL_CFG_OPTIONS;

    // Pack the IEI length : This is temporarily set to '0'
    ext_buf[msg_length++] = 0;
   
    // Pack the Configuration protocol field
    ext_buf[msg_length] = 0x80;  //set ext bit 1 and spare bits
    ext_buf[msg_length++] = ( (pdp->config_options.config_protocol & 0x07) | ext_buf[msg_length]) ;   
    
    // Pack all the individual protocol IDs
    for(i=0; i<pdp->config_options.num_protocol_ids; i++)
    { 
      /* Return parse failure if PCO exceeds MAX_OCTETS_L3_MESSAGE */
      if(gmm_max_length - (msg_length -1) < (3 + pdp->config_options.procol_info[i].protocol_len ))
      { 
         MSG_ERROR_DS_3( SM_SUB,"Protocol onwards ID %d, %d is skipped, message buff exceeding. Proto ID len: %d",
                          pdp->config_options.procol_info[i].protocol_id[0],
                          pdp->config_options.procol_info[i].protocol_id[1],
                          pdp->config_options.procol_info[i].protocol_len);
         pdp->current_cause = (cc_cause_T)SM_MESSAGE_EXCEED_MAX_L2_LIMIT;           
         return L3_PARSE_FAIL;
       }
      
      ext_buf[msg_length++] = 
        pdp->config_options.procol_info[i].protocol_id[0]; 

      ext_buf[msg_length++] = 
        pdp->config_options.procol_info[i].protocol_id[1];

      config_option_len += 2;
   
      ext_buf[msg_length++] = 
        pdp->config_options.procol_info[i].protocol_len;

      config_option_len ++;

      memscpy(&ext_buf[msg_length], (gmm_max_length - msg_length),
              pdp->config_options.procol_info[i].protocol_contents, 
              pdp->config_options.procol_info[i].protocol_len);

      msg_length = (msg_length + 
                           MIN(pdp->config_options.procol_info[i].protocol_len, (gmm_max_length - msg_length )));
      config_option_len = (config_option_len +
                           MIN(pdp->config_options.procol_info[i].protocol_len, (gmm_max_length - msg_length )));
    }
    MSG_HIGH_DS_2(SM_SUB,"=MM= config option length %d, message length %d", config_option_len,msg_length);

    // Set the Protocol configuration IEI length now.
    ext_buf[(msg_length - config_option_len) -2] = (byte)(config_option_len + 1);
  }
  }
#endif
  //set the message length
  *length = msg_length;
  return L3_PARSE_SUCCESS;
}

/*===========================================================================
FUNCTION PACK_SM_STATUS 

DESCRIPTION
  This function (encodes) packs a SM_STATUS message.

DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T pack_sm_status 
(
  word* length, 
  byte* ext_buf,
  pdp_context_info* pdp 
)
{
  word mesg_length;
  mesg_length = 0;

  //Pack the PD, TI and the Message type
  pack_pd_ti_msgtype (ext_buf, pdp->trans_id, SM_STATUS);
  mesg_length += get_message_data_index (ext_buf);  // Move the message length
  
  //Pack the SM cause for the status message
  ext_buf[mesg_length++] = (byte)pdp->current_cause;
 
  *length = mesg_length;
  return L3_PARSE_SUCCESS;
}

#ifdef FEATURE_DUAL_DATA
/*===========================================================================
FUNCTION SM_PACK_PDP_CONTEXT_STATUS

DESCRIPTION
  This function packs the PDP context status for 16 NSAPIs(0-15) in 16 bits

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  This function will be called by mm directly
===========================================================================*/
void sm_pack_pdp_context_status(byte *ext_buf, sys_modem_as_id_e_type  as_id)
{
  
  word pdp_index;
  boolean status;

  //Reset the buffer
  (void)memset(ext_buf, 0, 2);

  if(SM_IS_VALID_AS_ID(as_id) == FALSE)
  {
    return;
  }

  //Now Pack the bits
  for (pdp_index = 0; pdp_index < MAX_PDP_NUM; pdp_index++)
  {
    /* ----------------------------------------------------------------------
    **  Enter the Critical Section.
    ** --------------------------------------------------------------------*/
    rex_enter_crit_sect(&sm_crit_sect);

    if ( pdp_list_sim[as_id][pdp_index] != NULL && 
         pdp_list_sim[as_id][pdp_index]->trans_id != INVALID_ID && 
         pdp_list_sim[as_id][pdp_index]->nsapi.nsapi < MAX_NSAPI_NUM // to bypass run time case of invalid NSAPI and valid TI
       )
    {
      status = ((pdp_list_sim[as_id][pdp_index]->pdp_state == PDP_ACTIVE ||
                     pdp_list_sim[as_id][pdp_index]->pdp_state == PDP_MODIFY_PENDING ||
                     pdp_list_sim[as_id][pdp_index]->pdp_state == PDP_INACTIVE_PENDING||
                     pdp_list_sim[as_id][pdp_index]->pdp_state == PDP_ACTIVE_PENDING)
#ifdef FEATURE_GSM_GPRS
                    && ( !(is_ps_gsm_mode(as_id)) ||
                          (pdp_list_sim[as_id][pdp_index]->llc_sapi.valid
                          && pdp_list_sim[as_id][pdp_index]->radio_pri.valid))
#endif
                   )? TRUE: FALSE;

      //Now pack this status
      if (pdp_list_sim[as_id][pdp_index]->nsapi.nsapi < BYTE_LEN_INBITS)
      {
        b_packb(status, ext_buf, (word)((BYTE_LEN_INBITS - 1) - pdp_list_sim[as_id][pdp_index]->nsapi.nsapi), 1);
      }
      else
      {
        b_packb(status, ext_buf + 1, (word)((2 * BYTE_LEN_INBITS - 1) - pdp_list_sim[as_id][pdp_index]->nsapi.nsapi), 1);
      }
    }
    /* ----------------------------------------------------------------------
    **  Leave the Critical Section.
    ** --------------------------------------------------------------------*/
    rex_leave_crit_sect(&sm_crit_sect); 
  } 
}

#else
/*===========================================================================
FUNCTION SM_PACK_PDP_CONTEXT_STATUS

DESCRIPTION
  This function packs the PDP context status for 16 NSAPIs(0-15) in 16 bits

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  This function will be called by mm directly
===========================================================================*/
void sm_pack_pdp_context_status(byte *ext_buf, sys_modem_as_id_e_type as_id)
{
  
  word pdp_index;
  boolean status;
  //Reset the buffer
  (void)memset(ext_buf, 0, 2);

  //Now Pack the bits
  for (pdp_index = 0; pdp_index < MAX_PDP_NUM; pdp_index++)
  {
    /* ----------------------------------------------------------------------
    **  Enter the Critical Section.
    ** --------------------------------------------------------------------*/
    rex_enter_crit_sect(&sm_crit_sect);

    if ( pdp_list[pdp_index] != NULL && 
         pdp_list[pdp_index]->trans_id != INVALID_ID && 
         pdp_list[pdp_index]->nsapi.nsapi < MAX_NSAPI_NUM // to bypass run time case of invalid NSAPI and valid TI
       )
    {
      status = ((pdp_list[pdp_index]->pdp_state == PDP_ACTIVE ||
                     pdp_list[pdp_index]->pdp_state == PDP_MODIFY_PENDING ||
                     pdp_list[pdp_index]->pdp_state == PDP_INACTIVE_PENDING||
                     pdp_list[pdp_index]->pdp_state == PDP_ACTIVE_PENDING)
#ifdef FEATURE_GSM_GPRS
                    && ( !(is_gsm_mode()) ||
                          (pdp_list[pdp_index]->llc_sapi.valid
                          && pdp_list[pdp_index]->radio_pri.valid))
#endif
                   )? TRUE: FALSE;

      //Now pack this status
      if (pdp_list[pdp_index]->nsapi.nsapi < BYTE_LEN_INBITS)
      {
        b_packb(status, ext_buf, (word)((BYTE_LEN_INBITS - 1) - pdp_list[pdp_index]->nsapi.nsapi), 1);
      }
      else
      {
        b_packb(status, ext_buf + 1, (word)((2 * BYTE_LEN_INBITS - 1) - pdp_list[pdp_index]->nsapi.nsapi), 1);
      }
    }
    /* ----------------------------------------------------------------------
    **  Leave the Critical Section.
    ** --------------------------------------------------------------------*/
    rex_leave_crit_sect(&sm_crit_sect); 
  } 
}
#endif

#ifdef FEATURE_MBMS
#error code not present
#endif
/* -------------------------
** UNPACK ROUTINES BELOW 
** -----------------------*/

/*===========================================================================
FUNCTION VALIDATE_LAYER3_MSG 

DESCRIPTION
  This function validates a received Layer3 message.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  Transaction Id.

SIDE EFFECTS

===========================================================================*/
xlate_status_T validate_layer3_msg
(
  byte* ext_buf,         /* Buffer contains the received message */
  word* trans_id        /* Transaction Id returned after validation */
)
{
  word protocol_disc;
  xlate_status_T status;

  status = L3_PARSE_SUCCESS;
  
  // Check if the message is being sent to the Session Management PD.
  protocol_disc = ext_buf[0] & 0x0F;

  if (protocol_disc != SM_PD)
  {
    MSG_ERROR_DS_0( SM_SUB,"Ignore L3 Mesg, Wrong PD for SM");
    status = L3_PARSE_INVALID_PD_TI;
  }

  *trans_id = ext_buf[0] & 0x70;

  *trans_id = *trans_id >> 4;

#ifdef FEATURE_EXTENDED_TI

  if (*trans_id == 0x07)
  {
    *trans_id = ext_buf[1] & 0x7F;
    if (*trans_id < 0x07) 
    {
        // Enhanced TI value less than 7
        MSG_ERROR_DS_1( SM_SUB,"Enhanced TI value less than 7 : %d",*trans_id);
        status = L3_PARSE_INVALID_PD_TI;
    }
  }
    else if (*trans_id > 0x7F)
    {
        // Enhanced TI value less than 7
        MSG_ERROR_DS_1( SM_SUB,"Enhanced TI value greater than 127 : %d",*trans_id);
        status = L3_PARSE_INVALID_PD_TI;
    }
  } 
#else
  
  if (*trans_id == 0x07)
  {
    // Enhanced TI value is being used.
    MSG_ERROR_DS_1( SM_SUB,"Enhanced TI not supported : %d",*trans_id);
    *trans_id = ext_buf[1];
    status = L3_PARSE_INVALID_PD_TI;
  }
  
#endif
  
  MSG_MED_DS_1(SM_SUB, "Received message for TI = %d",*trans_id);
  return status;
}


/*===========================================================================
FUNCTION UNPACK_LAYER3_MESG 

DESCRIPTION
  This function (decodes) unpacks a Layer3 message to be sent to GMM. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T unpack_layer3_mesg
(
  uint16 length,             /* Length in bits of the Packed Layer3 message */
  byte* ext_buf,            /* Buffer contains the received message */
  pdp_context_info* pdp,    /* unpacked into the pdp context */
  sm_layer_msg_T* recd_msg  /* Received Message type */
)
{
  xlate_status_T status;    /* Return status after Unpacking L3 message */
     
  // process only the allowed message length. Higher layers will decide on rejection based on insufficient information.
   length = (uint16) MIN(length, gmm_max_L3_msg_length(MM_PS_DOMAIN));

  // Unpack the received L3 message from the network
  *recd_msg = (sm_layer_msg_T)ext_buf[get_message_data_index (ext_buf)-1];
  MSG_HIGH_DS_1( SM_SUB,"UnPacking L3 MSG:0x%X", *recd_msg);
  switch (*recd_msg)
  {
    case ACTIVATE_PDP_ACCEPT:      
      status = unpack_activate_pdp_accept(length, ext_buf, pdp);
    break;

    case ACTIVATE_PDP_REJECT:
      status = unpack_activate_pdp_reject(length, ext_buf, pdp);
    break;

    case REQUEST_PDP_ACTIVATION:
      status = unpack_request_pdp_activation(length, ext_buf, pdp);
    break;

    case DEACTIVATE_PDP_ACCEPT:
      status = L3_PARSE_SUCCESS;
     /* No extra fields to unpack for this message */
    break;

    case DEACTIVATE_PDP_REQUEST:
      status = unpack_request_pdp_deactivation(length, ext_buf, pdp);  
    break;

    case MODIFY_PDP_ACCEPT:
      status = unpack_modify_pdp_accept(length, ext_buf, pdp);
    break;

    case MODIFY_PDP_REJECT:
      status = unpack_modify_pdp_reject(length, ext_buf,pdp);
    break;

    case ACTIVATE_SEC_PDP_ACCEPT:
      status = unpack_activate_sec_pdp_accept(length, ext_buf,pdp);
    break;

    case ACTIVATE_SEC_PDP_REJECT:
      status = unpack_activate_sec_pdp_reject(length, ext_buf,pdp);
    break;

    case NETWORK_MODIFY_PDP_REQUEST:
      status = unpack_network_modify_pdp_req(length, ext_buf,pdp);
    break;

    case SM_STATUS:
      /*Get the SM cause*/
      pdp->status_message_cause = ext_buf[get_message_data_index (ext_buf)];
      status = L3_PARSE_SUCCESS;     
    break;
   
   #ifdef FEATURE_MBMS
#error code not present
#endif

    case REQUEST_SEC_PDP_ACTIVATION:
      status = unpack_request_sec_pdp_activation(length, ext_buf, pdp);
    break;
    default:
      status = L3_UNSUPPORTED_MSG;
      MSG_ERROR_DS_1( SM_SUB,"Unknown Message received: %d", *recd_msg);
    break;
  };

  return status;
}

/*===========================================================================
FUNCTION UNPACK_ACTIVATE_PDP_ACCEPT 

DESCRIPTION
  This function (decodes) unpacks the ACTIVATE_PDP_ACCEPT message received
  from the Network.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T unpack_activate_pdp_accept
(
  uint16 msg_length, 
  byte* ext_buf, 
  pdp_context_info *pdp
)
{
  uint16    iei_length;
  uint16    msg_index;
  byte    optional_ie_index;
  byte    len_mandatory_fields;
  xlate_status_T status;

  msg_index = 0;
  status = L3_PARSE_SUCCESS;

    /* ----------------------------------------------------------------------
  ** Unpack the Mandatory IEs from the message.
  ** --------------------------------------------------------------------*/

  // skip the PD_TI and the Message Type octets.
  msg_index = get_message_data_index (ext_buf);

  pdp->llc_sapi.valid = TRUE;
  // unpack the LLC-SAPI field. this field is not used for UMTS.
  pdp->llc_sapi.sapi = ext_buf[msg_index++]; 

  if(pdp->llc_sapi.sapi != LLC_SAPI_3 &&
     pdp->llc_sapi.sapi != LLC_SAPI_5 &&
     pdp->llc_sapi.sapi != LLC_SAPI_9 &&
     pdp->llc_sapi.sapi != LLC_SAPI_11 ) 
  {
    pdp->llc_sapi.valid = FALSE;
  }

#ifdef FEATURE_GSM_GPRS
  if ( is_gsm_mode () && 
       pdp->llc_sapi.valid == FALSE)
  {
    MSG_ERROR_DS_1(SM_SUB,"Invalid mandatory LLC SAPI field error %d", pdp->llc_sapi.sapi);
    return L3_INVALID_MANDATORY_INFO; 
  }
#endif

  // unpack the negotiated QOS
  qos_ie_length = unpack_qos( ext_buf, pdp, &msg_index);

  // unpack the radio priority. The spare length is also incremented.
  pdp->radio_pri.radio_priority = ext_buf[msg_index++];

  // If the received Radio priority is above level4, set it to level 4.
  if (pdp->radio_pri.radio_priority > RADIO_PRI_LEVEL4 )
  {
    pdp->radio_pri.radio_priority = RADIO_PRI_LEVEL4;
  }

  // Check if the received message has the minimum length required.
  if ( qos_ie_length > 3 )
    len_mandatory_fields = SM_ACTIVATE_PDP_CONTEXT_ACCEPT_LEN_REL99;
  else
    len_mandatory_fields = SM_ACTIVATE_PDP_CONTEXT_ACCEPT_LEN_REL97;

  if (msg_length < len_mandatory_fields)
  {
    MSG_LOW_DS_0(SM_SUB,"Invalid mandatory field error");
    return L3_INVALID_MANDATORY_INFO; 
  } 

  // processed all the Mandatory fields successfully, set the valid field.
  pdp->radio_pri.valid  = TRUE;
  pdp->qos.valid        = TRUE;

     
  /* ----------------------------------------------------------------------
  ** Parse the non-imperative message part. Set the valid fields to FALSE
  ** ------------------------------------------------------------------- */
  pdp->pdp_addr.valid   = FALSE;
  pdp->config_options.valid = FALSE;
  pdp->config_options.num_protocol_ids = 0;
  pdp->flow_id.valid        = FALSE;

  optional_ie_index = 0;
#ifdef FEATURE_DUAL_IP_BEARER
  pdp->current_cause = SM_UNKNOWN_CAUSE;
#endif
  while (msg_index < msg_length)
  {
    /* ------------------------------------------------------------------
    ** Optional IEs listed in order specified in TS 24.008 Table 9.5.2
    ** --------------------------------------------------------------- */
    switch (ext_buf[msg_index++])
    {  
      case IEI_PDP_ADDRESS:
        // Get the IE length from this IE
        iei_length = ext_buf[msg_index];

        if ((optional_ie_index == 0) && 
              ((msg_index + iei_length) <= msg_length))
        {
          status = unpack_pdp_addr (ext_buf, &msg_index, pdp);

          if (status != L3_PARSE_SUCCESS)
          {
            pdp->pdp_addr.valid = FALSE;
            return status;
          }
          optional_ie_index = 1;
        }
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }

      break;

      case IEI_PROTOCOL_CFG_OPTIONS:

        // Get the IE length from this IE
        iei_length = ext_buf[msg_index];

        if ((optional_ie_index <= 1) && 
              ((msg_index + iei_length) <= msg_length))
        {
          status = unpack_proto_config(ext_buf, &msg_index, pdp);   
          
          if (status != L3_PARSE_SUCCESS)
          {
            return status;
          }

          optional_ie_index = 2;
        }                
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }

      break;

      case IEI_PACKET_FLOW_IDENT:

        // Get the IE length from this IE
        iei_length = ext_buf[msg_index++];

        if ((optional_ie_index <= 2) && 
              ((msg_index + iei_length) <= msg_length))
        {
          // Process the PDP Address optional IE field.
          pdp->flow_id.valid = TRUE;
        
          pdp->flow_id.packet_flow_id = ext_buf[msg_index++];

          SM_VALIDATE_PFI( pdp->flow_id.packet_flow_id );          

          optional_ie_index = 3;
        }        
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }

      break;

#ifdef FEATURE_DUAL_IP_BEARER
      case IEI_SM_CAUSE:

        // Get the IE length from this IE
        iei_length = ext_buf[msg_index++];

        if ((optional_ie_index <= 3) && 
              ((msg_index + iei_length) <= msg_length))
        {
          pdp->current_cause = ext_buf[msg_index++];
          optional_ie_index = 4;
        }
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }
        break;
#endif

      default:
        /* ----------
        ** Unknown IE
        ** ---------- */ 
        status = process_unknown_ie(ext_buf, &msg_index);
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        } 

        break;
    } /* End of Switch statement */
  }

  return status;
}

 
/*===========================================================================
FUNCTION UNPACK_REQUEST_PDP_ACTIVATION 

DESCRIPTION
  This function (decodes) unpacks REQUEST_PDP_ACTIVATION sent by Network
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T unpack_request_pdp_activation
(
  uint16 msg_length, 
  byte* ext_buf, 
  pdp_context_info *pdp
)
{
  uint16 msg_index, iei_length;
  byte optional_ie_index;
  xlate_status_T status;

  msg_index = 0;
  status = L3_PARSE_SUCCESS;

  
  // Check if the received message has the minimum length required.
  if (msg_length < SM_REQUEST_PDP_CONTEXT_ACTIVATION_LEN)
  {
    MSG_ERROR_DS_0(SM_SUB,"Invalid mandatory field error");
    return L3_INVALID_MANDATORY_INFO; 
  }

  /* ----------------------------------------------------------------------
  ** Unpack the Mandatory IEs 
  ** --------------------------------------------------------------------*/

  // skip the PD_TI and the Message Type octets.
  msg_index += get_message_data_index (ext_buf);

  // Unpack the PDP address IE
  status = unpack_pdp_addr (ext_buf, &msg_index, pdp);
/*10.5.6.4 of 24.008
Length of PDP address contents (octet 2)
If the value of octet 2 equals 0000 0010, then:
- No PDP address is included in this information element; and
- If the PDP type is IP, dynamic addressing is applicable.*/
  pdp->pdp_addr_alloc_type = (pdp->pdp_addr.pdp_addr_len )
                              ?PDP_ADDR_STATIC:PDP_ADDR_DYNAMIC;
  

  if (status != L3_PARSE_SUCCESS)
  {
    return status;
  }

  /* ----------------------------------------------------------------------
  ** Process non imperative IEs in the message, if included.
  ** --------------------------------------------------------------------*/
  pdp->apn_name.valid = FALSE;
#ifdef FEATURE_REL5
  pdp->config_options.valid = FALSE;
  pdp->config_options.num_protocol_ids = 0;
#endif
  optional_ie_index = 0;

  while (msg_index < msg_length)
  {
    switch (ext_buf[msg_index++])
    {
      /* ------------------------------------------------------------------
      ** Optional IEs listed in order specified in TS 24.008 Table 9.5.2
      ** --------------------------------------------------------------- */
      case IEI_ACC_POINT_NAME:

        // Get the IE length from this IE
        iei_length = ext_buf[msg_index++];

        if ((optional_ie_index == 0) && 
              ((msg_index + iei_length) <= msg_length))
        {
          pdp->apn_name.valid = TRUE;

          // Unpack the APN info
          pdp->apn_name.apn_addr_len = iei_length;

          if (pdp->apn_name.apn_addr_len > MAX_APN_ADR_LEN)
          { 
            MSG_ERROR_DS_1(SM_SUB,"Incorrect APN Address length: %d - truncate and proceed", pdp->apn_name.apn_addr_len);
            pdp->apn_name.apn_addr_len = (byte)MIN(pdp->apn_name.apn_addr_len, MAX_APN_ADR_LEN);
          }
          memscpy(pdp->apn_name.address, sizeof(pdp->apn_name.address),
                  &ext_buf[msg_index], pdp->apn_name.apn_addr_len );

          msg_index = (uint16)(msg_index + iei_length);

          optional_ie_index = 1;
        }
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }
      break;
#ifdef FEATURE_REL5
      case IEI_PROTOCOL_CFG_OPTIONS:

        // Get the IE length from this IE
        iei_length = ext_buf[msg_index];

        if ((optional_ie_index <= 1) && 
              ((msg_index + iei_length) <= msg_length))
        {
          status = unpack_proto_config(ext_buf, &msg_index, pdp);   
          
          if (status != L3_PARSE_SUCCESS)
          {
            return status;
          }

          optional_ie_index = 2;
        }                
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }

      break;
#endif

      default:
        /* ----------
        ** Unknown IE
        ** ---------- */ 
        status = process_unknown_ie(ext_buf, &msg_index);
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        } 
      break;
    } /* End of Switch statement */
  } 
  
  return status;
}

/*===========================================================================
FUNCTION UNPACK_REQUEST_PDP_DEACTIVATION 

DESCRIPTION
  This function (decodes) unpacks REQUEST_PDP_DEACTIVATION sent by Network
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T unpack_request_pdp_deactivation
(
  uint16 msg_length, 
  byte* ext_buf, 
  pdp_context_info *pdp
)
{
  uint16 msg_index;
  ue_rev_level_indicator_type ue_revision;
#ifdef FEATURE_REL5
  byte optional_ie_index,iei_length;
  xlate_status_T status;
  status = L3_PARSE_SUCCESS;
#endif
  ue_revision = sm_get_revision_level_indicator();

  // Initialize the msg_index to 2, skip the pd_ti and msg fields.
  msg_index = get_message_data_index (ext_buf);
 
  
  // Check if the received message has the minimum length required.
  if (msg_length < SM_DEACTIVATE_PDP_CONTEXT_REQUEST_LEN)
  {
    MSG_ERROR_DS_0(SM_SUB,"Invalid mandatory field error");
    return L3_INVALID_MANDATORY_INFO; 
  }

  // decode the cause of the rejection 
  pdp->current_cause = ext_buf[msg_index++];
 
  if (ue_revision == UE_REV_REL99)
  {  
    // decode the Tear down Indicator
    if( ext_buf[msg_index] == (IEI_TI_INDICATOR | 0x01))
    {
      MSG_MED_DS_0( SM_SUB,"TI Indicator included by Network");
      pdp->teardown_ind.valid   = TRUE;
      pdp->teardown_ind.ti_flag = TRUE;
#ifdef FEATURE_REL5
      msg_index++;
#endif
    }
    else
    {
      pdp->teardown_ind.valid = FALSE;
      pdp->teardown_ind.ti_flag = FALSE;
    }
#ifdef FEATURE_REL5
  /* ----------------------------------------------------------------------
  ** Parse the non-imperative message part. Set the valid fields to FALSE
  ** ------------------------------------------------------------------- */
   pdp->config_options.valid = FALSE;
   pdp->config_options.num_protocol_ids = 0;
   optional_ie_index = 0;
#ifdef FEATURE_NAS_REL11 
   pdp->sm_gprs_type3_timer.active = TRUE;
   pdp->sm_gprs_type3_timer.timer_count = 0;
#endif
   while (msg_index < msg_length)
   {
    switch (ext_buf[msg_index++])
    {
      case IEI_PROTOCOL_CFG_OPTIONS:

        // Get the IE length from this IE
        iei_length = ext_buf[msg_index];

        if ((optional_ie_index == 0) && 
              ((msg_index + iei_length) <= msg_length))
        {
          status = unpack_proto_config(ext_buf, &msg_index, pdp);   
          
          if (status != L3_PARSE_SUCCESS)
          {
            return status;
          }

          optional_ie_index = 1;
        }                
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }

      break;
#ifdef FEATURE_MBMS
      #error code not present
#endif
#ifdef FEATURE_NAS_REL11
      case IEI_GPRS_TIMER_3_IE:

      // Get the IE length from this IE
      iei_length = ext_buf[msg_index];

      if ((optional_ie_index <= 1) && 
            ((msg_index + iei_length) <= msg_length)&&
            ((mm_nv_nas_rel_compliance > NV_NAS_COMPLIANCE_REL10)
              #ifdef FEATURE_LTE
              ||(emm_3gpp_spec_ver > LTE_3GPP_REL10_SEP12)
              #endif
             )            
          )
      {
        status = unpack_gprs_timer_3(ext_buf, &msg_index, pdp);   

        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        }
        optional_ie_index = 2;
      }                
      else
      {
        msg_index = (uint16)(msg_index + iei_length);
      }      

      break;
#endif
      default:
        /* ----------
        ** Unknown IE
        ** ---------- */ 
        status = process_unknown_ie(ext_buf, &msg_index);
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        } 
      break;
      
     }
   }
#endif

  }
    
  return L3_PARSE_SUCCESS;
}

/*===========================================================================
FUNCTION UNPACK_ACTIVATE_PDP_REJECT 

DESCRIPTION
  This function (decodes) unpacks ACTIVATE_PDP_REJECT message sent by Network
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T unpack_activate_pdp_reject
(
  uint16 msg_length, 
  byte* ext_buf,
  pdp_context_info *pdp
)
{
  uint16 msg_index;
  uint16 iei_length;
  byte optional_ie_index;
  xlate_status_T status;
  
  msg_index = get_message_data_index (ext_buf);     // Skip the PD_TI and the MSG_TYPE fields of the Msg.
  status = L3_PARSE_SUCCESS;

  // Check if the received message has the minimum length required.
  if (msg_length < SM_ACTIVATE_PDP_CONTEXT_REJECT_LEN)
  {
    MSG_ERROR_DS_0(SM_SUB,"Invalid mandatory field error");
    return L3_INVALID_MANDATORY_INFO; 
  }
  
  
  // decode the cause of the rejection 
  pdp->current_cause = ext_buf[msg_index++];

  /* ----------------------------------------------------------------------
  ** Parse the non-imperative message part. Set the valid fields to FALSE
  ** ------------------------------------------------------------------- */
  pdp->config_options.valid = FALSE;
  pdp->config_options.num_protocol_ids = 0; 
  
  optional_ie_index = 0;
  msg_index = SM_MODIFY_PDP_CONTEXT_REJECT_LEN; // later IMP CRITICAL why this assignment is made

  pdp->sm_gprs_type3_timer.active = TRUE;
  pdp->sm_gprs_type3_timer.timer_count = 0;

  while (msg_index < msg_length)
  {
    switch (ext_buf[msg_index++])
    {
      // Check if the IE is included

      case IEI_PROTOCOL_CFG_OPTIONS:

      // Get the IE length from this IE
      iei_length = ext_buf[msg_index];

      if ((optional_ie_index <= 1) && 
            ((msg_index + iei_length) <= msg_length))
      {
        status = unpack_proto_config(ext_buf, &msg_index, pdp);   
          
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        }

        optional_ie_index = 1;
      }                
      else
      {
        msg_index = (uint16)(msg_index + iei_length);
      }
      break;
      case IEI_GPRS_TIMER_3_IE:

      // Get the IE length from this IE
      iei_length = ext_buf[msg_index];

      if ((optional_ie_index <= 1) && 
            ((msg_index + iei_length) <= msg_length)&&
            ((mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL10)
              #ifdef FEATURE_LTE
              ||(emm_3gpp_spec_ver >=LTE_3GPP_REL10_SEP12)
              #endif
             )            
          )
      {
        status = unpack_gprs_timer_3(ext_buf, &msg_index, pdp);   
          
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        }

        optional_ie_index = 2;
      }                
      else
      {
        msg_index = (uint16)(msg_index + iei_length);
      }      

      break;

      default:
        /* ----------
        ** Unknown IE
        ** ---------- */ 
        status = process_unknown_ie(ext_buf, &msg_index);
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        } 
      break;
    }
  }

  return status;
}

/*===========================================================================
FUNCTION UNPACK_MODIFY_PDP_ACCEPT 

DESCRIPTION
  This function (decodes) unpacks MODIFY_PDP_ACCEPT message sent by Network.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
/*lint -e550*/
xlate_status_T unpack_modify_pdp_accept
(
  uint16 msg_length, 
  byte* ext_buf,
  pdp_context_info *pdp
)
{
  uint16    iei_length;
  uint16    msg_index;
  byte    optional_ie_index;
  xlate_status_T status;

  msg_index = 0;
  status = L3_PARSE_SUCCESS;

  
  /* ----------------------------------------------------------------------
  ** Unpack the Mandatory IEs from the message.
  ** --------------------------------------------------------------------*/

  // skip the PD_TI and the Message Type octets.
  msg_index = get_message_data_index (ext_buf);


  if (msg_length < 2)
  {
    MSG_LOW_DS_0(SM_SUB,"Invalid mandatory field error");
    return L3_INVALID_MANDATORY_INFO; 
  } 

#ifdef FEATURE_REL5
  pdp->config_options.num_protocol_ids = 0;
#endif
  optional_ie_index = 0;

  /* The PFI values explicitly assigned during the PDP context activation shall be kept 
  ** if in the PDP context modification no PFI IE is included.                                      */
  while (msg_index < msg_length)
  {
    /* ------------------------------------------------------------------
    ** Optional IEs listed in order specified in TS 24.008 Table 9.5.2
    ** --------------------------------------------------------------- */
    switch (ext_buf[msg_index++])
    {  
      case IEI_NEW_QOS:
        iei_length = ext_buf[msg_index];
        if ( optional_ie_index == 0 && 
          ((msg_index + iei_length) <= msg_length))
        {
           // unpack the negotiated QOS
          pdp->qos.valid = TRUE;
          qos_ie_length = unpack_qos( ext_buf, pdp, &msg_index);
          optional_ie_index = 1;
        }
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }
        /*Set the req qos to invalid*/
        pdp->req_qos.valid = FALSE;
      break;  

      case IEI_REQUEST_LLC_SAPI:
        pdp->llc_sapi.valid = TRUE;
        pdp->llc_sapi.sapi = ext_buf[msg_index++];
        optional_ie_index = 2;
        //Unpack radio pri if it exists
        if (msg_index < msg_length)
        {
          //see whether next byte has embedded radio pri IEI
          if (b_unpackw(&ext_buf[msg_index], 0, 4) == IEI_NEW_RADIO_PRIORITY)
          {
            pdp->radio_pri.valid = TRUE;
            pdp->radio_pri.radio_priority = b_unpackb(&ext_buf[msg_index], 5, 3);
            msg_index++;
            optional_ie_index = 3;
          }
        }
        /*Set the requested LLC SAPI to invalid*/
        //pdp->req_llc_sapi.valid = FALSE;
      break;
        
      case IEI_PACKET_FLOW_IDENT:

        // Get the IE length from this IE
        iei_length = ext_buf[msg_index++];

        if ((optional_ie_index <= 3) && 
              ((msg_index + iei_length) <= msg_length))
        {
          // Process the PDP Address optional IE field.
          pdp->flow_id.valid = TRUE;
        
          pdp->flow_id.packet_flow_id = ext_buf[msg_index++];

          SM_VALIDATE_PFI( pdp->flow_id.packet_flow_id );          

          optional_ie_index = 4;
        }        
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }

      break;
#ifdef FEATURE_REL5
      case IEI_PROTOCOL_CFG_OPTIONS:

        // Get the IE length from this IE
        iei_length = ext_buf[msg_index];

        if ((optional_ie_index <= 4) && 
              ((msg_index + iei_length) <= msg_length))
        {
          status = unpack_proto_config(ext_buf, &msg_index, pdp);   
          
          if (status != L3_PARSE_SUCCESS)
          {
            return status;
          }

          optional_ie_index = 5;
        }                
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }

      break;
#endif
      

      default:
        /* ----------
        ** Unknown IE
        ** ---------- */ 
        status = process_unknown_ie(ext_buf, &msg_index);
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        } 

        break;
    } /* End of Switch statement */
  }
  return status;
}
/*lint +e550*/
/*===========================================================================
FUNCTION UNPACK_MODIFY_PDP_REJECT 

DESCRIPTION
  This function (decodes) unpacks MODIFY_PDP_REJECT message sent by Network.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T unpack_modify_pdp_reject
(
  uint16 msg_length, 
  byte* ext_buf,
  pdp_context_info *pdp
)
{
  uint16 msg_index;
#ifdef FEATURE_REL5
  uint16    iei_length;
  byte    optional_ie_index;
  xlate_status_T status;
  status = L3_PARSE_SUCCESS;
  #endif
  msg_index = get_message_data_index (ext_buf);  // Skip the PD_TI and the MSG_TYPE fields of the Msg.

  
  // Check if the received message has the minimum length required.
  if (msg_length < SM_MODIFY_PDP_CONTEXT_REJECT_LEN)
  {
    MSG_ERROR_DS_0(SM_SUB,"Invalid mandatory field error");
    return L3_INVALID_MANDATORY_INFO; 
  }

  // decode the cause of the rejection 
  pdp->current_cause = ext_buf[msg_index++];
#ifdef FEATURE_REL5

  /* ----------------------------------------------------------------------
  ** Parse the non-imperative message part. Set the valid fields to FALSE
  ** ------------------------------------------------------------------- */
  pdp->config_options.num_protocol_ids = 0; 
  
  optional_ie_index = 0;
  msg_index = SM_MODIFY_PDP_CONTEXT_REJECT_LEN;

  pdp->sm_gprs_type3_timer.active = TRUE;
  pdp->sm_gprs_type3_timer.timer_count = 0;

  while (msg_index < msg_length)
  {
    switch (ext_buf[msg_index++])
    {
      // Check if the IE is included

      case IEI_PROTOCOL_CFG_OPTIONS:

      // Get the IE length from this IE
      iei_length = ext_buf[msg_index];

      if ((optional_ie_index <= 1) && 
            ((msg_index + iei_length) <= msg_length))
      {
        status = unpack_proto_config(ext_buf, &msg_index, pdp);   
          
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        }

        optional_ie_index = 1;
      }                
      else
      {
        msg_index = (uint16)(msg_index + iei_length);
      }

      break;

      case IEI_GPRS_TIMER_3_IE:

      // Get the IE length from this IE
      iei_length = ext_buf[msg_index];

      if ((optional_ie_index <= 1) && 
          ((msg_index + iei_length) <= msg_length)&&
            ((mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL10)
               #ifdef FEATURE_LTE
              ||(emm_3gpp_spec_ver >=LTE_3GPP_REL10_SEP12)
              #endif
            )
         )
      {
        status = unpack_gprs_timer_3(ext_buf, &msg_index, pdp);   
          
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        }

        optional_ie_index = 2;
      }                
      else
      {
        msg_index = (uint16)(msg_index + iei_length);
      } 

      break;

      default:
        /* ----------
        ** Unknown IE
        ** ---------- */ 
        status = process_unknown_ie(ext_buf, &msg_index);
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        } 
      break;
    }
  }
#endif
  return L3_PARSE_SUCCESS;
}

/*===========================================================================
FUNCTION UNPACK_NETWORK_MODIFY_PDP_REQ 

DESCRIPTION
  This function (decodes) unpacks UNPACK_NETWORK_MODIFY_PDP_REQ message sent
  by Network.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T unpack_network_modify_pdp_req
(
  uint16 msg_length, 
  byte* ext_buf,
  pdp_context_info *pdp
)
{
  uint16           msg_index;
  byte           optional_ie_index;
  uint16           iei_length;
  byte           len_mandatory_fields;
  pdp_context_info* pdp_temp=NULL;
  xlate_status_T status;
  boolean is_nw_initiated_qos_support = ds3gpp_per_subs_get_config_umts_nw_init_qos_support((sys_modem_as_id_e_type)sm_as_id);

  
#ifdef FEATURE_MODEM_HEAP
  pdp_temp = (pdp_context_info *)modem_mem_calloc(1,sizeof(pdp_context_info),MODEM_MEM_CLIENT_NAS);
#else 
  pdp_temp = (pdp_context_info *)gs_alloc(sizeof(pdp_context_info));
#endif

  if (pdp_temp == NULL)
  {
    MSG_ERROR_DS_0(SM_SUB,"Memory Allocation failed");
    return L3_PARSE_INVALID; 
  }

  sm_init_pdp_context(pdp_temp);
  
  status    = L3_PARSE_SUCCESS;
  msg_index = get_message_data_index (ext_buf);    // Skip the PD_TI and the MSG_TYPE fields of the Msg.

  
  if(is_nw_initiated_qos_support)
  {
  // unpack the radio priority 
  pdp->req_radio_pri.valid = TRUE;
  pdp->req_radio_pri.radio_priority = ext_buf[msg_index++];
  // unpack the LLC-SAPI field
  pdp->req_llc_sapi.valid = TRUE;
  pdp->req_llc_sapi.sapi = ext_buf[msg_index++];
  // unpack the negotiated QOS */
  qos_ie_length = unpack_req_qos( ext_buf, pdp, &msg_index);
  pdp->req_qos.valid = TRUE;
  }
  else
  {
    // unpack the radio priority 
    pdp->radio_pri.radio_priority = ext_buf[msg_index++];
    // unpack the LLC-SAPI field
    pdp->llc_sapi.sapi = ext_buf[msg_index++];
    // unpack the negotiated QOS */
  qos_ie_length = unpack_qos( ext_buf, pdp, &msg_index);
  pdp->qos.valid = TRUE;
  }

  // Check if the received message has the minimum length required.
  if ( qos_ie_length > 3 )
    len_mandatory_fields = SM_MODIFY_PDP_CONTEXT_REQUEST_LEN_REL99;
  else
    len_mandatory_fields = SM_MODIFY_PDP_CONTEXT_REQUEST_LEN_REL97;

  // Check if the received message has the minimum length required.
  if (msg_length < len_mandatory_fields)
  {
    MSG_ERROR_DS_0(SM_SUB,"Invalid mandatory field error");

    if (pdp_temp != NULL)
    {
#ifdef FEATURE_MODEM_HEAP
      modem_mem_free(pdp_temp,MODEM_MEM_CLIENT_NAS);
#else
       gs_free(pdp_temp);
#endif
      pdp_temp=NULL;
    }
    return L3_INVALID_MANDATORY_INFO; 
  }
#ifdef FEATURE_GSM_GPRS
  if(is_nw_initiated_qos_support)
  {
  // Check if the LLC SAPI is valid
  if (is_gsm_mode () && 
       pdp->req_llc_sapi.sapi != LLC_SAPI_3 &&
       pdp->req_llc_sapi.sapi != LLC_SAPI_5 &&
       pdp->req_llc_sapi.sapi != LLC_SAPI_9 &&
       pdp->req_llc_sapi.sapi != LLC_SAPI_11 )
  {
    MSG_ERROR_DS_0(SM_SUB,"Invalid mandatory field error");
    if (pdp_temp != NULL)
    {
#ifdef FEATURE_MODEM_HEAP
      modem_mem_free(pdp_temp,MODEM_MEM_CLIENT_NAS);
#else
       gs_free(pdp_temp);
#endif
      pdp_temp=NULL;
    }
    return L3_INVALID_MANDATORY_INFO;
  }
  }
  else
  {
  if ( is_gsm_mode () && 
       pdp->llc_sapi.sapi != LLC_SAPI_3 &&
       pdp->llc_sapi.sapi != LLC_SAPI_5 &&
       pdp->llc_sapi.sapi != LLC_SAPI_9 &&
       pdp->llc_sapi.sapi != LLC_SAPI_11 ) 
  {
    MSG_ERROR_DS_0(SM_SUB,"Invalid mandatory field error");
    if (pdp_temp != NULL)
    {
#ifdef FEATURE_MODEM_HEAP
      modem_mem_free(pdp_temp,MODEM_MEM_CLIENT_NAS);
#else
       gs_free(pdp_temp);
#endif
      pdp_temp=NULL;
    }
    return L3_INVALID_MANDATORY_INFO; 
  }
  }
#endif /*#ifdef FEATURE_GSM_GPRS*/

  if(is_nw_initiated_qos_support == 0)
  {
  // All the mandatory fields are successfully parsed, set the valid field
  pdp->radio_pri.valid  = TRUE;
  pdp->llc_sapi.valid   = TRUE;
  pdp->qos.valid        = TRUE;
  pdp->pdp_addr.valid   = TRUE;
  }

  
  /* ----------------------------------------------------------------------
  ** Parse the non-imperative message part. Set the valid fields to FALSE
  ** ------------------------------------------------------------------- */
  /* The PFI values explicitly assigned during the PDP context activation shall be kept 
  ** if in the PDP context modification no PFI IE is included.                                      */
  if(is_nw_initiated_qos_support)
  {
  pdp->req_flow_id.valid        = FALSE;
  pdp->tft.valid = FALSE;
  }




#ifdef FEATURE_REL5
  pdp->config_options.num_protocol_ids = 0;
#endif
  optional_ie_index = 0;

  while (msg_index < msg_length)
  {
    switch (ext_buf[msg_index++])
    {
      /* ------------------------------------------------------------------
      ** Optional IEs listed in order specified in TS 24.008 Table 9.5.2
      ** --------------------------------------------------------------- */
      case IEI_PDP_ADDRESS:
        
        // Get the IE length from this IE
        iei_length = ext_buf[msg_index];

        if ((optional_ie_index == 0) && 
              ((msg_index + iei_length) <= msg_length))
        {
          status = unpack_pdp_addr (ext_buf, &msg_index, pdp_temp);

          if (status != L3_PARSE_SUCCESS)
          {
            pdp_temp->pdp_addr.valid = FALSE;
#ifdef FEATURE_MODEM_HEAP
            modem_mem_free(pdp_temp,MODEM_MEM_CLIENT_NAS);
#else
            gs_free(pdp_temp);
#endif
            pdp_temp = NULL;
            return status;
          }
          optional_ie_index = 1;          
          /*DS API is used to compare the address as IPv6 address comparison can't be done at UMTS when L to W scenario Happens*/
#ifdef FEATURE_DUAL_SIM  
         if (ds_3gpp_compare_pdp_address((uint8)(pdp->nsapi.nsapi), SYS_SYS_MODE_WCDMA, (sys_modem_as_id_e_type)sm_as_id, &(pdp_temp->pdp_addr) ) != TRUE)
#else
         if (ds_3gpp_compare_pdp_address((uint8)(pdp->nsapi.nsapi), SYS_SYS_MODE_WCDMA, 0, &(pdp_temp->pdp_addr) ) != TRUE)
#endif 
          {
            /* Do Not Support PDP Address modification by Network*/
            MSG_ERROR_DS_0(SM_SUB,"Network Init PDP Addr Mod not supported");
#ifdef FEATURE_MODEM_HEAP
            modem_mem_free(pdp_temp,MODEM_MEM_CLIENT_NAS);
#else
            gs_free(pdp_temp);
#endif
            pdp_temp = NULL;
            return L3_UNSUPPORTED_IE;
          }
        }
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }
        break;


      case IEI_PACKET_FLOW_IDENT:

        // Get the IE length from this IE
        iei_length = ext_buf[msg_index++];

        if ((optional_ie_index <= 1) && 
              ((msg_index + iei_length) <= msg_length))
        {
          // Process the PDP Address optional IE field.
          if(is_nw_initiated_qos_support)
          {
            pdp->req_flow_id.valid = TRUE;

            pdp->req_flow_id.packet_flow_id = ext_buf[msg_index++];

            SM_VALIDATE_PFI( pdp->req_flow_id.packet_flow_id );
          }
          else
          {
            pdp->flow_id.valid = TRUE;
        
            pdp->flow_id.packet_flow_id = ext_buf[msg_index++];

            SM_VALIDATE_PFI( pdp->flow_id.packet_flow_id );          
           }
          optional_ie_index = 2;
        }        

        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }

      break;
#ifdef FEATURE_REL5
      case IEI_PROTOCOL_CFG_OPTIONS:

        // Get the IE length from this IE
        iei_length = ext_buf[msg_index];

        if ((optional_ie_index <= 2) && 
              ((msg_index + iei_length) <= msg_length))
        {
          status = unpack_proto_config(ext_buf, &msg_index, pdp);   
          
          if (status != L3_PARSE_SUCCESS)
          {
            if (pdp_temp != NULL)
          {
#ifdef FEATURE_MODEM_HEAP
              modem_mem_free(pdp_temp,MODEM_MEM_CLIENT_NAS);
#else
               gs_free(pdp_temp);
#endif
              pdp_temp=NULL;
            }
            return status;
          }

          optional_ie_index = 3;
        }                
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }

      break;
#endif
      
      case IEI_TRAFFIC_FLOW_TEMPLATE:

        // Get the IE length from this IE
        iei_length = ext_buf[msg_index];

        if ((optional_ie_index <= 3) &&
             ((msg_index + iei_length) <= msg_length))
        {
          status = unpack_tft(ext_buf, &msg_index, pdp);

          if (status != L3_PARSE_SUCCESS)
          {
            if (pdp_temp != NULL)
          {
#ifdef FEATURE_MODEM_HEAP
              modem_mem_free(pdp_temp,MODEM_MEM_CLIENT_NAS);
#else
               gs_free(pdp_temp);
#endif
              pdp_temp=NULL;
            }
            return status;
          }
          optional_ie_index = 4;
        }
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }
        break;

      default:
        // Process Unknown IE 
        status = process_unknown_ie(ext_buf, &msg_index);
        if (status != L3_PARSE_SUCCESS)
        {
          if (pdp_temp != NULL)
        {
  #ifdef FEATURE_MODEM_HEAP
            modem_mem_free(pdp_temp,MODEM_MEM_CLIENT_NAS);
  #else
             gs_free(pdp_temp);
  #endif
            pdp_temp=NULL;
          }
          return status;
        } 

        break;
    } /* End of Switch statement */
  }
  if (pdp_temp != NULL)
  {
#ifdef FEATURE_MODEM_HEAP
    modem_mem_free(pdp_temp,MODEM_MEM_CLIENT_NAS);
#else
     gs_free(pdp_temp);
#endif
    pdp_temp=NULL;
  }
  return status;
}

/*===========================================================================
FUNCTION UNPACK_ACTIVATE_SEC_PDP_ACCEPT 

DESCRIPTION
  This function (decodes) unpacks ACTIVATE_SEC_PDP_ACCEPT msg sent to Network. 
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
/*lint -e550*/
xlate_status_T unpack_activate_sec_pdp_accept
(
  uint16 msg_length,
  byte* ext_buf,
  pdp_context_info *pdp
)
{
  uint16 msg_index, iei_length;
  byte optional_ie_index;
  xlate_status_T status;

  msg_index = 0;
  status = L3_PARSE_SUCCESS;

  
  // Check if the received message has the minimum length required.
  if (msg_length < SM_SEC_PDP_CONTEXT_ACC_LEN)
  {
    MSG_ERROR_DS_0(SM_SUB,"Invalid mandatory field error");
    return L3_INVALID_MANDATORY_INFO; 
  }

  // skip the PD_TI and the Message Type octets.
  msg_index = get_message_data_index (ext_buf);

  pdp->llc_sapi.valid = TRUE;
  // unpack the LLC-SAPI field. this field is not used for UMTS.
  pdp->llc_sapi.sapi = ext_buf[msg_index++]; 

  if(pdp->llc_sapi.sapi != LLC_SAPI_3 &&
     pdp->llc_sapi.sapi != LLC_SAPI_5 &&
     pdp->llc_sapi.sapi != LLC_SAPI_9 &&
     pdp->llc_sapi.sapi != LLC_SAPI_11 ) 
  {
    pdp->llc_sapi.valid = FALSE;
  }

#ifdef FEATURE_GSM_GPRS
  if (is_gsm_mode () && 
       pdp->llc_sapi.valid == FALSE)
  {
    MSG_ERROR_DS_1(SM_SUB,"Invalid mandatory LLC SAPI field error %d",pdp->llc_sapi.sapi);
    return L3_INVALID_MANDATORY_INFO; 
  }
#endif

  // unpack the negotiated QOS
  qos_ie_length = unpack_qos( ext_buf, pdp, &msg_index);

  // unpack the radio priority. The spare length is also incremented.
  pdp->radio_pri.radio_priority = ext_buf[msg_index++];

  // If the received Radio priority is above level4, set it to level 4.
  if (pdp->radio_pri.radio_priority > RADIO_PRI_LEVEL4 )
  {
    pdp->radio_pri.radio_priority = RADIO_PRI_LEVEL4;
  }

  // processed all the Mandatory fields successfully, set the valid field.
  pdp->radio_pri.valid  = TRUE;
  pdp->qos.valid        = TRUE;

#ifdef FEATURE_REL5
  pdp->config_options.valid = FALSE;
  pdp->config_options.num_protocol_ids = 0;
#endif
  optional_ie_index = 0;
  while (msg_index < msg_length)
  {
    switch (ext_buf[msg_index++])
    {
      /* ------------------------------------------------------------------
      ** Optional IEs listed in order specified in TS 24.008 Table 9.5.2
      ** --------------------------------------------------------------- */
    case IEI_PACKET_FLOW_IDENT:
    {
      // Get the IE length from this IE
      iei_length = ext_buf[msg_index++];

      if ((optional_ie_index <= 1) &&
          (msg_index + iei_length) <= msg_length)
      {
        // Process the PDP Address optional IE field.
        pdp->flow_id.valid = TRUE;
        
        pdp->flow_id.packet_flow_id = ext_buf[msg_index++];

        SM_VALIDATE_PFI( pdp->flow_id.packet_flow_id );

        optional_ie_index = 1;
      }        
      else
      {
        msg_index = (uint16)(msg_index + iei_length);
      }
    }
    break;
#ifdef FEATURE_REL5
    case IEI_PROTOCOL_CFG_OPTIONS:
    {
              // Get the IE length from this IE
        iei_length = ext_buf[msg_index];

        if ((optional_ie_index <= 2) && 
              ((msg_index + iei_length) <= msg_length))
        {
          status = unpack_proto_config(ext_buf, &msg_index, pdp);   
          
          if (status != L3_PARSE_SUCCESS)
          {
            return status;
          }

          optional_ie_index = 2;
        }                
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }
    }
    break;
#endif

      default:
        // Process Unknown IE 
        status = process_unknown_ie(ext_buf, &msg_index);
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        } 

        break;
    }
  }
  return status;
}
/*lint +e550*/
/*===========================================================================
FUNCTION UNPACK_ACTIVATE_SEC_PDP_REJECT 

DESCRIPTION
  This function (decodes) unpacks ACTIVATE_SEC_PDP_REJECT sent to Network.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/

xlate_status_T unpack_activate_sec_pdp_reject
(
  uint16 msg_length,
  byte* ext_buf,
  pdp_context_info *pdp
)
{
  uint16 msg_index;
  xlate_status_T status;
#ifdef FEATURE_REL5
  uint16    iei_length;
  byte    optional_ie_index;
#endif
  
  msg_index = get_message_data_index (ext_buf);     // Skip the PD_TI and the MSG_TYPE fields of the Msg.
  status = L3_PARSE_SUCCESS;
  
   // Check if the received message has the minimum length required.
  if (msg_length < SM_SEC_PDP_CONTEXT_ACT_REJ_LEN)
  {
    MSG_ERROR_DS_0(SM_SUB,"Invalid mandatory field error");
    return L3_INVALID_MANDATORY_INFO; 
  }

  
  // Decode the cause of the rejection 
  pdp->current_cause = ext_buf[msg_index++];

#ifdef FEATURE_REL5
  /* ----------------------------------------------------------------------
  ** Parse the non-imperative message part. Set the valid fields to FALSE
  ** ------------------------------------------------------------------- */
  pdp->config_options.valid = FALSE;
  pdp->config_options.num_protocol_ids = 0; 
  
  optional_ie_index = 0;
  msg_index = SM_SEC_PDP_CONTEXT_ACT_REJ_LEN;

  pdp->sm_gprs_type3_timer.active = TRUE;
  pdp->sm_gprs_type3_timer.timer_count = 0;

  while (msg_index < msg_length)
  {
    switch (ext_buf[msg_index++])
    {
      // Check if the IE is included

      case IEI_PROTOCOL_CFG_OPTIONS:

      // Get the IE length from this IE
      iei_length = ext_buf[msg_index];

      if ((optional_ie_index <= 1) && 
            ((msg_index + iei_length) <= msg_length))
      {
        status = unpack_proto_config(ext_buf, &msg_index, pdp);   
          
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        }

        optional_ie_index = 1;
      }                
      else
      {
        msg_index = (uint16)(msg_index + iei_length);
      }

      break;

      case IEI_GPRS_TIMER_3_IE:

      // Get the IE length from this IE
      iei_length = ext_buf[msg_index];

      if ((optional_ie_index <= 1) && 
            ((msg_index + iei_length) <= msg_length)&&
            ((mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL10)
#ifdef FEATURE_LTE
            ||(emm_3gpp_spec_ver >=LTE_3GPP_REL10_SEP12)
#endif
             )
         )
      {
        status = unpack_gprs_timer_3(ext_buf, &msg_index, pdp);   
          
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        }

        optional_ie_index = 2;
      }                
      else
      {
        msg_index = (uint16)(msg_index + iei_length);
      } 

      break;

      default:
        /* ----------
        ** Unknown IE
        ** ---------- */ 
        status = process_unknown_ie(ext_buf, &msg_index);
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        } 
      break;
    }
  }
#endif
  return status;

}

#ifdef FEATURE_GPRS_CALLCONTROL
/*===========================================================================
FUNCTION UNPACK_PDP_ACTIVATE_MESG 

DESCRIPTION
  This function (decodes) unpacks ACTIVATE_PDP_Request sent to Network.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T unpack_pdp_activate_mesg
(
  uint16 msg_length, 
  byte* ext_buf, 
  pdp_context_info *pdp
)
{
  uint16 msg_index, iei_length;
  byte optional_ie_index;
  xlate_status_T status;

  msg_index = 0;
  status = L3_PARSE_SUCCESS;

  //MSG_MED_DS_0(SM_SUB, "Unpack REQUEST_PDP_ACTIVATION message");

  // Check if the received message has the minimum length required.
  //if (msg_length < SM_REQUEST_PDP_CONTEXT_ACTIVATION_LEN)
  //{
  //  MSG_ERROR_DS_0(SM_SUB,"Invalid mandatory field error");
  //  return L3_INVALID_MANDATORY_INFO; 
  //}

  /* ----------------------------------------------------------------------
  ** Unpack the Mandatory IEs 
  ** --------------------------------------------------------------------*/
  
  // protocol Descr
  //pdp->pd = ext_buf[msg_index] & 0x0F;

  // Trans ident
  pdp->trans_id = ext_buf[msg_index] & 0x70;
  pdp->trans_id = pdp->trans_id >> 4;

  // Message Type octets.
  msg_index += 2;

  // copy nsapi
  pdp->nsapi.nsapi = ext_buf[msg_index++];

  // copy LLC
  pdp->llc_sapi.sapi   = ext_buf[msg_index++];

  // Get QOS
  (void) unpack_qos (ext_buf, pdp, &msg_index);


  // PDP address is a mandatory field and does not have a IEI
  // Reversing whatever is being done in pack_pdp_activate_mesg
  pdp->pdp_addr.valid = TRUE;
  pdp->pdp_addr.pdp_addr_len = ext_buf[msg_index++] - 2;
  pdp->pdp_addr.pdp_type_org = ext_buf[msg_index++];
  pdp->pdp_addr.pdp_type_num = ext_buf[msg_index++]; 

  if((pdp->pdp_addr.pdp_addr_len > MAX_PDP_ADR_LEN) && 
      (msg_length < (pdp->pdp_addr.pdp_addr_len + msg_index)))
  {
    MSG_ERROR_DS_3(SM_SUB,"pdp addr len err pdp_addr_len %d msg_length %d msg_index %d ",
                         pdp->pdp_addr.pdp_addr_len, msg_length, msg_index);
    return L3_PARSE_FAIL;
  }
  memscpy(pdp->pdp_addr.address, sizeof(pdp->pdp_addr.address),
          &ext_buf[msg_index], pdp->pdp_addr.pdp_addr_len);

  msg_index = (uint16)(msg_index + pdp->pdp_addr.pdp_addr_len);


  /* ----------------------------------------------------------------------
  ** Process non imperative IEs in the message, if included.
  ** --------------------------------------------------------------------*/
  pdp->apn_name.valid = FALSE;
#ifdef FEATURE_REL5
  pdp->config_options.valid = FALSE;
  pdp->config_options.num_protocol_ids = 0;
#endif
  optional_ie_index = 0;

  while (msg_index < msg_length)
  {
    switch (ext_buf[msg_index++])
    {
      /* ------------------------------------------------------------------
      ** Optional IEs listed in order specified in TS 24.008 Table 9.5.2
      ** --------------------------------------------------------------- */
      case IEI_ACC_POINT_NAME:

        // Get the IE length from this IE
        iei_length = ext_buf[msg_index++];

        if ((optional_ie_index == 0) && 
              ((msg_index + iei_length) <= msg_length))
        {
          pdp->apn_name.valid = TRUE;

          // Unpack the APN info
          pdp->apn_name.apn_addr_len = iei_length;

          if(pdp->apn_name.apn_addr_len > MAX_APN_ADR_LEN)
          {
            MSG_ERROR_DS_1(SM_SUB,"apn_addr_len: %d exceeded ", pdp->apn_name.apn_addr_len);
          }
          memscpy(pdp->apn_name.address, sizeof(pdp->apn_name.address),
                  &ext_buf[msg_index], pdp->apn_name.apn_addr_len);

          msg_index = (uint16)(msg_index + iei_length);

          if (status != L3_PARSE_SUCCESS)
          {
            return status;
          }
          optional_ie_index = 1;
        }
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }
      break;
#ifdef FEATURE_REL5
      case IEI_PROTOCOL_CFG_OPTIONS:

        // Get the IE length from this IE
        iei_length = ext_buf[msg_index];

        if ((optional_ie_index <= 1) && 
              ((msg_index + iei_length) <= msg_length))
        {
          status = unpack_proto_config(ext_buf, &msg_index, pdp);   
          
          if (status != L3_PARSE_SUCCESS)
          {
            return status;
          }

          optional_ie_index = 2;
        }                
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }

      break;
#endif

      default:
        /* ----------
        ** Unknown IE
        ** ---------- */ 
        status = process_unknown_ie(ext_buf, &msg_index);
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        } 
      break;
    } /* End of Switch statement */
  } 
  
  return status;
}
/*===========================================================================
FUNCTION UNPACK_SEC_PDP_ACTIVATE_MESG 

DESCRIPTION
  This function (decodes) unpacks SECONDARY ACTIVATE_PDP_Request sent to Network.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T unpack_sec_pdp_activate_mesg
(
  word msg_length, 
  byte* ext_buf, 
  pdp_context_info *pdp
)
{
  uint16 msg_index;
  uint16 iei_length;
  byte optional_ie_index;
  byte linked_ti_value;
  xlate_status_T status;

  msg_index = 0;
  status = L3_PARSE_SUCCESS;

//  MSG_MED_DS_0( SM_SUB,"Unpack ACTIVATE_SECONDARY_PDP_CONTEXT message");

  // Check if the received message has the minimum length required.
//  if (msg_length < SM_ACTIVATE_SECONDARY_PDP_CONTEXT_LEN)
//  {
    MSG_ERROR_DS_0(SM_SUB,"Invalid mandatory field error");
//    return L3_INVALID_MANDATORY_INFO; 
//  }

  /* ----------------------------------------------------------------------
  ** Unpack the Mandatory IEs 
  ** --------------------------------------------------------------------*/
  
  // protocol Descr
//  pdp->pd = ext_buf[msg_index] & 0x0F;

  // Trans ident
  pdp->trans_id = ext_buf[msg_index] & 0x70;
  pdp->trans_id = pdp->trans_id >> 4;

  // Message Type octets.
  msg_index += 2;

  // copy nsapi
//shall we need to set the valid field to true ??
  pdp->nsapi.nsapi = ext_buf[msg_index++];

  // copy LLC
    // shall we need to set the valid field to true ??
  pdp->llc_sapi.sapi   = ext_buf[msg_index++];

  // Get QOS
  (void) unpack_qos (ext_buf, pdp, &msg_index);

  // skip the header for Linked TI
//  msg_index++;
  // sushim plz check this out whether for the linked ti the length is one or not.
  if (ext_buf[msg_index] == 1)
    {
        msg_index++;
        linked_ti_value = ext_buf[msg_index] & 0x70;
        linked_ti_value = linked_ti_value >> 4;
        pdp->linked_ti = linked_ti_value;
        msg_index++;
    }
  else {
        // this is a linked TI case. raise Exception. 
        // sushim is this retun value ok ??
        return L3_PARSE_INVALID_PD_TI;
    }

//  msg_index++;
  

  /* ----------------------------------------------------------------------
  ** Process non imperative IEs in the message, if included.
  ** --------------------------------------------------------------------*/
  // LATER
#ifdef FEATURE_REL5
  pdp->config_options.valid = FALSE;
  pdp->config_options.num_protocol_ids = 0;
#endif

  optional_ie_index = 0;

  while (msg_index < msg_length)
  {
    switch (ext_buf[msg_index++])
    {
      /* ------------------------------------------------------------------
      ** Optional IEs listed in order specified in TS 24.008 Table 9.5.2
      ** --------------------------------------------------------------- */
      case IEI_TRAFFIC_FLOW_TEMPLATE:

        // Get the IE length from this IE
        iei_length = ext_buf[msg_index];

        if ((optional_ie_index == 0) && 
              ((msg_index + iei_length) <= msg_length))
        {
          status = unpack_tft(ext_buf, &msg_index, pdp);
          
          if (status != L3_PARSE_SUCCESS)
          {
            return status;
          }

          optional_ie_index = 1;
        }                
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }

      break;

#ifdef FEATURE_REL5
      case IEI_PROTOCOL_CFG_OPTIONS:

        // Get the IE length from this IE
        iei_length = ext_buf[msg_index];

        if ((optional_ie_index <= 1) && 
              ((msg_index + iei_length) <= msg_length))
        {
          status = unpack_proto_config(ext_buf, &msg_index, pdp);   
          
          if (status != L3_PARSE_SUCCESS)
          {
            return status;
          }

          optional_ie_index = 2;
        }                
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }

      break;
#endif

      default:
        /* ----------
        ** Unknown IE
        ** ---------- */ 
        status = process_unknown_ie(ext_buf, &msg_index);
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        } 
      break;
    } /* End of Switch statement */
  } 
  
  return status;
}

/*===========================================================================
FUNCTION UNPACK_TFT 

DESCRIPTION
  This function (decodes) unpacks TFT.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T unpack_tft
(
  byte* ext_buf,
  uint16 *index,
  pdp_context_info* pdp
)

{
  uint16 msg_index;
  uint16 iei_length;
  byte octet3;
  byte operation_code;
  byte e_bit;
  byte num_filters;
  byte direction;
  word i;
  msg_index = *index;

  iei_length = (uint16)(ext_buf[msg_index++] - 1);

  // set the TFT optional value to TRUE.
  pdp->tft.valid = TRUE;
  pdp->tft.tft_total_length = iei_length;
  
  octet3 = ext_buf[msg_index++];
  
  // set the operation code;
  operation_code = octet3 & 0xE0;
  operation_code = operation_code >> 5;
  pdp->tft.operation_code = operation_code;
  
#ifdef FEATURE_REL5
  // set the e_bit
  e_bit= octet3 & 0x10;
  e_bit= e_bit >> 4;
  pdp->tft.e_bit= e_bit;
#endif

  // set the number of packet filters
  num_filters = octet3 & 0x0F;
  pdp->tft.num_filters = num_filters;

  if (pdp->tft.num_filters > MAX_PACKET_FILTERS)
  {
    return L3_PARSE_INVALID;
  }
  // operation_code value is Delete Packet Filters from existing TFT
  if (operation_code == 5)
  {
    for(i=0; i < num_filters; i++)
    {
        pdp->tft.filter_list[i].identifier = ext_buf[msg_index++] ;
    }
  }
  
  // operation_code value is 
  // Create New TFT or 
  // Add Packet Filters to existing TFT or 
  // Replace Packet Filters in existing TFT.
  if (operation_code == 1 || operation_code == 3 || operation_code == 4)
  {
    for(i=0; i < num_filters; i++)
    {
        pdp->tft.filter_list[i].identifier = ext_buf[msg_index++] ;
        if(ds3gpp_per_subs_get_config_umts_nw_init_qos_support((sys_modem_as_id_e_type)sm_as_id))
        {
          // identifier field is not changed in NAS;  DS has the identifier and direction deocde logic as done in LTE
          direction = pdp->tft.filter_list[i].identifier;
          direction = direction & 0x30;
          direction = direction >> 4;
          pdp->tft.filter_list[i].direction = (packet_filter_direction_T)direction;
        }
        else
        {
          pdp->tft.filter_list[i].identifier = pdp->tft.filter_list[i].identifier & 0x07;
        }

        pdp->tft.filter_list[i].eval_precedence = ext_buf[msg_index++];
        pdp->tft.filter_list[i].content_length = ext_buf[msg_index++];
        
        // check for the boundary conditions
        if (pdp->tft.filter_list[i].content_length > MAX_FILTER_CONTENT_LENGTH)
        {
          return L3_PARSE_INVALID;
         }
        memscpy( pdp->tft.filter_list[i].filter_contents, sizeof(pdp->tft.filter_list[i].filter_contents),
        &ext_buf[msg_index], pdp->tft.filter_list[i].content_length);
        
        msg_index = (uint16)(msg_index + pdp->tft.filter_list[i].content_length);
        }
    }
  
  if (operation_code == 6 && iei_length > 1)
  {
    pdp->current_cause = (cc_cause_T)SM_SYNTACTIC_ERR_IN_TFT;
  }
  
  /* structure for supporting Parameter list is Not Defined.in the code
  // check if e_bit is absent and still some message part is present
  if (e_bit == 0 && msg_index < iei_length)
    {
        return L3_PARSE_INVALID;
    }
  // check if e_bit is present and message part is exhausted
    if (e_bit == 1 && msg_index == iei_length)
    {
        return L3_PARSE_INVALID;
    }
  // check if e_bit is present and message part is exhausted
    if (e_bit == 1 && msg_index < iei_length)
    {
    }
    */
  *index = msg_index;
  
  return L3_PARSE_SUCCESS;
}
#endif

/*===========================================================================
FUNCTION PACK_QOS 

DESCRIPTION
  This function (encodes) packs the QOS information in the L3 messages.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void pack_qos(byte* ext_buf, qos_T* qos, uint16* msg_length)
{
  uint16 cur_pos;
  ue_rev_level_indicator_type ue_revision;
  ue_revision = sm_get_revision_level_indicator();

  // Convert the received message length into bits.
  cur_pos = (uint16)(*msg_length * 8);
#ifdef FEATURE_REL5
  // Pack OCTET 1
  if ((ue_revision == UE_REV_REL99) && (mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL5))
  {
   MSG_HIGH_DS_1(SM_SUB, "WCDMA HSUPA category NV (4210) is Set to : %d", mm_nv_wcdma_hsupa_category);
   if(mm_nv_wcdma_hsupa_category >= MM_RRC_WCMDA_DC_HSUPA_CATEGORY)
   {
    ext_buf[cur_pos/8] = 0x10;  // IEI Length
   }
   else
   {
    ext_buf[cur_pos/8] = 0xE;  // IEI Length
   }

  }else 
#endif
  if (ue_revision == UE_REV_REL99)
  {
    ext_buf[cur_pos/8] = 0xB;  // IEI Length
  } 
  else
  {
    ext_buf[cur_pos/8] = 0x3;  // IEI Length
  }

  ADD_BYTE_LEN(cur_pos);

  // Pack OCTET 2
  b_packb(0x0, ext_buf, cur_pos, 2);
  cur_pos += 2;

  b_packb(qos->delay_class, ext_buf, cur_pos, 3);
  cur_pos += 3;

  b_packb(qos->reliability_class, ext_buf, cur_pos, 3);
  cur_pos += 3;

  // Pack OCTET 3
  b_packb(qos->peak_throughput, ext_buf, cur_pos, 4);
  cur_pos += 4;

  b_packb(qos->precedence_class, ext_buf, cur_pos, 4);
  cur_pos += 4;

  // Pack OCTET 4
  ext_buf[cur_pos/8] = qos->mean_throughput;
  ADD_BYTE_LEN(cur_pos);

  if (ue_revision == UE_REV_REL99)
  {
    // Pack OCTET 5
    b_packb(qos->traffic_class, ext_buf, cur_pos, 3);
    cur_pos += 3;

    b_packb(qos->deliver_order, ext_buf, cur_pos, 2);
    cur_pos += 2;

    b_packb(qos->deliver_err_sdu, ext_buf, cur_pos, 3);
    cur_pos += 3;

    //Pack OCTET 6
    ext_buf[cur_pos/8]= qos->max_sdu_size;
    ADD_BYTE_LEN(cur_pos);

    //Pack OCTET 7
    ext_buf[cur_pos/8] = qos->max_bit_rare_uplink;
    ADD_BYTE_LEN(cur_pos);

    //Pack OCTET 8
    ext_buf[cur_pos/8] = qos->max_bit_rare_downlink;
    ADD_BYTE_LEN(cur_pos);

    //Pack OCTET 9
    b_packb(qos->residual_ber, ext_buf, cur_pos, 4);
    cur_pos += 4;

    b_packb(qos->sdu_err_ratio, ext_buf, cur_pos, 4);
    cur_pos += 4;

    //Pack OCTET 10
    b_packb(qos->tranfer_delay, ext_buf, cur_pos, 6);
    cur_pos += 6;
  
    b_packb(qos->traffic_priority, ext_buf, cur_pos, 2);
    cur_pos += 2;

    //Pack OCTET 11
    ext_buf[cur_pos/8] = qos->guarantee_ber_uplink;
    ADD_BYTE_LEN(cur_pos);

    //Pack OCTET 12
    ext_buf[cur_pos/8] = qos->guarantee_ber_dwnlink;
    ADD_BYTE_LEN(cur_pos);
#ifdef FEATURE_REL5
    if (mm_nv_nas_rel_compliance >= NV_NAS_COMPLIANCE_REL5)
    {
      //Pack OCTET 13
      b_packb(0, ext_buf, cur_pos, 3); //Packing the spare octets
      cur_pos += 3;
      b_packb(qos->sig_ind, ext_buf, cur_pos, 1);
      cur_pos += 1;
      b_packb(qos->src_stat_desc, ext_buf, cur_pos, 4);
      cur_pos += 4;
      //Pack OCTET 14
      ext_buf[cur_pos/8] = qos->ext_max_bit_rate_downlink;
      ADD_BYTE_LEN(cur_pos);
  
      //Pack OCTET 15
      ext_buf[cur_pos/8] = qos->ext_gtd_bit_rate_downlink;
      ADD_BYTE_LEN(cur_pos);  

    
    if(mm_nv_wcdma_hsupa_category >= MM_RRC_WCMDA_DC_HSUPA_CATEGORY)
    {
      //Pack OCTET 16
      ext_buf[cur_pos/8] = qos->ext_max_bit_rate_uplink;
      ADD_BYTE_LEN(cur_pos);
      
      //Pack OCTET 17
      ext_buf[cur_pos/8] = qos->ext_gtd_bit_rate_uplink;
      ADD_BYTE_LEN(cur_pos);  
    }

    }
#endif
  }

  *msg_length = (uint16)(cur_pos/8);
}


/*===========================================================================
FUNCTION UNPACK_QOS 

DESCRIPTION
  This function (decodes) unpacks the QOS information in the L3 messages.
  
DEPENDENCIES

  None.
  
RETURN VALUE

  Length of the QoS Field in octets.

SIDE EFFECTS

===========================================================================*/
byte unpack_qos (byte* ext_buf, pdp_context_info *pdp, uint16 *msg_length)
{
  uint16 cur_pos;
  byte ie_length;
  qos_T  req_qos;
  ue_rev_level_indicator_type ue_revision;
  ue_revision = sm_get_revision_level_indicator();
  // Convert the msg_length into the bit length
  cur_pos = (uint16)(*msg_length * 8);  

  ie_length = ext_buf[cur_pos/8];  
  ADD_BYTE_LEN(cur_pos);    // Increment Length

  cur_pos += 2; // Increment Spare

  //Backup the requested qos
  req_qos = pdp->qos;

  pdp->qos.delay_class = b_unpackb(ext_buf, cur_pos, 3);
  cur_pos += 3;

  pdp->qos.reliability_class = b_unpackb(ext_buf, cur_pos, 3);
  cur_pos += 3;

  // unpack OCTET 3
  pdp->qos.peak_throughput = b_unpackb(ext_buf, cur_pos, 4);
  cur_pos += 4;

  cur_pos += 1; // Increment Spare

  pdp->qos.precedence_class = b_unpackb(ext_buf, cur_pos, 3);
  cur_pos += 3;

  // unpack OCTET 4
  pdp->qos.mean_throughput = ext_buf[cur_pos/8];
  ADD_BYTE_LEN(cur_pos);

  if (( ue_revision == UE_REV_REL99) && 
       (ie_length <= 3))
  {
   
  }
  else if (( ue_revision == UE_REV_PRE_REL99 ) && (ie_length > 3)) 
  {
   
  }

  MSG_HIGH_DS_2( SM_SUB,"Unpacking QoS: ue_revision :0x%X ie_length :0x%X (UE_REV_REL99:1, length>3)",ue_revision, ie_length );

  if ( ie_length > 3 )
  {  
    /* Release 99 QoS Construction */
    
    // unpack OCTET 5
    pdp->qos.traffic_class = b_unpackb(ext_buf, cur_pos, 3);
    cur_pos += 3;

    pdp->qos.deliver_order = b_unpackb(ext_buf, cur_pos, 2);
    cur_pos += 2;

    pdp->qos.deliver_err_sdu = b_unpackb(ext_buf, cur_pos, 3);
    cur_pos += 3;

    //unpack OCTET 6
    pdp->qos.max_sdu_size = ext_buf[cur_pos/8];
    ADD_BYTE_LEN(cur_pos);

    //unpack OCTET 7
    pdp->qos.max_bit_rare_uplink = ext_buf[cur_pos/8];
    ADD_BYTE_LEN(cur_pos);

    //unpack OCTET 8
    pdp->qos.max_bit_rare_downlink = ext_buf[cur_pos/8];
    ADD_BYTE_LEN(cur_pos);

    //unpack OCTET 9
    pdp->qos.residual_ber = b_unpackb(ext_buf, cur_pos, 4);
    cur_pos += 4;

    pdp->qos.sdu_err_ratio = b_unpackb(ext_buf, cur_pos, 4);
    cur_pos += 4;

    //unpack OCTET 10
    pdp->qos.tranfer_delay = b_unpackb(ext_buf, cur_pos, 6);
    cur_pos += 6;
  
    pdp->qos.traffic_priority = b_unpackb(ext_buf, cur_pos, 2);
    cur_pos += 2;

    //unpack OCTET 11
    pdp->qos.guarantee_ber_uplink = ext_buf[cur_pos/8];
    ADD_BYTE_LEN(cur_pos);

    //unpack OCTET 12
    pdp->qos.guarantee_ber_dwnlink = ext_buf[cur_pos/8];
    ADD_BYTE_LEN(cur_pos);

#ifdef FEATURE_REL5   
    if (ie_length > 0xB)
    {
      //unpack OCTET 13
      pdp->qos.sig_ind = (b_unpackb(ext_buf, cur_pos, 4)) ? TRUE: FALSE;
      cur_pos += 4;
      pdp->qos.src_stat_desc = b_unpackb(ext_buf, cur_pos, 4);
      cur_pos += 4;
    
      if (ie_length > 12)
      {
        //unpack OCTET 14
        pdp->qos.ext_max_bit_rate_downlink = ext_buf[cur_pos/8];

        if ( pdp->qos.ext_max_bit_rate_downlink > 0xFA)
        {
           pdp->qos.ext_max_bit_rate_downlink = 0xFA;
        }
        ADD_BYTE_LEN(cur_pos);
      }

      if (ie_length > 13)
      {
        //unpack OCTET 14
        pdp->qos.ext_gtd_bit_rate_downlink = ext_buf[cur_pos/8];

        if ( pdp->qos.ext_gtd_bit_rate_downlink > 0xFA)
        {
           pdp->qos.ext_gtd_bit_rate_downlink = 0xFA;
        }
        ADD_BYTE_LEN(cur_pos);
      }

     if(mm_nv_wcdma_hsupa_category >= MM_RRC_WCMDA_DC_HSUPA_CATEGORY)
     {
      if (ie_length > 14)
      {
        //unpack OCTET 15
        pdp->qos.ext_max_bit_rate_uplink = ext_buf[cur_pos/8];

        if ( pdp->qos.ext_max_bit_rate_uplink > 0xFA)
        {
           pdp->qos.ext_max_bit_rate_uplink = 0xFA;
        }
        ADD_BYTE_LEN(cur_pos);
      }
      else
      {
        pdp->qos.ext_max_bit_rate_uplink = 0;
        pdp->qos.ext_gtd_bit_rate_uplink = 0;
      }

      if (ie_length > 15)
      {
        //unpack OCTET 16
        pdp->qos.ext_gtd_bit_rate_uplink = ext_buf[cur_pos/8];
        
        if ( pdp->qos.ext_gtd_bit_rate_uplink > 0xFA)
        {
           pdp->qos.ext_gtd_bit_rate_uplink = 0xFA;
        }
        ADD_BYTE_LEN(cur_pos);
      }
      else
      {
        pdp->qos.ext_gtd_bit_rate_uplink = 0;
      }
     } 
    }
    else
    {
      pdp->qos.sig_ind                   = 0;
      pdp->qos.src_stat_desc             = 0;
      pdp->qos.ext_max_bit_rate_downlink = 0;
      pdp->qos.ext_gtd_bit_rate_downlink = 0;
    
      pdp->qos.ext_max_bit_rate_uplink = 0;
      pdp->qos.ext_gtd_bit_rate_uplink = 0;
    }
    MSG_HIGH_DS_5(SM_SUB,"N/W MBR extn donwlink %d N/W GBR extn donwlink %d N/W MBR extn uplink %d N/W GBR extn uplink %d HSUPA category NV (4210) : %d",
                          pdp->qos.ext_max_bit_rate_downlink, pdp->qos.ext_gtd_bit_rate_downlink, pdp->qos.ext_max_bit_rate_uplink, pdp->qos.ext_gtd_bit_rate_uplink, mm_nv_wcdma_hsupa_category);
#endif
  }
  else
  {
    pdp->qos.traffic_class         = 0;
    pdp->qos.deliver_order         = 0;
    pdp->qos.deliver_err_sdu       = 0;
    pdp->qos.max_sdu_size          = 0;
    pdp->qos.max_bit_rare_uplink   = 0;
    pdp->qos.max_bit_rare_downlink = 0;
    pdp->qos.residual_ber          = 0;
    pdp->qos.sdu_err_ratio         = 0;
    pdp->qos.tranfer_delay         = 0;
    pdp->qos.traffic_priority      = 0;
    pdp->qos.guarantee_ber_uplink  = 0;
    pdp->qos.guarantee_ber_dwnlink = 0;
  }
  
  if (sm_validate_qos(pdp->qos, req_qos))
  {
    pdp->is_net_alloc_qos_diff_from_req = FALSE;
  }
  else
  {
    pdp->is_net_alloc_qos_diff_from_req = TRUE;
  }
  // *msg_length = (byte)(cur_pos/8);
  *msg_length +=   (ie_length + 1);

  return (ie_length);
}

/*===========================================================================
FUNCTION UNPACK_PDP_ADDR

DESCRIPTION
  This function (decodes) unpacks the PDP Address IE in SM-SM peer messages.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T unpack_pdp_addr
(
  byte* ext_buf,
  uint16 *index,
  pdp_context_info* pdp
)

{
  uint16 msg_index;
  byte pdp_addr_ie_len;
  msg_index = *index;

  // Process the PDP Address optional IE field.
  pdp->pdp_addr.valid = TRUE;
  pdp_addr_ie_len = ext_buf[msg_index];
        
  if (pdp_addr_ie_len < 2)
  {
    MSG_ERROR_DS_1(SM_SUB,"Incorrect PDP Address length: %d - Reject with Invalid Mandatory Info", pdp_addr_ie_len );
    return L3_INVALID_MANDATORY_INFO;
  }
  
  // decode the Length of the PDP address contents
  pdp->pdp_addr.pdp_addr_len = (byte)(ext_buf[msg_index++] - 2); 

  if (pdp->pdp_addr.pdp_addr_len > MAX_PDP_ADR_LEN)
  { 
    MSG_ERROR_DS_1(SM_SUB,"Incorrect PDP Address length: %d - truncate and continue", pdp->pdp_addr.pdp_addr_len);
    pdp->pdp_addr.pdp_addr_len = MAX_PDP_ADR_LEN;
  }
    
  // decode the offered pdp address
  pdp->pdp_addr.pdp_type_org = ext_buf[msg_index++];

  if ((pdp->pdp_addr.pdp_type_org != PDP_ETSI_TYPE) &&
       (pdp->pdp_addr.pdp_type_org != PDP_IETF_TYPE))
  { 
    //Invalid pdp type organisation.
    return L3_INVALID_MANDATORY_INFO;
  }
    
  // decode the PDP type number
  pdp->pdp_addr.pdp_type_num = ext_buf[msg_index++];
          
  if ((pdp->pdp_addr.pdp_type_num != PPP_TYPE)  &&
      (pdp->pdp_addr.pdp_type_num != IPV4_TYPE) &&
      (pdp->pdp_addr.pdp_type_num != IPV6_TYPE) 
#ifdef FEATURE_DUAL_IP_BEARER
      &&(pdp->pdp_addr.pdp_type_num != IPV4V6_TYPE)
#endif
      )
  { 
    if (pdp->pdp_addr.pdp_type_num == 0)
    {
      //Invalid pdp type organisation.
      return L3_INVALID_MANDATORY_INFO;
    }
    else
    {
      pdp->pdp_addr.pdp_type_num = IPV4_TYPE; 
    }
  }

  // Copy the PDP address based on its length
  memscpy( pdp->pdp_addr.address, sizeof(pdp->pdp_addr.address),
           &ext_buf[msg_index], pdp->pdp_addr.pdp_addr_len );

  msg_index = (uint16)(msg_index + (pdp_addr_ie_len - 2));
  *index = msg_index;

  return L3_PARSE_SUCCESS;
}

/*===========================================================================
FUNCTION UNPACK_PROTO_CONFIG

DESCRIPTION
  This function (decodes) unpacks the Protocol Configuration IE.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T unpack_proto_config
(
  byte* ext_buf,
  uint16 *index,
  pdp_context_info* pdp
)

{
  uint16 msg_index, cur_index;
  uint16 iei_length;
  word i;
  cur_index = msg_index = *index;

  if (ext_buf[msg_index] == 0)
  {
    msg_index++;
    *index = (uint16) msg_index;
    return L3_PARSE_SUCCESS;
  }

  iei_length = (uint16)(ext_buf[msg_index++] - 1);

  // Process the PDP Address optional IE field.
  pdp->config_options.valid = TRUE;
        
  // Pack Configuration protocol field
  if (msg_index <= (cur_index + iei_length + 1))
  {
    pdp->config_options.config_protocol = ext_buf[msg_index++];
  }
  else
  {
    msg_index = (uint16)(cur_index + iei_length + 1) ;
  }
          
  // Mask the ext bit (8 bit)
  pdp->config_options.config_protocol &= 0x7F;
               
  pdp->config_options.num_protocol_ids = 0;
               
  // Pack all the individual protocol IDs, maximum 10 assumed
  for(i=0; msg_index + 2 <= (cur_index + iei_length + 1) && 
    pdp->config_options.num_protocol_ids < NUM_PROTOCOL_ID; i++)
  {
    pdp->config_options.num_protocol_ids += 1;

    pdp->config_options.procol_info[i].protocol_id[0]= 
                                       ext_buf[msg_index++];
    pdp->config_options.procol_info[i].protocol_id[1] = 
                                       ext_buf[msg_index++];
  
    pdp->config_options.procol_info[i].protocol_len =  
                                       ext_buf[msg_index++];

    if ((msg_index-1) + pdp->config_options.procol_info[i].protocol_len <= (cur_index + iei_length + 1) )
    {
      memscpy( pdp->config_options.procol_info[i].protocol_contents, sizeof(pdp->config_options.procol_info[i].protocol_contents),
               &ext_buf[msg_index], pdp->config_options.procol_info[i].protocol_len);        
      msg_index = (uint16)(msg_index + 
                          pdp->config_options.procol_info[i].protocol_len);

      pdp->config_options.procol_info[i].protocol_len =  
          MIN(pdp->config_options.procol_info[i].protocol_len, MAX_PROTOCOL_ID_CONTENT);        
    }
    else
    {
       // skip processing remaining protocol ids and proceed with next IE in the message.
       pdp->config_options.procol_info[i].protocol_len = 0;
       msg_index = (uint16)(cur_index + iei_length+1) ;
       // advance to next byte in the message.
       msg_index++;  
    }
  }

  if (msg_index >= (gmm_max_L3_msg_length(MM_PS_DOMAIN)) || 
    msg_index < (cur_index + iei_length + 1)) 
  {
     MSG_HIGH_DS_1(SM_SUB,"msg_index: %d", msg_index);
     // take msg_index to the end of the IE
     msg_index = (uint16)(cur_index + iei_length + 1) ;
     // advance to next byte in the message.
     msg_index++;  
  }

  *index = (uint16) msg_index;
  return L3_PARSE_SUCCESS;
}


/*===========================================================================
FUNCTION UNPACK_GPRS_TIMER_3

DESCRIPTION
  This function (decodes) unpacks the GPRS TIMER 3 IE.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T unpack_gprs_timer_3
(
  byte* ext_buf,
  uint16 *index,
  pdp_context_info* pdp
)

{
  uint16 msg_index;

  byte timer_value ;

  dword timer_value_unit;

  msg_index = *index;  

  if (ext_buf[msg_index++] < 1)
  {
    return L3_INVALID_MANDATORY_INFO;
  }  

  timer_value = ext_buf[msg_index++];  

  pdp->sm_gprs_type3_timer.active = TRUE ;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* ------------------------------------------------
  ** Convert timer value per TS 24.008 Table 10.5.163a
  ** ------------------------------------------------ */
  if ((timer_value & 0xE0) == 0x00)
  {
    /* ----------------------------------------------
    ** Value is incremented in multiples of 10 minutes 
    ** ---------------------------------------------- */
    timer_value_unit = 600000;
  }
  else if ((timer_value & 0xE0) == 0x20)
  {
    /* ----------------------------------------------
    ** Value is incremented in multiples of 1 hour
    ** ---------------------------------------------- */
    timer_value_unit = 3600000 ;
  }

  else if ((timer_value & 0xE0) == 0x40)
  {
    /* ----------------------------------------------
    ** Value is incremented in multiples of 10  hour
    ** ---------------------------------------------- */
    timer_value_unit = 36000000 ;
  }

  else if ((timer_value & 0xE0) == 0x60)
  {
    /* ----------------------------------------------
    ** Value is incremented in multiples of 2 Secs
    ** ---------------------------------------------- */
    timer_value_unit = 2000;
  }

  else if ((timer_value & 0xE0) == 0x80)
  {
    /* ----------------------------------------------
    ** Value is incremented in multiples of 30 Secs
    ** ---------------------------------------------- */
    timer_value_unit = 30000;
  }

  else if ((timer_value & 0xE0) == 0xa0)
  {
    /* ----------------------------------------------
    ** Value is incremented in multiples of 1 minute
    ** ---------------------------------------------- */
    timer_value_unit = 60000;
  }

  else if ((timer_value & 0xE0) == 0xd0)
  {
    /* ----------------------------------------------
    ** Value is incremented in multiples of 1 hour
    ** ---------------------------------------------- */
    timer_value_unit = 3600000 ;
  }
  else if ((timer_value & 0xE0) == 0xE0)
  {
    /* ---------------------------------------------
    ** Value indicates that the timer is deactivated
    ** --------------------------------------------- */
    timer_value_unit = 0;

    pdp->sm_gprs_type3_timer.active = FALSE ;
  }
  else
  {
    /* ----------------------------------------------------------
    ** Other values shall be interpreted as multiples of 1 hour
    ** ---------------------------------------------------------- */
    timer_value_unit = 3600000;
  }

  timer_value = timer_value & 0x1F;

  pdp->sm_gprs_type3_timer.timer_count = timer_value_unit * timer_value;

  *index = (uint16) msg_index;
  return L3_PARSE_SUCCESS;
}

/*===========================================================================
FUNCTION PROCESS_UNKNOWN_IE

DESCRIPTION
  This function (decodes) unpacks the Protocol Configuration IE.
  
DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T process_unknown_ie
(
  byte* ext_buf,
  uint16 *index
)
{
  uint16 msg_index;
  //Get the IEI of the unknown IE
  msg_index = *index - 1;

  if ((ext_buf[msg_index] & 0xF0) == 0x00)
  {
    /* --------------------------------------
    ** Comprehension required (TS 24.008 8.5)
    ** -------------------------------------- */
    return L3_INVALID_MANDATORY_INFO;
  }
  else
  {
    /* ------------------------------------------------------------
    ** Ignore unknown IE - increment msg_index per TS 24.007 11.2.4
    ** ------------------------------------------------------------ */
    if ((ext_buf[msg_index] & 0x80) == 0x80)
    {
      /* --------------------------------------------------------------
      ** A 1 valued bit 8 indicates that the whole IE is one octet long
      ** -------------------------------------------------------------- */
      msg_index = (uint16)(msg_index + 1);
    }
    else
    {
      /* ---------------------------------------------------------------------
      ** A 0 valued bit 8 indicates that the following octet is a length octet
      ** --------------------------------------------------------------------- */
      if (msg_index + 2 + ext_buf[msg_index+1] >= gmm_max_L3_msg_length(MM_PS_DOMAIN))
      {
        msg_index = gmm_max_L3_msg_length(MM_PS_DOMAIN);
        MSG_ERROR_DS_1(SM_SUB,"msg_index: %d - ignore remaining message contents", msg_index);
      }
      else
      {
        msg_index = (uint16)(msg_index + 2 + ext_buf[msg_index+1]);
      }
    }
  }

  *index = msg_index;
  return L3_PARSE_SUCCESS;
}

/*===========================================================================
FUNCTION PACK_PD_TI_MSGTYPE 

DESCRIPTION
  This function (decodes) unpacks the QOS information in the L3 messages.
  
DEPENDENCIES

  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pack_pd_ti_msgtype 
(
  byte* ext_buf,
  word t_id, 
  sm_layer_msg_T sm_msg
)
{
  word tio;
  word cur_pos;
  boolean ti_flag;

  cur_pos = 0;
  MSG_HIGH_DS_2( SM_SUB,"Packing PD : %d :: TI : %d",sm_pd,t_id); 
  /* For Network Initiated PDP context transaction, set the TI_flag to 1 */
  if (sm_msg == REQUEST_PDP_ACTIVATION_REJ ||
       sm_msg == NETWORK_MODIFY_PDP_REQUEST
       #ifdef FEATURE_MBMS
       #error code not present
#endif
       )
  {
    ti_flag = TRUE;
  }
  else
  {
    //set the ti_flag
    ti_flag = (t_id >= MIN_MT_TI) ? TRUE : FALSE; 
  }

  if (t_id >= MIN_MT_TI)
    {
        t_id = t_id - MIN_MT_TI; // reset the most significant bit
    }
  
  // Pack the TI flag
  b_packw(ti_flag, ext_buf, cur_pos, 1);
  cur_pos += 1;

  tio = t_id ;
  if (t_id > 6) // incase of extended TI
    {
        tio = 7;
    }
  
  // Pack the TI value
  b_packw(tio, ext_buf, cur_pos, TI_VALUE);
  cur_pos += TI_VALUE;

  // Pack PD
  b_packw(sm_pd, ext_buf, cur_pos, PD_LEN);
  cur_pos += PD_LEN; 
  
  //If the TI is '111' construct the EXT ti flag byte
  if (tio == 0x07)
  {
    b_packw(0, ext_buf, cur_pos, BYTE_LEN);
    cur_pos += BYTE_LEN;

    b_packw(t_id, ext_buf, cur_pos, TIE_VALUE);
    cur_pos += TIE_VALUE;
  }

     
  //Pack Message type
  ext_buf[cur_pos/8] =(byte)sm_msg;
}

/*===========================================================================
FUNCTION PACK_SM_STATUS_INFO 

DESCRIPTION
  This function (encodes) packs a SM_STATUS message with the supplied 
  transaction ID and cause.

DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T pack_sm_status_info 
(
  word* length,
  byte* ext_buf, 
  word trans_id,
  word cause
)
{
  word mesg_length;
  mesg_length = 0;

  if (trans_id >= INVALID_EXTENDED_TID)
  {
  //Pack the PD, TI and the Message type
    pack_pd_ti_msgtype (ext_buf, INVALID_EXTENDED_TID, SM_STATUS);
    ext_buf[mesg_length+1] = (byte)trans_id;
    mesg_length += 3; //In case of ext flag move msg lenght by 3
  }
  else
  {
    //Pack the PD, TI and the Message type
    pack_pd_ti_msgtype (ext_buf, trans_id, SM_STATUS);
    mesg_length += 2;  // Move the message length by two octets
  }
  
  //Pack the SM cause for the status message
  ext_buf[mesg_length++] = (byte)cause;
 
  *length = mesg_length;
  return L3_PARSE_SUCCESS;
}

/*===========================================================================
FUNCTION PACK_TFT

DESCRIPTION
  This function (encodes) packs the TFT

DEPENDENCIES

  None.
  
RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T pack_tft
(
  word        *msg_len,
  tft_type_T  *tft,
  byte        *ext_buf
)
{
    word cur_pos, i;
    word tft_len = 0;
    byte len_index = (byte)(*msg_len + 1);  
    uint16 gmm_max_length = gmm_max_L3_msg_length(MM_PS_DOMAIN);

    ext_buf[(*msg_len)++] = IEI_TRAFFIC_FLOW_TEMPLATE;
    ext_buf[(*msg_len)++] = 0;  // exact value will be set below

    cur_pos = (word)(*msg_len * 8);  // set the current bit position
      
    b_packb(tft->operation_code, ext_buf, cur_pos, 3);
    cur_pos += 3;
#ifdef FEATURE_REL5
    b_packb(tft->e_bit, ext_buf, cur_pos, 1);
    cur_pos += 1;

    b_packb(tft->num_filters, ext_buf, cur_pos, 4);
    cur_pos += 4; 
#else
    b_packb(tft->num_filters, ext_buf, cur_pos, 5);
    cur_pos += 5; 
#endif

    (*msg_len)++;

    if (tft->operation_code == 0x5)  /* Delete TFT from existing ones */
    {
      // Pack the individual Packet filter information
      for (i=0; i<tft->num_filters; i++)
      {
        tft_len++;
        ext_buf[(*msg_len)++] = tft->filter_list[i].identifier;
      }
    }
    else
    {
       // Pack the individual Packet filter information
      for (i=0; i<tft->num_filters; i++)
      {
        /* Return L3_PARSE_FAIL if TFT packing results in packed message exceeding MAX_OCTETS_L3_MESSAGE */
        if(gmm_max_length - (*msg_len -1) < (3 + tft->filter_list[i].content_length ))
        {
            MSG_ERROR_DS_2( SM_SUB,"TFT onwards TFT ID %d is skipped, message buff exceeding. TFT len: %d",tft->filter_list[i].identifier,
                      tft->filter_list[i].content_length);
            return L3_PARSE_FAIL;
        }
        //DS sends the direction and identifier in same field
        ext_buf[(*msg_len)++] = tft->filter_list[i].identifier;

        
        ext_buf[(*msg_len)++] = tft->filter_list[i].eval_precedence;
        ext_buf[(*msg_len)++] = tft->filter_list[i].content_length;

        tft_len += 3;
        memscpy(&ext_buf[*msg_len], (gmm_max_length - *msg_len),
               tft->filter_list[i].filter_contents, 
               (int)tft->filter_list[i].content_length );

        tft_len = (uint16)(tft_len + MIN(tft->filter_list[i].content_length, (gmm_max_length - *msg_len)));
        *msg_len = (uint16)(*msg_len + MIN(tft->filter_list[i].content_length, (gmm_max_length - *msg_len)));
        MSG_HIGH_DS_3(SM_SUB,"=MM= tft_length %d, msg length is %d, iei index %d",tft_len,*msg_len,len_index);
      }
    }
    ext_buf[len_index] = (byte)(*msg_len - (len_index + 1));

    return L3_PARSE_SUCCESS;
}

/*===========================================================================
FUNCTION PACK_LINKED_TI 

DESCRIPTION
  This function (encodes) packs  LINKED TI with the given linked TI value

DEPENDENCIES

  None.
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void pack_linked_ti
(
  word  *msg_length,
  byte  link_ti_value,
  byte  *ext_buf
)
{
  word cur_pos;
  boolean ti_flag;
  byte t_id;

  //pack the lenght of TI
  ext_buf[(*msg_length)++] = LINKED_TI_LENGTH;    
  //Pack the TI flag to false as UE initiated TI

  //set the ti_flag
  ti_flag = (link_ti_value >= MIN_MT_TI) ? TRUE : FALSE;

  cur_pos = (word)(*msg_length * 8);
  //Pack the TI value
  b_packw(ti_flag, ext_buf, cur_pos, 1);
  cur_pos += 1;

  t_id = link_ti_value;
  if (link_ti_value >= MIN_MT_TI)
  {
      t_id = t_id - MIN_MT_TI; // reset the most significant bit
  }

  if (t_id > 6) // incase of extended TI
  {
      t_id = 7;
  }

  // Pack the TI value
  b_packw(t_id, ext_buf, cur_pos, TI_VALUE);
  cur_pos += TI_VALUE;

  //Pack the spare bits
  b_packw(0, ext_buf, cur_pos, SPARE_LEN);
  cur_pos += SPARE_LEN;

  (*msg_length)++;

  //If the TI is '111' construct the EXT ti flag byte
  if (t_id == 0x07)
  {
    b_packw(0, ext_buf, cur_pos, BYTE_LEN);
    cur_pos += BYTE_LEN;

    b_packw(link_ti_value, ext_buf, cur_pos, TIE_VALUE);
    cur_pos += TIE_VALUE;

    (*msg_length)++;
  }


}

/*===========================================================================
FUNCTION get_message_data_index 

DESCRIPTION
  This function returns the index in the message data after Message Type.
  
DEPENDENCIES
  None.
  
RETURN VALUE
  3 incase Extended TI is used, else 2.

SIDE EFFECTS

===========================================================================*/
static byte get_message_data_index
(
  const byte* ext_buf         /* Buffer contains the received message */
)
{
  byte trans_id, index = 2;

  trans_id = ext_buf[0] & 0x70;
  trans_id = trans_id >> 4;
  if (trans_id == 0x07)
  {
    index = 3;
  }
  
  return index;
}

#ifdef FEATURE_MBMS
#error code not present
#endif


/*===========================================================================
FUNCTION UNPACK_LINKED_TI

DESCRIPTION
  This function decodes Linked TI. TI will be converted to semantics of UE.

DEPENDENCIES

  None.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

xlate_status_T unpack_linked_ti(byte* ext_buf, pdp_context_info *pdp, uint16* cur_pos)
{
  byte trans_id;
  uint16 msg_index;  
  byte ti_flag;
  xlate_status_T status = L3_PARSE_SUCCESS;

  msg_index = *cur_pos;

  msg_index++; //incrementing length feild
  trans_id = ext_buf[msg_index] & 0x70;
  ti_flag = ext_buf[msg_index++] & 0x80;

  trans_id = trans_id >> 4;

#ifdef FEATURE_EXTENDED_TI
  if (trans_id == 0x07)
  {
    trans_id = ext_buf[msg_index] & 0x7F;
    ti_flag = ext_buf[msg_index++] & 0x80;
    if (trans_id < 0x07)
    {
      // Enhanced TI value less than 7
      MSG_ERROR_DS_1(SM_SUB, "Enhanced TI value less than 7 : %d", trans_id);
      status = L3_PARSE_INVALID_PD_TI;
    }
    else if (trans_id > 0x7F)
    {
     // Enhanced TI value less than 7
     MSG_ERROR_DS_1(SM_SUB, "Enhanced TI value greater than 127 : %d",trans_id);
     status = L3_PARSE_INVALID_PD_TI;
    }
  }
#else
  if (trans_id == 0x07)
  {
    // Enhanced TI value is being used.
    MSG_ERROR_DS_1(SM_SUB, "Enhanced TI not supported : %d",trans_id);
    status = L3_PARSE_INVALID_PD_TI;
  }
#endif /*FEATURE_EXTENDED_TI*/

  // converting TI to UE semantics
  trans_id = (!ti_flag)?  (trans_id + MIN_MT_TI) : trans_id;

  pdp->linked_ti = trans_id;
  *cur_pos = msg_index;

  return status;
}

/*===========================================================================
FUNCTION UNPACK_REQUEST_SEC_PDP_ACTIVATION

DESCRIPTION
  Helper function for unpacking NW request for secondary PDP activation

DEPENDENCIES

  None.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

xlate_status_T unpack_request_sec_pdp_activation
(
  uint16 msg_length,
  byte* ext_buf,
  pdp_context_info *pdp
)
{
  uint16    iei_length;
  uint16    msg_index;
  byte    optional_ie_index;
//  byte    len_mandatory_fields;
  xlate_status_T status;

  msg_index = 0;
  status = L3_PARSE_SUCCESS;

  MSG_MED_DS_0(SM_SUB, "Unpack REQUEST_SEC_PDP_ACTIVATION message");
 
  // Check if the received message has the minimum length required.
  if (msg_length < SM_REQUEST_SEC_PDP_CONTEXT_ACTIVATION_LEN)
  {
    MSG_ERROR_DS_0(SM_SUB,"Invalid mandatory field error");
    return L3_INVALID_MANDATORY_INFO; 
  }
  /* ----------------------------------------------------------------------
  ** Unpack the Mandatory IEs from the message.
  ** --------------------------------------------------------------------*/

  // skip the PD_TI and the Message Type octets.
  msg_index = get_message_data_index (ext_buf);

  // unpack the negotiated QOS
  (void)unpack_qos( ext_buf, pdp, &msg_index);
  pdp->qos.valid = TRUE;

  //unpack linked ti
  status = unpack_linked_ti( ext_buf, pdp, &msg_index);
  if (status != L3_PARSE_SUCCESS)
  {
    return status;
  }

  /* ----------------------------------------------------------------------
  ** Parse the non-imperative message part. Set the valid fields to FALSE
  ** ------------------------------------------------------------------- */
  pdp->config_options.valid = FALSE;
  pdp->config_options.num_protocol_ids = 0;
  pdp->tft.valid = FALSE;
  pdp->tft.num_filters = 0;

  optional_ie_index = 0;



  while (msg_index < msg_length)
  {
    /* ------------------------------------------------------------------
    ** Optional IEs listed in order specified in TS 24.008 Table 9.5.2
    ** --------------------------------------------------------------- */
    switch (ext_buf[msg_index++])
    {
      case IEI_TRAFFIC_FLOW_TEMPLATE:
        // Get the IE length from this IE
        iei_length = ext_buf[msg_index];

        if ((optional_ie_index == 0) &&
              ((msg_index + iei_length) <= msg_length))
        {
          status = unpack_tft (ext_buf, &msg_index, pdp);

          if (status != L3_PARSE_SUCCESS)
          {
            pdp->pdp_addr.valid = FALSE;
            return status;
          }
          optional_ie_index = 1;
        }
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }

      break;

      case IEI_PROTOCOL_CFG_OPTIONS:

        // Get the IE length from this IE
        iei_length = ext_buf[msg_index];

        if ((optional_ie_index <= 1) &&
              ((msg_index + iei_length) <= msg_length))
        {
          status = unpack_proto_config(ext_buf, &msg_index, pdp);

          if (status != L3_PARSE_SUCCESS)
          {
            return status;
          }

          optional_ie_index = 2;
        }
        else
        {
          msg_index = (uint16)(msg_index + iei_length);
        }

      break;

      default:
        /* ----------
        ** Unknown IE
        ** ---------- */
        status = process_unknown_ie(ext_buf, &msg_index);
        if (status != L3_PARSE_SUCCESS)
        {
          return status;
        }

        break;
    } /* End of Switch statement */
  }

  return status;

}



/*===========================================================================
FUNCTION PACK_PDP_ACTIVATE_MESG

DESCRIPTION
  This function (encodes) packs PDP_ACTIVATE_MESG sent to the Network.

DEPENDENCIES

  None.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T pack_req_sec_pdp_activation_rej
(
  word* length, byte* ext_buf,
  pdp_context_info* pdp
)
{
  word mesg_length;
  uint16 config_option_len;
  byte i;
  uint16 gmm_max_length = gmm_max_L3_msg_length(MM_PS_DOMAIN);

  config_option_len = 0;
  mesg_length = 0;

  //Pack the PD, TI and the Message type
  pack_pd_ti_msgtype (ext_buf, pdp->trans_id, REQUEST_SEC_PDP_ACTIVATION_REJ);
  mesg_length += get_message_data_index (ext_buf);  // Move the message length

  //Pack the SM cause for rejection
  ext_buf[mesg_length++] = (byte)pdp->current_cause;

  // Check if Protocol Configuration options field is included
  if (pdp->config_options.valid)
  {
    // pack the IE value of Optional Protocol Configurations
    ext_buf[mesg_length++] = IEI_PROTOCOL_CFG_OPTIONS;
  
    // Pack the IEI length : This is temporarily set to '0'
    ext_buf[mesg_length++] = 0;
  
    // Pack the Configuration protocol field
    ext_buf[mesg_length] = 0x80;  //set ext bit 1 and spare bits
    ext_buf[mesg_length++] = ( (pdp->config_options.config_protocol & 0x07) | ext_buf[mesg_length]) ;
  
    // Pack all the individual protocol IDs
    for(i=0; i<pdp->config_options.num_protocol_ids; i++)
    {
      /* Return parse failure if PCO exceeds MAX_OCTETS_L3_MESSAGE */
      if(gmm_max_length - (mesg_length -1) < (3 + pdp->config_options.procol_info[i].protocol_len ))
      {
        MSG_ERROR_DS_3( SM_SUB,"Protocol ID onwards %d, %d is skipped, message buff exceeding. Proto ID len: %d",
                      pdp->config_options.procol_info[i].protocol_id[0],
                      pdp->config_options.procol_info[i].protocol_id[1],
                      pdp->config_options.procol_info[i].protocol_len);
        pdp->current_cause = (cc_cause_T)SM_MESSAGE_EXCEED_MAX_L2_LIMIT;
        return L3_PARSE_FAIL;
      }
      
      ext_buf[mesg_length++] =
        pdp->config_options.procol_info[i].protocol_id[0];
  
      ext_buf[mesg_length++] =
        pdp->config_options.procol_info[i].protocol_id[1];
  
      config_option_len += 2;
  
      ext_buf[mesg_length++] =
        pdp->config_options.procol_info[i].protocol_len;
  
      config_option_len ++;
  
      memscpy(&ext_buf[mesg_length], (gmm_max_length - mesg_length),
              pdp->config_options.procol_info[i].protocol_contents,
              pdp->config_options.procol_info[i].protocol_len);
  
      mesg_length = (uint16)(mesg_length +
                           MIN(pdp->config_options.procol_info[i].protocol_len, (gmm_max_length - mesg_length)));
      config_option_len = (uint16)(config_option_len +
                         MIN(pdp->config_options.procol_info[i].protocol_len, (gmm_max_length - mesg_length)));
    }
    
    MSG_HIGH_DS_2(SM_SUB,"=MM= config option length %d, message length %d", config_option_len,mesg_length);
    // Set the Protocol configuration IEI length now.
    ext_buf[mesg_length - config_option_len -2] = (byte)(config_option_len + 1);
  }
  

  *length = mesg_length;
  return L3_PARSE_SUCCESS;

}


/*===========================================================================
FUNCTION PACK_PDP_ACTIVATE_MESG

DESCRIPTION
  This function (encodes) packs PDP_ACTIVATE_MESG sent to the Network.

DEPENDENCIES

  None.

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
xlate_status_T pack_modify_pdp_reject
(
  word* length, byte* ext_buf,
  pdp_context_info* pdp
)
{
  word mesg_length;
  mesg_length = 0;

  //Pack the PD, TI and the Message type
  pack_pd_ti_msgtype (ext_buf, pdp->trans_id, MODIFY_PDP_REJECT);
  mesg_length += get_message_data_index (ext_buf);  // Move the message length

  //Pack the SM cause for rejection
  ext_buf[mesg_length++] = (byte)pdp->current_cause;
  
  *length = mesg_length;
  return L3_PARSE_SUCCESS;

}

/*===========================================================================
FUNCTION UNPACK_REQ_QOS

DESCRIPTION
  This function (decodes) unpacks the QOS information in the L3 messages.

DEPENDENCIES

  None.

RETURN VALUE

  Length of the QoS Field in octets.

SIDE EFFECTS

===========================================================================*/
byte unpack_req_qos (byte* ext_buf, pdp_context_info *pdp, uint16 *msg_length)
{
  uint16 cur_pos;
  byte ie_length;
  ue_rev_level_indicator_type ue_revision;
  ue_revision = sm_get_revision_level_indicator();
  // Convert the msg_length into the bit length
  cur_pos = (uint16)(*msg_length * 8);

  ie_length = ext_buf[cur_pos/8];
  ADD_BYTE_LEN(cur_pos);    // Increment Length

  cur_pos += 2; // Increment Spare

  pdp->req_qos.delay_class = b_unpackb(ext_buf, cur_pos, 3);
  cur_pos += 3;

  pdp->req_qos.reliability_class = b_unpackb(ext_buf, cur_pos, 3);
  cur_pos += 3;

  // unpack OCTET 3
  pdp->req_qos.peak_throughput = b_unpackb(ext_buf, cur_pos, 4);
  cur_pos += 4;

  cur_pos += 1; // Increment Spare

  pdp->req_qos.precedence_class = b_unpackb(ext_buf, cur_pos, 3);
  cur_pos += 3;

  // unpack OCTET 4
  pdp->req_qos.mean_throughput = ext_buf[cur_pos/8];
  ADD_BYTE_LEN(cur_pos);

  if (( ue_revision == UE_REV_REL99) &&
       (ie_length <= 3))
  {
    MSG_HIGH_DS_0(SM_SUB, "Unpacking QoS: Exp R99, Got R97");
  }
  else if (( ue_revision == UE_REV_PRE_REL99 ) && (ie_length > 3))
  {
    MSG_HIGH_DS_0(SM_SUB, "Unpacking QoS: Exp R97, Got R99");
  }

  if ( ie_length > 3 )
  {
    /* Release 99 QoS Construction */

    // unpack OCTET 5
    pdp->req_qos.traffic_class = b_unpackb(ext_buf, cur_pos, 3);
    cur_pos += 3;

    pdp->req_qos.deliver_order = b_unpackb(ext_buf, cur_pos, 2);
    cur_pos += 2;

    pdp->req_qos.deliver_err_sdu = b_unpackb(ext_buf, cur_pos, 3);
    cur_pos += 3;

    //unpack OCTET 6
    pdp->req_qos.max_sdu_size = ext_buf[cur_pos/8];
    ADD_BYTE_LEN(cur_pos);

    //unpack OCTET 7
    pdp->req_qos.max_bit_rare_uplink = ext_buf[cur_pos/8];
    ADD_BYTE_LEN(cur_pos);

    //unpack OCTET 8
    pdp->req_qos.max_bit_rare_downlink = ext_buf[cur_pos/8];
    ADD_BYTE_LEN(cur_pos);

    //unpack OCTET 9
    pdp->req_qos.residual_ber = b_unpackb(ext_buf, cur_pos, 4);
    cur_pos += 4;

    pdp->req_qos.sdu_err_ratio = b_unpackb(ext_buf, cur_pos, 4);
    cur_pos += 4;

    //unpack OCTET 10
    pdp->req_qos.tranfer_delay = b_unpackb(ext_buf, cur_pos, 6);
    cur_pos += 6;

    pdp->req_qos.traffic_priority = b_unpackb(ext_buf, cur_pos, 2);
    cur_pos += 2;

    //unpack OCTET 11
    pdp->req_qos.guarantee_ber_uplink = ext_buf[cur_pos/8];
    ADD_BYTE_LEN(cur_pos);

    //unpack OCTET 12
    pdp->req_qos.guarantee_ber_dwnlink = ext_buf[cur_pos/8];
    ADD_BYTE_LEN(cur_pos);

    if (ie_length > 0xB)
    {
      //unpack OCTET 13
      pdp->req_qos.sig_ind = (b_unpackb(ext_buf, cur_pos, 4)) ? TRUE: FALSE;
      cur_pos += 4;
      pdp->req_qos.src_stat_desc = b_unpackb(ext_buf, cur_pos, 4);
      cur_pos += 4;

      if (ie_length > 12)
      {
        //unpack OCTET 14
        pdp->req_qos.ext_max_bit_rate_downlink = ext_buf[cur_pos/8];
        MSG_HIGH_DS_1(SM_SUB,"N/W MBR extn donwlink %d",pdp->req_qos.ext_max_bit_rate_downlink);
        if ( pdp->req_qos.ext_max_bit_rate_downlink > 0xFA)
        {
           pdp->req_qos.ext_max_bit_rate_downlink = 0xFA;
        }
        ADD_BYTE_LEN(cur_pos);
      }

      if (ie_length > 13)
      {
        //unpack OCTET 14
        pdp->req_qos.ext_gtd_bit_rate_downlink = ext_buf[cur_pos/8];
        MSG_HIGH_DS_1(SM_SUB,"N/W GBR extn donwlink %d",pdp->req_qos.ext_gtd_bit_rate_downlink);
        if ( pdp->req_qos.ext_gtd_bit_rate_downlink > 0xFA)
        {
           pdp->req_qos.ext_gtd_bit_rate_downlink = 0xFA;
        }
        ADD_BYTE_LEN(cur_pos);
      }
     if(mm_nv_wcdma_hsupa_category >= MM_RRC_WCMDA_DC_HSUPA_CATEGORY)
     {
      if (ie_length > 14)
      {
        //unpack OCTET 15
        pdp->req_qos.ext_max_bit_rate_uplink = ext_buf[cur_pos/8];
        MSG_HIGH_DS_1(SM_SUB,"N/W MBR extn uplink %d",pdp->req_qos.ext_max_bit_rate_uplink);
        if ( pdp->req_qos.ext_max_bit_rate_uplink > 0xFA)
        {
           pdp->req_qos.ext_max_bit_rate_uplink = 0xFA;
        }
        ADD_BYTE_LEN(cur_pos);
      }
      else
      {
        pdp->req_qos.ext_max_bit_rate_uplink = 0;
        pdp->req_qos.ext_gtd_bit_rate_uplink = 0;
      }

      if (ie_length > 15)
      {
        //unpack OCTET 16
        pdp->req_qos.ext_gtd_bit_rate_uplink = ext_buf[cur_pos/8];
        MSG_HIGH_DS_1(SM_SUB,"N/W GBR extn uplink %d",pdp->req_qos.ext_gtd_bit_rate_uplink);
        if ( pdp->req_qos.ext_gtd_bit_rate_uplink > 0xFA)
        {
           pdp->req_qos.ext_gtd_bit_rate_uplink = 0xFA;
        }
        ADD_BYTE_LEN(cur_pos);
      }
      else
      {
        pdp->req_qos.ext_gtd_bit_rate_uplink = 0;
      }
     }
    }
    else
    {
      pdp->req_qos.sig_ind                   = 0;
      pdp->req_qos.src_stat_desc             = 0;
      pdp->req_qos.ext_max_bit_rate_downlink = 0;
      pdp->req_qos.ext_gtd_bit_rate_downlink = 0;
      pdp->req_qos.ext_max_bit_rate_uplink = 0;
      pdp->req_qos.ext_gtd_bit_rate_uplink = 0;
    }
  }
  else
  {
    pdp->req_qos.traffic_class         = 0;
    pdp->req_qos.deliver_order         = 0;
    pdp->req_qos.deliver_err_sdu       = 0;
    pdp->req_qos.max_sdu_size          = 0;
    pdp->req_qos.max_bit_rare_uplink   = 0;
    pdp->req_qos.max_bit_rare_downlink = 0;
    pdp->req_qos.residual_ber          = 0;
    pdp->req_qos.sdu_err_ratio         = 0;
    pdp->req_qos.tranfer_delay         = 0;
    pdp->req_qos.traffic_priority      = 0;
    pdp->req_qos.guarantee_ber_uplink  = 0;
    pdp->req_qos.guarantee_ber_dwnlink = 0;
  }

  *msg_length +=   (ie_length + 1);

  return (ie_length);
}
