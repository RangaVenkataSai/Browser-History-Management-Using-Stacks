#ifndef BROWSER_H
#define BROWSER_H

#include "stack.h"

/**
 * Browser Structure:
 * - backStack: Stack storing visited pages you can go back to
 * - forwardStack: Stack storing pages you can go forward to after going back
 * - currentPage: String storing the currently active website URL
 * - hasCurrentPage: Flag indicating if a page is currently open (1) or not (0)
 */
typedef struct {
    Stack backStack;
    Stack forwardStack;
    char currentPage[MAX_URL_LEN];
    int hasCurrentPage;
} Browser;

/* Browser Operation Prototypes */

// Initializes the browser and both stacks
void initBrowser(Browser *b);

// Visits a new website URL
void visitPage(Browser *b, const char *url);

// Navigates back to the previous page
void goBack(Browser *b);

// Navigates forward to the next page
void goForward(Browser *b);

// Displays the back history stack
void displayBackHistory(Browser *b);

// Displays the forward history stack
void displayForwardHistory(Browser *b);

// Displays the currently opened web page
void displayCurrentPage(Browser *b);

// Clears all history (both back and forward stacks) and current page
void clearHistory(Browser *b);

#endif // BROWSER_H
