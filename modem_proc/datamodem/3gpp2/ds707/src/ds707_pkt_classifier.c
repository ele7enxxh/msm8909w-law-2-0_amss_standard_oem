/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

GENERAL DESCRIPTION
  Pakcet classifier for on target verfication without actual signal

EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2004-2014 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_pkt_classifier.c#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/25/14   hr      Initial version 

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"
#include "ps_ppp.h"
#include "cm.h"
#include "dsm.h"
#include "ps_ppp_sig.h"
#include "ds707_pdn_context.h"
#include "ds707_data_session_profile.h"

#include "ds707_pkt_classifier.h"
#include "ps_svc.h"
#include "data_msg.h"
#include "ds707_wmk.h"
#include "ds707_pkt_mgr.h"
#include "dshdr_an_wmk.h"
#include "nv.h"
#include "nv_items.h"
#include "dstaski.h"
#include "sys.h"
#include "cai.h"
#include "ps_ppp.h"
#include "ps_ppp_ncp.h"
#include "ps_meta_info.h"
#include "ps_pppi.h"
#include "crc.h"
#include "hdrcom_api.h"
#include "hdrds.h"
#include "ds707_s101.h"
#include "ds_dsd_ext_i.h"
#include "ds707_pkt_mgr_hdlr.h"
#include "dsutil.h"
#include "dssdns.h"
#include "ps_in.h"
#include "dssocket_defs.h"
#include "ps_iface_defs.h"

#ifdef FEATURE_DATA_PKT_CLASSIFIER

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/
/*-------------------------------------------------------------------------- 
  Table used to calculate FCS
--------------------------------------------------------------------------*/
static uint16 fcstab[256] = {
    0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
    0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
    0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
    0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
    0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
    0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
    0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
    0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
    0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
    0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
    0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
    0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
    0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
    0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
    0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
    0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
    0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
    0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
    0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
    0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
    0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
    0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
    0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
    0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
    0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
    0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
    0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
    0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
    0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
    0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
    0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
    0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

/*-------------------------------------------------------------------------- 
  To records if packet classifier is enabled in NV
--------------------------------------------------------------------------*/
static boolean ds707_pkt_classifier_enabled = FALSE;

/*-------------------------------------------------------------------------- 
  To record the current testing RAT
    SYS_SYS_MODE_CDMA - 1X/HRPD
    SYS_SYS_MODE_HDR  - eHRPD
--------------------------------------------------------------------------*/
static sys_sys_mode_e_type ds707_pkt_classifier_sys_mode = SYS_SYS_MODE_CDMA;

/*-------------------------------------------------------------------------- 
  To record if IPCP request is sent or not
--------------------------------------------------------------------------*/
static boolean ds707_pkt_classifier_um_ipcp_req_sent = FALSE;
static boolean ds707_pkt_classifier_um_ipv6cp_req_sent = FALSE;

/*-------------------------------------------------------------------------- 
  IP Pool
--------------------------------------------------------------------------*/
static uint32 efs_ip[DS707_MAX_PDN_INSTANCES + 1] = { 
                            0x647B2A0A,//GW IP 
                            0x6A7B2A0A //UE IP
                          };

static uint64 efs_iid[2] = {
                             0x5A26715F79027A5C, //GW IID
                             0x5B26715F79027A5C  //UE IID
                           };

/*-------------------------------------------------------------------------- 
  Multi PDN capability
--------------------------------------------------------------------------*/
static uint8 ds707_pkt_classifier_max_supported_pdn = 1;
static uint8 ds707_pkt_classifier_assigned_pdn = 0;

#if 0 /*Not used for now*/
static uint8 server_ready_to_send =0;
static uint8 client_ready_to_send =0x02;
#endif /* if 0 */

extern void ds3gi_cm_call_event_cb
(
  cm_call_event_e_type          call_event, /* The call event that occurred*/
  const cm_mm_call_info_s_type *call_info_ptr          /* Call information */
);

extern void ds707_pkti_sys_chng_hdlr_ex
(
  sys_sys_mode_e_type sys_mode
);

extern void ds_hdr_an_ppp_start_pkt_classifier
(
  void
);

/*===========================================================================
                        INTERNAL FUNCTION DEFINTIONS
===========================================================================*/
/*===========================================================================
FUNCTION CAL_FCS

DESCRIPTION
  This function calculates the FCS for the packet
 
DEPENDENCIES
  None

RETURN VALUE
  uint16 fcs value

SIDE EFFECTS
  None
===========================================================================*/
uint16 cal_fcs(uint8 *cp, uint16 len)
{
  uint16 fcs = 0xffff;

  while (len--)
  {
    if (*cp == 0x7D)
    {
      cp++;
      fcs = (fcs >> 8) ^ fcstab[(fcs ^ ((*cp++) - 0x20)) & 0xff];
	  len--;
    }
    else
    {
	fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];
    }
  }
  fcs ^= 0xffff;

  return fcs;
}/* cal_fcs */

/*===========================================================================
FUNCTION DS707_PKT_CLASSIFIER_PUT_ON_AN_WMK

DESCRIPTION
  This function puts the packet on AN RX watermark
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pkt_classifier_put_on_an_wmk
(
   dsm_item_type   **dsm_item_ptr_ptr
)
{
  dsm_enqueue(get_dshdr_an_wmk_um_fwd_wmk_ptr(), dsm_item_ptr_ptr);
  PS_SET_SIGNAL(PS_PPP_UM_AN_RX_SIGNAL);

  return;
}/* ds707_pkt_classifier_put_on_an_wmk */

/*===========================================================================
FUNCTION DS707_IS_PKT_CLASSIFIER_ENABLED

DESCRIPTION
  This function returns if packet classifier is enabled in NV
 
DEPENDENCIES
  None

RETURN VALUE
  TRUE  if packet classifier is enabled in NV
  FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_is_pkt_classifier_enabled
(
  void
)
{

  return ds707_pkt_classifier_enabled;
}/* ds707_is_pkt_classifier_enabled */

/*===========================================================================
FUNCTION DS707_PKT_CLASSIFIER_READ_IP_FROM_EFS

DESCRIPTION
  This function reads UE and GW IP from the EFS
 
DEPENDENCIES
  None

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pkt_classifier_read_ip_from_efs
(
  ds3gcfgmgr_efs_token_type  *efs_db
)
{
  uint8 index = 0;
  uint8 length = 0;
  int16 dss_errno = 0;
  char *from, *to; /* ptrs to start and end of the token */
  char temp_array[DS_EFS_READ_BUFFER_SZ];
  struct ps_in_addr temp_in_addr;
  ds3gcfgmgr_efs_token_parse_status_type ret_val 
                                           = DS3GCFGMGR_EFS_TOKEN_PARSE_SUCCESS;
/*-------------------------------------------------------------------------*/

  /* set the seperator as ; */
  efs_db->seperator = ';';

  while( (index < (DS707_MAX_PDN_INSTANCES + 1)) &&
         (DS3GCFGMGR_EFS_TOKEN_PARSE_EOF
            != (ret_val = ds3gcfgmgr_efs_tokenizer(efs_db, &from, &to))) )
  {
    length = to - from;
    if ((length > 0) && (length < DS_EFS_READ_BUFFER_SZ))
    {
      /*-----------------------------------------------------------------
        strlcpy copies the string and adds '\0' in the end. This is
        the reason for copying length+1 characters.
      -----------------------------------------------------------------*/
      (void) strlcpy(temp_array,from,length+1);

      if (DSS_SUCCESS == dss_inet_pton(temp_array,
                                       DSS_AF_INET,
                                       &temp_in_addr,
                                       sizeof(struct ps_in_addr),
                                       &dss_errno ))
      {
        /* copy the address in the profile structure */
        (void) memscpy(&efs_ip[index],
                       sizeof(uint32),
                       &temp_in_addr.ps_s_addr,
                       sizeof(uint32));

        DATA_IS707_MSG1(MSG_LEGACY_HIGH,
                        "IP address read 0x%x",
                        efs_ip[index]);

        if (index > 1)
        {
          ds707_pkt_classifier_max_supported_pdn++;
        }
        index++;
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_ERROR,
                        "Invalid IP address format");
      }
    }
  } /* end of while loop */
}/*ds707_pkt_classifier_read_ip_from_efs*/

