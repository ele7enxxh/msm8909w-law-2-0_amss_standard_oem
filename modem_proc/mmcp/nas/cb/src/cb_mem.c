/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     CB MEMORY CONTROL MODULE (CB_MEM.C)

GENERAL DESCRIPTION

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001,2002,2003, 2004 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cb/src/cb_mem.c#1 $ $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/15/06   kvk     Renamed cb_free and cb_alloc to cb_mem_free and cb_mem_alloc 
                   to make it compatible with L4 builds
05/16/05   kvk     New memory pools defined for allocating the buffer to store
                   the status of the PDUs carrying the New Message information.
03/28/05   kvk     New memory pools for allocating buffers to store Tx and ReTx 
                   lists were defined.
10/01/04   vdr     Moved memory allocations from Heap into Data Segment
07/14/04   vdr     Added support for Skip request memory pool
05/07/04   vdr     Initial version

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "mmcp_variation.h"
#include "customer.h"

#ifdef FEATURE_GSM_CB

#include <string.h>
#include "comdef.h"
#include "cbmsg.h"
#include "msg.h"
#include "err.h"
#include "gs_v.h"
#include "cb_mem.h"
#include "cbwmsif.h"
#include "cb_utils.h"
#ifdef FEATURE_NAS_CBS_DSDS
#include "cb_state.h"
#endif

#ifdef FEATURE_MODEM_HEAP
#include "modem_mem.h"
#endif

#ifdef FEATURE_NAS_CBS_DSDS
extern cb_as_id_e_type cb_as_id;
#endif
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*
** CB task memory control descriptor
*/
typedef struct cb_alloc_descriptor_tag
{
  void *data_ptr ;
  dword data_alloc_length ;
  struct cb_alloc_descriptor_tag *next ;
  struct cb_alloc_descriptor_tag *previous ;
} cb_alloc_descriptor_type ;

/*
** Memory merge type
*/
typedef enum
{
  CB_NO_MERGE,
  CB_MERGE_INTO_CURRENT ,
  CB_MERGE_INTO_LIST
} cb_mem_merge_type ;

#define CB_SLOT_SIZE 4

/*
** Maximum number of CB messages which could be processed at the same time
** is equal to CB_MAXIMUM_NUMBER_OF_MESSAGES_ALLOWED
*/

#if (defined FEATURE_UMTS_BMC_DRX_SUPPORTED) || (defined FEATURE_GSM_CB_DRX_SUPPORTED)

/*
** Type, Macro and Data definitions are used when DRX mode is supported
*/

#define CB_MAX_SLOT_NUMBER 48

#define CB_MAX_SCHEDULING_SFN_NUMBER  (2*256)

#define CB_MAX_NEW_MSG   (2*256)

/*
** cb_first_tx_schedule_type memory pool definition
*/
#define FIRST_TX_SCHEDULE_POOL_SIZE (sizeof(cb_first_tx_schedule_type)*CB_MAX_SCHEDULING_SFN_NUMBER)

/*
** cb_retx_schedule_type memory pool definition
*/
#define RETX_SCHEDULE_POOL_SIZE (sizeof(cb_retx_schedule_type)*(CB_MAX_SCHEDULING_SFN_NUMBER/2))

/*
** cb_bmc_new_msg_bit_map_info_t memory pool definition
*/
#define NEW_MSG_BIT_MAP_POOL_SIZE (sizeof(cb_bmc_new_msg_bit_map_info_t)*CB_MAX_NEW_MSG)

#if defined(FEATURE_TRIPLE_SIM)
//TSTS start
static dword cb_current_first_tx_schedule_pool_size_sim[MAX_AS_IDS] = {FIRST_TX_SCHEDULE_POOL_SIZE,FIRST_TX_SCHEDULE_POOL_SIZE,FIRST_TX_SCHEDULE_POOL_SIZE} ;
#define   cb_current_first_tx_schedule_pool_size cb_current_first_tx_schedule_pool_size_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static byte cb_first_tx_schedule_pool_sim[MAX_AS_IDS][FIRST_TX_SCHEDULE_POOL_SIZE] ;

#else
static byte *cb_first_tx_schedule_pool_sim[MAX_AS_IDS] = {NULL,NULL,NULL};
#endif //#ifndef FEATURE_MODEM_HEAP
#define   cb_first_tx_schedule_pool cb_first_tx_schedule_pool_sim[cb_as_id]

static dword cb_first_tx_schedule_ctrl_scan_length_sim[MAX_AS_IDS] = {0,0,0} ;
#define   cb_first_tx_schedule_ctrl_scan_length cb_first_tx_schedule_ctrl_scan_length_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_first_tx_schedule_ctrl_pool_sim[MAX_AS_IDS][CB_MAX_SCHEDULING_SFN_NUMBER] ;

#else
static cb_alloc_descriptor_type *cb_first_tx_schedule_ctrl_pool_sim[MAX_AS_IDS] = {NULL,NULL,NULL};
#endif //#ifndef FEATURE_MODEM_HEAP
#define   cb_first_tx_schedule_ctrl_pool cb_first_tx_schedule_ctrl_pool_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_first_tx_schedule_free_sim[MAX_AS_IDS] = {NULL,NULL,NULL} ;
#define   cb_first_tx_schedule_free cb_first_tx_schedule_free_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_first_tx_schedule_alloc_sim[MAX_AS_IDS] = {NULL,NULL,NULL} ;
#define   cb_first_tx_schedule_alloc cb_first_tx_schedule_alloc_sim[cb_as_id]

static dword cb_retx_schedule_pool_size_sim[MAX_AS_IDS] = {RETX_SCHEDULE_POOL_SIZE,RETX_SCHEDULE_POOL_SIZE} ;
#define   cb_retx_schedule_pool_size cb_retx_schedule_pool_size_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP

static byte cb_retx_schedule_pool_sim[MAX_AS_IDS][RETX_SCHEDULE_POOL_SIZE] ;

#else
static byte *cb_retx_schedule_pool_sim[MAX_AS_IDS] = {NULL,NULL,NULL};

#endif //#ifndef FEATURE_MODEM_HEAP
#define   cb_retx_schedule_pool cb_retx_schedule_pool_sim[cb_as_id]

static dword cb_retx_schedule_ctrl_scan_length_sim[MAX_AS_IDS] = {0,0} ;
#define   cb_retx_schedule_ctrl_scan_length cb_retx_schedule_ctrl_scan_length_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_retx_schedule_ctrl_pool_sim[MAX_AS_IDS][CB_MAX_SCHEDULING_SFN_NUMBER] ;

#else
static cb_alloc_descriptor_type *cb_retx_schedule_ctrl_pool_sim[MAX_AS_IDS] = {NULL,NULL,NULL};

#endif //#ifndef FEATURE_MODEM_HEAP
#define   cb_retx_schedule_ctrl_pool cb_retx_schedule_ctrl_pool_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_retx_schedule_free_sim[MAX_AS_IDS] = {NULL,NULL,NULL} ;
#define   cb_retx_schedule_free cb_retx_schedule_free_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_retx_schedule_alloc_sim[MAX_AS_IDS] = {NULL,NULL,NULL} ;
#define   cb_retx_schedule_alloc cb_retx_schedule_alloc_sim[cb_as_id]



static dword cb_current_new_msg_pool_size_sim[MAX_AS_IDS] = {CB_MAX_NEW_MSG,CB_MAX_NEW_MSG} ;
#define   cb_current_new_msg_pool_size cb_current_new_msg_pool_size_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static byte cb_new_msg_pool_sim[MAX_AS_IDS][NEW_MSG_BIT_MAP_POOL_SIZE] ;

#else
static byte *cb_new_msg_pool_sim[MAX_AS_IDS] = {NULL,NULL,NULL};

#endif //#ifndef FEATURE_MODEM_HEAP
#define   cb_new_msg_pool cb_new_msg_pool_sim[cb_as_id]

static dword cb_new_msg_ctrl_scan_length_sim[MAX_AS_IDS] = {0,0,0} ;
#define   cb_new_msg_ctrl_scan_length cb_new_msg_ctrl_scan_length_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_new_msg_ctrl_pool[CB_MAX_NEW_MSG] ;

#else
static cb_alloc_descriptor_type *cb_new_msg_ctrl_pool_sim[MAX_AS_IDS] = {NULL,NULL,NULL};
#define   cb_new_msg_ctrl_pool cb_new_msg_ctrl_pool_sim[cb_as_id]

#endif //#ifndef FEATURE_MODEM_HEAP

static cb_alloc_descriptor_type *cb_new_msg_free_sim[MAX_AS_IDS]  = {NULL,NULL,NULL} ;
#define   cb_new_msg_free cb_new_msg_free_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_new_msg_alloc_sim[MAX_AS_IDS] = {NULL,NULL,NULL} ;
#define   cb_new_msg_alloc cb_new_msg_alloc_sim[cb_as_id]
//TSTS END HERE
#elif defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM) && (defined FEATURE_GSM_CB_DRX_SUPPORTED)

static dword cb_current_first_tx_schedule_pool_size_sim[MAX_AS_IDS] = {FIRST_TX_SCHEDULE_POOL_SIZE,FIRST_TX_SCHEDULE_POOL_SIZE} ;
#define   cb_current_first_tx_schedule_pool_size cb_current_first_tx_schedule_pool_size_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static byte cb_first_tx_schedule_pool_sim[MAX_AS_IDS][FIRST_TX_SCHEDULE_POOL_SIZE] ;

#else
static byte *cb_first_tx_schedule_pool_sim[MAX_AS_IDS] = {NULL,NULL};
#endif //#ifndef FEATURE_MODEM_HEAP
#define   cb_first_tx_schedule_pool cb_first_tx_schedule_pool_sim[cb_as_id]

static dword cb_first_tx_schedule_ctrl_scan_length_sim[MAX_AS_IDS] = {0,0} ;
#define   cb_first_tx_schedule_ctrl_scan_length cb_first_tx_schedule_ctrl_scan_length_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_first_tx_schedule_ctrl_pool_sim[MAX_AS_IDS][CB_MAX_SCHEDULING_SFN_NUMBER] ;

#else
static cb_alloc_descriptor_type *cb_first_tx_schedule_ctrl_pool_sim[MAX_AS_IDS] = {NULL,NULL};
#endif //#ifndef FEATURE_MODEM_HEAP
#define   cb_first_tx_schedule_ctrl_pool cb_first_tx_schedule_ctrl_pool_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_first_tx_schedule_free_sim[MAX_AS_IDS] = {NULL,NULL} ;
#define   cb_first_tx_schedule_free cb_first_tx_schedule_free_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_first_tx_schedule_alloc_sim[MAX_AS_IDS] = {NULL,NULL} ;
#define   cb_first_tx_schedule_alloc cb_first_tx_schedule_alloc_sim[cb_as_id]

static dword cb_retx_schedule_pool_size_sim[MAX_AS_IDS] = {RETX_SCHEDULE_POOL_SIZE,RETX_SCHEDULE_POOL_SIZE} ;
#define   cb_retx_schedule_pool_size cb_retx_schedule_pool_size_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP

static byte cb_retx_schedule_pool_sim[MAX_AS_IDS][RETX_SCHEDULE_POOL_SIZE] ;

#else
static byte *cb_retx_schedule_pool_sim[MAX_AS_IDS] = {NULL,NULL};

#endif //#ifndef FEATURE_MODEM_HEAP
#define   cb_retx_schedule_pool cb_retx_schedule_pool_sim[cb_as_id]

static dword cb_retx_schedule_ctrl_scan_length_sim[MAX_AS_IDS] = {0,0} ;
#define   cb_retx_schedule_ctrl_scan_length cb_retx_schedule_ctrl_scan_length_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_retx_schedule_ctrl_pool_sim[MAX_AS_IDS][CB_MAX_SCHEDULING_SFN_NUMBER] ;

#else
static cb_alloc_descriptor_type *cb_retx_schedule_ctrl_pool_sim[MAX_AS_IDS] = {NULL,NULL};

