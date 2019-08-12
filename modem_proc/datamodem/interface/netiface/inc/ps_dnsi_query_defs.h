#ifndef PS_DNSI_QUERY_DEFS_H
#define PS_DNSI_QUERY_DEFS_H
/*===========================================================================

              P S  _  D N S I  _  Q U E R Y  _  D E F S  .  H

DESCRIPTION

  A Data Services DNS module internal Header File. Contains declarations
  of enums related to queries used by the DNS subsystem.

EXTERNALIZED FUNCTIONS
  None.

INITIALIZATIONS AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/

#include "queue.h"

/*===========================================================================

                            INTERNAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Maximum length of domain names and service names
---------------------------------------------------------------------------*/
#define PS_DNSI_MAX_DOMAIN_NAME_LEN     256

/*---------------------------------------------------------------------------
  Typedef for Query Class: Currently supported is only IN (internet) class.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_DNSI_QUERY_CLASS_MIN    = 0,
  PS_DNSI_QUERY_CLASS_IN     = PS_DNSI_QUERY_CLASS_MIN,
  PS_DNSI_QUERY_CLASS_MAX    = 1,
  PS_DNSI_QUERY_CLASS_FORCE_SIGNED_32_BIT = 0x7FFFFFFF
} ps_dnsi_query_class_enum_type;

/*---------------------------------------------------------------------------
  Enum for query type: currently supported query types are only A/AAAA/PTR
---------------------------------------------------------------------------*/
typedef enum
{
  PS_DNSI_QUERY_TYPE_MIN      = 0,
  PS_DNSI_QUERY_TYPE_A        = PS_DNSI_QUERY_TYPE_MIN,
  PS_DNSI_QUERY_TYPE_AAAA     = 1,
  PS_DNSI_QUERY_TYPE_PTR      = 2,
  PS_DNSI_QUERY_TYPE_CNAME    = 3,
  PS_DNSI_QUERY_TYPE_MAX      = 4,
  PS_DNSI_QUERY_TYPE_FORCE_SIGNED_32_BIT = 0x7FFFFFFF
} ps_dnsi_query_type_enum_type;

/*---------------------------------------------------------------------------
  Data structure for a Generic Resource Record
---------------------------------------------------------------------------*/
typedef struct ps_dnsi_generic_rr_s_type
{
  q_link_type                      link;
  uint32                           ttl;
  char                             domain_name[PS_DNSI_MAX_DOMAIN_NAME_LEN]; //Should be char * we are an embedded system, memory is precious :-)
  unsigned char                  * rdata;
  int16                            rdata_len;
  ps_dnsi_query_type_enum_type     rr_type;
  ps_dnsi_query_class_enum_type    rr_class;
} ps_dnsi_generic_rr_type;

#endif  /* PS_DNSI_QUERY_DEFS_H */
