#ifndef PS_STAT_LLC_H
#define PS_STAT_LLC_H

/*===========================================================================

                          P S _ S T A T _ L L C . H

GENERAL DESCRIPTION
  This header file provides the structure, enum and function definitions
  for the statistics pertaining to 802.2 LLC Protocol.

EXTERNALIZED FUNCTIONS
  ps_stat_get_llc()
    This function supplies the required 802.2 LLC stats.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2006-2009 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when        who     what, where, why
----------  ---     ---------------------------------------------------------
08/19/09    pg      Changes related to WAPI implementation
02/27/07   scb     Added changes to fix RCVT compilation warning
02/21/07    scb     Replaced the INC macro implementation to use the
                    COMMON_INC macro
08/18/06    scb     Updated enum values to start with 0 for calculating
                    correct offset.
06/23/2006  scb     Initial version

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "ps_stat_common.h"


/*---------------------------------------------------------------------------
  LLC Statistics - Instance stats only.
---------------------------------------------------------------------------*/
typedef struct
{
  uint32  frms_tx                         ;
  uint32  llc_mode0_tx                    ;
  uint32  llc_mode1_tx                    ;
  uint32  frms_fail_tx                    ;
  uint32  unknown_proto_drop_tx           ;
  uint32  invalid_mode_drop_tx            ;
  uint32  frms_rx                         ;
  uint32  llc_mode0_rx                    ;
  uint32  llc_mode1_rx                    ;
  uint32  llc_control_frms_rx             ;
  uint32  frms_drop_rx                    ;
  uint32  instance_stop_rx                ;
  uint32  instance_start_pending_rx       ;
  uint32  inv_dest_addr_rx                ;
  uint32  inv_src_addr_rx                 ;
  uint32  inv_mode_rx                     ;
  uint32  mode0_inv_ethlen_rx             ;
  uint32  mode1_inv_snaphdr_len_rx        ;
  uint32  mode1_inv_snaphdr_rx            ;
  uint32  mode1_inv_oui_rx                ;
  uint32  mode1_vlan_frms_rx              ;
  uint32  mode1_inv_vlan_len_rx           ;
  uint32  mode1_inv_vlan_frm_rx           ;
  uint32  mode1_inv_ethlen_rx             ;
  uint32  llc_802_1x_unsupported          ;
  uint32  llc_802_1x_preauth_unsupported  ;
  uint32  llc_wapi_unsupported            ;
} ps_stat_llc_i_s_type;


