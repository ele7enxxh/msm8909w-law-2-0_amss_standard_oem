/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

GENERAL DESCRIPTION
  DataItemStringifierConcreteTypes

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/

#ifndef __IZAT_MANAGER_DATAITEMSTRINGIFIERCONCRETETYPES_H__
#define __IZAT_MANAGER_DATAITEMSTRINGIFIERCONCRETETYPES_H__


#include <DataItemStringifierBase.h>

namespace izat_manager {

using namespace std;

class IDataItem;

// Stringifier concrete types. For each new DataItem defined, define an
// implementation of DataItemStringifierBase here.

class AirplaneModeDataItemStringifier : public DataItemStringifierBase {
public:
    AirplaneModeDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~AirplaneModeDataItemStringifier ();
};

class ENHDataItemStringifier : public DataItemStringifierBase {
public:
    ENHDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~ENHDataItemStringifier ();
};

class GPSStateDataItemStringifier : public DataItemStringifierBase {
public:
    GPSStateDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~GPSStateDataItemStringifier ();
};

class NLPStatusDataItemStringifier : public DataItemStringifierBase {
public:
    NLPStatusDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~NLPStatusDataItemStringifier ();
};

class WifiHardwareStateDataItemStringifier : public DataItemStringifierBase {
public:
    WifiHardwareStateDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~WifiHardwareStateDataItemStringifier ();
};

class ScreenStateDataItemStringifier : public DataItemStringifierBase {
public:
    ScreenStateDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~ScreenStateDataItemStringifier ();
};

class PowerConnectStateDataItemStringifier : public DataItemStringifierBase {
public:
    PowerConnectStateDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~PowerConnectStateDataItemStringifier ();
};

class TimeZoneChangeDataItemStringifier : public DataItemStringifierBase {
public:
    TimeZoneChangeDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~TimeZoneChangeDataItemStringifier ();
};

class TimeChangeDataItemStringifier : public DataItemStringifierBase {
public:
    TimeChangeDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~TimeChangeDataItemStringifier ();
};

class ShutdownStateDataItemStringifier : public DataItemStringifierBase {
public:
    ShutdownStateDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~ShutdownStateDataItemStringifier ();
};

class AssistedGpsDataItemStringifier : public DataItemStringifierBase {
public:
    AssistedGpsDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~AssistedGpsDataItemStringifier ();
};

class NetworkInfoDataItemStringifier : public DataItemStringifierBase {
public:
    NetworkInfoDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~NetworkInfoDataItemStringifier ();
};

class ServiceStatusDataItemStringifier : public DataItemStringifierBase {
public:
    ServiceStatusDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~ServiceStatusDataItemStringifier ();
};

class ModelDataItemStringifier : public DataItemStringifierBase {
public:
    ModelDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~ModelDataItemStringifier ();
};

class ManufacturerDataItemStringifier : public DataItemStringifierBase {
public:
    ManufacturerDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~ManufacturerDataItemStringifier ();
};

class RilCellInfoDataItemStringifier : public DataItemStringifierBase {
public:
    RilCellInfoDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~RilCellInfoDataItemStringifier ();
};

class RilServiceInfoDataItemStringifier : public DataItemStringifierBase {
public:
    RilServiceInfoDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~RilServiceInfoDataItemStringifier ();
};

class PipUserSettingDataItemStringifier : public DataItemStringifierBase {
public:
    PipUserSettingDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~PipUserSettingDataItemStringifier ();
};

class WifiSupplicantStatusDataItemStringifier : public DataItemStringifierBase {
public:
    WifiSupplicantStatusDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~WifiSupplicantStatusDataItemStringifier ();
};

class TacDataItemStringifier : public DataItemStringifierBase {
public:
    TacDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~TacDataItemStringifier ();
};

class MccmncDataItemStringifier : public DataItemStringifierBase {
public:
    MccmncDataItemStringifier (IDataItem * d);
    virtual void stringify (string & valueStr);
    virtual ~MccmncDataItemStringifier ();
};


} // namespace izat_manager

#endif // #ifndef __IZAT_MANAGER_DATAITEMSTRINGIFIERCONCRETETYPES_H__
