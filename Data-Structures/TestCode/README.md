# Data Structures Test Suite

A comprehensive testing framework for Binary Trees, Linked Lists, Stacks, Queues, and Binary Search Trees with enhanced crash detection and error handling.

## 📋 Overview

This repository contains 4 test suites covering fundamental data structure operations in C:

- **BT_Test.c** - Binary Tree operations (8 questions)
- **LL_Test.c** - Linked List operations (7 questions)
- **SQ_Test.c** - Stack & Queue operations (7 questions)
- **BST_Test.c** - Binary Search Tree traversals (5 questions)

## ✨ Features

- **Crash Detection**: Automatically catches segmentation faults, illegal instructions, and other fatal errors
- **Timeout Detection**: Identifies infinite loops with 3-second timeout per test
- **Detailed Error Reporting**: Shows expected vs actual output with line numbers
- **Comprehensive Statistics**: Tracks passed, failed, crashed, and timed-out tests
- **Visual Feedback**: Color-coded output with emoji indicators (✅ ❌ 🔴 ⏱️)

## 🚀 Quick Start

### Compilation

```bash
# Binary Tree Tests
gcc BT_Test.c -o bt_test

# Linked List Tests
gcc LL_Test.c -o ll_test

# Stack & Queue Tests
gcc SQ_Test.c -o sq_test

# Binary Search Tree Tests
gcc BST_Test.c -o bst_test
```

### Running Tests

```bash
./bt_test   # Run Binary Tree tests
./ll_test   # Run Linked List tests
./sq_test   # Run Stack & Queue tests
./bst_test  # Run BST tests
```

## 📚 Test Coverage

### Binary Tree (BT_Test.c)

| Question | Function | Description |
|----------|----------|-------------|
| Q1 | `identical()` | Check if two trees are identical |
| Q2 | `maxHeight()` | Find maximum height of tree |
| Q3 | `countOneChildNodes()` | Count nodes with exactly one child |
| Q4 | `sumOfOddNodes()` | Sum all odd-valued nodes |
| Q5 | `mirrorTree()` | Mirror a tree (swap left/right) |
| Q6 | `printSmallerValues()` | Print values smaller than m |
| Q7 | `smallestValue()` | Find smallest value in tree |
| Q8 | `hasGreatGrandchild()` | Find nodes with great-grandchildren |

### Linked List (LL_Test.c)

| Question | Function | Description |
|----------|----------|-------------|
| Q1 | `insertSortedLL()` | Insert into sorted list |
| Q2 | `alternateMergeLinkedList()` | Merge two lists alternately |
| Q3 | `moveOddItemsToBack()` | Move odd values to end |
| Q4 | `moveEvenItemsToBack()` | Move even values to end |
| Q5 | `frontBackSplitLinkedList()` | Split list into two halves |
| Q6 | `moveMaxToFront()` | Move maximum value to front |
| Q7 | `RecursiveReverse()` | Reverse list recursively |

### Stack & Queue (SQ_Test.c)

| Question | Function | Description |
|----------|----------|-------------|
| Q1 | `removeOddValues()` | Remove odd values from queue |
| Q2 | `removeEvenValues()` | Remove even values from stack |
| Q3 | `isStackPairwiseConsecutive()` | Check consecutive pairs |
| Q4 | `reverse()` | Reverse queue using stack |
| Q5 | `recursiveReverse()` | Reverse queue recursively |
| Q6 | `removeUntil()` | Remove elements until value |
| Q7 | `balanced()` | Check balanced parentheses |

### Binary Search Tree (BST_Test.c)

| Question | Function | Description |
|----------|----------|-------------|
| Q1 | `levelOrderTraversal()` | Level-order (breadth-first) |
| Q2 | `inOrderIterative()` | In-order (iterative) |
| Q3 | `preOrderIterative()` | Pre-order (iterative) |
| Q4 | `postOrderIterativeS1()` | Post-order (single stack) |
| Q5 | `postOrderIterativeS2()` | Post-order (two stacks) |

