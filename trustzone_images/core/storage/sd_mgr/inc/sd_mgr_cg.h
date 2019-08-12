/** @file sd_mgr_cg.h
   
  This file provides RPMB Content Generation (CG) 
  external definitions. 
  
  Copyright (c) 2012-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY

when         who     what, where, why
----------   ---     --------------------------------------------------------- 
2016-04-01   jt      Add listener buffer argument to process_read  
2013-02-19   jt      Added extra logic to distinguish between write and "erase"
2012-12-05   jt      Initial Version 

=============================================================================*/

#include <comdef.h>

#define  PROG_KEY_REQ              0x0001
#define  READ_COUNTER_REQ          0x0002
#define  WRITE_DATA_REQ            0x0003
#define  READ_DATA_REQ             0x0004
#define  READ_RESULT_REQ           0x0005

#define  PROG_KEY_RESP             0x0100
#define  READ_COUNTER_RESP         0x0200
#define  WRITE_DATA_RESP           0x0300
#define  READ_DATA_RESP            0x0400

#define  BLOCK_SIZE                512

/* Big Endian. Index to the fields of the RPMB Data Frame */
#define  REQ_INDEX                 (BLOCK_SIZE - 2)
#define  RESP_INDEX                (BLOCK_SIZE - 2)
#define  RESULT_INDEX              (BLOCK_SIZE - 4)
#define  BLK_CNT_INDEX             (BLOCK_SIZE - 6)
#define  ADDR_INDEX                (BLOCK_SIZE - 8)
#define  WRITE_COUNTER_INDEX       (BLOCK_SIZE - 12)
#define  NONCE_INDEX               (BLOCK_SIZE - 28)
#define  DATA_INDEX                (BLOCK_SIZE - 284)
#define  KEY_INDEX                 (BLOCK_SIZE - 316)   /* Same index for MAC or Key */
#define  MAC_INDEX                 (BLOCK_SIZE - 316)   /* Same index for MAC or Key */

#define  REQ_FIELD_LEN             0x02   /* 2 Bytes - Bit[0:1] */
#define  RESP_FIELD_LEN            0x02   /* 2 Bytes - Bit[0:1] */
#define  RESULT_FIELD_LEN          0x02   /* 2 Bytes - [2:3] */
#define  BLK_CNT_FIELD_LEN         0x02   /* 2 Bytes - [4:5] */
#define  ADDR_FIELD_LEN            0x02   /* 2 Bytes - [6:7] */
#define  WRITE_COUNTER_FIELD_LEN   0x04   /* 4 Bytes - [8:11] */
#define  NONCE_FIELD_LEN           0x10   /* 16 Bytes [12:27] */
#define  DATA_FIELD_LEN            0x100  /* 256 Bytes [28:283] */
#define  MAC_FIELD_LEN             0x20   /* 32 Bytes - [284:315]. Same field for Key or MAC */
#define  KEY_FIELD_LEN             0x20   /* 32 Bytes - [284:315]. Same field for Key or MAC */
#define  STUFF_BYTES_FIELD_LEN     0xC4   /* 196 Bytes - [316:511] */

#define  MAC_INPUT_LEN  (BLOCK_SIZE - MAC_FIELD_LEN - STUFF_BYTES_FIELD_LEN)

/* RPMB Errors */
#define  RPMB_SUCCESS                 0
#define  RPMB_GENERAL_FAILURE         1
#define  RPMB_AUTHENTICATION_FAILURE  2
#define  RPMB_COUNTER_FAILURE         3
#define  RPMB_ADDR_FAILURE            4
#define  RPMB_WRITE_FAILURE           5
#define  RPMB_READ_FAILURE            6
#define  RPMB_NOT_PROVISIONED_FAILURE 7

typedef struct 
{ 
   uint8 stuff_bytes[196]; 
   uint8 key_MAC[32];
   uint8 data[256];
   uint8 nonce[16];
   uint8 write_counter[4];
   uint8 address[2];
   uint8 block_count[2]; 
   uint8 result[2];
   uint8 request_response[2]; 
} __attribute__ ((packed)) rpmb_data_frame;

/******************************************************************************
* Name: generate_rpmb_key_provision_pkt
*
* Description:
* This function retrieves a Key from TZ and creates the KeyProvision packet
* with this Key. The packet will be sent to the eMMC to program the eMMC's Key
*
* Arguments:
*    key         [IN]:  Pointer to the provision key 
*    pkt_buffer  [OUT]: Pointer to a 512-byte formatted packet for Key provision.
* 
* Returns:
*    SD_MGR_SUCCESS:                   Provision Key packet created successfully
*    SD_MGR_RPMB_KEY_GENERATION_ERROR: Error in creating the provision key 
*    SD_MGR_RPMB_INVALID_PARAM_ERROR:  Invalid pointer to the passed in Packet buffer
* 
******************************************************************************/
int generate_rpmb_key_provision_pkt (uint8 *key, uint8 *pkt_buffer);

/******************************************************************************
* Name: generate_rpmb_read_counter_pkt
*
* Description:
* This function creates the RPMB read counter packet to read the RPMB write 
* counter value.
*
* Arguments:                       
*    pkt_buffer  [OUT]: Pointer to a 512-byte formatted packet
* 
* Returns:
*    SD_MGR_SUCCESS:                   Read counter packet created successfully
*    SD_MGR_RPMB_INVALID_PARAM_ERROR:  Invalid pointer to the passed in Packet buffer
*    SD_MGR_RPMB_RNG_GENERATION_ERROR: Error in generating the Random number
*                                     (used by the Nonce field)
* 
******************************************************************************/
int generate_rpmb_read_counter_pkt (uint8 *pkt_buffer);

