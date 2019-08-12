#ifndef PS_ICMP_HDR_H
#define PS_ICMP_HDR_H

#include "comdef.h"
#include "customer.h"
#include "ps_in.h"

/*---------------------------------------------------------------------------
  Generic ICMP header information.
---------------------------------------------------------------------------*/
struct icmp_hdr
{
  uint8 type;
  uint8 code;
  uint16 chksum;
};

/*---------------------------------------------------------------------------
  ICMP arguments.
---------------------------------------------------------------------------*/
struct mtu_s
{
  uint16 unused;
  uint16 mtu;
};

struct echo_s
{
  uint16 id;
  uint16 seq;
};

union icmp_args
{
    uint8             pointer;
    uint32            unused;
    struct            mtu_s mtu;
    struct ps_in_addr address;
    struct echo_s     echo;
};

/*---------------------------------------------------------------------------
  Structure for timestamps
---------------------------------------------------------------------------*/
struct icmp_timestamp
{
  uint32 orig_timestamp;
  uint32 rx_timestamp;
  uint32 tx_timestamp;
};
/*---------------------------------------------------------------------------
  Internal type definitnion for ICMP header.
---------------------------------------------------------------------------*/
struct icmp
{
  struct icmp_hdr       info;
  union  icmp_args      arg;
  struct icmp_timestamp t_stamp;
};


#endif
