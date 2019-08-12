/*===========================================================================

   P R O T O T O C O L   S E R V I C E S   D Y N A M I C   M E M O R Y

                          F I L E

GENERAL DESCRIPTION
  This module implements a simple dynamic memory management mechanism for
  the protocol services.

EXTERNALIZED FUNCTIONS
  ps_mem_init()              - Initializes ps_mem module
  ps_mem_pool_init()         - Initializes a single ps_mem pool
  ps_mem_dynamic_pool_init() - Initializes a single ps_mem_pool to use dynamic
                               heap memory.
  ps_mem_get_buf()           - Allocates a memory buffer for the specified poolid
  ps_mem_is_valid()          - Validates that this is memory item is of the
                               specified poolid
  ps_mem_free()              - Free the allocated memory, like free.
  ps_mem_index_to_buf()      - Given an index return corresponding buffer.
  ps_mem_buf_to_index()      - Given a buffer, return its index in the pool.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  ps_mem_init() has to be called exactly once before using any externalized
  functions provided by this module and each client has to initialize its pool
  using ps_mem_pool_init()

Copyright (c) 2003-2015 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ps_mem.c#1 $
  $DateTime: 2016/12/13 08:00:03 $Author: $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/04/15    gk     Removed redundant F3 as it is being printed in data path
06/14/11    aa     Added DSSNetActive and DSSNetMonitored pools.
01/28/11    ss     Removing IS_BUFFER_ADDR_VALID macro.
01/27/11    mct    IPSec fixes.
01/27/11    ss     Fixing bug in macro IS_BUFFER_ADDR_VALID.
12/27/10    rp     RAM optimization changes.
12/23/10    ss     Cleaning up of Globals for thread safety in Q6 Free 
                   Floating environment.
10/01/10    sa     Added appropriate log messages before ASSERT(0).
02/19/09    am     Added support to detect multi-inits from DCC/PS tasks.
02/23/09    mjb    Added ability to dynamically allocate buffers from heap.
10/24/08    pp     Fixed compile warnings observed from OFFTARGET build.
09/15/08    dm     Restoring ps_mem_pool_init signature
09/12/08    pp     Metainfo optimizations.
07/17/08    dm     Featurized stat and logging code under feature
                   FEATURE_DATA_PS_MEM_STAT_LOGGING
08/15/07    va     Re-architected the Module
05/08/07    sv     Removed references to dhcp_config_cb as the ioctl is not
                   supported.
05/07/07    mct    IPv6 Neighbor Discovery
05/04/07    hm     Additions for new DNS API
05/04/07    vp     Increased ps_iface_event_cback_buf's
04/12/07    ssh    Using single buffer for MEM cmds, renamed the struct.
04/11/07    ssh    Additions for MIP6 Registration State Machine (RSM)
04/09/07    vp     Additions for mip6_bootstrap_info
03/27/07    msr    Increased Iface event cback buffers for EMPA enhancements
02/27/07    scb    Added changes to fix RVCT compiler warning in stats module
02/01/07    hm     Added IOCTL to get fresh DHCP configuration
01/29/07    ks     Increased Iface and Flow event cback buffers for QMI_QOS.
11/21/06    rt     Added IGMP/MLD support for MBMS.
11/04/06    hm     Added support for domain name search list for DHCPv6 client
11/02/06    mct    Added support for dynamic allocation of IPv6 addresses.
09/12/06    msr    Returning EWOULDBLOCK instead of ENOMEM if metainfo
                   generation is failed in dssocki_sendmsg()
08/25/06    tbh    Added support for Link Local Addr Mgr buffer.
08/23/06    ks     Increased Iface and Phys link event callback buffers for
                   multiple rmnet instaces.
07/31/06    rt     PS mem code for Route Scope.
07/20/06  mpa/msr  Using local critical section
06/23/06    ssh    Bumped up allowed HDLC instances to 40
04/30/06    rt     Code for SIP server address and domain names.
04/18/06    mct    Increased psiface event cback buffers for LO optimizations
04/12/06    msr    Optimized calling of low WM callbacks
02/22/06    msr    Using single critical section
02/06/06    msr    Updated for L4 tasklock/crit sections.
12/05/05    sv     Added constant for RSVP control block size.
12/06/05    vp     Added num_used_buffers to ps stats
12/05/05    msr    Increased iface ev buffers to accomodate dynamic QoS
12/04/05    msr    Added RSVP to ps_mem_buf_arr
11/30/05    rt     Increased app_to_ps command buffers to 100.
11/29/05    msr    Changed offsetof to FPOS
11/29/05    sv     PS mem for RSVP control blocks.
09/14/05    ssh    Changed include file ps_hdlc_lib.h to ps_hdlc_libi.h
08/16/05    msr    Fixed PS_BRANCH_TASKFREE()
08/15/05    msr    Increased number of buffers in pool 2 to 100.
08/15/05    kr     Added support for stats update
07/07/05    msr    Added ptrs to ps_mem buffers to make debugging easier.
06/09/05    mct    Added mem pool for support multiple QOS IP flow specs.
06/15/05    msr    Increased number of buffers in pool 3 to 100.
05/18/05    ssh    Fixed lint errors.
05/05/05    msr    Fixed buffer of sizes of pools 18 and 19.
04/17/05    msr    Added new pools to handle ps_flow related stuff.
04/16/05    ks     Decreased the number of dssocki_iface_event_info_u_type
                   buffers to 100.
02/22/05    ssh    Replaced INTLOCK()s with TASKLOCK()s
01/17/05    mct    Fixed error in which ps_mem items not being freed.
01/10/05    sv     Added ps_mem_dup function.
01/08/05    hba    Added support for LAN LLC buffers.
01/08/05    msr    Added new pool for Rx IP filters.
10/28/04    ks     Increased the number of dssocki_iface_event_info_u_type
                   buffers.
08/30/04  ks/msr   Added memory pools for ICMP control blocks (ICMB).
                   A dummy pool is added with an id of 0. Changed
                   PS_MEM_BUF_HEADER to PS_MEM_BUF_HDR_SIZE. Changed function
                   prototype of ps_memi_make_pool. Increased the size of
                   alloc_idx to uint32. Ensured that ps_mem_heap is allocated
                   at word boundary. Moved free_list pointer from
                   ps_mem_pool_type to ps_mem_pool_info_s_type and deleted
                   ps_mem_pool_type.
08/16/04  ks/msr   Added memory pools for SCB/ACB/TCB/UCBs. Added
                   ps_mem_buf_to_index and ps_mem_index_to_buf functions.
                   Changing buffer's address using PS_MEM_BUF_HEADER instead
                   of "--buffer" or "++buffer" etc.
09/07/04    sv     Fixed total memroy size computation bug due to word
                   alignment.
09/01/04    mvl    Fixed a bug there is valid was not checking the correct
                   range.  Also inlined a ps_memi_getmem().
08/31/04    mct    Fixed merge issue.
08/16/04    sv     Added memory pool for storing TCP SACK blocks.
07/23/04    msr    Updated description of ps_mem_reg_mem_avail_cb function
                   and mem_avail_cb function pointer.
07/22/04    msr    Renamed the pointer lo_wm_cb to mem_avail_cb and the
                   function ps_mem_reg_low_wm_cb to ps_mem_reg_mem_avail_cb.
06/15/04    ifk    Added memory pool for MEM event information structures
06/09/04    sv     Removed featurization around numused increment in get buf.
06/07/04   aku/sv  Added functions to register low watermark call back and
                   query if items are less than low watermark.
04/23/04    mvl    Added memory pool for HDLC instances - made
                   ps_mem_is_valid() stricter.
04/21/04    usb     Added memory pool for pkt info stored in meta info.
03/15/04    ifk    Added memory pool for MEM configuration buffers.
02/04/04    ak     Include ps_phys_linki_event.h.
02/02/04    jd     Free TASKLOCK in ps_mem_get_buf early return
01/23/04    aku    Task locked ps_mem_get_buf() and ps_mem_free() routines as
                   they can be called from multiple tasks.
12/12/03    sv     Added new memory pool for sockets iface_event_cmd.
12/02/03    aku    Increased number of app_to_ps command buffers to 50.
11/26/03    aku    Increased number of metainfo items to 50 from 30.
11/03/03    aku    Increased ps_iface_event pool to 50 from 25.
10/22/03    mct    Added the ps_iface_ipfilteri_type buffer type and included
                   the ps_iface_ipfltr.h
10/20/03    aku    Increased the number of meta_info items to 30 from 20.
08/25/03    aku    ps_mem_free() takes pointer-to-pointer as arg and set
                   buffer to NULL in ps_mem_free().
08/15/03    aku    Created Module.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "customer.h"
#include "comdef.h"

#include "msg.h"
#include "err.h"
#include "IxErrno.h"
#include "ps_mem_ext.h"
#include "ps_mem.h"
#include "ps_crit_sect.h"
#include "ps_stat_logging.h"
#include "ps_stat_mem.h"


#include <stringl/stringl.h>
#include <string.h>


#include "ps_system_heap.h"
#include "ds_Utils_DebugMsg.h"
#include "ps_tx_meta_info.h"


/*===========================================================================

                         LOCAL DEFINITIONS AND DATA

===========================================================================*/

