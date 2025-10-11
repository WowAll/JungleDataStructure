//////////////////////////////////////////////////////////////////////////////////
/* Enhanced Linked List Test Suite with Detailed Error Reporting */
//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////
// Test Statistics
//////////////////////////////////////////////////////////////////////////////////

typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
} TestStats;

TestStats global_stats = {0, 0, 0};

//////////////////////////////////////////////////////////////////////////////////
// Enhanced Assertion Macros
//////////////////////////////////////////////////////////////////////////////////

#define TEST_ASSERT_INT_EQ(actual, expected, test_name) do { \
    global_stats.total_tests++; \
    if ((actual) != (expected)) { \
        global_stats.failed_tests++; \
        printf("❌ FAILED: %s\n", test_name); \
        printf("   Expected: %d\n", (expected)); \
        printf("   Actual:   %d\n", (actual)); \
        printf("   Location: Line %d\n", __LINE__); \
        return; \
    } else { \
        global_stats.passed_tests++; \
        printf("✓ %s\n", test_name); \
    } \
} while(0)

//////////////////////////////////////////////////////////////////////////////////
// Data Structures
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

void printListArray(LinkedList *ll) {
    ListNode *cur = ll->head;
    printf("[");
    while (cur != NULL) {
        printf("%d", cur->item);
        if (cur->next) printf(", ");
        cur = cur->next;
    }
    printf("]");
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

int compareListWithDetails(LinkedList *ll, int *expected, int expectedSize, const char *test_name) {
    global_stats.total_tests++;
    
    if (ll->size != expectedSize) {
        global_stats.failed_tests++;
        printf("❌ FAILED: %s\n", test_name);
        printf("   Size mismatch!\n");
        printf("   Expected size: %d\n", expectedSize);
        printf("   Actual size:   %d\n", ll->size);
        printf("   Expected: ");
        for (int i = 0; i < expectedSize; i++) {
            if (i == 0) printf("[");
            printf("%d", expected[i]);
            if (i < expectedSize - 1) printf(", ");
            else printf("]\n");
        }
        printf("   Actual:   ");
        printListArray(ll);
        printf("\n");
        return 0;
    }
    
    ListNode *cur = ll->head;
    for (int i = 0; i < expectedSize; i++) {
        if (cur == NULL) {
            global_stats.failed_tests++;
            printf("❌ FAILED: %s\n", test_name);
            printf("   List ended prematurely at index %d\n", i);
            return 0;
        }
        
        if (cur->item != expected[i]) {
            global_stats.failed_tests++;
            printf("❌ FAILED: %s\n", test_name);
            printf("   Mismatch at index %d\n", i);
            printf("   Expected value: %d\n", expected[i]);
            printf("   Actual value:   %d\n", cur->item);
            printf("   Expected: ");
            for (int j = 0; j < expectedSize; j++) {
                if (j == 0) printf("[");
                printf("%d", expected[j]);
                if (j < expectedSize - 1) printf(", ");
                else printf("]\n");
            }
            printf("   Actual:   ");
            printListArray(ll);
            printf("\n");
            return 0;
        }
        cur = cur->next;
    }
    
    if (cur != NULL) {
        global_stats.failed_tests++;
        printf("❌ FAILED: %s\n", test_name);
        printf("   List has extra elements beyond expected size\n");
        return 0;
    }
    
    global_stats.passed_tests++;
    printf("✓ %s\n", test_name);
    return 1;
}

//////////////////////////////////////////////////////////////////////////////////
// Function Prototypes
//////////////////////////////////////////////////////////////////////////////////

int insertSortedLL(LinkedList *ll, int item);
void alternateMergeLinkedList(LinkedList *ll1, LinkedList *ll2);
void moveOddItemsToBack(LinkedList *ll);
void moveEvenItemsToBack(LinkedList *list);
void frontBackSplitLinkedList(LinkedList *ll, LinkedList *resultFrontList, LinkedList *resultBackList);
int moveMaxToFront(ListNode **ptrHead);
void RecursiveReverse(ListNode **ptrHead);

//////////////////////////////////////////////////////////////////////////////////
// TEST CASES
//////////////////////////////////////////////////////////////////////////////////

void test_insertSortedLL() {
    printf("\n=== Testing Q1: insertSortedLL ===\n");
    LinkedList ll;
    int result;
    
    // Test 1
    initList(&ll);
    result = insertSortedLL(&ll, 5);
    TEST_ASSERT_INT_EQ(result, 0, "Test 1: Insert into empty list - return value");
    TEST_ASSERT_INT_EQ(ll.size, 1, "Test 1: Insert into empty list - size check");
    removeAllItems(&ll);
    
    // Test 2
    initList(&ll);
    insertSortedLL(&ll, 2);
    insertSortedLL(&ll, 3);
    insertSortedLL(&ll, 5);
    insertSortedLL(&ll, 7);
    insertSortedLL(&ll, 9);
    int expected[] = {2, 3, 5, 7, 9};
    compareListWithDetails(&ll, expected, 5, "Test 2: Insert in ascending order");
    
    // Test 3
    result = insertSortedLL(&ll, 8);
    TEST_ASSERT_INT_EQ(result, 4, "Test 3: Insert in middle - return value");
    int expected2[] = {2, 3, 5, 7, 8, 9};
    compareListWithDetails(&ll, expected2, 6, "Test 3: Insert in middle - list check");
    
    // Test 4
    result = insertSortedLL(&ll, 5);
    TEST_ASSERT_INT_EQ(result, -1, "Test 4: Reject duplicate value");
    TEST_ASSERT_INT_EQ(ll.size, 6, "Test 4: Size unchanged after duplicate");
    
    // Test 5
    initList(&ll);
    insertSortedLL(&ll, 10);
    insertSortedLL(&ll, 20);
    result = insertSortedLL(&ll, 5);
    TEST_ASSERT_INT_EQ(result, 0, "Test 5: Insert at beginning");
    
    removeAllItems(&ll);
}

void test_alternateMergeLL() {
    printf("\n=== Testing Q2: alternateMergeLinkedList ===\n");
    LinkedList ll1, ll2;
    
    // Test 1
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
    int expected2[] = {7};
    compareListWithDetails(&ll1, expected1, 6, "Test 1: Basic merge - ll1");
    compareListWithDetails(&ll2, expected2, 1, "Test 1: Basic merge - ll2");
    removeAllItems(&ll1);
    removeAllItems(&ll2);
    
    // Test 2
    initList(&ll1);
    initList(&ll2);
    for (int i = 1; i <= 6; i++) insertNode(&ll1, ll1.size, i);
    for (int i = 10; i <= 12; i++) insertNode(&ll2, ll2.size, i);
    
    alternateMergeLinkedList(&ll1, &ll2);
    TEST_ASSERT_INT_EQ(ll1.size, 9, "Test 2: ll1 larger - size check");
    TEST_ASSERT_INT_EQ(ll2.size, 0, "Test 2: ll2 becomes empty");
    removeAllItems(&ll1);
    removeAllItems(&ll2);
    
    // Test 3
    initList(&ll1);
    initList(&ll2);
    insertNode(&ll1, 0, 1);
    insertNode(&ll1, 1, 2);
    alternateMergeLinkedList(&ll1, &ll2);
    TEST_ASSERT_INT_EQ(ll1.size, 2, "Test 3: Empty ll2 - no change");
    removeAllItems(&ll1);
}

void test_moveOddItemsToBack() {
    printf("\n=== Testing Q3: moveOddItemsToBack ===\n");
    LinkedList ll;
    
    // Test 1
    initList(&ll);
    int input1[] = {2, 3, 4, 7, 15, 18};
    for (int i = 0; i < 6; i++) insertNode(&ll, ll.size, input1[i]);
    moveOddItemsToBack(&ll);
    int expected1[] = {2, 4, 18, 3, 7, 15};
    compareListWithDetails(&ll, expected1, 6, "Test 1: Mixed odd and even");
    removeAllItems(&ll);
    
    // Test 2
    initList(&ll);
    int input2[] = {2, 7, 18, 3, 4, 15};
    for (int i = 0; i < 6; i++) insertNode(&ll, ll.size, input2[i]);
    moveOddItemsToBack(&ll);
    int expected2[] = {2, 18, 4, 7, 3, 15};
    compareListWithDetails(&ll, expected2, 6, "Test 2: Another arrangement");
    removeAllItems(&ll);
    
    // Test 3
    initList(&ll);
    int input3[] = {1, 3, 5};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input3[i]);
    moveOddItemsToBack(&ll);
    compareListWithDetails(&ll, input3, 3, "Test 3: All odd numbers");
    removeAllItems(&ll);
    
    // Test 4
    initList(&ll);
    int input4[] = {2, 4, 6};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input4[i]);
    moveOddItemsToBack(&ll);
    compareListWithDetails(&ll, input4, 3, "Test 4: All even numbers");
    removeAllItems(&ll);
}

