#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "browser.h"

/**
 * main.c
 * 
 * Console-based Browser History Management application.
 * 
 * Demonstrates:
 * - Back Stack (to revisit past websites)
 * - Forward Stack (to revisit pages after going back)
 * - Current Page tracking
 * 
 * Features an interactive while loop and switch-case menu.
 */

// Function to print the available menu options to the console
void displayMenu(void) {
    printf("\n=========================================\n");
    printf("     BROWSER HISTORY MANAGEMENT MENU     \n");
    printf("=========================================\n");
    printf(" 1. Visit New Website\n");
    printf(" 2. Go Back\n");
    printf(" 3. Go Forward\n");
    printf(" 4. Display Back History\n");
    printf(" 5. Display Forward History\n");
    printf(" 6. Display Current Page\n");
    printf(" 7. Clear History\n");
    printf(" 8. Exit\n");
    printf("=========================================\n");
    printf("Enter your choice (1-8): ");
}

int main(void) {
    // Step 1: Create and initialize the Browser instance
    Browser browser;
    initBrowser(&browser);

    int choice = 0;
    char url[MAX_URL_LEN];

    printf("*************************************************\n");
    printf("* Welcome to Browser History Management System! *\n");
    printf("*      (Implemented using Stacks in C)          *\n");
    printf("*************************************************\n");

    // Main loop: Repeatedly display the menu until the user selects Exit (8)
    while (1) {
        displayMenu();

        // Validate user input: check if user typed an integer
        if (scanf("%d", &choice) != 1) {
            // If the user entered letters or symbols instead of a number,
            // clear the input buffer so the loop doesn't get stuck.
            while (getchar() != '\n');
            printf("\n[Error] Invalid input! Please enter a number between 1 and 8.\n");
            continue;
        }

        // Consume the leftover newline character in the input stream
        while (getchar() != '\n');

        // Handle the user's choice using a switch-case statement
        switch (choice) {
            case 1:
                // Option 1: Visit New Website
                printf("\nEnter website URL (e.g. google.com): ");
                if (fgets(url, sizeof(url), stdin) != NULL) {
                    // Remove trailing newline character added by fgets
                    url[strcspn(url, "\r\n")] = '\0';
                    visitPage(&browser, url);
                }
                break;

            case 2:
                // Option 2: Go Back to previous website
                printf("\nNavigating Back...\n");
                goBack(&browser);
                break;

            case 3:
                // Option 3: Go Forward to next website
                printf("\nNavigating Forward...\n");
                goForward(&browser);
                break;

            case 4:
                // Option 4: Display Back History Stack
                displayBackHistory(&browser);
                break;

            case 5:
                // Option 5: Display Forward History Stack
                displayForwardHistory(&browser);
                break;

            case 6:
                // Option 6: Display Current Active Page
                printf("\n");
                displayCurrentPage(&browser);
                break;

            case 7:
                // Option 7: Clear All History
                printf("\nClearing History...\n");
                clearHistory(&browser);
                break;

            case 8:
                // Option 8: Exit the program
                printf("\nThank you for using Browser History Manager. Goodbye!\n");
                exit(0);

            default:
                // Handles numbers outside 1-8
                printf("\n[Error] Invalid choice '%d'! Please select a number from 1 to 8.\n", choice);
                break;
        }
    }

    return 0;
}
