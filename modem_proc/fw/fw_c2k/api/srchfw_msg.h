/*! @mainpage Searcher MDSP Firmware Interface Document
*
* @authors Madhura Sane
* @section Requirements Requirements
*  - Support two (SRCH_NUM_HPQ) high priority queues for
* HDR/1x searches (The queues are not tied to a specific search
* type)
*  - The search/abort/flush command parameters from SW to FW and
* subsequent response parameters from FW-SW are sent via the
* message router. The actual contents of the task queue as well
* as FW results are placed in shared memory.
*  - SW-FW have a prior understanding of shared memory addresses
* where the task parameters coming from SW and results sent from
* FW would be placed for each high-priority queue. Currently,
* the address in shared memory for task queue (read-only from FW
* perspective) and results buffer (write-only from FW
* perspective) are sent back to the SW through HDR enable
* response. 
*  - Queue sizes (SRCH_MAX_HPQ_TASKS) for the two higher
* priority queues HPQ0 and HPQ1 will be 28.
* @section todo To Do 
* - Identify search app enable procedure. (Do we need a separate
*   srch_enable message?. (Currently srch_app is anabled
*   whenever HDR app is enabled)
*
*/
/*!
  @file
  srchfw_msg.h


  Defines the external message interface for the HDRFW module

*/

/*===========================================================================

  Copyright (c) 2008-2012 Qualcomm Technologies Incorporated. All Rights Reserved

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

===========================================================================*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/fw/fw_c2k/api/srchfw_msg.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

#ifndef SRCHFW_MSG_H
#define SRCHFW_MSG_H

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif	

#include "msgr.h"
#include "msgr_umid.h"

#ifdef __cplusplus
} // extern "C"
#endif

/*===========================================================================

                   EXTERNAL DEFINITIONS AND TYPES

===========================================================================*/

/*! \brief The text string for the external SRCHFW Task Queue. */
#define SRCHFW_MSG_Q_NAME           "SRCHFW_Q"

/*! \brief Maximum number of HPQ tasks that can be issued per queue. */
#define SRCH_MAX_HPQ_TASKS             32
/*! \brief Maximum number of LPQ tasks that can be issued per queue. */
#define SRCH_MAX_LPQ_TASKS             16

/*! \brief Maximum number of high priority task queues. */
#define SRCH_NUM_HPQ                   2
/*! \brief Number of sorted peaks in the result for each search task. */
#define NUM_SORTED_PEAKS               6
/*! \brief Maximum search window size for Traffic searches */
#define SRCH_MAX_WINDOW_SIZE_CX1_TRAFFIC   2048
/*! \brief Maximum search window size for IRAT searches */
#define SRCH_MAX_WINDOW_SIZE_CX1_IRAT      1024


#define MSGR_MODULE_SRCH_FW              0x01


#define MSGR_CDMA_SRCH_FW         MSGR_TECH_MODULE( MSGR_TECH_CDMA, MSGR_MODULE_SRCH_FW)


/*! Internal msg queue size of 8 to accomodate SRCH, ABORT, 
    and RESULTS, among others */
#define SRCHFW_NUM_CMDI_MSG 8

/*-----------------------------------------------------------------------*/
/*     SRCHFW Supervisory                                                */
/*-----------------------------------------------------------------------*/

/*! \brief Enumeration of Searcher FW Supervisory commands  */
enum
{
  MSGR_DEFINE_UMID(CDMA, SRCH_FW, SPR, LOOPBACK, MSGR_ID_LOOPBACK,
                   msgr_spr_loopback_struct_type),
  MSGR_DEFINE_UMID(CDMA, SRCH_FW, SPR, LOOPBACK_REPLY, MSGR_ID_LOOPBACK_REPLY,
                   msgr_spr_loopback_reply_struct_type),
};

/*-----------------------------------------------------------------------*/

/*! \brief Supervisory Loopback  - This message is sent by the message router
           and is to be sent back to it in a srchfw_spr_loopback_reply_s to
           confirm that the  message queues are operating correctly. */
typedef msgr_spr_loopback_struct_type       srchfw_spr_loopback_s;

