#ifndef DS_NET_BEARER_TECH_H
#define DS_NET_BEARER_TECH_H

/*==========================================================================*/
/*!
  @file
  ds_Net_BearerTech.h

  @brief
  This file defines the ds::Net::BearerTech class.

  Copyright (c) 2008-2010 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_BearerTech.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2009-08-20 hm  Use crit sect interface instead of direct objects.
  2008-03-20 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_IBearerInfo.h"
#include "ds_Utils_ICritSect.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_Platform.h"

namespace ds
{
namespace Net
{
/*lint -esym(1510, IQI) */
/*lint -esym(1510, IBearerInfo) */

/*!
  @brief
  This class implements IBearerInfo interface.

  @details
  This class implements IBearerInfo interface. This class also
  implements IQI interface. The BearerTech object is created using
  INetwork's method: GetBearerInfo(). The BearerTech object can be
  released via IQI interface's Release() method. This object does not
  support any events to be registered on it.

  @see Interface IQI
  @see Interface IBearerInfo
*/
class BearerTech : public  IBearerInfo
{

private:
  /*-------------------------------------------------------------------------
    Private types and variable definitions.
  -------------------------------------------------------------------------*/
  typedef enum
  {
    DS_NET_BEARER_TECH_PARAMS_NETWORK_TYPE              = 0,
    DS_NET_BEARER_TECH_PARAMS_CDMA_TYPE_MASK            = 1,
    DS_NET_BEARER_TECH_PARAMS_CDMA_SERVICE_OPTION_MASK  = 2,
    DS_NET_BEARER_TECH_PARAMS_UMTS_TYPE_MASK            = 3,
    DS_NET_BEARER_TECH_PARAMS_BAERER_RATE               = 4
  } ParamsType;

  ds::Net::IfaceNameType                     mNetworkType;
  uint32                                     mCdmaTypeMask;
  uint32                                     mCdmaServiceOptionMask;
  uint32                                     mUmtsTypeMask;
  ICritSect                                 *mpICritSect;
  ds::Net::BearerTechRateType          mBearerRate;

  /*-------------------------------------------------------------------------
    Private function declarations.
  -------------------------------------------------------------------------*/
  /*!
  @brief
  This is the destructor for the BearerTech class.

  @details
  This function is meant only for internal DSNET usage. To release this
  object, please use IQI interface's Release() function.

  @param      None.
  @note       None.
  @return     None.
  */
  virtual ~BearerTech
  (
    void
  ) throw();

  /*!
  @brief
  Internal function for getting different params for BearerTech.

  @param[in]  paramName - Name of the parameter.
  @param[out] pOut - Destination buffer
  @param[in]  pIn - Source buffer
  @param[in]  len - Length of buffer that needs to be copied.

  @see        enum ds::Net::BearerTech::ParamsType
  @return     AEE_SUCCESS - On success
  @return     QDS_EFAULT - Invalid arguments.
  */
  virtual ds::ErrorType GetParams
  (
    ParamsType paramName,
    void *     pOut,
    void *     pIn,
    int        len
  );


public:
  /*-------------------------------------------------------------------------
    Constructor.
  -------------------------------------------------------------------------*/

  /*!
  @brief
  This is the constructor for the BearerTech class.

  @details
  The constructor is not directly exported to the applications. Only
  internal DSNET code can call the constructor. To create an object of this
  type use CreateInstance with apropriate class ID.

  @param[in]  ifaceName - Iface name
  @param[in]  cdmaTypeMask - CDMA option mask
  @param[in]  cdmaServiceOptionMask - CDMA service option mask
  @param[in]  umtsTypeMask - Umts option mask
  @param[in]  pDataBearerRate - Bearer Rate information. Can be NULL if no information is availible

  @see        Namespace ds::Net::BearerTech::TypeMask::CDMA
  @see        Namespace ds::Net::BearerTech::ServiceOption::CDMA
  @see        Namespace ds::Net::BearerTech::TypeMask::UMTS
  @see        Namespace ds::Net::BearerTechnologyRateType
  @return     Returns the Bearer tech object created.
  */
  BearerTech
  (
    ds::Net::IfaceNameType                   ifaceName,
    uint32                                   cdmaTypeMask,
    uint32                                   cdmaServiceOptionMask,
    uint32                                   umtsTypeMask,
    BearerTechRateType*                      pDataBearerRate
  );


  /*-------------------------------------------------------------------------
    Functions inherited from IBearerInfo
  -------------------------------------------------------------------------*/

  /*!
  @brief
  Return the network type for the bearer tech object.

  @details
  This function returns the network type or iface name of the bearer tech
  object.

  @param[out] ifaceName - Iface name

  @see        Namespace ds::Net::IfaceName
  @return     AEE_SUCCESS - if iface name is successfully returned.
  @return     QDS_EFAULT - On error in input argument.
  */
  virtual ds::ErrorType CDECL GetNetwork
  (
    ::ds::Net::IfaceNameType*                ifaceName
  );

  /*!
  @brief
  Returns the CDMA type mask for the bearer tech object.

  @param[out] pCDMATypeMask - CDMA type mask associated with this object.

  @see        Namespace ds::Net::BearerTech::TypeMask::CDMA
  @return     AEE_SUCCESS - on success.
  @return     QDS_EFAULT - on invalid input argument.
  */
  virtual ds::ErrorType CDECL GetCDMATypeMask
  (
    unsigned int                      *pCDMATypeMask
  );

  /*!
  @brief
  Returns the CDMA service options mask for the bearer tech obj.

  @param[out] pCDMAServiceOptionsMask - CDMA service option mask

  @see        Namespace ds::Net::BearerTech::ServiceOptions::CDMA
  @return     AEE_SUCCESS - on success.
  @return     QDS_EFAULT - on invalid input argument.
  */
  virtual ds::ErrorType CDECL GetCDMAServiceOptionsMask
  (
    unsigned int                      *pCDMAServiceOptionsMask
  );

  /*!
  @brief
  Returns the UMTS Type mask for the bearer tech object.

  @param[out] pUMTSTypeMask - UMTS type mask.

  @see        Namespace ds::Net::BearerTech::TypeMask::UMTS
  @return     AEE_SUCCESS - on success.
  @return     QDS_EFAULT - on invalid input argument.
  */
  virtual ds::ErrorType CDECL GetUMTSTypeMask
  (
    unsigned int                      *pUMTSTypeMask
  );

  /**
  @brief      Gets the bearer rate of the underlying network.

  This function gets the bearer rate of underlying network.

  @param[out] argBearerRatePtr - Out parameter for bearer rate.

  @see        No external dependencies

  @return     AEE_SUCCESS - on success
  @return     Error code on error.
  */
  virtual ds::ErrorType CDECL GetRate
  (
    BearerTechRateType* bearerRate
  );

  /**
  @brief      Determines if the bearer is the NULL bearer for the technology.

  @param[out] isNullBearer - True when the bearer is the NULL bearer.

  @see        No external dependencies

  @return     AEE_SUCCESS - on success
  @return     Error code on error.
  */
  virtual ds::ErrorType CDECL GetBearerIsNull
  (
    boolean*  BearerIsNull
  );

  /*-------------------------------------------------------------------------
    Functions inherited from IQI interface.
  -------------------------------------------------------------------------*/
  DSIQI_IMPL_DEFAULTS(IBearerInfo)

  /*-------------------------------------------------------------------------
    Functions to overload the new and delete operators of this class.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS(PS_MEM_DS_NET_BEARER_TECH)


}; /* class BearerTech */
}  /* Namespace Net */
}  /* Namespace ds */

#endif /* DS_NET_BEARER_TECH_H */


