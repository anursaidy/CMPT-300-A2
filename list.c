#include "List.h"
#include <stdio.h>

NodeContainer currentContainer;
static bool listInit = false;

List* List_create(){
    if(!listInit){
        printf("Initializing List\n");
        for(int i = 0; i < LIST_MAX_NUM_HEADS;i++){
            currentContainer.availHeads[i].current = NULL;
            currentContainer.availHeads[i].head = NULL;
            currentContainer.availHeads[i].tail = NULL;
            currentContainer.availHeads[i].size = 0;
        }
        printf("Initializing Nodes\n");
        for(int i = 0; i < LIST_MAX_NUM_NODES; i++){
            currentContainer.availNodes[i].next = NULL;
            currentContainer.availNodes[i].prev = NULL;
            currentContainer.availNodes[i].index = i;
            currentContainer.availNodes[i].containerIndex = 0;
            currentContainer.availNodes[i].boundsEnum = 3;
            currentContainer.availNodes[i].item = NULL;
        }
        listInit = true;
        currentContainer.usedHeads = 0;
        currentContainer.usedNodes = 0;
    }
    if(currentContainer.usedHeads >= LIST_MAX_NUM_HEADS){
        return NULL;
    }
    else{
        List* newList = &currentContainer.availHeads[currentContainer.usedHeads];
        currentContainer.usedHeads++;
        printf("Remaining Heads: %d\n", LIST_MAX_NUM_HEADS - currentContainer.usedHeads);
        return newList;
        
    }

}

int List_count(List* pList){
    return pList->size;
}

void* List_first(List* pList){
    if(pList->head == NULL || pList->head->item == NULL){
        return NULL;
    }
    else{
        return pList->head->item;
    }
}

void* List_last(List* pList){
    if(pList->tail == NULL || pList->tail->item == NULL || pList->size <= 1){
        return NULL;
    }
    else{
        return pList->tail->item;
    }
}

void* List_curr(List* pList){
    return pList->current->item;
}

int initializeListHead(List* pList, Node* givenNode){
            pList->head = givenNode;
            pList->current = givenNode;
            pList->size++;
            currentContainer.usedNodes++;
            return 0;
}

int List_append(List* pList, void* pItem){
    if(currentContainer.usedNodes >= LIST_MAX_NUM_NODES){
        return -1;
    }
    else{
        Node* newItem = &currentContainer.availNodes[currentContainer.usedNodes];
        newItem->containerIndex = currentContainer.usedNodes;
        newItem->item = pItem;
        newItem->index = pList->size;
        //Case #1: List is empty
        if(pList->size == 0){
            return initializeListHead(pList,newItem);
        }
        //Case #2: There is only one item in the list
        else if(pList->size == 1){
            pList->tail = newItem;
            pList->tail->prev = pList->head;
            pList->head->next = pList->tail;
            pList->current = pList->tail;
        }
        //Case #3: There are more than one item in the list
        else{
            pList->tail->next = newItem;
            newItem->prev = pList->tail;
            pList->tail = newItem;
            pList->current = pList->tail;
        }
        pList->size++;
        currentContainer.usedNodes++;
        return 0;
    }
}

int List_prepend(List* pList, void* pItem){
    if(currentContainer.usedNodes >= LIST_MAX_NUM_NODES){
        return -1;
    }
    else{
        //Logic should remain the same as append
        Node* newItem = &currentContainer.availNodes[currentContainer.usedNodes];
        newItem->containerIndex = currentContainer.usedNodes;
        newItem->index = pList->size;
        newItem->item = pItem;
        if(pList->size == 0){
            return initializeListHead(pList,newItem);
        }
        else if(pList->size == 1){
            pList->head->prev = newItem;
            newItem->next = pList->head;
            pList->head = newItem;
            pList->tail = newItem->next;
            pList->current = pList->head;
        }
        else{
            pList->head->prev = newItem;
            newItem->next = pList->head;
            pList->head = newItem;
            pList->current = pList->head;
        }
        pList->size++;
        currentContainer.usedNodes++;
        return 0;
    }
}

void* List_next(List* pList){
    //Case #1: The current is at the tail
    if(pList->current->next == NULL || pList->current->next->index > pList->size){
        //Possible Theory: We can check the next and previous nodes and store it there, otherwise we jump to the end + 1 size of the list
        //Keep shifting until there are no more nodes that are taken up
        int newLocation = LIST_MAX_NUM_NODES - pList->size;
        // Debugging message for the new location
        // printf("%d\n",newLocation);
        Node* newNodeLocation = &currentContainer.availNodes[newLocation];
        newNodeLocation->containerIndex = newLocation;
        newNodeLocation->item = pList->current->item;
        newNodeLocation->boundsEnum = LIST_OOB_END;
        // Debug Message commented out below
        // printf("new node location: %d\nnew node item: %d\n",newNodeLocation->containerIndex,newNodeLocation->item);
        pList->current = newNodeLocation;
        currentContainer.usedNodes++;
        return NULL;
    }
    //Case #2: The current is within the bounds
    else{
        pList->current = pList->current->next;
        return pList->current->item;
    }
}



