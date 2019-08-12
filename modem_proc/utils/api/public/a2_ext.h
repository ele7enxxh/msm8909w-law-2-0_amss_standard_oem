#ifndef A2_EXT_H
#define A2_EXT_H
/*!
  @file a2_ext.h

  @brief
   Interface to the A2 module.

  @ingroup a2_ext_interface
*/

/*==============================================================================

  Copyright (c) 2012 Qualcomm Technologies Incorporated. All Rights Reserved

  Qualcomm Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/utils/api/public/a2_ext.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------------
01/21/14   ars     QMAP minimum pad negotiation 
05/18/12   yuw     Added 2 new return codes
03/23/12   yuw     Added API for statistics
03/16/12   yuw     Initial version

==============================================================================*/


/*==============================================================================

                   EXTERNAL DEFINITIONS AND TYPES

==============================================================================*/

/*! @brief This structure defines the variables for traffic statistics in A2 */
typedef struct
{
  /*! total number of packets processed on DL (across all end points)
      by A2 */
  uint32 total_dl_num_pkts;

  /*! total number of bytes processed on DL (across all end points)
      by A2 */
  uint32 total_dl_num_bytes;

  /*!< number of packets received on A2 ul end point 0 */
  uint32 num_pkts_ul_endp_0;

  /*!< number of bytes received on A2 ul end point 0 */
  uint32 num_bytes_ul_endp_0;

  /*!< number of packets received on A2 ul end point 1 */
  uint32 num_pkts_ul_endp_1;

  /*!< number of bytes received on A2 ul end point 1 */
  uint32 num_bytes_ul_endp_1;

  /*!< number of packets received on A2 ul end point 2 */
  uint32 num_pkts_ul_endp_2;

  /*!< number of bytes received on A2 ul end point 2 */
  uint32 num_bytes_ul_endp_2;
}a2_ext_traffic_stats_s;


/*! @brief This structure defines the statistics for capturing wakeup packet */
typedef struct
{
  /*!< Counters of reset API is called */
  uint32 num_reset_wakeup_pkt_calls;

  /*!< Counters of get API is called */
  uint32 num_get_wakeup_pkt_calls;

  /*!< Counters of times that wakeup pkt is saved */
  uint32 num_times_wakeup_pkt_saved;
}a2_ext_wakeup_pkt_stats_s;

/*
  List of return code
 */
typedef enum
{
  A2_SUCCESS = 0,
  A2_ERROR_FAILURE,
  A2_ERROR_INVALID_FIRST_ARGUMENT,
  A2_ERROR_INVALID_SECOND_ARGUMENT,
  A2_ERROR_UNCIPHERED_PKT_NOT_AVAILABLE,
  A2_ERROR_CAPTURE_WAKEUP_PKT_IN_PROGRESS,
  A2_ERROR_WAKEUP_PKT_NOT_AVAILABLE,
  A2_ERROR_RESET_API_NOT_CALLED,
  A2_ERROR_FEATURE_NOT_AVAILABLE,
  A2_ERROR_MUX_NOT_SUPPORTED,
  A2_ERROR_MIN_PAD_NOT_WORD_ALIGNED
}a2_error_e;

/*==============================================================================

                    EXTERNAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

  FUNCTION:  a2_per_get_uldl_data_statistics

==============================================================================*/
/*!
  @brief Provide A2 data traffic statistics on UL/DL
  
  @arguments:
   *stats_ptr: 
      Pointer to memory (allocated by caller) where stores
      statistics info based on structure a2_ext_traffic_stats_s
  @note:
   "statistics_ptr" shall NOT be null
*/
/*============================================================================*/
void a2_per_get_uldl_data_statistics( a2_ext_traffic_stats_s *statistics_ptr );


/*==============================================================================

  FUNCTION:  a2_per_reset_ul_data_statistics

==============================================================================*/
/*!
  @brief reset A2 UL data traffic statistics to zero per A2 endpoint

  @note:
   'endp_idx' shall be set to ONLY valid values {0, 1, 2}
   
  @arguments:
   endp_idx:  index of A2 endpoint, valid value is  0 or 1 or 2

*/
/*============================================================================*/
void a2_per_reset_ul_data_statistics( uint8 endp_idx );

