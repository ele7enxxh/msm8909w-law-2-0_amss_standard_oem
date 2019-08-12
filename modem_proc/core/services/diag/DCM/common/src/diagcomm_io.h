#ifndef DIAGCOMM_IO_H 
#define DIAGCOMM_IO_H
/*==========================================================================

              Diag I/O Wrapper APIs for Interprocessor Communication and I/O 

General Description
  
Copyright (c) 2010-2014 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.mpss/3.5.c12.3/services/diag/DCM/common/src/diagcomm_io.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/14   rs      Removed EQUIP_ID_MAX.
06/03/14   rh      Changes to support 8KB cmd/rsp
04/24/14   tbg     Updated comments for STREAM_LISTENER usage
09/20/13   tbg     Added support for Diag over STM 
08/31/13   is      Support for peripheral buffering mode
01/03/13   sg      Command Response Channel separation
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
11/15/12   is      Support for preset masks
10/11/12   rh      Added new api declarations for Tx wmq
05/03/12   sa      Mainlining DIAG_CENTRAL_ROUTING
03/07/12   rh      Added DIAG_MIN_STREAM_ID to go with DIAG_MAX_STREAM_ID
10/14/11   hm      Diag consumer API - modem side changes
10/07/11   is      Support sending log mask per equipment id
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
08/09/11   wjg     Added data_pending flag to diagcomm_io_conn_type for use
                     in diagcomm_smdl_buffer_read
03/04/11   is      Support for Diag over SMD-Lite

===========================================================================*/

#include "core_variation.h"

#ifdef DIAG_SIO_SUPPORT  
  #include "sio.h"
#endif

#ifdef DIAG_SMDL_SUPPORT   
  #include "smd_lite.h"
#endif

#include "dsm.h"                              /* For dsm_item_type */
#include "feature.h"                          /* For DIAG_INT_FEATURE_MASK_LEN */
#include "msgcfg.h"                           /* MSG_MASK_TBL_CNT */

#ifndef DIAG_SIO_SUPPORT  
  typedef int sio_bitrate_type;               /* Defined by sio.h, but used in Diag public API headers */
  typedef int rdm_device_enum_type;
#endif

#ifdef DIAG_SMDL_SUPPORT  
  #define DIAG_SMDL_CHANNEL_NAME_SIZE_MAX 20  /* Should match SMD_CHANNEL_NAME_SIZE_MAX from smd_internal.h */
#endif

/* ------------------------------------------------------------------------
** Typedefs and defines
** ------------------------------------------------------------------------ */

#define DIAGCOMM_IO_NO_STREAM_ID -1 /* SIO_NO_STREAM_ID - Returned when sio_open() fails */
#define NUM_IO_CHANNELS 2
#define NUM_SMD_PORTS 2             /* size of diagcomm_enum_port_type */
#define NUM_SIO_PORTS 2

#define NUM_SMD_PORT_TYPES 2        /* size of diagcomm_port_type */

/* For log/event/f3 mask streams */
#define DIAG_MIN_STREAM_ID          1
#define DIAG_MAX_STREAM_ID          2
#define DIAG_STREAM_ALL             0 /* For all streams */
#define DIAG_STREAM_1               1 /* QXDM stream */
#define DIAG_STREAM_2               2 /* DCI stream */

/* Alias used for STM Mode (indicates which stream is piped to STM, if Diag STM is active) */
#define DIAG_STREAM_STM DIAG_STREAM_1
/* 'Listener' stream ID is used indicate packet is needed by a listener. */
/* (Streams 1 and 2 should not be used to designate listern usage) */
#define DIAG_STREAM_LISTENER 0x80

#define DIAG_PRESET_MASK_ALL        0 /* For all presets */
#define DIAG_MIN_PRESET_ID          1
#define DIAG_MAX_PRESET_ID          2
#define DIAG_PRESET_MASK_1          1
#define DIAG_PRESET_MASK_2          2
#define DIAG_DCI_MASK_1             1
#define DIAG_MIN_DCI_ID             1
#define DIAG_MAX_DCI_ID             1

