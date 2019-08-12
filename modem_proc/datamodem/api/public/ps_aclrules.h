#ifndef PS_ACLRULES_H
#define PS_ACLRULES_H
/*===========================================================================

                          P S _ A C L R U L E S . H

GENERAL DESCRIPTION
  Access control lists are used to define IP packet filters. These
  filters can be used e.g. for firewall functionality, address
  translation for selected ranges, or to build policy based routing
  rules.
  ACLs are defined as integer functions, which take information
  about the IP packet filtered as parameters. The return value
  is either '0' for a deny (packet drop) or the highest priority
  rule class for which a match was found.
  This file defines a common ACL function prototype as well as some
  macros to specify ACLs more similar to the usual language based
  syntax on routers and hosts.
  The following is an example of an ACL:

  ACL_DEF( inbound_filter )
  ACL_START
    DENY( ACL_DEFAULT_CLASS, DST_ADDR_IS_IN( IPV4( 127,0,0,1 ), 0xffffffff ) )
    DENY( ACL_DEFAULT_CLASS, SRC_ADDR_IS_IN( LOCALHOST, 0xffffffff ) )
    PASS( ACL_DEFAULT_CLASS, DST_ADDR_IS_IN( LOCALHOST, 0xffffffff ) )
  ACL_END

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2002-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/public/ps_aclrules.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
05/04/15    sc     Added new ACL Deny reason for MIP on v6 calls
11/04/14    sd     Added new ACL deny reason for subs_id mismatch.
10/10/14    ssh    IP Translation Engine 464XLAT 
07/09/14    niv    Added new ACL Deny reason for NV Refresh in progress
06/30/10    rt     Added ACL_NAT_CLASS.
01/28/10    pp     htonl=>ps_htonl renaming.
06/24/09    am     Renamed hton/ntoh macros with ps_ prefix.
04/02/09    ar     Added ACL_PROXY_CONFIG_CLASS.
03/26/09    pp     CMI De-featurization.
01/29/09    pp     Added SLIP mode class to the list of ACL rule classes.
12/28/08    pp     Common Modem Interface: Public/Private API split.
06/23/08    am     Added new DENY rule to pass any reason by mode handler.
04/29/08    pp     Lint fixes.
05/04/07    vp     Added PROTOCOL_IS_IP and MIP CCOA specific stuff.
02/20/07    sv     Added support for binding to v4 interface if the address
                   family is UNSPEC and IPv6 feature is turned off.
11/22/06    sv     Added support for IFACE_ADDR_FAMILY_UNSPEC
09/18/06    hm     Changed the macros IFACE_ID_MATCH and
                   DIFFERENT_IFACE_ID_REQUIRED to refer to new structure of
                   iface_id as described in ps_iface_defs.h
04/18/05    mct    Added v6 loopback check.
04/17/05    msr    Fixed missing parenthesis in IFACE_IS_UP().
02/18/05    jd     Added IFACE_ID_MATCH macro.
10/30/04    msr    Changes DIFFERENT_IFACE_ID_REQUIRED() to ignore phys_link
                   instance while comparing IDs.
10/20/04    mct    Fixed ACL rule to correctly compare the group names.
10/13/04    vp     Changed the IPV4 macro to construct addresses in network
                   order.
07/27/04    mvl    Added some IPv6 related ACLs.
06/14/04    mct    Changed dss_iface_policy_flag_enum names.
06/11/04    vp     Changes due representation of IP addresses as struct ps_in_addr
                   or struct ps_in6_addr. Replaced use of TCP_PTCL, UDP_PTCL etc.
                   with IPPROTO_ versions.
04/21/04    mct    Featurized some V6 macros.
04/13/04    usb    Do not check for pkt_info for rules where its not used.
04/06/04    usb    Fixed REQUIRED_IFACE_TYPE_IS to get the iface name
                   correctly out of the policy info.
03/15/04    mct    Updated and simplified some ACL rules. Also renamed
                   acl_ipfilter_info_type to ip_pkt_info_type and
                   ipfilter_info_ptr to pkt_info_ptr.
02/27/04    mct    Added rules to check for new v4/v6 conditions.
02/03/04    usb    Renamed THIS_IFACE_NAME_IS to INCOMING_IFACE_TYPE_IS to
                   more accurately reflect the functionality. Also added
                   ACL_START_GLOBAL for global ACL's (NULL this_if_ptr)
07/22/03    usb    Added/Modified rules to support
                   - New routing policy information
                   - Multiple simultaneous interfaces (multiple PDP contexts)
                   - Routing in true multimode environment
05/27/03    om     Added rules for use by IPSec.
05/13/03    usb    Added new rules DIFFERENT_IFACE_REQUIRED and
                   IFACE_IS_PREF_MODE. Renamed IFACE_IS to REQUIRD_IFACE_IS
                   & IFACE_NAME_IS to THIS_IFACE_NAME_IS. Validating
                   this_iface_ptr at the begining of ACL function.
02/12/03    om     Fixed IFACE_IS() rule, added IFACE_NAME_IS() rule.
11/25/02    usb    Modified rules which perform NULL ptr check
                   to return FALSE instead of ACL_DENY. Init acl_ret_val to
                   ACL_DEFAULT_CLASS instead of 0.
11/19/02     om    Changed ACL_DENY to be -1 (separate from DEFAULT_CLASS).
11/19/02     usb   Added new rule to check if an iface is in use, removed
                   class parameter from DENY rule.
10/10/02     rc    Added new rule for checking if the context is
                   sharable (UMTS).
09/25/02     om    Added NULL IP info or policy info resulting in ACL_DENY.
09/13/02     om    Added IP address rules for inbound ACL.
09/09/02     om    Updates for ACL postprocessors.
08/30/02     om    Updated IFACE related rules.
03/08/02     om    Created Module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "amssassert.h"
#include "ps_acl.h"
#include "dss_netpolicy.h"
#include "ps_iface.h"
#include "ps_pkt_info.h"

/*===========================================================================

                         FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
MACRO IS_PKT_INFO_NULL

DESCRIPTION
  Checks whether the packet info pointer passed is null

DEPENDENCIES
  None.

PARAMETERS
pkt_info_ptr : IP Packet Info Pointer

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define IS_PKT_INFO_NULL() is_pkt_info_null(pkt_info_ptr)

boolean is_pkt_info_null
(
  ip_pkt_info_type *pkt_info_ptr
);

/*===========================================================================
MACRO IS_POL_INFO_NULL

DESCRIPTION
  Checks whether the policy info pointer passed is null

DEPENDENCIES
  None.
PARAMETERS
  policy_info_ptr : ACL Policy Info Pointer

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define IS_POL_INFO_NULL() is_pol_info_null(policy_info_ptr)

boolean is_pol_info_null
(
  acl_policy_info_type *policy_info_ptr
);

/*===========================================================================
MACRO PROC_ID_ANY

DESCRIPTION
  Checks whether the proc ID is PROC_ID_ANY

DEPENDENCIES
  None.
PARAMETERS
  policy_info_ptr : ACL Policy Info Pointer

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define PROC_ID_ANY() proc_id_any(policy_info_ptr)

boolean proc_id_any
(
  acl_policy_info_type *policy_info_ptr
);

/*===========================================================================
MACRO IFACE_ID_MATCH

DESCRIPTION
  Checks whether the iface id matches in the iface pointer and the policy
  info pointer

DEPENDENCIES
  None

PARAMETERS
  this_if_ptr        :  iface pointer
  policy_info_ptr    :  ACL policy info pointer

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define IFACE_ID_MATCH() iface_id_match(this_if_ptr, policy_info_ptr)

boolean iface_id_match
(
  ps_iface_type             *this_if_ptr,
  acl_policy_info_type      *policy_info_ptr
);

/*===========================================================================
MACRO DIFFERENT_PROC_ID_REQUIRED

DESCRIPTION
  Checks whether the proc id matches in the iface pointer and the policy
  info pointer

DEPENDENCIES
  None

PARAMETERS
  this_if_ptr        :  iface pointer
  policy_info_ptr    :  ACL policy info pointer

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define DIFFERENT_PROC_ID_REQUIRED()                                        \
  different_proc_id_required(this_if_ptr, policy_info_ptr)

boolean different_proc_id_required
(
  ps_iface_type             *this_if_ptr,
  acl_policy_info_type      *policy_info_ptr
);

/*===========================================================================
MACRO REQUIRED_IFACE_TYPE_IS(ifname)

DESCRIPTION
  Verifies whether the iface id in the policy info pointer matches the iface
  id in ifname, by checking either id, or name or group.

DEPENDENCIES
  None.

PARAMETERS
  ifname   :  interface name

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define REQUIRED_IFACE_TYPE_IS(ifname)                                      \
  required_iface_type_is(policy_info_ptr, ifname)

boolean required_iface_type_is
(
  acl_policy_info_type         *policy_info_ptr,
  uint16                        ifname
);

/*===========================================================================
MACRO INCOMING_IFACE_TYPE_IS

DESCRIPTION
  Checks if the incoming iface from packet info pointer has the same name
  as the parameter iface-name passed
DEPENDENCIES
  None.

PARAMETERS
  ifname  :    interface name

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define INCOMING_IFACE_TYPE_IS(ifname)                                      \
  incoming_iface_type_is(pkt_info_ptr, ifname)

boolean incoming_iface_type_is
(
  ip_pkt_info_type          *pkt_info_ptr,
  uint16                     ifname
);

/*===========================================================================
MACRO  LOCALHOST

DESCRIPTION
  Checks if the current packet info pointer contains the IPv4 address
  for the local interface

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define LOCALHOST localhost(pkt_info_ptr)

boolean localhost
(
  ip_pkt_info_type    *pkt_info_ptr
);

/*===========================================================================
MACRO PROTOCOL_IS_IP

DESCRIPTION
  Checks if the protocol field in the ip header of v4, contains the ipproto_ip
type

DEPENDENCIES
  None.

PARAMETERS
  pkt_info_ptr : Packet info pointer

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define PROTOCOL_IS_IP  protocol_is_ip(pkt_info_ptr)

boolean protocol_is_ip
(
  ip_pkt_info_type *pkt_info_ptr
);

/*===========================================================================
MACRO SRC_ADDR_IS_IN

DESCRIPTION
  Checks if src IPv4 addresses part of network defined by netmask and netid

DEPENDENCIES
  None

PARAMETERS
  netid      :  net id of the IPv4 address
  netmask    :  net mask of the IPv4 address

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define SRC_ADDR_IS_IN(netid, netmask)                                      \
  src_addr_is_in(pkt_info_ptr, netid, netmask)

boolean src_addr_is_in
(
  ip_pkt_info_type    *pkt_info_ptr,
  uint32               netid,
  uint32               netmask
);

/*===========================================================================
MACRO DST_ADDR_IS_IN

DESCRIPTION
  Checks if dst IPv4 addresses part of network defined by netmask and netid

DEPENDENCIES
  None

PARAMETERS
  netid      :  net id of the IPv4 address
  netmask    :  net mask of the IPv4 address

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define DST_ADDR_IS_IN(netid, netmask)                                      \
  dst_addr_is_in(pkt_info_ptr, netid, netmask)

boolean dst_addr_is_in
(
   ip_pkt_info_type *pkt_info_ptr,
   uint32            netid,
   uint32            netmask
);

/*===========================================================================
MACRO IFACE_IS_PREF_MODE

DESCRIPTION
  Determine if this iface is preferred mode for packet data calls

DEPENDENCIES
  None.

PARAMETERS
  pref_mode : Pointer to a function which takes the mode as an argument
  mode      : sys_sys_mode_e_type

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define IFACE_IS_PREF_MODE(pref_mode_fptr, mode) pref_mode_fptr( mode )

/*===========================================================================
MACRO IFACE_IS_PREF_MODE_DEF2

DESCRIPTION
  Determine if this iface is preferred mode for packet data calls based on
  preferred system or policy info

DEPENDENCIES
  None.

PARAMETERS
  pref_mode : Pointer to a function which takes the mode as an argument
  mode      : sys_sys_mode_e_type

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define IFACE_IS_PREF_MODE_DEF2(pref_mode_fptr, mode)                       \
  pref_mode_fptr( policy_info_ptr, mode )

/*===========================================================================
MACRO SPECIFIED_CONFIG_IS_SHAREABLE

DESCRIPTION
  Determine if this iface is already up with required config

DEPENDENCIES
  None.

PARAMETERS
  is_cfg_shareable_ptr:  Pointer to a function which takes iface pointerm policy
                         info pointer and packet info pointer as arguments

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define SPECIFIED_CONFIG_IS_SHAREABLE(is_cfg_shareable_fptr)                \
  specified_config_is_shareable(is_cfg_shareable_fptr,                      \
                                this_if_ptr,                                \
                                policy_info_ptr,                            \
                                pkt_info_ptr)

boolean specified_config_is_shareable
(
  boolean               (*is_cfg_shareable_fptr)
                                (void                       *this_if_ptr,
                                 acl_policy_info_type       *policy_info_ptr,
                                 ip_pkt_info_type           *pkt_info_ptr),
  ps_iface_type         *this_if_ptr,
  acl_policy_info_type  *policy_info_ptr,
  ip_pkt_info_type      *pkt_info_ptr
);

/*===========================================================================
MACRO IFACE_UP_PREFFERED_AND_UP

DESCRIPTION
  UP iface preferred and this iface is UP

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define IFACE_UP_PREFERRED_AND_UP()                                         \
  iface_up_preferred_and_up(policy_info_ptr, this_if_ptr)

boolean iface_up_preferred_and_up
(
   acl_policy_info_type             *policy_info_ptr,
   ps_iface_type                    *this_if_ptr
);

/*---------------------------------------------------------------------------
  Pointer checks on the policy and IP filter info
  If the required pointer is NULL, the rule becomes NOOP except when
  NOT is used before a rule.  It is not recommended to put a rule inside
  NOT.  Using NOT inside a rule is acceptable
---------------------------------------------------------------------------*/

