/*===========================================================================
                        P S _ L O G G I N G _ D I A G . C

GENERAL DESCRIPTION
  This file contains data path logging request handler functions which are
  diag callback functions registered during powerup.  These callback
  functions are the interface between the DM (diagnostic tool) and the PS.
  Also provides access to the lookup table maintained locally.

EXTERNAL FUNCTIONS
  dpl_init()
    Registers functions with diag.

  dpli_set_ps_iface_ptr()
    Set ps_iface table in the lookup table.

  dpli_get_ps_iface_default_desc()
    Get default description of the ifname from the lookup table.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

Copyright (c) 2004-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

 $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/src/ps_logging_diag.c#1 $
 $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ---------------------------------------------------------- 
11/08/13    vl     Enabled default DPL logging for IWLAN S2B and WLAN PROXY
                   ifaces.
12/01/11    sa     Adding description for TLB Mode B Rm Ifaces in DPL list.
11/30/11    su     Adding log code support for DPL logging
03/22/11    kk     Adding description for new RmNet ifaces in DPL list.
10/01/10    sa     Added appropriate log messages before ASSERT(0).
07/09/10    sy     Fixed type-punned compiler warnings.
03/26/09    pp     CMI De-featurization.
21/11/08    am     Using new macros for DPL IID flags.
10/17/08    am     Fixed ANSI C warnings for bit-field usage.
09/04/08    rt     Added IWLAN iface for logging.
09/04/08    ssh    Removed featurization from ps_iface_dpl_lookup_table
09/01/08    ssh    Added desc for RmNet and UICC ifaces to lookup table
07/18/08    dm     Fixed compiler warnings
02/14/07    scb    Fixed Critical LINT errors
02/08/07    scb    Fixed Klocwork High errors
12/25/06    msr    Fixed broken secondary link logging
07/17/06    mp     Fixed logging of zero length DPL packets
06/05/06    mp     Moved DPL link logging control block from ps_iface to
                   ps_phys_link
04/04/06    jd     Cleaned up include files
02/22/06    msr    Using single critical section
02/06/06    msr    Updated for L4 tasklock/crit sections.
08/16/05    msr    Fixed PS_BRANCH_TASKFREE()
08/15/05    msr    Using ps_flow instead of ps_phys_link for flow_logging
05/31/05   aku/jd  Added WLAN iface to ifname list
05/12/05    ks     Fixed Lint errors.
03/14/05    ks     Fixed handling of wildcard for link logging
01/27/05    ssh    Changed include file name dsbyte.h to ps_byte.h
01/12/05    msr    Added code review comments.
01/10/05    msr    Added support for flow and link logging.
11/16/04    ks     Changed the check for duplicate IIDs in set logging
                   request.
11/16/04    msr    Changed the condition which checks if DPL_IID_IFNAME_MAX
                   is greater than MAX_SYSTEM_IFACES.
11/08/04    msr    Added the condition, which checks if DPL_IID_IFNAME_MAX is
                   greater than MAX_SYSTEM_IFACES.
11/2/04    ks/msr  Changed Diag cmd handler code's names.
11/1/04   msr/ks   Generating correct responses when diag tool
                   sends illegal requests
10/31/04  msr/ks   Clean up.
09/10/04  ks/msr   Major cleanup.
08/09/04    vd     created file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"
#include "target.h"
#include "customer.h"



#include <stringl/stringl.h>
#include "diagcmd.h"
#include "diagpkt.h"

#include "ps_iface.h"
#include "ps_phys_link.h"
#include "ps_byte.h"
#include "ps_logging_defs.h"
#include "ps_iface_logging.h"
#include "ps_logging_diag.h"
#include "ps_loggingi.h"
#include "ps_utils.h"
#include "ps_crit_sect.h"
#include "ds_Utils_DebugMsg.h"

#include "log.h"
#include "log_codes.h"

#include "rcinit_rex.h"
#include "ps_dpm_hw.h"

/*===========================================================================

                                  CONSTANTS

===========================================================================*/
/*---------------------------------------------------------------------------
  Size of header added to each DIAG request/response
---------------------------------------------------------------------------*/
#define DIAG_HDR_LEN  (sizeof(diagpkt_subsys_header_type))