#define DIAG_INDEX_TO_ID( index )	       (index+1)
#define DIAG_ID_TO_INDEX( id )		       (id-1)
#define DIAG_ID_TO_PORT( id )              (id-1)       /* DIAG_STREAM_1 (1) -> DIAGCOMM_PORT_1 (0), etc */
#define DIAG_PORT_TO_INDEX( port_index )   (port_index) /* DIAGCOMM_PORT_1 (0) -> DIAG_STREAM_1 (1) */

typedef void (*diagcomm_close_cb_fnc_ptr_type)( void );

typedef enum
{
  DIAGCOMM_PORT_SMD = 0,
  DIAGCOMM_PORT_SIO = 1
} diagcomm_port_type;

/*lint -save -e{641} */
typedef enum
{
  DIAGCOMM_PORT_1 = 0,  
  DIAGCOMM_PORT_2 = 1,
  DIAGCOMM_PORT_NUM_LAST = 1  
} diagcomm_enum_port_type;
/*lint -restore */

typedef enum
{
  DIAGCOMM_IO_DATA = 0,
  DIAGCOMM_IO_CTRL = 1,
  DIAGCOMM_IO_CMD = 2
  
} diagcomm_io_channel_type;

typedef enum
{ 
  DIAGCOMM_IO_TYPE_INVALID = -1,
  DIAGCOMM_IO_TYPE_SIO = 0,
  DIAGCOMM_IO_TYPE_SMDL = 1
  
} diagcomm_io_type;


typedef union 
{  
#ifdef DIAG_SIO_SUPPORT
  sio_stream_id_type  sio_handle;  /* int2 */
#endif

#ifdef DIAG_SMDL_SUPPORT  
  smdl_handle_type    smdl_handle; /* typedef struct smd_stream_info_struct * smdl_handle_type; */
#endif

} diagcomm_io_handle_type;


typedef union
{
#ifdef DIAG_SIO_SUPPORT
  struct
  {
    sio_open_type                  params;
    sio_open_param_type            open_param;
    diagcomm_close_cb_fnc_ptr_type close_func_ptr;
  } sio_params;
#endif
 
#ifdef DIAG_SMDL_SUPPORT  
  struct
  {
    char                      name[DIAG_SMDL_CHANNEL_NAME_SIZE_MAX];
    smd_channel_type          edge;
    uint32                    flags;
    uint32                    fifo_size;
    smdl_callback_t           notify;
    void                     *cb_data;
    
    dsm_watermark_type       *rx_queue;
    dsm_watermark_type       *tx_queue;
    dsm_mempool_id_enum_type  dsm_mempool_id;    /**< DSM pool id type. */
  } smdl_params;
#endif

} diagcomm_io_open_params_type;


typedef union
{
  dsm_item_type      *dsm_ptr;               /* Both uses DSMs for now */

#ifdef DIAG_SMDL_SUPPORT  
  struct
  {
    uint32            len;
    void             *buffer;
    uint32            flags;
  } smdl_params;                             /* Unused now */
#endif

} diagcomm_io_tx_params_type;


