#ifndef __DSS_QOS_H__
#define __DSS_QOS_H__

/*======================================================

FILE:  DSS_QoS.h

SERVICES:
QoS class

GENERAL DESCRIPTION:

=====================================================

Copyright (c) 2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dss/src/DSS_QoS.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-09-12 ea  Class created.

===========================================================================*/


//===================================================================
//   Includes and Public Data Declarations
//===================================================================

//-------------------------------------------------------------------
// Include Files
//-------------------------------------------------------------------
#include "comdef.h"
#include "customer.h"

#include "ds_Utils_IWeakRef.h"
#include "ds_Utils_IWeakRefSupport.h"
#include "ds_Net_IQoS.h"
#include "dssocket_defs.h"

//-------------------------------------------------------------------
// Constant / Define Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Type Declarations (typedef, struct, enum, etc.)
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Constant Data Declarations
//-------------------------------------------------------------------

//-------------------------------------------------------------------
// Global Data Declarations
//-------------------------------------------------------------------


//-------------------------------------------------------------------
// Forward Declarations
//-------------------------------------------------------------------
class DSSNetActive;
//===================================================================
//              Macro Definitions
//===================================================================

//===================================================================
//              Class Definitions
//===================================================================

using ds::Utils::IWeakRef;

class DSSQoS : public IWeakRef
{
//-------------------------------------------------------------------
//  Constructors/Destructors
//-------------------------------------------------------------------
  public:

    virtual void Destructor() throw() = 0;

//-------------------------------------------------------------------
//  Interface members
//-------------------------------------------------------------------
  public:

    // Set() and Get() of parent DSSNetApp object.
    // GetDSSNetApp() returns NetApp with storng ref, or NULL if
    // NetApp freed already. who calls GetDSSNetApp(), should call
    // Release() on NetApp when done.
    virtual AEEResult GetDSSNetActive(DSSNetActive** ppDSSNetActive) = 0;
    virtual void SetDSSNetActive(DSSNetActive* pDSSNetActive) = 0;

    virtual AEEResult GetQoS(ds::Net::IQoS** ppQoS) = 0;
    virtual void GetHandle(dss_qos_handle_type* pHandle) = 0;
};

//===================================================================

#endif // __DSS_QOS_H__