/*===========================================================================
MACRO IP_PKT_FAMILY_IS

DESCRIPTION
  checks whether the ip type matches in the packet info pointer

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define IP_PKT_FAMILY_IS(ipvsn) ip_pkt_family_is(ipvsn, pkt_info_ptr)

boolean ip_pkt_family_is
(
  int               ipvsn,
  ip_pkt_info_type *pkt_info_ptr
);

/*===========================================================================
MACRO IP_POL_FAMILY_IS

DESCRIPTION
  checks whether the ip pol family matches in the policy info pointer

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define IP_POL_FAMILY_IS(ipfam) ip_pol_family_is(ipfam, policy_info_ptr)

boolean ip_pol_family_is
(
  int                   ipfam,
  acl_policy_info_type *policy_info_ptr
);

/*===========================================================================
MACRO IFACE_AND_PKT_FAMILY_MATCH

DESCRIPTION
  Checks whether the ip addr family matches in iface pointer and the packet
  info pointer

DEPENDENCIES
  None.

PARAMETERS
  None.

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define IFACE_AND_PKT_FAMILY_MATCH()                                        \
  iface_and_pkt_family_match(this_if_ptr, pkt_info_ptr)

boolean iface_and_pkt_family_match
(
  ps_iface_type          *this_if_ptr,
  ip_pkt_info_type       *pkt_info_ptr
);

/*===========================================================================
MACRO IFACE_AND_POL_FAMILY_MATCH

DESCRIPTION
  Checks whether the ip addr family matches in iface pointer and the policy
  info pointer

DEPENDENCIES
  None.

PARAMETERS
  policy_info_ptr :        ACL Policy Info Pointer
  pkt_info_ptr    :        IP Packet Info Type

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define IFACE_AND_POL_FAMILY_MATCH()                                        \
  iface_and_pol_family_match(this_if_ptr, policy_info_ptr)

boolean iface_and_pol_family_match
(
  ps_iface_type          *this_if_ptr,
  acl_policy_info_type   *policy_info_ptr
);

/*===========================================================================
MACRO DIFFERENT_IFACE_ID_REQUIRED

DESCRIPTION
  Checks if the required iface id explicitly specified, different from this one

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define DIFFERENT_IFACE_ID_REQUIRED()                                       \
  diff_iface_id_required(this_if_ptr, policy_info_ptr)

boolean diff_iface_id_required
(
  ps_iface_type             *this_if_ptr,
  acl_policy_info_type      *policy_info_ptr
);

/*===========================================================================
MACRO DIFFERENT_IFACE_NAME_REQUIRED

DESCRIPTION
  Checks if the iface name explicitly specified, different from this one

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define DIFFERENT_IFACE_NAME_REQUIRED()                                     \
  diff_iface_name_required(this_if_ptr, policy_info_ptr)

boolean diff_iface_name_required
(
  ps_iface_type             *this_if_ptr,
  acl_policy_info_type      *policy_info_ptr
);

/*===========================================================================
MACRO DIFFERENT_IFACE_GROUP_REQUIRED

DESCRIPTION
  Checks if the Iface group explicitly specified, different from this one

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define DIFFERENT_IFACE_GROUP_REQUIRED()                                    \
  diff_iface_group_required(this_if_ptr, policy_info_ptr)

boolean diff_iface_group_required
(
  ps_iface_type            *this_if_ptr,
  acl_policy_info_type     *policy_info_ptr
);

/*---------------------------------------------------------------------------
  Is the destination IPv6 addr in the prefix/netmask passed in?
---------------------------------------------------------------------------*/
#define base_v6_hdr hdr_body.base_hdr
#define DST_ADDR6_IS_IN( prefix, netmask )                                  \
  USE_PKT_INFO(                                                             \
   (pkt_info_ptr->ip_hdr.v6.base_v6_hdr.dst_addr.ps_s6_addr64[0] & netmask) \
    ==prefix)

