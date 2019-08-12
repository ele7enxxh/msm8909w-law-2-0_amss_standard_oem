/*============================================================================

                       U S B H O S T _ A P I . H

=============================================================================*/

/**
@file usbhost_api.h

Contains USB Host API layer data structures and function prototypes.

Externalized functions:
- usbhost_power_vbus
- usbhost_register_pwr_violate
- usbhost_deregister_pwr_violate
- usbhost_notify_appln_pwr_violation
- usbhost_register
- usbhost_deregister
- usbhost_req_open
- usbhost_close
- usbhost_ioctl
- usbhost_close_req_rsp
- usbhost_api_init
*/

/*============================================================================
 Copyright (c) 2005-2010 by Qualcomm Technologies Incorporated.
 All Rights Reserved.
 Qualcomm Confidential and Proprietary.
=============================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

    $Header: //components/rel/tz.bf/2.5.c10/trustzone_images/core/api/wiredconnectivity/usbhost_api.h#1 $
    $DateTime: 2016/06/17 14:31:11 $ $Author: pwbldsvc $

=============================================================================
when      who  what, where, why
--------  ---  --------------------------------------------------------------
06/26/10  lo   (Tech Pubs) Edited/added Doxygen comments and markup.
10/27/06  rlp  Added support for data_report_id and data_unitExpo in the
                 data event structure.
10/20/06   sb  Feature added, APIs added for registration and de registration
                for notification on power violation.
09/07/06  rlp  HID V15 updates.
06/14/06  rlp  Added IOCTL support to get/set hid device idle rate.
               Added timestamp field to usbhost_hid_event_type.
06/12/06  sb   Added the function declaration for usbhost_api_init()
04/12/06  vs   task_table_type_s now has only sem_sig for internal host
                stack usage
04/13/06  rlp  Removed HID ifdef in PdevType. Always need access to these.
04/07/06  rlp  USBHOST_HID_USAGE_SLIDER is now a sampled data event.
04/06/06  rlp  Added matching keyboard usages from those omitted by TDI in
                 hid.h.
               Removed unused USBHOST_HID_USAGE_KEYBOARD.
               Enabled pdev_type USBHOST_PDEV_TYPE_HID_GAME_PAD.
02/23/06  rlp  Remove _POS from X_POS, Y_POS, Z_POS HID definitions.
02/22/06  rlp  Added new keyboard usage definitions.
02/07/06  rlp   Changed client_notify_cb param type from a device instance id
                  to a device handle.
02/06/06  rlp  Adding missing definition for USBHOST_HID_USAGE_SYSMENUSEL.
02/03/06  rlp  Added the following defines from usbhost_api.c to expose them
                 for client visibility:
                   USBHOST_INVALID_CLIENT_ID
                   USBHOST_INVALID_DEV_HNDL
                   USBHOST_CLIENT_DESC_SIZE
               Removed USBHOST_PDEV_TYPE_HID_GENERIC which is not used nor
                 required.
01/31/06  rlp  Fixed usage of device instance id versus a device handle.
               Added USBHOST_OPEN_NO_HANDLES, removed USBHOST_PDEV_TYPE_ALL.
               Added usbhost_ioctl_cmd_type.
01/25/06  rlp  HID related Lint changes prior to HID release.
01/17/06  rlp  Added HID support.  Performed minor code cleanup.
09/07/05   vs  Added struct usbhost_task_table_type
08/02/05   vs  File created.

---------------------------------------------------------------------------*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#ifndef USBHOST_API_H
#define USBHOST_API_H

#include "assert.h"
#include "rex.h"
#include "msg.h"
#include "qw.h"

/** @ingroup hsusb_qc_api
    @{
*/

#define USBHOST_INVALID_CLIENT_ID      0xffff
#define USBHOST_INVALID_DEV_HNDL       0
#define USBHOST_CLIENT_DESC_SIZE       32

/** Device instance for a device.
*/
typedef uint32 usbhost_dev_instance_type;

/** Device handle for a device.
*/
typedef uint32 usbhost_dev_handle_type;

/** Unique client ID assigned at registration time.
*/
typedef uint32 usbhost_client_id_type;

/** Return values from IOCTLs and HOST API functions.
*/
typedef enum
{
  USBHOST_SUCCESS,
  /**< -- IOCTL/host API was successful. */
  USBHOST_ERROR,
  /**< -- IOCTL/command was not successful. */
  USBHOST_ERROR_MEDIA_CHANGED,
  /**< -- Return the value for mass storage specific to CHECK_MEDIA IOCTL. */
  USBHOST_ERROR_MEDIA_NOT_PRESENT,
  /**< -- Return the value for mass storage specific to CHECK_MEDIA IOCTL. */
  USBHOST_MAX_STATUS
  /**< -- Maximum value for type. */
} usbhost_status_type;

/** Indicates the notification type for devices.
*/
typedef enum
{
  USBHOST_DEV_CONNECTED,    /**< -- Device is inserted. */
  USBHOST_DEV_DISCONNECTED, /**< -- Device is unplugged or removed. */
  USBHOST_DEV_ERROR,        /**< -- Erroneous behavior (currently unused). */
  USBHOST_MAX_DEV_STATUS    /**< -- Maximum value for type. */
} usbhost_dev_status_type;

