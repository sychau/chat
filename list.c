/*
Siu Yu Chau 301604828 CMPT300 Assignment 1 2024-01-30
*/

#include <assert.h>
#include "list.h"

#define NULL 0

static List listPool[LIST_MAX_NUM_HEADS];
static Node nodePool[LIST_MAX_NUM_NODES];
static List* freeListTop;
static Node* freeNodeTop;
static bool isInitialized = false;

List* List_create() {
    // Initial setup
    if (!isInitialized) {
        for (int i = 0; i < LIST_MAX_NUM_HEADS - 1; ++i) {
            listPool[i].next = &listPool[i + 1];
        }
        for (int i = 0; i < LIST_MAX_NUM_NODES - 1; ++i) {
            nodePool[i].next = &nodePool[i + 1];
        }
        listPool[LIST_MAX_NUM_HEADS - 1].next = NULL;
        nodePool[LIST_MAX_NUM_NODES - 1].next = NULL;

        freeListTop = &listPool[0];
        freeNodeTop = &nodePool[0];
        isInitialized = true;
    }
    // No available list slot
    if (!freeListTop) {
        return NULL;
    }

    List* retList = freeListTop;
    freeListTop = freeListTop->next;

    // Initialize list
    retList->count = 0;
    retList->oob = false;
    retList->oobState = LIST_OOB_START;
    retList->curr = NULL;
    retList->head = NULL;
    retList->tail = NULL;
    retList->next = NULL;
    return retList;
}

int List_count(List* pList) {
    assert(pList && "List_count: pList is NULL");
    return pList->count;
}

void* List_first(List* pList) {
    assert(pList && "List_first: pList is NULL");
    if (pList->count == 0) {
        pList->curr = NULL;
        return NULL;
    }
    pList->curr = pList->head;
    pList->oob = false;
    return pList->head->item;
}

void* List_last(List* pList) {
    assert(pList && "List_last: pList is NULL");
    if (pList->count == 0) {
        pList->curr = NULL;
        return NULL;
    }
    pList->curr = pList->tail;
    pList->oob = false;
    return pList->tail->item;
}

void* List_next(List* pList) {
    assert(pList && pList->count > 0 && "List_next: pList is NULL or empty");
    if (pList->oob) {
        // Case 1: curr is oob start
        if (pList->oobState == LIST_OOB_START) {
            pList->curr = pList->head;
            pList->oob = false;
            return pList->curr->item;

        // Case 2: curr is oob end
        } else {
            return NULL;
        }
    }
    
    pList->curr = pList->curr->next;
    // Case 3a: curr is tail
    if (!pList->curr) {
        pList->oob = true;
        pList->oobState = LIST_OOB_END;
        return NULL;
    }
    // Case 3b: curr is not tail
    return pList->curr->item;
}

void* List_prev(List* pList) {
    assert(pList && pList->count > 0 && "List_prev: pList is NULL or empty");
    if (pList->oob) {
        // Case 1: curr is oob end
        if (pList->oobState == LIST_OOB_END) {
            pList->curr = pList->tail;
            pList->oob = false;
            return pList->curr->item;

        // Case 2: curr is oob start
        } else {
            return NULL;
        }
    }
    
    pList->curr = pList->curr->prev;
    // Case 3a: curr is head
    if (!pList->curr) {
        pList->oob = true;
        pList->oobState = LIST_OOB_START;
        return NULL;
    }
    // Case 3b: curr is not head
    return pList->curr->item;
}

void* List_curr(List* pList) {
    assert(pList && "List_curr: pList is NULL");
    if (pList->oob || !pList->curr) {
        return NULL;
    }
    return pList->curr->item;
};

// Helper function for insertion
static bool List_insert_edge_cases(List* pList, Node* insertedNode) {
    // Case 1: empty list
    if (pList->count == 0) {
        insertedNode->prev = NULL;
        insertedNode->next = NULL;
        pList->head = insertedNode;
        pList->tail = insertedNode;
        pList->curr = insertedNode;
        pList->count++;
        return true;

    } else if (pList->oob) {
        // Case 2: curr is oob start
        if (pList->oobState == LIST_OOB_START) {
            insertedNode->next = pList->head;
            insertedNode->prev = NULL;
            pList->head->prev = insertedNode;
            pList->head = insertedNode;
        // Case 3: curr is oob end
        } else {
            insertedNode->next = NULL;
            insertedNode->prev = pList->tail;
            pList->tail->next = insertedNode;
            pList->tail = insertedNode;
        }
        pList->curr = insertedNode;
        pList->oob = false;
        pList->count++;
        return true;
    } 
    return false;
}

int List_insert_after(List* pList, void* pItem) {
    assert(pList && "List_insert_after: pList is NULL");
    // No available node
    if (!freeNodeTop) {
        return LIST_FAIL;
    }

    Node* insertedNode = freeNodeTop;
    insertedNode->item = pItem;
    
    freeNodeTop = freeNodeTop->next;

    // Case 1-3: OOB start/end or empty
    bool isEdgeCase = List_insert_edge_cases(pList, insertedNode);
    if (isEdgeCase) {
        return LIST_SUCCESS;
    }

    Node* currNext = pList->curr->next;
    insertedNode->next = currNext;
    insertedNode->prev = pList->curr;

    // Case 4a: curr is tail
    if (pList->curr == pList->tail) { 
        pList->tail = insertedNode;

    // Case 4b: curr is not tail
    } else {
        currNext->prev = insertedNode;
    }

    pList->curr->next = insertedNode;
    pList->curr = insertedNode;
    pList->count++;
    return LIST_SUCCESS;
}