#endif //#ifndef FEATURE_MODEM_HEAP
#define   cb_retx_schedule_ctrl_pool cb_retx_schedule_ctrl_pool_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_retx_schedule_free_sim[MAX_AS_IDS] = {NULL,NULL} ;
#define   cb_retx_schedule_free cb_retx_schedule_free_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_retx_schedule_alloc_sim[MAX_AS_IDS] = {NULL,NULL} ;
#define   cb_retx_schedule_alloc cb_retx_schedule_alloc_sim[cb_as_id]



static dword cb_current_new_msg_pool_size_sim[MAX_AS_IDS] = {CB_MAX_NEW_MSG,CB_MAX_NEW_MSG} ;
#define   cb_current_new_msg_pool_size cb_current_new_msg_pool_size_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static byte cb_new_msg_pool_sim[MAX_AS_IDS][NEW_MSG_BIT_MAP_POOL_SIZE] ;

#else
static byte *cb_new_msg_pool_sim[MAX_AS_IDS] = {NULL,NULL};

#endif //#ifndef FEATURE_MODEM_HEAP
#define   cb_new_msg_pool cb_new_msg_pool_sim[cb_as_id]

static dword cb_new_msg_ctrl_scan_length_sim[MAX_AS_IDS] = {0,0} ;
#define   cb_new_msg_ctrl_scan_length cb_new_msg_ctrl_scan_length_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_new_msg_ctrl_pool[CB_MAX_NEW_MSG] ;

#else
static cb_alloc_descriptor_type *cb_new_msg_ctrl_pool_sim[MAX_AS_IDS] = {NULL,NULL};
#define   cb_new_msg_ctrl_pool cb_new_msg_ctrl_pool_sim[cb_as_id]

#endif //#ifndef FEATURE_MODEM_HEAP

static cb_alloc_descriptor_type *cb_new_msg_free_sim[MAX_AS_IDS]  = {NULL,NULL} ;
#define   cb_new_msg_free cb_new_msg_free_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_new_msg_alloc_sim[MAX_AS_IDS] = {NULL,NULL} ;
#define   cb_new_msg_alloc cb_new_msg_alloc_sim[cb_as_id]

#else //else FEATURE_NAS_CBS_DSDS


static dword cb_current_first_tx_schedule_pool_size = FIRST_TX_SCHEDULE_POOL_SIZE ;

#ifndef FEATURE_MODEM_HEAP 
static byte cb_first_tx_schedule_pool[FIRST_TX_SCHEDULE_POOL_SIZE] ;

#else
static byte *cb_first_tx_schedule_pool = NULL;
#endif //#ifndef FEATURE_MODEM_HEAP


static dword cb_first_tx_schedule_ctrl_scan_length = 0 ;

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_first_tx_schedule_ctrl_pool[CB_MAX_SCHEDULING_SFN_NUMBER] ;

#else
static cb_alloc_descriptor_type *cb_first_tx_schedule_ctrl_pool = NULL;
#endif //#ifndef FEATURE_MODEM_HEAP


static cb_alloc_descriptor_type *cb_first_tx_schedule_free = NULL ;

static cb_alloc_descriptor_type *cb_first_tx_schedule_alloc = NULL ;

static dword cb_retx_schedule_pool_size = RETX_SCHEDULE_POOL_SIZE ;

#ifndef FEATURE_MODEM_HEAP

static byte cb_retx_schedule_pool[RETX_SCHEDULE_POOL_SIZE] ;

#else
static byte *cb_retx_schedule_pool = NULL;

#endif //#ifndef FEATURE_MODEM_HEAP


static dword cb_retx_schedule_ctrl_scan_length = 0 ;

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_retx_schedule_ctrl_pool[CB_MAX_SCHEDULING_SFN_NUMBER] ;

#else
static cb_alloc_descriptor_type *cb_retx_schedule_ctrl_pool = NULL;

#endif //#ifndef FEATURE_MODEM_HEAP

static cb_alloc_descriptor_type *cb_retx_schedule_free = NULL ;

static cb_alloc_descriptor_type *cb_retx_schedule_alloc = NULL ;



static dword cb_current_new_msg_pool_size = CB_MAX_NEW_MSG ;

#ifndef FEATURE_MODEM_HEAP
static byte cb_new_msg_pool[NEW_MSG_BIT_MAP_POOL_SIZE] ;

#else
static byte *cb_new_msg_pool = NULL;

#endif //#ifndef FEATURE_MODEM_HEAP

static dword cb_new_msg_ctrl_scan_length = 0 ;

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_new_msg_ctrl_pool[CB_MAX_NEW_MSG] ;

#else
static cb_alloc_descriptor_type *cb_new_msg_ctrl_pool = NULL;

#endif //#ifndef FEATURE_MODEM_HEAP

static cb_alloc_descriptor_type *cb_new_msg_free  = NULL ;

static cb_alloc_descriptor_type *cb_new_msg_alloc = NULL ;

#endif //#if (defined FEATURE_NAS_CBS_DSDS) && (defined FEATURE_GSM_CB_DRX_SUPPORTED)

#endif /* DRX */

/*
** cb_user_page_info_type memory pool definition
*/
#define CB_MAXIMUM_NUMBER_OF_MESSAGES_ALLOWED_IN_CACHE 100
#define USER_PAGE_INFO_NUMBER_OF_RECORDS CB_MAXIMUM_NUMBER_OF_MESSAGES_ALLOWED_IN_CACHE

#define USER_PAGE_INFO_POOL_SIZE (USER_PAGE_INFO_NUMBER_OF_RECORDS * sizeof(cb_user_page_info_type))
/*
** cb_message_list_type memory pool definition
*/
#define MESSAGE_LIST_NUMBER_OF_RECORDS CB_MAXIMUM_NUMBER_OF_MESSAGES_ALLOWED_IN_CACHE

#define MESSAGE_LIST_POOL_SIZE (MESSAGE_LIST_NUMBER_OF_RECORDS * sizeof(cb_message_list_type))

/*
** cb_missing_page_type memory pool definition
*/
/*
**Maximum Number of PAGE records is obtianed as 244 from computing the 
**reminder of the space available for paging pool after increasing the Maximum 
**number of messages allowed in cache to 100 
*/

#define MISSING_PAGE_NUMBER_OF_RECORDS 224

#define MISSING_PAGE_POOL_SIZE (MISSING_PAGE_NUMBER_OF_RECORDS * sizeof(cb_missing_page_type))

/*
** Skip request memory pool definition
*/
#define SKIP_SEQUENCE_NUMBER_OF_RECORDS 10

#if (!defined FEATURE_UMTS_BMC_DRX_SUPPORTED) && (!defined FEATURE_GSM_CB_DRX_SUPPORTED)
#define SIZEOF_SKIP_SEQUENCE (CB_SLOT_SIZE * sizeof(dword))
#else
#define SIZEOF_SKIP_SEQUENCE (CB_SLOT_SIZE * CB_MAX_SLOT_NUMBER * sizeof(dword))
#endif

#define SKIP_SEQUENCE_POOL_SIZE (SKIP_SEQUENCE_NUMBER_OF_RECORDS * SIZEOF_SKIP_SEQUENCE)


#ifdef FEATURE_TRIPLE_SIM
//TSTS Start HERE
static dword cb_user_page_info_pool_size_sim[MAX_AS_IDS] = {0,0,0} ;
#define   cb_user_page_info_pool_size cb_user_page_info_pool_size_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static byte cb_user_page_info_pool_sim[MAX_AS_IDS][USER_PAGE_INFO_POOL_SIZE] ;
#else
static byte *cb_user_page_info_pool_sim[MAX_AS_IDS] = {NULL,NULL,NULL};
#endif
#define   cb_user_page_info_pool cb_user_page_info_pool_sim[cb_as_id]

static dword cb_user_page_info_ctrl_scan_length_sim[MAX_AS_IDS] = {0,0,0} ;
#define   cb_user_page_info_ctrl_scan_length cb_user_page_info_ctrl_scan_length_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_user_page_info_ctrl_pool_sim[MAX_AS_IDS][USER_PAGE_INFO_NUMBER_OF_RECORDS] ;
#else
static cb_alloc_descriptor_type *cb_user_page_info_ctrl_pool_sim[MAX_AS_IDS] = {NULL,NULL,NULL};
#endif
#define   cb_user_page_info_ctrl_pool cb_user_page_info_ctrl_pool_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_user_page_info_free_sim[MAX_AS_IDS] = {NULL,NULL,NULL} ;
#define   cb_user_page_info_free cb_user_page_info_free_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_user_page_info_alloc_sim[MAX_AS_IDS] = {NULL,NULL,NULL} ;
#define   cb_user_page_info_alloc cb_user_page_info_alloc_sim[cb_as_id]

static dword cb_message_list_pool_size_sim[MAX_AS_IDS] = {0,0,0} ;
#define   cb_message_list_pool_size cb_message_list_pool_size_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static byte cb_message_list_pool_sim[MAX_AS_IDS][MESSAGE_LIST_POOL_SIZE] ;

#else
static byte *cb_message_list_pool_sim[MAX_AS_IDS] = {NULL,NULL,NULL};
#endif
#define   cb_message_list_pool cb_message_list_pool_sim[cb_as_id]

static dword cb_message_list_ctrl_scan_length_sim[MAX_AS_IDS] = {0,0,0} ;
#define   cb_message_list_ctrl_scan_length cb_message_list_ctrl_scan_length_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_message_list_ctrl_pool_sim[MAX_AS_IDS][MESSAGE_LIST_NUMBER_OF_RECORDS] ;
#else
static cb_alloc_descriptor_type *cb_message_list_ctrl_pool_sim[MAX_AS_IDS] = {NULL,NULL,NULL};
#endif
#define   cb_message_list_ctrl_pool cb_message_list_ctrl_pool_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_message_list_free_sim[MAX_AS_IDS] = {NULL,NULL,NULL} ;
#define   cb_message_list_free cb_message_list_free_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_message_list_alloc_sim[MAX_AS_IDS] = {NULL,NULL,NULL} ;
#define   cb_message_list_alloc cb_message_list_alloc_sim[cb_as_id]

static dword cb_missing_page_pool_size_sim[MAX_AS_IDS] = {0,0,0} ;
#define   cb_missing_page_pool_size cb_missing_page_pool_size_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static byte cb_missing_page_pool_sim[MAX_AS_IDS][MISSING_PAGE_POOL_SIZE] ;
#else
static byte *cb_missing_page_pool_sim[MAX_AS_IDS] = {NULL,NULL,NULL};
#endif
#define   cb_missing_page_pool cb_missing_page_pool_sim[cb_as_id]

static dword cb_missing_page_ctrl_scan_length_sim[MAX_AS_IDS] = {0,0,0} ;
#define   cb_missing_page_ctrl_scan_length cb_missing_page_ctrl_scan_length_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_missing_page_ctrl_pool_sim[MAX_AS_IDS][MISSING_PAGE_NUMBER_OF_RECORDS] ;
#else
static cb_alloc_descriptor_type *cb_missing_page_ctrl_pool_sim[MAX_AS_IDS] = {NULL,NULL,NULL};
#endif
#define   cb_missing_page_ctrl_pool cb_missing_page_ctrl_pool_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_missing_page_free_sim[MAX_AS_IDS] = {NULL,NULL,NULL} ;
#define   cb_missing_page_free cb_missing_page_free_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_missing_page_alloc_sim[MAX_AS_IDS] = {NULL,NULL,NULL} ;
#define   cb_missing_page_alloc cb_missing_page_alloc_sim[cb_as_id]


static dword cb_skip_sequence_pool_size_sim[MAX_AS_IDS] = {0,0,0} ;
#define   cb_skip_sequence_pool_size cb_skip_sequence_pool_size_sim[cb_as_id]


