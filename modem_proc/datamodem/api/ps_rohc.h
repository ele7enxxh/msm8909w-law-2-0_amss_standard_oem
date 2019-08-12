#ifndef _PS_ROHC_H
#define _PS_ROHC_H
/*===========================================================================
                           P S _ R O H C . H

DESCRIPTION
  Header file for the RObust Header Compression (ROHC) protocol.
 (RFC 3095)

Copyright (c) 2005-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/datamodem/api/ps_rohc.h#1 $
  $Author: mplcsds1 $
  $DateTime: 2016/12/13 08:00:03 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
09/17/14    ds     Added support for UE defense mechanism in ROHC when conse-
                   cutive SNACKs for DL and UL.
05/27/14    ds     Added New ioctl definition to add support for moving the 
                   compressor state to FO instead of IR after context reinit.
08/26/11    cp     Added the Multithread Support to Sink Feedback in 
                   Compressor User Task.
05/07/09    pp     CMI Phase-4: SU Level API Effort.
10/24/08    pp     Modified rohc_get_ctrl_blk prototype to fix compiler 
                   warnings.
11/28/07    pp     Lint medium/low - fixes.
11/27/07    pp     Included IP profile interface definitions.
11/26/07    pp     Fixed RVCT 2.2 Compiler warning.
10/29/07    pp     Lint fixes.
10/12/07    sp     Added the API rohc_get_supported_profiles().
07/26/07    sp     Added support for ROHC Dynamic Memory Organisation Feature.
11/02/06    ifk    Merged Effnet 2.2.2-3 release.
08/17/06    ifk    Added IOCTL to switch decompressor mode.
08/31/06    mjp    Added IPHC and ROHC Logging
06/12/06    ifk    Added ESP profile to profile enumeration.
04/28/06    ifk    Added p_sn and const_ip_id_handling parameters.  Added
                   IOCTLS to change channel parameters.  Added enum for
                   mode.
01/26/06    ifk    Added ioctl to set fb_cookie in the decompressor.
01/19/06    ifk    Added ioctl to set feedback_for in the decompressor.
07/06/05    ifk    Created module.
===========================================================================*/

/*===========================================================================

                              DATA DECLARATIONS

===========================================================================*/
#include "comdef.h"
#include "IxErrno.h"
#include "dsm.h"
#include "ps_in.h"
#include "ps_logging_defs.h"

/*---------------------------------------------------------------------------
  Compile time defined value of the number of entries in the allowed packet
  size array supplied as part of ROHC compressor channel configuration
---------------------------------------------------------------------------*/
#define ROHC_MAX_NUM_ALLOWED_PKT_SIZES    8

#define ROHC_MAX_INSTANCE_COUNT           16

/*---------------------------------------------------------------------------
  Enums for ROHC profiles and mask of ROHC profiles
---------------------------------------------------------------------------*/
typedef enum rohc_profiles_e_type
{
  ROHC_PROFILE_UNCOMPRESSED  = 0x0000,
  ROHC_PROFILE_RTP            = 0x0001,
  ROHC_PROFILE_UDP            = 0x0002,
  ROHC_PROFILE_ESP            = 0x0003,
  ROHC_PROFILE_IP             = 0x0004,
  ROHC_PROFILE_MAX
} rohc_profiles_e_type;

typedef enum rohc_profiles_mask_e_type
{
  ROHC_PROFILE_NONE_MASK           = 0, /* Value indicating no profile     */
  ROHC_PROFILE_UNCOMPRESSED_MASK   = (1 << ROHC_PROFILE_UNCOMPRESSED),
  ROHC_PROFILE_RTP_MASK            = (1 << ROHC_PROFILE_RTP),
  ROHC_PROFILE_UDP_MASK            = (1 << ROHC_PROFILE_UDP),
  ROHC_PROFILE_ESP_MASK            = (1 << ROHC_PROFILE_ESP),
  ROHC_PROFILE_IP_MASK             = (1 << ROHC_PROFILE_IP),
  ROHC_PROFILE_MAX_MASK            = 0x40000000
} rohc_profiles_mask_e_type;

/*---------------------------------------------------------------------------
  Enum for ROHC mode supplied to decompressor as preferred mode for it to
  operate in
---------------------------------------------------------------------------*/
typedef enum rohc_mode_e_type {
    ROHC_U_MODE = 0x00,
    ROHC_O_MODE = 0x01,
    ROHC_B_MODE = ROHC_O_MODE,
    ROHC_R_MODE = 0x02
} rohc_mode_e_type;

