//////////////////////////////////////////////////////////////////////////////////
/* Enhanced Binary Tree Test Suite with Crash & Error Detection */
//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
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

typedef struct _btnode {
    int item;
    struct _btnode *left;
    struct _btnode *right;
} BTNode;

//////////////////////////////////////////////////////////////////////////////////
// Helper Functions
//////////////////////////////////////////////////////////////////////////////////

BTNode* createBTNode(int item) {
    BTNode *newNode = (BTNode*)malloc(sizeof(BTNode));
    newNode->item = item;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void removeAll(BTNode **node) {
    if(*node != NULL) {
        removeAll(&((*node)->left));
        removeAll(&((*node)->right));
        free(*node);
        *node = NULL;
    }
}

void printTreeStructure(BTNode *node, int level, const char *prefix) {
    if (node == NULL) {
        for (int i = 0; i < level; i++) printf("    ");
        printf("%sNULL\n", prefix);
        return;
    }
    printTreeStructure(node->right, level + 1, "R: ");
    for (int i = 0; i < level; i++) printf("    ");
    printf("%s%d\n", prefix, node->item);
    printTreeStructure(node->left, level + 1, "L: ");
}

BTNode* createSampleTree1() {
    BTNode *root = createBTNode(5);
    root->left = createBTNode(3);
    root->right = createBTNode(7);
    root->left->left = createBTNode(1);
    root->left->right = createBTNode(2);
    root->right->left = createBTNode(4);
    root->right->right = createBTNode(8);
    return root;
}

BTNode* createSampleTree2() {
    BTNode *root = createBTNode(4);
    root->left = createBTNode(2);
    root->right = createBTNode(6);
    root->left->left = createBTNode(1);
    root->left->right = createBTNode(3);
    root->right->left = createBTNode(5);
    root->right->right = createBTNode(7);
    return root;
}

BTNode* createSampleTree3() {
    BTNode *root = createBTNode(50);
    root->left = createBTNode(20);
    root->right = createBTNode(60);
    root->left->left = createBTNode(10);
    root->left->right = createBTNode(30);
    root->left->right->left = createBTNode(55);
    root->right->right = createBTNode(80);
    return root;
}

int compareTreesDetailed(BTNode *tree1, BTNode *tree2, const char *test_name, int show_structure) {
    global_stats.total_tests++;
    
    if (tree1 == NULL && tree2 == NULL) {
        global_stats.passed_tests++;
        printf("✓ %s\n", test_name);
        return 1;
    }
    
    if (tree1 == NULL || tree2 == NULL) {
        global_stats.failed_tests++;
        printf("❌ FAILED: %s\n", test_name);
        printf("   Structure mismatch (NULL vs non-NULL)\n");
        if (show_structure) {
            printf("   Expected:\n");
            printTreeStructure(tree2, 1, "");
            printf("   Actual:\n");
            printTreeStructure(tree1, 1, "");
        }
        return 0;
    }
    
    if (tree1->item != tree2->item) {
        global_stats.failed_tests++;
        printf("❌ FAILED: %s\n", test_name);
        printf("   Value mismatch: Expected %d, Got %d\n", tree2->item, tree1->item);
        if (show_structure) {
            printf("   Expected:\n");
            printTreeStructure(tree2, 1, "");
            printf("   Actual:\n");
            printTreeStructure(tree1, 1, "");
        }
        return 0;
    }
    
    int left_match = 1, right_match = 1;
    
    if ((tree1->left == NULL) != (tree2->left == NULL) || 
        (tree1->left && tree2->left && tree1->left->item != tree2->left->item)) {
        left_match = 0;
    }
    
    if ((tree1->right == NULL) != (tree2->right == NULL) || 
        (tree1->right && tree2->right && tree1->right->item != tree2->right->item)) {
        right_match = 0;
    }
    
    if (!left_match || !right_match) {
        global_stats.failed_tests++;
        printf("❌ FAILED: %s\n", test_name);
        printf("   Subtree mismatch\n");
        if (show_structure) {
            printf("   Expected:\n");
            printTreeStructure(tree2, 1, "");
            printf("   Actual:\n");
            printTreeStructure(tree1, 1, "");
        }
        return 0;
    }
    
    global_stats.passed_tests++;
    printf("✓ %s\n", test_name);
    return 1;
}

static int printed_values[100];
static int printed_count = 0;

void reset_printed() {
    printed_count = 0;
}

void capture_print(int value) {
    if (printed_count < 100) {
        printed_values[printed_count++] = value;
    }
}

//////////////////////////////////////////////////////////////////////////////////
// Function Prototypes
//////////////////////////////////////////////////////////////////////////////////

int identical(BTNode *tree1, BTNode *tree2);
int maxHeight(BTNode *root);
int countOneChildNodes(BTNode *root);
int sumOfOddNodes(BTNode *root);
void mirrorTree(BTNode *node);
void printSmallerValues(BTNode *node, int m);
int smallestValue(BTNode *node);
int hasGreatGrandchild(BTNode *node);

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
        global_stats.passed_tests--; \
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

void test_identical() {
    printf("\n=== Testing Q1: identical ===\n");
    BTNode *tree1, *tree2;
    
    // Test 1
    tree1 = createSampleTree1();
    tree2 = createSampleTree1();
    TEST_ASSERT_INT_EQ(identical(tree1, tree2), 1, "Test 1: Two identical trees");
    removeAll(&tree1);
    removeAll(&tree2);
    
    // Test 2
    tree1 = createSampleTree1();
    tree2 = createSampleTree2();
    TEST_ASSERT_INT_EQ(identical(tree1, tree2), 0, "Test 2: Two different trees");
    removeAll(&tree1);
    removeAll(&tree2);
    
    // Test 3
    tree1 = NULL;
    tree2 = NULL;
    TEST_ASSERT_INT_EQ(identical(tree1, tree2), 1, "Test 3: Both NULL trees");
    
    // Test 4
    tree1 = createBTNode(5);
    tree2 = NULL;
    TEST_ASSERT_INT_EQ(identical(tree1, tree2), 0, "Test 4: One NULL, one not");
    removeAll(&tree1);
    
    // Test 5
    tree1 = createBTNode(5);
    tree1->left = createBTNode(3);
    tree2 = createBTNode(5);
    tree2->left = createBTNode(4);
    TEST_ASSERT_INT_EQ(identical(tree1, tree2), 0, "Test 5: Different values");
    removeAll(&tree1);
    removeAll(&tree2);
}

void test_maxHeight() {
    printf("\n=== Testing Q2: maxHeight ===\n");
    BTNode *tree;
    
    // Test 1
    tree = createSampleTree2();
    TEST_ASSERT_INT_EQ(maxHeight(tree), 2, "Test 1: Balanced tree height = 2");
    removeAll(&tree);
    
    // Test 2
    tree = createBTNode(5);
    TEST_ASSERT_INT_EQ(maxHeight(tree), 0, "Test 2: Single node height = 0");
    removeAll(&tree);
    
    // Test 3
    tree = NULL;
    TEST_ASSERT_INT_EQ(maxHeight(tree), -1, "Test 3: NULL tree height = -1");
    
    // Test 4
    tree = createBTNode(1);
    tree->left = createBTNode(2);
    tree->left->left = createBTNode(3);
    tree->left->left->left = createBTNode(4);
    TEST_ASSERT_INT_EQ(maxHeight(tree), 3, "Test 4: Skewed tree height = 3");
    removeAll(&tree);
    
    // Test 5
    tree = createSampleTree3();
    TEST_ASSERT_INT_EQ(maxHeight(tree), 3, "Test 5: Unbalanced tree height = 3");
    removeAll(&tree);
}

void test_countOneChildNodes() {
    printf("\n=== Testing Q3: countOneChildNodes ===\n");
    BTNode *tree;
    
    // Test 1
    tree = createSampleTree3();
    TEST_ASSERT_INT_EQ(countOneChildNodes(tree), 2, "Test 1: Tree with 2 one-child nodes");
    removeAll(&tree);
    
    // Test 2
    tree = createSampleTree2();
    TEST_ASSERT_INT_EQ(countOneChildNodes(tree), 0, "Test 2: Perfect binary tree = 0");
    removeAll(&tree);
    
    // Test 3
    tree = createBTNode(5);
    TEST_ASSERT_INT_EQ(countOneChildNodes(tree), 0, "Test 3: Single node = 0");
    removeAll(&tree);
    
    // Test 4
    tree = createBTNode(1);
    tree->left = createBTNode(2);
    tree->left->right = createBTNode(3);
    TEST_ASSERT_INT_EQ(countOneChildNodes(tree), 2, "Test 4: Chain = 2");
    removeAll(&tree);
    
    // Test 5
    tree = NULL;
    TEST_ASSERT_INT_EQ(countOneChildNodes(tree), 0, "Test 5: NULL tree = 0");
}

void test_sumOfOddNodes() {
    printf("\n=== Testing Q4: sumOfOddNodes ===\n");
    BTNode *tree;
    
    // Test 1
    tree = createBTNode(50);
    tree->left = createBTNode(40);
    tree->right = createBTNode(60);
    tree->left->left = createBTNode(11);
    tree->left->right = createBTNode(35);
    tree->right->left = createBTNode(80);
    tree->right->right = createBTNode(85);
    TEST_ASSERT_INT_EQ(sumOfOddNodes(tree), 131, "Test 1: Mixed sum = 131");
    removeAll(&tree);
    
    // Test 2
    tree = createBTNode(2);
    tree->left = createBTNode(4);
    tree->right = createBTNode(6);
    TEST_ASSERT_INT_EQ(sumOfOddNodes(tree), 0, "Test 2: All even = 0");
    removeAll(&tree);
    
    // Test 3
    tree = createBTNode(1);
    tree->left = createBTNode(3);
    tree->right = createBTNode(5);
    TEST_ASSERT_INT_EQ(sumOfOddNodes(tree), 9, "Test 3: All odd = 9");
    removeAll(&tree);
    
    // Test 4
    tree = createBTNode(7);
    TEST_ASSERT_INT_EQ(sumOfOddNodes(tree), 7, "Test 4: Single odd = 7");
    removeAll(&tree);
    
    // Test 5
    tree = NULL;
    TEST_ASSERT_INT_EQ(sumOfOddNodes(tree), 0, "Test 5: NULL tree = 0");
}

void test_mirrorTree() {
    printf("\n=== Testing Q5: mirrorTree ===\n");
    BTNode *tree, *expected;
    
    // Test 1
    tree = createBTNode(4);
    tree->left = createBTNode(2);
    tree->right = createBTNode(6);
    tree->left->left = createBTNode(1);
    tree->left->right = createBTNode(3);
    tree->right->left = createBTNode(5);
    tree->right->right = createBTNode(7);
    
    expected = createBTNode(4);
    expected->left = createBTNode(6);
    expected->right = createBTNode(2);
    expected->left->left = createBTNode(7);
    expected->left->right = createBTNode(5);
    expected->right->left = createBTNode(3);
    expected->right->right = createBTNode(1);
    
    mirrorTree(tree);
    if(!compareTreesDetailed(tree, expected, "Test 1: Mirror tree", 1))
        return;
    removeAll(&tree);
    removeAll(&expected);
    
    // Test 2
    tree = createBTNode(5);
    mirrorTree(tree);
    global_stats.total_tests++;
    if (tree->item == 5 && tree->left == NULL && tree->right == NULL) {
        global_stats.passed_tests++;
        printf("✓ Test 2: Single node unchanged\n");
    } else {
        global_stats.failed_tests++;
        printf("❌ FAILED: Test 2: Single node changed\n");
        return;
    }
    removeAll(&tree);
    
    // Test 3
    tree = NULL;
    mirrorTree(tree);
    global_stats.total_tests++;
    if (tree == NULL) {
        global_stats.passed_tests++;
        printf("✓ Test 3: NULL tree unchanged\n");
    } else {
        global_stats.failed_tests++;
        printf("❌ FAILED: Test 3: NULL tree modified\n");
        return;
    }
    
    // Test 4
    tree = createSampleTree2();
    expected = createSampleTree2();
    mirrorTree(tree);
    mirrorTree(tree);
    if(!compareTreesDetailed(tree, expected, "Test 4: Double mirror", 0))
        return;
    removeAll(&tree);
    removeAll(&expected);
}

void test_printSmallerValues() {
    printf("\n=== Testing Q6: printSmallerValues ===\n");
    BTNode *tree;
    
    // Test 1
    tree = createBTNode(50);
    tree->left = createBTNode(30);
    tree->right = createBTNode(60);
    tree->left->left = createBTNode(25);
    tree->left->right = createBTNode(65);
    tree->right->left = createBTNode(10);
    tree->right->right = createBTNode(75);
    
    reset_printed();
    printSmallerValues(tree, 55);
    int expected_values[] = {50, 30, 25, 10};
    TEST_ASSERT_ARRAY_EQ(printed_values, expected_values, 4, "Test 1: Found 4 values < 55");
    
    removeAll(&tree);
    
    // Test 2
    tree = createBTNode(100);
    tree->left = createBTNode(200);
    reset_printed();
    printed_count = 0;
    printSmallerValues(tree, 50);
    TEST_ASSERT_INT_EQ(printed_count, 0, "Test 2: No values smaller");
    
    removeAll(&tree);
    
    // Test 3
    tree = createBTNode(10);
    tree->left = createBTNode(5);
    tree->right = createBTNode(15);
    reset_printed();
    printed_count = 0;
    printSmallerValues(tree, 100);
    int expected_values3[] = {10, 5, 15};
    TEST_ASSERT_ARRAY_EQ(printed_values, expected_values3, 3, "Test 3: All values smaller");
    removeAll(&tree);
}

void test_smallestValue() {
    printf("\n=== Testing Q7: smallestValue ===\n");
    BTNode *tree;
    
    // Test 1
    tree = createBTNode(50);
    tree->left = createBTNode(30);
    tree->right = createBTNode(60);
    tree->left->left = createBTNode(25);
    tree->left->right = createBTNode(65);
    tree->right->left = createBTNode(10);
    tree->right->right = createBTNode(75);
    TEST_ASSERT_INT_EQ(smallestValue(tree), 10, "Test 1: Smallest = 10");
    removeAll(&tree);
    
    // Test 2
    tree = createBTNode(42);
    TEST_ASSERT_INT_EQ(smallestValue(tree), 42, "Test 2: Single node = 42");
    removeAll(&tree);
    
    // Test 3
    tree = createBTNode(1);
    tree->left = createBTNode(100);
    tree->right = createBTNode(200);
    TEST_ASSERT_INT_EQ(smallestValue(tree), 1, "Test 3: Smallest at root");
    removeAll(&tree);
    
    // Test 4
    tree = createBTNode(50);
    tree->left = createBTNode(20);
    tree->left->left = createBTNode(5);
    tree->right = createBTNode(100);
    TEST_ASSERT_INT_EQ(smallestValue(tree), 5, "Test 4: Smallest in left");
    removeAll(&tree);
    
    // Test 5
    tree = createBTNode(0);
    tree->left = createBTNode(-10);
    tree->right = createBTNode(10);
    TEST_ASSERT_INT_EQ(smallestValue(tree), -10, "Test 5: Negative values");
    removeAll(&tree);
}

void test_hasGreatGrandchild() {
    printf("\n=== Testing Q8: hasGreatGrandchild ===\n");
    BTNode *tree;
    
    // Test 1
    tree = createBTNode(50);
    tree->left = createBTNode(30);
    tree->right = createBTNode(60);
    tree->left->left = createBTNode(25);
    tree->left->right = createBTNode(65);
    tree->left->right->left = createBTNode(20);
    tree->right->left = createBTNode(10);
    tree->right->right = createBTNode(75);
    tree->right->right->right = createBTNode(15);
    
    reset_printed();
    hasGreatGrandchild(tree);
    int expected_values1[] = {50};
    TEST_ASSERT_ARRAY_EQ(printed_values, expected_values1, 1, "Test 1: Found 1 node (50)");
    removeAll(&tree);
    
    // Test 2
    tree = createSampleTree2();
    reset_printed();
    hasGreatGrandchild(tree);
    int expected_values2[] = {};
    TEST_ASSERT_ARRAY_EQ(printed_values, expected_values2, 0, "Test 2: No great-grandchildren");
    removeAll(&tree);
    
    // Test 3
    tree = createBTNode(1);
    tree->left = createBTNode(2);
    tree->right = createBTNode(3);
    tree->left->left = createBTNode(4);
    tree->left->right = createBTNode(5);
    tree->right->left = createBTNode(6);
    tree->right->right = createBTNode(7);
    tree->left->left->left = createBTNode(8);
    tree->right->right->right = createBTNode(9);
    tree->right->right->right->left = createBTNode(10);
    
    reset_printed();
    hasGreatGrandchild(tree);
    int expected_values3[] = {3, 1};
    TEST_ASSERT_ARRAY_EQ(printed_values, expected_values3, 2, "Test 3: Found 2 nodes");
    removeAll(&tree);
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
    printf("║  Binary Tree Test Suite - All 8 Questions            ║\n");
    printf("║  Enhanced with Crash & Error Detection               ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    
    RUN_SAFE_TEST(test_identical);
    RUN_SAFE_TEST(test_maxHeight);
    RUN_SAFE_TEST(test_countOneChildNodes);
    RUN_SAFE_TEST(test_sumOfOddNodes);
    RUN_SAFE_TEST(test_mirrorTree);
    RUN_SAFE_TEST(test_printSmallerValues);
    RUN_SAFE_TEST(test_smallestValue);
    RUN_SAFE_TEST(test_hasGreatGrandchild);
    
    print_test_summary();
    
    return (global_stats.failed_tests == 0 && 
            global_stats.crashed_tests == 0 && 
            global_stats.timeout_tests == 0) ? 0 : 1;
}

//////////////////////////////////////////////////////////////////////////////////
// Q1: identical
//////////////////////////////////////////////////////////////////////////////////

int identical(BTNode *tree1, BTNode *tree2) {
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// Q2: maxHeight
//////////////////////////////////////////////////////////////////////////////////

int maxHeight(BTNode *node) {
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// Q3: countOneChildNodes
//////////////////////////////////////////////////////////////////////////////////

int countOneChildNodes(BTNode *node)
{
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// Q4: sumOfOddNodes
//////////////////////////////////////////////////////////////////////////////////

int sumOfOddNodes(BTNode *root) {
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////
// Q5: mirrorTree
//////////////////////////////////////////////////////////////////////////////////

void mirrorTree(BTNode *node) {
    return;
}

//////////////////////////////////////////////////////////////////////////////////
// Q6: printSmallerValues
//////////////////////////////////////////////////////////////////////////////////

void printSmallerValues(BTNode *node, int m) {
// use capture_print(node->item) to print nodes with smaller values
    return;
}


//////////////////////////////////////////////////////////////////////////////////
// Q7: smallestValue
//////////////////////////////////////////////////////////////////////////////////

int smallestValue(BTNode *node) {
    
}

//////////////////////////////////////////////////////////////////////////////////
// Q8: hasGreatGrandchild
//////////////////////////////////////////////////////////////////////////////////

int hasGreatGrandchild(BTNode *node) {
// use capture_print(node->item) to print nodes with great-grandchildren
    return 0;
}