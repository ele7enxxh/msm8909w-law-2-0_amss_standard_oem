#ifndef QDSSLITE_H
#define QDSSLITE_H

/*=============================================================================

FILE:         QDSSLite.h

DESCRIPTION:  This file has the QDSS Lite API intented for use with
              memory constrained systems.

================================================================================
                Copyright 2011 Qualcomm Technologies Incorporated.
                         All Rights Reserved.
                QUALCOMM Proprietary and Confidential
==============================================================================*/

#include <DALSysTypes.h>

/**
 * Initialize QDSS drivers and hardware
 *
 * This function initializes all the components of the QDSS sub-system
 * Should be called before other functions.
 *
 * @return None
 */
void QDSSInit(void);

/**
 * Enables QDSS trace on RPM
 *
 * This function votes the QDSS clocks ON and enables QDSS trace output.
 * If trace is already enabled the function will return DAL_SUCCESS without
 * making any changes.
 *
 * @return  DAL_SUCCESS on success,
 *          otherwise an error code indicating the failure.
 */
int QDSSEnableTrace(void);


/**
 * Disables QDSS trace on RPM
 *
 * This function disables QDSS trace output and votes QDSS clocks OFF.
 * If trace is already disabled the function will return DAL_SUCCESS without
 * making any changes.
 *
 * @return  DAL_SUCCESS on success,
 *          otherwise an error code indicating the failure.
 */
int QDSSDisableTrace(void);


/**
 * Allocate an STM stimulus port
 *
 * This function allocates a dedicated STM stimulus port for the caller to 
 * ouptut trace data.
 *
 * @param pPort OUT parameter for returning the port.
 *              On success, the location pointed to by pPort will have the
 *              newly allocated port.
 *
 * @return  DAL_SUCCESS on success,
 *          otherwise an error code indicating the failure.
 */
int QDSSAllocPort(uint32 *pPort);


/**
 * Free a previously allocated STM stimulus port
 *
 * This function releases a  STM stimulus port previously allocated for 
 * the caller
 *
 * @param port the port to free.
 *
 * @return  DAL_SUCCESS on success,
 *          otherwise an error code indicating the failure.
 */
int QDSSFreePort(uint32 port);

/**
 * Flush the QDSS trace data pipes
 *
 * This function does a manual flush of the QDSS trace sink, which in turn
 * results in flushing the trace links and trace sources up the stream
 *
 * @return  DAL_SUCCESS on success,
 *          otherwise an error code indicating the failure.
 */

int QDSSFlush(void);

/**
 * Output a software event
 *
 * This function outputs a simple software event without parameters
 * to the trace stream.
 *
 * @param port The port to output the event on.
 * @param event_id A unique id for the event
 *
 * @return  DAL_SUCCESS on success,
 *          otherwise an error code indicating the failure.
 */
int QDSSEvent(uint32 port, int event_id);


/**
 * Write an OST data packet
 *
 * This function outputs data to the trace stream,
 * encapsulated in a MIPI OST packet. Default values are
 * used for Entity ID(2) and protocol ID(0)
 *
 * @param port The port to output data on.
 * @param ost_header OST Header formed with the QDSSOSTHeader below
 * @param data A pointer to the start of the data buffer
 * @param len   Length of the data buffer.
 *
 * @return  DAL_SUCCESS on success,
 *          otherwise an error code indicating the failure.
 */
int QDSSWriteOSTPacket(uint32 port, uint32 ost_header, byte *data, int nLen);


/**
 * Make an OST Header with the given parameters
 *
 * This is a helper function for use with QDSSWriteOSTPacket
 *
 * @param entity   OST entity field
 * @param protocol OST protocol field
 *
 * @return  A 32 bit OST packet header
 */

static __inline uint32 QDSSOSTHeader(byte entity,byte protocol)
{
   #define QDSS_OST_STARTSIMPLE  0x10
   #define QDSS_OST_VERSION      0x10


   return ( QDSS_OST_STARTSIMPLE << 24) |
      (QDSS_OST_VERSION << 16) | (entity << 8) | protocol; 
}

#define QDSS_OST_ENTITY_ULOG    11



/**
 * Sets a new value in the STM timstamp frequency register.
 *
 * This function is called when the QDSS timestamp clock 
 * frequency changes.
 *
 * @param dwFreq New frequency value to be set.
 */
void QDSSSTMSetTimestampFreq(uint32 dwFreq);


/**
 * This function is to be called before XO shutdown
 *
 * It does the following
 *    - Saves the QDSS timestamp counter and MPM ARM spec counter values
 *
 */
void QDSSPreXOShutdown(void);

/**
 * This function is to be called while coming back from an XO shutdown state
 *
 * It does the following
 *     - Restores the QDSS timestamp counter based on the MPM spec counter 
 */
void QDSSPostXOShutdown(void);


/**
 * This function is to be called in the XO shudtdown sequence before 
 *  DDR goes to self-refresh mode
 *
 * It does the following
 *    - If currently configured trace sink is DDR, disables trace capture
 *      to prevent QDSS writes to DDR 
 * 
 * If the current trace sink is not DDR, this function has no effect.
 *
 */
void QDSSDDRTraceDisable(void);

/**
 * This function is to be called while coming back from an XO shutdown state
 * after DDR exits self refresh mode
 *
 * It does the following
 *     - If DDR trace was disabled during XO shutown, it enables it back 
 * 
 * If the current trace sink is not DDR this function has no effect.
 */
void QDSSDDRTraceEnable(void);


/**
 * This function called to control ETM tracing on RPM
 *
 *   @param option   An ETM option to set
 *   @param value    Value to set for the ETM option
 * 
 */
int QDSSETMControl(uint32 option, uint64 value); 


#endif //QDSSLITE_H