#ifndef FEATURE_MODEM_HEAP
static byte cb_skip_sequence_pool_sim[MAX_AS_IDS][SKIP_SEQUENCE_POOL_SIZE] ;
#else
static byte *cb_skip_sequence_pool_sim[MAX_AS_IDS] = {NULL,NULL,NULL};
#endif
#define   cb_skip_sequence_pool cb_skip_sequence_pool_sim[cb_as_id]

static dword cb_skip_sequence_ctrl_scan_length_sim[MAX_AS_IDS] = {0,0,0} ;
#define   cb_skip_sequence_ctrl_scan_length cb_skip_sequence_ctrl_scan_length_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_skip_sequence_ctrl_pool_sim[MAX_AS_IDS][SKIP_SEQUENCE_NUMBER_OF_RECORDS] ;
#else
static cb_alloc_descriptor_type *cb_skip_sequence_ctrl_pool_sim[MAX_AS_IDS] = {NULL,NULL,NULL};
#endif
#define   cb_skip_sequence_ctrl_pool cb_skip_sequence_ctrl_pool_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_skip_sequence_free_sim[MAX_AS_IDS] = {NULL,NULL,NULL} ;
#define   cb_skip_sequence_free cb_skip_sequence_free_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_skip_sequence_alloc_sim[MAX_AS_IDS] = {NULL,NULL,NULL} ;
#define   cb_skip_sequence_alloc cb_skip_sequence_alloc_sim[cb_as_id]


//TSTS END HERE
#elif defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)


static dword cb_user_page_info_pool_size_sim[MAX_AS_IDS] = {0,0} ;
#define   cb_user_page_info_pool_size cb_user_page_info_pool_size_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static byte cb_user_page_info_pool_sim[MAX_AS_IDS][USER_PAGE_INFO_POOL_SIZE] ;
#else
static byte *cb_user_page_info_pool_sim[MAX_AS_IDS] = {NULL,NULL};
#endif
#define   cb_user_page_info_pool cb_user_page_info_pool_sim[cb_as_id]

static dword cb_user_page_info_ctrl_scan_length_sim[MAX_AS_IDS] = {0,0} ;
#define   cb_user_page_info_ctrl_scan_length cb_user_page_info_ctrl_scan_length_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_user_page_info_ctrl_pool_sim[MAX_AS_IDS][USER_PAGE_INFO_NUMBER_OF_RECORDS] ;
#else
static cb_alloc_descriptor_type *cb_user_page_info_ctrl_pool_sim[MAX_AS_IDS] = {NULL,NULL};
#endif
#define   cb_user_page_info_ctrl_pool cb_user_page_info_ctrl_pool_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_user_page_info_free_sim[MAX_AS_IDS] = {NULL,NULL} ;
#define   cb_user_page_info_free cb_user_page_info_free_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_user_page_info_alloc_sim[MAX_AS_IDS] = {NULL,NULL} ;
#define   cb_user_page_info_alloc cb_user_page_info_alloc_sim[cb_as_id]

static dword cb_message_list_pool_size_sim[MAX_AS_IDS] = {0,0} ;
#define   cb_message_list_pool_size cb_message_list_pool_size_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static byte cb_message_list_pool_sim[MAX_AS_IDS][MESSAGE_LIST_POOL_SIZE] ;

#else
static byte *cb_message_list_pool_sim[MAX_AS_IDS] = {NULL,NULL};
#endif
#define   cb_message_list_pool cb_message_list_pool_sim[cb_as_id]

static dword cb_message_list_ctrl_scan_length_sim[MAX_AS_IDS] = {0,0} ;
#define   cb_message_list_ctrl_scan_length cb_message_list_ctrl_scan_length_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_message_list_ctrl_pool_sim[MAX_AS_IDS][MESSAGE_LIST_NUMBER_OF_RECORDS] ;
#else
static cb_alloc_descriptor_type *cb_message_list_ctrl_pool_sim[MAX_AS_IDS] = {NULL,NULL};
#endif
#define   cb_message_list_ctrl_pool cb_message_list_ctrl_pool_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_message_list_free_sim[MAX_AS_IDS] = {NULL,NULL} ;
#define   cb_message_list_free cb_message_list_free_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_message_list_alloc_sim[MAX_AS_IDS] = {NULL,NULL} ;
#define   cb_message_list_alloc cb_message_list_alloc_sim[cb_as_id]

static dword cb_missing_page_pool_size_sim[MAX_AS_IDS] = {0,0} ;
#define   cb_missing_page_pool_size cb_missing_page_pool_size_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static byte cb_missing_page_pool_sim[MAX_AS_IDS][MISSING_PAGE_POOL_SIZE] ;
#else
static byte *cb_missing_page_pool_sim[MAX_AS_IDS] = {NULL,NULL};
#endif
#define   cb_missing_page_pool cb_missing_page_pool_sim[cb_as_id]

static dword cb_missing_page_ctrl_scan_length_sim[MAX_AS_IDS] = {0,0} ;
#define   cb_missing_page_ctrl_scan_length cb_missing_page_ctrl_scan_length_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_missing_page_ctrl_pool_sim[MAX_AS_IDS][MISSING_PAGE_NUMBER_OF_RECORDS] ;
#else
static cb_alloc_descriptor_type *cb_missing_page_ctrl_pool_sim[MAX_AS_IDS] = {NULL,NULL};
#endif
#define   cb_missing_page_ctrl_pool cb_missing_page_ctrl_pool_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_missing_page_free_sim[MAX_AS_IDS] = {NULL,NULL} ;
#define   cb_missing_page_free cb_missing_page_free_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_missing_page_alloc_sim[MAX_AS_IDS] = {NULL,NULL} ;
#define   cb_missing_page_alloc cb_missing_page_alloc_sim[cb_as_id]


static dword cb_skip_sequence_pool_size_sim[MAX_AS_IDS] = {0,0} ;
#define   cb_skip_sequence_pool_size cb_skip_sequence_pool_size_sim[cb_as_id]


#ifndef FEATURE_MODEM_HEAP
static byte cb_skip_sequence_pool_sim[MAX_AS_IDS][SKIP_SEQUENCE_POOL_SIZE] ;
#else
static byte *cb_skip_sequence_pool_sim[MAX_AS_IDS] = {NULL,NULL};
#endif
#define   cb_skip_sequence_pool cb_skip_sequence_pool_sim[cb_as_id]

static dword cb_skip_sequence_ctrl_scan_length_sim[MAX_AS_IDS] = {0,0} ;
#define   cb_skip_sequence_ctrl_scan_length cb_skip_sequence_ctrl_scan_length_sim[cb_as_id]

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_skip_sequence_ctrl_pool_sim[MAX_AS_IDS][SKIP_SEQUENCE_NUMBER_OF_RECORDS] ;
#else
static cb_alloc_descriptor_type *cb_skip_sequence_ctrl_pool_sim[MAX_AS_IDS] = {NULL,NULL};
#endif
#define   cb_skip_sequence_ctrl_pool cb_skip_sequence_ctrl_pool_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_skip_sequence_free_sim[MAX_AS_IDS] = {NULL,NULL} ;
#define   cb_skip_sequence_free cb_skip_sequence_free_sim[cb_as_id]

static cb_alloc_descriptor_type *cb_skip_sequence_alloc_sim[MAX_AS_IDS] = {NULL,NULL} ;
#define   cb_skip_sequence_alloc cb_skip_sequence_alloc_sim[cb_as_id]

#else //#ifndef FEATURE_NAS_CBS_DSDS

static dword cb_user_page_info_pool_size = 0 ;
#ifndef FEATURE_MODEM_HEAP
static byte cb_user_page_info_pool[USER_PAGE_INFO_POOL_SIZE] ;

#else
static byte *cb_user_page_info_pool = NULL;
#endif

static dword cb_user_page_info_ctrl_scan_length = 0 ;

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_user_page_info_ctrl_pool[USER_PAGE_INFO_NUMBER_OF_RECORDS] ;

#else
static cb_alloc_descriptor_type *cb_user_page_info_ctrl_pool = NULL;
#endif

static cb_alloc_descriptor_type *cb_user_page_info_free = NULL ;

static cb_alloc_descriptor_type *cb_user_page_info_alloc = NULL ;

static dword cb_message_list_pool_size = 0 ;

#ifndef FEATURE_MODEM_HEAP
static byte cb_message_list_pool[MESSAGE_LIST_POOL_SIZE] ;

#else
static byte *cb_message_list_pool = NULL;
#endif

static dword cb_message_list_ctrl_scan_length = 0 ;

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_message_list_ctrl_pool[MESSAGE_LIST_NUMBER_OF_RECORDS] ;

#else
static cb_alloc_descriptor_type *cb_message_list_ctrl_pool = NULL;
#endif

static cb_alloc_descriptor_type *cb_message_list_free = NULL ;

static cb_alloc_descriptor_type *cb_message_list_alloc = NULL ;

static dword cb_missing_page_pool_size = 0 ;

#ifndef FEATURE_MODEM_HEAP
static byte cb_missing_page_pool[MISSING_PAGE_POOL_SIZE] ;

#else
static byte *cb_missing_page_pool = NULL;
#endif

static dword cb_missing_page_ctrl_scan_length = 0 ;

#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_missing_page_ctrl_pool[MISSING_PAGE_NUMBER_OF_RECORDS] ;

#else
static cb_alloc_descriptor_type *cb_missing_page_ctrl_pool = NULL;
#endif

static cb_alloc_descriptor_type *cb_missing_page_free = NULL ;

static cb_alloc_descriptor_type *cb_missing_page_alloc = NULL ;

static dword cb_skip_sequence_pool_size = 0 ;

#ifndef FEATURE_MODEM_HEAP
static byte cb_skip_sequence_pool[SKIP_SEQUENCE_POOL_SIZE] ;

#else
static byte *cb_skip_sequence_pool = NULL;
#endif
static dword cb_skip_sequence_ctrl_scan_length = 0 ;
#ifndef FEATURE_MODEM_HEAP
static cb_alloc_descriptor_type cb_skip_sequence_ctrl_pool[SKIP_SEQUENCE_NUMBER_OF_RECORDS] ;

#else
static cb_alloc_descriptor_type *cb_skip_sequence_ctrl_pool = NULL;
#endif
static cb_alloc_descriptor_type *cb_skip_sequence_free = NULL ;

static cb_alloc_descriptor_type *cb_skip_sequence_alloc = NULL ;

#endif//#ifdef FEATURE_NAS_CBS_DSDS

extern void cb_send_wms_error_indication( cb_error_type_T error_value );

/*===========================================================================

FUNCTION    CB_GET_MEM_DESCRIPTOR

DESCRIPTION
  This function allocates new memory descriptor from the specific descriptor
  pool

DEPENDENCIES
  None

RETURN VALUE
  Pointer to new descriptor

SIDE EFFECTS
  None

===========================================================================*/
cb_alloc_descriptor_type *cb_get_mem_descriptor
(
  dword *scanable_pool_size,
  dword max_pool_size,
  cb_alloc_descriptor_type *descriptor_pool
)
{
  dword index ;

  for (index=0; index<*scanable_pool_size; index++)
  {
    if (descriptor_pool[index].data_ptr == NULL)
    {
      return &descriptor_pool[index] ;
    }
  }

  if (index<max_pool_size)
  {
    (*scanable_pool_size)++ ;

    return &descriptor_pool[index] ;
  }
  else
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: No memory descriptor left") ;
  }

  return NULL ;/* No descriptors left */

} /* end of cb_get_mem_descriptor() */

/*===========================================================================

FUNCTION    CB_RET_MEM_DESCRIPTOR

DESCRIPTION
  This function returns memory descriptor to the specific descriptor pool

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_ret_mem_descriptor
(
  cb_alloc_descriptor_type *mem_descriptor,
  dword *scanable_pool_size,
  cb_alloc_descriptor_type *descriptor_pool
)
{
  dword index ;
  boolean descriptor_returned = FALSE ;

  for (index=0; index<*scanable_pool_size; index++)
  {
    if (&descriptor_pool[index] == mem_descriptor)
    {
      memset((void *)mem_descriptor,0x00,sizeof(cb_alloc_descriptor_type)) ;

      descriptor_returned = TRUE ;

      break ;
    }
  }

  if (!descriptor_returned)
  {
    MSG_FATAL_DS( cb_as_id,"CB: Access violation",0,0,0) ;
  }

  if ((index + 1) == *scanable_pool_size)
  {
    (*scanable_pool_size)-- ;

    index= *scanable_pool_size ;

    do
    {
      if (index != 0)
      {
        index-- ;

        if (descriptor_pool[index].data_ptr == NULL)
        {
          (*scanable_pool_size)-- ;
        }
        else
        {
          break ;
        }
      }
    } while (index != 0) ;
  }
} /* end of cb_ret_mem_descriptor() */

