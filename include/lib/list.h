/******************************************************************************\
**
**  This file is part of the Poseidon Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2018-2024 - The Poseidon Authors
**
\******************************************************************************/

/*
** Heavily inspired by `list.h` in the Linux Kernel, also made available under
** the terms of the GNU General Public License version 2.
**
** https://github.com/torvalds/linux/tree/master/include/linux/list.h
*/

#pragma once

#include <stddef.h>

struct linked_list {
    struct linked_list *next;
    struct linked_list *prev;
};

#define LIST_HEAD_INIT(name)    ((struct linked_list) { &(name), &(name) })
#define LIST_HEAD(name)         struct linked_list name = LIST_HEAD_INIT(name)

static inline
bool
list_is_empty(
    struct linked_list *head
) {
    return head == head->next;
}

static inline
void
__list_add(
    struct linked_list *node,
    struct linked_list *prev,
    struct linked_list *next
) {
    node->next = next;
    node->prev = prev;
    next->prev = node;
    prev->next = node;
}

static inline
void
list_add_head(
    struct linked_list *head,
    struct linked_list *node
) {
    __list_add(node, head, head->next);
}

static inline
void
list_add_tail(
    struct linked_list *head,
    struct linked_list *node
) {
    __list_add(node, head->prev, head);
}

static inline
void
__list_remove(
    struct linked_list *prev,
    struct linked_list *next
) {
    next->prev = prev;
    prev->next = next;
}

static inline
void
list_remove(
    struct linked_list *node
) {
    __list_remove(node->prev, node->next);
    node->prev = NULL;
    node->next = NULL;
}

static inline
void
list_replace(
    struct linked_list *old,
    struct linked_list *new
) {
    new->next = old->next;
    new->next->prev = new;

    new->prev = old->prev;
    new->prev->next = new;

    old->next = NULL;
    old->prev = NULL;
}

static inline
void
list_move_tail(
    struct linked_list *head,
    struct linked_list *node
) {
    list_remove(node);
    list_add_tail(head, node);
}

#define list_entry(cursor, type, member) ({                                     \
        unsigned char *ucursor = (unsigned char *)(cursor);                     \
        (type *)(ucursor - offsetof(type, member));                             \
    })

#define list_next_entry(cursor, member) ({                                      \
        list_entry((cursor)->member.next, typeof(*(cursor)), member);           \
    })

#define list_first_entry_or_null(_head, type, member) ({                        \
        struct linked_list *head = (_head);                                     \
        struct linked_list *cursor = head->next;                                \
                                                                                \
        (head != cursor) ? list_entry(cursor, type, member) : NULL;             \
    })

#define list_for_each(cursor, head)                                             \
    for (cursor = (head)->next; cursor != (head); cursor = cursor->next)

static inline
size_t
list_length(
    struct linked_list *head
) {
    struct linked_list *cursor;
    size_t length;

    length = 0;
    list_for_each(cursor, head) {
        length += 1;
    }
    return length;
}
