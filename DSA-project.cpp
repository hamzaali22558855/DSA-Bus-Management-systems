#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <sstream>
using namespace std;


struct Bus {
    int busID;
    string driverName;
    string destination;
    int seats;
    Bus* next;
};


struct Booking {
    int bookingID;
    int busID;
    string passengerName;
    Booking* next;
};


struct Action {
    string type;  // "ADD_BUS", "BOOK_TICKET", or "DELETE_BUS"
    int busID;
    int bookingID;
};

class BusTicketManagementSystem {
private:
    Bus* busHead;           
    Booking* bookingHead;
    int bookingCounter;     
    stack<Action> undoStack; 

public:
    BusTicketManagementSystem() : busHead(nullptr), bookingHead(nullptr), bookingCounter(1) {}

    void addBus() {
        Bus* newBus = new Bus;

        cout << "\nEnter Bus ID: ";
        cin >> newBus->busID;

        cout << "Enter Driver's Name: ";
        cin.ignore();
        getline(cin, newBus->driverName);

        cout << "Enter Destination: ";
        getline(cin, newBus->destination);

        cout << "Enter Number of Seats: ";
        cin >> newBus->seats;

        newBus->next = busHead;
        busHead = newBus;

        
        undoStack.push({"ADD_BUS", newBus->busID, 0});
        cout << "Bus added successfully!\n";
    }

    
    void viewBuses() const {
        if (!busHead) {
            cout << "\nNo buses available.\n";
            return;
        }

        cout << "\n--- Available Buses ---\n";
        Bus* temp = busHead;
        while (temp) {
            cout << "Bus ID: " << temp->busID
                 << ", Driver: " << temp->driverName
                 << ", Destination: " << temp->destination
                 << ", Seats Available: " << temp->seats << "\n";
            temp = temp->next;
        }
    }

    
    void bookTicket() {
        int busID;
        cout << "\nEnter Bus ID for Booking: ";
        cin >> busID;

        Bus* bus = findBus(busID);
        if (!bus) {
            cout << "Bus not found.\n";
            return;
        }

        if (bus->seats <= 0) {
            cout << "No seats available on this bus.\n";
            return;
        }

        Booking* newBooking = new Booking;
        newBooking->bookingID = bookingCounter++;
        newBooking->busID = busID;

        cout << "Enter Passenger Name: ";
        cin.ignore();
        getline(cin, newBooking->passengerName);

        newBooking->next = bookingHead;
        bookingHead = newBooking;

        bus->seats--; // Reduce available seats
        undoStack.push({"BOOK_TICKET", busID, newBooking->bookingID}); // Push action to undo stack
        cout << "Ticket booked successfully! Booking ID: " << newBooking->bookingID << "\n";
    }

    
    void viewBookings() const {
        if (!bookingHead) {
            cout << "\nNo bookings available.\n";
            return;
        }

        cout << "\n--- All Bookings ---\n";
        Booking* temp = bookingHead;
        while (temp) {
            cout << "Booking ID: " << temp->bookingID
                 << ", Bus ID: " << temp->busID
                 << ", Passenger: " << temp->passengerName << "\n";
            temp = temp->next;
        }
    }

    
    void saveToFile() const {
        ofstream busFile("buses.txt");
        ofstream bookingFile("bookings.txt");

        if (!busFile || !bookingFile) {
            cout << "Error saving to files.\n";
            return;
        }

        
        Bus* tempBus = busHead;
        while (tempBus) {
            busFile << tempBus->busID << "," << tempBus->driverName << ","
                    << tempBus->destination << "," << tempBus->seats << "\n";
            tempBus = tempBus->next;
        }

        
        Booking* tempBooking = bookingHead;
        while (tempBooking) {
            bookingFile << tempBooking->bookingID << "," << tempBooking->busID << ","
                        << tempBooking->passengerName << "\n";
            tempBooking = tempBooking->next;
        }

        cout << "\nData saved successfully.\n";
    }

    
    void loadFromFile() {
        ifstream busFile("buses.txt");
        ifstream bookingFile("bookings.txt");

        if (!busFile || !bookingFile) {
            cout << "Error loading from files.\n";
            return;
        }

       
        clearData();

        string line;

    
        while (getline(busFile, line)) {
            Bus* newBus = new Bus;
            stringstream ss(line);
            string seats;
            getline(ss, seats, ','); newBus->busID = stoi(seats);
            getline(ss, newBus->driverName, ',');
            getline(ss, newBus->destination, ',');
            getline(ss, seats); newBus->seats = stoi(seats);

            newBus->next = busHead;
            busHead = newBus;
        }

       
        while (getline(bookingFile, line)) {
            Booking* newBooking = new Booking;
            stringstream ss(line);
            string bookingID, busID;
            getline(ss, bookingID, ','); newBooking->bookingID = stoi(bookingID);
            getline(ss, busID, ','); newBooking->busID = stoi(busID);
            getline(ss, newBooking->passengerName);

            newBooking->next = bookingHead;
            bookingHead = newBooking;
        }

        cout << "\nData loaded successfully.\n";
    }

    
    void undo() {
        if (undoStack.empty()) {
            cout << "No actions to undo.\n";
            return;
        }

        Action lastAction = undoStack.top();
        undoStack.pop();

        if (lastAction.type == "ADD_BUS") {
            deleteBus(lastAction.busID, false); // Delete without recording another undo
        } else if (lastAction.type == "BOOK_TICKET") {
            cancelBooking(lastAction.bookingID, false); // Cancel without recording another undo
        }
        cout << "Last action undone.\n";
    }

