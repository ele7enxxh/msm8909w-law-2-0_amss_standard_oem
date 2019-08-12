#ifndef SNS_SAM_H
#define SNS_SAM_H

/*============================================================================
  @file sns_sam.h

  @brief
  Data structures and constants used by the SAM Framework.  These APIs should
  only be used internally to the framework, and not by the algorithms or
  external modules.

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*===========================================================================
  Include Files
  ===========================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "sns_common.h"
#include "qmi_idl_lib.h"
#include "sns_debug_str.h"
#include "fixed_point.h"
#include "sns_queue.h"
#include "sns_usmr.h"
#include "sns_em.h"
#include "sns_common_v01.h"
#include "sns_sam_algo_api.h"
#include "sns_sam_prov_api.h"
#include "sns_sam_pm.h"
#include "sns_dl.h"

/*============================================================================
  Preprocessor Definitions and Constants
  ===========================================================================*/

/**
 * All algorithm services must support the following messages with specified
 * message ids.  Algorithms are only permitted to have custom versions of the
 * enable req, get report resp, report ind, and batch ind.  All other messages
 * must use the "official" structure.
 */
#define SNS_SAM_ALGO_CANCEL_REQ 0x00
#define SNS_SAM_ALGO_CANCEL_RESP 0x00
#define SNS_SAM_ALGO_VERSION_REQ 0x01
#define SNS_SAM_ALGO_VERSION_RESP 0x01
#define SNS_SAM_ALGO_ENABLE_REQ 0x02
#define SNS_SAM_ALGO_ENABLE_RESP 0x02
#define SNS_SAM_ALGO_DISABLE_REQ 0x03
#define SNS_SAM_ALGO_DISABLE_RESP 0x03
#define SNS_SAM_ALGO_GET_REPORT_REQ 0x04
#define SNS_SAM_ALGO_GET_REPORT_RESP 0x04
#define SNS_SAM_ALGO_REPORT_IND 0x05
#define SNS_SAM_ALGO_ERROR_IND 0x06
#define SNS_SAM_ALGO_UPDATE_REQ 0x20
#define SNS_SAM_ALGO_UPDATE_RESP 0x20
#define SNS_SAM_ALGO_BATCH_REQ 0x21
#define SNS_SAM_ALGO_BATCH_RESP 0x21
#define SNS_SAM_ALGO_BATCH_IND 0x22
#define SNS_SAM_ALGO_UPDATE_BATCH_PERIOD_REQ 0x23
#define SNS_SAM_ALGO_UPDATE_BATCH_PERIOD_RESP 0x23
#define SNS_SAM_ALGO_GET_ATTRIB_REQ 0x24
#define SNS_SAM_ALGO_GET_ATTRIB_RESP 0x24

/* Each signal should use a unique bit */
#define SNS_SAM_SVC_RESUME_SIG 0x01
#define SNS_SAM_CLIENT_TIMER_SIG 0x02
#define SNS_SAM_REG_INIT_TIMER_SIG 0x04
#define SNS_SAM_CLIENT_INIT_SIG 0x08
#define SNS_SAM_CLIENT_ERROR_SIG 0x10
// #define UNUSED 0x20
// #define UNUSED 0x40
#define SNS_SAM_REQ_MSG_SIG 0x80
#define SNS_SAM_CLNT_MSG_SIG 0x100
// #define UNUSED 0x200
#define SNS_SAM_PM_SIG 0x400
#define SNS_SAM_ALGO_TIMER_SIG 0x800
#define SNS_SAM_LOWMEM_CB_SIG 0x1000
#define SNS_SAM_SVC_DISC_SIG 0x2000
#define SNS_SAM_CLIENT_RELEASE_SIG 0x4000

#define SNS_SAM_SIGNAL_MASK (SNS_SAM_REQ_MSG_SIG | SNS_SAM_CLNT_MSG_SIG |   \
        SNS_SAM_CLIENT_TIMER_SIG | SNS_SAM_REG_INIT_TIMER_SIG |             \
        SNS_SAM_CLIENT_INIT_SIG | SNS_SAM_CLIENT_ERROR_SIG |                \
        SNS_SAM_SVC_RESUME_SIG | SNS_SAM_PM_SIG |                           \
        SNS_SAM_ALGO_TIMER_SIG | SNS_SAM_LOWMEM_CB_SIG |                    \
        SNS_SAM_SVC_DISC_SIG | SNS_SAM_CLIENT_RELEASE_SIG )

