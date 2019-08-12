#ifndef IXIPSEC_H
#define IXIPSEC_H

/*===========================================================================
    Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
    Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/
/**
  @file  IxIPSec.h
  
  @brief
  This file provides a set of accessor functions to perform the following operations:
  - Creating and manipulating IPsec Security Association objects through the IxIPSecSA class
  - Adding, removing and updating SAs from the Security Association DataBase
  - Creating and manipulating IPsec security policy objects through the IxIPSecSP class
  - Adding, removing and updating security policy from the Security Policy DataBase.

  @section A1 Functions defined in Class IxIPSecSA

  These functions are used for manual keying only, and they allow the following 
  SA options to be set or retrieved:
   - Security parameter index
   - Encapsulation mode
   - Hash keys
   - Crypto algorithms
   - Crypto keys
   - Self/peer identity
   - Security Assocation lifetime

  Externalized functions:
    - SetOpt()
    - GetOpt()

  Initialization and sequencing requirements:
  None.

  @section A2 Functions defined in Class IxIPSecSADB
 
  These functions are used for manual keying only, and they allow SAs 
  to be added, removed, or updated.

  Externalized functions:
    - Add()
    - Remove()
    - Update()
    - GenerateSPI()

  Initialization and sequencing requirements:
  None.

 @section A3 Functions defined in Class IxIPSecSP

  These functions are used to set or retreive the security policy options,
  which include:
  - Protection policy
  - Selector
  - Encapsulation mode
  - Gateway
  - Encapsulated Security Payload/Authentication Header protection
  - Authentication data
  - Key management type
  - Self/peer identity
  - Configuration attribute information

  Externalized functions:
    - SetOpt()
    - GetOpt()

  Initialization and sequencing requirements:
  None.

  @section A4 Functions defined in Class IxIPSecSPDB

  These functions are used to add, remove, and update a security policy that
  includes the following information:
  - Key management type (IKEv2/manual)
  - Encapsulation mode (Tunnel/Transport)
  - Proposal transforms
  - Gateway information
  - Peer/Self Identity
  - Configuration attributes

  Externalized Functions:
    - AddPolicy()
    - RemovePolicy()
    - Update()

  Initialization and sequencing requirements:
  None.
*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/sec/api/ipsec/IxIPSec.h#1 $ $DateTime: 2016/12/13 07:58:24 $ $Author: mplcsds1 $


  when       who     what, where, why
  --------   ---     ----------------------------------------------------------
  07/28/10   dm     Re-architectured IPsec API files
  07/07/10   sw      (Tech Pubs) edited/added Doxygen comments and markup.
  09/22/09   nk      IPsec CMI Decoupling.
  08/13/07   ssm     Manual SA cleanup fixes from DvBH integration
  03/22/07   ssm     Support for Multiple SA's for DvBH
  07/07/06   ssm     Created Module

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "stdlib.h"
#include "err.h"
#include "string.h"
#include "IPSec.h"
#include "queue.h"
#include "IxErrno.h"
#include "IxCallbackDef.h"

/*===========================================================================

                      DEFINITIONS AND CONSTANTS

===========================================================================*/


/*===========================================================================

                 DEFINITIONS AND TYPE DECLARATIONS

===========================================================================*/

typedef struct
{
  q_link_type  Link;
  IPSecSAType  SA;
} IPSecSALinkType;

/*---------------------------------------------------------------------------
 *   D E F I N I T I O N S  F O R  S E C U R I T Y  A S S O C I A T I O N
 --------------------------------------------------------------------------*/

class IxIPSecSA
{
  friend class IxIPSecSADB; /* temp until we have getopt implemented */
  public:
/*------------------------------------------------------------------------
                               PUBLIC TYPES
------------------------------------------------------------------------*/

/*------------------------------------------------------------------------
                              PUBLIC METHODS
 ------------------------------------------------------------------------*/
/**
   @addtogroup ipsec_brew_manual_api
  @{
*/

/**
  Default Constructor that creates a new Security Association object with the 
  default options set.

  @return
  None.

  @dependencies
  None.
*/
  IxIPSecSA ();

/** 
  @}
*/ /* end_group ipsec_brew_manual_api */


/*
  ~IxIPSecSA: Destructor
  @dependencies  None.
  @return  None.
*/
  ~IxIPSecSA ();

/*
  IxIPSecSA: Copy Constructor
  @param SA [in] -- Reference to a Security Association object.
  @dependencies  None.
  @return  None.
*/
  IxIPSecSA(const IxIPSecSA &SA);

/*
  operator=: Overloaded = operator
  @param SA [in] -- Reference to a Security Association object.
  @dependencies  None.
  @return  None.
*/
  IxIPSecSA &operator= (const IxIPSecSA &SA);


/**
 @addtogroup ipsec_brew_manual_api
  @{
*/

/**
  Sets the Security Association options.

  @param Opt       [in] -- Option type to be set.
  @param *pOptVal  [in] -- Corresponding option value.
  @param dwOptSize [in] -- Size of data pointed by pOptVal

  @return
  IxErrnoType -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
  IxErrnoType SetOpt ( const IPSecSA_OptionEType  Opt, 
                       void                      *pOptVal, 
                       const int                  dwOptSize );

/**
  Retrieves the Security Association options.

  @param Opt           [in] -- Option type to be retrieved.
  @param **pOptVal [in/out] -- Corresponding option value.
  @param dwOptSize [in/out] -- Size of the option value.

  @return
  IxErrnoType -- E_SUCCESS or a corresponding failure code.

  @dependencies
  pOptVal needs to be freed by caller
*/
  IxErrnoType GetOpt ( const IPSecSA_OptionEType   Opt, 
                       void                      **pOptVal, 
                       int                        &dwOptSize );

/**
  To free the IPSecSA created by GetOpt()

  @param Opt           [in] -- Option type to be retrieved.
  @param **pOptVal [in] -- Corresponding option value.

  @return
  IxErrnoType -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None
*/
  IxErrnoType FreeIpsecSA ( const IPSecSA_OptionEType   Opt, 
                       void                      **pOptVal );

/** 
  @}
*/ /* end_group ipsec_brew_manual_api */


private:
  /*------------------------------------------------------------------------
                                PRIVATE TYPES
  ------------------------------------------------------------------------*/
  q_type       __SAQue;   /* Can be a single SA or multiple SA's */
  uint32       __NumSA;   /* Number of SA's...max value IPSECSA_MAX_NUM_SA */
  
