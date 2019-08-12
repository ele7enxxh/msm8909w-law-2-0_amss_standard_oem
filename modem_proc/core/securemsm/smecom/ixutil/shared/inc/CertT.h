#ifndef CERTT_H
#define CERTT_H

/*===========================================================================
FILE:  CertT.h

DESCRIPTION
  The CertT provides easy accessor methods to store data/len type of 
  information

EXTERNALIZED FUNCTIONS : 


  Copyright (c) 2005 by Qualcomm Technologies, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //components/rel/core.mpss/3.5.c12.3/securemsm/smecom/ixutil/shared/inc/CertT.h#1 $
  $DateTime: 2016/12/13 07:59:23 $
  $Author: mplcsds1 $ 


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/06   ssm     Use UxMalloc
05/16/06   ssm     Fixes for target build
4/30/06    ssm     Created
===========================================================================*/


/*===========================================================================
                           
                                INCLUDE FILES

============================================================================*/

#include "IxErrno.h"
#include "OpaqueT.h"
#include "comdef.h"
#include "err.h"
#include "stdlib.h"
#include "secdiag.h"

/*===========================================================================

                              CLASS DEFINITION

===========================================================================*/

#ifdef __cplusplus

class CertT : public OpaqueT
{
public:
  
  /* Default Constructor */
  CertT() 
  {
    pData = NULL;
    dwLen = 0;
  }

  /* Constructor */
  CertT(uint8* pInData, uint32 InLength) : OpaqueT(pInData, InLength) {}
  
  /* Destructor */
  ~CertT() {}
  
  /* Copy Constructor */
  CertT( const CertT & InCert ) : OpaqueT(InCert) {}

  /* Overload the assignment operator */
  CertT &operator = (const CertT& Cert) 
  { 
    OpaqueT::operator = (Cert);
    return *this;
  }

  /* Set Method */
  IxErrnoType Set(uint8* pInData, uint32 InLength)
  {
    OpaqueT::Set(pInData, InLength);
    return E_SUCCESS;
  }
};
#endif // __cplusplus
#endif
