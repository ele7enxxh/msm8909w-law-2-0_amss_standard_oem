#ifndef QURT_TIMER_H
#define QURT_TIMER_H
/*=============================================================================

                 qurt_timer.h -- H E A D E R  F I L E

GENERAL DESCRIPTION
   Prototypes of qurt_timer API 

EXTERNAL FUNCTIONS
   None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2010 - 2011
                    by Qualcomm Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.mpss/3.5.c12.3/kernel/qurt_mba/install/MBAv5/include/qurt_timer.h#1 $ 
$DateTime: 2016/12/13 07:59:23 $ $Author: mplcsds1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/03/11   op      Add header file, Fix warning when running doxygen 
=============================================================================*/

#include "qurt_anysignal.h"
#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================
                        CONSTANTS AND MACROS
=============================================================================*/

/**
 Define magic                                                          */
#define QURT_TIMER_MAGIC                   0xadadadad
#define QURT_TIMER_EXPIRED                 0xedededed
#define QURT_TIMER_INVALID                 0xdfdfdfdf
#define QURT_TIMER_ATTR_MAGIC              0xaeaeaeae
#define QURT_TIMER_ATTR_INVALID            QURT_TIMER_INVALID

/**
 Default Values                                                            
*/
#define QURT_TIMER_DEFAULT_TYPE            QURT_TIMER_ONESHOT
#define QURT_TIMER_DEFAULT_DURATION        1000uL
#define QURT_TIMER_DEFAULT_EXPIRY          0uL

/**
 Conversion from micor seconds to Sleep ticks
 */
#define QURT_TIMER_TIMETICK_FROM_US(us) qurt_timer_timetick_from_us(us)

/**
 Conversion from sleep clock ticks to microseconds at the nominal frequency 
*/
#define QURT_TIMER_TIMETICK_TO_US(ticks) qurt_timer_timetick_to_us(ticks)

/**
 MINIMUM Microseconds Value = 100 Microseconds (Sleep timer)               
*/
#define QURT_TIMER_MIN_DURATION             100uL

/**
  For RGPT: MAXIMUM Microseconds Value = 36 hours                                     
  For Qtimer: MAXIMUM Microseconds Value = 1042499 hours
*/
#define QURT_TIMER_MAX_DURATION  qurt_timer_max_duration()

/** 
 For RGPT: Sleep clock is 32768 Hz
 For Qtimer: Timer clock is 19.2MHz
*/
#define QURT_TIMER_MAX_DURATION_TICKS qurt_timer_max_duration_ticks()

/** 
 For RGPT: Sleep Timer Error Margin, setup time (2 ticks) + resolution (1 tick) ~90us
 For Qtimer: Sleep Timer Error Margin, 1000 ticks ~52us 
*/
#define QURT_TIMETICK_ERROR_MARGIN qurt_timetick_error_magin()

/** 
  qurt_timer group defines                                                    
*/
#define QURT_TIMER_MAX_GROUPS              5
#define QURT_TIMER_DEFAULT_GROUP           0

/**
 * qurt_timer_type types                                                       
 */
typedef enum
{
  QURT_TIMER_ONESHOT = 0,
  QURT_TIMER_PERIODIC
} qurt_timer_type_t;


/*=============================================================================
                        TYPEDEFS
=============================================================================*/
/**
 * qurt_timer_t types                                                           
 */
typedef unsigned int                        qurt_timer_t;

/**
 * qurt_timer_duration_t types                                                 
 */
typedef unsigned long long                  qurt_timer_duration_t;

/**
 * qurt_timer_time_t types                                                 
 */
typedef unsigned long long                  qurt_timer_time_t;

/**
 * qurt_timer_attr_t types                                                      
 */
typedef struct 
{
    /**
     * Magic number to verify qmsgq_attr_t pointer 
     */
    unsigned int        magic;
    
    /**
     * This attribute specifies the duration of new timer. 
     */ 
    qurt_timer_duration_t   duration; 

    /**
     * This attribute specifies the absolute expiry of new timer. 
     */ 
    qurt_timer_time_t   expiry; 

    /**
     * This attribute specifies the remaining time of an active timer. 
     */ 
    qurt_timer_duration_t   remaining; 

    /**
     * This attribute specifies the timer type, only QURT_TIMER_ONESHOT and
     * QURT_TIMER_PERIODIC are currently supported 
     */
    qurt_timer_type_t       type;

    /**
     * Group no of the timer, the criterion used to disable or enable the set
     * of timers.
     */
    unsigned int        group;
}
qurt_timer_attr_t;

/*=============================================================================
                        FUNCTIONS
=============================================================================*/

/**
 * Stop a one-shot timer  
 *
 * @param timer        Timer ID. 
 * @return             EOK:        Successful stop,
 * @return             EINVALID:   Invalid timer ID or duration value
 * @return             ENOTALLOWED: Timer is not a oneshot timer.
 * @return             EMEM:        Out of memory error,
 */
int qurt_timer_stop (qurt_timer_t timer);



/**
 * Restart a one-shot timer with a duration. 
 *
 * @param timer        Timer ID. 
 * @param duration     Timer duration.
 * @return             EOK:        Successful restart,
 * @return             EINVALID:   Invalid timer ID or duration value
 * @return             ENOTALLOWED: Timer is not a oneshot timer.
 * @return             EMEM:        Out of memory error,
 */