  /*------------------------------------------------------------------------
                                PRIVATE METHODS
   ------------------------------------------------------------------------*/
};

class IxIPSecSADB
{
  public:
  /*------------------------------------------------------------------------
                                 PUBLIC TYPES
  ------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
                                PUBLIC METHODS
  -------------------------------------------------------------------------*/
/**
   @addtogroup ipsec_brew_manual_api
  @{
*/

/** 
  Generates a unique security parameter index.

  @param SPI [out] -- Generated security parameter index.

  @return
  IxErrnoType -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
  static IxErrnoType GenerateSPI (uint32 &SPI);


/**
  Adds a Security Association to the SADB.
 
  @param SA       [in] -- SA to be added.
  @param SAIndex [out] -- Handle to the SA.
  @param *pcb     [in] -- Pointer to the callback function.

  @return
  None.

  @dependencies
  None.
*/
  static void Add( const IxIPSecSA    &SA,
                   IPSecSADBIndexType &SAIndex,
                   IxCallback         *pcb );

/**
  Removes a Security Association from the SADB.

  @param SAIndex [in] -- Handle to the SA that is to be removed.
  @param *pcb    [in] -- Pointer to the callback function..

  @return
  None. 

  @dependencies
  None.
*/
  static void Remove ( const IPSecSADBIndexType &SAIndex,
                       IxCallback               *pcb );
                       
/**
  Updates a Security Association in the SADB.

  @param SAIndex [in] -- Handle to the SA that is to be updated.
  @param SA      [in] -- Reference to the new SA.
  @param *pcb    [in] -- Pointer to the callback function.

  @return
  IxErrnoType -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None. 
*/
  static IxErrnoType Update ( const IPSecSADBIndexType  &SAIndex,
                              const IPSecSAType         &SA,
                              IxCallback                *pcb);
/** 
  @}
*/  /* end_group ipsec_brew_manual_api */


  /*------------------------------------------------------------------------
                                PRIVATE TYPES
  ------------------------------------------------------------------------*/
  
  /*------------------------------------------------------------------------
                                PRIVATE METHODS
   ------------------------------------------------------------------------*/
};

class IxIPSecSP
{
public:
  friend class IxIPSecSPDB; /* Temporary until we have getopt implemented */
  /*------------------------------------------------------------------------
                                 PUBLIC TYPES
  ------------------------------------------------------------------------*/
  