/*---------------------------------------------------------------------------
  typdef ps_mem_pool_info_s is used to store the static pool information.
  Size of buffer and number of buffers of that size. Size of the buffer
  is the size of free space that can be allocated. It does not include
  the overhead for pointer to the next item, etc.
---------------------------------------------------------------------------*/
typedef struct ps_mem_pool_info_s
{
  struct  mem_cb_stats
  {
    uint32   alloc_errors;         /* # of allocation errors for this pool */
    uint32   high_wm_exceeds;      /* # of times high wm has been exceeded */
    uint32   max_alloc_buffers;    /* max # of buffers ever allocated      */
    uint32   num_used_buffers;     /* number of buffers used currently     */
    uint32   queried;
  } mem_i_stats;

  int32                    * first_element; /* Pointer to the first element*/
  int32                    * last_element;  /* Pointer to the last element */
  int32                    * free_list;     /* Pointer to free buffer list */
  ps_mem_wm_cb_f_ptr_type    mem_avail_cb;  /* Memory available callback,
                                               triggered on the way up     */
  uint16                     size;          /* Size of buffer              */
  int16                      total_num;     /* Total # of buffers          */
  int16                      num_used;      /* Number of buffers being used*/
  int16                      hi_watermark;  /* High watermark not to exceed*/
  int16                      lo_watermark;  /* Low watermark not to exceed */
  boolean                    is_high_wm_hit;/* Is high WM reached?         */
  boolean                    is_static;     /* Is a static pool/dynamic    */
#ifdef FEATURE_DATA_PS_MEM_DEBUG
  int32                    * buf_hdr_arr;   /* keep buf_hdr_arr for dynamic*/
  int32                    * buf_ptr_arr;   /* keep buf_ptr_arr for dynamic*/
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
} ps_mem_pool_info_s_type;

static ps_mem_pool_info_s_type ps_mem_pool_info[PS_MEM_MAX_POOLS];

/*---------------------------------------------------------------------------
  Critical section used to provide mutual exclusion to ps_mem_pool_info
---------------------------------------------------------------------------*/
static ps_crit_sect_type  ps_mem_crit_section;

/*---------------------------------------------------------------------------
  The following character array holds the names of all of the ps_mem pools
  Used to describe the memory pools uniquely
---------------------------------------------------------------------------*/
static char *ps_mem_pool_name_arr[] =
{
  "DUMMY_POOL",
  "TX_META_INFO",
  "PS_IFACE_EVT_CB",
  "DSS_IOCTL_EVT_CB",
  "APP_TO_PS_CMD",
  "PS_PHYS_LINK_EVT_CB",
  "PS_IFACE_IPFLTER",
  "PS_IFACE_EVENT_CMD",
  "PPP_CMD_BUF",
  "PKT_META_INFO",
  "HDLC_INSTANCE",
  "PPP_EVENT_INFO_ITEM",
  "TCP_SACK_BLOCKS",
  "DSS_SCB",
  "DSS_ACB",
  "DSS_TCB",
  "DSS_UCB",
  "DSS_ICMB",
  "RX_QOS_FLTR_BUF",
  "IFACE_RX_QOS_FLTR_NODE",
  "LAN_LLC_CFG_BUFFER",
  "QOS_INFO",
  "PS_FLOW",
  "PS_FLOW_EVT_CB",
  "PS_FLOW_EVENT_CMD",
  "DSS_IOCTL_FLOW_EVT_CB",
  "PS_FLOW_IP_FLOW_SPEC",
  "RSVP_CB",
  "PS_SIP_SERV_ADDR",
  "PS_SIP_SERV_DOMAIN_NAME",
  "PS_ROUTE_SCOPE",
  "LL_ADDR_MGR_BUFFER",
  "IPV6_ADDR",
  "PS_DOMAIN_NAME_SEARCH_LIST_NODE",
  "MCAST_SEND_MSG",
  "PS_MIP6_BOOTSTRAP_CFG_INFO",
  "MIP6_RSM_BUF",
  "DNSI_API_CB",
  "DNSI_SESSION_MGR_CB",
  "DNSI_RESOLVER_CB",
  "DNSI_IO_MGR_CB",
  "DNSI_CACHE_ENTRY",
  "DNSI_GENERIC_RR",
  "DSS_NET_MGR_CB",
  "DNSI_HOSTENT_CB",
  "IPV6_ND_NBR_CACHE_ENTRY",
  "IPV6_ND_PREFIX_LIST_ENTRY",
  "MCAST_ADD_GRP",
  "MIP6_RSM_CMD_DATA_BUF",
  "HDLCI_PP_QUEUE_NODE",
  "LOG_STAT_CMD",
  "LOG_DESC_CMD",
  "PS_POLICY",
  "RT_META_INFO",
  "RX_META_INFO",
  "META_INFO_TYPE_EX",
  "PS_SOCKET_CMD",
  "PLATFORM_SOCKET",
  "TCP_SOCKET",
  "UDP_SOCKET",
  "ICMP_SOCKET",
  "SOCKET_FACTORY",
  "DS_NET_NETWORK_FACTORY",
  "DS_NET_EVENT_MANAGER",
  "DS_NET_EVENT_NODE",
  "DS_NET_NETWORK_MONITORED",
  "DS_NET_NETWORK_1X",
  "DS_NET_NETWORK_IPV6",
  "DS_NET_NETWORK_MBMS",
  "DS_NET_NETWORK_UMTS",
  "DS_NET_POLICY",
  "DS_NET_PHYS_LINK",
  "DS_NET_QOS_DEFAULT",
  "DS_NET_QOS_SECONDARY",
  "DS_NET_QOS_MANAGER",
  "DS_NET_QOS_1X",
  "DS_NET_QOS_FLOW_SPEC",
  "DS_NET_IP_FILTER_SPEC",
  "DS_NET_IP_FILTER_REG",
  "DS_NET_MCAST_MANAGER",
  "DS_NET_MCAST_SESSION",
  "DS_NET_BEARER_TECH",
  "DS_NET_MBMS_JOIN_INFO",
  "DS_NET_TECH_UMTS_FACTORY",
  "DS_NET_MTPD_REG",
  "DS_NET_QOS_SECONDARIES_INPUT",
  "DS_NET_QOS_SECONDARIES_OUTPUT",
  "DS_NET_PS_QOS_SPEC_WRAPPER",
  "DS_NET_QOS_MANAGER_JSON", // Json is deprecated
  "DS_NET_FIREWALL_MANAGER",
  "DS_NET_FIREWALL_RULE",
  "DS_NET_NAT_SESSION",
  "DS_UTILS_SIGNAL",
  "DS_UTILS_SIGNAL_CTL",
  "DS_UTILS_SIGNAL_BUS",
  "DS_UTILS_SIGNAL_FACTORY",
  "DS_UTILS_CRIT_SECT",
  "TCP_BYTE_STREAM",
  "UDP_DGRAM_QUEUE",
  "DNSI_LOCAL_CACHE_ENTRY",
  "QMH_MSG_BUF",
  "PLATFORM_SOCKET_FACTORY",
  "DSAL_GLOBALS",
  "DSAL_NET_APP",
  "DSAL_NET_ACTIVE_APP",
  "DSAL_SOCKET",
  "DSAL_FILTER_REG_INFO",
  "DSAL_NET_QOS_DEFAULT",
  "DSAL_NET_QOS_SECONDARY",
  "DSAL_MCAST",
  "DSAL_NET_MCAST_MBMS_CTRL",
  "DSAL_EVENT_HANDLER",
  "DSAL_PRIV_IPV6_ADDR",
  "DS_NET_NETWORK_IPV6_ADDRESS",
  "DCC_CMD_DATA_BUF",
  "ICMP_ERR_INFO",
  "RECV_IF_INFO",
  "DS_NET_BCMCS_UPDATE_PARAM",
  "DNSI_SESSION_DNS_SERVERS",
  "PPP_NCP_CB",
  "SOCKET_FACTORY_CLIENT",
  "DS_UTILS_FULL_PRIV_SET",
  "SOCK_1_NET_CTL",
  "SOCK_2_NET_CTL",
  "SOCK_3_NET_CTL",
  "DS_NET_NETWORK_FACTORY_CLIENT",
  "DS_NET_QOS_JSON_2_PS", // Json is deprecated
  "DS_NET_QOS_JSON", // Json is deprecated
  "DS_NET_QOS_SECONDARY_JSON", // Json is deprecated
  "DS_NET_QOS_DEFAULT_JSON", // Json is deprecated
  "DS_NET_MCAST_MANAGER_PRIV",
  "DS_NET_MCAST_SESSION_PRIV",
  "DS_NET_MCAST_SESSIONS_INPUT",
  "DS_NET_MCAST_SESSIONS_OUTPUT",
  "DNSI_MSG_RR",
  "DSAL_FIREWALL",
  "DNS_PROXY_SOCK_EVENT_INFO",
  "DNS_PROXY_ENABLE_INFO",
  "DNS_PROXY_DISABLE_INFO",
  "DNS_PROXY_IFACE_EVENT_INFO",
  "DS_NET_NAT_MANAGER",
  "DSAL_NET_MONITORED_TYPE",
  "TEST_POOL",
  "DS_NET_QOS_NET_INITIATED_REQ",
  "DS_NET_QOS_NET_INITIATED",
  "PS_QOS_NET_INITIATED_REQ", 
  "PS_QOS_NET_INITIATED_REQ_INFO",
  "PS_QOS_NET_INITIATED_FLTR",
  "DSAL_NET_QOS_NET_INITIATED_REQ",
  "DSAL_NET_QOS_NET_INITIATED",
  "DS_NET_BEARER_TECH_EX",
  "DS_NET_NETWORK_ACTIVE",
  "PROFILE_DB_CALLBACK_MGR_MODEM",
  "PROFILE_DB_DBMGR",
  "PROFILE_DB_FILE_EFS",
  "PROFILE_DB_MUTEX_MODEM",
  "PROFILE_DB_PARAMS_3GPP",
  "PROFILE_DB_PARAMS_3GPP2",
  "PROFILE_DB_PARAMS_COMMON",
  "PROFILE_DB_PARAMS_GENERAL",
  "PROFILE_DB_PROFILE",
  "PROFILE_DB_TECH_MGR_3GPP",
  "PROFILE_DB_TECH_MGR_3GPP2",
  "PROFILE_DB_NV_MGR",
  "PS_MEM_CLAT_INSTANCE_TYPE",
  "MAX_POOLS"
};