/*! \brief Supervisory Loopback Reply  - This reply is sent back to the
           message router in response to receiving a umbsrch_spr_loopback_s
           message to confirm that the message queues are operating
           correctly. */
typedef msgr_spr_loopback_reply_struct_type srchfw_spr_loopback_reply_s;


/*-----------------------------------------------------------------------*/
/*     SRCHFW COMMAND & RESPONSE UMIDs                                   */
/*-----------------------------------------------------------------------*/

/*! \brief Commands the SRCH FW module receives. */
enum {
  /*! \brief The first message in the enum */
  SRCH_FW_CMD_FIRST = MSGR_UMID_BASE(MSGR_CDMA_SRCH_FW, MSGR_TYPE_CMD),

  MSGR_DEFINE_UMID(CDMA, SRCH_FW, CMD, SRCH_START,    0x00, srchfw_search_cmd_msg_t),
  MSGR_DEFINE_UMID(CDMA, SRCH_FW, CMD, SRCH_ABORT,    0x01, srchfw_abort_cmd_msg_t),
  MSGR_DEFINE_UMID(CDMA, SRCH_FW, CMD, SRCH_OQPCH,    0x02, srchfw_oqpch_cmd_msg_t),
  MSGR_DEFINE_UMID(CDMA, SRCH_FW, CMD, SRCH_SHUTDOWN, 0x03, srchfw_shutdown_cmd_msg_t),
  MSGR_DEFINE_UMID(CDMA, SRCH_FW, CMD, APP_CFG,       0x04, srchfw_app_cfg_cmd_msg_t),
   
  /*! \brief The last message in the enum */
  SRCH_FW_CMD_LAST = CDMA_SRCH_FW_CMD_APP_CFG

};

/*! \brief Responsed the SRCH FW module sends back. */
enum {
  /*! \brief The first message in the enum */
  SRCH_FW_RSP_FIRST = MSGR_UMID_BASE(MSGR_CDMA_SRCH_FW, MSGR_TYPE_RSP),

  MSGR_DEFINE_UMID(CDMA, SRCH_FW, RSP, CDMA_SRCH_RESULTS,  0x00, srchfw_search_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, SRCH_FW, RSP, HDR_SRCH_RESULTS,   0x01, srchfw_search_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, SRCH_FW, RSP, AFLT_SRCH_RESULTS,  0x02, srchfw_search_rsp_msg_t), 
  MSGR_DEFINE_UMID(CDMA, SRCH_FW, RSP, CDMA_SRCH_ABORT,    0x03, srchfw_search_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, SRCH_FW, RSP, HDR_SRCH_ABORT,     0x04, srchfw_search_rsp_msg_t), 
  MSGR_DEFINE_UMID(CDMA, SRCH_FW, RSP, AFLT_SRCH_ABORT,    0x05, srchfw_search_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, SRCH_FW, RSP, OQPCH_SRCH_RESULTS, 0x06, srchfw_oqpch_rsp_msg_t),
  MSGR_DEFINE_UMID(CDMA, SRCH_FW, RSP, APP_CFG,            0x07, srchfw_app_cfg_rsp_msg_t),  
  SRCH_FW_RSP_LAST = CDMA_SRCH_FW_RSP_APP_CFG

};



/*-----------------------------------------------------------------------*/
/*     DATA STRUCTURES FOR SW commands to FW                             */
/*-----------------------------------------------------------------------*/
/**
    \defgroup appCfg App Config Command
    Searcher App Config Command Message Structures. */
/*\{*/
/*! \brief App Config Command
 *
 *   This is the message definition for the searcher app enable command
 */

typedef struct {
  msgr_hdr_struct_type hdr;              /*!< MSGR header */
   /** place holder- currently nothing is passed into this message
  */  
  
} srchfw_app_cfg_cmd_msg_t;
/*\}*/

/**
    \defgroup startsearch Start Search Command
    Searcher Search Command Message Structures. */
/*\{*/

/** Search Type 1x/HDR             */
typedef enum 
{
  CDMA_1X,    /**<  1x search*/
  HDR,       /**<  HDR Search*/
  AFLT,       /**<  AFLT Search*/
  OQPCH       /**< OQPCH Search*/
}srch_type_t;


