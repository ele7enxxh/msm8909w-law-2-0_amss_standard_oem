#ifndef A2_IPFILTER_H
#define A2_IPFILTER_H
/*!
  @file a2_ipfilter.h

  @brief
    This file contains the interface to functions used to 
    configure the filtering rules.

  The procedure for configuring filter rules are as follows:

  -# Create a filter ruleset via a2_ipfilter_add_ruleset()
  -# Add rules to the rule set
  -# Repeat for as many rules and rule sets are necessary.
  -# Commit changes via a2_ipfilter_commit_rules()

  All rules in a ruleset will be "anded" together to verify match of that
  ruleset.  Each ruleset will be compared against until one rule matches.
  Any data which is matched using a ruleset will be sent back to PS using
  one of the ds watermarks.

  Many filter rules are based upon fields in the ip headers which follow.
  
  IPv4 Header:
  @verbatim
   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |Version|  IHL  |Type of Service|          Total Length         |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |         Identification        |Flags|      Fragment Offset    |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |  Time to Live |    Protocol   |         Header Checksum       |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                       Source Address                          |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Destination Address                        |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Options                    |    Padding    |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                             data                              |
  \                                                               \
  \                                                               \
  |                                                               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  @endverbatim
  
  IPv6 Base Header:
  @verbatim
    0                   1                   2                   3   
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |Version| Traffic Class |           Flow Label                  |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |         Payload Length        |  Next Header  |   Hop Limit   |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                                                               |
   +                                                               +
   |                                                               |
   +                         Source Address                        +
   |                                                               |
   +                                                               +
   |                                                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   |                                                               |
   +                                                               +
   |                                                               |
   +                      Destination Address                      +
   |                                                               |
   +                                                               +
   |                                                               |
   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  @endverbatim

  @ingroup per_implementation
  @ingroup a2_ext_interface
*/

/*==============================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  QUALCOMM Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of QUALCOMM Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/a2/a2_ipfilter.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/17/14   bn      CR 597515, increase ip filter MAX_NUM_RULE_BUFFER
02/26/13   bn      CR 455796 fix 8-pdn issue, expand ip filter rule buffer
11/22/10   bn      Use DSM for IP filter RAM to support multi-clients
05/24/10   bn      Added in NULL ruleset API
04/30/09   sah     Use common ip type enum.
04/03/09   sah     Tweak function names to be more explicit.
02/18/09   sah     Initial Checkin

==============================================================================*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

#include <comdef.h>
#include <sio.h>
#include <a2_common.h>

/*==============================================================================

                               MACROS

==============================================================================*/
/*! @brief the number of filter rule sets
 */
#define A2_IPFILTER_MAX_NUM_RULE_SETS 32

/*! @brief the SW maximum number of rule buffer
 */
#define A2_IPFILTER_SW_MAX_NUM_RULE_BUFFER (4 * A2_IPFILTER_MAX_NUM_RULE_SETS)

/*! @brief The maximum number of Mask Equal 32 Rules */
#define A2_IPFILTER_RULE_NUM_MEQ_32_RULES 4

/*! @brief The maximum number of Ip header length offset Mask Equal 32 Rules*/
#define A2_IPFILTER_RULE_NUM_IHL_MEQ_32_RULES 2

/*! @brief The maximum number of Mask Equal 128 Rules */
#define A2_IPFILTER_RULE_NUM_MEQ_128_RULES 2

/*! @brief The maximum number of Ip header length offset Range Check 16 Rules*/
#define A2_IPFILTER_RULE_NUM_IHL_RANGE_16_RULES 2


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! The rule handle used to identify a rule */
typedef void* a2_ipfilter_rule_set_handle_t;

/*! Return code for configuring a filter rule */
typedef enum
{
  A2_IPFILTER_RULE_INSUFFICIENT_SPACE,
  A2_IPFILTER_RULE_TOO_MANY_RULES_OF_THAT_TYPE,
  A2_IPFILTER_RULE_SUCCESS
} a2_ipfilter_rule_result_e;


/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  a2_ipfilter_add_ruleset

==============================================================================*/
/*!
  @brief
  Adds a ruleset to the filter rules

  @return NULL when no ruleset is available.
*/
/*============================================================================*/
a2_ipfilter_rule_set_handle_t* a2_ipfilter_add_ruleset 
( 
  /*! The ip type of the rule being added.  This rule will be applied to 
      all incoming data of that ip type.
   */
  a2_ip_type_e ip_type,
  /*! The port to send the data to if the ruleset is a match. 
    (SIO_PORT_NULL for external memory) */
  sio_port_id_type      sio_port
);

/*==============================================================================

  FUNCTION:  a2_ipfilter_remove_ruleset

==============================================================================*/
/*!
  @brief
  Initializes the filter module

  @return NULL when no ruleset is available.
*/
/*============================================================================*/
void a2_ipfilter_remove_ruleset 
( 
  /*! The rule set to be removed. */
  a2_ipfilter_rule_set_handle_t rule_handle,
  /*! The ip type of the rule being removed */
  a2_ip_type_e ip_type
);

