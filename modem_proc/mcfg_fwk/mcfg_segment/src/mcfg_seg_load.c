/**
  @file mcfg_seg_load.c

  @brief  
*/
/*
  Copyright (c) 2013 QUALCOMM Technologies Incorporated.
          All Rights Reserved.
    Qualcomm Confidential and Proprietary
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/mcfg_segment/src/mcfg_seg_load.c#1 $
  $DateTime: 2016/12/13 08:00:27 $


  when        who     what, where, why
  ---------   ---     ---------------------------------------------------------------------------
 2015/06/17   skotalwa Fix CR 855997 - Remvoing unnecessary features
 2015/03/24   skotalwa Fix CR 811284 - DSM initilization in FTM mode
 2015/2/19    skotalwa Fix FR CR797742 Reducing amss heap borrowing by 1M
 2015/2/17    skotalwa Fix for CR792657 Enque efs sync
 2014/11/24   skotalwa fix for CR761457- DSM initilizaiton
 2014/11/13   skotawla fix for CR716360 borrow 3M from HLOS 
 2014/10/27   skotalwa CR 746336
 2015/28/10   skotalwa CR 746437 Making timer non-deferable 
 2014/06/10   skotalwa CR 670990: removing unnecessary dependency of mmu update on segment loading feature. 
 2014/5/10    skotawla CR658741 disable both segments
 2014/04/24   skotalwa CR652248 - F3 log timer fix
 2014/04/18   skotalwa Fix for CR651921
 2014/04/24   skotalwa CR651132 TLB OPT CHANGES 
 2014/04/04   skotalwa CR644068
 2014/04/02   skotalwa Fix for CR_642646 
 2014/03/14   sk      Fix for CR_632152
 2014/03/13   hzhi    Added F3 messages to indicate currently loaded segment for MST testing. 
 2014/03/11   hzhi    Updated segload_config_handler() to simplify call flows. 
 2014/02/14   hzhi    Fix for CR_616433
 2013/12/19   sk      Fix for CR_591729
 2013/12/18   sk      Fix for CR_591396
 2013/12/13   sk      updates to mcfg_nv_read/write
 2013/12/10   sk      Updated for constant name change and FEATURE SEGLOAD ALL
 2013/11/26   hzhi    Updated FUNCTION segload_config_handler and related API to deal with EFS. 
 2013/11/14   hzhi    Moved FUNCTION get_tdscdma_interface and FUNCTION get_wcdma_interface definition from MMCP. 
 2013/09/19   hzhi    Moved mcfg_segment_set_boot_params(uint32 BootParam) from test_interface.c.
 2013/09/05   hzhi    Added mcfg_segment_exclude_technologies() function for segment loading. 
 2013/05/17   hzhi    Added Function seg_load_release_inactive_mode(void) and seg_load_init(void).
 2013/05/07   sk      Added file hearder according to qualcomm standards.
 2013/03/23   hzhi    Added Function mcfg_segment_reset(uint32 ResetParam).
 2013/01/01   ngeng   Created for segment loading.

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "sys_type.h"
#include "sys.h"
#include "dsm_init.h"

#ifdef FEATURE_SEGMENT_LOADING

#include "mcfg_common.h"
#include "mcfg_int.h"
#include "mcfg_nv.h"
#include "mcfg_osal.h"
#include "mcfg_utils.h"
#include "mcfg_seg_load.h"

#include "nv_items.h"

//for get_wcdma_interface() and get_tdscdma_interface(). 
#include "atomic_ops.h"

//for reclaiming TLB region via QURTK API. 
//#include "qurt_memory.h"

//for using heap API. 
#include "modem_mem.h"        /* External interface to modem_mem.c */
#include "memheap.h"
#ifdef FEATURE_BORROW_FROM_HLOS
#include "dynamic_heap.h"
#endif
/*=============================================================================================
 * adding timer to delay mcfg_modem_reset in order to print out F3 msg before crash happen. 
 *=============================================================================================*/
#include "timer.h"

static timer_type       mcfg_segment_f3_timer;  
static timer_group_type mcfg_segment_f3_timer_group;
#define MCFG_SEGMENT_F3_TIMER_FIRST_TIMEOUT (0) 
#define MCFG_SEGMENT_F3_TIMER_PERIOD_TIMEOUT (10*1000)
boolean f3_config;

/*=============================================================================================*/

typedef struct
{
  clsid_t       clsid;
  get_iface_fn  pGetIfaceFn;
} component_entry;

/* local interface pointer for create_instance() in mcfg_segment_exclude_technologies(uint32  technologies) */
static interface_t *pi_tdscdma = NULL; 
static interface_t *pi_wcdma = NULL;
static int curr_boot_up = -1;
static mcfg_segload_status_e_type mcfg_seg_last_error = MCFG_SEGLOAD_SUCCESS;