#ifdef TEST_FRAMEWORK
 #error code not present
#elif defined(FEATURE_QDSP6)
 #define  CALLER_ADDRESS(level) ((void *)__builtin_return_address(level))
#else
 #define  CALLER_ADDRESS(level) ((void *) __return_address())
#endif

#ifdef TEST_FRAMEWORK 
#error code not present
#endif /* TEST_FRAMEWORK */

/*===========================================================================
MACRO MEM_STAT_OFFSET_SIZE (INDEX,field, size)

DESCRIPTION
  This macro fills the offset and size fields of the ps_stat_mem_i_table.
  INDEX is used only for readability, not for usage

  This macro is used to statically initialize ps_mem_stats structure at the
  time of its definition (for ex, struct xyz_type a = {1, 2})

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define MEM_STAT_OFFSET_SIZE(INDEX,FIELD, SIZE)                          \
{																		  \
  FPOS_STAT(ps_mem_pool_info_s_type,FIELD), 							 \
  sizeof(SIZE), 														  \
}

/*---------------------------------------------------------------------------
  MEM stats access table
-------------------------------------------------------------------------- */
static const ps_stat_access_table_s_type ps_stat_mem_i_table[PS_STAT_MEM_ALL + 1] = 
{
  MEM_STAT_OFFSET_SIZE(0,mem_i_stats.alloc_errors,       uint32),
  MEM_STAT_OFFSET_SIZE(1,mem_i_stats.high_wm_exceeds,    uint32),
  MEM_STAT_OFFSET_SIZE(2,mem_i_stats.max_alloc_buffers,  uint32),
  MEM_STAT_OFFSET_SIZE(3,mem_i_stats.num_used_buffers,   uint32),
  /*-------------------------------------------------------------------------
	For MEM_ALL
  -------------------------------------------------------------------------*/
  MEM_STAT_OFFSET_SIZE(4,mem_i_stats.alloc_errors,      ps_stat_mem_i_s_type)
};


static boolean ps_mem_initialized = FALSE;


/*===========================================================================

                        LOCAL FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================
FUNCTION      PS_MEMI_POOL_INIT()

DESCRIPTION   This function is used to initialize the PS mem buffer pools.
              It creates the memory pool based on the different pool id
              (size) and number of buffers for this pool id, as defined in
              ps_mem_pool_info. As such, these numbers should be tuned to
              achieve minimum yet sufficient memory usage. This internal
              version takes the boolean is_static to determine static v.s.
              dynamic (system heap) buffer allocation.

PARAMETERS
  pool_id                       - Pool ID Number 1..PS_MEM_MAX_POOLS-1
  ps_mem_meta_data              - Allocation for integer pointer array
                                  that is sizeof(int*) *  num_buf in size.
  buf_size                      - Size of each buffer or structure
  num_buf                       - Number of buffers
  high_wm                       - Callback trigger for high watermark
  low_wm                        - Callback trigger for low watermark
  mem_avail_cb_f_ptr            - Callback that is triggered by watermarks
  buf_hdr_arr                   - Array of pointers to buffer headers
  buf_ptr_arr                   - Array of pointers to buffer payloads
                                  Suggest to make these of the type in use
RETURN VALUE  0 Success, -1 Failure

DEPENDENCIES  Called only once by each client
==========================================================================*/

static int32 ps_memi_pool_init
(
  ps_mem_pool_enum_type      pool_id,
  int32                    * buf_memory,
  uint16                     buf_size,
  int16                      num_buf,
  int16                      high_wm,
  int16                      low_wm,
  ps_mem_wm_cb_f_ptr_type    mem_avail_cb_f_ptr,
  int32                      buf_hdr_arr[],
  int32                      buf_ptr_arr[],
  boolean                    is_static
)
{
  int32   ** last;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (pool_id >= PS_MEM_MAX_POOLS)
  {
    LOG_MSG_ERROR_1("ps_memi_pool_init(): "
                    "Incorrect pool_id %d", pool_id);
    return -1;
  }

  if (buf_memory == NULL)
  {
    MEM_INC_INSTANCE_STATS(pool_id, alloc_errors, 1);
    LOG_MSG_ERROR_0("ps_memi_pool_init(): "
                    "NULL ptr");
    return -1;
  }

  if (num_buf <= 0 || high_wm > num_buf ||
      low_wm < 0 || low_wm >= num_buf || low_wm >= high_wm)
  {
    MEM_INC_INSTANCE_STATS(pool_id, alloc_errors, 1);
    LOG_MSG_ERROR_3("ps_memi_pool_init(): "
                    "Incorrect mem levels, num_buf %d, high %d, low %d",
                    num_buf, high_wm, low_wm);
    return -1;
  }

  if ( (buf_size == 0) || (buf_size % sizeof(int32)) != 0 )
  {
    MEM_INC_INSTANCE_STATS(pool_id, alloc_errors, 1);
    LOG_MSG_ERROR_1("ps_memi_pool_init(): "
                    "Incorrect mem size %d", buf_size);
    return -1;
  }

  if (ps_mem_pool_info[pool_id].size != 0)
  {
    MEM_INC_INSTANCE_STATS(pool_id, alloc_errors, 1);
    LOG_MSG_ERROR_1("ps_memi_pool_init(): "
                    "Pool %d is inited already", pool_id);
    return -1;
  }

  PS_ENTER_CRIT_SECTION(&ps_mem_crit_section);

  /*-------------------------------------------------------------------------
    Initialize the control block
  -------------------------------------------------------------------------*/
  memset(&ps_mem_pool_info[pool_id], 0, sizeof(ps_mem_pool_info_s_type));

  ps_mem_pool_info[pool_id].size         = buf_size;
  ps_mem_pool_info[pool_id].total_num    = num_buf;
  ps_mem_pool_info[pool_id].hi_watermark = high_wm;
  ps_mem_pool_info[pool_id].lo_watermark = low_wm;
  ps_mem_pool_info[pool_id].mem_avail_cb = mem_avail_cb_f_ptr;
  ps_mem_pool_info[pool_id].is_static = is_static;
#ifdef FEATURE_DATA_PS_MEM_DEBUG
  ps_mem_pool_info[pool_id].buf_hdr_arr = buf_hdr_arr;
  ps_mem_pool_info[pool_id].buf_ptr_arr = buf_ptr_arr;
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

  /*-------------------------------------------------------------------------
    Update first_element and free_list of ps_mem_pool_info
  -------------------------------------------------------------------------*/
  ps_mem_pool_info[pool_id].first_element = buf_memory;
  ps_mem_pool_info[pool_id].free_list     = buf_memory;

  /*-------------------------------------------------------------------------
    For the dynamic case, reset the hdr and ptr arrays while we still have
    the num_buf value.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_MEM_DEBUG
  if( FALSE == is_static )
  {
    if( buf_hdr_arr != NULL )
    {
      memset( buf_hdr_arr,0,num_buf * sizeof(void *));
    }
    if( buf_ptr_arr != NULL )
    {
      memset( buf_ptr_arr,0,num_buf * sizeof(void *));
    }
  } /* dynamic case */
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

  /*-------------------------------------------------------------------------
    Now link all the buffers together, last buffer will be linked at the end
    of this loop.
  -------------------------------------------------------------------------*/
  last = &ps_mem_pool_info[pool_id].free_list;

  while( num_buf-- != 0 )
  {
    *last = buf_memory;
    last =  (int32**)buf_memory;

    if( TRUE == is_static )
    {

      ((ps_mem_buf_hdr_type*)buf_memory)->index =
        ps_mem_pool_info[pool_id].total_num - num_buf - 1;

      /*---------------------------------------------------------------------
        Update buf_ptr to point to starting address of structure in ps_mem
        buf and buf_hdr to point to the first buffer header.
      ---------------------------------------------------------------------*/
#ifdef FEATURE_DATA_PS_MEM_DEBUG
      if (buf_hdr_arr != NULL)
      {
        *buf_hdr_arr = (int32)buf_memory;
         buf_hdr_arr++;
      }

      if (buf_ptr_arr != NULL)
      {
        *buf_ptr_arr = (uint32) ((char*)buf_memory + PS_MEM_BUF_HDR_SIZE);
        buf_ptr_arr++;
      }
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
      /* use pointer math to avoid alignment warnings from casting */
      buf_memory += ( buf_size + PS_MEM_BUF_HDR_SIZE )/sizeof(int32);
    }
    else
    {
      buf_memory++;
    } /* dynamic */
  } /* while num_buf-- */

  ps_mem_pool_info[pool_id].last_element = (int32*) last;
  *last = NULL;   /* Last buffer in the pool */

  PS_LEAVE_CRIT_SECTION(&ps_mem_crit_section);
  return 0;

} /* ps_memi_pool_init() */


