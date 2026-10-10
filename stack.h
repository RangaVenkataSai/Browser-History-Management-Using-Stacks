#ifndef STACK_H
#define STACK_H

#define MAX_SIZE 100
#define MAX_URL_LEN 256

/**
 * Stack Structure:
 * - items: 2D char array to store website URLs as strings
 * - top: integer representing the index of the top element (-1 when empty)
 */
typedef struct {
    char items[MAX_SIZE][MAX_URL_LEN];
    int top;
} Stack;

/* Function Declarations (Prototypes) */

// Initializes the stack by setting top = -1
void initStack(Stack *s);

// Checks if the stack is full (returns 1 if true, 0 if false)
int isFull(Stack *s);

// Checks if the stack is empty (returns 1 if true, 0 if false)
int isEmpty(Stack *s);

// Pushes a new URL onto the stack
void push(Stack *s, const char *url);

// Pops and returns the top URL from the stack into destination buffer
int pop(Stack *s, char *destination);

// Returns the top URL without removing it (returns 1 if found, 0 if empty)
int peek(Stack *s, char *destination);

// Displays all URLs currently stored in the stack
void display(Stack *s);
void displayStack(Stack *s);

// Clears all elements from the stack
void clearStack(Stack *s);

#endif // STACK_H