/*===========================================================================
MACRO DST_ADDR6_IS_UNSPECIFIED

DESCRIPTION
  Checks if the destination address for V6 is unspecified

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define DST_ADDR6_IS_UNSPECIFIED() dest_addr6_is_unspecified(pkt_info_ptr)

boolean dest_addr6_is_unspecified
(
  ip_pkt_info_type    *pkt_info_ptr
);

/*===========================================================================
MACRO IFACE_NOT_AVAILABLE

DESCRIPTION
  Determine if this iface is in exclusive use

DEPENDENCIES
  None.

PARAMETERS
  is_available_fptr: Pointer to a function which takes iface ptr, policy info
                     pointer and packet info pointer as arguments

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define IFACE_NOT_AVAILABLE(is_available_fptr)                              \
  iface_not_available(is_available_fptr,                                    \
                      this_if_ptr,                                          \
                      policy_info_ptr,                                      \
                      pkt_info_ptr)

boolean iface_not_available
(
  boolean               (*is_available_fptr)
                                       (void                 *this_if_ptr,
                                        acl_policy_info_type *policy_ptr,
                                        ip_pkt_info_type     *pkt_info_ptr),
  ps_iface_type         *this_if_ptr,
  acl_policy_info_type  *policy_info_ptr,
  ip_pkt_info_type      *pkt_info_ptr
);

/*===========================================================================
MACRO SPECIFIED_CONFIG_NOT_SUPPORTED

DESCRIPTION
  Determine if this iface can come up with required config

DEPENDENCIES
  None

PARAMETERS
  is_cfg_supported_fptr : Pointer to a function which takes iface pointer,
                          policy info pointer and packet info pointer as args

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define SPECIFIED_CONFIG_NOT_SUPPORTED(is_cfg_supported_fptr)               \
  specified_config_not_supported(is_cfg_supported_fptr,                     \
                                 this_if_ptr,                               \
                                 policy_info_ptr,                           \
                                 pkt_info_ptr)

boolean specified_config_not_supported
(
  boolean               (*is_cfg_supported_fptr)
                                    (void                  *this_if_ptr,
                                     acl_policy_info_type  *policy_info_ptr,
                                     ip_pkt_info_type      *pkt_info_ptr),
  ps_iface_type         *this_if_ptr,
  acl_policy_info_type  *policy_info_ptr,
  ip_pkt_info_type      *pkt_info_ptr
);

/*===========================================================================
MACRO IFACE_UP_REQUIRED_BUT_NOT_UP

DESCRIPTION
  Required only UP iface but this iface is not UP

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define IFACE_UP_REQUIRED_BUT_NOT_UP()                                      \
  iface_up_required_but_not_up(policy_info_ptr, this_if_ptr)

boolean iface_up_required_but_not_up
(
  acl_policy_info_type              *policy_info_ptr,
  ps_iface_type                     *this_if_ptr
);

/*===========================================================================
MACRO IFACE_UP_REQUIRED_BUT_NOT_UP

DESCRIPTION
  checks if the app requires sip iface

DEPENDENCIES
  None

PARAMETERS
  None

RETURN VALUE
  In case of successful completion, returns TRUE, else returns FALSE

SIDE EFFECTS
  None
===========================================================================*/
#define POL_REQUIRES_SIP_IFACE()  pol_requires_sip_iface(policy_info_ptr)

