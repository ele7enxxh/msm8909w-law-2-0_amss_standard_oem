#include <stdlib.h>

#include "comdef.h"
#include "rpm_settling_timer.h"
#include "time_service.h"
#include "rpm_config.h"
#include "rpmserver.h"
#include "rpm_resources.h"
#include "rpm_hwio.h"
#include "rpm_handler.h"

#include "cortex-m3.h"

#define QTMR_IRQ 51

#include "CoreVerify.h"

static settling_node *settling_list;
static settling_node *settling_empty;

static void set_timer(void)
{
    uint32 lo, hi;
    lo = (uint32)settling_list->settling_time;
    hi = settling_list->settling_time >> 32;

    HWIO_OUT(RPM_F1_QTMR_V1_CNTP_CTL, 0);
    HWIO_OUT(RPM_F1_QTMR_V1_CNTP_CVAL_LO, lo);
    HWIO_OUT(RPM_F1_QTMR_V1_CNTP_CVAL_HI, hi);
    HWIO_OUT(RPM_F1_QTMR_V1_CNTP_CTL, 1);
}

static void timer_list_pop(void)
{
    if(NULL == settling_list)
        return;

    settling_node *newly_empty = settling_list;
    settling_list = settling_list->next;

    newly_empty->next = settling_empty;
    newly_empty->settling_time = 0xFFFFFFFFFFFFFFFF;

    settling_empty = newly_empty;
}

static void timer_list_push(settling_node *insert)
{
    settling_node *temp_settling = settling_list;
    settling_node *prev_settling = NULL;

    if (!settling_list)
    {
        settling_list = insert;
        set_timer();
    }
    else if(insert->settling_time < settling_list->settling_time)
    {
        insert->next = temp_settling;
        settling_list = insert;
        set_timer();
    }
    else
    {
        while((NULL != temp_settling) && (insert->settling_time >= temp_settling->settling_time))
        {
            prev_settling = temp_settling;
            temp_settling = temp_settling->next;
        }

        CORE_VERIFY_PTR(prev_settling);

        prev_settling->next = insert;
        insert->next = temp_settling;
    }
}

void settling_timer_isr(void) __irq
{
    INTLOCK();
    settling_node *head = settling_list;
    uint64 current_time = time_service_now();

    //go through timer list
    //call callbacks
    while ((head != NULL) && (head->settling_time < current_time))
    {
        head->cb(head->ctxt);
        head = head->next;
        timer_list_pop();
        current_time = time_service_now();
    }

    if(settling_list)
        set_timer();
    else
        HWIO_OUT(RPM_F1_QTMR_V1_CNTP_CTL, 0);

    INTFREE();
}

boolean settling_timer_register(settling_callback cb, void *ctxt, uint64 settling_time)
{
    INTLOCK();
    //add new timer to timer list

    settling_node *insert = settling_empty;

    if (NULL == settling_empty)
        return FALSE;

    settling_empty = settling_empty->next;
    insert->settling_time = settling_time;
    insert->ctxt = ctxt;
    insert->cb = cb;
    insert->next = NULL;

    timer_list_push(insert);

    INTFREE();

    return TRUE;
}

void callback(void *ctxt)
{

}

void rpm_settling_test(void)
{
    uint64 current_time = time_service_now();
    current_time += 0xB71B000; //10 seconds
    settling_timer_register(callback, (void *)NULL, current_time + 0xB71B000);
    settling_timer_register(callback, (void *)NULL, current_time);
}

void rpm_settling_timer_init(void)
{
    settling_node *temp_node;

    //allow read/write access to timer registers
    HWIO_OUTI(RPM_QTMR_AC_CNTACR_n, 1, 33);

    //enable RPM QTMR
    HWIO_OUTF(RPM_F1_QTMR_V1_CNTP_CTL, EN, 1);

    interrupt_set_isr(QTMR_IRQ, settling_timer_isr);
    interrupt_configure(QTMR_IRQ, LEVEL_HIGH);
    interrupt_enable(QTMR_IRQ);

    settling_empty = (settling_node *)malloc(sizeof(settling_node));
    CORE_VERIFY_PTR(settling_empty);
    settling_empty->settling_time = 0xFFFFFFFFFFFFFFFF;
    settling_empty->ctxt = NULL;
    settling_empty->cb = NULL;
    settling_empty->next = NULL;

    temp_node = settling_empty;

    //number of settling time possibilities is:
    //   set_transitions: num_ees (only one set transition at a time per master)
    //   messages:        num_ees * num_settling_contexts (constant number of resources settling at a time per master)
    for (unsigned i = 0; i < ((rpm->num_ees) - 1); i++)
    {
        temp_node->next = (settling_node *)malloc(sizeof(settling_node));
        CORE_VERIFY_PTR(temp_node->next);
        temp_node->next->settling_time = 0xFFFFFFFFFFFFFFFF;
        temp_node->next->ctxt = NULL;
        temp_node->next->cb = NULL;
        if(i == ((rpm->num_ees) - 2))
            temp_node->next->next = NULL;
        else
            temp_node = temp_node->next;
    }
}

