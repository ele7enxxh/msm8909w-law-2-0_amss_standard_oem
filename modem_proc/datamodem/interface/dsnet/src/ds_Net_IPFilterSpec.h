#ifndef IP_FILTER_SPEC_H
#define IP_FILTER_SPEC_H
/*==========================================================================*/ 
/*! 
  @file 
  ds_Net_IPFilterSpec.h

  @brief
  This file defines the class that implements the IIPFilterPriv interface.

  @details
  The IIPFilterPriv interface acts as a interface for specifying the filter
  specification used for QOS requests, filter registrations etc. This file
  provides implementation for the interface.

  Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*/
/*=========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  Please notice that the changes are listed in reverse chronological order.

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dsnet/src/ds_Net_IPFilterSpec.h#1 $
  $DateTime: 2016/12/13 08:00:03 $$Author: mplcsds1 $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2008-08-10 hm  Created module.

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_StdErr.h"
#include "ds_Net_IIPFilterPriv.h"
#include "ds_Net_Platform.h"
#include "ds_Net_Utils.h"
#include "ds_Net_MemManager.h"
#include "ds_Net_PSQoSSpecWrapper.h"

namespace ds
{
namespace Net
{

/*lint -esym(1510, IQI) */
/*lint -esym(1510, IIPFilterPriv) */


/*!
  @class
  IPFilterSpec

  @brief
  This class provides implementation for IIPFilterPriv interface.

  @todo
  write details
*/
class IPFilterSpec : public IIPFilterPriv
{
private:

  /* To suppress lint error 429 */        
  IPFilterSpec*                     mpIPFilterSpecClone;
  PSQoSSpecWrapper*                 mpPSQoSSpecWrapper;

  /*!
  @brief      
  Private auxillary function to support GetValidOptions() and 
  GetErroneousOptions().

  @param[out] pOpts - Options array
  @param[out] optsLen - Options array length
  @param[out] pOptsLenReq - LenReq arg for options array.
  @param[in]  isErrMask - If err mask(TRUE) or field mask (FALSE) 

  @return     AEE_SUCCESS on success.
  @return     QDS_EFAULT on invalid parameters.
  @see        GetValidOptions()
  @see        GetErroneousOptions()
  */
  virtual ds::ErrorType CDECL GetOptionsInternal
  (
    IPFilterIDType*  pOpts,
    int                       optsLen,
    int *                     pOptsLenReq,
    boolean                   isErrMask
  );

  /*!
  @brief      
  Destructor of the IPFilterSpec object.

  @details
  Destructor of the IPFilterSpec object. The destructor is private and 
  hence we cannot call "delete pFilterSpec" etc. Use Release() method
  to release the pointer to this object.

  @param      None.
  @return     None.
  @see        IQI::Release()
  */
  virtual ~IPFilterSpec
  (
    void 
  ) 
  throw();

public:
  /*!
  @brief      
  Constructor of the IPFilterSpec object.

  @param      None.
  @return     None.
  @see        ds::CreateInstance().
  */
  IPFilterSpec();

  /* Functions derived from IIPFilter interface */

  /*!
  @brief      
  This method creates a duplicate of the current object.

  @param[out] ppIPFilterSpec - Output interface
  @return     AEE_SUCCESS - On Success
  @return     AEE_ENOMEMORY - Out of memory
  @note       Memory is allocated for the returned object. Application should
              call Release() to free this memory.
  */
  virtual ds::ErrorType CDECL Clone 
  (
    IIPFilterPriv** ppIPFilterSpec
  );

  /*!
  @brief      
  Return the field mask for the IPFilterSpec interface. This indicates which
  fields of the interface have been set.

  @param[out] ppValidOptions - Array of option IDs which are set (field mask)
  @param[in]  validOptionsLen - Length of array provided by application.
  @param[out] pValidOptionsLenReq - LenReq argument for the array.

  @return     AEE_SUCCESS - On Success
  @return     QDS_EFAULT - Invalid arguments.
  */
  virtual ds::ErrorType CDECL GetValidOptions 
  (
    IPFilterIDType* ValidOptions, 
    int ValidOptionsLen, 
    int* ValidOptionsLenReq
  );

  //TODO: Write documentation for all functions.
  virtual ds::ErrorType CDECL GetErroneousOptions 
  ( 
    IPFilterIDType* ErroneousOptions, 
    int ErroneousOptionsLen, 
    int* ErroneousOptionsLenReq
  );

  virtual ds::ErrorType CDECL GetIPVsn 
  (
    IPFilterIPVersionType* IPVsn
  );

  virtual ds::ErrorType CDECL SetIPVsn 
  (
    IPFilterIPVersionType IPVsn
  );

  virtual ds::ErrorType CDECL GetNextHdrProt 
  (
    IPFilterIPNextProtocolType* NextHdrProt
  );

  virtual ds::ErrorType CDECL SetNextHdrProt 
  (
    IPFilterIPNextProtocolType NextHdrProt
  );

