/*==============================================================================
 
FILE:      DDITF.h
 
DESCRIPTION: This file exposes a dalified testing framework's backend code         
 
PUBLIC CLASSES:  DalTFHandle
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 
        Copyright © 2009 - 2010 Qualcomm Technologies Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
==============================================================================*/

#ifndef __DDITF_H__
#define __DDITF_H__

#include "DalDevice.h"

#define DALTF_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)

#define TF_MAX_INSTANCES 16

// Parameter Types
#define TF_PARAM_IDENTIFIER_SIZE 2  /**< -- # of bytes in each TF_PARAM_*              */
#define TF_PARAM_UINT8  "0"         /**< -- Specifies that the parameter is a uint8    */
#define TF_PARAM_UINT16 "1"         /**< -- Specifies that the parameter is a uint16   */
#define TF_PARAM_UINT32 "2"         /**< -- Specifies that the parameter is a uint32   */
#define TF_PARAM_INT8   "3"         /**< -- Specifies that the parameter is a byte     */
#define TF_PARAM_INT16  "4"         /**< -- Specifies that the parameter is a int16    */
#define TF_PARAM_INT32  "5"         /**< -- Specifies that the parameter is a int32    */
#define TF_PARAM_STRING "6"         /**< -- Specifies that the parameter is a string   */

/** @brief Testing Framework User Response Codes
   @detail
   These are the values that should be returned from test cases.
 */
typedef enum
{
   TF_RESULT_CODE_SUCCESS     = 0,     /**< -- if test case runs successfully          */
   TF_RESULT_CODE_FAILURE     = 1,     /**< -- if test case run unsuccessfully         */
   TF_RESULT_CODE_BAD_PARAM   = 2      /**< -- if test case's parameters are invalid   */
} TF_TestCaseResponseCodes ;

/** @brief Testing Framework User Response Codes
   @detail
   These are the values that should be returned from test cases.
 */
typedef enum
{
   TF_SUCCESS                 = 0, /**< -- if DalTF API completed successfully     */
   TF_ERROR_GENERIC           = 0x8001, /**< -- if DalTF API met an unknown error       */
   TF_ERROR_TESTCASE_NONEXIST = 0x8002, /**< -- if test case is not valid               */
   TF_ERROR_ALLOC             = 0x8003, /**< -- if a malloc operation failed            */
   TF_ERROR_BAD_PARAM         = 0x8004, /**< -- if a bad arg was passed to a DalTF API  */
   TF_ERROR_UNINIT            = 0x8005, /**< -- if DalTF hasnt initialized yet          */
   TF_ERROR_MSGS_DISABLED     = 0x8006, /**< -- if DalTF isnt supporting messages       */
   TF_ERROR_MSGS_DROPPED      = 0x8007, /**< -- if DalTF has dropped messages           */
   TF_ERROR_IN_USE            = 0x8008, /**< -- if DalTF is in use by another server    */
   TF_ERROR_RESYNC            = 0x8009, /**< -- if DalTF's test DB was updated          */
   TF_ERROR_TESTCASE_NOTRUN   = 0x800A, /**< -- if a TestCase is not running            */
} TF_Result ;

//Message Categories
typedef enum
{
  TF_MSG_TYPE_NONE      = 0,
  TF_MSG_TYPE_SYSTEM    = 1,
  TF_MSG_TYPE_TESTCASE  = 2,
}TF_MsgType;


// Configurable Properties
#define TF_MAX_PARAMETERS 30
#define TF_TIME_UNTIL_FIRST_RUN 50
#define TF_MIN_PERIOD 50


/** @brief Test Case Function Pointer Type
   @detail
   Function prototype of test case entry points
 */
typedef uint32 (*TF_TCFunctionPtr)(uint32 dwParam, char* apszParam[]);

/** @brief Testing Framwork Parameter Descriptions
   @detail 
   Description of a test case parameter's type, name and help/description.
 */
typedef struct 
{
   char *pszType;         /**< -- parameter's type. Values are of type TF_PARAM        */ 
   char *pszName;         /**< -- name of the parameter. Nullable                      */ 
   char *pszHelpString;   /**< -- General help info about this parameter. Nullable     */
} TF_ParamDescr;

