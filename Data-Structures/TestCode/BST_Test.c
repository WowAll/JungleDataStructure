//////////////////////////////////////////////////////////////////////////////////
/* Enhanced BST Test Suite with Crash & Error Detection */
//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>

//////////////////////////////////////////////////////////////////////////////////
// Error Detection System
//////////////////////////////////////////////////////////////////////////////////

static jmp_buf test_env;
static volatile sig_atomic_t timeout_occurred = 0;
static const char* current_test_name = NULL;

#define TEST_TIMEOUT_SECONDS 3

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
    
    longjmp(test_env, sig);
}

void timeout_handler(int sig) {
    timeout_occurred = 1;
    printf("\n⏱️  TIMEOUT: Test exceeded %d seconds (possible infinite loop)\n", TEST_TIMEOUT_SECONDS);
    printf("   In test: %s\n", current_test_name ? current_test_name : "Unknown");
    longjmp(test_env, SIGALRM);
}

void setup_error_detection() {
    signal(SIGSEGV, crash_handler);
    signal(SIGFPE, crash_handler);
    signal(SIGABRT, crash_handler);
    signal(SIGILL, crash_handler);
    signal(SIGALRM, timeout_handler);
}

void start_timeout() {
    timeout_occurred = 0;
    alarm(TEST_TIMEOUT_SECONDS);
}

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