  /*------------------------------------------------------------------------
                                PUBLIC METHODS
   ------------------------------------------------------------------------*/
/**
   @addtogroup ipsec_brew_ikev2_api
  @{
*/

/**
  Default constructor that creates a new security policy object with default 
  options set.
  
  The event callback function is called for any events related to this Security 
  Policy.

  @param *pcbSP [in] -- Callback that is called for any events related to this 
                        security policy. 

  @return
  None.

  @dependencies
  None.
*/
  IxIPSecSP (IxCallback *pcbSP);

/** 
  @}
*/ /* end_group ipsec_brew_ikev2_api */


/*
  Destructor.
  @dependencies   None.
  @return  None.
*/
  ~IxIPSecSP ();

/*
  Copy Constructor.
  @param SP [in] -- Reference to the security policy object.
  @dependencies  None.
  @return  None.
*/
  IxIPSecSP(const IxIPSecSP &SP);

/*
  Overloaded = operator
  @param SP [in] -- Reference to security policy Object
  @dependencies  None.
  @return  None.
*/
  IxIPSecSP &operator= (const IxIPSecSP &SP);


/**
   @addtogroup ipsec_brew_ikev2_api
  @{
*/

/**
  Sets security policy options.\ This function copies the option values into 
  the private data of the security policy object.
   
  @param SlctrIndex [in] -- Selector index; used only for selector options.
  @param Opt        [in] -- Option type to be set.
  @param *pOptVal   [in] -- Corresponding option value.
  @param dwOptSize  [in] -- Size of the data pointed to by pOptVal.
  
  @return
  IxErrnoType -- E_SUCCESS or a corresponding failure code.
 
  @dependencies
  This function frees pOptVal.
*/
  IxErrnoType SetOpt( const uint8                  SlctrIndex,
                      const IPSecSP_OptionEType    Opt, 
                      void                        *pOptVal, 
                      const uint32                 dwOptSize );

/**
  Retrieves security policy options.
 
  @param SlctrIndex [in]     -- Selector index; used only for selector options.
  @param Opt        [in]     -- Option type to be retrieved.
  @param **pOptVal  [in/out] -- Corresponding option value.
  @param dwOptSize  [in/out] -- Size of the data pointed to by pOptVal.

  @return
  IxErrnoType -- E_SUCCESS or a corresponding failure code.

  @dependencies
  pOptVal must be freed by the caller.
*/
  IxErrnoType GetOpt( const uint8                  SlctrIndex,
                      const IPSecSP_OptionEType    Opt, 
                      void                       **pOptVal, 
                      uint32                      &dwOptSize );

/** 
  @}
*/ /* end_group ipsec_brew_ikev2_api */


private:
  /*------------------------------------------------------------------------
                                PRIVATE TYPES
  ------------------------------------------------------------------------*/
  uint32                   __PhysIfaceId;
  IPSecIdentificationType  __SelfID;
  IPSecIdentificationType  __PeerID;
  IPSecSP_DirectionEType   __Direction;
  IPSecSP_ProtectionType   __Protection;
  IPSecSlctrSpecType       __Selector;
  IxCallback              *__pcbSP;
  IPSecCfgDataType         __CfgAttrs; 
  /*------------------------------------------------------------------------
                                PRIVATE METHODS
   ------------------------------------------------------------------------*/
};

class IxIPSecSPDB
{
  public:
  /*------------------------------------------------------------------------
                                 PUBLIC TYPES
  ------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
                                PUBLIC METHODS
   ------------------------------------------------------------------------*/

/**
   @addtogroup ipsec_brew_ikev2_api
  @{
*/

/**
  Adds a security policy to the SPDB.\ This function does the following:
   - Adds the security policy information to the SPDB. This information 
     includes the key management type (IKEv2/manual), encapsulation mode 
     (Tunnel/Transport), proposal transforms, gateway information, peer/self 
     identity, and configuration attributes.
   - Installs the IPsec input/output filters.
   - IKEv2 case -- triggers the IKEv2 exchange.
   - Manual keying case -- updates the SA information.

  @param SP       [in]  -- Reference to the security policy that is to be added.
  @param Priority [in]  -- Priority of this policy.
  @param SPIndex  [out] -- Handle to this policy in the SPDB.

  @return
  IxErrnoType -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
  static IxErrnoType AddPolicy( const IxIPSecSP     &SP,
                                const uint8          Priority,
                                IPSecSPDBIndexType  &SPIndex );

/**
  Removes a security policy from the SPDB.\ This function does the following:
   - Deletes the security policy.
   - Deletes the filters related to this policy.
   - IKEv2 case -- tears down the tunnel.
   - Manual keying case -- cleans up the SA information.

  @param SPIndex [in] -- Handle to this security policy in the SPDB.

  @return
  IxErrnoType -- E_SUCCESS.

  @dependencies
  None.
*/
  static IxErrnoType RemovePolicy(const IPSecSPDBIndexType &SPIndex);

/**
  Updates security policy information in the SPDB with the provided options.

  @note Currently, only the DPD Timer value option can be updated.
      
  @param SPIndex   [in] -- Reference to the security policy that is to be updated.
  @param Opt       [in] -- Option to be updated.
  @param *pOptVal  [in] -- Option value.
  @param dwOptSize [in] -- Option size.

  @return
  IxErrnoType -- E_SUCCESS or a corresponding failure code.

  @dependencies
  None.
*/
static IxErrnoType Update
(
  const IPSecSPDBIndexType    &SPIndex,
  const IPSecSP_OptionEType    Opt,
  void                        *pOptVal,
  const uint32                 dwOptSize
);

/** 
  This function returns the MTU of IPsec Iface that corresponds
  to the passed SPD pointer.

  @param SPIndex   [in]  -- Reference to the security policy.
  @param pMTU      [out] -- MTU value of corresponding IPsec Iface.

  @return
  IxErrnoType -- E_SUCCESS or E_FAILURE.

  @dependencies
  AddPolicy should have been called before this.
*/
static IxErrnoType GetIfaceMTU
(
  const IPSecSPDBIndexType &SPIndex,
  uint32                   *pMTU
);

/** 
  @}
*/ /* end_group ipsec_brew_ikev2_api */


private:
  /*------------------------------------------------------------------------
                                PRIVATE TYPES
  ------------------------------------------------------------------------*/

  /*------------------------------------------------------------------------
                                PRIVATE METHODS
   ------------------------------------------------------------------------*/
};

#endif  /* IXIPSEC_H */