/*===========================================================================

                        PUBLIC FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION      PS_MEM_INIT()

DESCRIPTION   This function is used to initialize ps_mem module

RETURN VALUE  None

DEPENDENCIES  Called only once from PS_TASK
===========================================================================*/
void ps_mem_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_mem_initialized == FALSE)
  {
    PS_INIT_CRIT_SECTION(&ps_mem_crit_section);
    ps_mem_initialized = TRUE;
  }

} /* ps_mem_init() */

/*===========================================================================
FUNCTION      PS_MEM_DEINIT()

DESCRIPTION   This function is used to cleanup ps_mem module

RETURN VALUE  None

DEPENDENCIES  Called only once from PS_TASK
===========================================================================*/
void ps_mem_deinit
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (ps_mem_initialized == TRUE)
  {
    PS_DESTROY_CRIT_SECTION(&ps_mem_crit_section);
    ps_mem_initialized = FALSE;
  }

} /* ps_mem_init() */

/*===========================================================================
FUNCTION      PS_MEM_POOL_INIT()

DESCRIPTION   This function is used to initialize the PS Memory buffer pools.
              It creates the memory pool based on the different pool id
              (size) and number of buffers for this pool id, as defined in
              ps_mem_pool_info. As such, these numbers should be tuned to
              achieve minimum yet sufficient memory usage.

PARAMETERS
  pool_id                       - Pool ID Number 1..PS_MEM_MAX_POOLS-1
  buf_memory                    - Integer-aligned memory buffer location
                                  that is (buf_size+hdr_size) * num_buf in size.
  buf_size                      - Size of each buffer or structure
  num_buf                       - Number of buffers
  high_wm                       - Callback trigger for high watermark
  low_wm                        - Callback trigger for low watermark
  mem_avail_cb_f_ptr            - Callback that is triggered by watermarks
  buf_hdr_arr                   - Array of pointers to buffer headers
  buf_ptr_arr                   - Array of pointers to buffer payloads
                                  Suggest to make these of the type in use
RETURN VALUE  None

DEPENDENCIES  Called only once by each client
===========================================================================*/
int32 ps_mem_pool_init
(
  ps_mem_pool_enum_type      pool_id,
  int                      * buf_memory,
  uint16                     buf_size,
  int16                      num_buf,
  int16                      high_wm,
  int16                      low_wm,
  ps_mem_wm_cb_f_ptr_type    mem_avail_cb_f_ptr,
  int                        buf_hdr_arr[],
  int                        buf_ptr_arr[]
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_memi_pool_init( pool_id,
                            (int32*)buf_memory,
                            buf_size,
                            num_buf,
                            high_wm,
                            low_wm,
                            mem_avail_cb_f_ptr,
                            (int32*)buf_hdr_arr,
                            (int32*)buf_ptr_arr,
                            TRUE);
} /* ps_mem_pool_init */

/*===========================================================================
FUNCTION      PS_MEM_DYNAMIC_POOL_INIT()

DESCRIPTION   This function is used to initialize the PS Memory buffer pools.
              It creates the memory pool based on the different pool id
              (size) and number of buffers for this pool id, as defined in
              ps_mem_pool_info. As such, these numbers should be tuned to
              achieve minimum yet sufficient memory usage.
              This call enables the use of system heap for buffers.

PARAMETERS
  pool_id                       - Pool ID Number 1..PS_MEM_MAX_POOLS-1
  ps_mem_meta_data              - Allocation for integer pointer array
                                  that is sizeof(int*) *  num_buf in size.
  buf_size                      - Size of each buffer or structure
  num_buf                       - Number of buffers
  high_wm                       - Callback trigger for high watermark
  low_wm                        - Callback trigger for low watermark
  mem_avail_cb_f_ptr            - Callback that is triggered by watermarks
  buf_hdr_arr                   - Array of pointers to buffer headers
  buf_ptr_arr                   - Array of pointers to buffer payloads
                                  Suggest to make these of the type in use
RETURN VALUE  0 Success, -1 Failure

DEPENDENCIES  Called only once by each client
===========================================================================*/
int32 ps_mem_dynamic_pool_init
(
  ps_mem_pool_enum_type      pool_id,
  int                      * ps_mem_meta_data, /* num_buf * sizeof(int*) */
  uint16                     buf_size,
  int16                      num_buf,
  int16                      high_wm,
  int16                      low_wm,
  ps_mem_wm_cb_f_ptr_type    mem_avail_cb_f_ptr,
  int                        buf_hdr_arr[],
  int                        buf_ptr_arr[]
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  return ps_memi_pool_init( pool_id,
                            (int32*)ps_mem_meta_data,
                            buf_size,
                            num_buf,
                            high_wm,
                            low_wm,
                            mem_avail_cb_f_ptr,
                            (int32*)buf_hdr_arr,
                            (int32*)buf_ptr_arr,
                            FALSE);
} /* ps_mem_pool_init_dynamic */


