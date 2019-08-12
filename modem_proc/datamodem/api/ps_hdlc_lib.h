#ifndef PS_HDLC_LIB_H
#define PS_HDLC_LIB_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          P S _ H D L C _ L I B . H


GENERAL DESCRIPTION
  The implementation of the HDLC library.

EXTERNALIZED FUNCTIONS

  hdlc_init_config()
    Initializes the config data structure with default values

  hdlc_alloc_instance()
    Allocates an instance for HDLC framing/unframing

  hdlc_free_instance()
    Resets and frees up the memory for the specified HDLC instance

  hdlc_reset()
    Reset the state of the HDLC instance.

  hdlc_errno()
    Returns the errno of the last operation on the specified HDLC instance

  hdlc_unframe()
    Unframes the passed DSM chain for the specified HDLC instance

  hdlc_frame()
    Frames the passed DSM chain for the specified HDLC instance

  hdlc_ioctl()
    Performs the specified operation on the specified HDLC instance

INITIALIZATION AND SEQUENCING REQUIREMENTS
    All functions in this library except hdlc_init_config() and
    hdlc_alloc_instance() need to take an instance handle -
    this is allocated in the hdlc_alloc_instance() call.
    An error will be returned if the handle is invalid.

Copyright (c) 2006-2011 Qualcomm Technologies Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/ps_hdlc_lib.h#1 $
  $DateTime: 2016/12/13 08:00:03 $

===========================================================================*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                                INCLUDE FILES

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "comdef.h"
#include "queue.h"
#include "ps_ppp_ext.h"
#include "dsm.h"


/*---------------------------------------------------------------------------
  HDLC errno enum
---------------------------------------------------------------------------*/
typedef enum hdlc_errno_enum_type
{
  HDLC_OK              = 0,
  HDLC_FAILURE         = 1,   /* An HDLC failure occured                   */
  HDLC_BAD_HANDLE      = 2,   /* Instance handle is invalid.               */
  HDLC_NO_MEMORY       = 3,   /* Unable to allocate memory for unframing   */
  HDLC_BAD_CRC         = 4,   /* Bad CRC - getting data depends on config  */
  HDLC_BAD_PACKET      = 5,   /* < 2 bytes of HDLC payload, i.e. need CRC  */
  HDLC_BAD_PARAM       = 6,   /* framed, or unframed invalid               */
  HDLC_ZERO_LEN_PKT    = 7,   /* Packet unframed ok, but is empty          */
  HDLC_PKT_TOO_LONG    = 8,   /* length > config max_len, data is tossed   */
  HDLC_MORE_PKTS_AVAIL = 9,   /* The unframer has more packets to return   */
  HDLC_INVALID_OP      = 10   /* Requested operation is invalid            */
} hdlc_errno_enum_type;

/*---------------------------------------------------------------------------
  HDLC normal and panic result type
---------------------------------------------------------------------------*/

typedef struct hdlci_pp_item_s
{
  q_link_type   link;
  void          *user_data_ptr;
} hdlci_pp_item_s_type;

/*===========================================================================
FUNCTION HDLC_PP_PROCESS_CB_TYPE

DESCRIPTION
  Function callback called by the normal or panic process callbacks
  previously registered by the PP with the DM.

PARAMETERS

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  The ACCM or HDLC mode for the specified instance may be changed.
===========================================================================*/

typedef void (*hdlc_pp_process_cb_type) 
(
  hdlci_pp_item_s_type *user_data
);

/*===========================================================================
TYPEDEF HDLC_CB_PTR_TYPE

DESCRIPTION
  Type of function called when framing is complete.
  This function pointer is present in the HDLC instance, and gets set 
  during the function call to hdlc_alloc_instance(). When a framing
  operation on a packet is complete, this function is called which 
  transmits data to the lower layers.

PARAMETERS

DEPENDENCIES
  None

RETURN VALUE
  None
SIDE EFFECTS
  The ACCM or HDLC mode for the specified instance may be changed.
===========================================================================*/
typedef void (*hdlc_cb_ptr_type)
(
  ppp_dev_enum_type device, /*To hold information such as device associated*/
  dsm_item_type  ** item_ptr,  /* addr of ptr to destination pkt   */
  void           *user_data_ptr /*To hold information such as meta_info_ptr etc*/
);

/*---------------------------------------------------------------------------
TYPEDEF HDLC_CONFIG_TYPE

DESCRIPTION
  The type that contains the HDLC configuration parameters that can be passed
  in for an alloc() call.
  IMPORTANT NOTE:
  Configuration for max_len and strip_ff03_if_present is not supported
  currently.
---------------------------------------------------------------------------*/
typedef struct hdlc_config_type
{
  uint16   max_len;
  boolean  return_pkt_on_bad_crc;
  boolean  strip_ff03_if_present;
  uint32   accm;
  ppp_dev_enum_type device;
  hdlc_cb_ptr_type  framer_complete_cb;
  hdlc_cb_ptr_type  deframer_complete_cb;
  hdlc_pp_process_cb_type frm_cb;
  hdlc_pp_process_cb_type dfrm_cb;
} hdlc_config_type;

