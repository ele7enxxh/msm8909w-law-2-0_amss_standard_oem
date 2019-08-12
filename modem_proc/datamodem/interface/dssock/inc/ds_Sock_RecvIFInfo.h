#ifndef DS_SOCK_RECVIFINFO_H
#define DS_SOCK_RECVIFINFO_H
/*===========================================================================
  @file IPlatformSocketFactory.h

  This file defines the interface for Sockets Platform. This interface
  abstracts various platforms such as WinMobile, Linux, and BMP etc. from
  the sockets library and provides methods to create a PlatformSocket.

  Copyright (c) 2008 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/dssock/inc/ds_Sock_RecvIFInfo.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "ds_Sock_IRecvIFInfoPriv.h"
#include "ds_Utils_CSSupport.h"
#include "ds_Utils_StdErr.h"
#include "ds_Errors_Def.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
//TODO Document Class
namespace ds
{
  namespace Sock
  {
    class RecvIFInfo : public IRecvIFInfoPriv
    {
      public:
        static void * operator new
        (
          unsigned int numBytes
        ) throw();

        static void operator delete
        (
          void *  bufPtr
        ) throw();

        RecvIFInfo
        (
          unsigned int  recvIFHandle
        );

        virtual ds::ErrorType CDECL GetAncID
        (
          ds::Sock::AncDataIDType *  ancIDPtr
        );

        virtual ds::ErrorType CDECL SetAncID
        (
          ds::Sock::AncDataIDType  ancID
        );

        virtual ds::ErrorType CDECL GetRecvIFHandle
        (
          unsigned int *  recvIFHandlePtr
        );

	virtual ~RecvIFInfo() {}
	
        /*-------------------------------------------------------------------
          IQI interface Methods
        -------------------------------------------------------------------*/
        DSIQI_DECL_LOCALS()
        DSIQI_ADDREF()
        DSIQI_RELEASE()
        virtual ds::ErrorType CDECL QueryInterface
        (
          AEEIID   iid,
          void **  objPtrPtr
        );

      private:
        unsigned int  recvIFHandle;
    };
  } /* namespace Sock */
} /* namespace ds */

#endif /* DS_SOCK_RECVIFINFO_H */