/*===========================================================================
FUNCTION DS707_PKT_CLASSIFIER_PUT_ON_SN_WMK

DESCRIPTION
  This function puts the packet on SN RX watermark
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pkt_classifier_put_on_sn_wmk
(
   dsm_item_type   **dsm_item_ptr_ptr
)
{
  dsm_watermark_type    *fwd_wmk_list;
#if 0
  ds707_flow_type       *default_flow_ptr;
  ds707_tc_state_type   *default_tc_state_ptr;
  dsm_item_type 		*dsm_my_data;
#endif/* if 0*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - */

  fwd_wmk_list = ds707_get_wmk_um_SN_fwd_wm_list();
  dsm_enqueue(fwd_wmk_list, dsm_item_ptr_ptr);
  ds707_pkti_um_rlp_fwd_func(1);

#if 0
  default_tc_state_ptr = ds707_get_pri_tc_ptr();
  default_flow_ptr     = DS707_PRI_DS_FLOW_PTR(default_tc_state_ptr);

  shrawan_set_client_data_from_server();
  if (shrawan_ready_to_send_data_from_client() == TRUE) 
  {//send data
	
	dsm_my_data = dsm_dequeue(default_flow_ptr->tx_wm_list);
	if (dsm_my_data != NULL) 
	{
	  shrawan_reset_client_ready();
	  DATA_IS707_MSG0(MSG_LEGACY_HIGH,"dequeue from UM WMK and put on AN WMK");
	  ds707_pkt_classifier_an_enque(&dsm_my_data);
	  
	}
  }
#endif

  return;
}/* ds707_pkt_classifier_put_on_sn_wmk */

/*===========================================================================
                         EXTERNAL FUNCTION DEFINTIONS
===========================================================================*/

/*===========================================================================
FUNCTION DS707_PKT_CLASSIFIER_SET_AN_PPP_CONFIG

DESCRIPTION
  This function stores the AN PPP configurations
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pkt_classifier_set_an_ppp_config
(
  ppp_dev_opts_type *ppp_config_info
)
{
  if (ds707_is_pkt_classifier_enabled())
  {
    ppp_config_info->lcp_info.mode            = PPP_CP_ACTIVE;
    ppp_config_info->ipcp_info.mode           = PPP_CP_PASSIVE;

    ppp_config_info->auth_info.passwd_len =10;

    ppp_set_v4_server_opts(ppp_config_info,
                           efs_ip[0],
                           efs_ip[1],
                           0x08080808,
                           0x08080808,
                           /*-------------------------------------------------
                             For WINS support - Passing 0 for primary and
                             secondary nbns address as 1x doesn't support WINS
                           --------------------------------------------------*/
                           0,                     
                           0
                           );
  }
}/* ds707_pkt_classifier_set_an_ppp_config */

/*===========================================================================
FUNCTION DS707_PKT_CLASSIFIER_ROUTE_AN_TX_UM_DATA

DESCRIPTION
  This function parses the AN tx data packet and routes it.
  LCP packets are reverted back to AN.
  IPCP packets (1X/HRPD) are routed to SN.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pkt_classifier_route_an_tx_um_data
(
  void
)
{
  dsm_item_type *pkt_head_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ds707_is_pkt_classifier_enabled())
  {
    pkt_head_ptr = dsm_dequeue(get_dshdr_an_wmk_um_rev_wmk_ptr());

    if (pkt_head_ptr == NULL)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "NULL detected");
      return;
    }

    if ( (*(char *)((*pkt_head_ptr).data_ptr + 4) == 0xC0) &&
         (*(char *)((*pkt_head_ptr).data_ptr + 5) == 0x21) 
       )
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH,"AN LCP packets, revert back to AN");
      ds707_pkt_classifier_put_on_an_wmk(&pkt_head_ptr);
    }
    else
    {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "AN IPCP packets, put it on SN wmk");
      ds707_pkt_classifier_put_on_sn_wmk(&pkt_head_ptr);
    }
  }

#if 0 /*Not used now*/
  	if (config request...) 
    {
	  DATA_IS707_MSG0(MSG_LEGACY_HIGH,"IP address");
	  ds707_pkt_classifier_deque_from_AN_and_put_on_SN_wmk();
	}
	if (config_ack) 
	{

	}

	  temp1 = *(char *)((*(*dsm_item_ptr_ptr)).data_ptr+1);
	  DATA_IS707_MSG0(MSG_LEGACY_HIGH,"temp1 = %x",temp1,0,0);
	  if (temp1 == (char)0x80) 
	  {
		DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Match");
		
	  }

	  temp2 = *(char *)((*(*dsm_item_ptr_ptr)).data_ptr+2);
	  DATA_IS707_MSG0(MSG_LEGACY_HIGH,"temp2 = %x",temp2,0,0);
	  if (temp2 == (char)0x21) 
	  {
		DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Match");
	  }

	  /* If IPCP config ack, revert the packet*/
	  temp3 = *(char *)((*(*dsm_item_ptr_ptr)).data_ptr+3);
	  DATA_IS707_MSG0(MSG_LEGACY_HIGH,"temp3 = 0x%x",temp3,0,0);

	/*  if (*(char *)((*(*pkt_head_ptr)).data_ptr+1) == 80 && 
		  *(char *)((*(*pkt_head_ptr)).data_ptr+2) == 21
		  ) 
		  */
	  if ((temp1 == (char)0x80) && (temp2 == (char)0x21)) 
	  {
		if (temp3 == (char)0x01) //IPCP Request
		{
		 // dsm_enqueue(get_dshdr_an_wmk_um_fwd_wmk_ptr(), pkt_head_ptr);
		 // DATA_IS707_MSG0(MSG_LEGACY_HIGH,"put IPCP on AN WMK, it is [%d] packet",i,0,0);
		 // PS_SET_SIGNAL(PS_PPP_UM_AN_RX_SIGNAL);
		  DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Send CONFIG REQ to UM");
		  ds707_pkt_classifier_deque_from_AN_and_put_on_SN_wmk();
		}
		else if (temp3 == (char)0x02) //This is ACK.. revert it back.
		{
		  //DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Reverting ACK for the UM IFACE");
		  //ds707_pkt_classifier_put_on_sn_wmk(pkt_head_ptr);

		  /* send it to the AN..*/
		  //dsm_enqueue(get_dshdr_an_wmk_um_fwd_wmk_ptr(), pkt_head_ptr);
		  //DATA_IS707_MSG0(MSG_LEGACY_HIGH,"put IPCP on AN WMK, it is [%d] packet",i,0,0);
		  //PS_SET_SIGNAL(PS_PPP_UM_AN_RX_SIGNAL);	 
		  DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Send ACK from AN to UM") ;
		  ds707_pkt_classifier_deque_from_AN_and_put_on_SN_wmk();
		}
		else if (temp3 == (char)0x03)
		{
		  DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Send NAK from AN to UM") ;
		  ds707_pkt_classifier_deque_from_AN_and_put_on_SN_wmk();
		}
		else
		{
		  DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Send Other pkt from AN to UM") ;
		  ds707_pkt_classifier_deque_from_AN_and_put_on_SN_wmk();
		}
	  }
	  else
	  {
		DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Spurious message");
	  }
#endif/* if 0*/

  return;
}/* ds707_pkt_classifier_route_an_tx_um_data */