/** Search Mode             */
typedef enum {
  /*!< Default mode for online searches.  This mode should be
   * used for any search that is strictly not one of the other modes */
  DEFAULT,
  /*!< Mode for HDR search on TRK pilots */
  HDR_SRCH_ON_TRK_PILOTS,
  /*!< Deprecated, please use OFFLINE_SRCH */
  IRAT_SRCH,
  /*!< Mode for all offline searches.  Sample stop RTC
   * should be provided in the search command (cx2_rtc) */
  OFFLINE_SRCH = IRAT_SRCH,
  /*!< Mode for searches during inactivity period between
   * another tech's tx bursts.  Searcher will freeze the srch
   * write pointer to search over a specific group of samples */
  DSDA_OFFLINE
} csrch_mode_e;


/*! \brief Search Command
 *
 *   This is the message definition for the SrchFW Search Command
 *          to issue a queue of search tasks. Completion of the command
 *          will generate Response with results from SrchFW */
typedef struct {
  msgr_hdr_struct_type hdr;                /*!< MSGR header */

  srch_type_t   search_type;                /*!< Search type: 1x, HDR */ 
  boolean       peak_comp_dis;             /*!< Flag to indicate whether peak energies compensation 
                                                by extrapolation using shoulder energies needs to 
                                                be disabled(HDR only)*/
  boolean       peak_norm_dis;             /*!< Flag to indicate whether peak energies normalization 
                                                needs to be disabled (HDR only)*/
  /** Search queue number (0,., SRCH_NUM_HPQ)
   *  - FW knows the corresponding address in shared memory where SW places srch task parameters,
   *  in the format as per srchfw_search_hdr_params_t or
   *  srchfw_search_1x_params_t depending on the search type
   *  - The shared memory buffer needs to be of type
   *    srchfw_task_params_queue_u array of size =
   *    [SRCH_MAX_HPQ_TASKS] for each high priority queue
  */
  uint16        queue;                     
  uint16        num_tasks;                  /*!< Number of search tasks */
  uint16        priority;                  /*!< Search queue priority (Valid priority values are +ve; 1-lowest, 32767-highest)*/
  uint16        freq_offset;                /*!< Frequency offset */ 

  csrch_mode_e  srch_mode;                  /*!< Mode for FW search operation */
  uint16        cx2_rtc;                    /*!< Value for searcher fw to use for
                                          offline wr_ptr during IRAT or TRK searches*/

  uint32        dsda_inactivity_start_ustmr;/*!< Current DSDA Inactivity Start Time */
  uint32        dsda_inactivity_end_ustmr;  /*!< Current DSDA Inactivity End Time */
  uint32        max_sample_reqd_cx1;        /*!< Maximum no. of samples to be 
                                          captured before starting the search */

} srchfw_search_cmd_msg_t;

/*! \brief Search Command
 *
 *   This is the message definition for the SrchFW OQPCH Search Command.
 *          Completion of the command
 *          will generate Response with results from SrchFW */

typedef struct {
  msgr_hdr_struct_type hdr;                /*!< MSGR header */
  
  uint16   queueNum;                       /*!< HPQ# */
  uint16   indLength;                      /*!< # of chips the indicator symbol is spread over */
  uint16   indPosition;                    /*!< The position of the indicator bit in a QPCH frame */
  int16    freqEst;                        /*!< freq estimate*/
  uint16   filtLength;                     /*!< Length of the filter used in computing filtered pilot */
  uint16   pilotIndex;                     /*!< Pilot PN sequence offset in units of 64 chips */
  uint16   walsh;                          /*!< Walsh function */
  uint16   sampRamWritePtr;                /*!< Sample RAM rewind */
  uint16   numPaths;                       /*!< # of multipaths in which QPCH symbol is demodulated */
  int16    pathPosCx1[6];                  /*!< PN position of path k with respect to the CDMA system time */
  int16    pathPhase[6];                   /*!< Used to get interpolation coefficients and determine the 
                                                position of where to get searcher results for generating 
                                                interpolated symbols from DMAed results for path k. 
                                                It is computed as (reacqSrchPeakPosCx8 & 0x7) */
  int16    diversityMode;                  /*!< ant0/1 diversity*/
  uint16   carrierSelect;                  /*!< carrier0/1/2/3*/ 

} srchfw_oqpch_cmd_msg_t;

