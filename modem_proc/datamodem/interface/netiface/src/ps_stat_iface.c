/*===========================================================================

                          P S _ S T A T _ I F A C E . C

GENERAL DESCRIPTION
  This program specifically deals with statistics at the iface layer of the
  data servcies stack

EXTERNALIZED FUNCTIONS
  ps_stat_get_iface
    This function supplies the required iface stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2005-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/netiface/src/ps_stat_iface.c#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/12   pgm     Getting Stats on base iface.
03/09/12   wc      Fix stats reset issue
12/27/10   rp      RAM optimization changes.
12/24/10   ua      Fixed compilation errors.
02/28/08   rr      Added statistics to track the number of bytes tx and rx
04/11/07   scb     Fixed LINT High errors
02/27/07   scb     Added changes to fix RCVT compilation warning
08/15/05   mct     Fixed naming causing some issues w/C++ compilers.
08/15/05   kr      Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"
#include "IxErrno.h"
#include "err.h"

#include "ps_stat_iface.h"
#include "ps_stat_common.h"
#include "ps_stat_logging.h"
#include "ps_iface.h"
#include "ps_utils.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_ifacei.h"

#include <stringl/stringl.h>
#include <stddef.h>
#include <string.h>

/*===========================================================================

                              LOCAL DECLARATIONS

===========================================================================*/

/*===========================================================================
MACRO IFACE_STAT_OFFSET_SIZE(INDEX,FIELD,SIZE)

DESCRIPTION
  This macro fills the offset and size fields of the ps_stat_iface_i_table.
  INDEX used only for readability, not for usage

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define IFACE_STAT_OFFSET_SIZE(INDEX,FIELD,SIZE)                        \
{                                                                       \
  FPOS_STAT(ps_iface_type, FIELD),                                      \
  sizeof(SIZE)                                                          \
}

/*---------------------------------------------------------------------------
  IFACE stats access table
-------------------------------------------------------------------------- */
const ps_stat_access_table_s_type ps_stat_iface_i_table[PS_STAT_IFACE_ALL + 1] =
{
  IFACE_STAT_OFFSET_SIZE(0,iface_i_stats.pkts_rx, uint32),
  IFACE_STAT_OFFSET_SIZE(1,iface_i_stats.mcast_pkts_rx, uint32),
  IFACE_STAT_OFFSET_SIZE(2,iface_i_stats.pkts_dropped_rx, uint32),
  IFACE_STAT_OFFSET_SIZE(3,iface_i_stats.pkts_tx, uint32),
  IFACE_STAT_OFFSET_SIZE(4,iface_i_stats.mcast_pkts_tx, uint32),
  IFACE_STAT_OFFSET_SIZE(5,iface_i_stats.pkts_dropped_tx, uint32),
  IFACE_STAT_OFFSET_SIZE(6,iface_i_stats.bytes_rx, uint64),
  IFACE_STAT_OFFSET_SIZE(7,iface_i_stats.bytes_tx, uint64),
  IFACE_STAT_OFFSET_SIZE(8,iface_private.state, uint16),
  /*---------------------------------------------------------------------------
  Index 9 is unused. Fill with zeros
  - Refer ps_stat_iface_enum_type
  -------------------------------------------------------------------------- */
  STAT_FILL_NULL(9)
};

/*===========================================================================

                             INTERNAL FORWARD DECLARATIONS

===========================================================================*/
INLINE int32 ps_stat_fill_iface_name
(
  ps_iface_type            *iface_ptr,
  uint8                    *log_pkt_ptr,
  int32                     log_pkt_avail_size
);

static errno_enum_type ps_stat_get_iface_instance_all
(
  ps_stat_iface_enum_type  stat,
  ps_iface_type            *iface_ptr,
  void                     *return_value,
  uint16                   ret_len
);

static int32 ps_stat_get_iface_desc_len_default_cback
(
  ps_iface_type         * iface_ptr
);

static void ps_stat_fill_iface_inst_desc_default_cback
(
  ps_iface_type * iface_ptr,
  uint8         * log_pkt_ptr,
  int32           log_pkt_avail_size
);

static void ps_stat_get_iface_desc_log_pkt
(
  int32        handle
);

