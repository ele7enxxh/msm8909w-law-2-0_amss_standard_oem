/**
  @file mcfg_seg_load.h

  @brief  Definitions used in dynamically loaded objects (segment-loading)
*/
/*
  Copyright (c) 2013 Qualcomm Technologies Incorporated.
          All Rights Reserved.
    Qualcomm Confidential and Proprietary
*/

/*===========================================================================
                      EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mcfg_fwk/api/mcfg_seg_load.h#1 $
  $DateTime: 2016/12/13 08:00:27 $


  when        who     what, where, why
  ---------   ---     ---------------------------------------------------------------------------
 2014/03/14   hzhi    Updated API mcfg_VA_to_PA() and mcfg_PA_to_VA() to support no-segmentloading and OFF-TARGET use cases. 
 2014/02/14   hzhi    Fix for CR_616433: Added FUNCTION mcfg_VA_to_PA() and mcfg_PA_to_VA().
 2013/12/19   sk      Fix for CR_591729
 2013/12/18   sk      Fix for CR_591396
 2013/12/10   sk      Updated for constant name change and FEATURE SEGLOAD ALL
 2013/11/14   hzhi    Moved FUNCTION get_tdscdma_interface and FUNCTION get_wcdma_interface definition from MMCP. 
 2013/09/19   hzhi    Moved mcfg_segment_set_boot_params(uint32 BootParam) from test_interface.c.
 2013/09/05   hzhi    Added mcfg_segment_exclude_technologies() function for segment loading. 
 2013/05/17   hzhi    Added Function seg_load_release_inactive_mode(void) and seg_load_init(void).
 2013/05/07   sk      Added file hearder according to qualcomm standards.
 2013/03/23   hzhi    Added Function mcfg_segment_reset(uint32 ResetParam).
 2013/01/01   ngeng   Created for segment loading.

===========================================================================*/
#ifndef _MCFG_SEG_LOAD_H_
#define _MCFG_SEG_LOAD_H_

#include "ref_cnt_obj.h"
#include "comdef.h"
#include "IxErrno.h"
#include "qurt_memory.h"

#include "mcfg_mmu.h"

/*=============================================================================
  For mcfg_segment_exclude_technologies() to use as input.
  =============================================================================*/
#define MCFG_SEGMENT_LOAD_ALL        0x00000000
#define MCFG_SEGMENT_EXCLUDE_WCDMA   0x00000001
#define MCFG_SEGMENT_EXCLUDE_TDSCDMA 0x00000002

/*-----------------------------------------------------------------------------
  Definition of interface IDs.  This will go in a separate .h file.
-----------------------------------------------------------------------------*/
typedef enum
{
  IID_IUNKNOWN    = 0,

  IID_MAX         = 0x7ffffff
} iid_t;  


/*-----------------------------------------------------------------------------
  Definition of class IDs.  This will go in a separate .h file.
-----------------------------------------------------------------------------*/
typedef enum
{
  CLSID_TDSCDMA   = 0x8000000,
  CLSID_WCDMA     = 0x8000001,
  CLSID_WCDMA_MMCP     = 0x8000002,
  CLSID_WCDMA_RFTECH   = 0x8000003,
  CLSID_WCDMA_QCHAT    = 0x8000004
} clsid_t;

/*-----------------------------------------------------------------------------
  Definition of return types for segment loading functions. 
-----------------------------------------------------------------------------*/
typedef enum
{
  MCFG_SEGLOAD_SUCCESS                      = 0,
  MCFG_SEGLOAD_IS_FTM_OFFLINE_MODE          = 1,
  MCFG_SEGLOAD_EFS_QUERY_FAIL               = 2,
  MCFG_SEGLOAD_EFS_SET_FAIL                 = 3,
  MCFG_SEGLOAD_RECLAIM_TLB_FAIL             = 4,
  MCFG_SEGLOAD_REMAP_UNLOADED_SEGMENT_FAIL  = 5, 
  MCFG_SEGLOAD_LOAD_ALL_TECHNOLOGY          = 6,
  MCFG_SEGLOAD_UPDATE_MMU_TRANSLATION_FAIL  = 7,
  MCFG_SEGLOAD_UPDATE_MMU_ENTRY_OVERFLOW_FAIL = 8,
  MCFG_SEGLOAD_UPDATE_HEAP_FROM_AMSS_MALLOC_FAIL         = 9,
  MCFG_SEGLOAD_EFS_SET_FAIL_AND_SHOULD_NOT_SUPPORT_ALL_RATS_MODE = 10
} mcfg_segload_status_e_type;


