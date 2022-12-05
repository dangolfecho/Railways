#include <iostream>
#include <stdlib.h>
#include<vector>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
using namespace std;

const string server = "tcp://127.0.0.1:3306";
const string username = "root";
const string password = "root";

class Backend {
private:
    sql::Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;
    sql::PreparedStatement* pstmt;
public:
    Backend();
    ~Backend();
    void create();
    void add(string d_station, string e_station, string duration, int tickets, int price);
    void remove(int train_no);
};


Backend::Backend() {
    try
    {
        driver = get_driver_instance();
        con = driver->connect(server, username, password);
    }
    catch (sql::SQLException e)
    {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        system("pause");
        exit(1);
    }
    stmt = con->createStatement();
    stmt->execute("CREATE TABLE IF NOT EXISTS schedule(train_no INTEGER(7) PRIMARY KEY, starting_date VARCHAR(10), departure_station VARCHAR(50), departure_time VARCHAR(8), arrival_station VARCHAR(5), arrival_time VARCHAR(8), duration VARCHAR(8), tickets INTEGER(3), price INTEGER(3));");
    stmt->execute("CREATE TABLE IF NOT EXISTS userinfo(username VARCHAR(50) PRIMARY KEY, password VARCHAR(50));");
    stmt->execute("CREATE TABLE IF NOT EXISTS booked_tickets(username VARCHAR(50) PRIMARY KEY, journey_date VARCHAR(50), train_no INTEGER(7), no_of_tickets INTEGER(3));");
}

Backend:: ~Backend(){
    delete stmt;
    delete pstmt;
    delete con;
}

void Backend::create() {
    vector<string> stations = { "Chennai", "Madurai", "Trichy", "Coimbatore", "Bangalore", "Mettupalayam", "Mumbai", "Delhi", "Kolkata", "Hyderabad" };
}

class Login {
private:
    string username;
    string password;
public:
    int userLogin();
    int adminLogin();
};

int Login::userLogin() {
    cout << "Enter the username\n";
    cin >> username;
    cout << "Enter the password\n";
    cin >> password;
    int valid = 1;//do SQL here
    if (valid == 1) {
        return 1;
    }
    else {
        cout << "Incorrect credentials\n";
        return -1;
    }
}

int adminLogin() {
    string username, password;
    cout << "Enter the username\n";
    cin >> username;
    cout << "Enter the password\n";
    cin >> password;
    if (username == "admin" && password == "root") {
        return 1;
    }
    else {
        cout << "Incorrect credentials\n";
        return -1;
    }
}

class User {
private:
    Backend* database;
    int choice;
public:
    User(Backend* b);
   int start();
   int menu();
};

User::User(Backend* b) {
    database = b;
}

int User::start() {
    cout << "Enter 1 to login\n";
    cout << "Enter 2 to exit\n";
    cin >> choice;
    return choice;
}

class Admin {
private:
    Backend* database;
    int choice;
    void decide();
    void populate();
    void addTrain();
    void deleteTrain();
    void updateTrain();
public:
    Admin(Backend* b);
    int start();
    int menu();
};

Admin::Admin(Backend* b) {
    database = b;
}

void Admin::populate(){
    database->create();
}

void Admin::addTrain() {
    string start_station;
    string end_station;
    string duration;
    int tickets;
    int price;
    cout << "Enter the starting station\n";
    cin >> start_station;
    cout << "Enter the ending station\n";
    cin >> end_station;
    cout << "Enter the duration\n";
    cin >> duration;
    cout << "Enter the number of available tickets\n";
    cin >> tickets;
    cout << "Enter the price of a ticket\n";
    cin >> price;
    database->add(start_station, end_station, duration, tickets, price);
}

void Admin::deleteTrain() {
    int train_no;
    cout << "Enter the train number of the train to be deleted\n";
    cin >> train_no;
    database->remove(train_no);
}

void Admin::updateTrain() {
    int train_no;
    cout << "Enter the train number for which details need to be updated\n";
    cin >> train_no;

}

int Admin::start() {
    cout << "Enter 1 to login\n";
    cout << "Enter 2 to exit\n";
    cin >> choice;
    return choice;
}

int Admin::menu() {
    while (true) {
        cout << "Enter 1 to populate the database\n";
        cout << "Enter 2 to add a train route\n";
        cout << "Enter 3 to delete a train route\n";
        cout << "Enter 4 to update a train route\n";
        cout << "Enter 5 to exit\n";
        cin >> choice;
        if (choice == 1) {
            populate();
        }
        else if (choice == 2) {
            addTrain();
        }
        else if (choice == 3) {
            deleteTrain();
        }
        else if (choice == 4) {
            updateTrain();
        }
        else if (choice == 5) {
            return -1;
        }
        else {
            cout << "Enter a valid choice\n";
        }
    }
}

class Menu{
private:
    int choice;
    int decide();
public:
    int start();
    int menu();
};

int Menu::decide() {
    if (choice >= 1 && choice <= 3) {
        return choice;
    }
    else {
        return -1;
    }
}

int Menu::start() {
    while (true) {
        cout << "Welcome to the Railways Booking Page\n";
        cout << "Enter 1 to login as an user\n";
        cout << "Enter 2 to login as an admin\n";
        cout << "Enter 3 to exit\n";
        cin >> choice;
        int return_val = decide();
        if (return_val != -1) {
            return return_val;
        }
        else {
            cout << "Enter a valid input\n";
        }
    }
}

int main(){
    Menu m;
    Login l;
    Backend b;
    while (true) {
        int decide = m.start();
        if (decide == 1) {
            User u(&b);
            int decision = u.start();
            if (decision == 2) {
                break;
            }
            int attempt;
            while (true) {
                attempt = l.userLogin();
                if (attempt == 1) {
                    break;
                }
                else {
                    string answer;
                    cout << "Do you want to retry\n";
                    cout << "Enter y for yes\nEnter n for no\n";
                    cin >> answer;
                    if (answer == "y" || answer == "Y" || answer == "yes") {
                        cout << "\n";
                    }
                    else {
                        break;
                    }
                }
            }
            if (attempt == 1) {
                while (true) {
                    int val = u.menu();
                    if (val == -1) {
                        break;
                    }
                }
            }
        }
        else if (decide == 2) {
            Admin a(&b);
            int decision = a.start();
            if (decision == 2) {
                break;
            }
            int attempt;
            while (true) {
                attempt = l.adminLogin();
                if (attempt == 1) {
                    break;
                }
                else {
                    string answer;
                    cout << "Do you want to retry\n";
                    cout << "Enter y for yes\nEnter n for no\n";
                    cin >> answer;
                    if (answer == "y" || answer == "Y" || answer == "yes") {
                        cout << "\n";
                    }
                    else {
                        break;
                    }
                }
            }
            if (attempt == 1) {
                while (true) {
                    int val = a.menu();
                    if (val == -1) {
                        break;
                    }
                }
            }
        }
        else {
            break;
        }
    }
    /*
    pstmt = con->prepareStatement("INSERT INTO inventory(name, quantity) VALUES(?,?)");
    pstmt->setString(1, "banana");
    pstmt->setInt(2, 150);
    pstmt->execute();
    cout << "One row inserted." << endl;

    pstmt->setString(1, "orange");
    pstmt->setInt(2, 154);
    pstmt->execute();
    cout << "One row inserted." << endl;

    pstmt->setString(1, "apple");
    pstmt->setInt(2, 100);
    pstmt->execute();
    cout << "One row inserted." << endl;

    delete pstmt;
    delete con;
    system("pause");
    return 0;
    */
}