/*---------------------------------------------------------------------------
  Offset in to DIAG header where sub system command code (SSCC) is found
---------------------------------------------------------------------------*/
#define DIAG_HDR_SSCC_OFFSET  2

/*===========================================================================

                                VARIABLES

===========================================================================*/

dpl_state_enum_type dpl_state = DPL_DISABLED;

/*---------------------------------------------------------------------------
  Type used to define lookup table
---------------------------------------------------------------------------*/
typedef struct ps_iface_dpl_lookup_info
{
  ps_iface_type         * ps_iface_ptr;
  char                    desc[DPL_IFNAME_DESC_S_LEN];
} ps_iface_dpl_lookup_info_type;

/*---------------------------------------------------------------------------
  Lookup table used to store default description of each ps_iface and
  pointer to each ps_iface.
---------------------------------------------------------------------------*/
static
  ps_iface_dpl_lookup_info_type ps_iface_dpl_lookup_table[DPL_IID_IFNAME_MAX] =
{
  /* ifname  ps_iface_ptr   description                                    */
  /* 0  */    {NULL,         "NULL"                      } /* Must not be
                                                 used          */
  /* 1  */    ,{NULL,         "CDMA SN IFACE 1"           }
  /* 2  */    ,{NULL,         "CDMA SN IFACE 2"           }
  /* 3  */    ,{NULL,         "CDMA SN IFACE 3"           }
  /* 4  */    ,{NULL,         "CDMA SN IFACE 4"           }
  /* 5  */    ,{NULL,         "CDMA SN IFACE 5"           }
  /* 6  */    ,{NULL,         "CDMA SN IFACE 6"           }
  /* 7  */    ,{NULL,         "CDMA SN IFACE 7"           }
  /* 8  */    ,{NULL,         "CDMA SN IFACE 8"           }
  /* 9  */    ,{NULL,         "CDMA SN IFACE 9"           }
  /* 10 */    ,{NULL,         "CDMA SN IFACE 10"          }
  /* 11 */    ,{NULL,         "CDMA SN IFACE 11"          }
  /* 12 */    ,{NULL,         "CDMA SN IFACE 12"          }
  /* 13 */    ,{NULL,         "CDMA SN IFACE 13"          }
  /* 14 */    ,{NULL,         "CDMA SN IFACE 14"          }
  /* 15 */    ,{NULL,         "CDMA SN IFACE 15"          }
  /* 16 */    ,{NULL,         "CDMA SN IFACE 16"          }
  /* 17 */    ,{NULL,         "CDMA Um SN ANY PKT"        }
  /* 18 */    ,{NULL,         "CDMA Um SN ASYNC"          }
  /* 19 */    ,{NULL,         "CDMA An "                  }
  /* 20 */    ,{NULL,         "Rm - SIO UMTS"             }
  /* 21 */    ,{NULL,         "Rm - SIO CDMA"             }
  /* 22 */    ,{NULL,         "CDMA Broadcast"            }
  /* 23 */    ,{NULL,         "IP Security"               }
  /* 24 */    ,{NULL,         "Loopback V4"               }
  /* 25 */    ,{NULL,         "Loopback V6"               }  
  /* 26 */    ,{NULL,         "DVBH IPv4 Iface "          }
  /* 27 */    ,{NULL,         "DVBH IPv6 Iface "          }
  /* 28 */    ,{NULL,         "Rm Network 1"              }
  /* 29 */    ,{NULL,         "Rm Network 2"              }
  /* 30 */    ,{NULL,         "Rm Network 3"              }
  /* 31 */    ,{NULL,         "Rm Network 4"              }
  /* 32 */    ,{NULL,         "Rm Network 5"              }
  /* 33 */    ,{NULL,         "Rm Network 6"              }
  /* 34 */    ,{NULL,         "Rm Network 7"              }
  /* 35 */    ,{NULL,         "Rm Network 8"              }
  /* 36 */    ,{NULL,         "Rm Network 9"              }
  /* 37 */    ,{NULL,         "Rm Network 10"             }
  /* 38 */    ,{NULL,         "Rm Network 11"             }
  /* 39 */    ,{NULL,         "Rm Network 12"             }
  /* 40 */    ,{NULL,         "Rm Network 13"             }
  /* 41 */    ,{NULL,         "Rm Network 14"             }
  /* 42 */    ,{NULL,         "Rm Network 15"             }  
  /* 43 */    ,{NULL,         "Rm Network 16"             }  
  /* 44 */    ,{NULL,         "UICC"                      }
  /* 45 */    ,{NULL,         "IWLAN 3GPP2"               }
  /* 46 */    ,{NULL,         "IWLAN 3GPP PDP 0"          }
  /* 47 */    ,{NULL,         "IWLAN 3GPP PDP 1"          }
  /* 48 */    ,{NULL,         "IWLAN 3GPP PDP 2"          }
  /* 49 */    ,{NULL,         "STA"                       }
  /* 50 */    ,{NULL,         "UW_FMC"                    }
  /* 51 */    ,{NULL,         "Proxy Iface 0"             }
  /* 52 */    ,{NULL,         "Proxy Iface 1"             }
  /* 53 */    ,{NULL,         "Proxy Iface 2"             }
  /* 54 */    ,{NULL,         "Proxy Iface 3"             }
  /* 55 */    ,{NULL,         "Proxy Iface 4"             }
  /* 56 */    ,{NULL,         "3GPP  Iface 0"             }
  /* 57 */    ,{NULL,         "3GPP  Iface 1"             }
  /* 58 */    ,{NULL,         "3GPP  Iface 2"             }
  /* 59 */    ,{NULL,         "3GPP  Iface 3"             }
  /* 60 */    ,{NULL,         "3GPP  Iface 4"             }
  /* 61 */    ,{NULL,         "3GPP  Iface 5"             }
  /* 62 */    ,{NULL,         "3GPP  Iface 6"             }
  /* 63 */    ,{NULL,         "3GPP Iface 7"              }
  /* 64 */    ,{NULL,         "3GPP Iface 8"              }
  /* 65 */    ,{NULL,         "3GPP Iface 9"              }
  /* 66 */    ,{NULL,         "3GPP Iface 10"             }
  /* 67 */    ,{NULL,         "3GPP Iface 11"             }
  /* 68 */    ,{NULL,         "3GPP Iface 12"             }
  /* 69 */    ,{NULL,         "3GPP Iface 13"             }
  /* 70 */    ,{NULL,         "3GPP Iface 14"             }
  /* 71 */    ,{NULL,         "3GPP Iface 15"             }
  /* 72 */    ,{NULL,         "3GPP Emergency 0"          }
  /* 73 */    ,{NULL,         "3GPP Emergency 1"          }
  /* 74 */    ,{NULL,         "NAT  Iface 1"              }  
  /* 75 */    ,{NULL,         "WLAN Iface"               }
  /* 76 */    ,{NULL,         "WLAN Network Adhoc"        }
  /* 77 */    ,{NULL,         "WLAN Network SoftAP"       }
  /* 78 */    ,{NULL,         "TLB Mode B V4 Rm Iface"    }
  /* 79 */    ,{NULL,         "TLB Mode B V6 Rm Iface"    }
  /* 80 */    ,{NULL,         "eMBMS Iface"               }
  /* 81 */    ,{NULL,         "Softap Iface"              }
  /* 82 */    ,{NULL,         "Modem Link V6 Iface"       }
  /* 83 */    ,{NULL,         "IWLAN S2B IFACE 1"         }
  /* 84 */    ,{NULL,         "IWLAN S2B IFACE 2"         }
  /* 85 */    ,{NULL,         "IWLAN S2B IFACE 3"         }
  /* 86 */    ,{NULL,         "IWLAN S2B IFACE 4"         }
  /* 87 */    ,{NULL,         "IWLAN S2B IFACE 5"         }
  /* 88 */    ,{NULL,         "IWLAN S2B IFACE 6"         }
  /* 89 */    ,{NULL,         "IWLAN S2B IFACE 7"         }
  /* 90 */    ,{NULL,         "IWLAN S2B IFACE 8"         }
  /* 91 */    ,{NULL,         "IWLAN S2B IFACE 9"         }
  /* 92 */    ,{NULL,         "IWLAN S2B IFACE 10"        }
  /* 93 */    ,{NULL,         "IWLAN S2B IFACE 11"        }
  /* 94 */    ,{NULL,         "IWLAN S2B IFACE 12"        }
  /* 95 */    ,{NULL,         "IWLAN S2B IFACE 13"        }
  /* 96 */    ,{NULL,         "IWLAN S2B IFACE 14"        }
  /* 97 */    ,{NULL,         "IWLAN S2B IFACE 15"        }
  /* 98 */    ,{NULL,         "IWLAN S2B IFACE 16"        }  
};