//interface constructor defined in wcdma_interface.c and tdscdma_interface.c. 
extern interface_t *wcdma_interface_ctor(void);
extern interface_t *wcdma_interface_mmcp_ctor(void);
extern interface_t *wcdma_interface_rftech_ctor(void);
extern interface_t *wcdma_interface_qchat_ctor(void);
extern interface_t *tdscdma_interface_ctor(void);

//interface instance table indicating which segment is available. 
static  component_entry  components[] =
{

	/*	added for interface	*/
	{CLSID_TDSCDMA, tdscdma_interface_ctor},
	{CLSID_WCDMA, wcdma_interface_ctor},
  {CLSID_WCDMA_MMCP, wcdma_interface_mmcp_ctor},
  {CLSID_WCDMA_RFTECH, wcdma_interface_rftech_ctor},
  {CLSID_WCDMA_QCHAT, wcdma_interface_qchat_ctor}
};
/*******************************************/

/*------------------------------------------------------------------*
 * symbols defined in linker script, representing segment boundries.* 
 *------------------------------------------------------------------*/
extern char RECLAIM_TDSCDMA_startaddr[];
extern char RECLAIM_TDSCDMA_endaddr[];
extern char RECLAIM_WCDMA_startaddr[];
extern char RECLAIM_WCDMA_endaddr[];

extern char __segment_loading_starting__; 
extern char __segment_loading_text_tdscdma_start__;
extern char __segment_loading_bss_tdscdma_start__;
extern char __segment_loading_text_wcdma_start__;
extern char __segment_loading_bss_wcdma_start__;
extern char __segment_loading_ending__;

/*------------------------------------------------------------------*
 * Temprorary featurization till DI.3.0 and DPM branches.* 
 *------------------------------------------------------------------*/
extern char __segment_loading_bss_tdscdma_end__;
extern char __segment_loading_bss_wcdma_end__;

/*------------------------------------------------------------------*
 * struct declared for segment loading configuration datatype       *
 * stored in EFS file.                                              *
 *------------------------------------------------------------------*/
extern mem_heap_type modem_mem_heap;

extern qurt_mem_pool_t qurt_mem_default_pool;

#ifdef FEATURE_BORROW_FROM_AMSS

#define BUF_FROM_AMSS_SIZE 2*1024*1024
/*------------------------------------------------------------------*
 * external linkage to AMSS API malloc() from amssheap.c            *
 *------------------------------------------------------------------*/
#ifdef AMSSHEAP_UNIT_TEST
extern void *
  malloc_ut
  (
    size_t size
  );
#else /*AMSSHEAP_UNIT_TEST*/
extern void *
  malloc
  (
    size_t size
  );
#endif /*AMSSHEAP_UNIT_TEST*/

#endif /*FEATURE_BORROW_FROM_AMSS*/

#ifdef FEATURE_BORROW_FROM_HLOS
#define BUF_FROM_HLOS_SIZE 3*1024*1024
uint32 hlos_mem_borrow_result;
boolean RECLAIM_FROM_HLOS = FALSE;
#endif /*FEATURE_BORROW_FROM_HLOS*/

/*------------------------------------------------------------------*
 * external linkage to kernel API mem_heap_add_section()            *
 *------------------------------------------------------------------*/
extern void 
  mem_heap_add_section
  (
     mem_heap_type *heap_ptr, 
     void *section_start, 
     unsigned long section_size
  );

/*------------------------------------------------------------------*
 * global link symbol table containing link symbol defined          *
 * in linker script.                                                *
 *------------------------------------------------------------------*/
char * QURTK_reclaim_areas[] = 
{
   RECLAIM_TDSCDMA_startaddr,
   RECLAIM_TDSCDMA_endaddr,
   RECLAIM_WCDMA_startaddr,
   RECLAIM_WCDMA_endaddr,
   NULL                              /* Terminate table with NULL pointer */
};

/*------------------------------------------------------------------*
 * Segment loading configuration information data type.             *
 *------------------------------------------------------------------*/
typedef struct
{
  int ver;
  int val;
}segload_config_t;


/*------------------------------------------------------------------*
 * local data types to access EFS file for query and setting        *
 * segment loading configuration information.                       *
 *------------------------------------------------------------------*/
static segload_config_t segload_config;//data ptr storing EFS file item contents.

static void seg_load_release_inactive_mode(int curr_boot_up_config);

/*===========================================================================

FUNCTION segload_is_normal_mode

DESCRIPTION
  This local function tells if current mode is FTM. 

DEPENDENCIES
  None. 

RETURN VALUE
  True  --> non-FTM mode. 
  False --> FTM mode or offline mode. 

SIDE EFFECTS
  None.

===========================================================================*/
static boolean segload_is_normal_mode(void)
{
  //only when FTM_NV == 0, it's non-FTM mode. 
  //return TRUE when it's non-FTM mode. 
  //nv_items_enum_type ftm_nv = NV_FTM_MODE_I;
  mcfg_nv_status_e_type ret = MCFG_NV_STATUS_NOT_ACTIVE;
  byte byFTM_NV = 1;

  ret = mcfg_nv_read(NV_FTM_MODE_I, 0, 0, (void *)&byFTM_NV, sizeof(byte));
  if (ret == MCFG_NV_STATUS_OK && byFTM_NV == 0)
  {
      return TRUE;
  }
  return FALSE;
}

