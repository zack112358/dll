/**
 * @file dll.h
 * @author Zachary McCord (zmccord@andrew.cmu.edu)
 * @brief Header for our doubly-linked-list circular list library.
 *
 * @warning This library doesn't handle locking. Locking is the library user's
 *          problem. Considering as this was originally written to be used *in*
 *          a thread library, this design decision should make sense; using a
 *          locking queue to implement locks kinda begs the question.
 *
 * @note This linked list design was first hashed out in a largely
 *       410-unrelated conversation about how a really good linked-list library
 *       would look years ago with mbrewer. Credit where it's
 *       due, he gave me some good design ideas. This long after, I don't
 *       remember which are his, and the functions themselves are of course
 *       shaped so that they'll be a good variable-queue implementation.
 *       -zmccord
 *
 * @note The user of this library will declare a list as dll_root_t. Most
 *       operations will be expressed in terms of ops on a dll_root_t, which is
 *       an opaque type representing an abstract linked list.
 *
 * @note Any structure linked into this list will need to
 *       contain a member of type dll_link_t.
 *
 * @note At no point does any function in this library *ever* allocate or
 *       deallocate memory. That's the library user's problem.
 *
 * @note Offsets. Most of the functions in this library take an 'offset'
 *       parameter. This is the offset in bytes from the address of the data
 *       structure where the link field can be found. For convenience, the
 *       offset arguments are defined as void*, but they should be thought of as
 *       ptr_diff_t.<br>
 *<br>
 *       An example might make things clearer:<br>
 *<br>
 *       typedef struct {<br>
 *           int value;<br>
 *           dll_link_t link;<br>
 *       } node_t;<br>
 *       node_t a;<br>
 *       dll_root_t list;<br>
 *       dll_init_head(list);<br>
 *       dll_init_link(&a.link);<br>
 *       dll_push_head(&((node_t*)0).link, &a);<br>
 *<br>
 *       Taking offset as void* allows the above addressof trick to be
 *       convenient, and that's the expected common case. If you plan to use
 *       this library much, though, it's probably worth your time to define some
 *       quick convenience functions or macros to save you having to specify the
 *       offset argument all the time.<br>
 *<br>
 *       If you have variable-size objects, you might want to give an offset of
 *       ((void*) 0) and do your addressing math by hand --- although one
 *       alternative is to place the link field at a consistent offset in each
 *       different type or size of object.
 *
 * @warning We have discovered that the offsets-as-void* tends to lead to bugs
 *          where one tries to add byte offsets to pointers without remembering
 *          that the pointers to objects of size other than 1, and loses. Had we
 *          more time we would therefore convert offsets to ptrdiff_t or
 *          similar.
 *
 * @note I'm sorry the comments are so long. I comment a lot when I'm tired.
 *
**/
#ifndef DOUBLY_LINKED_LIST_H
#define DOUBLY_LINKED_LIST_H

/**
 * @brief The type of a list link. This is the type of the link element that
 *        should be declared inside structs that want to be list members.
 */
typedef struct dll_link {
    /** Pointer to the next element in the list (or the head, if this is the
     * tail element */
    struct dll_link *prev_p;
    /** Pointer to the previous element in the list (or the tail, if this is the
     * head element) */
    struct dll_link *next_p;
} dll_link_t;

/**
 * @brief The type of a linked list. This is notionally an opaque type. Of
 *         course, C doesn't have existential types, so it's more of a...
 *         translucent type. Cloudy, perhaps.
 */
typedef dll_link_t *dll_root_t;

/**
 * @brief We're going to work strictly with void*, but dll_data_t* just *looks*
 *        so much *nicer*.
 */
typedef void dll_data_t;

/**
 * @brief Initialize a new list.
 * @param root_p Pointer to the list to initialize.
 */
void dll_init_root(dll_root_t *root_p);

/**
 * @brief Initialize the link member of a struct.
 * @param link_p Pointer to link to init.
 */
void dll_init_link(dll_link_t *link_p);

/**
 * @brief Push an element onto the head of the linked list given.
 * @param offset See note at top of file.
 * @param list_p A pointer to the list to push onto.
 * @param elt_p A pointer to the element to push on.
 */
