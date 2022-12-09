#include<iostream>
#include <stdlib.h>
#include<vector>
#include<time.h>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/prepared_statement.h>
using namespace std;


class Backend {
private:
    const string server = "tcp://127.0.0.1:3306";
    const string username = "root";
    const string password = "root";
    sql::Driver* driver;
    sql::Connection* con;
    sql::Statement* stmt;
    sql::PreparedStatement* pstmt;
    sql::ResultSet* res;
    vector<string> stations = { "Chennai", "Madurai", "Trichy", "Coimbatore", "Bangalore", "Mettupalayam", "Mumbai", "Delhi", "Kolkata", "Hyderabad" };
    string starting_date = "08/12/2022";
	vector<int> train_no;
    vector<vector<string>> details;
	vector<int> tickets;
	vector<int> price;
    vector<int> booked_train_no;
    vector<int> booked_tickets;
    int generateNumber(int no_of_digits);
    string generateTime();
    string findTime(string start, string end);
    string generateDate(string date, int offset);
public:
    Backend();
    ~Backend();
    void create();
    void add(string d_station, string e_station, int tickets, int price);
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
    int val = 0;
    int mult = 1;
    for (int i = 0; i < no_of_digits; i++) {
        val += mult * (rand() % 10);
        mult *= 10;
    }
    return val;
}

string Backend::generateTime() {
    string solve;
    int hrs = rand() % 24;
    if (hrs < 10) {
        solve += '0';
    }
    solve += to_string(hrs);
    solve += ":";
    int min = rand() % 60;
    if (min < 10) {
        solve += '0';
    }
    solve += to_string(min);
    return solve;
}

string Backend::generateDate(string date, int offset) {
    map<string, int>month = { {"01",31},{"02",28},{"03",31}, {"04",30}, {"05",31},{"06",30},{"07",31}, {"08",31}, {"09",30}, {"10",31}, {"11",30} , {"12",31} };
    int day = stoi(date.substr(0, 2));
    string actual_month = date.substr(3, 2);
    int actual_month_no = stoi(actual_month);
    int month_max = month[date.substr(3, 2)];
    int year = stoi(date.substr(6, 4));
    if (year % 4 == 0 && month_max == 28) {
        month_max++;
    }
    day++;
    if (day > month_max) {
        day = 1;
        actual_month_no++;
    }
    if (actual_month_no > 12) {
        actual_month_no = 1;
        year++;
    }
    string date_str;
    if (day < 10) {
        date_str += '0';
    }
    date_str += to_string(day);
    string month_str;
    if (actual_month_no < 10) {
        month_str += '0';
    }
    month_str += to_string(actual_month_no);
    string return_val = date_str + "/" + month_str + "/" + to_string(year);
    return return_val;
}
string Backend::findTime(string start, string end) {
    int start_hrs = stoi(start.substr(0, 2));
    int start_min = stoi(start.substr(3, 2)) + start_hrs * 60;
    int end_hrs = stoi(end.substr(0, 2));
    int end_min = stoi(end.substr(3, 2)) + end_hrs * 60;
    int final_val = ((end_min - start_min) % (24 * 60) + (24 * 60) ) % (24*60);
    int hrs = final_val / 60;
    string final_hrs;
    if (hrs < 10) {
        final_hrs += '0';
    }
    final_hrs += to_string(hrs) + ":";
    final_val %= 60;
    if (final_val < 10) {
        final_hrs += '0';
    }
    final_hrs += to_string(final_val);
    return final_hrs;
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
    con->setSchema("railways");
    stmt = con->createStatement();
    stmt->executeUpdate("CREATE TABLE IF NOT EXISTS schedule(train_no INTEGER(10) PRIMARY KEY, starting_date VARCHAR(12), departure_station VARCHAR(50), departure_time VARCHAR(12), arrival_station VARCHAR(40), arrival_time VARCHAR(12), duration VARCHAR(8), tickets INTEGER(3), price INTEGER(3));");
    stmt->executeUpdate("CREATE TABLE IF NOT EXISTS userinfo(username VARCHAR(50) PRIMARY KEY, password VARCHAR(50));");
    stmt->executeUpdate("CREATE TABLE IF NOT EXISTS booked_tickets(username VARCHAR(50), journey_date VARCHAR(50), train_no INTEGER(7), no_of_tickets INTEGER(3));");
    //create();
}

