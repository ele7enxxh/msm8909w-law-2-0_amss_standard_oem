/*============================================================================
  @file aon_db.cpp

  @brief
    This file contains the implementation for Alwayson service data base.
 
               Copyright (c) 2014 QUALCOMM Atheros, Inc.
               All Rights Reserved.
               QUALCOMM Proprietary and Confidential.
			   Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
               Confidential and Proprietary - Qualcomm Technologies, Inc.

============================================================================*/

/*============================================================================

                           EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/gps/gnss/loc_mw/aon/src/aon_db.cpp#1 $ 
  $DateTime: 2016/12/13 07:59:45 $ 
  $Author: mplcsds1 $ 



when        who  what, where, why
----------  ---  ----------------------------------------------------------- 
09/30/15    sj    Added hysterisis for unsolicited status indications  
04/06/15    sj    LB 2.0 integration
07/31/14    sj    Initial creation of file.


=============================================================================*/
/*****************************************************************************
 * Include Files
 * *************************************************************************/

#include "aon_db.h"
#include "aon_transac.h"
#include "aon_gmproxy.h"
#include "loc_api_internal.h"

AonDatabase *AonDatabase::m_pInstance = 0;

AonDatabase :: AonDatabase(): mCurrentStatus(AON_ABLE_TO_TRACK),
                            mReportedStatus(AON_ABLE_TO_TRACK)
{
  for(int i = 0; i < AON_DB_MAX_GF_TRANSACTIONS ; i++)
  {
	m_TransacInfoMap[i] = NULL;
  }

  mSessionStatusHystTimer = os_TimerCreate(LOC_MIDDLEWARE_TIMER_ID_RESERVED_AON_HYSTERISIS , THREAD_ID_LOC_MIDDLEWARE);
  if( NULL == mSessionStatusHystTimer)
  {
	AON_MSG_ERROR("AonDatabase:: could not create mSessionStatusHystTimer.",0, 0, 0);
  }
}
 
AonDatabase * AonDatabase::Instance()
{
  if (!m_pInstance)   
  {
	m_pInstance = new  AonDatabase;

  } 
   
  return m_pInstance; 
}

boolean AonDatabase::addAonTransac(AonTransac* aonTransac)
{
  if(NULL == aonTransac)
  {
	AON_MSG_ERROR("AonDatabase:: addAonTransac NULL transac input", 0,0,0);
	return FALSE;
  }
  
  //store the AON transaction instance in the transaction list of the client
  uint32 ctr = 0;
  while( 0 != m_TransacInfoMap[ctr])
  {
	ctr++;
	if(AON_DB_MAX_GF_TRANSACTIONS <= ctr)
	{
	  AON_MSG_ERROR("AonDatabase::addAonTransac Client's transaction Buffer is full",
		0,0,0);
	 return FALSE;
	}
  }
  
  m_TransacInfoMap[ctr] = aonTransac;
  return (setCommonConfig());
}

AonTransac * AonDatabase::getAonTransacByReqId(uint32 reqId)
{
  AonTransac *p_transac = NULL;
    uint32 ctr = 0;
  while(ctr < AON_DB_MAX_GF_TRANSACTIONS)
  {
	p_transac = m_TransacInfoMap[ctr];
	if( (NULL != p_transac) && (reqId == p_transac->getReqId()))
	{
	  return p_transac;
	}
	ctr ++;
  }
  
  AON_MSG_ERROR("AonDatabase:: getAonTransacByReqId did not find transaction", 
	  0,0,0);
  return NULL;
}

AonTransac * AonDatabase::getAonTransacByGfId(uint32 gfId)
{
  AonTransac *p_transac = NULL;
  uint32 ctr = 0;
  while(ctr < AON_DB_MAX_GF_TRANSACTIONS)
  {
	p_transac = m_TransacInfoMap[ctr];
	if( (NULL != p_transac) && (gfId == p_transac->getGfId())
	  && (TRUE == p_transac->validGfId()) )
	{
	  return p_transac;
	}
	ctr ++;
  }
  
  AON_MSG_ERROR("AonDatabase:: getAonTransacByGfId did not find transaction", 
	  0,0,0);
  return NULL;
}

boolean AonDatabase::delAonTransac(AonTransac* aonTransac)
{
  uint32 ctr = 0;
  while(ctr < AON_DB_MAX_GF_TRANSACTIONS)
  {
	if( aonTransac == m_TransacInfoMap[ctr])
	{
	  delete (aonTransac);
	  m_TransacInfoMap[ctr] = NULL;
	  return (setCommonConfig());
	}
	ctr ++;
  }
  
  AON_MSG_ERROR("AonDatabase:: delAonTransac did not find transaction", 
	  0,0,0);
  return FALSE;
}

