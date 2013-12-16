/**
 * @file dll.c
 * @author Zachary McCord
 * @brief Contains the implementation of a circular doubly-linked-list library.
 *
 * @note In this implementation, a list is a pointer to the first element's link
 *       field.
 */
#include "dll.h"

#include <stddef.h>

/**
 * @brief
 * Convert a pointer to a link element to a pointer to a data element, using
 * the link element offset
 * @param offset See note in dll.h.
 * @param link_p Pointer to link element
 * @return Pointer to data element in which link element resides
**/
static inline dll_data_t *data_from_link(ptrdiff_t offset, dll_link_t *link_p)
{
    return (dll_data_t *) (((char*) link_p) - offset);
}

/**
 * @brief
 * Convert a pointer to a data element to a pointer to a link element, using
 * the link element offset
 * @param offset See note in dll.h.
 * @param data_p Pointer to data element
 * @return Pointer to link element residing in data element
**/
static inline dll_link_t *link_from_data(ptrdiff_t offset, dll_data_t *data_p)
{
    return (dll_link_t *) (((char*) data_p) + offset);
}

/** A macro that calls data_from_link with the identifier 'offset' as the first
 * argument. As it turns out, consistent naming makes this a real time saver. */
#define DFL(link_p) data_from_link(offset, link_p)
/** A macro that calls link_from_data with the identifier 'offset' as the first
 * argument. As it turns out, consistent naming makes this a real time saver. */
#define LFD(data_p) link_from_data(offset, data_p)

void dll_init_root(dll_root_t *root_pp)
{
    // Init to empty
    *root_pp = NULL;
}

void dll_init_link(dll_link_t *link_p)
{
    // We don't really need to init at all, so let's make it beefy
    link_p->next_p = (void*) 0xdeadbeef;
    link_p->prev_p = (void*) 0xdeadbeef;
}

void dll_push_head(ptrdiff_t offset,
                   dll_root_t *head_link_pp,
                   dll_data_t *elt_p)
{
    dll_push_tail(offset, head_link_pp, elt_p);
    *head_link_pp = LFD(elt_p);
}

void dll_push_tail(ptrdiff_t offset,
                   dll_root_t *head_link_pp,
                   dll_data_t *elt_p)
{
    dll_link_t *new_link_p = LFD(elt_p);
    // If the list is nonempty
    if (*head_link_pp) {
        // Find left and right sides of the gap we're sliding into
        dll_link_t *right_link_p = *head_link_pp;
        dll_link_t *left_link_p = right_link_p->prev_p;
        // Weave new element in
        new_link_p->next_p = right_link_p;
        new_link_p->prev_p = left_link_p;
        left_link_p->next_p = new_link_p;
        right_link_p->prev_p = new_link_p;
    } else {
        // One is all and all is one
        *head_link_pp = new_link_p->next_p = new_link_p->prev_p = new_link_p;
    }
}

dll_data_t *dll_pop_head(ptrdiff_t offset, dll_root_t *head_link_pp)
{
    // Can't pop an empty list
    if (!*head_link_pp) {
        return NULL;
    }
    dll_link_t *pop_link_p = *head_link_pp;
    // If this is not the only element
    if (pop_link_p->next_p != pop_link_p) {
        // Sew up the hole
        dll_link_t *right_link_p = pop_link_p->next_p;
        dll_link_t *left_link_p = pop_link_p->prev_p;
        left_link_p->next_p = right_link_p;
        right_link_p->prev_p = left_link_p;
        // Advance head pointer
        *head_link_pp = right_link_p;
    } else {
        // Null head, as we are emptying list
        *head_link_pp = NULL;
    }
    // Make sure no one uses the stale links in the popped element
    dll_init_link(pop_link_p);
    return DFL(pop_link_p);
}

dll_data_t *dll_pop_tail(ptrdiff_t offset, dll_root_t *head_link_pp)
{
    return dll_remove(offset, head_link_pp, dll_tail(offset, head_link_pp));
}

dll_data_t *dll_head(ptrdiff_t offset, dll_root_t *head_link_pp)
{
    if (*head_link_pp) {
        return DFL(*head_link_pp);
    } else {
        return NULL;
    }
}

dll_data_t *dll_tail(ptrdiff_t offset, dll_root_t *head_link_pp)
{
    if (*head_link_pp) {
        return DFL((*head_link_pp)->prev_p);
    } else {
        return NULL;
    }
}

dll_data_t *dll_next(ptrdiff_t offset, dll_data_t *elt_p)
{
    return DFL(LFD(elt_p)->next_p);
}

dll_data_t *dll_prev(ptrdiff_t offset, dll_data_t *elt_p)
{
    return DFL(LFD(elt_p)->prev_p);
}

void dll_ins_after(ptrdiff_t offset,
                   dll_data_t *insert_after_me_p,
                   dll_data_t *new_elt_p)
{
    dll_ins_before(offset,
                   NULL,
                   dll_next(offset, insert_after_me_p),
                   new_elt_p);
}

void dll_ins_before(ptrdiff_t offset, dll_root_t *head_link_pp,
                    dll_data_t *insert_before_me_p, dll_data_t *new_elt_p)
{
    // If we're inserting before the head
    if (head_link_pp &&
        (dll_head(offset, head_link_pp) == LFD(insert_before_me_p)))
    {
        dll_push_head(offset, head_link_pp, new_elt_p);
    } else {
        dll_link_t *link_p = LFD(insert_before_me_p);
        dll_push_head(offset, &link_p, new_elt_p);
    }
}

dll_data_t *dll_remove(ptrdiff_t offset, dll_root_t *head_link_pp,
                       dll_data_t *remove_me_p)
{
    if (head_link_pp && (dll_head(offset, head_link_pp) == remove_me_p)) {
        return dll_pop_head(offset, head_link_pp);
    } else {
        dll_link_t *local_head_p = LFD(remove_me_p);
        return dll_pop_head(offset, &local_head_p);
    }
}

int dll_is_empty(dll_root_t *head_link_pp) {
    return !*head_link_pp;
}
