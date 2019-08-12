#ifndef cmlib_h
#define cmlib_h
/*===========================================================================
            COPYRIGHT INFORMATION

Copyright (c) 2001 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.

            EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath:  L:/src/asw/MSM5200/cnm/vcs/cmlib.h_v   1.7   26 Oct 2001 15:36:36   vtawker  $   
$Header: //commercial/MPSS.JO.2.0.c1.4/Main/modem_proc/mmcp/nas/cnm/inc/cmlib.h#1 $    $DateTime: 2016/12/13 08:00:05 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------

08/07/01   CD      Fixed lint errors related to redundant function declarations

08/14/01   AB      Renamed connection manager references to CNM from CM.

24-Oct-2001  VT   DTMF: Added connection_id_T as parameter to the following 
                    function prototypes:
                    CNM_add_to_dtmf_queue() & CNM_pop_dtmf_queue_entry()

26-Oct-2001  VT   DTMF: Moved the definition of dtmf_queue_entry_T to mn_cc.h
                      from this file.

===========================================================================*/

#include "mn_cnmtask.h"
#include "mn_cnmtask_int.h"


/*
 * definitions required for library functions
 */

#define MO_CALL   1
#define MT_CALL   2
#define BI_CALL   3



#define  ALL_CC_TIMERS        0xf0  /* should not match any of the CNM timers */



/*
 * Macro used by CNM_get_next_IE_allowed() and CNM_setup_IE_data_read() and
 * functions which use them, when navigating the IE database
 */

#define GET_FIRST_REF(MsgPtr) (struct MessageIETag *)(MsgPtr)->FirstMessageIE;


/*
 * define used to describe an IE in a particular message as either 
 * optional or mandatory
 */

/* #define OPTIONAL_IE  0
#define MANDATORY_IE 1
#define CONDITIONAL_IE 2 */

/*
 * Definition of structures used in the link lists of data relating to
 * message content
 *
 * Structure MsgDetail is a linked list of all the expected L3 messages
 * (including separater entries for each direction if the message is 
 * different between MO and MT directions)
 * If the corresponding message has and IEs (apart from PD/TI and message 
 * type then the entry FirstMessageIE is the root of a linked list of 
 * MessageIE structures, which identifies all the IEs which can be present
 * in the message in the order of occurrence.
 * These MessageIE structures also have a pointer to the entry in a data 
 * table of IE descriptors, corresponding to the IE.
 * This entry has information on the IEI for the IE, the maximum length,
 * and the format type.
 */

struct MessageIETag
{
   struct  IETypeTag *     IEType;
   byte                    Format;
   boolean                 Mandatory;
   struct MessageIETag *NextMessageIE;
};

struct MsgDetailTag
{
   byte                    message_type;
   byte                    Direction;
   byte                    ImperativeLength;
   struct MsgDetailTag     *NextMsgDetail;
   struct MessageIETag     *FirstMessageIE;
};

struct IETypeTag
{
   byte IEI;
   byte Length;
   byte Type;
};


/*
 * function prototypes
 */

VOID_FUNC CNM_init_dtmf_queue( void );

VOID_FUNC CNM_add_to_dtmf_queue(connection_id_T connection_id,
                                void *);

boolean CNM_pop_dtmf_queue_entry(connection_id_T connection_id,
                                 dtmf_queue_entry_T *);

struct MsgDetailTag   * CNM_setup_IE_data_read(message_type_T message,
                                              byte direction);

boolean CNM_get_next_IE_allowed(byte *IEI, 
                               byte *Type, 
                               byte *Length,
                               byte *Presence, 
                               byte *Format,
                               struct MessageIETag** NextRef);

VOID_FUNC *CNM_alloc( unsigned int );

VOID_FUNC  CNM_free( VOID_DATA *);

/*mn_cnm_cmd_type *mn_cnm_get_cmd_buf(void);
void mn_cnm_put_cmd(mn_cnm_cmd_type *cmd_ptr);
void mn_cnm_timer_expiry(unsigned long param);*/

#endif