/*---------------------------------------------------------------------------
  Enum defining policy for k1 out of n1 and k2 out of n2 parameters
---------------------------------------------------------------------------*/
typedef enum rohc_k1n1_k2n2_policy_e_type
{
  ROHC_POLICY_1_OUT_1   = 0,
  ROHC_POLICY_3_OUT_5   = 1,
  ROHC_POLICY_2_OUT_3   = 2,
  ROHC_POLICY_2_OUT_2   = 3,
  ROHC_POLICY_3_OUT_3   = 4,
  ROHC_POLICY_MAX
} rohc_k1n1_k2n2_policy_e_type;

typedef void *rohc_handle_type;

/*---------------------------------------------------------------------------
  Function pointer type for handling outbound ROHC feedback packets.  This
  function is registered with the decompressor and is called when there is
  feedback to be sent out.  The fb_pkt argument contains the DSM item chain
  with the feedback in it and is expected to be freed by the callee.  The
  fb_cookie is the cookie registered along with the feedback handling
  function with the decompressor
---------------------------------------------------------------------------*/
typedef void (*rohc_outbound_fb_f_type)
(
  dsm_item_type **fb_pkt,
  void           *fb_cookie
);

/*---------------------------------------------------------------------------
  Function pointer type for handling inbound ROHC feedback packets.  This
  function is registered with the decompressor and is called when there is
  feedback to be sinked to respective compressor. 
  This is MUST for clients who runs Compressor, Decompressor in two different
  contexts [in a multi threaded plaforms]
  The fb_pkt argument contains the DSM item chain with the feedback in it 
  and is expected to be passed to the compressor in Compressor context (after
  this handler is called). 
  The inbound_fb_cookie is the cookie registered along with the inbound 
  feedback handling  function with the decompressor.
  fb_cid - CID received in the Feedback
---------------------------------------------------------------------------*/
typedef void (*rohc_inbound_fb_f_type)
(
  dsm_item_type **fb_pkt,
  void           *fb_cookie,
  uint16          fb_cid
);

/*---------------------------------------------------------------------------
  TYPEDEF ROHC_CLI_EVENT_ENUM

  enum to specify the direction in which SNACK is either being received or 
  sent in case same callback is registered by PDCP.
---------------------------------------------------------------------------*/
typedef enum
{
  PS_ROHC_CLI_EVENT_MIN = 0,
  PS_ROHC_DOWNLINK_SNACK_EV = 1,
  PS_ROHC_UPLINK_SNACK_EV = 2,
  PS_ROHC_DOWNLINK_UNKNOWN_PROFILE_EV = 3,
  PS_ROHC_CLI_EVENT_MAX = 4  
}rohc_cli_event_enum;

/*---------------------------------------------------------------------------
  TYPEDEF ROHC_CLI_EVENT_INFO_S_TYPE

  This argument is neccessary to be filled with sufficient information so that
  Clients can handle/recover accordingly.
---------------------------------------------------------------------------*/
typedef struct rohc_cli_event_info_s_type
{
  rohc_handle_type      handle;
  rohc_cli_event_enum   event;
  void                 *cb_data;
}rohc_cli_event_info_s_type;

/*---------------------------------------------------------------------------
  Function pointer type for handling consecutive SNACKs recieved in both 
  downlink and uplink direction.  
  This is a optional callback mechanism for ROHC clients who runs Compressor,
  Decompressor and wants to enable optimal handling by ROHC to inform its 
  clients if there is any SNACK being generated in UL/DL direction so that 
  client can take appropriate action.
  the rohc_snack_event_type argument is filled with required information 
  used by ROHC clients to recover.
---------------------------------------------------------------------------*/

typedef void (*rohc_cli_event_cb_fn_type)
(
  rohc_cli_event_info_s_type* info_ptr
);