void test_moveEvenItemsToBack() {
    printf("\n=== Testing Q4: moveEvenItemsToBack ===\n");
    LinkedList ll;
    
    // Test 1
    initList(&ll);
    int input1[] = {2, 3, 4, 7, 15, 18};
    for (int i = 0; i < 6; i++) insertNode(&ll, ll.size, input1[i]);
    moveEvenItemsToBack(&ll);
    int expected1[] = {3, 7, 15, 2, 4, 18};
    compareListWithDetails(&ll, expected1, 6, "Test 1: Mixed odd and even");
    removeAllItems(&ll);
    
    // Test 2
    initList(&ll);
    int input2[] = {2, 7, 18, 3, 4, 15};
    for (int i = 0; i < 6; i++) insertNode(&ll, ll.size, input2[i]);
    moveEvenItemsToBack(&ll);
    int expected2[] = {7, 3, 15, 2, 18, 4};
    compareListWithDetails(&ll, expected2, 6, "Test 2: Another arrangement");
    removeAllItems(&ll);
    
    // Test 3
    initList(&ll);
    int input3[] = {1, 3, 5};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input3[i]);
    moveEvenItemsToBack(&ll);
    compareListWithDetails(&ll, input3, 3, "Test 3: All odd numbers");
    removeAllItems(&ll);
    
    // Test 4
    initList(&ll);
    int input4[] = {2, 4, 6};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input4[i]);
    moveEvenItemsToBack(&ll);
    compareListWithDetails(&ll, input4, 3, "Test 4: All even numbers");
    removeAllItems(&ll);
}

