// Bank-account program reads a random-access file sequentially,
// updates data already written to the file, creates new data to
// be placed in the file, and deletes data previously in the file.
#include <stdio.h>
#include <stdlib.h>

#define MAX_ACCOUNTS 100

// clientData structure definition
struct clientData
{
    unsigned int acctNum; // account number
    char lastName[15];    // account last name
    char firstName[10];   // account first name
    double balance;       // account balance
}; // end structure clientData

const size_t RECORD_SIZE = sizeof(struct clientData);

// prototypes
unsigned int enterChoice(void);
void clearInput(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void initializeFile(FILE *fPtr);

int main(int argc, char *argv[])
{
    FILE *cfPtr;         // credit.dat file pointer
    unsigned int choice; // user's choice
    int newFile = 0;

    // fopen opens the file; create it if it does not exist
    if ((cfPtr = fopen("credit.dat", "rb+")) == NULL)
    {
        if ((cfPtr = fopen("credit.dat", "wb+")) == NULL)
        {
            printf("%s: File could not be opened or created.\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        newFile = 1;
    }

    if (newFile)
    {
        initializeFile(cfPtr);
    }

    // enable user to specify action
    while ((choice = enterChoice()) != 5)
    {
        switch (choice)
        {
        case 1: // create text file from record file
            textFile(cfPtr);
            break;
        case 2: // update record
            updateRecord(cfPtr);
            break;
        case 3: // create record
            newRecord(cfPtr);
            break;
        case 4: // delete existing record
            deleteRecord(cfPtr);
            break;
        default:
            puts("Incorrect choice");
            break;
        }
    }

    fclose(cfPtr); // fclose closes the file
    return 0;
} // end main

// clear any leftover characters from stdin
void clearInput(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

// initialize the file with blank records when it is first created
void initializeFile(FILE *fPtr)
{
    struct clientData blankClient = {0, "", "", 0.0};
    unsigned int i;

    rewind(fPtr);
    for (i = 0; i < MAX_ACCOUNTS; ++i)
    {
        if (fwrite(&blankClient, RECORD_SIZE, 1, fPtr) != 1)
        {
            perror("Error initializing file");
            exit(EXIT_FAILURE);
        }
    }
    fflush(fPtr);
    rewind(fPtr);
}

// create formatted text file for printing
void textFile(FILE *readPtr)
{
    FILE *writePtr; // accounts.txt file pointer
    struct clientData client = {0, "", "", 0.0};

    if ((writePtr = fopen("accounts.txt", "w")) == NULL)
    {
        puts("accounts.txt could not be opened.");
        return;
    }

    rewind(readPtr); // sets pointer to beginning of file
    fprintf(writePtr, "%-6s%-16s%-11s%10s\n", "Acct", "Last Name", "First Name", "Balance");

    while (fread(&client, RECORD_SIZE, 1, readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr, "%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName,
                    client.balance);
        }
    }

    fclose(writePtr); // fclose closes the file
} // end function textFile

// update balance in record
void updateRecord(FILE *fPtr)
{
    unsigned int account; // account number
    double transaction;   // transaction amount
    struct clientData client = {0, "", "", 0.0};

    printf("Enter account to update (1 - %d): ", MAX_ACCOUNTS);
    if (scanf("%u", &account) != 1)
    {
        clearInput();
        puts("Invalid account number.");
        return;
    }

    if (account < 1 || account > MAX_ACCOUNTS)
    {
        puts("Account number must be between 1 and 100.");
        return;
    }

    if (fseek(fPtr, (account - 1) * RECORD_SIZE, SEEK_SET) != 0)
    {
        perror("Seek error");
        return;
    }

    if (fread(&client, RECORD_SIZE, 1, fPtr) != 1 || client.acctNum == 0)
    {
        printf("Account #%u has no information.\n", account);
        return;
    }

    printf("%-6u%-16s%-11s%10.2f\n\n", client.acctNum, client.lastName, client.firstName, client.balance);
    printf("Enter charge (+) or payment (-): ");
    if (scanf("%lf", &transaction) != 1)
    {
        clearInput();
        puts("Invalid transaction amount.");
        return;
    }

    client.balance += transaction;
    printf("%-6u%-16s%-11s%10.2f\n", client.acctNum, client.lastName, client.firstName, client.balance);

    if (fseek(fPtr, -RECORD_SIZE, SEEK_CUR) != 0)
    {
        perror("Seek error");
        return;
    }

    if (fwrite(&client, RECORD_SIZE, 1, fPtr) != 1)
    {
        perror("Write error");
        return;
    }

    fflush(fPtr);
} // end function updateRecord

// delete an existing record
void deleteRecord(FILE *fPtr)
{
    struct clientData client;                        // stores record read from file
    struct clientData blankClient = {0, "", "", 0.0}; // blank client
    unsigned int accountNum;                         // account number

    printf("Enter account number to delete (1 - %d): ", MAX_ACCOUNTS);
    if (scanf("%u", &accountNum) != 1)
    {
        clearInput();
        puts("Invalid account number.");
        return;
    }

    if (accountNum < 1 || accountNum > MAX_ACCOUNTS)
    {
        puts("Account number must be between 1 and 100.");
        return;
    }

    if (fseek(fPtr, (accountNum - 1) * RECORD_SIZE, SEEK_SET) != 0)
    {
        perror("Seek error");
        return;
    }

    if (fread(&client, RECORD_SIZE, 1, fPtr) != 1 || client.acctNum == 0)
    {
        printf("Account %u does not exist.\n", accountNum);
        return;
    }

    if (fseek(fPtr, (accountNum - 1) * RECORD_SIZE, SEEK_SET) != 0)
    {
        perror("Seek error");
        return;
    }

    if (fwrite(&blankClient, RECORD_SIZE, 1, fPtr) != 1)
    {
        perror("Write error");
        return;
    }

    fflush(fPtr);
} // end function deleteRecord

// create and insert record
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int accountNum; // account number

    printf("Enter new account number (1 - %d): ", MAX_ACCOUNTS);
    if (scanf("%u", &accountNum) != 1)
    {
        clearInput();
        puts("Invalid account number.");
        return;
    }

    if (accountNum < 1 || accountNum > MAX_ACCOUNTS)
    {
        puts("Account number must be between 1 and 100.");
        return;
    }

    if (fseek(fPtr, (accountNum - 1) * RECORD_SIZE, SEEK_SET) != 0)
    {
        perror("Seek error");
        return;
    }

    if (fread(&client, RECORD_SIZE, 1, fPtr) == 1 && client.acctNum != 0)
    {
        printf("Account #%u already contains information.\n", client.acctNum);
        return;
    }

    printf("Enter lastname, firstname, balance\n? ");
    if (scanf("%14s%9s%lf", client.lastName, client.firstName, &client.balance) != 3)
    {
        clearInput();
        puts("Invalid account data.");
        return;
    }

    client.acctNum = accountNum;
    if (fseek(fPtr, (client.acctNum - 1) * RECORD_SIZE, SEEK_SET) != 0)
    {
        perror("Seek error");
        return;
    }

    if (fwrite(&client, RECORD_SIZE, 1, fPtr) != 1)
    {
        perror("Write error");
        return;
    }

    fflush(fPtr);
} // end function newRecord

// enable user to input menu choice
unsigned int enterChoice(void)
{
    unsigned int menuChoice = 0; // variable to store user's choice

    printf("\nEnter your choice\n"
           "1 - store a formatted text file of accounts called\n"
           "    \"accounts.txt\" for printing\n"
           "2 - update an account\n"
           "3 - add a new account\n"
           "4 - delete an account\n"
           "5 - end program\n? ");

    if (scanf("%u", &menuChoice) != 1)
    {
        clearInput();
        menuChoice = 0;
    }

    return menuChoice;
} // end function enterChoice