#define TEST_ASSERT_ARRAY_EQ(actual_array, expected_array, count, test_name) do { \
    global_stats.total_tests++; \
    int match = 1; \
    for (int i = 0; i < (count); i++) { \
        if ((actual_array)[i] != (expected_array)[i]) { \
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
            printf("%d ", (actual_array)[i]); \
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

typedef struct _bstnode {
    int item;
    struct _bstnode *left;
    struct _bstnode *right;
} BSTNode;

typedef struct _stackNode {
    BSTNode *data;
    struct _stackNode *next;
} StackNode;

typedef struct _stack {
    StackNode *top;
} Stack;

typedef struct _QueueNode {
    BSTNode *data;
    struct _QueueNode *nextPtr;
} QueueNode;

typedef struct _queue {
    QueueNode *head;
    QueueNode *tail;
} Queue;

//////////////////////////////////////////////////////////////////////////////////
// Helper Functions
//////////////////////////////////////////////////////////////////////////////////

static int captured_values[100];
static int captured_count = 0;

void reset_capture() {
    captured_count = 0;
}

void capture_print(int value) {
    if (captured_count < 100) {
        captured_values[captured_count++] = value;
    }
}

BSTNode* createBSTNode(int item) {
    BSTNode *newNode = (BSTNode*)malloc(sizeof(BSTNode));
    newNode->item = item;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void removeAll(BSTNode **node) {
    if (*node != NULL) {
        removeAll(&((*node)->left));
        removeAll(&((*node)->right));
        free(*node);
        *node = NULL;
    }
}

BSTNode* createSampleBST1() {
    BSTNode *root = createBSTNode(20);
    root->left = createBSTNode(15);
    root->right = createBSTNode(50);
    root->left->left = createBSTNode(10);
    root->left->right = createBSTNode(18);
    root->right->left = createBSTNode(25);
    root->right->right = createBSTNode(80);
    return root;
}

BSTNode* createSampleBST2() {
    BSTNode *root = createBSTNode(20);
    root->left = createBSTNode(15);
    root->right = createBSTNode(50);
    root->left->left = createBSTNode(10);
    root->left->right = createBSTNode(18);
    return root;
}

BSTNode* createSampleBST3() {
    BSTNode *root = createBSTNode(20);
    root->left = createBSTNode(15);
    root->right = createBSTNode(50);
    root->left->left = createBSTNode(10);
    root->left->right = createBSTNode(18);
    root->right->left = createBSTNode(25);
    root->right->right = createBSTNode(80);
    return root;
}

BSTNode* createSampleBST4() {
    BSTNode *root = createBSTNode(20);
    root->left = createBSTNode(15);
    root->right = createBSTNode(50);
    root->left->left = createBSTNode(10);
    root->left->right = createBSTNode(18);
    root->right->left = createBSTNode(25);
    root->right->right = createBSTNode(80);
    return root;
}

//////////////////////////////////////////////////////////////////////////////////
// Stack Operations
//////////////////////////////////////////////////////////////////////////////////

void push(Stack *stack, BSTNode *node) {
    StackNode *temp = malloc(sizeof(StackNode));
    if (temp == NULL) return;
    temp->data = node;
    if (stack->top == NULL) {
        stack->top = temp;
        temp->next = NULL;
    } else {
        temp->next = stack->top;
        stack->top = temp;
    }
}

BSTNode* pop(Stack *s) {
    StackNode *temp, *t;
    BSTNode *ptr = NULL;
    t = s->top;
    if (t != NULL) {
        temp = t->next;
        ptr = t->data;
        s->top = temp;
        free(t);
    }
    return ptr;
}

BSTNode* peek(Stack *s) {
    StackNode *temp = s->top;
    if (temp != NULL) return temp->data;
    return NULL;
}

int isEmptyStack(Stack *s) {
    return s->top == NULL;
}

void removeAllFromStack(Stack *s) {
    while (!isEmptyStack(s)) {
        pop(s);
    }
}

//////////////////////////////////////////////////////////////////////////////////
// Queue Operations
//////////////////////////////////////////////////////////////////////////////////

void enqueue(QueueNode **headPtr, QueueNode **tailPtr, BSTNode *node) {
    QueueNode *newPtr = malloc(sizeof(QueueNode));
    if (newPtr != NULL) {
        newPtr->data = node;
        newPtr->nextPtr = NULL;
        if (*headPtr == NULL) {
            *headPtr = newPtr;
        } else {
            (*tailPtr)->nextPtr = newPtr;
        }
        *tailPtr = newPtr;
    }
}

BSTNode* dequeue(QueueNode **headPtr, QueueNode **tailPtr) {
    if (*headPtr == NULL) return NULL;
    BSTNode *node = (*headPtr)->data;
    QueueNode *tempPtr = *headPtr;
    *headPtr = (*headPtr)->nextPtr;
    if (*headPtr == NULL) {
        *tailPtr = NULL;
    }
    free(tempPtr);
    return node;
}

int isEmptyQueue(QueueNode *head) {
    return head == NULL;
}

//////////////////////////////////////////////////////////////////////////////////
// Function Prototypes
//////////////////////////////////////////////////////////////////////////////////

void levelOrderTraversal(BSTNode *root);
void inOrderIterative(BSTNode *root);
void preOrderIterative(BSTNode *root);
void postOrderIterativeS1(BSTNode *root);
void postOrderIterativeS2(BSTNode *root);

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

void test_levelOrderTraversal() {
    printf("\n=== Testing Q1: levelOrderTraversal ===\n");
    BSTNode *tree;
    
    // Test 1: Full tree
    tree = createSampleBST1();
    reset_capture();
    levelOrderTraversal(tree);
    int expected1[] = {20, 15, 50, 10, 18, 25, 80};
    TEST_ASSERT_ARRAY_EQ(captured_values, expected1, 7, "Test 1: Full BST level-order");
    removeAll(&tree);
    
    // Test 2: Partial tree
    tree = createSampleBST2();
    reset_capture();
    levelOrderTraversal(tree);
    int expected2[] = {20, 15, 50, 10, 18};
    TEST_ASSERT_ARRAY_EQ(captured_values, expected2, 5, "Test 2: Partial BST level-order");
    removeAll(&tree);
    
    // Test 3: Single node
    tree = createBSTNode(42);
    reset_capture();
    levelOrderTraversal(tree);
    int expected3[] = {42};
    TEST_ASSERT_ARRAY_EQ(captured_values, expected3, 1, "Test 3: Single node level-order");
    removeAll(&tree);
    
    // Test 4: Empty tree
    tree = NULL;
    reset_capture();
    levelOrderTraversal(tree);
    global_stats.total_tests++;
    if (captured_count == 0) {
        global_stats.passed_tests++;
        printf("✓ Test 4: Empty tree returns nothing\n");
    } else {
        global_stats.failed_tests++;
        printf("❌ FAILED: Test 4: Empty tree should return nothing\n");
        return;
    }
}

void test_inOrderIterative() {
    printf("\n=== Testing Q2: inOrderIterative ===\n");
    BSTNode *tree;
    
    // Test 1: Full tree
    tree = createSampleBST2();
    reset_capture();
    inOrderIterative(tree);
    int expected1[] = {10, 15, 18, 20, 50};
    TEST_ASSERT_ARRAY_EQ(captured_values, expected1, 5, "Test 1: In-order traversal");
    removeAll(&tree);
    
    // Test 2: Full tree with more nodes
    tree = createSampleBST1();
    reset_capture();
    inOrderIterative(tree);
    int expected2[] = {10, 15, 18, 20, 25, 50, 80};
    TEST_ASSERT_ARRAY_EQ(captured_values, expected2, 7, "Test 2: Full BST in-order");
    removeAll(&tree);
    
    // Test 3: Single node
    tree = createBSTNode(42);
    reset_capture();
    inOrderIterative(tree);
    int expected3[] = {42};
    TEST_ASSERT_ARRAY_EQ(captured_values, expected3, 1, "Test 3: Single node in-order");
    removeAll(&tree);
    
    // Test 4: Empty tree
    tree = NULL;
    reset_capture();
    inOrderIterative(tree);
    global_stats.total_tests++;
    if (captured_count == 0) {
        global_stats.passed_tests++;
        printf("✓ Test 4: Empty tree returns nothing\n");
    } else {
        global_stats.failed_tests++;
        printf("❌ FAILED: Test 4: Empty tree should return nothing\n");
        return;
    }
}

void test_preOrderIterative() {
    printf("\n=== Testing Q3: preOrderIterative ===\n");
    BSTNode *tree;
    
    // Test 1: Full tree
    tree = createSampleBST3();
    reset_capture();
    preOrderIterative(tree);
    int expected1[] = {20, 15, 10, 18, 50, 25, 80};
    TEST_ASSERT_ARRAY_EQ(captured_values, expected1, 7, "Test 1: Pre-order traversal");
    removeAll(&tree);
    
    // Test 2: Partial tree
    tree = createSampleBST2();
    reset_capture();
    preOrderIterative(tree);
    int expected2[] = {20, 15, 10, 18, 50};
    TEST_ASSERT_ARRAY_EQ(captured_values, expected2, 5, "Test 2: Partial BST pre-order");
    removeAll(&tree);
    
    // Test 3: Single node
    tree = createBSTNode(42);
    reset_capture();
    preOrderIterative(tree);
    int expected3[] = {42};
    TEST_ASSERT_ARRAY_EQ(captured_values, expected3, 1, "Test 3: Single node pre-order");
    removeAll(&tree);
    
    // Test 4: Empty tree
    tree = NULL;
    reset_capture();
    preOrderIterative(tree);
    global_stats.total_tests++;
    if (captured_count == 0) {
        global_stats.passed_tests++;
        printf("✓ Test 4: Empty tree returns nothing\n");
    } else {
        global_stats.failed_tests++;
        printf("❌ FAILED: Test 4: Empty tree should return nothing\n");
        return;
    }
}

void test_postOrderIterativeS1() {
    printf("\n=== Testing Q4: postOrderIterativeS1 ===\n");
    BSTNode *tree;
    
    // Test 1: Full tree
    tree = createSampleBST4();
    reset_capture();
    postOrderIterativeS1(tree);
    int expected1[] = {10, 18, 15, 25, 80, 50, 20};
    TEST_ASSERT_ARRAY_EQ(captured_values, expected1, 7, "Test 1: Post-order (single stack)");
    removeAll(&tree);
    
    // Test 2: Partial tree
    tree = createSampleBST2();
    reset_capture();
    postOrderIterativeS1(tree);
    int expected2[] = {10, 18, 15, 50, 20};
    TEST_ASSERT_ARRAY_EQ(captured_values, expected2, 5, "Test 2: Partial BST post-order");
    removeAll(&tree);
    
    // Test 3: Single node
    tree = createBSTNode(42);
    reset_capture();
    postOrderIterativeS1(tree);
    int expected3[] = {42};
    TEST_ASSERT_ARRAY_EQ(captured_values, expected3, 1, "Test 3: Single node post-order");
    removeAll(&tree);
    
    // Test 4: Empty tree
    tree = NULL;
    reset_capture();
    postOrderIterativeS1(tree);
    global_stats.total_tests++;
    if (captured_count == 0) {
        global_stats.passed_tests++;
        printf("✓ Test 4: Empty tree returns nothing\n");
    } else {
        global_stats.failed_tests++;
        printf("❌ FAILED: Test 4: Empty tree should return nothing\n");
        return;
    }
}

void test_postOrderIterativeS2() {
    printf("\n=== Testing Q5: postOrderIterativeS2 ===\n");
    BSTNode *tree;
    
    // Test 1: Full tree
    tree = createSampleBST1();
    reset_capture();
    postOrderIterativeS2(tree);
    int expected1[] = {10, 18, 15, 25, 80, 50, 20};
    TEST_ASSERT_ARRAY_EQ(captured_values, expected1, 7, "Test 1: Post-order (two stacks)");
    removeAll(&tree);
    
    // Test 2: Partial tree
    tree = createSampleBST2();
    reset_capture();
    postOrderIterativeS2(tree);
    int expected2[] = {10, 18, 15, 50, 20};
    TEST_ASSERT_ARRAY_EQ(captured_values, expected2, 5, "Test 2: Partial BST post-order");
    removeAll(&tree);
    
    // Test 3: Single node
    tree = createBSTNode(42);
    reset_capture();
    postOrderIterativeS2(tree);
    int expected3[] = {42};
    TEST_ASSERT_ARRAY_EQ(captured_values, expected3, 1, "Test 3: Single node post-order");
    removeAll(&tree);
    
    // Test 4: Empty tree
    tree = NULL;
    reset_capture();
    postOrderIterativeS2(tree);
    global_stats.total_tests++;
    if (captured_count == 0) {
        global_stats.passed_tests++;
        printf("✓ Test 4: Empty tree returns nothing\n");
    } else {
        global_stats.failed_tests++;
        printf("❌ FAILED: Test 4: Empty tree should return nothing\n");
        return;
    }
}

//////////////////////////////////////////////////////////////////////////////////
// Test Summary
//////////////////////////////////////////////////////////////////////////////////

void print_test_summary() {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║               TEST SUITE SUMMARY                     ║\n");
    printf("╠══════════════════════════════════════════════════════╣\n");
    printf("║  Total Tests:  %-4d                                  ║\n", global_stats.total_tests);
    printf("║  Passed:       %-4d  ✅                              ║\n", global_stats.passed_tests);
    printf("║  Failed:       %-4d  ❌                              ║\n", global_stats.failed_tests);
    printf("║  Crashed:      %-4d  🔴                              ║\n", global_stats.crashed_tests);
    printf("║  Timeout:      %-4d  ⏱️                               ║\n", global_stats.timeout_tests);
    printf("╠══════════════════════════════════════════════════════╣\n");
    
    if (global_stats.failed_tests == 0 && global_stats.crashed_tests == 0 && global_stats.timeout_tests == 0) {
        printf("║  🎉 ALL TESTS PASSED! 🎉                            ║\n");
    } else {
        double pass_rate = (double)global_stats.passed_tests / global_stats.total_tests * 100;
        printf("║  Pass Rate: %.1f%%                                   ║\n", pass_rate);
        if (global_stats.failed_tests > 0)
            printf("║  ⚠️  Some tests failed. Review errors above.        ║\n");
        if (global_stats.crashed_tests > 0)
            printf("║  🔴 Some tests crashed. Check for memory errors.    ║\n");
        if (global_stats.timeout_tests > 0)
            printf("║  ⏱️  Some tests timed out. Check for infinite loops.║\n");
    }
    
    printf("╚══════════════════════════════════════════════════════╝\n");
}

//////////////////////////////////////////////////////////////////////////////////
// MAIN
//////////////////////////////////////////////////////////////////////////////////

int main() {
    setup_error_detection();
    
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║  BST Test Suite - All 5 Questions                   ║\n");
    printf("║  Enhanced with Crash & Error Detection              ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n");
    
    RUN_SAFE_TEST(test_levelOrderTraversal);
    RUN_SAFE_TEST(test_inOrderIterative);
    RUN_SAFE_TEST(test_preOrderIterative);
    RUN_SAFE_TEST(test_postOrderIterativeS1);
    RUN_SAFE_TEST(test_postOrderIterativeS2);
    
    print_test_summary();
    
    return (global_stats.failed_tests == 0 && 
            global_stats.crashed_tests == 0 && 
            global_stats.timeout_tests == 0) ? 0 : 1;
}

//////////////////////////////////////////////////////////////////////////////////
// Q1: levelOrderTraversal
//////////////////////////////////////////////////////////////////////////////////

void levelOrderTraversal(BSTNode* root)
{
	// use capture_print(node->item) instead of printf
}


//////////////////////////////////////////////////////////////////////////////////
// Q2: inOrderIterative
//////////////////////////////////////////////////////////////////////////////////

void inOrderIterative(BSTNode *root) {
    // use capture_print(node->item) instead of printf
    return;
}

//////////////////////////////////////////////////////////////////////////////////
// Q3: preOrderIterative
//////////////////////////////////////////////////////////////////////////////////

void preOrderIterative(BSTNode *root)
{
	// use capture_print(node->item) instead of printf
    return;
}

//////////////////////////////////////////////////////////////////////////////////
// Q4: postOrderIterativeS1
//////////////////////////////////////////////////////////////////////////////////

void postOrderIterativeS1(BSTNode *root) {
    // use capture_print(node->item) instead of printf
    return;
}

//////////////////////////////////////////////////////////////////////////////////
// Q5: postOrderIterativeS2
//////////////////////////////////////////////////////////////////////////////////

void postOrderIterativeS2(BSTNode *root) {
    // use capture_print(node->item) instead of printf
    return;
}