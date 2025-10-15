//////////////////////////////////////////////////////////////////////////////////
/* Enhanced Linked List Test Suite with Crash & Error Detection */
//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <sys/time.h>

//////////////////////////////////////////////////////////////////////////////////
// Error Detection System
//////////////////////////////////////////////////////////////////////////////////

static jmp_buf test_env;
static volatile sig_atomic_t timeout_occurred = 0;
static const char* current_test_name = NULL;

#define TEST_TIMEOUT_SECONDS 3

// Signal handler for crashes
void crash_handler(int sig) {
    const char* error_type = "UNKNOWN";
    
    switch(sig) {
        case SIGSEGV:
            error_type = "SEGMENTATION FAULT (accessing invalid memory)";
            break;
        case SIGFPE:
            error_type = "FLOATING POINT EXCEPTION (division by zero)";
            break;
        case SIGABRT:
            error_type = "ABORT (program terminated abnormally)";
            break;
        case SIGILL:
            error_type = "ILLEGAL INSTRUCTION";
            break;
    }
    
    printf("\n🔴 CRASH DETECTED: %s\n", error_type);
    printf("   In test: %s\n", current_test_name ? current_test_name : "Unknown");
    printf("   Signal: %d\n", sig);
    
    // Jump back to safe point
    longjmp(test_env, sig);
}

// Alarm handler for timeout
void timeout_handler(int sig) {
    timeout_occurred = 1;
    printf("\n⏱️  TIMEOUT: Test exceeded %d seconds (possible infinite loop)\n", TEST_TIMEOUT_SECONDS);
    printf("   In test: %s\n", current_test_name ? current_test_name : "Unknown");
    longjmp(test_env, SIGALRM);
}

// Setup signal handlers
void setup_error_detection() {
    signal(SIGSEGV, crash_handler);
    signal(SIGFPE, crash_handler);
    signal(SIGABRT, crash_handler);
    signal(SIGILL, crash_handler);
    signal(SIGALRM, timeout_handler);
}

// Start timeout timer
void start_timeout() {
    timeout_occurred = 0;
    alarm(TEST_TIMEOUT_SECONDS);
}

// Stop timeout timer
void stop_timeout() {
    alarm(0);
}

//////////////////////////////////////////////////////////////////////////////////
// Test Statistics
//////////////////////////////////////////////////////////////////////////////////

typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
    int crashed_tests;
    int timeout_tests;
} TestStats;

TestStats global_stats = {0, 0, 0, 0, 0};

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

#define TEST_ASSERT_ARRAY_EQ(actual_array, expected_array, count, test_name) do { \
    global_stats.total_tests++; \
    int match = 1; \
    for (int i = 0; i < (count); i++) { \
        if ((actual_array)[i].item != (expected_array)[i]) { \
            match = 0; \
            break; \
        } \
    } \
    if (!match) { \
        global_stats.failed_tests++; \
        printf("❌ FAILED: %s\n", test_name); \
        printf("   Expected: "); \
        for (int i = 0; i < (count); i++) { \
            printf("%d ", (expected_array)[i]); \
        } \
        printf("\n   Actual:   "); \
        for (int i = 0; i < (count); i++) { \
            printf("%d ", (actual_array)[i].item); \
        } \
        printf("\n   Location: Line %d\n", __LINE__); \
        return; \
    } else { \
        global_stats.passed_tests++; \
        printf("✓ %s\n", test_name); \
    } \
} while(0)