boolean pol_requires_sip_iface
(
  acl_policy_info_type              *policy_info_ptr
);

/*===========================================================================
FUNCTION ACL_TRACE_PASS

DESCRIPTION
  Prints the ACL debug message for  rules with priority

DEPENDENCIES
  None.

PARAMETERS
acl_rule_val  :  ACL rule priority

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void acl_trace_pass
(
  uint32    acl_rule_val
);

/*===========================================================================
FUNCTION ACL_TRACE_DENY

DESCRIPTION
  Prints the ACL debug message for denied ifaces with reason

DEPENDENCIES
  None.

PARAMETERS
iface_ptr   : Pointer to the iface
deny_reason : Reaosn for denying iface

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void acl_trace_deny
(
  void      * this_if_ptr,
  int       reason
);

/*===========================================================================
FUNCTION ACL_TRACE_START

DESCRIPTION
  Prints the ACL debug message for acl start macros

DEPENDENCIES
  None.

PARAMETERS
iface_ptr : POinter to the iface

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void acl_trace_start
(
  void      * this_if_ptr
);

/*===========================================================================
FUNCTION ACL_TRACE_END

DESCRIPTION
  Prints the ACL debug message for passed ifaces with priority

DEPENDENCIES
  None.

PARAMETERS
iface_ptr     : POinter to the iface
acl_rule_val  :  ACL rule priority

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void acl_trace_end
(
  void      * this_if_ptr,
  uint32    acl_rule_val
);

/*===========================================================================

                        MACRO DEFINITIONS

===========================================================================*/