/** @brief Testing Framework Test Context description
   @detail 
   Used to describe a test case's context: what the test case is being used on
 */
typedef struct
{
   uint8 cSubsysId;            /**< -- Subsystem id of the test case                   */
   uint16 wContextId;          /**< -- Context id of the test case                     */
} TF_ContextDescr;

/** @brief Help Description
   @detail 
   Contains information about a specific test case. 
   Parameter information does not need to be included.
 */
typedef struct 
{
   const char *pszFunctionDescr;  /**< --  Test case description. Should not be NULL    */
   uint32 dwParametersLength;      /**< --  Number of parameters expected.   Can be zero */
   const TF_ParamDescr*pParameters;/**< --  Parameter descriptions. Can be NULL         */
} TF_HelpDescr;

/** @brief Testing Framework Test Function Description
   @detail 
   Describes everything about a test case needed to run it. The testing table is 
   an array of these elements. Each one of these is used to register a single 
   test case into the Test Framework.
 */
typedef struct 
{
   const char *pszTestName;        /**< --  Unique name for the test case. Nonnullable  */
   TF_TCFunctionPtr pTestFunc;     /**< --  Pointer to test entry point. Nonnullable    */
   const TF_HelpDescr *pHelp;      /**< --  Information about test case. Nullable       */
   const TF_ContextDescr *pContext;/**< --  Context the tc desiged for. Nullable        */
} TF_TestFunction_Descr;

/** @brief Testing Framework Capability Information Structure
    @detail
    Describes everything about hte test framework, such as how
    many processors are running DalTF, each one's diag
    component, how many tests are attached to the DalTF etc
 */
 
typedef struct { 
   uint32 dwRev;                /**< -- Revision of the DalTF   */
   uint32 dwNumInstances;       /**< -- Number of instances of DalTF in the UE */
   uint32 dwInstance;           /**< -- Unique ID for this instance in the UE */
   uint32 dwNumTests;           /**< -- Number of tests with this instance     */
   uint32 dwDiagCmdCodes[TF_MAX_INSTANCES]; /**< -- Diag Cmd Codes for each instance */
   char szInstanceName[32];     /**< -- Name of the instance (proc) in the UE */
}TF_CapsDescr;



/** @brief Testing Framework Running Test Case Statistics
    @detail
    Data being collected about the currently running (or just finished) test case
 */
typedef struct _TF_TestMetrics
{
   uint32 dwMaxRuns;     /**< -- times test case should be run                        */
   uint32 dwPeriodTime;  /**< -- interval in msecs between each run                   */
   uint32 dwRunCount;    /**< -- times test case has run so far                       */
   uint32 dwSuccessCount;/**< -- times test case has run successfully so far          */      
   uint32 dwMinTime;     /**< -- shortest time this test case ran in                  */
   uint32 dwMaxTime;     /**< -- longest time this test case ran in                   */
   uint32 dwLastRunResult;       /**< -- if the test case parameters are invalid     */
   uint8  bStopOnError;   /**< -- Stop the testcase stress if an error happened?       */
} TF_TestMetrics;


/** @brief Testing Framework Message Payload
    @detail 
    This structure is used to store messages in the messaging 
    queue. It is also returned in a message response packet.
 */
typedef struct
{
   uint32 dwTestFuncId;/**< -- the id of the test case this msg corresponds to */
   uint8 msgCategory;  /**< -- the message category (TF_MSG_TYPE_XX)               */
   uint8 reserved[2];  /**< -- reserved                                        */
   char payload[1];    /**< -- the start of the variable length payload           */
} TF_Msg;