static void ps_stat_get_iface_stat_log_pkt
(
  int32        handle
);
/*===========================================================================

                              LOCAL FUNCTIONS

===========================================================================*/
/*===========================================================================
FUNCTION PS_STAT_FILL_IFACE_NAME()

DESCRIPTION
  This function fills the passed in iface name into a log packet

DEPENDENCIES
  None

RETURN VALUE
  Number of bytes copied to log packet

PARAMETERS
  iface_ptr         - IN - pointer that points to the IFACE control block
  log_pkt_ptr       - IN - pointer to log packet being filled
  log_pkt_avail_size- IN - remaining size of the log_pkt_ptr

SIDE EFFECTS
  None
===========================================================================*/
//TODO: rename as ps_stati_...
int32 ps_stat_fill_iface_name
(
  ps_iface_type            *iface_ptr,
  uint8                    *log_pkt_ptr,
  int32                     log_pkt_avail_size
)
{
  char         * name_ptr;
  char         * log_pkt_fill_ptr  = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  // TODO: add asserts after msg_errors
  if (FALSE == PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_stat_fill_iface_name(): "
                    "Invalid iface_ptr 0x%p", iface_ptr);
    return 0;
  }

  if (NULL == log_pkt_ptr)
  {
    LOG_MSG_ERROR_0("ps_stat_fill_iface_name(): "
                    "Null log packet");
    return 0;
  }

  log_pkt_fill_ptr = (char *) log_pkt_ptr;

  switch(iface_ptr->name)
  {
    // TODO: use enum values
    case CDMA_SN_IFACE:
      name_ptr = "CDMA_SN";
      break;

    case CDMA_AN_IFACE:
      name_ptr = "CDMA_AN";
      break;

    case UMTS_IFACE:
      name_ptr = "UMTS";
      break;

    case SIO_IFACE:
      name_ptr = "SIO";
      break;

    case CDMA_BCAST_IFACE:
      name_ptr = "CDMA_BCAST";
      break;

    case WLAN_IFACE:
      name_ptr = "WLAN";
      break;

    case DUN_IFACE:
      name_ptr = "DUN";
      break;

    case FLO_IFACE:
      name_ptr = "FLO";
      break;

    case DVBH_IFACE:
      name_ptr = "DVBH";
      break;

    case STA_IFACE:
      name_ptr = "STA";
      break;

    case IPSEC_IFACE:
      name_ptr = "IPSEC";
      break;

    case LO_IFACE:
      name_ptr = "LO";
      break;

    case MBMS_IFACE:
      name_ptr = "MBMS";
      break;

    case IWLAN_3GPP_IFACE:
      name_ptr = "IWLAN_3GPP";
      break;

    case IWLAN_3GPP2_IFACE:
      name_ptr = "IWLAN_3GPP2";
      break;

    case MIP6_IFACE:
      name_ptr = "MIP6";
      break;

    default:
      LOG_MSG_ERROR_0("ps_stat_fill_iface_name(): "
                      "Invalid iface");
      return 0;
  } /* switch */

  return (strlcpy(log_pkt_fill_ptr, name_ptr, log_pkt_avail_size));

} /* ps_stat_fill_iface_name */