/*===========================================================================

                           LOCAL MACRO DEFINITIONS

===========================================================================*/

/**
  @brief To set protocol and snap length 
 
  This method sets the protocol, snap length and for an iface 
   
  @param[in]  dpl_cb_ptr  dpl callback pointer 
  @param[in]  snap_len    length of packet in bytes
  @param[in]  protocol    Protocol to log
 
  @return
    None 
  
  @dependencies 
    None
*/
#define DPLI_SET_SNAPLEN_PROTOCOL(dpl_cb_ptr, snap_len, protocol)   \
  dpl_cb_ptr->snaplen[protocol]  = snap_len;                        \
  dpl_cb_ptr->mask               |= (0x01 << (protocol - 1));       \

/*===========================================================================

                                  EXTERNAL FUNCTIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DPL_GET_DPL_STATE

DESCRIPTION
  This method will return current DPL state

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
dpl_state_enum_type dpl_get_dpl_state
(
  void
)
{
  return dpl_state;
}/*dpl_get_dpl_state*/


/*===========================================================================
FUNCTION    dpli_update_logging_config

DESCRIPTION
  This method enables the logging in iface, sets protocol and packet
  log length
 
PARAMETERS
  ps_iface_ptr[in]   Pointer to the interface 
  
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
 void dpli_update_logging_config
(
  ps_iface_type       *ps_iface_ptr,
  dpl_state_enum_type  dpl_state
)
{
  dpl_net_dir_cb_type  *tx_dpl = NULL;
  dpl_net_dir_cb_type  *rx_dpl = NULL;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if (!PS_IFACE_IS_VALID(ps_iface_ptr))
  {
    LOG_MSG_INFO3_1("dpli_update_logging_config(): iface 0x%x is invalid",
                      ps_iface_ptr);
    return;
  }

  /*-------------------------------------------------------------------------
    If interface is not RM, UMTS and CDMA_SN return
  -------------------------------------------------------------------------*/
  if (SIO_IFACE     != ps_iface_ptr->name &&
      UMTS_IFACE    != ps_iface_ptr->name &&
      CDMA_SN_IFACE != ps_iface_ptr->name &&
      EMBMS_IFACE != ps_iface_ptr->name &&
      IWLAN_S2B_IFACE != ps_iface_ptr->name &&
      WLAN_IFACE != ps_iface_ptr->name &&
      CLAT_IFACE != ps_iface_ptr->name &&
      LO_IFACE != ps_iface_ptr->name)
  {
    return;
  }

    rx_dpl = &ps_iface_ptr->dpl_net_cb.recv_cb;
    tx_dpl = &ps_iface_ptr->dpl_net_cb.tx_cb;

  switch(dpl_state)
    {
    case DPL_ENABLED_PARTIAL:
    {
      DPLI_SET_SNAPLEN_PROTOCOL(tx_dpl, DPL_PACKET_PARTIAL_LENGTH, DPL_IID_NETPROT_IP);
      DPLI_SET_SNAPLEN_PROTOCOL(rx_dpl, DPL_PACKET_PARTIAL_LENGTH, DPL_IID_NETPROT_IP);
      rx_dpl->is_logging_enabled = TRUE;
      tx_dpl->is_logging_enabled  = TRUE;
      break;
  }
    case DPL_ENABLED_FULL:
    {
      DPLI_SET_SNAPLEN_PROTOCOL(tx_dpl, SNAPLEN_ALL, DPL_IID_NETPROT_IP);
      DPLI_SET_SNAPLEN_PROTOCOL(rx_dpl, SNAPLEN_ALL, DPL_IID_NETPROT_IP);
      rx_dpl->is_logging_enabled = TRUE;
      tx_dpl->is_logging_enabled = TRUE;
      break;
    }
    case DPL_DISABLED:
    {
      DPLI_SET_SNAPLEN_PROTOCOL(tx_dpl, 0, DPL_IID_NETPROT_IP);
      DPLI_SET_SNAPLEN_PROTOCOL(rx_dpl, 0, DPL_IID_NETPROT_IP);
      rx_dpl->is_logging_enabled = FALSE;
      tx_dpl->is_logging_enabled = FALSE;

      break;
    }
    default :
      break;
  }

}/*dpli_update_logging_config*/