/******************************************************************************
* Name: generate_rpmb_read_pkt
*
* Description:
* This function generates a read command packet to do an authenticated read.
*
* Arguments:
*    start_sector   [IN] : Starting sector to read from 
*    pkt_buffer     [OUT]: Pointer to a formatted packet to be sent to eMMC
* 
* Returns:
*    SD_MGR_SUCCESS:                   The Read packet generation succeeded
*    SD_MGR_RPMB_INVALID_PARAM_ERROR:  Invalid passed in pointer
*    SD_MGR_RPMB_RNG_GENERATION_ERROR: Error in generating the Random number
*                                      (used by the Nonce field)
* 
******************************************************************************/
int generate_rpmb_read_pkt (uint16 start_sector, uint8 *pkt_buffer);

/******************************************************************************
* Name: generate_rpmb_write_pkts
*
* Description:
* This function generates RPMB Data Frames as defined by the eMMC specification.
* Depending on the value of the Reliable Write Count (RW), the RPMB frames will be
* generated as follows:
* a. If RW > 1 and (2*Count) is a multiple of RW, the RPMB frames will be created
*    so that the driver must write to the RPMB partition with Write transfer size
*    equals the RW Count value. For example, if RW Count is 4 and Count = 8 sectors
*    of data, 16 512-byte frames buffer will be created. The driver must write
*    4 512-byte frames to the RPMB each write transfer for a total of 4 transfers.
* b. If RW == 1 or (2*Count) is not a multiple of RW Count, the RPMB frames will be
*    created so that the driver must write a single 512-byte frame for each write transfer.
*
* Arguments:
*    erase               [IN]:  Indicates if the write is a "erase" or not 
*    start_sector        [IN]:  Starting Sector
*    half_sector_count   [IN]:  Number of half sectors to write 
*    data                [IN]:  Pointer to Data to be written to RPMB partition
*    pkt_buffer          [OUT]: Pointer to a authenticated write packet that CG has
*                               formated
* 
* Returns:
*    SD_MGR_SUCCESS:                   Write Packet generated successfully
*    SD_MGR_RPMB_INVALID_PARAM_ERROR:  Invalid passed in pointer
*    SD_MGR_RPMB_MAC_GENERATION_ERROR: Error when generating the HMAC
* 
******************************************************************************/
int generate_rpmb_write_pkts (boolean erase, uint16 start_sector, 
                              uint16 half_sector_count, 
                              uint8 *data, uint8 *pkt_buffer);

/******************************************************************************
* Name: process_rpmb_read_response_pkts
*
* Description:
* This function validates the Read Response packets. It does the
* following (in order)
*   1. Make sure the response type is either an authenticated read
*      response or a read counter response 
*   2. Check the result field for any errors
*   3. Compare CG's nonce with the nonce provided in pkt_buffer 
*   4. Generate the HMAC and compare with the one provided in pkt_buffer
*
* Arguments:
*    pkt_buffer   [IN]: Pointer to RPMB read response packets
*    num_pkts     [IN]: Number of read response packets
*    req_nonce    [IN]: Value of the nonce of the request
*    listener_buf [IN]: Listener buffer pointer 
* 
* Returns:
*    SD_MGR_SUCCESS:                   The HMAC and nonce authentication succeeded                                   
*    SD_MGR_RPMB_INVALID_PARAM_ERROR:  Invalid pointer to the passed in Packet buffer
*    SD_MGR_RPMB_MAC_GENERATION_ERROR: The HMAC generation failed
*    SD_MGR_RPMB_MAC_ERROR:            The HMAC authentication failed
*    SD_MGR_NONCE_ERROR:               The nonce authentication failed 
* 
******************************************************************************/
int process_rpmb_read_response_pkts (uint8 *pkt_buffer, uint32 num_pkts, 
                                     uint8 *req_nonce, uint8* listener_buf);

/******************************************************************************
* Name: process_rpmb_write_response_pkts
*
* Description:
* This function validates the Write Response packet. It does the following (in order)
*   1. Make sure the response type is either an authenticated write
*      response or a key provisioning response 
*   2. Check the result field for any errors
*   3. Generate the HMAC and compare with the one provided in pkt_buffer
*   4. Double check the RPMB write counter value 
*
* Arguments:
*    pkt_buffer  [IN]: Pointer to RPMB write response packet
* 
* Returns:
*    SD_MGR_SUCCESS:                   The HMAC authentication and Write Counter
*                                      validation succeeded
*    SD_MGR_RPMB_INVALID_PARAM_ERROR:  Invalid pointer to the passed in Packet buffer
*    SD_MGR_RPMB_MAC_GENERATION_ERROR: The HMAC generation failed.
*    SD_MGR_RPMB_MAC_ERROR:            The HMAC authentication failed
*    SD_MGR_RPMB_WRITE_COUNTER_ERROR:  The Write Counters validation failed
* 
******************************************************************************/
int process_rpmb_write_response_pkt (uint8 *pkt_buffer);

/******************************************************************************
* Name: set_rel_write_count
*
* Description:
* This function sets the eMMC reliable write count.
* 
* Arguments:                       
*    rw_count [IN]: eMMC reliable write count
* 
* Returns:
*    None
* 
******************************************************************************/
void set_rel_write_count (uint32 rw_count);

/******************************************************************************
* Name: set_rpmb_counter
*
* Description:
* This function sets the RPMB Write Counter value. 
* 
* Arguments:                       
*    rsp_pkt [IN]: Pointer to a RPMB response packet
* 
* Returns:
*    SD_MGR_SUCCESS if no errors, error code otherwise
* 
******************************************************************************/
int set_rpmb_counter (uint8 *rsp_pkt);