/*! \brief Search Params
 *
 *   This is the message definition for the SrchFW Search Command
 *          to issue a queue of search tasks. Completion of the command
 *          will generate Response with results from SrchFW */

/** HDR search parameters */
typedef struct {
  /**   Bit-packing for halfword 0
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     |                               Window Size                                     |  
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     \endverbatim
  */

  uint16 windowSize:16;
  /*!< The window size specifies the size of the search area in chips over a PN range of 0 - 32767. The maximum size of the search window is 32767 chips. Six peaks are reported per window, therefore the minimum search window size is 6 chips.*/

  /**   Bit-packing for halfword 1
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     |    |                               PN Position                                |     
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     \endverbatim
  */
  uint16 pnPos:15;
  /*!< The PN position specifies the starting point of the search window in chip resolution and may range over the entire PN circle from 0 - 32767. */
  uint16 halfword_1_unused:1; 
  /*!< Padding with unused bits to align to 16 bit boundaries. */
  /**   Bit-packing for halfword 2
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     |   carrier select  |                   Coherent Length                         |     
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     \endverbatim
  */
  uint16 cohLen:12;                            
  /*!< The coherent integration length is specified in chips and typically ranges from 64 to 2048 chips. This interface supports integration lengths of up to 8191 chips, however, this is a function of sample ram size therefore additional constraints on the integration length may be required. */
  uint16 carrierSelect:4;
  /*!< Carrier selection, corresponding to a particular bank of sample server. For MSMs that do not support multicarrier operation, this field shall be set to 0.*/
  /**   Bit-packing for halfword 3
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     |             Pilot Index                    |              |Burst Len|NonCo Len|     
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     \endverbatim
  */
  uint16 nonCohLen:2;                         
  /*!< The non-coherent integration length is specified in terms of the number of coherent integrations to be summed together. This interface supports non-coherent integration lengths of up to 127 for CDMA searches.  Valid non-coherent integration length for HDR searches is 1 to 2. */
  uint16 burstLength:2;                             
  /*!< The HDR burst length specifies the number of pilot bursts to search.  Valid values are 1 and 2 for one and two bursts. */
  uint16 halfword_3_unused:3; 
  /*!< Padding with unused bits to align to 16 bit boundaries. */
  uint16 pilotIndex:9;                        
  /*!< The pilot index specifies a particular base station's pilot offset and is represented in terms of 64 chip units. The allowable range for the pilot index is from 0 - 511. */
  /**   Bit-packing for halfword 4
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     |Diversity|   Coh Trunc  |   QOF   |             PilotWalsh                     |      
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     \endverbatim
  */


  uint16 walshIndex:9;                        
  /*!< This field specifies the Walsh Channel Index for the current search task and is changeable on a per task basis. */     
  uint16 qof:2;                               
  /*!< This field specifies the Quasi-Orthogonal Function (QOF) Index for the current search task and is changeable on a per task basis. */
    /**   Table for coherent truncation
      \verbatim
      Coh Trunc |	Coherent round |	Non coherent | 	Equivalent Dora | 	Equivalent 
        Value   |     Value        |  truncation     |   Coh Trunc      |  Searcher2 gain
    +===========+==================+=================+==================+================
        000	    |       1	       |    1 bit	     |        101	    |       Na
        001	    |       2	       |    2 bits	     |        110	    |       16
        010	    |       4	       |    3 bits	     |        111	    |        8
        011	    |       8	       |    4 bits	     |        000	    |        4
        100	    |      16	       |    5 bits	     |        001	    |        2
        101     |      32	       |    6 bits	     |        010	    |       Na
        110	    |      64	       |    7 bits	     |        011	    |       Na
        111	    |     128	       |    8 bits	     |        100	    |       Na             
     \endverbatim
  */
  uint16 cohTrunc:3;                          
  /*!< This field specifies the coherent truncation control parameter which controls where the coherent accumulation is to be truncated/rounded prior to saturating for the non-coherent 8 bit squaring operation.  */
  uint16 diversity:2;                         
  /*!< This field specifies the receive path of samples used for searching.  In the dual antenna mode, non-coherent integration energies from both antennas are combined.  */
} srchfw_search_hdr_params_t;