/*===========================================================================
FUNCTION DS707_PKT_CLASSIFIER_ROUTE_UM_TX_SN_DATA

DESCRIPTION
  This function parses the UM tx data packet, modifies it and routes it.
  LCP packets are reverted back to SN.
  IPCP packets (1X/HRPD) are routed to AN.
  VSNCP packets (eHRPD) are modified and reverted back to SN.
  Ping packets are modified and reverted back to SN.
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pkt_classifier_route_um_tx_sn_data
(
  dsm_watermark_type *wm_ptr
)
{
  uint16 fcs = 0xffff;
  uint16 copy_iterator;
  uint16 pkt_len = 0;
  uint16 apn_len = 0;
  char temp_copy[4];
  char buf_write[222] = {0};
  char buf_cache[47] = {0};
  char buf_discard[28] = {0};
  char apn_cache[100] = {0};
  char pdn_id = 0;
  char vsncp_id = 0;
  dsm_item_type *pkt_head_ptr = NULL;
  dsm_item_type *config_req_item_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( ds707_is_pkt_classifier_enabled() )
  {
    pkt_head_ptr = dsm_dequeue(wm_ptr);

    if (pkt_head_ptr == NULL)
    {
      DATA_IS707_MSG0(MSG_LEGACY_ERROR, "NULL detected");
      return;
    }

    if( ds707_pkt_classifier_sys_mode == SYS_SYS_MODE_CDMA )
    {
      if ( (*(char *)((*pkt_head_ptr).data_ptr + 4) == 0xC0) &&
           (*(char *)((*pkt_head_ptr).data_ptr + 5) == 0x21) &&
           ((*(char *)((*pkt_head_ptr).data_ptr + 7) == 0x21) ||
            (*(char *)((*pkt_head_ptr).data_ptr + 7) == 0x22))
         )      
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH,"SN LCP REQ/ACK, revert back");

        /*-------------------------------------------------------------------
          Ajust the magic number
        -------------------------------------------------------------------*/
        for (copy_iterator = 0, temp_copy[0] = 4;
             copy_iterator < temp_copy[0]; copy_iterator++) 
        {
          if(*(char *)((*pkt_head_ptr).data_ptr + (30 + copy_iterator)) == 0x7D)
          {
            copy_iterator++;
            temp_copy[0]++;
          }
          else
          {
            if (*(char *)((*pkt_head_ptr).data_ptr + 7) == 0x21)
            {
              (*(char *)((*pkt_head_ptr).data_ptr + (30 + copy_iterator)))++;
            }
            else
            {
              (*(char *)((*pkt_head_ptr).data_ptr + (30 + copy_iterator)))--;
            }
            break;
          }
        }

        pkt_len = pkt_head_ptr->used;

        /*----------------------------------------------------------------
          Calculate FCS
        ----------------------------------------------------------------*/
        fcs = cal_fcs((uint8 *)((*pkt_head_ptr).data_ptr + 1), pkt_len - 4);
        *(char *)((*pkt_head_ptr).data_ptr + pkt_len - 3) = (uint8)(fcs & 0xff);
        *(char *)((*pkt_head_ptr).data_ptr + pkt_len - 2) = (uint8)((fcs >> 8) & 0xff);

        ds707_pkt_classifier_put_on_sn_wmk(&pkt_head_ptr);
      }
      else if( (*(char *)((*pkt_head_ptr).data_ptr + 4) == 0xC0) &&
               (*(char *)((*pkt_head_ptr).data_ptr + 5) == 0x21) &&
               (*(char *)((*pkt_head_ptr).data_ptr + 7) == 0x25)
             )
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH,"SN LCP TERM REQ, send TERM ACK");

        /*-------------------------------------------------------------------
          TERM ACK
        -------------------------------------------------------------------*/
        *(char *)((*pkt_head_ptr).data_ptr + 7) = 0x26;

        pkt_len = pkt_head_ptr->used;

        /*----------------------------------------------------------------
          Calculate FCS
        ----------------------------------------------------------------*/
        fcs = cal_fcs((uint8 *)((*pkt_head_ptr).data_ptr + 1), pkt_len - 4);
        *(char *)((*pkt_head_ptr).data_ptr + pkt_len - 3) = (uint8)(fcs & 0xff);
        *(char *)((*pkt_head_ptr).data_ptr + pkt_len - 2) = (uint8)((fcs >> 8) & 0xff);


        ds707_pkt_classifier_put_on_sn_wmk(&pkt_head_ptr);
      }
      else if ( (*(char *)((*pkt_head_ptr).data_ptr + 1) == 0x80) &&
                (*(char *)((*pkt_head_ptr).data_ptr + 2) == 0x21)
              )
      {
        ds707_pkt_classifier_um_ipcp_req_sent = TRUE;

        DATA_IS707_MSG0(MSG_LEGACY_HIGH,"SN IPCP Packet, put it on AN WMK");
        ds707_pkt_classifier_put_on_an_wmk(&pkt_head_ptr);
      }
      else if ( (*(char *)((*pkt_head_ptr).data_ptr + 1) == 0x80) &&
                (*(char *)((*pkt_head_ptr).data_ptr + 2) == 0x57) &&
                (*(char *)((*pkt_head_ptr).data_ptr + 3) == 0x01)
              )
      {
        pkt_len = pkt_head_ptr->used;

        if (ds707_pkt_classifier_um_ipv6cp_req_sent == FALSE)
        {
          DATA_IS707_MSG0(MSG_LEGACY_HIGH,"SN IPV6CP Request, send IPV6CP REQ");

          /*----------------------------------------------------------------
            Revert IPV6CP REQ
          ----------------------------------------------------------------*/
          config_req_item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
          dsm_extract(pkt_head_ptr, 0,buf_cache, pkt_len);
          dsm_pushdown(&config_req_item_ptr, buf_cache, pkt_len, DSM_DS_SMALL_ITEM_POOL);

          /* Server IP */
          memscpy((void *)((*config_req_item_ptr).data_ptr + 9),
                  sizeof(uint64),
                  (void*)(&efs_iid[0]),
                  sizeof(uint64));

          /*----------------------------------------------------------------
            Calculate FCS
          ----------------------------------------------------------------*/
          fcs = cal_fcs((uint8 *)((*config_req_item_ptr).data_ptr + 1), pkt_len - 4);
          *(char *)((*config_req_item_ptr).data_ptr + pkt_len - 3) = (uint8)(fcs & 0xff);
          *(char *)((*config_req_item_ptr).data_ptr + pkt_len - 2) = (uint8)((fcs >> 8) & 0xff);

          ds707_pkt_classifier_put_on_sn_wmk(&config_req_item_ptr);
          ds707_pkt_classifier_um_ipv6cp_req_sent = TRUE;
        }

        if ( 0 != memcmp((void *)((*pkt_head_ptr).data_ptr + 9),
                         (void*)(&efs_iid[1]),
                         sizeof(uint64))
           )
        {
          DATA_IS707_MSG0(MSG_LEGACY_HIGH,
                          "SN IPV6CP Request with undesired IID, "
                          "send IPV6CP NAK with configured IID");

          /*----------------------------------------------------------------
            Generate IPV6CP NAK
          ----------------------------------------------------------------*/
          *(char *)((*pkt_head_ptr).data_ptr + 3) = 0x03;

          /*----------------------------------------------------------------
            Indicate desired IID
          ----------------------------------------------------------------*/
          memscpy((void *)((*pkt_head_ptr).data_ptr + 9),
                  sizeof(uint64),
                  (void*)(&efs_iid[1]),
                  sizeof(uint64));

          /*----------------------------------------------------------------
            Calculate FCS
          ----------------------------------------------------------------*/
          fcs = cal_fcs((uint8 *)((*pkt_head_ptr).data_ptr + 1), pkt_len - 4);
          *(char *)((*pkt_head_ptr).data_ptr + pkt_len - 3) = (uint8)(fcs & 0xff);
          *(char *)((*pkt_head_ptr).data_ptr + pkt_len - 2) = (uint8)((fcs >> 8) & 0xff);

          ds707_pkt_classifier_put_on_sn_wmk(&pkt_head_ptr);
        }
        else
        {
          DATA_IS707_MSG0(MSG_LEGACY_HIGH,"SN IPV6CP Request, send IPV6CP ACK");

          /*----------------------------------------------------------------
            Generate IPV6CP ACK
          ----------------------------------------------------------------*/
          *(char *)((*pkt_head_ptr).data_ptr + 3) = 0x02;

          /*----------------------------------------------------------------
            Calculate FCS
          ----------------------------------------------------------------*/
          fcs = cal_fcs((uint8 *)((*pkt_head_ptr).data_ptr + 1), pkt_len - 4);
          *(char *)((*pkt_head_ptr).data_ptr + pkt_len - 3) = (uint8)(fcs & 0xff);
          *(char *)((*pkt_head_ptr).data_ptr + pkt_len - 2) = (uint8)((fcs >> 8) & 0xff);

          ds707_pkt_classifier_put_on_sn_wmk(&pkt_head_ptr);
        }
      }
      else if ( (*(char *)((*pkt_head_ptr).data_ptr + 1) == 0x57) &&
                (*(char *)((*pkt_head_ptr).data_ptr + 8) == 0x3A) &&
                (*(char *)((*pkt_head_ptr).data_ptr + 42) == 0x85)
              )
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH,"SN RS, send RA");

        /* Payload Length*/
        (*(char *)((*pkt_head_ptr).data_ptr + 7)) += 0x30;

        /* Server IP */
        (*(char *)((*pkt_head_ptr).data_ptr + 25))--;

        /* Dest IP */
        *(char *)((*pkt_head_ptr).data_ptr + 41) = 0x1;

        /* RA */
        (*(char *)((*pkt_head_ptr).data_ptr + 42))++;

        /* CRC */
        *(char *)((*pkt_head_ptr).data_ptr + 44) = 0x5A;
        *(char *)((*pkt_head_ptr).data_ptr + 45) = 0x90;

        buf_write[0]  = 0x40;
        buf_write[1]  = 0x00;
        buf_write[2]  = 0x03;
        buf_write[3]  = 0xE8;
        buf_write[4]  = 0x00;
        buf_write[5]  = 0x00;
        buf_write[6]  = 0x00;
        buf_write[7]  = 0x00;
        buf_write[8]  = 0x00;
        buf_write[9]  = 0x00;
        buf_write[10]  = 0x00;
        buf_write[11]  = 0x00;
        buf_write[12]  = 0x03;
        buf_write[13]  = 0x04;
        buf_write[14]  = 0x40;
        buf_write[15]  = 0xC0;
        buf_write[16]  = 0x00;
        buf_write[17]  = 0x00;
        buf_write[18]  = 0x1C;
        buf_write[19]  = 0x20;
        buf_write[20]  = 0x00;
        buf_write[21]  = 0x00;
        buf_write[22]  = 0x0E;
        buf_write[23]  = 0x10;
        buf_write[24]  = 0x00;
        buf_write[25]  = 0x00;
        buf_write[26]  = 0x00;
        buf_write[27]  = 0x00;
        buf_write[28]  = 0x20;
        buf_write[29]  = 0x02;
        buf_write[30]  = 0xC0;
        buf_write[31]  = 0x23;
        buf_write[32]  = 0x9C;
        buf_write[33]  = 0x17;
        buf_write[34]  = 0x04;
        buf_write[35]  = 0x9E;
        buf_write[36]  = 0x00;
        buf_write[37]  = 0x00;
        buf_write[38]  = 0x00;
        buf_write[39]  = 0x00;
        buf_write[40]  = 0x00;
        buf_write[41]  = 0x00;
        buf_write[42]  = 0x00;
        buf_write[43]  = 0x00;
        buf_write[44]  = 0x05;
        buf_write[45]  = 0x01;
        buf_write[46]  = 0x00;
        buf_write[47]  = 0x00;
        buf_write[48]  = 0x00;
        buf_write[49]  = 0x00;
        buf_write[50]  = 0x05;
        buf_write[51]  = 0x00;
        buf_write[52]  = 0xF7;
        buf_write[53]  = 0x2F;

        dsm_pullup(&pkt_head_ptr, buf_cache, 46);
        dsm_pullup(&pkt_head_ptr, buf_discard, 6);
        dsm_pushdown(&pkt_head_ptr, buf_write, 54, DSM_DS_SMALL_ITEM_POOL);
        dsm_pushdown(&pkt_head_ptr, buf_cache, 46, DSM_DS_SMALL_ITEM_POOL);


        ds707_pkt_classifier_put_on_sn_wmk(&pkt_head_ptr);
      }
      else if( (*(char *)((*pkt_head_ptr).data_ptr + 11) == 1) &&
               (*(char *)((*pkt_head_ptr).data_ptr + 22) == 8)
             )
      {
        /*-------------------------------------------------------------------
          Revert Ping packets
        -------------------------------------------------------------------*/
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Ping packet, revert ping resp");
        *(char *)((*pkt_head_ptr).data_ptr + 22) = 0;

        /*----------------------------------------------------------------
          Revert source and destination IP
        ----------------------------------------------------------------*/
        for (copy_iterator = 0; copy_iterator<4; copy_iterator++) 
        {
          temp_copy[copy_iterator] = 
                *(char *)((*pkt_head_ptr).data_ptr + (14 + copy_iterator));

          *(char *)((*pkt_head_ptr).data_ptr + (14 + copy_iterator)) = 
                  *(char *)((*pkt_head_ptr).data_ptr + (18 + copy_iterator));

          *(char *)((*pkt_head_ptr).data_ptr + (18 + copy_iterator)) = 
                                                  temp_copy[copy_iterator];
        }
              
        /*----------------------------------------------------------------
          Calculate CRC
        ----------------------------------------------------------------*/
        *(char *)((*pkt_head_ptr).data_ptr + 24) += 0x08;

        pkt_len = pkt_head_ptr->used;
        if (*(char *)((*pkt_head_ptr).data_ptr + (24)) == 0x7E)
        {
          *(char *)((*pkt_head_ptr).data_ptr + (24)) = 0x5E;

          buf_write[0] = 0x7D;
          dsm_pullup(&pkt_head_ptr, buf_cache, 24);
          dsm_pushdown(&pkt_head_ptr, buf_write, 1, DSM_DS_SMALL_ITEM_POOL);
          dsm_pushdown(&pkt_head_ptr, buf_cache, 24, DSM_DS_SMALL_ITEM_POOL);
          pkt_len++;
        }

        /*-------------------------------------------------------------------
          Calculate FCS
        -------------------------------------------------------------------*/
        fcs = cal_fcs((uint8 *)((*pkt_head_ptr).data_ptr + 1), pkt_len - 4);
        *(char *)((*pkt_head_ptr).data_ptr + (pkt_len - 3)) = (uint8)(fcs & 0xff);
        *(char *)((*pkt_head_ptr).data_ptr + (pkt_len - 2)) = (uint8)((fcs >> 8) & 0xff);

        ds707_pkt_classifier_put_on_sn_wmk(&pkt_head_ptr);

      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Dropping unhandled packets");
        dsm_free_packet(&pkt_head_ptr);
      }
    }/* CDMA */
    else
    {
      /*-------------------------------------------------------------------
        LCP packet, revert back to SN RX
      -------------------------------------------------------------------*/
      if ( (*(char *)((*pkt_head_ptr).data_ptr + 4) == 0xC0) &&
           (*(char *)((*pkt_head_ptr).data_ptr + 5) == 0x21) &&
           ((*(char *)((*pkt_head_ptr).data_ptr + 7) == 0x21) ||
            (*(char *)((*pkt_head_ptr).data_ptr + 7) == 0x22))
         )
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH,"LCP REQ/ACK, revert back");
        ds707_pkt_classifier_put_on_sn_wmk(&pkt_head_ptr);
      }
      else if( (*(char *)((*pkt_head_ptr).data_ptr + 4) == 0xC0) &&
               (*(char *)((*pkt_head_ptr).data_ptr + 5) == 0x21) &&
               (*(char *)((*pkt_head_ptr).data_ptr + 7) == 0x25)
             )
      {

        DATA_IS707_MSG0(MSG_LEGACY_HIGH,"SN LCP TERM REQ, send TERM ACK");
        *(char *)((*pkt_head_ptr).data_ptr + 7) = 0x26;

        pkt_len = pkt_head_ptr->used;

        /*----------------------------------------------------------------
          Calculate FCS
        ----------------------------------------------------------------*/
        fcs = cal_fcs((uint8 *)((*pkt_head_ptr).data_ptr + 1), pkt_len - 4);
        *(char *)((*pkt_head_ptr).data_ptr + pkt_len - 3) = (uint8)(fcs & 0xff);
        *(char *)((*pkt_head_ptr).data_ptr + pkt_len - 2) = (uint8)((fcs >> 8) & 0xff);

        ds707_pkt_classifier_put_on_sn_wmk(&pkt_head_ptr);

        ds707_pkt_classifier_assigned_pdn = 0;
      }
      else if ( (*(char *)((*pkt_head_ptr).data_ptr + 1) == 0x80) &&
                (*(char *)((*pkt_head_ptr).data_ptr + 2) == 0x5B) &&
                (*(char *)((*pkt_head_ptr).data_ptr + 3) == 0x01) ) 
      {
        if ( ds707_pkt_classifier_assigned_pdn < 
               ds707_pkt_classifier_max_supported_pdn )
#if 0 
        /* working code */
        {
          DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                           "VSNCP REQ, revert as ACK and generate REQ" );

          pdn_id = *(char *)((*pkt_head_ptr).data_ptr + 12);

          /*-------------------------------------------------------------------
           config ack
          -------------------------------------------------------------------*/
          *(char *)((*pkt_head_ptr).data_ptr + 3) = 0x2;

          /*-------------------------------------------------------------------
           VSCP len
          -------------------------------------------------------------------*/
          *(char *)((*pkt_head_ptr).data_ptr + 6) = 0x33;

          /*-------------------------------------------------------------------
           PDN Addr len
          -------------------------------------------------------------------*/
          *(char *)((*pkt_head_ptr).data_ptr + 28) = 0xf;

          /*-------------------------------------------------------------------
           PDN Addr type
          -------------------------------------------------------------------*/
          *(char *)((*pkt_head_ptr).data_ptr + 29) = 0x3; // v4v6

          /*-------------------------------------------------------------------
           default router address
          -------------------------------------------------------------------*/
          *(char *)((*pkt_head_ptr).data_ptr + 63) = efs_ip[0] & 0xff;
          *(char *)((*pkt_head_ptr).data_ptr + 64) = (efs_ip[0]>>8) & 0xff;
          *(char *)((*pkt_head_ptr).data_ptr + 65) = (efs_ip[0]>>16) &0xff;
          *(char *)((*pkt_head_ptr).data_ptr + 66) = (efs_ip[0]>>24) & 0xff;

          /*-------------------------------------------------------------------
           allocation success
          -------------------------------------------------------------------*/
          *(char *)((*pkt_head_ptr).data_ptr + 69) = 0xff;

          /*-------------------------------------------------------------------
           CRC
          -------------------------------------------------------------------*/
          *(char *)((*pkt_head_ptr).data_ptr + 70) = 0xdb;
          *(char *)((*pkt_head_ptr).data_ptr + 71) = 0xcf;

          /*-------------------------------------------------------------------
           PDN Addr IPv6
          -------------------------------------------------------------------*/
          buf_write[7] = 0x1;

          /*-------------------------------------------------------------------
           PDN Addr IPv4
          -------------------------------------------------------------------*/
          buf_write[8] = efs_ip[ds707_pkt_classifier_assigned_pdn + 1] & 0xff;
          buf_write[9] = (efs_ip[ds707_pkt_classifier_assigned_pdn + 1]>>8) & 0xff;
          buf_write[10] = (efs_ip[ds707_pkt_classifier_assigned_pdn + 1]>>16) & 0xff;
          buf_write[11] = (efs_ip[ds707_pkt_classifier_assigned_pdn + 1]>>24) & 0xff;

          ds707_pkt_classifier_assigned_pdn++;

          dsm_pullup(&pkt_head_ptr, buf_cache, 30);
          dsm_pullup(&pkt_head_ptr, buf_discard, 28);
          dsm_pushdown(&pkt_head_ptr, buf_write, 12, DSM_DS_SMALL_ITEM_POOL);
          dsm_pushdown(&pkt_head_ptr, buf_cache, 30, DSM_DS_SMALL_ITEM_POOL);

          /*-------------------------------------------------------------------
           FCS
          -------------------------------------------------------------------*/
          fcs = cal_fcs((uint8 *)((*pkt_head_ptr).data_ptr + 1), 53);
          *(char *)((*pkt_head_ptr).data_ptr + (54)) = (uint8)(fcs & 0xff);
          *(char *)((*pkt_head_ptr).data_ptr + (55)) = (uint8)((fcs >> 8) & 0xff);

          /*-------------------------------------------------------------------
           Put VSNCP ACK back on SN RX
          -------------------------------------------------------------------*/
          ds707_pkt_classifier_put_on_sn_wmk(&pkt_head_ptr);

          /*-------------------------------------------------------------------
            Generate config REQ
          -------------------------------------------------------------------*/
          buf_write[0]  = 0x7E;
          buf_write[1]  = 0x80;
          buf_write[2]  = 0x5B;
          buf_write[3]  = 0x01;
          buf_write[4]  = 0x01;
          buf_write[5]  = 0x00;
          buf_write[6]  = 0x0A;
          buf_write[7]  = 0xCF;
          buf_write[8]  = 0x00;
          buf_write[9]  = 0x02;
          buf_write[10] = 0x01;
          buf_write[11] = 0x03;
          buf_write[12] = pdn_id;

          fcs = cal_fcs((uint8*)(&buf_write[1]), 12);
          buf_write[13] = (uint8)(fcs & 0xff);
          buf_write[14] = (uint8)((fcs >> 8) & 0xff);

          buf_write[15] = 0x7E;

          config_req_item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
          dsm_pushdown(&config_req_item_ptr, buf_write, 16, DSM_DS_SMALL_ITEM_POOL);
          ds707_pkt_classifier_put_on_sn_wmk(&config_req_item_ptr);
        }