int List_insert_before(List* pList, void* pItem) {
    assert(pList && "List_insert_before: pList is NULL");
    // No available node
    if (!freeNodeTop) {
        return LIST_FAIL;
    }

    Node* insertedNode = freeNodeTop;
    insertedNode->item = pItem;
    
    freeNodeTop = freeNodeTop->next;

    // Case 1-3: OOB start/end or empty
    bool isEdgeCase = List_insert_edge_cases(pList, insertedNode);
    if (isEdgeCase) {
        return LIST_SUCCESS;
    }

    Node* currPrev = pList->curr->prev;
    insertedNode->next = pList->curr;
    insertedNode->prev = currPrev;

    // Case 4a: curr is head
    if (pList->curr == pList->head) { 
        pList->head = insertedNode;

    // Case 4b: curr is not head
    } else {
        currPrev->next = insertedNode;
    }

    pList->curr->prev = insertedNode;
    pList->curr = insertedNode;
    pList->count++;
    return LIST_SUCCESS;
}

int List_append(List* pList, void* pItem) {
    assert(pList && "List_append: pList is NULL");
    List_last(pList);
    return List_insert_after(pList, pItem);
}

int List_prepend(List* pList, void* pItem) {
    assert(pList && "List_prepend: pList is NULL");
    List_first(pList);
    return List_insert_before(pList, pItem);
}

void* List_remove(List* pList) {
    assert(pList && "List_remove: pList is NULL");
    // curr is oob or nothing to remove    
    if (pList->oob || pList->count == 0) {
        return NULL;
    }

    void* rmItem = pList->curr->item;
    Node* newFreeNode = pList->curr;
   
    // Case 1: pList has only one item
    if (pList->count == 1) {
        pList->head = NULL;
        pList->tail = NULL;
        pList->curr = NULL;

    // Case 2: curr is head only
    } else if (pList->curr == pList->head) {
        Node* newHead = pList->head->next;
        pList->head = newHead;
        pList->curr = newHead;
        newHead->prev = NULL;

    // Case 3: curr is tail only
    } else if (pList->curr == pList->tail) {
        Node* newTail = pList->tail->prev;
        pList->tail = newTail;
        pList->curr = NULL;
        pList->oob = true;
        pList->oobState = LIST_OOB_END;
        newTail->next = NULL;
    
    // Case 4: curr is neither
    } else {
        Node* currPrev = pList->curr->prev;
        Node* currNext = pList->curr->next;
        currPrev->next = currNext;
        currNext->prev = currPrev;
        pList->curr = currNext;
    }

    newFreeNode->next = freeNodeTop;
    freeNodeTop = newFreeNode;
    pList->count--;
    return rmItem;
}

void* List_trim(List* pList) {
    assert(pList && "List_trim: pList is NULL");
    if (pList->count == 0) {
        return NULL;
    }
    List_last(pList);
    void* rmItem = List_remove(pList);
    pList->oob = false;
    pList->curr = pList->tail;
    return rmItem;
}

void List_concat(List* pList1, List* pList2) {
    assert(pList1 && pList2 && "List_concat: pList1/2 is NULL");

    // Case 1: both list empty
    if (pList1->count == 0 && pList2->count == 0) {
        // do nothing

    // Case 2: list1 empty
    } else if (pList1->count == 0) {
        pList1->head = pList2->head;
        pList1->tail = pList2->tail;
        pList1->count += pList2->count;

    // Case 3: list2 empty
    } else if (pList2->count == 0) {
        // do nothing

    // Case 4: neither list empty
    } else {
        pList1->tail->next = pList2->head;
        pList1->tail = pList2->tail;
        pList1->count += pList2->count;
    }

    // Reclaim pList2 slot
    List* currFreeListTop = freeListTop;
    freeListTop = pList2;
    freeListTop->next = currFreeListTop;
}

void List_free(List* pList, FREE_FN pItemFreeFn) {
    assert(pList && "List_free: pList is NULL");
    assert(pItemFreeFn && "List_free: pItemFreeFn is NULL");
    // Free node item and reclaim node slot
    Node* node = pList->head;
    while (node) {
        (*pItemFreeFn)(node->item);
        pList->count--;
        Node* currFreeNodeTop = freeNodeTop;
        freeNodeTop = node;
        node = node->next;
        freeNodeTop->next = currFreeNodeTop;
    }

    // Reclaim list slot
    List* currFreeListTop = freeListTop;
    freeListTop = pList;
    freeListTop->next = currFreeListTop;
}

void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg) {
    assert(pList && "List_search: pList is NULL");
    assert(pComparator && "List_search: pComparator is NULL");
    assert(pComparisonArg && "List_search: pComparisonArg is NULL");
    if (pList->oob) {
        if (pList->oobState == LIST_OOB_START) {
            List_first(pList);
        } else {
            return NULL;
        }
    }
    while(pList->curr) {
        if ((*pComparator)(pList->curr->item, pComparisonArg)) {
            return pList->curr->item;
        }
        pList->curr = pList->curr->next;
    }
    // No match found
    pList->oob = true;
    pList->oobState = LIST_OOB_END;
    return NULL;
}