// Should not be used directly
typedef struct DalTF DalTF;
struct DalTF
{
   DalDevice DalDevice;
   DALResult (*RemoveTests)(DalDeviceHandle *, const TF_TestFunction_Descr*[], uint32);
   DALResult (*AddTests)(DalDeviceHandle *, const TF_TestFunction_Descr*[], uint32);
   DALResult (*GetCaps)(DalDeviceHandle* ,TF_CapsDescr *);
   DALResult (*GetTests)(DalDeviceHandle* ,uint32 *,void **,uint32 *);
   DALResult (*GetHelp)(DalDeviceHandle *,uint32 , uint32 * ,void **,uint32 *);
   DALResult (*StartTestCase)(DalDeviceHandle *, uint32 , uint32 , uint32 ,DALBOOL ,uint32 ,uint8 *,uint32 );
   DALResult (*StopTestCase)(DalDeviceHandle *, uint32 );
   DALResult (*GetTestCaseStatus)(DalDeviceHandle *,uint32 ,DALBOOL *,TF_TestMetrics *);
   DALResult (*GetMessage)(DalDeviceHandle *,void **,uint32 *,uint32 *);
};

// This structures fields should not be manipulated directly
typedef struct DalTFHandle DalTFHandle; 
struct DalTFHandle 
{
   uint32 dwDalHandleId;
   const DalTF * pVtbl;
   void * pClientCtxt; 
};

/* ============================================================================
**  Function : TF_Bab_MarkStart
** ========================================================================== */
/*!
   @brief
   Sets the babylon gpio to indicate that a timing interval has started.

   @param cID           - [IN] To differentiate between events
   @param cExtendedID   - [IN] User defined meaning

   @par Dependencies
   None

   @par Side Effects
   None

   @return   
   DAL_SUCCESS is the babylon marker is set. DAL_ERROR if it 
   isn't set as a result of the DalTF not being initialized 
   or the absense of BABYLON_FEATURE

   @sa  TF_Bab_MarkEnd
   @sa  TF_Bab_SingleEvent
*/
DALResult TF_Bab_MarkStart(uint8 cID, uint8 cExtendedID);

/* ============================================================================
**  Function : TF_Bab_MarkEnd
** ========================================================================== */
/*!
   @brief
   Sets the babylon gpio to indicate that a timing interval has finished.

   @param cID           - [IN] To differentiate between events
   @param cExtendedID   - [IN] User defined meaning

   @par Dependencies
   None

   @par Side Effects
   None

   @return   
   DAL_SUCCESS is the babylon marker is set. DAL_ERROR if 
   it isn't set as a result of the DalTF not being initialized 
   or the absense of BABYLON_FEATURE

   @sa  TF_Bab_MarkStart
   @sa  TF_Bab_SingleEvent
*/
DALResult TF_Bab_MarkEnd(uint8 cID, uint8 cExtendedID);

/* ============================================================================
**  Function : TF_Bab_SingleEvent
** ========================================================================== */
/*!
   @brief
   Sets the babylon gpio to mark a singular event.

   @param cID                 - [IN] To differentiate between events
   @param cExtendedID         - [IN] User defined meaning

   @par Dependencies
   None

   @par Side Effects
   None

   @return   
   DAL_SUCCESS is the babylon marker is set. DAL_ERROR if 
   it isn't set as a result of the DalTF not being initialized 
   or the absense of BABYLON_FEATURE

   @sa  TF_Bab_MarkStart
   @sa  TF_Bab_MarkEnd
*/
DALResult TF_Bab_SingleEvent(uint8 cID, uint8 cExtendedID);


/* ============================================================================
**  Function : TF_Bab_MarkStartEx
** ========================================================================== */
/*!
   @brief
   Sets the babylon gpio to indicate that a timing interval has started.

   @param cStream       - [IN] Stream ID to be used
   @param cID           - [IN] To differentiate between events
   @param cExtendedID   - [IN] User defined meaning

   @par Dependencies
   None

   @par Side Effects
   None

   @return   
   DAL_SUCCESS is the babylon marker is set. DAL_ERROR if it 
   isn't set as a result of the DalTF not being initialized 
   or the absense of BABYLON_FEATURE

   @sa  TF_Bab_MarkEnd
   @sa  TF_Bab_SingleEvent
*/
DALResult TF_Bab_MarkStartEx(uint8 cStream, uint8 cID, uint8 cExtendedID);

