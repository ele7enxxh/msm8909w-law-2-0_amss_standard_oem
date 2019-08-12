/*===========================================================================

                         D S 7 0 7 _ C C M _ L O G
GENERAL DESCRIPTION
  This file contains the Memory logging for DS707. 
  Currently it runs in FC task context.
  Copyright (c) 2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

EXTERNALIZED FUNCTIONS
 DS707_CCM_MEMORY_LOG
   Logs the current memory usage info
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_rmsm.h_v   1.6   23 Jan 2003 16:34:28   randrew  $
  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/3gpp2/ds707/src/ds707_ccm_log.c#1 $ 
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $
  
when       who        what, where, why
--------   ---        ------------------------------------------------------- 
11/18/11   vpk     	   HDR SU API cleanup
09/19/11   msh         Fixed compiler warnings. 
07/22/11   vpk         First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "datamodem_variation.h"
#include "dsm.h"
#include "log_dmss.h"
#include "ds707_rmsm.h"
#include "ds707_wmk.h"
#include "log.h"
#include "data_msg.h"
#include "log_codes.h"
#ifdef FEATURE_HDR
  #include "hdrcom_api.h"
#endif
#ifndef LOG_DS_MEMORY_PACKETS 
#define LOG_DS_MEMORY_PACKETS 0x1556
#endif

typedef PACK(struct)
{
  uint32  hi_wmk; /* High level of watermark */
  uint32  lo_wmk; /* Low level of watermark */
}log_watermark_info_type;
   
typedef PACK(struct)
{
  uint16  free_dsm_items;  /* Number of free DSM items */
  uint16  max_dsm_items;  /* Max Number of DSM items */
} log_dsm_info_type;
  
/*---------------------------------------------------------------------------
                          LOG_DS_MEMORY_PACKETS_C
---------------------------------------------------------------------------*/
LOG_RECORD_DEFINE(LOG_DS_MEMORY_PACKETS_C)
    uint8 ver; /* Version number */
    uint8 out_cmd; /* Out command to FC
            0 – OFF
            1 - INVALID
            4 – UP
            6 – FREEZE
            7 – DWN
            */
  uint8 pattern; /* Current flow pattern 
            NO_FLOW_CONTROL = 0,    
            FLOW_CONTROL_ONE_TENTH = 1,  
            FLOW_CONTROL_TWO_TENTH = 2,    
            FLOW_CONTROL_THREE_TENTH = 3, 
            FLOW_CONTROL_FOUR_TENTH = 4,
            FLOW_CONTROL_FIVE_TENTH = 5,    
            FLOW_CONTROL_SIX_TENTH = 6,  
            FLOW_CONTROL_THREE_QUARTER = 7,
            FLOW_CONTROL_EIGHT_TENTH = 8,
            FLOW_CONTROL_NINE_TENTH = 9,  
            FLOW_CONTROL_ALWAYS = 10, 
            FLOW_CONTROL_INVALID = 0xFF,
              */
  boolean b_is_rm_wmk_valid; /* Rm Watermark is valid */
  log_watermark_info_type wmk_rm_fwd; /* Fwd Rm Watermark info */  
  log_watermark_info_type wmk_rm_rev; /* Rev Rm Watermark info */  
  log_watermark_info_type wmk_um_fwd; /* Fwd Um Watermark info */  
  log_watermark_info_type wmk_um_rev; /* Rev Um Watermark info */  
  log_dsm_info_type small_item_pool; /* Small item Dsm Pool info */
  log_dsm_info_type large_item_pool; /* Large item Dsm Pool info */
  log_dsm_info_type dup_item_pool; /* Dup item Dsm Pool info */
LOG_RECORD_END