/*===========================================================================

FUNCTION segload_config_set

DESCRIPTION
  This local function sets configuration information in EFS for segment
  loading. 

DEPENDENCIES
  None. 

RETURN VALUE
  MCFG_SEGLOAD_SUCCESS      --> Succesfully set segment loading 
                                configuration in EFS. 
  MCFG_SEGLOAD_EFS_SET_FAIL --> Failed to set segment loading 
                                configuration in EFS.  

SIDE EFFECTS
  None.

===========================================================================*/
static mcfg_segload_status_e_type segload_config_set(const int new_boot_option)
{
  //edge case as illegal pass-in param. 
  if(new_boot_option != MCFG_SEGMENT_EXCLUDE_WCDMA && new_boot_option != MCFG_SEGMENT_EXCLUDE_TDSCDMA && new_boot_option != MCFG_SEGMENT_LOAD_ALL)
  {
    return MCFG_SEGLOAD_EFS_SET_FAIL;
  }

  segload_config.ver = 1;
  segload_config.val = new_boot_option;
  if(mcfg_nv_write(MCFG_NV_ITEM_SEGLOAD_CONFIG_I, 0, 0, (void *)(&segload_config), sizeof(segload_config_t)) != MCFG_NV_STATUS_OK)
  {
    return MCFG_SEGLOAD_EFS_SET_FAIL;
  }
  
  return MCFG_SEGLOAD_SUCCESS;//return 0 if config info is set. 
}

/*===========================================================================

FUNCTION segload_config_query

DESCRIPTION
  This local function queries configuration information in EFS for segment
  loading. 

DEPENDENCIES
  None. 

RETURN VALUE
  MCFG_SEGLOAD_SUCCESS          --> Succesfully query segment loading configuration in EFS.
  MCFG_SEGLOAD_EFS_QUERY_FAIL   --> Failed to query segment loading configuration in EFS. 

SIDE EFFECTS
  None.

===========================================================================*/
static mcfg_segload_status_e_type segload_config_query(segload_config_t *segload_config)
{
  if(mcfg_nv_read(MCFG_NV_ITEM_SEGLOAD_CONFIG_I, 0, 0, (void *)segload_config, sizeof(segload_config_t)) != MCFG_NV_STATUS_OK)
  {
    /* Error handling -  cannot read efs item from file */
    return MCFG_SEGLOAD_EFS_QUERY_FAIL;
  }
  if(segload_config->val != MCFG_SEGMENT_LOAD_ALL && segload_config->val != MCFG_SEGMENT_EXCLUDE_WCDMA && segload_config->val != MCFG_SEGMENT_EXCLUDE_TDSCDMA)
  {
    /* Error handling -  cannot read correct contents from efs item */
    return MCFG_SEGLOAD_EFS_QUERY_FAIL;
  }
  return MCFG_SEGLOAD_SUCCESS;
}

/*===========================================================================

FUNCTION segload_remap_unloaded_segment

DESCRIPTION
  This local function remap unloaded segment area to RW region before 
  reclaimed by heap API.

DEPENDENCIES
  None. 

RETURN VALUE
  MCFG_SEGLOAD_SUCCESS   --> Succesfully remapped unloaded memory region.
  MCFG_SEGLOAD_REMAP_UNLOADED_SEGMENT_FAIL   
                         --> Failed to remap unloaded memory region.

SIDE EFFECTS
  None.

===========================================================================*/

static mcfg_segload_status_e_type segload_remap_unloaded_segment(qurt_addr_t *pvaddr, qurt_size_t size, qurt_mem_pool_t pool)
{
  qurt_mem_region_attr_t attr;
  qurt_mem_region_t region;
  int ret;

  *pvaddr = 0;
  qurt_mem_region_attr_init (&attr);
  ret = qurt_mem_region_create (&region, size, pool, &attr);
  if(ret != QURT_EOK)
  {
    return MCFG_SEGLOAD_REMAP_UNLOADED_SEGMENT_FAIL;
  }

  if(qurt_mem_region_attr_get(region, &attr) != QURT_EOK)                // Query the region attributes
  {
    return MCFG_SEGLOAD_REMAP_UNLOADED_SEGMENT_FAIL;
  }
  qurt_mem_region_attr_get_virtaddr(&attr, pvaddr);

  return MCFG_SEGLOAD_SUCCESS;
}

/*=============================================================================================*
FUNCTION mcfg_segment_f3_timer_cb

DESCRIPTION
  This call_back function print out F3 messages to indicate currently loaded segment for MST 
  testing usage. 

DEPENDENCIES
  None. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
 *=============================================================================================*/