#if defined(SNS_DSPS_BUILD) | defined(FAST_PLAYBACK)
# define SNS_SAM_MODULE              SNS_MODULE_DSPS_SAM
# define SNS_SAM_MODULE_PRIORITY     SNS_MODULE_PRI_DSPS_SAM
# define SNS_SAM_DBG_MOD             SNS_DBG_MOD_DSPS_SAM
# define SNS_SAM_MODULE_STK_SIZE     SNS_MODULE_STK_SIZE_DSPS_SAM
# define SNS_SAM_PROCESSOR_TYPE      SNS_PROC_SSC_V01
# define SNS_SAM_SMR_SVC_PRI         SNS_SMR_SVC_PRI_MED
#else
# define SNS_SAM_MODULE              SNS_MODULE_APPS_SAM
# define SNS_SAM_MODULE_PRIORITY     SNS_MODULE_PRI_APPS_SAM
# define SNS_SAM_MR_MODULE_PRIORITY  SNS_MODULE_PRI_APPS_SAM_MR
# define SNS_SAM_DBG_MOD             SNS_DBG_MOD_APPS_SAM
# define SNS_SAM_MODULE_STK_SIZE     SNS_MODULE_STK_SIZE_APPS_SAM
# define SNS_SAM_PROCESSOR_TYPE      SNS_PROC_APPS_V01
# define SNS_SAM_SMR_SVC_PRI         SNS_SMR_SVC_PRI_LOW
#endif

/**
  Maximum registry groups that a single algorithm might read.
*/
#define SNS_SAM_MAX_REG_GRP 3

/** Maximum dependent sensors (SMGR + SAM). */
#define SNS_SAM_MAX_DEPENDENT_SENSOR 10

/* Randomly generated 32 bit number used to verify that a client request as valid */
#define SAM_CLIENT_REQ_MAGIC_KEY 0xee626604


/*============================================================================
  External Objects
  ===========================================================================*/

/* The processor on which this instance of SAM is running. */
extern sns_proc_type_e_v01 localProcessor;

/* SAM module ID to be used for debug string messages */
extern sns_debug_module_id_e samModule;

/* Contains all active algorithm instances. List is sorted such that bigImage
 * algorithms are first, uImage algorithms last. */
extern sns_q_s algoInstQ;
/* Contains all local and remote algorithms and other sensors (i.e. SMGR).
 * All local SAM algorithms will be located at the beginning of this queue. */
extern sns_q_s sensorQ;
/* Report Timers that are active*/
extern sns_q_s samReportTimersQ;

/* SAM event signal */
extern OS_FLAG_GRP *sns_sam_sig_event;

/* Callback functions available for all algorithm instances */
extern sns_sam_algo_callback cbFuncs;

/*============================================================================
  Type Declarations
  ===========================================================================*/

/* Size of an error string related to dynamic loading of algos */
#define SAM_DL_ERROR_STR_SIZE 32

struct sns_sam_dl_info
{
  /* Algorithm registration symbol */
  char entrySymbol[ SNS_DSO_LOAD_SYM_NAME_SZ ];
  /* Full file path to the algorithm's .so file */
  char fullFilePath[ SNS_DSO_FILEPATH_SZ ];
  /* Error string containing any error information */
  char errStr[ SAM_DL_ERROR_STR_SIZE ];
  /* Load attribute specified when loading the algo */
  uint32_t loadAttr;
  /* File handle of the loaded algorithm */
  void const *fileHandle;
  /* A flag determining if the SNS algo was loaded in uimage or big image */
  bool uImageAlgo;
};
typedef struct sns_sam_dl_info sns_sam_dl_info;

/**
 * Refers to a SAM algorithm that exists in this instance of the SAM
 * Framework.  Conceptually, this is a subclass of sns_sam_sensor.
 */
struct sns_sam_sensor_algo
{
  sns_sam_sensor sensor;

  /* Algorithm API, implemented per algorithm */
  struct sns_sam_algo_api *algoAPI;
  /* Algorithm-provided API which translates custom request/etc. messages */
  struct sns_sam_algo_msg_api *algoMsgAPI;
  /* Outstanding registry groups requested from the algorithm; or -1 */
  sns_sam_reg_group registryGroups[ SNS_SAM_MAX_REG_GRP ];
  /* Sensors that *must* exist for this algorithm to operate */
  struct sns_sam_sensor *dependencies[ SNS_SAM_MAX_DEPENDENT_SENSOR ];
  /* Size of the various static buffers used by the algorithm API */
  sns_sam_algo_const_buf_size bufferSizes;
  /* Persistant data across algorithm instances (i.e. registry data);
   * no concurrency protection */
  sns_sam_algo_persist persistData;
  /* SMR Service handle for this local SAM algorithm */
  smr_service_hndl serviceProvider;
  /* Indication message size */
  uint32_t qmiIndSize;
  /* Batch indication message size */
  uint32_t qmiBatchIndSize;
};
typedef struct sns_sam_sensor_algo sns_sam_sensor_algo;

