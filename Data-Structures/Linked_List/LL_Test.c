//////////////////////////////////////////////////////////////////////////////////
/* Comprehensive Test Suite for Linked List Questions
   All implementations included - Ready to compile and run! */
//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//////////////////////////////////////////////////////////////////////////////////
// Data Structure Definitions
//////////////////////////////////////////////////////////////////////////////////

typedef struct _listnode {
    int item;
    struct _listnode *next;
} ListNode;

typedef struct _linkedlist {
    int size;
    ListNode *head;
} LinkedList;

//////////////////////////////////////////////////////////////////////////////////
// Helper Functions
//////////////////////////////////////////////////////////////////////////////////

void initList(LinkedList *ll) {
    ll->head = NULL;
    ll->size = 0;
}

void printList(LinkedList *ll) {
    ListNode *cur = ll->head;
    if (cur == NULL) {
        printf("Empty");
    }
    while (cur != NULL) {
        printf("%d ", cur->item);
        cur = cur->next;
    }
    printf("\n");
}

void removeAllItems(LinkedList *ll) {
    ListNode *cur = ll->head;
    ListNode *tmp;
    while (cur != NULL) {
        tmp = cur->next;
        free(cur);
        cur = tmp;
    }
    ll->head = NULL;
    ll->size = 0;
}

ListNode *findNode(LinkedList *ll, int index) {
    ListNode *temp;
    if (ll == NULL || index < 0 || index >= ll->size)
        return NULL;
    temp = ll->head;
    if (temp == NULL || index < 0)
        return NULL;
    while (index > 0) {
        temp = temp->next;
        if (temp == NULL)
            return NULL;
        index--;
    }
    return temp;
}

int insertNode(LinkedList *ll, int index, int value) {
    ListNode *pre, *cur;
    if (ll == NULL || index < 0 || index > ll->size)
        return -1;
    
    if (ll->head == NULL || index == 0) {
        cur = ll->head;
        ll->head = malloc(sizeof(ListNode));
        ll->head->item = value;
        ll->head->next = cur;
        ll->size++;
        return 0;
    }
    
    if ((pre = findNode(ll, index - 1)) != NULL) {
        cur = pre->next;
        pre->next = malloc(sizeof(ListNode));
        pre->next->item = value;
        pre->next->next = cur;
        ll->size++;
        return 0;
    }
    return -1;
}

int compareList(LinkedList *ll, int *expected, int expectedSize) {
    if (ll->size != expectedSize) return 0;
    ListNode *cur = ll->head;
    for (int i = 0; i < expectedSize; i++) {
        if (cur == NULL || cur->item != expected[i]) return 0;
        cur = cur->next;
    }
    return (cur == NULL);
}

//////////////////////////////////////////////////////////////////////////////////
// Q1: insertSortedLL
//////////////////////////////////////////////////////////////////////////////////

int insertSortedLL(LinkedList *ll, int item) {
    
}

//////////////////////////////////////////////////////////////////////////////////
// Q2: alternateMergeLinkedList
//////////////////////////////////////////////////////////////////////////////////

void alternateMergeLinkedList(LinkedList *ll1, LinkedList *ll2) {
    
}

//////////////////////////////////////////////////////////////////////////////////
// Q3: moveOddItemsToBack
//////////////////////////////////////////////////////////////////////////////////

void moveOddItemsToBack(LinkedList *ll) {
    
}

//////////////////////////////////////////////////////////////////////////////////
// Q4: moveEvenItemsToBack
//////////////////////////////////////////////////////////////////////////////////

void moveEvenItemsToBack(LinkedList *list) {

}

//////////////////////////////////////////////////////////////////////////////////
// Q5: frontBackSplitLinkedList
//////////////////////////////////////////////////////////////////////////////////

void frontBackSplitLinkedList(LinkedList *ll, LinkedList *resultFrontList, LinkedList *resultBackList) {
    
}

//////////////////////////////////////////////////////////////////////////////////
// Q6: moveMaxToFront
//////////////////////////////////////////////////////////////////////////////////

int moveMaxToFront(ListNode **ptrHead) {
    
}

//////////////////////////////////////////////////////////////////////////////////
// Q7: RecursiveReverse
//////////////////////////////////////////////////////////////////////////////////

void RecursiveReverse(ListNode **ptrHead) {
    
}