Backend:: ~Backend(){
    delete stmt;
    delete pstmt;
    delete con;
}

void Backend::create() {
    int count = stations.size();
    pstmt = con->prepareStatement("INSERT INTO schedule VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?);");
    vector<string> dates;
    string departure_time = "a";
    string arrival_time = "e";
    string duration = "f";
    for (int i = 0; i < 90; i++) {
        dates.push_back(starting_date);
        starting_date = generateDate(starting_date, 1);
    }
    int train_no = 0;
    for (int i = 0; i < count; i++) {
        pstmt->setString(3, stations[i]);
        for (int j = 0; j < count; j++) {
            if (i != j) {
                departure_time = generateTime();
                arrival_time = generateTime();
                duration = findTime(departure_time, arrival_time);
                pstmt->setString(4, departure_time);
                pstmt->setString(5, stations[j]);
                pstmt->setString(6, arrival_time);
                pstmt->setString(7, duration);
                pstmt->setInt(8, generateNumber(3));
                pstmt->setInt(9, generateNumber(3));
                for (int k = 0; k < 90; k++) {
                    pstmt->setInt(1, train_no);
                    pstmt->setString(2, dates[k]);
                    pstmt->executeUpdate();
                    train_no++;
                }
            }
        }
    }
}

void Backend::add(string d_station, string e_station, int tickets, int price) {
    pstmt = con->prepareStatement("INSERT INTO schedule(train_no, starting_date, departure_station, departure_time, arrival_station, arrival_time, duration, tickets, price) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?);");
    int train_num = generateNumber(8);
    if (find(stations.begin(), stations.end(), d_station) == stations.end()) {
        stations.push_back(d_station);
    }
    if (find(stations.begin(), stations.end(), e_station) == stations.end()) {
        stations.push_back(e_station);
    }
    string departure_time = generateTime();;
    string arrival_time = generateTime();
	pstmt->setString(3, d_station);
	pstmt->setString(4, departure_time);
	pstmt->setString(5, e_station);
	pstmt->setString(6, arrival_time);
	pstmt->setString(7, findTime(departure_time, arrival_time));
	pstmt->setInt(8, tickets);
	pstmt->setInt(9, price);
    string save = starting_date;
	for (int k = 0; k < 90; k++) {
        pstmt->setInt(1, train_num);
		pstmt->setString(2, starting_date);
		pstmt->execute();
        starting_date = generateDate(starting_date, 1);
        train_num = generateNumber(8);
	}
    starting_date = save;
    cout << "Train added successfully!\n";
}

void Backend::remove(int train_no) {
    pstmt = con->prepareStatement("DELETE FROM schedule WHERE train_no = ?;");
    pstmt->setInt(1, train_no);
    pstmt->execute();
    cout << "The train has been removed successfully\n";
}
    
