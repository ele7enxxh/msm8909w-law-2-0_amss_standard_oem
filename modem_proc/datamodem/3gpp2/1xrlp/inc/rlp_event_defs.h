#ifndef _RLP_EVENT_DEFS_H
#define _RLP_EVENT_DEFS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     R L P _ E V E N T _ D E F S. H

GENERAL DESCRIPTION
  This header file contains the definitions of the enums that are used to
  generate RLP events.



    Copyright (c) 2003 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                            EDIT HISTORY FOR FILE

  $Header:   L:/src/asw/MM_DATA/vcs/ps_ppp_events.h_v   1.1   27 Feb 2003 15:43

when        who    what, where, why
--------    ---    ----------------------------------------------------------
01/03/07    ac     reorder event_rlp_nak_abort_payload_type for alignment issue
03/04/03    js     initial revision.
===========================================================================*/
#include "comdef.h"


#include "event.h"
#include "event_defs.h"

/*===========================================================================
  NOTE: changes to these enums MUST be coordinated with the diag/tools team.
        as of the creation date mail had to be sent to asw.diag.request
===========================================================================*/

/*----------------------------------------------------------------------------
   TYPEDEF RLP_EVENT_PAYLOAD_TYPE

   Various types of RLP events. Translated by the tools. 
----------------------------------------------------------------------------*/
typedef enum
{
   event_rlp_undef = 0,
   event_rlp_reset,
   event_rlp_nak_abort,
   event_rlp_establishing,
   event_rlp_established
}event_rlp_enum_type;


/*----------------------------------------------------------------------------
   TYPEDEF EVENT_RLP_INFO_ENUM_TYPE

   Enum type for additional information in reporting events. Not translated
   by the tools. 
----------------------------------------------------------------------------*/
typedef enum
{
   event_rlp_info_enum_undef = 0,
   event_rlp_info_reset_unknown,         /* reset due to reasons unknown at 
                                            the time of the reporting       */
   event_rlp_info_reset_sync_to,         /* reset due to sync time out      */
   event_rlp_info_reset_mem,             /* reset due to critical mem event */
   event_rlp_info_reset_syncx_required,  /* reset due to sync exchange
                                            procedure is required           */
   event_rlp_info_reset_blob_in_call,    /* reset due to receiving a blob
                                            during a call                 */
   event_rlp_info_reset_bad_bsnak,
   event_rlp_info_reset_bad_bsvn
}event_rlp_info_enum_type;

/*----------------------------------------------------------------------------
   TYPEDEF RLP_EVENT_PAYLOAD_TYPE

   Payload in reporting RLP events. 
----------------------------------------------------------------------------*/
typedef struct
{
   event_rlp_enum_type  event_rlp_type;
   uint8                info;
}event_rlp_payload_type;

/*----------------------------------------------------------------------------
   TYPEDEF RLP_EVENT_PAYLOAD_TYPE

   Payload in reporting RLP events. 
----------------------------------------------------------------------------*/
typedef struct
{
  uint32              first_seq;  /* 1st missing Sequence number            */
  uint32              last_seq;   /* last missing Sequence number          */
  uint8               info;       /* RLP ID                                */
}event_rlp_nak_abort_payload_type;


typedef struct
{
  uint32                   leading_edge;
  uint32                   trailing_edge;
  uint8                    nak_type;
}event_rlp_multilinknak_payload_type;


typedef struct
{
  byte                rlp_id;
  byte                route;
  uint8               sched_grp_id;
}event_rlp_qn_instance_add_drop_payload_type;


typedef struct
{
  uint8               info;
  uint32              start_seq_num;
  uint32              window_len;
}event_rlp_nak_rev_link_payload_type;

#endif /* _RLP_EVENT_DEFS_H */
