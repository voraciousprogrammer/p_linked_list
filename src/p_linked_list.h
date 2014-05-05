#ifndef _P_LINKED_LIST_
#define _P_LINKED_LIST_

enum order {
        OR_HEAD,        /* Add/remove element at/from the head of the list. */
        OR_TAIL,        /* Add/remove element at/from the tail of the list. */
        OR_ORDER        /* Insert the element in order (only for inserts). */
};

/* Forward declaration of opaque types. */
typedef struct p_linked_list p_linked_list_t;

/* Exported functions. */
p_linked_list_t *p_linked_list_create(void (*)(const void *),
                                      int (*)(const void *, const void *));
void p_linked_list_destroy(p_linked_list_t *);
int p_linked_list_add_element(p_linked_list_t *, void *, enum order);
void *p_linked_list_remove_element(p_linked_list_t *, enum order);
int p_linked_list_is_empty(p_linked_list_t *);

#endif /* !_P_LINKED_LIST_ */