/*===========================================================================

FUNCTION    CB_MEM_POOL_INIT

DESCRIPTION
  This function initializes single memory pool

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
boolean cb_mem_pool_init
(
  void *mem_pool,
  dword actual_mem_pool_alloc_size,
  dword *mem_pool_size,
  dword *ctrl_scanable_length,
  dword ctrl_max_length,
  cb_alloc_descriptor_type *ctrl_pool,
  cb_alloc_descriptor_type **free_pool_descriptor,
  cb_alloc_descriptor_type **alloc_pool_descriptor
)
{
  *mem_pool_size = actual_mem_pool_alloc_size ;
  memset(mem_pool,0xAA,*mem_pool_size) ;

  if ((*free_pool_descriptor = cb_get_mem_descriptor(
                      ctrl_scanable_length,ctrl_max_length,ctrl_pool)) == NULL)
  {
    MSG_ERROR_DS_0(cb_as_id,"CB init: Can not allocate mem descriptor") ;

    return FALSE ;
  }

  (*free_pool_descriptor)->data_ptr = mem_pool ;
  (*free_pool_descriptor)->data_alloc_length = *mem_pool_size ;
  (*free_pool_descriptor)->next = NULL ;
  (*free_pool_descriptor)->previous = NULL ;

  (*alloc_pool_descriptor) = NULL ;

  return TRUE ;
} /* end of cb_mem_pool_init() */

/*===========================================================================

FUNCTION    CB_MEM_INIT

DESCRIPTION
  This function initializes CB task's memory pools

DEPENDENCIES
  None

RETURN VALUE
  TRUE if memory pools have been allocated successfully and FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
#ifndef FEATURE_MODEM_HEAP
boolean cb_mem_init
(
  void
)
{
  dword total_memory_pool_size = 0 ;

#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM) && (defined FEATURE_GSM_CB_DRX_SUPPORTED)
  for ( cb_as_id=CB_AS_ID_1; cb_as_id < MAX_AS_IDS; cb_as_id++ )
#endif
  {
#if (defined FEATURE_UMTS_BMC_DRX_SUPPORTED) || (defined FEATURE_GSM_CB_DRX_SUPPORTED)


    /* 
    ** Create cb_first_tx_schedule_pool memory pool
    */

    if (cb_mem_pool_init(cb_first_tx_schedule_pool,
                         FIRST_TX_SCHEDULE_POOL_SIZE,
                         &cb_current_first_tx_schedule_pool_size,
                         &cb_first_tx_schedule_ctrl_scan_length,
                         CB_MAX_SCHEDULING_SFN_NUMBER,
                         cb_first_tx_schedule_ctrl_pool,
                         &cb_first_tx_schedule_free,
                         &cb_first_tx_schedule_alloc) == FALSE)
    {
      return FALSE ;
    }

    total_memory_pool_size += 
             /* Memory pool size */
             FIRST_TX_SCHEDULE_POOL_SIZE + 

             /* 
             ** Control list containing "cb_alloc_descriptor_type" elements
             ** of CB_MAX_SCHEDULING_SFN_NUMBER long
             */
             CB_MAX_SCHEDULING_SFN_NUMBER * sizeof(cb_alloc_descriptor_type) +

             /* Four control dwords */
             4 * sizeof(dword);

    /* 
    ** Create cb_retx_schedule_pool memory pool
    */

    if (cb_mem_pool_init(cb_retx_schedule_pool,
                         RETX_SCHEDULE_POOL_SIZE,
                         &cb_retx_schedule_pool_size,
                         &cb_retx_schedule_ctrl_scan_length,
                         CB_MAX_SCHEDULING_SFN_NUMBER,
                         cb_retx_schedule_ctrl_pool,
                         &cb_retx_schedule_free,
                         &cb_retx_schedule_alloc) == FALSE)
    {
      return FALSE ;
    }

    total_memory_pool_size += 
             /* Memory pool size */
             RETX_SCHEDULE_POOL_SIZE + 

             /* 
             ** Control list containing "cb_alloc_descriptor_type" elements
             ** of CB_MAX_SCHEDULING_SFN_NUMBER long
             */
             CB_MAX_SCHEDULING_SFN_NUMBER * sizeof(cb_alloc_descriptor_type) +

             /* Four control dwords */
             4 * sizeof(dword);

    if (cb_mem_pool_init(cb_new_msg_pool,
                         NEW_MSG_BIT_MAP_POOL_SIZE,
                         &cb_current_new_msg_pool_size,
                         &cb_new_msg_ctrl_scan_length,
                         CB_MAX_NEW_MSG,
                         cb_new_msg_ctrl_pool,
                         &cb_new_msg_free,
                         &cb_new_msg_alloc) == FALSE)
    {
      return FALSE ;
    }

    total_memory_pool_size += 
             /* Memory pool size */
             NEW_MSG_BIT_MAP_POOL_SIZE + 

             /* 
             ** Control list containing "cb_bmc_new_msg_bit_map_info_t" elements
             ** of CB_MAX_NEW_MSG long
             */
             CB_MAX_NEW_MSG * sizeof(cb_bmc_new_msg_bit_map_info_t) +

             /* Four control dwords */
             4 * sizeof(dword);

#endif /* DRX */
  }
  /* 
  ** Create cb_user_page_info_type memory pool
  */

#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)
  for ( cb_as_id=CB_AS_ID_1; cb_as_id < MAX_AS_IDS; cb_as_id++ )
#endif
  {
    if (cb_mem_pool_init(cb_user_page_info_pool,
                         USER_PAGE_INFO_POOL_SIZE,
                         &cb_user_page_info_pool_size,
                         &cb_user_page_info_ctrl_scan_length,
                         USER_PAGE_INFO_NUMBER_OF_RECORDS,
                         cb_user_page_info_ctrl_pool,
                         &cb_user_page_info_free,
                         &cb_user_page_info_alloc) == FALSE)
    {
      return FALSE ;
    }

    total_memory_pool_size += 
             /* Memory pool size */
             USER_PAGE_INFO_POOL_SIZE + 

             /* 
             ** Control list containing "cb_alloc_descriptor_type" elements
             ** of USER_PAGE_INFO_NUMBER_OF_RECORDS long
             */
             USER_PAGE_INFO_NUMBER_OF_RECORDS * sizeof(cb_alloc_descriptor_type) +

             /* Six control dwords */
             6 * sizeof(dword);

    /* 
    ** Create cb_message_list_type memory pool
    */
   if (cb_mem_pool_init(cb_message_list_pool,
                         MESSAGE_LIST_POOL_SIZE,
                         &cb_message_list_pool_size,
                         &cb_message_list_ctrl_scan_length,
                         MESSAGE_LIST_NUMBER_OF_RECORDS,
                         cb_message_list_ctrl_pool,
                         &cb_message_list_free,
                         &cb_message_list_alloc) == FALSE)
    {
      return FALSE ;
    }

    total_memory_pool_size += 
             /* Memory pool size */
             MESSAGE_LIST_POOL_SIZE + 

             /* 
             ** Control list containing "cb_alloc_descriptor_type" elements
             ** of MESSAGE_LIST_NUMBER_OF_RECORDS long
             */
             MESSAGE_LIST_NUMBER_OF_RECORDS * sizeof(cb_alloc_descriptor_type) +

             /* Six control dwords */
             6 * sizeof(dword);


    /* 
    ** Create cb_missing_page_type memory pool
    */
   if (cb_mem_pool_init(cb_missing_page_pool,
                         MISSING_PAGE_POOL_SIZE,
                         &cb_missing_page_pool_size,
                         &cb_missing_page_ctrl_scan_length,
                         MISSING_PAGE_NUMBER_OF_RECORDS,
                         cb_missing_page_ctrl_pool,
                         &cb_missing_page_free,
                         &cb_missing_page_alloc) == FALSE)
    {
      return FALSE ;
    }

    total_memory_pool_size += 
             /* Memory pool size */
             MISSING_PAGE_POOL_SIZE + 

             /* 
             ** Control list containing "cb_alloc_descriptor_type" elements
             ** of MISSING_PAGE_NUMBER_OF_RECORDS long
             */
             MISSING_PAGE_NUMBER_OF_RECORDS * sizeof(cb_alloc_descriptor_type) +

             /* Six control dwords */
             6 * sizeof(dword);
  
    /* 
    ** Create cb_missing_page_type memory pool
    */
    if (cb_mem_pool_init(cb_skip_sequence_pool,
                         SKIP_SEQUENCE_POOL_SIZE,
                         &cb_skip_sequence_pool_size,
                         &cb_skip_sequence_ctrl_scan_length,
                         SKIP_SEQUENCE_NUMBER_OF_RECORDS,
                         cb_skip_sequence_ctrl_pool,
                         &cb_skip_sequence_free,
                         &cb_skip_sequence_alloc) == FALSE)
    {
      return FALSE ;
    }

    total_memory_pool_size += 
             /* Memory pool size */
             SKIP_SEQUENCE_POOL_SIZE + 

             /* 
             ** Control list containing "cb_alloc_descriptor_type" elements
             ** of SKIP_SEQUENCE_NUMBER_OF_RECORDS long
             */
             SKIP_SEQUENCE_NUMBER_OF_RECORDS * sizeof(cb_alloc_descriptor_type) +

             /* Six control dwords */
             6 * sizeof(dword);

  }
#if defined (FEATURE_NAS_CBS_DSDS) && defined (FEATURE_TWO_3GPP_SIM)
   cb_as_id = CB_AS_ID_1;
#endif
  MSG_MED_DS_1(cb_as_id,"CB: Memory pools have been allocated successfully (%d bytes)",total_memory_pool_size) ;

  return TRUE ;

} /* end of cb_mem_init() */
#endif
/*===========================================================================

FUNCTION    CB_FIND_BEST_ALLOC

DESCRIPTION
  This function searching for the best currently available free buffer 
  according to requested memory size

DEPENDENCIES
  None

RETURN VALUE
  Memory descriptor containing pointer to memory block if there is a 
  requested memory buffer available or NULL pointer otherwise

SIDE EFFECTS
  None

===========================================================================*/
cb_alloc_descriptor_type *cb_find_best_alloc
(
  dword required_length,
  cb_alloc_descriptor_type **current_free_pool_ptr,
  dword *ctrl_scan_length,
  cb_alloc_descriptor_type *current_ctrl_ptr,
  dword max_ctrl_length
)
{
  cb_alloc_descriptor_type *current_free_ptr = *current_free_pool_ptr ;
  cb_alloc_descriptor_type *tmp_free = NULL;

  if (current_free_ptr == NULL)
  {
    return NULL ;
  }
  else
  {
    do
    {
      if (current_free_ptr->data_alloc_length >= required_length)
      {
        /* 
        ** If current memory block descriptor is bigger of equil to requested it 
        ** could be used for requested memory allocation 
        */

        if (current_free_ptr->data_alloc_length == required_length)
        {
          /* 
          ** Exact memory block is found 
          */

          /* Save properties of the found memory descriptor */
          tmp_free = current_free_ptr ;

          break ;
        }
        else
        {
          /* Found block is bigger than requested */

          if (tmp_free == NULL)
          {
            /* This is first match */

            /* Save properties of the found memory descriptor */
            tmp_free = current_free_ptr ;
          }
          else
          {
            if (tmp_free->data_alloc_length > current_free_ptr->data_alloc_length)
            {
              /* If smoller than previouse block is found do the following */

              /* Replace old match */
              tmp_free = current_free_ptr ;
            }
          }
        }
      }
    } while ((current_free_ptr = current_free_ptr->next) != NULL) ;
  }

  if (tmp_free != NULL)
  {
    if (tmp_free->data_alloc_length == required_length)
    {
      /* 
      ** The exact memory block has been found, so, remove
      ** memory block descriptor from the descriptor list
      */
      if (tmp_free->previous != NULL)
      {
        tmp_free->previous->next = tmp_free->next ;
      }

      if (tmp_free->next != NULL)
      {
        tmp_free->next->previous = tmp_free->previous ;
      }

      if (tmp_free == *current_free_pool_ptr)
      {
        *current_free_pool_ptr = tmp_free->next ;
      }
    }
    else
    {
      /* 
      ** Bigger memory block has been found, so, part of it gotta be used for
      ** requested allocation and remaining part (the rest of the originaly
      ** free block) is still free
      */

      current_free_ptr = tmp_free ;

      if ((tmp_free = cb_get_mem_descriptor(
                   ctrl_scan_length,max_ctrl_length,current_ctrl_ptr)) == NULL)
      {
        return NULL ;
      }

      tmp_free->data_alloc_length = required_length ;

      tmp_free->data_ptr = current_free_ptr->data_ptr ;

      current_free_ptr->data_alloc_length -= required_length ;

      current_free_ptr->data_ptr = (byte *)(current_free_ptr->data_ptr) + required_length ;
    }
  
    tmp_free->next     = NULL ;
    tmp_free->previous = NULL ;
  }

  return tmp_free ;
} /* end of cb_find_best_alloc() */

