#ifndef DS_UTILS_CS_SUPPORT_H
#define DS_UTILS_CS_SUPPORT_H
/*=========================================================================*/
/*!
  @file
  ds_Utils_CSSupport.h

  @brief
  This file defines a set of macros for which can be used as default
  implementations for IQI methods.

  The following macros are exported:

  DSIQI_CREATE_INSTANCE()
    Provides a macro around the extern C to call DSBase create instance.
    This macro needs to be placed outside the class declaration.

  DSIQI_ADDREF()
    Macro that implements IQI::AddRef() method.

  DSIQI_RELEASE()
    Macro that implements IQI::Release() method.

  DSIQI_QUERY_INTERFACE()
    Macro that implements default behavior for IQI::QueryInterface(). This
    macro implements the most general case where one class is implementing
    one interface along with the IQI interface.

  DSIQI_IQI_DEFAULTS()
    This macro provides a wrapper around DSIQI_ADDREF(), DSIQI_RELEASE() and
    DSIQI_QUERY_INTERFACE().

  Copyright (c) 2008-2014 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/utils/inc/ds_Utils_CSSupport.h#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-06-23 hm  Created module.

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_StdDef.h"
#include "ds_Utils_Atomic.h"
#include "ds_Utils_DebugMsg.h"

/*!
  This macro implements the factory function to create a component. This
  macro needs to be defined outside of the class for which CreateInstance
  function is needed.

  @param[in] ns
  The name of the namespace in which the class is defined

  @param[in] classname
  The name of the class that implements the component

  @param[in] clsid
  The class ID corresponding to the implementation of the class.

  @param[out] newObj
  A pointer to the newly created instance is stored here.
*/
#define DSIQI_CREATE_INSTANCE(ns,classname)                                 \
    extern "C"                                                              \
    int ns##classname##CreateInstance(void* env,                            \
                                      AEECLSID clsid,                       \
                                      void** newObj)                        \
    {                                                                       \
        return static_cast<int>                                             \
        (ns::classname::CreateInstance(clsid, env, newObj));       \
    }

/*!
  This macro implements the factory function to create a component. This
  macro needs to be defined outside of the class for which CreateInstance
  function is needed. Use this macro if your class is within two
  namespaces. (For example ds::Net::Network)
 
  @param[in] ns1
  The name of the namespace in which the class is defined

  @param[in] ns2
  The name of the second namespace

  @param[in] classname
  The name of the class that implements the component

  @param[in] clsid
  The class ID corresponding to the implementation of the class.

  @param[out] newObj
  A pointer to the newly created instance is stored here.
*/
#define DSIQI_DECL_CREATE_INSTANCE2(ns1,ns2,classname)                      \
    extern "C"                                                              \
    int ns1##ns2##classname##CreateInstance(void* env,                      \
                                            AEECLSID clsid,                 \
                                            void** newObj);

/*!
  This macro implements the factory function to create a component. This
  macro needs to be defined outside of the class for which CreateInstance
  function is needed. Use this macro if your class is within two
  namespaces. (For example ds::Net::Network)
 
  @param[in] ns1
  The name of the namespace in which the class is defined
 
  @param[in] ns2
  The name of the second namespace
 
  @param[in] classname
  The name of the class that implements the component
 
  @param[in] clsid
  The class ID corresponding to the implementation of the class.
 
  @param[out] newObj
  A pointer to the newly created instance is stored here.
*/
#define DSIQI_IMPL_CREATE_INSTANCE2(ns1,ns2,classname)                      \
    extern "C"                                                              \
    int ns1##ns2##classname##CreateInstance(void* env,                      \
                                            AEECLSID clsid,                 \
                                            void** newObj)                  \
    {                                                                       \
        return static_cast<int>                                             \
        (ns1::ns2::classname::CreateInstance(env, clsid, newObj)); \
    }

/*!
  Internal macro that declares the locals used for AddRef and Release()
  methods.

  @param      None.
  @return     None.
*/
#define DSIQI_DECL_LOCALS()  uint32 refCnt;

/*!
  @brief
  This macro implements the IQI::AddRef() method.

  @details
  This macro implements the IQI::AddRef() method. This macro needs to be
  called from within the class that implements the IQI interface.

  @param      None.
  @return     None.
*/
#define DSIQI_ADDREF()                                                   \
  virtual uint32 CDECL AddRef()  throw()                                 \
  {                                                                      \
    (void)ds_utils_atomic_Add(&refCnt, 1);                               \
    return refCnt;                                                       \
  }

/*! special version for singletons */
#define DSIQI_ADDREF_SINGLETON()                                         \
  virtual uint32 CDECL AddRef()  throw()                                 \
  {                                                                      \
    return 1;                                                            \
  }

