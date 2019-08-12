#ifndef PS_LAN_LLC_H
#define PS_LAN_LLC_H
/*===========================================================================
                               P S _ LAN _ LLC . H
===========================================================================*/

/**
  @file ps_lan_llc.h
  @brief 
  Header file for the PS 802 LLC protocol suite Interfacing functions.
 */
 
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the linkLayer group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE
               
Derived from Jeff Dyck's original Ethernet implementation

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_lan_llc.h#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ 
  $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/09/12    sj     Update lan_lle_enum_type for 8 PDN support
02/03/12    vs     Updated lan_lle_enum_type with LAN_LLE_3GPP_RMSM.
09/20/11    cp     Added the support for Broadcasting GARP in periodic
                   intervals. 
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
08/11/11    cp     Added Doxygen markup and comments.
05/07/11    sd     (Tech Pubs) Edited/added Doxygen markup and comments.
10/07/10    dm     Adding new APIs for SoftAP throughput optimizations
06/17/10    vs     Extra LLC instances for simultaneous IPV6/IPV4 over QMI
08/20/09    pp     WAPI update.
01/31/09    pp     CMI: arp_flush API moved from ps_arp.h
12/19/08    pp     Common Modem Interface: Public/Private API split.
09/12/08    pp     Metainfo optimizations.
===========================================================================*/

/* group: linkLayer */
/*===========================================================================

                                INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "dsm.h"
#include "ps_iface_defs.h"
#include "ps_meta_info.h"

/** @addtogroup constants 
  @{
*/
/*===========================================================================

                             MACROS & DATA DECLARATIONS

===========================================================================*/

/** 
  Standard IEEE MAC address length.
*/
#define LAN_IEEE_MAC_ADDR_LEN        6

/**
  Special IEEE MAC address value.
*/
extern const byte LAN_IEEE_MAC_BROADCAST_ADDR [LAN_IEEE_MAC_ADDR_LEN] ;

/**
  @name Ethernet Frame Payload Protocol
  Indicates which protocol is encapsulated in the payload of an 
  Ethernet frame.
  @{
*/
#define LAN_ETHERTYPE_IPV4   0x0800  /**< IPv4 Ethernet type. */
#define LAN_ETHERTYPE_IPV6   0x86DD  /**< IPv6 Ethernet type. */
#define LAN_ETHERTYPE_ARP    0x0806  /**< ARP Ethernet type. */
#define LAN_ETHERTYPE_802_1X          0x888E
  /**< Port-based network access control. Refer to IEEE 802.1X @xhyperref{S39,[S39]}. */
#define LAN_ETHERTYPE_802_1X_PREAUTH  0x88C7
  /**< Pre-authentication. Refer to IEEE 802.11i @xhyperref{S41,[S41]}.  */
#define LAN_ETHERTYPE_802_1Q_TAG      0x8100
  /**< Customer VLAN tag type. Refer to IEEE 802.1Q @xhyperref{S38,[S38]}. */
#define LAN_ETHERTYPE_WAPI            0x88B4
  /**< WAPI authentication protocol. */
/** @} */ /* end_name Ethernet Frame Payload Protocol */
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/**
  Supported logical link entities. Currently, there is a one-to-one mapping
  between an LLE, a PS_IFACE, and a supported MAC interface.
*/
typedef enum
{
  LAN_LLE_MIN     = -1,    /**< Minimum value of the enumerator. */
  LAN_LLE_RMNET1  =  0,    /**< OSIF WWAN RM MAC instance 1 for IPv4. */
  LAN_LLE_WLAN    =  1,    /**< Wireless local area network medium access
                                control. Refer to IEEE 802.11 
                                @xhyperref{S40,[S40]}. */
  LAN_LLE_RMNET2  =  2,    /**< OSIF WWAN RM instance 2 for IPv4. */
  LAN_LLE_RMNET3  =  3,    /**< OSIF WWAN RM instance 3 for IPv4. */
  LAN_LLE_RMNET4  =  4,    /**< OSIF WWAN RM instance 4 for IPv4. */
  LAN_LLE_RMNET5  =  5,    /**< OSIF WWAN RM instance 5 for IPv4. */
  LAN_LLE_RMNET6  =  6,    /**< OSIF WWAN RM instance 6 for IPv4. */
  LAN_LLE_RMNET7  =  7,    /**< OSIF WWAN RM instance 7 for IPv4. */
  LAN_LLE_RMNET8  =  8,    /**< OSIF WWAN RM instance 8 for IPv4. */
  LAN_LLE_UICC    =  9,    /**< Logical link entities for the UICC iface. */
  LAN_LLE_RMNET1_2  =  10,  /**< OSIF WWAN RM instance 1 for IPv6. */
  LAN_LLE_RMNET2_2  =  11,  /**< OSIF WWAN RM instance 2 for IPv6. */
  LAN_LLE_RMNET3_2  =  12,  /**< OSIF WWAN RM instance 3 for IPv6. */
  LAN_LLE_RMNET4_2  =  13, /**< OSIF WWAN RM instance 4 for IPv6. */
  LAN_LLE_RMNET5_2  =  14, /**< OSIF WWAN RM instance 5 for IPv6. */
  LAN_LLE_RMNET6_2  =  15, /**< OSIF WWAN RM instance 6 for IPv6. */
  LAN_LLE_RMNET7_2  =  16, /**< OSIF WWAN RM instance 7 for IPv6. */
  LAN_LLE_RMNET8_2  =  17, /**< OSIF WWAN RM instance 8 for IPv6. */
  LAN_LLE_3GPP_RMSM =  18, /**< 3GPP Rm instance for DUN IPv6 */
  LAN_LLE_MAX  /**< Maximum value of the enumerator. */
} lan_lle_enum_type;

