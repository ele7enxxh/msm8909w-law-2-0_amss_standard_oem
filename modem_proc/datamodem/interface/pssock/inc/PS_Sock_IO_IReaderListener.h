#ifndef PS_SOCK_IO_IREADERLISTENER_H
#define PS_SOCK_IO_IREADERLISTENER_H
/*===========================================================================
  @file PS_Sock_IO_IReaderListener.h

  This file defines an interface to notify the users of IReader objects when
  there is data to read and when the data pipe becomes empty.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/inc/PS_Sock_IO_IReaderListener.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*lint -esym(1510, IReaderListener) */
namespace PS
{
  namespace Sock
  {
    namespace IO
    {
      /**
        @brief Provides notifications to the users of IReader objects

        Defines methods which are called when a IReader object has some data
        to read (0 to non-0 transition) or when it is empty. Users interested
        in these notifications has to implement this interface.

        @see PS::Sock::IO::IReader
      */
      class IReaderListener
      {
        public:
          /**
            @brief Notifies when reader has data to read

            Notifies the implentor of this interface when reader didn't have
            any data to read before but has data to read now, i.e. only on a
            0 to non-0 transition.

            A typical usage is

            @code
            class A : public IReaderListener
            {
              public:
                A()
                {
                  r.RegisterReaderListener( this);
                }

                void funcA()
                {
                  int32  c;

                  while (true)
                  {
                    c = r.Read( ...);
                  }
                }

                void EmptyQueue
                {
                  m.wait();
                }

                void DataAvailable
                {
                  m.release();
                }

              private:
                Mutex   m;
                Reader  r;
            }
            @endcode

            @param  None

            @retval None

            @see PS::Sock::IO::IReader
          */
          virtual void DataAvailable
          (
            void
          ) throw() = 0;

          /**
            @brief Notifies when reader doesn't have any more data to read

            Notifies the implentor of this interface when reader doesn't have
            any more data to read, i.e. on a non-0 to 0 transition.

            A typical usage is

            @code
            class A : public IReaderListener
            {
              public:
                A()
                {
                  r.RegisterReaderListener( this);
                }

                void funcA()
                {
                  int32  c;

                  while (true)
                  {
                    c = r.Read( ...);
                  }
                }

                void EmptyQueue
                {
                  m.wait();
                }

                void DataAvailable
                {
                  m.release();
                }

              private:
                Mutex   m;
                Reader  r;
            }
            @endcode

            @param  None

            @retval None

            @see PS::Sock::IO::IReader
          */
          virtual void EmptyQueue
          (
            void
          ) = 0;
      };

    } /* namespace IO */
  } /* namespace Sock */
} /* namespace PS */

#endif /* PS_SOCK_IO_IREADERLISTENER_H */
