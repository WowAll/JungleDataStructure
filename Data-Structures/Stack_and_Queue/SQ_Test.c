//////////////////////////////////////////////////////////////////////////////////
/* Enhanced Stack & Queue Test Suite with Detailed Error Reporting */
//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MIN_INT -1000

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
    ListNode *tail;
} LinkedList;

typedef struct _stack {
    LinkedList ll;
} Stack;

typedef struct _queue {
    LinkedList ll;
} Queue;

//////////////////////////////////////////////////////////////////////////////////
// Helper Functions
//////////////////////////////////////////////////////////////////////////////////

void initList(LinkedList *ll) {
    ll->head = NULL;
    ll->tail = NULL;
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
    ll->tail = NULL;
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

int removeNode(LinkedList *ll, int index) {
    ListNode *pre, *cur;
    if (ll == NULL || index < 0 || index >= ll->size)
        return -1;
    
    if (index == 0) {
        cur = ll->head->next;
        free(ll->head);
        ll->head = cur;
        ll->size--;
        return 0;
    }
    
    if ((pre = findNode(ll, index - 1)) != NULL) {
        if (pre->next == NULL)
            return -1;
        cur = pre->next;
        pre->next = cur->next;
        free(cur);
        ll->size--;
        return 0;
    }
    return -1;
}

int compareListWithDetails(LinkedList *ll, int *expected, int expectedSize, const char *test_name) {
    global_stats.total_tests++;
    
    if (ll->size != expectedSize) {
        global_stats.failed_tests++;
        printf("❌ FAILED: %s\n", test_name);
        printf("   Size mismatch! Expected: %d, Actual: %d\n", expectedSize, ll->size);
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
        if (cur == NULL || cur->item != expected[i]) {
            global_stats.failed_tests++;
            printf("❌ FAILED: %s\n", test_name);
            if (cur == NULL) {
                printf("   List ended at index %d\n", i);
            } else {
                printf("   Mismatch at index %d: Expected %d, Got %d\n", i, expected[i], cur->item);
            }
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
    
    global_stats.passed_tests++;
    printf("✓ %s\n", test_name);
    return 1;
}

//////////////////////////////////////////////////////////////////////////////////
// Stack Operations
//////////////////////////////////////////////////////////////////////////////////

void push(Stack *s, int item) {
    insertNode(&(s->ll), 0, item);
}

int pop(Stack *s) {
    int item;
    if (s->ll.head != NULL) {
        item = ((s->ll).head)->item;
        removeNode(&(s->ll), 0);
        return item;
    }
    return MIN_INT;
}

int peek(Stack *s) {
    if (s->ll.size == 0)
        return MIN_INT;
    return ((s->ll).head)->item;
}

int isEmptyStack(Stack *s) {
    return (s->ll.size == 0);
}

void removeAllItemsFromStack(Stack *s) {
    if (s == NULL)
        return;
    while (s->ll.head != NULL) {
        pop(s);
    }
}

//////////////////////////////////////////////////////////////////////////////////
// Queue Operations
//////////////////////////////////////////////////////////////////////////////////

void enqueue(Queue *q, int item) {
    insertNode(&(q->ll), q->ll.size, item);
}

int dequeue(Queue *q) {
    int item;
    if (q->ll.head != NULL) {
        item = ((q->ll).head)->item;
        removeNode(&(q->ll), 0);
        return item;
    }
    return -1;
}

int isEmptyQueue(Queue *q) {
    return (q->ll.size == 0);
}

void removeAllItemsFromQueue(Queue *q) {
    int count, i;
    if (q == NULL)
        return;
    count = q->ll.size;
    for (i = 0; i < count; i++)
        dequeue(q);
}

//////////////////////////////////////////////////////////////////////////////////
// Function Prototypes
//////////////////////////////////////////////////////////////////////////////////

void removeOddValues(Queue *q);
void removeEvenValues(Stack *s);
int isStackPairwiseConsecutive(Stack *s);
void reverse(Queue *q);
void recursiveReverse(Queue *q);
void removeUntil(Stack *s, int value);
int balanced(char *expression);

//////////////////////////////////////////////////////////////////////////////////
// TEST CASES
//////////////////////////////////////////////////////////////////////////////////

void test_removeOddValues() {
    printf("\n=== Testing Q1: removeOddValues ===\n");
    LinkedList ll;
    Queue q;
    
    // Test 1
    initList(&ll);
    q.ll = ll;
    int input1[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) enqueue(&q, input1[i]);
    removeOddValues(&q);
    int expected1[] = {2, 4};
    compareListWithDetails(&q.ll, expected1, 2, "Test 1: Remove odd from {1, 2, 3, 4, 5}");
    removeAllItemsFromQueue(&q);
    
    // Test 2
    initList(&ll);
    q.ll = ll;
    int input2[] = {1, 3, 5, 7};
    for (int i = 0; i < 4; i++) enqueue(&q, input2[i]);
    removeOddValues(&q);
    TEST_ASSERT_INT_EQ(q.ll.size, 0, "Test 2: {1, 3, 5, 7} All odd values removed");
    
    // Test 3
    initList(&ll);
    q.ll = ll;
    int input3[] = {2, 4, 6, 8};
    for (int i = 0; i < 4; i++) enqueue(&q, input3[i]);
    removeOddValues(&q);
    compareListWithDetails(&q.ll, input3, 4, "Test 3: {2, 4, 6, 8} No odd values to remove");
    removeAllItemsFromQueue(&q);
}

void test_removeEvenValues() {
    printf("\n=== Testing Q2: removeEvenValues ===\n");
    LinkedList ll;
    Stack s;
    
    // Test 1
    initList(&ll);
    s.ll = ll;
    int input1[] = {1, 3, 5, 6, 7};
    for (int i = 0; i < 5; i++) push(&s, input1[i]);
    removeEvenValues(&s);
    int expected1[] = {7, 5, 3, 1};
    compareListWithDetails(&s.ll, expected1, 4, "Test 1: {1, 3, 5, 6, 7} Remove even values");
    removeAllItemsFromStack(&s);
    
    // Test 2
    initList(&ll);
    s.ll = ll;
    int input2[] = {2, 4, 6, 8};
    for (int i = 0; i < 4; i++) push(&s, input2[i]);
    removeEvenValues(&s);
    TEST_ASSERT_INT_EQ(s.ll.size, 0, "Test 2: {2, 4, 6, 8} All even values removed");
    
    // Test 3
    initList(&ll);
    s.ll = ll;
    int input3[] = {1, 3, 5, 7};
    for (int i = 0; i < 4; i++) push(&s, input3[i]);
    removeEvenValues(&s);
    int expected3[] = {7, 5, 3, 1};
    compareListWithDetails(&s.ll, expected3, 4, "Test 3: {1, 3, 5, 7} No even values to remove");
    removeAllItemsFromStack(&s);
}

void test_isStackPairwiseConsecutive() {
    printf("\n=== Testing Q3: isStackPairwiseConsecutive ===\n");
    Stack s;
    
    // Test 1
    initList(&s.ll);
    int input1[] = {16, 15, 11, 10, 5, 4};
    for (int i = 0; i < 6; i++) push(&s, input1[5-i]);
    TEST_ASSERT_INT_EQ(isStackPairwiseConsecutive(&s), 1, "Test 1: {16, 15, 11, 10, 5, 4} Stack IS pairwise consecutive");
    removeAllItemsFromStack(&s);
    
    // Test 2
    initList(&s.ll);
    int input2[] = {16, 15, 11, 10, 5, 1};
    for (int i = 0; i < 6; i++) push(&s, input2[5-i]);
    TEST_ASSERT_INT_EQ(isStackPairwiseConsecutive(&s), 0, "Test 2: {16, 15, 11, 10, 5, 1} Stack NOT pairwise consecutive");
    removeAllItemsFromStack(&s);
    
    // Test 3
    initList(&s.ll);
    int input3[] = {16, 15, 11, 10, 5};
    for (int i = 0; i < 5; i++) push(&s, input3[4-i]);
    int result = isStackPairwiseConsecutive(&s);
    global_stats.total_tests++;
    if (result == 1 || result == 0) {
        global_stats.passed_tests++;
        printf("✓ Test 3: {16, 15, 11, 10, 5} Odd elements handled (result=%d)\n", result);
    } else {
        global_stats.failed_tests++;
        printf("❌ FAILED: Test 3: Invalid return value %d\n", result);
    }
    removeAllItemsFromStack(&s);
    
    // Test 4
    initList(&s.ll);
    TEST_ASSERT_INT_EQ(isStackPairwiseConsecutive(&s), 1, "Test 4: Empty stack");
}

void test_reverseQueue() {
    printf("\n=== Testing Q4: reverse (Queue) ===\n");
    Queue q;
    
    // Test 1
    initList(&q.ll);
    int input1[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) enqueue(&q, input1[i]);
    reverse(&q);
    int expected1[] = {5, 4, 3, 2, 1};
    compareListWithDetails(&q.ll, expected1, 5, "Test 1: {1, 2, 3, 4, 5} Reverse 5 elements");
    removeAllItemsFromQueue(&q);
    
    // Test 2
    initList(&q.ll);
    int input2[] = {10, 20};
    for (int i = 0; i < 2; i++) enqueue(&q, input2[i]);
    reverse(&q);
    int expected2[] = {20, 10};
    compareListWithDetails(&q.ll, expected2, 2, "Test 2: {10, 20} Reverse 2 elements");
    removeAllItemsFromQueue(&q);
    
    // Test 3
    initList(&q.ll);
    enqueue(&q, 18);
    reverse(&q);
    int expected3[] = {18};
    compareListWithDetails(&q.ll, expected3, 1, "Test 3: {18} Single element");
    removeAllItemsFromQueue(&q);
}

void test_recursiveReverse() {
    printf("\n=== Testing Q5: recursiveReverse (Queue) ===\n");
    Queue q;
    
    // Test 1
    initList(&q.ll);
    int input1[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) enqueue(&q, input1[i]);
    recursiveReverse(&q);
    int expected1[] = {5, 4, 3, 2, 1};
    compareListWithDetails(&q.ll, expected1, 5, "Test 1: {1, 2, 3, 4, 5} Recursive reverse 5 elements");
    removeAllItemsFromQueue(&q);
    
    // Test 2
    initList(&q.ll);
    int input2[] = {10, 20};
    for (int i = 0; i < 2; i++) enqueue(&q, input2[i]);
    recursiveReverse(&q);
    int expected2[] = {20, 10};
    compareListWithDetails(&q.ll, expected2, 2, "Test 2: {10, 20} Recursive reverse 2 elements");
    removeAllItemsFromQueue(&q);
    
    // Test 3
    initList(&q.ll);
    enqueue(&q, 18);
    recursiveReverse(&q);
    int expected3[] = {18};
    compareListWithDetails(&q.ll, expected3, 1, "Test 3: {18} Single element");
    removeAllItemsFromQueue(&q);
}

void test_removeUntil() {
    printf("\n=== Testing Q6: removeUntil ===\n");
    Stack s;
    
    // Test 1
    initList(&s.ll);
    int input1[] = {1, 2, 3, 4, 5, 6, 7};
    for (int i = 0; i < 7; i++) push(&s, input1[i]);
    removeUntil(&s, 4);
    int expected1[] = {4, 5, 6, 7};
    compareListWithDetails(&s.ll, expected1, 4, "Test 1: {1, 2, 3, 4, 5, 6, 7} Remove until 4");
    removeAllItemsFromStack(&s);
    
    // Test 2
    initList(&s.ll);
    int input2[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++) push(&s, input2[i]);
    removeUntil(&s, 5);
    int expected2[] = {5};
    compareListWithDetails(&s.ll, expected2, 1, "Test 2: {1, 2, 3, 4, 5} Remove until 5");
    removeAllItemsFromStack(&s);
    
    // Test 3
    initList(&s.ll);
    int input3[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) push(&s, input3[i]);
    int orig_size = s.ll.size;
    removeUntil(&s, 99);
    global_stats.total_tests++;
    if (s.ll.size == orig_size || s.ll.size == 0) {
        global_stats.passed_tests++;
        printf("✓ Test 3: {1, 2, 3} Value not found (size=%d)\n", s.ll.size);
    } else {
        global_stats.failed_tests++;
        printf("❌ FAILED: Test 3: Unexpected size change\n");
    }
    removeAllItemsFromStack(&s);
}

void test_balanced() {
    printf("\n=== Testing Q7: balanced ===\n");
    
    TEST_ASSERT_INT_EQ(balanced("()"), 0, "Test 1: () is balanced");
    TEST_ASSERT_INT_EQ(balanced("()[]{}"), 0, "Test 2: ()[]{}  is balanced");
    TEST_ASSERT_INT_EQ(balanced("([{}])"), 0, "Test 3: ([{}]) is balanced");
    TEST_ASSERT_INT_EQ(balanced("("), 1, "Test 4: ( is NOT balanced");
    TEST_ASSERT_INT_EQ(balanced("(]"), 1, "Test 5: (] is NOT balanced");
    TEST_ASSERT_INT_EQ(balanced("())"), 1, "Test 6: ()) is NOT balanced");
    TEST_ASSERT_INT_EQ(balanced("{[()()]}"), 0, "Test 7: {[()()]} is balanced");
    TEST_ASSERT_INT_EQ(balanced("{[(])}"), 1, "Test 8: {[(])} is NOT balanced");
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
    printf("║  Stack & Queue Test Suite - All 7 Questions           ║\n");
    printf("║  Enhanced with Detailed Error Reporting               ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    
    test_removeOddValues();
    test_removeEvenValues();
    test_isStackPairwiseConsecutive();
    test_reverseQueue();
    test_recursiveReverse();
    test_removeUntil();
    test_balanced();
    
    print_test_summary();
    
    return (global_stats.failed_tests == 0) ? 0 : 1;
}

//////////////////////////////////////////////////////////////////////////////////
// Q1: createQueueFromLinkedList & removeOddValues
//////////////////////////////////////////////////////////////////////////////////

void createQueueFromLinkedList(LinkedList *ll, Queue *q) {

}

void removeOddValues(Queue *q) {

}

//////////////////////////////////////////////////////////////////////////////////
// Q2: createStackFromLinkedList & removeEvenValues
//////////////////////////////////////////////////////////////////////////////////

void createStackFromLinkedList(LinkedList *ll, Stack *s) {

}

void removeEvenValues(Stack *s) {
	
}

//////////////////////////////////////////////////////////////////////////////////
// Q3: isStackPairwiseConsecutive
//////////////////////////////////////////////////////////////////////////////////

int isStackPairwiseConsecutive(Stack *s) {
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// Q4: reverse (Queue using Stack)
//////////////////////////////////////////////////////////////////////////////////

void reverse(Queue *q) {

}

//////////////////////////////////////////////////////////////////////////////////
// Q5: recursiveReverse (Queue)
//////////////////////////////////////////////////////////////////////////////////

void recursiveReverse(Queue *q) {

}

//////////////////////////////////////////////////////////////////////////////////
// Q6: removeUntil
//////////////////////////////////////////////////////////////////////////////////

void removeUntil(Stack *s, int value) {
}

//////////////////////////////////////////////////////////////////////////////////
// Q7: balanced
//////////////////////////////////////////////////////////////////////////////////

int balanced(char *expression) {
	return 0;
}