/**
  LLC operation mode enumeration:\n
  - Mode 0 -- Transparent LLC mode. No LLC/SNAP header is expected within
              the LLC PDU, which is a concatenation of the ethertype and IP 
              packet.\n
              The LLC PDU frame uses the follwoing format:\n
              @verbatim
                  |-----------|-------------------|
                  | Ethertype |   IP Packet Body  |
                  |-----------|-------------------|
              @endverbatim

  - Mode 1 -- Unacknowledged and connectionless LLC mode of the operation 
              where the LLC PDUs are encapsulated in the SNAP header.\n
              The LLC PDU frame uses the follwoing format:\n
              @verbatim
                  |---------------|-----------|------------------|
                  | SNAP header   | Ethertype |  IP Packet Body  |
                  |---------------|-----------|------------------|
              @endverbatim
*/
typedef enum
{
  LAN_LLC_MODE_MIN = -1,  /**< Minimum value for the enumeration. */
  LAN_LLC_MODE_0   =  0,  /**< Transparent LLC mode. */
  LAN_LLC_MODE_1   =  1,  /**< Connectionless LLC mode. */
  LAN_LLC_MODE_MAX  /**< Maximum value for the enumeration. */
} lan_llc_mode_enum_type;

/**
  Rx signal associated with an LLE.
*/
typedef int32 lan_llc_sig;


/**
  Used by the LLC to retrieve data received by the MAC layer.
  Within returned DSM item, the source and destination hardware addresses
  are pushed ahead of the LLC PDU with the destination address first.

  @datatypes
  #ps_meta_info_type_ex

  @vertspace
  @param[in] user_data  Pointer to the user data passed in start_info() by the
                        user while starting the LAN LLC instance.
  @param[out] meta_info Meta information for the received packet to be filled
                        by the MAC layer.

  @return
  The packet received at the MAC layer.

  @dependencies
  None.
*/
typedef dsm_item_type* (*lan_llc_rx_f_ptr_type)
(
  void* user_data, 
  ps_meta_info_type_ex** meta_info
);

/**
  Used by the LLC to request LLC PDU transmission at the MAC layer.
  Within the passed DSM item, the source and destination hardware addresses
  are pushed ahead of the LLC PDU with the destination address first.

  @datatypes
  #dsm_item_type

  @vertspace
  @param[in] pkt        Packet to be sent out by the MAC layer.
  @param[in] user_data  Pointer to the user data passed in start_info() by the
                        user while starting the LAN LLC instance.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*lan_llc_tx_f_ptr_type)
(
  dsm_item_type** pkt,
  void* user_data
);

/**
  Used by the LLC to retrieve QoS related meta information from the PS meta_info and
  ethertype of the user_data packet that contains data belonging to the entity
  registering the callback.

  @datatypes
  #ps_tx_meta_info_type

  @vertspace
  @param[in] ps_mi_ptr  PS specific meta information pointer.
  @param[in] ethertype  Packet Ethernet type.
  @param[in] user_data  User data passed in start_info by the user while
                        starting the LAN LLC instance.

  @return
  QoS meta information for the packet.

  @dependencies
  None.
*/
typedef void* (*lan_llc_get_qos_mi_f_ptr_type)
(
  ps_tx_meta_info_type* ps_mi_ptr,
  uint16             ethertype,
  void*              user_data
);

