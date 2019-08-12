#ifndef _DSMIP_EVENT_DEFS_H
#define _DSMIP_EVENT_DEFS_H

/*===========================================================================

         M O B I L E   I P   E V E N T S    D E F I N I T I O N S
                   
DESCRIPTION

 The Data Services Mobile IP Header File.  Contains the mobile I/P task
 entry point and exports any necessary information.

 Copyright (c) 2003-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/protocols/mip/src/dsmip_event_defs.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/24/13   ds      Fixing Klocwork errors.
01/19/03   jd      Added dropped bit to info when reporting MIP event == RRP
                   Cleaned up MIP event enum type (capitalized enum).
03/07/03   js      Initial definition. 
===========================================================================*/

/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"

#include "event.h"
#include "event_defs.h"



/*===========================================================================

                             EXPORTED DEFINITIONS

===========================================================================*/

/*===========================================================================
  NOTE: changes to these enums MUST be coordinated with the diag/tools team.
        as of the creation date mail had to be sent to asw.diag.request
===========================================================================*/

#define UNDEFINED 0


/*---------------------------------------------------------------------------
  TYPEDEF MIP_EVENT_ENUM_TYPE

  Type enumerating the possible mobile IP events to be generated. 
  Translated by the tool into strings. 
---------------------------------------------------------------------------*/
typedef enum
{
  EVENT_MIP_ASM_SENT = 0,
  EVENT_MIP_AAM_RCVD = 1,
  EVENT_MIP_RRQ_SENT = 2,
  EVENT_MIP_RRP_RCVD = 3,
  EVENT_MIP_INVALID  = 255
} mip_event_enum_type;


/*---------------------------------------------------------------------------
  TYPEDEF MIP_EVENT_INFO_ENUM

  Possible values for the info field when the mip event type is 'asm_sent' 
  or 'rrq_sent'.  Not translated by the tools. 
---------------------------------------------------------------------------*/
typedef enum 
{
  MIP_EVENT_INFO_NA        = 0,      /* info filed no applicable/available */  
  MIP_EVENT_INFO_SENT_OTA  = 1,      /* Sent over the air (for ASM, RRQ)   */
  MIP_EVENT_INFO_SENT_FAIL = 2       /* Unsuccessful sent                  */
} mip_event_info_enum;


/*---------------------------------------------------------------------------
  TYPEDEF MIP_EVENT_RRP_INFO_ENUM

  Structure of the info field when the mip event type is 'rrp_rcvd'
---------------------------------------------------------------------------*/
typedef PACKED struct PACKED_POST
{
  byte   rrp_code;
  PACKED struct PACKED_POST
  {
    char            unused : 7;         /* bit 8 - 14 */
    unsigned char   dropped : 1;        /* bit 15 (packed structures go lsb to msb) */
  } flags;
  char   reserved1;
  char   reserved2;
} mip_event_rrp_info_enum;


/*---------------------------------------------------------------------------
  TYPEDEF MIP_EVENT_INFO_TYPE

  Union of info contents in MIP events depending on 'mip_event' type
---------------------------------------------------------------------------*/
typedef union
{
  uint32                   zero;         /* use this to zero the structure */
  mip_event_info_enum      i_asm;
  mip_event_info_enum      i_aam;
  mip_event_info_enum      i_rrq;
  mip_event_rrp_info_enum  i_rrp;
} mip_event_info_type;


/*---------------------------------------------------------------------------
  TYPEDEF MIP_EVENT_PAYLOAD_TYPE

  MIP event payload type. 
---------------------------------------------------------------------------*/
typedef struct
{
  mip_event_enum_type  mip_event;           /* event                       */
  mip_event_info_type  info;                /* info field (event specific) */
} mip_event_payload_type;

#endif /* DSMIP_EVENT_DEFS_H */
