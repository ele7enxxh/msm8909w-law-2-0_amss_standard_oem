#ifndef DS_NET_BEARER_TECH_EX_H
#define DS_NET_BEARER_TECH_EX_H

/*==========================================================================*/
/*!
  @file
  ds_Net_BearerTechEx.h

  @brief
  This file defines the ds::Net::BearerTechEx class.

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_BearerTechEx.h#1 $
  $DateTime: 2016/12/13 08:00:03 $
  $Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2013-02-13 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_IBearerTechEx.h"
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
/*lint -esym(1510, IBearerTechEx) */

/*!
  @brief
  This class implements IBearerTechEx interface.

  @details
  This class implements IBearerTechEx interface. This class also
  implements IQI interface. The BearerTechEx object is created using
  INetwork's method: GetBearerTechEx(). The BearerTechEx object can be
  released via IQI interface's Release() method. This object does not
  support any events to be registered on it.

  @see Interface IQI
  @see Interface IBearerTechEx
*/
class BearerTechEx : public  IBearerTechEx
{

private:
  /*-------------------------------------------------------------------------
    Private types and variable definitions.
  -------------------------------------------------------------------------*/
  typedef enum
  {
    DS_NET_BEARER_TECH_EX_PARAMS_NETWORK_TYPE              = 0,
    DS_NET_BEARER_TECH_EX_PARAMS_RAT_VALUE                 = 1,
    DS_NET_BEARER_TECH_EX_PARAMS_SO_MASK                   = 2,
  } ParamsType;

  uint32                                     mTechnology;
  uint32                                     mRatValue;
  uint32                                     mSoMask;
  ICritSect                                 *mpICritSect;

  /*-------------------------------------------------------------------------
    Private function declarations.
  -------------------------------------------------------------------------*/
  /*!
  @brief
  This is the destructor for the BearerTechEx class.

  @details
  This function is meant only for internal DSNET usage. To release this
  object, please use IQI interface's Release() function.

  @param      None.
  @note       None.
  @return     None.
  */
  virtual ~BearerTechEx
  (
    void
  ) throw();

  /*!
  @brief
  Internal function for getting different params for BearerTechEx.

  @param[in]  paramName - Name of the parameter.
  @param[out] pOut - Destination buffer
  @param[in]  pIn - Source buffer
  @param[in]  len - Length of buffer that needs to be copied.

  @see        enum ds::Net::BearerTechEx::ParamsType
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
  This is the constructor for the BearerTechEx class.

  @details
  The constructor is not directly exported to the applications. Only
  internal DSNET code can call the constructor. To create an object of this
  type use CreateInstance with appropariate class ID.

  @param[in]  technology - Technology type (3GPP/3GPP2)
  @param[in]  rat - The sub radio access technology of the above
  @param[in]  soMask - CDMA service option mask

  @return     Returns the Bearer tech object created.
  */
  BearerTechEx
  (
    uint32   technology,
    uint32   rat,
    uint32   soMask
  );


  /*-------------------------------------------------------------------------
    Functions inherited from IBearerTechEx
  -------------------------------------------------------------------------*/

  /**
    * This attribute indicates the technology type of the data connection (3GPP/3GPP2)
    * @param value Attribute value
           */
         virtual ::AEEResult CDECL GetTechnology(unsigned int* value);
         
         /**
           * This attribute indicates the RAT of the bearer.
           * @param value Attribute value
           */
         virtual ::AEEResult CDECL GetRatValue(unsigned int* value);
         
         /**
           * This attribute indicates the SO mask for 3GPP2.
           * @param value Attribute value
           */
         virtual ::AEEResult CDECL GetSoMask(unsigned int* value);
  /*-------------------------------------------------------------------------
    Functions inherited from IQI interface.
  -------------------------------------------------------------------------*/
  DSIQI_IMPL_DEFAULTS(IBearerTechEx)

  /*-------------------------------------------------------------------------
    Functions to overload the new and delete operators of this class.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS(PS_MEM_DS_NET_BEARER_TECH_EX)


}; /* class BearerTechEx */
}  /* Namespace Net */
}  /* Namespace ds */

#endif /* DS_NET_BEARER_TECH_H */


