#ifndef DS_SOCK_EVENTDEFS_H
#define DS_SOCK_EVENTDEFS_H

/*===========================================================================
  @file ds_Sock_EventDefs.h

  This file provides definitions for different event groups and event infos.

  TODO: Write detailed explanation.

  Copyright (c) 2008 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/inc/ds_Sock_EventDefs.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-06-22 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Sock_ISocketExt.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
namespace ds
{
  namespace Sock
  {
    namespace Event
    {
      typedef enum
      {
        EVENT_GROUP_MIN             = 0,
        EVENT_GROUP_NETWORK         = EVENT_GROUP_MIN,
        EVENT_GROUP_PS_MEM          = 1,
        EVENT_GROUP_MAX
      } EventGroupType;

      typedef struct EventInfo
      {
        EventGroupType    eventGroup;
        int32             eventMask;
        int32             handle;
      } EventInfoType;

      typedef struct DoSAckEventInfo : public EventInfo
      {
        ds::Sock::DoSAckStatusType  dosAckStatus;
        uint32                      overflow;
      } DoSAckEventInfoType;

      typedef struct IPFilterEventInfo : public EventInfo
      {
        int32             clientID; 
      } IPFilterEventInfoType;

    } /* namespace Event */
  } /* namespace Sock */
} /* namespace ds */

#endif /* DS_SOCK_EVENTDEFS_H */