/*---------------------------------------------------------------------------
  Logical expressions
---------------------------------------------------------------------------*/
#define AND             &&
#define OR              ||
#define NOT( arg )      (! (arg))
#define ALL             TRUE

/*---------------------------------------------------------------------------
  Define the result of a rule (pass packet or drop it)
  Any new deny reasons that is being added should have
  a corresponding net down reason defined, values are
  interleaved so any new deny reasons can grouped together
  logically.
  ACL_DENY_REASON_POL_NO_MATCH should always be the lowest
---------------------------------------------------------------------------*/
#define ACL_DENY_REASON_POL_NO_MATCH                  -1000
#define ACL_DENY_REASON_IFACE_IN_USE                  -2
#define ACL_DENY_REASON_IFACE_IN_USE_CFG_MATCH        -3

#define ACL_DENY_REASON_APN_FORMAT_INVALID            -11
#define ACL_DENY_REASON_THERMAL_MITIGATION            -12
#define ACL_DENY_REASON_NV_REFRESH_IN_PROGRESS        -13
#define ACL_DENY_REASON_MANDATORY_APN_DISABLED        -14
#define ACL_DENY_REASON_APN_DISABLED                  -15
#define ACL_DENY_REASON_APN_DISALLOWED_ON_ROAMING     -16
#define ACL_DENY_REASON_DATA_SETTING_DISABLED         -17
#define ACL_DENY_REASON_DATA_ROAMING_SETTING_DISABLED -18
#define ACL_DENY_REASON_V6_DISABLED                   -25
#define ACL_DENY_REASON_TEST_LOOPBACK_MODE_A_OR_B_ENABLED  -26

#define ACL_DENY_REASON_ERR_PDN_IPV4_CALL_THROTTLED   -30
#define ACL_DENY_REASON_ERR_PDN_IPV6_CALL_THROTTLED   -31
#define ACL_DENY_REASON_ERR_PDN_IPV4_CALL_DISALLOWED  -32
#define ACL_DENY_REASON_ERR_PDN_IPV6_CALL_DISALLOWED  -33
#define ACL_DENY_REASON_EHRPD_ERR_VSNCP_3GPP2I_RECONNECT_NOT_ALLOWED -34
#define ACL_DENY_REASON_V4_DISALLOWED_SUBS_LIMITED    -35
#define ACL_DENY_REASON_V6_DISALLOWED_SUBS_LIMITED    -36
#define ACL_DENY_REASON_COMPANION_IFACE_IN_USE        -37
#define ACL_DENY_REASON_MIP_V6_DISALLOWED             -38

#define ACL_DENY_REASON_MAX_V4_CONNECTIONS            -45
#define ACL_DENY_REASON_MAX_V6_CONNECTIONS            -46

#define ACL_DENY_REASON_EMERGENCY_AUTH_FAILURE        -63
#define ACL_DENY_REASON_IP_PDP_MISMATCH               -64
#define ACL_DENY_REASON_IP_VERSION_MISMATCH           -65
#define ACL_DENY_REASON_IP_ADDRESS_MISMATCH           -66
#define ACL_DENY_REASON_IFACE_AND_POL_FAMILY_MISMATCH -67

#define ACL_DENY_REASON_APN_MISMATCH                  -70

#define ACL_DENY_REASON_DUN_CALL_DISALLOWED           -75

#define ACL_DENY_REASON_INVALID_PROFILE               -85
#define ACL_DENY_REASON_INVALID_PROFILE_ID            -86
#define ACL_DENY_REASON_SUBS_ID_MISMATCH              -87

#define ACL_DENY_REASON_EPC_NONEPC_TRANSITION         -95

#define ACL_DENY_REASON_IFACE_MISMATCH                -105
#define ACL_DENY_REASON_UNPREFERRED_RAT               -106
#define ACL_DENY_REASON_EMERGENCY_IFACE_ONLY          -107


#define PASS( class, rule )                                                 \
  if ( rule )                                                               \
  {                                                                         \
    acl_ret_val = MAX( acl_ret_val, ACL_PRIORITY( class ) );                \
    acl_trace_pass(acl_ret_val);                                            \
  }

#define PASS_DEFAULT( class )                                               \
  acl_ret_val = MAX( acl_ret_val, ACL_PRIORITY( class ) );

#define PASS_QUICK( class, rule )                                           \
  if ( rule )                                                               \
  {                                                                         \
    acl_ret_val = MAX( acl_ret_val, ACL_PRIORITY( class ) );                \
    acl_trace_pass(acl_ret_val);                                            \
    return MAX( acl_ret_val, ACL_PRIORITY( class ) );                       \
  }

#define DENY( rule )   DENY_WITH_REASON( rule, ACL_DENY_REASON_POL_NO_MATCH )

#define DENY_WITH_REASON( rule, reason )                                    \
  ASSERT(reason >= ACL_DENY_REASON_POL_NO_MATCH);                            \
  deny_reason = reason;                                                     \
  if ( rule )                                                               \
  {                                                                         \
    acl_trace_deny(this_if_ptr, reason);                                    \
    return deny_reason;                                                     \
  }