## 🔧 Implementation Guide

### Step 1: Locate Function Stubs

Each test file contains empty function implementations at the bottom:

```c
//////////////////////////////////////////////////////////////////////////////////
// Q1: functionName
//////////////////////////////////////////////////////////////////////////////////

int functionName(Parameters) {
    return 0;  // Replace with your implementation
}
```

### Step 2: Implement Functions

Replace the stub with your solution:

```c
int identical(BTNode *tree1, BTNode *tree2) {
    if (tree1 == NULL && tree2 == NULL) return 1;
    if (tree1 == NULL || tree2 == NULL) return 0;
    
    return (tree1->item == tree2->item) &&
           identical(tree1->left, tree2->left) &&
           identical(tree1->right, tree2->right);
}
```

### Step 3: Test Your Implementation

```bash
gcc BT_Test.c -o bt_test && ./bt_test
```

### Step 4: Review Results

```
╔══════════════════════════════════════════════════════╗
║               TEST SUITE SUMMARY                     ║
╠══════════════════════════════════════════════════════╣
║  Total Tests:  28                                    ║
║  Passed:       28  ✅                                ║
║  Failed:       0   ❌                                ║
║  Crashed:      0   🔴                                ║
║  Timeout:      0   ⏱️                                 ║
╠══════════════════════════════════════════════════════╣
║  🎉 ALL TESTS PASSED! 🎉                             ║
╚══════════════════════════════════════════════════════╝
```

## 📝 Important Notes

### For Binary Tree Tests
- Use `capture_print(value)` instead of `printf()` in Q6 and Q8
- Tree structure is checked for Q5 (mirrorTree)
- NULL trees should be handled gracefully

### For Linked List Tests
- Functions modify the list in-place unless specified
- Size field must be updated correctly
- Handle edge cases (empty list, single node)

### For Stack & Queue Tests
- Use provided push/pop/enqueue/dequeue functions
- Q7 (balanced) should return 0 for balanced, 1 for unbalanced
- Handle odd-length stacks in Q3

### For BST Tests
- Use `capture_print(value)` instead of `printf()`
- All traversals are iterative (no recursion)
- **Q1-Q5 are already implemented** - study them as reference

## 🐛 Common Errors

### Segmentation Fault
```
🔴 CRASH DETECTED: SEGMENTATION FAULT (accessing invalid memory)
   In test: test_functionName
```
**Solution**: Check for NULL pointer access, array bounds

### Timeout
```
⏱️ TIMEOUT: Test exceeded 3 seconds (possible infinite loop)
   In test: test_functionName
```
**Solution**: Check loop conditions, ensure progress toward base case

### Failed Test
```
❌ FAILED: Test 1: Description
   Expected: 1 2 3
   Actual:   1 2 4
   Location: Line 234
```
**Solution**: Review logic, check edge cases

## 🎯 Tips for Success

1. **Start Simple**: Implement and test one function at a time
2. **Handle Edge Cases**: Test with empty structures, single elements
3. **Memory Management**: Free allocated memory in Linked List/Tree operations
4. **Use Helper Functions**: Leverage provided stack/queue operations
5. **Read Test Cases**: Understand expected behavior from test descriptions

## 📊 Grading

Exit codes:
- `0` - All tests passed
- `1` - Some tests failed, crashed, or timed out

## 🔍 Debugging Tips

1. **Add Debug Prints**: Temporarily add `printf()` statements
2. **Test Incrementally**: Comment out other tests, focus on one
3. **Check Test Input**: Review sample tree/list structures in test cases
4. **Valgrind**: Use `valgrind ./test_name` to detect memory leaks

## 📄 License

Educational use only. Part of a data structures course assignment.

## 🤝 Contributing

This is an educational test suite. Implement the functions yourself to learn!

---

**Good luck with your implementation! 🚀**