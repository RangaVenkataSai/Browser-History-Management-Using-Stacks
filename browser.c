#include <stdio.h>
#include <string.h>
#include "browser.h"

/**
 * Initializes the browser system:
 * - Initializes backStack (top = -1)
 * - Initializes forwardStack (top = -1)
 * - Clears currentPage and marks hasCurrentPage = 0
 */
void initBrowser(Browser *b) {
    initStack(&b->backStack);
    initStack(&b->forwardStack);
    b->currentPage[0] = '\0';
    b->hasCurrentPage = 0;
}

/**
 * Visits a new website:
 * 1. If a page is already open, push the currentPage into backStack.
 * 2. Set the new URL as the currentPage.
 * 3. Clear forwardStack (visiting a new link invalidates any forward history).
 */
void visitPage(Browser *b, const char *url) {
    // Validate that the URL is not empty
    if (url == NULL || strlen(url) == 0) {
        printf("Error: URL cannot be empty.\n");
        return;
    }

    // Step 1: Save current page into backStack if a page is currently open
    if (b->hasCurrentPage) {
        push(&b->backStack, b->currentPage);
    }

    // Step 2: Make the new URL the current page
    strncpy(b->currentPage, url, MAX_URL_LEN - 1);
    b->currentPage[MAX_URL_LEN - 1] = '\0';
    b->hasCurrentPage = 1;

    // Step 3: Clear forwardStack whenever a new website is visited
    clearStack(&b->forwardStack);

    printf("Successfully visited: %s\n", b->currentPage);
}

/**
 * Navigates back to the previous website:
 * 1. Check if backStack is empty. If so, display "No previous page".
 * 2. Otherwise, push currentPage into forwardStack.
 * 3. Pop the top URL from backStack and make it the new currentPage.
 */
void goBack(Browser *b) {
    // Step 1: If backStack is empty, there is no previous page to return to
    if (isEmpty(&b->backStack)) {
        printf("No previous page (Back stack is empty).\n");
        return;
    }

    // Step 2: Push current page into forwardStack so we can return to it later
    if (b->hasCurrentPage) {
        push(&b->forwardStack, b->currentPage);
    }

    // Step 3: Pop the top page from backStack and make it the currentPage
    pop(&b->backStack, b->currentPage);
    b->hasCurrentPage = 1;

    printf("Went back to: %s\n", b->currentPage);
}

/**
 * Navigates forward to the next website:
 * 1. Check if forwardStack is empty. If so, display "No forward page".
 * 2. Otherwise, push currentPage into backStack.
 * 3. Pop the top URL from forwardStack and make it the new currentPage.
 */
void goForward(Browser *b) {
    // Step 1: If forwardStack is empty, there is no forward page
    if (isEmpty(&b->forwardStack)) {
        printf("No forward page (Forward stack is empty).\n");
        return;
    }

    // Step 2: Push current page into backStack so we can go back to it later
    if (b->hasCurrentPage) {
        push(&b->backStack, b->currentPage);
    }

    // Step 3: Pop the top page from forwardStack and make it the currentPage
    pop(&b->forwardStack, b->currentPage);
    b->hasCurrentPage = 1;

    printf("Went forward to: %s\n", b->currentPage);
}

/**
 * Displays the currently active web page.
 */
void displayCurrentPage(Browser *b) {
    if (!b->hasCurrentPage) {
        printf("Current Page: None (No page opened yet)\n");
    } else {
        printf("Current Page: %s\n", b->currentPage);
    }
}

/**
 * Displays the history stored in the back stack.
 */
void displayBackHistory(Browser *b) {
    printf("\n=== Back History Stack ===\n");
    display(&b->backStack);
}

/**
 * Displays the history stored in the forward stack.
 */
void displayForwardHistory(Browser *b) {
    printf("\n=== Forward History Stack ===\n");
    display(&b->forwardStack);
}

/**
 * Clears all history (both back and forward stacks) and resets current page.
 */
void clearHistory(Browser *b) {
    clearStack(&b->backStack);
    clearStack(&b->forwardStack);
    b->currentPage[0] = '\0';
    b->hasCurrentPage = 0;
    printf("Browser history cleared successfully.\n");
}