/*!
  @brief
  This macro implements the IQI::Release() method.

  @details
  This macro implements the IQI::Release() method. This macro needs to
  be called from within the class that implements the IQI interface.

  @param      None.
  @return     None.
*/
#define DSIQI_RELEASE()                                                  \
  virtual uint32 CDECL Release()  throw()                                \
  {                                                                      \
    uint32 tmpRefCnt = ds_utils_atomic_Add(&refCnt, -1);                 \
    if (tmpRefCnt == 0)                                                  \
    {                                                                    \
      delete this;                                                       \
    }                                                                    \
    return tmpRefCnt;                                                    \
  }

/*! special version for singletons */
#define DSIQI_RELEASE_SINGLETON()                                        \
  virtual uint32 CDECL Release()  throw()                                \
  {                                                                      \
    return 1;                                                            \
  }

/*!
  @brief
  This macro implements the IQI::QueryInterface() method.

  @details
  This macro implements the IQI::QueryInterface() method. This macro
  needs to be called from within the class.

  @param      None.
  @return     None.
*/
#define DSIQI_QUERY_INTERFACE(interfaceName)                             \
  virtual int CDECL QueryInterface (AEEIID iid, void **ppo)  throw()     \
  {                                                                      \
    switch (iid)                                                         \
    {                                                                    \
      case AEEIID_##interfaceName:                                       \
      case AEEIID_IQI:                                                   \
        *ppo = reinterpret_cast <void *> (this);                         \
        (void) AddRef ();                                                \
        break;                                                           \
                                                                         \
      default:                                                           \
        return AEE_ECLASSNOTSUPPORT;                                     \
    }                                                                    \
                                                                         \
    return AEE_SUCCESS;                                                  \
  }

/*!
  @brief
  This macro implements the IQI::QueryInterface() method.

  @details
  Classes that do not expose any public interfaces besides IQI should
  use this macro instead of DSIQI_QUERY_INTERFACE.

  @param      None.
  @return     None.
*/
#define DSIQI_QUERY_INTERFACE_IQI()                                      \
  virtual int CDECL QueryInterface (AEEIID iid, void **ppo)  throw()     \
  {                                                                      \
    switch (iid)                                                         \
    {                                                                    \
      case AEEIID_IQI:                                                   \
        *ppo = reinterpret_cast <void *> (this);                         \
        (void) AddRef ();                                                \
        break;                                                           \
                                                                         \
      default:                                                           \
        return AEE_ECLASSNOTSUPPORT;                                     \
    }                                                                    \
                                                                         \
    return AEE_SUCCESS;                                                  \
  }

/*! special version for singletons */
#define DSIQI_QUERY_INTERFACE_SINGLETON(interfaceName)                             \
  virtual int CDECL QueryInterface (AEEIID iid, void **ppo)  throw()     \
  {                                                                      \
    switch (iid)                                                         \
    {                                                                    \
      case AEEIID_##interfaceName:                                       \
      case AEEIID_IQI:                                                   \
        *ppo = reinterpret_cast <void *> (this);                         \
        (void) AddRef ();                                                \
        break;                                                           \
                                                                         \
      default:                                                           \
        return AEE_ECLASSNOTSUPPORT;                                     \
    }                                                                    \
                                                                         \
    return AEE_SUCCESS;                                                  \
  }

/*!
  This macro is a wrapper around  addref/release.

  @param      interfaceName - Name of the interface
  @return     None.
*/
#define DSIQI_IMPL_DEFAULTS_NO_QI()                                      \
  DSIQI_DECL_LOCALS()                                                    \
  DSIQI_ADDREF()                                                         \
  DSIQI_RELEASE()

/*!
  This macro is a wrapper around all the IQI methods.

  @param      interfaceName - Name of the interface
  @return     None.
*/
#define DSIQI_IMPL_DEFAULTS(interfaceName)                               \
  DSIQI_IMPL_DEFAULTS_NO_QI()                                            \
  DSIQI_QUERY_INTERFACE(interfaceName)

/*!
  This macro is a wrapper around all the IQI methods - Singleton edition

  @param      interfaceName - Name of the interface
  @return     None.
*/
#define DSIQI_IMPL_DEFAULTS_SINGLETON(interfaceName)                      \
   DSIQI_ADDREF_SINGLETON()                                               \
   DSIQI_RELEASE_SINGLETON()                                              \
   DSIQI_QUERY_INTERFACE_SINGLETON(interfaceName)

/**
  @brief
  Utility macro to perform Release() on an interface pointer.
  @details
  This macro releases an interface if it is non-NULL. It then sets the pointer
  to NULL.
  @param[in]  pInterface - Double pointer to the interface to be released.
  @see       IQI::Release()
*/
#define DS_UTILS_RELEASEIF(pInterface)                                      \
  do                                                                        \
  {                                                                         \
    /*lint -save -e1550 -e1551 -e423 */                                     \
    if (NULL != (pInterface))                                               \
    {                                                                       \
      (void) (pInterface)->Release();                                       \
      (pInterface) = NULL;                                                  \
    }                                                                       \
    /*lint -restore */                                                      \
  } while (0)