void dll_push_head(void* offset, dll_root_t *list_p, dll_data_t *elt_p);

/**
 * @brief Push an element onto the tail of the linked list given.
 * @param offset See note at top of file.
 * @param list_p A pointer to the list to push onto.
 * @param elt_p A pointer to the element to push on.
 */
void dll_push_tail(void* offset, dll_root_t *list_p, dll_data_t *elt_p);

/**
 * @brief Pop an element off the head of the linked list given and return it.
 * @param offset See note at top of file.
 * @param list_p A pointer to the list to pop.
 * @return Pointer to the popped element, or null if list was empty.
 */
dll_data_t *dll_pop_head(void *offset, dll_root_t *list_p);

/**
 * @brief Pop an element off the tail of the linked list given and return it.
 * @param offset See note at top of file.
 * @param list_p A pointer to the list to pop.
 * @return Pointer to the popped element, or null if list was empty.
 */
dll_data_t *dll_pop_tail(void *offset, dll_root_t *list_p);

/**
 * @brief Return the first element of the given linked list.
 * @param offset See note at top of file.
 * @param list_p A pointer to the list to inspect.
 * @return Pointer to the first element, or null if list is empty.
 */
dll_data_t *dll_head(void *offset, dll_root_t *list_p);

/**
 * @brief Return the last element of the given linked list.
 * @param offset See note at top of file.
 * @param list_p A pointer to the list to inspect.
 * @return Pointer to the last element, or null if list is empty.
 */
dll_data_t *dll_tail(void *offset, dll_root_t *list_p);

/**
 * @brief Get the next element in the list after this one.
 * @param offset See note at top of file.
 * @param elt_p Pointer to the element whose successor we want to find.
 * @return Pointer to the next element.
 * @warning In a single element list, the element is its own successor. Thus
 *          this function may return elt_p.
 */
dll_data_t *dll_next(void *offset, dll_data_t *elt_p);

/**
 * @brief Get the prev element in the list after this one.
 * @param offset See note at top of file.
 * @param elt_p Pointer to the element whose predecessor we want to find.
 * @return Pointer to the prev element.
 * @warning In a single element list, the element is its own predecessor. Thus
 *          this function may return elt_p.
 */
dll_data_t *dll_prev(void *offset, dll_data_t *elt_p);

/**
 * @brief Insert another element after this one. Automatically updates tail to
 *         the new value if you insert after the tail.
 * @param offset See note at top of file.
 * @param insert_after_me_p Pointer to the element after which to insert.
 * @param new_elt_p Pointer to the new element to insert.
 */
void dll_ins_after(void *offset, dll_data_t *insert_after_me_p, dll_data_t *new_elt_p);

/**
 * @brief Insert another element before this one.
 * @param offset See note at top of file.
 * @param list_p List root pointer. If you insert before the head, I can use
 *              this to update the head to the new element. If you don't want
 *              this behavior, pass list_p == NULL, which will cause inserting
 *              before the head to be equivalent to inserting after the tail.
 * @param insert_before_me_p Pointer to the element before which to insert.
 * @param new_elt_p Pointer to the new element to insert.
 */
void dll_ins_before(void *offset, dll_root_t *list_p, dll_data_t *insert_before_me_p,
                    dll_data_t *new_elt_p);

/**
 * @brief Remove this element from the list. Also moves the list head to the
 *         next element if we remove the list head.
 * @param offset See note at top of file.
 * @param list_p Pointer to list to remove from. This is necessary because we
 *        need to null out some fields in the case that we delete the last list
 *        element. However, if you pass NULL, we will ignore the list_p argument.
 *        We intend this behavior for lists embedded in other datastructures for
 *        which a distinguished head pointer does not exist. Be careful with
 *        this behavior, as failing to pass a list head risks invalidating it.
 * @param remove_me_p Pointer to the element to remove.
 * @return Pointer to the freshly removed element.
 */
dll_data_t *dll_remove(void *offset, dll_root_t *list_p, dll_data_t *remove_me_p);

/**
 * @brief Checks whether a list is empty.
 * @param list_p Pointer to list to inspect.
 * @return Nonzero iff list is empty.
 */
int dll_is_empty(dll_root_t *list_p);

#endif /* #ifndef DOUBLY_LINKED_LIST_H */
