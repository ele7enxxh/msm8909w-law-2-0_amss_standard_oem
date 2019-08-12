#ifndef DSMIP_PDSN_DB_H
#define DSMIP_PDSN_DB_H
/*===========================================================================

                        D S M I P _ P D S N _ D B . H

DESCRIPTION

  File containing the data structure and access methods for maintianing the
  SID/NID/PZID -> PDSN IP address mappings.  Also determines whether or not
  DRS should be set when "originating" on sid/nid/pzid change.

EXTERNALIZED FUNCTIONS
  pdsn_db_init()
    Initialize the database
  pdsn_db_enable()
    enable or disable the database
  pdsn_db_put()
    insert an entry into the database
  pdsn_db_get_drs()
    should the DRS bit be set on a re-origination?

 Copyright (c) 2001 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_pdsn_db.h_v   1.3   07 Aug 2001 22:02:02   jeffd  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/inc/dsmip_pdsn_db.h#1 $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
07/27/01    mvl/na Renamed pdsn_db_get_drs() to pdsn_db_get_kind(). Added
                   pdsn_db_kind_enum_type.
07/10/01    mvl    Added prototypes for  pdsn_db_get_current() and
                   pdsn_db_is_current() 
07/09/01    mvl    Changed decalaration to remove an error
07/05/01    mvl    Created module
===========================================================================*/


/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/
#include "datamodem_variation.h"
#include "comdef.h"

/*===========================================================================

                      REGIONAL DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Enum which returns what the mobile thinks the PDSN-PCF combination is -
  is it a new PDSN or visited one.
---------------------------------------------------------------------------*/
typedef enum
{
  DB_DISABLED,        /* The PDSN database has been disabled              */
  SAME_PDSN_SAME_PCF, /* The PDSN is current as is the RP link - same PCF */
  SAME_PDSN_DIFF_PCF, /* The PDSN is current but the RP isn't - diff PCF  */
  VISITED_PDSN,       /* The PDSN is not current but has been visited     */
  UNKNOWN_PDSN        /* The PDSN seems to be new now PCF ID in DB        */ 
} pdsn_db_kind_enum_type;

/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION PDSN_DB_INIT()

DESCRIPTION
  This function initializes the pdsn database.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pdsn_db_init( void);

/*===========================================================================
FUNCTION PDSN_DB_ENABLE()

DESCRIPTION
  This function is used to enable and disable the DRS algorithm

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pdsn_db_enable
(
  boolean enable
);

/*===========================================================================
FUNCTION PDSN_DB_PUT()

DESCRIPTION
  This function creates a new pdsn database entry.  If the DB is full the
  oldest entry will be replaced.  The oldest entry is the one with the
  smallest snpdbi_relative time entry.

PARAMETERS
  sid: the system ID to be entered
  nid: the network ID to be entered
  pzid: the packet zone id to be entered
  ip_addr: the PDSNs IP address

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pdsn_db_put
(
  uint32 ip_addr
);

/*===========================================================================
FUNCTION PDSN_DB_GET_KIND()

DESCRIPTION
  This function returns an indication of whether the PDSN correspinding to
  the current SID/NID/PZID is new, old or the same as the current one

PARAMETERS
  None

RETURN VALUE
  pdsn_kind_type

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
pdsn_db_kind_enum_type pdsn_db_get_kind
( 
  void
);

/*===========================================================================
FUNCTION PDSN_DB_GET_CURRENT()

DESCRIPTION
  This function returns the IP address of the current PDSN

PARAMETERS
  None

RETURN VALUE 
  uint32 - IP address of the current PDSN

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 pdsn_db_get_current
(
  void
);

/*===========================================================================
FUNCTION PDSN_DB_IS_CURRENT()

DESCRIPTION
  This function returns whether the PDSN address passed in corresponds to the
  PDSN for the PCF where we find ourselves.

PARAMETERS
  pdsn_ip: the IP address of the PDSN

RETURN VALUE
  boolean:
    TRUE:  if the PDSN address matches for the PCF where the mobile is
           currently.
    FALSE: if there is no match for the PDSN address passed in.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean pdsn_db_is_current
(
  uint32 pdsn_ip
);

#ifndef T_ARM
/*===========================================================================
FUNCTION PDSN_DB_PRINT()

DESCRIPTION
  This function prints the contents of the PDSN database.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pdsn_db_print
( 
  void
);

#endif /* T_ARM */
#endif /* DSMIP_PDSN_DB_H */
