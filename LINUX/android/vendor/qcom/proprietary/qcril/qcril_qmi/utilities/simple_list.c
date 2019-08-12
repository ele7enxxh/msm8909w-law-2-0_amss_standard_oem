/*!
  @file
  simple_list.c

  @brief
  List implemented as a linked list

*/

/*===========================================================================

  Copyright (c) 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/
#include <stdlib.h>
#include <string.h>

#include "simple_list.h"
#include "ref_counted.h"

struct simple_list_iter
{
    REF_COUNTED_DECL;
    simple_list_iter *prev;
    simple_list_iter *next;
    void *value;
};

struct simple_list
{
    simple_list_iter *head;
    simple_list_iter *tail;
};

simple_list *simple_list_new()
{
    simple_list *list = calloc(1, sizeof(simple_list));
    return list;
}

static simple_list_iter *simple_list_iter_new()
{
    simple_list_iter *iter;

    iter = calloc(1, sizeof(simple_list_iter));
    REF_COUNTED_INIT(iter, ref_counted_default_free);
    return iter;
}
void simple_list_append(simple_list *list, void *value)
{
    simple_list_iter *new_it;
    if ((!list) || /* Null list pointer provided */
        /* Inconsistent list. If tail is set, head must be set also */
        (!list->tail && list->head)
       )
    {
        return;
    }
    new_it = simple_list_iter_new();
    new_it->value = value;
    REF_COUNTED_REPL(new_it->prev, list->tail);

    if (list->tail)
    {
        list->tail->next = REF_COUNTED_INCR(new_it);
    }
    REF_COUNTED_REPL(list->tail, new_it);
    if (!list->head) list->head = REF_COUNTED_INCR(new_it);

    /* Drop the local reference */
    REF_COUNTED_DECR(new_it);
}

void simple_list_foreach(simple_list *list, simple_list_foreach_cb cb, void *userdata)
{
    simple_list_iter *it;
    if (!list || !cb)
    {
        return;
    }

    for(it = REF_COUNTED_INCR(list->head); it; REF_COUNTED_REPL(it, it->next))
    {
        if (cb(list, it, it->value, userdata))
        {
            break;
        }
    }
}

simple_list_iter *simple_list_first(simple_list *list)
{
    simple_list_iter *ret = NULL;

    if (list)
    {
        ret = REF_COUNTED_INCR(list->head);
    }
    return ret;
}

simple_list_iter *simple_list_iter_next(simple_list_iter *iter)
{
    REF_COUNTED_REPL(iter, iter->next);
    return iter;
}

void *simple_list_iter_get_value(simple_list_iter *iter)
{
    return iter->value;
}

void simple_list_iter_done(simple_list_iter *iter)
{
    REF_COUNTED_DECR(iter);
}