/*---------------------------------------------------------------------------
  ROHC compressor channel parameters

  Description of parameters:
   max_cid               Maximum context id to be used for compressor channel
                         (RFC 3095, Section: 5.1.1)

   use_large_cid         Boolean indicating if large CID representation is to
                         be used.  This parameter is used to further
                         constrain max_cid. (RFC 3095, Section: 5.1.1) 

   mrru                  Maximum reconstructed reception unit in segmenting
                         ROHC packets.  A value of 0 disables segmentation.
                         (RFC 3095, Section: 5.1.1)

   profiles              Set of profiles, given as a mask, to be supported on
                         this channel. (RFC 3095, Section: 5.1.1)

   max_jitter_cd         Maximum jitter value in packet arrival times on the
                         channel between the compressor and de-compressor.
                         Used in RTP timestamp compression.  Expressed in
                         msecs.  (RFC 3095, Section: 4.5.4)

   num_allowed_pkt_sizes Number of usable packet sizes supplied in the
                         allowed_pkt_sizes parameter.  A value of 0 disables
                         use of allowed packet sizes array. (RFC 3095,
                         Section 6.3.1)

   allowed_pkt_sizes     Array of packet sizes which the link layer can
                         support. (RFC 3095, Section 6.3.1)

   skip_cid              Parameter used for supporting PDCP's CID encoded in
                         PID mode.  A value of TRUE indicates that the ROHC
                         compressor should not include CID in the ROHC
                         header but return it seperately to be externally
                         encoded by the link layer.  A value of FALSE
                         disables this feature.
---------------------------------------------------------------------------*/
typedef struct rohc_params_comp_channel_s_type
{
  uint16  max_cid;         /* Max value of context identifier              */
  boolean use_large_cids;  /* Use large cid representation if TRUE         */
  uint16  mrru;            /* Max reconstructed reception unit             */
  rohc_profiles_mask_e_type
          profiles;        /* Mask of requested profiles                   */
  uint32  max_jitter_cd;   /* Max Jitter between compressor & decompressor */
  uint32  num_allowed_pkt_sizes;
                           /* Number of allowed packet sizes               */
  uint16  allowed_pkt_sizes[ROHC_MAX_NUM_ALLOWED_PKT_SIZES];
                           /* Array of allowed link layer packet sizes     */
  boolean skip_cid;        /* TRUE if CID is not included in ROHC packet   */
} rohc_params_comp_channel_s_type;

/*---------------------------------------------------------------------------
  ROHC compressor flow parameters

  Description of Parameters:
   ww_num_update         Number of reference values outstanding when using
                         window based LSB encoding.  It is also number of
                         updating packets to send for a change in context.

   num_update_IR         Number of IR packets the compressor should send in
                         the IR state before it can optimistically transition
                         to FO state.  Should be <= ww_num_update

   num_nonupdates        Ratio of packets updating context to non updating
                         packets when in R mode

   SO_IRt                Timeout in packets when operating is SO state in U
                         mode until transition to IR state is made.

   SO_FOt                Timeout in packets when operating in SO state in U
                         mode until transition is made to FO state

   timer_based_comp      Enable timer based compression of RTP timestamp if
                         TRUE

   profile_hint          Suggested profile to use for the flow.  Even though
                         the variable is a mask type only one profile
                         is expected to be set.

   f_max_time            Maximum time in millisecond spent without sending
                         any IR packets after which compressor shall send
                         next packet as an IR packet.  A value of 0x7fffffff
                         disables sending of periodic IR refreshes.

   exp_backoff           Controls slow start algorithm in U mode. An IR
                         packet is sent every:
                           N = exp_backoff ^ n  (n= 1, 2 ...)
                         packets until N reaches SO_IRt.

   time_stride           Specifies the time interval in milliseconds
                         corresponding to TS_STRIDE when using timer based
                         compression of the RTP timestamp.  A value of 0 is
                         used to signal that the compressor should calculate
                         time_stride itself.

   ts_stride_recalc      Boolean controlling recalculation of TS_STRIDE by the
                         compressor.  This should be set to FALSE for flows
                         where there are silence periods.

   loss_option           Enables handling of LOSS option sent by peer
                         decompressor if TRUE.

   p_sn                  For RTP profile, the RFC 3095 specified value of
                         P_SN is 1.  To support packet reordering at the 
                         link layer a value greater then 1 should be used.

   const_ip_id_handling  If set to TRUE enables the handling of constant
                         IPv4 identifaction field by replacing it with the
                         packet sequence number (RTP SN or UDP SN).
---------------------------------------------------------------------------*/
typedef struct rohc_params_comp_flow_s_type
{
  uint32  ww_num_update;     /* Num references in window-LSB encoding      */
                             /* Also number of updating packets to send    */
  uint32  num_update_IR;     /* Num IR packets to send when initializing   */
  uint32  num_nonupdates;    /* R mode updating to non-updating pkt ratio  */
  uint32  SO_IRt;            /* Packets between SO(U mode) to IR transition*/
  uint32  SO_FOt;            /* Packets between SO(U mode) to FO transition*/
  boolean timer_based_comp;  /* Do timer based compression of RTP timestap */
  rohc_profiles_mask_e_type
          profile_hint;      /* Profile suggested for flow                 */
  uint32  f_max_time;        /* Max time (ms) between IR packets in U mode */
  uint32  exp_backoff;       /* controls slow start in U mode              */
  uint32  time_stride;       /* interval between consecutive RTP SNs (msec)*/
  boolean ts_stride_recalc;  /* Enables recalculation of TS_STRIDE         */
  boolean loss_option;       /* Enables handling of loss option            */
  uint8   p_sn;              /* P value for SN encoding                    */
  boolean const_ip_id_handling; /* Enable handling of constant IPv4 ID     */
} rohc_params_comp_flow_s_type;

