#include<iostream>
#include <stdlib.h>
#include<vector>
#include<time.h>

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
	vector<int> train_no;
    vector<vector<string>> details;
	vector<int> tickets;
	vector<int> price;
    vector<int> booked_train_no;
    vector<int> booked_tickets;
    int generateNumber(int no_of_digits);
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
    void getSelectedTrains(int start, int end, string date);
    int printSelectedTrains();
    void book(int train_choice, int tickets);
    void updateUser(string username, string date, int train_no, int tickets);
    int getBookedTickets(string username);
    void printBookedTickets();
    void cancel(string username, int choice);
    void update(int choice);
};

int Backend::generateNumber(int no_of_digits) {
    srand(time(NULL));
    int val = 0;
    int mult = 1;
    for (int i = 0; i < no_of_digits; i++) {
        val += mult * (rand() % 10);
        mult *= 10;
    }
    return val;
}

bool Backend:: usernameAlreadyExists(string username) {
    pstmt = con->prepareStatement("SELECT * FROM userinfo WHERE username = ?;");
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
    con->setSchema("giffy");
    stmt = con->createStatement();
    stmt->execute("CREATE TABLE IF NOT EXISTS schedule(train_no INTEGER(7) PRIMARY KEY, starting_date VARCHAR(10), departure_station VARCHAR(50), departure_time VARCHAR(8), arrival_station VARCHAR(5), arrival_time VARCHAR(8), duration VARCHAR(8), tickets INTEGER(3), price INTEGER(3));");
    stmt->execute("CREATE TABLE IF NOT EXISTS userinfo(username VARCHAR(50) PRIMARY KEY, password VARCHAR(50));");
    stmt->execute("CREATE TABLE IF NOT EXISTS booked_tickets(username VARCHAR(50) PRIMARY KEY, journey_date VARCHAR(50), train_no INTEGER(7), no_of_tickets INTEGER(3));");
    create();
}

Backend:: ~Backend(){
    delete stmt;
    delete pstmt;
    delete con;
}

