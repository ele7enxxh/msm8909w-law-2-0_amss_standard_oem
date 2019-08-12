#ifndef PS_STAT_LOGGING_H
#define PS_STAT_LOGGING_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   P S _ S T A T _ L O G G I N G . H


GENERAL DESCRIPTION
  This file implements an API to log PS stats. Currently IP stats are logged

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ps_stat_logging_init() must be called

Copyright (c) 2005-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ps_stat_logging.h#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/04/11    ttv    Fixed the issue of incorrect display of ppp stats in QXDM.
08/17/09    pp     Defined reserved2 flags to make stat structures 8-byte
                   aligned on Q6.
09/04/08    am     Lint fixes.
12/05/05    msr    Created file
===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "customer.h"
#include "comdef.h"



#include "log.h"
#include "log_codes.h"


/*===========================================================================

                       CALLBACK FUNCTION DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------------*/
  /**
    @brief Function pointer prototype for instance fill description functions

    Defines for the fill description callbacks. There is a different callback
    function that is registered for instance type.  The callback functions
    are defined in the instance-specific files (i.e. ps_stat_iface.c), and 
    when the module is initialized it registers the callback with the stats
    logging module.

    This function creates a log packet for the specified instance type, gets
    descriptions for the instances specified, the dispatches the log packet.

    @param[in]  handle  Handle to the instance that the description is 
                        being copied from

    @retval    None
  */
/*-------------------------------------------------------------------------*/
typedef void (*ps_stat_inst_fill_desc_f_ptr_type)
(
  int32       handle
);

/*-------------------------------------------------------------------------*/
  /**
    @brief Function pointer prototype for function to get module statistics

    Defines for the get statistics callbacks. There is a different callback
    function that is registered for instance type.  The callback functions
    are defined in the instance-specific files (i.e. ps_stat_iface.c), and 
    when the module is initialized it registers the callback with the stats
    logging module.

    This function creates a log packet for the specified instance type, 
    gets statistics for that instance, then dispatches the log packet.

    @param[in]  handle  Handle to the instance that the statistics are being
                        copied from.  If registered as a global statistics
                        function, any value can be specified.

    @retval     None
  */
/*-------------------------------------------------------------------------*/
typedef void (*ps_stat_inst_get_stat_f_ptr_type)
(
  int32       handle
);

/*-------------------------------------------------------------------------*/
  /**
    @brief Function pointer prototype for function to reset module statistics

    Defines for the reset statistics callbacks. There is a different callback
    function that is registered for instance type.  The callback functions
    are defined in the instance-specific files (i.e. ps_iface.c), and when the
    module is initialized it registers the callback with the stats module.

    If the instance stats have been queried before, this function
    creates a log packet for the specified instance type,  gets statistics
    for that instance, then dispatches the log packet.  In all cases, the
    function then resets the statistics in the specified instance cb.

    @param[in]  handle  Handle to the instance that the statistics will be
                        reset on.

    @retval   None
  */
/*-------------------------------------------------------------------------*/
typedef void (*ps_stat_inst_reset_stat_f_ptr_type)
(
  int32       handle
);


/*===========================================================================

                           LOCAL DATA DECLARATIONS

===========================================================================*/
  /**
    @brief   Version of the log packet structure.  This field needs to be
             incremented every time an instance description or the statistics
             being maintained on an instance changes.
  */
#define PS_STAT_GLOBAL_TCP_LOG_PKT_VER     1
#define PS_STAT_GLOBAL_UDP_LOG_PKT_VER     1
#define PS_STAT_GLOBAL_IPV4_LOG_PKT_VER    1
#define PS_STAT_GLOBAL_IPV6_LOG_PKT_VER    1
#define PS_STAT_GLOBAL_ICMPV4_LOG_PKT_VER  1
#define PS_STAT_GLOBAL_ICMPV6_LOG_PKT_VER  1

#define PS_STAT_INST_TCP_LOG_PKT_VER       1
#define PS_STAT_INST_UDP_LOG_PKT_VER       1
#define PS_STAT_INST_PPP_LOG_PKT_VER       1
#define PS_STAT_INST_IFACE_LOG_PKT_VER     1
#define PS_STAT_INST_MEM_LOG_PKT_VER       1
#define PS_STAT_INST_FLOW_LOG_PKT_VER      1
#define PS_STAT_INST_PHYS_LINK_LOG_PKT_VER 1
#define PS_STAT_INST_ARP_LOG_PKT_VER       1
#define PS_STAT_INST_LLC_LOG_PKT_VER       1
#define PS_STAT_INST_IPHC_LOG_PKT_VER      1
#define PS_STAT_INST_ROHC_LOG_PKT_VER      1
#define PS_STAT_INST_RSVP_LOG_PKT_VER      1