/*===========================================================================
FUNCTION PS_STAT_GET_IFACE_INSTANCE_ALL()

DESCRIPTION
  This function supplies all the IFACE statistics for the instance handle
  supplied. This is needed because of the need to extract and format data
  available in the iface control block

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  iface_ptr       - IN - pointer that points to the IFACE control block
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_iface_instance_all
(
  ps_stat_iface_enum_type  stat,
  ps_iface_type            *iface_ptr,
  void                     *return_value,
  uint16                   ret_len
)
{
   ps_stat_iface_i_s_type *iface_i_stats;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    validate 'stat'; only PS_STAT_IFACE_ALL should be passed to this function
  -------------------------------------------------------------------------*/
  if (stat != PS_STAT_IFACE_ALL)
  {
    LOG_MSG_ERROR_1("ps_stat_get_iface_instance_all(): "
                    "Invalid stat %d", stat);
    ASSERT(0);
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    validate iface_ptr : should not be NULL
  -------------------------------------------------------------------------*/
  if (iface_ptr == NULL)
  {
    DATA_ERR_FATAL("ps_stat_get_iface_instance_all(): NULL iface");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    return_value validation : should not be null
  -------------------------------------------------------------------------*/

  if (return_value == NULL)
  {
    LOG_MSG_ERROR_0("ps_stat_get_iface_instance_all(): "
                    "NULL return_value");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    check the sizeof the stat requested. If the size is zero, the stat is
    invalid in the current context. return appropriate error
  -------------------------------------------------------------------------*/

  if (ret_len < sizeof(ps_stat_iface_i_s_type))
  {
    LOG_MSG_ERROR_0("ps_stat_get_iface_instance_all(): "
                    " Insufficient memory allocated for ptr return_value");
    return E_NOT_ALLOWED ;
  }

  iface_i_stats = (ps_stat_iface_i_s_type *) return_value ;

  iface_i_stats->pkts_rx         = iface_ptr->iface_i_stats.pkts_rx ;
  iface_i_stats->mcast_pkts_rx   = iface_ptr->iface_i_stats.mcast_pkts_rx;
  iface_i_stats->pkts_dropped_rx = iface_ptr->iface_i_stats.pkts_dropped_rx;
  iface_i_stats->pkts_tx         = iface_ptr->iface_i_stats.pkts_tx ;
  iface_i_stats->mcast_pkts_tx   = iface_ptr->iface_i_stats.mcast_pkts_tx;
  iface_i_stats->pkts_dropped_tx = iface_ptr->iface_i_stats.pkts_dropped_tx;
  iface_i_stats->bytes_rx        = iface_ptr->iface_i_stats.bytes_rx;
  iface_i_stats->bytes_tx        = iface_ptr->iface_i_stats.bytes_tx;

  iface_i_stats->state           = iface_ptr->iface_private.state ;

  iface_ptr->iface_i_stats.queried = PS_STAT_QUERIED;

  return E_SUCCESS ;
} /* ps_stat_get_iface_instance_all() */

// TODO: make all of the internal functions static

/*===========================================================================
FUNCTION PS_STAT_GET_IFACE_DESC_LEN_DEFAULT_CBACK()

DESCRIPTION
  This function determines the description length of the specified instance
  handle.  If the handle matches the handle for all descriptions, it
  determines the size of all of the active instance descriptions.  If the
  handle does not specify all instances but specifies a valid active instance,
  it determines the length of the specified instance. If the handle is invalid,
  nothing is done.

PARAMETERS
  handle:   Specifies which instance the description length is to be retrieved
            from.
  *len_ptr:  Pointer to the variable that the length should be stored in

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int32 ps_stat_get_iface_desc_len_default_cback
(
  ps_iface_type         * iface_ptr
)
{
  int32                   desc_len      = 0;
  struct ps_mcast_cb    * iface_mcast_grp_ptr;
#ifdef FEATURE_DATA_PS_IPV6
  int32                   i             = 0;
#endif /* FEATURE_DATA_PS_IPV6 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Sanity check the handle value
  -----------------------------------------------------------------------*/
  if (FALSE == PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_stat_get_iface_desc_len_default_cback(): "
                    "Invalid iface cb 0x%p", iface_ptr);
    return desc_len;
  }

  /*-------------------------------------------------------------------------
    If the iface is not up, we do not want to show it on QXDM
  -------------------------------------------------------------------------*/
  if (IFACE_UP != PS_IFACEI_GET_STATE(iface_ptr))
  {
    return desc_len;
  }

  desc_len += sizeof(ps_stat_inst_desc_iface_type);
#ifdef FEATURE_DATA_PS_IPV6
  /*-----------------------------------------------------------------------
    Iterate through the entire array of IPV6 addresses in the iface ctrl blk
  -----------------------------------------------------------------------*/
  for (i = 0; i < MAX_IPV6_ADDRS; ++i)
  {
    if (NULL != iface_ptr->iface_private.ipv6_addrs[i])
    {
      desc_len += sizeof(ps_stat_inst_desc_iface_v6_addr_type);
    }
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  /*-----------------------------------------------------------------------
    Traverse the list of multicast groups to determine the length
  -----------------------------------------------------------------------*/
  iface_mcast_grp_ptr = iface_ptr->iface_private.if_mcast_grps;

  while (NULL != iface_mcast_grp_ptr)
  {
    desc_len += sizeof(struct ps_in6_addr);
    iface_mcast_grp_ptr = iface_mcast_grp_ptr->next_mcast_grp;
  }

  // TODO: Show the desc format similar to header format in RFC

  desc_len += 12; /* TODO: replace with actual length of description */

  return desc_len;

} /* ps_stat_get_iface_desc_len_default_cback() */


/*===========================================================================
FUNCTION PS_STAT_FILL_IFACE_INST_DESC_DEFAULT_CBACK()

DESCRIPTION
  This function fills the description of the specified instance handle into
  a log packet.  If the handle matches the handle for all descriptions, it
  copies all of the active instance descriptions into the log packet.  If the
  handle does not specify all instances but specifies a valid active instance,
  the description associated with that instance is copied into the log packet.
  If the handle is invalid, nothing is done.

PARAMETERS
  handle            : Specifies which instance to retrieve the description from
  log_pkt_ptr       : Pointer to the memory buffer to fill with description
  log_pkt_avail_size: Remaining size of the log_pkt_ptr

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_fill_iface_inst_desc_default_cback
(
  ps_iface_type * iface_ptr,
  uint8         * log_pkt_ptr,
  int32           log_pkt_avail_size
)
{
  uint8                                 * log_pkt_fill_ptr;
  ps_stat_inst_desc_iface_type          * iface_desc_ptr;

  struct ps_mcast_cb                    * iface_mcast_grp_ptr;
  struct ps_in6_addr                      iface_mcast_v6_addr;
#ifdef FEATURE_DATA_PS_IPV6
  ps_stat_inst_desc_iface_v6_addr_type  * iface_v6_desc_ptr;
  int32                                   i                       = 0;
#endif /* FEATURE_DATA_PS_IPV6 */
  uint8                                   mcast_count             = 0;
  uint8                                   ipv6_addr_count         = 0;
  int32                                   iface_name_size         = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (FALSE == PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_stat_fill_iface_inst_desc_default_cback(): "
                    "Invalid iface 0x%p", iface_ptr);
    ASSERT(0);
    return;
  }

  if (NULL == log_pkt_ptr)
  {
    DATA_ERR_FATAL("ps_stat_fill_iface_inst_desc_default_cback(): NULL log pkt");
    return;
  }

  log_pkt_fill_ptr = log_pkt_ptr;

  //TODO: RFC-style log pkt description
  /*-----------------------------------------------------------------------
    Continue past the iface description block
    Fill in iface description at end to include mcast and ip addr counts
  -----------------------------------------------------------------------*/
  log_pkt_fill_ptr += sizeof(ps_stat_inst_desc_iface_type);
  log_pkt_avail_size -= sizeof(ps_stat_inst_desc_iface_type);

