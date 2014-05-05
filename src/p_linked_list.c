/*
 * Copyright (c) 2014,
 *      Jimmy Johansson <voraciousprogrammer@gmail.com>. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "p_linked_list.h"

typedef struct linked_list_node {
        void *le_data;                          /* The Data stored in list. */
        struct linked_list_node *le_next;       /* Next node in list. */
        struct linked_list_node *le_prev;       /* Previous node in list. */
} linked_list_node_t;

struct p_linked_list {
        linked_list_node_t *pt_list_head;               /* Head of the list. */
        linked_list_node_t *pt_list_tail;               /* Tail of the list. */
        int (*pt_cmpfn)(const void *, const void *);    /* Compare fn. */
        void (*pt_deallocator)(const void *);           /* Deallocation fn. */
};

static int insert_at_head(p_linked_list_t *, linked_list_node_t *);
static int insert_at_tail(p_linked_list_t *, linked_list_node_t *);
static int insert_in_order(p_linked_list_t *, linked_list_node_t *);

/*
 * Create an empty linked list.
 *
 * deallocator: Function used to deallocate elements when the list is destroyed.
 * comparefn: Function that can be used to compare the order between
 *            the elements or NULL if no comparison will ever be done.
 *            It should return less than, equal to, or greater than
 *            zero if the first argument is considered to be
 *            less than, equal to, or greater than the second.
 *
 * Returns: An empty linked list or NULL at failure.
 */
p_linked_list_t *
p_linked_list_create(void (*deallocator)(const void *),
                     int (*comparefn)(const void *, const void *))
{
        p_linked_list_t *new_list;

        if (deallocator == NULL)
                return NULL;

        new_list = calloc(1, sizeof(*new_list));
        if (new_list == NULL)
                return NULL;

        new_list->pt_list_head = NULL;
        new_list->pt_list_tail = NULL;
        new_list->pt_cmpfn = comparefn;
        new_list->pt_deallocator = deallocator;

        return new_list;
}

/*
 * Deallocates memory used by the linked list.
 *
 * linked_list: The linked list to deallocate.
 */
void
p_linked_list_destroy(p_linked_list_t *linked_list)
{
        linked_list_node_t *element, *next_element;

        if (linked_list != NULL) {

                if (!p_linked_list_is_empty(linked_list)) {
                        for (element = linked_list->pt_list_head;
                            element != NULL; element = next_element) {
                                next_element = element->le_next;
                                linked_list->pt_deallocator(element);
                        }
                }

                linked_list->pt_list_head = NULL;
                linked_list->pt_list_tail = NULL;
                linked_list->pt_cmpfn = NULL;
                linked_list->pt_deallocator = NULL;

                free(linked_list);
        }
}

/*
 * Adds the element element to the linked list linked_list using add_order to
 * decide where to insert the element.
 *
 * linked_list: The linked list to insert an element into.
 * element: The element to insert.
 * add_order: Specifies how to insert the element.
 *
 * Returns: 1 on success or 0 at failure.
 */
int
p_linked_list_add_element(p_linked_list_t *list, void *element,
                          enum order add_order)
{
        linked_list_node_t *node;

        if (list == NULL)
                return 0;

        node = calloc(1, sizeof(*node));
        if (node == NULL)
                return 0;

        node->le_data = element;
        node->le_prev = NULL;
        node->le_next = NULL;

        if (p_linked_list_is_empty(list)) {
                /*
                 * The list is empty so insert order doesn't matter.
                 */
                list->pt_list_head = node;
                list->pt_list_tail = node;
        } else {
                switch (add_order) {
                case OR_HEAD:
                        if (!insert_at_head(list, node)) {
                                free(node);
                                node = NULL;
                                return 0;
                        }
                        break;
                case OR_TAIL:
                        if (!insert_at_tail(list, node)) {
                                free(node);
                                node = NULL;
                                return 0;
                        }
                        break;
                case OR_ORDER:
                        if (!insert_in_order(list, node)) {
                                free(node);
                                node = NULL;
                                return 0;
                        }
                        break;
                default:
                        /* This should never happen! */

                        return 0;
                }
        }

        return 1;
}

