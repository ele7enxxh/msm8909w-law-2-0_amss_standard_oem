#ifndef DS_UM_LO_H
#define DS_UM_LO_H


#include "datamodem_variation.h"
#include "comdef.h"
#include "customer.h"

#if (defined   (FEATURE_DATA_TEST_LOOPBACK) || \
     defined (FEATURE_DATA_TEST_LOOPBACK_PDCPSIM) \
    )

#include "dsm.h"
#include "nv_items.h"
#include "dsumtsps.h"
#include "ps_iface.h"


/*----------------------------------------------------------------------------
  IP packet format                                                                             
-----------------------------------------------------------------------------*/
typedef struct  _dsumtsps_ip_format {
  unsigned char       bVer;
  unsigned char       bServField;
  unsigned short      hwTotalLen;
  unsigned short      hwID;
  unsigned short      hwFragOff;
  unsigned char       bTTL;
  unsigned char       bProt;
  unsigned short      hwHeaderChkSum;
  unsigned long int   wSource;
  unsigned long int   wDest;
} dsumtsps_ip_format;

typedef struct _dsumtsps_icmp_format {
  unsigned char       bType;
  unsigned char       bCode;
  unsigned short      hwCheckSum;
  unsigned short      wIdent;
  unsigned short      wSeqNum;
} dsumtsps_icmp_format;

typedef struct _dsumtsps_udp_hdr {
  unsigned long int   wSource;
  unsigned long int   wDest;
  unsigned char       hwProt[2];
  unsigned char       hwLen[2];
} dsumtsps_udp_hdr;

typedef struct _dsumtsps_loopback_ip_addr_type {
  uint32   ip_addr;
  boolean  is_set;
} dsumtsps_loopback_ip_addr_type;

typedef struct _dsumtsps_icmp6_gen_format {
  unsigned char       type;
  unsigned char       code;
  unsigned short      checkSum;
} dsumtsps_icmp6_gen_format;

typedef struct _dsumtsps_udp6_gen_format {
  unsigned short      source_port;
  unsigned short      dest_port;
  unsigned short      length;
  unsigned short      checksum;
} dsumtsps_udp6_gen_format;

typedef struct _dsumtsps_icmp6_ro_ad_format 
{
  unsigned int        type:8;
  unsigned int        code:8;
  unsigned int        checkSum:16;
  unsigned int        curHopLimit:8;
  unsigned int        MOFlag:8;
  unsigned int        roLifetime:16;
  unsigned int        reachableTime;
  unsigned int        retransTimer;
} dsumtsps_icmp6_ro_ad_format;

typedef struct _dsumtsps_icmp6_prefix_info_format {
  uint8       type;
  uint8       length;
  uint8       prefixLen;
  uint8       LAFlag;
  uint32      validLifetime;
  uint32      prefLifetime;
  uint32      reserved;
  struct ps_in6_addr  prefixAddr;              /* 128-bit source address     */
} dsumtsps_icmp6_prefix_info_format;


#define IP_HDR_LENGTH           20
#define IPV6_HDR_LENGTH         40
#define ICMP_CHECKSUM_OFFSET    2    
#define UDP_CHECKSUM_OFFSET     6  

/* Loopback packet handler type */
typedef void (*dsumtsps_loopback_data_hdlr)
                                          (
                                       /* Pointer to DSM item with the packet*/
                                            dsm_item_type     **item_ptr,
                                       /* Currently not used */
                                            uint32              a2_port
                                          );
/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_COMPUTE_CHECKSUM

DESCRIPTION
  Helper function to compute checksum     


PARAMETERS

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
unsigned short int
dsumtsps_loopback_compute_checksum(unsigned char * pkt, long lpkt, long offset);

/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_COMPUTE_ICMP_CHECKSUM

DESCRIPTION
  Computes ICMP packet checksum

PARAMETERS
  pkt - pointer to ICMP packet
  lpkt - length of the packet

DEPENDENCIES
  None.

RETURN VALUE
  int

SIDE EFFECTS
  None.

===========================================================================*/
unsigned short int
dsumtsps_loopback_compute_icmp_checksum(unsigned char * pkt, long lpkt);

/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_COMPUTE_UDP_CHECKSUM

DESCRIPTION
  Computes UDP packet checksum

PARAMETERS
  pkt - pointer to ICMP packet
  lpkt - length of the packet

DEPENDENCIES
  None.

RETURN VALUE
  int

SIDE EFFECTS
  None.

===========================================================================*/
unsigned short int
dsumtsps_loopback_compute_udp_checksum(unsigned char * pkt, long lpkt);

/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_COMPUTE_ICMP6_CHECKSUM

DESCRIPTION
  Computes UDP packet checksum

PARAMETERS
  pkt - pointer to ICMP packet
  lpkt - length of the packet

DEPENDENCIES
  None.

RETURN VALUE
  int

SIDE EFFECTS
  None.

===========================================================================*/
unsigned short int
dsumtsps_loopback_compute_icmp6_checksum(unsigned char * pkt, long lpkt);

/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_COMPUTE_V6UDP_CHECKSUM

DESCRIPTION
  Computes UDP packet checksum

PARAMETERS
  pkt - pointer to UDP packet
  lpkt - length of the packet