void test_frontBackSplitLL() {
    printf("\n=== Testing Q5: frontBackSplitLinkedList ===\n");
    LinkedList ll, front, back;
    
    // Test 1
    initList(&ll);
    initList(&front);
    initList(&back);
    int input1[] = {2, 3, 5, 6, 7};
    for (int i = 0; i < 5; i++) insertNode(&ll, ll.size, input1[i]);
    frontBackSplitLinkedList(&ll, &front, &back);
    int expectedFront1[] = {2, 3, 5};
    int expectedBack1[] = {6, 7};
    compareListWithDetails(&front, expectedFront1, 3, "Test 1: Odd elements - front");
    compareListWithDetails(&back, expectedBack1, 2, "Test 1: Odd elements - back");
    removeAllItems(&front);
    removeAllItems(&back);
    
    // Test 2
    initList(&ll);
    int input2[] = {1, 2, 3, 4};
    for (int i = 0; i < 4; i++) insertNode(&ll, ll.size, input2[i]);
    frontBackSplitLinkedList(&ll, &front, &back);
    int expectedFront2[] = {1, 2};
    int expectedBack2[] = {3, 4};
    compareListWithDetails(&front, expectedFront2, 2, "Test 2: Even elements - front");
    compareListWithDetails(&back, expectedBack2, 2, "Test 2: Even elements - back");
    removeAllItems(&front);
    removeAllItems(&back);
    
    // Test 3
    initList(&ll);
    insertNode(&ll, 0, 42);
    frontBackSplitLinkedList(&ll, &front, &back);
    int expectedFront3[] = {42};
    compareListWithDetails(&front, expectedFront3, 1, "Test 3: Single element - front");
    TEST_ASSERT_INT_EQ(back.size, 0, "Test 3: Single element - back empty");
    removeAllItems(&front);
    removeAllItems(&back);
}