    ~BusTicketManagementSystem() {
        clearData();
    }

private:
    
    void clearData() {
        while (busHead) {
            Bus* temp = busHead;
            busHead = busHead->next;
            delete temp;
        }
        while (bookingHead) {
            Booking* temp = bookingHead;
            bookingHead = bookingHead->next;
            delete temp;
        }
    }

    
    Bus* findBus(int id) const {
        Bus* temp = busHead;
        while (temp) {
            if (temp->busID == id)
                return temp;
            temp = temp->next;
        }
        return nullptr;
    }

    void deleteBus(int busID, bool recordUndo = true) {
        Bus** current = &busHead;
        while (*current && (*current)->busID != busID) {
            current = &((*current)->next);
        }
        if (*current) {
            Bus* temp = *current;
            *current = (*current)->next;
            delete temp;
            if (recordUndo) undoStack.push({"DELETE_BUS", busID, 0});
        }
    }

    
    void cancelBooking(int bookingID, bool recordUndo = true) {
        Booking** current = &bookingHead;
        while (*current && (*current)->bookingID != bookingID) {
            current = &((*current)->next);
        }
        if (*current) {
            Booking* temp = *current;
            *current = (*current)->next;
            if (recordUndo) undoStack.push({"CANCEL_BOOKING", 0, bookingID});
            delete temp;
        }
    }
};


int main() {
    BusTicketManagementSystem system;
    int choice;

    do {
        cout << "\n--- Bus Ticket Management System ---\n";
        cout << "1. Add Bus\n2. View Buses\n3. Book Ticket\n4. View Bookings\n";
        cout << "5. Save Data\n6. Load Data\n7. Undo Last Action\n8. Exit\nEnter choice: ";
        cin >> choice;

        switch (choice) {
            case 1: system.addBus(); break;
            case 2: system.viewBuses(); break;
            case 3: system.bookTicket(); break;
            case 4: system.viewBookings(); break;
            case 5: system.saveToFile(); break;
            case 6: system.loadFromFile(); break;
            case 7: system.undo(); break;
            case 8: cout << "Exiting system. Goodbye!\n"; break;
            default: cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 8);

    return 0;
}