void* List_prev(List* pList){
    //Case #1: if the current is NULL
    if(pList->current->prev == NULL || pList->current->prev->containerIndex < 0){
        int newLocation = 0;
        //This is under the presumption that the nodes are not available before its container index
        newLocation = LIST_MAX_NUM_NODES - pList->head->containerIndex;
        printf("New Location: %d\n",newLocation);
        //ISSUE still persists when the node is open and conflicted
        Node* nodeToMove = &currentContainer.availNodes[newLocation];
        currentContainer.usedNodes++;
        nodeToMove->item = pList->current->item;
        nodeToMove->containerIndex = newLocation;
        nodeToMove->boundsEnum = LIST_OOB_START;
        pList->current = nodeToMove;
        printf("new node location: %d\nnew node item: %d\n",nodeToMove->containerIndex,nodeToMove->item);
        return NULL;
    }
    else{
        pList->current = pList->current->prev;
        return pList->current->item;
    }
}

int List_insert_after(List* pList, void* pItem){
    int returnFlag = 0;
    //Check first if we can create nodes
    if(currentContainer.usedNodes <= LIST_MAX_NUM_NODES){
        //Case #1: The current pointer is way past the current head location
        if(pList->current->boundsEnum == LIST_OOB_START){
            returnFlag = List_prepend(pList,pItem);
        }
        //Case #2: It is past the tail's location
        else if(pList->current->boundsEnum == LIST_OOB_END){
            returnFlag = List_append(pList,pItem);
        }
        //Case #3: It is within bounds
        else{
            Node* newNode = &currentContainer.availNodes[currentContainer.usedNodes];
            newNode->containerIndex = currentContainer.usedNodes;
            newNode->index = pList->size;
            newNode->item = pItem;
            currentContainer.usedNodes++;
            pList->size++;
            newNode->prev = pList->current;
            newNode->next = pList->current->next;
            pList->current->next->prev = newNode;
            pList->current->next = newNode;
            returnFlag = 0;
        }
        return returnFlag;
    }
    else{
        return -1;
    }
}

int List_insert_before(List* pList, void* pItem){
    /**
     * Logic for this code should remain the same given that
     * the linkage of items will be different
    */
    int returnFlag = 0;
    //Check first if we can create nodes
    if(currentContainer.usedNodes <= LIST_MAX_NUM_NODES){
        //Case #1: The current pointer is way past the current head location
        if(pList->current->boundsEnum == LIST_OOB_START){
            returnFlag = List_prepend(pList,pItem);
        }
        //Case #2: It is past the tail's location
        else if(pList->current->boundsEnum == LIST_OOB_END){
            returnFlag = List_append(pList,pItem);
        }
        //Case #3: It is within bounds
        else{
            Node* newNode = &currentContainer.availNodes[currentContainer.usedNodes];
            newNode->containerIndex = currentContainer.usedNodes;
            newNode->index = pList->size;
            newNode->item = pItem;
            currentContainer.usedNodes++;
            newNode->prev = pList->current->prev;
            pList->current->prev->next = newNode;
            newNode->next = pList->current;
            pList->current->prev = newNode;
            pList->size++;
            returnFlag = 0;
        }
        return returnFlag;
    }
    else{
        return -1;
    }
}

void* List_remove(List* pList){
    int containerIndex = 0;
    void* currentItem = NULL;
    //ISSUE: We are missing the case where the item is the head or tail
    if(pList->size == 0 || pList->current->boundsEnum == LIST_OOB_END || pList->current->boundsEnum == LIST_OOB_START){
        return NULL;
    }
    //Case #2: The size is 1
    else if(pList->size == 1){
        currentItem = pList->head->item;
        containerIndex = pList->head->containerIndex;
    }
    //Case #3: The size is 2
    else if(pList->size == 2){
        currentItem = pList->tail->item;
        pList->head->next = NULL;
        pList->tail->prev = NULL;
        containerIndex = pList->tail->containerIndex;

    }
    //Case #4: Where the current pointer is not in either ends
    else{
        currentItem = pList->current->item;
        pList->current->prev->next = pList->current->next;
        pList->current->next->prev = pList->current->prev;
        containerIndex = pList->current->containerIndex;
        pList->current = pList->current->prev;
    }
    Node* freeNode = &currentContainer.availNodes[containerIndex];
    freeNode = NULL;
    pList->size--;
    currentContainer.usedNodes--;
    return currentItem;
}