/*===========================================================================
FUNCTION    dpli_update_ifaces_dpl_cb

DESCRIPTION
  This method run over all ifaces and enables the logging in valid iface, 
  sets protocol and packet
  log length
 
PARAMETERS
  ps_iface_ptr[in]   Pointer to the interface 
  
RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
 static void dpli_update_ifaces_dpl_cb
( 
  dpl_state_enum_type  dpl_state
)
{
  int32   loop;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  for (loop = 0;
       loop < MAX_SYSTEM_IFACES && global_iface_ptr_array[loop] != 0;
       loop++)
  {
    if (!PS_IFACE_IS_VALID(global_iface_ptr_array[loop]))
    {
      LOG_MSG_INFO3_1("dpli_check_log_codes(): iface 0x%x is invalid",
                      global_iface_ptr_array[loop]);
      continue;
    }

    dpli_update_logging_config(global_iface_ptr_array[loop],dpl_state);
  }

  return;
}/*dpli_update_ifaces_dpl_cb*/

/*===========================================================================
FUNCTION   DPL_CHECK_LOG_CODE()

DESCRIPTION
  This function will check partial and full log codes and will disable/enable DPL state.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
static void dpl_check_log_code
(
  void
)
{
  uint8       is_dpl_state_change =  FALSE;
  uint32      pkt_len             = 0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* 
  * Full byte logging (if both are enabled) gets higher precedence over  Partial logging .
  * Update packet len to DPM only when logging state change
  */
  if (TRUE == log_status( LOG_DATA_PROTOCOL_LOGGING_NETWORK_IP_RM_TX_FULL_C) ||
          TRUE == log_status( LOG_DATA_PROTOCOL_LOGGING_NETWORK_IP_RM_RX_FULL_C) ||
          TRUE == log_status( LOG_DATA_PROTOCOL_LOGGING_NETWORK_IP_UM_TX_FULL_C) ||
          TRUE == log_status( LOG_DATA_PROTOCOL_LOGGING_NETWORK_IP_UM_RX_FULL_C))
  {
    if (DPL_ENABLED_FULL != dpl_state)
    {
      dpl_state           = DPL_ENABLED_FULL;
      pkt_len             = SNAPLEN_ALL;
      is_dpl_state_change = TRUE;
    }
  }
  else if (TRUE == log_status( LOG_DATA_PROTOCOL_LOGGING_NETWORK_IP_RM_TX_80_BYTES_C) ||
    TRUE == log_status( LOG_DATA_PROTOCOL_LOGGING_NETWORK_IP_RM_RX_80_BYTES_C) ||
    TRUE == log_status( LOG_DATA_PROTOCOL_LOGGING_NETWORK_IP_UM_TX_80_BYTES_C) ||
    TRUE == log_status( LOG_DATA_PROTOCOL_LOGGING_NETWORK_IP_UM_RX_80_BYTES_C))
  {
    if (DPL_ENABLED_PARTIAL != dpl_state)
    {
      dpl_state           = DPL_ENABLED_PARTIAL;
      pkt_len             = DPL_PACKET_PARTIAL_LENGTH;
      is_dpl_state_change = TRUE;
    }
  }  
  else
  {
    if (DPL_DISABLED != dpl_state)
    {
      dpl_state           = DPL_DISABLED;
      pkt_len             = 0;
      is_dpl_state_change = TRUE;
    }
  }

  if (TRUE == is_dpl_state_change)
  {
    LOG_MSG_INFO1_2("dpl_diag_sig_handler() : DPL state %d update len %d", 
    dpl_state,pkt_len);

    /* Update all valid ifaces with DPL state*/
    dpli_update_ifaces_dpl_cb(dpl_state);

    /* Now Update pakcet len to DPM */
    ps_dpm_hw_control_dpl_logging(pkt_len);
  }

  return ;
} /* dpl_check_log_code */