#ifdef FEATURE_DATA_PS_IPV6
  /*-----------------------------------------------------------------------
    Populate the IPv6 addresses in the log packet
  -----------------------------------------------------------------------*/
  for (i = 0; i < MAX_IPV6_ADDRS; ++i)
  {
    if (NULL != iface_ptr->iface_private.ipv6_addrs[i])
    {
      iface_v6_desc_ptr =
        (ps_stat_inst_desc_iface_v6_addr_type *) log_pkt_fill_ptr;

      iface_v6_desc_ptr->addr_prefix =
        (iface_ptr->iface_private.ipv6_addrs[i])->prefix;
      iface_v6_desc_ptr->addr_iid    =
        (iface_ptr->iface_private.ipv6_addrs[i])->iid;
      iface_v6_desc_ptr->addr_type   =
        (iface_ptr->iface_private.ipv6_addrs[i])->addr_type;
      iface_v6_desc_ptr->addr_state  =
        (iface_ptr->iface_private.ipv6_addrs[i])->addr_state;
      memset(iface_v6_desc_ptr->reserved, 0, 2);

      log_pkt_fill_ptr += sizeof(ps_stat_inst_desc_iface_v6_addr_type);
      log_pkt_avail_size -= sizeof(ps_stat_inst_desc_iface_v6_addr_type);

      ++ipv6_addr_count;    
    }
  }
#endif /* FEATURE_DATA_PS_IPV6 */

  iface_mcast_grp_ptr = iface_ptr->iface_private.if_mcast_grps;

  /*-----------------------------------------------------------------------
    Populate the multicast group addresses in the log packet
  -----------------------------------------------------------------------*/
  while (NULL != iface_mcast_grp_ptr)
  {
    if (IPV4_ADDR == iface_mcast_grp_ptr->mcast_grp.type)
    {
      PS_IN6_GET_V4_MAPPED_V6_FROM_V4(
        iface_mcast_v6_addr.ps_s6_addr,
        iface_mcast_grp_ptr->mcast_grp.addr.v4.ps_s_addr);
    }
    else
    {
      iface_mcast_v6_addr = iface_mcast_grp_ptr->mcast_grp.addr.v6;
    }

    memscpy(log_pkt_fill_ptr, 
            sizeof(struct ps_in6_addr), 
            &iface_mcast_v6_addr, 
            sizeof(struct ps_in6_addr));

    log_pkt_fill_ptr += sizeof(struct ps_in6_addr);
    log_pkt_avail_size -= sizeof(struct ps_in6_addr);

    ++mcast_count;

    iface_mcast_grp_ptr = iface_mcast_grp_ptr->next_mcast_grp;
  }

  iface_name_size = ps_stat_fill_iface_name(iface_ptr, log_pkt_fill_ptr, log_pkt_avail_size);

  //TODO: either make a note saying not to use the fill ptr, or pass in a
  //      double ptr to update fill ptr value (prefer double ptr)
  /*-------------------------------------------------------------------------
    Populate the iface description information in the log packet
  -------------------------------------------------------------------------*/
  iface_desc_ptr = (ps_stat_inst_desc_iface_type *)log_pkt_ptr;

  iface_desc_ptr->handle             = (uint8)iface_ptr->iface_private.iface_index;
  iface_desc_ptr->name_size          = (uint8)iface_name_size;
  iface_desc_ptr->addr_v6_count      = ipv6_addr_count;
  iface_desc_ptr->mcast_group_count  = mcast_count;
  //TODO: use macro ps_iface_get_v4_addr
  iface_desc_ptr->addr_v4            = iface_ptr->iface_private.ipv4_addr;
  //TODO: talk to srinivas about macros afterwards
  iface_desc_ptr->netmask_v4         = iface_ptr->net_info.net_ip.v4.net_mask;

  /*-------------------------------------------------------------------------
    Memcopy used here because hw_addr is an array of uint8 values
  -------------------------------------------------------------------------*/
  //TODO: use macro to get hw addr
  memscpy(iface_desc_ptr->hw_addr,
          PS_IFACE_MAX_HW_ADDR_LEN,
         iface_ptr->iface_private.hw_addr,
         PS_IFACE_MAX_HW_ADDR_LEN);

  iface_desc_ptr->mtu           = PS_IFACE_GET_MTU(iface_ptr);
  iface_desc_ptr->state         = iface_ptr->iface_private.state;
  iface_desc_ptr->bcast_capable = iface_ptr->iface_private.is_bcast_iface;
  memset(iface_desc_ptr->reserved, 0, 2);

  return;
} /* ps_stat_fill_iface_inst_desc_default */


