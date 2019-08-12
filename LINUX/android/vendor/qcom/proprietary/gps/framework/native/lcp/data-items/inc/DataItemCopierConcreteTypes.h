/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemCopierConcreteTypes

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_DATAITEMCOPIERCONCRETETYPES_H__
#define __IZAT_MANAGER_DATAITEMCOPIERCONCRETETYPES_H__

#include <comdef.h>
#include <DataItemCopierBase.h>


namespace izat_manager {

class IDataItem;

// Copier concrete types. For each new DataItem defined, define an
// implementation of DataItemCopierBase here.

class AirplaneModeDataItemCopier : public DataItemCopierBase {
public:
    AirplaneModeDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~AirplaneModeDataItemCopier ();
};

class ENHDataItemCopier : public DataItemCopierBase {
public:
    ENHDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~ENHDataItemCopier ();
};

class GPSStateDataItemCopier : public DataItemCopierBase {
public:
    GPSStateDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~GPSStateDataItemCopier ();
};

class NLPStatusDataItemCopier : public DataItemCopierBase {
public:
    NLPStatusDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~NLPStatusDataItemCopier ();
};

class WifiHardwareStateDataItemCopier : public DataItemCopierBase {
public:
    WifiHardwareStateDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~WifiHardwareStateDataItemCopier ();
};

class ScreenStateDataItemCopier : public DataItemCopierBase {
public:
    ScreenStateDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~ScreenStateDataItemCopier ();
};

class PowerConnectStateDataItemCopier : public DataItemCopierBase {
public:
    PowerConnectStateDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~PowerConnectStateDataItemCopier ();
};

class TimeZoneChangeDataItemCopier : public DataItemCopierBase {
public:
    TimeZoneChangeDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~TimeZoneChangeDataItemCopier ();
};

class TimeChangeDataItemCopier : public DataItemCopierBase {
public:
    TimeChangeDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~TimeChangeDataItemCopier ();
};

class ShutdownStateDataItemCopier : public DataItemCopierBase {
public:
    ShutdownStateDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~ShutdownStateDataItemCopier ();
};

class AssistedGpsDataItemCopier : public DataItemCopierBase {
public:
    AssistedGpsDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~AssistedGpsDataItemCopier ();
};

class NetworkInfoDataItemCopier : public DataItemCopierBase {
public:
    NetworkInfoDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~NetworkInfoDataItemCopier ();
};

class ServiceStatusDataItemCopier : public DataItemCopierBase {
public:
    ServiceStatusDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~ServiceStatusDataItemCopier ();
};

class ModelDataItemCopier : public DataItemCopierBase {
public:
    ModelDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~ModelDataItemCopier ();
};

class ManufacturerDataItemCopier : public DataItemCopierBase {
public:
    ManufacturerDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~ManufacturerDataItemCopier ();
};

class RilCellInfoDataItemCopier : public DataItemCopierBase {
public:
    RilCellInfoDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~RilCellInfoDataItemCopier ();
};

class RilServiceInfoDataItemCopier : public DataItemCopierBase {
public:
    RilServiceInfoDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~RilServiceInfoDataItemCopier ();
};

class PipUserSettingDataItemCopier : public DataItemCopierBase {
public:
    PipUserSettingDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~PipUserSettingDataItemCopier ();
};

class WifiSupplicantStatusDataItemCopier : public DataItemCopierBase {
public:
    WifiSupplicantStatusDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~WifiSupplicantStatusDataItemCopier ();
};

class TacDataItemCopier : public DataItemCopierBase {
public:
    TacDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~TacDataItemCopier ();
};

class MccmncDataItemCopier : public DataItemCopierBase {
public:
    MccmncDataItemCopier (IDataItem * d);
    virtual int32 copy (IDataItem * src, bool *dataItemCopied = NULL);
    virtual ~MccmncDataItemCopier ();
};

} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_DATAITEMCOPIERCONCRETETYPES_H__