void* List_trim(List* pList){
    Node* givenNode = NULL;
    int containerIndex = pList->tail->containerIndex;
    //Case #1: Empty
    if(pList->size == 0){
        return NULL;
    }
    //Case #2: Size is 1
    else if(pList->size == 1){
        givenNode = pList->head;
        givenNode->item = pList->head->item;
        pList->head = NULL;

    }
    //Case #3: size is 2
    else if(pList->size == 2){
        givenNode = pList->tail;
        givenNode->item = pList->tail->item;
        //Freeing tail node and address
        pList->tail = NULL;
        //Cutting heads next pointer off from th tail
        pList->head->next = NULL;
    }
    //Case #4: size is more than 2
    else{
        givenNode = pList->tail;
        pList->tail = pList->tail->prev;
    }
    void* item = givenNode->item;
    Node* freeNode = &currentContainer.availNodes[givenNode->containerIndex];
    freeNode->next = NULL;
    freeNode->prev = NULL;
    freeNode->item = NULL;
    freeNode = NULL;
    pList->size--;
    currentContainer.usedNodes--;
    return item;
}

void List_concat(List* pList1, List* pList2){

    //Attach the first lists tail to the second lists head
    //Set the new tail of the first list to the current tail of the 2nd list
    //Then set the new size to the addition of both lists size
    pList1->tail->next = pList2->head;
    pList2->head->prev = pList1->tail;
    pList1->tail = pList2->tail;
    pList1->size = pList1->size + pList2->size;
    pList2 = NULL;
}

void setNodeIntAttrToZero(Node* givenNode){
    givenNode->containerIndex = 0;
    givenNode->index = 0;
    givenNode->boundsEnum = 0;
}

typedef void (*FREEN_FN)(void* pItem);
void List_free(List* pList, FREEN_FN pItemFreeFn){
    int incrementor = 0;
    Node* listHead = pList->head;
    while(listHead->next != NULL ){
        printf("Freeing item stored at list index: %d\nAnd container index: %d\n",listHead->index,listHead->containerIndex);
        (*pItemFreeFn)(listHead->item);
        setNodeIntAttrToZero(listHead);
        listHead = listHead->next;
        listHead->prev = NULL;
        pList->size--;
        currentContainer.usedNodes--;
        // Debugging comment to determine the next node that is being freed
        // printf("Now freeing node at: %d\n", listHead->index);
    }
    // Debugging comment if the list is truly freed from the memory
    // printf("The size of pList is: %d\n",pList->size);

    //My code iterates from head and to 2nd last node before tail so manually have to free the tail

    //Making sure that there are no connections left and if there are any for the tail, set it to null
    pList->tail->next = NULL;
    pList->tail->prev = NULL;

    //Freeing the tail item and setting it's attirbutes to 0
    (*pItemFreeFn)(pList->tail->item);
    setNodeIntAttrToZero(pList->tail);
    pList->tail = NULL;

    //Freeing head connections from the memory and freeing anything stored in the item, then finally decreasing size
    pList->head->next = NULL;
    pList->head->prev = NULL;
    (*pItemFreeFn)(pList->head->item);
    pList->head = NULL;
    pList->size--;

    pList->current = NULL;
    pList = NULL;

    currentContainer.usedHeads--;
}

typedef bool (*COMPARTOR_FN)(void* pItem, void* pComparisonArg);
void* List_search(List* pList, COMPARATOR_FN pComparator, void* pComparisonArg){
    //First step: Initializing the location of the iterator, depends on where the current starts
    Node* iterator;
    int incrementor = 0;
    printf("List current index: %d\nAnd has item stored: %d\n",pList->current->index, pList->current->item);
    if(pList->current->boundsEnum == LIST_OOB_START){
        iterator = pList->head;
    }
    else{
        iterator = pList->current;
    }
    while(iterator->next != NULL && incrementor < pList->size){
        bool isItemFound = (*pComparator)(iterator->item,pComparisonArg);
        //  0 if they dont match, 1 if they do match, for other cases the comparator returns any odd number
        //  Return null
        if(isItemFound == false){
            iterator = iterator->next;
            incrementor++;
        }
        //It is found
        else if(isItemFound == true){
            pList->current = iterator;
            return iterator->item;
        }
        else{
            return NULL;
        }
    }
}