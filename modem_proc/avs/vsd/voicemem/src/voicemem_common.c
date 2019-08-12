/*
  Copyright (C) 2009-2016 QUALCOMM Technologies Inc.
  All rights reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/avs/vsd/voicemem/src/voicemem_common.c#1 $
  $Author: mplcsds1 $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmstd.h"
#include "msg.h"
#include "err.h"
#ifdef WINSIM
  #include <stdlib.h>
#endif
#include "apr_errcodes.h"
#include "apr_memmgr.h"
#include "apr_objmgr.h"
#include "apr_lock.h"
#ifndef WINSIM
#include "smem.h"
#include "qurt.h"
#endif
#include "voicemem.h"

/****************************************************************************
 * DEFINES                                                                  *
 ****************************************************************************/

/* Size of the reserved SMEM_VOICE region. */
#define VOICEMEM_SMEM_HEAP_SIZE ( 81920 )

/* Flag to indicate SMEM to allocate cached memory. */
#define VOICEMEM_SMEM_ALLOC_FLAG_CACHED ( 0x80000000 )

/* The first 8 KB of the SMEM_VOICE region are reserved for use by ECall.
 * Therefore the SMEM heap available for VOICEMEM purposes starts at 8 KB and
 * only 72 KB are available. */
#define VOICEMEM_SMEM_HEAP_OFFSET ( 8192 )

/* Size of VOICEMEM internal book-keeping heap. */
#define VOICEMEM_INTERNAL_HEAP_SIZE_V ( 2048 )

#define VOICEMEM_HANDLE_TOTAL_BITS_V ( 16 )
#define VOICEMEM_HANDLE_INDEX_BITS_V ( 5 ) /* 5 bits = 32 handles. */
#define VOICEMEM_MAX_OBJECTS_V ( 1 << VOICEMEM_HANDLE_INDEX_BITS_V )

/*****************************************************************************
 * DEFINITIONS                                                               *
 ****************************************************************************/

typedef struct voicemem_region_object_t
{
  uint32_t handle;
  void* base_virt_addr;
  uint32_t size;
  uint8_t client_name[31];
}
  voicemem_region_object_t;
  uint64_t mem_page[20];
  uint8_t mem_page_usage[20];
  uint8_t *voicemem_smem_heap_phy_addr;

/****************************************************************************
 * VARIABLE DECLARATIONS                                                    *
 ****************************************************************************/

/* Object manager lock. */
static apr_lock_t voicemem_objmgr_lock;

/* VOICEMEM internal book-keeping heap lock. */
static apr_lock_t voicemem_internal_heap_lock;

/* VOICEMEM SMEM heap lock. */
static apr_lock_t voicemem_smem_heap_lock;

/* Object manager. */
static apr_objmgr_object_t voicemem_object_table[ VOICEMEM_MAX_OBJECTS_V ];
static apr_objmgr_t voicemem_objmgr;

/* VOICEMEM internal book-keeping heap manager. */
static uint8_t voicemem_internal_heap_pool[ VOICEMEM_INTERNAL_HEAP_SIZE_V ];
static apr_memmgr_type voicemem_internal_heapmgr;

/* SMEM heap manager. */
static void* voicemem_smem_heap_virt_addr;

static bool_t voicemem_initialized = FALSE;

/****************************************************************************
 * HELPER FUNCTIONS                                                         *
 ****************************************************************************/

VSD_PRIVATE void voicemem_objmgr_lock_fn ( void )
{
  ( void ) apr_lock_enter( voicemem_objmgr_lock );
}


VSD_PRIVATE void voicemem_objmgr_unlock_fn ( void )
{
  ( void ) apr_lock_leave( voicemem_objmgr_lock );
}


VSD_PRIVATE void voicemem_internal_heap_lock_fn ( void* heap_ptr )
{
  ( void ) apr_lock_enter( voicemem_internal_heap_lock );
}


VSD_PRIVATE void voicemem_internal_heap_unlock_fn ( void* heap_ptr )
{
  ( void ) apr_lock_leave( voicemem_internal_heap_lock );
}


VSD_PRIVATE void voicemem_smem_heap_lock_fn ( void* heap_ptr )
{
  ( void ) apr_lock_enter( voicemem_smem_heap_lock );
}