/** Describes the class of a device.
*/
typedef enum
{
  USBHOST_DEV_TYPE_CHAR,  /**< -- Character devices, asynchronous. */
  USBHOST_DEV_TYPE_BLOCK, /**< -- Block devices, synchronous. */
  USBHOST_MAX_DEV_TYPE    /**< -- Maximum value for type. */
} usbhost_dev_type;

/** Device types.
*/
typedef enum
{
  USBHOST_PDEV_TYPE_HID_GENERIC,   /**< -- Generic device. */
  USBHOST_PDEV_TYPE_HID_MOUSE,     /**< -- Mouse device. */
  USBHOST_PDEV_TYPE_HID_KEYBOARD,  /**< -- Keyboard device. */
  USBHOST_PDEV_TYPE_HID_JOYSTICK,  /**< -- Joystick device. */
  USBHOST_PDEV_TYPE_HID_GAME_PAD,  /**< -- Gamepad device. */
  USBHOST_PDEV_TYPE_HID_RESERVED2, /**< -- Reserved for future HID device. */
  USBHOST_PDEV_TYPE_HUB,           /**< -- Hub. */
  USBHOST_PDEV_TYPE_MASS_STORAGE,  /**< -- Mass storage device. */
  USBHOST_PDEV_TYPE_PRINTER,       /**< -- Printer device. */
  USBHOST_PDEV_TYPE_STILLIMG,      /**< -- Still image device. */
  USBHOST_PDEV_TYPE_HID_XBOX,      /**< -- Microsoft XBOX360 gamepad device. */
  USBHOST_MAX_PDEV_TYPE            /**< -- Maximum value for type. */
} usbhost_pdev_type;

/** If a device is unavailable, specify whether the client should try again
    or keep its request queued.
*/
typedef enum
{
  USBHOST_OPEN_RET_RESCHEDULE,
  /**< -- Keep the request queued with the API. */
  USBHOST_OPEN_RET_UNSCHEDULE,
  /**< -- Removed the request from the API; the client will try again. */
  USBHOST_MAX_OPEN_RET
  /**< -- Maximum value for type. */
} usbhost_open_ret_type;

/** Status of the device that was signaled from the API.
*/
typedef enum
{
  USBHOST_OPEN_AVAILABLE,     /**< -- Device is available for use. */
  USBHOST_OPEN_NOT_AVAILABLE, /**< -- Device is unavailable for use. */
  USBHOST_OPEN_NO_HANDLES,    /**< -- Device is out of handles. */
  USBHOST_MAX_OPEN_STATUS     /**< -- Maximum value for type. */
} usbhost_open_status;

/** Client's response type when the API requests the client to close a
    communication path with the device.
*/
typedef enum
{
  USBHOST_CLOSE_RSP_RELEASE,
    /**< -- Release the device; the client will call usbhost_close
            subsequently. */
  USBHOST_CLOSE_RSP_RETAIN,
    /**< -- Retain the handle; the client chooses not to relinquish control. */
  USBHOST_MAX_CLOSE_RSP
    /**< -- Maximum value for type. */
}usbhost_close_rsp_type;

/** Client's set of signals and the watchdog timer function.
*/
typedef struct usbhost_task_table_type_s
{
  uint32 sem_wait_sig; /**< Signal for the semaphore. */
  uint32 wait_sig;     /**< Deprecated signal. */
  uint32 dog_rpt_sig;  /**< Signal for dog timer function. */
  void (*dog_rpt_fn_ptr)(void);  /**< Pointer to the dog timer function. */
} usbhost_task_table_type;

/** Informs a client application of a power violation.
*/
typedef enum
{
  USBHOST_PWR_MAX_NOT_SUPPORTED = 0,
    /**< -- Maximum power at enumeration exceeds the phone power capacity. */
  USBHOST_MAX_PWR_EXCEEDED,
    /**< -- Current violation was detected. */
  USBHOST_MAX_PWR
    /**< -- Maximum value for type. */
} usbhost_pwr_violate_type;

/** Identifies a device and the type of power violation.
*/
typedef struct usbhost_pwr_violate_notification_type_s
{
  uint16                                 d_productID;
    /**< Device product identifier: four-digit BCD. */
  uint16                                 d_vendorID;
    /**< Device vendor identifier: four-digit BCD. */
  usbhost_pwr_violate_type                violate;
    /**< Power violation. */
} usbhost_pwr_violate_notification_type;