#endif
        {
          DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                           "VSNCP REQ, revert as ACK and generate REQ" );

          vsncp_id = *(char *)((*pkt_head_ptr).data_ptr + 4);
          pdn_id = *(char *)((*pkt_head_ptr).data_ptr + 12);
          apn_len = *(char *)((*pkt_head_ptr).data_ptr + 14) - 2;

          memscpy( apn_cache, 100,
                   (char *)((*pkt_head_ptr).data_ptr + 15), apn_len);

          buf_write[0]   = 0x80;
          buf_write[1]   = 0x5B;
          buf_write[2]   = 0x02;
          buf_write[3]   = vsncp_id;
          buf_write[4]   = 0x00;
          buf_write[5]   = 116 + apn_len;
          buf_write[6]   = 0xCF;
          buf_write[7]   = 0x00;
          buf_write[8]   = 0x02;
          buf_write[9]   = 0x01;
          buf_write[10]  = 0x03;
          buf_write[11]  = pdn_id;
          buf_write[12]  = 0x02;
          buf_write[13]  = apn_len + 2;

          memscpy( (void*)&buf_write[14], apn_len,
                   (void*)apn_cache, 100);

          buf_write[14 + apn_len]  = 0x03;
          buf_write[15 + apn_len]  = 0x03;
          buf_write[16 + apn_len]  = 0x03;
          buf_write[17 + apn_len]  = 0x04;
          buf_write[18 + apn_len]  = 0x0F;
          buf_write[19 + apn_len]  = 0x03;

          memscpy( (void*)&buf_write[20 + apn_len], sizeof(uint64),
                   (void*)&efs_iid[1], sizeof(uint64) );

          buf_write[28 + apn_len] = efs_ip[ds707_pkt_classifier_assigned_pdn + 1] & 0xff;
          buf_write[29 + apn_len] = (efs_ip[ds707_pkt_classifier_assigned_pdn + 1]>>8) & 0xff;
          buf_write[30 + apn_len] = (efs_ip[ds707_pkt_classifier_assigned_pdn + 1]>>16) & 0xff;
          buf_write[31 + apn_len] = (efs_ip[ds707_pkt_classifier_assigned_pdn + 1]>>24) & 0xff;

          buf_write[32 + apn_len]  = 0x05;
          buf_write[33 + apn_len]  = 0x4A;
          buf_write[34 + apn_len]  = 0x80;
          buf_write[35 + apn_len]  = 0x00;
          buf_write[36 + apn_len]  = 0x03;
          buf_write[37 + apn_len]  = 0x10;
          buf_write[38 + apn_len]  = 0x00;
          buf_write[39 + apn_len]  = 0x21;
          buf_write[40 + apn_len]  = 0x00;
          buf_write[41 + apn_len]  = 0x00;
          buf_write[42 + apn_len]  = 0x00;
          buf_write[43 + apn_len]  = 0x00;
          buf_write[44 + apn_len]  = 0x00;
          buf_write[45 + apn_len]  = 0x00;
          buf_write[46 + apn_len]  = 0x00;
          buf_write[47 + apn_len]  = 0x00;
          buf_write[48 + apn_len]  = 0x00;
          buf_write[49 + apn_len]  = 0x00;
          buf_write[50 + apn_len]  = 0x00;
          buf_write[51 + apn_len]  = 0x00;
          buf_write[52 + apn_len]  = 0x00;
          buf_write[53 + apn_len]  = 0x12;
          buf_write[54 + apn_len]  = 0x00;
          buf_write[55 + apn_len]  = 0x03;
          buf_write[56 + apn_len]  = 0x10;
          buf_write[57 + apn_len]  = 0x00;
          buf_write[58 + apn_len]  = 0x10;
          buf_write[59 + apn_len]  = 0x00;
          buf_write[60 + apn_len]  = 0x00;
          buf_write[61 + apn_len]  = 0x00;
          buf_write[62 + apn_len]  = 0x00;
          buf_write[63 + apn_len]  = 0x00;
          buf_write[64 + apn_len]  = 0x00;
          buf_write[65 + apn_len]  = 0x00;
          buf_write[66 + apn_len]  = 0x00;
          buf_write[67 + apn_len]  = 0x00;
          buf_write[68 + apn_len]  = 0x00;
          buf_write[69 + apn_len]  = 0x00;
          buf_write[70 + apn_len]  = 0x00;
          buf_write[71 + apn_len]  = 0x01;
          buf_write[72 + apn_len]  = 0x00;
          buf_write[73 + apn_len]  = 0x80;
          buf_write[74 + apn_len]  = 0x21;
          buf_write[75 + apn_len]  = 0x10;
          buf_write[76 + apn_len]  = 0x02;
          buf_write[77 + apn_len]  = 0x01;
          buf_write[78 + apn_len]  = 0x00;
          buf_write[79 + apn_len]  = 0x10;
          buf_write[80 + apn_len]  = 0x81;
          buf_write[81 + apn_len]  = 0x06;
          buf_write[82 + apn_len]  = 0xC0;
          buf_write[83 + apn_len]  = 0xA8;
          buf_write[84 + apn_len]  = 0x01;
          buf_write[85 + apn_len]  = 0x04;
          buf_write[86 + apn_len]  = 0x83;
          buf_write[87 + apn_len]  = 0x06;
          buf_write[88 + apn_len] = 0xC0;
          buf_write[89 + apn_len] = 0xA8;
          buf_write[90 + apn_len] = 0x01;
          buf_write[91 + apn_len] = 0x03;
          buf_write[92 + apn_len] = 0x00;
          buf_write[93 + apn_len] = 0x0D;
          buf_write[94 + apn_len] = 0x04;
          buf_write[95 + apn_len] = 0xC0;
          buf_write[96 + apn_len] = 0xA8;
          buf_write[97 + apn_len] = 0x01;
          buf_write[98 + apn_len] = 0x04;
          buf_write[99 + apn_len] = 0x00;
          buf_write[100 + apn_len] = 0x0D;
          buf_write[101 + apn_len] = 0x04;
          buf_write[102 + apn_len] = 0xC0;
          buf_write[103 + apn_len] = 0xA8;
          buf_write[104 + apn_len] = 0x01;
          buf_write[105 + apn_len] = 0x03;
          buf_write[106 + apn_len] = 0x07;
          buf_write[107 + apn_len] = 0x03;
          buf_write[108 + apn_len] = 0x01;
          buf_write[109 + apn_len] = 0x08;
          buf_write[110 + apn_len] = 0x06;
          buf_write[111 + apn_len] = efs_ip[0] & 0xff;
          buf_write[112 + apn_len] = (efs_ip[0]>>8) & 0xff;
          buf_write[113 + apn_len] = (efs_ip[0]>>16) & 0xff;
          buf_write[114 + apn_len] = (efs_ip[0]>>24) & 0xff;
          buf_write[115 + apn_len] = 0x09;
          buf_write[116 + apn_len] = 0x03;
          buf_write[117 + apn_len] = 0xFF;

          fcs = cal_fcs((uint8*)buf_write, 118 + apn_len );
          buf_write[118 + apn_len] = (uint8)(fcs & 0xff);
          buf_write[119 + apn_len] = (uint8)((fcs >> 8) & 0xff);

          buf_write[120 + apn_len] = 0x7E;

          /*-------------------------------------------------------------------
           Put VSNCP ACK back on SN RX
          -------------------------------------------------------------------*/
          dsm_free_packet(&pkt_head_ptr);
          pkt_head_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
          dsm_pushdown(&pkt_head_ptr, buf_write, 121 + apn_len, DSM_DS_SMALL_ITEM_POOL);
          ds707_pkt_classifier_put_on_sn_wmk(&pkt_head_ptr);

          /*-------------------------------------------------------------------
            Generate config REQ
          -------------------------------------------------------------------*/
          buf_write[0]  = 0x7E;
          buf_write[1]  = 0x80;
          buf_write[2]  = 0x5B;
          buf_write[3]  = 0x01;
          buf_write[4]  = 0x01;
          buf_write[5]  = 0x00;
          buf_write[6]  = 0x0A;
          buf_write[7]  = 0xCF;
          buf_write[8]  = 0x00;
          buf_write[9]  = 0x02;
          buf_write[10] = 0x01;
          buf_write[11] = 0x03;
          buf_write[12] = pdn_id;

          fcs = cal_fcs((uint8*)(&buf_write[1]), 12);
          buf_write[13] = (uint8)(fcs & 0xff);
          buf_write[14] = (uint8)((fcs >> 8) & 0xff);

          buf_write[15] = 0x7E;

          config_req_item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
          dsm_pushdown(&config_req_item_ptr, buf_write, 16, DSM_DS_SMALL_ITEM_POOL);
          ds707_pkt_classifier_put_on_sn_wmk(&config_req_item_ptr);
        }
        else
        {
          DATA_IS707_MSG0(MSG_LEGACY_ERROR, "Reached max allowed PDNs, reject");

          /* TODO: NAK it */
        }
      }
      else if ( (*(char *)((*pkt_head_ptr).data_ptr + 1) == 0x80) &&
                (*(char *)((*pkt_head_ptr).data_ptr + 2) == 0x5B) &&
                (*(char *)((*pkt_head_ptr).data_ptr + 3) == 0x05) ) 
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH,"VSNCP TERM REQ, send TERM ACK");
        *(char *)((*pkt_head_ptr).data_ptr + 3) = 0x06;

        pkt_len = pkt_head_ptr->used;
        /*-------------------------------------------------------------------
          Calculate FCS
        -------------------------------------------------------------------*/
        fcs = cal_fcs((uint8 *)((*pkt_head_ptr).data_ptr + 1), pkt_len - 4);
        *(char *)((*pkt_head_ptr).data_ptr + (pkt_len - 3)) = (uint8)(fcs & 0xff);
        *(char *)((*pkt_head_ptr).data_ptr + (pkt_len - 2)) = (uint8)((fcs >> 8) & 0xff);

        ds707_pkt_classifier_put_on_sn_wmk(&pkt_head_ptr);
      }
      else if ( (*(char *)((*pkt_head_ptr).data_ptr + 1) == 0x5B) &&
                (*(char *)((*pkt_head_ptr).data_ptr + 9) == 0x3A) &&
                (*(char *)((*pkt_head_ptr).data_ptr + 43) == 0x85)
              )
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH,"SN RS, send RA");

        pkt_len = pkt_head_ptr->used;
        /* Payload Length*/
        (*(char *)((*pkt_head_ptr).data_ptr + 8)) += 0x30;

        /* Server IP */
        (*(char *)((*pkt_head_ptr).data_ptr + 26))--;

        /* Dest IP */
        *(char *)((*pkt_head_ptr).data_ptr + 42) = 0x1;

        /* RA */
        (*(char *)((*pkt_head_ptr).data_ptr + 43))++;

        /* CRC */
        *(char *)((*pkt_head_ptr).data_ptr + 45) = 0x5A;
        *(char *)((*pkt_head_ptr).data_ptr + 46) = 0x90;

        buf_write[0]  = 0x40;
        buf_write[1]  = 0x00;
        buf_write[2]  = 0x03;
        buf_write[3]  = 0xE8;
        buf_write[4]  = 0x00;
        buf_write[5]  = 0x00;
        buf_write[6]  = 0x00;
        buf_write[7]  = 0x00;
        buf_write[8]  = 0x00;
        buf_write[9]  = 0x00;
        buf_write[10]  = 0x00;
        buf_write[11]  = 0x00;
        buf_write[12]  = 0x03;
        buf_write[13]  = 0x04;
        buf_write[14]  = 0x40;
        buf_write[15]  = 0xC0;
        buf_write[16]  = 0x00;
        buf_write[17]  = 0x00;
        buf_write[18]  = 0x1C;
        buf_write[19]  = 0x20;
        buf_write[20]  = 0x00;
        buf_write[21]  = 0x00;
        buf_write[22]  = 0x0E;
        buf_write[23]  = 0x10;
        buf_write[24]  = 0x00;
        buf_write[25]  = 0x00;
        buf_write[26]  = 0x00;
        buf_write[27]  = 0x00;
        buf_write[28]  = 0x20;
        buf_write[29]  = 0x02;
        buf_write[30]  = 0xC0;
        buf_write[31]  = 0x23;
        buf_write[32]  = 0x9C;
        buf_write[33]  = 0x17;
        buf_write[34]  = 0x04;
        buf_write[35]  = 0x9E;
        buf_write[36]  = 0x00;
        buf_write[37]  = 0x00;
        buf_write[38]  = 0x00;
        buf_write[39]  = 0x00;
        buf_write[40]  = 0x00;
        buf_write[41]  = 0x00;
        buf_write[42]  = 0x00;
        buf_write[43]  = 0x00;
        buf_write[44]  = 0x05;
        buf_write[45]  = 0x01;
        buf_write[46]  = 0x00;
        buf_write[47]  = 0x00;
        buf_write[48]  = 0x00;
        buf_write[49]  = 0x00;
        buf_write[50]  = 0x05;
        buf_write[51]  = 0x00;
        buf_write[52]  = 0x8B;
        buf_write[53]  = 0x74;
        buf_write[54]  = 0x7E;

        dsm_pullup(&pkt_head_ptr, buf_cache, 47);

        dsm_free_packet(&pkt_head_ptr);
        pkt_head_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
        dsm_pushdown(&pkt_head_ptr, buf_write, 54, DSM_DS_SMALL_ITEM_POOL);
        dsm_pushdown(&pkt_head_ptr, buf_cache, 47, DSM_DS_SMALL_ITEM_POOL);

        ds707_pkt_classifier_put_on_sn_wmk(&pkt_head_ptr);
      }
      else if ( (*(char *)((*pkt_head_ptr).data_ptr + 12) == 1) &&
                (*(char *)((*pkt_head_ptr).data_ptr + 23) == 8)
              )
      {
        /*-------------------------------------------------------------------
          Revert Ping packets
        -------------------------------------------------------------------*/
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Ping packet, revert ping resp");
        *(char *)((*pkt_head_ptr).data_ptr + 23) = 0;

        /*-------------------------------------------------------------------
          Revert source and destination IP
        -------------------------------------------------------------------*/
        for (copy_iterator = 0; copy_iterator<4; copy_iterator++) 
        {
          temp_copy[copy_iterator] = 
                *(char *)((*pkt_head_ptr).data_ptr + (15 + copy_iterator));

          *(char *)((*pkt_head_ptr).data_ptr + (15 + copy_iterator)) = 
                *(char *)((*pkt_head_ptr).data_ptr + (19 + copy_iterator));

          *(char *)((*pkt_head_ptr).data_ptr + (19 + copy_iterator)) = 
                                                   temp_copy[copy_iterator];
        }

        /*-------------------------------------------------------------------
          Calculate CRC
        -------------------------------------------------------------------*/
        *(char *)((*pkt_head_ptr).data_ptr + (25)) += 0x8;

        pkt_len = pkt_head_ptr->used;
        if (*(char *)((*pkt_head_ptr).data_ptr + (25)) == 0x7E)
        {
          *(char *)((*pkt_head_ptr).data_ptr + (25)) = 0x5E;

          buf_write[0] = 0x7D;
          dsm_pullup(&pkt_head_ptr, buf_cache, 25);
          dsm_pushdown(&pkt_head_ptr, buf_write, 1, DSM_DS_SMALL_ITEM_POOL);
          dsm_pushdown(&pkt_head_ptr, buf_cache, 25, DSM_DS_SMALL_ITEM_POOL);
          pkt_len++;
        }

        /*-------------------------------------------------------------------
          Calculate FCS
        -------------------------------------------------------------------*/
        fcs = cal_fcs((uint8 *)((*pkt_head_ptr).data_ptr + 1), pkt_len - 4);
        *(char *)((*pkt_head_ptr).data_ptr + (pkt_len - 3)) = (uint8)(fcs & 0xff);
        *(char *)((*pkt_head_ptr).data_ptr + (pkt_len - 2)) = (uint8)((fcs >> 8) & 0xff);

        ds707_pkt_classifier_put_on_sn_wmk(&pkt_head_ptr);

        return;
      }
      else
      {
        DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Dropping unhandled packets");
        dsm_free_packet(&pkt_head_ptr);
      }
    }
  }

  return;
}/* ds707_pkt_classifier_route_um_tx_sn_data */

