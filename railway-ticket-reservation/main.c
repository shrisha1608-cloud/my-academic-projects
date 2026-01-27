#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_TRAINS 10
#define MAX_SEATS 50
#define ADMIN_PASS "admin123"

typedef struct {
    int trainNo;
    char name[30];
    char source[20];
    char destination[20];
    int totalSeats;
    int availableSeats;
    float fare;
} Train;

typedef struct {
    char pname[30];
    int age;
    char gender;
} Passenger;

typedef struct {
    int pnr;
    int trainNo;
    Passenger passenger;
    char status[15];   // BOOKED / WL / CANCELLED
    int wlPosition;    // 0 if booked, >0 if WL
    time_t bookingTime;
} Reservation;

/* ---------- Utility ---------- */
void pressEnter() {
    printf("\nPress Enter to continue...");
    getchar(); getchar();
}

/* ---------- Case-insensitive search helper ---------- */
void toLowerStr(char *str) {
    for (int i = 0; str[i]; i++) str[i] = tolower(str[i]);
}

/* ---------- Global Demo Data ---------- */
Train trains[MAX_TRAINS] = {
    {101, "Express1", "CityA", "CityB", MAX_SEATS, MAX_SEATS, 500.0},
    {102, "Express2", "CityC", "CityD", MAX_SEATS, MAX_SEATS, 400.0},
};
int trainCount = 2;

Reservation reservations[100]; // max 100 demo bookings
int reservationCount = 0;
int pnrCounter = 1000;

/* ---------- Display ---------- */
void displayTrains() {
    printf("\nTrainNo  Name  From    To  Seats   Fare\n");
    for (int i = 0; i < trainCount; i++) {
        printf("%d  %s  %s  %s  %d  %.2f\n",
               trains[i].trainNo,
               trains[i].name,
               trains[i].source,
               trains[i].destination,
               trains[i].availableSeats,
               trains[i].fare);
    }
}

/* ---------- Search Trains ---------- */
void searchTrains() {
    char src[20], dest[20];
    printf("Enter Source: ");
    scanf("%s", src);
    printf("Enter Destination: ");
    scanf("%s", dest);

    char srcLower[20], destLower[20];
    strcpy(srcLower, src); strcpy(destLower, dest);
    toLowerStr(srcLower); toLowerStr(destLower);

    int found = 0;
    printf("\nMatching Trains:\n");
    printf("TrainNo  Name  Seats  Fare\n");

    for (int i = 0; i < trainCount; i++) {
        char tSrc[20], tDest[20];
        strcpy(tSrc, trains[i].source); strcpy(tDest, trains[i].destination);
        toLowerStr(tSrc); toLowerStr(tDest);

        if (strcmp(tSrc, srcLower) == 0 && strcmp(tDest, destLower) == 0) {
            printf("%d  %s  %d  %.2f\n",
                   trains[i].trainNo,
                   trains[i].name,
                   trains[i].availableSeats,
                   trains[i].fare);
            found = 1;
        }
    }
    if (!found) printf("No trains found for this route.\n");
}

/* ---------- Booking ---------- */
void bookTicket() {
    int tno;
    Passenger p;
    printf("Enter Train Number: ");
    scanf("%d", &tno);

    int idx = -1;
    for (int i = 0; i < trainCount; i++)
        if (trains[i].trainNo == tno) idx = i;

    if (idx == -1) {
        printf("Invalid train number.\n");
        return;
    }

    printf("Passenger Name: ");
    scanf("%s", p.pname);
    printf("Age: ");
    scanf("%d", &p.age);
    if (p.age <= 0) {
        printf("Invalid age.\n");
        return;
    }
    printf("Gender (M/F): ");
    scanf(" %c", &p.gender);

    Reservation r;
    r.pnr = ++pnrCounter;
    r.trainNo = tno;
    r.passenger = p;
    r.bookingTime = time(NULL);

    if (trains[idx].availableSeats > 0) {
        strcpy(r.status, "BOOKED");
        r.wlPosition = 0;
        trains[idx].availableSeats--;
    } else {
        strcpy(r.status, "WL");
        int wlCount = 0;
        for (int i = 0; i < reservationCount; i++)
            if (reservations[i].trainNo == tno && strcmp(reservations[i].status, "WL") == 0)
                wlCount++;
        r.wlPosition = wlCount + 1;
    }

    reservations[reservationCount++] = r;

    printf("\nTicket Details:\nPNR: %d\nTrain: %s (%d)\nStatus: %s", 
           r.pnr, trains[idx].name, trains[idx].trainNo, r.status);
    if (strcmp(r.status, "WL") == 0)
        printf(" (WL position %d)", r.wlPosition);
    printf("\nFare: %.2f\n", trains[idx].fare);
}