/*===========================================================================
FUNCTION PS_STAT_GET_IFACE_DESC_LOG_PKT()

DESCRIPTION
  This function fills the description of the specified instance handle into
  a log packet.  If the handle matches the handle for all descriptions, it
  copies all of the active instance descriptions into the log packet.  If the
  handle does not specify all instances but specifies a valid active instance,
  the description associated with that instance is copied into the log packet.
  If the handle is invalid, nothing is done.

PARAMETERS
  handle          : Specifies which instance to retrieve the description from

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
//TODO: name as cback
void ps_stat_get_iface_desc_log_pkt
(
  int32        handle
)
{
  uint8        idx;
  int32        desc_size          = 0;
  uint8        inst_count         = 0;
  int32        log_pkt_avail_size = 0;
  uint8      * log_pkt_ptr        = NULL;
  uint8      * log_pkt_fill_ptr   = NULL;

  ps_iface_type                              * iface_ptr             = NULL;
  ps_stat_desc_all_inst_hdr_log_pkt_type     * desc_all_inst_pkt_ptr = NULL;
  ps_stat_desc_delta_inst_hdr_log_pkt_type   * delta_pkt_ptr         = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_STAT_INST_DESC_ALL == handle)
  {
    for (idx = 0;
         idx < MAX_SYSTEM_IFACES && NULL != global_iface_ptr_array[idx];
         ++idx)
    {
      iface_ptr = global_iface_ptr_array[idx];

      if (NULL != iface_ptr->ps_iface_stat_get_desc_len_f_ptr)
      {
        desc_size = iface_ptr->ps_iface_stat_get_desc_len_f_ptr(iface_ptr);
      }
      else
      {
        desc_size = ps_stat_get_iface_desc_len_default_cback(iface_ptr);
      }

      //TODO: explain logic and copy into all files
      /*-------------------------------------------------------------------
        Insufficient amount of space to fit this description in the log
        packet, so commit the existing log
      -------------------------------------------------------------------*/
      if (log_pkt_avail_size < desc_size)
      {
        if (NULL != log_pkt_ptr)
        {
          desc_all_inst_pkt_ptr =
            (ps_stat_desc_all_inst_hdr_log_pkt_type *) log_pkt_ptr;

          desc_all_inst_pkt_ptr->count = inst_count;
          memset(desc_all_inst_pkt_ptr->reserved, 0, 3);

          ps_stat_logging_commit_log_pkt
          (
            log_pkt_ptr,
            (PS_STAT_MAX_LOG_PKT_SIZE - log_pkt_avail_size)
          );
        }

        inst_count         = 0;
        log_pkt_avail_size = PS_STAT_MAX_LOG_PKT_SIZE;
        log_pkt_ptr        = NULL;
        log_pkt_fill_ptr   = NULL;

        /*-------------------------------------------------------------------
          Get the pointer to the log packet
        -------------------------------------------------------------------*/
        ps_stat_logging_get_log_pkt(log_pkt_avail_size,
                                    LOG_PS_STAT_DESC_ALL_IFACE_INST_C,
                                    &log_pkt_ptr);

        /*-------------------------------------------------------------------
          If we fail to get a log packet, just return
        -------------------------------------------------------------------*/
        if (NULL == log_pkt_ptr)
        {
          LOG_MSG_ERROR_0("ps_stat_get_iface_desc_log_pkt(): "
                          "Unable to allocate log packet");
          return;
        }

        /*-------------------------------------------------------------------
          Leave space for the describe all instance header
        -------------------------------------------------------------------*/
        log_pkt_fill_ptr =
          (log_pkt_ptr +
            (uint8) sizeof(ps_stat_desc_all_inst_hdr_log_pkt_type));

        log_pkt_avail_size -= sizeof(ps_stat_desc_all_inst_hdr_log_pkt_type);
      }

      //TODO: pass in double pointer so they will update the log pkt ptr
      if (NULL != iface_ptr->ps_iface_stat_fill_desc_f_ptr)
      {
       iface_ptr->ps_iface_stat_fill_desc_f_ptr(iface_ptr, log_pkt_fill_ptr, log_pkt_avail_size);
      }
      else
      {
        ps_stat_fill_iface_inst_desc_default_cback(iface_ptr, log_pkt_fill_ptr, log_pkt_avail_size);
      }

      /*-------------------------------------------------------------------
        Increment the fill pointer by the size, and decrement the count
        by the same value
      -------------------------------------------------------------------*/
      log_pkt_fill_ptr += (uint8) desc_size;
      log_pkt_avail_size -= desc_size;
      inst_count++;
    } /* for all ifaces */

    /*-----------------------------------------------------------------------
      Dispatch the last packet
    -----------------------------------------------------------------------*/
    if (NULL != log_pkt_ptr)
    {
      desc_all_inst_pkt_ptr =
        (ps_stat_desc_all_inst_hdr_log_pkt_type *) log_pkt_ptr;

      desc_all_inst_pkt_ptr->count = inst_count;
      memset(desc_all_inst_pkt_ptr->reserved, 0, 3);

      ps_stat_logging_commit_log_pkt
      (
        log_pkt_ptr,
        (PS_STAT_MAX_LOG_PKT_SIZE - log_pkt_avail_size)
      );
    }
  } /* if all instances */
  else
  {
    /*-----------------------------------------------------------------------
      Sanity check the handle value
    -----------------------------------------------------------------------*/
    if ( (MAX_SYSTEM_IFACES <= handle) || ( 0 > handle) )
    {
      LOG_MSG_ERROR_1("ps_stat_get_iface_desc_log_pkt(): "
                      "Invalid handle %d", handle);
      return;
    }

    iface_ptr = global_iface_ptr_array[handle];

    if (FALSE == PS_IFACE_IS_VALID(iface_ptr))
    {
      LOG_MSG_ERROR_1("ps_stat_get_iface_desc_log_pkt(): "
                      "Invalid iface 0x%p", iface_ptr);
      return;
    }

    if (NULL != iface_ptr->ps_iface_stat_get_desc_len_f_ptr)
    {
      desc_size = iface_ptr->ps_iface_stat_get_desc_len_f_ptr(iface_ptr);
    }
    else
    {
      desc_size = ps_stat_get_iface_desc_len_default_cback(iface_ptr);
    }

    /*-----------------------------------------------------------------------
      We need to allocate enough memory for the description and the delta
      header
    -----------------------------------------------------------------------*/
    log_pkt_avail_size =
      desc_size + sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type);

    // TODO: add a log_pkt_req_size to clarify code semantics
    ps_stat_logging_get_log_pkt(log_pkt_avail_size,
                                LOG_PS_STAT_DESC_DELTA_INST_C,
                                &log_pkt_ptr);

    /*-----------------------------------------------------------------------
      If we fail to get a log packet, just return
    -----------------------------------------------------------------------*/
    if (NULL == log_pkt_ptr)
    {
      LOG_MSG_ERROR_0("ps_stat_get_iface_desc_log_pkt(): "
                      "Unable to allocate log packet");
      return;
    }

    /*-----------------------------------------------------------------------
      Fill in the delta log parameters
    -----------------------------------------------------------------------*/
    delta_pkt_ptr = (ps_stat_desc_delta_inst_hdr_log_pkt_type *) log_pkt_ptr;

    delta_pkt_ptr->type    = PS_STAT_MODULE_INST_IFACE;
    delta_pkt_ptr->deleted = FALSE;
    memset(delta_pkt_ptr->reserved, 0, 2);

    log_pkt_fill_ptr =
     (log_pkt_ptr + (uint8) sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type));
    log_pkt_avail_size -= sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type);

    if (NULL != iface_ptr->ps_iface_stat_fill_desc_f_ptr)
    {
      iface_ptr->ps_iface_stat_fill_desc_f_ptr(iface_ptr, log_pkt_fill_ptr, log_pkt_avail_size);
    }
    else
    {
      ps_stat_fill_iface_inst_desc_default_cback(iface_ptr, log_pkt_fill_ptr, log_pkt_avail_size);
    }

    /*-----------------------------------------------------------------------
      Dispatch the packet
    -----------------------------------------------------------------------*/
    ps_stat_logging_commit_log_pkt
    (
      log_pkt_ptr,
      (desc_size + sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type))
    );
  } /* else fill one iface instance */
} /* ps_stat_get_iface_desc_log_pkt */


