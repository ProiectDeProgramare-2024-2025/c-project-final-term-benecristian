#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#define CLEAR "cls"
#else
#define CLEAR "clear"
#endif

#define FILENAME "wallet.dat"
#define MAX_DESC 50
#define DATE_LEN 11

typedef struct {
    char date[DATE_LEN];
    char description[MAX_DESC];
    float amount;
    char type; // 'I'ncome or 'E'xpense
} Transaction;

void setColor(int color) {
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
#else
    printf("\033[1;%dm", color);
#endif
}

void resetColor() {
#ifdef _WIN32
    setColor(15); // White
#else
    printf("\033[0m");
#endif
}

void clearScreen() {
    system(CLEAR);
}

int validateDate(const char* date) {
    if (strlen(date) != 10) return 0;
    if (date[2] != '/' || date[5] != '/') return 0;

    int day, month, year;
    if (sscanf(date, "%d/%d/%d", &day, &month, &year) != 3) return 0;

    if (day < 1 || day > 31) return 0;
    if (month < 1 || month > 12) return 0;
    if (year < 1900 || year > 2100) return 0;

    return 1;
}

void getCurrentDate(char* date) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date, "%02d/%02d/%04d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900);
}

void addTransaction() {
    Transaction t;
    char buffer[100];

    clearScreen();
    setColor(11); // Light cyan
    printf("=== Add New Transaction ===\n");
    resetColor();

    // Date input with validation
    while (1) {
        printf("Transaction date (dd/mm/yyyy) [today]: ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (strlen(buffer) == 0) {
            getCurrentDate(t.date);
            break;
        }

        if (validateDate(buffer)) {
            strcpy(t.date, buffer);
            break;
        }

        setColor(12); // Light red
        printf("Invalid date format! Please use dd/mm/yyyy\n");
        resetColor();
    }

    // Description input
    while (1) {
        printf("Description (max %d chars): ", MAX_DESC-1);
        fgets(t.description, sizeof(t.description), stdin);
        t.description[strcspn(t.description, "\n")] = '\0';

        if (strlen(t.description) > 0) break;

        setColor(12);
        printf("Description cannot be empty!\n");
        resetColor();
    }

    // Amount input
    while (1) {
        printf("Amount (positive number): ");
        fgets(buffer, sizeof(buffer), stdin);

        if (sscanf(buffer, "%f", &t.amount) == 1 && t.amount > 0) {
            break;
        }

        setColor(12);
        printf("Invalid amount! Must be a positive number.\n");
        resetColor();
    }

    // Transaction type
    while (1) {
        printf("Type (I)ncome or (E)xpense: ");
        fgets(buffer, sizeof(buffer), stdin);
        t.type = toupper(buffer[0]);

        if (t.type == 'I' || t.type == 'E') break;

        setColor(12);
        printf("Please enter 'I' or 'E'!\n");
        resetColor();
    }

    // Save to file
    FILE* file = fopen(FILENAME, "ab");
    if (file == NULL) {
        setColor(12);
        printf("Error saving transaction!\n");
        resetColor();
        return;
    }

    fwrite(&t, sizeof(Transaction), 1, file);
    fclose(file);

    setColor(10); // Light green
    printf("\nTransaction saved successfully!\n");
    resetColor();
}

void viewTransactions() {
    clearScreen();
    setColor(11);
    printf("=== Transaction History ===\n");
    resetColor();

    FILE* file = fopen(FILENAME, "rb");
    if (file == NULL) {
        setColor(14); // Yellow
        printf("No transactions found.\n");
        resetColor();
        return;
    }

    Transaction t;
    float balance = 0.0;
    int count = 0;

    printf("%-12s %-20s %10s %10s\n", "Date", "Description", "Amount", "Balance");
    printf("------------------------------------------------\n");

    while (fread(&t, sizeof(Transaction), 1, file)) {
        setColor(15); // White
        printf("%-12s ", t.date);

        setColor(14); // Yellow
        printf("%-20s ", t.description);

        if (t.type == 'I') {
            setColor(10); // Green for income
            printf("%10.2f ", t.amount);
            balance += t.amount;
        } else {
            setColor(12); // Red for expense
            printf("%10.2f ", -t.amount);
            balance -= t.amount;
        }

        setColor(11); // Cyan for balance
        printf("%10.2f\n", balance);
        count++;
    }

    fclose(file);

    printf("\n");
    setColor(15);
    printf("Total transactions: ");
    setColor(14);
    printf("%d\n", count);

    setColor(15);
    printf("Current balance: ");
    if (balance >= 0) {
        setColor(10);
    } else {
        setColor(12);
    }
    printf("%.2f\n", balance);
    resetColor();
}

void showMainMenu() {
    clearScreen();
    setColor(14); // Yellow
    printf("=== Electronic Wallet System ===\n");
    resetColor();

    setColor(11); // Cyan
    printf("1. ");
    resetColor();
    printf("Add Transaction\n");

    setColor(11);
    printf("2. ");
    resetColor();
    printf("View Transactions\n");

    setColor(11);
    printf("3. ");
    resetColor();
    printf("Exit\n");

    setColor(10); // Green
    printf("\nSelect an option: ");
    resetColor();
}

int main() {
    int choice;
    char input[10];

    do {
        showMainMenu();
        fgets(input, sizeof(input), stdin);
        choice = atoi(input);

        switch(choice) {
            case 1:
                addTransaction();
                break;
            case 2:
                viewTransactions();
                break;
            case 3:
                clearScreen();
                setColor(10);
                printf("Thank you for using Electronic Wallet System!\n");
                resetColor();
                exit(0);
            default:
                setColor(12);
                printf("Invalid choice! Please select 1-3.\n");
                resetColor();
        }

        printf("\nPress Enter to continue...");
        while (getchar() != '\n');

    } while (1);

    return 0;
}