int qurt_timer_restart (qurt_timer_t timer, qurt_timer_duration_t duration);


/**
 * Creates a timer with specific attributes
 *
 * @param timer  [OUT] Timer object
 * @param attr         Specifies timer duration and timer type (
 *                     one-shot or periodic)
 * @param signal       Signal sent when timer expires
 * @param mask         Signal mask to be sent when timer expires
 * @return             EOK:       Successful create,
 * @return             EMEM:      Out of memory error,
 */
int qurt_timer_create (qurt_timer_t *timer, const qurt_timer_attr_t *attr,
                  const qurt_anysignal_t *signal, unsigned int mask);

/**
 * Initialize timer attributes
 *
 * @param attr  [OUT] Timer attributes object
 */
void qurt_timer_attr_init(qurt_timer_attr_t *attr);

/**
 * Set duration
 *
 * @param attr      Timer attributes object
 * @param duration  Duration for the timer
 */
void qurt_timer_attr_set_duration(qurt_timer_attr_t *attr, qurt_timer_duration_t duration);

/**
 * Set expiry
 *
 * @param attr      Timer attributes object
 * @param time      Absolute expiry time for the timer in micro-seconds
 */
void qurt_timer_attr_set_expiry(qurt_timer_attr_t *attr, qurt_timer_time_t time);

/**
 * Get duration
 *
 * @param attr      Timer attributes object
 * @param duration  Duration for the timer
 */
void qurt_timer_attr_get_duration(qurt_timer_attr_t *attr, qurt_timer_duration_t *duration);

/**
 * Get remaining time
 *
 * @param attr      Timer attributes object
 * @param remaining    Remaining time for the timer
 */
void qurt_timer_attr_get_remaining(qurt_timer_attr_t *attr, qurt_timer_duration_t *remaining);

/**
 * Set timer type - one shot or periodic
 *
 * @param attr   Timer attributes object
 * @param type   Timer type
 */
void qurt_timer_attr_set_type(qurt_timer_attr_t *attr, qurt_timer_type_t type);

/**
 * Set duration
 *
 * @param attr  Timer attributes object
 * @param type  Timer type
 */
void qurt_timer_attr_get_type(qurt_timer_attr_t *attr, qurt_timer_type_t *type);

/**
 * Set timer group, ranging 0 to QURT_TIMER_MAX_GROUPS - 1
 *
 * @param attr   Timer attributes object
 * @param group  Group ID
 */
void qurt_timer_attr_set_group(qurt_timer_attr_t *attr, unsigned int group);

/**
 * Get the group ID from the attribute
 *
 * @param attr  Timer attributes object
 * @param group  Timer group
 */
void qurt_timer_attr_get_group(qurt_timer_attr_t *attr, unsigned int *group);

/**
 * Get attributes of a message queue
 *
 * @param timer        Pointer to timer object
 * @param attr  [OUT]  Message queue attributes
 * @return             EOK:       get_attr successful,
 * @return             EVAL:  Wrong parameters,
 */
int qurt_timer_get_attr(qurt_timer_t timer, qurt_timer_attr_t *attr);

/**
 * Deletes timer
 *
 * @param timer  Pointer to timer object
 * @return       EOK:       Successful create,
 * @return       EVAL:  Wrong timer 
 */
int qurt_timer_delete(qurt_timer_t timer);

/**
 * Sleep for specified duration.
 *  
 * @param duration  number of us to sleep for 
 * @return             EOK:       Successful create,
 * @return             EMEM:      Out of memory error,
 */

int qurt_timer_sleep(qurt_timer_duration_t duration);

/**
 * Disable the timers that belong to the specified group. Timers
 * don't get processed with a timer interrupt, once they are disabled.
 * 
 * NOTE:
 * ----
 * This API is expected to be called by Power Management SW under
 * Single Threaded Mode, wheren only one HW thread runs. Otherwise,
 * it can be delays in response from Timer Server.
 *
 * @param group        The group of timers need to be disabled
 * @return             EOK:       successful
 */
int qurt_timer_group_disable (unsigned int group);

/**
 * Enable the timers that belong to the specified group, which are disabled. If
 * no timers were already disabled, do nothing. Process the timers which are
 * already expired.
 *
 * @param group        The group of timers need to be enabled
 * @return             EOK:       successful
 */
int qurt_timer_group_enable (unsigned int group);

/**
 * Communicate the timer server that we are recovering from Power Collapse. The server
 * has to account for any missed interrupts durint power collapse etc.
 *
 */
void qurt_timer_recover_pc (void);

/**
 * To find out if Qtimer is initialized.
 * 0        - not initialized.
 * non-zero - initialized.
 */
int qurt_timer_is_init (void);

/**
 * Get current ticks. The ticks are accumulated since the RTOS is started. Each tick
 * is equal to a single sleep clock cycle, where sleep clock runs at 32 KHz.
 *
 * @return             ticks since system started
 */
unsigned long long qurt_timer_get_ticks (void);


unsigned long long qurt_timer_timetick_from_us(unsigned long long us);
unsigned long long qurt_timer_timetick_to_us(unsigned long long ticks);
unsigned long long qurt_timer_max_duration(void);
unsigned long long qurt_timer_max_duration_ticks(void);
unsigned int qurt_timetick_error_magin(void);


#ifdef __cplusplus
} /* closing brace for extern "C" */
#endif

#endif /* QURT_TIMER_H */