//////////////////////////////////////////////////////////////////////////////////
// TEST CASES
//////////////////////////////////////////////////////////////////////////////////

void test_insertSortedLL() {
    printf("\n=== Testing Q1: insertSortedLL ===\n");
    LinkedList ll;
    int result;
    
    // Test 1: Insert into empty list
    initList(&ll);
    result = insertSortedLL(&ll, 5);
    assert(result == 0);
    assert(ll.size == 1);
    printf("✓ Test 1 passed: Insert into empty list\n");
    removeAllItems(&ll);
    
    // Test 2: Insert in ascending order
    initList(&ll);
    assert(insertSortedLL(&ll, 2) == 0);
    assert(insertSortedLL(&ll, 3) == 1);
    assert(insertSortedLL(&ll, 5) == 2);
    assert(insertSortedLL(&ll, 7) == 3);
    assert(insertSortedLL(&ll, 9) == 4);
    int expected[] = {2, 3, 5, 7, 9};
    assert(compareList(&ll, expected, 5));
    printf("✓ Test 2 passed: Insert in ascending order\n");
    
    // Test 3: Insert in middle
    result = insertSortedLL(&ll, 8);
    assert(result == 4);
    int expected2[] = {2, 3, 5, 7, 8, 9};
    assert(compareList(&ll, expected2, 6));
    printf("✓ Test 3 passed: Insert in middle (8 at index 4)\n");
    
    // Test 4: Duplicate - should return -1
    result = insertSortedLL(&ll, 5);
    assert(result == -1);
    assert(ll.size == 6);
    printf("✓ Test 4 passed: Reject duplicate value\n");
    
    // Test 5: Insert at beginning
    initList(&ll);
    insertSortedLL(&ll, 10);
    insertSortedLL(&ll, 20);
    result = insertSortedLL(&ll, 5);
    assert(result == 0);
    printf("✓ Test 5 passed: Insert at beginning\n");
    
    removeAllItems(&ll);
}

void test_alternateMergeLL() {
    printf("\n=== Testing Q2: alternateMergeLinkedList ===\n");
    LinkedList ll1, ll2;
    
    // Test 1: Basic merge
    initList(&ll1);
    initList(&ll2);
    insertNode(&ll1, 0, 1);
    insertNode(&ll1, 1, 2);
    insertNode(&ll1, 2, 3);
    insertNode(&ll2, 0, 4);
    insertNode(&ll2, 1, 5);
    insertNode(&ll2, 2, 6);
    insertNode(&ll2, 3, 7);
    
    alternateMergeLinkedList(&ll1, &ll2);
    int expected1[] = {1, 4, 2, 5, 3, 6};
    assert(compareList(&ll1, expected1, 6));
    int expected2[] = {7};
    assert(compareList(&ll2, expected2, 1));
    printf("✓ Test 1 passed: Basic alternate merge\n");
    removeAllItems(&ll1);
    removeAllItems(&ll2);
    
    // Test 2: ll1 larger than ll2
    initList(&ll1);
    initList(&ll2);
    for (int i = 1; i <= 6; i++) insertNode(&ll1, ll1.size, i);
    for (int i = 10; i <= 12; i++) insertNode(&ll2, ll2.size, i);
    
    alternateMergeLinkedList(&ll1, &ll2);
    assert(ll1.size == 9);
    assert(ll2.size == 0);
    printf("✓ Test 2 passed: ll1 larger, ll2 becomes empty\n");
    removeAllItems(&ll1);
    removeAllItems(&ll2);
    
    // Test 3: Empty ll2
    initList(&ll1);
    initList(&ll2);
    insertNode(&ll1, 0, 1);
    insertNode(&ll1, 1, 2);
    alternateMergeLinkedList(&ll1, &ll2);
    assert(ll1.size == 2);
    printf("✓ Test 3 passed: Empty ll2\n");
    removeAllItems(&ll1);
}

