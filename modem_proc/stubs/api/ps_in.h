#ifndef PS_IN_H
#define PS_IN_H
/*===========================================================================

                   M O D E L    H E A D E R    F I L E

DESCRIPTION


Copyright (c) 2003-2010 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/* IPv6 address structure */
struct ps_in6_addr
{
  union
  {
    uint8   u6_addr8[16];
    uint16  u6_addr16[8];
    uint32  u6_addr32[4];
    uint64  u6_addr64[2];
  } in6_u;

#define ps_s6_addr    in6_u.u6_addr8
#define ps_s6_addr16  in6_u.u6_addr16
#define ps_s6_addr32  in6_u.u6_addr32
#define ps_s6_addr64  in6_u.u6_addr64
};


#endif

