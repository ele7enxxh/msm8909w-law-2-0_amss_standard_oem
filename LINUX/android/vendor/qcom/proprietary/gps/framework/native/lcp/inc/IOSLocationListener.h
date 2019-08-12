/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  IOSLocationListener

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_IOSLOCATIONLISTENER_H__
#define __IZAT_MANAGER_IOSLOCATIONLISTENER_H__

#include "IzatTypes.h"
#include "IIzatLocationListener.h"

namespace izat_manager {

/**
 * @brief IOSLocationListener interface
 * @details IOSLocationListener interface;
 *          A OS dependent entity designated as the listener of network locations
 *          i.e, stream of network locations sent from Izat subsystem
 *          and or an entity designated as the listener of fused locations
 *          i.e, stream of fused locations sent from Izat subsystem
 *          shall implement this interface to receive
 *          location and provider related updates from Izat subsystem
 */
class IOSLocationListener
    : public IIzatLocationListener {

public:

    /**
     * @brief Gets bit masks of location streams that this listener listens to
     * @details Gets bit masks of location streams that this
     *          listener listens to
     * @return Location listener bit mask
     */
    virtual IzatLocationListenerMask listensTo () const = 0;

    /**
     * @brief Destructor
     * @details Destructor
     */
    virtual ~IOSLocationListener () {}
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_IOSLOCATIONLISTENER_H__