#define DENY_WITH_ANY_REASON( rule )                                        \
  if ( rule )                                                               \
  {                                                                         \
    acl_trace_deny(this_if_ptr, deny_reason);                               \
    return deny_reason;                                                     \
  }

/*---------------------------------------------------------------------------
  Pointer checks on the policy and IP filter info
  If the required pointer is NULL, the rule becomes NOOP except when
  NOT is used before a rule.  It is not recommended to put a rule inside
  NOT.  Using NOT inside a rule is acceptable
---------------------------------------------------------------------------*/
#define USE_PKT_INFO( rule )  \
  ( (pkt_info_ptr) ? (rule) : FALSE )
#define USE_POL_INFO( rule )  \
  ( (policy_info_ptr) ? (rule) : FALSE )
#define USE_PKT_AND_POL_INFO( rule )  \
  ( (pkt_info_ptr && policy_info_ptr) ? (rule) : FALSE )


#define IFACE_TYPE_IS_ID()                                                  \
  USE_POL_INFO(                                                             \
    (policy_info_ptr->iface.kind == DSS_IFACE_ID) )

#define IFACE_TYPE_IS_NAME()                                                \
  USE_POL_INFO(                                                             \
    (policy_info_ptr->iface.kind == DSS_IFACE_NAME) &&                      \
    ((uint16)policy_info_ptr->iface.info.name & (uint16)IFACE_MASK) )

#define IFACE_TYPE_IS_GROUP( ifname )                                       \
  USE_POL_INFO(                                                             \
    (policy_info_ptr->iface.kind == DSS_IFACE_NAME) &&                      \
     !((uint16)policy_info_ptr->iface.info.name & (uint16)IFACE_MASK) &&    \
     /*lint -e(506) */ !((ifname) & IFACE_MASK) )

#define IFACE_REQUESTED_TYPE_IS_GROUP()                                     \
  USE_POL_INFO(                                                             \
    (policy_info_ptr->iface.kind == DSS_IFACE_NAME) &&                      \
     !((uint16)policy_info_ptr->iface.info.name & (uint16)IFACE_MASK) )

#define PROXY_IFACE_HANDLE_NO_MATCH(iface_pol_info_ptr)                     \
  (policy_info_ptr->rt_result.if_hndl !=                                    \
     (iface_pol_info_ptr)->rt_result.if_hndl)

#define IFACE_IS_UP( weight )   \
  (( ((ps_iface_type*) this_if_ptr)->if_state == IF_STATE_UP) ?             \
    ((acl_ret_val = ACL_PRIORITY( class ) + weight)                         \
     == (ACL_PRIORITY( class ) + weight))                                   \
    : 0 )

#define IFACE_SUPPORTS_ROUTE_ON_DEMAND( )                                   \
  USE_POL_INFO( policy_info_ptr->is_sock_orig                               \
                  | ((ps_iface_type*) this_if_ptr)->can_route_on_demand )

/*---------------------------------------------------------------------------
  build IPv4 Address - in NETWORK order
---------------------------------------------------------------------------*/
#define IPV4( arg1, arg2, arg3, arg4 )                                      \
  ((uint32)( (((uint32)(arg4 & 0xff)) << 24)                                \
           | (((uint32)(arg3 & 0xff)) << 16)                                \
           | (((uint32)(arg2 & 0xff)) << 8)                                 \
           | (((uint32)(arg1 & 0xff)))))

/*---------------------------------------------------------------------------
  Determine if this iface can come up with required config
---------------------------------------------------------------------------*/
#define SPECIFIED_CONFIG_NOT_SUPPORTED2( is_cfg_supported2_fptr )       \
  USE_POL_INFO(                                                         \
    is_cfg_supported2_fptr(                                             \
      this_if_ptr, policy_info_ptr, pkt_info_ptr, &deny_reason)         \
    == FALSE )

/*---------------------------------------------------------------------------
  Determine if this iface is available
---------------------------------------------------------------------------*/
#define IFACE_NOT_AVAILABLE2( is_available_fptr )       \
  USE_POL_INFO(                                                         \
    is_available_fptr(                                             \
      this_if_ptr, policy_info_ptr, pkt_info_ptr, &deny_reason)         \
    == FALSE )

/*---------------------------------------------------------------------------
  check if the IPv6 address is loopback '::1'. Since V6 addresses are always
  stored in network order and since REX uses little endian, we need to
  compare with ps_htonl(1).
---------------------------------------------------------------------------*/
#define DST_ADDR6_IS_LOOPBACK()                                         \
  USE_PKT_INFO(                                                         \
    (pkt_info_ptr->ip_hdr.v6.base_v6_hdr.dst_addr.ps_s6_addr64[0] == 0) && \
    (pkt_info_ptr->ip_hdr.v6.base_v6_hdr.dst_addr.ps_s6_addr32[2] == 0) && \
    (pkt_info_ptr->ip_hdr.v6.base_v6_hdr.dst_addr.ps_s6_addr32[3] == ps_htonl(1)))