/** Device IOCTL enumerations.
*/
typedef enum
{
  USBHOST_IOCTL_RESERVED = 0,             /**< &nbsp; */
  USBHOST_IOCTL_GET_DEV_INFO,
  /**< -- Available for all connected devices.\ Returns information pertaining
          to the device, such a product ID, manufacturer ID, vendor ID, etc. */

  /* Mass Storage device commands */
  USBHOST_IOCTL_STOR_CHECK_MEDIA,         /**< &nbsp; */
  USBHOST_IOCTL_STOR_STORAGE_TYPE,        /**< &nbsp; */
  USBHOST_IOCTL_STOR_MEDIA_REMOVABLE,     /**< &nbsp; */
  USBHOST_IOCTL_STOR_MEDIA_WRITE_PROTECT, /**< &nbsp; */
  USBHOST_IOCTL_STOR_BLOCK_SIZE,          /**< &nbsp; */
  USBHOST_IOCTL_STOR_NUMBER_OF_BLOCKS,    /**< &nbsp; */
  USBHOST_IOCTL_STOR_BLOCK_READ,          /**< &nbsp; */
  USBHOST_IOCTL_STOR_BLOCK_WRITE,         /**< &nbsp; */

  /* HID device commands */
  USBHOST_IOCTL_HID_SET_CONFIGURATION,    /**< &nbsp; */
  USBHOST_IOCTL_HID_GET_CONFIGURATION,    /**< &nbsp; */
  USBHOST_IOCTL_HID_READ_SAMPLED,         /**< &nbsp; */
  USBHOST_IOCTL_HID_GET_ATTRIB,           /**< &nbsp; */
  USBHOST_IOCTL_HID_SET_IDLE_RATE,        /**< &nbsp; */
  USBHOST_IOCTL_HID_GET_IDLE_RATE,        /**< &nbsp; */
  USBHOST_IOCTL_HID_SET_PROTOCOL,         /**< &nbsp; */
  USBHOST_IOCTL_HID_GET_PROTOCOL,         /**< &nbsp; */
  USBHOST_IOCTL_HID_SET_REPORT,           /**< &nbsp; */
  USBHOST_IOCTL_HID_GET_REPORT,           /**< &nbsp; */
  USBHOST_IOCTL_HID_VIRT_CABLE_UNPLUG     /**< &nbsp; */
} usbhost_ioctl_cmd_type;

/*=============================================================================
FUNCTION    usbhost_power_vbus
=============================================================================*/
/**
Acquires or relinquinshes the power to VBUS.

@param acquire      [IN] -- TRUE: acquire VBUS; FALSE: relinquish VBUS.
@param session_time [IN] -- If acquire is TRUE, this indicates the time until
                            the VBUS will be powered to detect a device. A
							value of -1 applies power indefinitely.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void usbhost_power_vbus
(
  int8    session_time,
  boolean acquire
);

/** Device power violation notification.
*/
typedef void (*FUNCPTR_APPLN_ERR_NOTIFY) (usbhost_pwr_violate_notification_type *info);

/** Number of applications that can be supported.
*/
#define USBHOST_MAX_NUM_APPLN_REG_FOR_ERR_NOTIFY 10

/*=============================================================================
FUNCTION    usbhost_register_pwr_violate
=============================================================================*/
/**
Registers applications for error notification during enumeration.

@param *power_violate_notify_cb [IN] -- Callback function to be called upon a
                                        power violation.
@param *pwr_violate_info        [IN] -- Power violation information

@dependencies
Allows only USBHOST_MAX_NUM_APPLN_REG_FOR_ERR_NOTIFY to register.

@sideeffects
None.

@return
Handle to be used during deregisteration.
*/
usbhost_client_id_type usbhost_register_pwr_violate(
  void (* power_violate_notify_cb)   (usbhost_pwr_violate_notification_type *pwr_violate_info)
);

/*=============================================================================
FUNCTION    usbhost_deregister_pwr_violate
=============================================================================*/
/**
Deregisters applications for error notification during enumeration.

@param client_id [IN] -- Client ID.

@dependencies
Allows only USBHOST_MAX_NUM_APPLN_REG_FOR_ERR_NOTIFY to de-register.

@sideeffects
None.

@return
Handle to be used during de-registeration.
*/
usbhost_status_type usbhost_deregister_pwr_violate(
  usbhost_client_id_type client_id
);

/*=============================================================================
FUNCTION    usbhost_notify_appln_pwr_violation
=============================================================================*/
/**
Notifies registered applications of a power violation.

@param *pwr_violate_info [IN] -- Power violation description.

@dependencies
None.

@sideeffects
None.

@return
If the notification succeeded or an error.
*/
void usbhost_notify_appln_pwr_violation(
  usbhost_pwr_violate_notification_type *pwr_violate_info);

/*=============================================================================
FUNCTION    usbhost_register
=============================================================================*/
/**
Clients use this function to register with the API.

Actions performed:
  - Add the client to the table.
  - Check for devices of the same type; if there are any connected, call the
    callback function to notify the client of the presence of all devices of
	that type.

@param pdev_type      [IN] -- Device type for which the client is registering.
@param *client_desc   [IN] -- Description string for the client.
@param client_sigs    [IN] -- Set of signals and dog timer function specified
                              by the client.
@param *dev_notify_cb [IN] -- Pointer to the client's callback function for
                              for notifying the clients of device instances.
@param dev_type       [IN] -- Device type.
@param dev_stat       [IN] -- Device status (connected or disconnected).
@param dev_inst       [IN] -- Device instance.
@param *param         [IN] -- Generic parameter to be returned to the client.

@dependencies
None.

@sideeffects
None.

@return
Returns a unique client ID if registrations was successful, else
returns an invalid client ID.
*/
usbhost_client_id_type usbhost_register
(
   usbhost_pdev_type pdev_type,
   const char * client_desc,
   usbhost_task_table_type client_sigs,
   void (* dev_notify_cb)
   (usbhost_pdev_type dev_type,
   usbhost_dev_status_type dev_stat,
   usbhost_dev_instance_type dev_inst,
   void * param
    ),
 
  /* Void * pointer for client to specify a pointer */
  /* This pointer will be passed back on each callback function */
  void * param 
);