#define PS_STAT_DESC_ALL_TCP_INST_LOG_PKT_VER              1
#define PS_STAT_DESC_ALL_UDP_INST_LOG_PKT_VER              1
#define PS_STAT_DESC_ALL_MEM_POOL_INST_LOG_PKT_VER         1
#define PS_STAT_DESC_ALL_IFACE_INST_LOG_PKT_VER            1
#define PS_STAT_DESC_ALL_FLOW_INST_LOG_PKT_VER             1
#define PS_STAT_DESC_ALL_PHYS_LINK_INST_LOG_PKT_VER        1
#define PS_STAT_DESC_ALL_PPP_INST_LOG_PKT_VER              1
#define PS_STAT_DESC_ALL_ARP_INST_LOG_PKT_VER              1
#define PS_STAT_DESC_ALL_LLC_INST_LOG_PKT_VER              1
#define PS_STAT_DESC_ALL_RSVP_INST_LOG_PKT_VER             1
#define PS_STAT_DESC_DELTA_INST_LOG_PKT_VER                1

  /**
    @brief Flag used to indicate if the instance has been queried by
           QXDM previously
  */
#define PS_STAT_QUERIED                    0x01

  /**
    @brief Handle used when all of the instances or statistics on an
           instance are being requested
  */
#define PS_STAT_INST_DESC_ALL              (int32)0xFFFFFFFF

  /**
    @brief  Maximum number of bytes in a log packet
  */
#define PS_STAT_MAX_LOG_PKT_SIZE 500

  /**
    @brief  Type for the handle used to identify an instance
  */
typedef int32 ps_stat_inst_handle_type;

  /**
    @brief  This describes all of the different instance types that can 
            be queried for descriptions or statistics.
  */
typedef enum
{
  PS_STAT_MODULE_MIN             = 0,
  PS_STAT_MODULE_INST_TCP        = PS_STAT_MODULE_MIN,
  PS_STAT_MODULE_INST_UDP        = 1,
  PS_STAT_MODULE_INST_MEM        = 2,
  PS_STAT_MODULE_INST_IFACE      = 3,
  PS_STAT_MODULE_INST_FLOW       = 4,
  PS_STAT_MODULE_INST_PHYS_LINK  = 5,
  PS_STAT_MODULE_INST_PPP        = 6,
  PS_STAT_MODULE_INST_ARP        = 7,
  PS_STAT_MODULE_INST_LLC        = 8,
  PS_STAT_MODULE_INST_RSVP       = 9,
  PS_STAT_MODULE_INST_IPHC       = 10,
  PS_STAT_MODULE_INST_ROHC       = 11,
  PS_STAT_MODULE_GLOBAL_TCP      = 12,
  PS_STAT_MODULE_GLOBAL_UDP      = 13,
  PS_STAT_MODULE_GLOBAL_ICMPV4   = 14,
  PS_STAT_MODULE_GLOBAL_ICMPV6   = 15,
  PS_STAT_MODULE_GLOBAL_IPV4     = 16,
  PS_STAT_MODULE_GLOBAL_IPV6     = 17,
  PS_STAT_MODULE_MAX
} ps_stat_module_enum_type;

/*---------------------------------------------------------------------------
DESCRIPTION
  These specify which sort of instance description packet have been
  requested by QXDM.  Bitmask to allow multiple description requests within
  one packet from QXDM.
---------------------------------------------------------------------------*/
#define PS_STAT_MODULE_DESC_ALL_TCP_INST       (1 << PS_STAT_MODULE_INST_TCP)
#define PS_STAT_MODULE_DESC_ALL_UDP_INST       (1 << PS_STAT_MODULE_INST_UDP)
#define PS_STAT_MODULE_DESC_ALL_MEM_INST       (1 << PS_STAT_MODULE_INST_MEM)
#define PS_STAT_MODULE_DESC_ALL_IFACE_INST     (1 << PS_STAT_MODULE_INST_IFACE)
#define PS_STAT_MODULE_DESC_ALL_FLOW_INST      (1 << PS_STAT_MODULE_INST_FLOW)
#define PS_STAT_MODULE_DESC_ALL_PHYS_LINK_INST \
                                          (1 << PS_STAT_MODULE_INST_PHYS_LINK)