/*==============================================================================

  FUNCTION:  a2_ipfilter_commit_rules

==============================================================================*/
/*!
  @brief
  Commits the changes to the filter rules and updates the rules in the
  hardware.

*/
/*============================================================================*/
void a2_ipfilter_commit_rules( void );


/*==============================================================================

  FUNCTION:  a2_ipfilter_add_rule_meq_32

==============================================================================*/
/*!
  @brief
  Adds a Mask Equal 32 rule to the filter rule set.

  This rule will cause the A2 to perform the following comparison:
  
  <code>
  value == (*(uint32)((((uint8*)pkt_ptr) + offset)) & mask;
  </code>

  To apply the added rules call a2_ipfilter_commit_rules() to update the
  rules in the A2 hardware.

  @return A2_IPFILTER_RULE_SUCCESS upon success.
*/
/*============================================================================*/
a2_ipfilter_rule_result_e a2_ipfilter_add_rule_meq_32
( 
  /*! The rule set to add the rule to. */
  a2_ipfilter_rule_set_handle_t rule_handle,
  /*! The offset in bytes to use in the rule */
  uint8 offset,
  /*! The mask to use in the rule */
  uint32 mask,
  /*! The value to compare against after masking */
  uint32 value 
);

/*==============================================================================

  FUNCTION:  a2_ipfilter_add_rule_meq_128

==============================================================================*/
/*!
  @brief
  Adds a Mask Equal 128 rule to the filter rule set.

  This rule will cause the A2 to perform the following comparison in 
  pseudo code:
  
  <code>
  value == (*(uint128)((((uint8*)pkt_ptr) + offset)) & mask;
  </code>

  To apply the added rules call a2_ipfilter_commit_rules() to update the
  rules in the A2 hardware.

  @return A2_IPFILTER_RULE_SUCCESS upon success.
*/
/*============================================================================*/
a2_ipfilter_rule_result_e a2_ipfilter_add_rule_meq_128
( 
  /*! The rule set to add the rule to. */
  a2_ipfilter_rule_set_handle_t rule_handle,
  /*! The offset in bytes to use in the rule */
  uint8 offset,
  /*! Pointer to the mask to use in the rule, the bytes shall be ordered most 
   significant byte of the mask in mask[0] -> least significant byte in mask[15]
   */
  uint8 mask[16],
  /*! Pointer to the value to compare against after masking 
    The bytes shall be ordered most significant byte of the mask in value[0] 
    -> least significant byte in value[15].
   */
  uint8 value[16]
);

/*==============================================================================

  FUNCTION:  a2_ipfilter_add_rule_ihl_meq_32

==============================================================================*/
/*!
  @brief
  Adds an IHL  Mask Equal 32 rule to the filter rule set. This rule will add
  the header length read from the ip header to the offset.

  This rule will cause the A2 to perform the following comparison:
  
  <code>
  value == (*(uint32)((((uint8*)pkt_ptr) + ip_header_length + offset)) & mask
  </code>

  To apply the added rules call a2_ipfilter_commit_rules() to update the
  rules in the A2 hardware.

  @return A2_IPFILTER_RULE_SUCCESS upon success.
*/
/*============================================================================*/
a2_ipfilter_rule_result_e a2_ipfilter_add_rule_ihl_meq_32
( 
  /*! The rule set to add the rule to. */
  a2_ipfilter_rule_set_handle_t rule_handle,
  /*! The offset in bytes to use in the rule */
  uint8 offset,
  /*! The mask to use in the rule */
  uint32 mask,
  /*! The value to compare against after masking */
  uint32 value 
);


/*==============================================================================

  FUNCTION:  a2_ipfilter_add_rule_ihl_offset_eq_32

==============================================================================*/
/*!
  @brief
  Adds an IHL Equal 32 rule to the filter rule set. This rule will add
  the header length read from the ip header to the offset.

  This rule will cause the A2 to perform the following comparison:
  
  <code>
  value == (*(uint32)((((uint8*)pkt_ptr) + ip_header_length + offset))
  </code>

  To apply the added rules call a2_ipfilter_commit_rules() to update the
  rules in the A2 hardware.

  @return A2_IPFILTER_RULE_SUCCESS upon success.
*/
/*============================================================================*/
a2_ipfilter_rule_result_e a2_ipfilter_add_rule_ihl_offset_eq_32
( 
  /*! The rule set to add the rule to. */
  a2_ipfilter_rule_set_handle_t rule_handle,
  /*! The offset in bytes to use in the rule */
  uint8 offset,
  /*! The value to compare against after masking */
  uint32 value 
);

/*==============================================================================

  FUNCTION:  a2_ipfilter_add_rule_ihl_offset_eq_16

==============================================================================*/
/*!
  @brief
  Adds an IHL offset 16 rule to the filter rule set. This rule will add
  the header length read from the ip header to the offset.

  This rule will cause the A2 to perform the following comparison:
  
  <code>
  value == (*(uint16)((((uint8*)pkt_ptr) ip_header_length + offset)))
  </code>

  To apply the added rules call a2_ipfilter_commit_rules() to update the
  rules in the A2 hardware.

  @return A2_IPFILTER_RULE_SUCCESS upon success.
*/
/*============================================================================*/
a2_ipfilter_rule_result_e a2_ipfilter_add_rule_ihl_offset_eq_16
( 
  /*! The rule set to add the rule to. */
  a2_ipfilter_rule_set_handle_t rule_handle,
  /*! The offset in bytes to use in the rule */
  uint8 offset,
  /*! The value to compare against */
  uint16 value
);