/*==============================================================================

  FUNCTION:  a2_per_reset_dl_data_statistics

==============================================================================*/
/*!
  @brief reset A2 DL data traffic statistics to zero per A2 endpoint


  @arguments:  None

  @return:     None
*/
/*============================================================================*/
void a2_per_reset_dl_data_statistics( void );



/*==============================================================================

  FUNCTION:  a2_reset_wakeup_pkt_info

==============================================================================*/
/*!
  @brief Reset the buffer and start to capture the wakeup packet. also set the
  max length of packet that needs to be captured.

  @arguments:
    max_pkt_buffer_size_in_bytes: 
       max size of fist wakeup IP packet that needs to be captured when 
       function a2_get_wakeup_pkt_info() is called. 0 is NOT a valid value.

  @note:
   'max_pkt_buffer_size_in_bytes' shall be set to non zero value
   
  @return:
      A2_SUCCESS
      A2_ERROR_CAPTURE_WAKEUP_PKT_IN_PROGRESS
      A2_ERROR_FEATURE_NOT_AVAILABLE
*/
/*============================================================================*/
a2_error_e a2_reset_wakeup_pkt_info
(
  uint16 max_pkt_buffer_size_in_bytes
);


/*==============================================================================

  FUNCTION:  a2_get_wakeup_pkt_info

==============================================================================*/
/*!
  @brief Get the wakeup packet that wakes up BB

  @arguments:
   *pkt_buffer_ptr: 
      Pointer to memory (allocated by caller) where up to
      "max_pkt_buffer_size_in_bytes" (set via a2_reset_wakeup_pkt_info())
      of wakeup packet needs to be copied
          
   *total_pkt_length_in_bytes_ptr: 
      point of an uint32 variable to store total wakeup packet length
      in bytes(real length). return value 0 indicates no wakeup packet rcvd.

  @note:
      If no DL packet is received after a2_reset_wakeup_pkt_info() then
      total_pkt_length_in_bytes will be set to zero and function will return
      A2_SUCCESS
 
  @return:
      A2_SUCCESS - '*total_pkt_length_in_bytes_ptr' number of bytes copied to 
                   'pkt_buffer_ptr'
      A2_ERROR_INVALID_FIRST_ARGUMENT - if first argument is NULL
      A2_ERROR_INVALID_SECOND_ARGUMENT - if second argument is NULL
      A2_ERROR_UNCIPHERED_PKT_NOT_AVAILABLE - unciphered pkt is not available,
                   call function again to get unciphered packet
      A2_ERROR_WAKEUP_PKT_NOT_AVAILABLE - wakeup pkt is not available
      A2_ERROR_FAILURE - Failure in A2
      A2_ERROR_FEATURE_NOT_AVAILABLE - not available with current configuration
      A2_ERROR_RESET_API_NOT_CALLED - RESET API was not called yet
*/
/*============================================================================*/
a2_error_e a2_get_wakeup_pkt_info
( 
  uint8  *pkt_buffer_ptr, 
  uint32 *total_pkt_length_in_bytes_ptr 
);


/*==============================================================================

  FUNCTION:  a2_get_wakeup_pkt_stats

==============================================================================*/
/*!
  @brief Get the statistics of wake up packet calls

  Return: 
    Pointer of the structure of statistics related for capturing wakeup packet

*/
/*============================================================================*/
a2_ext_wakeup_pkt_stats_s* a2_get_wakeup_pkt_stats( void );

/*==============================================================================

  FUNCTION:  a2_ext_get_lib_version_addr

==============================================================================*/
/*!
  @brief returns address of a2 library version string (ends with '\0')

  Return: 
   char* : pointer to char string

*/
/*============================================================================*/
char *a2_ext_get_lib_version_addr(void);


#endif /* A2_EXT_H */

