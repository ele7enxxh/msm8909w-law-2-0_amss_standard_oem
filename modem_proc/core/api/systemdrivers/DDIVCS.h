#ifndef DDIVCS_H
#define DDIVCS_H
/*
===========================================================================
*/
/**
  @file DDIVCS.h
  @brief Public interface include file for accessing the VCS device driver.

  The DDIVCS.h file is the public API interface to the VCS 
  Device Access Library (DAL).
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The VCS_API_mainpage.dox file contains all file/group descriptions that 
      are in the output PDF generated using Doxygen and LaTeX. To edit or 
      update any of the file/group text in the PDF, edit the 
      VCS_API_mainpage.dox file or contact Tech Pubs.

===========================================================================*/
/*
  ===========================================================================

  Copyright (c) 2014 QUALCOMM Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.5.c12.3/api/systemdrivers/DDIVCS.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $

  when       who     what, where, why
  --------   ---     ---------------------------------------------------------- 

  ===========================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "DALSys.h"
#include "com_dtypes.h" /* for "boolean" */
#include "VCSDefs.h"
#include "ClockDefs.h"


/*=========================================================================
      Type Definitions
==========================================================================*/


/**
  Defines the interface version.
 */
#define DALVCS_INTERFACE_VERSION DALINTERFACE_VERSION(1,0)


/*=========================================================================
      Interface Definitions
==========================================================================*/

/** @cond 
*/

typedef struct DalVCS DalVCS;
struct DalVCS
{
   struct DalDevice DalDevice;
   DALResult (*SetRailMode)(DalDeviceHandle *_h, VCSRailType eRail, VCSRailModeType eMode);
   DALResult (*SetRailVoltage)(DalDeviceHandle *_h, VCSRailType eRail, uint32 nVoltageUV);
   DALResult (*SetCPUCorner)(DalDeviceHandle *_h, ClockCPUType eCPU, VCSCornerType eCorner);
   DALResult (*AddLDORestriction)(DalDeviceHandle *_h, ClockCPUType eCPU);
   DALResult (*RemoveLDORestriction)(DalDeviceHandle *_h, ClockCPUType eCPU);
};

typedef struct DalVCSHandle DalVCSHandle;
struct DalVCSHandle
{
   uint32 dwDalHandleId;
   const DalVCS * pVtbl;
   void * pClientCtxt;
};

#define DAL_VCSDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALVCS_INTERFACE_VERSION,hDalDevice)

/** @endcond */


/*=========================================================================
      Interface Declarations
==========================================================================*/


/** @addtogroup rail_functions
@{ */

/* =========================================================================
**  Function : DalVCS_SetRailMode
** =========================================================================*/
/**
  Sets a voltage rail to a specified mode.

  @param _h    [in] Handle to the DAL device.
  @param eRail [in] Rail enum.
  @param eMode [in] Mode type enum.

  @return
  DAL_SUCCESS -- Successfully set the rail to requested mode. \n
  DAL_ERROR_INVALID_PARAMETER -- Invalid rail passed in. \n
  DAL_ERROR_NOT_SUPPORTED -- Unsupported mode passed in.

  @dependencies
  None.
*/

static __inline DALResult
DalVCS_SetRailMode(DalDeviceHandle * _h, VCSRailType eRail, VCSRailModeType eMode)
{
   return ((DalVCSHandle *)_h)->pVtbl->SetRailMode( _h, eRail, eMode);
}


/* =========================================================================
**  Function : DalVCS_SetRailVoltage
** =========================================================================*/
/**
  Sets a voltage rail to a specified mode.

  @param _h    [in] Handle to the DAL device.
  @param eRail [in] Rail enum.
  @param nVoltage [in] Rail voltage.

  @return
  DAL_SUCCESS -- Successfully set the rail to requested voltage. \n
  DAL_ERROR_INVALID_PARAMETER -- Invalid rail passed in. \n
  DAL_ERROR_NOT_SUPPORTED -- Unsupported voltage passed in.

  @dependencies
  None.
*/

static __inline DALResult
DalVCS_SetRailVoltage(DalDeviceHandle * _h, VCSRailType eRail, uint32 nVoltageUV)
{
   return ((DalVCSHandle *)_h)->pVtbl->SetRailVoltage( _h, eRail, nVoltageUV);
}

/** @} */ /* end_addtogroup rail_functions */


/** @addtogroup ldo_functions
@{ */

/* =========================================================================
**  Function : DalVCS_SetCPUCorner
** =========================================================================*/
/**
  Function for the clock driver to make voltage requests for the CPU clock.

  This function places a voltage corner vote on the rail. In addition, it
  checks if we can use the LDO at the requested corner. If yes,
  this function enables and sets the LDO to the requested corner. If not,
  this function turns off the LDO.

  @param _h      [in] Handle to the DAL device.
  @param eCPU    [in] CPU identifier.
  @param eCorner [in] Voltage corner.

  @return
  DAL_SUCCESS -- Successfully placed a vote on the rail on behalf of the
                 CPU. Also set the LDO to requested corner, or disabled
                 it given constraints. \n
  DAL_ERROR_INVALID_PARAMETER -- Invalid CPU ID passed in.

  @dependencies
  None.
*/

static __inline DALResult
DalVCS_SetCPUCorner(DalDeviceHandle * _h, ClockCPUType eCPU, VCSCornerType eCorner)
{
   return ((DalVCSHandle *)_h)->pVtbl->SetCPUCorner( _h, eCPU, eCorner);
}


/* =========================================================================
**  Function : DalVCS_AddLDORestriction
** =========================================================================*/
/**
  Restrict the usage of a given LDO. This may be required for various
  reasons such as thermal mitigation. We keep a count of the number of
  restrictions so that we only re-allow LDO usage after all clients have
  lifted their restrictions. Any restriction on the LDO disables its
  usage completely.

  @param _h   [in] Handle to the DAL device.
  @param eCPU [in] Address of CPU node.

  @return
  DAL_SUCCESS -- Successfully restricted the usage of the requested LDO. \n
  DAL_ERROR_INVALID_PARAMETER -- Invalid CPU ID passed in.

  @dependencies
  None.
*/

static __inline DALResult
DalVCS_AddLDORestriction(DalDeviceHandle * _h, ClockCPUType eCPU)
{
   return ((DalVCSHandle *)_h)->pVtbl->AddLDORestriction( _h, eCPU);
}


/* =========================================================================
**  Function : DalVCS_RemoveLDORestriction
** =========================================================================*/
/**
  Remove a restriction on the usage of a given LDO. If the LDO has no more
  restrictions on it, we re-allow its usage.

  @param _h   [in] Handle to the DAL device.
  @param eCPU [in] Address of CPU node.

  @return
  DAL_SUCCESS -- Successfully removed the restriction on requested LDO. \n
  DAL_ERROR_INVALID_PARAMETER -- Invalid CPU ID passed in.

  @dependencies
  None.
*/

static __inline DALResult
DalVCS_RemoveLDORestriction(DalDeviceHandle * _h, ClockCPUType eCPU)
{
   return ((DalVCSHandle *)_h)->pVtbl->RemoveLDORestriction( _h, eCPU);
}

/** @} */ /* end_addtogroup ldo_functions */


/** @addtogroup misc_functions
@{ */


#endif /* !DDIVCS_H */

