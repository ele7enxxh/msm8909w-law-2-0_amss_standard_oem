#ifndef PS_SOCK_IO_IWRITERLISTENER_H
#define PS_SOCK_IO_IWRITERLISTENER_H
/*===========================================================================
  @file PS_Sock_IO_IWriterListener.h

  This file defines an interface to notify the users of IWriter objects when
  there is room in the data pipe to write more data.

  Copyright (c) 2009 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/interface/pssock/inc/PS_Sock_IO_IWriterListener.h#1 $
  $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/
/*lint -esym(1510, IWriterListener) */
namespace PS
{
  namespace Sock
  {
    namespace IO
    {
      /**
        @brief Provides notifications to the users of IWriter objects

        Defines methods which are called when a IWriter object has room for
        the user to write more data. Users interested in these notifications
        has to implement this interface.

        @see PS::Sock::IO::IWriter
      */
      class IWriterListener
      {
        public:
          /**
            @brief Notifies when writer has room to accept more data

            Notifies the implentor of this interface when there is room in
            the data pipe. This method is called only when data pipe was full
            and the reader read some data there by freeing space in the data
            pipe. Upon notification, user can write more data.

            Data pipe is treated as a WM and this method is called when low WM
            is hit. Reason is that if the writer is notified as soon as some
            data is read, then there could be lot of context switching between
            reader and writer (For example, if writer in UI task is notified
            when reader in PS task reads data from sndq, writer writes one
            more packet, gets EWOULDBLOCK and waits. Reader reads data again
            and gets pre-empted by writer as it is notified. This cycle
            repeats where UI and PS tasks can write/read only one packet at a
            time). To minimize it, writer should be notified only when a
            sufficient amount of space is available.

            Another option is to notify the IWriterListener only when data pipe
            becomes empty but in some cases, writer may need trigger something
            such as disk I/O to get more data. If so, PS would be sitting idle.
            On the other hand, if a non-zero low WM is used, PS would be
            processing packets that are in the pipe while writer schedules
            disk I/O in parallel.

            A typical usage is

            @code
            class A : public IWriterListener
            {
              public:
                A()
                {
                  w.RegisterWriterListener( this);
                }

                void funcA()
                {
                  int32  c;

                  while (true)
                  {
                    c = w.Write( ...);
                    if (-1 == c && DS_EWOULDBLOCK == psErrno)
                    {
                      m.wait();
                    }
                  }
                }

                void BufferAvailable
                {
                  m.release();
                }

              private:
                Mutex   m;
                Writer  w;
            }
            @endcode

            @param  None

            @retval None

            @see PS::Sock::IO::IWriter
          */
          virtual void BufferAvailable
          (
            void
          ) = 0;
      };

    } /* namespace IO */
  } /* namespace Sock */
} /* namespace PS */

#endif /* PS_SOCK_IO_IWRITERLISTENER_H */