/** 1x search parameters */
typedef struct {
  /**   Bit-packing for halfword 0
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     |                               Window Size                                     |  
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     \endverbatim
  */

  uint16 windowSize:16;
  /*!< The window size specifies the size of the search area in chips over a PN range of 0 - 32767. The maximum size of the search window is 32767 chips. Six peaks are reported per window, therefore the minimum search window size is 6 chips.*/

  /**   Bit-packing for halfword 1
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     |    |                               PN Position                                |     
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     \endverbatim
  */
  uint16 pnPos:15;
  /*!< The PN position specifies the starting point of the search window in chip resolution and may range over the entire PN circle from 0 - 32767. */
  uint16 halfword_1_unused:1; 
  /*!< Padding with unused bits to align to 16 bit boundaries. */
  /**   Bit-packing for halfword 2
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     |   carrier select  |                   Coherent Length                         |     
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     \endverbatim
  */
  uint16 cohLen:12;                            
  /*!< The coherent integration length is specified in chips and typically ranges from 64 to 2048 chips. This interface supports integration lengths of up to 8191 chips, however, this is a function of sample ram size therefore additional constraints on the integration length may be required. */
  uint16 carrierSelect:4;
  /*!< Carrier selection, corresponding to a particular bank of sample server. For MSMs that do not support multicarrier operation, this field shall be set to 0.*/
  /**   Bit-packing for halfword 3
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     |             Pilot Index                    |            NonCo Len             |     
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     \endverbatim
  */

  uint16 nonCohLen:7;                         
  /*!< The non-coherent integration length is specified in terms of the number of coherent integrations to be summed together. This interface supports non-coherent integration lengths of up to 127 for CDMA searches.  Valid non-coherent integration length for HDR searches is 1 to 2. */
  uint16 pilotIndex:9;                        
  /*!< The pilot index specifies a particular base station's pilot offset and is represented in terms of 64 chip units. The allowable range for the pilot index is from 0 - 511. */
  /**   Bit-packing for halfword 4
      \verbatim
        15   14   13   12   11   10    9    8    7    6    5    4    3    2    1    0
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     |Diversity|   Coh Trunc  |   QOF   |             PilotWalsh                     |      
     +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+
     \endverbatim
  */
  uint16 walshIndex:9;                        
  /*!< This field specifies the Walsh Channel Index for the current search task and is changeable on a per task basis. */     
  uint16 qof:2;                               
  /*!< This field specifies the Quasi-Orthogonal Function (QOF) Index for the current search task and is changeable on a per task basis. */

  /**   Table for coherent truncation
      \verbatim
      Coh Trunc |	Coherent round |	Non coherent | 	Equivalent Dora | 	Equivalent 
        Value   |     Value        |  truncation     |   Coh Trunc      |  Searcher2 gain
    +===========+==================+=================+==================+================
        000	    |       1	       |    1 bit	     |        101	    |       Na
        001	    |       2	       |    2 bits	     |        110	    |       16
        010	    |       4	       |    3 bits	     |        111	    |        8
        011	    |       8	       |    4 bits	     |        000	    |        4
        100	    |      16	       |    5 bits	     |        001	    |        2
        101     |      32	       |    6 bits	     |        010	    |       Na
        110	    |      64	       |    7 bits	     |        011	    |       Na
        111	    |     128	       |    8 bits	     |        100	    |       Na             
     \endverbatim
  */
  uint16 cohTrunc:3;                          
  /*!< This field specifies the coherent truncation control parameter which controls where the coherent accumulation is to be truncated/rounded prior to saturating for the non-coherent 8 bit squaring operation.  */
  
  uint16 diversity:2;                         
  /*!< This field specifies the receive path of samples used for searching.  In the dual antenna mode, non-coherent integration energies from both antennas are combined.  */
} srchfw_search_1x_params_t;
 
/** Diversity ant 0/1/both            */
typedef enum 
{
  ANT_0,       /**<  00: diversity antenna 0*/
  ANT_1,       /**<  01: diversity antenna 1*/
  DUAL_ANT     /**<  10: diversity dual antenna*/
}diversity_t;
/*\}*/