/*===========================================================================
MACRO PS_IFACE_IS_SHAREABLE()

DESCRIPTION
  This macro returns if the iface can be shared with the one that wants to use
  it now

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  None

RETURN VALUE
  TRUE: if iface is shareable among more than one entities
  FALSE: if iface can only be acquired and used by one entity at a time

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_IS_SHAREABLE( iface_ptr )                                 \
  (PS_IFACE_IS_VALID(iface_ptr)                                            \
     ? (PS_IFACE_GET_IS_SHAREABLE(iface_ptr) == TRUE ||                    \
        (iface_ptr)->iface_private.proc_id == PS_IFACE_PROC_ID_NONE ||     \
        (iface_ptr)->iface_private.proc_id == PS_IFACE_PROC_ID_ANY  ||     \
        (iface_ptr)->iface_private.proc_id == policy_info_ptr->proc_id ||  \
        (policy_info_ptr->proc_id == PS_IFACE_PROC_ID_ANY &&               \
         (iface_ptr)->iface_private.proc_id == PS_IFACE_PROC_ID_LOCAL) ||  \
        policy_info_ptr->is_routeable == TRUE) &&                          \
        ((iface_ptr)->iface_private.subs_id == policy_info_ptr->subs_id)   \
     : FALSE)

/*===========================================================================
MACRO PS_IFACE_IS_SUBS_ID_MATCH()

DESCRIPTION
  This macro checks if the iface subs id matches the policy subs id

PARAMETERS
  iface_ptr: Target iface ptr

RETURN VALUE
  None

RETURN VALUE
  TRUE: if iface and subs id matches
  FALSE: if there is a mismatch between iface and policy subs

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
#define PS_IFACE_IS_SUBS_ID_MATCH( iface_ptr )                             \
  (PS_IFACE_IS_VALID(iface_ptr)                                            \
     ? (iface_ptr)->iface_private.subs_id == policy_info_ptr->subs_id      \
     : FALSE)

/*---------------------------------------------------------------------------
  there should be checking the port ranges, but a currently no-ops
---------------------------------------------------------------------------*/
#define SRC_PORT_IS_IN( range_start, range_end )
#define DST_PORT_IS_IN( range_start, range_end )

/*---------------------------------------------------------------------------
  is the destination address the broadcast address?
---------------------------------------------------------------------------*/
#define DST_IS_BRDCST  USE_PKT_INFO( pkt_info_ptr->is_brdcst )

/*---------------------------------------------------------------------------
  Is the destination IPv6 addr in the prefix/netmask passed in?
---------------------------------------------------------------------------*/
#define base_v6_hdr hdr_body.base_hdr
#define DST_ADDR6_IS_IN( prefix, netmask )                               \
  USE_PKT_INFO(                                                          \
   (pkt_info_ptr->ip_hdr.v6.base_v6_hdr.dst_addr.ps_s6_addr64[0] & netmask) \
    ==prefix)

#define PROTOCOL_IS_TCP   \
  USE_PKT_INFO( pkt_info_ptr->ip_hdr.protocol == PS_IPPROTO_TCP )
#define PROTOCOL_IS_UDP   \
  USE_PKT_INFO( pkt_info_ptr->ip_hdr.protocol == PS_IPPROTO_UDP )
#define PROTOCOL_IS_ICMP  \
  USE_PKT_INFO( pkt_info_ptr->ip_hdr.protocol == PS_IPPROTO_ICMP )
#define PROTOCOL_IS_ESP   \
  USE_PKT_INFO( pkt_info_ptr->ip_hdr.protocol == PS_IPPROTO_ESP )

#define ICMP_TYPE_IS( type )  \
  USE_PKT_INFO( pkt_info_ptr->icmp_type == type )
#define ICMP_CODE_IS( code )  \
  USE_PKT_INFO( pkt_info_ptr->icmp_code == code )

#define TYPE_OF_SVC_IS( tos_id )  \
  USE_PKT_INFO( pkt_info_ptr->ip_hdr.tos == tos_id )

/*---------------------------------------------------------------------------
  the IPv4 address for the local interface
---------------------------------------------------------------------------*/
#define LOCALHOST6  \
  USE_PKT_INFO(PS_IFACE_IP_V6_ADDR( ACL_IPFI_IF_PTR( pkt_info_ptr )))


/*---------------------------------------------------------------------------
  build IPv6 Prefix - in Network order (which is why shifts are reversed)
---------------------------------------------------------------------------*/
#define IPV6_PREFIX( arg1, arg2, arg3, arg4 )   \
  ((uint64)( ((arg1 & 0xffff))                  \
           | ((arg2 & 0xffff) << 16)            \
           | ((arg3 & 0xffff) << 32)            \
           | ((arg4 & 0xffff))))

/*---------------------------------------------------------------------------
  build IPv6 IID - in Network order (which is why shifts are reversed)
---------------------------------------------------------------------------*/
#define IPV6_IID( arg1, arg2, arg3, arg4 )      \
  ((uint64)( ((arg1 & 0xffff))                  \
           | ((arg2 & 0xffff) << 16)            \
           | ((arg3 & 0xffff) << 32)            \
           | ((arg4 & 0xffff))))

#define PKT_IS_SECURE   \
  USE_PKT_INFO( pkt_info_ptr->is_secure )

#define QOS_IS( qos_type )  \
  USE_POL_INFO( policy_info_ptr->qos == qos_type )

/*===========================================================================
                             ACL function macros
===========================================================================*/

/*---------------------------------------------------------------------------
  Syntactical definition as a group of rules via a function
---------------------------------------------------------------------------*/
#define ACL_EXEC( name, arg1, arg2, arg3 ) name( arg1, arg2, arg3 )
#define ACL_DEF( name )   \
  int name                                                                  \
  (                                                                         \
    void*                    this_if_ptr,                                   \
    acl_policy_info_type*    policy_info_ptr,                               \
    ip_pkt_info_type*        pkt_info_ptr                                   \
  )