/*===========================================================================
FUNCTION     PS_MEM_GET_BUF()

DESCRIPTION  This function is used for dynamic buffer allocation of buffers
             for a specified poolid.

RETURN VALUE If successful, a pointer to the allocated memory block is
             returned. Otherwise, a NULL pointer is returned.

DEPENDENCIES None
===========================================================================*/
void *ps_mem_get_buf
(
  ps_mem_pool_enum_type poolid
)
{
  int32                * buffer = NULL;
  ps_mem_buf_hdr_type  * buf_hdr = NULL;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ((poolid >= PS_MEM_MAX_POOLS) || (ps_mem_pool_info[poolid].size == 0) )
  {
    LOG_MSG_ERROR_1("ps_mem_get_buf():poolid: %d"
                    "is Incorrect or not initialized", poolid);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Allocate before critical section
  -------------------------------------------------------------------------*/
  if( ps_mem_pool_info[poolid].is_static == FALSE )
  {
    PS_SYSTEM_HEAP_MEM_ALLOC(buf_hdr, 
                             ps_mem_pool_info[poolid].size + PS_MEM_BUF_HDR_SIZE,
                             ps_mem_buf_hdr_type*);
    if( buf_hdr == NULL )
    {
      MEM_INC_INSTANCE_STATS(poolid,alloc_errors, 1);
      return NULL;
    }
  }

  /*-------------------------------------------------------------------------
     CRITICAL SECTION 
  -------------------------------------------------------------------------*/

  PS_ENTER_CRIT_SECTION(&ps_mem_crit_section);

  /*-------------------------------------------------------------------------
    Find a free buffer
    This next line is part of free list de-queueing couplet placed here
    for LINT.
  -------------------------------------------------------------------------*/
  buffer = ps_mem_pool_info[poolid].free_list;
  if ( buffer != NULL )
  {
    if ( ps_mem_pool_info[poolid].hi_watermark <=
         ps_mem_pool_info[poolid].num_used )
    {
      MEM_INC_INSTANCE_STATS(poolid,high_wm_exceeds, 1);
      ps_mem_pool_info[poolid].is_high_wm_hit = TRUE;

      LOG_MSG_INFO1_2("ps_mem_get_buf(): "
                      "PS mem pool id %d exceed high wm %d",
                      poolid, ps_mem_pool_info[poolid].hi_watermark);
    }

    ps_mem_pool_info[poolid].num_used++;
    MEM_UPDATE_NUM_USED_BUFFERS(poolid);

    /*-----------------------------------------------------------------------
      Remove this buffer and link the next free buffer in Q.
      See assignment of buffer above before test for NULL
    -----------------------------------------------------------------------*/
    ps_mem_pool_info[poolid].free_list = (int32 *) *buffer;

    /*-----------------------------------------------------------------------
      Store the poolid in allocated buffer
    -----------------------------------------------------------------------*/
    if( ps_mem_pool_info[poolid].is_static == TRUE )
    {
      buf_hdr = (ps_mem_buf_hdr_type *) buffer;
      buf_hdr->poolid = (int32)poolid;
      buf_hdr->ref_cnt = 1;
      buffer += PS_MEM_BUF_HDR_SIZE/sizeof(int32);
    }
    else
    {
      if( buf_hdr != NULL )
      {
        buf_hdr->poolid = (int32)poolid;
        buf_hdr->ref_cnt = 1;       

        /*-------------------------------------------------------------------
          Pointer math creates proper index offset
        -------------------------------------------------------------------*/
        buf_hdr->index = buffer - ps_mem_pool_info[poolid].first_element;

        /*-------------------------------------------------------------------
          Save back-pointer to allocated buffer
        -------------------------------------------------------------------*/
        *buffer = (int32) buf_hdr;

        /*-------------------------------------------------------------------
          Pointer math advances beyond header
        -------------------------------------------------------------------*/
        buffer = (int32 *) (buf_hdr + 1);

#ifdef FEATURE_DATA_PS_MEM_DEBUG
        if( ps_mem_pool_info[poolid].buf_hdr_arr != NULL )
        {
          ps_mem_pool_info[poolid].buf_hdr_arr[buf_hdr->index] = (int32)buf_hdr;
        }
        if(  ps_mem_pool_info[poolid].buf_ptr_arr != NULL )
        {
          ps_mem_pool_info[poolid].buf_ptr_arr[buf_hdr->index] = (int32)buffer;
        }
#endif /* FEATURE_DATA_PS_MEM_DEBUG */

      } /* buf_hdr check */
    } /* dynamic pool */

    PS_BRANCH_LEAVE_CRIT_SECTION(&ps_mem_crit_section);

  }
  else
  {
    /*-----------------------------------------------------------------------
      Critical sections are used in stats, so leave on until done with stats
      -----------------------------------------------------------------------*/
    MEM_INC_INSTANCE_STATS(poolid,alloc_errors, 1);
#ifdef FEATURE_PS_MEM_TUNING
    ASSERT(ps_mem_pool_info[poolid].num_used ==
           ps_mem_pool_info[poolid].total_num);
#endif /* FEATURE_PS_MEM_TUNING */
    PS_BRANCH_LEAVE_CRIT_SECTION(&ps_mem_crit_section);
    LOG_MSG_ERROR_1("ps_mem_get_buf(): "
                    "Out of memory in pool %d", poolid);
    if( ps_mem_pool_info[poolid].is_static == FALSE )
    {
      PS_SYSTEM_HEAP_MEM_FREE(buf_hdr);
    }
  }

  return ((void *)buffer);

} /* ps_mem_get_buf() */

/*===========================================================================
FUNCTION     PS_MEM_IS_VALID()

DESCRIPTION  This function validates that this is memory item is of the
             specified poolid type.

PARAMETERS   buf_ptr: pointer to the memory item being checked

RETURN VALUE TRUE if the item is valid
             FALSE otherwise

DEPENDENCIES None

SIDE EFFECTS None
===========================================================================*/
boolean ps_mem_is_valid
(
  void                 *buf_ptr,
  ps_mem_pool_enum_type poolid
)
{
  ps_mem_buf_hdr_type  * buf_hdr;
  int16 index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (buf_ptr == NULL)
  {
    LOG_MSG_ERROR_0("ps_mem_is_valid(): "
                    "NULL ptr");
    return FALSE;
  }

  /* use pointer math to skip back to the header */
  buf_hdr = ((ps_mem_buf_hdr_type *)(buf_ptr)) - 1;

  /*-------------------------------------------------------------------------
    If there is an error in pool id return error. Pool ids start from 1.
  -------------------------------------------------------------------------*/
  if (poolid <= PS_MEM_DUMMY_POOL_TYPE || poolid >= PS_MEM_MAX_POOLS)
  {
    LOG_MSG_ERROR_1("ps_mem_is_valid(): "
                    "Invalid pool %d", poolid);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Indicate an error here, this can happen only when user is trying to
    deallocate a wrong buffer or the pool id in buffer got corrupted.
  -------------------------------------------------------------------------*/
  if ((int32)poolid != buf_hdr->poolid)
  {
    LOG_MSG_ERROR_2("ps_mem_is_valid(): "
                    "Pool %d doesnt match pool %d in buf_hdr",
                    poolid, buf_hdr->poolid);
    return FALSE;
  }

  if( ps_mem_pool_info[poolid].is_static == TRUE )
  {
    /*-----------------------------------------------------------------------
      Check that the buffer belongs to the pool in the header
    -----------------------------------------------------------------------*/
    if ((int32 *) buf_hdr < ps_mem_pool_info[poolid].first_element ||
        (int32 *) buf_hdr > ps_mem_pool_info[poolid].last_element)
    {
      LOG_MSG_ERROR_2("ps_mem_is_valid(): "
                      "Buf 0x%p is not of pool %d", buf_hdr, poolid);
      return FALSE;
    }

#ifdef FEATURE_DATA_PS_MEM_DEBUG
    /*-----------------------------------------------------------------------
      Check that buffer is a multiple of (buf_size + ps_mem_hdr_size)
      Somewhat costly due to the % operator, so only if PS_MEM_DEBUG
    -----------------------------------------------------------------------*/
    if ((uint32)((char *) buf_hdr - (char *) ps_mem_pool_info[poolid].first_element) %
        (ps_mem_pool_info[poolid].size + PS_MEM_BUF_HDR_SIZE) != 0 )
    {
      LOG_MSG_ERROR_2("ps_mem_is_valid(): "
                      "Invalid addr: buffer 0x%p pool %d", buf_hdr, poolid);
      return FALSE;
    }
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
  } /* static */
  else
  {
    /*-----------------------------------------------------------------------
      Verify meta index leads back to buffer
    -----------------------------------------------------------------------*/
    index = buf_hdr->index;

    if (index < 0 || index >= ps_mem_pool_info[poolid].total_num)
    {
      LOG_MSG_ERROR_2 ("ps_mem_is_valid(): "
                       "Invalid index %d, pool id %d", index, poolid);
      return FALSE;
    }

    if ((int *) ps_mem_pool_info[poolid].first_element[index] !=
          (int *) buf_hdr)
    {
      LOG_MSG_ERROR_3("ps_mem_is_valid(): "
                      "Invalid dynamic addr: buffer 0x%p pool %d index %d",
                      buf_hdr, poolid, index);
      return FALSE;
    }
  } /* dynamic */

  return TRUE;

} /* ps_mem_is_valid() */



/*===========================================================================
FUNCTION     PS_MEM_FREE()

DESCRIPTION  This function is used for deallocating the memory. It also
             checks for the possible memory corruptions.

RETURN VALUE None

DEPENDENCIES None
===========================================================================*/
void ps_mem_free
(
  void *buf
)
{
  ps_mem_buf_hdr_type  * buf_hdr;
  int32                * buffer;
  int32                  poolid;
  boolean                invoke_mem_avail_cback = FALSE;
  int16                  index;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (buf == NULL)
  {
    LOG_MSG_ERROR_0("ps_mem_free(): "
                    "Null buffer");
    return;
  }

  PS_ENTER_CRIT_SECTION(&ps_mem_crit_section);

  /* use pointer math to move to beginning of header */
  buf_hdr = ((ps_mem_buf_hdr_type *) buf) - 1;

  if ( ps_mem_is_valid(buf, (ps_mem_pool_enum_type)buf_hdr->poolid) != TRUE )
  {
    LOG_MSG_ERROR_1("ps_mem_free(): "
                    "Invalid buf 0x%p", buf);
    PS_BRANCH_LEAVE_CRIT_SECTION(&ps_mem_crit_section);
    return;
  }

  poolid = buf_hdr->poolid;

  /*-------------------------------------------------------------------------
    Linked list buffer is different in static and dynamic cases
   -------------------------------------------------------------------------*/
  if( ps_mem_pool_info[poolid].is_static == TRUE )
  {
    buffer  = (int32 *) buf_hdr;
  }
  else
  {
    /*-----------------------------------------------------------------------
      Calculate index of buffer using index metadata in item
      Error checking comes with ps_mem_buf_to_index
    -----------------------------------------------------------------------*/
    index =  ps_mem_buf_to_index( buf );
    if( index < 0 )
    {
      PS_BRANCH_LEAVE_CRIT_SECTION(&ps_mem_crit_section);
      return;
    }

    buffer = (int32*)&(ps_mem_pool_info[poolid].first_element[index]);

#ifdef FEATURE_DATA_PS_MEM_DEBUG
    if( ps_mem_pool_info[poolid].buf_hdr_arr != NULL )
    {
      ps_mem_pool_info[poolid].buf_hdr_arr[index] = (int32)NULL;
    }
    if( ps_mem_pool_info[poolid].buf_ptr_arr != NULL )
    {
      ps_mem_pool_info[poolid].buf_ptr_arr[index] = (int32)NULL;
    }
#endif /* FEATURE_DATA_PS_MEM_DEBUG */
  } /* else dynamic */

  if (--(buf_hdr->ref_cnt) != 0)
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&ps_mem_crit_section);
    return;
  }

  /*-------------------------------------------------------------------------
    Now link the buffer back to the free pool.
  -------------------------------------------------------------------------*/
  *buffer = (int) ps_mem_pool_info[poolid].free_list;
  ps_mem_pool_info[poolid].free_list = buffer;

  MEM_DEC_INSTANCE_STATS(poolid, num_used_buffers, 1);

  ps_mem_pool_info[poolid].num_used--;

  if (ps_mem_pool_info[poolid].mem_avail_cb != NULL &&
      ps_mem_pool_info[poolid].num_used ==
        ps_mem_pool_info[poolid].lo_watermark &&
      ps_mem_pool_info[poolid].is_high_wm_hit == TRUE)
  {
    ps_mem_pool_info[poolid].is_high_wm_hit = FALSE;
    invoke_mem_avail_cback = TRUE;
  }

  PS_LEAVE_CRIT_SECTION(&ps_mem_crit_section);

  /*-------------------------------------------------------------------------
    Free the allocated memory starting buf_hdr in the dynamic case.
    The buf_hdr variable will be set to NULL as a side effect.
  -------------------------------------------------------------------------*/
  if( ps_mem_pool_info[poolid].is_static == FALSE )
  {
    PS_SYSTEM_HEAP_MEM_FREE(buf_hdr); 
  }

  /*-------------------------------------------------------------------------
    Need to call the callback outside the critical section so that deadlock
    doesn't occur, if callback function enters another critical section.

    TASK A has global_ps_crit_section and is waiting on ps_mem_crit_section.
    TASK B has ps_mem_crit_section and is waiting on global_ps_crit_section
    in the memory available callback.
  -------------------------------------------------------------------------*/
  if (invoke_mem_avail_cback)
  {
    LOG_MSG_INFO1_1("ps_mem_free(): "
                    "Calling low wm callback for pool %d", poolid);
    ps_mem_pool_info[poolid].mem_avail_cb
    (
      ps_mem_pool_info[poolid].lo_watermark
    );
  }
} /* ps_mem_free() */