/* ============================================================================
**  Function : TF_Bab_MarkEndEx
** ========================================================================== */
/*!
   @brief
   Sets the babylon gpio to indicate that a timing interval has finished.

   @param cStream       - [IN] Stream ID to be used
   @param cID           - [IN] To differentiate between events
   @param cExtendedID   - [IN] User defined meaning

   @par Dependencies
   None

   @par Side Effects
   None

   @return   
   DAL_SUCCESS is the babylon marker is set. DAL_ERROR if 
   it isn't set as a result of the DalTF not being initialized 
   or the absense of BABYLON_FEATURE

   @sa  TF_Bab_MarkStart
   @sa  TF_Bab_SingleEvent
*/
DALResult TF_Bab_MarkEndEx(uint8 cStream, uint8 cID, uint8 cExtendedID);

/* ============================================================================
**  Function : TF_Bab_SingleEventEx
** ========================================================================== */
/*!
   @brief
   Sets the babylon gpio to mark a singular event.

   @param cStream             - [IN] Stream ID to be used
   @param cID                 - [IN] To differentiate between events
   @param cExtendedID         - [IN] User defined meaning

   @par Dependencies
   None

   @par Side Effects
   None

   @return   
   DAL_SUCCESS is the babylon marker is set. DAL_ERROR if 
   it isn't set as a result of the DalTF not being initialized 
   or the absense of BABYLON_FEATURE

   @sa  TF_Bab_MarkStart
   @sa  TF_Bab_MarkEnd
*/
DALResult TF_Bab_SingleEventEx(uint8 cStream, uint8 cID, uint8 cExtendedID);


/* ============================================================================
**  Function : DalTF_Msg
** ========================================================================== */
/*!
   @brief
   Send a message to the test log. Messages must be under 255 characters and 
   should typically be much less. Since the message queue is limited in size,
   message delivery is NOT guaranteed.

   By setting the msg_to_qxdm property, a copy of the message is also sent to
   QXDM.

   @param format         - [IN] string formatting
   @param ...            - [IN] additional arguments that may be expected

   @par Dependencies
   None

   @par Side Effects
   None

   @return   None
*/
void DalTF_Msg(const char * msg, ...);

/* ============================================================================
**  Function : DAL_TFDeviceAttach
** ========================================================================== */
/*!
   @brief
   Establish communication link with the testing framework

   @param DevID         - [IN] DalTF's Device Id
   @param hDalDevice    - [OUT] Contains DalTF Handle, 
                                if return code is DAL_SUCCESS

   @par Dependencies
   None

   @par Side Effects
   None

   @return DAL_SUCCESS when the device attaches successfully, DAL_ERROR otherwise
*/
#define DAL_TFDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALTF_INTERFACE_VERSION,hDalDevice)

/* ============================================================================
**  Function : DalTF_RemoveTests
** ========================================================================== */
/*!
   @brief
   Finds and removes pointers to test cases in the test case table.

   @param _h               - [IN] Handle to the DalTF logical device
   @param apTestsToAdd     - [IN] Test cases to be removed from DalTF
   @param dwTestsToRemove   - [IN] Number of elements in the apTestsToAdd array

   @par Dependencies
   None

   @par Side Effects
   None

   @return DAL_SUCCESS if all test cases could be removed, DAL_ERROR otherwise

   @sa DalTF_AddTests
*/
static __inline DALResult
DalTF_RemoveTests(DalDeviceHandle * _h, const TF_TestFunction_Descr* apTests[], 
              uint32  dwTestsToRemove)
{
   return ((DalTFHandle *)_h)->pVtbl->RemoveTests( _h, apTests, dwTestsToRemove);
}

/* ============================================================================
**  Function : DalTF_AddTests
** ========================================================================== */
/*!
   @brief
   Adds a number of test cases to the testing 
   framework so they can be called from diag

   @param _h          - [IN] Handle to the DalTF logical device
   @param apTests     - [IN] test case descriptions to be added to DalTF
   @param dwToAdd     - [IN] The number of elements in the apTestsToAdd array

   @par Dependencies
   None

   @par Side Effects
   None

   @return returns returns one of the following error codes
            > TF_SUCCESS      - if all test cases could be added, DAL_ERROR otherwise
            > TF_ERROR_UNINIT - if DalTF hasn't yet initialized
            > TF_ERROR_ALLOC  - if unable to add test cases due to out of memory
   @sa DalTF_RemoveTests
*/
static __inline DALResult
DalTF_AddTests(DalDeviceHandle* _h, 
            const TF_TestFunction_Descr* apTests[], uint32 dwToAdd)
{
   return ((DalTFHandle *)_h)->pVtbl->AddTests( _h, apTests, dwToAdd);
}