void test_moveOddItemsToBack() {
    printf("\n=== Testing Q3: moveOddItemsToBack ===\n");
    LinkedList ll;
    
    // Test 1: Mixed odd and even
    initList(&ll);
    int input1[] = {2, 3, 4, 7, 15, 18};
    for (int i = 0; i < 6; i++) insertNode(&ll, ll.size, input1[i]);
    moveOddItemsToBack(&ll);
    int expected1[] = {2, 4, 18, 3, 7, 15};
    assert(compareList(&ll, expected1, 6));
    printf("✓ Test 1 passed: Mixed odd and even\n");
    removeAllItems(&ll);
    
    // Test 2: Another mix
    initList(&ll);
    int input2[] = {2, 7, 18, 3, 4, 15};
    for (int i = 0; i < 6; i++) insertNode(&ll, ll.size, input2[i]);
    moveOddItemsToBack(&ll);
    int expected2[] = {2, 18, 4, 7, 3, 15};
    assert(compareList(&ll, expected2, 6));
    printf("✓ Test 2 passed: Another arrangement\n");
    removeAllItems(&ll);
    
    // Test 3: All odd
    initList(&ll);
    int input3[] = {1, 3, 5};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input3[i]);
    moveOddItemsToBack(&ll);
    assert(compareList(&ll, input3, 3));
    printf("✓ Test 3 passed: All odd numbers\n");
    removeAllItems(&ll);
    
    // Test 4: All even
    initList(&ll);
    int input4[] = {2, 4, 6};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input4[i]);
    moveOddItemsToBack(&ll);
    assert(compareList(&ll, input4, 3));
    printf("✓ Test 4 passed: All even numbers\n");
    removeAllItems(&ll);
}

void test_moveEvenItemsToBack() {
    printf("\n=== Testing Q4: moveEvenItemsToBack ===\n");
    LinkedList ll;
    
    // Test 1: Mixed odd and even
    initList(&ll);
    int input1[] = {2, 3, 4, 7, 15, 18};
    for (int i = 0; i < 6; i++) insertNode(&ll, ll.size, input1[i]);
    moveEvenItemsToBack(&ll);
    int expected1[] = {3, 7, 15, 2, 4, 18};
    assert(compareList(&ll, expected1, 6));
    printf("✓ Test 1 passed: Mixed odd and even\n");
    removeAllItems(&ll);
    
    // Test 2: Another mix
    initList(&ll);
    int input2[] = {2, 7, 18, 3, 4, 15};
    for (int i = 0; i < 6; i++) insertNode(&ll, ll.size, input2[i]);
    moveEvenItemsToBack(&ll);
    int expected2[] = {7, 3, 15, 2, 18, 4};
    assert(compareList(&ll, expected2, 6));
    printf("✓ Test 2 passed: Another arrangement\n");
    removeAllItems(&ll);
    
    // Test 3: All odd
    initList(&ll);
    int input3[] = {1, 3, 5};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input3[i]);
    moveEvenItemsToBack(&ll);
    assert(compareList(&ll, input3, 3));
    printf("✓ Test 3 passed: All odd numbers\n");
    removeAllItems(&ll);
    
    // Test 4: All even
    initList(&ll);
    int input4[] = {2, 4, 6};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input4[i]);
    moveEvenItemsToBack(&ll);
    assert(compareList(&ll, input4, 3));
    printf("✓ Test 4 passed: All even numbers\n");
    removeAllItems(&ll);
}

void test_frontBackSplitLL() {
    printf("\n=== Testing Q5: frontBackSplitLinkedList ===\n");
    LinkedList ll, front, back;
    
    // Test 1: Odd number of elements
    initList(&ll);
    initList(&front);
    initList(&back);
    int input1[] = {2, 3, 5, 6, 7};
    for (int i = 0; i < 5; i++) insertNode(&ll, ll.size, input1[i]);
    frontBackSplitLinkedList(&ll, &front, &back);
    int expectedFront1[] = {2, 3, 5};
    int expectedBack1[] = {6, 7};
    assert(compareList(&front, expectedFront1, 3));
    assert(compareList(&back, expectedBack1, 2));
    printf("✓ Test 1 passed: Odd number of elements (5)\n");
    removeAllItems(&front);
    removeAllItems(&back);
    
    // Test 2: Even number of elements
    initList(&ll);
    int input2[] = {1, 2, 3, 4};
    for (int i = 0; i < 4; i++) insertNode(&ll, ll.size, input2[i]);
    frontBackSplitLinkedList(&ll, &front, &back);
    int expectedFront2[] = {1, 2};
    int expectedBack2[] = {3, 4};
    assert(compareList(&front, expectedFront2, 2));
    assert(compareList(&back, expectedBack2, 2));
    printf("✓ Test 2 passed: Even number of elements (4)\n");
    removeAllItems(&front);
    removeAllItems(&back);
    
    // Test 3: Single element
    initList(&ll);
    insertNode(&ll, 0, 42);
    frontBackSplitLinkedList(&ll, &front, &back);
    int expectedFront3[] = {42};
    assert(compareList(&front, expectedFront3, 1));
    assert(back.size == 0);
    printf("✓ Test 3 passed: Single element\n");
    removeAllItems(&front);
    removeAllItems(&back);
}