VSD_PRIVATE void voicemem_smem_heap_unlock_fn ( void* heap_ptr )
{
  ( void ) apr_lock_leave( voicemem_smem_heap_lock );
}


VSD_PRIVATE uint32_t voicemem_create_region_object (
  voicemem_region_object_t** ret_region_obj
)
{
  uint32_t rc;
  uint32_t checkpoint = 0;
  voicemem_region_object_t* region_obj;
  apr_objmgr_object_t* objmgr_obj;

  for ( ;; )
  {
    if ( ret_region_obj == NULL )
    {
      rc = VSD_EBADPARAM;
      break;
    }

    { /* Allocate memory for the new region object. */
      region_obj = apr_memmgr_malloc( &voicemem_internal_heapmgr,
                                      sizeof( voicemem_region_object_t ) );
      if ( region_obj == NULL )
      {
        rc = VSD_ENORESOURCE;
        break;
      }
      checkpoint = 1;
    }

    { /* Allocate a new handle for the region object. */
      rc = apr_objmgr_alloc_object( &voicemem_objmgr, &objmgr_obj );
      if ( rc ) break;
      checkpoint = 2;
    }

    /* Link the region object to the handle */
    objmgr_obj->any.ptr = region_obj;

    { /* Initialize the region object. */
      mmstd_memset( region_obj, 0, sizeof( voicemem_region_object_t ) );
      region_obj->handle = objmgr_obj->handle;
    }

    *ret_region_obj = region_obj;

    return APR_EOK;
  }

  switch ( checkpoint )
  {
  case 2:
    ( void ) apr_objmgr_free_object( &voicemem_objmgr, objmgr_obj->handle );
    /*-fallthru */

  case 1:
    apr_memmgr_free( &voicemem_internal_heapmgr, region_obj );
    /*-fallthru */

  default:
    break;
  }

  return rc;
}


VSD_PRIVATE uint32_t voicemem_destroy_region_object (
  voicemem_region_object_t* region_obj
)
{
  if ( region_obj == NULL )
  {
    return VSD_EBADPARAM;
  }

  /* Free the object memory and object handle. */
  ( void ) apr_objmgr_free_object( &voicemem_objmgr, region_obj->handle );
  apr_memmgr_free( &voicemem_internal_heapmgr, region_obj );

  return VSD_EOK;
}


VSD_PRIVATE uint32_t voicemem_get_region_object (
  uint32_t handle,
  voicemem_region_object_t** ret_region_obj
)
{
  uint32_t rc;
  apr_objmgr_object_t* objmgr_obj;
  voicemem_region_object_t* region_obj;

  for ( ;; )
  {
    if ( ret_region_obj == NULL )
    {
      rc = VSD_EBADPARAM;
      break;
    }

    rc = apr_objmgr_find_object( &voicemem_objmgr, handle, &objmgr_obj );
    if ( rc ) break;

    region_obj = ( ( voicemem_region_object_t* ) objmgr_obj->any.ptr );
    if ( region_obj == NULL )
    {
      rc = VSD_EFAILED;
      break;
    }

    *ret_region_obj = region_obj;

    return VSD_EOK;
  }

  return rc;
}

/****************************************************************************
 * COMMAND PROCESSING FUNCTIONS                                             *
 ****************************************************************************/