/*==============================================================================

  FUNCTION:  a2_ipfilter_add_rule_ihl_offset_range_16

==============================================================================*/
/*!
  @brief
  Adds an IHL offset range 16 rule to the filter rule set. This rule will add
  the header length read from the ip header to the offset.

  This rule will cause the A2 to perform the following comparison:
  
  <code>
  ( range_low <= (*(uint16)((((uint8*)pkt_ptr) ip_header_length + offset))) &&
  (*(uint16)((((uint8*)pkt_ptr) ip_header_length + offset))) <= range_high )
  </code>

  To apply the added rules call a2_ipfilter_commit_rules() to update the
  rules in the A2 hardware.

  @return A2_IPFILTER_RULE_SUCCESS upon success.
*/
/*============================================================================*/
a2_ipfilter_rule_result_e a2_ipfilter_add_rule_ihl_offset_range_16
( 
  /*! The rule set to add the rule to. */
  a2_ipfilter_rule_set_handle_t rule_handle,
  /*! The offset in bytes to use in the rule */
  uint8 offset,
  /*! The low threshold of the range */
  uint16 range_low,
  /*! The high threshold of the range */
  uint16 range_high
);

/*==============================================================================

  FUNCTION:  a2_ipfilter_add_rule_tos_eq

==============================================================================*/
/*!
  @brief
  Checks that the type of service field in the ipv4 header matches the 
  specified value.

  To apply the added rules call a2_ipfilter_commit_rules() to update the
  rules in the A2 hardware.

  @return A2_IPFILTER_RULE_SUCCESS upon success.
*/
/*============================================================================*/
a2_ipfilter_rule_result_e a2_ipfilter_add_rule_tos_eq
( 
  /*! The rule set to add the rule to. */
  a2_ipfilter_rule_set_handle_t rule_handle,
  /*! The value to compare against */
  uint8 value
);

/*==============================================================================

  FUNCTION:  a2_ipfilter_add_rule_tc_eq

==============================================================================*/
/*!
  @brief
  Checks that the traffic class/priority field of the ipv6 header matches
  the specified value.

  To apply the added rules call a2_ipfilter_commit_rules() to update the
  rules in the A2 hardware.

  @return A2_IPFILTER_RULE_SUCCESS upon success.
*/
/*============================================================================*/
a2_ipfilter_rule_result_e a2_ipfilter_add_rule_tc_eq
( 
  /*! The rule set to add the rule to. */
  a2_ipfilter_rule_set_handle_t rule_handle,
  /*! The value to compare against */
  uint8 value
);

/*==============================================================================

  FUNCTION:  a2_ipfilter_add_rule_protocol_eq

==============================================================================*/
/*!
  @brief
  Checks that the protocol field of the ipv6 header compares matches
  specified value.

  To apply the added rules call a2_ipfilter_commit_rules() to update the
  rules in the A2 hardware.

  @return A2_IPFILTER_RULE_SUCCESS upon success.
*/
/*============================================================================*/
a2_ipfilter_rule_result_e a2_ipfilter_add_rule_protocol_eq
( 
  /*! The rule set to add the rule to. */
  a2_ipfilter_rule_set_handle_t rule_handle,
  /*! The value to compare against */
  uint8 value
);

/*==============================================================================

  FUNCTION:  a2_ipfilter_add_rule_flow_eq

==============================================================================*/
/*!
  @brief
  Checks that the flow field of the ipv6 header matches the
  specified value.

  To apply the added rules call a2_ipfilter_commit_rules() to update the
  rules in the A2 hardware.

  @return A2_IPFILTER_RULE_SUCCESS upon success.
*/
/*============================================================================*/
a2_ipfilter_rule_result_e a2_ipfilter_add_rule_flow_eq
( 
  /*! The rule set to add the rule to. */
  a2_ipfilter_rule_set_handle_t rule_handle,
  /*! The value to compare against */
  uint32 value
);

/*==============================================================================

  FUNCTION:  a2_ipfilter_init_null_ruleset

==============================================================================*/
/*!
  @brief
  Initilize the NULL rulesets for IPV4/IPV6 to the A2 internal HW IP Filter
  RAM.

  @Note
  1) For external caller , the input dl_per_context must be set to FALSE
  2) For caller in within DL PER context, it can be optimized calling it
  with the input dl_per_context set to TRUE and following calling 
  a2_dl_peri_commit_filter_rules

*/
/*============================================================================*/
dsm_item_type* a2_ipfilter_init_null_ruleset
(
  boolean dl_per_context /*!< caller in DL PER context? */
);
#endif /* A2_IPFILTER_H */

