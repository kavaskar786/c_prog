#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

// Define enumeration for room status
enum RoomStatus {
    NOT_OCCUPIED,
    OCCUPIED
};

// Define structures
struct Guest {
    int id;
    char name[50];
    char checkInDate[15];
    char checkOutDate[15];
};

struct Staff {
    char name[50];
};

struct Room {
    int roomNumber;
    int capacity;
    enum RoomStatus status;
    struct Guest guest;
    struct Staff dataEntryStaff;
    struct Staff housekeepingStaff;
};

struct Hotel {
    struct Room rooms[50];
};

// Function prototypes
void initializeRooms(struct Hotel* hotel);
void displayRooms(struct Hotel* hotel);
void checkIn(struct Hotel* hotel, sqlite3* db);
void checkOut(struct Hotel* hotel, sqlite3* db);
void searchGuest(struct Hotel* hotel);
void updateHousekeepingStaff(struct Hotel* hotel);
void updateDataEntryStaff(struct Hotel* hotel);
void saveData(struct Hotel* hotel, sqlite3* db);
void loadData(struct Hotel* hotel, sqlite3* db);

int main() {
    sqlite3* db;
    if (sqlite3_open("hotel.db", &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    struct Hotel hotel;
    initializeRooms(&hotel);
    loadData(&hotel, db);
    int choice;
    do {
        printf("\nHotel Management System\n");
        printf("1. Display Rooms\n");
        printf("2. Check-In\n");
        printf("3. Check-Out\n");
        printf("4. Search Guest\n");
        printf("5. Update Housekeeping Staff\n");
        printf("6. Update Data Entry Staff\n");
        printf("7. Save Data\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayRooms(&hotel);
                break;
            case 2:
                checkIn(&hotel, db);
                break;
            case 3:
                checkOut(&hotel, db);
                break;
            case 4:
                searchGuest(&hotel);
                break;
            case 5:
                updateHousekeepingStaff(&hotel);
                break;
            case 6:
                updateDataEntryStaff(&hotel);
                break;
            case 7:
                saveData(&hotel, db);
                break;
            case 8:
                printf("Exiting the program.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 8);

    sqlite3_close(db);
    return 0;
}

void initializeRooms(struct Hotel* hotel) {
    for (int i = 0; i < 50; i++) {
        hotel->rooms[i].roomNumber = i + 1;
        hotel->rooms[i].capacity = (i % 5) + 1; // Varying capacities (1 to 5)
        hotel->rooms[i].status = NOT_OCCUPIED; // Not occupied

        // Set default staff names
        sprintf(hotel->rooms[i].dataEntryStaff.name, "DataEntryStaff%d", i + 1);
        sprintf(hotel->rooms[i].housekeepingStaff.name, "HousekeepingStaff%d", i + 1);
    }
}

void displayRooms(struct Hotel* hotel) {
    printf("\nRoom Number\tCapacity\tOccupied\tData Entry Staff\tHousekeeping Staff\n");
    for (int i = 0; i < 50; i++) {
        printf("%d\t\t%d\t\t%s\t\t%s\t\t%s\n", hotel->rooms[i].roomNumber, hotel->rooms[i].capacity,
               hotel->rooms[i].status == OCCUPIED ? "Yes" : "No",
               hotel->rooms[i].dataEntryStaff.name, hotel->rooms[i].housekeepingStaff.name);
    }
}

void checkIn(struct Hotel* hotel, sqlite3* db) {
    int roomNumber;
    printf("Enter room number for check-in: ");
    scanf("%d", &roomNumber);

    if (roomNumber < 1 || roomNumber > 50) {
        printf("Invalid room number.\n");
        return;
    }

    if (hotel->rooms[roomNumber - 1].status == OCCUPIED) {
        printf("Room is already occupied.\n");
        return;
    }

    printf("Enter guest name: ");
    scanf("%s", hotel->rooms[roomNumber - 1].guest.name);
    printf("Enter check-in date: ");
    scanf("%s", hotel->rooms[roomNumber - 1].guest.checkInDate);
    printf("Enter check-out date: ");
    scanf("%s", hotel->rooms[roomNumber - 1].guest.checkOutDate);

    hotel->rooms[roomNumber - 1].status = OCCUPIED;
    printf("Check-in successful.\n");
}

void checkOut(struct Hotel* hotel, sqlite3* db) {
    int roomNumber;
    printf("Enter room number for check-out: ");
    scanf("%d", &roomNumber);

    if (roomNumber < 1 || roomNumber > 50) {
        printf("Invalid room number.\n");
        return;
    }

    if (hotel->rooms[roomNumber - 1].status == NOT_OCCUPIED) {
        printf("Room is not occupied.\n");
        return;
    }

    hotel->rooms[roomNumber - 1].status = NOT_OCCUPIED;
    printf("Check-out successful.\n");
}

void searchGuest(struct Hotel* hotel) {
    char guestName[50];
    printf("Enter guest name to search: ");
    scanf("%s", guestName);

    int found = 0;
    for (int i = 0; i < 50; i++) {
        if (hotel->rooms[i].status == OCCUPIED && strcmp(guestName, hotel->rooms[i].guest.name) == 0) {
            printf("Guest found in room %d\n", hotel->rooms[i].roomNumber);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Guest not found.\n");
    }
}

void updateHousekeepingStaff(struct Hotel* hotel) {
    int roomNumber;
    printf("Enter room number to update housekeeping staff: ");
    scanf("%d", &roomNumber);

    if (roomNumber < 1 || roomNumber > 50) {
        printf("Invalid room number.\n");
        return;
    }

    printf("Enter new housekeeping staff name: ");
    scanf("%s", hotel->rooms[roomNumber - 1].housekeepingStaff.name);
    printf("Housekeeping staff updated for room %d.\n", roomNumber);
}

void updateDataEntryStaff(struct Hotel* hotel) {
    int roomNumber;
    printf("Enter room number to update data entry staff: ");
    scanf("%d", &roomNumber);

    if (roomNumber < 1 || roomNumber > 50) {
        printf("Invalid room number.\n");
        return;
    }

    printf("Enter new data entry staff name: ");
    scanf("%s", hotel->rooms[roomNumber - 1].dataEntryStaff.name);
    printf("Data entry staff updated for room %d.\n", roomNumber);
}

void saveData(struct Hotel* hotel, sqlite3* db) {
    char* errMsg;
    if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS rooms (roomNumber INT, capacity INT, status INT, name TEXT, checkInDate TEXT, checkOutDate TEXT, dataEntryStaff TEXT, housekeepingStaff TEXT);", NULL, NULL, &errMsg) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        return;
    }

    sqlite3_exec(db, "BEGIN TRANSACTION", 0, 0, 0);

    for (int i = 0; i < 50; i++) {
        char query[400];
        sprintf(query, "INSERT INTO rooms (roomNumber, capacity, status, name, checkInDate, checkOutDate, dataEntryStaff, housekeepingStaff) VALUES (%d, %d, %d, '%s', '%s', '%s', '%s', '%s');",
                hotel->rooms[i].roomNumber, hotel->rooms[i].capacity, hotel->rooms[i].status,
                hotel->rooms[i].guest.name, hotel->rooms[i].guest.checkInDate, hotel->rooms[i].guest.checkOutDate,
                hotel->rooms[i].dataEntryStaff.name, hotel->rooms[i].housekeepingStaff.name);

        if (sqlite3_exec(db, query, NULL, NULL, &errMsg) != SQLITE_OK) {
            fprintf(stderr, "SQL error: %s\n", errMsg);
            sqlite3_free(errMsg);
        }
    }

    sqlite3_exec(db, "COMMIT", 0, 0, 0);
    printf("Data saved successfully.\n");
}

void loadData(struct Hotel* hotel, sqlite3* db) {
    sqlite3_stmt* stmt;
    char query[] = "SELECT roomNumber, capacity, status, name, checkInDate, checkOutDate, dataEntryStaff, housekeepingStaff FROM rooms;";
    
    if (sqlite3_prepare_v2(db, query, -1, &stmt, 0) != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int roomNumber = sqlite3_column_int(stmt, 0);
        hotel->rooms[roomNumber - 1].capacity = sqlite3_column_int(stmt, 1);
        hotel->rooms[roomNumber - 1].status = sqlite3_column_int(stmt, 2);
        strcpy(hotel->rooms[roomNumber - 1].guest.name, sqlite3_column_text(stmt, 3));
        strcpy(hotel->rooms[roomNumber - 1].guest.checkInDate, sqlite3_column_text(stmt, 4));
        strcpy(hotel->rooms[roomNumber - 1].guest.checkOutDate, sqlite3_column_text(stmt, 5));
        strcpy(hotel->rooms[roomNumber - 1].dataEntryStaff.name, sqlite3_column_text(stmt, 6));
        strcpy(hotel->rooms[roomNumber - 1].housekeepingStaff.name, sqlite3_column_text(stmt, 7));
    }

    sqlite3_finalize(stmt);
    printf("Data loaded successfully.\n");
}