/*=============================================================================
FUNCTION    usbhost_deregister
=============================================================================*/
/**
Clients use this function to deregister from the API.

Actions performed:
  - Check if the client exists in the table.
    If so, remove the client from the table and free up memory.
  - If the client is in the pending queue for the device type
    it is deregistering for, remove it.

@param client_id [IN] -- Client ID, assigned at registration time.
@param pdev_type [IN] -- Device type for which the client is deregistering.

@dependencies
None.

@sideeffects
None.

@return
USBHOST_SUCCESS -- Deregistration was successful.\n
USBHOST_ERROR   -- Deregistration failed.

*/
usbhost_status_type usbhost_deregister
(
  usbhost_client_id_type client_id,
  usbhost_pdev_type pdev_type
);

/*=============================================================================
FUNCTION    usbhost_req_open
=============================================================================*/
/**
Opens a communication path with a device after a client's successful
registration and notification of the presence of a device of the same type.
                                                      
Actions performed:
- Check if it is a valid device handle.
- Check if the client has registered for the device type.
- If there is no other device, call open_notify_fn.
- If there is another client using the device, remember that a client is
  waiting on a particular device and send close_notify() to the current client.

@param client_id       [IN] -- Client ID, assigned at registration time.
@param dev_inst        [IN] -- Device instance for which the client is
                               requesting a handle.
@param *open_notify_cb [IN] -- Callback function specified by the client. This
                               is invoked by the API when it needs to notify
							   the client whether the device is available.
@param close_req_cb    [IN] -- Callback function specified by the client. This
                               is invoked by the API when it requests the
							   client to close a communication path (handle)
							   with a device.

@dependencies
None.

@sideeffects
None.

@return
USBHOST_SUCCESS -- Open was processed successfully.\n
USBHOST_ERROR   -- Open was not processed successfully.
*/
usbhost_status_type usbhost_req_open
(
  usbhost_client_id_type client_id,
  usbhost_dev_instance_type dev_inst,
  usbhost_open_ret_type (* open_notify_cb)
    (
      usbhost_dev_instance_type, /**< Device instance for which the client is
								      being notified. */
      usbhost_dev_handle_type,   /**< Device handle allocated for the
								      communication path. */
      usbhost_open_status,       /**< Status of notification -- whether or not
								      available. */
      char *,            /**< If not available -- the client description of the
					          client that currently holds the handle and is not
							  giving up the handle. */
      void * param               /**< Generic parameter to be returned to the
                                      client. */
    ),
  
  void (* close_req_cb)          /**< Close callback function. */
    (
      usbhost_dev_handle_type,   /**< Device handle of the device the API is
								      requesting the client to close. */
      char *,                    /**< The client description of the client that
								      wants to gain access to the device. */
      usbhost_client_id_type,    /**< The client ID of the client that wants to
								       gain access to the device. */
      void * param               /**< Generic parameter to be returned to the
                                      client. */
    )
);

/*=============================================================================
FUNCTION    usbhost_close
=============================================================================*/
/**
Closes a communication path with a device after the client's successful
registration and notification of the presence of a device of the same type.
                                                      
Actions performed:
- Check if it is a valid client; if so, set curr_client to NULL.
- See if there is any existing client waiting for the device;
  pick the first in the queue and call its open_notify() function.

@param client_id [IN] -- Client ID, assigned at registration time.
@param dev_hndl  [IN] -- Device handle of the device the client is closing.

@dependencies
None.

@sideeffects
None.

@return
USBHOST_SUCCESS -- Close was successful.\n
USBHOST_ERROR   -- Close was not successful.
*/
usbhost_status_type usbhost_close
(
  usbhost_client_id_type client_id,
  usbhost_dev_handle_type dev_hndl
);

/*=============================================================================
FUNCTION    usbhost_ioctl
=============================================================================*/
/**
IOCTL function to communicate with the device.

@param client_id [IN] -- Client ID, assigned at registration time.
@param dev_hndl  [IN] -- Device handle.
@param command   [IN] -- Device-specific IOCTL command.
@param *parm     [IN] -- IOCTL-specific parameter.

@dependencies
None.

@sideeffects
None.

@return
Other device specific return types; IOCTL returns:
- USBHOST_SUCCESS -- IOCTL was successful.
- USBHOST_ERROR   -- IOCTL was not successful.
*/
usbhost_status_type usbhost_ioctl
(
  usbhost_client_id_type client_id,
  usbhost_dev_handle_type dev_hndl,
  usbhost_ioctl_cmd_type command,
  void * parm
);

/*============================================================================
FUNCTION    usbhost_close_req_rsp
=============================================================================*/
/**
Used by clients to notify the API whether it is willing to relinquish
control of the device.

@param client_id       [IN] -- Client ID, assigned at registration time.
@param client_id_close [IN] -- Client ID for the client requesting access to
                               the device.
@param dev_hndl        [IN] -- Device handle.
@param response        [IN] -- Client's response to the API to relinquish
                               control of the device.

@dependencies
None.

@sideeffects
None.

@return
USBHOST_SUCCESS -- Notification was valid.\n
USBHOST_ERROR   -- Notification was invalid.
*/
usbhost_status_type usbhost_close_req_rsp
(
  usbhost_client_id_type client_id,
  usbhost_client_id_type client_id_close,
  usbhost_dev_handle_type dev_hndl,
  usbhost_close_rsp_type response
);


/*=============================================================================
FUNCTION    usbhost_api_init
=============================================================================*/
/**
Initializes the data structures for the API. Called from TMC.

@dependencies
None.

@sideeffects
None.

@return
None.
*/
void usbhost_api_init
(
  void
);

/* Device specific data structures */

