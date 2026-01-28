#include <stdio.h>
#include <string.h>

#define MAX_BOOKS 100
#define MAX_MEMBERS 50
#define MAX_ISSUES 200
#define MAX_BORROW 3
#define FINE_PER_DAY 2

typedef struct {
    int id;
    char title[50];
    char author[50];
    int quantity;
    int issuedCount;
} Book;

typedef struct {
    int id;
    char name[50];
    char phone[15];
} Member;

typedef struct {
    int bookId;
    int memberId;
    int issueDay;
    int dueDay;
    int returnDay;
    int returned;
} Issue;

Book books[MAX_BOOKS];
Member members[MAX_MEMBERS];
Issue issues[MAX_ISSUES];

int bookCount = 0, memberCount = 0, issueCount = 0;

/* Function declarations */
void loadData();
void saveData();
int findBook(int id);
int findMember(int id);
int booksHeldByMember(int memberId);

void addBook();
void addMember();
void issueBook();
void returnBook();
void displayBooks();
void booksOfMember();
void searchBook();
void mostBorrowedBooks();

int main() {
    int choice;
    loadData();

    do {
        printf("\n===== LIBRARY MANAGEMENT SYSTEM =====\n");
        printf("1. Add Book\n");
        printf("2. Add Member\n");
        printf("3. Issue Book\n");
        printf("4. Return Book\n");
        printf("5. Display All Books\n");
        printf("6. Show Books of Member\n");
        printf("7. Search Book\n");
        printf("8. Most Borrowed Books\n");
        printf("9. Save & Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addBook(); break;
            case 2: addMember(); break;
            case 3: issueBook(); break;
            case 4: returnBook(); break;
            case 5: displayBooks(); break;
            case 6: booksOfMember(); break;
            case 7: searchBook(); break;
            case 8: mostBorrowedBooks(); break;
            case 9: saveData(); printf("Data saved. Exiting...\n"); break;
            default: printf("Invalid choice!\n");
        }
    } while (choice != 9);

    return 0;
}

/* File Handling */
void loadData() {
    FILE *fp = fopen("library.dat", "rb");
    if (fp) {
        fread(&bookCount, sizeof(int), 1, fp);
        fread(books, sizeof(Book), bookCount, fp);
        fread(&memberCount, sizeof(int), 1, fp);
        fread(members, sizeof(Member), memberCount, fp);
        fread(&issueCount, sizeof(int), 1, fp);
        fread(issues, sizeof(Issue), issueCount, fp);
        fclose(fp);
    }
}

void saveData() {
    FILE *fp = fopen("library.dat", "wb");
    fwrite(&bookCount, sizeof(int), 1, fp);
    fwrite(books, sizeof(Book), bookCount, fp);
    fwrite(&memberCount, sizeof(int), 1, fp);
    fwrite(members, sizeof(Member), memberCount, fp);
    fwrite(&issueCount, sizeof(int), 1, fp);
    fwrite(issues, sizeof(Issue), issueCount, fp);
    fclose(fp);
}

/* Helpers */
int findBook(int id) {
    for (int i = 0; i < bookCount; i++)
        if (books[i].id == id) return i;
    return -1;
}

int findMember(int id) {
    for (int i = 0; i < memberCount; i++)
        if (members[i].id == id) return i;
    return -1;
}

int booksHeldByMember(int memberId) {
    int count = 0;
    for (int i = 0; i < issueCount; i++)
        if (issues[i].memberId == memberId && !issues[i].returned)
            count++;
    return count;
}

/* Core Functions */
void addBook() {
    Book b;
    printf("Enter Book ID: ");
    scanf("%d", &b.id);

    if (findBook(b.id) != -1) {
        printf("Duplicate Book ID!\n");
        return;
    }

    printf("Enter Title: ");
    scanf(" %[^\n]", b.title);
    printf("Enter Author: ");
    scanf(" %[^\n]", b.author);
    printf("Enter Quantity: ");
    scanf("%d", &b.quantity);

    b.issuedCount = 0;
    books[bookCount++] = b;
    printf("Book added successfully.\n");
}

void addMember() {
    Member m;
    printf("Enter Member ID: ");
    scanf("%d", &m.id);

    if (findMember(m.id) != -1) {
        printf("Duplicate Member ID!\n");
        return;
    }

    printf("Enter Name: ");
    scanf(" %[^\n]", m.name);
    printf("Enter Contact Number: ");
    scanf("%s", m.phone);

    members[memberCount++] = m;
    printf("Member added successfully.\n");
}

void issueBook() {
    int bid, mid, day;
    printf("Enter Book ID: ");
    scanf("%d", &bid);
    printf("Enter Member ID: ");
    scanf("%d", &mid);

    int b = findBook(bid);
    int m = findMember(mid);

    if (b == -1 || m == -1) {
        printf("Invalid book or member.\n");
        return;
    }

    if (books[b].quantity == 0) {
        printf("Book out of stock.\n");
        return;
    }

    if (booksHeldByMember(mid) >= MAX_BORROW) {
        printf("Member reached borrowing limit.\n");
        return;
    }

    printf("Enter Issue Day (number): ");
    scanf("%d", &day);

    Issue i = {bid, mid, day, day + 14, 0, 0};
    issues[issueCount++] = i;

    books[b].quantity--;
    books[b].issuedCount++;

    printf("Book issued. Due on day %d.\n", i.dueDay);
}

void returnBook() {
    int bid, mid, day;
    printf("Enter Book ID: ");
    scanf("%d", &bid);
    printf("Enter Member ID: ");
    scanf("%d", &mid);

    for (int i = 0; i < issueCount; i++) {
        if (issues[i].bookId == bid &&
            issues[i].memberId == mid &&
            !issues[i].returned) {

            printf("Enter Return Day: ");
            scanf("%d", &day);
            issues[i].returned = 1;
            issues[i].returnDay = day;

            int late = day - issues[i].dueDay;
            if (late > 0)
                printf("Late by %d days. Fine: %d\n", late, late * FINE_PER_DAY);
            else
                printf("Returned on time.\n");

            books[findBook(bid)].quantity++;
            return;
        }
    }
    printf("No matching issue record found.\n");
}

void displayBooks() {
    for (int i = 0; i < bookCount; i++) {
        printf("%d | %s | %s | Qty: %d | %s\n",
               books[i].id,
               books[i].title,
               books[i].author,
               books[i].quantity,
               books[i].quantity > 0 ? "Available" : "Out of Stock");
    }
}

void booksOfMember() {
    int mid;
    printf("Enter Member ID: ");
    scanf("%d", &mid);

    for (int i = 0; i < issueCount; i++) {
        if (issues[i].memberId == mid) {
            printf("Book ID: %d | Returned: %s\n",
                   issues[i].bookId,
                   issues[i].returned ? "Yes" : "No");
        }
    }
}

void searchBook() {
    char key[50];
    printf("Enter title or author: ");
    scanf(" %[^\n]", key);

    for (int i = 0; i < bookCount; i++) {
        if (strstr(books[i].title, key) || strstr(books[i].author, key)) {
            printf("%d | %s | %s\n", books[i].id, books[i].title, books[i].author);
        }
    }
}

void mostBorrowedBooks() {
    printf("\nMost Borrowed Books:\n");
    for (int i = 0; i < bookCount; i++) {
        printf("%s -> Issued %d times\n", books[i].title, books[i].issuedCount);
    }
}