void test_moveMaxToFront() {
    printf("\n=== Testing Q6: moveMaxToFront ===\n");
    LinkedList ll;
    
    // Test 1
    initList(&ll);
    int input1[] = {30, 20, 40, 70, 50};
    for (int i = 0; i < 5; i++) insertNode(&ll, ll.size, input1[i]);
    moveMaxToFront(&(ll.head));
    int expected1[] = {70, 30, 20, 40, 50};
    compareListWithDetails(&ll, expected1, 5, "Test 1: Max in middle");
    removeAllItems(&ll);
    
    // Test 2
    initList(&ll);
    int input2[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) insertNode(&ll, ll.size, input2[i]);
    moveMaxToFront(&(ll.head));
    int expected2[] = {50, 10, 20, 30, 40};
    compareListWithDetails(&ll, expected2, 5, "Test 2: Max at end");
    removeAllItems(&ll);
    
    // Test 3
    initList(&ll);
    int input3[] = {100, 20, 30};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input3[i]);
    moveMaxToFront(&(ll.head));
    compareListWithDetails(&ll, input3, 3, "Test 3: Max already at front");
    removeAllItems(&ll);
    
    // Test 4
    initList(&ll);
    insertNode(&ll, 0, 42);
    moveMaxToFront(&(ll.head));
    int expected4[] = {42};
    compareListWithDetails(&ll, expected4, 1, "Test 4: Single element");
    removeAllItems(&ll);
}

void test_recursiveReverse() {
    printf("\n=== Testing Q7: RecursiveReverse ===\n");
    LinkedList ll;
    
    // Test 1
    initList(&ll);
    int input1[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) insertNode(&ll, ll.size, input1[i]);
    RecursiveReverse(&(ll.head));
    int expected1[] = {5, 4, 3, 2, 1};
    compareListWithDetails(&ll, expected1, 5, "Test 1: Reverse 5 elements");
    removeAllItems(&ll);
    
    // Test 2
    initList(&ll);
    int input2[] = {10, 20};
    for (int i = 0; i < 2; i++) insertNode(&ll, ll.size, input2[i]);
    RecursiveReverse(&(ll.head));
    int expected2[] = {20, 10};
    compareListWithDetails(&ll, expected2, 2, "Test 2: Reverse 2 elements");
    removeAllItems(&ll);
    
    // Test 3
    initList(&ll);
    insertNode(&ll, 0, 18);
    RecursiveReverse(&(ll.head));
    int expected3[] = {18};
    compareListWithDetails(&ll, expected3, 1, "Test 3: Single element");
    removeAllItems(&ll);
    
    // Test 4
    initList(&ll);
    int input4[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input4[i]);
    RecursiveReverse(&(ll.head));
    int expected4[] = {3, 2, 1};
    compareListWithDetails(&ll, expected4, 3, "Test 4: Reverse 3 elements");
    removeAllItems(&ll);
}

//////////////////////////////////////////////////////////////////////////////////
// Test Summary
//////////////////////////////////////////////////////////////////////////////////

void print_test_summary() {
    printf("\n");
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║               TEST SUITE SUMMARY                       ║\n");
    printf("╠════════════════════════════════════════════════════════╣\n");
    printf("║  Total Tests:  %-4d                                    ║\n", global_stats.total_tests);
    printf("║  Passed:       %-4d  ✅                                ║\n", global_stats.passed_tests);
    printf("║  Failed:       %-4d  ❌                                ║\n", global_stats.failed_tests);
    printf("╠════════════════════════════════════════════════════════╣\n");
    
    if (global_stats.failed_tests == 0) {
        printf("║  🎉 ALL TESTS PASSED! 🎉                              ║\n");
    } else {
        double pass_rate = (double)global_stats.passed_tests / global_stats.total_tests * 100;
        printf("║  Pass Rate: %.1f%%                                     ║\n", pass_rate);
        printf("║  ⚠️  Some tests failed. Review errors above.          ║\n");
    }
    
    printf("╚════════════════════════════════════════════════════════╝\n");
}

//////////////////////////////////////////////////////////////////////////////////
// MAIN
//////////////////////////////////////////////////////////////////////////////////

int main() {
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║  Linked List Test Suite - All 7 Questions             ║\n");
    printf("║  Enhanced with Detailed Error Reporting               ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    
    test_insertSortedLL();
    test_alternateMergeLL();
    test_moveOddItemsToBack();
    test_moveEvenItemsToBack();
    test_frontBackSplitLL();
    test_moveMaxToFront();
    test_recursiveReverse();
    
    print_test_summary();
    
    return (global_stats.failed_tests == 0) ? 0 : 1;
}

//////////////////////////////////////////////////////////////////////////////////
// Q1: insertSortedLL
//////////////////////////////////////////////////////////////////////////////////

int insertSortedLL(LinkedList *ll, int item) {
    return 0;
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
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// Q7: RecursiveReverse
//////////////////////////////////////////////////////////////////////////////////

void RecursiveReverse(ListNode **ptrHead) {
    
}