#define TEST_ASSERT_LL_EQ(actual_ll, expected_array, expected_count, test_name) do { \
    global_stats.total_tests++; \
    int match = 1; \
    \
    /* Check if actual_ll is NULL */ \
    if (actual_ll == NULL) { \
        if ((expected_count) == 0) { \
            match = 1; \
        } else { \
            match = 0; \
        } \
    } else { \
        /* Check size first */ \
        if ((actual_ll)->size != (expected_count)) { \
            match = 0; \
        } else { \
            /* Check if head is NULL when expected count is 0 */ \
            if ((expected_count) == 0) { \
                if ((actual_ll)->head != NULL) { \
                    match = 0; \
                } \
            } else { \
                /* Check if head is NULL when expected count > 0 */ \
                if ((actual_ll)->head == NULL) { \
                    match = 0; \
                } else { \
                    /* Compare each element */ \
                    ListNode *current = (actual_ll)->head; \
                    for (int i = 0; i < (expected_count); i++) { \
                        if (current == NULL || current->item != (expected_array)[i]) { \
                            match = 0; \
                            break; \
                        } \
                        current = current->next; \
                    } \
                    /* Check if there are extra elements */ \
                    if (current != NULL) { \
                        match = 0; \
                    } \
                } \
            } \
        } \
    } \
    \
    if (!match) { \
        global_stats.failed_tests++; \
        printf("❌ FAILED: %s\n", test_name); \
        printf("   Expected: "); \
        for (int i = 0; i < (expected_count); i++) { \
            printf("%d ", (expected_array)[i]); \
        } \
        printf("(size: %d)\n", (expected_count)); \
        printf("   Actual:   "); \
        if ((actual_ll) == NULL) { \
            printf("NULL"); \
        } else if ((actual_ll)->head == NULL) { \
            printf("NULL"); \
        } else { \
            ListNode *current = (actual_ll)->head; \
            while (current != NULL) { \
                printf("%d ", current->item); \
                current = current->next; \
            } \
        } \
        if ((actual_ll) != NULL) { \
            printf("(size: %d)", (actual_ll)->size); \
        } \
        printf("\n   Location: Line %d\n", __LINE__); \
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
// SAFE TEST WRAPPER
//////////////////////////////////////////////////////////////////////////////////

#define RUN_SAFE_TEST(test_func) do { \
    int sig; \
    current_test_name = #test_func; \
    start_timeout(); \
    if ((sig = setjmp(test_env)) == 0) { \
        test_func(); \
    } else { \
        if (sig == SIGALRM) { \
            global_stats.timeout_tests++; \
        } else { \
            global_stats.crashed_tests++; \
        } \
    } \
    stop_timeout(); \
    current_test_name = NULL; \
} while(0)

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
    TEST_ASSERT_LL_EQ(&ll, expected, 5, "Test 2: Insert in ascending order");
    
    // Test 3
    result = insertSortedLL(&ll, 8);
    TEST_ASSERT_INT_EQ(result, 4, "Test 3: Insert in middle - return value");
    int expected2[] = {2, 3, 5, 7, 8, 9};
    TEST_ASSERT_LL_EQ(&ll, expected2, 6, "Test 3: Insert in middle - list check");

    
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
    TEST_ASSERT_LL_EQ(&ll1, expected1, 6, "Test 1: Basic merge - ll1");
    TEST_ASSERT_LL_EQ(&ll2, expected2, 1, "Test 1: Basic merge - ll2");
    removeAllItems(&ll1);
    removeAllItems(&ll2);
    
    // Test 2
    initList(&ll1);
    initList(&ll2);
    for (int i = 1; i <= 6; i++) insertNode(&ll1, ll1.size, i);
    for (int i = 10; i <= 12; i++) insertNode(&ll2, ll2.size, i);
    
    alternateMergeLinkedList(&ll1, &ll2);
    int expected3[] = {1, 10, 2, 11, 3, 12, 4, 5, 6};
    TEST_ASSERT_LL_EQ(&ll1, expected3, 9, "Test 2: ll1 larger - size check");
    TEST_ASSERT_INT_EQ(ll2.size, 0, "Test 2: ll2 becomes empty");
    removeAllItems(&ll1);
    removeAllItems(&ll2);
    
    // Test 3
    initList(&ll1);
    initList(&ll2);
    insertNode(&ll1, 0, 1);
    insertNode(&ll1, 1, 2);
    alternateMergeLinkedList(&ll1, &ll2);
    int expected4[] = {1, 2};
    TEST_ASSERT_LL_EQ(&ll1, expected4, 2, "Test 3: Empty ll2 - no change");
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
    TEST_ASSERT_LL_EQ(&ll, expected1, 6, "Test 1: Mixed odd and even");
    removeAllItems(&ll);
    
    // Test 2
    initList(&ll);
    int input2[] = {2, 7, 18, 3, 4, 15};
    for (int i = 0; i < 6; i++) insertNode(&ll, ll.size, input2[i]);
    moveOddItemsToBack(&ll);
    int expected2[] = {2, 18, 4, 7, 3, 15};
    TEST_ASSERT_LL_EQ(&ll, expected2, 6, "Test 2: Another arrangement");
    removeAllItems(&ll);
    
    // Test 3
    initList(&ll);
    int input3[] = {1, 3, 5};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input3[i]);
    moveOddItemsToBack(&ll);
    TEST_ASSERT_LL_EQ(&ll, input3, 3, "Test 3: All odd numbers");
    removeAllItems(&ll);
    
    // Test 4
    initList(&ll);
    int input4[] = {2, 4, 6};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input4[i]);
    moveOddItemsToBack(&ll);
    TEST_ASSERT_LL_EQ(&ll, input4, 3, "Test 4: All even numbers");
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
    TEST_ASSERT_LL_EQ(&ll, expected1, 6, "Test 1: Mixed odd and even");
    removeAllItems(&ll);
    
    // Test 2
    initList(&ll);
    int input2[] = {2, 7, 18, 3, 4, 15};
    for (int i = 0; i < 6; i++) insertNode(&ll, ll.size, input2[i]);
    moveEvenItemsToBack(&ll);
    int expected2[] = {7, 3, 15, 2, 18, 4};
    TEST_ASSERT_LL_EQ(&ll, expected2, 6, "Test 2: Another arrangement");
    removeAllItems(&ll);
    
    // Test 3
    initList(&ll);
    int input3[] = {1, 3, 5};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input3[i]);
    moveEvenItemsToBack(&ll);
    TEST_ASSERT_LL_EQ(&ll, input3, 3, "Test 3: All odd numbers");
    removeAllItems(&ll);
    
    // Test 4
    initList(&ll);
    int input4[] = {2, 4, 6};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input4[i]);
    moveEvenItemsToBack(&ll);
    TEST_ASSERT_LL_EQ(&ll, input4, 3, "Test 4: All even numbers");
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
    TEST_ASSERT_LL_EQ(&front, expectedFront1, 3, "Test 1: Odd elements - front");
    TEST_ASSERT_LL_EQ(&back, expectedBack1, 2, "Test 1: Odd elements - back");
    removeAllItems(&front);
    removeAllItems(&back);
    
    // Test 2
    initList(&ll);
    int input2[] = {1, 2, 3, 4};
    for (int i = 0; i < 4; i++) insertNode(&ll, ll.size, input2[i]);
    frontBackSplitLinkedList(&ll, &front, &back);
    int expectedFront2[] = {1, 2};
    int expectedBack2[] = {3, 4};
    TEST_ASSERT_LL_EQ(&front, expectedFront2, 2, "Test 2: Even elements - front");
    TEST_ASSERT_LL_EQ(&back, expectedBack2, 2, "Test 2: Even elements - back");
    removeAllItems(&front);
    removeAllItems(&back);
    
    // Test 3
    initList(&ll);
    insertNode(&ll, 0, 42);
    frontBackSplitLinkedList(&ll, &front, &back);
    int expectedFront3[] = {42};
    TEST_ASSERT_LL_EQ(&front, expectedFront3, 1, "Test 3: Single element - front");
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
    TEST_ASSERT_LL_EQ(&ll, expected1, 5, "Test 1: Max in middle");
    removeAllItems(&ll);
    
    // Test 2
    initList(&ll);
    int input2[] = {10, 20, 30, 40, 50};
    for (int i = 0; i < 5; i++) insertNode(&ll, ll.size, input2[i]);
    moveMaxToFront(&(ll.head));
    int expected2[] = {50, 10, 20, 30, 40};
    TEST_ASSERT_LL_EQ(&ll, expected2, 5, "Test 2: Max at end");
    removeAllItems(&ll);
    
    // Test 3
    initList(&ll);
    int input3[] = {100, 20, 30};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input3[i]);
    moveMaxToFront(&(ll.head));
    TEST_ASSERT_LL_EQ(&ll, input3, 3, "Test 3: Max already at front");
    removeAllItems(&ll);
    
    // Test 4
    initList(&ll);
    insertNode(&ll, 0, 18);
    moveMaxToFront(&(ll.head));
    int expected4[] = {18};
    TEST_ASSERT_LL_EQ(&ll, expected4, 1, "Test 4: Single element");
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
    TEST_ASSERT_LL_EQ(&ll, expected1, 5, "Test 1: Reverse 5 elements");
    removeAllItems(&ll);
    
    // Test 2
    initList(&ll);
    int input2[] = {10, 20};
    for (int i = 0; i < 2; i++) insertNode(&ll, ll.size, input2[i]);
    RecursiveReverse(&(ll.head));
    int expected2[] = {20, 10};
    TEST_ASSERT_LL_EQ(&ll, expected2, 2, "Test 2: Reverse 2 elements");
    removeAllItems(&ll);
    
    // Test 3
    initList(&ll);
    insertNode(&ll, 0, 18);
    RecursiveReverse(&(ll.head));
    int expected3[] = {18};
    TEST_ASSERT_LL_EQ(&ll, expected3, 1, "Test 3: Single element");
    removeAllItems(&ll);
    
    // Test 4
    initList(&ll);
    int input4[] = {1, 2, 3};
    for (int i = 0; i < 3; i++) insertNode(&ll, ll.size, input4[i]);
    RecursiveReverse(&(ll.head));
    int expected4[] = {3, 2, 1};
    TEST_ASSERT_LL_EQ(&ll, expected4, 3, "Test 4: Reverse 3 elements");
    removeAllItems(&ll);
}