static void mcfg_segment_f3_timer_cb(timer_cb_data_type data)
{
  if(curr_boot_up == MCFG_SEGMENT_LOAD_ALL)
  {
    MCFG_MSG_HIGH("Segment Loading - LOADED both WCDMA and TDSCDMA segments.");
  }
  else if(curr_boot_up == MCFG_SEGMENT_EXCLUDE_TDSCDMA)
  {
    MCFG_MSG_HIGH("Segment Loading - LOADED WCDMA segment, while TDSCDMA is UNLOADED.");
  }
  else if(curr_boot_up == MCFG_SEGMENT_EXCLUDE_WCDMA)
  {
    MCFG_MSG_HIGH("Segment Loading - LOADED TDSCDMA segment, while WCDMA is UNLOADED.");
}
  else//edge case: should not be here unless EFS corruption. 
  {
    MCFG_MSG_HIGH("Segment Loading - Invalid Segment Loading, no way to find out what's currently loaded segment. Please check NV72542 for details.");
  }
  return;
} /* mcfg_segment_f3_timer_cb() */


/*=============================================================================================*
FUNCTION segload_timer_init

DESCRIPTION
  This init function periodically tigger mcfg_segment_f3_timer_cb() to print out F3 messages. 

DEPENDENCIES
  None. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.
 *=============================================================================================*/
static void segload_timer_init(void)
{
  timer_def(&mcfg_segment_f3_timer,
            &mcfg_segment_f3_timer_group,
            NULL,
            0,
            mcfg_segment_f3_timer_cb,
            0);

  timer_set(&mcfg_segment_f3_timer, 
            MCFG_SEGMENT_F3_TIMER_FIRST_TIMEOUT,
            MCFG_SEGMENT_F3_TIMER_PERIOD_TIMEOUT, 
            T_MSEC);
} /* segload_timer_init */

/*=============================================================================================*
FUNCTION segload_is_f3_active

DESCRIPTION
  This fucntion checks if segload_f3_config file is present in efs or not. if yes it enables F3 msg. 

DEPENDENCIES
  None. 

RETURN VALUE
  True if efs file is present.
  False if efs file dosent exitst.

SIDE EFFECTS
  None.
 *=============================================================================================*/

boolean segload_is_f3_active(void)
{
  char *path = "/nv/item_files/mcfg/segload_f3_config";
  int efs_ret;

   efs_ret =efs_open(path, O_RDONLY);
    
    if (efs_ret < 0)
     {
         f3_config = FALSE ; /*EFS file not found return FALSE*/
         return f3_config;
     }
    else
     {
          f3_config = TRUE ;  /*EFS file  found return TRUE*/
          return f3_config;
     }
}


