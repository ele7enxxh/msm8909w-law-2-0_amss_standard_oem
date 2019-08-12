#ifndef __VOICECFG_ITEMS_I_H__
#define __VOICECFG_ITEMS_I_H__

/**
  @file voicecfg_items_i.h
  @brief This file contains private item IDs supported by voice configuration. 
*/

/*
  ============================================================================

   Copyright (C) 2014 QUALCOMM Technologies Incorporated.
   All Rights Reserved.
   QUALCOMM Proprietary and Confidential

  ============================================================================

                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/inc/private/voicecfg_items_i.h#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  05/30/14  sre   Initial revision

  ============================================================================
*/

/** @addtogroup voicecfg_items
@{ */

/*
  The following is an example of how to retrieve a voice configuration item.
  
  Code example
  @code
#include "voicecfg_api.h"
#include "voicecfg_items_i.h"
#include "apr_errcodes.h"

uint32_t voicecfg_ret_val;
 
voicecfg_cmd_get_bool_item_t item; 
item.id = VOICECFG_CFG_IS_ERROR_FATAL_FOR_PACKET_MISS_ENABLED;
item.ret_value = FALSE; 

voicecfg_ret_val =  voicecfg_call( VOICECFG_CMD_GET_BOOL_ITEM, &item, 
                                   sizeof( item ) ); 
                
if ( ! voicecfg_ret_val  && item.ret_value == TRUE )
{ 
  // Enable error fatal on packet miss.
} 
else 
{ 
  // Disable error fatal on packet miss.
}
  @endcode 
*/

/**
  This item indicates MVS event sequence is enabled.

  @comment
  The caller must assume that MVS event sequence is disabled by default
  unless the voicecfg item exists and the item is boolean value of true.

  When the voicecfg item is set to TRUE, MVS event sequence is ENABLED.

  Voice Driver clients can call the voice configuration API to enable this
  feature. When enabled, the command success is sent to the client after 
  the INIT and READY events. When disabled (default), the command success
  is sent to the client before INIT and READY events. The disabled case is
  the sequence expected by GL1 since legacy targets.
*/
#define VOICECFG_CFG_IS_MVS_EVENT_SEQUENCE_ENABLED ( 0x00012EF5 )

/**
  This item indicates CVD sessions are destroyed at the end of each call.

  @comment
  The caller must assume that the CVD sessions are destroyed at the end of
  each call unless the voicecfg item exists and the item is boolean value
  of true.

  When the voicecfg item is set to TRUE, CVD sessions are retained.

  Voice Driver clients can call the voice configuration API to disable 
  destroying CVD sessions.
*/
#define VOICECFG_CFG_IS_RETAIN_CVD_AFTER_RELEASE_OPTIMIZATION_DISABLED ( 0x0001305A )

/**
  This item enables the ERROR_FATAL debug code whenever the number of 
  packet misses cross certain threshold.
 
  @comment
  ERROR_FATAL during packet miss is disbaled by default unless the 
  voicecfg item exists and the item is boolean value of true.
   
  VSD and VCP layers can call the voice configuration API to enable the
  ERROR_FATAL debug code.
*/
#define VOICECFG_CFG_IS_ERROR_FATAL_FOR_PACKET_MISS_ENABLED ( 0x000130A1 )

/**
  This item is used to enable Voice Logging.
 
  @comment
  Voice logging is disbaled by default unless the voicecfg item exists
  and the item is boolean value of true.
   
  If enabled, F3 messages are pre-processed by voicelogging and sent to 
  DIAG in the background.
*/
#define VOICECFG_CFG_IS_VOICE_LOGGING_ENABLED ( 0x000130A4 )

#endif /* __VOICECFG_ITEMS_I_H__ */

