#include <stdio.h>
#include <string.h>
#include "stack.h"

/**
 * Initializes the stack.
 * In a 0-indexed array, an empty stack is represented by top = -1.
 */
void initStack(Stack *s) {
    s->top = -1; // -1 indicates the stack currently contains no elements
}

/**
 * Checks if the stack is full.
 * Returns 1 (true) if the stack reached MAX_SIZE - 1, otherwise returns 0 (false).
 */
int isFull(Stack *s) {
    // If top reaches the last valid array index (MAX_SIZE - 1), stack is full
    return (s->top == MAX_SIZE - 1);
}

/**
 * Checks if the stack is empty.
 * Returns 1 (true) if top is -1, otherwise returns 0 (false).
 */
int isEmpty(Stack *s) {
    // When top is -1, there are no items in the stack
    return (s->top == -1);
}

/**
 * Pushes a new URL onto the top of the stack.
 * 
 * Steps:
 * 1. Check for Stack Overflow (cannot push if already full).
 * 2. Increment top by 1 to move to the next empty slot.
 * 3. Copy the URL string into the items array at the new top index.
 */
void push(Stack *s, const char *url) {
    // Step 1: Check for Stack Overflow condition
    if (isFull(s)) {
        printf("Error: Stack Overflow! Cannot push '%s'. The stack is full.\n", url);
        return;
    }

    // Step 2: Increment top to point to the new top slot
    s->top++;

    // Step 3: Safely copy the URL string into items at the new top position
    // strncpy prevents buffer overflow by limiting characters to MAX_URL_LEN - 1
    strncpy(s->items[s->top], url, MAX_URL_LEN - 1);

    // Ensure the copied string always ends with a null-terminator '\0'
    s->items[s->top][MAX_URL_LEN - 1] = '\0';
}

/**
 * Pops (removes and retrieves) the top URL from the stack.
 * 
 * Steps:
 * 1. Check for Stack Underflow (cannot pop if empty).
 * 2. Copy the top URL into the caller-provided destination buffer.
 * 3. Decrement top by 1 to remove the element from the stack.
 * 
 * Returns 1 on success, or 0 if the stack was empty.
 */
int pop(Stack *s, char *destination) {
    // Step 1: Check for Stack Underflow condition
    if (isEmpty(s)) {
        printf("Error: Stack Underflow! The stack is empty, nothing to pop.\n");
        return 0; // Return 0 indicating failure
    }

    // Step 2: Copy the item at the current top into the destination buffer
    strncpy(destination, s->items[s->top], MAX_URL_LEN - 1);
    destination[MAX_URL_LEN - 1] = '\0';

    // Step 3: Decrement top to remove the element from the stack
    s->top--;

    return 1; // Return 1 indicating success
}

/**
 * Peeks at the top URL without removing it.
 * 
 * Steps:
 * 1. Check if the stack is empty.
 * 2. Copy the top URL into destination buffer without modifying top.
 * 
 * Returns 1 on success, or 0 if the stack was empty.
 */
int peek(Stack *s, char *destination) {
    // Step 1: Check if the stack is empty
    if (isEmpty(s)) {
        return 0; // Return 0 indicating nothing to peek
    }

    // Step 2: Copy the top item without changing s->top
    strncpy(destination, s->items[s->top], MAX_URL_LEN - 1);
    destination[MAX_URL_LEN - 1] = '\0';

    return 1; // Return 1 indicating success
}

/**
 * Displays all elements in the stack from Top to Bottom (LIFO order).
 * 
 * Steps:
 * 1. Check if the stack is empty.
 * 2. Loop downwards from top down to 0, printing each URL.
 */
void display(Stack *s) {
    // Step 1: Check if stack is empty
    if (isEmpty(s)) {
        printf("  [Empty - No history]\n");
        return;
    }

    // Step 2: Print from top index down to 0 (Last-In, First-Out order)
    printf("  --- Top of Stack ---\n");
    for (int i = s->top; i >= 0; i--) {
        if (i == s->top) {
            printf("  -> [TOP] %s\n", s->items[i]);
        } else {
            printf("     [%d]   %s\n", i + 1, s->items[i]);
        }
    }
    printf("  --- Bottom of Stack ---\n");
}

/**
 * Alias function so calling either display() or displayStack() works seamlessly.
 */
void displayStack(Stack *s) {
    display(s);
}

/**
 * Clears the entire stack by resetting top to -1.
 */
void clearStack(Stack *s) {
    s->top = -1;
}