/*===========================================================================

FUNCTION segload_config_handler

DESCRIPTION
  This function handles segment loading configuration, as it sets default
  configuration if not existed, and reclaims unloaded segments according
  to configuration value after booting up. 

DEPENDENCIES
  None. 

RETURN VALUE
  MCFG_SEGLOAD_SUCCESS               --> Segment loading done successfully. 
  MCFG_SEGLOAD_IS_FTM_OFFLINE_MODE   --> Segment loading not performed due to in FTM mode. 
  MCFG_SEGLOAD_EFS_QUERY_FAIL        --> Segment loading failed due to configuration query failure. 
  MCFG_SEGLOAD_EFS_SET_FAIL          --> Segment loading failed due to configuration set failure. 
  MCFG_SEGLOAD_RECLAIM_TLB_FAIL      --> Segment loading failed due to TLB reclaiming failure. 
  MCFG_SEGLOAD_REMAP_UNLOADED_SEGMENT_FAIL    --> Segment loading failed due to unloaded region 
                                                  remap failure. 
  MCFG_SEGLOAD_LOAD_FULL_IMAGE       --> Segment loading aborted when full image should be loaded. 
  MCFG_SEGLOAD_UPDATE_MMU_TRANSLATION_FAIL    --> Segment loading MMU table updates failed due to 
                                                  kernel cannot find PA associated with given VA range. 
  MCFG_SEGLOAD_UPDATE_MMU_ENTRY_OVERFLOW_FAIL --> Segment loading MMU table updates failed due to 
                                                  heap allocation uses small chunk size and returns 
                                                  too many sections therefore. 
SIDE EFFECTS
  None.

===========================================================================*/
mcfg_segload_status_e_type segload_config_handler(void)
{
//  int curr_boot_up = 0;
#ifdef FEATURE_BORROW_FROM_HLOS
  dynamic_mem_client_id client_id = DYNAMIC_MEM_CLIENT_ID_FTM ;
  dynamic_mem_attribute attr = DYNAMIC_MEM_ATTRIBUTE_CMA;
  void *alloc_ptr= NULL;
#endif /* FEATURE_BORROW_FROM_HLOS */
  int status;
#ifdef FEATURE_BORROW_FROM_AMSS
  void* buf_from_amss = NULL;//memory chunk borrowed from amss and will put into modem_mem_heap in FTM mode.
#endif /*FEATURE_BORROW_FROM_AMSS*/

  qurt_addr_t remap_vaddr = 0;
  qurt_size_t heap_reclaim_size = 0;

  if(segload_is_normal_mode() == FALSE)//offline or FTM mode, exit directly. 
  {
    #ifdef FEATURE_BORROW_FROM_HLOS
    /*for borrowing memory durng FTM mode from HLOS*/  
    hlos_mem_borrow_result = dynamic_heap_g_init(client_id, attr, (size_t)BUF_FROM_HLOS_SIZE, NULL, &alloc_ptr);

    if(hlos_mem_borrow_result != 0)
    {
	  MCFG_MSG_HIGH("Segment Loading - Memory borrowing FROM HLOS FAILED.");
    }
    else
    {
	  mem_heap_add_section(&modem_mem_heap, alloc_ptr, (unsigned long)BUF_FROM_HLOS_SIZE);
	  RECLAIM_FROM_HLOS = TRUE;
    }
    #endif /*FEATURE_BORROW_FROM_HLOS*/
	  
    #ifdef FEATURE_BORROW_FROM_AMSS

    #ifdef AMSSHEAP_UNIT_TEST
      buf_from_amss = malloc_ut((size_t)BUF_FROM_AMSS_SIZE);
    #else  /*AMSSHEAP_UNIT_TEST*/
      buf_from_amss = malloc((size_t)BUF_FROM_AMSS_SIZE);
    #endif /*AMSSHEAP_UNIT_TEST*/
     if(!buf_from_amss)
     {
      mcfg_seg_last_error = MCFG_SEGLOAD_UPDATE_HEAP_FROM_AMSS_MALLOC_FAIL;
     }
     else 
     {
      //mcfg_seg_last_error = segload_update_heap_from_AMSS((qurt_addr_t)buf_from_amss, ((qurt_addr_t)buf_from_amss + BUF_FROM_AMSS_SIZE));
      mem_heap_add_section(&modem_mem_heap, buf_from_amss, (unsigned long)BUF_FROM_AMSS_SIZE);
      mcfg_seg_last_error = MCFG_SEGLOAD_IS_FTM_OFFLINE_MODE;
      }
      return mcfg_seg_last_error;
      #else /*FEATURE_BORROW_FROM_AMSS*/  
    mcfg_seg_last_error = MCFG_SEGLOAD_IS_FTM_OFFLINE_MODE;
    return mcfg_seg_last_error;
      #endif /*FEATURE_BORROW_FROM_AMSS*/

  }

/* Initilize DSM POOL*/
#ifdef FEATURE_DSM_TRIGGER_EXTERNAL_MEMORY_INIT

      dsm_trigger_external_memory_init();
 
#endif /*FEATURE_DSM_TRIGGER_EXTERNAL_MEMORY_INIT*/


  if(segload_config_query(&segload_config) != MCFG_SEGLOAD_SUCCESS)//EFS reads fail, create default config. 
  {
   #ifdef FEATURE_LOAD_ALL_DEFAULT_CONFIG
    if(segload_config_set(MCFG_SEGMENT_LOAD_ALL) != MCFG_SEGLOAD_SUCCESS)
    {
      mcfg_seg_last_error = MCFG_SEGLOAD_EFS_SET_FAIL;
      return mcfg_seg_last_error;
    }
   #else 
    if(segload_config_set(MCFG_SEGMENT_EXCLUDE_TDSCDMA) != MCFG_SEGLOAD_SUCCESS)
    {
      mcfg_seg_last_error = MCFG_SEGLOAD_EFS_SET_FAIL;
      return mcfg_seg_last_error;
    }
   #endif
    if(segload_config_query(&segload_config) != MCFG_SEGLOAD_SUCCESS)
    {
      mcfg_seg_last_error = MCFG_SEGLOAD_EFS_QUERY_FAIL;
      return mcfg_seg_last_error;
    }
  }
   
  curr_boot_up = segload_config.val;

  //start printing F3 messages to indicate currently loaded segment info. 
  if(segload_is_f3_active() == TRUE)
  {
  segload_timer_init();
  }

  if(curr_boot_up == MCFG_SEGMENT_LOAD_ALL)
  {
    #ifdef FEATURE_SEGMENT_LOADING_SUPPORT_LOAD_ALL_RATS
    mcfg_seg_last_error = MCFG_SEGLOAD_LOAD_ALL_TECHNOLOGY;
    return mcfg_seg_last_error;
    #else /*FEATURE_SEGMENT_LOADING_SUPPORT_LOAD_ALL_RATS*/
     if(segload_config_set(MCFG_SEGMENT_EXCLUDE_TDSCDMA) != MCFG_SEGLOAD_SUCCESS)
     {
        mcfg_seg_last_error = MCFG_SEGLOAD_EFS_SET_FAIL_AND_SHOULD_NOT_SUPPORT_ALL_RATS_MODE;
        return mcfg_seg_last_error;
     }
       curr_boot_up = MCFG_SEGMENT_EXCLUDE_TDSCDMA;
    #endif /*FEATURE_SEGMENT_LOADING_SUPPORT_LOAD_ALL_RATS*/
  }

  if(curr_boot_up == MCFG_SEGMENT_EXCLUDE_TDSCDMA)
  {
    //reinitialize WCDMA BSS segment. 
    //memset((void *)&__segment_loading_bss_wcdma_start__, 0, (&__segment_loading_bss_wcdma_end__  - &__segment_loading_bss_wcdma_start__));
    heap_reclaim_size = (qurt_size_t)(RECLAIM_TDSCDMA_endaddr - RECLAIM_TDSCDMA_startaddr);
    //directly add TDSCDMA  memory region back to heap. 
    mem_heap_add_section(&modem_mem_heap, (void *)&__segment_loading_bss_tdscdma_start__, (unsigned long)(&__segment_loading_bss_tdscdma_end__ - &__segment_loading_bss_tdscdma_start__));
    //reclaim TDSCDMA code segment. 
    status = qurt_mapping_reclaim((qurt_addr_t)RECLAIM_TDSCDMA_startaddr, heap_reclaim_size, qurt_mem_default_pool);
    if (status != 0)
    {
      mcfg_seg_last_error = MCFG_SEGLOAD_RECLAIM_TLB_FAIL;
      return mcfg_seg_last_error;
    }
  }
  else if(curr_boot_up == MCFG_SEGMENT_EXCLUDE_WCDMA)
  {
    //reinitialize TDSCDMA BSS segment. 
    //memset((void *)&__segment_loading_bss_tdscdma_start__, 0, (&__segment_loading_bss_tdscdma_end__  - &__segment_loading_bss_tdscdma_start__));
    heap_reclaim_size = (qurt_size_t)(RECLAIM_WCDMA_endaddr - RECLAIM_WCDMA_startaddr);
    //directly add WCDMA bss memory region back to heap. 
    mem_heap_add_section(&modem_mem_heap, (void *)&__segment_loading_bss_wcdma_start__, (unsigned long)(&__segment_loading_bss_wcdma_end__ - &__segment_loading_bss_wcdma_start__));
    //reclaim WCDMA code segment. 
    status = qurt_mapping_reclaim((qurt_addr_t)RECLAIM_WCDMA_startaddr, heap_reclaim_size, qurt_mem_default_pool);
    if (status != 0)
    {
      mcfg_seg_last_error = MCFG_SEGLOAD_RECLAIM_TLB_FAIL;
      return mcfg_seg_last_error;
    }
  }

  seg_load_release_inactive_mode(curr_boot_up);//release the unloaded interface instance. 

  if(segload_remap_unloaded_segment(&remap_vaddr, heap_reclaim_size, qurt_mem_default_pool) != MCFG_SEGLOAD_SUCCESS)
  {
    mcfg_seg_last_error = MCFG_SEGLOAD_REMAP_UNLOADED_SEGMENT_FAIL;
    return mcfg_seg_last_error;
  }

  //add reclaimed sections back to heap there instead. 
  mem_heap_add_section(&modem_mem_heap, (void *)remap_vaddr, (unsigned long)heap_reclaim_size);

    return mcfg_seg_last_error;
  }