/*
 * Removes and returns an element from the linked list linked_list using
 * remove_order to decide from where in the list the element is removed.
 *
 * linked_list: The linked list to remove an element from.
 * remove_order: Specifies from where in the linked list the element
 *               should be removed. Note that only OR_HEAD and OR_TAIL
 *               are valid.
 *
 * Returns: The removed element.
 */
void *
p_linked_list_remove_element(p_linked_list_t *linked_list,
                             enum order remove_order)
{
        linked_list_node_t *tmp_node;
        void *tmp_data;

        if (linked_list == NULL)
                return NULL;

        if (p_linked_list_is_empty(linked_list))
                return NULL;

        if (remove_order == OR_ORDER)
                return NULL;

        if (linked_list->pt_list_head->le_next == NULL) {
                /*
                 * Last element in the list, so it doesn't
                 * matter if we remove from head or tail.
                 */
                tmp_node = linked_list->pt_list_head;
                linked_list->pt_list_head = NULL;
                linked_list->pt_list_tail = NULL;
        } else {
                switch (remove_order) {
                case OR_HEAD:
                        tmp_node = linked_list->pt_list_head;
                        linked_list->pt_list_head = tmp_node->le_next;
                        linked_list->pt_list_head->le_prev = NULL;
                        break;
                case OR_TAIL:
                        tmp_node = linked_list->pt_list_tail;
                        linked_list->pt_list_tail = tmp_node->le_prev;
                        linked_list->pt_list_tail->le_next = NULL;
                        break;
                default:
                        /* This should never happen! */

                        return NULL;
                }
        }

        tmp_data = tmp_node->le_data;

        free(tmp_node);

        return tmp_data;
}

/*
 * Decide if the linked list is empty or not.
 *
 * linked_list: The linked list to check if empty.
 *
 * Returns: 1 if the linked list is empty or 0 otherwise.
 */
int
p_linked_list_is_empty(p_linked_list_t *linked_list)
{

        if (linked_list == NULL || linked_list->pt_list_head == NULL)
                return 1;
        else
                return 0;
}

/*
 * Inserts an element node at the head of the list list.
 *
 * list: The list to insert the new node into.
 * node: The node to insert.
 *
 * Returns: 1 at success and 0 at failure.
 */
static int
insert_at_head(p_linked_list_t *list, linked_list_node_t *node)
{

        if (list == NULL || node == NULL || p_linked_list_is_empty(list))
                return 0;

        node->le_next = list->pt_list_head;
        list->pt_list_head->le_prev = node;
        list->pt_list_head = node;

        return 1;
}

/*
 * Inserts an element node at the tail of the list list.
 *
 * list: The list to insert the new node into.
 * node: The node to insert.
 *
 * Returns: 1 at success and 0 at failure.
 */
static int
insert_at_tail(p_linked_list_t *list, linked_list_node_t *node)
{

        if (list == NULL || node == NULL || p_linked_list_is_empty(list) ||
            list->pt_list_tail == NULL)
                return 0;

        node->le_prev = list->pt_list_tail;
        list->pt_list_tail->le_next = node;
        list->pt_list_tail = node;

        return 1;
}

/*
 * Inserts an element node in order into the list according to a compare
 * function.
 *
 * list: The list to insert the element into.
 * node: The node to insert.
 *
 * Returns: 1 at success and 0 at failure.
 */
static int
insert_in_order(p_linked_list_t *list, linked_list_node_t *node)
{
        linked_list_node_t *tmp;

        if (list->pt_cmpfn == NULL)
                return 0;

        if (list == NULL || node == NULL || p_linked_list_is_empty(list))
                return 0;

        for (tmp = list->pt_list_head; tmp != NULL; tmp = tmp->le_next)
                if (list->pt_cmpfn(node->le_data, tmp->le_data) < 0)
                        break;

        if (tmp == list->pt_list_head) {
                if (!insert_at_head(list, node))
                        return 0;
        } else if (tmp == NULL) {
                if (!insert_at_tail(list, node))
                        return 0;
        } else {
                node->le_next = tmp;
                node->le_prev = tmp->le_prev;
                tmp->le_prev->le_next = node;
        }

        return 1;
}