/*===========================================================================
FUNCTION     PS_MEM_REG_MEM_AVAIL_CB()

DESCRIPTION  This function is used to register for the Memory available
             callback. The low watermark for each poolid is a compile time
             constant.  When the number of free items equals the low watermark
             value on the way up, the registered callback is called.

RETURN VALUE None

DEPENDENCIES None
===========================================================================*/
void ps_mem_reg_mem_avail_cb
(
  ps_mem_pool_enum_type    poolid,
  ps_mem_wm_cb_f_ptr_type  f_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(poolid >= PS_MEM_MAX_POOLS)
  {
    LOG_MSG_ERROR_2("ps_mem_reg_mem_avail_cb(): "
                    "Pool ID %d meets or exceeds MAX POOLS %d",
                    (uint32)poolid, (uint32)PS_MEM_MAX_POOLS);
    return;
  }

  ps_mem_pool_info[poolid].mem_avail_cb = f_ptr;

} /* ps_mem_reg_mem_avail_cb() */


/*===========================================================================
FUNCTION     PS_MEM_LESS_THAN_FEW_FREE_ITEMS()

DESCRIPTION  This function is used to query whether the number of free items
is below the low watermark or not.

RETURN VALUE
  TRUE if the number of free items is less than low watermark
  else FALSE.

DEPENDENCIES None
===========================================================================*/
boolean ps_mem_less_than_few_free_items
(
  ps_mem_pool_enum_type poolid
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(poolid >= PS_MEM_MAX_POOLS)
  {
    LOG_MSG_ERROR_2("ps_mem_less_than_few_free_items(): "
                    "Pool ID %d meets or exceeds MAX POOLS %d",
                    (uint32)poolid, (uint32)PS_MEM_MAX_POOLS);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Can't use the check
    "ps_mem_pool_info[poolid].num_used < ps_mem_pool_info[poolid].lo_watermark"
    If used, applications will be blocked from writing data even when
    high WM is not hit if lo WM < num_used < high WM
  -------------------------------------------------------------------------*/
  if ((ps_mem_pool_info[poolid].total_num -
         ps_mem_pool_info[poolid].num_used) <
       ps_mem_pool_info[poolid].lo_watermark)
  {
    return TRUE;
  }

  return FALSE;

} /* ps_mem_less_than_few_free_items() */


/*==========================================================================
FUNCTION     PS_MEM_INDEX_TO_BUF()

DESCRIPTION  This function returns a pointer to a buffer given an index into
             the corresponding memory pool.

RETURN VALUE If successful, a pointer to the corresponding memory block is
             returned. Otherwise, a NULL pointer is returned.

DEPENDENCIES None
===========================================================================*/
void *ps_mem_index_to_buf
(
  int16                  index,     /* Index in to the memory pool         */
  ps_mem_pool_enum_type poolid     /* pool id of the memory pool           */
)
{
  int32 *buffer;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(poolid >= PS_MEM_MAX_POOLS)
  {
    LOG_MSG_ERROR_1("ps_mem_index_to_buf(): Invalid poolid %d", poolid);
    ASSERT(0);
    return NULL;
  }

  if (index < 0 || index >= ps_mem_pool_info[poolid].total_num)
  {
    LOG_MSG_INFO1_2("ps_mem_index_to_buf(): "
                    "Index (%d) out of range for pool %d", index, poolid);
    return NULL;
  }

  if( ps_mem_pool_info[poolid].is_static == TRUE )
  {
    /*-----------------------------------------------------------------------
      Total size of buffers is converted to integer blocks and then applied
      with "pointer math" to avoid alignment/casting warnings.
    -----------------------------------------------------------------------*/
    buffer = ps_mem_pool_info[poolid].first_element +
             ( index *
               ( ps_mem_pool_info[poolid].size + PS_MEM_BUF_HDR_SIZE )/sizeof(int32) );

    /*-----------------------------------------------------------------------
      Make sure that buffer is allocated by comparing value in
      PS_MEM_BUF_HDR_SIZE to poolid.
    -----------------------------------------------------------------------*/
    if ((int32)poolid != *buffer)
    {
      LOG_MSG_ERROR_2("ps_mem_index_to_buf(): "
                      "Buf of index %d is not allocated, pool %d",
                      index, poolid);
      return NULL;
    }


  } /* static */
  else
  {
    buffer = (int32 *) ps_mem_pool_info[poolid].first_element[index];

    if (buffer == NULL)
    {
      LOG_MSG_ERROR_2("ps_mem_index_to_buf(): "
                      "Dynamic home entry is NULL for index %d, pool %d",
                      index, poolid);
      return NULL;
    }

    if ( ((ps_mem_buf_hdr_type *)buffer)->index != index )
    {
      return NULL;
    }
  } /* dynamic */

  /*-------------------------------------------------------------------------
     Skip PS_MEM_BUF_HDR_SIZE
    -------------------------------------------------------------------------*/
  return (void *) (((char *) buffer) + PS_MEM_BUF_HDR_SIZE);

} /* ps_mem_index_to_buf() */



/*==========================================================================
FUNCTION     PS_MEM_BUF_TO_INDEX()

DESCRIPTION  This function returns an index given a pointer to a buffer.

RETURN VALUE If successful, an index is returned. Otherwise, -1 is returned.

DEPENDENCIES None
===========================================================================*/
int16 ps_mem_buf_to_index
(
  void *buf
)
{
  ps_mem_buf_hdr_type *    buf_hdr;
  ps_mem_pool_enum_type    poolid;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (NULL == buf)
  {
    LOG_MSG_INFO1_0("ps_mem_buf_to_index(): "
                    "NULL buf");
    return -1;
  }

  buf_hdr = ((ps_mem_buf_hdr_type *)buf) - 1;
  poolid = (ps_mem_pool_enum_type) buf_hdr->poolid;

  /*-------------------------------------------------------------------------
    Return error if the buffer is tampered with
  -------------------------------------------------------------------------*/
  if (FALSE == ps_mem_is_valid(buf, poolid))
  {
    LOG_MSG_INFO1_0("ps_mem_buf_to_index(): "
                    "Invalid buf");
    return -1;
  }

  /* check range of index */
  if( buf_hdr->index < 0 || 
      buf_hdr->index >= ps_mem_pool_info[poolid].total_num )
  {
    LOG_MSG_ERROR_3("ps_mem_buf_to_index(): "
                    "Index range error: Buffer header 0x%p : index %d : Max %d ",
                    buf_hdr,
                    buf_hdr->index,
                    ps_mem_pool_info[poolid].total_num - 1);
        return -1;
  }

  return buf_hdr->index;
} /* ps_mem_buf_to_index() */



/*===========================================================================
FUNCTION     PS_MEM_DUP()

DESCRIPTION  This function is used to DUP a memory item.

RETURN VALUE None

DEPENDENCIES None
===========================================================================*/
void* ps_mem_dup
(
  void *buf
)
{
  int32                  poolid;
  ps_mem_buf_hdr_type  * buf_hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if ( buf == NULL )
  {
    LOG_MSG_INFO2_0("ps_mem_dup(): "
                    "NULL ptr");
    return NULL;
  }

  PS_ENTER_CRIT_SECTION(&ps_mem_crit_section);

  buf_hdr =  ((ps_mem_buf_hdr_type *)buf) - 1;
  poolid = buf_hdr->poolid;

  if (!ps_mem_is_valid(buf, (ps_mem_pool_enum_type)poolid))
  {
    PS_BRANCH_LEAVE_CRIT_SECTION(&ps_mem_crit_section);
    LOG_MSG_ERROR_1("ps_mem_dup(): "
                    "Invalid buf 0x%p", buf);
    ASSERT(0);
    return NULL;
  }

  buf_hdr->ref_cnt++;
  
  PS_LEAVE_CRIT_SECTION(&ps_mem_crit_section);
  return buf;

} /* ps_mem_dup() */

#ifdef PS_EXT_MEM_POOLS
/*===========================================================================
FUNCTION     PS_EXT_MEM_GET_BUF()

DESCRIPTION  This function is used for dynamic buffer allocation of buffers
             for a specified "external" poolid.

RETURN VALUE If successful, a pointer to the allocated memory block is
             returned. Otherwise, a NULL pointer is returned.

DEPENDENCIES None
===========================================================================*/
void *ps_ext_mem_get_buf
(
  ps_ext_mem_pool_enum_type poolid
)
{

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (poolid >= PS_EXT_MEM_MAX_POOLS)
  {
    LOG_MSG_ERROR_1("ps_ext_mem_get_buf(): "
                    "Unsupported ext pool_id %d", poolid);
    return NULL;
  }

  /*-------------------------------------------------------------------------
    Do the external to internal MEMPool mapping
  -------------------------------------------------------------------------*/
  switch (poolid)
  {
    case PS_EXT_MEM_META_INFO_TYPE_EX:
      return ps_mem_get_buf(PS_MEM_META_INFO_TYPE_EX);

    case PS_EXT_MEM_RT_META_INFO_TYPE:
      return ps_mem_get_buf(PS_MEM_RT_META_INFO_TYPE);

    case PS_EXT_MEM_PKT_META_INFO_TYPE:
      return ps_mem_get_buf(PS_MEM_PKT_META_INFO_TYPE);

 #ifdef PS_EXT_TX_META_INFO_GET_ALL
    case PS_EXT_MEM_TX_META_INFO_TYPE:
      return ps_mem_get_buf(PS_MEM_TX_META_INFO_TYPE);
 #endif /* PS_EXT_TX_META_INFO_GET_ALL */

    default:
    LOG_MSG_ERROR_1("ps_ext_mem_get_buf(): "
                    "Unsupported ext pool_id %d", poolid);
    break;
  }

  return NULL;
}/* ps_ext_mem_get_buf */
#endif /* PS_EXT_MEM_POOLS */

/*===========================================================================
FUNCTION PS_MEM_GET_STAT_INSTANCE_ALL()

DESCRIPTION
  This function supplies all the MEM statistics for the instance handle
  supplied. This is needed because of the need to extract and format data
  available in the mem control block.  Located in ps_mem.c because
  ps_mem_pool_info_type is strictly local to ps_mem.c

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  pool_info_ptr   - IN - pointer that points to the pool info block
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_mem_get_stat_instance_all
(
  ps_stat_mem_enum_type    stat,
  void                     *pool_info_ptr,
  void                     *return_value,
  uint16                   ret_len
)
{
   ps_stat_mem_i_s_type *mem_i_stats;
   ps_mem_pool_info_s_type *pool_ptr;

  /*-------------------------------------------------------------------------
    validate 'stat'; only PS_STAT_MEM_ALL should be passed to this function
  -------------------------------------------------------------------------*/
  if (stat != PS_STAT_MEM_ALL)
  {
    DATA_ERR_FATAL("ps_mem_get_stat_instance_all(): Invalid stat value passed to ps_mem_get_stat_i_all");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    validate pool_info_ptr : should not be NULL
  -------------------------------------------------------------------------*/
  if (pool_info_ptr == NULL)
  {
    DATA_ERR_FATAL("ps_mem_get_stat_instance_all:Null value of pool_info_ptr passed");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    return_value validation : should not be null
  -------------------------------------------------------------------------*/

  if (return_value == NULL)
  {
    LOG_MSG_ERROR_0("ps_mem_get_stat_instance_all(): "
                    "return_value should not be NULL ");
    return E_NOT_ALLOWED ;
  }

  /*-------------------------------------------------------------------------
    check the sizeof the stat requested. If the size is zero, the stat is
    invalid in the current context. return appropriate error
  -------------------------------------------------------------------------*/

  if (ret_len < sizeof(ps_stat_mem_i_s_type))
  {
    LOG_MSG_ERROR_0("ps_mem_get_stat_instance_all(): "
                    " Insufficient memory allocated for ptr return_value");
    return E_NOT_ALLOWED ;
  }

  mem_i_stats = (ps_stat_mem_i_s_type *) return_value ;
  pool_ptr = (ps_mem_pool_info_s_type *) pool_info_ptr;

  mem_i_stats->alloc_errors       = pool_ptr->mem_i_stats.alloc_errors;
  mem_i_stats->high_wm_exceeds    = pool_ptr->mem_i_stats.high_wm_exceeds;
  mem_i_stats->max_alloc_buffers  = pool_ptr->mem_i_stats.max_alloc_buffers;
  mem_i_stats->num_used_buffers   = pool_ptr->mem_i_stats.num_used_buffers;

  pool_ptr->mem_i_stats.queried = PS_STAT_QUERIED;

  return E_SUCCESS ;
}


/*===========================================================================
FUNCTION PS_MEM_GET_STAT()

DESCRIPTION
  This function supplies the required mem layer statistics. The first
  argument passed to the function is an enum that denotes the metric of
  interest.The second argument will be  a pointer to the address of the
  ps_mem_pool_info element corresponding to the pool_id. The third argument
  points to the memory location where the results will be copied.The result
  (statistic) will be copied to this memory address. The fourth argument
  passes the length of memory allocated for the return value and should be
  greater than or equal to the memory size of the statistic requested.

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  stat            - IN - type of stat
  instance_ptr    - IN - pointer that has the value of socket descriptor
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_mem_get_stat
(
  ps_stat_mem_enum_type  stat,
  void                   *instance_ptr,
  void                   *return_value,
  uint16                 ret_len
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (stat == PS_STAT_MEM_ALL)
  {
    return ps_mem_get_stat_instance_all(stat,
                                        instance_ptr,
                                        return_value,
                                        ret_len);
  }
  else
  {
    return ps_stat_get_common(stat,
                              ps_stat_mem_i_table,
                              PS_STAT_MEM_ALL,
                              instance_ptr,
                              return_value,
                              ret_len);
  }
} /* ps_mem_get_stat() */


/*===========================================================================
FUNCTION PS_MEM_INST_GET_DESC_LEN()

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
static void ps_mem_inst_get_desc_len
(
  int32              handle,
  uint32            * len_ptr
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Need to add 1 onto the size for the NULL termination character
  -------------------------------------------------------------------------*/
  *len_ptr =
    (
      sizeof(ps_stat_inst_desc_mem_type) +
      strlen(ps_mem_pool_name_arr[handle]) +
      1
    );

} /* ps_mem_inst_get_desc_len */


/*===========================================================================
FUNCTION PS_MEM_INST_FILL_ONE_DESC()

DESCRIPTION
  This function fills the description of one memory pool into the log packet.

PARAMETERS
  handle                : Specifies which instance to retrieve the description
                          from
  *log_pkt_ptr          : Pointer to the log packet that the description will
                          be copied to
  log_pkt_remaining_len : Remaining size in packet to copy desc

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
static void ps_mem_inst_fill_one_desc
(
  int32                          handle,
  uint8                        * log_pkt_fill_ptr,
  size_t                         log_pkt_remaining_len
)
{
  ps_stat_inst_desc_mem_type   * mem_desc_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mem_desc_ptr = (ps_stat_inst_desc_mem_type *) log_pkt_fill_ptr;

  mem_desc_ptr->handle    = (uint8)handle;
  

  log_pkt_fill_ptr += sizeof(ps_stat_inst_desc_mem_type);
  log_pkt_remaining_len -= sizeof(ps_stat_inst_desc_mem_type);

  mem_desc_ptr->name_size  = strlcpy((void *)log_pkt_fill_ptr,
                                     ps_mem_pool_name_arr[handle],
                                     log_pkt_remaining_len);

} /* ps_mem_inst_fill_one_desc */


/*===========================================================================
FUNCTION PS_MEM_GET_DESC_LOG_PKT()

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
static void ps_mem_get_desc_log_pkt
(
  int32            handle
)
{
  uint8            idx                = 0;
  uint32           desc_size          = 0;
  uint8            inst_count         = 0;
  size_t           log_pkt_avail_size = 0;
  uint8          * log_pkt_ptr        = NULL;
  uint8          * log_pkt_fill_ptr   = NULL;

  ps_stat_desc_all_inst_hdr_log_pkt_type      * desc_all_inst_pkt_ptr = NULL;
  ps_stat_desc_delta_inst_hdr_log_pkt_type    * delta_pkt_ptr         = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_STAT_INST_DESC_ALL == handle)
  {
    for (idx = (PS_MEM_DUMMY_POOL_TYPE + 1); idx < PS_MEM_MAX_POOLS; ++idx)
    {
      ps_mem_inst_get_desc_len(idx, &desc_size);

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
                                    LOG_PS_STAT_DESC_ALL_MEM_POOL_INST_C,
                                    &log_pkt_ptr);

        /*-------------------------------------------------------------------
          If we fail to get a log packet, just return
        -------------------------------------------------------------------*/
        if (NULL == log_pkt_ptr)
        {
          LOG_MSG_ERROR_0("ps_mem_get_desc_log_pkt(): "
                          "Unable to allocate log packet");
          return;
        }

        /*-------------------------------------------------------------------
          Leave space for the describe all instance header
        -------------------------------------------------------------------*/
        log_pkt_fill_ptr =
          (log_pkt_ptr + sizeof(ps_stat_desc_all_inst_hdr_log_pkt_type));

        log_pkt_avail_size -= (int32)sizeof(ps_stat_desc_all_inst_hdr_log_pkt_type);

      }

      if (NULL != log_pkt_fill_ptr)
      {
        ps_mem_inst_fill_one_desc(idx, log_pkt_fill_ptr, log_pkt_avail_size);

       /*-------------------------------------------------------------------
          Increment the fill pointer by the size, and decrement the count
          by the same value
        -------------------------------------------------------------------*/
        log_pkt_fill_ptr += desc_size;
        log_pkt_avail_size -= desc_size;
        inst_count++;
      }
    } /* for all memory pools */

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
  } /* if all instance descriptions */
  else
  {
    ps_mem_inst_get_desc_len(handle, &desc_size);

    /*-----------------------------------------------------------------------
      We need to allocate enough memory for the description and the standard
      header
    -----------------------------------------------------------------------*/
    log_pkt_avail_size =
              (desc_size + (int32)sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type));

    ps_stat_logging_get_log_pkt(log_pkt_avail_size,
                                LOG_PS_STAT_DESC_DELTA_INST_C,
                                &log_pkt_ptr);

    /*-----------------------------------------------------------------------
      If we fail to get a log packet, just return
    -----------------------------------------------------------------------*/
    if (NULL == log_pkt_ptr)
    {
      LOG_MSG_ERROR_0("ps_mem_get_desc_log_pkt(): "
                      "Unable to allocate log packet");
      return;
    }

    /*-----------------------------------------------------------------------
      Fill in the delta log parameters
    -----------------------------------------------------------------------*/
    delta_pkt_ptr = (ps_stat_desc_delta_inst_hdr_log_pkt_type *) log_pkt_ptr;

    delta_pkt_ptr->type    = PS_STAT_MODULE_INST_MEM;
    delta_pkt_ptr->deleted = FALSE;
    memset(delta_pkt_ptr->reserved, 0, 2);

    log_pkt_fill_ptr =
      (log_pkt_ptr + (uint8) sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type));

    log_pkt_avail_size -= sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type);
    ps_mem_inst_fill_one_desc(handle, log_pkt_fill_ptr, log_pkt_avail_size);

    /*-------------------------------------------------------------------------
      Dispatch the packet
    -------------------------------------------------------------------------*/
    ps_stat_logging_commit_log_pkt
    (
      log_pkt_ptr,
      (desc_size + (int32)sizeof(ps_stat_desc_delta_inst_hdr_log_pkt_type))
    );
  } /* else fill one mem instance */
} /* ps_mem_get_desc_log_pkt */