/*---------------------------------------------------------------------------
  ROHC de-compressor channel parameters

  Description of parameters:
   max_cid        Maximum context id to be used for de-compressor channel
                  (RFC 3095, Section: 5.1.1)

   use_large_cid  Boolean indicating if large CID representation is to
                  be used.  This parameter is used to further constrain
                  max_cid. (RFC 3095, Section: 5.1.1) 

   mrru           Maximum reconstructed reception unit in reassembling
                  ROHC packets.  A value of 0 disables segment reassembly
                  (RFC 3095, Section: 5.1.1)

   profiles       Set of profiles, given as a mask, to be supported on
                  this channel.  (RFC 3095, Section: 5.1.1)

   k1_out_n1      When operating in full context state, the de-compressor
                  declares the context to be invalid if out of n1 packets
                  received it fails to de-compress k1 packets.
                  (RFC 3095, Section: 5.3.2.2.3)

   k2_out_n2      When operating in static context state, the de-compressor
                  declares the context to be invalid if out of n2 packets
                  received it fails to de-compress k2 packets
                  (RFC 3095, Section: 5.3.2.2.3)

   skip_cid       Parameter used for supporting PDCP's CID encoded in
                  PID mode.  A value of TRUE indicates that the CID is not
                  part of the ROHC header but is passed seperately into
                  the decompressor.  A value of FALSE disables this feature.

   feedback_for   Indicates a ROHC instance to which all incoming feedback
                  received by the decompressor should be passed.  If set to
                  NULL then the associated compressor, if one, in the ROHC
                  instance is given the feedback.

   fb_func        Function registered by ROHC client to be called when
                  outbound feedback generated by the decompressor should
                  be given.  If set to NULL then the feedback is returned
                  from rohc_decompress()

   fb_cookie      Cookie registered by the client to be passed back to
                  fb_func along with any generated feedback.
   
   ll_sn_lower    Lower range of the link layer sequence number space.  Used
                  in enhanced local repair.  A value of 0 for both 
                  ll_sn_lower and ll_sn_upper disables enhanced local repair

   ll_sn_upper    Upper range of the link layer sequence number space.  Used
                  in enhanced local repair.  A value of 0 for both 
                  ll_sn_lower and ll_sn_upper disables enhanced local repair

---------------------------------------------------------------------------*/
typedef struct rohc_params_decomp_channel_s_type
{
  uint16 max_cid;          /* Max value of context identifier              */
  boolean use_large_cids;  /* Use large cid representation if TRUE         */
  uint16 mrru;             /* Max reconstructed reception unit             */
  rohc_profiles_mask_e_type
         profiles;         /* Mask of requested profiles                   */
  rohc_k1n1_k2n2_policy_e_type k1_out_n1;
                           /* k1 errors out of n1 pkts in full context     */
                           /* state before invalid context declared        */
  rohc_k1n1_k2n2_policy_e_type k2_out_n2;
                           /* k2 errors out of n2 pkts in static context   */
                           /* state before invalid context declared        */
  boolean skip_cid;        /* TRUE if CID is not to be included in ROHC pkt*/
  rohc_handle_type feedback_for; 
                           /* Destination for incoming feedback            */
  rohc_inbound_fb_f_type inbound_fb_func;
                           /* Hdlr function for inbound feedback 
                              (multi thread support) */
  void *inbound_fb_cookie; /* Cookie to be passed to inbound_fb_func 
                              along with fb */
  rohc_outbound_fb_f_type fb_func; 
                           /* Hdlr function for outbound feedback          */
  void *fb_cookie;         /* Cookie to be passed to fb_func along with fb */
  int16 ll_sn_lower;       /* Lower value forming link layer sequence num  */
                           /* range.  0 if not available                   */
  int16 ll_sn_upper;       /* Lower value forming link layer sequence num  */
                           /* range.  0 if not available                   */
} rohc_params_decomp_channel_s_type;