/**
  @brief
  Utility macro to perform ReleaseWeak() on an interface pointer.
  @details
  This macro releases an interface if it is non-NULL. It then sets the pointer
  to NULL.
  @param[in]  pInterface - pointer to the interface to be released.
  @see       IQI::Release()
*/
#define DS_UTILS_RELEASE_WEAKREF_IF(pInterface)                             \
  do                                                                        \
  {                                                                         \
    /*lint -save -e423 */                                                   \
    if (NULL != (pInterface))                                               \
    {                                                                       \
      (void) (pInterface)->ReleaseWeak();                                   \
      (pInterface) = NULL;                                                  \
    }                                                                       \
    /*lint -restore */                                                      \
  } while (0)

/*---------------------------------------------------------------------------
  Following macros provide IWeakRef implementation. They also
  provide IQI::AddRef(), IQI::Release() implementations when weak references
  are supported.
---------------------------------------------------------------------------*/
/*!
  This macro declares required object vars when weak refs are supported.
*/
#define DS_UTILS_IWEAKREF_DECL_LOCALS()                                     \
  uint32  refCnt;                                                           \
  uint32  weakRefCnt;

/*!
  This macro implements DS::Utils::IWeakRef::AddRefWeak() method
*/
#define DS_UTILS_IWEAKREF_ADDREF_WEAK()                                     \
  virtual uint32 AddRefWeak()  throw()                                      \
  {                                                                         \
    uint32 tmpRefCnt = ds_utils_atomic_Add(&weakRefCnt, 1);                 \
    return tmpRefCnt;                                                       \
  }

/*!
  This macro implements DS::Utils::IWeakRef::ReleaseWeak() method.
*/
#define DS_UTILS_IWEAKREF_RELEASE_WEAK()                                    \
  virtual uint32 ReleaseWeak()  throw()                                     \
  {                                                                         \
    uint32 tmpRefCnt = ds_utils_atomic_Add(&weakRefCnt, -1);                \
    if (tmpRefCnt != 0)                                                     \
    {                                                                       \
      return tmpRefCnt;                                                     \
    }                                                                       \
    delete this;                                                            \
    return 0;                                                               \
  }

/*!
  This macro implements DS::Utils::IWeakRef::GetStrongRef() method
*/
#define DS_UTILS_IWEAKREF_GET_STRONG_REF()                                  \
  virtual boolean GetStrongRef() throw()                                    \
  {                                                                         \
    uint32 tmpRefCnt = ds_utils_atomic_CompareOrAdd(&refCnt, 0, 1);         \
    return (tmpRefCnt > 0);                                                 \
  }

/*!
  This macro implements DS::Utils::IWeakRef::IQI::Release() method
*/
#define DS_UTILS_IWEAKREF_RELEASE()                                         \
  virtual uint32 CDECL Release()  throw()                                   \
  {                                                                         \
    uint32 tmpRefCnt = ds_utils_atomic_Add(&refCnt, -1);                    \
    if (0 == tmpRefCnt)                                                     \
    {                                                                       \
      Destructor();                                                         \
      (void) ReleaseWeak();                                                 \
    }                                                                       \
    return tmpRefCnt;                                                       \
  }

/*---------------------------------------------------------------------------
  Wrapper methods to be used by objects.
---------------------------------------------------------------------------*/
/*!
  This is a wrapper for methods IWeakRef methods,
  No QueryInterface() is defined.
*/
#define DS_UTILS_IWEAKREF_IMPL_DEFAULTS_NO_QI()                             \
  DS_UTILS_IWEAKREF_DECL_LOCALS()                                           \
  DSIQI_ADDREF()                                                            \
  DS_UTILS_IWEAKREF_RELEASE()                                               \
  DS_UTILS_IWEAKREF_ADDREF_WEAK()                                           \
  DS_UTILS_IWEAKREF_RELEASE_WEAK()                                          \
  DS_UTILS_IWEAKREF_GET_STRONG_REF()

/*!
  This is a wrapper for methods IWeakRef/IQI methods,
  QueryInterface() supports only AEEIID_IQI.
*/
#define DS_UTILS_IWEAKREF_IMPL_DEFAULTS()                                   \
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS_NO_QI()                                   \
  DSIQI_QUERY_INTERFACE_IQI()

/*!
  This is a wrapper for methods IWeakRef methods,
  QueryInterface() supports AEEIID_IQI and IQI and one additional interface.
*/
#define DS_UTILS_IWEAKREF_IMPL_DEFAULTS2(interfaceName)                     \
  DS_UTILS_IWEAKREF_IMPL_DEFAULTS_NO_QI()                                   \
  DSIQI_QUERY_INTERFACE(interfaceName)

#endif /* DS_UTILS_CS_SUPPORT_H */