/*==================================================================*/

/*-------- create_instance --------*/
/**
@brief  Get a pointer to the interface for a component.

@param[in]  clsid     The class ID for the component.
@param[out] ppIface   Pointer to storage for a pointer to the interface returned.

@return
  - E_SUCCESS if the requested component is available and supports the requested
    interface.
  - E_NOT_AVAILABLE if the requested component is not available or does not
    support the requested interface.
*/
IxErrnoType
create_instance(
  clsid_t     clsid,
  interface_t **ppIface
  )
{
  int   i;
  void  *pi = NULL;

  for (i = 0 ; i < ARR_SIZE(components) ; ++i)
  {
    if (components[i].clsid == clsid && components[i].pGetIfaceFn != NULL)
    {
      pi = components[i].pGetIfaceFn();
    }
  }

  *ppIface = pi;
  return (pi != NULL)? E_SUCCESS : E_NOT_AVAILABLE;
}


/*-------- release_instance --------*/
/**
@brief  FOR USE OF SEGMENT LOADER ONLY

        This NULLs the pointer to a component in the static tables that
        describe the available components.  It is used by the segment loader
        to remove entries that have not been loaded.

@param[in]  clsid   The class ID the component to be released.

@return
  - N/A
*/
void
release_instance(
  clsid_t clsid
  )
{
  int i;

  for (i = 0 ; i < ARR_SIZE(components) ; ++i)
  {
    if (components[i].clsid == clsid)
    {
      components[i].pGetIfaceFn = NULL;
      break;
    }
  }
}


/********************************************************
 FUNCTION: mcfg_segment_reset(void)
 To trigger modem reset for segment switch.
 
 @param[in]
  - ResetParam: no indication for now.  
 
 @return
  - N/A
 ********************************************************/