/*===========================================================================
FUNCTION DS707_PKT_CLASSIFIER_CM_CALL_CONNECTED

DESCRIPTION
  Simulate call connected from CM
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pkt_classifier_cm_call_connected
(
  cm_client_id_type  ds3g_cm_client_id,
  cm_call_id_type    call_id
)
{  
  cm_mm_call_info_s_type call_info;
/*- - - - - - - -  - - - - - - - - - - - - - - - -  - - - - - - - - - - -  */
 
  if (ds707_is_pkt_classifier_enabled())
  {
    call_info.call_client_id = ds3g_cm_client_id;
    call_info.call_id = call_id;
    call_info.call_state = CM_CALL_STATE_IDLE;
    call_info.call_type = CM_CALL_TYPE_PS_DATA;
    call_info.sys_mode = ds707_pkt_classifier_sys_mode;
  
    if ( ds707_pkt_classifier_sys_mode == SYS_SYS_MODE_HDR )
    {
      call_info.mode_info.info.cdma_call.srv_opt = CAI_SO_HDR_PKT_DATA;
    }
    else
    {
      call_info.mode_info.info.cdma_call.srv_opt = PS_IFACE_CDMA_1X_IS2000;
    }
  
    DATA_IS707_MSG2(MSG_LEGACY_HIGH, 
                    "Send call connected with so %d, sys_mode %d",
                    call_info.mode_info.info.cdma_call.srv_opt,
                    ds707_pkt_classifier_sys_mode);
  
    ds707_pkti_sys_chng_hdlr_ex(ds707_pkt_classifier_sys_mode);
  
    ds3gi_cm_call_event_cb(CM_CALL_EVENT_CONNECT, &call_info);  
  
    if (ds707_pkt_classifier_sys_mode == SYS_SYS_MODE_CDMA)
    {
      DATA_IS707_MSG0( MSG_LEGACY_HIGH, 
                       "starting AN PPP for packet classifier IPCP" );
      ds_hdr_an_ppp_start_pkt_classifier();
    }
  }

  return;
}/* ds707_pkt_classifier_cm_call_connected */

