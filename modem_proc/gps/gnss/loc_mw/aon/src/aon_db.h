#ifndef AON_DB_H
#define AON_DB_H
/*============================================================================
  @file aon_db.h

  @brief
    This file contains API definitions by which AON transaction database can
    be accessed & modified
 
               Copyright (c) 2014 - 2015 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               QUALCOMM Proprietary and Confidential.
			   Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
               Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/
/*============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/aon/src/aon_db.h#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  ----------------------------------------------------------- 
09/30/15    sj    Added hysterisis for unsolicited status indications 
04/06/15    sj    LB 2.0 integration 
02/10/15    sj    Configured max geofences needed by AON from GM 
01/14/15    sj    Added copyright info 
07/31/14    sj    Initial creation of file. 
=============================================================================*/
#include "comdef.h"
#include "aon_api.h"
#include "aries_os_api.h"

//Maximum concurrent Background engine transactions supported
#define AON_DB_MAX_GF_TRANSACTIONS (100)

class AonTransac;
class AonGmClientConfig;
class AonGeofenceConfig;
class AonGmProxy;

class AonDatabase {

public:
    //Singleton design pattern since only one instance for
    //managing generic AON operations
    AonDatabase();
    static AonDatabase * Instance();
	boolean addAonTransac(AonTransac* aonTransac);
	boolean delAonTransac(AonTransac* aonTransac);
	AonTransac *getAonTransacByReqId(uint32 reqId);
	AonTransac *getAonTransacByGfId(uint32 gfId);	
	boolean broadcastStatusInfo(aonSessionStatusType status);
    void timerHandler();
		
private:
    static AonDatabase* m_pInstance;
	//client and their transactions
    AonTransac* m_TransacInfoMap[AON_DB_MAX_GF_TRANSACTIONS];
	boolean setCommonConfig();
    os_TimerMsgType* mSessionStatusHystTimer;
    aonSessionStatusType mCurrentStatus;
    aonSessionStatusType mReportedStatus;
    boolean reportStatusToAllTransac(aonSessionStatusType status);
};

#endif 
/* AON_DB_H */