#define PS_STAT_MODULE_DESC_ALL_PPP_INST       (1 << PS_STAT_MODULE_INST_PPP)
#define PS_STAT_MODULE_DESC_ALL_ARP_INST       (1 << PS_STAT_MODULE_INST_ARP)
#define PS_STAT_MODULE_DESC_ALL_LLC_INST       (1 << PS_STAT_MODULE_INST_LLC)
#define PS_STAT_MODULE_DESC_ALL_RSVP_INST      (1 << PS_STAT_MODULE_INST_RSVP)
#define PS_STAT_MODULE_DESC_ALL_IPHC_INST      (1 << PS_STAT_MODULE_INST_IPHC)
#define PS_STAT_MODULE_DESC_ALL_ROHC_INST      (1 << PS_STAT_MODULE_INST_ROHC)

  /**
    @brief  Describes how the instance is being modified
  */
typedef enum
{
  PS_STAT_INST_UPDATE_MIN    = 0,
  PS_STAT_INST_CREATED       = PS_STAT_INST_UPDATE_MIN,
  PS_STAT_INST_MODIFIED      = 1,
  PS_STAT_INST_DELETED       = 2,
  PS_STAT_INST_UPDATE_MAX
} ps_stat_inst_update_enum_type;


/*===========================================================================

                          LOG PACKET DECLARATIONS

===========================================================================*/

/*-------------------------------------------------------------------------*/
  /**
    @brief Log packet header structure for all log packets relating to 
           statistics logging

    @param    hdr        Diag header (DONT MESS WITH THIS FIELD)
    @param    version    Version of this log pkt's definition
    @param    proc_id    Processor that these instances are active on
    @param    reserved   Defined so as to make structure word aligned
                         DONT CHANGE THIS as Diag requires log structures to
                         be 4-byte aligned
  */
/*-------------------------------------------------------------------------*/
typedef PACKED struct
{
  log_hdr_type          hdr;
  uint8                 version;
  uint8                 proc_id;
  uint8                 reserved[2];
} ps_stat_logging_hdr_log_pkt_type;

/*-------------------------------------------------------------------------*/
  /**
    @brief Log header structure for log packets containing instance
           descriptions

           This header is separate from the global statistics logging packet
           because the global header is not accessible by the client functions
           that fill instance descriptions, so it needs to be separate

    @param    count      Number of instances in this packet
    @param    reserved   Defined so as to make structure word aligned
                         DONT CHANGE THIS as Diag requires log structures to
                         be 4-byte aligned
              reserved2  Defined for Q6 to make this 8-byte aligned to avoid
                         mem align issues.
  */
/*-------------------------------------------------------------------------*/
typedef struct
{
  uint8                 count;
  uint8                 reserved[3];
#if defined(__qdsp6__)
  uint32                reserved2;
#endif
} ps_stat_desc_all_inst_hdr_log_pkt_type;

/*-------------------------------------------------------------------------*/
  /**
    @brief Log header structure for delta log packets

    @param    type       Type of instance that is being changed
    @param    deleted    TRUE if instance being removed, FALSE else
    @param    reserved   Defined so as to make structure word aligned
                         DONT CHANGE THIS as Diag requires log structures to
                         be 4-byte aligned
  */
/*-------------------------------------------------------------------------*/
typedef struct
{
  uint8                 type;
  uint8                 deleted;
  uint8                 reserved[2];
#if defined(__qdsp6__)
  uint32                reserved2;
#endif
} ps_stat_desc_delta_inst_hdr_log_pkt_type;

/*-------------------------------------------------------------------------*/
  /**
    @brief Type/handle pair used when passing subsys cmds from diag to PS
    
    The structure describes the parameters that are passed from the diag
    context to the ps context using the ps command to get all of the stats
    associated with the specified handle, or to reset an intstance statistic.

    @param    type       Type of instance
    @param    handle     Handle to the instance
  */
/*-------------------------------------------------------------------------*/
struct ps_stat_logging_stat_cmd_type
{
  ps_stat_module_enum_type                  type;
  ps_stat_inst_handle_type                  handle;
  struct ps_stat_logging_stat_cmd_type    * next_ptr;
};

typedef struct ps_stat_logging_stat_cmd_type ps_stat_logging_stat_cmd_type;

/*-------------------------------------------------------------------------*/
  /**
    @brief Type representing the instance types to get desc log pkts from

    The structure describing the parameters that are passed from the diag
    context to the ps context using the ps command to get all of the instance
   descriptions.
  */
/*-------------------------------------------------------------------------*/
typedef uint64 ps_stat_logging_desc_cmd_type;


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/
/*=========================================================================*/
  /**
    @brief Initializes PS stats logging module

    @depend     None

    @retval     None
  */