/**
  Used by the ARP module to determine whether to send a proxy ARP 
  for the specified source/target IP pair.

  @datatypes
  #lan_lle_enum_type  

  @vertspace
  @param[in] instance    LLE instance for which information is required.
  @param[in] src_ip      IP of the peer that is generating the ARP query.
  @param[in] target_ip   IP being queried by the ARP packet.

  @return
  TRUE -- Send a proxy ARP response for the target IP.\n
  FALSE -- Do not send a proxy ARP response for the target IP.

  @dependencies
  None.
*/
typedef boolean (*arp_query_proxy_f_ptr_type)
(
  lan_lle_enum_type instance,
  uint32 src_ip,
  uint32 target_ip
);

/**
  Used by a client of an LLE instance to register for a callback when packets 
  with a particular ethertype are received over the LLE instance.

  @datatypes
  #lan_lle_enum_type\n
  #dsm_item_type

  @vertspace
  @param[in] lle_instance LLE instance over which the packet is received.
  @param[in] pkt          Packet received from the MAC layer.
  @param[in] src_hw_addr  Source hardware address of the received packet.
  @param[in] dst_hw_addr  Destination hardware address of the received packet.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*lan_llc_input_f_ptr_type)
(
  lan_lle_enum_type lle_instance,
  dsm_item_type**   pkt,
  byte*             src_hw_addr,
  byte*             dst_hw_addr
);

/**
  @brief ARP configuration information required while starting a new ARP
  instance.
*/
typedef struct
{
  boolean                      enable_cache_maintenance;  
    /**< Indicates whether to enable the ARP cache refresh. */
  boolean                      enable_proxy;  
    /**< Indicates whether to enable the ARP proxy. */
  arp_query_proxy_f_ptr_type   arp_proxy_query_f_ptr;  
    /**< Used by the ARP module to determine whether to ARP proxy for
         the peer. */
  boolean                      enable_bcast_gratuitous_arp;
    /**< Indicates whether to enable broadcasting of Gratuitous ARP. */
  uint32                       bcast_gratuitous_arp_interval;
    /**< Periodic interval in which Gratuitous ARP are broadcasted. */
} arp_config_type;

/**
  Used by the ICMP6 neighbor discovery to notify of any special events.

  @param[in] event          ND event that occurred.
  @param[in] user_data_ptr  User data provided in the ND configuration
                            information while starting the ICMP6 ND.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*ps_icmp6_nd_cback_f_ptr_type)
(
  uint8                       event,
  void*                       user_data_ptr
);

/**
  @brief ICMP6 neighbor discovery to start the configuration items.
*/
typedef struct
{
  ps_icmp6_nd_cback_f_ptr_type  nd_cback_f_ptr;  
    /**< Callback to be called in case of ND events. */
  void                         *usr_data_ptr;  
    /**< User data to be passed back in the callback. */
} ps_icmp6_nd_config_type;

/**
  @brief LLE start information data structure passed to the LLC by a mode
  controller to start a supported LLE instance. It provides necessary
  information for the LLC to fully operate on this LLE instance.
*/
typedef struct
{
  ps_iface_type*           iface_ptr;  
    /**< Interface for which the LLE instance is to be started. */
  lan_llc_mode_enum_type   lle_mode;  
    /**< Mode of this LLE instance. */
  lan_llc_sig              lle_rx_sig;  
    /**< Rx signal associated with this LLE instance. */
  lan_llc_rx_f_ptr_type    rx_f_ptr;  
    /**< Used by the LLE to retrieve data arrived at the MAC layer. */
  void*                    rx_f_user_data;  
    /**< User data to be passed with rx_f_ptr. */
  lan_llc_tx_f_ptr_type    tx_f_ptr;  
    /**< Used by the LLE to request PDU transmission at the MAC layer. */
  void*                    tx_f_user_data;  
    /**< Used by the LLC to retrieve a QoS related meta_info from the 
         PS meta_information. */
  lan_llc_get_qos_mi_f_ptr_type   get_qos_mi_f_ptr;  
    /**< Used by the LLE to retrieve QoS related meta information. */
  void*                           get_qos_mi_f_user_data;  
    /**< User data to be passed with get_qos_mi_f_ptr. */
  arp_config_type          arp_config;  
    /**< Configuration information to be passed while starting an ARP
         instance. */
  ps_icmp6_nd_config_type  nd_config;  
    /**< ICMP6 neighbor discovery start configuration items. */
} lan_llc_start_info_type;