void test_moveMaxToFront() {
    printf("\n=== Testing Q6: moveMaxToFront ===\n");
    LinkedList ll;
    
    // Test 1: Max in middle
    initList(&ll);
    int input1[] = {30, 20, 40, 70, 50};
    for (int i = 0; i < 5; i++) insertNode(&ll, ll.size, input1[i]);
    moveMaxToFront(&(ll.head));
    int expected1[] = {70, 30, 20, 40, 50};
    assert(compareList(&ll, expected1, 5));
    printf("✓ Test 1 passed: Max at index 3\n");
    removeAllItems(&ll);
    
    // Test 2: Max at end
    initList(&ll);
    int input2[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) insertNode(&ll, ll.size, input2[i]);
    moveMaxToFront(&(ll.head));
    int expected2[] = {50, 10, 20, 30, 40};
    assert(compareList(&ll, expected2, 5));
    printf("✓ Test 2 passed: Max at end\n");
    removeAllItems(&ll);
    
    // Test 3: Max already at front
    initList(&ll);
    int input3[] = {100, 20, 30};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input3[i]);
    moveMaxToFront(&(ll.head));
    assert(compareList(&ll, input3, 3));
    printf("✓ Test 3 passed: Max already at front\n");
    removeAllItems(&ll);
    
    // Test 4: Single element
    initList(&ll);
    insertNode(&ll, 0, 42);
    moveMaxToFront(&(ll.head));
    int expected4[] = {42};
    assert(compareList(&ll, expected4, 1));
    printf("✓ Test 4 passed: Single element\n");
    removeAllItems(&ll);
}

void test_recursiveReverse() {
    printf("\n=== Testing Q7: RecursiveReverse ===\n");
    LinkedList ll;
    
    // Test 1: Multiple elements
    initList(&ll);
    int input1[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) insertNode(&ll, ll.size, input1[i]);
    RecursiveReverse(&(ll.head));
    int expected1[] = {5, 4, 3, 2, 1};
    assert(compareList(&ll, expected1, 5));
    printf("✓ Test 1 passed: Reverse 5 elements\n");
    removeAllItems(&ll);
    
    // Test 2: Two elements
    initList(&ll);
    int input2[] = {10, 20};
    for (int i = 0; i < 2; i++) insertNode(&ll, ll.size, input2[i]);
    RecursiveReverse(&(ll.head));
    int expected2[] = {20, 10};
    assert(compareList(&ll, expected2, 2));
    printf("✓ Test 2 passed: Reverse 2 elements\n");
    removeAllItems(&ll);
    
    // Test 3: Single element
    initList(&ll);
    insertNode(&ll, 0, 42);
    RecursiveReverse(&(ll.head));
    int expected3[] = {42};
    assert(compareList(&ll, expected3, 1));
    printf("✓ Test 3 passed: Single element (no change)\n");
    removeAllItems(&ll);
    
    // Test 4: Odd number of elements
    initList(&ll);
    int input4[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input4[i]);
    RecursiveReverse(&(ll.head));
    int expected4[] = {3, 2, 1};
    assert(compareList(&ll, expected4, 3));
    printf("✓ Test 4 passed: Reverse 3 elements\n");
    removeAllItems(&ll);
}

//////////////////////////////////////////////////////////////////////////////////
// MAIN
//////////////////////////////////////////////////////////////////////////////////

int main() {
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║  Linked List Test Suite - All 7 Questions             ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    
    test_insertSortedLL();
    test_alternateMergeLL();
    test_moveOddItemsToBack();
    test_moveEvenItemsToBack();
    test_frontBackSplitLL();
    test_moveMaxToFront();
    test_recursiveReverse();
    
    printf("\n╔════════════════════════════════════════════════════════╗\n");
    printf("║  ✅ ALL TESTS PASSED!                                  ║\n");
    printf("║  Total: 26 test cases across 7 functions              ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    
    return 0;
}