/**
 * A fixed size buffer containing an array of input/output samples.  The
 * structure of each sample is specified and managed by the algorithm. Each is
 * allocated from the input/output memory manager.
 */
struct sns_sam_algo_bin
{
  /* Data field necessary to add this object to a SAM list */
  sns_q_link_s qLink;

  /* Current write byte offset into buffer */
  uint16_t writeOffset;
  /* Current read byte offset into buffer */
  uint16_t readOffset;
  /* Total size of buffer, in bytes */
  uint32_t bufferSize;
  /* Beginning of the buffer.  (Must be 4-byte aligned) */
  uint8_t buffer[1];
};
typedef struct sns_sam_algo_bin sns_sam_algo_bin;

/**
 * A queue of buffered input data for an algorithm, of a specific and single
 * sensor type */
struct sns_sam_algo_input_q
{
  /* Data field necessary to add this object to a SAM list */
  sns_q_link_s qLink;

  /* Type of sensor data managed by this queue. */
  sns_sam_sensor_uid const *sensorUID;
  /* The total count of input data managed by this queue */
  uint32_t inputCnt;
  /* Queue of data buffers, each of which contain some number of input data
   * objects.  Queue elements have type sns_sam_algo_bin.  The buffer within
   * each bin will be an array of sns_sam_algo_input. */
  sns_q_s inputDataQ;
  sns_sam_timestamp lastInputTs;
};
typedef struct sns_sam_algo_input_q sns_sam_algo_input_q;

/**
 * A valid instance of a SAM algorithm.  May be shared amongst client requests.
 */
struct sns_sam_algo_inst
{
  /* Data field necessary to add this object to a SAM list */
  sns_q_link_s qLink;

  /* Corresponding algorithm */
  struct sns_sam_sensor_algo *algorithm;
  /* Queue of associated sam_client_req's */
  sns_q_s clientRequests;

  /* Algorithm-specific timer set via sns_sam_algo_timer_reg() */
  sns_em_timer_obj_t algoTimer;
  /* Callback function associated with algoTimer */
  sns_sam_algo_timer_cb algoTimerCB;

  /* Size of the various buffers used by the algorithm API */
  sns_sam_algo_buf_size bufferSizes;
  /* Opaque handle used by algorithms */
  sns_sam_algo_state algoStateData;
  /* List of sns_sam_algo_input_q objects */
  sns_q_s algoInputQ;
  /* BIGIMAGE - Preallocated IO, state, or code is located in DDR
   * NOMEM_UIMAGE - At least one item in the input/output buffer is in DDR
   * UIMAGE - All memory is in TCM; algorithm may safely run in uImage */
  sns_sam_algoinst_uimage_mode imageMode;
};
typedef struct sns_sam_algo_inst sns_sam_algo_inst;

/**
 * An instance of this object is allocated and added to the client's flushReqQ
 * whenever a new flush is in progress for that client.  Flushes prompted by
 * an external request will always involve the allocation of a new object.
 * Flushes prompted by batch timers or hitting the maximum batch size, will not
 * involve a new object if there is already an existing flush in progress.
 */
struct sns_sam_flush_req
{
  /* Data field necessary to add this object to a SAM list */
  sns_q_link_s qLink;

  /* SMR request handle as received in sns_sam_process_req.  Will be NULL
   * if this flush was not prompted by a request (e.g. batch timer). */
  smr_req_handle reqHndl;
  /* The report/batch rate change was deferred due to a flush-in-progress.
   * We need to inform the algorithm of the change once the flush complete.
   * FX_MIN_Q16 if no change is made. */
  q16_t batchPeriod;
};
typedef struct sns_sam_flush_req sns_sam_flush_req;

/**
 * A request for a SAM service from some client.  The client may be external,
 * or internal to the SSC or from another SAM service.
 */
struct sam_client_req
{
  /* Data field necessary to add this object to a SAM list */
  sns_q_link_s qLink;