/*=========================================================================*/
void ps_stat_logging_init
(
  void
);


/*=========================================================================*/
  /**
    @brief Registers the specified callback function to fill descriptions

    This function registers the module-specific fill description functions 
    with the stats module.

    @depend     None

    @param      func_type   Type of the function pointer being registered.
    @param      *func_ptr   Function pointer that is being registered.

    @retval     None
  */
/*===========================================================================*/
void ps_stat_logging_inst_reg_fill_desc_cback
(
  ps_stat_module_enum_type            func_type,
  ps_stat_inst_fill_desc_f_ptr_type   func_ptr
);


/*=========================================================================*/
  /**
    @brief Registers the specified callback function to get statistics
  
      This function is used to register the module-specific get statistics
      functions with the stats module.

    @depend     None

    @param      func_type   Type of the function pointer being registered.
    @param      *func_ptr   Function pointer that is being registered.

    @retval     None
  */
/*=========================================================================*/
void ps_stat_logging_inst_reg_get_stat_cback
(
  ps_stat_module_enum_type           func_type,
  ps_stat_inst_get_stat_f_ptr_type   func_ptr
);


/*=========================================================================*/
  /**
    @brief Registers the specified callback function to reset statistics
  
      This function is used to register the module-specific reset statistics
      functions with the stats module.

    @depend     None

    @param      func_type   Type of the function pointer being registered.
    @param      *func_ptr   Function pointer that is being registered.

    @retval     None
  */
/*=========================================================================*/
void ps_stat_logging_inst_reg_reset_stat_cback
(
  ps_stat_module_enum_type             func_type,
  ps_stat_inst_reset_stat_f_ptr_type   func_ptr
);


/*=========================================================================*/
  /**
    @brief Creates a delta log packet

      This function creates a delta log packet for the selected instance,
      fills in the description, and dispatches the log packet to QXDM.

      It is called by all of the instance creation, modification, and
      removal functions.

    @param[in]  inst_type    The type of instance that has changed.  Needed to 
                             select the proper callback function.
    @param[in]  inst_handle  Handle to the instance that is being changed.
    @param[in]  inst_update  What is changing about the instance? Is it being 
                             added, updating a description parameter, is it
                             being removed?

    @retval     None

    @depend     The callbacks to retrieve the instance length and description
                from the specified instance type must have already been
                registered with the statistics module.

    @seffect    A log packet is dispatched to the diagnostic channel.
  */
/*=========================================================================*/
void ps_stat_logging_inst_update_desc
(
  ps_stat_module_enum_type       inst_type,
  ps_stat_inst_handle_type       inst_handle,
  ps_stat_inst_update_enum_type  inst_update
);


/*=========================================================================*/
  /**
    @brief Statistics logging wrapper around the diag log_alloc function

      This function is used by statistics logging clients to allocate a log
      packet.  It handles filling in the common header information (such
      as packet version, processor ID, etc.  Provided to avoid the need
      to have all clients knowing about the diag functions

    @param[in]  pkt_size      How much memory is required
    @param[in]  log_pkt_type  Log code of the packet to allocate
    @param[out] log_pkt_ptr   Pointer to memory allocated.  NULL if allocation
                              failed.  Double ptr so we can return the pointer
                              to the allocated memory.

    @retval  None

    @depend  None
  */
/*=========================================================================*/
void ps_stat_logging_get_log_pkt
(
  int32     pkt_size,
  uint16    log_pkt_type,
  uint8  ** log_pkt_ptr
);


/*=========================================================================*/
  /**
    @brief Statistics logging wrapper around the diag log_commit function

      This function is used by statistics logging clients to commit a 
      prevously allocated log packet to the diag channel.  The function
      automatically shortens the packet to the appropriate size, in order to
      prevent us from committing unused bytes.
      
      THIS ONLY WORKS ON LOG PACKETS ALLOCATED BY PS_STAT_GET_LOG_PKT

    @param[in]  log_pkt_ptr           Pointer to log packet
    @param[in]  log_pkt_payload_size  Number of bytes in the payload
  
  
    @retval     None

    @depend     The log packet being commited MUST have been allocated by
                ps_stat_logging_get_log_pkt.

    @seffect    A log packet is dispatched
  */
/*=========================================================================*/
void ps_stat_logging_commit_log_pkt
(
  uint8    * log_pkt_ptr,
  int32      log_pkt_payload_size
);


#endif /* PS_STAT_LOGGING_H */