void mcfg_segment_reset(void)
{
  /* Print out old value outside sbss section */
  MCFG_MSG_HIGH("mcfg_segment_reset.");

  //mcfg_utils_reset_modem(SSREQ_QMI_RES_SEGMENT_LOADING);
  mcfg_utils_cmd_send(MCFG_SEGLOAD_RESET);
}


/********************************************************
 FUNCTION: seg_load_release_inactive_mode
 To release unused segment interface pointer according
 to current loaded segment information.
 
 @param[in]
  - N/A.  
 
 @return
  - N/A.
 ********************************************************/
static void seg_load_release_inactive_mode(int curr_config)
{
  if(curr_config == MCFG_SEGMENT_EXCLUDE_TDSCDMA)
  {
    release_instance(CLSID_TDSCDMA);
  }
  else if(curr_config == MCFG_SEGMENT_EXCLUDE_WCDMA)
  {
    release_instance(CLSID_WCDMA);
    release_instance(CLSID_WCDMA_MMCP);
    release_instance(CLSID_WCDMA_RFTECH);
    release_instance(CLSID_WCDMA_QCHAT);
  }
  return;
}

/********************************************************
 FUNCTION: seg_load_init(void)
 Init function to call seg_load_config_handler().
 
 @param[in]
  - N/A.  
 
 @return
  - N/A.
 ********************************************************/
void seg_load_init(void)
{
  if(segload_config_handler() != MCFG_SEGLOAD_SUCCESS)
  {
    MCFG_MSG_HIGH("Modem Config Segment Loading Failed or Aborted.");
  }
  return;
}

/*===========================================================================

FUNCTION get_wcdma_interface

DESCRIPTION
  This function returns the interface to the WCDMA Access Stratum segment.
  If the segment is not loaded, it will return NULL.

DEPENDENCIES
  None

RETURN VALUE
  NULL - WCDMA segment is NOT loaded. 
  else - WCDMA segment is LOADED.

SIDE EFFECTS
  None

===========================================================================*/
interface_t *get_wcdma_interface(void)
{
  static atomic_word_t  iwcdma_ptr = {-1};

  IxErrnoType status = E_FAILURE;
  static boolean flag = FALSE;
 
  /*---------------------------------------------------------------------------
    This is a standard singleton pattern.  We first see if the value has
    already been set.  If so, we're done and return the value.

    If it hasn't been set, we create the desired resource (in this case an
    interface instance).  If we don't get the interface, we set the pointer
    to NULL and we're done.  If we do, we then do an atomic operation that
    writes this pointer to the storage *only* if the value is still uninitialized.
    If that fails, it's because another thread initialized it while we were
    working and we just free the (now unnecessary) interface pointer we just got.
  ---------------------------------------------------------------------------*/
 
  if (flag == FALSE)
  {
    flag = TRUE;
    iwcdma_ptr.value = -1;
  }
  
  if (iwcdma_ptr.value == -1)
  {
    interface_t *pi;

    // Interface pointer isn't set - get the interface pointer.
    status = create_instance(CLSID_WCDMA, &pi);

    if (status != E_SUCCESS)
    {
      // No interface - set the interface to NULL
      atomic_set(&iwcdma_ptr, NULL);     
    }
    else if (!atomic_compare_and_set(&iwcdma_ptr, -1, (atomic_plain_word_t) pi))
    {
      // Value in interface pointer is no longer uninitialized - it must have
      // been set on another thread.  Release the interface we just got.
      ref_cnt_obj_release(pi);
    }
}

  return (interface_t *) iwcdma_ptr.value;
}

/*===========================================================================

FUNCTION get_wcdma_mmcp_interface

DESCRIPTION
  This function returns the interface to the WCDMA Access Stratum segment 
  for MMCP to use only.
  If the segment is not loaded, it will return NULL.

DEPENDENCIES
  None

RETURN VALUE
  NULL - WCDMA segment is NOT loaded. 
  else - WCDMA segment is LOADED. 

SIDE EFFECTS
  None

===========================================================================*/

interface_t *get_wcdma_mmcp_interface(void)
{
  static atomic_word_t  iwcdma_mmcp_ptr = {-1};

  IxErrnoType status = E_FAILURE;
  static boolean flag = FALSE;

  /*---------------------------------------------------------------------------
    This is a standard singleton pattern.  We first see if the value has
    already been set.  If so, we're done and return the value.

    If it hasn't been set, we create the desired resource (in this case an
    interface instance).  If we don't get the interface, we set the pointer
    to NULL and we're done.  If we do, we then do an atomic operation that
    writes this pointer to the storage *only* if the value is still uninitialized.
    If that fails, it's because another thread initialized it while we were
    working and we just free the (now unnecessary) interface pointer we just got.
  ---------------------------------------------------------------------------*/
 
  if (flag == FALSE)
  {
    flag = TRUE;
    iwcdma_mmcp_ptr.value = -1;
  }
  
  if (iwcdma_mmcp_ptr.value == -1)
  {
    interface_t *pi;

    // Interface pointer isn't set - get the interface pointer.
    status = create_instance(CLSID_WCDMA_MMCP, &pi);

    if (status != E_SUCCESS)
    {
      // No interface - set the interface to NULL
      atomic_set(&iwcdma_mmcp_ptr, NULL);     
    }
    else if (!atomic_compare_and_set(&iwcdma_mmcp_ptr, -1, (atomic_plain_word_t) pi))
    {
      // Value in interface pointer is no longer uninitialized - it must have
      // been set on another thread.  Release the interface we just got.
      ref_cnt_obj_release(pi);
    }
}

  return (interface_t *) iwcdma_mmcp_ptr.value;
}