/*---------------------------------------------------------------------------
  ROHC de-compressor flow parameters

  Description of parameters:
   preferred_mode        Mode the de-compressor should operated in
                         (RFC 3095, Section: 6.3.2)

   optional_ack_enabled  Boolean indicating if the optional acknowledgement
                         algorithm in the U mode is to be enabled or not.
                         (RFC 3095, Section: 5.3.2.3)

   rd_depth              Indicates the number of packets to queue in
                         the reverse decompression depth algorithm.  A
                         value of 0 disables reverse decompression depth
                         algorithm.  (RFC 3095, Section: 6.3.2)

   timer_based_decomp    Boolean indicating if timer based decompression
                         of RTP timestamps is to be enabled.

   loss_option_thres     Number of packets lost before the decompressor
                         sends a LOSS option to the peer compressor.  A 
                         value of 0 disables the sending of LOSS option.

   p_sn                  For RTP profile, the RFC 3095 specified value of
                         P_SN is 1.  To support packet reordering at the 
                         link layer a value greater then 1 should be used.
---------------------------------------------------------------------------*/
typedef struct rohc_params_decomp_flow_s_type
{
  rohc_mode_e_type preferred_mode;/* Mode de-compressor should operate in  */
  boolean optional_ack_enabled; /* Enable optional ack algorithm in U mode */
  uint32  rd_depth;             /* Packet depth for reverse decompression  */
  boolean timer_based_decomp;   /* Do timer based decomp of RTP timestamp  */
  uint8   loss_option_thres;    /* Packets lost before LOSS option is sent */
  uint8   p_sn;                 /* P value for SN encoding                 */
} rohc_params_decomp_flow_s_type;

/*---------------------------------------------------------------------------
  ROHC ioctls

  Description of supported ROHC ioctls:
  CONTEXT_REINITIALIZATION        Handles the context reinitialization
                                  signal.  Causes all active contexts for
                                  the compressor belonging to the ROHC
                                  handle to be moved back to IR state.
                                  (RFC 3095, Section: 6.3.1)
                                  Doesn't need a value passed in ioctl_data.

  SET_FEEDBACK_FOR                Sets the feedback_for parameter in the
                                  decompressor to the passed ROHC instance.
                                  Any feedback received by the ROHC
                                  decompressor is passed to the compressor
                                  associated with the feedback_for instance.

  SET_FB_COOKIE                   Sets the decompressor fb_cookie to the
                                  passed value.  The fb_cookie is returned
                                  in the ROHC client registered feedback
                                  function along with decompressor generated
                                  feedback.

  REINIT_DECOMP_CHANNEL           Reinitializes decompressor with the passed
                                  decompressor channel parameters.
                                  
  REINIT_COMP_CHANNEL             Reinitializes compressor with the passed
                                  compressor channel parameters

  SET_DEFAULT_COMP_FLOW_PARAMS    Sets the passed compressor flow
                                  parameters as the default value for all
                                  compressor flows.  The default flow
                                  parameters are used for a new flow unless
                                  overridden by flow parameters passed in
                                  rohc_compress() 

  SET_DEFAULT_DECOMP_FLOW_PARAMS  Sets the passed decompressor flow
                                  parameters as the default value for all
                                  decompressor flows.  The default flow
                                  parameters are used for a new flow unless
                                  overridden by flow parameters passed in
                                  rohc_decompress() 

  SWITCH_DCOMP_MODE               Switches the decompressor mode to the
                                  passed value of mode.  Returns an error
                                  if failed to switch mode.  Also changes
                                  default mode in the cached default 
                                  decompressor flow params structure.

  SET_INBOUND_FEEDBACK_FUNC  Sets the inbound_fb_func parameter in the
                             decompressor to the passed ROHC instance.
                             Any feedback received by the ROHC
                             decompressor will be passed to this function
                             for further processing.

  SET_INBOUND_FEEDBACK_COOKIE  Sets the decompressor inbound_fb_cookie to the
                               passed value.  The fb_cookie is returned
                               in the ROHC client registered inbound_fb_func
                               function along with piggybacked feedback packet.
---------------------------------------------------------------------------*/
typedef enum rohc_ioctl_e_type
{
  ROHC_IOCTL_CONTEXT_REINITIALIZATION       =  1,
  ROHC_IOCTL_SET_FEEDBACK_FOR               =  2,
  ROHC_IOCTL_SET_FB_COOKIE                  =  3,
  ROHC_IOCTL_REINIT_DECOMP_CHANNEL          =  4,
  ROHC_IOCTL_REINIT_COMP_CHANNEL            =  5,
  ROHC_IOCTL_SET_DEFAULT_COMP_FLOW_PARAMS   =  6,
  ROHC_IOCTL_SET_DEFAULT_DECOMP_FLOW_PARAMS =  7,
  ROHC_IOCTL_SWITCH_DECOMP_MODE             =  8,
  ROHC_IOCTL_RESET_ELR                      =  9,
  ROHC_IOCTL_SET_INBOUND_FEEDBACK_FUNC      = 10,
  ROHC_IOCTL_SET_INBOUND_FEEDBACK_COOKIE    = 11,
  ROHC_IOCTL_SET_PDCP_DISCARD_HANDLE        = 12,
  ROHC_IOCTL_MAX
} rohc_ioctl_e_type;