VSD_PRIVATE uint32_t voicemem_cmd_init ( void )
{
  int32_t i = 0;
#ifdef AVS_BEAR_FAMILY	 
  int32_t rc = 0;
  smem_alloc_params_type smem_params;
#endif  
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "====== voicemem_cmd_init() ======" );

  if ( voicemem_initialized == TRUE )
  {
    return VSD_EOK;
  }

  { /* Initialize object manager lock. */
    ( void ) apr_lock_create( APR_LOCK_TYPE_MUTEX, &voicemem_objmgr_lock );
  }

  { /* Initialize internal book-keeping heap lock. */
    ( void ) apr_lock_create( APR_LOCK_TYPE_MUTEX, &voicemem_internal_heap_lock );
  }

  { /* Initialize SMEM heap lock. */
    ( void ) apr_lock_create( APR_LOCK_TYPE_MUTEX, &voicemem_smem_heap_lock );
  }

  { /* Initialize the object manager. */
    apr_objmgr_setup_params_t params;

    params.table = voicemem_object_table;
    params.total_bits = VOICEMEM_HANDLE_TOTAL_BITS_V;
    params.index_bits = VOICEMEM_HANDLE_INDEX_BITS_V;
    params.lock_fn = voicemem_objmgr_lock_fn;
    params.unlock_fn = voicemem_objmgr_unlock_fn;
    ( void ) apr_objmgr_construct( &voicemem_objmgr, &params );
  }

  { /* Initialize VOICEMEM internal book-keeping heap manager. */
    apr_memmgr_init_heap( &voicemem_internal_heapmgr,
                          ( ( void* ) &voicemem_internal_heap_pool ),
                          sizeof( voicemem_internal_heap_pool ), NULL, NULL );

/*    ( void ) apr_memmgr_heap_set_lock ( &voicemem_internal_heapmgr,
                                        voicemem_internal_heap_lock_fn,
                                        voicemem_internal_heap_unlock_fn );*/
  }

#ifndef WINSIM
  { /* Initialize SMEM. */
    ( void ) smem_init( );
  }
#endif

  { /* Allocate SMEM voice heap and initialize the VOICEMEM SMEM heap manager. */
    uint32_t smem_heap_size;

#ifndef WINSIM
    /* Allocate SMEM heap. */

    /* NOTE: The 80 KB SMEM_VOICE region is shared between voice and ECall. ECall
     * allocates the first 8 KB. SMEM does not do heap management and there is no
     * smem_free() (hence the need for this VOICEMEM driver) therefore when a client
     * allocates a certain size from one of the predefined SMEM regions, other
     * clients that subsequently allocate from the same SMEM region will get the
     * same address and size allocated by the first client. Therefore it is imperative
     * that VOICEMEM is initialized BEFORE ECall, so that we can get the full 80 KB.
     */ 
	 
#ifdef AVS_BEAR_FAMILY	 
    mmstd_memset( (void*)(&smem_params), 0, sizeof( smem_alloc_params_type ) );
	
    /* NOTE : Remote host is the proc which is going to access the shared memory.
     * Host proc and remote proc cannot be the same, so SMEM_MODEM will not work.
     * Populating SMEM_APPS as the remote host to make the smem alloc successful.
    */
    smem_params.remote_host = SMEM_APPS;
    smem_params.smem_type = SMEM_VOICE;
    smem_params.size = VOICEMEM_SMEM_HEAP_SIZE;
    smem_params.buffer = NULL;
    smem_params.flags = VOICEMEM_SMEM_ALLOC_FLAG_CACHED;
    /* Allocating cached SMEM. */
    rc = smem_alloc_ex(&smem_params);
    
    if ( rc != SMEM_STATUS_SUCCESS )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "voicemem_cmd_init(): Failed to " \
            "allocate SMEM heap rc %d", rc );
      return VSD_EFAILED;
    }
    voicemem_smem_heap_virt_addr = smem_params.buffer;
    voicemem_smem_heap_virt_addr = ( void*) ( ( ( ( uint32_t )voicemem_smem_heap_virt_addr + 4096 - 1 )
                                                          & ( ~( 4096 - 1 ) ) ) + VOICEMEM_SMEM_HEAP_OFFSET );

    /* NOTE: smem_alloc_ex() API updates the size param with the
     * allocated size if the current allocation is successful
     * or the buffer size if it was previously allocated.
    */
    smem_heap_size = smem_params.size;
#else /* AVS_BEAR_FAMILY */
    voicemem_smem_heap_virt_addr = smem_alloc( SMEM_VOICE, VOICEMEM_SMEM_HEAP_SIZE );
    voicemem_smem_heap_virt_addr = ( void*) ( ( ( ( uint32_t )voicemem_smem_heap_virt_addr + 4096 - 1 )
                                                          & ( ~( 4096 - 1 ) ) ) + VOICEMEM_SMEM_HEAP_OFFSET );
	
    if ( voicemem_smem_heap_virt_addr == NULL )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "voicemem_cmd_init(): Failed to allocate SMEM heap" );
      return VSD_EFAILED;
    }

    ( void ) smem_get_addr( SMEM_VOICE, &smem_heap_size );
