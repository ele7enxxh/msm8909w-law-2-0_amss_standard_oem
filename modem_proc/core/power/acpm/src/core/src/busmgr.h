/*
* Copyright (c) 2013 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.
*/

/*
 * busmgr.h
 *
 *  Created on: Feb 13, 2012
 *      Author: yrusakov
 */

#ifndef BUSMGR_H_
#define BUSMGR_H_

#include "requestmgr.h"
#include "asic.h"


/**
*@enum BUSMgrBwTypeType - define bus bw type 
*/

typedef enum{
   BUSMgr_Bw_None = 0,
   BUSMgr_Bw_Internal_Only = 1,
   BUSMgr_Bw_External_Only = 2,
   BUSMgr_Bw_Internal_External = 3,
   BUSMgr_Bw_ForceTo8bits = 0x7f
}BUSMgrBwTypeType;

/**
*struct busMgrReqDataType  - define internal data structure for bus management
*/

typedef struct
{
   AdsppmBusBWRequestValueType *pBWreq;
   AdsppmBusRouteType extRoute;  //!< Defines route for external BW using
}busMgrExtBwReqType;

/**
*struct busMgrReqDataType  - define internal data structure for bus management
*/

typedef struct
{
   AdsppmBusBWRequestValueType *pBWreq;
   AdsppmClkIdType busClk_M; //!< define AHB branch for internal data bw for master port
   AdsppmClkIdType busClk_S; //!< define AHB branch for internal data bw for slave port
   AdsppmClkIdType csrClk; //!< define AHB branch for internal control for reg programming (slave port only)
}busMgrInterBwReqType;
/**
*struct  busMgrCtxType - define bus management context
*/

typedef struct
{
   DALSYSSyncHandle       BUSMgrCtxLock;
   uint32                 extRouteNum;
   AsicBusExtRouteType    *pExtRoutes;
   uint32                 preCurrentBwIndex;
   AdsppmBusBWRequestValueType *pBUSMgrExtBWAggregate[2];
   uint32                 preCurrentClkIndex;
   uint32 AHBClkSts[2][AdsppmClk_EnumMax-1];
   busMgrExtBwReqType     externalBwReq[(MAX_NUM_OF_ADSPPM_CLIENTS-1)*MAX_NUM_OF_ADSPPM_BW_ARRAY]; //!< Array to store processing data for ext BW
   busMgrInterBwReqType   internalBwReq[(MAX_NUM_OF_ADSPPM_CLIENTS-1)*MAX_NUM_OF_ADSPPM_BW_ARRAY]; //!< Array to store processing data for int BW
}BUSMgrCtxType;


/**
*@fn BUS_Init  - bus management init
*/


Adsppm_Status BUS_Init(void);

/**
*@fn BUS_ProcessRequest  - bus management main function
*/

Adsppm_Status BUS_ProcessRequest(coreUtils_Q_Type *pBWReqQ);

#endif /* BUSMGR_H_ */
