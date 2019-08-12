#ifndef DHCP_SERVER_DEFS_H
#define DHCP_SERVER_DEFS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                           D H C P _ S E R V E R _ D E F S . H

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/**
  @file dhcp_server_defs.h
  @brief Contains the definitions and declarations to interface with the DHCP server.
 */

/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The dms_mainpage.dox file contains all file/group descriptions 
      that are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      dms_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the dhcp group 
      description in the dms_mainpage.dox file. 
=============================================================================*/
/*=============================================================================
Copyright (c) 2010-2011 Qualcomm Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dhcp_server_defs.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/28/11    sd     (Tech Pubs) Applied new Doxygen grouping markup.
08/23/11    ag     Added the support for NBNS option.
01/05/11    cp     Added support for the reservation ip leases.
10/08/10    cp     Added support for getting hostname and connection time 
                   using dhcp_server_get_conn_info() API. Also added 
                   support to make lease time configurable.                   
07/02/10    cp     Moved the declarations and definitions 
                   from dhcp_server_mgr.h to here.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ps_in.h"

/*===========================================================================

                      DEFINITIONS AND DECLARATIONS

===========================================================================*/
/* group: dhcp */

/** @addtogroup datatypes 
  @{
*/

/**
  Supported DHCP message types.
*/
typedef enum { 
  DHCP_MIN_MSG = 0,       /**< Invalid message type. */
  DHCP_DHCPDISCOVER = 1,  /**< DHCP DISCOVER message type. */
  DHCP_DHCPOFFER = 2,     /**< DHCP OFFER Message type. */
  DHCP_DHCPREQUEST = 3,   /**< DHCP REQUEST message type. */
  DHCP_DHCPDECLINE = 4,   /**< DHCP DECLINE message type. */
  DHCP_DHCPACK = 5,       /**< DHCP ACK message type. */
  DHCP_DHCPNACK = 6,      /**< DHCP NACK message type. */
  DHCP_DHCPRELEASE = 7,   /**< DHCP RELEASE message type. */
  DHCP_DHCPINFORM = 8,    /**< DHCP INFORM message type. */
  DHCP_MAX_MSG            /**< Maximum supported message type. */
} dhcp_msg_type;

/**
  Callback invoked on various DHCP messages being received/transmitted.
*/
typedef void (* dhcp_msg_cback_type)
(
  void * userdata,         /**< User data passed in when calling 
						        dhcp_server_mgr_start(). */
  dhcp_msg_type msg_type,  /**< Type of the DHCP message. */
  uint32 ipv4_addr         /**< IPv4 address assigned. */
);
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup constants
  @{
*/
/**
  Maximum client hardware length.
*/
#define DHCP_SERVER_MGR_CLIENT_CHADDR_LEN (16)

/**
  Maximum client hostname length.
*/
#define DHCP_SERVER_MGR_MAX_HOST_NAME_LEN (32)

/**
  @brief Used to get information about the connected devices.
*/
typedef struct dhcp_server_conn_devices_info
{
  /** IP address of the client. */
  struct ps_in_addr client_ip;
  uint8             client_hw[DHCP_SERVER_MGR_CLIENT_CHADDR_LEN];
     /**< Hardware address of the client. */
  uint32            client_hw_len;
     /**< Length of the hardware address. */
  uint8             client_hostname[DHCP_SERVER_MGR_MAX_HOST_NAME_LEN];
     /**< Client hostname. */
  uint32            client_hostname_len;
     /**< Length of the hostname. */
  uint32            client_conn_time;
     /**< Time in seconds when the client connected. Use the time_jul_from_secs 
	      function defined in time_jul.h to convert seconds into Julian format.
          Refer to @xhyperref{Q2,[Q2]}. */
  boolean           is_static;
     /**< Indicates whether the IP address allocation through DHCP is static or dynamic .\n
		  - TRUE -- IP address allocation is static.
		  - FLASE -- IP address allocation is dynamic. */
} dhcp_server_conn_devices_info_s;
/** @} */ /* end_addtogroup datatypes */

/** @addtogroup constants
  @{
*/
/**
  Maximum number of DHCP reservations supported.
*/
#define MAX_NUM_OF_DHCP_RESERVATIONS 5
/** @} */ /* end_addtogroup constants */

/** @addtogroup datatypes
  @{
*/
/**
  @brief Contains the DHCP reservation addresses information.
*/
typedef struct dhcp_reservation
{
  /** IP address of the client. */
  struct ps_in_addr        client_ip;
  uint8                    client_hw[DHCP_SERVER_MGR_CLIENT_CHADDR_LEN];
     /**< Hardware address of the client. */
  uint32                   client_hw_len;
     /**< Length of the hardware address. */
  uint8                    client_hostname[DHCP_SERVER_MGR_MAX_HOST_NAME_LEN];
     /**< Hostname of the client. */
  uint32                   client_hostname_len;
     /**< Length of the hostname. */
}dhcp_reservation_type;

/**
  @brief DHCP configuration parameters.
*/
typedef struct dhcp_server_config_params
{
  /** Starting IP address to be allocated. */
  struct ps_in_addr     ipv4_addr_start;
  /** Last IP address to be allocated. */     
  struct ps_in_addr     ipv4_addr_end;
  /** Gateway/router IP address to be communicated to clients. This is the 
	server ID. */   
  struct ps_in_addr     gateway_addr;
  /** Subnet mask. */   
  struct ps_in_addr     net_mask;
  /** Primary DNS address. */   
  struct ps_in_addr     prim_dns;
  /** Secondary DNS address. */   
  struct ps_in_addr     sec_dns;
  /** Primary NBNS address. */   
  struct ps_in_addr     prim_nbns;
  /** Secondary NBNS address. */ 
  struct ps_in_addr     sec_nbns;
  dhcp_reservation_type dhcp_reservations[MAX_NUM_OF_DHCP_RESERVATIONS];
     /**< DHCP reservations. */
  uint8                 num_of_reservations;
     /**< Number of DHCP reservations. */
  uint32                lease_time;
     /**< Lease time in minutes. */
  boolean               enable_acd;
     /**< Enable address conflict detection. */
} dhcp_server_config_params_s;
/** @} */ /* end_addtogroup datatypes */

#endif /* DHCP_SERVER_DEFS_H */
