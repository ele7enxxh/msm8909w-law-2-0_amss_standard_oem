#ifndef PS_DNSI_MSG_MGR_H
#define PS_DNSI_MSG_MGR_H

/*===========================================================================

                       P S _ D N S I _ M S G _ M G R _ H

DESCRIPTION

  The Data Services DNS Subsystem Message manager header file. Contains
  declarations of functions, variables, macros, structs and enums
  used by DNS message manager.

EXTERNALISED FUNCTIONS

  ps_dnsi_msg_mgr_init()
    Initializes the msg manager module during power-up.

  ps_dnsi_msg_mgr_form_query()
    Using query type, query class and query data; construct a DSM item chain
    corresponding to the DNS query message to be sent.

  ps_dnsi_msg_mgr_parse_response()
    Parse the DSM item chain reply into set of RRs.

INTIALIZATIONS AND SEQUENCING REQUIREMENTS
  This module should be run only in PS task context.

Copyright (c) 2007-2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"


#include "dssdns.h"
#include "ps_dnsi_defs.h"
#include "ps_mem.h"
#include "dssinternal.h"
#include "ps_handle_mgr.h"
#include "dsm.h"
#include "ps_dnsi_resolver.h"


/*===========================================================================

                         EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================
FUNCTION  PS_DNSI_MSG_MGR_INIT()

DESCRIPTION
  Initializes the msg manager module during power-up.

PARAMETERS
  None.

RETURN VALUE
  None.

DEPENDENCIES
  The Queue utils subsystem must be initialzed before this.

SIDE EFFECTS
  None.
===========================================================================*/
void ps_dnsi_msg_mgr_init
(
  void
);

/*===========================================================================
FUNCTION  PS_DNSI_MSG_MGR_RESOLVE_WILDCARD_QUERY()

DESCRIPTION
  Check if the query is a "wildcard query": a NULL hostname with 
  AI_PASSIVE flag set. If so returns the result as a generic RR Q node 
  in the Q parameter passed in.

PARAMETERS
  query_data_ptr       - Pointer to the query data
  query_type           - Type of query
  query_class          - Class of query
  rr_q                 - Pointer to the Q where the results should be
                         returned.
  errno                - Error code in case of error.

RETURN VALUE
  On Success, Return DSS_SUCCESS and set the rr_q parameter
  On Error, Return DSS_ERROR

  errno values
  ------------
  DS_EFAULT         - If the arguments are wrong.
  DS_NAMEERR        - Domain name malformed.
  DS_ENOMEM         - Out of memory.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 ps_dnsi_msg_mgr_resolve_wildcard_query
(
  ps_dnsi_query_type_enum_type    query_type,
  ps_dnsi_query_class_enum_type   query_class,
  char                          * query_data_ptr,
  q_type                        * rr_q,
  int16                         * ps_errno
);

/*===========================================================================
FUNCTION  PS_DNSI_MSG_MGR_RESOLVE_NUMERICAL_QUERY()

DESCRIPTION
  Checks if the query passed in IPv4 dotted decimal or IPv6 colon seperated
  format. If so returns the result as a generic RR Q node in the Q parameter
  passed in.

PARAMETERS
  query_type           - Type of query
  query_class          - Class of query
  query_data_ptr       - Pointer to the query data
  rr_q                 - Pointer to the Q where the results should be
                         returned.
  errno                - Error code in case of error.

RETURN VALUE
  On Success, Return DSS_SUCCESS and set the rr_q parameter
  On Error, Return DSS_ERROR

  errno values
  ------------
  DS_EFAULT         - If the arguments are wrong.
  DS_NAMEERR        - Domain name malformed.
  DS_ENOMEM         - Out of memory.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 ps_dnsi_msg_mgr_resolve_numerical_query
(
  ps_dnsi_query_type_enum_type         query_type,
  ps_dnsi_query_class_enum_type        query_class,
  char                               * query_data_ptr,
  q_type                             * rr_q,
  int16                              * ps_errno
);

/*===========================================================================
FUNCTION  PS_DNSI_MSG_MGR_FORM_QUERY()

DESCRIPTION
  Using query type, query class and query data; construct a DSM item chain
  corresponding to the DNS query message to be sent.

PARAMETERS
  domain_name_ptr      - Hostname argument(for A/AAAA/CNAME query) or
                         a.b.c.d.in-addr.arpa(for PTR query)
  query_type           - Type of query
  query_class          - Class of query
  is_recursion_desired - Should the request indicate DNS recursion
  dns_query_ptr_ptr    - DSM item pointer where the resultant query
                         would be returned.
  errno                - Error code in case of error.

RETURN VALUE
  On Success, Return message id for the formed message
  On Error, Return DSS_ERROR

  errno values
  ------------
  DS_EFAULT         - If the arguments are wrong.
  DS_ESYSTEM        - System failure.
  DS_NAMEERR        - Domain name malformed.
  DS_ENOMEM         - Out of memory.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 ps_dnsi_msg_mgr_form_query
(
  char                               * domain_name_ptr,
  ps_dnsi_query_type_enum_type         query_type,
  ps_dnsi_query_class_enum_type        query_class,
  boolean                              is_recursion_desired,
  dsm_item_type                     ** dns_query_ptr_ptr,
  int16                             *  ps_errno
);

/*===========================================================================
FUNCTION  PS_DNSI_MSG_MGR_PARSE_RESPONSE

DESCRIPTION
  Parse the DSM item chain response into set of RRs.
  A generic RR is a node for holding information from Answer,
  authority, and additional info RRs. The RR Queue is returned
  in rr_q variable, which should be initialized before calling
  this function.

PARAMETERS
  dns_response_ptr  - DSM item pointer to DNS response msg
  rr_q              - Out parameter. Q of generic RRs to be returned.
  errno             - Error code in case of error.

RETURN VALUE
  On success returns the msg_id of the parsed message.
  DSS_ERROR   - On error and puts the error code in errno.

  errno values
  ------------
  DS_EFAULT         - If the arguments are wrong.
  DS_ESYSTEM        - System failure.
  DS_NAMEERR        - Domain name malformed.
  DS_EHOSTNOTFOUND  - Host not found.
  DS_EOPNOTSUPP     - Option not supported.
  DS_EINVAL         - Invalid Operation.
  DS_ECONNREFUSED   - Connection refused by peer.
  DS_ENOMEM         - Out of memory.

DEPENDENCIES
  None.

SIDE EFFECTS
  None.
===========================================================================*/
int16 ps_dnsi_msg_mgr_parse_response
(
  ps_dnsi_query_type_enum_type          query_type,
  ps_dnsi_query_class_enum_type         query_class,
  char                                * query_data_ptr,
  dsm_item_type               * dns_response_ptr,
  q_type                      * rr_q,
  int16                       * ps_errno
);

#endif /* PS_DNSI_MSG_MGR_H */