/*===========================================================================
FUNCTION   DPL_DIAG_SIG_HANDLER()

DESCRIPTION
  This function process signal registered with diag.

DEPENDENCIES
  None.

PARAMETERS
  sig: the Signal that is handled
  user_data_ptr: NOT USED

RETURN VALUE
  TRUE: no more processing to be done
  FALSE: Needs to be called again.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean dpl_diag_sig_handler
(
  ps_sig_enum_type       sig,
  void                 * user_data_ptr
)
{

  dpl_check_log_code ();
  
  return TRUE;
} /* dpl_diag_sig_handler */

/*===========================================================================
FUNCTION    DPL_INIT

DESCRIPTION
  Registers functions with DIAG.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void dpl_init
(
  void
)
{
  RCEVT_SIGEX_SIGREX rcevt_diag_sig;  

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Register log mask change for diag.
  -------------------------------------------------------------------------*/
  rcevt_diag_sig.signal = rex_self();
  rcevt_diag_sig.mask = 1 << PS_DPL_DIAG_SIGNAL;  

  if (RCEVT_NULL != rcevt_register_sigex_name(DIAG_LOG_MASK_CHANGE_NOTIFY, 
                                              RCEVT_SIGEX_TYPE_SIGREX, 
                                              &rcevt_diag_sig))
  {
    (void) ps_set_sig_handler(PS_DPL_DIAG_SIGNAL, dpl_diag_sig_handler, NULL);

    ps_enable_sig( PS_DPL_DIAG_SIGNAL );

    LOG_MSG_INFO1_0("ps_mm_task(): Registration Successful for diag event");
  }

  /*-------------------------------------------------------------------------
    Initilize DPL state
  -------------------------------------------------------------------------*/
  dpl_state = DPL_DISABLED;

  /*-------------------------------------------------------------------------
  Check Log Codes
  -------------------------------------------------------------------------*/
  dpl_check_log_code ();

} /* dpl_init() */

