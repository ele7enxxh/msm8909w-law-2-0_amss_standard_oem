/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright  (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved. Qualcomm Technologies Proprietary and Confidential.
=============================================================================*/

#define LOG_TAG "OSNPGlue"
#define LOG_NDEBUG 0

#include <algorithm>
#include <list>
#include <comdef.h>
#include <IzatTypes.h>
#include <IzatRequest.h>
#include <IzatLocation.h>
#include <IIzatLocationListener.h>
#include <OSNPGlue.h>
#include <OSNPGlue_jni.h>

using namespace std;
using namespace izat_manager;

OSNPGlue * OSNPGlue :: mInstance = NULL;

OSNPGlue * OSNPGlue :: getInstance () {

    if (!mInstance) {
        bool status = onInitJNI ();
        if (status) {
            mInstance = new OSNPGlue ();
        }
    }

    return mInstance;
}

void OSNPGlue :: destroyInstance () {
    onDestroyJNI ();
    delete mInstance;
    mInstance = NULL;
}

// ctor
OSNPGlue :: OSNPGlue () {}

// dtor
OSNPGlue :: ~OSNPGlue () {}

void OSNPGlue :: onLocationChanged (IzatLocation * location) {
    list <IIzatLocationListener *> :: iterator it = mListeners.begin ();
    for ( ; it != mListeners.end (); ++it) {
       (*it)->onLocationChanged (location, IZAT_LOCATION_STATUS_FINAL);
    }
}

void OSNPGlue :: onStatusChanged (IzatProviderStatus status) {
    list <IIzatLocationListener *> :: iterator it = mListeners.begin ();
    for ( ; it != mListeners.end (); ++it) {
       (*it)->onStatusChanged (status);
    }
}

int32 OSNPGlue :: subscribe (IIzatLocationListener * izatListener) {
    if (izatListener) {
        mListeners.push_back (izatListener);
        mListeners.unique ();
    }
    return 0;
}

int32 OSNPGlue :: setRequest (const IzatRequest * request) {
    if (request) {
        onSetRequestJNI (const_cast <IzatRequest *> (request));
        return 0;
    }
    return -1;
}

void OSNPGlue :: stopRequest () {
    onStopRequestJNI ();
}

int32 OSNPGlue :: unsubscribe (IIzatLocationListener * izatListener) {
    if(izatListener) {
        list <IIzatLocationListener *> :: iterator it =
            find (mListeners.begin (), mListeners.end (), izatListener);
        if (it != mListeners.end ()) {
            mListeners.erase (it);
        }
    }
    return 0;
}

