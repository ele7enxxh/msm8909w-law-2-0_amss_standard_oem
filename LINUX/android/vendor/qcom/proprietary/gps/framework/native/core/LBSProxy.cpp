/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
  Copyright (c) 2013-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
=============================================================================*/
#define LOG_NDDEBUG 0
#define LOG_TAG "LocSvc_LBSProxy"

#include <unistd.h>
#include <dlfcn.h>
#ifdef _HAS_LOC_V02_
#include <LBSApiV02.h>
#elif _HAS_LOC_RPC_
#include <LBSApiRpc.h>
#endif
#include <LBSProxy.h>
#include <log_util.h>
#include <loc_cfg.h>
#include <LBSAdapterBase.h>
#include <MsgTask.h>
#include <loc_target.h>

#ifdef MODEM_POWER_VOTE
#include <pm-service.h>
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#include <mdm_detect.h>
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /*MODEM_POWER_VOTE*/


#define IZAT_CONF_FILE "/etc/izat.conf"

using namespace loc_core;
using namespace lbs_core;

namespace lbs_core {

pthread_mutex_t LBSProxy::mLock = PTHREAD_MUTEX_INITIALIZER;
UlpProxyBase* LBSProxy::mUlp = NULL;
LocAdapterBase* LBSProxy::mAdapter = NULL;
unsigned long LBSProxy::mCapabilities = 0;
const char * LBSProxy::COM_QUALCOMM_LOCATION_APK_FILE_PATH =
    "/system/priv-app/com.qualcomm.location/com.qualcomm.location.apk";
const char * LBSProxy::FFOS_LOCATION_EXTENDED_CLIENT =
    "/system/b2g/distribution/bundles/location/location.so";
const bool LBSProxy::mLocationExtendedClientExists =
    LBSProxy::checkIfLocationExtendedClientExists();
const bool LBSProxy::mNativeXtraClientExists =
    LBSProxy::checkIfNativeXtraClientExists();

#ifdef MODEM_POWER_VOTE
typedef struct {
    //MAX_NAME_LEN defined in mdm_detect.h
    char modem_name[MAX_NAME_LEN];
    //MAX_PATH_LEN defined in mdm_detect.h
    char powerup_node[MAX_PATH_LEN];
    //this handle is used by peripheral mgr
    void *handle;
    int mdm_fd;
    MdmType mdm_type;
    bool peripheral_mgr_supported;
    bool peripheral_mgr_registered;
}s_loc_mdm_info;
static s_loc_mdm_info loc_mdm_info;
static void pm_event_notifier(void *client_data, enum pm_event event);
// For shutting down MDM in fusion devices
static int mdm_fd = -1;
#endif /*MODEM_POWER_VOTE*/

LocApiBase* LBSProxy::getLocApi(const MsgTask* msgTask,
                                LOC_API_ADAPTER_EVENT_MASK_T exMask,
                                ContextBase *context) const {
#ifdef _HAS_LOC_V02_
    void *handle = NULL;
    if((handle = dlopen("libizat_api_pds.so", RTLD_NOW)) != NULL) {
        LOC_LOGD("%s:%d]: libizat_api_pds.so is present. Now trying to load IzatApiPds.", __func__, __LINE__);
        getLocApi_t* getter = (getLocApi_t*)dlsym(handle, "getLocApi");
        if(getter != NULL) {
            LocApiBase* locApi = (*getter)(msgTask, exMask, context);
            if ( locApi != NULL ) {
                return locApi;
            }
        }
    }
    return new LBSApiV02(msgTask, exMask, context);
#elif _HAS_LOC_RPC_
    return new LBSApiRpc(msgTask, exMask, context);
#endif
    return NULL;
}

#ifdef MODEM_POWER_VOTE
static void pm_event_notifier(void *client_data, enum pm_event event)
{
    ENTRY_LOG();
    LOC_LOGD("%s:%d]: event: %d", __func__, __LINE__, (int)event);
    pm_client_event_acknowledge(loc_mdm_info.handle, event);
    EXIT_LOG(%s, VOID_RET);
}
#endif /*MODEM_POWER_VOTE*/

void LBSProxy::modemPowerVote(bool power) const {

#ifdef MODEM_POWER_VOTE
    if (power) {
        enum pm_event mdm_state;
        static int mdm_index = -1;
        int peripheral_mgr_ret = PM_RET_FAILED;

        //if index is 0 or more, then we've looked for mdm already
        LOC_LOGD("%s:%d]: mdm_index: %d", __func__, __LINE__,
                 mdm_index);
        if (mdm_index < 0) {
            struct dev_info modem_info;
            memset(&modem_info, 0, sizeof(struct dev_info));
            if (get_system_info(&modem_info) != RET_SUCCESS) {
                LOC_LOGE("%s:%d]: Error: get_system_info returned error",
                         __func__, __LINE__);
                return;
            }

            for (mdm_index = 0;
                mdm_index < modem_info.num_modems;
                mdm_index++) {

                // Check if mdm_name contains a valid value
                if(modem_info.mdm_list[mdm_index].mdm_name[0] != '\0'){

                    //Copy modem name to register with peripheral manager
                    strlcpy(loc_mdm_info.modem_name,
                            modem_info.mdm_list[mdm_index].mdm_name,
                            sizeof(loc_mdm_info.modem_name));
                    //copy powerup node name if we need to use mdmdetect method
                    strlcpy(loc_mdm_info.powerup_node,
                            modem_info.mdm_list[mdm_index].powerup_node,
                            sizeof(loc_mdm_info.powerup_node));
                    loc_mdm_info.mdm_type = modem_info.mdm_list[mdm_index].type;
                    LOC_LOGD("%s:%d]: Found modem: %s, powerup node:%s at index: %d",
                             __func__, __LINE__, loc_mdm_info.modem_name, loc_mdm_info.powerup_node,
                             mdm_index);
                    break;
                }
            }
        }
        if (loc_mdm_info.peripheral_mgr_registered != true) {
            peripheral_mgr_ret = pm_client_register(pm_event_notifier,
                                                    &loc_mdm_info,
                                                    loc_mdm_info.modem_name,
                                                    "GPS",
                                                    &mdm_state,
                                                    &loc_mdm_info.handle);
            if (peripheral_mgr_ret == PM_RET_SUCCESS) {
                loc_mdm_info.peripheral_mgr_supported = true;
                loc_mdm_info.peripheral_mgr_registered = true;
                LOC_LOGD("%s:%d]: registered with peripheral mgr for %s",
                         __func__, __LINE__, loc_mdm_info.modem_name);
            }
            else if (peripheral_mgr_ret == PM_RET_UNSUPPORTED) {
                loc_mdm_info.peripheral_mgr_registered = true;
                loc_mdm_info.peripheral_mgr_supported = false;
                LOC_LOGD("%s:%d]: peripheral mgr unsupported for: %s",
                         __func__, __LINE__, loc_mdm_info.modem_name);
            }
            else {
                //Not setting any flags here so that we can try again the next time around
                LOC_LOGE("%s:%d]: Error: pm_client_register returned: %d",
                         __func__, __LINE__, peripheral_mgr_ret);
            }
        }

        if (loc_mdm_info.peripheral_mgr_supported == false &&
            loc_mdm_info.peripheral_mgr_registered == true) {
            //Peripheral mgr is not supported
            //use legacy method to open the powerup node
            LOC_LOGD("%s:%d]: powerup_node: %s", __func__, __LINE__,
                     loc_mdm_info.powerup_node);
            loc_mdm_info.mdm_fd = open(loc_mdm_info.powerup_node, O_RDONLY);

            if (loc_mdm_info.mdm_fd < 0) {
                LOC_LOGE("Error: %s open failed: %s\n",
                         loc_mdm_info.powerup_node, strerror(errno));
            } else {
                LOC_LOGD("%s opens success!", loc_mdm_info.powerup_node);
            }
        } else if(loc_mdm_info.peripheral_mgr_supported == true &&
                  loc_mdm_info.peripheral_mgr_registered == true) {
            LOC_LOGD("%s:%d]: Voting for modem power up", __func__, __LINE__);
            pm_client_connect(loc_mdm_info.handle);
        } else {
            LOC_LOGD("%s:%d]: Not voted for modem power up due to errors", __func__, __LINE__);
        }

    } else {
        if (loc_mdm_info.peripheral_mgr_supported == true) {
            LOC_LOGD("%s:%d]: Voting for modem power down", __func__, __LINE__);
            pm_client_disconnect(loc_mdm_info.handle);
        }
        else if (loc_mdm_info.mdm_fd >= 0) {
            LOC_LOGD("closing the powerup node");
            close(loc_mdm_info.mdm_fd);
            loc_mdm_info.mdm_fd = -1;
            LOC_LOGD("finished closing the powerup node");
        }
        else {
            LOC_LOGD("powerup node has not been opened yet.");
        }
    }
#endif /*MODEM_POWER_VOTE*/

}

void LBSProxy::locRequestUlp(LocAdapterBase* adapter,
                              unsigned long capabilities) {
    pthread_mutex_lock(&LBSProxy::mLock);

    if (NULL == LBSProxy::mUlp) {
        LOC_LOGV("%s] %p", __func__, LBSProxy::mUlp);
    } else {
        LOC_LOGV("%s] ulp %p adapter %p", __func__,
                 LBSProxy::mUlp, adapter);
        LBSProxy::mUlp->setAdapter(adapter);
        LBSProxy::mUlp->setCapabilities(capabilities);
        adapter->setUlpProxy(LBSProxy::mUlp);
    }
    LBSProxy::mAdapter = adapter;
    LBSProxy::mCapabilities = capabilities;

    pthread_mutex_unlock(&LBSProxy::mLock);
}

void LBSProxy::ulpRequestLoc(UlpProxyBase* ulp) {
    pthread_mutex_lock(&LBSProxy::mLock);

    if (NULL == LBSProxy::mAdapter) {
        LOC_LOGV("%s] %p", __func__, LBSProxy::mAdapter);
    } else {
        LOC_LOGV("%s] ulp %p adapter %p", __func__, ulp,
                 LBSProxy::mAdapter);
        ulp->setAdapter(LBSProxy::mAdapter);
        ulp->setCapabilities(LBSProxy::mCapabilities);
        LBSProxy::mAdapter->setUlpProxy(ulp);
    }
    LBSProxy::mUlp = ulp;

    pthread_mutex_unlock(&LBSProxy::mLock);
}

bool LBSProxy::checkIfLocationExtendedClientExists()
{

    // To differentiate between PDK and non PDK builds
    if((access(COM_QUALCOMM_LOCATION_APK_FILE_PATH, F_OK) != -1) ||
        (access(FFOS_LOCATION_EXTENDED_CLIENT, F_OK) != -1)) {
        ALOGD("%s] %s",__func__, "File Exists");
        return true;
    } else {
        ALOGD("%s] %s",__func__, "File does not Exist");
        return false;
    }
}

#define NATIVE_XTRA_CLIENT_MODULE "libizat_core.so"
#define NATIVE_XTRA_CLIENT_API "_ZN9izat_xtra8initXtraEv"
bool LBSProxy::checkIfNativeXtraClientExists()
{
    // check if XTRA Client 2.0 exists
    void * handle = dlopen(NATIVE_XTRA_CLIENT_MODULE, RTLD_NOW);
    if (handle == NULL) {
      ALOGD("%s] %s",__func__, "XTRA Client 2.0 module not exists.");
      return false;
    }

    dlerror();

    void *fnptr = dlsym(handle, NATIVE_XTRA_CLIENT_API);
    const char *error = NULL;

    if((error=dlerror()) != NULL || fnptr == NULL) {
        ALOGD("%s] %s reason:error=%s function=%x",__func__,
              "XTRA Client 2.0 not exists.", error, fnptr);
        dlclose(handle);
        handle = NULL;
        return false;
    }

    dlclose(handle);
    handle = NULL;

    ALOGD("%s] %s",__func__, "XTRA Client 2.0 Exist");
    return true;
}

bool LBSProxy :: reportPositionToUlp(UlpLocation &location,
                                     GpsLocationExtended &locationExtended,
                                     void* locationExt,
                                     enum loc_sess_status status,
                                     LocPosTechMask techMask)
{
    LOC_LOGD("%s:%d]: Enter", __func__, __LINE__);
    if(mUlp != NULL) {
        LOC_LOGD("%s:%d]: Exit. Sent to ulp", __func__, __LINE__);
        return mUlp->reportPosition(location, locationExtended, locationExt, status,
                                    techMask);
    }
    else {
        LOC_LOGD("%s:%d]: Exit", __func__, __LINE__);
        return false;
    }
}
void LBSProxy::injectFeatureConfig(ContextBase* context) const
{
    class InjectAdapter:public LBSAdapterBase {
    public:
        InjectAdapter(ContextBase *context):
            LBSAdapterBase(0, context) {}
        virtual void injectFeatureConfig() const
        {
            e_premium_mode sap_mode=DISABLED;
            e_premium_mode gtp_cell_mode=DISABLED;
            char conf_feature_gtp_cell_proc[32];
            char conf_feature_gtp_cell[32];
            char conf_feature_sap[32];
            loc_param_s_type izat_conf_feature_table[] =
            {
                {"GTP_CELL_PROC", &conf_feature_gtp_cell_proc, NULL, 's'},
                {"GTP_CELL",      &conf_feature_gtp_cell,      NULL, 's'},
                {"SAP",           &conf_feature_sap,           NULL, 's'}
            };
            UTIL_READ_CONF(IZAT_CONF_FILE, izat_conf_feature_table);

            //GTP CELL
            if(strcmp(conf_feature_gtp_cell_proc, "AP") == 0) {
                LOC_LOGD("%s:%d]: GTP_CELL_PROC set to AP", __func__, __LINE__);
            }
            //Defaulting to modem if it is set to MODEM or any other
            //random string
            else {
                LOC_LOGD("%s:%d]: GTP_CELL_PROC set to MODEM", __func__, __LINE__);
                if(strcmp(conf_feature_gtp_cell, "BASIC") == 0) {
                    LOC_LOGD("%s:%d]: GTP CELL mode set to BASIC", __func__, __LINE__);
                    gtp_cell_mode = BASIC;
                }
                else if(strcmp(conf_feature_gtp_cell, "PREMIUM") == 0) {
                    LOC_LOGE("%s:%d]: GTP CELL does not support PREMIUM mode."\
                             " Setting to default mode: BASIC", __func__, __LINE__);
                    gtp_cell_mode = BASIC;
                }
                else if(strcmp(conf_feature_gtp_cell, "DISABLED") == 0) {
                    LOC_LOGD("%s:%d]: GTP CELL mode set to DISABLED", __func__, __LINE__);
                }
                else {
                    LOC_LOGE("%s:%d]: Unrecognized value for GTP CELL Mode." \
                             " Setting GTP CELL to default mode: BASIC", __func__, __LINE__);
                    gtp_cell_mode = BASIC;
                }
            }

            //SAP
            if(strcmp(conf_feature_sap, "BASIC") == 0) {
                sap_mode = BASIC;
                LOC_LOGD("%s:%d]: Setting SAP to mode: BASIC", __func__, __LINE__);
            }
            else if(strcmp(conf_feature_sap, "PREMIUM") == 0) {
                LOC_LOGD("%s:%d]: Setting SAP to mode: PREMIUM", __func__, __LINE__);
                sap_mode = PREMIUM;
            }
            else if(strcmp(conf_feature_sap, "DISABLED") == 0) {
                LOC_LOGD("%s:%d]: Setting SAP to mode: DISABLED", __func__, __LINE__);
            }
            else {
                LOC_LOGE("%s:%d]: Unrecognized value for SAP Mode."     \
                         " Setting SAP to default mode: BASIC", __func__, __LINE__);
                sap_mode = BASIC;
            }

            mLBSApi->injectFeatureConfig(sap_mode, gtp_cell_mode);
        }
    };

    struct LBSProxyInjectFeatureConfig : public LocMsg {
        InjectAdapter mAdapter;
        inline LBSProxyInjectFeatureConfig(ContextBase *context) :
            LocMsg(), mAdapter(context)
        {
            locallog();
        }
        virtual void proc() const {
            mAdapter.injectFeatureConfig();
        }
        virtual void locallog()
        {
            LOC_LOGD("%s:%d]:LBSProxyInjectFeatureConfig", __func__, __LINE__);
        }
    };
    LOC_LOGD("%s:%d]: Enter", __func__, __LINE__);
    context->sendMsg(new LBSProxyInjectFeatureConfig(context));
    LOC_LOGD("%s:%d]: Exit", __func__, __LINE__);
}

bool LBSProxy :: reportPositionsToUlp(const FlpExtLocation * locations,
                                      int32_t number_of_locations)
{
    LOC_LOGD("%s:%d]: Enter", __func__, __LINE__);
    if(mUlp != NULL) {
        LOC_LOGD("%s:%d]: Exit. Sent to ulp", __func__, __LINE__);
        return mUlp->reportPositions(locations,
                                     number_of_locations);
    }
    else {
        LOC_LOGD("%s:%d]: Exit", __func__, __LINE__);
        return false;
    }
}

bool LBSProxy :: reportBatchingSessionToUlp(FlpExtBatchOptions &options,
                                            bool active)
{
    LOC_LOGD("%s:%d]: Enter", __func__, __LINE__);
    if(mUlp != NULL) {
        LOC_LOGD("%s:%d]: Exit. Sent to ulp", __func__, __LINE__);
        return mUlp->reportBatchingSession(options, active);
    }
    else {
        LOC_LOGD("%s:%d]: Exit", __func__, __LINE__);
        return false;
    }
}

}  // namespace lbs_core

extern "C" {
LBSProxyBase* getLBSProxy() {
    return new lbs_core::LBSProxy();
}
}
