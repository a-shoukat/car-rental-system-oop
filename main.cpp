// ============================================================
//  Car Rental System - OOP Semester Project
//  Demonstrates: Encapsulation, Inheritance, Polymorphism,
//                Constructors, File Handling
//  Author: Ayesha Shoukat - CS @ UET Narowal
// ============================================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
using namespace std;

// ---------------- BASE CLASS ----------------
class Vehicle {
protected:
    const string manufacturer;          // constant data member
public:
    Vehicle(const string& man) : manufacturer(man) {}
    virtual ~Vehicle() {}

    string getManufacturer() const { return manufacturer; }

    // pure virtual -> derived classes MUST override (polymorphism)
    virtual void display() const = 0;
    virtual int getPrice() const = 0;
};

// ---------------- DERIVED CLASS (inheritance) ----------------
class Car : public Vehicle {
private:
    string model;
    int pricePerDay;                    // encapsulated data
public:
    Car(const string& man, const string& model, int price)
        : Vehicle(man), model(model), pricePerDay(price) {}

    string getModel() const { return model; }
    int getPrice() const override { return pricePerDay; }

    void display() const override {
        cout << manufacturer << " " << model
             << "  -  Rs " << pricePerDay << "/day" << endl;
    }
};

// ---------------- CUSTOMER (encapsulation) ----------------
class Customer {
private:
    string name;
    long cnic;
public:
    Customer() : name("Guest"), cnic(0) {}

    void input() {
        cout << "\n\tWhat's your Name? ";
        getline(cin, name);
        if (name.empty()) name = "Guest";

        cout << "\n\tEnter your CNIC Number: ";
        while (!(cin >> cnic)) {
            if (cin.eof()) {
                cout << "\n\tEnd of input. Exiting.\n";
                exit(0);
            }
            cout << "\tInvalid input. Enter CNIC Number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    string getName() const { return name; }
    long getCnic() const { return cnic; }
};

// ---------------- RENTAL SYSTEM ----------------
class RentalSystem {
private:
    vector<Car> inventory;      // all cars loaded from file
    vector<Car> filtered;       // cars within customer's budget
    vector<Car> selected;       // cars chosen by the customer
    int budget;
    int days;

    int readInt(const string& prompt) {
        int v;
        cout << prompt;
        while (true) {
            if (cin >> v && v > 0) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return v;
            }
            if (cin.eof()) {
                cout << "\n\tEnd of input. Exiting.\n";
                exit(0);
            }
            cout << "\tPlease enter a valid positive number: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

public:
    RentalSystem() : budget(0), days(0) {}

    // Load car data from file (file handling)
    bool loadCars(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cerr << "Error: could not open " << filename << endl;
            return false;
        }
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string man, model, priceStr;
            getline(ss, man, ',');
            getline(ss, model, ',');
            getline(ss, priceStr);
            inventory.push_back(Car(man, model, stoi(priceStr)));
        }
        return true;
    }

    // Filter cars within the customer's budget
    int filter(int maxPrice) {
        filtered.clear();
        for (const auto& car : inventory)
            if (car.getPrice() <= maxPrice)
                filtered.push_back(car);
        return (int)filtered.size();
    }

    // Let the customer select up to 5 cars (0 = finished selecting)
    int select() {
        selected.clear();
        cout << "\n\tEnter car numbers to rent (0 to finish, max 5):\n";
        while ((int)selected.size() < 5) {
            int choice;
            cout << "\tCar number (0 = done): ";
            while (true) {
                if (cin >> choice && choice >= 0 &&
                    choice <= (int)filtered.size()) {
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }
                if (cin.eof()) {
                    cout << "\n\tEnd of input. Exiting.\n";
                    exit(0);
                }
                cout << "\tInvalid choice, try again: ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            if (choice == 0) break;
            selected.push_back(filtered[choice - 1]);
            cout << "\tAdded: ";
            filtered[choice - 1].display();
        }
        return (int)selected.size();
    }

    // Calculate total rental price
    int price() const {
        int total = 0;
        for (const auto& car : selected)
            total += car.getPrice() * days;
        return total;
    }

    void showFiltered() const {
        cout << "\n\t--- Cars within your budget ---\n";
        for (size_t i = 0; i < filtered.size(); i++) {
            cout << "\t" << (i + 1) << ". ";
            filtered[i].display();
        }
    }

    void showSelected() const {
        cout << "\n\t--- Selected Cars ---\n";
        for (const auto& car : selected) {
            cout << "\t";
            car.display();
        }
    }

    // Main program flow
    void run() {
        cout << "\n\n\t\t*** Welcome To Car Rental Company ***\n";

        Customer customer;
        customer.input();

        budget = readInt("\n\tEnter your budget per day (Rs): ");

        int count = filter(budget);
        if (count == 0) {
            cout << "\n\tNo car available within Rs " << budget << "/day.\n";
            return;
        }
        showFiltered();

        if (select() == 0) {
            cout << "\n\tNo cars selected. Goodbye!\n";
            return;
        }
        showSelected();

        days = readInt("\n\tFor how many days? ");

        cout << "\n\t==========================================\n";
        cout << "\tCustomer : " << customer.getName() << "\n";
        cout << "\tCars     : " << selected.size() << "\n";
        cout << "\tDays     : " << days << "\n";
        cout << "\tTOTAL    : Rs " << price() << "\n";
        cout << "\t==========================================\n";
        cout << "\n\tThank you for renting with us!\n";
    }
};

int main() {
    RentalSystem system;
    if (!system.loadCars("cars.txt"))
        return 1;
    system.run();
    return 0;
}