/*-------- iunknown_t --------*/
/**
@brief  This is the definition of the base class of all interface pointers.
*/
typedef struct    interface_s interface_t;
typedef struct    iunknown_s  iunknown_t;



/*-------- add_ref_fn --------*/
/**
@brief  Pointer to a function that adds a reference to an object.

@param[in]  pObject Pointer to the object.

@return
  Nothing.
*/
typedef void      (*add_ref_fn)(void *pObject);


/*-------- release_fn --------*/
/**
@brief  Pointer to a function to release a reference to an object.

@param[in]  pObject Pointer to the object.

@return
  Nothing.
*/
typedef void      (*release_fn)(void *pObject);


/*-------- qi_fn --------*/
/**
@brief  Pointer to function that returns a specific interface to an object.

@param[in]  pObject Pointer to the object.
@param[in]  pIID    Pointer to the guid of the desired interface.
@param[out] ppIface Pointer to storage for a pointer to the requested interface.

@return
  - E_SUCCESS if the requested interface is available on the object.
  - E_NOT_AVAILABLE if the requested interface is not available on the object.
*/
typedef IxErrnoType (*qi_fn)(void *pObject, iid_t *pIID, interface_t **ppIface);

/*-------- iunknown_s --------*/
/**
@brief  The dispatch table for the iunknown interface.
*/
struct iunknown_s
{
  add_ref_fn  add_ref;
  release_fn  release;
  qi_fn       query_interface;
};



struct interface_s
{
  _REF_CNT_OBJ;
  iunknown_t  *pVtable;
};


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
  );





/*-------- get_iface_fn --------*/
/**
@brief  Function implemented by a given component to return a pointer to its
        interface.

@param  None

@return
  Pointer to a reference-counted interface object, which contains a pointer to
  the class's vtable.
*/
typedef interface_t* (*get_iface_fn)(void);



/*-------- release_instance --------*/
/**
@brief  FOR USE OF SEGMENT LOADER ONLY

        This NULLs the pointer to a component in the static tables that
        describe the available components.  It is used by the segment loader
        to remove entries that have not been loaded.

@param[in]  clsid   The class ID the component to be released.

@return
  
*/
void
release_instance(
  clsid_t clsid
  );

/*===========================================================================

FUNCTION get_wcdma_interface

DESCRIPTION
  This function returns the interface to the WCDMA Access Stratum segment.
  If the segment is not loaded, it will return NULL.

DEPENDENCIES
  None

RETURN VALUE
  --> wcdma interface pointer if WCDMA segment is loaded. 
  --> NULL if WCDMA segment is unloaded. 

SIDE EFFECTS
  None

===========================================================================*/
interface_t *get_wcdma_interface(void);

/*===========================================================================

FUNCTION get_wcdma_mmcp_interface

DESCRIPTION
  This function returns the interface to the WCDMA Access Stratum segment 
  for MMCP only to use.
  If the segment is not loaded, it will return NULL.

DEPENDENCIES
  None

RETURN VALUE
  --> wcdma interface pointer if WCDMA segment is loaded. 
  --> NULL if WCDMA segment is unloaded. 

SIDE EFFECTS
  None

===========================================================================*/
interface_t *get_wcdma_mmcp_interface(void);

/*===========================================================================

FUNCTION get_tdscdma_interface

DESCRIPTION
  This function returns the interface to the TDSCDMA Access Stratum segment.
  If the segment is not loaded, it will return NULL.

DEPENDENCIES
  None

RETURN VALUE
  --> tdscdma interface pointer if TDSCDMA segment is loaded. 
  --> NULL if TDSCDMA segment is unloaded. 

SIDE EFFECTS
  None

===========================================================================*/
interface_t *get_tdscdma_interface(void);

/*-------- mcfg_segment_exclude_technologies --------*/
/**
@brief  Called by MMCP system selection algo, to exclude specific technology.

@param[in]  technologies     The const defined for related technology.
@param[out] int   Indicate if exclusion request succeeds.

@return
  -->  0 if the request is successful.
  --> -1 if the request is denied.
*/
int
mcfg_segment_exclude_technologies(
  uint32  technologies
  );

#endif /* _MCFG_SEG_LOAD_H_ */