/**
  @brief LLE configuration buffer that corresponds to an LLC instance
  configuration information.
*/
typedef struct
{
  lan_lle_enum_type        lle_instance;  /**< LLE instance. */
  lan_llc_start_info_type  start_info;  
    /**< Configuration information for this instance. */
} lan_lle_config_type;

/** @} */ /* end_addtogroup datatypes */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION LAN_LLC_START()
===========================================================================*/
/** @ingroup lan_llc_start
  Posts a start command to the PS task for a particular LLE instance. It is
  called by the specific mode controller.

  @datatypes
  #lan_lle_enum_type\n
  #lan_llc_start_info_type

  @vertspace
  @param[in] lle_instance   LLE instance to be started.
  @param[in] config_ptr     Pointer to the configuration information for the 
                            LLE to start. This memory belongs to the caller and
                            is not freed by the PS LAN LLC.

  @return
  0  -- LLC start command is posted successfully.\n
  -1 -- Error occurred.

  @dependencies
  None.
  @newpage
*/
int lan_llc_start
(
  lan_lle_enum_type        lle_instance,
  lan_llc_start_info_type* config_ptr
);

/*===========================================================================
FUNCTION LAN_LLC_RECONFIG()
===========================================================================*/
/** @ingroup lan_llc_reconfig
  Changes the configuration of an LAN LLC instance after it was started.

  @datatypes
  #lan_lle_enum_type\n
  #lan_llc_start_info_type

  @vertspace
  @param[in] lle_instance  LLE instance to be reconfigured.
  @param[in] config_ptr    Pointer to the new configuration for the LLE instance.

  @return
   0 -- LLC reconfiguration command posted successfully.\n
  -1 -- Error occurred.

  @dependencies
  None.
  @newpage
*/
int lan_llc_reconfig
(
  lan_lle_enum_type        lle_instance,
  lan_llc_start_info_type* config_ptr
);

/*===========================================================================
FUNCTION LAN_LLC_STOP()
===========================================================================*/
/** @ingroup lan_llc_stop
  Posts a stop command to the PS task for a particular LLE. It is called by the
  specific mode controller that is not running in PS task context.

  @datatypes
  #lan_lle_enum_type

  @vertspace
  @param[in] lle_instance  LLE instance to be stopped.

  @return
   0 -- LLC stop command posted successfully.\n
  -1 -- Error occurred.

  @dependencies
  None.
  @newpage
*/
int lan_llc_stop
(
  lan_lle_enum_type  lle_instance
);


/*===========================================================================
FUNCTION LAN_LLC_SET_INPUT_HANDLER()
===========================================================================*/
/** @ingroup lan_llc_set_input_handler
  Registers with a particular LLE instance the handler for
  packets of a particular ethertype. 

  @note1hang The LLC natively supports the handler for IP and ARP packets. 
  The handlers do not have to be registered explicitly.

  @datatypes
  #lan_lle_enum_type\n
  #lan_llc_input_f_ptr_type

  @vertspace
  @param[in] lle_instance     LLE instance for which the input handler is to be 
                              set.
  @param[in] ethertype        Ethernet type of the packets.
  @param[in] lle_input_f_ptr  Handler to be set.

  @return
   0 -- Handler registered successfully.\n
  -1 -- Error occurred.

  @dependencies
  None.
  @newpage
*/
int lan_llc_set_input_handler
(
  lan_lle_enum_type        lle_instance,
  uint16                   ethertype,
  lan_llc_input_f_ptr_type lle_input_f_ptr
);


/*===========================================================================
FUNCTION LAN_LLC_IS_STARTED()
===========================================================================*/
/** @ingroup lan_llc_is_started
  Determines whether a paticular LLC instance has been started.

  @datatypes
  #lan_lle_enum_type

  @vertspace
  @param[in] lle_instance  LLE instance to be queried.

  @return
  TRUE  -- Instance has been started.\n
  FALSE -- Instance is stopped.

  @dependencies
  None.
  @newpage
*/
boolean lan_llc_is_started
(
  lan_lle_enum_type        lle_instance
);