/*===========================================================================
FUNCTION    DPLI_SET_PS_IFACE_PTR

DESCRIPTION
  Set ps_iface_ptr in the lookup table

PARAMETERS
  ps_iface_ptr : pointer to ps_iface structure
  ifname       : IID iface ID

RETURN VALUE
  None

DEPENDENCIES
  ps_iface_ptr and ifname must have been validated

SIDE EFFECTS
  None
===========================================================================*/
void dpli_set_ps_iface_ptr
(
  ps_iface_type           * ps_iface_ptr,
  dpl_iid_ifname_enum_type  ifname
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( (ifname < DPL_IID_IFNAME_MIN) ||
       (ifname >= DPL_IID_IFNAME_MAX) )
  {
    LOG_MSG_ERROR_1("dpli_set_ps_iface_ptr(): "
                    "ifname out of range %d", ifname);
    return;
  }
  ps_iface_dpl_lookup_table[ifname].ps_iface_ptr = ps_iface_ptr;

} /* dpli_set_ps_iface_ptr() */

/*===========================================================================
FUNCTION    DPLI_GET_PS_IFACE_DEFAULT_DESC

DESCRIPTION
  Get default description for this ifname from the lookup table

PARAMETERS
  ifname : IID ifname

RETURN VALUE
  Description of the iface corresponding to this ifname

DEPENDENCIES
  ifname must have been validated

SIDE EFFECTS
  None
===========================================================================*/
const char *dpli_get_ps_iface_default_desc
(
  dpl_iid_ifname_enum_type  ifname
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if ( (ifname < DPL_IID_IFNAME_MIN) ||
       (ifname >= DPL_IID_IFNAME_MAX) )
  {
    LOG_MSG_ERROR_1("dpli_get_ps_iface_default_desc(): "
                    "ifname out of range %d", ifname);
    return NULL;
  }
  
  return  ps_iface_dpl_lookup_table[ifname].desc;

} /* dpli_get_ps_iface_default_desc() */
/*lint -restore Restore Lint Warning 641: Converting enum '{...}' to int*/