#endif	/* AVS_BEAR_FAMILY */
#else
    voicemem_smem_heap_virt_addr = malloc( VOICEMEM_SMEM_HEAP_SIZE );
    voicemem_smem_heap_virt_addr = ( void*) ( ( ( uint64_t )voicemem_smem_heap_virt_addr + 4096 - 1 )
                                                          & ( ~( 4096 - 1 ) ) );
    if ( voicemem_smem_heap_virt_addr == NULL )
    {
      MSG(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "voicemem_cmd_init(): Failed to allocate SMEM heap" );
      return VSD_EFAILED;
    }

    smem_heap_size = VOICEMEM_SMEM_HEAP_SIZE;
#endif

    /* Check that the amount of heap SMEM allocated matches what we asked for. */
    if ( smem_heap_size < VOICEMEM_SMEM_HEAP_SIZE )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "voicemem_cmd_init(): " \
              "Insufficient SMEM_VOICE heap allocated: requested %d, " \
              "allocated %d",
              VOICEMEM_SMEM_HEAP_SIZE, smem_heap_size );

      /* smem_free( voicemem_smem_heap_virt_addr ); */ /* smem_free() is not actually implemented. */

      return VSD_EFAILED;
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "voicemem_cmd_init(): Allocated " \
             "SMEM_VOICE heap: address 0x%0X8, size %d",
             voicemem_smem_heap_virt_addr, smem_heap_size );

      /* Initialize VOICEMEM SMEM heap manager. */
      for(i=0 ; i < 20 ; i++)
      {
        mem_page_usage[i] = 0;
      }
      mem_page_usage[0] = 1;

      #ifndef WINSIM
      voicemem_smem_heap_phy_addr = ( uint8_t* ) qurt_lookup_physaddr( ( ( qurt_addr_t ) voicemem_smem_heap_virt_addr ) );
      for(i=0 ; i < 20 ; i++)
      {
        mem_page[i] = (uint32_t) voicemem_smem_heap_phy_addr + (i<<12);
      }
      #else
      for(i=0 ; i < 20 ; i++)
      {
        mem_page[i] = (uint64_t) voicemem_smem_heap_virt_addr + (i<<12);
      }
      #endif
    }
  }

  voicemem_initialized = TRUE;

  return VSD_EOK;
}


VSD_PRIVATE uint32_t voicemem_cmd_deinit ( void )
{
  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,  "====== voicemem_cmd_deinit() ======"  \
       );

  if ( voicemem_initialized == FALSE )
    return VSD_EOK;

#ifndef WINSIM
  /* smem_free( voicemem_smem_heap_virt_addr ); */ /* NOTE: smem_free is not implemented. */
#else
  free( voicemem_smem_heap_virt_addr );
#endif

  /* There is no smem_deinit. */

  { /* Destroy the object manager. */
    ( void ) apr_objmgr_destruct( &voicemem_objmgr );
  }

  { /* Destroy the SMEM heap lock. */
    ( void ) apr_lock_destroy( voicemem_smem_heap_lock );
  }

  { /* Destroy the internal book-keeping heap lock. */
    ( void ) apr_lock_destroy( voicemem_internal_heap_lock );
  }

  { /* Destroy the object manager lock. */
    ( void ) apr_lock_destroy( voicemem_objmgr_lock );
  }

  voicemem_initialized = FALSE;

  return VSD_EOK;
}