/*===========================================================================
FUNCTION PS_MEM_GET_STAT_LOG_PKT()

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
static void ps_mem_get_stat_log_pkt
(
  int32               handle
)
{
  uint8                               idx                  = 0;
  uint8                             * log_pkt_ptr          = NULL;
  ps_mem_pool_info_s_type           * mem_cb_ptr           = NULL;
  ps_stat_inst_mem_log_pkt_type     * inst_mem_log_pkt_ptr = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (PS_STAT_INST_DESC_ALL == handle)
  {
    /*-----------------------------------------------------------------------
      Iterate through all mem pools.
    -----------------------------------------------------------------------*/
    for (idx = (PS_MEM_DUMMY_POOL_TYPE + 1); idx < PS_MEM_MAX_POOLS; ++idx)
    {
      /*---------------------------------------------------------------------
        Convert the handle into the instance control block
      ---------------------------------------------------------------------*/
      mem_cb_ptr = &ps_mem_pool_info[idx];

      ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_mem_log_pkt_type),
                                  LOG_PS_STAT_MEM_INST_C,
                                  &log_pkt_ptr);

      if (NULL == log_pkt_ptr)
      {
        LOG_MSG_ERROR_0("ps_mem_get_stat_log_pkt(): "
                        "Log packet allocated for MEM failed");
        return;
      }

      inst_mem_log_pkt_ptr = (ps_stat_inst_mem_log_pkt_type *) log_pkt_ptr;

      inst_mem_log_pkt_ptr->handle  = idx;

      (void) ps_stat_get_mem(PS_STAT_MEM_ALL,
                              (void *) mem_cb_ptr,
                              &(inst_mem_log_pkt_ptr->inst_mem_stats),
                              sizeof(ps_stat_mem_i_s_type));

      ps_stat_logging_commit_log_pkt
      (
        log_pkt_ptr,
        sizeof(ps_stat_inst_mem_log_pkt_type)
      );

    } /* for all mem instances*/
  } /* if fill all mem instances */
  else
  {
    if (PS_MEM_MAX_POOLS <= handle || PS_MEM_DUMMY_POOL_TYPE >= handle)
    {
      LOG_MSG_ERROR_1("ps_mem_get_stat_log_pkt(): "
                      "Invalid instance getting mem stats %d", handle);
      return;
    }
    /*---------------------------------------------------------------------
      Convert the handle into the instance control block
    ---------------------------------------------------------------------*/
    mem_cb_ptr = &ps_mem_pool_info[handle];

    ps_stat_logging_get_log_pkt(sizeof(ps_stat_inst_mem_log_pkt_type),
                                LOG_PS_STAT_MEM_INST_C,
                                &log_pkt_ptr);

    if (NULL == log_pkt_ptr)
    {
      LOG_MSG_ERROR_0("ps_mem_get_stat_log_pkt(): "
                      "Log packet allocated for MEM failed");
      return;
    }

    inst_mem_log_pkt_ptr = (ps_stat_inst_mem_log_pkt_type *) log_pkt_ptr;

    inst_mem_log_pkt_ptr->handle  = handle;

    (void) ps_stat_get_mem(PS_STAT_MEM_ALL,
                            (void *) mem_cb_ptr,
                            &(inst_mem_log_pkt_ptr->inst_mem_stats),
                            sizeof(ps_stat_mem_i_s_type));

    ps_stat_logging_commit_log_pkt
    (
      log_pkt_ptr,
      sizeof(ps_stat_inst_mem_log_pkt_type)
    );
  } /* else fill one mem instance */
} /* ps_mem_get_stat_log_pkt */