/** IOCTL transfer structure.
*/
typedef struct _usbhost_IoctlTransfer
{
  void  *buffer;         /**< Buffer to be sent. */
  uint32 length;         /**< Length of the buffer. */
  uint32 startBlock;     /**< Start block. */
  uint32 numberBlocks;   /**< Number of blocks. */
} usbhost_IoctlTransfer; 


/** IOCTL structure to retrieve device and connection information via the
usbhost_ioctl_get_dev_info() command.
*/
typedef struct _usbhost_IoctlDevInfo
{
  uint16        d_productID;      /**< Device product identifier. */
  uint16        d_vendorID;       /**< Device vendor identifier. */
  uint8         d_manufacturerID; /**< Manufacturer identifer. */
  uint8         d_class;          /**< Device class. */
  uint8         d_subclass;       /**< Device subclass.*/
  uint8         d_protocol;       /**< Device protocol. */
} usbhost_ioctl_devInfo_type;

typedef usbhost_ioctl_devInfo_type usbhost_IoctlDevinfo;


/*=======================================================================

 PRIMARY HID DEFINITIONS AND SUPPORT STRUCTURES

=======================================================================*/


/*=======================================================================
  Defines
=======================================================================*/

/** Macro to check a data event report's data type.

Sample usage: Check if the field data type is relative.
@verbatim
     U8 data_type;

     data_type = USBHOST_HID_GetDataType(event->data_flag,
                                         USBHOST_HID_DATATYPE_RELATIVE);

     if (data_type == USBHOST_HID_DATATYPE_RELATIVE)
     {
       // process relative data
     }
@endverbatim
*/
#define USBHOST_HID_GetDataType(data_flag, data_type) HID_CheckTypeData(data_flag, data_type)

/** Macro to extract systems of units and unit exponents from a data report 
   (internally calls the Oxford API to retrieve the value).

 Sample usage 1: Get the system of units.
@verbatim
     U8 system_units;

     system_units = USBHOST_HID_GetUnit(event->data_unit, USBHOST_HID_UNIT_SYSTEM);

     switch (system_units)
     {
       case USBHOST_HID_SYSTEM_NONE:
         // blah
         break;
       case USBHOST_HID_SYSTEM_SI_LINEAR:
         // blah
         break;
       // etc
     }
@endverbatim
 Sample usage 2: Get the exponent of length in the system units
@verbatim
     U8 length_expo;

     length_expo = USBHOST_HID_GetUnit(event->data_unitExpo, USBHOST_HID_UNIT_LENGTH);
@endverbatim
*/
#define USBHOST_HID_GetUnit(unit, index) HID_GetUnit(unit, index)

/* HID Unit Indices */
#define USBHOST_HID_UNIT_SYSTEM             0
#define USBHOST_HID_UNIT_LENGTH             1
#define USBHOST_HID_UNIT_MASS               2
#define USBHOST_HID_UNIT_TIME               3
#define USBHOST_HID_UNIT_TEMPERATURE        4
#define USBHOST_HID_UNIT_CURRENT            5
#define USBHOST_HID_UNIT_LUMINOUS_INTENSITY 6

/* HID Unit Systems */
#define USBHOST_HID_SYSTEM_NONE             0x0
#define USBHOST_HID_SYSTEM_SI_LINEAR        0x1
#define USBHOST_HID_SYSTEM_SI_ROTATION      0x2
#define USBHOST_HID_SYSTEM_ENGLISH_LINEAR   0x3
#define USBHOST_HID_SYSTEM_ENGLISH_ROTATION 0x4


/* HID Field Data Types */
#define USBHOST_HID_DATATYPE_DATA         0x00010000
#define	USBHOST_HID_DATATYPE_CONSTANT     0x00010001
#define	USBHOST_HID_DATATYPE_ARRAY        0x00020000
#define	USBHOST_HID_DATATYPE_VARIABLE     0x00020002
#define	USBHOST_HID_DATATYPE_ABSOLUTE     0x00040000
#define	USBHOST_HID_DATATYPE_RELATIVE     0x00040004
#define	USBHOST_HID_DATATYPE_NOWRAP       0x00080000
#define	USBHOST_HID_DATATYPE_WRAP         0x00080008
#define	USBHOST_HID_DATATYPE_LINEAR       0x00100000
#define	USBHOST_HID_DATATYPE_NONLINEAR    0x00100010
#define	USBHOST_HID_DATATYPE_PREFERED     0x00200000
#define	USBHOST_HID_DATATYPE_NONPREFERED  0x00200020
#define	USBHOST_HID_DATATYPE_NONULL       0x00400000
#define	USBHOST_HID_DATATYPE_NULL         0x00400040
#define	USBHOST_HID_DATATYPE_BITFIELD     0x01000000
#define	USBHOST_HID_DATATYPE_BUFFERED     0x01000100

#define USBHOST_HID_CAPS_LOCK_ON          0x0001
#define USBHOST_HID_SCROLL_LOCK_ON        0x0002
#define USBHOST_HID_NUM_LOCK_ON           0x0004
#define USBHOST_HID_SHIFT_KEY_ACTIVE      0x0008
#define USBHOST_HID_CTRL_KEY_ACTIVE       0x0010
#define USBHOST_HID_ALT_KEY_ACTIVE        0x0020
#define USBHOST_HID_GUI_KEY_ACTIVE        0x0040