/*===========================================================================
FUNCTION DS707_PKT_CLASSIFIER_IS_EHRPD_MODE

DESCRIPTION
  Return if packet classifier is being tested under eHRPD mode
 
DEPENDENCIES
  None

RETURN VALUE
  TRUE  - if eHRPD mode is being tested
  FALSE - otherwise

SIDE EFFECTS
  None
===========================================================================*/
boolean ds707_pkt_classifier_is_ehrpd_mode
(
  void 
)
{
  ps_iface_bearer_technology_type   bearer_tech;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (ds707_is_pkt_classifier_enabled())
  {
    return (ds707_pkt_classifier_sys_mode == SYS_SYS_MODE_HDR);
  }
  else
  {
    memset( &bearer_tech, 
            0, 
            sizeof(ps_iface_bearer_technology_type));

    (void) ds707_pkt_mgr_get_bearer_tech_info(&bearer_tech);
    if(( (bearer_tech.data.cdma_type.rat_mask == PS_IFACE_CDMA_EVDO_REVA) && 
         (bearer_tech.data.cdma_type.so_mask == PS_IFACE_CDMA_EVDO_EMPA_EHRPD) ) || 
       ( (bearer_tech.data.cdma_type.rat_mask == PS_IFACE_CDMA_EVDO_REVB) && 
         (bearer_tech.data.cdma_type.so_mask == PS_IFACE_CDMA_EVDO_MMPA_EHRPD) )  )
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
}/* ds707_pkt_classifier_is_ehrpd_mode */

/*===========================================================================
FUNCTION DS707_PKT_CLASSIFIER_INIT

DESCRIPTION
  This function is called during DS task boot up to read NVs to determine
  if packet classifier testing is enabled, and the RAT being tested on
 
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_pkt_classifier_init
(
  void
)
{
  char efs_file_name[29] = "/eHRPD/packet_classifier.txt";
  ds3gcfgmgr_efs_token_type  efs_db;
  ds_dsd_event_info_type event_info = {0};
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(ds3gcfgmgr_efs_file_init(efs_file_name, &efs_db) == -1)
  {
    DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Packet classifier not enabled");
    return;
  }
  else
  {
    ds707_pkt_classifier_enabled = TRUE;
    ds707_pkt_classifier_read_ip_from_efs(&efs_db);
    ds3gcfgmgr_efs_file_close(&efs_db);
  }

  if( NV_MODE_HDR_ONLY == ds707_get_curr_mode_pref() )
  {
    ds707_pkt_classifier_sys_mode = SYS_SYS_MODE_HDR;

    event_info.event = DS_DSD_CLIENT_RADIO_ACQUIRED_EV;
    event_info.subs_id = 1;
    event_info.data.cdma_p_rev_ind.rat_mask = PS_IFACE_CDMA_EVDO_REVA;
    event_info.data.cdma_p_rev_ind.so_mask = PS_IFACE_CDMA_EVDO_EMPA_EHRPD;

    ds_dsd_ext_event_notify(DS_DSD_RADIO_HDR, &event_info);

    ds707_set_last_good_sys(DS707_SYS_INFO_EPC_SYSTEM);
  }

  return;
}/* ds707_pkt_classifier_init */

#if 0 /* Not used for now */
//server
void ds707_pkt_classifier_reset_server_ready(void)
{
  DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Server Reset");
  server_ready_to_send = 0;
}

void ds707_pkt_classifier_set_server_data_from_app(void)
{
 // server_ready_to_send = server_ready_to_send | 0x01;
  DATA_IS707_MSG1(MSG_LEGACY_HIGH,"Server Data from APPS 0x%x",server_ready_to_send);
}

void ds707_pkt_classifier_set_server_data_from_client(void)
{
 // server_ready_to_send = server_ready_to_send | 0x02;
  DATA_IS707_MSG1(MSG_LEGACY_HIGH,"Server Data from Client 0x%x",server_ready_to_send);
}

boolean ds707_pkt_classifier_ready_to_send_data_from_server(void)
{
  return TRUE; //hack

  if (server_ready_to_send == 3) 
  {
	DATA_IS707_MSG1(MSG_LEGACY_HIGH,"Server Reday to send data 0x%x",server_ready_to_send);
	return TRUE;
  }
  else
  {
	DATA_IS707_MSG1(MSG_LEGACY_HIGH,"Server Not Reday to send data 0x%x",server_ready_to_send);
	return FALSE;
  }
}

//client
void ds707_pkt_classifier_reset_client_ready(void)
{
  DATA_IS707_MSG0(MSG_LEGACY_HIGH,"Client Reset");
  client_ready_to_send = 0;
}

void ds707_pkt_classifier_set_client_data_from_app(void)
{
  //client_ready_to_send = client_ready_to_send | 0x01;
  DATA_IS707_MSG1(MSG_LEGACY_HIGH,"Client Data from APPS 0x%x",client_ready_to_send);
}

void ds707_pkt_classifier_set_client_data_from_server(void)
{
 // client_ready_to_send = client_ready_to_send | 0x02;
  DATA_IS707_MSG1(MSG_LEGACY_HIGH,"Client Data from Server 0x%x",client_ready_to_send);

}

boolean ds707_pkt_classifier_ready_to_send_data_from_client(void)
{
  return TRUE;
  if (client_ready_to_send == 3) 
  {
	DATA_IS707_MSG1(MSG_LEGACY_HIGH,"Client Reday to send data 0x%x",client_ready_to_send);
	return TRUE;
  }
  else
  {
	DATA_IS707_MSG1(MSG_LEGACY_HIGH,"Client not ready yet 0x%x",client_ready_to_send);
	return FALSE;
  }
}
#endif /* if 0 */

#endif/* FEATURE_DATA_PKT_CLASSIFIER */