VSD_PRIVATE uint32_t voicemem_cmd_alloc_default (
  voicemem_cmd_alloc_default_t* params
)
{
  uint32_t i = 0;
  uint32_t num_blocks = 0;
  uint32_t rc;
  uint32_t checkpoint = 0;
  uint32_t start_index = 0;
  voicemem_region_object_t* region_obj;


  for ( ;; )
  {
    if ( params == NULL )
    {
      rc = VSD_EBADPARAM;
      break;
    }

    if ( ( params->size == 0 ) ||
         ( params->ret_voicemem_handle == NULL ) ||
         ( params->ret_virt_addr == NULL ) )
    {
      rc = VSD_EBADPARAM;
      break;
    }

    if ( mmstd_strnlen( (char_t*)params->client_name, sizeof( params->client_name ) ) == 0 )
    {
      rc = VSD_EBADPARAM;
      break;
    }

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,  "voicemem_cmd_alloc_default(): " \
          "size=%d, client_name=%.20s",
                                          params->size, params->client_name );

    { /* Create a VOICEMEM region object. */
      rc = voicemem_create_region_object( &region_obj );
      if ( rc ) break;

      checkpoint = 1;
    }

    { /* Allocate the SMEM memory. */
      num_blocks = ( params->size ) / ( 4096 );
      if( ( (params->size) %  4096 ) != 0 )
      {
         num_blocks = num_blocks + 1;
      }
      for(i=1 ; i<20; i++)
      {
        if(mem_page_usage[i] == 0)
      {
          start_index = i;
        break;
        }
      }

      if((20-start_index) < num_blocks)
      {
         MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  \
              "voicemem_cmd_alloc_default():Not enough memory left");
      }
      //checkpoint = 2;

      region_obj->base_virt_addr = ( uint8_t* )voicemem_smem_heap_virt_addr + (start_index << 12);
      while(num_blocks)
      {
        mem_page_usage[start_index] = 1;
        start_index =start_index + 1;
        num_blocks = num_blocks - 1;
      }
      region_obj->size = params->size;
      ( void ) mmstd_memcpy( region_obj->client_name,
                             sizeof( params->client_name ),
                             params->client_name,
                             sizeof( params->client_name ) );

      *( params->ret_voicemem_handle ) = region_obj->handle;
      *( params->ret_virt_addr ) = region_obj->base_virt_addr;
    }

    return VSD_EOK;
  }

  switch ( checkpoint )
  {
  case 2:
    //apr_memmgr_free( &voicemem_smem_heapmgr, base_virt_addr );
    /*-fallthru */

  case 1:
    voicemem_destroy_region_object( region_obj );
    /*-fallthru */

  default:
    break;
  }

  return rc;
}


VSD_PRIVATE uint32_t voicemem_cmd_cache_invalidate (
  voicemem_cmd_cache_invalidate_t* params
)
{
  uint32_t rc = VSD_EOK;
#ifndef WINSIM
  int qurt_mem_rc;
#endif

  if ( params == NULL )
  {
    return VSD_EBADPARAM;
  }

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_LOW, "voicemem_cmd_cache_invalidate(): "
         "handle=0x%08X, addr=0x%016X, size=%d",params->voicemem_handle,
         params->virt_addr, params->size );

  for (;;) 
  {
#ifndef WINSIM
     qurt_mem_rc = qurt_mem_cache_clean( ( ( qurt_addr_t ) params->virt_addr ), params->size,
                                         QURT_MEM_CACHE_INVALIDATE, 
                                         QURT_MEM_DCACHE );
     if ( qurt_mem_rc != QURT_EOK )
     {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "voicemem_cmd_cache_invalidate(): "
               "qurt_mem_cache_clean(): addr=0x%016X,  result=0x%08X, size=%d",
               params->virt_addr, qurt_mem_rc, params->size );
        rc = VSD_EFAILED;
        break;
     }
#endif
     break;
  }
 
  return rc;
}


VSD_PRIVATE uint32_t voicemem_cmd_cache_flush (
  voicemem_cmd_cache_flush_t* params
)
{
  uint32_t rc = VSD_EOK;
#ifndef WINSIM
  int qurt_mem_rc;
#endif

  if ( params == NULL )
  {
    return VSD_EBADPARAM;
  }
  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_LOW, "voicemem_cmd_cache_flush(): "
         "handle=0x%08X, addr=0x%016X, size=%d",
         params->voicemem_handle, params->virt_addr, params->size );

  for (;;) 
  {
#ifndef WINSIM
     qurt_mem_rc = qurt_mem_cache_clean( ( ( qurt_addr_t ) params->virt_addr ), params->size,
                                         QURT_MEM_CACHE_FLUSH, 
                                         QURT_MEM_DCACHE );
     if ( qurt_mem_rc != QURT_EOK )
     {
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "voicemem_cmd_cache_invalidate(): "
               "qurt_mem_cache_clean(): addr=0x%016X,  result=0x%08X, size=%d",
               params->virt_addr, qurt_mem_rc, params->size );
        rc = VSD_EFAILED;
        break;
     }
