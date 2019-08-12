/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemDeSerializerConcreteTypes Implementation

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#include <comdef.h>
#include <DataItemDeSerializerConcreteTypes.h>
#include <postcard.h>
#include <DataItemDefines.h>
#include <log_util.h>

using namespace qc_loc_fw;
using namespace izat_manager;

#define DATA_ITEM_DESERIALIZER_CTOR(S) \
S::S(IDataItem * d) \
: \
DataItemDeSerializerBase(d) \
{}

#define DATA_ITEM_DESERIALIZER_DTOR(S) \
S::~S() {}


#define DATA_ITEM_DESERIALIZE(T) \
int32 T::deSerialize \
(InPostcard * ic)

/*
    This macro to be used when we start filling the methods
    defined using DATA_ITEM_DESERIALIZE macro
#define CHECK_AND_ADD_NEW_CARD() \
    if( (mD == NULL) || (ic == NULL) ) { result = 1; break; } \
    if( DataItemDeSerializerBase::hasCardName() ) { \
        card = OutPostcard::createInstance(); \
        if(card == NULL) { result = 2; break; } \
        BREAK_IF_NON_ZERO(3,oc->addCard(DataItemDeSerializerBase::getCardName().c_str(), card)); \
    }

*/

// Ctors
DATA_ITEM_DESERIALIZER_CTOR(AirplaneModeDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(ENHDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(GPSStateDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(NLPStatusDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(WifiHardwareStateDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(ScreenStateDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(PowerConnectStateDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(TimeZoneChangeDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(TimeChangeDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(ShutdownStateDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(AssistedGpsDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(NetworkInfoDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(ServiceStatusDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(ModelDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(ManufacturerDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(RilCellInfoDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(RilServiceInfoDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(WifiSupplicantStatusDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(TacDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_CTOR(MccmncDataItemDeSerializer)

// Dtors
DATA_ITEM_DESERIALIZER_DTOR(AirplaneModeDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(ENHDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(GPSStateDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(NLPStatusDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(WifiHardwareStateDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(ScreenStateDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(PowerConnectStateDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(TimeZoneChangeDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(TimeChangeDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(ShutdownStateDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(AssistedGpsDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(NetworkInfoDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(ServiceStatusDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(ModelDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(ManufacturerDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(RilCellInfoDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(RilServiceInfoDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(WifiSupplicantStatusDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(TacDataItemDeSerializer)
DATA_ITEM_DESERIALIZER_DTOR(MccmncDataItemDeSerializer)

// deSerialize TODO
DATA_ITEM_DESERIALIZE(AirplaneModeDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(ENHDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(GPSStateDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(NLPStatusDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(WifiHardwareStateDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(ScreenStateDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(PowerConnectStateDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(TimeZoneChangeDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(TimeChangeDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(ShutdownStateDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(AssistedGpsDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(NetworkInfoDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(ServiceStatusDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(ModelDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(ManufacturerDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(RilCellInfoDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(RilServiceInfoDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(WifiSupplicantStatusDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(TacDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
DATA_ITEM_DESERIALIZE(MccmncDataItemDeSerializer) {
    UNUSED(ic);
    int32 result = 0;
    ENTRY_LOG();
    do {
    } while(0);
    EXIT_LOG_WITH_ERROR("%d",result);
    return result;
}