typedef struct 
{
  diagcomm_io_type io_type;                  /* IO_TYPE_SIO or IO_TYPE_SMDL */
  diagcomm_enum_port_type port_num;          /* PORT_1, PORT_2, etc */
  diagcomm_port_type port_type;              /* PORT_SMD or PORT_SIO */
  diagcomm_io_channel_type channel_type;     /* IO_DATA or IO_CTRL */
  boolean connected;                         /* Port connection state */
  boolean channel_connected;                 /* Channel (both ports) connection state */
  diagcomm_io_handle_type handle;            /* SIO port ID / SMDL edge */
  diagcomm_io_open_params_type open_params;  /* Params to diagcomm_io_open() */
  
  boolean data_pending;                      /* Connection waiting to be read from */
  
  /* Needed for CTRL channel only */
#if defined(DIAG_MP_MASTER)
  uint8 num_presets;                                                        /* Number of preset masks supported by remote proc */
  dword mask_preset_update[DIAG_MAX_PRESET_ID];                             /* Bitmask to keep track of which masks needs to be sent to Slaves */
  dword mask_dci_update[DIAG_MAX_DCI_ID];
  
  boolean * msg_mask_update[DIAG_MAX_STREAM_ID];
  boolean msg_mask_preset_update[DIAG_MAX_PRESET_ID][MSG_MASK_TBL_CNT];     /* Keeps track of which F3 mask has been updated for Stream 1 */
  #if defined(DIAG_STREAM_2_MASK_CHECK)
    boolean msg_mask_dci_update[DIAG_MAX_DCI_ID][MSG_MASK_TBL_CNT];
  #endif
  boolean * log_mask_update[DIAG_MAX_STREAM_ID];
  boolean log_mask_preset_update[DIAG_MAX_PRESET_ID][LOG_EQUIP_ID_MAX + 1];         /* Keeps track of which equip id log mask has been updated for Stream 1 */
  boolean log_mask_dci_update[DIAG_MAX_DCI_ID][LOG_EQUIP_ID_MAX + 1];
#endif  
  byte feature_mask[DIAG_INT_FEATURE_MASK_LEN];                             /* Diag feature mask of remote proc */
#if defined DIAG_SMDL_SUPPORT 
  boolean diag_prev_tx_pending;
  smdl_iovec_type diag_tx_iovec;
  smdl_iovec_type diag_rx_iovec;
#endif

} diagcomm_io_conn_type;

/* ------------------------------------------------------------------------
** Function prototypes
** ------------------------------------------------------------------------ */
void diagcomm_io_init( void );
void diagcomm_io_open_init( diagcomm_io_conn_type * conn );
boolean diagcomm_io_open( diagcomm_io_conn_type * conn );
void diagcomm_io_close( diagcomm_io_conn_type * conn );
void diagcomm_io_transmit( diagcomm_io_conn_type * conn, diagcomm_io_tx_params_type * tx_param );
void diagcomm_io_ioctl( diagcomm_io_conn_type * conn, uint32 cmd, void * param );
void diagcomm_io_read( diagcomm_io_conn_type * conn );
boolean diagcomm_io_status( diagcomm_io_conn_type * conn );

void diagcomm_io_enable_flow( diagcomm_io_conn_type * conn );
void diagcomm_io_disable_flow( diagcomm_io_conn_type * conn );
boolean diagcomm_is_ondevice_mode( diagcomm_io_conn_type * conn );

void diagcomm_io_set_port( diagcomm_io_conn_type * conn );
void diagcomm_io_flush_tx( diagcomm_io_conn_type * conn, void (*flush_cb) (void),unsigned int timeoutVal );
uint32 diagcomm_io_get_rx_wmq_cnt( diagcomm_io_conn_type * conn );
uint32 diagcomm_io_get_tx_wmq_cnt( diagcomm_io_conn_type * conn );
dsm_item_type * diagcomm_io_dequeue_rx_wmq( diagcomm_io_conn_type * conn );
dsm_item_type * diagcomm_io_dequeue_tx_wmq( diagcomm_io_conn_type * conn );
void diag_io_detect_connection( diagcomm_io_conn_type * conn );
void diagcomm_io_conn_changed( diagcomm_io_conn_type * conn, boolean connected );
boolean diagcomm_io_has_tx_data( diagcomm_io_conn_type * conn );

#if defined(DIAG_MP_MASTER)
void diagcomm_mask_update( dword mask, uint8 stream_id, uint8 preset_id, boolean send_now );
#endif

#endif //DIAGCOMM_IO_H