/*===========================================================================
FUNCTION HDLC_INIT_CONFIG()

DESCRIPTION
  This function initializes the config data structure that was passed in to
  the default values for the various fields.

PARAMETERS
  config: pointer to the configuration data structure to intialize.
  Must not be passed in as NULL.

RETURN VALUE
  None

DEPENDENCIES
  IMPORTANT NOTE:
  Configuration for max_len and strip_ff03_if_present in hdlc_config_type
  is not supported currently.

SIDE EFFECTS
  None
===========================================================================*/
void hdlc_init_config
(
  hdlc_config_type * config
);

/*===========================================================================
FUNCTION HDLC_ALLOC_INSTANCE()

DESCRIPTION
  This function will allocate an instance of the HDLC engine, set the
  instance variables per the config passed in, and initialize the function
  pointers to do use either hardware or software mode depending on
  featurization.

PARAMETERS
  config: pointer to the configuration data structure.  Must not be passed
  in as NULL.

RETURN VALUE
  NULL: on failure
  pointer to handle instance.

DEPENDENCIES
  IMPORTANT NOTE:
  Configuration for max_len and strip_ff03_if_present in hdlc_config_type
  is not supported currently.

SIDE EFFECTS
  None
===========================================================================*/
void *hdlc_alloc_instance
(
  hdlc_config_type * config
);

/*===========================================================================
FUNCTION HDLC_FREE_INSTANCE()

DESCRIPTION
  This function will reset the instance passed in and free the memory.

PARAMETERS
  instance: a referance to an instance pointer - this will be set to NULL

RETURN VALUE
  0 on success
 -1 on failure (which will only happen if the handle is invalid).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int hdlc_free_instance
(
  void ** handle
);

/*===========================================================================
FUNCTION HDLC_ERRNO()

DESCRIPTION
  This function will return the errno of the last operation on the instance
  passed in.

PARAMETERS
  instance: the instance on which the errno is being checked.

RETURN VALUE
  0 if there is no error
  one of the values defined in the HDLC errno enum (look in the header file)
  Note: the following are currently not implemented:
  HDLC_PKT_TOO_LONG (there is currently no support for limiting packet
  lengths).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
hdlc_errno_enum_type hdlc_errno
(
  void * instance
);

/*===========================================================================
FUNCTION HDLC_ERRNO()

DESCRIPTION
  This function will return the errno of the last operation on the instance
  passed in.

PARAMETERS
  instance: the instance on which the errno is being checked.

RETURN VALUE
  0 if there is no error
  one of the values defined in the HDLC errno enum (look in the header file)
  Note: the following are currently not implemented:
  HDLC_PKT_TOO_LONG (there is currently no support for limiting packet
  lengths).

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
hdlc_errno_enum_type hdlc_errno
(
  void * instance
);

/*===========================================================================
FUNCTION HDLC_UNFRAME()

DESCRIPTION
  This function performs the HDLC unframing.  The function returning -1 does
  not always mean that there is no data being generated.  In the following
  cases it is possible to have unframed data returned:
    1. HDLC_NO_MEMORY: This errno is generated when it is not possible to
       copy unframed data to a dsm item when there are two packets in a
       single item.  In this case the fully unframed packet will be returned.
    2. HDLC_BAD_CRC: if the unframer has been configured to return packets in
      this case *unframed will contain data when this error occurs.
    3. HDLC_MORE_PKTS_AVAIL: If the unframer has more packets queued up
       internally for the framed chain passed in. The caller of the API is
       expected to call the function again to retrieve these packets.

   Clients of this API should invoke the API until *framed goes NULL AND
   the errno returned is HDLC_OK. This recommended semantic is shown with
   an example below.

   ==*==*==*==*==*==*==*==   BEGIN CODE SNIPPET   =*=*==*==*==*==*==*==*==*==

   int errno = HDLC_OK;
   int retval = 0;

   while((*framed != NULL) || (errno != HDLC_OK))
   {
     *unframed = NULL;
     retval = hdlc_unframe(instance, framed, unframed);
     errno = hdlc_errno(instance);
     if(retval < 0)
     {
       switch(errno)
       {
         case HDLC_MORE_PKTS_AVAIL:
           use_unframed_pkt();
           continue;

         case HDLC_BAD_HANDLE:
           dsm_free_packet(framed);
           dsm_free_packet(unframed);
           break;

         default:
           break;
       }

       if(*unframed == NULL)
       {
         continue;
       }
     }
     use_unframed_pkt(unframed);
   }

   ==*==*==*==*==*==*==*==*   END CODE SNIPPET   ==*=*==*==*==*==*==*==*==*==

PARAMETERS
  handle: the instance of the unframer to use (stores any unframed packets
    and the current state of unframing.
  framed: value return parameter
    in:  reference to the framed data that needs unframing
    out: NULL if there is no more data to unframe or pointing to the next
         block of data that needs unframing.
  unframed: will be set to point to an unframed packet when one is available.
    Must be NULL when called.
  user_data: Any additional user data to be passed on as a part of asynchronous
             processing by the protocol processor.

RETURN VALUE
  0 on success
 -1 on failure or if the unframer has more packets to return:
   - use hdlc_errno() for specifics of error.  The header file
                 has the definitions of the various errnos.

DEPENDENCIES
  None

SIDE EFFECTS
  on memory errors if the item being processed contains the start of a new
  packet, this packet will be lost.
===========================================================================*/
int hdlc_unframe
(
  void           * handle, 
  dsm_item_type ** framed, 
  dsm_item_type ** unframed,
  void           * user_data
);

#endif /* PS_HDLC_LIB_H */