/*---------------------------------------------------------------------------
  ROHC ioctls data.  The description of the IOCTL indicates which data type
  to pass with a specific IOCTL.
---------------------------------------------------------------------------*/
typedef union rohc_ioctl_u_type
{
  rohc_handle_type rohc_handle;
  rohc_inbound_fb_f_type inbound_fb_func;
  void *inbound_fb_cookie;
  void *fb_cookie;
  rohc_params_comp_channel_s_type *comp_channel_params;
  rohc_params_decomp_channel_s_type *decomp_channel_params;
  rohc_params_comp_flow_s_type *comp_flow_params;
  rohc_params_decomp_flow_s_type *decomp_flow_params;
  rohc_mode_e_type switch_decomp_mode;
  uint16 elr_reset_num_pkts;
} rohc_ioctl_u_type;


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================
FUNCTION ROHC_POWERUP_INIT

DESCRIPTION
  Initializes the ROHC layer at powerup.  Sets up the ROHC memory heap.

  THIS FUNCTION MUST BE CALLED BEFORE ANY ROHC INSTANCE IS ALLOCATED.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Initializes ROHC state variables
===========================================================================*/
void rohc_powerup_init
(
  void
);


/*===========================================================================
FUNCTION ROHC_ALLOC

DESCRIPTION
  Allocates a ROHC instance and returns a handle to it.  A NULL return
  indicates failure to allocate an instance.  The error code is returned
  in the err_no argument.  The handle is passed to all ROHC functions and is
  freed with a call to rohc_free().

  If only compressor parameters are passed, only a compressor is allocated.
  Similarly if only decompressor parameters are passed only a decompressor
  will be allocated.

  Compressor and decompressor parameters can be changed by calling the
  rohc_ioctl() function.
  
DEPENDENCIES
  The ROHC memheap must have been initialized prior to calling rohc_alloc()

RETURN VALUE
  non-NULL handle   on success
  NULL              on failure with the error code returned in err_no
                    argument.  err_no codes include:

                    E_INVALID_ARG   Invalid arguments passed
                    E_NO_MEMORY     Insufficient memory
  

SIDE EFFECTS
  Allocates a ROHC instance configuring it with the passed configuration
  parameters.
===========================================================================*/
rohc_handle_type rohc_alloc
(
  rohc_params_comp_channel_s_type         *comp_channel_params,
  const rohc_params_comp_flow_s_type      *comp_flow_params,
  rohc_params_decomp_channel_s_type       *decomp_channel_params,
  const rohc_params_decomp_flow_s_type    *decomp_flow_params,
  errno_enum_type                         *err_no
);

/*===========================================================================
FUNCTION ROHC_SET_LOGGING

DESCRIPTION
  Sets up ROHC logging if called.

DEPENDENCIES
  The ROHC memheap must have been initialized and rohc_alloc called prior to
  calling rohc_set_logging()

PARAMETERS
  rohc_handle_type *handle - pointer to the rohc handle
  dpl_link_cb_type *dpl_cb - pointer to the dpl cb of associated phys link

RETURN VALUE
   0   if successful.
  -1   in case of null arguments.

SIDE EFFECTS
  Sets the ROHC logging control block address variable
===========================================================================*/
int rohc_set_logging
(
  rohc_handle_type handle,
  dpl_link_cb_type *dpl_cb
);

/*===========================================================================
FUNCTION ROHC_FREE_LOGGING

DESCRIPTION
  Frees up logging so it is no longer done.

DEPENDENCIES
  The ROHC memheap must have been initialized and rohc_alloc called prior to
  calling rohc_free_logging()

PARAMETERS
  rohc_handle_type *handle - pointer to the rohc handle

RETURN VALUE
   0   if successful.
  -1   in case of null handle.

SIDE EFFECTS
  Sets the ROHC logging control block address variable to null
===========================================================================*/
int rohc_free_logging
(
  rohc_handle_type handle
);


