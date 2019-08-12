/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemDeSerializerConcreteTypes

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_DATAITEMDESERIALIZERCONCRETETYPES_H__
#define __IZAT_MANAGER_DATAITEMDESERIALIZERCONCRETETYPES_H__

#include <comdef.h>
#include <DataItemDeSerializerBase.h>

namespace qc_loc_fw {
    class InPostcard;
}

namespace izat_manager {

class IDataItem;

using namespace ::qc_loc_fw;

// DeSerializer concrete types. For each new DataItem defined, define an
// implementation of DataItemDeSerializerBase here.

class AirplaneModeDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    AirplaneModeDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~AirplaneModeDataItemDeSerializer ();
};

class ENHDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    ENHDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~ENHDataItemDeSerializer ();
};

class GPSStateDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    GPSStateDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~GPSStateDataItemDeSerializer ();
};

class NLPStatusDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    NLPStatusDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~NLPStatusDataItemDeSerializer ();
};

class WifiHardwareStateDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    WifiHardwareStateDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~WifiHardwareStateDataItemDeSerializer ();
};

class ScreenStateDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    ScreenStateDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~ScreenStateDataItemDeSerializer ();
};

class PowerConnectStateDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    PowerConnectStateDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~PowerConnectStateDataItemDeSerializer ();
};

class TimeZoneChangeDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    TimeZoneChangeDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~TimeZoneChangeDataItemDeSerializer ();
};

class TimeChangeDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    TimeChangeDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~TimeChangeDataItemDeSerializer ();
};

class ShutdownStateDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    ShutdownStateDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~ShutdownStateDataItemDeSerializer ();
};

class AssistedGpsDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    AssistedGpsDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~AssistedGpsDataItemDeSerializer ();
};

class NetworkInfoDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    NetworkInfoDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~NetworkInfoDataItemDeSerializer ();
};

class ServiceStatusDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    ServiceStatusDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~ServiceStatusDataItemDeSerializer ();
};

class ModelDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    ModelDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~ModelDataItemDeSerializer ();
};

class ManufacturerDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    ManufacturerDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~ManufacturerDataItemDeSerializer ();
};

class RilCellInfoDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    RilCellInfoDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~RilCellInfoDataItemDeSerializer ();
};

class RilServiceInfoDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    RilServiceInfoDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~RilServiceInfoDataItemDeSerializer ();
};

class WifiSupplicantStatusDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    WifiSupplicantStatusDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~WifiSupplicantStatusDataItemDeSerializer ();
};

class TacDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    TacDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~TacDataItemDeSerializer ();
};

class MccmncDataItemDeSerializer : public DataItemDeSerializerBase {
public:
    MccmncDataItemDeSerializer (IDataItem * d);
    virtual int32 deSerialize (InPostcard * in);
    virtual ~MccmncDataItemDeSerializer ();
};



} // namespace izat_manager
#endif // #ifndef __IZAT_MANAGER_DATAITEMDESERIALIZERCONCRETETYPES_H__