/*===========================================================================

FUNCTION    CB_REGISTER_ALLOC

DESCRIPTION
  This function writes down allocated memory block into 'used' linked list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_register_alloc
(
  cb_alloc_descriptor_type *new_alloc,
  cb_alloc_descriptor_type *allocated_list_ptr
)
{
  if (allocated_list_ptr == NULL)
  {
    /* 
    ** It is impossible to happen because all memory descriptor buffers
    ** must be allocated upon power up
    **/
    MSG_FATAL_DS( cb_as_id,"CB: Allocated block descriptor list does not exist",0,0,0) ;
  }
  else
  {
    while (allocated_list_ptr->next != NULL)
    {
      allocated_list_ptr = allocated_list_ptr->next ;
    }

    allocated_list_ptr->next = new_alloc ;
    new_alloc->previous = allocated_list_ptr ;
    new_alloc->next = NULL ;
  }
} /* end of cb_register_alloc() */
 
/*===========================================================================

FUNCTION    CB_POOL_ALLOC

DESCRIPTION
  This function allocate memory from the CB TASK's specific memory pool

DEPENDENCIES
  None

RETURN VALUE
  Allocated buffer pointer if there is requested memory buffer available or 
  NULL otherwise

SIDE EFFECTS
  None

===========================================================================*/
void *cb_pool_alloc
(
  dword required_length,
  dword *current_pool_size,
  cb_alloc_descriptor_type **current_free_ptr,
  cb_alloc_descriptor_type **current_alloc_ptr,
  dword *ctrl_scan_length,
  cb_alloc_descriptor_type *current_ctrl_ptr,
  dword max_ctrl_length
)
{
  cb_alloc_descriptor_type *new_alloc = NULL;

  if (required_length <= *current_pool_size)
  {
    if ((new_alloc = cb_find_best_alloc(required_length,
                                        current_free_ptr,
                                        ctrl_scan_length,
                                        current_ctrl_ptr,
                                        max_ctrl_length)) != NULL)
    {
      /* Decrease total pool size */
      *current_pool_size -= required_length ;

      if (*current_alloc_ptr == NULL)
      {
        *current_alloc_ptr = new_alloc ;
      }
      else
      {
        cb_register_alloc(new_alloc,*current_alloc_ptr) ;
      }

    }
    else
    {
      MSG_ERROR_DS_0(cb_as_id,"CB: Memory pool is fragmented") ;
    }
  }

  if (new_alloc != NULL)
  {
    return new_alloc->data_ptr ;
  }
  else
  {
    return NULL ;
  }
} /* end of cb_pool_alloc() */

/*===========================================================================

FUNCTION    CB_FIND_MEMALLOC

DESCRIPTION
  This function searching for the existing memory block descriptor according 
  to the allocated memory pointer

DEPENDENCIES
  None

RETURN VALUE
  Pointer to allocated buffer descriptor if there is a requested memory buffer 
  allocated or NULL otherwise

SIDE EFFECTS
  None

===========================================================================*/
cb_alloc_descriptor_type *cb_find_memalloc
(
  void *memory_block_ptr,
  cb_alloc_descriptor_type **entry_allocated_block_ptr
)
{
  cb_alloc_descriptor_type *allocated_block_ptr = *entry_allocated_block_ptr ;
  boolean memory_block_found = FALSE ;

  if (allocated_block_ptr != NULL)
  {
    do
    {
      if (allocated_block_ptr->data_ptr == memory_block_ptr)
      {

        /*
        ** Memory block has been found
        */
        memory_block_found = TRUE ;

        /* Remove memory block descriptor from the list */

        if (allocated_block_ptr->previous != NULL)
        {
          allocated_block_ptr->previous->next = allocated_block_ptr->next ;
        }

        if (allocated_block_ptr->next != NULL)
        {
          allocated_block_ptr->next->previous = allocated_block_ptr->previous ;
        }

        if (allocated_block_ptr == *entry_allocated_block_ptr)
        {
          *entry_allocated_block_ptr = allocated_block_ptr->next ;
        }

        break ;
      }
    }
    while ((allocated_block_ptr = allocated_block_ptr->next) != NULL) ;
  }
  else
  {
    MSG_ERROR_DS_0(cb_as_id,"CB: Alloc block descriptor list does not exist") ;
  }

  if (memory_block_found)
  {
    return allocated_block_ptr ;
  }
  else
  {
    return NULL ;
  }

} /* end cb_find_memalloc() */

/*===========================================================================

FUNCTION    CB_BRIDGE_FINDER

DESCRIPTION
  This function searches for the memory blocks which are the 

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the neighbor memory descriptor

SIDE EFFECTS
  None

===========================================================================*/
cb_alloc_descriptor_type *cb_bridge_finder
(
  cb_alloc_descriptor_type *current_descriptor,
  cb_alloc_descriptor_type *list,
  cb_mem_merge_type        *merge_type
)
{
  *merge_type = CB_NO_MERGE ;

  if (list == NULL)
  {
    return NULL ;
  }

  while (list != NULL)
  {
    if (list != current_descriptor)
    {
      /* Check left boundary of the current memory descriptor */
      if (((byte *)list->data_ptr+list->data_alloc_length) == current_descriptor->data_ptr)
      {
        *merge_type = CB_MERGE_INTO_LIST ;

        return list ;
      }
      /* Check right boundary of the current memory descriptor */
      else if (((byte *)current_descriptor->data_ptr+current_descriptor->data_alloc_length) == list->data_ptr)
      {
        *merge_type = CB_MERGE_INTO_CURRENT ;

        return list ;
      }
    }

    list = list->next ;
  }

  /* Neighbor memory descriptor has not been found */
  return NULL ;
} /* end of cb_bridge_finder() */

/*===========================================================================

FUNCTION    CB_MERGE_MEM_BLOCKS

DESCRIPTION
  This function merges two memory blocks acording to the merge rule which is
  provided by the caller function and returns remaining memory descriptor into
  descriptor pool

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_merge_mem_blocks
(
  cb_alloc_descriptor_type *current_descriptor,
  cb_alloc_descriptor_type *list,
  cb_mem_merge_type merge_rule,
  dword *scanable_pool_size,
  cb_alloc_descriptor_type *descriptor_pool,
  cb_alloc_descriptor_type **list_entry_ptr
)
{
  cb_alloc_descriptor_type *donor = NULL ;
  cb_alloc_descriptor_type *recipient = NULL ;

  switch(merge_rule)
  {
    case CB_NO_MERGE:

    /* Nothing to do */

    return ;

    case CB_MERGE_INTO_CURRENT:

      donor = list ;
      recipient = current_descriptor ;

      break ;
    case CB_MERGE_INTO_LIST:

      donor = current_descriptor ;
      recipient = list ;

      break ;
    default:
      MSG_FATAL_DS( cb_as_id,"CB: Unknown merge type",0,0,0) ;
  }

  /* Concatenate donor and recipient memory blocks */
  recipient->data_alloc_length += donor->data_alloc_length ;

  /* Clear the data buffer */
  memset(recipient->data_ptr,0xAA,recipient->data_alloc_length) ;

  /* Disconnect the donor descriptor from the list */
  if (donor->next != NULL)
  {
    donor->next->previous = donor->previous ;
  }

  if (donor->previous != NULL)
  {
    donor->previous->next = donor->next ;
  }

  if (donor == *list_entry_ptr)
  {
    *list_entry_ptr = donor->next ;
  }

  /* Return donor memory descriptor into descriptor pool */
  cb_ret_mem_descriptor(donor,scanable_pool_size,descriptor_pool) ;

} /* end of cb_merge_mem_blocks() */

/*===========================================================================

FUNCTION    CB_POOL_FREE

DESCRIPTION
  This function returns memory block descriptor into specific pool

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_pool_free
(
  cb_alloc_descriptor_type *new_free_descriptor,
  cb_alloc_descriptor_type **current_free_pool,
  dword *scanable_pool_size,
  cb_alloc_descriptor_type *descriptor_pool
)
{
  cb_alloc_descriptor_type *existing_descriptor = *current_free_pool ;
  cb_alloc_descriptor_type *tobe_bridged_descriptor = NULL ;
  void *new_memory_block_boundary = NULL ;
  cb_mem_merge_type merge_type = CB_NO_MERGE ;
 

  /* Start garbage collection */
 
  /* Find out whether or not there is an existing memory block followed by the new one */
  while (existing_descriptor != NULL)
  {
    if (((byte *)existing_descriptor->data_ptr+existing_descriptor->data_alloc_length) == new_free_descriptor->data_ptr)
    {
      /* Concatenate new and existing memory blocks */
      existing_descriptor->data_alloc_length += new_free_descriptor->data_alloc_length ;

      /* Clear the data buffer */
      memset(existing_descriptor->data_ptr,0xAA,existing_descriptor->data_alloc_length) ;

      /* Return memory descriptor into specific pool */
      cb_ret_mem_descriptor(new_free_descriptor,scanable_pool_size,descriptor_pool) ;

      if ((tobe_bridged_descriptor = cb_bridge_finder(existing_descriptor,*current_free_pool,&merge_type)) != NULL)
      {
        if (merge_type != CB_NO_MERGE)
        {
          /* Merge neighbor memory blocks */
          cb_merge_mem_blocks(existing_descriptor,tobe_bridged_descriptor,
                                                  merge_type,scanable_pool_size,descriptor_pool,current_free_pool) ;
        }
      }

      return ;
    }

    /* Get next desciptor */
    existing_descriptor = existing_descriptor->next ;
  }

  /* Reset current descriptor pointer */
  existing_descriptor = *current_free_pool ;

  /* Calculate right free memory block boundary */
  new_memory_block_boundary = (byte *)new_free_descriptor->data_ptr+new_free_descriptor->data_alloc_length ;

  /* Find out whether or not the new free memory block is followed by an existing one */
  while(existing_descriptor != NULL)
  {
    if (new_memory_block_boundary == existing_descriptor->data_ptr)
    {
      /* Concatenate new and existing memory blocks */
      existing_descriptor->data_alloc_length += new_free_descriptor->data_alloc_length ;

      existing_descriptor->data_ptr = new_free_descriptor->data_ptr ;

      /* Clear the data buffer */
      memset(existing_descriptor->data_ptr,0xAA,existing_descriptor->data_alloc_length) ;

      /* Return memory descriptor into specific pool */
      cb_ret_mem_descriptor(new_free_descriptor,scanable_pool_size,descriptor_pool) ;

      if ((tobe_bridged_descriptor = cb_bridge_finder(existing_descriptor,*current_free_pool,&merge_type)) != NULL)
      {
        if (merge_type != CB_NO_MERGE)
        {
          /* Merge neighbor memory blocks */
          cb_merge_mem_blocks(existing_descriptor,tobe_bridged_descriptor,
                                                  merge_type,scanable_pool_size,descriptor_pool,current_free_pool) ;
        }
      }

      return ;
    }

    /* Get next desciptor */
    existing_descriptor = existing_descriptor->next ;
  }

  /* Garbage collection has been finished */

  /* 
  ** If we are about here, it means that new free data block 
  ** needs to be added separately into end of the list
  */

  if (*current_free_pool == NULL)
  {
    new_free_descriptor->previous = NULL ;
    new_free_descriptor->next = NULL ;

    *current_free_pool = new_free_descriptor ;
  }
  else
  {
    existing_descriptor = *current_free_pool ;

    /* Get the last list element */
    while (existing_descriptor->next != NULL)
    {
      existing_descriptor = existing_descriptor->next ;
    }

    existing_descriptor->next = new_free_descriptor ;
    new_free_descriptor->previous = existing_descriptor ;
    new_free_descriptor->next = NULL ;
  }

  /* Clear the data buffer */
  memset(new_free_descriptor->data_ptr,0xAA,new_free_descriptor->data_alloc_length) ;

} /* end of cb_pool_free() */