/*===========================================================================
FUNCTION LAN_LLC_SEND_PKT()
===========================================================================*/
/** @ingroup lan_llc_send_pkt
  Sends an IP packet without address resolution (i.e., broadcast IP packets), 
  or sends either the queued IP or ARP packets using the ARP module. As such,
  this function performs the necessary LLC/SNAP encapulation based on the LLE
  mode of operation. All pointers supplied as function arguments belong to
  the caller and are not freed by this function.

  @datatypes
  #lan_lle_enum_type\n
  #dsm_item_type

  @vertspace
  @param[in] lle_instance   LLE instance transmitting this packet.
  @param[in] pkt_chain_ptr  Pointer to the chain of DMS items that hold the 
                            payload of the packet that is to be transmitted.
  @param[in] ethertype      Packet for the Ethernet type.
  @param[in] dst_hw_addr    Pointer to the destination hardware address.

  @return
   0 -- IP or ARP packet sent successfully.\n
  -1 -- Error occurred.

  @dependencies
  None.
  @newpage
*/
int lan_llc_send_pkt
(
  lan_lle_enum_type lle_instance,
  dsm_item_type**   pkt_chain_ptr,
  uint16            ethertype,
  byte*             dst_hw_addr
);

/*===========================================================================
FUNCTION LAN_LLC_RX_PKT()
===========================================================================*/
/** @ingroup lan_llc_rx_pkt
  Processes the received LLC PDU. LLC/SNAP header is removed (if applicable)
  and packets are processed and dispacthed to either IP stack or ARP module.

  @datatypes
  #lan_lle_enum_type\n
  #dsm_item_type\n
  #ps_meta_info_type_ex

  @vertspace
  @param[in] lle_instance     LLE instance receiving this packet.
  @param[in] rx_pkt_ptr       Pointer to the chain of DMS items that hold the 
                              payload of the packet that is received.
  @param[in] meta_info_ex_ptr Meta info for the received packet.

  @return
   0 -- IP or ARP packet received successfully.\n
  -1 -- Error occurred.

  @dependencies
  None.
  @newpage
*/
int lan_llc_rx_pkt
(
  lan_lle_enum_type      lle_instance,
  dsm_item_type*         rx_pkt_ptr,
  ps_meta_info_type_ex  *meta_info_ex_ptr
);

/*===========================================================================
FUNCTION ARP_FLUSH()
===========================================================================*/
/** @ingroup arp_flush
  Flushes all ARP entries related to a particular LLE. The caller may be the
  ARP module itself or the mode controller because it has determined the need 
  to flush the ARP cache.

  @datatypes
  #lan_lle_enum_type

  @vertspace
  @param[in] lle_instance LLE instance for which entries are to be flushed.

  @return
   0 -- ARP cache was flushed.\n
  -1 -- Error occurred.

  @dependencies
  None.
  @newpage
*/
int arp_flush
(
  lan_lle_enum_type lle_instance
);

/*===========================================================================
FUNCTION ARP_START_BCAST_GARP()
===========================================================================*/
/** @ingroup arp_start_bcast_garp
  Starts broadcasting gratuitous ARPs on the interface related to a particular LLE. 
  This function will be called by LLC clients [eg: WLAN].

  @datatypes
  #lan_lle_enum_type\n

  @vertspace
  @params[in] lle_instance LLE instance on which grautious ARPs are broadcasted..

  @return
  0  - on Success
  -1 - on Failure

  @dependencies
  None.
  @newpage
*/
int arp_start_bcast_garp
(
  lan_lle_enum_type lle_instance
);

/*===========================================================================
FUNCTION LAN_LLC_SOFTAP_START()
===========================================================================*/
/** @ingroup lan_llc_softap_start
  Synchronizes a start command for the soft AP in the protocol task for 
  a particular LLE instance. It is called by the specific mode controller.

  @datatypes
  #lan_lle_enum_type\n
  #lan_llc_start_info_type

  @vertspace
  @param[in] lle_instance   LLE instance which is to be started.
  @param[in] config_ptr     Pointer to configuration information for the LLE to
                            start. This memory belongs to the caller and does
                            not be freed by the PS LAN LLC.

  @return
   0 -- LLC start command posted successfully.\n
  -1 -- Error occurred.

  @dependencies
  None.
  @newpage
*/
int lan_llc_softap_start
(
  lan_lle_enum_type        lle_instance,
  lan_llc_start_info_type* config_ptr
);

/*===========================================================================
FUNCTION LAN_LLC_SET_SIGNAL()
===========================================================================*/
/** @ingroup lan_llc_set_signal
  Sets the appropiate protocol task signal for the corresponding LLE instance.

  @datatypes
  #lan_lle_enum_type

  @vertspace
  @param[in] instance   LLE instance to set the signal.

  @return
   0 -- Protocol task signal is set successfully.\n
  -1 -- Error occurred.

  @dependencies
  None.
  @newpage
*/
int lan_llc_set_signal
(
  lan_lle_enum_type instance
);


#endif /* PS_LAN_LLC_H */