/*===========================================================================
FUNCTION PS_MEM_RESET_STAT()

DESCRIPTION
  This function resets the instance statistics associated with the specified
  instance.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle: Handle to the instance

SIDE EFFECTS
  None
===========================================================================*/
void ps_mem_reset_stat
(
  int32                     handle
)
{
  ps_mem_pool_info_s_type * mem_pool_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  mem_pool_ptr = &ps_mem_pool_info[handle];

  if (PS_STAT_QUERIED == mem_pool_ptr->mem_i_stats.queried)
  {
    ps_mem_get_stat_log_pkt(handle);
  }
  memset(&(mem_pool_ptr->mem_i_stats), 0, sizeof(mem_pool_ptr->mem_i_stats));

  return;
} /* ps_mem_reset_stat */


/*===========================================================================
FUNCTION PS_STAT_INIT_MEM()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_mem
(
  void
)
{
  ps_stat_logging_inst_reg_fill_desc_cback(PS_STAT_MODULE_INST_MEM,
                                           ps_mem_get_desc_log_pkt);

  ps_stat_logging_inst_reg_get_stat_cback(PS_STAT_MODULE_INST_MEM,
                                          ps_mem_get_stat_log_pkt);

  ps_stat_logging_inst_reg_reset_stat_cback(PS_STAT_MODULE_INST_MEM,
                                            ps_mem_reset_stat);
}


/*===========================================================================
FUNCTION PS_MEM_GET_POOLINFO_FROM_POOLID

DESCRIPTION
  This function supplies the address of the element of ps_mem_pool_info
  corresponding to poolid supplied

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  poolid       - denotes the pool number
  pool_address - a void ** pointer that is ued to pass back the address of
                 poolinfo corr. to poolid. This is a void ** so that the
                 void * value can be directlyused as the instance_ptr when
                 calling ps_stat_get_mem

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_mem_get_poolinfo_from_poolid
(
  ps_mem_pool_enum_type     poolid,
  void                   ** pool_address
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Input Validations
  -------------------------------------------------------------------------*/
  if(poolid < PS_MEM_TX_META_INFO_TYPE || poolid >= PS_MEM_MAX_POOLS)
  {
    return E_INVALID_ARG;
  }

  if(pool_address == NULL)
  {
    return E_INVALID_ARG;
  }

  *pool_address = (void *) (&ps_mem_pool_info[poolid]) ;
  return E_SUCCESS ;

} /* ps_mem_get_poolinfo_from_poolid() */



/*===========================================================================
FUNCTION PS_MEM_POOL_FREE

DESCRIPTION
  This function is INTERNAL ONLY and is used for testing purposes. Frees up
  the pool id used by STA ps_mem client

PARAMETERS
  poolid - the dummy pool

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ps_mem_pool_free
(
  ps_mem_pool_enum_type poolid
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if (poolid != PS_MEM_DUMMY_POOL_TYPE)
  {
    LOG_MSG_ERROR_1("ps_mem_pool_free(): Invalid poolid %d", poolid);
    ASSERT(0);
    return;
  }

  memset( &ps_mem_pool_info[poolid], 0, sizeof(ps_mem_pool_info_s_type));

} /* ps_mem_pool_free() */

#ifdef TEST_FRAMEWORK
#error code not present
#endif /* TEST_FRAMEWORK */