/* ============================================================================
**  Function : DalTF_GetCaps
** ============================================================================
*/
/*!
    @brief
    Retrieves the capabilities information from the DalTF
    
    @details
    Retrieves the capabilities information from the DalTF. The information
    would typically include number of servers present in the system, instance
    of this server etc.
 
    @param *_h      -  [IN]  Handle to the DalTF logical device
    @param *pCaps   -  [OUT] Structure to be filled with the capability info

    @par Dependencies
    None
    
    @par Side Effects
    None
    
    @retval returns returns one of the following error codes
            > TF_SUCCESS      - Capabilities were retrieved successfully
            > TF_ERROR_UNINIT - if DalTF hasn't yet initialized
            > DAL Error Codes - Errors occuring in DAL APIs
    
    @sa None
*/
static __inline DALResult
DalTF_GetCaps(DalDeviceHandle* _h,
               TF_CapsDescr *pCaps)
{
    return ((DalTFHandle *)_h)->pVtbl->GetCaps( _h, pCaps);
}


/* ============================================================================
**  Function : DalTF_GetTests
** ============================================================================
*/
/*!
    @brief
    Gets the tests list from DalTF
    
    @details
    Gets the information about the tests added to the DalTF Test Database.
    This is a very expensive operation and should be done minimally. It is
    also the responsibility of the caller to free the buffer allocated for
    ppSyncBuf. The buffer is a packed structure expected to be transportable
    across user-kernel space boundaries and to other remote targets (like a PC).
 
    The format of the List Buffer is as follows:
     Bytes           Item
    -------------------------------------------------
     [Variable Size] Test 0 Name
     [2]             Test 0 Index
     [1]             Valid context info Bool for Test 0
     [1]             Subsys ID for Test 0
     [2]             Context ID for Test 0
     [Variable Size] Test 2 Name
     [2]             Test 2 Index
        .....
     [1]             Valid context info Bool for Test (dwNumTests-1)
     [1]             Subsys ID for Test (dwNumTests-1)
     [2]             Context ID for Test (dwNumTests-1)

    @param *_h              -  [IN ] Handle to the DalTF logical device
    @param *dwNumTests      -  [OUT] Number of tests in the database
    @param **ppSyncBuf      -  [OUT] List containing the info on the tests 
    @param *dwSyncBufSize   -  [OUT] Size of the buffer containing the list

    @par Dependencies
    None
    
    @par Side Effects
    None
    
    @retval returns one of the following error codes
            > TF_SUCCESS      - Test DB was successfully retrieved
            > TF_ERROR_UNINIT - if DalTF hasn't yet initialized
            > TF_ERROR_ALLOC  - Memory allocation error 
            > DAL Error Codes - Errors occuring in DAL APIs

    
    @sa None
*/

static __inline DALResult
DalTF_GetTests(DalDeviceHandle* _h,
               uint32 *dwNumTests,void **ppSyncBuf,uint32 *dwSyncBufSize)
{
    return ((DalTFHandle *)_h)->pVtbl->GetTests( _h, dwNumTests,ppSyncBuf,dwSyncBufSize);
}