/*===========================================================================

FUNCTION get_tdscdma_interface

DESCRIPTION
    This function returns the interface to the TDSDMA Access Stratum segment.
    If the segment is not loaded, it will return NULL.

DEPENDENCIES
  None

RETURN VALUE
  NULL - TDSCDMA segment is NOT loaded. 
  else - TDSCDMA segment is LOADED.

SIDE EFFECTS
  None

===========================================================================*/
interface_t *get_tdscdma_interface(void)
{
    static atomic_word_t  itdscdma_ptr = {-1};

    IxErrnoType status = E_FAILURE;

    static boolean flag = FALSE;
  /*---------------------------------------------------------------------------
    This is a standard singleton pattern.  We first see if the value has
    already been set.  If so, we're done and return the value.

    If it hasn't been set, we create the desired resource (in this case an
    interface instance).  If we don't get the interface, we set the pointer
    to NULL and we're done.  If we do, we then do an atomic operation that
    writes this pointer to the storage *only* if the value is still uninitialized.
    If that fails, it's because another thread initialized it while we were
    working and we just free the (now unnecessary) interface pointer we just got.
  ---------------------------------------------------------------------------*/
    if (flag == FALSE)
    {
     flag = TRUE;
     itdscdma_ptr.value = -1;
    }

  if (itdscdma_ptr.value == -1)
  {
    interface_t *pi;
    // Interface pointer isn't set - get the interface pointer.
    status = create_instance(CLSID_TDSCDMA, &pi);

    if (status != E_SUCCESS)
    {
      // No interface - set the interface to NULL
      atomic_set(&itdscdma_ptr, NULL); 
    }
    else if (!atomic_compare_and_set(&itdscdma_ptr, -1, (atomic_plain_word_t) pi))
    {
      // Value in interface pointer is no longer uninitialized - it must have
      // been set on another thread.  Release the interface we just got.
      ref_cnt_obj_release(pi);
    }
  }
  return (interface_t *) itdscdma_ptr.value;
}

/******************************************************************
 FUNCTION: mcfg_segment_exclude_technologies(uint32  technologies)
 New API for MMCP system selection to switch segment, for segment 
 loading only. 
 
 @param[in]
  - technologies ==> Const indicating target segment to unload.
 
 @return
  - -1 ==> request denied. 
  -  0 ==> good to go. 
 *******************************************************************/
int mcfg_segment_exclude_technologies(uint32  technologies)
{
  if(technologies != MCFG_SEGMENT_EXCLUDE_WCDMA && technologies != MCFG_SEGMENT_EXCLUDE_TDSCDMA)
  {
    MCFG_MSG_HIGH("Segload - Receive invalid request, will stay in current mode.");
    return -1;
  }
  if(technologies == MCFG_SEGMENT_EXCLUDE_WCDMA) //want to switch to TDS mode.
  {
    MCFG_MSG_HIGH("Segload - Receive request to switch to non-WCDMA mode.");
    if(create_instance(CLSID_TDSCDMA, &pi_tdscdma)==E_SUCCESS) //currently already in TDS mode.
    {
      MCFG_MSG_HIGH("Segload - Currently in non-WCDMA mode, will stay in current mode.");
      return -1;
    }
    else //currently in non-TDS mode.
    {
      MCFG_MSG_HIGH("Segload - Currently in non-TDS mode, will switch to non-WCDMA mode.");
      segload_config_set(MCFG_SEGMENT_EXCLUDE_WCDMA);
      mcfg_segment_reset();
      return 0;
    }
  }
  else //want to switch to WCDMA mode.
  {
    MCFG_MSG_HIGH("Segload - Receive request to switch to non-TDS mode.");
    if(create_instance(CLSID_WCDMA, &pi_wcdma)==E_SUCCESS) //currently already in WCDMA mode.
    {
      MCFG_MSG_HIGH("Segload - Currently in non-TDS mode, will stay in current mode.");
      return -1;
    }
    else //currently in TDS mode.
    {
      MCFG_MSG_HIGH("Segload - Currently in non-WCDMA mode, will switch to non-TDS mode.");
      segload_config_set(MCFG_SEGMENT_EXCLUDE_TDSCDMA);
      mcfg_segment_reset();
      return 0;
    }
  }
}

#endif /* FEATURE_SEGMENT_LOADING */