/*===========================================================================
FUNCTION PS_STAT_GET_IFACE_STAT_LOG_PKT()

DESCRIPTION
  This function fills the statistics of the specified instance handle into
  a log packet.  If the handle matches the handle for all statistics, it
  copies all of the active instance statistics into the log packet.  If the
  handle does not specify all instances but specifies a valid active instance,
  the statistics associated with that instance is copied into the log packet.
  If the handle is invalid, nothing is done.

PARAMETERS
  handle          : Specifies which instance to retrieve the stats from

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_get_iface_stat_log_pkt
(
  int32        handle
)
{
  uint8                                  idx;
  uint8                                * log_pkt_ptr            = NULL;
  ps_iface_type                        * iface_ptr              = NULL;
  ps_stat_inst_iface_log_pkt_type      * inst_iface_log_pkt_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_STAT_INST_DESC_ALL == handle)
  {
    for (idx = 0; idx < MAX_SYSTEM_IFACES; ++idx)
    {
      iface_ptr = global_iface_ptr_array[idx];

      //TODO: add check to get_desc as well
      /*---------------------------------------------------------------------
        There is no valid stats for this handle, so do nothing
      ---------------------------------------------------------------------*/
      if (IFACE_UP != PS_IFACEI_GET_STATE(iface_ptr))
      {
        continue;
      }

      ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_iface_log_pkt_type),
                                  LOG_PS_STAT_IFACE_INST_C,
                                  &log_pkt_ptr);

      if (NULL == log_pkt_ptr)
      {
        LOG_MSG_ERROR_0("ps_stat_get_iface_stat_log_pkt(): "
                        "Log packet allocated for IFACE failed");
        return;
      }

      inst_iface_log_pkt_ptr = (ps_stat_inst_iface_log_pkt_type *) log_pkt_ptr;

      inst_iface_log_pkt_ptr->handle  = idx;

      (void) ps_stat_get_iface(PS_STAT_IFACE_ALL,
                               (void *) iface_ptr,
                               &(inst_iface_log_pkt_ptr->inst_iface_stats),
                               sizeof(ps_stat_iface_i_s_type));

      ps_stat_logging_commit_log_pkt
      (
        log_pkt_ptr,
        sizeof(ps_stat_inst_iface_log_pkt_type)
      );

    } /* for all ifaces */
  } /* if all instances */
  else
  {
    /*-----------------------------------------------------------------------
      Sanity check the handle value
    -----------------------------------------------------------------------*/
    if ( (MAX_SYSTEM_IFACES <= handle) || ( 0 > handle) )
    {
      LOG_MSG_ERROR_1("ps_stat_get_iface_stat_log_pkt(): "
                      "Invalid handle %d", handle);
      return;
    }

    iface_ptr = global_iface_ptr_array[handle];

    /*---------------------------------------------------------------------
      There is no valid stats for this handle, so do nothing
    ---------------------------------------------------------------------*/
    if (IFACE_UP != PS_IFACEI_GET_STATE(iface_ptr))
    {
      LOG_MSG_ERROR_1("ps_stat_get_iface_stat_log_pkt(): "
                      "Invalid handle %d", handle);
      return;
    }

    ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_iface_log_pkt_type),
                                LOG_PS_STAT_IFACE_INST_C,
                                &log_pkt_ptr);

    if (NULL == log_pkt_ptr)
    {
      LOG_MSG_ERROR_0("ps_stat_get_iface_stat_log_pkt(): "
                      "Log pkt alloc failed");
      return;
    }

    inst_iface_log_pkt_ptr = (ps_stat_inst_iface_log_pkt_type *) log_pkt_ptr;

    inst_iface_log_pkt_ptr->handle  = handle;

    (void) ps_stat_get_iface(PS_STAT_IFACE_ALL,
                             (void *) iface_ptr,
                             &(inst_iface_log_pkt_ptr->inst_iface_stats),
                             sizeof(ps_stat_iface_i_s_type));

    ps_stat_logging_commit_log_pkt
    (
      log_pkt_ptr,
      sizeof(ps_stat_inst_iface_log_pkt_type)
    );
  } /* else fill one iface instance */
} /* ps_stat_get_iface_stat_log_pkt */