//////////////////////////////////////////////////////////////////////////////////
// Test Summary
//////////////////////////////////////////////////////////////////////////////////

void print_test_summary() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║               TEST SUITE SUMMARY                      ║\n");
    printf("╠═══════════════════════════════════════════════════════╣\n");
    printf("║  Total Tests:  %-4d                                   ║\n", global_stats.total_tests);
    printf("║  Passed:       %-4d  ✅                               ║\n", global_stats.passed_tests);
    printf("║  Failed:       %-4d  ❌                               ║\n", global_stats.failed_tests);
    printf("║  Crashed:      %-4d  🔴                               ║\n", global_stats.crashed_tests);
    printf("║  Timeout:      %-4d  ⏱️                                ║\n", global_stats.timeout_tests);
    printf("╠═══════════════════════════════════════════════════════╣\n");
    
    if (global_stats.failed_tests == 0 && global_stats.crashed_tests == 0 && global_stats.timeout_tests == 0) {
        printf("║  🎉 ALL TESTS PASSED! 🎉                             ║\n");
    } else {
        double pass_rate = (double)global_stats.passed_tests / global_stats.total_tests * 100;
        printf("║  Pass Rate: %.1f%%                                    ║\n", pass_rate);
        if (global_stats.failed_tests > 0)
            printf("║  ⚠️  Some tests failed. Review errors above.         ║\n");
        if (global_stats.crashed_tests > 0)
            printf("║  🔴 Some tests crashed. Check for memory errors.     ║\n");
        if (global_stats.timeout_tests > 0)
            printf("║  ⏱️  Some tests timed out. Check for infinite loops. ║\n");
    }
    
    printf("╚═══════════════════════════════════════════════════════╝\n");
}

//////////////////////////////////////////////////////////////////////////////////
// MAIN
//////////////////////////////////////////////////////////////////////////////////

int main() {
    setup_error_detection();
    
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║  Linked List Test Suite - All 7 Questions            ║\n");
    printf("║  Enhanced with Crash & Error Detection               ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    
    RUN_SAFE_TEST(test_insertSortedLL);
    RUN_SAFE_TEST(test_alternateMergeLL);
    RUN_SAFE_TEST(test_moveOddItemsToBack);
    RUN_SAFE_TEST(test_moveEvenItemsToBack);
    RUN_SAFE_TEST(test_frontBackSplitLL);
    RUN_SAFE_TEST(test_moveMaxToFront);
    RUN_SAFE_TEST(test_recursiveReverse);
    
    print_test_summary();
    
    return (global_stats.failed_tests == 0 && 
            global_stats.crashed_tests == 0 && 
            global_stats.timeout_tests == 0) ? 0 : 1;
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
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// Q7: RecursiveReverse
//////////////////////////////////////////////////////////////////////////////////

void RecursiveReverse(ListNode **ptrHead) {

}