/**
*   \defgroup abort Abort Command
   Searcher Abort Command Message Structures */
/*\{*/
/*! \brief Abort Command parameters
 *
 *   This is the message definition for the SrchFW Abort Command
 *          to indicate search tasks on a high priority queue
 *          specified in the message needs to be aborted. 
 */

typedef struct {
  msgr_hdr_struct_type hdr;              /*!< MSGR header */
   /** Search queue number (0,..., SRCH_NUM_HPQ)
   *   - FW knows the corresponding location of task parameters
   *     which were used to trigger search on that particular
   *     queue.
  */
  uint16 queue; 
  srch_type_t search_type;                         
  
} srchfw_abort_cmd_msg_t;
/*\}*/


/**
*   \defgroup Searcher App Shutdown Command
   Searcher App Shutdown Command Message Structures */
/*\{*/
/*! \brief App Shutdown parameters
 *
 *   This is the message definition for the SrchFW Abort Command
 *          to terminate the message processing thread.
 */

typedef struct {
  msgr_hdr_struct_type hdr;              /*!< MSGR header */
} srchfw_shutdown_cmd_msg_t;
/*\}*/

/*-----------------------------------------------------------------------*/
/*     DATA STRUCTURES FOR SRCHFW RESPONSES                              */
/*-----------------------------------------------------------------------*/
/**
    \defgroup searchresults Result Response
   Search Response Message Structures. */
/*\{*/

/*! \brief Search Results
 *   This is the message definition for the response FW send back to SW upon completing 
 *          the HW search and results post-processing.
 */
typedef struct {
  uint16 peakPos;
  /*!< Peak Position, Cx2 resolution, absolute PN position */
  uint16 peakMag;
  /*!< Peak Magnitude*/
  uint16 peakLeft;
  /*!< Peak Left (Early) Shoulder Magnitude */
  uint16 peakRight;
  /*!< Peak Right (Late) Shoulder Magnitude */
} srchfw_search_results_t;


/*! \brief Search Results Response
 *   This is the message definition for FW response to a start_search command from SW.
 *   FW sends this response upon completition of HW search and post-processing of results 
 *   for 1x/ HDR search. There are two different UMID for 1x and HDR search responses 
 *   as two different SW entities need to register for those. But they follow the same 
 *   response parameters.
 */
typedef struct {
  msgr_hdr_struct_type hdr; /*!< MSGR header */
  uint16 numResults;        /*!< Total Number of host tasks completed */
  /** Search queue sequence number. (0,..., SRCH_NUM_HPQ)
  *    - SW knows the corresponding address in shared memory where FW
  *      places the results, in the format as per 
  *      srchfw_search_results_t 
  *    - The shared memory location needs to have allocated memory 
  *      enough for a 2-D array of type srchfw_search_results_t and of 
  *      size [SRCH_MAX_HPQ_TASKS][NUM_SORTED_PEAKS]
  */
  uint16 queue;             /*!< Search queue sequence number. (0,., SRCH_NUM_HPQ) */  
  srch_type_t search_type; /*!< CDMA_1X, HDR, AFLT, OQPCH */

  /*!< Validity of each task in the search (used only in DSDA mode)
   * NOTE: numResults indicates the no. of tasks scheduled in actual
   *       and hence the taskValid status for other tasks are not applicable
   *       and remain invalid
   */
  boolean taskValid[SRCH_MAX_HPQ_TASKS];

  /** XXX: srchStartUstmr and srchEndUstmr are no more valid in single task scheduling
   *       design and should be removed from the API once the SRCH4 changes for DSDA mode
   *       are checked in.
   */
  uint32 srchStartUstmr; /*!< USTMR time stamp when search was issued to HW */
  uint32 srchEndUstmr;  /*!< USTMR time stamp when search completed */                        
} srchfw_search_rsp_msg_t;