/*===========================================================================

FUNCTION    CB_MEM_FREE

DESCRIPTION
  This function deallocates allocated memory block and returns it into 
  corresponding memory pool

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cb_mem_free
(
  void *memory_block_ptr
)
{
  cb_alloc_descriptor_type *allocated_block = NULL ;
  dword *current_pool_size = NULL ;
  cb_alloc_descriptor_type **current_free_ptr = NULL ;
  cb_alloc_descriptor_type **current_alloc_ptr = NULL;
  dword *scanable_pool_size = NULL ;
  cb_alloc_descriptor_type *descriptor_pool = NULL ;

#if (defined FEATURE_UMTS_BMC_DRX_SUPPORTED) || (defined FEATURE_GSM_CB_DRX_SUPPORTED)
  if (((byte *)memory_block_ptr >= cb_first_tx_schedule_pool) && 
      ((byte *)memory_block_ptr < (cb_first_tx_schedule_pool+FIRST_TX_SCHEDULE_POOL_SIZE)))
  {
    current_pool_size  = &cb_current_first_tx_schedule_pool_size ;
    current_free_ptr   = &cb_first_tx_schedule_free ;
    current_alloc_ptr  = &cb_first_tx_schedule_alloc ;
    scanable_pool_size = &cb_first_tx_schedule_ctrl_scan_length ;
#ifdef FEATURE_MODEM_HEAP
    if(cb_first_tx_schedule_ctrl_pool == NULL)
    {
      MSG_FATAL_DS( cb_as_id,"CB: Access violation in cb_first_tx_schedule_ctrl_pool",0,0,0);
    }
#endif
    descriptor_pool    = cb_first_tx_schedule_ctrl_pool ;
  }
  else if (((byte *)memory_block_ptr >= cb_retx_schedule_pool) && 
           ((byte *)memory_block_ptr < (cb_retx_schedule_pool+RETX_SCHEDULE_POOL_SIZE)))
  {
    current_pool_size  = &cb_retx_schedule_pool_size ;
    current_free_ptr   = &cb_retx_schedule_free ;
    current_alloc_ptr  = &cb_retx_schedule_alloc ;
    scanable_pool_size = &cb_retx_schedule_ctrl_scan_length ;
#ifdef FEATURE_MODEM_HEAP
    if(cb_retx_schedule_ctrl_pool == NULL)
    {
      MSG_FATAL_DS( cb_as_id,"CB: Access violation in cb_retx_schedule_ctrl_pool",0,0,0);
    }
#endif
    descriptor_pool    = cb_retx_schedule_ctrl_pool ;
  }
  else if (((byte *)memory_block_ptr >= cb_new_msg_pool) && 
           ((byte *)memory_block_ptr < (cb_new_msg_pool+NEW_MSG_BIT_MAP_POOL_SIZE)))
  {
    current_pool_size  = &cb_current_new_msg_pool_size ;
    current_free_ptr   = &cb_new_msg_free;
    current_alloc_ptr  = &cb_new_msg_alloc ;
    scanable_pool_size = &cb_new_msg_ctrl_scan_length ;
#ifdef FEATURE_MODEM_HEAP
    if(cb_new_msg_ctrl_pool == NULL)
    {
      MSG_FATAL_DS( cb_as_id,"CB: Access violation in cb_new_msg_ctrl_pool",0,0,0);
    }
#endif
    descriptor_pool    = cb_new_msg_ctrl_pool ;
  }
  else
#endif /* DRX */
  if (((byte *)memory_block_ptr >= cb_user_page_info_pool) && 
      ((byte *)memory_block_ptr < (cb_user_page_info_pool+USER_PAGE_INFO_POOL_SIZE)))
  {
    current_pool_size  = &cb_user_page_info_pool_size ;
    current_free_ptr   = &cb_user_page_info_free ;
    current_alloc_ptr  = &cb_user_page_info_alloc ;
    scanable_pool_size = &cb_user_page_info_ctrl_scan_length ;
#ifdef FEATURE_MODEM_HEAP
    if(cb_user_page_info_ctrl_pool == NULL)
    {
      MSG_FATAL_DS( cb_as_id,"CB: Access violation in cb_user_page_info_ctrl_pool",0,0,0);
    }
#endif
    descriptor_pool    = cb_user_page_info_ctrl_pool ;
  }
  else if (((byte *)memory_block_ptr >= cb_message_list_pool) && 
           ((byte *)memory_block_ptr < (cb_message_list_pool+MESSAGE_LIST_POOL_SIZE)))
  {
    current_pool_size  = &cb_message_list_pool_size ;
    current_free_ptr   = &cb_message_list_free ;
    current_alloc_ptr  = &cb_message_list_alloc ;
    scanable_pool_size = &cb_message_list_ctrl_scan_length ;
#ifdef FEATURE_MODEM_HEAP
    if(cb_message_list_ctrl_pool == NULL)
    {
      MSG_FATAL_DS( cb_as_id,"CB: Access violation in cb_message_list_ctrl_pool",0,0,0);
    }
#endif
    descriptor_pool    = cb_message_list_ctrl_pool ;
  }
  else if (((byte *)memory_block_ptr >= cb_missing_page_pool) && 
           ((byte *)memory_block_ptr < (cb_missing_page_pool+MISSING_PAGE_POOL_SIZE)))
  {
    current_pool_size  = &cb_missing_page_pool_size ;
    current_free_ptr   = &cb_missing_page_free ;
    current_alloc_ptr  = &cb_missing_page_alloc ;
    scanable_pool_size = &cb_missing_page_ctrl_scan_length ;
#ifdef FEATURE_MODEM_HEAP
    if(cb_missing_page_ctrl_pool == NULL)
    {
      MSG_FATAL_DS( cb_as_id,"CB: Access violation in cb_missing_page_ctrl_pool",0,0,0);
    }
#endif
    descriptor_pool    = cb_missing_page_ctrl_pool ;
  } 
  else if (((byte *)memory_block_ptr >= cb_skip_sequence_pool) && 
           ((byte *)memory_block_ptr < (cb_skip_sequence_pool+SKIP_SEQUENCE_POOL_SIZE)))
  {
    current_pool_size  = &cb_skip_sequence_pool_size ;
    current_free_ptr   = &cb_skip_sequence_free ;
    current_alloc_ptr  = &cb_skip_sequence_alloc ;
    scanable_pool_size = &cb_skip_sequence_ctrl_scan_length ;
#ifdef FEATURE_MODEM_HEAP
    if(cb_skip_sequence_ctrl_pool == NULL)
    {
      MSG_FATAL_DS( cb_as_id,"CB: Access violation in cb_skip_sequence_ctrl_pool",0,0,0);
    }
#endif
    descriptor_pool    = cb_skip_sequence_ctrl_pool ;
  }
  else
  {
    /* This pointer is not belonging to the existing CB task allocation pool */
    MSG_FATAL_DS( cb_as_id,"CB: Access violation %X",(uint32)memory_block_ptr,0,0) ;
  }

  if ((allocated_block = cb_find_memalloc(memory_block_ptr,current_alloc_ptr)) != NULL)
  {
    /* Increase total pool size */
    *current_pool_size += allocated_block->data_alloc_length ;

    cb_pool_free(allocated_block,current_free_ptr,scanable_pool_size,descriptor_pool) ;
  }
  else
  {
    /* This pointer has not been allocated using CB task allocation routines */
    MSG_FATAL_DS( cb_as_id,"CB: Memory access violation %X",(uint32)memory_block_ptr,0,0) ;
  }

} /* end of cb_mem_free() */