DEPENDENCIES
  None.

RETURN VALUE
  int

SIDE EFFECTS
  None.

===========================================================================*/
unsigned short int dsumtsps_loopback_compute_v6udp_checksum(unsigned char * pkt, long lpkt);
/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_CONVERT_PACKET

DESCRIPTION
  This function converts ICMP ping request packets to ping responces, and
  swaps addresses in UDP iperf traffic
  In process, it will verify checksum (first on the packet before conversion,
  then on the packet after coversion)
  
PARAMETERS
   item_ptr - pointer to the DSM item with the packet
   pkt_len  - will be filled in with the length of resulting pakcet 
              (can be passed in as NULL if not needed)
   
DEPENDENCIES
  None.
  
RETURN VALUE
  TRUE  if conversion was successful
  FALSE if packet was corrupted (note, FALSE only indicates checksum failure.
  packet may still be converted even if FALSE was returned)
  
SIDE EFFECTS
  Caller must check item_ptr for NULL after calling this function
  
===========================================================================*/
boolean dsumtsps_loopback_convert_packet
(
  dsm_item_type     **item_ptr,
  uint16            * pkt_len
);

#endif /* ..LOOPBACK or ..LOOPBACK_PDCPSIM */ 

#ifdef FEATURE_DATA_TEST_LOOPBACK

/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_IFACE_UP_CMD_HDLR

DESCRIPTION
Called in DS TASK context in response to an iface up command
  on the UMTS iface.
  Brings up the iface requested after allocating a PDN context and a bearer
  context.  
  
PARAMETERS
  iface_p       - Pointer to the iface that is to be brought up.
  info_ptr      - Pointer to data passed in from the app.

DEPENDENCIES
  Called in DS TASK context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

  
===========================================================================*/
void dsumtsps_loopback_iface_up_cmd_hdlr
(
    ps_iface_type *iface_p,
    void          *info_ptr
);

/*===========================================================================

              LOOPBACK API DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_TEST_PC_REGISTER_DATA_HDLR

DESCRIPTION
  API to register a callback which will be called every time a packet is 
  recieved on tx path. Packet will be modified prior to delivering it to 
  the callback 
  
PARAMETERS  
    pfn_hdlr - pointer to the handler to register
    a2_port  - currently ignored
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.

===========================================================================*/
void dsumtsps_loopback_test_pc_register_data_hdlr 
(
   dsumtsps_loopback_data_hdlr pfn_hdlr,
   uint32                      a2_port 
);


/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_TEST_PC_SET_IP_ADDR

DESCRIPTION
  This funciton allows to set the IP address of the loopback iface 
  
PARAMETERS  
  ip_addr  - address of the loopback adapter
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.

SIDE EFFECTS 
  None.

===========================================================================*/
void dsumtsps_loopback_test_pc_set_ip_addr
(
  uint32  ip_addr
);


/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_TEST_PC_ACTIVATE

DESCRIPTION
  Activates looback mechanism
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None. 
  
SIDE EFFECTS 
  None.

===========================================================================*/
void dsumtsps_loopback_test_pc_activate (void);     


/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_TEST_PC_IS_ACTIVE

DESCRIPTION
  Checks if looback mechanism is active
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  TRUE - active
  FALSE - inactive
  
SIDE EFFECTS 
  None.

===========================================================================*/
boolean dsumtsps_loopback_test_pc_is_active (void); 


/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_TEST_PC_DEACTIVATE

DESCRIPTION
  De-activates loopback mechanism
  
PARAMETERS  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.

===========================================================================*/
void dsumtsps_loopback_test_pc_deactivate(void);


/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_TO_PS_HIGH_WATERMARK

DESCRIPTION
  This function is called when the Loopback to PS watermark reaches the low
  watermark. 
  
PARAMETERS
   wm            : pointer to the watermark
   callback_data : Call instance registered with the watermark callback
   
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  
===========================================================================*/
void dsumtsps_loopback_ps_high_watermark_cb
(
  dsm_watermark_type *wm,
  void*              callback_data
);

/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_TO_PS_LOW_WATERMARK

DESCRIPTION
  This function is called when the Loopback to PS watermark reaches the low
  watermark. 
  
PARAMETERS
   wm            : pointer to the watermark
   callback_data : Call instance registered with the watermark callback
   
DEPENDENCIES
  None.
  
RETURN VALUE
  None.
  
SIDE EFFECTS
  
===========================================================================*/
void dsumtsps_loopback_ps_low_watermark_cb 
(
  dsm_watermark_type *wm,
  void*              callback_data
);

/*===========================================================================
FUNCTION DSUMTSPS_LOOPBACK_SET_UDP_CHECK_VERIF

DESCRIPTION
  De-activates loopback mechanism
  
PARAMETERS
  bEnable - TRUE to enable checksum verification
            FALSE to disable checksum verification  
    
DEPENDENCIES 
  None.
  
RETURN VALUE 
  None.
  
SIDE EFFECTS 
  None.

===========================================================================*/
void dsumtsps_loopback_set_udp_checksum_verif(boolean bEnable);



#endif /* FEATURE_DATA_TEST_LOOPBACK */
#endif /* DS_UM_LO_H */