/*---------------------------------------------------------------------------
 LLC statistics enum along with the description and type of each entity
---------------------------------------------------------------------------*/
typedef enum
{
  /*
    Desc: Number of LLC frames transmitted.
    Type: uint32
  */
  PS_STAT_LLC_TX                          = 0   ,
  /*
    Desc: Number of Mode 0 LLC frames transmitted.
    Type: uint32
  */
  PS_STAT_LLC_MODE_0_TX                   = 1   ,
  /*
    Desc: Number of Mode 1 LLC frames transmitted.
    Type: uint32
  */
  PS_STAT_LLC_MODE_1_TX                   = 2   ,
  /*
    Desc: Number of LLC frames that failed transmission.
    Type: uint32
  */
  PS_STAT_LLC_FAILED_TX                   = 3   ,
  /*
    Desc: Number of LLC frames with Unknown protocol that failed
          transmission.
    Type: uint32
  */
  PS_STAT_LLC_UNKNOWN_PROTOCOL_TX         = 4   ,
  /*
    Desc: Number of LLC frames with an invalid mode that failed
          transmission.
    Type: uint32
  */
  PS_STAT_LLC_INVALID_MODE_TX             = 5   ,
  /*
    Desc: Number of LLC frames received.
    Type: uint32
  */
  PS_STAT_LLC_RX                          = 6   ,
  /*
    Desc: Number of Mode 0 LLC frames received.
    Type: uint32
  */
  PS_STAT_LLC_MODE_0_RX                   = 7   ,
  /*
    Desc: Number of Mode 1 LLC frames received.
    Type: uint32
  */
  PS_STAT_LLC_MODE_1_RX                   = 8   ,
  /*
    Desc: Number of LLC Control frames recieved.
    Type: uint32
  */
  PS_STAT_LLC_CONTROL_FRMS_RX             = 9  ,
  /*
    Desc: Number of LLC frames dropped due to errors.
    Type: uint32
  */
  PS_STAT_LLC_DROPPED_RX                  = 10  ,
  /*
    Desc: Number of LLC frames dropped as LLC instance has stopped.
    Type: uint32
  */
  PS_STAT_LLC_INSTANCE_STOP_RX            = 11  ,
  /*
    Desc: Number of LLC frames received when the LLC instance start
          is still pending.
    Type: uint32
  */
  PS_STAT_LLC_INSTANCE_START_PENDING_RX   = 12  ,
  /*
    Desc: Number of LLC frames dropped with invalid destination
          address.
    Type: uint32
  */
  PS_STAT_LLC_INVALID_DEST_ADDR_RX        = 13  ,
  /*
    Desc: Number of LLC frames with invalid source address dropped.
    Type: uint32
  */
  PS_STAT_LLC_INVALID_SRC_ADDR_RX         = 14  ,
  /*
    Desc: Number of LLC frames with invalid mode dropped.
    Type: uint32
  */
  PS_STAT_LLC_INVALID_MODE_RX             = 15  ,
  /*
    Desc: Number of Mode 0 LLC frames with invalud etherlen dropped.
    Type: uint32
  */
  PS_STAT_LLC_M0_INVALID_ETHLEN_RX        = 16  ,
  /*
    Desc: Number of LLC frames of type mode 1 with invalid
          snap header length dropped.
    Type: uint32
  */
  PS_STAT_LLC_M1_INVALID_SNAPHDR_LEN_RX   = 17  ,
  /*
    Desc: Number of Mode 1 LLC frames with invalid snap header
          dropped.
    Type: uint32
  */
  PS_STAT_LLC_M1_INVALID_SNAPHDR_RX       = 18  ,
  /*
    Desc: Number of Mode 1 LLC frames with Invalid OUI dropped.
    Type: uint32
  */
  PS_STAT_LLC_M1_INVALID_OUI_RX           = 19  ,
  /*
    Desc: Number of Mode 1 VLAN frames received.
    Type: uint32
  */
  PS_STAT_LLC_M1_VLAN_FRMS_RX             = 20  ,
  /*
    Desc: Number of Mode 1 LLC frames with Invalid VLAN Length dropped.
    Type: uint32
  */
  PS_STAT_LLC_M1_INVALID_VLANLEN_RX       = 21  ,
  /*
    Desc: Number of Mode 1 LLC frames with invalid VLAN frame dropped.
    Type: uint32
  */
  PS_STAT_LLC_M1_INVALID_VLANFRM_RX       = 22  ,
  /*
    Desc: Number of Mode 1 LLC frames with invalud etherlen dropped.
    Type: uint32
  */
  PS_STAT_LLC_M1_INVALID_ETHLEN_RX        = 23  ,
  /*
    Desc: Number of LLC frames of 802.1X protocol which is unsupported
          received.
    Type: uint32
  */
  PS_STAT_LLC_802_1X_UNSUPPORTED          = 24  ,
  /*
    Desc: Number of LLC frames of 802.1X preauth protocol which is
          unsupported received.
    Type: uint32
  */
  PS_STAT_LLC_802_1X_PREAUTH_UNSUPPORTED  = 25  ,

  /*
    Desc: Number of LLC frames of WAPI protocol which is unsupported 
          received.
    Type: uint32
  */
  PS_STAT_LLC_802_WAPI_UNSUPPORTED        = 26  ,

  PS_STAT_LLC_ALL
} ps_stat_llc_enum_type ;