  virtual ds::ErrorType CDECL GetSrcPort 
  (
    IPFilterPortType* SrcPort
  );

  virtual ds::ErrorType CDECL SetSrcPort 
  (
    const IPFilterPortType* SrcPort
  );

  virtual ds::ErrorType CDECL GetDstPort 
  (
    IPFilterPortType* DstPort
  );

  virtual ds::ErrorType CDECL SetDstPort 
  (
    const IPFilterPortType* DstPort
  );

  virtual ds::ErrorType CDECL GetSrcV4 
  (
    IPFilterIPv4AddrType* SrcV4
  );

  virtual ds::ErrorType CDECL SetSrcV4 
  (
    const IPFilterIPv4AddrType* SrcV4
  );

  virtual ds::ErrorType CDECL GetDstV4 
  (
    IPFilterIPv4AddrType* DstV4
  );

  virtual ds::ErrorType CDECL SetDstV4 
  (
    const IPFilterIPv4AddrType* DstV4
  );

  virtual ds::ErrorType CDECL GetTos 
  (
    IPFilterTOSType* Tos
  );

  virtual ds::ErrorType CDECL SetTos 
  (
    const IPFilterTOSType* Tos
  );

  virtual ds::ErrorType CDECL GetFlowLabel 
  (
    IPFilterIPv6FlowLabelType* FlowLabel
  );

  virtual ds::ErrorType CDECL SetFlowLabel 
  (
    IPFilterIPv6FlowLabelType FlowLabel
  );

  virtual ds::ErrorType CDECL GetSrcV6 
  (
    IPFilterIPv6AddrType* SrcV6
  );

  virtual ds::ErrorType CDECL SetSrcV6 
  (
    const IPFilterIPv6AddrType* SrcV6
  );

  virtual ds::ErrorType CDECL GetDstV6
  (
    IPFilterIPv6AddrType* DstV6
  );

  virtual ds::ErrorType CDECL SetDstV6 
  (
    const IPFilterIPv6AddrType* DstV6
  );

  virtual ds::ErrorType CDECL GetTrafficClass 
  ( 
    IPFilterIPv6TrafficClassType* TrafficClass
  );

  virtual ds::ErrorType CDECL SetTrafficClass 
  (
    const IPFilterIPv6TrafficClassType* TrafficClass
  );

  virtual ds::ErrorType CDECL GetEspSpi
  (
    int* EspSpi
  );

  virtual ds::ErrorType CDECL SetEspSpi
  (
    int EspSpi
  );

  virtual ds::ErrorType CDECL GetAhSpi
  (
    int* AhSpi
  );

  virtual ds::ErrorType CDECL SetAhSpi
  (
    int AhSpi
  );


  virtual ds::ErrorType CDECL GetICMPType
  ( 
    unsigned char* ICMPType
  );
  virtual ds::ErrorType CDECL SetICMPType
  ( 
    unsigned char ICMPType
  );     
  virtual ds::ErrorType CDECL GetICMPCode 
  ( 
    unsigned char* ICMPCode
  );
  virtual ds::ErrorType CDECL SetICMPCode
  (
    unsigned char ICMPCode
  );

  virtual ds::ErrorType CDECL GetAuxInfo 
  (
    IPFilterAuxInfoType* AuxInfo
  );


  /*!
  @function
  QueryInterface()

  @brief      
  QueryInterface method of IIPFilterPriv object.

  @details
              The IIPFilterPriv object implements the following interfaces:
              IQI
              IIPFilterPriv
  All of these interfaces can be got by using the QueryInterface() method 
  using appropriate IID.

  @param[in]  iid - Interface Id of the required interface.
  @param[out] ppo - Pointer to the interface returned.

  @see        IQI::QueryInterface()

  @return     AEE_SUCCESS on success
  @return     AEE_ECLASSNOSUPPORT - if the IID is not supported.
  @return     QDS_EFAULT - on invalid inputs.
  */
  virtual int CDECL QueryInterface
  (
    AEEIID                                  iid,
    void **                                 ppo
  );


  /* Functions inherited from IQI interface */
  DSIQI_DECL_LOCALS()                                                    
  DSIQI_ADDREF()                                                         
  DSIQI_RELEASE()     

  /* Auxillary function exported only internally */
  virtual ds::ErrorType CDECL GetPSIPFilterSpec (NetPlatform::PSFilterSpecType *pPSIPFilterSpec);
  virtual ds::ErrorType CDECL UpdateSpec (NetPlatform::PSFilterSpecType *pPSFilterSpec);

  /*-------------------------------------------------------------------------
    Methods to overload new/delete operators.
  -------------------------------------------------------------------------*/
  DSNET_OVERLOAD_OPERATORS (PS_MEM_DS_NET_IP_FILTER_SPEC)

}; /* class IPFilterSpec */

} /* namespace Net */
} /* namespace ds */

#endif /* IP_FILTER_SPEC_H */