/*===========================================================================

                             EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION PS_STAT_RESET_IFACE()

DESCRIPTION
  This function resets the statistics on the specified interface.

PARAMETERS
  handle:   Handle to the iface being reset

RETURN VALUE
  DS_SUCCESS on success
  DS_ERROR   on failure

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_iface
(
  int32              handle
)
{
  ps_iface_type    * iface_cb_ptr, * iface_base_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-----------------------------------------------------------------------
    Sanity check the handle value
  -----------------------------------------------------------------------*/
  if ( (MAX_SYSTEM_IFACES <= handle) || ( 0 > handle) )
  {
    LOG_MSG_ERROR_1("ps_stat_reset_iface(): "
                    "Invalid handle %d", handle);
    return;
  }

  iface_cb_ptr = global_iface_ptr_array[handle];

  /*---------------------------------------------------------------------
    There is no valid stats for this handle, so do nothing
  ---------------------------------------------------------------------*/
  if (iface_cb_ptr == NULL)
  {
    LOG_MSG_ERROR_1("ps_stat_reset_iface(): "
                    "Attempting to reset statistics on NULL interface. "
                    "Handle: %d", handle);
    return;
  }

  iface_base_ptr = PS_IFACE_GET_BASE_IFACE (iface_cb_ptr);

  if (PS_STAT_QUERIED == iface_base_ptr->iface_i_stats.queried)
  {
    ps_stat_get_iface_stat_log_pkt(handle);
  }
  
  ps_stat_reset_iface_stats_by_iface_ptr(iface_cb_ptr);
  
  return;

} /* ps_stat_reset_iface */

/*===========================================================================
FUNCTION PS_STAT_RESET_IFACE_STATS_USING_INSTANCE_PTR()

DESCRIPTION
  This function resets the statistics on the specified interface.

PARAMETERS
  instance_ptr: Handle to the iface being reset

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_iface_stats_by_iface_ptr
(
  void*      instance_ptr
)
{
  ps_iface_type     *iface_ptr, * iface_base_ptr;
  ps_iface_stat_adjustment_type    stat_adjustment;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  iface_ptr = (ps_iface_type*)instance_ptr;
  /*-----------------------------------------------------------------------
    Sanity checks
  -----------------------------------------------------------------------*/
  if (!PS_IFACE_IS_VALID(iface_ptr))
  {
    LOG_MSG_ERROR_1("ps_stat_reset_iface_stats_by_iface_ptr(): "
                    "Invalid iface_ptr 0x%p", iface_ptr);
    return;
  }
  
  iface_base_ptr = PS_IFACE_GET_BASE_IFACE (iface_ptr);


  memset(&(iface_base_ptr->iface_i_stats), 0,
         sizeof(iface_base_ptr->iface_i_stats));

  /*---------------------------------------------------------------------
    Cache the packet stats queried from hardware. Next time when 
    client calls function to retrieve the stats, we would return difference
    of cached stats and what hardware returns.
  ---------------------------------------------------------------------*/
  if (iface_base_ptr->ps_iface_stat_get_adjustment_f_ptr != NULL)
  {
    memset(&stat_adjustment, 0, sizeof(ps_iface_stat_adjustment_type));
    iface_base_ptr->ps_iface_stat_get_adjustment_f_ptr(iface_base_ptr, 
                                                       &stat_adjustment);
    memscpy(&(iface_base_ptr->stat_adjustment_cache),
            sizeof(ps_iface_stat_adjustment_type),
           &stat_adjustment,
           sizeof(ps_iface_stat_adjustment_type));
  }
 
  return;
} /* ps_stat_reset_iface_stats_by_iface_ptr() */