/*---------------------------------------------------------------------------
  this_if_ptr must point to the iface associatd with the ACL
---------------------------------------------------------------------------*/
#define ACL_START  { int acl_ret_val = (int)ACL_DEFAULT_CLASS;              \
  int deny_reason = ACL_DENY_REASON_POL_NO_MATCH;                           \
  if(this_if_ptr == NULL)                                                   \
  {                                                                         \
    ASSERT(0);                                                              \
    return -1;                                                              \
  }                                                                         \
  PASS_QUICK( ACL_CONFIG_CLASS, IFACE_ID_MATCH() );                         \
  acl_trace_start(this_if_ptr);

/*---------------------------------------------------------------------------
  Global ACLs are not associated with any iface, so this_if_ptr is NULL
---------------------------------------------------------------------------*/
#define ACL_START_GLOBAL  { int acl_ret_val = ACL_DEFAULT_CLASS;            \
   /*-----------------------------------------------------------------------
     The below var is added to avoid errors/warnings for DENY rule. No real
     usage for START_GLOBAL.
   -----------------------------------------------------------------------*/\
                            int deny_reason = ACL_DENY_REASON_POL_NO_MATCH;

#define ACL_END                                                             \
  acl_trace_end(this_if_ptr, acl_ret_val);                                  \
return acl_ret_val; }

/*---------------------------------------------------------------------------
  Syntactical declaration of the ACL's postprocessor
---------------------------------------------------------------------------*/
#define ACL_POST_PROC_DEF( name )  \
  void name                                                                 \
  (                                                                         \
   void*                    this_if_ptr,                                    \
   acl_policy_info_type*    policy_info_ptr,                                \
   ip_pkt_info_type*        pkt_info_ptr                                    \
  )

/*---------------------------------------------------------------------------
  Syntactical declaration of iface is available function
---------------------------------------------------------------------------*/
#define ACL_IS_AVAILABLE_DEF( name )  \
  boolean name                                                              \
  (                                                                         \
   void*                    this_if_ptr,                                    \
   acl_policy_info_type*    policy_info_ptr,                                \
   ip_pkt_info_type*        pkt_info_ptr                                    \
  )

/*---------------------------------------------------------------------------
  Syntactical declaration of iface is available function which returns
  reason
---------------------------------------------------------------------------*/
#define ACL_IS_AVAILABLE_DEF2( name )  \
  boolean name                                                              \
  (                                                                         \
   void*                    this_if_ptr,                                    \
   acl_policy_info_type*    policy_info_ptr,                                \
   ip_pkt_info_type*        pkt_info_ptr,                                   \
   int*                     reason_for_deny                                 \
  )

/*---------------------------------------------------------------------------
  Syntactical declaration of config supported function
---------------------------------------------------------------------------*/
#define ACL_CFG_SUPPORTED_DEF( name )  \
  boolean name                                                              \
  (                                                                         \
   void*                    this_if_ptr,                                    \
   acl_policy_info_type*    policy_info_ptr,                                \
   ip_pkt_info_type*        pkt_info_ptr                                    \
  )

/*---------------------------------------------------------------------------
  Syntactical declaration of config supported function which returns reason
---------------------------------------------------------------------------*/
#define ACL_CFG_SUPPORTED_DEF2( name )  \
  boolean name                                                              \
  (                                                                         \
   void*                    this_if_ptr,                                    \
   acl_policy_info_type*    policy_info_ptr,                                \
   ip_pkt_info_type*        pkt_info_ptr,                                   \
   int*                     reason_for_deny                                 \
  )

/*---------------------------------------------------------------------------
  Syntactical declaration config shareable function
---------------------------------------------------------------------------*/
#define ACL_CFG_SHAREABLE_DEF( name )  \
  boolean name                                                              \
  (                                                                         \
   void*                    this_if_ptr,                                    \
   acl_policy_info_type*    policy_info_ptr,                                \
   ip_pkt_info_type*        pkt_info_ptr                                    \
  )

/*---------------------------------------------------------------------------
  Map a rule's class into a routing priority
---------------------------------------------------------------------------*/
#define ACL_PRIORITY( class )  acl_class_priorities[class]


/*===========================================================================

                              TYPE DEFINITIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Classes of ACL rules to prioritize multiple matches:
    The DEFAULT_CLASS is used to specify the default fallback priority, e.g.,
    for routing ACLs the decision between two DEFAULT_CLASS priorities would
    be based on IP address based routing (always priority 1). The additional
    rule classes allow tp prioritize between individual policies, e.g., a
    directly specified interface type would have a higher priority than a QOS
    type or a source IP address.
---------------------------------------------------------------------------*/
typedef enum
{
  ACL_DEFAULT_CLASS = 0,
  ACL_IFGROUP_CLASS,
  ACL_IFNAME_CLASS,
  ACL_IPADDR_CLASS,
  ACL_IPPORT_CLASS,
  ACL_QOS_CLASS,
  ACL_LOOPBACK_CLASS,
  ACL_SLIP_MODE_CLASS,
  ACL_IPSEC_CLASS,
  ACL_PREF_MODE_CLASS,
  ACL_CONFIG_CLASS,
  ACL_UP_PREF_CLASS,
  ACL_EMRG_CLASS,
  ACL_MIP_CLASS,
  ACL_IWLAN_CLASS,
  ACL_PROXY_CONFIG_CLASS,
  ACL_NAT_CLASS,
  ACL_CLAT_CLASS,
  ACL_NUM_CLASSES
} acl_rule_class_enum_type;


/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Priority lookup table to map rule classes into routing priorities
---------------------------------------------------------------------------*/
extern const int acl_class_priorities[ACL_NUM_CLASSES];

#endif    /* PS_ACLRULES_H */