/* ============================================================================
**  Function : DalTF_GetHelp
** ============================================================================
*/
/*!
    @brief
    Gets the help information for the testcase 
    
    @details
    Gets the help information for the testcase given by testcase id. This is a
    very expensive operation and should be done minimally. It is also the
    responsibility of the caller to free the buffer allocated for
    ppHelpBuf. The buffer is a packed structure expected to be transportable
    across user-kernel space boundaries and to other remote targets (like a PC).
 
    The format of the List Buffer is as follows:
     Bytes           Item
    ---------------------------------------------------------------------------
    [Variable Size]  Test Case Help Description
    [2]              Param 0 Type - defined in TF_PARAM*
    [Variable Size]  Param 0 Name
    [Variable Size]  Param 0 Description
    ...
    [2]              Param (((dwNumStrings-1)/3)-1) Type - defined in TF_PARAM*
    [Variable Size]  Param (((dwNumStrings-1)/3)-1) Name
    [Variable Size]  Param (((dwNumStrings-1)/3)-1) Description
    
    @param *_h              -  [IN ] Handle to the DalTF logical device
    @param testCaseID       -  [IN ] Test Case for which help is requested
    @param *dwNumStrings    -  [OUT] Number of strings in the buffer
    @param **ppHelpBuf      -  [OUT] Pointer to the buffer containing the strings 
    @param *dwHelpBufSize   -  [OUT] Size of the buffer

    @par Dependencies
    None
    
    @par Side Effects
    None
    
    @retval returns one of the following error codes
            > TF_SUCCESS                  - Help was retrieved succesfully
            > TF_ERROR_UNINIT             - If DalTF hasn't yet initialized
            > TF_ERROR_TESTCASE_NONEXIST  - Test case was not found in the DB.
            > TF_ERROR_ALLOC              - Memory allocation error 
            > DAL Error Codes             - Errors occuring in DAL APIs
    
    @sa None
*/
static __inline DALResult
DalTF_GetHelp(DalDeviceHandle *_h,uint32 testCaseID, uint32 *dwNumStrings,void **ppHelpBuf,uint32 *dwHelpBufSize)
{
    return ((DalTFHandle *)_h)->pVtbl->GetHelp( _h, testCaseID, dwNumStrings,ppHelpBuf,dwHelpBufSize);
}


/* ============================================================================
**  Function : DalTF_StartTestCase
** ============================================================================
*/
/*!
    @brief
    Start a test case
    
    @details
    Start a test case with the characteristics passed as arguments. This is an
    asynchronous call. The testcase will be queued to the Workloop DalTF uses
    to execute Tests.Only one test can be run at any moment by DalTF. If there
    was test already running, it will return DAL_ERROR for any subsequent
    StartTestCase requests
 
    The arguments buffer is a packed buffer containing all the strings
    concatenated and separated by the NULL character.
    
    @param *_h            -  [IN ] Handle to the DalTF logical device
    @param dwTestCaseID   -  [IN ] Unique ID for the test case to be run
    @param dwMaxRuns      -  [IN ] Max Iterations the test is to be run
    @param dwPeriodTime   -  [IN ] Time between two runs in millseconds
    @param bStopOnError   -  [IN ] Stop further iterations on error
    @param dwNumArgs      -  [IN ] Number of args to the test case
    @param *pArgBuf       -  [OUT] Buffer containing all the args to the test
    @param dwArgBufSize   -  [IN ] Size of the buffer

    @par Dependencies
    None
    
    @par Side Effects
    None
    
    @retval returns one of the following error codes
            > TF_SUCCESS                  - Test case was succesfully started
            > TF_ERROR_UNINIT             - If DalTF hasn't yet initialized
            > TF_ERROR_TESTCASE_NONEXIST  - Test case was not found in the DB.
            > TF_ERROR_ALLOC              - Memory allocation error 
            > TF_ERROR_GENERIC            - Test case was not started
            > DAL Error Codes             - Errors occuring in DAL APIs
 
    @sa None
*/
static __inline DALResult
DalTF_StartTestCase(DalDeviceHandle *_h,uint32 dwTestCaseID, uint32 dwMaxRuns, uint32 dwPeriodTime, DALBOOL bStopOnError, uint32 dwNumArgs, uint8 *pArgBuf, uint32 dwArgBufSize)
{
    return ((DalTFHandle *)_h)->pVtbl->StartTestCase(_h, dwTestCaseID, dwMaxRuns, dwPeriodTime, bStopOnError, dwNumArgs, pArgBuf, dwArgBufSize);
}