  /* Algorithm instance created or used to service this client */
  struct sns_sam_algo_inst *algoInstance;
  /* Instance ID returned to the sensors client to refer to this stream. */
  uint32_t extInstanceID;
  /* SMR handle to be used to send reports to the associated client */
  smr_qmi_client_handle serviceHndl;
  /* Corresponding algorithm */
  struct sns_sam_sensor_algo const *algorithm;
  /* A client event has occurred; SAM will initiate the flush procedure */
  sns_em_timer_obj_t clientTimer;
  /* Maximum batching period requested; copied from batch_req->batch_period */
  q16_t batchPeriod;
  /* A 32 bit key to verify that a client request is valid */
  uint32_t magicNumber;
  /* Attributes describing the client's request */
  sns_sam_client_attr clientAttr;
  /* Queue of output data that will be sent to the client during the next
   * report or batching period. (Type: sns_sam_algo_bin) */
  sns_q_s outputDataQ;
  /* Number of actual samples in the outputDataQ */
  uint32_t outputSampleCnt;
  /* Queue of in-progress flush requests. */
  sns_q_s flushReqQ;
  /* If batching, what was the type of the last indication sucessfully sent. */
  sns_sam_batch_ind_type lastIndType;
  /* The timestamp of the next periodic report */
  sns_sam_timestamp nextReportTS;
  /* If the client is busy; whether to send indications at the moment */
  bool clientBusy;
  /* An event was not sent to the client; either AP suspend or client busy */
  bool missedEvent;
};
typedef struct sam_client_req sam_client_req;

/* For incoming request messages to the SAM Framework */
struct sns_sam_req_msg
{
  /* Data field necessary to add this object to a SAM list */
  sns_q_link_s qLink;

  /* The actual message and its meta data */
  struct sns_sam_msg msg;
  /* The SMR request handle associated with the request message */
  smr_req_handle reqHndl;
  /* SMR handle to the client connection that initiated this request */
  smr_qmi_client_handle serviceHndl;
  /* Memory allocated by sns_sam_service to hold the response message */
  struct sns_sam_msg respMsg;
};
typedef struct sns_sam_req_msg sns_sam_req_msg;

/* For incoming response and indication messages to the SAM Framework */
struct sns_sam_msg_int
{
  /* Data field necessary to add this object to a SAM list */
  sns_q_link_s qLink;

  /* The actual message and its meta data */
  struct sns_sam_msg msg;
  /* The sensor request associated with this message */
  sns_sam_sensor_req *sensorReq;
  /* True if this is a response message; false if an indication */
  bool isResp;
};
typedef struct sns_sam_msg_int sns_sam_resp;
typedef struct sns_sam_msg_int sns_sam_ind;

/*============================================================================
  Internal Function Declarations
  ===========================================================================*/

/**
 * Looks-up a local SAM algorithm from a sensor UID.
 *
 * @param[i] sensorUID Sensor to look-up
 *
 * @return Algorithm pointer, or NULL if not found.
 */
sns_sam_sensor_algo* sns_sam_lookup_algo( sns_sam_sensor_uid const *sensorUID );

/**
 * Lookup a sensor based on the SensorUID from the "sensorQ".
 * The sensor may be a local or remote SAM algorithm, SMGR sensor, etc..
 *
 * @param[i] sensorUID Sensor to look-up
 *
 * @return Pointer to sensor object if found; NULL otherwise
 */
sns_sam_sensor* sns_sam_lookup_sensor_from_suid( sns_sam_sensor_uid const *sensorUID );

/**
 * Check if an algorithm is uimage capable or not. For static algos these
 * have been preconfigured. For dynamic algos, it was determined at
 * runtime where the SNS algo was loaded.
 *
 * @param[i] algo Algorithm to lookup
 *
 * @return SNS_SAM_ALGOINST_UIMAGE or SNS_SAM_ALGOINST_BIGIMAGE
 */
sns_sam_algoinst_uimage_mode
sns_sam_determine_uimage( sns_sam_sensor_algo const *algo );

/**
 * Compare two timestamps.  Take into account possible clock rollover.
 *
 * @param[i] ts1 Timestamp 1
 * @param[i] ts2 Timestamp 2
 *
 * @return -1 if ts1 is earlier than ts2
 *         0 ts1 and ts2 are equal
 *         1 if ts1 is later than ts2
 */
int sns_sam_compare_ts( sns_sam_timestamp const ts1,
    sns_sam_timestamp const ts2 );

#endif /* SNS_SAM_H */