/*=======================================================================
  Typedefs and Enumerations
=======================================================================*/

/** Human Interface Device usage identifiers.
*/
typedef enum
{
  USBHOST_HID_USAGE_NONE = 0,      /**< &nbsp; */
  USBHOST_HID_USAGE_BUTTON,        /**< &nbsp; */
  USBHOST_HID_USAGE_DIAL,          /**< &nbsp; */
  USBHOST_HID_USAGE_WHEEL,         /**< &nbsp; */
  USBHOST_HID_USAGE_HATSWITCH,     /**< &nbsp; */
  USBHOST_HID_USAGE_BYTECOUNTER,   /**< &nbsp; */
  USBHOST_HID_USAGE_MOTIONWAKEUP,  /**< &nbsp; */
  USBHOST_HID_USAGE_START,         /**< &nbsp; */
  USBHOST_HID_USAGE_SELECT,        /**< &nbsp; */
  USBHOST_HID_USAGE_SYSCONTRL,     /**< &nbsp; */
  USBHOST_HID_USAGE_SYSPOWERDOWN,  /**< &nbsp; */
  USBHOST_HID_USAGE_SYSSLEEP,      /**< &nbsp; */
  USBHOST_HID_USAGE_SYSWAKE,       /**< &nbsp; */
  USBHOST_HID_USAGE_SYSCONMENU,    /**< &nbsp; */
  USBHOST_HID_USAGE_SYSMAINMENU,   /**< &nbsp; */
  USBHOST_HID_USAGE_SYSAPPMENU,    /**< &nbsp; */
  USBHOST_HID_USAGE_SYSMENUHELP,   /**< &nbsp; */
  USBHOST_HID_USAGE_SYSMENUEXIT,   /**< &nbsp; */
  USBHOST_HID_USAGE_SYSMENUSEL,    /**< &nbsp; */
  USBHOST_HID_USAGE_SYSMENURIGHT,  /**< &nbsp; */
  USBHOST_HID_USAGE_SYSMENULEFT,   /**< &nbsp; */
  USBHOST_HID_USAGE_SYSMENUUP,     /**< &nbsp; */
  USBHOST_HID_USAGE_SYSMENUDOWN,   /**< &nbsp; */
  USBHOST_HID_USAGE_DPADUP,        /**< &nbsp; */
  USBHOST_HID_USAGE_DPADDOWN,      /**< &nbsp; */
  USBHOST_HID_USAGE_DPADRIGHT,     /**< &nbsp; */
  USBHOST_HID_USAGE_DPADLEFT,      /**< &nbsp; */

  /* Sampled Data Usages Go Here - ** DO NOT ALTER POSITIONING */

  USBHOST_HID_USAGE_X,             /**< &nbsp; */
  USBHOST_HID_USAGE_Y,             /**< &nbsp; */
  USBHOST_HID_USAGE_Z,             /**< &nbsp; */
  USBHOST_HID_USAGE_RX,            /**< &nbsp; */
  USBHOST_HID_USAGE_RY,            /**< &nbsp; */
  USBHOST_HID_USAGE_RZ,            /**< &nbsp; */
  USBHOST_HID_USAGE_VX,            /**< &nbsp; */
  USBHOST_HID_USAGE_VY,            /**< &nbsp; */
  USBHOST_HID_USAGE_VZ,            /**< &nbsp; */
  USBHOST_HID_USAGE_VBRX,          /**< &nbsp; */
  USBHOST_HID_USAGE_VBRY,          /**< &nbsp; */
  USBHOST_HID_USAGE_VBRZ,          /**< &nbsp; */
  USBHOST_HID_USAGE_VNO,           /**< &nbsp; */
  USBHOST_HID_USAGE_SLIDER,        /**< &nbsp; */

  /* End of Sampled Data Usages */

  /* Keyboard device usages */
  USBHOST_HID_USAGE_KEY_A,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_B,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_C,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_D,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_E,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_F,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_G,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_H,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_I,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_J,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_K,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_L,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_M,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_N,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_O,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_P,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_Q,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_R,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_S,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_T,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_U,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_V,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_W,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_X,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_Y,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_Z,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_1,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_2,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_3,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_4,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_5,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_6,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_7,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_8,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_9,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_0,         /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_ENTER,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_ESCAPE,    /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_BACKSPACE, /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_TAB,       /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_SPACE,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_MINUS,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_EQUAL,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_LEFT_BRACKET,  /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_RIGHT_BRACKET, /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_BACKSLASH, /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_SEMICOLON, /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_QUOTE,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_GRAVE,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_COMMA,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PERIOD,    /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_FORWARD_SLASH,  /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_CAPS_LOCK, /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_F1,        /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_F2,        /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_F3,        /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_F4,        /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_F5,        /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_F6,        /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_F7,        /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_F8,        /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_F9,        /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_F10,       /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_F11,       /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_F12,       /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PRINT_SCREEN, /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_SCROLL_LOCK,  /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAUSE,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_INSERT,    /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_HOME,      /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAGE_UP,   /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_DELETE,    /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_END,       /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAGE_DOWN, /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_RIGHT,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_LEFT,      /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_DOWN,      /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_UP,        /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_NUM_LOCK,  /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_DIVIDE,   /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_MULTIPLY, /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_SUBTRACT, /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_ADD,   /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_ENTER, /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_1,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_2,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_3,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_4,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_5,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_6,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_7,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_8,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_9,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_0,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_DECIMAL,   /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_PAD_EQUAL,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_LEFT_CONTROL,  /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_LEFT_SHIFT,    /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_LEFT_ALT,  /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_LEFT_GUI,  /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_RIGHT_CONTROL, /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_RIGHT_SHIFT,   /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_RIGHT_ALT,     /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_RIGHT_GUI,  /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_NON_US_BACKSLASH, /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_APPL,       /**< &nbsp; */
  USBHOST_HID_USAGE_KEY_POWER,      /**< &nbsp; */

  /* LED device usages */
  USBHOST_HID_USAGE_LED_NUMLOCK,    /**< &nbsp; */
  USBHOST_HID_USAGE_LED_CAPSLOCK,   /**< &nbsp; */
  USBHOST_HID_USAGE_LED_SCROLLLOCK, /**< &nbsp; */

/* Reserved for future device support and release 

   Simulation Device Usages 
  USBHOST_HID_USAGE_FLIGHT_CTRL_STICK,
  USBHOST_HID_USAGE_FLIGHT_CTRL,
  USBHOST_HID_USAGE_CYCLIC_CTRL,
  USBHOST_HID_USAGE_CYCLIC_TRIM,
  USBHOST_HID_USAGE_FLIGHT_YOKE,
  USBHOST_HID_USAGE_TRACK_CTRL,
  USBHOST_HID_USAGE_AILERON,
  USBHOST_HID_USAGE_AILERON_TRIM,
  USBHOST_HID_USAGE_ANTITORQUE_CTRL,
  USBHOST_HID_USAGE_AUTOPILOT_ENA,
  USBHOST_HID_USAGE_CHAFF_REL,
  USBHOST_HID_USAGE_COLLECTIVE_CTRL,
  USBHOST_HID_USAGE_DIVE_BRAKE,
  USBHOST_HID_USAGE_E_COUNTERMEASURE,
  USBHOST_HID_USAGE_ELEVATOR,
  USBHOST_HID_USAGE_ELEVATOR_TRIM,
  USBHOST_HID_USAGE_RUDDER,
  USBHOST_HID_USAGE_THROTTLE,
  USBHOST_HID_USAGE_FLIGHT_COMMU
  USBHOST_HID_USAGE_FLARE_REL,
  USBHOST_HID_USAGE_LANDING_GEAR,
  USBHOST_HID_USAGE_TOE_BRAKE,
  USBHOST_HID_USAGE_TRIGGER,
  USBHOST_HID_USAGE_WEAPON_ARM,
  USBHOST_HID_USAGE_WEAPON_SEL,
  USBHOST_HID_USAGE_WING_FLAPS,
  USBHOST_HID_USAGE_ACCELERATOR,
  USBHOST_HID_USAGE_BRAKE,
  USBHOST_HID_USAGE_CLUTCH,
  USBHOST_HID_USAGE_SHIFTER,
  USBHOST_HID_USAGE_STEERING,
  USBHOST_HID_USAGE_TURRET_DIR,
  USBHOST_HID_USAGE_BARREL_ELEVATION,
  USBHOST_HID_USAGE_DIVE_PLANE,
  USBHOST_HID_USAGE_BALLAST,
  USBHOST_HID_USAGE_BICYCLE_CRANK,
  USBHOST_HID_USAGE_HANDLE_BARS,
  USBHOST_HID_USAGE_FRONT_BRAKE,
  USBHOST_HID_USAGE_REAR_BRAKE,
  USBHOST_HID_USAGE_REAR_BRAKE,
*/
  USBHOST_HID_USAGE_MAX      /**< &nbsp; */
} usbhost_hid_usage_type;

