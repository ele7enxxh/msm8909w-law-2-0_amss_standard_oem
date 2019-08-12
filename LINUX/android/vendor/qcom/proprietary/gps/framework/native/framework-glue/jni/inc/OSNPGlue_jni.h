/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  Network Location Provider Proxy JNI

  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#ifndef __OSNPGLUEJNI_H__
#define __OSNPGLUEJNI_H__

#include <IzatTypes.h>

namespace izat_manager {
class IzatRequest;
}

using namespace izat_manager;

bool onInitJNI ();
void onDestroyJNI ();
void onSetRequestJNI (IzatRequest * request);
void onStopRequestJNI ();

#endif // #ifndef __NETWORKLOCATIONPROVIDERGLUE_H__