/*===========================================================================

FUNCTION    CB_MEM_ALLOC

DESCRIPTION
  This function allocate memory from the CB TASK's memory pools

DEPENDENCIES
  None

RETURN VALUE
  Allocated buffer pointer if there is requested memory buffer available or 
  NULL otherwise

SIDE EFFECTS
  None

===========================================================================*/
void *cb_mem_alloc
(
  cb_mem_alloc_id_type mem_alloc_id,
  dword required_length
)
{
  void *allocated_buffer = NULL ;
  dword *current_pool_size = NULL ;
  cb_alloc_descriptor_type **current_free_ptr = NULL ;
  cb_alloc_descriptor_type **current_alloc_ptr = NULL ;
  dword *ctrl_scan_length = NULL ;
  cb_alloc_descriptor_type *current_ctrl_ptr = NULL ;
  dword max_ctrl_length = 0 ;

  switch (mem_alloc_id)
  {
#if (defined FEATURE_UMTS_BMC_DRX_SUPPORTED) || (defined FEATURE_GSM_CB_DRX_SUPPORTED)
    case CB_FIRST_TX_SCHEDULE_ALLOC_ID:
#ifdef FEATURE_MODEM_HEAP
      if(cb_first_tx_schedule_pool == NULL)
      {
        if (( (cb_first_tx_schedule_pool = (byte *) modem_mem_calloc(1,FIRST_TX_SCHEDULE_POOL_SIZE,MODEM_MEM_CLIENT_NAS) ) != NULL) )
        {
          MSG_MED_DS_1(cb_as_id, "memory allocated for cb_first_tx_schedule_pool with location %x",cb_first_tx_schedule_pool);
          if( (cb_first_tx_schedule_ctrl_pool == NULL) && (( (cb_first_tx_schedule_ctrl_pool = (cb_alloc_descriptor_type *) modem_mem_calloc(1,sizeof(cb_alloc_descriptor_type) *CB_MAX_SCHEDULING_SFN_NUMBER,MODEM_MEM_CLIENT_NAS) ) != NULL)))
          {
            MSG_MED_DS_1(cb_as_id, "memory allocated for cb_first_tx_schedule_ctrl_pool with location %x",cb_first_tx_schedule_ctrl_pool);
            if (cb_mem_pool_init(cb_first_tx_schedule_pool,
                                 FIRST_TX_SCHEDULE_POOL_SIZE,
                                 &cb_current_first_tx_schedule_pool_size,
                                 &cb_first_tx_schedule_ctrl_scan_length,
                                 CB_MAX_SCHEDULING_SFN_NUMBER,
                                 cb_first_tx_schedule_ctrl_pool,
                                 &cb_first_tx_schedule_free,
                                 &cb_first_tx_schedule_alloc) == FALSE)
            {
              return NULL;
            }
            else
            {
              MSG_MED_DS_1(cb_as_id,"FIRST TX SCHEDULE pool size is initialized here as %x", cb_current_first_tx_schedule_pool_size);
            }
          }
          else
          {
            MSG_FATAL_DS( cb_as_id, "ERROR: Unable to allocate memory for cb_first_tx_schedule_ctrl_pool %x", (int) cb_first_tx_schedule_ctrl_pool,0,0 );
          }
        }
        else
        {
          MSG_FATAL_DS( cb_as_id, "ERROR: Unable to allocate memory for cb_first_tx_schedule_pool", 0,0,0 );
        }
      }
#endif
      current_pool_size = &cb_current_first_tx_schedule_pool_size;
      current_free_ptr = &cb_first_tx_schedule_free ;
      current_alloc_ptr = &cb_first_tx_schedule_alloc ;
      ctrl_scan_length = &cb_first_tx_schedule_ctrl_scan_length ;
      current_ctrl_ptr = cb_first_tx_schedule_ctrl_pool ;
      max_ctrl_length = FIRST_TX_SCHEDULE_POOL_SIZE ;

      break ; 

    case CB_RETX_SCHEDULE_ALLOC_ID:
#ifdef FEATURE_MODEM_HEAP
      if(cb_retx_schedule_pool == NULL)
      {
        if(( (cb_retx_schedule_pool = (byte *) modem_mem_calloc(1,RETX_SCHEDULE_POOL_SIZE,MODEM_MEM_CLIENT_NAS) ) != NULL) )
        {
          MSG_MED_DS_1(cb_as_id, "memory allocated for cb_retx_schedule_pool with location %x",cb_first_tx_schedule_pool);
          if( (cb_retx_schedule_ctrl_pool == NULL) && (( (cb_retx_schedule_ctrl_pool = (cb_alloc_descriptor_type *) modem_mem_calloc(1,sizeof(cb_alloc_descriptor_type) *CB_MAX_SCHEDULING_SFN_NUMBER,MODEM_MEM_CLIENT_NAS) ) != NULL) ))
          {
            MSG_MED_DS_1(cb_as_id, "memory allocated for cb_retx_schedule_ctrl_pool with location %x",cb_first_tx_schedule_ctrl_pool);
            if (cb_mem_pool_init(cb_retx_schedule_pool,
                                 RETX_SCHEDULE_POOL_SIZE,
                                 &cb_retx_schedule_pool_size,
                                 &cb_retx_schedule_ctrl_scan_length,
                                 CB_MAX_SCHEDULING_SFN_NUMBER,
                                 cb_retx_schedule_ctrl_pool,
                                 &cb_retx_schedule_free,
                                 &cb_retx_schedule_alloc) == FALSE)
            {
              return NULL ;
            }
            else
            {
              MSG_MED_DS_1(cb_as_id,"RETX SCHEDULE pool size is initialized here as %x",cb_retx_schedule_pool_size);
            }
          }
          else
          {
            MSG_FATAL_DS( cb_as_id, "ERROR: Unable to allocate memory for cb_retx_schedule_ctrl_pool %x", (int) cb_retx_schedule_ctrl_pool,0,0 );
          }
        }
        else
        {
          MSG_FATAL_DS( cb_as_id, "ERROR: Unable to allocate memory for cb_retx_schedule_pool", 0,0,0 );
        }
      }
#endif
      current_pool_size = &cb_retx_schedule_pool_size;
      current_free_ptr = &cb_retx_schedule_free ;
      current_alloc_ptr = &cb_retx_schedule_alloc ;
      ctrl_scan_length = &cb_retx_schedule_ctrl_scan_length ;
      current_ctrl_ptr = cb_retx_schedule_ctrl_pool ;
      max_ctrl_length = CB_MAX_SCHEDULING_SFN_NUMBER ;

      break ;

    case CB_NEW_MSG_ALLOC_ID:
#ifdef FEATURE_MODEM_HEAP
      if(cb_new_msg_pool == NULL)
      {
        if(( (cb_new_msg_pool = (byte *) modem_mem_calloc(1,NEW_MSG_BIT_MAP_POOL_SIZE,MODEM_MEM_CLIENT_NAS) ) != NULL) )
        {
          MSG_MED_DS_1(cb_as_id, "memory allocated for cb_new_msg_pool with location %x",cb_first_tx_schedule_pool);
          if( (cb_new_msg_ctrl_pool == NULL) && (( (cb_new_msg_ctrl_pool = (cb_alloc_descriptor_type *) modem_mem_calloc(1,sizeof(cb_alloc_descriptor_type) *CB_MAX_NEW_MSG,MODEM_MEM_CLIENT_NAS) ) != NULL) ))
          {
            MSG_MED_DS_1(cb_as_id, "memory allocated for cb_new_msg_ctrl_pool with location %x",cb_first_tx_schedule_ctrl_pool);
            if (cb_mem_pool_init(cb_new_msg_pool,
                                 NEW_MSG_BIT_MAP_POOL_SIZE,
                                 &cb_current_new_msg_pool_size,
                                 &cb_new_msg_ctrl_scan_length,
                                 CB_MAX_NEW_MSG,
                                 cb_new_msg_ctrl_pool,
                                 &cb_new_msg_free,
                                 &cb_new_msg_alloc) == FALSE)
            {
              return NULL;
            }
            else
            {
              MSG_MED_DS_1(cb_as_id,"NEW MESSAGE pool size is initialized here as %x",cb_current_new_msg_pool_size);
            }
          }
          else
          {
            MSG_FATAL_DS( cb_as_id, "ERROR: Unable to allocate memory for cb_new_msg_ctrl_pool %x", (int) cb_new_msg_ctrl_pool,0,0 );
          }
        } 
        else
        {
          MSG_FATAL_DS( cb_as_id, "ERROR: Unable to allocate memory for cb_new_msg_pool", 0,0,0 );
        }
      }
#endif
      current_pool_size = &cb_current_new_msg_pool_size;
      current_free_ptr = &cb_new_msg_free ;
      current_alloc_ptr = &cb_new_msg_alloc ;
      ctrl_scan_length = &cb_new_msg_ctrl_scan_length ;
      current_ctrl_ptr = cb_new_msg_ctrl_pool ;
      max_ctrl_length = CB_MAX_NEW_MSG ;

      break ;


#else /* DRX */

    case CB_FIRST_TX_SCHEDULE_ALLOC_ID:
    case CB_RETX_SCHEDULE_ALLOC_ID:

      MSG_ERROR_DS_0(cb_as_id,"CB: DRX mode is not supported") ;
        
      return NULL ;

#endif

    case CB_USER_PAGE_INFO_ALLOC_ID:
#ifdef FEATURE_MODEM_HEAP
      if (cb_user_page_info_pool == NULL )
      {
        if(( (cb_user_page_info_pool = (byte *) modem_mem_calloc(1,USER_PAGE_INFO_POOL_SIZE,MODEM_MEM_CLIENT_NAS) ) != NULL) )
        {
          MSG_MED_DS_1(cb_as_id, "memory allocated for cb_user_page_info_pool with location %x",cb_user_page_info_pool);
          if( (cb_user_page_info_ctrl_pool == NULL ) && ((( cb_user_page_info_ctrl_pool = (cb_alloc_descriptor_type *) modem_mem_calloc(1,sizeof(cb_alloc_descriptor_type) *USER_PAGE_INFO_NUMBER_OF_RECORDS,MODEM_MEM_CLIENT_NAS) ) != NULL) ))
          {
            MSG_MED_DS_1(cb_as_id, "Memory allocated for cb_user_page_info_ctrl_pool and location is %x", cb_user_page_info_ctrl_pool);
            if (cb_mem_pool_init(cb_user_page_info_pool,
                                 USER_PAGE_INFO_POOL_SIZE,
                                 &cb_user_page_info_pool_size,
                                 &cb_user_page_info_ctrl_scan_length,
                                 USER_PAGE_INFO_NUMBER_OF_RECORDS,
                                 cb_user_page_info_ctrl_pool,
                                 &cb_user_page_info_free,
                                 &cb_user_page_info_alloc) == FALSE)
            {
              return NULL;
            }
            else
            {
              MSG_MED_DS_1(cb_as_id,"USER PAGE INFO pool size is initialized here as %x",cb_user_page_info_pool_size);
            }
          }
          else
          {
            MSG_FATAL_DS( cb_as_id, "ERROR: Unable to allocate memory for cb_user_page_info_ctrl_pool %x", (int) cb_user_page_info_ctrl_pool,0,0 );
          }
        }
        else
        {
          MSG_FATAL_DS( cb_as_id, "ERROR: Unable to allocate memory for cb_user_page_info_pool", 0,0,0 );
        }
      }
#endif
      current_pool_size = &cb_user_page_info_pool_size ;
      current_free_ptr = &cb_user_page_info_free ;
      current_alloc_ptr = &cb_user_page_info_alloc ;
      ctrl_scan_length = &cb_user_page_info_ctrl_scan_length ;
      current_ctrl_ptr = cb_user_page_info_ctrl_pool ;
      max_ctrl_length = USER_PAGE_INFO_NUMBER_OF_RECORDS ;

      break ;

    case CB_MESSAGE_LIST_ALLOC_ID:
#ifdef FEATURE_MODEM_HEAP
      if( cb_message_list_pool == NULL )
      {
        if((( cb_message_list_pool = (byte *) modem_mem_calloc(1,MESSAGE_LIST_POOL_SIZE,MODEM_MEM_CLIENT_NAS) ) != NULL) )
        {
          MSG_MED_DS_1(cb_as_id, "memory allocated for cb_message_list_pool with location %x",cb_message_list_pool);
          if( ( cb_message_list_ctrl_pool == NULL ) && ((( cb_message_list_ctrl_pool = (cb_alloc_descriptor_type *) modem_mem_calloc(1,sizeof(cb_alloc_descriptor_type) *MESSAGE_LIST_NUMBER_OF_RECORDS,MODEM_MEM_CLIENT_NAS) ) != NULL) ))
          {
            MSG_MED_DS_1(cb_as_id, "Memory allocated for cb_message_list_ctrl_pool and location is %x", cb_message_list_ctrl_pool);
            if (cb_mem_pool_init(cb_message_list_pool,
                                 MESSAGE_LIST_POOL_SIZE,
                                 &cb_message_list_pool_size,
                                 &cb_message_list_ctrl_scan_length,
                                 MESSAGE_LIST_NUMBER_OF_RECORDS,
                                 cb_message_list_ctrl_pool,
                                 &cb_message_list_free,
                                 &cb_message_list_alloc) == FALSE)
            {
              return NULL;
            }
            else
            {
              MSG_MED_DS_1(cb_as_id,"MESSAGE LIST pool size is initialized here as %x",cb_message_list_pool_size);
            }
          }
          else
          {
            MSG_FATAL_DS( cb_as_id, "ERROR: Unable to allocate memory for cb_message_list_ctrl_pool %x", (int) cb_message_list_ctrl_pool,0,0 );
          }
        }
        else
        {
          MSG_FATAL_DS( cb_as_id, "ERROR: Unable to allocate memory for cb_message_list_pool", 0,0,0 );
        }
      }
#endif
      current_pool_size = &cb_message_list_pool_size ;
      current_free_ptr = &cb_message_list_free ;
      current_alloc_ptr = &cb_message_list_alloc ;
      ctrl_scan_length = &cb_message_list_ctrl_scan_length ;
      current_ctrl_ptr = cb_message_list_ctrl_pool ;
      max_ctrl_length = MESSAGE_LIST_NUMBER_OF_RECORDS ;

      break ;

    case CB_MISSING_PAGE_ALLOC_ID:
#ifdef FEATURE_MODEM_HEAP
      if (cb_missing_page_pool == NULL)
      {
        if((( cb_missing_page_pool = (byte *) modem_mem_calloc(1,MISSING_PAGE_POOL_SIZE,MODEM_MEM_CLIENT_NAS) ) != NULL))
        {
          MSG_MED_DS_1(cb_as_id, "memory allocated for cb_missing_page_pool with location %x",cb_missing_page_pool);
          if( ( cb_missing_page_ctrl_pool == NULL ) && ((( cb_missing_page_ctrl_pool = (cb_alloc_descriptor_type *) modem_mem_calloc(1,sizeof(cb_alloc_descriptor_type) *MISSING_PAGE_NUMBER_OF_RECORDS,MODEM_MEM_CLIENT_NAS) ) != NULL) ))
          {
            MSG_MED_DS_1(cb_as_id, "Memory allocated for cb_missing_page_ctrl_pool and location is %x", cb_missing_page_ctrl_pool);
            if (cb_mem_pool_init(cb_missing_page_pool,
                                 MISSING_PAGE_POOL_SIZE,
                                 &cb_missing_page_pool_size,
                                 &cb_missing_page_ctrl_scan_length,
                                 MISSING_PAGE_NUMBER_OF_RECORDS,
                                 cb_missing_page_ctrl_pool,
                                 &cb_missing_page_free,
                                 &cb_missing_page_alloc) == FALSE)
            {
              return NULL ;
            }
            else
            {
              MSG_MED_DS_1(cb_as_id,"MISSING PAGE pool size is initialized here as %x",cb_missing_page_pool_size);
            }
          }
          else
          {
            MSG_FATAL_DS( cb_as_id, "ERROR: Unable to allocate memory for cb_missing_page_ctrl_pool %x", (int) cb_missing_page_ctrl_pool,0,0 );
          }
        }
        else
        {
          MSG_FATAL_DS( cb_as_id, "ERROR: Unable to allocate memory for cb_missing_page_pool %x", (int)cb_missing_page_pool,0,0 );
        }
      }
#endif
      current_pool_size = &cb_missing_page_pool_size ;
      current_free_ptr = &cb_missing_page_free ;
      current_alloc_ptr = &cb_missing_page_alloc ;
      ctrl_scan_length = &cb_missing_page_ctrl_scan_length ;
      current_ctrl_ptr = cb_missing_page_ctrl_pool ;
      max_ctrl_length = MISSING_PAGE_NUMBER_OF_RECORDS ;

      break ;

    case CB_SKIP_SEQUENCE_ALLOC_ID:
#ifdef FEATURE_MODEM_HEAP
      if (cb_skip_sequence_pool == NULL )
      {
        if((( cb_skip_sequence_pool = (byte *) modem_mem_calloc(1,SKIP_SEQUENCE_POOL_SIZE,MODEM_MEM_CLIENT_NAS) ) != NULL) )
        {
          MSG_MED_DS_1(cb_as_id, "memory allocated for cb_skip_squence_pool with location %x",cb_skip_sequence_pool);
          if( (cb_skip_sequence_ctrl_pool == NULL ) && ((( cb_skip_sequence_ctrl_pool = (cb_alloc_descriptor_type *) modem_mem_calloc(1,sizeof(cb_alloc_descriptor_type) *SKIP_SEQUENCE_NUMBER_OF_RECORDS,MODEM_MEM_CLIENT_NAS) ) != NULL) ))
          {
            MSG_MED_DS_1(cb_as_id, "Memory allocated for cb_skip_sequence_ctrl_pool and location is %x", cb_skip_sequence_ctrl_pool);
            if (cb_mem_pool_init(cb_skip_sequence_pool,
                                 SKIP_SEQUENCE_POOL_SIZE,
                                 &cb_skip_sequence_pool_size,
                                 &cb_skip_sequence_ctrl_scan_length,
                                 SKIP_SEQUENCE_NUMBER_OF_RECORDS,
                                 cb_skip_sequence_ctrl_pool,
                                 &cb_skip_sequence_free,
                                 &cb_skip_sequence_alloc) == FALSE)
            {
              return NULL ;
            }
            else
            {
              MSG_MED_DS_1(cb_as_id,"SKIP SEQUENCE pool size is initialized here as %x",cb_skip_sequence_pool_size);
            }
          }
          else
          {
            MSG_FATAL_DS( cb_as_id, "ERROR: Unable to allocate memory for cb_skip_sequence_ctrl_pool %x", (int) cb_skip_sequence_ctrl_pool,0,0 );
          }
        }
        else
        {
          MSG_FATAL_DS( cb_as_id, "ERROR: Unable to allocate memory for cb_skip_sequence_pool", 0,0,0 );
        }
      }
#endif
      current_pool_size = &cb_skip_sequence_pool_size ;
      current_free_ptr = &cb_skip_sequence_free ;
      current_alloc_ptr = &cb_skip_sequence_alloc ;
      ctrl_scan_length = &cb_skip_sequence_ctrl_scan_length ;
      current_ctrl_ptr = cb_skip_sequence_ctrl_pool ;
      max_ctrl_length = SKIP_SEQUENCE_NUMBER_OF_RECORDS ;

      break ;

    default:
      MSG_FATAL_DS( cb_as_id,"CB: Illigal mem_alloc_id = %d",(int)mem_alloc_id,0,0) ;
  }

  /* Allocate reuested memory buffer */
  allocated_buffer = cb_pool_alloc(required_length,
                                   current_pool_size,
                                   current_free_ptr,
                                   current_alloc_ptr,
                                   ctrl_scan_length,
                                   current_ctrl_ptr,
                                   max_ctrl_length) ;

  if (allocated_buffer == NULL)
  {
    MSG_ERROR_DS_3(cb_as_id,"CB: Not enough memory (%d of %d) to allocate %d",
                             required_length,*current_pool_size,mem_alloc_id) ;
    if ((mem_alloc_id == CB_MESSAGE_LIST_ALLOC_ID) ||
        (mem_alloc_id == CB_USER_PAGE_INFO_ALLOC_ID) ||
        (mem_alloc_id == CB_MISSING_PAGE_ALLOC_ID))
    {
      cb_send_wms_error_indication(CB_WMS_ERROR_MEMORY_FULL);
    }
  }

  return allocated_buffer ;
} /* end of cb_mem_alloc() */