/*===========================================================================

                          LOG PACKET DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Log structure for LOG_PS_STAT_LLC_INST_C

  Header defined in ps_stat_logging.h

  Fields are
    handle           : handle to the instance
    inst_llc_stats   : instance LLC stats
---------------------------------------------------------------------------*/
typedef struct
{
  int32                     handle;
  ps_stat_llc_i_s_type      inst_llc_stats;
} ps_stat_inst_llc_log_pkt_type;

/*---------------------------------------------------------------------------
TYPEDEF PS_STAT_INST_DESC_LLC_TYPE

DESCRIPTION
  The structure describing a LLC instance.

  handle        : Handle to the LLC instance.  Index into the LLC array
  iface_handle  : Handle to the iface the LLC instance is associated with
  reserved      : Used to maintain a 4-byte alignment
---------------------------------------------------------------------------*/
typedef struct
{
  uint8   handle;
  uint8   iface_handle;
  uint8   reserved[2];
} ps_stat_inst_desc_llc_type;


/*===========================================================================

                         EXTERNAL FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
MACRO LLC_INC_INSTANCE_STATS()

DESCRIPTION
  This macro increments the LLC statistic(field) by the necessary
  quantity (inc).

DEPENDENCIES
  None

PARAMETERS
  INSTANCE_ID - Id of the LLC instance control block
  FIELD       - The field of the corresponding statistics structure that is
                to be incremented.
  INC         - The increment by which the field is to be incremented.

SIDE EFFECTS
  None
===========================================================================*/
#define LLC_INC_INSTANCE_STATS(INSTANCE_ID, FIELD, INC)             \
    PS_COMMON_INC_STATS(lle_array[INSTANCE_ID].llc_i_stats.FIELD, 4, INC)

/*===========================================================================
FUNCTION PS_STAT_INIT_LLC()

DESCRIPTION

DEPENDENCIES
  None

PARAMETERS

RETURN VALUE

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_init_llc
(
  void
);

/*===========================================================================
FUNCTION PS_STAT_GET_LLC()

DESCRIPTION
  This function supplies the required 802.2 LLC layer statistics.
  The first argument passed to the function is an enum that denotes the
  metric  of interest. The second argument will be a pointer that provides
  a handle to an instance ( For per-instance statistics only).The third
  argument points to the memory location where the results will be copied.
  The result (statistic) will be copied to this memory address. The
  fourth argument passes the length of memory that allocated for the
  return value and should be greater than or equal to the memory size of
  the statistic requested.

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h. The function also calls err_fatal in
  the case of any error.

PARAMETERS
  stat            - IN - type of stat
  instance_ptr    - IN - pointer to instance; should be NULL for LAN LLC
  return_value    - IN - pointer for return value
                  - OUT- return value (statistics)
  ret_len         - length of memory allocated

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_llc
(
  ps_stat_llc_enum_type     stat,
  void                      *instance_ptr,
  void                      *return_value,
  uint16                    ret_len
) ;

/*===========================================================================
FUNCTION PS_STAT_RESET_LLC()

DESCRIPTION
  This function resets the instance statistics associated with the specified
  instance.

DEPENDENCIES
  None

RETURN VALUE
  E_SUCCESS (0) for success and any other number for failure . The error
  codes are as defined in errno.h.

PARAMETERS
  handle  : Handle to the instance

SIDE EFFECTS
  None
===========================================================================*/
void ps_stat_reset_llc
(
  int32        handle
);

/*===========================================================================
FUNCTION PS_STAT_GET_LLC_CONTROL_BLOCK()

DESCRIPTION
  This function supplies the address of the LLC control block
  corresponding the LLC device passed.

DEPENDENCIES
  None

RETURN VALUE

  E_SUCCESS (0) for success and any other number for failure.The error
  codes are as defined in errno.h.

PARAMETERS
  device        - IN  - LLC device
  llccb_address - OUT - address of the llc control block corresponding
                        to the device.

SIDE EFFECTS
  None
===========================================================================*/
errno_enum_type ps_stat_get_llc_control_block
(
  int     device ,
  void    **llccb_address
);


#endif /* PS_STAT_LLC_H */
