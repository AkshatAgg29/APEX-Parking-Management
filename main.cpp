#include <iostream>
#include <vector>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <regex>

using namespace std;

struct CarEntry {
    time_t entryTime;
    double fare;
    string parkingNumber;
    string carNumber;
    CarEntry* next;
};

struct ParkingSpace {
    bool isOccupied;
    string carNumber;
};

struct User {
    string username;
    string password;
};

struct LinkedList {
    CarEntry* head;
    LinkedList() : head(nullptr) {}
    ~LinkedList() {
        while (head) {
            CarEntry* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

map<string, User> userDatabase;

bool validateCarNumber(string carNumber) {
    regex pattern("[A-Za-z]{2}[0-9]{2}[A-Za-z]{2}[0-9]{4}");
    return regex_match(carNumber, pattern);
}

double calculateFare(time_t parkingTime) {
    double fare = 0;
    int minutes = parkingTime / 60;

    if (minutes <= 30) {
        fare = 50;
    } else if (minutes <= 180) {
        fare = 50 + ((minutes - 30) / 30) * 50;
    } else if (minutes <= 360) {
        fare = 400 + ((minutes - 180) / 30) * 50;
    } else {
        fare = 900 + ((minutes - 360) / 30) * 50;
    }

    return fare;
}

bool authenticateUser(const string& username, const string& password) {
    if (userDatabase.find(username) != userDatabase.end()) {
        if (userDatabase[username].password == password) {
            return true;
        }
    }
    return false;
}

void enterCar(LinkedList& parkingDatabase, vector<ParkingSpace>& parkingSpaces, int& availableSpaces) {
    if (availableSpaces > 0) {
        string carNumber;
        time_t currentTime = time(nullptr);

        do {
            cout << "Enter car number (format: XX00XX0000): ";
            cin >> carNumber;
            transform(carNumber.begin(), carNumber.end(), carNumber.begin(), ::toupper);

            if (!validateCarNumber(carNumber)) {
                cout << "Invalid car number format. Please try again." << endl;
            }
        } while (!validateCarNumber(carNumber));

        if (parkingDatabase.head != nullptr) {
            CarEntry* current = parkingDatabase.head;
            while (current != nullptr) {
                if (current->carNumber == carNumber) {
                    cout << "Car is already parked." << endl;
                    return;
                }
                current = current->next;
            }
        }

        for (int i = 0; i < parkingSpaces.size(); i++) {
            if (!parkingSpaces[i].isOccupied) {
                CarEntry* entry = new CarEntry;
                entry->entryTime = currentTime;
                entry->parkingNumber = "P" + to_string(i + 1);
                entry->carNumber = carNumber;
                entry->next = nullptr;

                CarEntry* lastEntry = parkingDatabase.head;
                if (lastEntry != nullptr) {
                    while (lastEntry->next != nullptr) {
                        lastEntry = lastEntry->next;
                    }
                    lastEntry->next = entry;
                } else {
                    parkingDatabase.head = entry;
                }

                parkingSpaces[i].isOccupied = true;
                parkingSpaces[i].carNumber = carNumber;
                availableSpaces--;
                cout << "Car parked in space " << i + 1 << " with parking number " << entry->parkingNumber << endl;
                return;
            }
        }
    } else {
        cout << "No available parking spaces." << endl;
    }
}



void exitCar(LinkedList& parkingDatabase, vector<ParkingSpace>& parkingSpaces, int& availableSpaces) {
    string carNumber;
    time_t currentTime = time(nullptr);

    do {
        cout << "Enter car number (format: XX00XX0000): ";
        cin >> carNumber;
        transform(carNumber.begin(), carNumber.end(), carNumber.begin(), ::toupper);

        if (!validateCarNumber(carNumber)) {
            cout << "Invalid car number format. Please try again." << endl;
        }
    } while (!validateCarNumber(carNumber));

    CarEntry* prev = nullptr;
    CarEntry* current = parkingDatabase.head;

    while (current != nullptr && current->carNumber != carNumber) {
        prev = current;
        current = current->next;
    }

    if (current != nullptr) {
        CarEntry* entry = current;
        cout << "Entry time: " << put_time(localtime(&entry->entryTime), "%c") << endl;
        cout << "Exit time: " << put_time(localtime(&currentTime), "%c") << endl;
        time_t parkingTime = currentTime - entry->entryTime;
        cout << "Total parking time: " << parkingTime << " seconds" << endl;

        double totalFare = calculateFare(parkingTime);

        if (prev != nullptr) {
            prev->next = current->next;
        } else {
            parkingDatabase.head = current->next;
        }

        for (int i = 0; i < parkingSpaces.size(); i++) {
            if (parkingSpaces[i].carNumber == carNumber) {
                parkingSpaces[i].isOccupied = false;
                parkingSpaces[i].carNumber = "";
                availableSpaces++;
                break;
            }
        }

        delete entry;

        cout << "Total fare: ₹" << totalFare << endl;
    } else {
        cout << "Car not found in the parking lot." << endl;
    }
}

void displayAvailableSpaces(int availableSpaces) {
    cout << "Available parking spaces: " << availableSpaces << endl;
}

void displayMenu() {
    cout << "Parking Management System" << endl;
    cout << "1. Existing User" << endl;
    cout << "2. Register User" << endl;
    cout << "3. Display Available Spaces" << endl;
    cout << "4. Exit" << endl;
    cout << "Enter your choice: ";
}


void checkParkingSpace(const LinkedList& parkingDatabase) {
    string carNumber;
    cout << "Enter car number (format: XX00XX0000): ";
    cin >> carNumber;
    transform(carNumber.begin(), carNumber.end(), carNumber.begin(), ::toupper);

    CarEntry* current = parkingDatabase.head;

    while (current != nullptr && current->carNumber != carNumber) {
        current = current->next;
    }

    if (current != nullptr) {
        const CarEntry& entry = *current;
        cout << "Car with registered number " << carNumber << " is parked in space " << entry.parkingNumber << endl;
    } else {
        cout << "Car not found in the parking lot." << endl;
    }
}

void checkParkedCar(const LinkedList& parkingDatabase, const vector<ParkingSpace>& parkingSpaces) {
    string parkingNumber;
    cout << "Enter parking number (format: P1, P2, ...): ";
    cin >> parkingNumber;

    CarEntry* current = parkingDatabase.head;

    while (current != nullptr && current->parkingNumber != parkingNumber) {
        current = current->next;
    }

    if (current != nullptr) {
        const CarEntry& entry = *current;
        const string& carNumber = entry.carNumber;

        cout << "Car with car number " << carNumber << " is parked in space " << parkingNumber << endl;
    } else {
        cout << "Parking space " << parkingNumber << " is not occupied." << endl;
    }
}

void drawParkingSpaces(const vector<ParkingSpace>& parkingSpaces) {
    cout << "Parking Spaces:" << endl;
    for (const ParkingSpace& space : parkingSpaces) {
        if (space.isOccupied) {
            cout << ". . . . . . . . . . . .  . . .  "<<endl;
            cout << ".      .. . . . .            .  "<<endl;
            cout << ".      ..  . .  .            .  "<<endl;
            cout << ".      .. . . . .            .  "<<endl;
            cout << ".      ..                    .  "<<endl;
            cout << ".      .. .                  .  "<<endl;
            cout << ". . . . . . . . . . .  . . . .  "<<endl;
        } else {
            cout << ". . . . . . . . . . . .  . . .  "<<endl;
            cout << ".     ..       ..            .  "<<endl;
            cout << ".      ..     ..             .  "<<endl;
            cout << ".       ..   ..              .  "<<endl;
            cout << ".        .. ..               .  "<<endl;
            cout << ".         ...                .  "<<endl;
            cout << ". . . . . . . . . . .  . . . .  "<<endl;
        }
    }
    cout << endl;
}

int main() {
    const int totalParkingSpaces = 20;
    int availableSpaces = totalParkingSpaces;
    LinkedList parkingDatabase;
    vector<ParkingSpace> parkingSpaces(totalParkingSpaces);

    while (true) {
        displayMenu();
        int choice;
        cin >> choice;

        if (choice == 1) {
            string username, password;
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;

            if (authenticateUser(username, password)) {
                while (true) {
                    cout << "Parking Management System" << endl;
                    cout << "1. Enter Car" << endl;
                    cout << "2. Exit Car" << endl;
                    cout << "3. Display Available Spaces" << endl;
                    cout << "4. Check Parking Space" << endl;
                    cout << "5. Check Parked Car" << endl;
                    cout << "6. Exit" << endl;
                    cout << "Enter your choice: ";
                    cin >> choice;

                    if (choice == 1) {
                        enterCar(parkingDatabase, parkingSpaces, availableSpaces);
                        drawParkingSpaces(parkingSpaces);
                    } else if (choice == 2) {
                        exitCar(parkingDatabase, parkingSpaces, availableSpaces);
                        drawParkingSpaces(parkingSpaces);
                    } else if (choice == 3) {
                        displayAvailableSpaces(availableSpaces);
                    } else if (choice == 4) {
                        checkParkingSpace(parkingDatabase);
                    } else if (choice == 5) {
                        checkParkedCar(parkingDatabase, parkingSpaces);
                    } else if (choice == 6) {
                        break;
                    } else {
                        cout << "Invalid choice. Please select a valid option." << endl;
                    }
                }
            } else {
                cout << "Wrong username or password. Please try again." << endl;
            }
        } else if (choice == 2) {
            string newUsername, newPassword;
            cout << "Enter a new username: ";
            cin >> newUsername;
            cout << "Enter a password: ";
            cin >> newPassword;
            userDatabase[newUsername] = {newUsername, newPassword};
            cout << "User registered successfully." << endl;
        } else if (choice == 3) {
            drawParkingSpaces(parkingSpaces);
        } else if (choice == 4) {
            break;
        } else {
            cout << "Invalid choice. Please select a valid option." << endl;
        }
    }

    cout << "Exiting the parking management system." << endl;

    return 0;
}