void Backend::create() {
    int count = stations.size();
    pstmt = con->prepareStatement("INSERT INTO schedule(train_no, starting_date, departure_station, departure_time, arrival_station, arrival_time, duration, tickets, price) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?);");
    string starting_date = "08/12/2022";
    string departure_time = "a";
    string arrival_time = "e";
    string duration = "f";
    for (int i = 0; i < count; i++) {
        pstmt->setString(3, stations[i]);
        for (int j = 0; j < count; j++) {
            if (i != j) {
                pstmt->setString(4, departure_time);
                pstmt->setString(5, stations[j]);
                pstmt->setString(6, arrival_time);
                pstmt->setString(7, duration);
                pstmt->setInt(8, generateNumber(3));
                pstmt->setInt(9, generateNumber(3));
                for (int k = 0; k < 90; k++) {
                    pstmt->setInt(1, generateNumber(7));
                    pstmt->setString(2, starting_date);
                    pstmt->execute();
                }
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
    
void Backend::getSelectedTrains(int start, int end, string date) {
    details.clear();
    price.clear();
    tickets.clear();
    string dep_city = stations[start - 1];
    string end_city = stations[end - 1];
    pstmt = con->prepareStatement("SELECT * FROM schedule WHERE (departure_station = '?' AND arrival_station = '?' AND date = '?');");
    pstmt->setString(1, dep_city);
    pstmt->setString(2, end_city);
    pstmt->setString(3, date);
    res = pstmt->executeQuery();
    int size = 0;
    vector<string> temp;
    while (res->next()) {
        train_no.push_back(res->getInt("train_no"));
        temp.push_back("starting_date");
        temp.push_back("departure_station");
        temp.push_back("departure_time");
        temp.push_back("arrival_station");
        temp.push_back("arrival_time");
        temp.push_back("duration");
        tickets.push_back(res->getInt("tickets"));
        price.push_back(res->getInt("price"));
        details.push_back(temp);
        temp.clear();
    }
}

int Backend::printSelectedTrains() {
    if (details.size() == 0) {
        cout << "No trains are available\n";
        return -1;
    }
    int i = 0;
    printf("%d %10s %10s %10s %10s %10s %10s %5d %5d", "Date of Journey Start", "Departure Station", "Departure Time", "Arrival Station", "Arrival Time", "Duration", "Tickets", "Price");
    for (int i = 0; i < details.size(); i++) {
        printf("%d ", (i + 1));
        for (int j = 0; j < details[i].size(); j++) {
            printf("%10s ", details[i][j]);
        }
        printf("%5d %5d\n", tickets[i], price[i]);
    }
    return i;
}

void Backend::book(int train_choice, int tickets) {
    pstmt = con->prepareStatement("UPDATE schedule SET tickets = tickets - ? WHERE train_no = ?;");
    pstmt->setInt(1, train_no[train_choice-1]);
    pstmt->executeUpdate();
}

void Backend::updateUser(string username, string date, int train_no, int tickets) {
    pstmt = con->prepareStatement("INSERT INTO booked_tickets(username, journey_date, train_no, no_of_tickets) VALUES(?, ?, ?, ?);");
    pstmt->setString(1, username);
    pstmt->setString(2, date);
    pstmt->setInt(3, train_no);
    pstmt->setInt(4, tickets);
    pstmt->execute();
}

int Backend::getBookedTickets(string username) {
    booked_tickets.clear();
    booked_train_no.clear();
    pstmt = con->prepareStatement("SELECT train_no, no_of_tickets FROM booked_tickets WHERE username = ?;");
    pstmt->setString(1, username);
    res = pstmt->executeQuery();
    int count = 0;
    while (res->next()) {
        booked_tickets.push_back(res->getInt("train_no"));
        booked_train_no.push_back(res->getInt("no_of_tickets"));
    }
    return count;
}

void Backend::printBookedTickets() {
    string d_station;
    string e_station;
    pstmt = con->prepareStatement("SELECT departure_station, arrival_station FROM schedule WHERE train_no = ?;");
    int i = 0;
    printf("%10s %10s %10d %10d\n", "FROM", "TO", "TRAIN NO", "TICKETS");
    while (i < booked_tickets.size()) {
        pstmt->setInt(1, booked_train_no[i]);
        res = pstmt->executeQuery();
        d_station = res->getString("departure_statiion");
        e_station = res->getString("arrival_statiion");
        printf("%10s %10s %10d %10d\n", d_station, e_station, booked_train_no[i], booked_tickets[i]);
        i++;
    }

}

void Backend::cancel(string username, int choice) {
    pstmt = con->prepareStatement("DELETE FROM booked_tickets WHERE (username = ? AND train_no = ?);");
    pstmt->setString(1, username);
    pstmt->setInt(2, booked_train_no[choice - 1]);
    pstmt->execute();
}

void Backend::update(int choice) {
    pstmt = con->prepareStatement("UPDATE schedule SET tickets = tickets + ? WHERE train_no = ?;");
    pstmt->setInt(1, booked_tickets[choice-1]);
    pstmt->setInt(2, booked_train_no[choice - 1]);
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
    database->getSelectedTrains(start, end, date);
    int val = database->printSelectedTrains();
    if (val == -1) {
        return;
    }
    string ans;
    cout << "Do you want to book the ticket\n";
    cin >> ans;
    if(ans != "y"){
        "Okay!\n";
        return;
    }
    int train_choice;
    cout << "Enter the train\n";
    cin >> train_choice;
    if (train_choice < 1 || train_choice > val) {
        cout << "Enter a valid choice\n";
        return;
    }
    int tickets;
    cout << "Enter the no of tickets you want\n";
    cin >> tickets;
    database->book(train_choice, tickets);
    //username VARCHAR(50) PRIMARY KEY, journey_date VARCHAR(50), train_no INTEGER(7), no_of_tickets INTEGER(3)
    database->updateUser(username,  date, train_choice, tickets);
    cout << "Tickets have been succesfully booked!\n";
}

void User::cancel() {
    int val = database->getBookedTickets(username);
    if (val == 0) {
        cout << "No tickets have been booked for the current user!\n";
        return;
    }
    database->printBookedTickets();
    int choice;
    cout << "Enter which ticket do you want to cancel\n";
    cin >> choice;
    if (choice > 1 && choice <= val) {
        database->cancel(username, choice);
        database->update(choice);
        cout << "Ticket cancelled successfully!\n";
    }
    else {
        cout << "Enter a valid choice\n";
    }
}

User::User(Backend* b, Login* l) {
    database = b;
    this->l = l;
}

int User::start() {
    cout << "Enter 1 to login\n";
    cout << "Enter 2 to create a new account\n";
    cout << "Enter 3 to exit\n";
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
            if (decision == 3) {
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
