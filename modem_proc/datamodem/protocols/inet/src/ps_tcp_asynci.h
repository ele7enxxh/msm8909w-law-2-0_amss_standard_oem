#ifndef PS_TCP_ASYNCI_H
#define PS_TCP_ASYNCI_H
/*===========================================================================

  @file PS_TCP_ASYNCI.h

  This file extends implementation of SocketIO abstract class to support a
  data pipe which doesn't maintain message boundaries. This class is used by
  TCP sockets.
 
  The Class Definitions section is applicable only to C++ code and should be
  removed for C.

  Copyright (c) 2010 - 2011 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
===========================================================================


===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_tcpsubr.c_v   1.0   08 Aug 2002 11:19:52   akhare  $
  $Header $ $DateTime: 2016/12/13 08:00:03 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---    ----------------------------------------------------------
08/29/11   dvk     Fixed KW error.
08/27/10   sn      Fixed compilation errors.
07/26/10   ps      Created the module.
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"       /* Customer Specific Features */
#include "target.h"

#ifdef FEATURE_DATA_IS707_ASYNC
#include "ps_tcp_async.h"

#include "PS_Sock_IO_SocketIOFactory.h"
#include "PS_Sock_IO_SocketIO.h"
#include "ds_Utils_CreateInstance.h"
#include "ds_Utils_Conversion.h"
#include "ds_Errors_Def.h"
#include "ds_Sock_AddrUtils.h"
#include "ps_mem_ext.h"


using namespace PS::Sock::IO;
using namespace ds::Error;
using namespace ds::Sock;


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

namespace PS
{
  namespace Sock
  {
    namespace IO
    {
      /**
        @brief Defines an interface to implement Async/fax functionality.

        It is a singleton class whicH defines methods to allow the client 
        to setup a Asynchronus path and transfer data bidirectionally.

      */
      class Async :public PS::Sock::IO::IReaderListener,
                    public PS::Sock::IO::IWriterListener
      {

        private:
	   	
        ICritSect               *critSectPtr;
        IReader                 *readerPtr_Recvq;
        IWriter                 *writerPtr_Sendq;
        ps_async_func_tbl_type  ps_async_func_tbl;
        int32                   pcb_handle;
	   	
        /**
          @brief Constructor for Async class. 

          Initializes a Async class object.

          @param Pointer to function table for callbacks.
               
          @retval pointer to object of async class
                 
        */

	       Async
        (
          ps_async_func_tbl_type *func_table
        );


        public:

        /**
          @brief Interface to client to create instance Async connection. 

          It performs all the intializations like setting up of Data 
          pipe on both send and recieve path and tcb block intialization 
          etc. This is a static function and ensure the singleton client 
          of Async class. 
	
          @param pointer to function table for required callbacks and 
          pointer to iface block having routing info.
				  
	  @retval Handle of Async class.
						  
	*/
        static int32 CreateInstance
        (
          ps_async_func_tbl_type *func_table,
          ps_iface_type *iface_ptr
        );
		 
	/**
          @brief Notifies the Async module when there is a space in the 
          send pipe to push data.

          This function is called by TCP routine and will eventually calls           
          707 Asyc module to notify the availabity of space.

          @param None.
				  
	  @retval None
						  
	*/

	virtual void BufferAvailable
        (
           void
        );
	 
	/**
          @brief Notifies the Async module if there is data available in 
          recive pipe to read.
		 
	  This function is called by TCP routine and will eventually calls           
          707 Asyc module to notify the availabity of data.
	 
          @param None.
					  
	  @retval None
					  
	*/

        virtual void DataAvailable
        (
          void
        ) throw();
		 
	/**
          @brief No action
		 
          @param None.
						  
          @retval None
	  
	*/
        virtual void EmptyQueue
        (
          void
        ); 

        /**
	  @brief  Defines a method to Async to shutdown the associated  
	  TCP connection.  
          	 
	  @param boolean flag to indicate whether it is slient closing 
          or not.
					  
	  @retval None
		  
	*/
	void Async_reset_tcp 
        (
          boolean silent_close_flag
        );

	/**
        @brief Defines a method to process the send data pipe. 
        	 
	@param Length of the packet added to write pipe.
						  
        @retval None
		  
        */

	 void Async_tcp_output
         ( 
           int32 pkt_length
         );

	/**
          @brief Defines a method to open a tcp connection for Async call. 
	 
	  @param connection parameters.
						  
	  @retval None
		  
	*/

	void Async_tcp_open
        (
          struct  connection *ps707_async_connection,
          word mode,
          uint16  rwindow,
          uint16  mss
        );

        /**
          @brief Defines a method to put data on send pipe.
 
          @param Pointer to data item.
						  
          @retval No of packets that has been pushed on send pipe.
	  
        */
        int32 Async_send_data
        (
          dsm_item_type   *new_item_ptr
        );

        /**
	  @Defines a method to reterive the local address.
		 
	  @param None.
						  
	  @retval None
		  
	*/

        struct ps_in6_addr Async_get_remote_address
        (
          void
        );

        /**
	  @brief Defines a method to notify Async module for state change. 
	 
	  @param New state.
						  
	  @retval None
		  
        */

	void Async_state_change_upcall
        (
          async_tcp_state_async_type state
        );

	  
	/**
          @brief Defines a method to reterive the local address
   
	  @param None.
								
	  @retval None
				
	*/

        struct ps_in6_addr Async_get_local_address
        (
          void
        );

        /**
	  @brief Defines a method to get , how much data is still outstanding
          in TCP send pipe. This includes un-acknowledged data as well. 
		 
	  @param None.
						  
	  @retval None	  
        */
        uint32 Async_outstanding_data
        (
          void
        );

        /**
	  @brief Defines a method to reterive the TCB handle corresponds to
          Async connection.
		 
	  @param None.
						  
	  @retval TCB handle.	  
	*/

        static int32 Async_get_tcb_handle
        (
          void
        );

        /**
	  @brief Defines a method to reterive the Async handle.
 
	  @param None.
						  
	  @retval Async handle
		  
        */
	static int32 Async_get_async_handle
        (
          void
        );		 
      };

    }

  }

}	
#endif /* FEATURE_DATA_IS707_ASYNC */
#endif  /* PS_TCP_ASYNCI_H */
