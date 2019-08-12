#ifndef __SIO_MAINPAGE_H__
#define __SIO_MAINPAGE_H__

/*  
  ====================================================================
                             Edit History 
  $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/docsrc/sio/sio_MainPage.h#1 $ $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $
  when      who   what, where, why 
  --------  ----  ----------------------------------------------------
  06/10/10  amv    Created
  
  ====================================================================
 
  Copyright (c) 2010 Qualcomm Technologies Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary and Confidential
 
  ==================================================================== 
*/ 

/**
@mainpage SIO API Documentation

@section S1 Introduction

   SIO allows clients/applications to access SIO-drivers through common
   interface using the unique Port ID associated with driver. SIO provides
   stream based interface to client by providing dynamic stream_id on successful
   sio_open(). Clients can then use the issued stream_id for subsequent calls to
   other SIO client apis. There is 1:1 mapping from Client interface to Driver
   interface. Each driver provides the sio-compliant driver interface, which
   gets registered with SIO dynamically on start up.

@section S2 SIO Client Interface

    Client can use SIO-Client interface to access serial devices. Client
    interface is common for all the supported serial devices.


@subsection SS21 SIO Port ID

    Sio port id is 16 bit number, which comprises SIO_MAJOR_NUMBER and
    SIO_MINOR_NUMBER. (SIO_PORT_ID = DEVICE_ID(SIO_MAJOR_NUMBER,
    SIO_MINOR_NUMBER). Upper byte indicates the major number while lower byte
    indicates the minor number. Clients can use SIO port id to communicate with
    given logical port of a particular driver, where logical port is identified
    by minor number and the driver is identified by major number. SIO major
    number is assigned to each unique driver interface. Each Major number can
    support certain number of minor numbers (logical ports), maximum minor
    number is limited to 64. In other words, drivers can support as many as 64
    logical ports with one major number.

    SIO_PORT_ID = DEVICE_ID(major, minor) => ((major << 8) | (minor & 0x00ff))

@subsection SS22 A SIO Stream ID

    Sio issues dynamically allocated stream_id on successful sio_open() and
    sio_control_open(), Clients can use the issued stream id to make calls to
    subsequent sio client apis. However, clients should not use stream_id after
    calling sio_close() as stream_id is disposed on sio_close. Sio issues
    different stream id for sio_open() and sio_control_open().

@subsection SS23 Data Stream Interface
    <ul>
      <li>   sio_stream_id_type sio_open( sio_open_type *open_ptr);
      <li>   void sio_close(sio_stream_id_type stream_id, void
         (*close_func_ptr)(void));
      <li>   void sio_transmit( sio_stream_id_type stream_id, dsm_item_type
          tx_ptr );
      <li>   void sio_flush_tx( sio_stream_id_type stream_id, void
         (*close_func_ptr)(void));
      <li>   void sio_ioctl(sio_stream_id_type stream_id, sio_ioctl_cmd_type
         cmd, sio_ioctl_param_type *param);
    </ul>

@subsection SS24 Control Stream Interface

    <ul>
      <li>   sio_stream_id_type sio_control_open( sio_open_type *open_ptr);
      <li>   void sio_control_close( sio_stream_id_type stream_id, void
         (*close_func_ptr)(void));
      <li>   void sio_control_transmit( sio_stream_id_type stream_id,
          dsm_item_type tx_ptr);
      <li>   void sio_control_ioctl(sio_stream_id_type
         stream_id,sio_ioctl_cmd_type cmd,sio_ioctl_param_type *param);
    </ul>


@section S3 SIO Driver Interface

    SIO device driver registers following interface dynamically on powerup. SIO
    uses stub functions if some of the apis are not registered.

@subsection SS31 Data Interface


  <ul>
    <li>   sio_status_type (*HW_open) (sio_open_type *open_ptr);
    <li>   void (*HW_close) (sio_stream_id_type stream_id,sio_port_id_type
       port_id,void (*close_func_ptr)(void));
    <li>   void (*HW_ioctl) (sio_stream_id_type stream_id,sio_port_id_type
       port_id,sio_ioctl_cmd_type cmd,sio_ioctl_param_type *param);
    <li>   void (*HW_transmit) ( sio_stream_id_type stream_id,sio_port_id_type
       port_id,dsm_item_type *tx_ptr);
    <li>   void (*HW_flush_tx) (sio_stream_id_type stream_id,sio_port_id_type
       port_id,void (*flush_func_ptr)(void));
    <li>   void (*HW_disable_device) (sio_port_id_type port_id);
  </ul>


@subsection SS32 Control Interface

  <ul>
    <li>   sio_status_type (*HW_control_open) (sio_open_type *open_ptr);
    <li>   void (*HW_control_close) (sio_stream_id_type
       stream_id,sio_port_id_type port_id,void (*close_func_ptr)(void));
    <li>   void (*HW_control_ioctl) (sio_stream_id_type
       stream_id,sio_port_id_type port_id,sio_ioctl_cmd_type
       cmd,sio_ioctl_param_type *param);
    <li>   void (*HW_control_transmit) (sio_stream_id_type
       stream_id,sio_port_id_type port_id,dsm_item_type *tx_ptr);
  </ul>


@section S4 Initialization

    The new SIO is initialized by calling sio_init( ). Once this call occurs, it
    initializes the hardware and software SIO components. It should be made only
    once within the software. An application can then call sio_open( ) to gain
    control of a particular port.

    SIO calls driver's initialization api (uint32
    sio_xxx_driver_init(sio_register_device_driver_cb_type
    register_driver_cb_fn)) in sio_init() to initialize each device driver.
    Driver then registers its interface with SIO using callback function  passed
    in initialization api.

    In order to use SIO services, each application must open a port using
    sio_open( ) before performing any other SIO operation. The application
    passes a structure to sio_open( ) that defines some of the behavior of the
    port. The structure defines: Port to use (primary/auxiliary UART, USB,
    Bluetooth?) Bit rate (ignored for USB, Bluetooth) Mode (Autodetect, Packet,
    Raw Data) Rx and Tx watermarks Flow control method (hardware, software,
    etc.), Optional Rx callback. Data is moved between the device driver and the
    application through dsm small items and watermarks. The dsm watermarks are
    created by the application and registered with the SIO with the sio_open( )
    call.




@section S5 Data transfers

@subsection SS51 Transmitting data

    An application/client transmits data by calling sio_transmit( ). The data to
    be transmitted is stored in a dsm small item, which is then passed as a
    parameter to the function. If there is no data preceding this small item,
    then the data from the dsm small item is immediately put into the Tx FIFO.
    Otherwise, the SIO driver enqueues the dsm small item onto the Tx watermark.
    As the Tx FIFO empties, the SIO driver dequeues the oldest item from the
    registered Tx watermark and puts the data contents of the item into the Tx
    FIFO. This continues until the watermark is empty.


@subsection SS52 Receiving data

    When data comes into the Rx FIFO, the SIO driver stores the received data
    into a dsm small item and then enqueues the item onto the registered Rx
    watermark. If the user has defined an optional Rx callback, then the SIO
    driver calls this function instead, passing the same dsm small item. An
    application knows when to read from the watermark via a REX signal. The
    signal can be set in one of two ways:
  <ul>
    <li> Based on the Rx watermark :- An application can configure the watermark to
    have lo-water and hi-water marks. These watermarks represent specific data
    byte counts stored in the watermark. When these watermark thresholds are met
    or surpassed, registered functions (also configured by the application) are
    called. Generally, these functions set the appropriate REX signal to wake up
    the interested task.
    <li> Optional Rx callback :- Alternately, the optional Rx callback,
    specified in sio_open( ), may process the dsm small item and then set a
    signal for the interested task.
  </ul>
    NOTE: In either case, these functions are called in interrupt context, and
    must execute quickly.

@section S6 Control and miscellaneous

    When an application no longer needs a device port that the application
    controls (that is, currently has open), it should call sio_close( ) to
    relinquish the port to allow another application to use it. Applications may
    perform various control functions to an open port using sio_ioctl( ) &
    sio_control_ioctl()

*/

#endif /* #ifndef __SIO_MAINPAGE_H__ */
