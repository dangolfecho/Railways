#include<iostream>
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
    sql::ResultSet* res;
    vector<string> stations = { "Chennai", "Madurai", "Trichy", "Coimbatore", "Bangalore", "Mettupalayam", "Mumbai", "Delhi", "Kolkata", "Hyderabad" };
public:
    Backend();
    ~Backend();
    void create();
    void add(string d_station, string e_station, string duration, int tickets, int price);
    void remove(int train_no);
    bool usernameAlreadyExists(string username);
    void createUser(string username, string password);
    bool checkUser(string username, string password);
    vector<string> getCities();
};

bool Backend:: usernameAlreadyExists(string username) {
    pstmt = con->prepareStatement("SELECT * FROM userinfo WHERE username = '?';");
    pstmt->setString(1, username);
    res = pstmt->executeQuery();
    int count = res->rowsCount();
    if (count == 0) {
        return false;
    }
    return true;
}

void Backend::createUser(string username, string password) {
    pstmt = con->prepareStatement("INSERT INTO userinfo(username, password) VALUES(?, ?)");
    pstmt->setString(1, username);
    pstmt->setString(2, password);
    pstmt->execute();
}

bool Backend::checkUser(string username, string password) {
    pstmt = con->prepareStatement("SELECT * FROM userinfo WHERE username = ? AND password = ?;");
    pstmt->setString(1, username);
    pstmt->setString(2, password);
    res = pstmt->executeQuery();
    int count = res->rowsCount();
    if (count) {
        return true;
    }
    return false;
}

vector<string> Backend::getCities() {
    return stations;
}

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
    int count = stations.size();
    pstmt = con->prepareStatement("INSERT INTO schedule(train_no, starting_date, departure_station, departure_time, arrival_station, arrival_time, duration, tickets, price) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?);");
    int train_no = 11;
    string starting_date = "a";
    string departure_station = "B";
    string departure_time = "a";
    string arrival_station = "c";
    string arrival_time = "e";
    string duration = "f";
    int tickets = 1;
    int price = 5;
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < count; j++) {
            pstmt->setInt(1, train_no);
            pstmt->setString(3, departure_station);
            pstmt->setString(4, departure_time);
            pstmt->setString(5, arrival_station);
            pstmt->setString(6, arrival_time);
            pstmt->setString(7, duration);
            pstmt->setInt(8, tickets);
            pstmt->setInt(9, price);
            for (int k = 0; k < 90; k++) {
                pstmt->setString(2, starting_date);
                pstmt->execute();
            }
        }
    }
}

void Backend::add(string d_station, string e_station, string duration, int tickets, int price) {
    pstmt = con->prepareStatement("INSERT INTO schedule(train_no, starting_date, departure_station, departure_time, arrival_station, arrival_time, duration, tickets, price) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?);");
    int train_no = 1;
    string starting_date = "0";
    string departure_time = "2";
    string arrival_time = "2";
    pstmt->setInt(1, train_no);
	pstmt->setString(3, d_station);
	pstmt->setString(4, departure_time);
	pstmt->setString(5, e_station);
	pstmt->setString(6, arrival_time);
	pstmt->setString(7, duration);
	pstmt->setInt(8, tickets);
	pstmt->setInt(9, price);
	for (int k = 0; k < 90; k++) {
		pstmt->setString(2, starting_date);
		pstmt->execute();
	}
}

void Backend::remove(int train_no) {
    pstmt = con->prepareStatement("DELETE * FROM schedule WHERE train_no = ?;");
    pstmt->setInt(1, train_no);
    pstmt->execute();
}

class Login {
private:
    Backend* database;
    string username;
    string password;
public:
    Login(Backend* b);
    string getUsername();
    int userLogin(int choice);
    int adminLogin();
};

string Login::getUsername() {
    return username;
}

Login::Login(Backend* b) {
    database = b;
}

int Login::userLogin(int choice) {
    if (choice == 1) {
        cout << "Enter the username\n";
        cin >> username;
        cout << "Enter the password\n";
        cin >> password;
        bool valid = database->checkUser(username, password);//do SQL here
        if (valid) {
            return 1;
        }
        else {
            cout << "Incorrect credentials\n";
            return -1;
        }
    }
    else {
        while (true) {
            cout << "Enter the username\n";
            cin >> username;
            if (database->usernameAlreadyExists(username)) {
                cout << "Username already exists!\n";
            }
            else {
                cout << "Enter the password\n";
                cin >> password;
                database->createUser(username, password);
                cout << "Account has been succesfully created!\n";
                return 1;
            }
            string choice;
            cout << "Do you want to enter another username\nEnter y for yes\nEnter n for no";
            cin >> choice;
            if (choice == "y" || choice == "Y" || choice == "Yes") {
                continue;
            }
            else {
                return -1;
            }
        }
    }
}


int Login:: adminLogin() {
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
    Login* l;
    string username;
    int choice;
    vector<string> cities;
    void printCities();
    void book();
    void cancel();
public:
   User(Backend* b, Login* l);
   int start();
   int menu();
};

void User::printCities() {
    for (int i = 0; i < cities.size(); i++) {
        cout << (i + 1) << " " << cities[i] << "\n";
    }
}

void User::book() {
    cities = database->getCities();
    int start;
    int end;
    printCities();
    cout << "Choose the starting point\n";
    cin >> start;
    printCities();
    cout << "Choose the ending point\n";
    cin >> end;
    if (start == end) {
        cout << "Starting point and ending point cannot be the same!\n";
        return;
    }
    string date;
    cout << "Enter the date\n";
    cin >> date;
    database->printSelectedTrains(start, end, date);
    string ans;
    cout << "Do you want to book the ticket\n";
    cin >> ans;
    if(ans != "y"){
        "Okay!\n";
        return;
    }
    int tickets;
    cout << "Enter the no of tickets you want\n";
    cin >> tickets;
    database->book(start, end, date, tickets);
    int train_no = database->getTrainNo(start, end, date);
    //username VARCHAR(50) PRIMARY KEY, journey_date VARCHAR(50), train_no INTEGER(7), no_of_tickets INTEGER(3)
    database->updateUser(username, date, train_no, tickets);
    cout << "Tickets have been succesfully booked!\n";
}

void User::cancel() {
    vector<vector<string>> tickets = database->getBookedTickets();
    //print tickets here
    int choice;
    cout << "Enter which ticket do you want to cancel\n";
    cin >> choice;
    if (choice > 1 * *choice <= tickets.size()) {
        database->cancel(username, date, train_no);
        database->update(date, train_no, tickets);
    }
}

User::User(Backend* b, Login* l) {
    database = b;
    this->l = l;
}

int User::start() {
    cout << "Enter 1 to login\n";
    cout << "Enter 2 to create a new account\n";
    cout << "Enter 2 to exit\n";
    cin >> choice;
    return choice;
}

int User::menu() {
    username = l->getUsername();
    while (true) {
        cout << "Enter 1 to book a ticket\n";
        cout << "Enter 2 to cancel a ticket\n";
        cout << "Enter 3 to exit\n";
        cin >> choice;
        if (choice == 1) {
            book();
        }
        else if (choice == 2) {
            cancel();
        }
        else if (choice == 3) {
            return -1;
        }
        else {
            cout << "Enter a valid choice\n";
        }
    }
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
    Backend b;
    Login l(&b);
    while (true) {
        int decide = m.start();
        if (decide == 1) {
            User u(&b,&l);
            int decision = u.start();
            if (decision == 2) {
                break;
            }
            int attempt;
            while (true) {
                attempt = l.userLogin(decision);
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
    return 0;
}