typedef struct {
  msgr_hdr_struct_type hdr;     /*!< MSGR header */
  int16 freqEst;                /*!< Freq estimate */ 
  int16 pathFreqOffset[6];      /*!< Frequency offset which (when corrected for)
                                     yields maximum pilot coherent accumulation energy */  
  int32 sumSoftI;               /*!< QPCHind I component */
  int32 sumSoftQ;               /*!< QPCHind Q component */
  int32 sumTh1;                 /*!< Pilot threshold */
  int16 softIResult;            /*!< Scaled QPCHind I component */
  int16 softQResult;            /*!< Scaled PCHind Q component */
  int16 th1Result;              /*!< Scaled Pilot threshold */

} srchfw_oqpch_rsp_msg_t;
/*\}*/


/**
*   \defgroup appenableresponse App Config Response
*      App Config Response Message Structures. */
/*\{*/
/*! \brief App Config Response
 * This is a message definition for the app_cfg response to be sent to SW to 
 * indicate to SW that search APP has been enabled and also to let SW know the shared
 * memory address for search in TCM where it can transfer microCmd task queues as well
 * as find FW search results.
 */

typedef struct {
  msgr_hdr_struct_type hdr;                               /*!< MSGR header */
  /** Current status of the SRCH app. If SRCH is disabled, the
   *  shared memory addresses included in this message should be
   *  ignored. */
  uint16                    statusEnabled:1;
  /** Physical address of the shared memory interface. \sa srchfw_sw_shared_memory_interface_t */
  uint32                    sharedMemoryAddr;   
                  
}srchfw_app_cfg_rsp_msg_t;
/*\}*/


/**
    \defgroup unions Message Unions
    Unions of message structures for MSGR receive buffer declaration. */
/*\{*/


/*! \brief Union of all message received by firmware */
typedef union {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  srchfw_search_cmd_msg_t       search;
  srchfw_abort_cmd_msg_t        abort;
  srchfw_oqpch_cmd_msg_t        oqpch;
  srchfw_shutdown_cmd_msg_t     shutdown;
  srchfw_app_cfg_cmd_msg_t      app_cfg;
}srchfw_msg_cmd_u;

/*! \brief Union of all response messages recevied by software  */
  
typedef union {
  /*! \brief Message header */
  msgr_hdr_struct_type      hdr;
  srchfw_search_rsp_msg_t       search;
  srchfw_oqpch_rsp_msg_t        oqpch;
  srchfw_app_cfg_rsp_msg_t      app_cfg;
}srchfw_msg_rsp_u;

/*\}*/
/**
    \defgroup smem Shared Memory Interfaces */
/*\{*/
/*! \brief Union containing the search task queue with maximum allowable number of tasks for each HPQ
    CDMA/HDR searches*/
typedef union {
  srchfw_search_hdr_params_t hdr_srch_task_params;
  srchfw_search_1x_params_t  cdma_srch_task_params;
  srchfw_search_1x_params_t  aflt_srch_task_params;
}srchfw_task_params_queue_u;

/*! \brief Shared Memory Interface Structure. This structure
 *  contains the buffer for
 *    - SW search task queue parameters
 *    - FW post-processed results buffer. */

typedef struct {
  
  /** Results buffer for FW post-processed results */
  srchfw_search_results_t srch_results[SRCH_NUM_HPQ][SRCH_MAX_HPQ_TASKS][NUM_SORTED_PEAKS];
  /** Search task parameters passed on from SW for each HPQ */
  srchfw_task_params_queue_u task_params[SRCH_NUM_HPQ][SRCH_MAX_HPQ_TASKS];

  srchfw_search_results_t aflt_srch_results[SRCH_MAX_LPQ_TASKS][NUM_SORTED_PEAKS];
  srchfw_task_params_queue_u aflt_task_params[SRCH_MAX_LPQ_TASKS];
} srchfw_sw_shared_memory_interface_t;

/*\}*/
#define SRCHFW_MAX_MSG_SIZE        sizeof(srchfw_msg_cmd_u)
#define SRCHFW_MAX_RSP_MSG_SIZE    sizeof(hdrfw_msg_rsp_u)

#define SRCHFW_NUM_CMD_MSG       (SRCH_FW_CMD_LAST - SRCH_FW_CMD_FIRST + 1)
#define SRCHFW_NUM_RSP_MSG       (SRCH_FW_RSP_LAST - SRCH_FW_RSP_FIRST + 1)
#endif /* SRCHFW_MSG_H */