#endif
     break;
  }
 
  return rc;
}


VSD_PRIVATE uint32_t voicemem_cmd_free (
  voicemem_cmd_free_t* params
)
{
  uint32_t rc;
  uint32_t j;
  uint32_t num_blocks;
  voicemem_region_object_t* region_obj;
  void* virtual_address;
#ifndef WINSIM
  qurt_addr_t phys_addr;
  uint32_t i;
#endif

  if ( params == NULL )
  {
    rc = VSD_EBADPARAM;
    return rc;
  }

  /* Find the VOICEMEM region. */
  rc = voicemem_get_region_object( params->voicemem_handle, &region_obj );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "voicemem_cmd_free(): Invalid " \
            "voicemem_handle = 0x%08X",
                                            params->voicemem_handle );
    return rc;
  }

  MSG_1(MSG_SSID_DFLT, MSG_LEGACY_MED, "voicemem_cmd_free(): handle=0x%0X8",
                                        params->voicemem_handle );

  /* Deallocate the SMEM memory. */
  num_blocks = (region_obj->size) / (4096);
  if( ((region_obj->size) % (4096)) != 0)
  {
    num_blocks = num_blocks + 1;
  }
  virtual_address = region_obj->base_virt_addr ;
  #ifndef WINSIM
  for(i=0;i<num_blocks;i++)
  {
    phys_addr = qurt_lookup_physaddr( ( ( qurt_addr_t ) virtual_address ) );
    if ( phys_addr == 0 )
    {
      rc = APR_EBADPARAM;
      return rc;
    }
   for(j=0;j<20;j++)
   {
     if(mem_page[j] == (uint64_t)phys_addr)
	 {
       mem_page_usage[j] = 0;
	 }
   }
    virtual_address = &virtual_address + 4096;
  }
#else
  {
   for(j=0;j<20;j++)
   {
     if(mem_page[j] == (uint64_t)virtual_address)
	 {
       mem_page_usage[j] = 0;
	   break;
	 }
   }
    virtual_address = &virtual_address + 4096;
  }
  #endif
  /* Destroy the region object. */
  ( void ) voicemem_destroy_region_object( region_obj );

  return VSD_EOK;
}


VSD_PUBLIC vsd_status_t voicemem_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc;

  if ( ( ( size > 0 ) && ( params == NULL ) ) ||
       ( ( size == 0 ) && ( params != NULL ) ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "voicemem_call(): cmd_id " \
            "(0x%0X8) params (0x%08X) size (%d) is invaild",
                                            cmd_id );
    return APR_EBADPARAM;
  }
    if ( ( voicemem_initialized == FALSE ) &&
         ( cmd_id != VOICEMEM_CMD_INIT ) )
    {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,  "voicemem_call(): cmd_id " \
            "(0x%0X8) called before VOICEMEM has been initialized",
                                              cmd_id );
      return APR_ENOTREADY;
    }
  switch ( cmd_id )
  {
    case VOICEMEM_CMD_INIT:
    rc = voicemem_cmd_init();
    break;

    case VOICEMEM_CMD_DEINIT:
    rc = voicemem_cmd_deinit();
    break;

    case VOICEMEM_CMD_ALLOC_DEFAULT:
      rc = voicemem_cmd_alloc_default( ( voicemem_cmd_alloc_default_t* ) params );
      break;

    case VOICEMEM_CMD_CACHE_INVALIDATE:
      rc = voicemem_cmd_cache_invalidate( ( voicemem_cmd_cache_invalidate_t* ) params );
      break;

    case VOICEMEM_CMD_CACHE_FLUSH:
      rc = voicemem_cmd_cache_flush( ( voicemem_cmd_cache_flush_t* ) params );
      break;

    case VOICEMEM_CMD_FREE:
      rc = voicemem_cmd_free( ( voicemem_cmd_free_t* ) params );
      break;

#if 0
  case VOICEMEM_CMD_GET_ATTRIBUTES:
    break;
#endif /* 0 */
    default:
    MSG_1(MSG_SSID_DFLT, MSG_LEGACY_ERROR, "Unsupported cmd_id (0x%08X)",
                                            cmd_id );
    rc = VSD_EUNSUPPORTED;
     break;
  }

  return rc;
}

