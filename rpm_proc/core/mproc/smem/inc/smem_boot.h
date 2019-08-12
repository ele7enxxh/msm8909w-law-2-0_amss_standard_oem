#ifndef _SMEM_BOOT_H_
#define _SMEM_BOOT_H_

/*===========================================================================

                    S H A R E D   M E M O R Y

DESCRIPTION
    Defines the structures to be used by SMD and SMSM, to be initialized in 
    Shared Memory by SMEM drivers at BOOT time.

Copyright (c) 2011-2012 by Qualcomm Technologies, Incorporated.  All Rights Reserved.

===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/rpm.bf/2.1.1/core/mproc/smem/inc/smem_boot.h#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
08/23/12    bt     Set SMEM_SMD_LOOPBACK_NUM_CHANNELS to 15, avoid dependencies.
07/31/12    pa     Cleaned up header inclusions.
02/02/12    bt     Remove inclusion of smem_os.h so it can stand alone in inc/.
10/31/11    bm     Initial version.

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "smd_type.h"
/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
#define SMEM_SMD_LOOPBACK_NUM_CHANNELS 15

#define SMEM_SMD_NWAY_EDGE_UNUSED      0xFFFFFFFF

/**
   SMSM hosts and entries size information.

   These values are set in smem_boot_init() in the Apps' sbl2, before any 
   processor calls smem_init().  In each smem_init(), the SMSM shared memory
   items will be allocated with a size based on these values.  E.g., the
   state_shared has num_entries words, and the intr_mask is a 2D array of 
   dimensions num_hosts * num_entries.  This will prevent future interprocessor
   dependencies on these values because they must only be changed once when 
   set in smem_boot_init().
*/
typedef struct
{
  uint32 num_hosts;
  uint32 num_entries;
  uint32 reserved0;
  uint32 reserved1;
}smem_smsm_size_info_type;

/**
 * A copy of this command structure is used for each loopback channel.
 * It contains information that is used by the client API, as well as by the 
 * server when responding to the client's commands.
 */
typedef struct
{
  /** Indicates what action/state the client wishes the server to perform. */
  uint32 client_state;
  /** Indicates the current state of the server. */
  uint32 server_state;
  /** The name used for the underlying SMD port. */
  char   name[SMD_CHANNEL_NAME_SIZE_MAX];
  /** The edge indicating which two processors are joined by the SMD port. */
  uint32 edge;
  /** The total bytes of data that the port can hold in each direction is
   * fifo_size - 4.  Each loopback port has a tx and rx buffer of this size.  
   * In SMD_LOOPBACK_ECHO mode, the client may therefore write no more than 
   * twice this value + 512 (for the loopback buffer) bytes to the port, before 
   * reading some of it out again after it has been looped back.  The actual 
   * max may be less than twice the (fifo_size - 4) depending on the xfer_type 
   * and organization of the data passed. */
  uint32 fifo_size;
  /** Indicates whether the server will handle the data as if it was transferred 
   * in Streaming or Packet mode. */
  uint8  xfer_type;
  /** The \c smd_loopback_api_type that the server will use.  Not affected by 
   * the client's chosen API type. */
  uint8  server_api;
  /** The \c smd_loopback_mode_type for this port, such as SMD_LOOPBACK_ECHO. */
  uint8  loopback_mode;
  /** Not used, should remain 0. */
  uint8  reserved;
  /** The following four values indicate the server's RS232 signals as seen by
   * the client.  The server sets them when the port is first opened, and then
   * the client changes them when server signal changes are requested via 
   * \c smd_loopback_signal. */
  uint8  sig_dtr;
  uint8  sig_cts;
  uint8  sig_cd;
  uint8  sig_ri;
} smem_smd_loopback_command_type;

#endif /* _SMEM_BOOT_H_ */

