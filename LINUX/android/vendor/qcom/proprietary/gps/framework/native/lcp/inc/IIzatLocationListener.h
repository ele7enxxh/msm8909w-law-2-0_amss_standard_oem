/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IIzatLocationListener

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __IZAT_MANAGER_IIZATLOCATIONLISTENER_H__
#define __IZAT_MANAGER_IIZATLOCATIONLISTENER_H__

#include "IzatTypes.h"

namespace izat_manager {

class IzatLocation;

/**
 * @brief IIzatLocationListener interface
 * @details IIzatLocationListener interface;
 *          Must be implemented by OS dependent code to listen for location and status changes
 *
 */
class IIzatLocationListener {

public:

    /**
     * @brief Invoked when location changes
     * @details Invoked when location changes
     *
     * @param location Pointer to an instance of IzatLocation
     */
    virtual void onLocationChanged (const IzatLocation * location, const IzatLocationStatus status) = 0;

    /**
     * @brief Invoked when status changes
     * @details Invoked when status changes
     *
     * @param status Pointer to an instance of IzatProviderStatus
     */
    virtual void onStatusChanged (const IzatProviderStatus status) = 0;

    /**
     * @brief Destructor
     * @details Destructor
     */
    virtual ~IIzatLocationListener () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IIZATLOCATIONLISTENER_H__