/*===========================================================================
FUNCTION ROHC_IOCTL

DESCRIPTION
  Performs a requested operation on the passed ROHC instance.  The requested
  operation may asynchronously change state of the instance or a parameter
  associated with the instance.

  Takes a ROHC instance, the type of ioctl and associated data.
  Depending on the IOCTL the data may be used to return information or used
  to pass information.

DEPENDENCIES
  A valid handle must be passed unless performing a GET_DEFAULT_OPTIONS
  ioctl.  Also, ioctl_data must be non-NULL unless using a
  CONTEXT_INITIALIZATION ioctl.

RETURN VALUE
   0   in case of success
  -1   in case of error with the error code returned in err_no.  Values of
       err_no returned include:

       E_INVALID_ARG    Returned when the arguments were invalid
       E_NOT_ALLOWED    Operation not allowed.  For example calling
                        context reinitialization on an instance with no
                        associated compressor

SIDE EFFECTS
  The operation requested is performed on the passed handle.  This may
  change parameters or state of the ROHC instance.
===========================================================================*/
int rohc_ioctl
(
  rohc_handle_type         handle,     /* Instance of ROHC                 */
  rohc_ioctl_e_type        ioctl_type, /* The IOCTL                        */
  const rohc_ioctl_u_type *ioctl_data, /* Associated ioctl information     */
  errno_enum_type         *err_no      /* Error code returned, if any error*/
);


/*===========================================================================
FUNCTION ROHC_COMPRESS

DESCRIPTION
  The ROHC compression function.  It takes the ROHC instance handle along
  with the packet to compress and returns the compressed packet in pkt along
  with the compressed packet type in pkt_type argument.

  If the cid argument is non-NULL the function returns the context identifier
  of the ROHC context used to compress the packet.

  The flowp argument, if non-NULL, is used to initialize a new flow if the
  packet is the first packet in a new flow.

  In case of failure the error that occured is returned in the err argument.

DEPENDENCIES
  A valid handle must be passed.

RETURN VALUE
   0   if successful.
  -1   in case of error with the error code returned in err argument.  Error
       codes returned include:

       E_INVALID_ARG   If arguments passed are invalid

   Compressed packet in pkt, compressed packet type in pkt_type.  CID in
   cid.

SIDE EFFECTS
  Compresses the packet if successful returning it in pkt with the packet
  type returned in pkt_type.  The context identifier of the context used
  is returned in cid argument.  If a new flow is started by the packet it
  is initalized with default flow parameters or the flow parameters passed
  in flowp if non-NULL.
===========================================================================*/
int rohc_compress
(
  rohc_handle_type               handle,  /* ROHC instance                 */
  uint16                        *cid,     /* return CID of compressed pkt  */
  const rohc_params_comp_flow_s_type  *flowp, /* Params to use for new flow    */
  dsm_item_type                **pkt,     /* Packet to compress            */
  errno_enum_type               *err      /* Error code in case of failure */
);


/*===========================================================================
FUNCTION ROHC_DECOMPRESS

DESCRIPTION
  This function is called with a ROHC compressed packet to decompress.  The
  decompressed packet is returned in pkt argument, and the IP version is
  returned in ipvsn argument.

  If the packet is the first in a new flow the flowp argument, if non-NULL can
  be used to initialize the flow.  If flowp is NULL the default flow parameters
  passed in at rohc_alloc() are used to initialize the new flow.

  The ROHC client can pass in the context identifier, if knows, in the in_cid
  argument.

  If a feedback packet is to be sent to the remote compressor, a
  dsm item chain containing the packet will be allocated, and fb_pkt argument
  will contain the packet.
  
DEPENDENCIES
  A valid handle must be passed; fb_pkt, if non-NULL, should point to
  a NULL dsm_item_type

RETURN VALUE
   0   if successful.
  -1   in case of error with the error code returned in err argument.  Error
       codes returned include:
       E_INVALID_ARG   If arguments passed are invalid
       E_DATA_INVALID  The packet passed in contains invalid data
       E_FAILURE       Catch-all error code for any failure that occured
  
  The decompressed packet's IP version is returned in ipvsn.  The decompressed
  packet is returned in pkt argument.

SIDE EFFECTS
  Decompresses the received packet.  May generate a dsm item containing a
  feedback packet.  Note that the caller is responsible for freeing this
  buffer.
  If an error occurs, *pkt will still be valid (and must be freed by the
  caller) but its contents are undefined.  ipvsn may also be modified
  even if an error occurs.
===========================================================================*/
int rohc_decompress
(
  rohc_handle_type                 handle,/* ROHC instance                 */
  const rohc_params_decomp_flow_s_type  *flowp, /* Params to use for new flow */
  const uint16                          *in_cid,/* Context identifier of flow */
  dsm_item_type                  **pkt,   /* Packet to decompress          */
  ip_version_enum_type            *ipvsn, /* IP version of decompressed hdr*/
  dsm_item_type                  **fb_pkt,/* Feedback packet generated     */
  int16                            ll_sn, /* Link layer seq no if available*/
  errno_enum_type                 *err_no  /* Error code in case of failure */
);


