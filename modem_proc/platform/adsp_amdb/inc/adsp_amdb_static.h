#ifndef ADSP_AMDB_H
#define ADSP_AMDB_H
/*==============================================================================

Copyright (c) 2012-2013 Qualcomm Technologies, Incorporated.  All Rights Reserved. 
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.

==============================================================================*/

#include "Elite_APPI.h"
#include "Elite_CAPI.h"
#include "Elite_CAPI_new.h"
#include "Elite_CAPI_V2.h"

#include "adsp_amdb.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus

/******************************************************************************* 
 ADSP Audio Module DataBase (adsp_amdb) API Overview

 AMDB is a repository for APPI and CAPI based audio modules.  It allows both
 static and dynamic modules to be added, queried, and removed.
 
 Modules added to the database are tracked using numeric reference counting.
 The reference is incremented by one when the module is added to the database
 and for each time the module is successfully queried.  upon a successful query
 a pointer to the module is returned, the reference count can be incremented or
 decremented using the addref and release functions.  When no longer needed the
 reference count added when the module is queried must be released.
 
 For example:
 
 adsp_amdb_add_appi - sets ref count to 1
 adsp_amdb_get_appi() - increments reference count and returns pointer to module
 adsp_amdb_appi_addref - increments ref count
 adsp_amdb_appi_release - decrements ref count
 adsp_amdb_appi_release - decrements ref count
 adsp_amdb_remove_appi - decreement ref count, count is no zero and module is
                         freed
 
 As long as a module is being used a reference count must be non-zero.
 
 Once a module is queried, that module can be used to create instances of
 the APPI or CAPI objects.  The modules reference count is incremented each time
 an APPI or CAPI object is created and decremented when the APPI or CAPI End()
 method is called.
 
 To create an APPI object:
 
   call adsp_amdb_appi_getsize_f()
   allocate memory
   call adsp_amdb_appi_init_f() and pass the pointer allocated
 
 To create a CAPI object:
 
   call adsp_amdb_capi_new_f()
 
 Once an APPI or CAPI object is created that interface's End() method must be
 called to decrement the reference count.  If it is not then that object will
 never be freed.
 
 When dealing with dynamic audio modules the module will only be unloaded when
 all objects using it are freed (ref count goes to zero).
 
*******************************************************************************/

/******************************************************************************* 
  init/deinit, operates on a global instance of adsp_amdb
*******************************************************************************/
ADSPResult adsp_amdb_init(void);
void adsp_amdb_deinit(void);

/******************************************************************************* 
  amdb modules types
*******************************************************************************/
typedef struct adsp_amdb_capi_t adsp_amdb_capi_t;
typedef struct adsp_amdb_appi_t adsp_amdb_appi_t;
typedef struct adsp_amdb_capi_v2_t adsp_amdb_capi_v2_t;

/******************************************************************************* 
  increment/decrement the amdb module
*******************************************************************************/
uint32_t adsp_amdb_capi_addref(adsp_amdb_capi_t* me);
uint32_t adsp_amdb_capi_release(adsp_amdb_capi_t* me);
uint32_t adsp_amdb_capi_v2_addref(adsp_amdb_capi_v2_t* me);
uint32_t adsp_amdb_capi_v2_release(adsp_amdb_capi_v2_t* me);
uint32_t adsp_amdb_appi_addref(adsp_amdb_appi_t* me);
uint32_t adsp_amdb_appi_release(adsp_amdb_appi_t* me);

/******************************************************************************* 
  creates new CAPI object
*******************************************************************************/
ADSPResult adsp_amdb_capi_new_f(adsp_amdb_capi_t* me, uint32_t format,
                                uint32_t bps, ICAPI** capi_ptr_ptr);

capi_v2_err_t adsp_amdb_capi_v2_get_static_properties_f(adsp_amdb_capi_v2_t* me,
                                                     capi_v2_proplist_t* init_set_properties,
                                                     capi_v2_proplist_t* static_properties);
capi_v2_err_t adsp_amdb_capi_v2_init_f(adsp_amdb_capi_v2_t* me,
                                    capi_v2_t* _pif,
                                    capi_v2_proplist_t* init_set_properties);

/******************************************************************************* 
  queries size and then initializes an APPI object
*******************************************************************************/
ADSPResult adsp_amdb_appi_getsize_f(adsp_amdb_appi_t* me,
                                    const appi_buf_t* params_ptr,
                                    uint32_t* size_ptr);
ADSPResult adsp_amdb_appi_init_f(adsp_amdb_appi_t* me, appi_t* appi_ptr,
                                 bool_t* is_inplace_ptr,
                                 const appi_format_t* in_format_ptr,
                                 appi_format_t* out_format_ptr, appi_buf_t* info_ptr);

/******************************************************************************* 
  Get an APPI or CAPI module based on module ID
*******************************************************************************/
ADSPResult adsp_amdb_get_capi(int id, adsp_amdb_capi_t** capi_ptr_ptr);
ADSPResult adsp_amdb_get_capi_ex(int type, int id1, int id2,
                                 adsp_amdb_capi_t** capi_ptr_ptr);
ADSPResult adsp_amdb_get_capi_v2(int type, int id1, int id2,
                                 adsp_amdb_capi_v2_t** capi_v2_ptr_ptr);
ADSPResult adsp_amdb_get_appi(int id, adsp_amdb_appi_t** appi_ptr_ptr);

/******************************************************************************* 
  Add a static module to the database
*******************************************************************************/
ADSPResult adsp_amdb_add_static_capi(int id, capi_getsize_f getsize_f,
                                     capi_ctor_f ctor_f);
ADSPResult adsp_amdb_add_static_capi_ex(int type, int id1, int id2,
                                        capi_getsize_f getsize_f,
                                        capi_ctor_f ctor_f);
ADSPResult adsp_amdb_add_static_capi_v2(int type, int id1, int id2,
                                        capi_v2_get_static_properties_f get_static_properties_f,
                                        capi_v2_init_f init_f);
ADSPResult adsp_amdb_add_static_appi(int id, appi_getsize_f getsize_f,
                                     appi_init_f init_f);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // ADSP_AMDB_H