/*=======================================================================
  Structures
=======================================================================*/

#define USBHOST_HID_CLIENT_CALLBACK_TYPE(HID_ClientCallBack) \
void HID_ClientCallback \
(\
	usbhost_dev_handle_type        dev_hndl,\
	usbhost_pdev_typ_type          pdev_type,\
	usbhost_hid_event_type *	   event\
);



/** Used as the data I/O parameter for the usbhost_ioctl_hid_read_sampled()
HID IOCTL command and as the event structure used to send usage events to a
client via the immediate client notification callback.
*/
typedef struct usbhost_hid_event_s
{
  usbhost_hid_usage_type           usage;
  /**< Indicates which specific usage the event is for.\ For example:
  - USBHOST_HID_USAGE_BUTTON
  - USBHOST_HID_USAGE_DIAL
  - USBHOST_HID_USAGE_WHEEL
  - Etc.
  It is the responsibility of the client application to apply the appropriate
  context for each usage as per application requirements. */

  uint32                           seq_num;
  /**< Sequence number for the last update.\ Only applies to sampled data events. */

  int32                            data_val;
  /**< Usage data value.

  For example, if the usage is USBHOST_HID_USAGE_BUTTON, this field will
  indicate the specific button pressed (e.g., 2). */

  int32                            data_min;
  /**< Minimum data value. */

  int32                            data_max;
  /**< Maximum data value. */

  uint32                           data_unit;
  /**< Indicates the usage units.\ Not defined at this time */

  uint32                           data_unitExpo;
  /**< Unit exponent. */

  uint32                           data_flag;
  /**< Data attribute flags that define the usage type. Used by the client
       application as needed.\ Consult [U1], [U2] for additional information.\ 
	   This is a bitmask field with the following values:

  - USBHOST_HID_DATATYPE_DATA
  - USBHOST_HID_DATATYPE_CONSTANT
  - USBHOST_HID_DATATYPE_ARRAY
  - USBHOST_HID_DATATYPE_VARIABLE
  - USBHOST_HID_DATATYPE_ABSOLUTE
  - USBHOST_HID_DATATYPE_RELATIVE
  - USBHOST_HID_DATATYPE_NOWRAP
  - USBHOST_HID_DATATYPE_WRAP
  - USBHOST_HID_DATATYPE_LINEAR
  - USBHOST_HID_DATATYPE_NONLINEAR
  - USBHOST_HID_DATATYPE_PREFERED
  - USBHOST_HID_DATATYPE_NONPREFERED
  - USBHOST_HID_DATATYPE_NONULL
  - USBHOST_HID_DATATYPE_NULL
  - USBHOST_HID_DATATYPE_BITFIELD
  - USBHOST_HID_DATATYPE_BUFFERED */

  uint32                           data_modifier;
  /**< Data-specific modifier, if applicable.\ This is a bit-masekd field with
       the following values (when set (1) value indicates modifier is in use):

  - USBHOST_HID_CAPS_LOCK_ON     -- 0x0001, Caps lock is ON.
  - USBHOST_HID_SCROLL_LOCK_ON   -- 0x0002, Scroll lock is ON.
  - USBHOST_HID_NUM_LOCK_ON      -- 0x0004, Num lock is ON.
  - USBHOST_HID_SHIFT_KEY_ACTIVE -- 0x0008, SHIFT key is active.
  - USBHOST_HID_CTRL_KEY_ACTIVE  -- 0x0010, CTRL key is active.
  - USBHOST_HID_ALT_KEY_ACTIVE   -- 0x0020, ALT key is active.
  - USBHOST_HID_GUI_KEY_ACTIVE   -- 0x0040, GUI key is active. */

  uint32                           data_mkbrk;
  /**< Indicates the make or break status if applicable.\ For example, when a
       a mouse button is held down, 'make' is enabled.\ When it is released,
       'break' is enabled.\ 'Break' is always the default status, and it is up
	   to the client application to maintain proper state control. */

  qword                            timestamp;
  /**< Timestamp of the event at the point the event is passed to the
       client. */

  uint32                           data_report_id;
  /**< Device data report identifier. */
} usbhost_hid_event_type;