/* ============================================================================
**  Function : DalTF_GetTestCaseStatus
** ============================================================================
*/
/*!
    @brief
    Gets the status of the test case
    
    @details
    Gets the status of the test case. This will include whether the test is
    running or not. At the moment, the DalTF is a uni-threaded system
    and so only one test can run at any moment. So dwTestCaseID is reserved
    for future use.
    
    @param *_h             -  [IN ] Handle to the DalTF logical device
    @param dwTestCaseID    -  [IN ] Test whose status requested (reserved)
    @param *bIsRunning     -  [OUT] Set if test is running
    @param *pTestMetrics   -  [OUT] Metrics for the running/ran test 

    @par Dependencies
    None
    
    @par Side Effects
    None
    
    @retval returns one of the following codes:
            > TF_SUCCESS         - If Metrics were retrieved successfully
            > TF_ERROR_UNINIT    - If DalTF hasn't yet initialized
            > TF_ERROR_BAD_PARAM - If any of the argument was null
            > DAL Error Codes    - Errors occuring in DAL APIs
    
    @sa None
*/
static __inline DALResult 
DalTF_GetTestCaseStatus(DalDeviceHandle *_h,uint32 dwTestCaseID, DALBOOL *bIsRunning,TF_TestMetrics *pTestMetrics)
{
    return ((DalTFHandle *)_h)->pVtbl->GetTestCaseStatus(_h,dwTestCaseID,bIsRunning,pTestMetrics);
}

/* ============================================================================
**  Function : DalTF_StopTestCase
** ============================================================================
*/
/*!
    @brief
    Stops a test case
    
    @details
    Stops the currently running test case after it has completed its current
    run. It is not possible to abort the current run at the moment. This is a
    system limitation. Also at the moment, the DalTF is a uni-threaded system
    and so only one test can run at any moment. So dwTestCaseID is reserved
    for future use.
    
    @param *_h            -  [IN] Handle to the DalTF logical device
    @param dwTestCaseID   -  [IN] Test which is to be stopped (reserved)
    

    @par Dependencies
    None
    
    @par Side Effects
    None
    
    @retval returns one of the following codes:
            > TF_SUCCESS       - if test was stopped
            > TF_ERROR_UNINIT  - if DalTF hasn't yet initialized
            > TF_ERROR_GENERIC - if test had already stopped 
            > DAL Error Codes  - Errors occuring in DAL APIs
    
    @sa None
*/

static __inline DALResult 
DalTF_StopTestCase(DalDeviceHandle *_h, uint32 dwTestCaseID)
{
    return ((DalTFHandle *)_h)->pVtbl->StopTestCase(_h, dwTestCaseID);
}

/* ============================================================================
**  Function : DalTF_GetMessage
** ============================================================================
*/
/*!
    @brief
    Brief_Description_Goes_here
    
    @details
    Detailed_Description_Goes_here
    
    @param *_h           -  [IN ] Handle to the DalTF logical device
    @param **ppMsgBuf    -  [OUT] Pointer to the buffer containing a message
    @param *dwMsgSize    -  [OUT] Size of the message buffer
    @param *dwMsgCount   -  [OUT] Number of Msgs in the DalTF MsgQ including 
                                  this message
    @par Dependencies
    Messaging must have been configured to be operational by DalTF
    
    @par Side Effects
    None
    
    @retval returns one of the following codes:
            > DAL_SUCCESS            - retrieved a msg or there were no msgs.
            > TF_ERROR_UNINIT        - if DalTF hasn't yet initialized
            > TF_ERROR_MSGS_DROPPED  - Same as DAL_SUCCESS except that DalTF 
                                       had to drop a few msgs from its MsgQ to
                                       accomodate newer msgs
            > TF_ERROR_MSGS_DISABLED - Either DalTF Msging system isnt
                                       configured or encountered an error. All
                                       subsequent msgs will be dropped.
            > TF_ERROR_ALLOC         - Couldnt allocate space for msg buffer to
                                       be returned
            > DAL Error Codes        - Errors occuring in DAL APIs

    
    @sa DalTF_Msg
*/
static __inline DALResult DalTF_GetMessage(DalDeviceHandle *_h,void **ppMsgBuf,uint32 *dwMsgSize,uint32 *dwMsgCount)
{
   return ((DalTFHandle *)_h)->pVtbl->GetMessage(_h,ppMsgBuf,dwMsgSize,dwMsgCount);
}
#endif