/*===========================================================================
FUNCTION ROHC_FREE

DESCRIPTION
  Frees the ROHC instance.  The handle should not be used after a call to
  rohc_free().

DEPENDENCIES
  A valid handle must be passed

RETURN VALUE
  None

SIDE EFFECTS
  Frees the ROHC instance
===========================================================================*/
void rohc_free
(
  rohc_handle_type          handle     /* Instance of ROHC                 */
);

/*===========================================================================
FUNCTION ROHC_COMP_SINK_FEEDBACK

DESCRIPTION
  The ROHC compressor's sink feedback function.  It takes the ROHC instance
  handle along with the feedback packet to sink. This also takes fb_cid as
  another argument, to quickly get flow context instead of re-doing the 
  parsing.

  This API is for multi threaded clients, where Compressor, Decompressor runs 
  in two different contexts.

  When Decompressor gets an inbound feedback through inbound_fb_func 
  handler (called by Decompressor), the handler handover the feedback packet
  to Compressor user task. The Compressor user task calls this API to sink
  the feedback by providing compressor handle.

  In case of failure the error that occured is returned in the err argument.

DEPENDENCIES
  A valid handle must be passed.

RETURN VALUE
   0   if successful.
  -1   in case of error with the error code returned in err argument.  Error
       codes returned include:
       E_INVALID_ARG   If arguments passed are invalid
       E_FAILURE       Catch-all error code for any failure that occured

SIDE EFFECTS
  Upon sinking the feedback, the DSM item is freed.
===========================================================================*/
int rohc_comp_sink_feedback
(
  rohc_handle_type                     handle,/* ROHC compressor instance  */
  uint16                               fb_cid,/* return CID of compressed pkt  */
  dsm_item_type                      **pkt,   /* Feedback Packet to sink */
  errno_enum_type                     *err    /* Error code in case of failure */
);

/*===========================================================================
FUNCTION ROHC_GET_SUPPORTED_PROFILES

DESCRIPTION
  This function returns the integer which has the masks set for all the
  supported ROHC profiles.
  ROHC users can use this function to get all the profiles supported by
  the ROHC Engine.

DEPENDENCIES
  None

RETURN VALUE
  Unsigned Integer with supported ROHC profiles.

SIDE EFFECTS
  None
===========================================================================*/
uint32 rohc_get_supported_profiles
(
  void
);

/*===========================================================================
FUNCTION ROHC_GET_CTRL_BLK_INDEX

DESCRIPTION
  Given a control block pointer, returns the index of the control block

DEPENDENCIES
  None

RETURN VALUE
  Index of the control block

SIDE EFFECTS
  None
===========================================================================*/
uint8 rohc_get_ctrl_blk_index
(
  void * instp
);

/*===========================================================================
FUNCTION ROHC_GET_STAT_QUERY_STATUS

DESCRIPTION
  Given a control block pointer, returns if the control block has had its
  statistics queried yet

DEPENDENCIES
  None

RETURN VALUE
  Query status of the instance

SIDE EFFECTS
  None
===========================================================================*/
uint8 rohc_get_stat_query_status
(
  void * instp
);

/*===========================================================================
FUNCTION ROHC_GET_CTRL_BLK

DESCRIPTION
  Given a control block index, returns the pointer to the control block

DEPENDENCIES
  None

RETURN VALUE
  Pointer to the control block
  NULL if invalid index provided

SIDE EFFECTS
  None
===========================================================================*/
void *rohc_get_ctrl_blk
(
  int32 index
);

/*===========================================================================
FUNCTION ROHC_CLI_SNACK_EVENT_CB_REGISTER

DESCRIPTION
  This api is used to register a callback by the client which is invoked 
  by ROHC when SNACK event generated. 

DEPENDENCIES
  None

RETURN VALUE
  -1   in case of error with the error code returned in err argument.  Error
       codes returned include:
  E_INVALID_ARG If arguments passed are invalid
  E_FAILURE Catch-all error code for any failure that occured

SIDE EFFECTS
  None
===========================================================================*/
int rohc_cli_snack_event_cb_register
(
  rohc_handle_type           handle,       /* ROHC compressor instance   */
  uint8                      ul_num_snack, /* For UL channel,            */ 
                 /* number of SNACKs received after which cb is invoked  */
  uint8                      dl_num_snack,/* For DL channel,             */
                     /* number of SNACKs sent after which cb is invoked  */
  rohc_cli_event_cb_fn_type  snack_event_cb_fn, /* Callback function     */
  void                      *cb_data,      /* Callback data              */
  errno_enum_type            *err   /* Error code in case of failure     */
);

#endif /* _PS_ROHC_H */
