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
 * @param linkP Pointer to link element
 * @return Pointer to data element in which link element resides
**/
static inline dll_data_t *dataFromLink(void *offset, dll_link_t *linkP)
{
    return (dll_data_t *) (((char*) linkP) - ((ptrdiff_t) offset));
}

/**
 * @brief
 * Convert a pointer to a data element to a pointer to a link element, using
 * the link element offset
 * @param offset See note in dll.h.
 * @param dataP Pointer to data element
 * @return Pointer to link element residing in data element
**/
static inline dll_link_t *linkFromData(void *offset, dll_data_t *dataP)
{
    return (dll_link_t *) (((char*) dataP) + ((ptrdiff_t) offset));
}

/** A macro that calls dataFromLink with the identifier 'offset' as the first
 * argument. As it turns out, consistent naming makes this a real time saver. */
#define DFL(linkP) dataFromLink(offset, linkP)
/** A macro that calls linkFromData with the identifier 'offset' as the first
 * argument. As it turns out, consistent naming makes this a real time saver. */
#define LFD(dataP) linkFromData(offset, dataP)

void dllInitRoot(dll_root_t *rootPP)
{
    // Init to empty
    *rootPP = NULL;
}

void dllInitLink(dll_link_t *linkP)
{
    // We don't really need to init at all, so let's make it beefy
    linkP->nextP = (void*) 0xdeadbeef;
    linkP->prevP = (void*) 0xdeadbeef;
}

void dllPushHead(void* offset, dll_root_t *headLinkPP, dll_data_t *eltP)
{
    dllPushTail(offset, headLinkPP, eltP);
    *headLinkPP = LFD(eltP);
}

void dllPushTail(void* offset, dll_root_t *headLinkPP, dll_data_t *eltP)
{
    dll_link_t *newLinkP = LFD(eltP);
    // If the list is nonempty
    if (*headLinkPP) {
        // Find left and right sides of the gap we're sliding into
        dll_link_t *rightLinkP = *headLinkPP;
        dll_link_t *leftLinkP = rightLinkP->prevP;
        // Weave new element in
        newLinkP->nextP = rightLinkP;
        newLinkP->prevP = leftLinkP;
        leftLinkP->nextP = newLinkP;
        rightLinkP->prevP = newLinkP;
    } else {
        // One is all and all is one
        *headLinkPP = newLinkP->nextP = newLinkP->prevP = newLinkP;
    }
}

dll_data_t *dllPopHead(void *offset, dll_root_t *headLinkPP)
{
    // Can't pop an empty list
    if (!*headLinkPP) {
        return NULL;
    }
    dll_link_t *popLinkP = *headLinkPP;
    // If this is not the only element
    if (popLinkP->nextP != popLinkP) {
        // Sew up the hole
        dll_link_t *rightLinkP = popLinkP->nextP;
        dll_link_t *leftLinkP = popLinkP->prevP;
        leftLinkP->nextP = rightLinkP;
        rightLinkP->prevP = leftLinkP;
        // Advance head pointer
        *headLinkPP = rightLinkP;
    } else {
        // Null head, as we are emptying list
        *headLinkPP = NULL;
    }
    // Make sure no one uses the stale links in the popped element
    dllInitLink(popLinkP);
    return DFL(popLinkP);
}

dll_data_t *dllPopTail(void *offset, dll_root_t *headLinkPP)
{
    return dllRemove(offset, headLinkPP, dllTail(offset, headLinkPP));
}

dll_data_t *dllHead(void *offset, dll_root_t *headLinkPP)
{
    if (*headLinkPP) {
        return DFL(*headLinkPP);
    } else {
        return NULL;
    }
}

dll_data_t *dllTail(void *offset, dll_root_t *headLinkPP)
{
    if (*headLinkPP) {
        return DFL((*headLinkPP)->prevP);
    } else {
        return NULL;
    }
}

dll_data_t *dllNext(void *offset, dll_data_t *eltP)
{
    return DFL(LFD(eltP)->nextP);
}

dll_data_t *dllPrev(void *offset, dll_data_t *eltP)
{
    return DFL(LFD(eltP)->prevP);
}

void dllInsAfter(void *offset, dll_data_t *insertAfterMeP, dll_data_t *newEltP)
{
    dllInsBefore(offset, NULL, dllNext(offset, insertAfterMeP), newEltP);
}

void dllInsBefore(void *offset, dll_root_t *headLinkPP,
                  dll_data_t *insertBeforeMeP, dll_data_t *newEltP)
{
    // If we're inserting before the head
    if (headLinkPP && (*headLinkPP == insertBeforeMeP)) {
        dllPushHead(offset, headLinkPP, newEltP);
    } else {
        dll_link_t *linkP = LFD(insertBeforeMeP);
        dllPushHead(offset, &linkP, newEltP);
    }
}

dll_data_t *dllRemove(void *offset, dll_root_t *headLinkPP,
                      dll_data_t *removeMeP)
{
    if (headLinkPP && (dllHead(offset, headLinkPP) == removeMeP)) {
        return dllPopHead(offset, headLinkPP);
    } else {
        dll_link_t *localHeadP = LFD(removeMeP);
        return dllPopHead(offset, &localHeadP);
    }
}

int dllIsEmpty(dll_root_t *headLinkPP) {
    return !*headLinkPP;
}