boolean AonDatabase::setCommonConfig()
{
  uint32 ctr = 0;
  AonGmClientConfig commonGmConfig = AonGmClientConfig();
  boolean retVal = FALSE;
  
  AonGmProxy *gmProxy = AonGmProxy::Instance();
  if(NULL == gmProxy)
  {
	AON_MSG_ERROR("AonDatabase::setCommonConfig cannot get GMProxy instance",0,0,0);
	return retVal;
  }
  
  while(ctr < AON_DB_MAX_GF_TRANSACTIONS)
  {
	AonGmClientConfig transacGmConfig;
	AonTransac* aonTransac = m_TransacInfoMap[ctr];
	if(aonTransac && (aonTransac->getEqGmClientConfig(transacGmConfig)))
	{
	  //atleast one transaction active 
	  retVal = TRUE;
	  //calculate minimum among max Bo times		 
	  if(commonGmConfig.mGmBackOffMax > transacGmConfig.mGmBackOffMax)
	  {
		commonGmConfig.mGmBackOffMax = transacGmConfig.mGmBackOffMax;
	  }

	  //calculate min amoung min Bo Times
	  if(commonGmConfig.mGmBackOffMin > transacGmConfig.mGmBackOffMin)
	  {
		commonGmConfig.mGmBackOffMin = transacGmConfig.mGmBackOffMin;
	  }

	  //calculate max session timeout. It is a requirement from GM that
      //its clients calculate the maximum session tiemout amongs it requests
      // and inject that information into GM so that it knows how long it
      //can attempt a GNSS position fix
	  if(commonGmConfig.mSessionTimeout < transacGmConfig.mSessionTimeout)
	  {
		commonGmConfig.mSessionTimeout = transacGmConfig.mSessionTimeout;
	  }

	  //If any of the transacs want agressive exit then the common config will have it
	  commonGmConfig.mGnssUnavailAggresiveExit |= transacGmConfig.mGnssUnavailAggresiveExit;

	  /* Caclulate the minimum CPI req rate, if no clients sets this rate then GF defaults shall be used */
	  if( transacGmConfig.mCpiReqRate < commonGmConfig.mCpiReqRate)
	  {
		commonGmConfig.mCpiReqRate = transacGmConfig.mCpiReqRate;
	  }
	  
	}	
	ctr ++;
  }

  if(TRUE == retVal)
  {
	retVal = gmProxy->setGfEngineParam(commonGmConfig);
  }
  else
  {
	/* Since the client request queue is empty we need to reset the "Unable to track" 
	 * (UAT) flag . THis is because we dont want aon_transac to send an UAT
	 * indication even before GM actually tries GNSS & CPI for a session if UAT
	 * was sent in the previous session
	 */
    gmProxy->setUnableToTrackReported(FALSE);
  }
  return retVal;
}

boolean AonDatabase::broadcastStatusInfo(aonSessionStatusType status)
{
  boolean retVal = FALSE;

  AON_MSG_HIGH("AonDatabase:: status %d mCurrentStatus %d mReportedStatus %d ", 
               status, mCurrentStatus, mReportedStatus);

  uint32 hystTimeRemaining =  os_TimerCheck(mSessionStatusHystTimer);
  AON_MSG_HIGH("AonDatabase:: broadcastStatusInfo hystTimeRemaining "
               , hystTimeRemaining, 0, 0);
  //Hysterisis for throttling the Unable to track/Able to track indications
  if (AON_UNABLE_TO_TRACK == status)
  {
      if (hystTimeRemaining == 0)
      {
          //Timer has not been started yet so start it
          (void)os_TimerStart(mSessionStatusHystTimer , 
                              AON_SESSION_STATUS_HYSTERISIS_INTERVAL_MSEC, 0);
      }
      
      mCurrentStatus = status;
      retVal = TRUE;
  }
  else if (AON_ABLE_TO_TRACK == status)
  {
      if (hystTimeRemaining == 0)
      {
          //If timer not ctive report immediately
          retVal = reportStatusToAllTransac(status);

      }
      mCurrentStatus = status;
  }
  else
  {
      //Report to all transactions for all other types of status info
      retVal = reportStatusToAllTransac(status);
  }

  AON_MSG_MED("AonDatabase:: status %d retVal %d", 
	  status, retVal, 0);
  return retVal;
}

boolean AonDatabase::reportStatusToAllTransac(aonSessionStatusType status)
{
    boolean retVal = FALSE;
    uint32 ctr = 0;
    mReportedStatus =  mCurrentStatus;

    while(ctr < AON_DB_MAX_GF_TRANSACTIONS)
    {
    	 AonTransac* aonTransac = m_TransacInfoMap[ctr];
    	 if(NULL != aonTransac )
    	 {
    		 aonTransac->statusCallbackHandler(aonTransac->getReqId(), status);
    		 retVal =  TRUE; //We had atleast one valid transaction in the list
    	 }	
    	ctr ++;
    }

    return retVal;
}

void AonDatabase:: timerHandler()
{
  AON_MSG_HIGH("AonDatabase:: timerHandler(), timer expiry!. "
               "mCurrentStatus %d mReportedStatus %d ", 
               mCurrentStatus, mReportedStatus, 0);
  os_TimerStop(mSessionStatusHystTimer);
    /* The hysterisis timer has expired. Need to report the currrent state only if
       it is different from the revious reported state */
  if( mCurrentStatus != mReportedStatus )
  {
     reportStatusToAllTransac(mCurrentStatus);
  }
  return;
}