void Backend::getSelectedTrains(int start, int end, string date) {
    details.clear();
    price.clear();
    tickets.clear();
    string dep_city = stations[start - 1];
    string end_city = stations[end - 1];
    pstmt = con->prepareStatement("SELECT * FROM schedule WHERE (departure_station = ? AND arrival_station = ? AND starting_date = ?);");
    pstmt->setString(1, dep_city);
    pstmt->setString(2, end_city);
    pstmt->setString(3, date);
    res = pstmt->executeQuery();
    int size = 0;
    vector<string> temp;
    while (res->next()) {
        train_no.push_back(res->getInt("train_no"));
        temp.push_back(res->getString("starting_date"));
        temp.push_back(res->getString("departure_station"));
        temp.push_back(res->getString("departure_time"));
        temp.push_back(res->getString("arrival_station"));
        temp.push_back(res->getString("arrival_time"));
        temp.push_back(res->getString("duration"));
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
    printf("%20s %20s %20s %20s %20s %20s %20s %20s %20s %20s\n", "S.No", "Train number ","Date of Journey", "Departure Station", "Departure Time", "Arrival Station", "Arrival Time", "Duration", "Tickets", "Price");
    for (; i < details.size(); i++) {
        printf("%20d ", (i + 1));
        cout.width(20);
        cout << train_no[i] << " ";
        for (int j = 0; j < details[i].size(); j++) {
            cout.width(20);
            cout << details[i][j];
        }
        printf("%20d %20d\n", tickets[i], price[i]);
    }
    return i;
}

void Backend::book(int train_choice, int tickets) {
    pstmt = con->prepareStatement("UPDATE schedule SET tickets = tickets - ? WHERE train_no = ?;");
    pstmt->setInt(1, tickets);
    pstmt->setInt(2, train_no[train_choice-1]);
    pstmt->executeUpdate();
}

void Backend::updateUser(string username, string date, int train_choice, int tickets) {
    pstmt = con->prepareStatement("INSERT INTO booked_tickets(username, journey_date, train_no, no_of_tickets) VALUES(?, ?, ?, ?);");
    pstmt->setString(1, username);
    pstmt->setString(2, date);
    pstmt->setInt(3, train_no[train_choice-1]);
    pstmt->setInt(4, tickets);
    pstmt->executeUpdate();
}

int Backend::getBookedTickets(string username) {
    booked_tickets.clear();
    booked_train_no.clear();
    pstmt = con->prepareStatement("SELECT train_no, no_of_tickets FROM booked_tickets WHERE username = ?;");
    pstmt->setString(1, username);
    res = pstmt->executeQuery();
    int count = 0;
    while (res->next()) {
        booked_tickets.push_back(res->getInt("no_of_tickets"));
        booked_train_no.push_back(res->getInt("train_no"));
        count++;
    }
    return count;
}

void Backend::printBookedTickets() {
    string d_station;
    string e_station;
    pstmt = con->prepareStatement("SELECT departure_station, arrival_station FROM schedule WHERE train_no = ?;");
    int i = 0;
    printf("%20s %20s %20s %20s %20s\n","S.No",  "FROM", "TO", "TRAIN NO", "TICKETS");
    while (i < booked_tickets.size()) {
        pstmt->setInt(1, booked_train_no[i]);
        res = pstmt->executeQuery();
        while (res->next()) {
            d_station = res->getString("departure_station");
            e_station = res->getString("arrival_station");
            cout.width(20);
            cout << (i + 1) << " ";
            cout.width(20);
            cout << d_station << " ";
            cout.width(20);
            cout << e_station << " ";
            cout.width(20);
            cout << booked_train_no[i] << " ";
            cout.width(20);
            cout << booked_tickets[i] << "\n";
            i++;
        }
    }

}

void Backend::cancel(string username, int choice) {
    pstmt = con->prepareStatement("DELETE FROM booked_tickets WHERE (username = ? AND train_no = ?);");
    pstmt->setString(1, username);
    pstmt->setInt(2, booked_train_no[choice - 1]);
    pstmt->executeUpdate();
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
        bool valid = database->checkUser(username, password);
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
    cout << "Enter the train of your choice\n";
    cin >> train_choice;
    if (train_choice < 1 || train_choice > val) {
        cout << "Enter a valid choice\n";
        return;
    }
    int tickets;
    cout << "Enter the number of tickets you want\n";
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
    if (choice > 0 && choice <= val) {
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
    void populate();
    void addTrain();
    void deleteTrain();
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
    int tickets;
    int price;
    cout << "Enter the starting station\n";
    cin >> start_station;
    cout << "Enter the ending station\n";
    cin >> end_station;
    cout << "Enter the number of available tickets\n";
    cin >> tickets;
    cout << "Enter the price of a ticket\n";
    cin >> price;
    database->add(start_station, end_station, tickets, price);
}

void Admin::deleteTrain() {
    int train_no;
    cout << "Enter the train number of the train to be deleted\n";
    cin >> train_no;
    database->remove(train_no);
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
        cout << "Enter 4 to exit\n";
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
    srand(time(NULL));
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
        else if (decide != 1) {
            return 0;
        }
    }
    cout << "Thank you for using this application!\n";
    return 0;
}
