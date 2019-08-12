/*!
  @file tf_dsm_main.cpp

  @brief Contains the functions for printing DSM item, pool and packet.

*/

/*===========================================================================

  Copyright (c) 2009 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.5.c12.3/memory/dsm/test/tf_dsm_main.cpp#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/31/09   ag      Replaced printf with TF_MSG.
07/30/09   ag      Initial Revision. Ported from linux tests.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#include <TestFramework.h>
extern "C" {
  #include "dsm.h"
  #if (DSM_MAJOR_VER_NO > 2)
    #include "dsmi.h"
  #endif
  #include "tf_dsm_main.h"
}


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                           FUNCTIONS DEFINITIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION TF_DSM_PRINT_PACKET()

DESCRIPTION
  Prints the content of a packet chain.

DEPENDENCIES
  None

PARAMETERS
  item - A pointer to an packet chain

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tf_dsm_print_packet( dsm_item_type * item)
{
  TF_MSG("PACKET START:item 0x%X",(int)item);
  while(item != NULL) {
    tf_dsm_print_item(item);
    item = item->pkt_ptr;
  }
  TF_MSG("PACKET END:item 0x%X",(int)item);
}

/*===========================================================================
FUNCTION TF_DSM_PRINT_ITEM()

DESCRIPTION
  Prints content of a DSM item.

DEPENDENCIES
  None

PARAMETERS
  item - A pointer to an item to be printed.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tf_dsm_print_item( dsm_item_type * item)
{
  int i;
  TF_MSG("print item 0x%X:POOL_ID 0x%X:SIZE %d:USED %d:REFERENCES %d", 
            (unsigned int)item,
            (unsigned int)item->pool_id,
  //          ( ( (dsm_pool_mgmt_table_type*) ( (item)->pool_id ) )->pool_item_size ),
            DSMI_SIZE(item),
            item->used,
            item->references);

#if defined(DSM_APP_FIELD) && defined(DSM_APP_PTR)
  TF_MSG("print item 0x%X:APP_FIELD 0x%X:APP_PTR 0x%X:KIND %d", 
            (unsigned int)item,
            (unsigned int)item->app_field,
            (unsigned int)item->app_ptr,
            item->kind);
#endif
  
  TF_MSG("print item 0x%X:DATA_PTR 0x%X (%s):PKT_PTR 0x%X:DUP_PTR 0x%X", 
            (unsigned int)item,
            (unsigned int)item->data_ptr,
           #if (DSM_MAJOR_VER_NO > 2)
            (!DSMI_DUP(item)?"LOCAL":"DUP"),
           #else
            "??",
           #endif
            (unsigned int)item->pkt_ptr,
            (unsigned int)item->dup_ptr);
  
 #if (DSM_MAJOR_VER_NO > 2)
  TF_MSG("print item 0x%X:DATA_PTR OFFSET %d:",
            (unsigned int)item,
            ( item->data_ptr - DSMI_ITEM_HEAD(item)));
 #endif

 #if (DSM_MAJOR_VER_NO > 2) && defined FEATURE_DSM_MEM_CHK && \
     DSM_TRACE_LEN > 0
  for(i=0;i<DSM_TRACE_LEN;i++)
  TF_MSG("TRACE FILE[%d] %s:TRACE LINE[%d] %d", 
            i,
            item->trace[i].file,
            i,
            (int)item->trace[i].line);
 #endif
  
  
 #if(DSM_MAJOR_VER_NO > 2)
  if( DSM_POOL_ITEM_SIZE(item->pool_id) != 0)
  {
    TF_MSG("TEST TRACE:print item 0x%X:PAYLOAD",(unsigned int)item);
    tf_print_buf(DSMI_ITEM_HEAD(item),DSM_POOL_ITEM_SIZE(item->pool_id));
  }
 #endif

 #if(DSM_MAJOR_VER_NO > 2)
  if( DSM_POOL_ITEM_SIZE(item->pool_id) == 0 && DSM_POOL_PAD_SIZE(item->pool_id) > 0 )
  {
    TF_MSG("TEST TRACE:print item 0x%X:META",(unsigned int)item);
    tf_print_buf((uint8*)( item ) - DSM_POOL_PAD_SIZE(item->pool_id),
                   DSM_POOL_PAD_SIZE(item->pool_id)
                  );
    TF_MSG(":\n");
  }
 #endif
}

/*===========================================================================
FUNCTION TF_PRINT_BUF()

DESCRIPTION
  Prints the content of a buffer.

DEPENDENCIES
  None

PARAMETERS
  buf - A pointer to a buffer to be printed.
  len - Length of the buffer.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tf_print_buf(uint8 * buf, int len)
{
  int i;
  for(i = 0;i < len;i++)
  { 
    TF_MSG(" %02X",(unsigned int)(buf[i]));
  }
}

/*===========================================================================
FUNCTION TF_DSM_PRINT_POOL()

DESCRIPTION
  Prints the content of an entire DSM pool.

DEPENDENCIES
  None

PARAMETERS
  pool - Pool id of the pool which is to be printed.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void tf_dsm_print_pool(dsm_mempool_id_enum_type pool)
{
  int i=0;
 #if(DSM_MAJOR_VER_NO > 2)
  dsm_pool_mgmt_table_type *table;
  table = (dsm_pool_mgmt_table_type*)pool;
 #else
  dsmi_pool_mgmt_table_type *table; /* Temp table pointer */
  table = &dsmi_pool_mgmt_table[pool];
 #endif
  
 #if(DSM_MAJOR_VER_NO > 2)
  TF_MSG("print pool 0x%X:DESC %s:FILE %s:LINE %d:NEXT 0x%X",
            (unsigned int) pool,
            ((table->desc == '\0') ? "NONE" : table->desc),
            table->file,
            (int)table->line,
            (unsigned int)table->next);

  TF_MSG("print pool 0x%X:COOKIE 0x%X (%s)",
            (unsigned int)pool,
            (unsigned int)table->cookie,
            (table->cookie == DSM_POOL_MGMT_TABLE_COOKIE?"GOOD":"BAD"));
 #endif
  
  TF_MSG("print pool 0x%X:ITEM_COUNT %d:FREE_COUNT %d:MIN_FREE_COUNT %d:POOL_ITEM_SIZE %d",
            (unsigned int)pool,
            (int)table->item_count,
            (int)table->free_count,
            (int)table->stats_min_free_count,
            (int)table->pool_item_size);

  TF_MSG("print pool 0x%X:PAD_SIZE %d:sizeof dsm_item_type %d",
            (unsigned int)pool,
            (int)table->pad_size,
            sizeof(dsm_item_type));

  TF_MSG("print pool 0x%X:NEXT_LEVEL %d",
            (unsigned int)pool,
            (int)table->next_level);

  for(i=0;i<DSM_MEM_LEVEL_MAX;i++)
  {
   #if(DSM_MAJOR_VER_NO > 2)
    TF_MSG("print pool 0x%X:MEM_EVENT %d:LEVEL %d:AVAIL_ITEM_COUNT %d:FREE_CB %X:NEW_CB %X",
              (unsigned int)pool,
              i,
              (int)table->mem_event_cb_list[i].level,
              (int)table->mem_event_cb_list[i].avail_item_count,
              (int)table->mem_event_cb_list[i].mem_free_event_cb,
              (int)table->mem_event_cb_list[i].mem_new_event_cb); 
   #else  
        TF_MSG("print pool 0x%X:MEM_EVENT %d:AVAIL_ITEM_COUNT %d:FREE_CB %X:NEW_CB %X",
              (unsigned int)pool,
              i,
              (int)table->mem_event_cb_list[i].avail_item_count,
              (int)table->mem_event_cb_list[i].mem_free_event_cb,
              (int)table->mem_event_cb_list[i].mem_new_event_cb); 
   #endif  

  }

