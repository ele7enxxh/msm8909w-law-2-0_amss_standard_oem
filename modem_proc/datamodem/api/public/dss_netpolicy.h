#ifndef DSS_NETPOLICY_H
#define DSS_NETPOLICY_H
/*===========================================================================

         D A TA   S E R V I C E S   N E T W O R K   P O L I C Y  I N F O
                       H E A D E R  F I L E

DESCRIPTION

 The Network policy info Header File contains shared variables
 and enums, as well as declarations for functions related to network polciy.


Copyright (c) 2003-2009 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/dss_netpolicy.h#1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
02/05/07    sv     Added IM_CN flag to dss_netpolicy.
03/30/04    vp     Merged changes from June04Dev_05.00.00 PS branch.
11/12/03    sv     Added is_routeable flag to dss_netpolicy.
10/28/03    sv     Added version macro for dss_get_iface_id_by_policy.
10/20/03    sv     Added dss_get_iface_id_by_policy function.
09/11/03    sv     Renamed private to dss_netpolicy_private.
08/28/03    ss     Moved declarations of iface name and id to dssocket_defs.h
07/17/03    sv     Moved netpolicy info data structures to new header file.

===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "dssocket_defs.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
typedef enum
{
  DSS_IFACE_ID = 0,
  DSS_IFACE_NAME = 1
} dss_iface_id_kind_enum_type;

/*---------------------------------------------------------------------------
  dss_iface_id_type will be treated as opaque and typedef'ed to int
  16 bits for iface name, 8 bits for instance and 8 for phys link
  the user will get the iface_id by supplying iface and instance to a
  function that will return dss_iface_id_type. an attempt to get the
  iface_id for "ANY" iface will return error.
---------------------------------------------------------------------------*/

typedef struct
{
  dss_iface_id_kind_enum_type kind;
  union
  {
    dss_iface_id_type id;
    dss_iface_name_enum_type name;
  } info;
} dss_iface_type;

/*---------------------------------------------------------------------------
                       Network policy data structures.
---------------------------------------------------------------------------*/

typedef enum
{
  DSS_IFACE_POLICY_ANY          = 0,
  DSS_IFACE_POLICY_UP_ONLY      = 1,
  DSS_IFACE_POLICY_UP_PREFERRED = 2,
} dss_iface_policy_flags_enum_type;

typedef enum
{
  DSS_APN_TYPE_UNSPECIFIED = 0,
  DSS_APN_TYPE_IMS         = 1,
}dss_apn_enum_type;

/* Definitions to maintain backwards compatibility */
#define DSS_IFACE_POLICY_SPECIFIED  DSS_IFACE_POLICY_ANY
#define DSS_IFACE_POLICY_UP_SPEC    DSS_IFACE_POLICY_UP_PREFERRED

/*---------------------------------------------------------------------------
  Constants defining various app identifiers. These values could be changed
  in future
---------------------------------------------------------------------------*/
#define DSS_APP_IDENTIFIER_EMBEDDED_ON_DEMAND_APP         0x0
#define DSS_APP_IDENTIFIER_EMBEDDED_ALWAYS_ON_APP         0x00100000
#define DSS_APP_IDENTIFIER_EXTERNAL_DEVICE_ON_DEMAND_APP  0x10000000

/*---------------------------------------------------------------------------
  Structure specifying policy to be used in network selection

  Fields are
    policy_flag             - Specifies if UP interface is required
    iface                   - Specifies the kind of needed interface
    app_identifier          - Identifies an application
    ipsec_disabled          - Is IPSEC disabled?
    family                  - Indicates if IPv4/IPv6 is preferred or don't care
    is_routeable            - Is interface routeable?
    sip_iface_reqd          - Is physical iface required?
    pdp_profile_num         - UMTS profile to be used
    im_cn_flag              - IM-CN flag for IMS
    data_session_profile_id - CDMA profile to be used
    cookie                  - If set to a valid value, indicates that policy
                              structure is initialized by calling
                              dss_init_net_policy_info(). INTERNAL USE only
---------------------------------------------------------------------------*/
typedef struct
{
  dss_iface_policy_flags_enum_type policy_flag;
  dss_iface_type                   iface;
  uint64                           app_identifier;
  boolean                          ipsec_disabled;
  int                              family;
  boolean                          is_routeable;
  boolean sip_iface_reqd;

  /*-------------------------------------------------------------------------
    UMTS specific data session policy information
  -------------------------------------------------------------------------*/
  struct
  {
    int pdp_profile_num;
    boolean im_cn_flag;

    struct
    {
      uint8        length;
      byte         *name;       /* UMTS apn string */
    } apn;

  } umts;

  /*-------------------------------------------------------------------------
    CDMA specific data session policy information
  -------------------------------------------------------------------------*/
  struct
  {
    int data_session_profile_id;
  } cdma;

  struct
  {
    int cookie;
  } dss_netpolicy_private;

  uint32    subs_id; /* Subscription identifier */
  dss_apn_enum_type  apn_type; /* APN type */
} dss_net_policy_info_type;


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION DSS_GET_IFACE_ID_BY_POLICY()

DESCRIPTION
  This function return the interface id based on the specified network policy.

DEPENDENCIES
  None.

PARAMETERS
  dss_net_policy_info_type  net_policy_info: specifies the network policy for
  determining the interface.

RETURN VALUE
  iface_id: If a valid iface could be obtained based on policy
   On error, return DSS_IFACE_INVALID_ID and places the error condition value
   in *dss_errno.

  dss_errno Values
  ----------------
  DS_EFAULT      Netpolicy structure is not initialized.
  DS_ENOROUTE    No interface can be determined from the network policy.

SIDE EFFECTS
  None.
===========================================================================*/
#define DSS_GET_IFACE_ID_BY_POLICY_VERS (1)
dss_iface_id_type dss_get_iface_id_by_policy
(
  dss_net_policy_info_type  net_policy_info,        /* Network policy info */
  sint15                  * dss_errno             /* error condition value */
);

#ifdef __cplusplus
}
#endif

#endif /* DSS_NETPOLICY_H */