/* ---------- Cancellation ---------- */
void cancelTicket() {
    int pnr, found = 0;
    printf("Enter PNR: ");
    scanf("%d", &pnr);

    for (int i = 0; i < reservationCount; i++) {
        Reservation *r = &reservations[i];
        if (r->pnr == pnr && strcmp(r->status, "BOOKED") == 0) {
            strcpy(r->status, "CANCELLED");
            found = 1;

            // Update seat availability
            for (int j = 0; j < trainCount; j++) {
                if (trains[j].trainNo == r->trainNo) {
                    trains[j].availableSeats++;
                    break;
                }
            }

            // Promote first WL passenger
            for (int k = 0; k < reservationCount; k++) {
                if (reservations[k].trainNo == r->trainNo && strcmp(reservations[k].status, "WL") == 0) {
                    strcpy(reservations[k].status, "BOOKED");
                    reservations[k].wlPosition = 0;

                    // Decrease availableSeats again for promoted passenger
                    for (int j = 0; j < trainCount; j++) {
                        if (trains[j].trainNo == reservations[k].trainNo) {
                            trains[j].availableSeats--;
                            break;
                        }
                    }

                    break;  // promote only first WL
                }
            }

            break;
        }
    }

    if (found)
        printf("Ticket cancelled. Refund processed.\n");
    else
        printf("PNR not found or already cancelled.\n");
}

/* ---------- Status ---------- */
void checkStatus() {
    int pnr;
    printf("Enter PNR: ");
    scanf("%d", &pnr);

    for (int i = 0; i < reservationCount; i++) {
        Reservation r = reservations[i];
        if (r.pnr == pnr) {
            printf("\nPassenger: %s\nStatus: %s", r.passenger.pname, r.status);
            if (strcmp(r.status, "WL") == 0)
                printf(" (WL position %d)", r.wlPosition);
            printf("\n");
            return;
        }
    }
    printf("PNR not found.\n");
}

/* ---------- Admin ---------- */
void adminPanel() {
    char pass[20];
    printf("Admin Password: ");
    scanf("%s", pass);

    if (strcmp(pass, ADMIN_PASS) != 0) {
        printf("Access denied.\n");
        return;
    }

    int choice;
    printf("\n1.Add Train\n2.View Trains\nChoice: ");
    scanf("%d", &choice);

    if (choice == 1 && trainCount < MAX_TRAINS) {
        Train t;
        printf("Train No: "); scanf("%d", &t.trainNo);
        printf("Name: "); scanf("%s", t.name);
        printf("Source: "); scanf("%s", t.source);
        printf("Destination: "); scanf("%s", t.destination);
        printf("Fare: "); scanf("%f", &t.fare);
        t.totalSeats = MAX_SEATS;
        t.availableSeats = MAX_SEATS;
        trains[trainCount++] = t;
        printf("Train added.\n");
    } else if (choice == 2) {
        displayTrains();
    }
}

/* ---------- Main ---------- */
int main() {
    // Pre-fill demo bookings
    reservations[reservationCount++] = (Reservation){++pnrCounter, 101, {"Alice", 25, 'F'}, "BOOKED", 0, time(NULL)};
    reservations[reservationCount++] = (Reservation){++pnrCounter, 102, {"Bob", 30, 'M'}, "BOOKED", 0, time(NULL)};

    int choice;
    while (1) {
        printf("\n--- Railway Reservation System ---\n");
        printf("1.View Trains\n2.Search Trains (Source-Destination)\n3.Book Ticket\n4.Cancel Ticket\n5.Check Status\n6.Admin\n7.Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: displayTrains(); break;
            case 2: searchTrains(); break;
            case 3: bookTicket(); break;
            case 4: cancelTicket(); break;
            case 5: checkStatus(); break;
            case 6: adminPanel(); break;
            case 7: exit(0);
            default: printf("Invalid choice.\n");
        }
        pressEnter();
    }
}