/** Data I/O parameter for the usbhost_ioctl_hid_get_attrib() command.

This command allows a client to retrieve HID device-specific attributes, such
as the maximum number of supported devices, mininum/maximum x, y, and z
positions, etc.
*/
typedef struct usbhost_hid_attrib_s
{
  uint16                           max_buttons;
    /**< Maximum number of supported buttons. */
  int32                            min_x;
    /**< Minimum X position value. */
  int32                            max_x;
    /**< Maximum X position value. */
  int32                            min_y;
    /**< Minimum Y position value. */
  int32                            max_y;
    /**< Maximum Y position value. */
  int32                            min_z;
    /**< Minimum Z position value. */
  int32                            max_z;
    /**< Maximum Z position value. */
} usbhost_hid_attrib_type;

/** Data I/O parameter for the usbhost_ioctl_hid_set_configuration() and
usbhost_ioctl_hid_get_configuration() HID IOCTL commands.
*/
typedef struct usbhost_ioctl_hid_cfg_s
{
  /** For SET_CONFIGURATION, the client must populate client_notify_cb with
  an appropriate callback function.\ For GET_CONFIGURATION, the HID driver
  returns the current callback function via client_notify_cb.
  */
  void (* client_notify_cb) (
      usbhost_dev_handle_type   dev_hndl,
	  usbhost_pdev_type         pdev_type,
	  usbhost_hid_event_type *  event
       );

} usbhost_ioctl_hid_cfg_type;

/** HID channel types.
*/
typedef enum 
{
  HID_CONTROL,
    /**< -- Control channel. */
  HID_INTERRUPT
    /**< -- Interrupt channel. */
} usbhost_hid_channel_type;

/** Data I/O parameter for the usbhost_ioctl_hid_set_report() HID IOCTL
commands.
*/
typedef struct usbhost_ioctl_hid_set_rep_s
{
  usbhost_hid_channel_type      channel;
    /**< HID channel type, whether control or interrupt. */
  uint16                        conn_id;
    /**< Connection handle of Bluetooth HID connection. */
  uint8                         report_type;
    /**< Report type. */
  uint8                         report_id;
    /**< Report ID. */
  uint8*                        data;
    /**< Pointer to report data. */
  uint16                        size;
    /**< Pointer to report the length of the data. */
} usbhost_ioctl_hid_set_rep_type;

/** Data I/O parameter for the usbhost_ioctl_hid_get_report() HID IOCTL
commands.
*/
typedef struct usbhost_ioctl_hid_get_rep_s
{
  usbhost_hid_channel_type      channel;
    /**< HID channel type -- control or interrupt. */
  uint16                        conn_id;
    /**< Connection handle of Bluetooth&reg; &nbsp; HID connection. */
  uint8                         report_type;
    /**< Report type. */
  uint8                         report_id;
    /**< Report ID. */
  uint16                        max_bytes;
    /**< Requested maximum bytes. */
} usbhost_ioctl_hid_get_rep_type;

/** @} */ /* end_group hsusb_qc_api */

#endif /* USBHOST_API_H */