/*===========================================================================
FUNCTION PS_STAT_INIT_IFACE()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_iface
(
  void
)
{
  ps_stat_logging_inst_reg_fill_desc_cback(PS_STAT_MODULE_INST_IFACE,
                                           ps_stat_get_iface_desc_log_pkt);

  ps_stat_logging_inst_reg_get_stat_cback(PS_STAT_MODULE_INST_IFACE,
                                          ps_stat_get_iface_stat_log_pkt);

  ps_stat_logging_inst_reg_reset_stat_cback(PS_STAT_MODULE_INST_IFACE,
                                            ps_stat_reset_iface);

} /* ps_stat_init_iface */


/*===========================================================================
FUNCTION PS_STAT_GET_IFACE()

DESCRIPTION
  This function supplies the required iface layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer to the address of the
  iface control block. The third argument points to the memory location
  where the results will be copied.The result (statistic) will be copied to
  this memory address. The fourth argument passes the length of memory
  allocated for the return value and should be  greater than or equal to
  the memory size of the statistic requested.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  instance_ptr    - IN - Handle to instance
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_iface
(
  ps_stat_iface_enum_type  stat,
  void                     *instance_ptr,
  void                     *return_value,
  uint16                   ret_len
)
{
  ps_iface_type                  * iface_ptr;
  ps_stat_iface_i_s_type         * iface_i_stats;
  ps_iface_stat_adjustment_type    stat_adjustment;
  errno_enum_type                  ret_val;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Validate instace_ptr
  -------------------------------------------------------------------------*/
  iface_ptr = (ps_iface_type *) instance_ptr;
  if (!PS_IFACE_IS_VALID(iface_ptr) || ( NULL == return_value ))
  {
    LOG_MSG_ERROR_2("ps_stat_get_iface():"
                    "Invalid iface 0x%x,or ret value 0x%x", 
                     iface_ptr, return_value);
    return E_INVALID_ARG;
  }
  
  /*------------------------------------------------------------------------
    If iface has inherit IP info true then get the base iface of the logical
    chain, else do nothing.
  -------------------------------------------------------------------------*/
  
  iface_ptr = PS_IFACE_GET_BASE_IFACE(instance_ptr);
  if (stat == PS_STAT_IFACE_ALL)
  {
     ret_val = ps_stat_get_iface_instance_all(stat,
                                              iface_ptr,
                                              return_value,
                                              ret_len);
  }
  else
  {
    ret_val =  ps_stat_get_common(stat,
                                  ps_stat_iface_i_table,
                                  PS_STAT_IFACE_ALL,
                                  iface_ptr,
                                  return_value,
                                  ret_len);
  }

  /*-------------------------------------------------------------------------
    Get stats adjustment if it is needed
  -------------------------------------------------------------------------*/
  if (iface_ptr->ps_iface_stat_get_adjustment_f_ptr != NULL)
  {

    memset(&stat_adjustment, 0, sizeof(ps_iface_stat_adjustment_type));
    iface_ptr->ps_iface_stat_get_adjustment_f_ptr(iface_ptr, &stat_adjustment);

    /*-------------------------------------------------------------------------
      Update results with adjustment and cached values from last pkt stats 
      reset call.
    -------------------------------------------------------------------------*/
    switch (stat)
    {
      case PS_STAT_IFACE_ALL:
        iface_i_stats = (ps_stat_iface_i_s_type *) return_value;

        iface_i_stats->pkts_rx  += 
          (stat_adjustment.pkts_rx - iface_ptr->stat_adjustment_cache.pkts_rx);		
        iface_i_stats->pkts_tx  += 
          (stat_adjustment.pkts_tx - iface_ptr->stat_adjustment_cache.pkts_tx);
        iface_i_stats->bytes_rx += 
          (stat_adjustment.bytes_rx - iface_ptr->stat_adjustment_cache.bytes_rx);
        iface_i_stats->bytes_tx += 
          (stat_adjustment.bytes_tx - iface_ptr->stat_adjustment_cache.bytes_tx);		
        break;

      case PS_STAT_IFACE_PKTS_RX:
        (*(uint32 *)return_value) += 
          (stat_adjustment.pkts_rx - iface_ptr->stat_adjustment_cache.pkts_rx);        
        break;

      case PS_STAT_IFACE_PKTS_TX:
        (*(uint32 *)return_value) += 
          (stat_adjustment.pkts_tx - iface_ptr->stat_adjustment_cache.pkts_tx);
        break;

      case PS_STAT_IFACE_BYTES_RX:
        (*(uint64 *)return_value) += 
          (stat_adjustment.bytes_rx - iface_ptr->stat_adjustment_cache.bytes_rx);
        break;

      case PS_STAT_IFACE_BYTES_TX:
        (*(uint64 *)return_value) += 
          (stat_adjustment.bytes_tx - iface_ptr->stat_adjustment_cache.bytes_tx);
        break;

      default:
        // do nothing
        break;
    }
  }
		
  return ret_val;
} /* ps_stat_get_iface */

