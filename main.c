#include "List.h"
#include <stdio.h>


void freeItem(void* item){
    if(sizeof(item) == sizeof(int)){
        int free = &item;
        free = 0;
        item = NULL;
    }
}

bool intComparator(void* firstItem, void* secondItem){
    printf("item 1: %d\n",firstItem);
    printf("item 2: %d\n",secondItem);
    return firstItem == secondItem ? true : false;
    // if(sizeof(firstItem) == sizeof(int) && secondItem == sizeof(int)){
    //     int firstArg = firstItem;
    //     int secondArg = secondItem;
    //     return firstArg == secondArg ? true : false;
    // }
    // else{
    //     return false;
    // }
}

int main(){

    printf("Test\n");

    List* newList = List_create();
    printf("The current size of the newList is: %d\n", newList->size);
    List_append(newList,64);
    printf("The current node's item is: %d\n",newList->current->item);
    List_append(newList,50);
    printf("The current node's item is: %d\n",newList->current->item);
    List_append(newList,2);
    printf("The current node's item is: %d\n",newList->current->item);
    List_append(newList,99);
    printf("The current node's item is: %d\n",newList->current->item);
    List_append(newList,88);
    printf("The current node's item is: %d\n",newList->current->item);
    Node* debugNode = newList->head;
    int increment = 0;
    printf("The current head item is: %d\n", debugNode->item);
    
    printf("*-----------------------NEW LIST WHILE LOOP----------------------------------*\n");

    while(debugNode->next != NULL && debugNode->item != NULL && increment < newList->size){
        printf("Current Item In The Node: %d\n",debugNode->item);
        printf("The Container index of the node is: %d\n",debugNode->containerIndex);
        printf("The index of the node in the list: %d\n", debugNode->index);
        debugNode = debugNode->next;
        increment++;
    };
    printf("The tail item is: %d\n",newList->tail->item);
    printf("The size of the list now is: %d\n", newList->size);

    printf("*-----------------------PRE LIST CREATION----------------------------------*\n");

    List* preList = List_create();
    printf("The prepend list has: %d\n", preList->size);
    List_prepend(preList,22);
    printf("The current node's item is: %d\n",preList->current->item);
    List_prepend(preList,33);
    printf("The current node's item is: %d\n",preList->current->item);
    printf("The list head contains: %d\nThe list tail contains: %d\n",preList->head->item,preList->tail->item);
    printf("The container index of both head and tail is: %d %d\nTheir respective list index is: %d %d\n",preList->head->containerIndex,preList->tail->containerIndex,preList->head->index,preList->tail->index);
    printf("The preList now has the size: %d\n",preList->size);

    List_append(preList,5);
    List_append(preList,10);
    List_append(preList,15);

    printf("Size after the append is: %d\n",preList->size);

    Node* debugNodeA = preList->head;
    int secondIncrement = 0;

    printf("*-----------------------PRE LIST WHILE LOOP----------------------------------*\n");

    while(debugNodeA->next != NULL && debugNodeA->item != NULL && secondIncrement < preList->size){
        printf("Current Item In The Node: %d\n",debugNodeA->item);
        printf("The Container index of the node is: %d\n",debugNodeA->containerIndex);
        printf("The index of the node in the list: %d\n", debugNodeA->index);
        debugNodeA = debugNodeA->next;
        secondIncrement++;
    };

    printf("*-----------------------PRE LIST NEXT & PREV FUNCTION----------------------------------*\n");

    printf("*-----------------------PRE LIST NEXT---------------------------------*\n");

    printf("The current item before the list next function is: %d\n",preList->current->item);

    void* nextItem = List_next(preList);
    if(nextItem == NULL){
        printf("The item has been moved far from the end of the list!\n");
        printf("The current node is stored at location: %d\nAnd has the item: %d\n",preList->current->containerIndex,preList->current->item);
        printf("The status of is in bounds = %d\n",preList->current->boundsEnum);
    }
    else{
        printf("The next item on preList is: %d\n",List_next(preList));
    }

    List_prepend(preList,20);
    List_prepend(preList,100);

    printf("*-----------------------PRE LIST FUNCTION----------------------------------*\n");

    void* prevItem = List_prev(preList);
    if(prevItem == NULL){
        printf("The item has been moved far from the start of the list!\n");
        printf("The current node is stored at location: %d\nAnd has the item: %d\n",preList->current->containerIndex,preList->current->item);
        printf("The status of is in bounds = %d\n",preList->current->boundsEnum);
    }
    else{
        printf("The next item on preList is: %d\n",List_prev(preList));
    }

    prevItem = NULL;
    
    printf("Case #1 for List insert after\n");

    List_prepend(preList,444);
    prevItem = List_prev(preList);
    if(prevItem == NULL){
        printf("The item has been moved far from the start of the list!\n");
        printf("The current node is stored at location: %d\nAnd has the item: %d\n",preList->current->containerIndex,preList->current->item);
        printf("The status of is in bounds = %d\n",preList->current->boundsEnum);
    }
    else{
        printf("The next item on preList is: %d\n",List_prev(preList));
    }
    int status = List_insert_after(preList,999);

    if(status == 0){
        printf("Success! Case #1 has been done successfully\n");
        printf("The current node is stored at location: %d\nAnd has the item: %d\n",preList->current->containerIndex,preList->current->item);
        printf("The status of is in bounds = %d\n",preList->current->boundsEnum);
    }

    printf("Case #2 for List insert after\n");

    List_append(preList,1);
    prevItem = NULL;
    prevItem = List_next(preList);
    if(prevItem == NULL){
        printf("The item has been moved far from the start of the list!\n");
        printf("The current node is stored at location: %d\nAnd has the item: %d\n",preList->current->containerIndex,preList->current->item);
        printf("The status of is in bounds = %d\n",preList->current->boundsEnum);
    }
    else{
        printf("The next item on preList is: %d\n",List_prev(preList));
    }

    status = List_insert_after(preList,12);

    if(status == 0){
        printf("Success! Case #2 returned successfully\n");
        printf("The current node is stored at location: %d\nAnd has the item: %d\n",preList->current->containerIndex,preList->current->item);
        printf("The status of is in bounds = %d\n",preList->current->boundsEnum);
    }

    printf("Case 3 for List insert after\n");

    List_append(preList,12);
    List_append(preList,44);
    List_append(preList,21);

    List_prev(preList);

    status = List_insert_after(preList,23);

    if(status == 0){
        printf("Success! Case #3 has returned successfully\n");
        printf("The current node is stored at location: %d\nAnd has the item: %d\n",preList->current->containerIndex,preList->current->item);
        printf("The status of is in bounds = %d\n",preList->current->boundsEnum);
    }

    printf("*-----------------------PRE LIST Loop Function----------------------------------*\n");

    debugNodeA = preList->head;
    secondIncrement = 0;


    while(debugNodeA->next != NULL && debugNodeA->item != NULL && secondIncrement < preList->size){
        printf("Current Item In The Node: %d\n",debugNodeA->item);
        printf("The Container index of the node is: %d\n",debugNodeA->containerIndex);
        printf("The index of the node in the list: %d\n", debugNodeA->index);
        debugNodeA = debugNodeA->next;
        secondIncrement++;
    };

    void* item = List_remove(preList);

    if(item == NULL){
        printf("The item cannot be removed!\n");
    }
    else{
        printf("The current item popped is: %d\n",item);
    }

    item = NULL;

    printf("preList tail: %d\n", preList->tail->item);

    item = List_trim(preList);
    
    if(item == NULL){
        printf("ERROR! the remove function was not successful\n");
    }
    else{
        printf("Success! The item removed was: %d\n",item);
    }

    printf("The size of the old pre list is: %d\n",preList->size);

    List_concat(preList,newList);

    printf("The new list is: \n");

    printf("*-----------------------PRE LIST Concat Loop Function----------------------------------*\n");

    debugNodeA = preList->head;
    secondIncrement = 0;


    while(debugNodeA->next != NULL && secondIncrement < preList->size){
        printf("Current Item In The Node: %d\n",debugNodeA->item);
        printf("The index of the node in the list: %d\n", debugNodeA->index);
        debugNodeA = debugNodeA->next;
        secondIncrement++;
    };
    printf("\n");

    printf("The new size of preList is: %d\n",preList->size);

    printf("*-----------------------Searching Pre List----------------------------------*\n");
    printf("\n");
    printf("*-----------------------Case #1: The Item is found----------------------------------*\n");
    item = List_search(preList,intComparator,23);
    if(item == NULL){
        printf("The item has not been found! starting from the given current\n");
    }
    else{
        printf("The item was found and is: %d\n",item);
    }
    printf("\n");
    printf("*-----------------------Case #2: The Item is not found----------------------------------*\n");
    item = List_search(preList,intComparator,9423);
    if(item == NULL){
        printf("The item has not been found! starting from the given current\n");
    }
    else{
        printf("The item was found and is: %d\n",item);
    }
    printf("*-----------------------Freeing Pre List----------------------------------*\n");

    List_free(preList,freeItem);

    if(preList->size == 0 && preList->head == NULL && preList->tail == NULL){
        printf("The list has been cleared and free to use!\n");
    }

    return 0;
}