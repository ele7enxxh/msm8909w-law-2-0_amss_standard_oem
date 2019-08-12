/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemSerializerConcreteTypes

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_DATAITEMSERIALIZERCONCRETETYPES_H__
#define __IZAT_MANAGER_DATAITEMSERIALIZERCONCRETETYPES_H__

#include <comdef.h>
#include <DataItemSerializerBase.h>

namespace qc_loc_fw {
    class OutPostcard;
}

namespace izat_manager {

class IDataItem;

using namespace ::qc_loc_fw;

// Serializer concrete types. For each new Data Item defined, define
// an implementation of DataItemSerializerBase here.

class AirplaneModeDataItemSerializer : public DataItemSerializerBase {
public:
    AirplaneModeDataItemSerializer (IDataItem * d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~AirplaneModeDataItemSerializer ();
};

class ENHDataItemSerializer : public DataItemSerializerBase {
public:
    ENHDataItemSerializer (IDataItem *d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~ENHDataItemSerializer ();
};

class GPSStateDataItemSerializer : public DataItemSerializerBase {
public:
    GPSStateDataItemSerializer (IDataItem *d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~GPSStateDataItemSerializer ();
};

class NLPStatusDataItemSerializer : public DataItemSerializerBase {
public:
    NLPStatusDataItemSerializer (IDataItem *d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~NLPStatusDataItemSerializer ();
};

class WifiHardwareStateDataItemSerializer : public DataItemSerializerBase {
public:
    WifiHardwareStateDataItemSerializer (IDataItem *d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~WifiHardwareStateDataItemSerializer ();
};

class ScreenStateDataItemSerializer : public DataItemSerializerBase {
public:
    ScreenStateDataItemSerializer (IDataItem *d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~ScreenStateDataItemSerializer ();
};

class PowerConnectStateDataItemSerializer : public DataItemSerializerBase {
public:
    PowerConnectStateDataItemSerializer (IDataItem *d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~PowerConnectStateDataItemSerializer ();
};

class TimeZoneChangeDataItemSerializer : public DataItemSerializerBase {
public:
    TimeZoneChangeDataItemSerializer (IDataItem *d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~TimeZoneChangeDataItemSerializer ();
};

class TimeChangeDataItemSerializer : public DataItemSerializerBase {
public:
    TimeChangeDataItemSerializer (IDataItem *d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~TimeChangeDataItemSerializer ();
};

class ShutdownStateDataItemSerializer : public DataItemSerializerBase {
public:
    ShutdownStateDataItemSerializer (IDataItem *d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~ShutdownStateDataItemSerializer ();
};

class AssistedGpsDataItemSerializer : public DataItemSerializerBase {
public:
    AssistedGpsDataItemSerializer (IDataItem *d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~AssistedGpsDataItemSerializer ();
};

class NetworkInfoDataItemSerializer : public DataItemSerializerBase {
public:
    NetworkInfoDataItemSerializer (IDataItem *d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~NetworkInfoDataItemSerializer ();
};

class ServiceStatusDataItemSerializer : public DataItemSerializerBase {
public:
    ServiceStatusDataItemSerializer (IDataItem *d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~ServiceStatusDataItemSerializer ();
};

class ModelDataItemSerializer : public DataItemSerializerBase {
public:
    ModelDataItemSerializer (IDataItem *d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~ModelDataItemSerializer ();
};

class ManufacturerDataItemSerializer : public DataItemSerializerBase {
public:
    ManufacturerDataItemSerializer (IDataItem *d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~ManufacturerDataItemSerializer ();
};

class RilCellInfoDataItemSerializer : public DataItemSerializerBase {
public:
    RilCellInfoDataItemSerializer (IDataItem * d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~RilCellInfoDataItemSerializer ();
};

class RilServiceInfoDataItemSerializer : public DataItemSerializerBase {
public:
    RilServiceInfoDataItemSerializer (IDataItem * d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~RilServiceInfoDataItemSerializer ();
};


class WifiSupplicantStatusDataItemSerializer : public DataItemSerializerBase {
public:
    WifiSupplicantStatusDataItemSerializer (IDataItem * d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~WifiSupplicantStatusDataItemSerializer ();
};

class TacDataItemSerializer : public DataItemSerializerBase {
public:
    TacDataItemSerializer (IDataItem *d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~TacDataItemSerializer ();
};

class MccmncDataItemSerializer : public DataItemSerializerBase {
public:
    MccmncDataItemSerializer (IDataItem *d);
    virtual int32 serialize (OutPostcard * out);
    virtual ~MccmncDataItemSerializer ();
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_DATAITEMSERIALIZERCONCRETETYPES_H__