#ifdef FEATURE_MODEM_HEAP
void cb_free_dynamic_pool_NDRX(void )
{
  if(cb_user_page_info_pool != NULL)
  {
    modem_mem_free((byte*) cb_user_page_info_pool,MODEM_MEM_CLIENT_NAS);
    cb_user_page_info_pool = NULL;
    MSG_MED_DS_1(cb_as_id, "Freeing cb_user_page_info_pool and location is %x",cb_user_page_info_pool);
  }
  if(cb_user_page_info_ctrl_pool != NULL)
  {
    modem_mem_free((cb_alloc_descriptor_type*) cb_user_page_info_ctrl_pool,MODEM_MEM_CLIENT_NAS);
    cb_user_page_info_ctrl_pool = NULL;
    MSG_MED_DS_1(cb_as_id, "Freeing cb_user_page_info_ctrl_pool and location is %x",cb_user_page_info_ctrl_pool);
  }
  if(cb_message_list_pool != NULL)
  {
    modem_mem_free((byte*) cb_message_list_pool,MODEM_MEM_CLIENT_NAS);
    cb_message_list_pool = NULL;
    MSG_MED_DS_1(cb_as_id, "Freeing cb_message_list_pool and location is %x",cb_message_list_pool);
  }
  if(cb_message_list_ctrl_pool != NULL)
  {
    modem_mem_free((cb_alloc_descriptor_type*) cb_message_list_ctrl_pool,MODEM_MEM_CLIENT_NAS);
    cb_message_list_ctrl_pool = NULL;
    MSG_MED_DS_1(cb_as_id, "Freeing cb_message_list_ctrl_pool and location is %x",cb_message_list_ctrl_pool);
  }
  if(cb_missing_page_pool != NULL)
  {
    modem_mem_free((byte*) cb_missing_page_pool,MODEM_MEM_CLIENT_NAS);
    cb_missing_page_pool = NULL;
    MSG_MED_DS_1(cb_as_id, "Freeing cb_missing_page_pool and location is %x",cb_missing_page_pool);
  }
  if(cb_missing_page_ctrl_pool != NULL)
  {
    modem_mem_free((cb_alloc_descriptor_type*) cb_missing_page_ctrl_pool,MODEM_MEM_CLIENT_NAS);
    cb_missing_page_ctrl_pool = NULL;
    MSG_MED_DS_1(cb_as_id, "Freeing cb_missing_page_ctrl_pool and location is %x",cb_missing_page_ctrl_pool);
  }
}

void cb_free_skip_pool_if_needed(void)
{
  /* Return skip pool to heap if its not occupied */
  if(cb_skip_sequence_alloc == NULL)
  {
    if(cb_skip_sequence_pool != NULL)
    {
      modem_mem_free((byte*) cb_skip_sequence_pool,MODEM_MEM_CLIENT_NAS);
      cb_skip_sequence_pool = NULL;
      MSG_MED_DS_1(cb_as_id, "Freeing cb_skip_sequence_pool and location is %x",cb_skip_sequence_pool); 
    }
    if(cb_skip_sequence_ctrl_pool != NULL)
    {
      modem_mem_free((cb_alloc_descriptor_type*) cb_skip_sequence_ctrl_pool,MODEM_MEM_CLIENT_NAS);
      cb_skip_sequence_ctrl_pool = NULL;
      MSG_MED_DS_1(cb_as_id, "Freeing cb_skip_sequence_ctrl_pool and location is %x",cb_skip_sequence_ctrl_pool);
    }
  }
}

#if (defined FEATURE_UMTS_BMC_DRX_SUPPORTED) || (defined FEATURE_GSM_CB_DRX_SUPPORTED)

void cb_free_dynamic_pool_DRX(void )
{
  if(cb_first_tx_schedule_pool != NULL)
  {
    modem_mem_free((byte*) cb_first_tx_schedule_pool,MODEM_MEM_CLIENT_NAS);
    cb_first_tx_schedule_pool = NULL;
    MSG_MED_DS_1(cb_as_id, "Freeing cb_first_tx_schedule_pool and location is %x",cb_first_tx_schedule_pool);
  }

  if(cb_first_tx_schedule_ctrl_pool != NULL)
  {
    modem_mem_free((cb_alloc_descriptor_type*) cb_first_tx_schedule_ctrl_pool,MODEM_MEM_CLIENT_NAS);
    cb_first_tx_schedule_ctrl_pool = NULL;
    MSG_MED_DS_1(cb_as_id, "Freeing cb_first_tx_schedule_ctrl_pool and location is %x",cb_first_tx_schedule_ctrl_pool);
  }

  if(cb_retx_schedule_pool != NULL)
  {
    modem_mem_free((byte*) cb_retx_schedule_pool,MODEM_MEM_CLIENT_NAS);
    cb_retx_schedule_pool = NULL;
    MSG_MED_DS_1(cb_as_id, "Freeing cb_retx_schedule_pool and location is %x",cb_retx_schedule_pool);
  }

  if(cb_retx_schedule_ctrl_pool != NULL)
  {
    modem_mem_free((cb_alloc_descriptor_type*) cb_retx_schedule_ctrl_pool,MODEM_MEM_CLIENT_NAS);
    cb_retx_schedule_ctrl_pool = NULL;
    MSG_MED_DS_1(cb_as_id, "Freeing cb_retx_schedule_ctrl_pool and location is %x",cb_retx_schedule_ctrl_pool);
  }

  if(cb_new_msg_pool != NULL)
  {
    modem_mem_free((byte*) cb_new_msg_pool,MODEM_MEM_CLIENT_NAS);
    cb_new_msg_pool = NULL;
    MSG_MED_DS_1(cb_as_id, "Freeing cb_new_msg_pool and location is %x",cb_new_msg_pool);
  }

  if(cb_new_msg_ctrl_pool != NULL)
  {
    modem_mem_free((cb_alloc_descriptor_type*) cb_new_msg_ctrl_pool,MODEM_MEM_CLIENT_NAS);
    cb_new_msg_ctrl_pool = NULL;
    MSG_MED_DS_1(cb_as_id, "Freeing cb_new_msg_ctrl_pool and location is %x",cb_new_msg_ctrl_pool);
  }

}
#endif

#endif
#endif /* #ifdef FEATURE_GSM_CB */