#ifdef FEATURE_DSM_MAP_ARRAY
  {
    TF_MSG("print pool 0x%X:MAP_ARRAY 0x%X:MAP_ARRAY_ITEM_SIZE (%d):COUNT (%d)",
              (int)pool,(int)table->cookie_list,sizeof(uint32),table->item_count);
    for(i=0;i < table->item_count;i++){
      if( 0 == (i%8) ){
        if( i ) TF_MSG(":\n");
        TF_MSG("TEST TRACE:MSG print pool 0x%X:MAP_ARRAY %08X:",(int) pool,(int)((table->cookie_list)+i));
      }
      TF_MSG( " %08X",(int)(table->cookie_list[i]) );
    }
    TF_MSG(":\n");
  }
#endif /* FEATURE_DSM_MAP_ARRAY */
}

/*===========================================================================
FUNCTION TF_DSM_PRINT_WM()

DESCRIPTION
  Prints the contents of the watermark queue.

DEPENDENCIES
  None

PARAMETERS
  wm_ptr - watermark pointer.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void tf_dsm_print_wm ( dsm_watermark_type *wm_ptr )
{
#ifdef FEATURE_DSM_UINT64_RX_CNT
  TF_MSG("print wm 0x%X:LO %u:HI %u:DNE %u:CURRENT %u:HIGHEST %u:TOTAL RCD %u:U64 TOTAL RCD %u",
            (unsigned int)wm_ptr,
            (unsigned int)wm_ptr->lo_watermark,
            (unsigned int)wm_ptr->hi_watermark,
            (unsigned int)wm_ptr->dont_exceed_cnt,
            (unsigned int)wm_ptr->current_cnt,
            (unsigned int)wm_ptr->highest_cnt,
            (unsigned int)wm_ptr->total_rcvd_cnt,
            (unsigned long long int)wm_ptr->total_rcvd_cnt_u64 );
#else
  TF_MSG("print wm 0x%X:LO %u:HI %u:DNE %u:CURRENT %u:HIGHEST %u:TOTAL RCD %u",
            (unsigned int)wm_ptr,
            (unsigned int)wm_ptr->lo_watermark,
            (unsigned int)wm_ptr->hi_watermark,
            (unsigned int)wm_ptr->dont_exceed_cnt,
            (unsigned int)wm_ptr->current_cnt,
            (unsigned int)wm_ptr->highest_cnt,
            (unsigned int)wm_ptr->total_rcvd_cnt);
#endif

}

/*===========================================================================
FUNCTION TF_DSM_CMP_ITEM()

DESCRIPTION
  Compares two DSM items.

DEPENDENCIES
  None

PARAMETERS
  item1, item2 - Pointers to DSM items.
 
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int tf_dsm_cmp_item( dsm_item_type * item1, dsm_item_type * item2 )
{
  if( item1 == NULL && item2 == NULL)
    return 0;
  else if( item1 == NULL && item2 != NULL )
    return 1;
  else if (item1 != NULL && item2 == NULL )
    return -1;

  if( item1->used < item2->used )
    return -1;
  else if( item1->used > item2->used )
    return 1;
  
  else return memcmp(item1->data_ptr,item2->data_ptr,item1->used);
}

/*===========================================================================
FUNCTION TF_DSM_CMP_PACKET_NATIVE()

DESCRIPTION
  Compares two DSM packets.

DEPENDENCIES
  None

PARAMETERS
  item1, item2 - Pointer to two DSM packet chains.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
int tf_dsm_cmp_packet_naive( dsm_item_type * item1, dsm_item_type * item2 )
{

  int ret;

  while( (ret = tf_dsm_cmp_item( item1, item2 )) == 0 &&
          item1 != NULL
       )
    {
      item1 = item1->pkt_ptr;
      item2 = item2->pkt_ptr;
    }

  return ret;
         
}
