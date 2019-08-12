#ifndef AEEISUPPORTSWEAKREF_H
#define AEEISUPPORTSWEAKREF_H
/*
=======================================================================
              Copyright 2005-2008 Qualcomm Technologies Incorporated.
                     All Rights Reserved.
                  QUALCOMM Proprietary and Confidential
=======================================================================
*/
#include "datamodem_variation.h"
#include "ds_Utils_IQI.h"
#include "ds_Utils_IWeakRef.h"

#define AEEIID_ISupportsWeakRef 0x0104709e

#if defined(AEEINTERFACE_CPLUSPLUS)
struct ISupportsWeakRef : public IQI
{
   virtual int CDECL GetWeakRef(IWeakRef** piWeak) = 0;
};
#else
#define INHERIT_ISupportsWeakRef(iname) \
   INHERIT_IQI(iname);\
   int (*GetWeakRef) (iname*  me, IWeakRef** piWeak)

AEEINTERFACE_DEFINE(ISupportsWeakRef);

static __inline uint32 ISupportsWeakRef_AddRef(ISupportsWeakRef* me)
{
   return AEEGETPVTBL(me,ISupportsWeakRef)->AddRef(me);
}

static __inline uint32 ISupportsWeakRef_Release(ISupportsWeakRef* me)
{
   return AEEGETPVTBL(me,ISupportsWeakRef)->Release(me);
}

static __inline int ISupportsWeakRef_QueryInterface(ISupportsWeakRef* me, 
                                                    AEEIID iid, void** ppo)
{
   return AEEGETPVTBL(me,ISupportsWeakRef)->QueryInterface(me, iid, ppo);
}

static __inline int ISupportsWeakRef_GetWeakRef(ISupportsWeakRef* me, 
                                                IWeakRef** ppiWeak)
{
   return AEEGETPVTBL(me,ISupportsWeakRef)->GetWeakRef(me, ppiWeak);
}
#endif

/*=====================================================================
INTERFACES DOCUMENTATION
=======================================================================
=======================================================================
ISupportsWeakRef Interface

Description:

   ISupportsWeakRef is used to retrieve a weak reference from an object.

See Also:
   IWeakRef
   AEECObjWR.h

=============================================================================

ISupportsWeakRef_AddRef()

Description:
   This method is inherited from IQI_AddRef(). 

See Also:
   ISupportsWeakRef_Release()

=============================================================================

ISupportsWeakRef_Release()


Description:
   This method is inherited from IQI_Release(). 

See Also:
   ISupportsWeakRef_AddRef()

=============================================================================

ISupportsWeakRef_QueryInterface()


Description:
   This method is inherited from IQI_QueryInterface().

See Also:
   ISupportsWeakRef_AddRef(), ISupportsWeakRef_Release()

=============================================================================

ISupportsWeakRef_GetWeakRef()

Description:

   This method obtains a weak reference.

Prototype:
   int ISupportsWeakRef_GetWeakRef(ISupportsWeakRef* me, 
                                   IWeakRef** ppiWeak)

Parameters:
    me : [in] : Pointer to the ISupportsWeakRef Interface object.
    ppiWeak : [out] : Pointer to the objects weak reference.

Return Value:
    AEE_SUCCESS, IWeakRef was created and returned.
    or other AEEError, on failure

Comments:
    None

See Also:
    IWeakRef_QueryReferent()

=======================================================================*/



#endif //AEEISUPPORTSWEAKREF_H