/*===========================================================================

FUNCTION ds707_ccm_log_get_Um_watermark_info

DESCRIPTION
  Gets Um Watermark Info.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_ccm_log_get_Um_watermark_info
(
  log_watermark_info_type *fwd_wmk_info_ptr, 
  log_watermark_info_type *rev_wmk_info_ptr
)
{
  ds707_wmk_type um_wmk;
  ds707_wmk_get_wmk_levels(&um_wmk);

  fwd_wmk_info_ptr->lo_wmk = um_wmk.ds707_wmk_fwd_lo_wmk;
  fwd_wmk_info_ptr->hi_wmk = um_wmk.ds707_wmk_fwd_hi_wmk;

  rev_wmk_info_ptr->lo_wmk = um_wmk.ds707_wmk_pri_lo_wmk;
  rev_wmk_info_ptr->hi_wmk = um_wmk.ds707_wmk_pri_hi_wmk;
}


/*===========================================================================

FUNCTION ds707_ccm_log_get_Rm_watermark_info

DESCRIPTION
  Gets  Rm Watermark Info.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_ccm_log_get_Rm_watermark_info
(
  log_watermark_info_type *fwd_wmk_info_ptr, 
  log_watermark_info_type *rev_wmk_info_ptr
)
{
  ds707_rmsm_wmk_type rmsm_wmk;  
  ds707_rmsm_get_wmk_val(&rmsm_wmk);
  
  fwd_wmk_info_ptr->lo_wmk = rmsm_wmk.ds707_rmsm_fwd_lo_wmk;
  fwd_wmk_info_ptr->hi_wmk = rmsm_wmk.ds707_rmsm_fwd_hi_wmk;
    
  rev_wmk_info_ptr->lo_wmk = rmsm_wmk.ds707_rmsm_rev_lo_wmk;
  rev_wmk_info_ptr->hi_wmk = rmsm_wmk.ds707_rmsm_rev_hi_wmk;
  
}

/*===========================================================================

FUNCTION DS707_CCM_LOG_MEMORY

DESCRIPTION
  Logs current usage info of DSM items and Watermarks

  Takes outcommand as parameter and captures in the log.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void ds707_ccm_log_memory
( 
  uint8 out_cmd
)
{
  LOG_DS_MEMORY_PACKETS_C_type  *memory_fc_log_pkt_ptr = NULL;/* Log Packet ptr    */
  
  if (log_status( LOG_DS_MEMORY_PACKETS ))  
  {
    /* We'll try... */
    memory_fc_log_pkt_ptr = (LOG_DS_MEMORY_PACKETS_C_type *) 
    log_alloc( LOG_DS_MEMORY_PACKETS, sizeof( LOG_DS_MEMORY_PACKETS_C_type));

   
    if( memory_fc_log_pkt_ptr != NULL )
    {
      memory_fc_log_pkt_ptr->ver = 1; /* Version */
      memory_fc_log_pkt_ptr->out_cmd = out_cmd; /* Out Command */
      /* Get the current pattern from HDR */
#ifdef FEATURE_HDR
      memory_fc_log_pkt_ptr->pattern = hdrl1_fmac_get_fc_pattern(); 
#else
      memory_fc_log_pkt_ptr->pattern = 0;
#endif
      /* Check the current data call type */
      memory_fc_log_pkt_ptr->b_is_rm_wmk_valid = ds707_rmsm_is_packet_call(); 
      
      if ( 1 == memory_fc_log_pkt_ptr->b_is_rm_wmk_valid) {
        ds707_ccm_log_get_Rm_watermark_info(&memory_fc_log_pkt_ptr->wmk_rm_fwd, &memory_fc_log_pkt_ptr->wmk_rm_rev);
      }
      
      ds707_ccm_log_get_Um_watermark_info(&memory_fc_log_pkt_ptr->wmk_um_fwd, &memory_fc_log_pkt_ptr->wmk_um_rev);

      memory_fc_log_pkt_ptr->small_item_pool.free_dsm_items = DSM_POOL_FREE_CNT(DSM_DS_SMALL_ITEM_POOL);
      memory_fc_log_pkt_ptr->small_item_pool.max_dsm_items = DSM_DS_SMALL_ITEM_CNT;

      memory_fc_log_pkt_ptr->large_item_pool.free_dsm_items = DSM_POOL_FREE_CNT(DSM_DS_LARGE_ITEM_POOL);
      memory_fc_log_pkt_ptr->large_item_pool.max_dsm_items = DSM_DS_LARGE_ITEM_CNT;

      memory_fc_log_pkt_ptr->dup_item_pool.free_dsm_items = DSM_POOL_FREE_CNT(DSM_DUP_ITEM_POOL);
      memory_fc_log_pkt_ptr->dup_item_pool.max_dsm_items = DSM_DUP_ITEM_CNT;

      log_commit( (log_type*) memory_fc_log_pkt_ptr );
    }
    else {
      DATA_IS707_MSG0(MSG_LEGACY_HIGH, "Failed to allocate Memory FC");
    }
  }/* if */
} /* ds707_ccm_memory_log */
