#ifndef DS1X_META_INFO_H
#define DS1X_META_INFO_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         D S 1 X _ M E T A _ I N F O . H

GENERAL DESCRIPTION

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

     Copyright (c)2007 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/inc/ds1x_meta_info.h#1 $
  $Author: mplcsds1 $ $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/21/06    spn    Created the file.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "amssassert.h"
#include "comdef.h"


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            DEFINES AND TYPEDEFS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*---------------------------------------------------------------------------
  This is the kind that the DSM ITEM will be set to if there is a DS1X META 
  INFO
---------------------------------------------------------------------------*/
#define DS1X_META_INFO_KIND 	2

/*---------------------------------------------------------------------------
TYPEDEF DS1X_META_SDB_TYPE

DESCRIPTION
  Meta info if we have DOS/SDB.
---------------------------------------------------------------------------*/

typedef struct
{
  uint32                  tx_flags;
  int32                   sockfd;
} ds1x_meta_sdb_type;

/*---------------------------------------------------------------------------
TYPEDEF DS1X_META_TIMESTAMP_TYPE

DESCRIPTION
  Meta info if we have TIMESTAMP along with the packet.
---------------------------------------------------------------------------*/
typedef struct 
{
  qword                   value;
} ds1x_meta_timestamp_type;

/*---------------------------------------------------------------------------
TYPEDEF DS1X_META_INFO_KIND_ENUM_TYPE

DESCRIPTION
  Enum for the different of META INFOs that we support. This is a bitmask.
---------------------------------------------------------------------------*/
typedef enum
{
  DS1X_META_SDB_KIND          =   0x01,
  DS1X_META_TIMESTAMP_KIND    =   0x02
} ds1x_meta_kind_enum_type;

/*---------------------------------------------------------------------------
TYPEDEF DS1X_META_INFO_TYPE

DESCRIPTION
  This is used to pass per packet meta information through the stack.
  It moves along with the dsm item carrying the actual IP packet.
---------------------------------------------------------------------------*/
typedef struct
{
  ds1x_meta_kind_enum_type    ds1x_meta_kind;
  struct
  {
    ds1x_meta_sdb_type        sdb;
    ds1x_meta_timestamp_type  timestamp;
  } info;
} ds1x_meta_info_type;


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                   MACROS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#endif /* DS1X_META_INFO_H */
