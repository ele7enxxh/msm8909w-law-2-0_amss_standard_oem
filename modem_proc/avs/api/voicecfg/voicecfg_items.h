#ifndef __VOICECFG_ITEMS_H__
#define __VOICECFG_ITEMS_H__

/**
  @file voicecfg_items.h
  @brief This file contains item IDs supported by voice configuration. 
*/

/*
  ============================================================================

   Copyright (C) 2013-2014 QUALCOMM Technologies Incorporated.
   All Rights Reserved.
   QUALCOMM Proprietary and Confidential

  ============================================================================
                             Edit History

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/api/voicecfg/voicecfg_items.h#1 $
  $Author: mplcsds1 $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------
  10/11/13  sre   Initial revision

  ============================================================================
*/

/** @addtogroup voicecfg_items
@{ */

/*
  The following is an example of how to retrieve a voice configuration item.
  
  Code example
  @code
#include "voicecfg_api.h"
#include "apr_errcodes.h"

uint32_t voicecfg_ret_val;
 
voicecfg_cmd_get_bool_item_t item; 
item.id = VOICECFG_CFG_IS_VOICE_SUBSYSTEM_DISABLED;
item.ret_value = FALSE; 

voicecfg_ret_val =  voicecfg_call( VOICECFG_CMD_GET_BOOL_ITEM, &item, 
                                   sizeof( item ) ); 
                
if ( ! voicecfg_ret_val  && item.ret_value == TRUE )
{ 
  // Disable the voice path.
} 
else 
{ 
  // Enable the voice path.
}
  @endcode 
*/

/**
  This item indicates eAMR mode change detection is enabled.

  @comment
  The caller must assume that eAMR mode change detection is disabled by default
  unless the voicecfg item exists and the item is set to boolean value TRUE.

  When the voicecfg item is set to TRUE, eAMR mode change detection is enabled.

  Voice Driver clients can call the voice configuration API to determine 
  if eAMR mode change detection is enabled or not. This feature allows the 
  OEM to control the sample rate of their external voice pre-post processing
  module based on eAMR mode change notification.
*/
#define VOICECFG_CFG_IS_EAMR_REPORT_MODE_CHANGE_ENABLED ( 0x00012E5B )

/**
  This item indicates the voice subsystem is disabled.

  @comment
  The caller must assume that the voice subsystem is enabled by default
  unless the voicecfg item exists and the item is set to boolean value TRUE.

  When the voicecfg item is set to TRUE, voice subsystem is disabled.

  The modem protocol stack can call the voice configuration API to determine 
  if the voice subsystem is disabled or not.
*/
#define VOICECFG_CFG_IS_VOICE_SUBSYSTEM_DISABLED ( 0x00012F06 )

/**
  This item indicates eAMR on 2G network is enabled.

  @comment
  The caller must assume that eAMR on 2G network is disabled by default unless the 
  voicecfg item exists and the item is set to boolean value TRUE.

  When the voicecfg item is set to TRUE, eAMR on 2G network is enabled.

  Voice Driver clients can call the voice configuration API to determine 
  if eAMR on 2G network is enabled or not.
*/
#define VOICECFG_CFG_IS_2G_EAMR_ENABLED ( 0x0001309C )

/**
  This item indicates eAMR on 3G network is enabled.

  @comment
  The caller must assume that eAMR on 3G network is disabled by default unless the
  voicecfg item exists and the item is set to boolean value TRUE.

  When the voicecfg item is set to TRUE, eAMR on 3G network is enabled.

  Voice Driver clients can call the voice configuration API to determine 
  if eAMR on 3G network is enabled or not.
*/
#define VOICECFG_CFG_IS_3G_EAMR_ENABLED ( 0x0001309D )

/**
  This item indicates eAMR on TDSCDMA network is enabled.

  @comment
  The caller must assume that eAMR on TDSCDMA network is disabled by default unless 
  the voicecfg item exists and the item is set to boolean value TRUE.

  When the voicecfg item is set to TRUE, eAMR on TDSCDMA network is enabled.

  Voice Driver clients can call the voice configuration API to determine 
  if eAMR on TDSCDMA network is enabled or not.
*/
#define VOICECFG_CFG_IS_TDS_EAMR_ENABLED ( 0x0001309E )

/**
  This item indicates eAMR on LTE network is enabled.

  @comment
  The caller must assume that eAMR on LTE network is disabled by default unless 
  the voicecfg item exists and the item is set to boolean value TRUE.

  When the voicecfg item is set to TRUE, eAMR on LTE network is enabled.

  Voice Driver clients can call the voice configuration API to determine 
  if eAMR on LTE network is enabled or not.
*/
#define VOICECFG_CFG_IS_VOLTE_EAMR_ENABLED ( 0x0001309F )

/**
  This item indicates eAMR on GSM network over SUB2 is enabled.

  @comment
  The caller must assume that eAMR on GSM network over SUB2 is disabled by default
  unless the voicecfg item exists and the item is set to boolean value TRUE.

  When the voicecfg item is set to TRUE, eAMR on GSM network over SUB2 is enabled.

  Voice Driver clients can call the voice configuration API to determine 
  if eAMR on GSM network over SUB2 is enabled or not.
*/
#define VOICECFG_CFG_IS_2G_SUB2_EAMR_ENABLED ( 0x000130B4 )

/** @} */ /* end_addtogroup voicecfg_items */

#endif  /* __VOICECFG_ITEMS_H__ */

