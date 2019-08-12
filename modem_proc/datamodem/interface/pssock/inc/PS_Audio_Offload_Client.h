#ifndef _PS_AUDIO_OFFLOAD_CLIENT_H
#define _PS_AUDIO_OFFLOAD_CLIENT_H
/*===========================================================================

                         PS _ AUDIO _ OFFLOAD _ CLIENT  . H

DESCRIPTION

  PS Audio offload client APIs interface for the clients like QMI.

FUNCTIONS

  ps_audio_offload_add_filter
  ps_audio_offload_delete_filter
  ps_audio_offload_get_statistics
  ps_audio_offload_get_max_filters

Copyright (c) 2004-2013 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header:

when        who    what, where, why
--------    ---    ----------------------------------------------------------
08/14/13    az     First revision of PS Audio offload client APIs interface
                   for clients like QMI.
===========================================================================*/

/*===========================================================================
                                Definitions
===========================================================================*/


/*===========================================================================
                                Data Structures
===========================================================================*/

/* Structure use to get and return packet statistics */
typedef struct
{
 	uint64  bytes_tx;
	uint64  bytes_rx;
	uint32  pkts_tx;
  uint32  pkts_rx;
} ps_audio_offload_client_stats_type;

/*===========================================================================
                              EXTERNAL FUNCTIONS
===========================================================================*/
void ps_audio_offload_client_powerup
(
  void
);

/*===========================================================================
  FUNCTION PS_AUDIO_OFFLOAD_GET_MAX_FILTERS()

  DESCRIPTION
    Return the max filters supported by offload client

    Note: This is a synchronous call for the calling client

  PARAMETERS
    None

  RETURN VALUE
    Max no of filters supported

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
uint8 ps_audio_offload_client_get_max_fltr
(
   void
);

/*===========================================================================
  FUNCTION PS_AUDIO_OFFLOAD_ADD_FILTER()

  DESCRIPTION
    Clients like QMI layer, call this function to install an audio offload filter.
    It opens a socket and an SIO port to attach to the filter. After successfull
    addition of the filter, all the targetted trafic gets routed to OSA/ADSP
    module instead of its original path to app processor.
    Note: This is a synchronous call for the calling client

  PARAMETERS
    fltr_mask   : Filter mask passed by the client layer. Contains one bit as
                  set
    iface_id    : Interface ID on which this filter has to be installed
    fltr_spec   : IP filter parameters
    dss_errno   : Contains error code in case if the call is not successfull

  RETURN VALUE
    filter ID or failture : If positive no, means call is successfull and
                            filter ID is returned.

    On error, return DSS_ERROR and places the error condition value in
    *dss_errno.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int32 ps_audio_offload_client_add_fltr
(
  uint8                  fltr_mask,
  ps_iface_id_type       iface_id,
  ip_filter_spec_type  * fltr_spec,
  int16                * dss_errno
);

/*===========================================================================
  FUNCTION PS_AUDIO_OFFLOAD_DELETE_FILTER()

  DESCRIPTION
    Function delets the filter given the filter handle. All the associated
    resources are deallocated
 
    Note: This is a synchronous call for the calling client

  PARAMETERS
    fltr_mask   : Filter handle, initially returned to the caller on
                  ps_audio_offload_client_add_fltr() call
    dss_errno   : Contains error code in case if the call is not successfull

  RETURN VALUE
    Success or error code

    On error, return DSS_ERROR and places the error condition value in
    *dss_errno.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int32 ps_audio_offload_client_delete_fltr
(
  int32     fltr_mask,
  int16   * dss_errno
);

/*===========================================================================
  FUNCTION PS_AUDIO_OFFLOAD_GET_STATISTICS()

  DESCRIPTION
    Retrieve and return the data traffic statistics associated with the given
    filter

    Note: This is a synchronous call for the calling client

  PARAMETERS
    fltr_mask         : Filter mask for which traffic statistics
                        are required
    offload_stats_ptr : User space pointer where statistics have to be returned
    dss_errno         : Contains error code in case if the call is not
                        successfull 
  RETURN VALUE
    status : Success or error code

    On error, return DSS_ERROR and places the error condition value in
    *dss_errno.

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
int32 ps_audio_offload_client_get_stats
(
  int32                                 fltr_mask,
  ps_audio_offload_client_stats_type  * offload_stats_ptr,
  int16                               * dss_errno
);

#endif
